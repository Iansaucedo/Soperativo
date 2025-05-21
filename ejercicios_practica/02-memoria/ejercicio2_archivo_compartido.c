/**
 * Ejercicio 2: Archivo compartido con mmap
 * 
 * En este ejercicio implementarás un programa que usa mmap para
 * mapear un archivo a memoria y compartirlo entre procesos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define FILENAME "datos.txt"
#define MAX_SIZE 4096

int main() {
    int fd;
    struct stat sb;
    char *mapped_data;
    
    printf("Demostración de mapeo de archivo con mmap\n");
    
    // TODO: Crear/abrir un archivo para lectura y escritura
    // Si el archivo no existe, crearlo
    // Usar open() con los flags adecuados
    
    // TODO: Obtener el tamaño del archivo
    // Si el archivo está vacío, escribir datos iniciales en él
    // Puedes usar write() para escribir unos datos de prueba
    
    // TODO: Obtener el tamaño actualizado del archivo con fstat
    
    // TODO: Mapear el archivo a memoria usando mmap
    // El mapeo debe permitir lectura y escritura
    
    // TODO: Crear un proceso hijo con fork
    
    // TODO: En el proceso hijo:
    // 1. Leer e imprimir el contenido actual del archivo desde la memoria
    // 2. Modificar parte del contenido
    // 3. Hacer msync para guardar los cambios
    // 4. Salir con código 0
    
    // TODO: En el proceso padre:
    // 1. Esperar a que el hijo termine
    // 2. Leer e imprimir el contenido modificado desde la memoria
    // 3. Actualizar una parte diferente del contenido
    // 4. Hacer msync para guardar los cambios
    // 5. Liberar el mapeo con munmap
    // 6. Cerrar el archivo
    
    printf("Proceso completado. Verificar el archivo %s\n", FILENAME);
    return 0;
}
