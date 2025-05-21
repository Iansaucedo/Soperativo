/**
 * Ejercicio 1: Uso básico de mmap para memoria compartida
 * 
 * En este ejercicio implementarás un programa que utiliza mmap
 * para crear una región de memoria compartida entre un proceso padre y un hijo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

#define MEM_SIZE 4096

int main() {
    printf("Demostración de memoria compartida con mmap\n");
    
    // TODO: Crear una región de memoria compartida con mmap
    // La región debe tener tamaño MEM_SIZE y ser compartida entre procesos
    // Establecer permisos de lectura y escritura
    
    // TODO: Copiar un mensaje inicial en la región de memoria
    // Por ejemplo: "Hola desde el proceso padre"
    
    // TODO: Crear un proceso hijo usando fork
    
    // TODO: En el proceso hijo:
    // 1. Leer el mensaje colocado por el padre e imprimirlo
    // 2. Modificar el mensaje (por ejemplo: "Modificado por el hijo")
    // 3. Salir con código de salida 0
    
    // TODO: En el proceso padre:
    // 1. Esperar a que el hijo termine
    // 2. Leer e imprimir el mensaje modificado por el hijo
    // 3. Liberar la región de memoria con munmap
    
    return 0;
}
