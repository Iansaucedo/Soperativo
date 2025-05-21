/**
 * Ejercicio 3: Calculadora usando tuberías
 *
 * En este ejercicio implementarás una calculadora que utiliza procesos
 * y tuberías para realizar operaciones matemáticas básicas.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// Pipes para comunicación entre procesos
int tuberia_padre_hijo[2];  // Padre escribe, hijo lee
int tuberia_hijo_padre[2];  // Hijo escribe, padre lee

// Estructura que representa una operación matemática
typedef struct {
    float operando1;
    float operando2;
    char operacion;  // '+', '-', '*', '/'
} Operacion;

// Estructura que representa el resultado de una operación
typedef struct {
    float resultado;
    int error;  // 0: sin error, 1: error (por ejemplo, división por cero)
} Resultado;

// Función para el proceso hijo (calculadora)
void proceso_calculadora() {
    printf("[Calculadora] Proceso calculadora iniciado (PID: %d)\n", getpid());
    
    // Cerrar extremos no utilizados de las tuberías
    close(tuberia_padre_hijo[1]);  // Cerrar escritura en tubería padre->hijo
    close(tuberia_hijo_padre[0]);  // Cerrar lectura en tubería hijo->padre
    
    Operacion op;
    Resultado res;
    
    // Bucle para recibir operaciones y enviar resultados
    while (1) {
        // Leer operación de la tubería
        ssize_t bytes_leidos = read(tuberia_padre_hijo[0], &op, sizeof(op));
        
        if (bytes_leidos <= 0) {
            // El padre ha cerrado la tubería o ha ocurrido un error
            break;
        }
        
        printf("[Calculadora] Recibida operación: %.2f %c %.2f\n", 
               op.operando1, op.operacion, op.operando2);
        
        // Realizar la operación
        res.error = 0;
        switch (op.operacion) {
            case '+':
                res.resultado = op.operando1 + op.operando2;
                break;
            case '-':
                res.resultado = op.operando1 - op.operando2;
                break;
            case '*':
                res.resultado = op.operando1 * op.operando2;
                break;
            case '/':
                if (op.operando2 == 0) {
                    res.error = 1;  // Error: división por cero
                    res.resultado = 0;
                } else {
                    res.resultado = op.operando1 / op.operando2;
                }
                break;
            default:
                res.error = 1;  // Error: operación no reconocida
                res.resultado = 0;
        }
        
        // Enviar resultado al padre
        write(tuberia_hijo_padre[1], &res, sizeof(res));
    }
    
    printf("[Calculadora] Proceso calculadora finalizado\n");
    
    // Cerrar extremos restantes de las tuberías
    close(tuberia_padre_hijo[0]);
    close(tuberia_hijo_padre[1]);
    
    exit(0);
}

// Función principal para el proceso padre
int main() {
    printf("=== CALCULADORA CON TUBERÍAS ===\n");
    
    // Crear las tuberías
    if (pipe(tuberia_padre_hijo) == -1 || pipe(tuberia_hijo_padre) == -1) {
        perror("Error al crear las tuberías");
        return 1;
    }
    
    // Crear proceso hijo
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Error al crear proceso hijo");
        return 1;
    } else if (pid == 0) {
        // Proceso hijo
        proceso_calculadora();
        // No debería llegar aquí
        return 0;
    }
    
    // Proceso padre
    printf("[Padre] Proceso principal iniciado (PID: %d)\n", getpid());
    printf("[Padre] Proceso calculadora creado con PID: %d\n", pid);
    
    // Cerrar extremos no utilizados de las tuberías
    close(tuberia_padre_hijo[0]);  // Cerrar lectura en tubería padre->hijo
    close(tuberia_hijo_padre[1]);  // Cerrar escritura en tubería hijo->padre
    
    // Variables para la interfaz de usuario
    char entrada[100];
    int continuar = 1;
    Operacion op;
    Resultado res;
    
    printf("Introduce operaciones en el formato 'num1 operador num2'\n");
    printf("Operadores válidos: +, -, *, /\n");
    printf("Escribe 'salir' para terminar\n");
    
    // Bucle principal
    while (continuar) {
        printf("\n> ");
        fflush(stdout);
        
        // Leer entrada del usuario
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            break;
        }
        
        // Eliminar el salto de línea final
        entrada[strcspn(entrada, "\n")] = '\0';
        
        // Verificar si el usuario quiere salir
        if (strcmp(entrada, "salir") == 0) {
            continuar = 0;
            continue;
        }
        
        // Parsear la entrada del usuario
        if (sscanf(entrada, "%f %c %f", &op.operando1, &op.operacion, &op.operando2) != 3) {
            printf("Error: formato incorrecto. Usa 'num1 operador num2'\n");
            continue;
        }
        
        // Enviar operación al proceso calculadora
        write(tuberia_padre_hijo[1], &op, sizeof(op));
        
        // Leer resultado
        read(tuberia_hijo_padre[0], &res, sizeof(res));
        
        // Mostrar resultado
        if (res.error) {
            printf("Error en la operación\n");
        } else {
            printf("Resultado: %.2f\n", res.resultado);
        }
    }
    
    // Cerrar tuberías antes de terminar
    close(tuberia_padre_hijo[1]);
    close(tuberia_hijo_padre[0]);
    
    // Esperar a que el proceso hijo termine
    waitpid(pid, NULL, 0);
    
    printf("[Padre] Calculadora finalizada\n");
    
    return 0;
}
