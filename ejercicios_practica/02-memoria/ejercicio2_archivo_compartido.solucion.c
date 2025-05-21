/**
 * Ejercicio 2: SOLUCIÓN - Archivo compartido con mmap
 * 
 * Esta solución implementa un programa que usa mmap para
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
#define INITIAL_DATA "Este es el contenido inicial del archivo.\nLínea 2 de datos de prueba.\nLínea 3 para demostrar mmap.\n"

int main() {
    int fd;
    struct stat sb;
    char *mapped_data;
    
    printf("Demostración de mapeo de archivo con mmap\n");
    
    // Crear/abrir un archivo para lectura y escritura
    fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error al abrir/crear el archivo");
        return 1;
    }
    
    // Verificar si el archivo está vacío
    if (fstat(fd, &sb) == -1) {
        perror("Error en fstat");
        close(fd);
        return 1;
    }
    
    // Si el archivo está vacío, escribir datos iniciales
    if (sb.st_size == 0) {
        printf("El archivo está vacío. Escribiendo datos iniciales...\n");
        if (write(fd, INITIAL_DATA, strlen(INITIAL_DATA)) == -1) {
            perror("Error al escribir datos iniciales");
            close(fd);
            return 1;
        }
    }
    
    // Obtener el tamaño actualizado del archivo
    if (fstat(fd, &sb) == -1) {
        perror("Error en fstat");
        close(fd);
        return 1;
    }
    
    // Verificar que el tamaño del archivo no sea demasiado grande
    if (sb.st_size > MAX_SIZE) {
        fprintf(stderr, "El archivo es demasiado grande para mapear\n");
        close(fd);
        return 1;
    }
    
    // Mapear el archivo a memoria
    mapped_data = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_data == MAP_FAILED) {
        perror("Error al mapear el archivo");
        close(fd);
        return 1;
    }
    
    printf("Archivo mapeado a memoria. Tamaño: %ld bytes\n", (long)sb.st_size);
    
    // Crear un proceso hijo
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Error en fork");
        munmap(mapped_data, sb.st_size);
        close(fd);
        return 1;
    } else if (pid == 0) {
        // Código del proceso hijo
        
        // Leer y mostrar el contenido actual del archivo
        printf("\n--- HIJO: Contenido actual del archivo ---\n");
        printf("%s", mapped_data);
        printf("----------------------------------------\n");
        
        // Modificar parte del contenido (por ejemplo, añadir una línea)
        char mensaje[] = "\n[MODIFICADO POR HIJO] Nueva línea agregada por el proceso hijo.\n";
        strcat(mapped_data, mensaje);
        
        // Asegurar que los cambios se guarden en el archivo
        msync(mapped_data, sb.st_size + strlen(mensaje), MS_SYNC);
        
        printf("HIJO: Contenido modificado\n");
        exit(0);
    } else {
        // Código del proceso padre
        
        // Esperar a que el hijo termine
        int status;
        waitpid(pid, &status, 0);
        
        // Leer y mostrar el contenido modificado
        printf("\n--- PADRE: Contenido modificado por el hijo ---\n");
        printf("%s", mapped_data);
        printf("------------------------------------------\n");
        
        // Actualizar una parte diferente del contenido
        char mensaje[] = "\n[MODIFICADO POR PADRE] Línea adicional añadida por el proceso padre.\n";
        strcat(mapped_data, mensaje);
        
        // Asegurar que los cambios se guarden en el archivo
        msync(mapped_data, sb.st_size + strlen(mensaje), MS_SYNC);
        
        printf("PADRE: Contenido modificado nuevamente\n");
        
        // Liberar el mapeo
        if (munmap(mapped_data, sb.st_size) == -1) {
            perror("Error al liberar el mapeo");
        }
        
        // Cerrar el archivo
        close(fd);
    }
    
    printf("Proceso completado. Verificar el archivo %s\n", FILENAME);
    return 0;
}
