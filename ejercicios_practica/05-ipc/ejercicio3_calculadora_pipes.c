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

// TODO: Implementa la función para el proceso hijo (calculadora)
// Esta función debe:
// 1. Cerrar los extremos no utilizados de las tuberías
// 2. Entrar en un bucle para recibir operaciones del padre
// 3. Realizar las operaciones matemáticas según el operador recibido
// 4. Enviar los resultados al padre
// 5. Salir cuando se cierre la tubería

void proceso_calculadora() {
    // TU CÓDIGO AQUÍ
}

int main() {
    printf("=== CALCULADORA CON TUBERÍAS ===\n");
    
    // TODO: Crear las tuberías
    // TU CÓDIGO AQUÍ
    
    // TODO: Crear el proceso hijo
    // TU CÓDIGO AQUÍ
    
    // TODO: En el proceso padre:
    // 1. Cerrar los extremos no utilizados de las tuberías
    // 2. Solicitar operaciones al usuario
    // 3. Enviar las operaciones al proceso hijo
    // 4. Recibir y mostrar los resultados
    // 5. Finalizar cuando el usuario lo indique
    
    // TU CÓDIGO AQUÍ
    
    return 0;
}