/**
 * Ejercicio 1: Mutex Básico
 * 
 * En este ejercicio implementarás un programa que usa mutex para
 * proteger una sección crítica con un contador compartido.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_HILOS 5
#define ITERACIONES 1000000

// Variables globales
long contador = 0;

// TODO: Declarar un mutex para proteger el acceso al contador

// Estructura para pasar datos a los hilos
typedef struct {
    int id;
    int incrementos;
} DatosHilo;

// Función que ejecutará cada hilo
void* incrementar_contador(void* arg) {
    DatosHilo* datos = (DatosHilo*)arg;
    
    printf("Hilo %d iniciado\n", datos->id);
    
    for (int i = 0; i < datos->incrementos; i++) {
        // TODO: Implementar el incremento seguro del contador usando el mutex
        // 1. Bloquear el mutex
        // 2. Incrementar el contador
        // 3. Desbloquear el mutex
        
        // Incremento sin protección (genera condiciones de carrera)
        contador++;
    }
    
    printf("Hilo %d terminado\n", datos->id);
    return NULL;
}

int main() {
    pthread_t hilos[NUM_HILOS];
    DatosHilo datos[NUM_HILOS];
    
    printf("Programa de demostración de mutex\n");
    
    // TODO: Inicializar el mutex
    
    // Crear varios hilos para incrementar el contador
    for (int i = 0; i < NUM_HILOS; i++) {
        datos[i].id = i;
        datos[i].incrementos = ITERACIONES;
        
        if (pthread_create(&hilos[i], NULL, incrementar_contador, &datos[i]) != 0) {
            perror("Error al crear hilo");
            exit(1);
        }
    }
    
    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // TODO: Destruir el mutex
    
    // Comprobar el resultado
    printf("El contador debería ser %ld\n", (long)NUM_HILOS * ITERACIONES);
    printf("El contador actual es %ld\n", contador);
    
    if (contador == (long)NUM_HILOS * ITERACIONES) {
        printf("¡CORRECTO! No hay condiciones de carrera.\n");
    } else {
        printf("¡ERROR! Hay condiciones de carrera.\n");
    }
    
    return 0;
}
