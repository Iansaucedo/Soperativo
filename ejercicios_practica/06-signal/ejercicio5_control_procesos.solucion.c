/**
 * Ejercicio 5: Control de procesos con señales (SOLUCIÓN)
 *
 * En este ejercicio implementarás un programa que crea varios procesos
 * hijos y los controla mediante señales.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_HIJOS 4

// Variables globales
pid_t hijos[NUM_HIJOS];
int hijos_activos = NUM_HIJOS;
int continuar_ejecucion = 1;

// Manejador de SIGINT para el padre
// Este manejador:
// 1. Muestra un mensaje indicando que se va a detener a todos los hijos
// 2. Envía SIGSTOP a todos los procesos hijo activos
void manejador_sigint_padre(int sig) {
    printf("\n[Padre] Recibido SIGINT: Deteniendo todos los hijos\n");
    
    for (int i = 0; i < NUM_HIJOS; i++) {
        // Verificar si el hijo está activo
        if (hijos[i] > 0) {
            printf("[Padre] Enviando SIGSTOP al hijo %d (PID: %d)\n", i, hijos[i]);
            kill(hijos[i], SIGSTOP);
        }
    }
}

// Manejador de SIGQUIT para el padre
// Este manejador:
// 1. Muestra un mensaje indicando que se van a reanudar todos los hijos
// 2. Envía SIGCONT a todos los procesos hijo activos
void manejador_sigquit_padre(int sig) {
    printf("\n[Padre] Recibido SIGQUIT: Reanudando todos los hijos\n");
    
    for (int i = 0; i < NUM_HIJOS; i++) {
        // Verificar si el hijo está activo
        if (hijos[i] > 0) {
            printf("[Padre] Enviando SIGCONT al hijo %d (PID: %d)\n", i, hijos[i]);
            kill(hijos[i], SIGCONT);
        }
    }
}

// Manejador de SIGTERM para el padre
// Este manejador:
// 1. Muestra un mensaje indicando que se van a terminar todos los hijos
// 2. Envía SIGTERM a todos los procesos hijo activos
// 3. Cambia continuar_ejecucion a 0 para que el bucle principal termine
void manejador_sigterm_padre(int sig) {
    printf("\n[Padre] Recibido SIGTERM: Terminando el programa\n");
    
    for (int i = 0; i < NUM_HIJOS; i++) {
        // Verificar si el hijo está activo
        if (hijos[i] > 0) {
            printf("[Padre] Enviando SIGTERM al hijo %d (PID: %d)\n", i, hijos[i]);
            kill(hijos[i], SIGTERM);
        }
    }
    
    // Indicar al bucle principal que debe terminar
    continuar_ejecucion = 0;
}

// Manejador de SIGUSR1 para el padre
// Este manejador gestiona la notificación de que un hijo ha terminado su tarea
// 1. Decrementa el contador de hijos activos
// 2. Muestra un mensaje indicando cuántos hijos siguen activos
void manejador_sigusr1_padre(int sig) {
    hijos_activos--;
    printf("\n[Padre] Un hijo ha terminado su tarea. Hijos activos: %d\n", hijos_activos);
}

// Manejador de SIGTERM para los hijos
// Este manejador:
// 1. Muestra un mensaje de despedida
// 2. Termina el proceso hijo
void manejador_sigterm_hijo(int sig) {
    printf("[Hijo %d] Recibido SIGTERM: Terminando proceso\n", getpid());
    exit(0);
}

// Manejador de SIGCONT para los hijos
// Este manejador muestra un mensaje informativo cuando se reanuda un proceso
void manejador_sigcont_hijo(int sig) {
    printf("[Hijo %d] Recibido SIGCONT: Reanudando ejecución\n", getpid());
}

// Función que ejecuta cada proceso hijo
void proceso_hijo(int id) {
    printf("[Hijo %d] (PID: %d) iniciado\n", id, getpid());
    
    // Configurar manejadores de señales para el hijo
    signal(SIGTERM, manejador_sigterm_hijo);
    signal(SIGCONT, manejador_sigcont_hijo);
    
    // Generar un tiempo aleatorio para la tarea
    srand(time(NULL) + id);
    int tiempo_total = 20 + rand() % 20; // Entre 20 y 39 segundos
    int tiempo_transcurrido = 0;
    
    // Simular una tarea de larga duración
    while (tiempo_transcurrido < tiempo_total) {
        sleep(1);
        tiempo_transcurrido++;
        
        // Mostrar progreso cada 5 segundos
        if (tiempo_transcurrido % 5 == 0) {
            printf("[Hijo %d] Progreso: %d/%d segundos\n", id, tiempo_transcurrido, tiempo_total);
        }
    }
    
    printf("[Hijo %d] ¡Tarea completada!\n", id);
    
    // Notificar al padre que la tarea está completa
    kill(getppid(), SIGUSR1);
    exit(0);
}

int main() {
    printf("Programa de control de procesos con señales\n");
    printf("Se crearán %d procesos hijos que realizarán tareas\n", NUM_HIJOS);
    printf("Comandos disponibles (desde otra terminal):\n");
    printf("  kill -SIGINT %d   : Detener todos los hijos\n", getpid());
    printf("  kill -SIGQUIT %d  : Reanudar todos los hijos\n", getpid());
    printf("  kill -SIGTERM %d  : Terminar el programa\n", getpid());
    
    // Configurar manejadores de señales para el padre
    signal(SIGINT, manejador_sigint_padre);
    signal(SIGQUIT, manejador_sigquit_padre);
    signal(SIGTERM, manejador_sigterm_padre);
    signal(SIGUSR1, manejador_sigusr1_padre);
    
    // Crear los procesos hijos
    for (int i = 0; i < NUM_HIJOS; i++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("Error en fork");
            exit(1);
        } else if (pid == 0) {
            // Código del hijo
            proceso_hijo(i);
            exit(0); // No debería llegar aquí, pero por seguridad
        } else {
            // Código del padre
            hijos[i] = pid;
            printf("[Padre] Creado hijo %d con PID %d\n", i, pid);
        }
    }
    
    // Bucle principal del padre
    while (continuar_ejecucion && hijos_activos > 0) {
        printf("[Padre] Esperando. Hijos activos: %d\n", hijos_activos);
        sleep(3);
    }
    
    // Esperar a que terminen todos los hijos
    for (int i = 0; i < NUM_HIJOS; i++) {
        waitpid(hijos[i], NULL, 0);
    }
    
    printf("[Padre] Todos los hijos han terminado. Fin del programa.\n");
    return 0;
}
