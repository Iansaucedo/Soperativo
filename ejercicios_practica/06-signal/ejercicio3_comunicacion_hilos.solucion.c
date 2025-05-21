/**
 * Ejercicio 3: Comunicación entre hilos con señales (SOLUCIÓN)
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

// Manejador de señales para el productor
// El manejador activa la bandera que indica que el consumidor ha terminado
void manejador_productor(int sig) {
    if (sig == SIGUSR1) {
        consumidor_listo = 1;
    }
}

// Manejador de señales para el consumidor
// El manejador activa la bandera que indica que el productor ha generado un dato
void manejador_consumidor(int sig) {
    if (sig == SIGUSR2) {
        productor_listo = 1;
    }
}

// Función del hilo productor
// Esta función:
// 1. Configura el manejador de señales
// 2. Genera datos aleatorios (uno a la vez) y los almacena en el array global
// 3. Notifica al consumidor cada vez que produce un dato
// 4. Espera confirmación de procesamiento antes de producir el siguiente
void* funcion_productor(void* arg) {
    // Configurar el manejador de señales para SIGUSR1 (consumidor -> productor)
    struct sigaction sa;
    sa.sa_handler = manejador_productor;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    
    // Bucle de producción
    for (int i = 0; i < NUM_ELEMENTOS; i++) {
        // Generar dato aleatorio
        datos[dato_actual] = rand() % 100;
        
        // Imprimir información del dato generado
        printf("[Productor] Generado dato %d = %d\n", i, datos[dato_actual]);
        
        // Incrementar contadores
        dato_actual = (dato_actual + 1) % NUM_ELEMENTOS;
        datos_producidos++;
        
        // Notificar al consumidor que hay un dato disponible
        productor_listo = 0;
        pthread_kill(hilo_consumidor, SIGUSR2);
        
        // Esperar a que el consumidor procese el dato
        while (!consumidor_listo) {
            usleep(1000);  // Pequeña pausa para no consumir demasiada CPU
        }
    }
    
    return NULL;
}

// Función del hilo consumidor
// Esta función:
// 1. Configura el manejador de señales
// 2. Espera a que el productor genere un dato
// 3. Procesa el dato (calcula su cuadrado y lo muestra)
// 4. Notifica al productor que ha terminado el procesamiento
void* funcion_consumidor(void* arg) {
    // Configurar el manejador de señales para SIGUSR2 (productor -> consumidor)
    struct sigaction sa;
    sa.sa_handler = manejador_consumidor;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR2, &sa, NULL);
    
    int indice_procesamiento = 0;
    
    // Bucle de consumo
    for (int i = 0; i < NUM_ELEMENTOS; i++) {
        // Esperar a que el productor genere un dato
        while (!productor_listo) {
            usleep(1000);  // Pequeña pausa para no consumir demasiada CPU
        }
        
        // Procesar el dato (calcular su cuadrado)
        int valor = datos[indice_procesamiento];
        int cuadrado = valor * valor;
        
        // Mostrar resultado
        printf("[Consumidor] Procesado dato %d: %d^2 = %d\n", i, valor, cuadrado);
        
        // Incrementar contadores
        indice_procesamiento = (indice_procesamiento + 1) % NUM_ELEMENTOS;
        datos_procesados++;
        
        // Notificar al productor que se ha procesado el dato
        consumidor_listo = 1;
        pthread_kill(hilo_productor, SIGUSR1);
    }
    
    return NULL;
}

int main() {
    srand(time(NULL));
    
    printf("Programa de comunicación entre hilos usando señales\n");
    printf("Se producirán %d elementos y se procesarán uno a uno\n", NUM_ELEMENTOS);
    
    // Crear los hilos productor y consumidor
    pthread_create(&hilo_consumidor, NULL, funcion_consumidor, NULL);
    // Pequeña pausa para asegurar que el consumidor inicie primero
    usleep(10000);
    pthread_create(&hilo_productor, NULL, funcion_productor, NULL);
    
    // Esperar a que ambos hilos terminen
    pthread_join(hilo_productor, NULL);
    pthread_join(hilo_consumidor, NULL);
    
    printf("\nResumen final:\n");
    printf("Datos producidos: %d\n", datos_producidos);
    printf("Datos procesados: %d\n", datos_procesados);
    
    return 0;
}
