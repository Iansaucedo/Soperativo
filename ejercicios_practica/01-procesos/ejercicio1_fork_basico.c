/**
 * Ejercicio 1: Fork Básico
 * 
 * En este ejercicio implementarás un programa que crea procesos hijos
 * y demuestra la comunicación básica entre ellos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_HIJOS 3

int main() {
    printf("Programa de demostración de fork básico\n");
    printf("Proceso padre PID: %d\n", getpid());
    
    // TODO: Crear un bucle que genere NUM_HIJOS procesos hijos
    // Cada hijo debe:
    // 1. Imprimir su PID y el PID de su padre
    // 2. Esperar un tiempo aleatorio entre 1 y 5 segundos
    // 3. Salir con un código de salida igual a su número de orden (0, 1, o 2)

    // El padre debe:
    // 1. Esperar a que todos los hijos terminen
    // 2. Imprimir el PID y código de salida de cada hijo
    
    return 0;
}
