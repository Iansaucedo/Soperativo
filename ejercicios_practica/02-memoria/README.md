# Ejercicios de Memoria

Esta carpeta contiene ejercicios para practicar la gestión de memoria y memoria compartida en sistemas operativos UNIX/Linux.

## Lista de ejercicios

1. `ejercicio1_mmap_basico.c`: Uso básico de mmap para memoria compartida
2. `ejercicio2_archivo_compartido.c`: Compartir un archivo entre procesos con mmap
3. `ejercicio3_contador_compartido.c`: Implementar un contador compartido entre procesos
4. `ejercicio4_matriz_compartida.c`: Manipular una matriz en memoria compartida
5. `ejercicio5_seccion_critica.c`: Implementar secciones críticas con memoria compartida

## Instrucciones generales

- Cada archivo contiene código parcial con comentarios `TODO` que indican qué partes debes completar
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c`
- Sigue las indicaciones en cada archivo para completar los ejercicios

## Conceptos importantes a dominar

- Uso de `mmap()` para crear regiones de memoria compartida
- Sincronización entre procesos al acceder a memoria compartida
- Mapeo de archivos a memoria
- Secciones críticas y control de concurrencia
- Liberación adecuada de recursos con `munmap()`
