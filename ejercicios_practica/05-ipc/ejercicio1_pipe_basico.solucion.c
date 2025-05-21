/**
 * Ejercicio 1: SOLUCIÓN - Pipe básico
 * 
 * Esta solución implementa un programa que usa una tubería (pipe)
 * para comunicación básica entre un proceso padre y un hijo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int tuberia[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    
    printf("Demostración de tubería básica\n");
    
    // Crear la tubería
    if (pipe(tuberia) == -1) {
        perror("Error al crear la tubería");
        exit(1);
    }
    
    printf("Tubería creada. Descriptores: [lectura: %d, escritura: %d]\n", 
           tuberia[0], tuberia[1]);
    
    // Crear un proceso hijo con fork()
    pid = fork();
    
    if (pid == -1) {
        // Error en fork
        perror("Error al crear el proceso hijo");
        exit(1);
    } else if (pid == 0) {
        // Código del proceso hijo
        
        printf("Proceso hijo (PID: %d) iniciado\n", getpid());
        
        // Cerrar el extremo de lectura de la tubería ya que no lo usaremos
        close(tuberia[0]);
        
        // Preparar mensaje
        char mensaje[] = "Hola desde el proceso hijo";
        printf("Hijo: Enviando mensaje: '%s'\n", mensaje);
        
        // Escribir en la tubería
        write(tuberia[1], mensaje, strlen(mensaje) + 1);
        
        // Cerrar el extremo de escritura
        close(tuberia[1]);
        printf("Hijo: Mensaje enviado y descriptor de escritura cerrado\n");
        
        // Salir con código 0
        exit(0);
    } else {
        // Código del proceso padre
        
        printf("Proceso padre (PID: %d) creó hijo (PID: %d)\n", getpid(), pid);
        
        // Cerrar el extremo de escritura de la tubería ya que no lo usaremos
        close(tuberia[1]);
        
        // Leer el mensaje de la tubería
        int bytes_leidos = read(tuberia[0], buffer, BUFFER_SIZE);
        
        if (bytes_leidos > 0) {
            printf("Padre: Mensaje recibido del hijo: '%s'\n", buffer);
            printf("Padre: Se leyeron %d bytes\n", bytes_leidos);
        } else if (bytes_leidos == 0) {
            printf("Padre: La tubería está cerrada, no hay más datos\n");
        } else {
            perror("Error al leer de la tubería");
        }
        
        // Cerrar el extremo de lectura
        close(tuberia[0]);
        printf("Padre: Descriptor de lectura cerrado\n");
        
        // Esperar a que el hijo termine
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Padre: El hijo terminó con código de salida %d\n", 
                   WEXITSTATUS(status));
        } else {
            printf("Padre: El hijo terminó de forma anormal\n");
        }
    }
    
    printf("Programa finalizado\n");
    return 0;
}
