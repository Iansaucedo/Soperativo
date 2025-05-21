/*
 * Ejercicio 3: Estructuras y Memoria
 * 
 * Objetivo: Practicar el uso de estructuras y gestión de memoria dinámica.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de la estructura Estudiante
typedef struct {
    char *nombre;
    int edad;
    float promedio;
} Estudiante;

// TODO: Implementar función para crear un nuevo estudiante
Estudiante* crearEstudiante(const char *nombre, int edad, float promedio) {
    // Tu código aquí
    return NULL;
}

// TODO: Implementar función para liberar la memoria de un estudiante
void liberarEstudiante(Estudiante *estudiante) {
    // Tu código aquí
}

// Función para imprimir los datos de un estudiante
void imprimirEstudiante(const Estudiante *estudiante) {
    if (estudiante == NULL || estudiante->nombre == NULL) {
        printf("Estudiante inválido\n");
        return;
    }
    printf("Nombre: %s\n", estudiante->nombre);
    printf("Edad: %d\n", estudiante->edad);
    printf("Promedio: %.2f\n", estudiante->promedio);
}

int main() {
    // Crear un estudiante
    Estudiante *est1 = crearEstudiante("Juan Pérez", 20, 8.5);
    
    // Imprimir sus datos
    printf("Datos del estudiante:\n");
    imprimirEstudiante(est1);
    
    // Liberar memoria
    liberarEstudiante(est1);
    
    return 0;
}
