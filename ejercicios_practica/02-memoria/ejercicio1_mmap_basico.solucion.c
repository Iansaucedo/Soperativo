/**
 * Ejercicio 1: SOLUCIÓN - Uso básico de mmap para memoria compartida
 * 
 * Esta solución implementa un programa que utiliza mmap
 * para crear una región de memoria compartida entre procesos.
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
    
    // Crear una región de memoria compartida con mmap
    void *shared_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, 
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (shared_memory == MAP_FAILED) {
        perror("Error al crear memoria compartida");
        exit(1);
    }
    
    printf("Memoria compartida creada en dirección %p\n", shared_memory);
    
    // Copiar un mensaje inicial en la región de memoria
    char *mensaje = (char *)shared_memory;
    strcpy(mensaje, "Hola desde el proceso padre");
    
    printf("Padre: Escribió mensaje en memoria compartida: '%s'\n", mensaje);
    
    // Crear un proceso hijo usando fork
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Error en fork");
        munmap(shared_memory, MEM_SIZE);
        exit(1);
    } else if (pid == 0) {
        // Código del proceso hijo
        
        // Leer el mensaje colocado por el padre
        printf("Hijo: Leyó mensaje de memoria compartida: '%s'\n", mensaje);
        
        // Modificar el mensaje
        strcpy(mensaje, "Modificado por el hijo");
        printf("Hijo: Modificó mensaje a: '%s'\n", mensaje);
        
        // Salir con código 0
        exit(0);
    } else {
        // Código del proceso padre
        
        // Esperar a que el hijo termine
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Padre: Hijo terminó con código %d\n", WEXITSTATUS(status));
        }
        
        // Leer el mensaje modificado por el hijo
        printf("Padre: Mensaje modificado en memoria compartida: '%s'\n", mensaje);
        
        // Liberar la región de memoria
        if (munmap(shared_memory, MEM_SIZE) == -1) {
            perror("Error al liberar memoria compartida");
            exit(1);
        }
        printf("Memoria compartida liberada\n");
    }
    
    return 0;
}
