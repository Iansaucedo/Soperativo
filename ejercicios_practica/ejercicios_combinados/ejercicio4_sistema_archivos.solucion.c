/**
 * Ejercicio 4: Sistema de Archivos Distribuido (SOLUCIÓN)
 * 
 * Este ejercicio simula un sistema de archivos distribuido utilizando procesos,
 * hilos, memoria compartida y mecanismos de sincronización.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define MAX_NODOS 5
#define MAX_ARCHIVOS 20
#define MAX_NOMBRE 50
#define MAX_CONTENIDO 1024
#define MAX_OPERACIONES 100
#define NOMBRE_SEM_ARCHIVOS "/sem_archivos"
#define NOMBRE_SEM_LOGS "/sem_logs"

// Estructura para representar un archivo
typedef struct {
    char nombre[MAX_NOMBRE];
    char contenido[MAX_CONTENIDO];
    int tamanio;
    int propietario; // ID del nodo propietario
    time_t ultima_modificacion;
    int bloqueado; // 0: no bloqueado, 1: bloqueado
    int nodo_bloqueo; // ID del nodo que tiene el bloqueo
} Archivo;

// Estructura para representar un registro en el log
typedef struct {
    time_t timestamp;
    int nodo;
    int tipo_operacion; // 0: crear, 1: leer, 2: escribir, 3: eliminar
    char nombre_archivo[MAX_NOMBRE];
} LogEntry;

// Estructura para la memoria compartida
typedef struct {
    Archivo archivos[MAX_ARCHIVOS];
    int num_archivos;
    LogEntry log[MAX_OPERACIONES];
    int indice_log;
    int nodos_activos[MAX_NODOS];
} SistemaArchivos;

// Variables globales
SistemaArchivos *sistema = NULL;
sem_t *sem_archivos = NULL;
sem_t *sem_logs = NULL;
int id_nodo;
int continuar = 1;

// Prototipos de funciones
void inicializar_sistema();
void finalizar_sistema();
void *hilo_sincronizacion(void *arg);
void *hilo_monitor(void *arg);
int crear_archivo(const char *nombre, const char *contenido);
int leer_archivo(const char *nombre, char *buffer);
int escribir_archivo(const char *nombre, const char *nuevo_contenido);
int eliminar_archivo(const char *nombre);
void registrar_log(int tipo_operacion, const char *nombre_archivo);
void mostrar_archivos();
void mostrar_log();
void mostrar_estado_nodos();
void manejador_sigint(int sig);

// Inicializar el sistema de archivos
void inicializar_sistema() {
    // Crear memoria compartida
    sistema = mmap(NULL, sizeof(SistemaArchivos), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                  
    if (sistema == MAP_FAILED) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Inicializar semáforos
    sem_archivos = sem_open(NOMBRE_SEM_ARCHIVOS, O_CREAT, 0644, 1);
    sem_logs = sem_open(NOMBRE_SEM_LOGS, O_CREAT, 0644, 1);
    
    if (sem_archivos == SEM_FAILED || sem_logs == SEM_FAILED) {
        perror("Error al crear los semáforos");
        munmap(sistema, sizeof(SistemaArchivos));
        exit(EXIT_FAILURE);
    }
    
    // Bloquear semáforo para inicializar estructura
    sem_wait(sem_archivos);
    
    // Inicializar estructura de sistema de archivos
    memset(sistema->archivos, 0, sizeof(sistema->archivos));
    sistema->num_archivos = 0;
    sistema->indice_log = 0;
    memset(sistema->nodos_activos, 0, sizeof(sistema->nodos_activos));
    
    // Marcar este nodo como activo
    sistema->nodos_activos[id_nodo] = 1;
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
}

// Finalizar el sistema de archivos y liberar recursos
void finalizar_sistema() {
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Marcar este nodo como inactivo
    sistema->nodos_activos[id_nodo] = 0;
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    // Liberar recursos
    munmap(sistema, sizeof(SistemaArchivos));
    sem_close(sem_archivos);
    sem_close(sem_logs);
    
    // Intentar eliminar semáforos si es el último nodo
    int nodos_restantes = 0;
    for (int i = 0; i < MAX_NODOS; i++) {
        if (sistema->nodos_activos[i]) {
            nodos_restantes++;
        }
    }
    
    if (nodos_restantes == 0) {
        sem_unlink(NOMBRE_SEM_ARCHIVOS);
        sem_unlink(NOMBRE_SEM_LOGS);
    }
}

// Función para el hilo de sincronización
void *hilo_sincronizacion(void *arg) {
    struct timespec ts = {0, 500000000}; // 500ms
    
    while (continuar) {
        // Pequeña pausa para no consumir demasiada CPU
        nanosleep(&ts, NULL);
        
        // Bloquear semáforo
        sem_wait(sem_archivos);
        
        // Verificar si hay archivos bloqueados por nodos inactivos
        for (int i = 0; i < sistema->num_archivos; i++) {
            if (sistema->archivos[i].bloqueado && 
                !sistema->nodos_activos[sistema->archivos[i].nodo_bloqueo]) {
                printf("[Sincronización] Liberando bloqueo del archivo %s de un nodo inactivo\n", 
                       sistema->archivos[i].nombre);
                sistema->archivos[i].bloqueado = 0;
                sistema->archivos[i].nodo_bloqueo = -1;
            }
        }
        
        // Desbloquear semáforo
        sem_post(sem_archivos);
    }
    
    return NULL;
}

// Función para el hilo de monitoreo
void *hilo_monitor(void *arg) {
    struct timespec ts = {0, 2000000000}; // 2 segundos
    
    while (continuar) {
        // Pequeña pausa
        nanosleep(&ts, NULL);
        
        // Mostrar estado actual del sistema
        printf("\n=== ESTADO DEL SISTEMA (Nodo %d) ===\n", id_nodo);
        mostrar_estado_nodos();
        mostrar_archivos();
    }
    
    return NULL;
}

// Función para crear un nuevo archivo
int crear_archivo(const char *nombre, const char *contenido) {
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Verificar si el archivo ya existe
    for (int i = 0; i < sistema->num_archivos; i++) {
        if (strcmp(sistema->archivos[i].nombre, nombre) == 0) {
            sem_post(sem_archivos);
            return -1; // El archivo ya existe
        }
    }
    
    // Verificar si hay espacio para más archivos
    if (sistema->num_archivos >= MAX_ARCHIVOS) {
        sem_post(sem_archivos);
        return -2; // Sistema lleno
    }
    
    // Crear el nuevo archivo
    int idx = sistema->num_archivos;
    strncpy(sistema->archivos[idx].nombre, nombre, MAX_NOMBRE - 1);
    strncpy(sistema->archivos[idx].contenido, contenido, MAX_CONTENIDO - 1);
    sistema->archivos[idx].tamanio = strlen(contenido);
    sistema->archivos[idx].propietario = id_nodo;
    sistema->archivos[idx].ultima_modificacion = time(NULL);
    sistema->archivos[idx].bloqueado = 0;
    sistema->archivos[idx].nodo_bloqueo = -1;
    
    // Incrementar contador de archivos
    sistema->num_archivos++;
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    // Registrar operación en el log
    registrar_log(0, nombre);
    
    printf("[Nodo %d] Archivo %s creado correctamente\n", id_nodo, nombre);
    return 0; // Éxito
}

// Función para leer un archivo
int leer_archivo(const char *nombre, char *buffer) {
    int resultado = -1;
    
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Buscar el archivo
    for (int i = 0; i < sistema->num_archivos; i++) {
        if (strcmp(sistema->archivos[i].nombre, nombre) == 0) {
            // Copiar el contenido al buffer
            strcpy(buffer, sistema->archivos[i].contenido);
            resultado = sistema->archivos[i].tamanio;
            break;
        }
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    if (resultado >= 0) {
        // Registrar operación en el log
        registrar_log(1, nombre);
        printf("[Nodo %d] Archivo %s leído correctamente (%d bytes)\n", 
               id_nodo, nombre, resultado);
    }
    
    return resultado;
}

// Función para escribir en un archivo
int escribir_archivo(const char *nombre, const char *nuevo_contenido) {
    int resultado = -1;
    
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Buscar el archivo
    for (int i = 0; i < sistema->num_archivos; i++) {
        if (strcmp(sistema->archivos[i].nombre, nombre) == 0) {
            // Verificar si el archivo está bloqueado
            if (sistema->archivos[i].bloqueado && 
                sistema->archivos[i].nodo_bloqueo != id_nodo) {
                sem_post(sem_archivos);
                return -2; // Archivo bloqueado por otro nodo
            }
            
            // Actualizar el contenido del archivo
            strncpy(sistema->archivos[i].contenido, nuevo_contenido, MAX_CONTENIDO - 1);
            sistema->archivos[i].tamanio = strlen(nuevo_contenido);
            sistema->archivos[i].ultima_modificacion = time(NULL);
            resultado = sistema->archivos[i].tamanio;
            break;
        }
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    if (resultado >= 0) {
        // Registrar operación en el log
        registrar_log(2, nombre);
        printf("[Nodo %d] Archivo %s modificado correctamente (%d bytes)\n", 
               id_nodo, nombre, resultado);
    }
    
    return resultado;
}

// Función para eliminar un archivo
int eliminar_archivo(const char *nombre) {
    int resultado = -1;
    
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Buscar el archivo
    for (int i = 0; i < sistema->num_archivos; i++) {
        if (strcmp(sistema->archivos[i].nombre, nombre) == 0) {
            // Verificar si el archivo está bloqueado
            if (sistema->archivos[i].bloqueado && 
                sistema->archivos[i].nodo_bloqueo != id_nodo) {
                sem_post(sem_archivos);
                return -2; // Archivo bloqueado por otro nodo
            }
            
            // Verificar si somos propietarios
            if (sistema->archivos[i].propietario != id_nodo) {
                sem_post(sem_archivos);
                return -3; // No es el propietario
            }
            
            // Eliminar el archivo moviendo los demás hacia arriba
            for (int j = i; j < sistema->num_archivos - 1; j++) {
                memcpy(&sistema->archivos[j], &sistema->archivos[j + 1], sizeof(Archivo));
            }
            
            sistema->num_archivos--;
            resultado = 0;
            break;
        }
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    if (resultado == 0) {
        // Registrar operación en el log
        registrar_log(3, nombre);
        printf("[Nodo %d] Archivo %s eliminado correctamente\n", id_nodo, nombre);
    }
    
    return resultado;
}

// Función para bloquear un archivo (antes de modificarlo)
int bloquear_archivo(const char *nombre) {
    int resultado = -1;
    
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Buscar el archivo
    for (int i = 0; i < sistema->num_archivos; i++) {
        if (strcmp(sistema->archivos[i].nombre, nombre) == 0) {
            // Verificar si el archivo ya está bloqueado
            if (sistema->archivos[i].bloqueado) {
                sem_post(sem_archivos);
                return -2; // Archivo ya bloqueado
            }
            
            // Bloquear el archivo
            sistema->archivos[i].bloqueado = 1;
            sistema->archivos[i].nodo_bloqueo = id_nodo;
            resultado = 0;
            break;
        }
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    if (resultado == 0) {
        printf("[Nodo %d] Archivo %s bloqueado correctamente\n", id_nodo, nombre);
    }
    
    return resultado;
}

// Función para desbloquear un archivo
int desbloquear_archivo(const char *nombre) {
    int resultado = -1;
    
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    // Buscar el archivo
    for (int i = 0; i < sistema->num_archivos; i++) {
        if (strcmp(sistema->archivos[i].nombre, nombre) == 0) {
            // Verificar si somos quien tiene el bloqueo
            if (sistema->archivos[i].bloqueado && 
                sistema->archivos[i].nodo_bloqueo != id_nodo) {
                sem_post(sem_archivos);
                return -2; // No tiene el bloqueo
            }
            
            // Desbloquear el archivo
            sistema->archivos[i].bloqueado = 0;
            sistema->archivos[i].nodo_bloqueo = -1;
            resultado = 0;
            break;
        }
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
    
    if (resultado == 0) {
        printf("[Nodo %d] Archivo %s desbloqueado correctamente\n", id_nodo, nombre);
    }
    
    return resultado;
}

// Función para registrar una operación en el log
void registrar_log(int tipo_operacion, const char *nombre_archivo) {
    // Bloquear semáforo de logs
    sem_wait(sem_logs);
    
    // Crear entrada de log
    int idx = sistema->indice_log;
    sistema->log[idx].timestamp = time(NULL);
    sistema->log[idx].nodo = id_nodo;
    sistema->log[idx].tipo_operacion = tipo_operacion;
    strncpy(sistema->log[idx].nombre_archivo, nombre_archivo, MAX_NOMBRE - 1);
    
    // Actualizar índice (circular)
    sistema->indice_log = (sistema->indice_log + 1) % MAX_OPERACIONES;
    
    // Desbloquear semáforo de logs
    sem_post(sem_logs);
}

// Función para mostrar los archivos existentes
void mostrar_archivos() {
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    printf("--- ARCHIVOS (%d) ---\n", sistema->num_archivos);
    printf("%-20s %-10s %-10s %-20s %-10s\n", 
           "Nombre", "Tamaño", "Propietario", "Última Modificación", "Estado");
    
    for (int i = 0; i < sistema->num_archivos; i++) {
        // Formatear timestamp
        struct tm *tm_info = localtime(&sistema->archivos[i].ultima_modificacion);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tm_info);
        
        // Estado de bloqueo
        char estado[20];
        if (sistema->archivos[i].bloqueado) {
            sprintf(estado, "Bloq. por %d", sistema->archivos[i].nodo_bloqueo);
        } else {
            strcpy(estado, "Disponible");
        }
        
        printf("%-20s %-10d %-10d %-20s %-10s\n", 
               sistema->archivos[i].nombre,
               sistema->archivos[i].tamanio,
               sistema->archivos[i].propietario,
               buffer,
               estado);
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
}

// Función para mostrar el log de operaciones
void mostrar_log() {
    // Bloquear semáforo de logs
    sem_wait(sem_logs);
    
    printf("--- LOG DE OPERACIONES ---\n");
    printf("%-20s %-10s %-15s %-20s\n", 
           "Timestamp", "Nodo", "Operación", "Archivo");
    
    // Determinar cuántas entradas mostrar
    int total_entradas = MAX_OPERACIONES; // Máximo posible
    
    for (int i = 0; i < total_entradas; i++) {
        int idx = (sistema->indice_log - i - 1);
        if (idx < 0) idx += MAX_OPERACIONES;
        
        // Si encontramos una entrada vacía, paramos
        if (sistema->log[idx].timestamp == 0) {
            break;
        }
        
        // Formatear timestamp
        struct tm *tm_info = localtime(&sistema->log[idx].timestamp);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tm_info);
        
        // Tipo de operación
        char operacion[15];
        switch(sistema->log[idx].tipo_operacion) {
            case 0:
                strcpy(operacion, "Crear");
                break;
            case 1:
                strcpy(operacion, "Leer");
                break;
            case 2:
                strcpy(operacion, "Escribir");
                break;
            case 3:
                strcpy(operacion, "Eliminar");
                break;
            default:
                strcpy(operacion, "Desconocida");
                break;
        }
        
        printf("%-20s %-10d %-15s %-20s\n", 
               buffer,
               sistema->log[idx].nodo,
               operacion,
               sistema->log[idx].nombre_archivo);
    }
    
    // Desbloquear semáforo de logs
    sem_post(sem_logs);
}

// Función para mostrar el estado de los nodos
void mostrar_estado_nodos() {
    // Bloquear semáforo
    sem_wait(sem_archivos);
    
    printf("--- NODOS ACTIVOS ---\n");
    for (int i = 0; i < MAX_NODOS; i++) {
        printf("Nodo %d: %s\n", i, sistema->nodos_activos[i] ? "ACTIVO" : "INACTIVO");
    }
    
    // Desbloquear semáforo
    sem_post(sem_archivos);
}

// Manejador de señal SIGINT (Ctrl+C)
void manejador_sigint(int sig) {
    printf("\nDesconectando del sistema de archivos...\n");
    continuar = 0;
}

int main(int argc, char *argv[]) {
    // Verificar argumentos
    if (argc != 2) {
        printf("Uso: %s <id_nodo>\n", argv[0]);
        return 1;
    }
    
    // Parsear ID del nodo
    id_nodo = atoi(argv[1]);
    if (id_nodo < 0 || id_nodo >= MAX_NODOS) {
        printf("Error: ID de nodo debe estar entre 0 y %d\n", MAX_NODOS - 1);
        return 1;
    }
    
    // Configurar manejador de señal para SIGINT
    signal(SIGINT, manejador_sigint);
    
    printf("=== SISTEMA DE ARCHIVOS DISTRIBUIDO ===\n");
    printf("Iniciando nodo %d...\n", id_nodo);
    
    // Inicializar el sistema de archivos
    inicializar_sistema();
    
    // Crear hilos para sincronización y monitoreo
    pthread_t hilo_sync, hilo_mon;
    pthread_create(&hilo_sync, NULL, hilo_sincronizacion, NULL);
    pthread_create(&hilo_mon, NULL, hilo_monitor, NULL);
    
    char comando[MAX_CONTENIDO];
    char buffer[MAX_CONTENIDO];
    
    printf("\nComandos disponibles:\n");
    printf("  crear <nombre> <contenido>  - Crear nuevo archivo\n");
    printf("  leer <nombre>               - Leer archivo\n");
    printf("  escribir <nombre> <texto>   - Escribir en archivo\n");
    printf("  eliminar <nombre>           - Eliminar archivo\n");
    printf("  bloquear <nombre>           - Bloquear archivo\n");
    printf("  desbloquear <nombre>        - Desbloquear archivo\n");
    printf("  lista                       - Listar archivos\n");
    printf("  log                         - Mostrar log de operaciones\n");
    printf("  nodos                       - Mostrar estado de nodos\n");
    printf("  salir                       - Salir del sistema\n");
    
    // Bucle principal de comandos
    while (continuar) {
        printf("\n[Nodo %d]> ", id_nodo);
        if (fgets(comando, MAX_CONTENIDO, stdin) == NULL) {
            break;
        }
        comando[strcspn(comando, "\n")] = 0;
        
        // Tokenizar el comando
        char *token = strtok(comando, " ");
        if (token == NULL) {
            continue;
        }
        
        if (strcmp(token, "crear") == 0) {
            // Obtener nombre del archivo
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Error: Falta el nombre del archivo\n");
                continue;
            }
            char nombre[MAX_NOMBRE];
            strncpy(nombre, token, MAX_NOMBRE - 1);
            
            // Obtener contenido
            token = strtok(NULL, "");
            if (token == NULL) {
                printf("Error: Falta el contenido\n");
                continue;
            }
            
            // Crear el archivo
            crear_archivo(nombre, token);
            
        } else if (strcmp(token, "leer") == 0) {
            // Obtener nombre del archivo
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Error: Falta el nombre del archivo\n");
                continue;
            }
            
            // Leer el archivo
            if (leer_archivo(token, buffer) >= 0) {
                printf("Contenido de '%s':\n%s\n", token, buffer);
            } else {
                printf("Error: No se pudo leer el archivo '%s'\n", token);
            }
            
        } else if (strcmp(token, "escribir") == 0) {
            // Obtener nombre del archivo
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Error: Falta el nombre del archivo\n");
                continue;
            }
            char nombre[MAX_NOMBRE];
            strncpy(nombre, token, MAX_NOMBRE - 1);
            
            // Obtener nuevo contenido
            token = strtok(NULL, "");
            if (token == NULL) {
                printf("Error: Falta el contenido\n");
                continue;
            }
            
            // Escribir en el archivo
            if (escribir_archivo(nombre, token) < 0) {
                printf("Error al escribir en el archivo '%s'\n", nombre);
            }
            
        } else if (strcmp(token, "eliminar") == 0) {
            // Obtener nombre del archivo
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Error: Falta el nombre del archivo\n");
                continue;
            }
            
            // Eliminar el archivo
            int res = eliminar_archivo(token);
            if (res == -3) {
                printf("Error: No eres el propietario del archivo '%s'\n", token);
            } else if (res < 0) {
                printf("Error al eliminar el archivo '%s'\n", token);
            }
            
        } else if (strcmp(token, "bloquear") == 0) {
            // Obtener nombre del archivo
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Error: Falta el nombre del archivo\n");
                continue;
            }
            
            // Bloquear el archivo
            if (bloquear_archivo(token) < 0) {
                printf("Error al bloquear el archivo '%s'\n", token);
            }
            
        } else if (strcmp(token, "desbloquear") == 0) {
            // Obtener nombre del archivo
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Error: Falta el nombre del archivo\n");
                continue;
            }
            
            // Desbloquear el archivo
            if (desbloquear_archivo(token) < 0) {
                printf("Error al desbloquear el archivo '%s'\n", token);
            }
            
        } else if (strcmp(token, "lista") == 0) {
            mostrar_archivos();
            
        } else if (strcmp(token, "log") == 0) {
            mostrar_log();
            
        } else if (strcmp(token, "nodos") == 0) {
            mostrar_estado_nodos();
            
        } else if (strcmp(token, "salir") == 0) {
            continuar = 0;
            
        } else {
            printf("Comando desconocido: %s\n", token);
        }
    }
    
    // Finalizar el sistema y liberar recursos
    printf("Cerrando el nodo %d...\n", id_nodo);
    continuar = 0;
    
    // Esperar a los hilos
    pthread_join(hilo_sync, NULL);
    pthread_join(hilo_mon, NULL);
    
    finalizar_sistema();
    
    printf("Nodo %d finalizado correctamente.\n", id_nodo);
    return 0;
}
