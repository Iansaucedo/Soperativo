/**
 * Ejercicio 1: SOLUCIÓN - Manejador básico de señales
 * 
 * Esta es la solución al ejercicio de implementación de manejadores básicos de señales.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Variables globales
int contador_interrupciones = 0;
int programa_activo = 1;

// Manejador para SIGINT (Ctrl+C)
void manejador_sigint(int sig) {
    contador_interrupciones++;
    printf("\n[SIGINT] Has pulsado Ctrl+C %d veces\n", contador_interrupciones);
    
    if (contador_interrupciones >= 3) {
        printf("Has alcanzado el límite de 3 interrupciones.\n");
        printf("La próxima vez que envíes SIGTERM, el programa terminará.\n");
    } else {
        printf("Pulsa Ctrl+C de nuevo o envía SIGTERM para terminar.\n");
    }
}

// Manejador para SIGTERM
void manejador_sigterm(int sig) {
    printf("\n[SIGTERM] Se ha recibido la señal de terminación.\n");
    printf("El programa terminará en breve...\n");
    programa_activo = 0;
}

int main() {
    // Identificador del proceso actual
    pid_t pid = getpid();
    
    printf("Programa de manejo básico de señales iniciado (PID: %d)\n", pid);
    printf("Pulsa Ctrl+C para enviar SIGINT o ejecuta 'kill -SIGTERM %d' desde otra terminal\n", pid);
    
    // Registrar los manejadores de señales
    if (signal(SIGINT, manejador_sigint) == SIG_ERR) {
        perror("Error al registrar manejador para SIGINT");
        return 1;
    }
    
    if (signal(SIGTERM, manejador_sigterm) == SIG_ERR) {
        perror("Error al registrar manejador para SIGTERM");
        return 1;
    }
    
    // Bucle principal
    while (programa_activo) {
        printf("Programa en ejecución. Interrupciones recibidas: %d\n", contador_interrupciones);
        sleep(2);
    }
    
    printf("Programa terminado correctamente.\n");
    return 0;
}
