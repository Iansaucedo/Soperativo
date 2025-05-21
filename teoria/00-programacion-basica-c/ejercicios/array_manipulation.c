/*
 * Ejercicio 1: Manipulación Básica de Arrays
 * 
 * Objetivo: Implementar funciones básicas para manipular arrays usando punteros.
 */

#include <stdio.h>
#include <stdlib.h>

// Función para imprimir un array
void printArray(const int *arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// TODO: Implementar función para invertir un array usando punteros
void reverseArray(int *arr, size_t size) {
    // Tu código aquí
}

// TODO: Implementar función para encontrar el máximo usando punteros
int findMax(const int *arr, size_t size) {
    // Tu código aquí
    return 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    printf("Array original: ");
    printArray(arr, size);

    // Prueba la función reverse
    reverseArray(arr, size);
    printf("Array invertido: ");
    printArray(arr, size);

    // Prueba la función findMax
    int max = findMax(arr, size);
    printf("Valor máximo: %d\n", max);

    return 0;
}
