/**
 * Ejercicio 3: SOLUCIÓN - Suma paralela de matrices
 *
 * Esta solución implementa un programa que usa múltiples hilos
 * para sumar dos matrices.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 100  // Tamaño de las matrices
#define NUM_HILOS 4  // Número de hilos a utilizar

// Matrices globales
int A[N][N];
int B[N][N];
int C[N][N];  // Resultado

// Estructura para pasar datos a los hilos
typedef struct {
    int id_hilo;
    int filas_por_hilo;
} DatosHilo;

// Inicializar matrices con valores aleatorios
void inicializar_matrices() {
    int i, j;
    srand(time(NULL));
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }
}

// Función para imprimir una matriz (solo para matrices pequeñas)
void imprimir_matriz(int matriz[N][N]) {
    if (N > 10) {
        printf("Matriz demasiado grande para imprimir\n");
        return;
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%3d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Función que ejecutará cada hilo
void* sumar_matrices_parcial(void* arg) {
    DatosHilo* datos = (DatosHilo*)arg;
    
    // Calcular las filas que corresponden a este hilo
    int filas_por_hilo = datos->filas_por_hilo;
    int inicio = datos->id_hilo * filas_por_hilo;
    int fin = (datos->id_hilo == NUM_HILOS - 1) ? N : inicio + filas_por_hilo;
    
    printf("Hilo %d: Sumando filas %d a %d\n", datos->id_hilo, inicio, fin - 1);
    
    // Sumar las matrices para las filas asignadas
    for (int i = inicio; i < fin; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    
    printf("Hilo %d: Suma completada\n", datos->id_hilo);
    return NULL;
}

// Función para verificar que la suma es correcta
void verificar_resultado() {
    int i, j;
    int errores = 0;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (C[i][j] != A[i][j] + B[i][j]) {
                errores++;
                break;
            }
        }
        if (errores > 0) break;
    }
    
    if (errores == 0) {
        printf("La suma se realizó correctamente\n");
    } else {
        printf("Se encontraron errores en la suma\n");
    }
}

int main() {
    // Declarar un array de identificadores de hilo
    pthread_t hilos[NUM_HILOS];
    
    // Declarar un array de estructuras para los parámetros de cada hilo
    DatosHilo datos_hilos[NUM_HILOS];
    
    printf("Programa de suma de matrices en paralelo\n");
    
    // Inicializar matrices
    inicializar_matrices();
    
    // Calcular el número de filas por hilo
    int filas_por_hilo = N / NUM_HILOS;
    printf("Tamaño de matriz: %d x %d\n", N, N);
    printf("Número de hilos: %d\n", NUM_HILOS);
    printf("Filas por hilo: %d\n", filas_por_hilo);
    
    // Medir tiempo de inicio
    clock_t inicio = clock();
    
    // Crear hilos para sumar las matrices
    for (int i = 0; i < NUM_HILOS; i++) {
        datos_hilos[i].id_hilo = i;
        datos_hilos[i].filas_por_hilo = filas_por_hilo;
        
        if (pthread_create(&hilos[i], NULL, sumar_matrices_parcial, 
                          &datos_hilos[i]) != 0) {
            perror("Error al crear hilo");
            return 1;
        }
    }
    
    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // Medir tiempo de fin
    clock_t fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
    
    printf("Suma completada en %.6f segundos\n", tiempo);
    
    // Verificar el resultado
    verificar_resultado();
    
    return 0;
}
