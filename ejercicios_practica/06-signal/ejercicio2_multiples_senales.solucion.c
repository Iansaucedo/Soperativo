/**
 * Ejercicio 2: SOLUCIÓN - Multiples señales
 * 
 * Esta es la solución al ejercicio de manejo de múltiples señales con
 * diferentes comportamientos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

// Variables globales
int contador_usr1 = 0;
int contador_usr2 = 0;
time_t tiempo_inicio;
int programa_activo = 1;

// Manejador para SIGUSR1
void manejador_usr1(int sig) {
    contador_usr1++;
    time_t tiempo_actual = time(NULL);
    double segundos = difftime(tiempo_actual, tiempo_inicio);
    
    printf("\n[SIGUSR1] Señal recibida %d veces\n", contador_usr1);
    printf("Han pasado %.2f segundos desde el inicio del programa\n", segundos);
    printf("Total de señales SIGUSR1 recibidas: %d\n", contador_usr1);
}

// Manejador para SIGUSR2
void manejador_usr2(int sig) {
    contador_usr2++;
    
    printf("\n[SIGUSR2] Señal recibida %d veces\n", contador_usr2);
    
    if (contador_usr2 % 2 == 1) {
        // Si el contador es impar, ignorar temporalmente SIGUSR1
        signal(SIGUSR1, SIG_IGN);
        printf("SIGUSR1 ahora está siendo IGNORADA\n");
    } else {
        // Si el contador es par, restaurar el manejador original para SIGUSR1
        signal(SIGUSR1, manejador_usr1);
        printf("SIGUSR1 ahora está siendo CAPTURADA\n");
    }
}

// Manejador para SIGTERM
void manejador_term(int sig) {
    printf("\n[SIGTERM] Señal de terminación recibida\n");
    printf("Resumen de señales recibidas:\n");
    printf("- SIGUSR1: %d veces\n", contador_usr1);
    printf("- SIGUSR2: %d veces\n", contador_usr2);
    
    time_t tiempo_final = time(NULL);
    printf("Tiempo total de ejecución: %.2f segundos\n", 
           difftime(tiempo_final, tiempo_inicio));
    
    programa_activo = 0;
}

// Manejador para SIGINT (Ctrl+C)
void manejador_int(int sig) {
    printf("\n[SIGINT] Señal Ctrl+C recibida y bloqueada\n");
    printf("Esta señal está bloqueada. Para terminar el programa, use SIGTERM:\n");
    printf("kill -SIGTERM %d\n", getpid());
}

int main() {
    // Registrar el tiempo de inicio
    tiempo_inicio = time(NULL);
    
    // Obtener el PID del proceso
    pid_t pid = getpid();
    
    printf("Programa de manejo de múltiples señales iniciado (PID: %d)\n", pid);
    printf("Este programa responde a varias señales:\n");
    printf("- SIGUSR1: Muestra el tiempo transcurrido y cuenta señales\n");
    printf("- SIGUSR2: Alterna entre ignorar y capturar SIGUSR1\n");
    printf("- SIGINT (Ctrl+C): Bloqueada, muestra mensaje de ayuda\n");
    printf("- SIGTERM: Termina el programa mostrando estadísticas\n\n");
    printf("Para enviar señales desde otra terminal, usa:\n");
    printf("  kill -SIGUSR1 %d\n", pid);
    printf("  kill -SIGUSR2 %d\n", pid);
    printf("  kill -SIGTERM %d\n", pid);
    
    // Registrar los manejadores de señales
    if (signal(SIGUSR1, manejador_usr1) == SIG_ERR ||
        signal(SIGUSR2, manejador_usr2) == SIG_ERR ||
        signal(SIGTERM, manejador_term) == SIG_ERR ||
        signal(SIGINT, manejador_int) == SIG_ERR) {
        perror("Error al registrar los manejadores de señales");
        return 1;
    }
    
    // Bucle principal
    while (programa_activo) {
        printf("Programa en ejecución. SIGUSR1: %d, SIGUSR2: %d\n", 
               contador_usr1, contador_usr2);
        sleep(3);
    }
    
    printf("Programa terminado correctamente.\n");
    return 0;
}
