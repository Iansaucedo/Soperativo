/**
 * Ejercicio 1: Manejador básico de señales
 * 
 * En este ejercicio implementarás un programa que capture y maneje 
 * las señales SIGINT (Ctrl+C) y SIGTERM de manera personalizada.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Variables globales
int contador_interrupciones = 0;
int programa_activo = 1;

// TODO: Implementar el manejador para SIGINT (Ctrl+C)
// El manejador debe:
// 1. Incrementar el contador de interrupciones
// 2. Mostrar un mensaje con el número de veces que se ha pulsado Ctrl+C
// 3. Si se han recibido 3 o más interrupciones, mostrar un mensaje de despedida pero NO terminar el programa

void manejador_sigint(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementar el manejador para SIGTERM
// El manejador debe:
// 1. Mostrar un mensaje indicando que se ha recibido la señal de terminación
// 2. Cambiar la variable programa_activo a 0 para salir del bucle principal

void manejador_sigterm(int sig) {
    // TU CÓDIGO AQUÍ
}

int main() {
    // Identificador del proceso actual
    pid_t pid = getpid();
    
    printf("Programa de manejo básico de señales iniciado (PID: %d)\n", pid);
    printf("Pulsa Ctrl+C para enviar SIGINT o ejecuta 'kill -SIGTERM %d' desde otra terminal\n", pid);
    
    // TODO: Registrar los manejadores de señales para SIGINT y SIGTERM
    // TU CÓDIGO AQUÍ
    
    // Bucle principal
    while (programa_activo) {
        printf("Programa en ejecución. Interrupciones recibidas: %d\n", contador_interrupciones);
        sleep(2);
    }
    
    printf("Programa terminado correctamente.\n");
    return 0;
}
