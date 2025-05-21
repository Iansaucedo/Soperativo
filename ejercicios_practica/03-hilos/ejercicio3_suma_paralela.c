/**
 * Ejercicio 2: Suma paralela de matrices
 *
 * En este ejercicio implementarás un programa que usa múltiples hilos
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

// TODO: Implementar la función que ejecutará cada hilo
// Esta función debe:
// 1. Calcular qué filas le corresponden a este hilo
// 2. Sumar las matrices A y B para esas filas, guardando el resultado en C

void* sumar_matrices_parcial(void* arg) {
    // TU CÓDIGO AQUÍ
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
    // TODO: Declarar un array de identificadores de hilo
    // TODO: Declarar un array de estructuras para los parámetros de cada hilo
    
    printf("Programa de suma de matrices en paralelo\n");
    
    // Inicializar matrices
    inicializar_matrices();
    
    // Medir tiempo de inicio
    clock_t inicio = clock();
    
    // TODO: Crear NUM_HILOS hilos para sumar las matrices
    // Cada hilo debe procesar una parte de las matrices
    
    // TODO: Esperar a que todos los hilos terminen
    
    // Medir tiempo de fin
    clock_t fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
    
    printf("Suma completada en %.6f segundos\n", tiempo);
    
    // Verificar el resultado
    verificar_resultado();
    
    return 0;
}
