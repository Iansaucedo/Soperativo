/**
 * Ejercicio 4: Sistema de Archivos Distribuido
 * 
 * En este ejercicio implementarás un sistema de archivos distribuido
 * utilizando procesos, memoria compartida y señales. El sistema permitirá
 * a varios procesos cliente acceder a archivos gestionados por un servidor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#define MAX_CLIENTES 5
#define MAX_ARCHIVOS 10
#define MAX_RUTA 100
#define MAX_CONTENIDO 1024
#define MAX_OPERACIONES 100
#define MAX_NOMBRE 30

// Tipos de operaciones sobre archivos
typedef enum {
    CREAR,
    LEER,
    ESCRIBIR,
    ELIMINAR,
    LISTAR
} TipoOperacion;

// Estado de una operación
typedef enum {
    PENDIENTE,
    EN_PROCESO,
    COMPLETADA,
    ERROR
} EstadoOperacion;

// Estructura para representar un archivo
typedef struct {
    char nombre[MAX_NOMBRE];
    char contenido[MAX_CONTENIDO];
    int tamano;
    time_t fecha_creacion;
    time_t fecha_modificacion;
    int en_uso;  // 0: libre, 1: en uso
} Archivo;

// Estructura para representar una operación de archivo
typedef struct {
    int id;
    TipoOperacion tipo;
    char nombre_archivo[MAX_NOMBRE];
    char contenido[MAX_CONTENIDO];
    int tamano;
    int cliente_id;
    EstadoOperacion estado;
    int resultado;  // Código de retorno de la operación
} OperacionArchivo;

// Estructura para la memoria compartida
typedef struct {
    Archivo archivos[MAX_ARCHIVOS];
    OperacionArchivo operaciones[MAX_OPERACIONES];
    int indice_operaciones;
    int num_archivos;
    int servidor_activo;
    pid_t clientes[MAX_CLIENTES];
    int num_clientes;
} SistemaArchivos;

// Variables globales
SistemaArchivos *sistema = NULL;
sem_t *sem_sistema = NULL;
int cliente_id = -1;
pid_t pid_servidor;

// Prototipos de funciones
void inicializar_sistema();
void finalizar_sistema();
int conectar_cliente();
void desconectar_cliente();
int crear_archivo(const char *nombre);
int leer_archivo(const char *nombre, char *contenido);
int escribir_archivo(const char *nombre, const char *contenido);
int eliminar_archivo(const char *nombre);
void listar_archivos();
void procesar_operaciones();
void manejador_sigint(int sig);
void manejador_sigusr1(int sig);

// Función para el servidor
void servidor() {
    printf("[Servidor] Iniciando servidor de archivos distribuido\n");
    
    // Configurar manejador de señales para SIGINT
    signal(SIGINT, manejador_sigint);
    
    // Inicializar el sistema de archivos
    inicializar_sistema();
    
    // Bucle principal del servidor
    while (sistema->servidor_activo) {
        // TODO:
        // 1. Bloquear el semáforo
        // 2. Procesar operaciones pendientes
        // 3. Desbloquear el semáforo
        // 4. Dormir un poco para no saturar la CPU
        
        sleep(1);
    }
    
    printf("[Servidor] Finalizando servidor\n");
    finalizar_sistema();
}

// Función para el cliente
void cliente() {
    char comando[MAX_NOMBRE];
    char nombre_archivo[MAX_NOMBRE];
    char contenido[MAX_CONTENIDO];
    int resultado;
    
    printf("[Cliente %d] Iniciando cliente\n", cliente_id);
    
    // Configurar manejador de señales para SIGUSR1
    signal(SIGUSR1, manejador_sigusr1);
    
    // Menú del cliente
    while (1) {
        printf("\n--- CLIENTE %d ---\n", cliente_id);
        printf("1. Crear archivo\n");
        printf("2. Leer archivo\n");
        printf("3. Escribir en archivo\n");
        printf("4. Eliminar archivo\n");
        printf("5. Listar archivos\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        
        if (scanf("%s", comando) != 1) {
            while (getchar() != '\n'); // Limpiar buffer
            printf("Entrada inválida\n");
            continue;
        }
        
        // TODO: Implementar las acciones para cada opción del menú
        // Para cada opción:
        // 1. Solicitar los datos necesarios
        // 2. Llamar a la función correspondiente
        // 3. Mostrar el resultado de la operación
        
        if (strcmp(comando, "6") == 0 || strcmp(comando, "salir") == 0) {
            break;
        }
    }
    
    printf("[Cliente %d] Desconectándose del sistema\n", cliente_id);
    desconectar_cliente();
}

// Inicializar el sistema de archivos
void inicializar_sistema() {
    // TODO:
    // 1. Crear la zona de memoria compartida
    // 2. Inicializar la estructura SistemaArchivos
    // 3. Crear el semáforo para sincronización
}

// Finalizar el sistema de archivos
void finalizar_sistema() {
    // TODO:
    // 1. Notificar a todos los clientes conectados que el servidor se cierra
    // 2. Liberar la memoria compartida
    // 3. Cerrar y eliminar el semáforo
}

// Conectar un cliente al sistema
int conectar_cliente() {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Verificar si hay espacio para más clientes
    // 3. Agregar el PID del cliente al array de clientes
    // 4. Asignar un ID único al cliente
    // 5. Incrementar el contador de clientes
    // 6. Desbloquear el semáforo
    // 7. Retornar el ID asignado
    
    return -1; // Error
}

// Desconectar un cliente del sistema
void desconectar_cliente() {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Buscar el cliente en el array por su PID
    // 3. Marcar la posición como libre
    // 4. Decrementar el contador de clientes
    // 5. Desbloquear el semáforo
}

// Crear un nuevo archivo
int crear_archivo(const char *nombre) {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Verificar si ya existe un archivo con ese nombre
    // 3. Verificar si hay espacio para más archivos
    // 4. Crear el archivo en la primera posición libre
    // 5. Incrementar el contador de archivos
    // 6. Desbloquear el semáforo
    // 7. Retornar 0 si todo salió bien, o un código de error
    
    return -1; // Error
}

// Leer contenido de un archivo
int leer_archivo(const char *nombre, char *contenido) {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Buscar el archivo por su nombre
    // 3. Si existe, copiar su contenido al buffer proporcionado
    // 4. Desbloquear el semáforo
    // 5. Retornar el tamaño leído, o -1 si el archivo no existe
    
    return -1; // Error
}

// Escribir contenido en un archivo
int escribir_archivo(const char *nombre, const char *contenido) {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Buscar el archivo por su nombre
    // 3. Si existe, actualizar su contenido y fecha de modificación
    // 4. Desbloquear el semáforo
    // 5. Retornar el tamaño escrito, o -1 si el archivo no existe
    
    return -1; // Error
}

// Eliminar un archivo
int eliminar_archivo(const char *nombre) {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Buscar el archivo por su nombre
    // 3. Si existe, marcar la posición como libre y ajustar el contador
    // 4. Desbloquear el semáforo
    // 5. Retornar 0 si se eliminó, o -1 si no existía
    
    return -1; // Error
}

// Listar todos los archivos disponibles
void listar_archivos() {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Recorrer el array de archivos e imprimir información de cada uno:
    //    - Nombre, tamaño, fecha de creación y modificación
    // 3. Desbloquear el semáforo
}

// Procesar las operaciones pendientes (ejecutado por el servidor)
void procesar_operaciones() {
    // TODO:
    // 1. Recorrer el array de operaciones
    // 2. Para cada operación en estado PENDIENTE:
    //    a. Cambiar estado a EN_PROCESO
    //    b. Según el tipo, llamar a la función correspondiente
    //    c. Actualizar resultado y cambiar estado a COMPLETADA o ERROR
    //    d. Notificar al cliente que la operación fue procesada (señal)
}

// Manejador de SIGINT para servidor
void manejador_sigint(int sig) {
    printf("\n[Servidor] Recibida señal de terminación\n");
    sistema->servidor_activo = 0;
}

// Manejador de SIGUSR1 para cliente (notificación de operación completada)
void manejador_sigusr1(int sig) {
    // TODO:
    // Verificar si hay resultados de operaciones para este cliente
    // y mostrar mensajes apropiados
}

int main(int argc, char *argv[]) {
    // Verificar si es servidor o cliente
    if (argc > 1 && strcmp(argv[1], "cliente") == 0) {
        // Ejecutar como cliente
        // Conectar al sistema existente
        cliente_id = conectar_cliente();
        if (cliente_id < 0) {
            printf("Error al conectar con el servidor\n");
            return 1;
        }
        
        // Ejecutar cliente
        cliente();
    } else {
        // Ejecutar como servidor
        servidor();
    }
    
    return 0;
}
