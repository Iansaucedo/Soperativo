/**
 * Ejercicio 3: Comunicación entre hilos con señales
 *
 * En este ejercicio implementarás un sistema de comunicación entre hilos
 * utilizando señales para coordinar la ejecución.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#define NUM_ELEMENTOS 10

// Variables globales
int datos[NUM_ELEMENTOS];
int dato_actual = 0;
int datos_procesados = 0;
int datos_producidos = 0;

// Identificadores de hilos
pthread_t hilo_productor, hilo_consumidor;

// Variables de control
int productor_listo = 0;
int consumidor_listo = 0;

// TODO: Implementa los manejadores de señales para el productor
// El manejador debe activar la bandera que indica que el consumidor ha terminado

void manejador_productor(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementa los manejadores de señales para el consumidor
// El manejador debe activar la bandera que indica que el productor ha generado un dato

void manejador_consumidor(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementa la función del hilo productor
// Esta función debe:
// 1. Configurar el manejador de señales
// 2. Generar datos aleatorios (uno a la vez) y almacenarlos en el array global
// 3. Notificar al consumidor cada vez que produce un dato
// 4. Esperar confirmación de procesamiento antes de producir el siguiente

void* funcion_productor(void* arg) {
    // TU CÓDIGO AQUÍ
    return NULL;
}

// TODO: Implementa la función del hilo consumidor
// Esta función debe:
// 1. Configurar el manejador de señales
// 2. Esperar a que el productor genere un dato
// 3. Procesar el dato (calcular su cuadrado y mostrarlo)
// 4. Notificar al productor que ha terminado el procesamiento

void* funcion_consumidor(void* arg) {
    // TU CÓDIGO AQUÍ
    return NULL;
}

int main() {
    srand(time(NULL));
    
    printf("Programa de comunicación entre hilos usando señales\n");
    printf("Se producirán %d elementos y se procesarán uno a uno\n", NUM_ELEMENTOS);
    
    // TODO: Crear los hilos productor y consumidor
    // TU CÓDIGO AQUÍ
    
    // TODO: Esperar a que ambos hilos terminen
    // TU CÓDIGO AQUÍ
    
    printf("\nResumen final:\n");
    printf("Datos producidos: %d\n", datos_producidos);
    printf("Datos procesados: %d\n", datos_procesados);
    
    return 0;
}
