/**
 * Ejercicio 1: SOLUCIÓN - Mutex Básico
 * 
 * Esta solución implementa un programa que usa mutex para
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

// Mutex para proteger el acceso al contador
pthread_mutex_t mutex_contador = PTHREAD_MUTEX_INITIALIZER;

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
        // Implementar el incremento seguro del contador usando el mutex
        
        // 1. Bloquear el mutex
        pthread_mutex_lock(&mutex_contador);
        
        // 2. Incrementar el contador (sección crítica)
        contador++;
        
        // 3. Desbloquear el mutex
        pthread_mutex_unlock(&mutex_contador);
        
        // Nota: Se podría usar usleep(1) aquí para simular más carga y
        // hacer más evidente el problema de las condiciones de carrera
    }
    
    printf("Hilo %d terminado\n", datos->id);
    return NULL;
}

int main() {
    pthread_t hilos[NUM_HILOS];
    DatosHilo datos[NUM_HILOS];
    
    printf("Programa de demostración de mutex\n");
    
    // El mutex ya fue inicializado estáticamente con PTHREAD_MUTEX_INITIALIZER
    // Si se quisiera inicializar dinámicamente:
    // pthread_mutex_init(&mutex_contador, NULL);
    
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
    
    // Destruir el mutex
    pthread_mutex_destroy(&mutex_contador);
    
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
