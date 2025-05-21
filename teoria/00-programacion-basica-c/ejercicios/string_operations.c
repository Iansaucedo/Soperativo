/*
 * Ejercicio 2: Strings y Caracteres
 * 
 * Objetivo: Practicar operaciones básicas con strings y caracteres.
 */

#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 100

// TODO: Implementar función para contar vocales en un string
int countVowels(const char *str) {
    // Tu código aquí
    return 0;
}

// TODO: Implementar función para convertir string a mayúsculas sin usar toupper()
void toUpperCase(char *str) {
    // Tu código aquí
}

// TODO: Implementar función para invertir un string sin usar strrev()
void reverseString(char *str) {
    // Tu código aquí
}

int main() {
    char texto[MAX_LENGTH];
    printf("Ingrese un texto: ");
    fgets(texto, MAX_LENGTH, stdin);
    
    // Eliminar el \n que fgets agrega al final
    texto[strcspn(texto, "\n")] = 0;

    printf("Texto original: %s\n", texto);
    
    // Contar vocales
    int vowels = countVowels(texto);
    printf("Número de vocales: %d\n", vowels);
    
    // Convertir a mayúsculas
    toUpperCase(texto);
    printf("En mayúsculas: %s\n", texto);
    
    // Invertir string
    reverseString(texto);
    printf("Texto invertido: %s\n", texto);

    return 0;
}
