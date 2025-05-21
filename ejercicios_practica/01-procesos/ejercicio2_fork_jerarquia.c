/**
 * Ejercicio 2: Jerarquía de procesos con fork
 * 
 * En este ejercicio implementarás un programa que crea una jerarquía
 * de procesos con forma de árbol.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Estructura de árbol: cada proceso puede tener 0, 1 o 2 hijos
#define NIVEL_MAXIMO 3

// Función para crear procesos hijos recursivamente
void crear_hijos(int nivel_actual, int max_nivel, int id_proceso) {
    if (nivel_actual >= max_nivel) {
        // Llegamos al nivel máximo, este proceso no tiene hijos
        printf("Proceso hoja: nivel=%d, id=%d, PID=%d\n", 
               nivel_actual, id_proceso, getpid());
        sleep(2);  // Simular trabajo
        exit(nivel_actual);  // Terminar con código de nivel
    }
    
    printf("Proceso interno: nivel=%d, id=%d, PID=%d\n", 
           nivel_actual, id_proceso, getpid());
    
    // TODO: Crear el primer hijo
    // 1. Usar fork()
    // 2. Si es el hijo, llamar recursivamente a crear_hijos incrementando nivel
    //    y actualizando el id_proceso
    
    // TODO: Crear el segundo hijo (similar al primero pero con otro id)
    
    // TODO: El padre debe esperar a que ambos hijos terminen
    // Usar wait o waitpid e imprimir información del estado de terminación
    
    exit(nivel_actual);  // El padre también termina con su nivel
}

int main() {
    printf("Creando jerarquía de procesos\n");
    
    // TODO: Iniciar la jerarquía llamando a crear_hijos
    // El proceso raíz tiene nivel 0 e id 0
    
    return 0;
}
