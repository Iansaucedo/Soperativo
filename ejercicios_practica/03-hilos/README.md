# Ejercicios de Hilos (Threads)

Esta carpeta contiene ejercicios para practicar la creación y gestión de hilos en sistemas operativos UNIX/Linux.

## Lista de ejercicios

1. `ejercicio1_hilos_basicos.c`: Crear y gestionar hilos básicos
2. `ejercicio2_multiples_hilos.c`: Trabajar con múltiples hilos en paralelo
3. `ejercicio3_suma_paralela.c`: Implementar una suma de matriz en paralelo
4. `ejercicio4_productor_consumidor.c`: Implementar un problema productor-consumidor básico
5. `ejercicio5_calculo_pi.c`: Calcular π usando múltiples hilos

## Instrucciones generales

- Cada archivo contiene código parcial con comentarios `TODO` que indican qué partes debes completar
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c -pthread`
- La bandera `-pthread` es necesaria para enlazar la biblioteca de hilos

## Conceptos importantes a dominar

- Creación de hilos con `pthread_create()`
- Espera de hilos con `pthread_join()`
- Paso de argumentos a hilos
- Sincronización básica entre hilos
- Variables compartidas y cómo protegerlas
- Paralelismo vs. concurrencia
