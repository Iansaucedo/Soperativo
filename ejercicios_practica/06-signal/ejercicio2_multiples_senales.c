/**
 * Ejercicio 2: Manejo de múltiples señales
 * 
 * En este ejercicio implementarás un programa que maneje múltiples señales
 * y demuestre diferentes comportamientos para cada una.
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

// TODO: Implementar un manejador para SIGUSR1
// El manejador debe:
// 1. Incrementar contador_usr1
// 2. Mostrar el tiempo transcurrido desde el inicio del programa
// 3. Mostrar el número total de señales SIGUSR1 recibidas

void manejador_usr1(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementar un manejador para SIGUSR2
// El manejador debe:
// 1. Incrementar contador_usr2
// 2. Si el contador es impar, ignorar temporalmente SIGUSR1 (usar SIG_IGN)
// 3. Si el contador es par, restaurar el manejador original para SIGUSR1
// 4. Mostrar el estado actual (si SIGUSR1 está siendo ignorada o no)

void manejador_usr2(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementar un manejador para SIGTERM
// El manejador debe:
// 1. Mostrar un resumen de las señales recibidas
// 2. Cambiar programa_activo a 0 para que el bucle principal termine

void manejador_term(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementar un manejador para SIGINT (Ctrl+C)
// El manejador debe:
// 1. Mostrar un mensaje indicando que SIGINT está bloqueada
// 2. Sugerir usar SIGTERM para terminar el programa

void manejador_int(int sig) {
    // TU CÓDIGO AQUÍ
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
    
    // TODO: Registrar todos los manejadores de señales
    // TU CÓDIGO AQUÍ
    
    // Bucle principal
    while (programa_activo) {
        printf("Programa en ejecución. SIGUSR1: %d, SIGUSR2: %d\n", 
               contador_usr1, contador_usr2);
        sleep(3);
    }
    
    printf("Programa terminado correctamente.\n");
    return 0;
}
