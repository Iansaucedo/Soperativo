/**
 * Ejercicio 1: Gestor de Tareas (SOLUCIÓN)
 * 
 * Este ejercicio simula un sistema de gestión de tareas con procesos,
 * hilos y comunicación mediante señales.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MAX_TAREAS 10
#define MAX_DESCRIPCION 100
#define MAX_TRABAJADORES 5
#define NOMBRE_SEMAFORO "/gestor_tareas_sem"

// Definición de los estados de las tareas
typedef enum {
    PENDIENTE = 0,
    EN_PROCESO = 1,
    COMPLETADA = 2,
    CANCELADA = 3
} EstadoTarea;

// Estructura para representar una tarea
typedef struct {
    int id;
    char descripcion[MAX_DESCRIPCION];
    int prioridad;  // 1-5, donde 5 es la más alta
    EstadoTarea estado;
    time_t tiempo_creacion;
    time_t tiempo_inicio;
    time_t tiempo_fin;
    pid_t proceso_asignado;
} Tarea;

// Estructura para la memoria compartida
typedef struct {
    Tarea tareas[MAX_TAREAS];
    int num_tareas;
    int trabajadores_activos;
    pthread_mutex_t mutex;
    pthread_cond_t nueva_tarea;
    int siguiente_id;
} GestorTareas;

// Variables globales
GestorTareas *gestor = NULL;
sem_t *sem_gestor = NULL;
int continuar = 1;
int soy_coordinador = 0;
int id_trabajador = -1;

// Prototipos de funciones
void inicializar_gestor();
void finalizar_gestor();
void *hilo_monitor(void *arg);
void *hilo_trabajador(void *arg);
int agregar_tarea(const char *descripcion, int prioridad);
int asignar_tarea();
int completar_tarea(int id_tarea);
int cancelar_tarea(int id_tarea);
void mostrar_tareas();
void procesar_tarea(Tarea *tarea);
void manejador_sigint(int sig);
void manejador_sigusr1(int sig);

// Inicializar el gestor de tareas
void inicializar_gestor() {
    // Crear o abrir la memoria compartida
    gestor = mmap(NULL, sizeof(GestorTareas), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                  
    if (gestor == MAP_FAILED) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Inicializar semáforo
    sem_gestor = sem_open(NOMBRE_SEMAFORO, O_CREAT, 0644, 1);
    if (sem_gestor == SEM_FAILED) {
        perror("Error al crear el semáforo");
        munmap(gestor, sizeof(GestorTareas));
        exit(EXIT_FAILURE);
    }
    
    // Si somos el coordinador, inicializar la estructura
    if (soy_coordinador) {
        sem_wait(sem_gestor);
        gestor->num_tareas = 0;
        gestor->trabajadores_activos = 0;
        gestor->siguiente_id = 1;
        
        // Inicializar mutex y variable de condición
        pthread_mutexattr_t mutex_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&gestor->mutex, &mutex_attr);
        
        pthread_condattr_t cond_attr;
        pthread_condattr_init(&cond_attr);
        pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&gestor->nueva_tarea, &cond_attr);
        
        // Limpiar el array de tareas
        memset(gestor->tareas, 0, sizeof(gestor->tareas));
        
        sem_post(sem_gestor);
    }
    
    // Incrementar contador de trabajadores
    if (!soy_coordinador) {
        sem_wait(sem_gestor);
        gestor->trabajadores_activos++;
        sem_post(sem_gestor);
    }
}

// Finalizar el gestor de tareas y liberar recursos
void finalizar_gestor() {
    // Decrementar contador de trabajadores
    if (!soy_coordinador) {
        sem_wait(sem_gestor);
        gestor->trabajadores_activos--;
        sem_post(sem_gestor);
    }
    
    // Si somos el coordinador y no hay trabajadores activos, destruir recursos
    if (soy_coordinador && gestor->trabajadores_activos == 0) {
        pthread_mutex_destroy(&gestor->mutex);
        pthread_cond_destroy(&gestor->nueva_tarea);
        sem_unlink(NOMBRE_SEMAFORO);
    }
    
    // Liberar recursos comunes
    munmap(gestor, sizeof(GestorTareas));
    sem_close(sem_gestor);
}

// Función para el hilo de monitoreo
void *hilo_monitor(void *arg) {
    struct timespec ts = {0, 500000000}; // 500ms
    
    while (continuar) {
        // Mostrar estado actual del sistema
        printf("\n=== ESTADO DEL GESTOR DE TAREAS ===\n");
        printf("Trabajadores activos: %d\n", gestor->trabajadores_activos);
        mostrar_tareas();
        
        // Pequeña pausa para no saturar la pantalla
        nanosleep(&ts, NULL);
    }
    
    return NULL;
}

// Función para el hilo trabajador
void *hilo_trabajador(void *arg) {
    int mi_id = *((int *)arg);
    printf("[Trabajador %d] Iniciado\n", mi_id);
    
    while (continuar) {
        // Intentar tomar una tarea
        int id_tarea = asignar_tarea();
        
        if (id_tarea > 0) {
            // Procesar la tarea asignada
            sem_wait(sem_gestor);
            for (int i = 0; i < gestor->num_tareas; i++) {
                if (gestor->tareas[i].id == id_tarea) {
                    printf("[Trabajador %d] Procesando tarea %d: %s\n", 
                           mi_id, id_tarea, gestor->tareas[i].descripcion);
                    
                    Tarea tarea_actual = gestor->tareas[i];
                    sem_post(sem_gestor);
                    
                    // Procesar la tarea (simulación)
                    procesar_tarea(&tarea_actual);
                    
                    // Marcar como completada
                    completar_tarea(id_tarea);
                    break;
                }
            }
        } else {
            // Si no hay tareas disponibles, esperar
            struct timespec ts = {0, 500000000}; // 500ms
            nanosleep(&ts, NULL);
        }
    }
    
    printf("[Trabajador %d] Finalizado\n", mi_id);
    free(arg);
    return NULL;
}

// Función para agregar una nueva tarea
int agregar_tarea(const char *descripcion, int prioridad) {
    if (prioridad < 1 || prioridad > 5) {
        return -1;  // Prioridad inválida
    }
    
    sem_wait(sem_gestor);
    
    // Verificar si hay espacio para más tareas
    if (gestor->num_tareas >= MAX_TAREAS) {
        sem_post(sem_gestor);
        return -2;  // Gestor lleno
    }
    
    // Buscar la posición para la nueva tarea
    int pos = gestor->num_tareas;
    
    // Crear la nueva tarea
    gestor->tareas[pos].id = gestor->siguiente_id++;
    strncpy(gestor->tareas[pos].descripcion, descripcion, MAX_DESCRIPCION - 1);
    gestor->tareas[pos].prioridad = prioridad;
    gestor->tareas[pos].estado = PENDIENTE;
    gestor->tareas[pos].tiempo_creacion = time(NULL);
    gestor->tareas[pos].tiempo_inicio = 0;
    gestor->tareas[pos].tiempo_fin = 0;
    gestor->tareas[pos].proceso_asignado = 0;
    
    // Incrementar contador de tareas
    gestor->num_tareas++;
    
    // Guardar el ID para retornarlo
    int nuevo_id = gestor->tareas[pos].id;
    
    // Señalizar que hay una nueva tarea disponible
    pthread_cond_signal(&gestor->nueva_tarea);
    
    sem_post(sem_gestor);
    
    printf("Tarea %d agregada: %s (Prioridad: %d)\n", 
           nuevo_id, descripcion, prioridad);
    
    return nuevo_id;
}

// Función para asignar una tarea a un trabajador
int asignar_tarea() {
    int id_tarea = -1;
    
    sem_wait(sem_gestor);
    
    // Buscar tareas pendientes por orden de prioridad
    int mayor_prioridad = 0;
    int indice_seleccionado = -1;
    
    for (int i = 0; i < gestor->num_tareas; i++) {
        if (gestor->tareas[i].estado == PENDIENTE && 
            gestor->tareas[i].prioridad > mayor_prioridad) {
            mayor_prioridad = gestor->tareas[i].prioridad;
            indice_seleccionado = i;
        }
    }
    
    // Si se encontró una tarea pendiente, asignarla
    if (indice_seleccionado >= 0) {
        gestor->tareas[indice_seleccionado].estado = EN_PROCESO;
        gestor->tareas[indice_seleccionado].tiempo_inicio = time(NULL);
        gestor->tareas[indice_seleccionado].proceso_asignado = getpid();
        
        id_tarea = gestor->tareas[indice_seleccionado].id;
    }
    
    sem_post(sem_gestor);
    
    return id_tarea;
}

// Función para marcar una tarea como completada
int completar_tarea(int id_tarea) {
    int resultado = -1;
    
    sem_wait(sem_gestor);
    
    // Buscar la tarea por su ID
    for (int i = 0; i < gestor->num_tareas; i++) {
        if (gestor->tareas[i].id == id_tarea) {
            // Verificar que la tarea está en proceso y asignada a este proceso
            if (gestor->tareas[i].estado != EN_PROCESO ||
                gestor->tareas[i].proceso_asignado != getpid()) {
                sem_post(sem_gestor);
                return -2;  // No está en proceso o no es de este proceso
            }
            
            // Marcar como completada
            gestor->tareas[i].estado = COMPLETADA;
            gestor->tareas[i].tiempo_fin = time(NULL);
            gestor->tareas[i].proceso_asignado = 0;
            
            resultado = 0;
            printf("Tarea %d completada: %s\n", 
                   id_tarea, gestor->tareas[i].descripcion);
            break;
        }
    }
    
    sem_post(sem_gestor);
    
    return resultado;
}

// Función para cancelar una tarea
int cancelar_tarea(int id_tarea) {
    int resultado = -1;
    
    sem_wait(sem_gestor);
    
    // Buscar la tarea por su ID
    for (int i = 0; i < gestor->num_tareas; i++) {
        if (gestor->tareas[i].id == id_tarea) {
            // Solo se pueden cancelar tareas pendientes o en proceso
            if (gestor->tareas[i].estado != PENDIENTE && 
                gestor->tareas[i].estado != EN_PROCESO) {
                sem_post(sem_gestor);
                return -2;  // No se puede cancelar
            }
            
            // Si está asignada a un proceso, enviar señal de cancelación
            if (gestor->tareas[i].estado == EN_PROCESO && 
                gestor->tareas[i].proceso_asignado > 0) {
                kill(gestor->tareas[i].proceso_asignado, SIGUSR1);
            }
            
            // Marcar como cancelada
            gestor->tareas[i].estado = CANCELADA;
            gestor->tareas[i].tiempo_fin = time(NULL);
            gestor->tareas[i].proceso_asignado = 0;
            
            resultado = 0;
            printf("Tarea %d cancelada: %s\n", 
                   id_tarea, gestor->tareas[i].descripcion);
            break;
        }
    }
    
    sem_post(sem_gestor);
    
    return resultado;
}

// Función para mostrar todas las tareas
void mostrar_tareas() {
    sem_wait(sem_gestor);
    
    printf("=== LISTA DE TAREAS (%d) ===\n", gestor->num_tareas);
    printf("%-4s %-20s %-10s %-12s %-8s\n", 
           "ID", "Descripción", "Prioridad", "Estado", "PID");
    
    for (int i = 0; i < gestor->num_tareas; i++) {
        // Convertir estado a texto
        char estado_txt[20];
        switch(gestor->tareas[i].estado) {
            case PENDIENTE:
                strcpy(estado_txt, "Pendiente");
                break;
            case EN_PROCESO:
                strcpy(estado_txt, "En proceso");
                break;
            case COMPLETADA:
                strcpy(estado_txt, "Completada");
                break;
            case CANCELADA:
                strcpy(estado_txt, "Cancelada");
                break;
            default:
                strcpy(estado_txt, "Desconocido");
                break;
        }
        
        printf("%-4d %-20s %-10d %-12s %-8d\n", 
               gestor->tareas[i].id,
               gestor->tareas[i].descripcion,
               gestor->tareas[i].prioridad,
               estado_txt,
               gestor->tareas[i].proceso_asignado);
    }
    
    sem_post(sem_gestor);
}

// Función para procesar una tarea (simulación)
void procesar_tarea(Tarea *tarea) {
    // Tiempo de procesamiento basado en la prioridad (inversamente proporcional)
    int tiempo_espera = (6 - tarea->prioridad) * 2;
    
    printf("Procesando tarea %d (%s) durante %d segundos...\n",
           tarea->id, tarea->descripcion, tiempo_espera);
    
    // Simular procesamiento
    for (int i = 0; i < tiempo_espera && continuar; i++) {
        sleep(1);
        printf(".");
        fflush(stdout);
    }
    
    printf("\nProcesamiento de tarea %d completado.\n", tarea->id);
}

// Manejador de señal SIGINT (Ctrl+C)
void manejador_sigint(int sig) {
    printf("\nFinalizando gestor de tareas...\n");
    continuar = 0;
}

// Manejador de señal SIGUSR1 (Cancelación de tarea)
void manejador_sigusr1(int sig) {
    printf("\nRecibida señal de cancelación de tarea.\n");
    // No hacemos nada específico aquí, solo notificamos
    // La tarea se cancelará en la próxima iteración del bucle
}

int main(int argc, char *argv[]) {
    // Verificar el modo de ejecución (coordinador o trabajador)
    if (argc > 1 && strcmp(argv[1], "trabajador") == 0) {
        soy_coordinador = 0;
        if (argc > 2) {
            id_trabajador = atoi(argv[2]);
        } else {
            id_trabajador = 0;  // ID por defecto
        }
    } else {
        soy_coordinador = 1;
    }
    
    // Configurar manejadores de señales
    signal(SIGINT, manejador_sigint);
    signal(SIGUSR1, manejador_sigusr1);
    
    // Mostrar modo de ejecución
    if (soy_coordinador) {
        printf("=== GESTOR DE TAREAS - MODO COORDINADOR ===\n");
    } else {
        printf("=== GESTOR DE TAREAS - MODO TRABAJADOR %d ===\n", id_trabajador);
    }
    
    // Inicializar el gestor de tareas
    inicializar_gestor();
    
    if (soy_coordinador) {
        // Modo coordinador: crear hilos para monitorizar y lanzar procesos trabajadores
        
        // Crear hilo monitor
        pthread_t hilo_mon;
        pthread_create(&hilo_mon, NULL, hilo_monitor, NULL);
        
        // Mostrar menú de opciones
        printf("\nComandos disponibles:\n");
        printf("  agregar <descripción> <prioridad>   - Agregar nueva tarea\n");
        printf("  cancelar <id>                       - Cancelar tarea\n");
        printf("  listar                             - Mostrar lista de tareas\n");
        printf("  trabajador <id>                    - Crear nuevo trabajador\n");
        printf("  salir                              - Salir del gestor\n");
        
        char comando[MAX_DESCRIPCION];
        char desc_tarea[MAX_DESCRIPCION];
        int prioridad, id;
        
        // Bucle principal de comandos
        while (continuar) {
            printf("\n[Coordinador]> ");
            if (fgets(comando, MAX_DESCRIPCION, stdin) == NULL) {
                break;
            }
            comando[strcspn(comando, "\n")] = 0;
            
            // Tokenizar el comando
            char *token = strtok(comando, " ");
            if (token == NULL) {
                continue;
            }
            
            if (strcmp(token, "agregar") == 0) {
                // Obtener la descripción de la tarea
                token = strtok(NULL, " ");
                if (token == NULL) {
                    printf("Error: Falta la descripción de la tarea\n");
                    continue;
                }
                strncpy(desc_tarea, token, MAX_DESCRIPCION - 1);
                
                // Obtener la prioridad
                token = strtok(NULL, " ");
                if (token == NULL) {
                    printf("Error: Falta la prioridad\n");
                    continue;
                }
                prioridad = atoi(token);
                
                // Agregar la tarea
                if (agregar_tarea(desc_tarea, prioridad) < 0) {
                    printf("Error al agregar la tarea\n");
                }
                
            } else if (strcmp(token, "cancelar") == 0) {
                // Obtener el ID de la tarea
                token = strtok(NULL, " ");
                if (token == NULL) {
                    printf("Error: Falta el ID de la tarea\n");
                    continue;
                }
                id = atoi(token);
                
                // Cancelar la tarea
                if (cancelar_tarea(id) < 0) {
                    printf("Error al cancelar la tarea %d\n", id);
                }
                
            } else if (strcmp(token, "listar") == 0) {
                mostrar_tareas();
                
            } else if (strcmp(token, "trabajador") == 0) {
                // Obtener el ID del trabajador
                token = strtok(NULL, " ");
                int worker_id = (token != NULL) ? atoi(token) : gestor->trabajadores_activos;
                
                // Crear proceso trabajador
                pid_t pid = fork();
                
                if (pid == 0) {
                    // Código del proceso hijo
                    char id_str[10];
                    sprintf(id_str, "%d", worker_id);
                    
                    // Ejecutar nuevo trabajador
                    execlp(argv[0], argv[0], "trabajador", id_str, NULL);
                    
                    // Si llegamos aquí, ocurrió un error
                    perror("Error al ejecutar trabajador");
                    exit(EXIT_FAILURE);
                } else if (pid < 0) {
                    perror("Error al crear proceso trabajador");
                } else {
                    printf("Trabajador %d creado con PID %d\n", worker_id, pid);
                }
                
            } else if (strcmp(token, "salir") == 0) {
                continuar = 0;
                
            } else {
                printf("Comando desconocido: %s\n", token);
            }
        }
        
        // Esperar a que el hilo monitor termine
        pthread_join(hilo_mon, NULL);
        
    } else {
        // Modo trabajador: crear hilo para procesar tareas
        pthread_t hilo_worker;
        int *worker_id = malloc(sizeof(int));
        *worker_id = id_trabajador;
        pthread_create(&hilo_worker, NULL, hilo_trabajador, worker_id);
        
        // Esperar señal para terminar
        while (continuar) {
            sleep(1);
        }
        
        // Esperar a que el hilo trabajador termine
        pthread_join(hilo_worker, NULL);
    }
    
    // Finalizar y liberar recursos
    finalizar_gestor();
    
    if (soy_coordinador) {
        printf("Coordinador finalizado correctamente.\n");
    } else {
        printf("Trabajador %d finalizado correctamente.\n", id_trabajador);
    }
    
    return 0;
}
