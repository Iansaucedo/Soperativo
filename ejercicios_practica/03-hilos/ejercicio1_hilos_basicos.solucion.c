/**
 * Ejercicio 1: SOLUCIÓN - Hilos básicos
 * 
 * Esta solución implementa un programa que crea y gestiona
 * hilos básicos para realizar tareas simples.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_HILOS 3

// Estructura para pasar datos a los hilos
typedef struct {
    int id;                 // Identificador del hilo
    int iteraciones;        // Número de iteraciones
    int tiempo_espera;      // Tiempo de espera entre iteraciones (en segundos)
} DatosHilo;

// Función que ejecutará cada hilo
void* funcion_hilo(void* arg) {
    // Obtener los datos del argumento
    DatosHilo* datos = (DatosHilo*)arg;
    
    printf("Hilo %d: Iniciando con %d iteraciones y %d segundos de espera\n", 
           datos->id, datos->iteraciones, datos->tiempo_espera);
    
    // Bucle para realizar las iteraciones
    for (int i = 1; i <= datos->iteraciones; i++) {
        printf("Hilo %d: Iteración %d de %d\n", 
               datos->id, i, datos->iteraciones);
        
        // Esperar el tiempo especificado
        sleep(datos->tiempo_espera);
    }
    
    // Preparar valor de retorno (usar el heap para que no se pierda al salir)
    int* resultado = malloc(sizeof(int));
    *resultado = datos->id * 10 + datos->iteraciones;
    
    printf("Hilo %d: Finalizando. Valor de retorno: %d\n", 
           datos->id, *resultado);
    
    // Retornar el valor (usando pthread_exit para mayor claridad)
    pthread_exit(resultado);
}

int main() {
    // Declarar array de identificadores de hilo
    pthread_t hilos[NUM_HILOS];
    
    // Declarar array de estructuras para los parámetros de cada hilo
    DatosHilo datos_hilos[NUM_HILOS];
    
    printf("Programa de demostración de hilos básicos\n");
    
    // Inicializar las estructuras de datos para los hilos
    datos_hilos[0].id = 1;
    datos_hilos[0].iteraciones = 3;
    datos_hilos[0].tiempo_espera = 1;
    
    datos_hilos[1].id = 2;
    datos_hilos[1].iteraciones = 5;
    datos_hilos[1].tiempo_espera = 2;
    
    datos_hilos[2].id = 3;
    datos_hilos[2].iteraciones = 2;
    datos_hilos[2].tiempo_espera = 3;
    
    // Crear NUM_HILOS hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        if (pthread_create(&hilos[i], NULL, funcion_hilo, &datos_hilos[i]) != 0) {
            perror("Error al crear hilo");
            return 1;
        }
        printf("Hilo %d creado\n", datos_hilos[i].id);
    }
    
    printf("Todos los hilos han sido creados\n");
    
    // Esperar a que todos los hilos terminen
    void* retval;
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], &retval);
        int valor_retorno = *(int*)retval;
        printf("Hilo %d ha terminado. Valor retornado: %d\n", 
               datos_hilos[i].id, valor_retorno);
        free(retval);  // Liberar la memoria asignada en el hilo
    }
    
    printf("Todos los hilos han terminado\n");
    return 0;
}
