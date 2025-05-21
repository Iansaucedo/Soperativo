/**
 * Ejercicio 3: SOLUCIÓN - Ejecución de comandos con exec
 * 
 * Esta solución implementa un programa que usa fork y exec
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
        
        // Dividir el comando en argumentos
        memset(args, 0, sizeof(args));
        tokenizar_comando(comando, args);
        
        // Verificar si se ingresó algún comando
        if (args[0] == NULL) {
            continue;  // Línea vacía, volver al inicio del bucle
        }
        
        // Crear un proceso hijo para ejecutar el comando
        pid_t pid = fork();
        
        if (pid < 0) {
            // Error al hacer fork
            perror("Error al crear proceso hijo");
        } else if (pid == 0) {
            // Código del proceso hijo
            
            // Ejecutar el comando
            execvp(args[0], args);
            
            // Si llegamos aquí, es porque execvp falló
            perror("Error al ejecutar el comando");
            exit(1);
        } else {
            // Código del proceso padre
            // Esperar a que el hijo termine
            waitpid(pid, &estado, 0);
            
            if (WIFEXITED(estado)) {
                printf("Comando terminado con código %d\n", WEXITSTATUS(estado));
            } else {
                printf("Comando terminado de forma anormal\n");
            }
        }
    }
    
    printf("Programa terminado\n");
    return 0;
}
