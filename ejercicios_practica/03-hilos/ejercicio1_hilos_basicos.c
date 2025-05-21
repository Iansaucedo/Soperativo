/**
 * Ejercicio 1: Hilos básicos
 * 
 * En este ejercicio implementarás un programa que crea y gestiona
 * hilos básicos para realizar tareas simples.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_HILOS 3

// TODO: Declara una estructura para pasar datos a los hilos
// La estructura debe contener:
// - Identificador del hilo (número)
// - Número de iteraciones
// - Tiempo de espera entre iteraciones (en segundos)

// Función que ejecutará cada hilo
void* funcion_hilo(void* arg) {
    // TODO: Obtener los datos del argumento
    
    // TODO: Implementar un bucle que:
    // 1. Imprima el identificador del hilo y la iteración actual
    // 2. Espere el tiempo especificado entre iteraciones
    // 3. Repita esto el número de veces indicado en la estructura
    
    // TODO: Imprimir un mensaje de finalización del hilo
    
    // TODO: Retornar un valor (usar pthread_exit)
    return NULL;
}

int main() {
    // TODO: Declarar un array de identificadores de hilo (pthread_t)
    
    // TODO: Declarar un array de estructuras para los parámetros de cada hilo
    
    printf("Programa de demostración de hilos básicos\n");
    
    // TODO: Inicializar las estructuras de datos para los hilos
    // Asignar diferentes valores para los tiempos y número de iteraciones
    
    // TODO: Crear NUM_HILOS hilos usando pthread_create
    // Pasar la estructura correspondiente a cada hilo
    
    printf("Todos los hilos han sido creados\n");
    
    // TODO: Esperar a que todos los hilos terminen usando pthread_join
    // Mostrar el valor retornado por cada hilo
    
    printf("Todos los hilos han terminado\n");
    return 0;
}
