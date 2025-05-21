/**
 * Ejercicio 1: Gestor de Tareas
 * 
 * En este ejercicio implementarás un gestor de tareas que utiliza procesos,
 * hilos y señales para administrar y ejecutar tareas en segundo plano.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define MAX_TAREAS 10
#define MAX_COMANDO 100
#define MAX_ARGS 10

// Estructura para representar una tarea
typedef struct {
    int id;                 // Identificador único de la tarea
    pid_t pid;              // PID del proceso que ejecuta la tarea
    char comando[MAX_COMANDO]; // Comando a ejecutar
    time_t tiempo_inicio;   // Tiempo de inicio de la tarea
    int estado;             // 0: pendiente, 1: en ejecución, 2: completada, 3: cancelada
    int prioridad;          // 1: baja, 2: normal, 3: alta
} Tarea;

// Variables globales
Tarea tareas[MAX_TAREAS];
int num_tareas = 0;
pthread_mutex_t mutex_tareas;
pthread_t hilo_monitor;
int continuar_monitor = 1;

// Prototipos de funciones
void inicializar_gestor();
void *monitorear_tareas(void *arg);
int agregar_tarea(char *comando, int prioridad);
void ejecutar_tarea(int id_tarea);
void cancelar_tarea(int id_tarea);
void listar_tareas();
void finalizar_gestor();
void manejador_sigchld(int sig);

// Inicializar el gestor de tareas
void inicializar_gestor() {
    // TODO: 
    // 1. Inicializar el array de tareas
    // 2. Inicializar el mutex para proteger el acceso al array
    // 3. Configurar el manejador de la señal SIGCHLD para detectar cuando las tareas terminan
    // 4. Crear el hilo de monitoreo que periódicamente comprobará el estado de las tareas
}

// Función para el hilo que monitorea las tareas
void *monitorear_tareas(void *arg) {
    // TODO:
    // 1. En un bucle mientras continuar_monitor sea verdadero:
    //   a. Bloquear el mutex
    //   b. Recorrer todas las tareas en ejecución y actualizar estadísticas
    //      (como tiempo de ejecución)
    //   c. Desbloquear el mutex
    //   d. Dormir durante un tiempo (por ejemplo, 1 segundo)
    
    return NULL;
}

// Agregar una nueva tarea al gestor
int agregar_tarea(char *comando, int prioridad) {
    // TODO:
    // 1. Bloquear el mutex
    // 2. Verificar si hay espacio para una nueva tarea
    // 3. Inicializar la nueva tarea con los parámetros proporcionados
    //    (estado inicial: pendiente)
    // 4. Incrementar el contador de tareas
    // 5. Desbloquear el mutex
    // 6. Retornar el ID de la nueva tarea
    
    return -1; // Error
}

// Ejecutar una tarea específica
void ejecutar_tarea(int id_tarea) {
    // TODO:
    // 1. Bloquear el mutex
    // 2. Verificar que la tarea existe y está en estado pendiente
    // 3. Crear un proceso hijo usando fork() para ejecutar la tarea
    // 4. En el proceso hijo:
    //   a. Dividir el comando en argumentos
    //   b. Ejecutar el comando con execvp()
    // 5. En el proceso padre:
    //   a. Actualizar el PID y el estado de la tarea a "en ejecución"
    //   b. Registrar el tiempo de inicio
    // 6. Desbloquear el mutex
}

// Cancelar una tarea en ejecución
void cancelar_tarea(int id_tarea) {
    // TODO:
    // 1. Bloquear el mutex
    // 2. Verificar que la tarea existe y está en ejecución
    // 3. Enviar la señal SIGTERM al proceso que ejecuta la tarea
    // 4. Actualizar el estado de la tarea a "cancelada"
    // 5. Desbloquear el mutex
}

// Listar todas las tareas y su estado
void listar_tareas() {
    // TODO:
    // 1. Bloquear el mutex
    // 2. Recorrer el array de tareas e imprimir información de cada una:
    //   - ID, comando, estado, tiempo de ejecución, prioridad
    // 3. Desbloquear el mutex
}

// Finalizar el gestor de tareas
void finalizar_gestor() {
    // TODO:
    // 1. Cancelar todas las tareas en ejecución
    // 2. Detener el hilo de monitoreo (cambiar continuar_monitor y esperar con join)
    // 3. Destruir el mutex
    // 4. Liberar cualquier otro recurso
}

// Manejador de la señal SIGCHLD para detectar cuando terminan las tareas
void manejador_sigchld(int sig) {
    // TODO:
    // 1. Usar waitpid() con WNOHANG para recoger el estado de los procesos terminados
    // 2. Buscar la tarea correspondiente al PID terminado
    // 3. Actualizar su estado a "completada"
    // 4. Registrar el tiempo de finalización
}

int main() {
    int opcion;
    char comando[MAX_COMANDO];
    int id_tarea, prioridad;
    
    printf("=== GESTOR DE TAREAS ===\n");
    
    // Inicializar el gestor
    inicializar_gestor();
    
    // Menú principal
    while (1) {
        printf("\nOPCIONES:\n");
        printf("1. Agregar tarea\n");
        printf("2. Ejecutar tarea\n");
        printf("3. Cancelar tarea\n");
        printf("4. Listar tareas\n");
        printf("5. Salir\n");
        printf("Seleccione una opción: ");
        
        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n'); // Limpiar buffer
            printf("Entrada inválida\n");
            continue;
        }
        
        switch (opcion) {
            case 1:
                // Agregar tarea
                printf("Ingrese el comando: ");
                while (getchar() != '\n'); // Limpiar buffer
                fgets(comando, MAX_COMANDO, stdin);
                comando[strcspn(comando, "\n")] = 0; // Eliminar salto de línea
                
                printf("Prioridad (1: baja, 2: normal, 3: alta): ");
                scanf("%d", &prioridad);
                
                id_tarea = agregar_tarea(comando, prioridad);
                if (id_tarea >= 0) {
                    printf("Tarea agregada con ID %d\n", id_tarea);
                }
                break;
                
            case 2:
                // Ejecutar tarea
                printf("Ingrese el ID de la tarea a ejecutar: ");
                scanf("%d", &id_tarea);
                ejecutar_tarea(id_tarea);
                break;
                
            case 3:
                // Cancelar tarea
                printf("Ingrese el ID de la tarea a cancelar: ");
                scanf("%d", &id_tarea);
                cancelar_tarea(id_tarea);
                break;
                
            case 4:
                // Listar tareas
                listar_tareas();
                break;
                
            case 5:
                // Salir
                printf("Finalizando gestor de tareas...\n");
                finalizar_gestor();
                return 0;
                
            default:
                printf("Opción no válida\n");
        }
    }
    
    return 0;
}
