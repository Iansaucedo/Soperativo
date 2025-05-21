/**
 * Ejercicio 3: Ejecución de comandos con exec
 * 
 * En este ejercicio implementarás un programa que usa fork y exec
 * para ejecutar comandos externos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMANDO 100
#define MAX_ARGS 10

// Función para dividir una cadena en tokens (argumentos)
void tokenizar_comando(char *comando, char *args[]) {
    int i = 0;
    args[i] = strtok(comando, " ");
    
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        i++;
        args[i] = strtok(NULL, " ");
    }
}

int main() {
    char comando[MAX_COMANDO];
    char *args[MAX_ARGS];
    int estado;
    
    printf("Programa para ejecución de comandos\n");
    printf("Escribe 'salir' para terminar\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        // Leer comando del usuario
        if (fgets(comando, MAX_COMANDO, stdin) == NULL) {
            break;
        }
        
        // Eliminar el salto de línea final
        comando[strcspn(comando, "\n")] = '\0';
        
        // Comprobar si el usuario quiere salir
        if (strcmp(comando, "salir") == 0) {
            break;
        }
        
        // TODO: Dividir el comando en argumentos
        // Usar la función tokenizar_comando
        
        // TODO: Crear un proceso hijo para ejecutar el comando
        // 1. Usar fork()
        // 2. El hijo debe ejecutar el comando con execvp()
        // 3. El padre debe esperar a que el hijo termine
        // 4. Imprimir el código de salida del comando
        
    }
    
    printf("Programa terminado\n");
    return 0;
}
