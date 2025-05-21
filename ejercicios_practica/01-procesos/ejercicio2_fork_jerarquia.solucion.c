/**
 * Ejercicio 2: SOLUCIÓN - Jerarquía de procesos con fork
 * 
 * Esta solución implementa un programa que crea una jerarquía
 * de procesos con forma de árbol.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Estructura de árbol: cada proceso puede tener 0, 1 o 2 hijos
#define NIVEL_MAXIMO 3

// Función para crear procesos hijos recursivamente
void crear_hijos(int nivel_actual, int max_nivel, int id_proceso) {
    if (nivel_actual >= max_nivel) {
        // Llegamos al nivel máximo, este proceso no tiene hijos
        printf("Proceso hoja: nivel=%d, id=%d, PID=%d\n", 
               nivel_actual, id_proceso, getpid());
        sleep(2);  // Simular trabajo
        exit(nivel_actual);  // Terminar con código de nivel
    }
    
    printf("Proceso interno: nivel=%d, id=%d, PID=%d\n", 
           nivel_actual, id_proceso, getpid());
    
    // Crear el primer hijo
    pid_t pid1 = fork();
    
    if (pid1 < 0) {
        perror("Error en fork del primer hijo");
        exit(1);
    } else if (pid1 == 0) {
        // Código del primer hijo
        crear_hijos(nivel_actual + 1, max_nivel, id_proceso * 2 + 1);
        // Este código no se ejecutará debido al exit en crear_hijos
    }
    
    // El padre crea el segundo hijo
    pid_t pid2 = fork();
    
    if (pid2 < 0) {
        perror("Error en fork del segundo hijo");
        kill(pid1, SIGTERM);  // Terminar el primer hijo si hubo error
        exit(1);
    } else if (pid2 == 0) {
        // Código del segundo hijo
        crear_hijos(nivel_actual + 1, max_nivel, id_proceso * 2 + 2);
        // Este código no se ejecutará debido al exit en crear_hijos
    }
    
    // El padre espera a que ambos hijos terminen
    int status1, status2;
    pid_t terminado1 = waitpid(pid1, &status1, 0);
    pid_t terminado2 = waitpid(pid2, &status2, 0);
    
    printf("Proceso %d: Hijo izquierdo (PID %d) terminó con estado %d\n", 
           id_proceso, terminado1, WEXITSTATUS(status1));
    printf("Proceso %d: Hijo derecho (PID %d) terminó con estado %d\n", 
           id_proceso, terminado2, WEXITSTATUS(status2));
    
    exit(nivel_actual);  // El padre también termina con su nivel
}

int main() {
    printf("Creando jerarquía de procesos\n");
    
    // Iniciar la jerarquía llamando a crear_hijos
    // El proceso raíz tiene nivel 0 e id 0
    crear_hijos(0, NIVEL_MAXIMO, 0);
    
    return 0;  // Este punto nunca se alcanzará debido al exit en crear_hijos
}
