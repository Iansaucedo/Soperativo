/**
 * Ejercicio 1: SOLUCIÓN - Fork Básico
 * 
 * Esta es la solución al ejercicio de fork básico para crear procesos hijos
 * y demostrar la comunicación básica entre ellos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_HIJOS 3

int main() {
    printf("Programa de demostración de fork básico\n");
    printf("Proceso padre PID: %d\n", getpid());
    
    // Inicializar la semilla para números aleatorios
    srand(time(NULL));
    
    // Array para almacenar los PIDs de los hijos
    pid_t pids[NUM_HIJOS];
    
    // Crear NUM_HIJOS procesos hijos
    for (int i = 0; i < NUM_HIJOS; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            // Error al hacer fork
            perror("Error al crear proceso hijo");
            exit(1);
        } else if (pid == 0) {
            // Código del proceso hijo
            printf("Hijo %d: Mi PID es %d, PID de mi padre es %d\n", 
                   i, getpid(), getppid());
            
            // Esperar un tiempo aleatorio entre 1 y 5 segundos
            int tiempo = 1 + rand() % 5;
            printf("Hijo %d: Trabajando durante %d segundos...\n", i, tiempo);
            sleep(tiempo);
            
            printf("Hijo %d: Terminando con código %d\n", i, i);
            exit(i);  // Salir con código igual a su número de orden
        } else {
            // Código del proceso padre
            printf("Padre: Creado hijo %d con PID %d\n", i, pid);
            pids[i] = pid;
        }
    }
    
    // El padre espera a que todos los hijos terminen
    printf("Padre: Esperando a que terminen todos los hijos...\n");
    
    for (int i = 0; i < NUM_HIJOS; i++) {
        int status;
        pid_t pid_terminado = waitpid(pids[i], &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Padre: Hijo con PID %d terminó con código %d\n", 
                   pid_terminado, WEXITSTATUS(status));
        } else {
            printf("Padre: Hijo con PID %d terminó de forma anormal\n", pid_terminado);
        }
    }
    
    printf("Padre: Todos los hijos han terminado. Finalizando programa.\n");
    return 0;
}
