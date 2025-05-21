/**
 * Ejercicio 1: Pipe básico
 * 
 * En este ejercicio implementarás un programa que usa una tubería (pipe)
 * para comunicación básica entre un proceso padre y un hijo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100

int main() {
    int tuberia[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    
    printf("Demostración de tubería básica\n");
    
    // TODO: Crear la tubería
    // Usar la función pipe() para crear una tubería unidireccional
    
    // TODO: Manejar errores de creación de la tubería
    
    // TODO: Crear un proceso hijo con fork()
    
    // TODO: En el proceso hijo:
    // 1. Cerrar el extremo de lectura de la tubería
    // 2. Escribir un mensaje en la tubería
    // 3. Cerrar el extremo de escritura
    // 4. Salir con código 0
    
    // TODO: En el proceso padre:
    // 1. Cerrar el extremo de escritura de la tubería
    // 2. Leer el mensaje de la tubería
    // 3. Mostrar el mensaje recibido
    // 4. Cerrar el extremo de lectura
    // 5. Esperar a que el hijo termine
    
    return 0;
}
