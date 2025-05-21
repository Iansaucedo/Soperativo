# Ejercicios de Sincronización

Esta carpeta contiene ejercicios para practicar los mecanismos de sincronización entre hilos y procesos en sistemas operativos UNIX/Linux.

## Lista de ejercicios

1. `ejercicio1_mutex_basico.c`: Protección de secciones críticas con mutex
2. `ejercicio2_semaforos_basicos.c`: Uso básico de semáforos
3. `ejercicio3_productor_consumidor.c`: Implementación del problema productor-consumidor
4. `ejercicio4_lectores_escritores.c`: Implementación del problema de lectores-escritores
5. `ejercicio5_filosofos.c`: Problema de los filósofos comensales

## Instrucciones generales

- Cada archivo contiene código parcial con comentarios `TODO` que indican qué partes debes completar
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c -pthread`
- Sigue las indicaciones en cada archivo para completar los ejercicios

## Conceptos importantes a dominar

- Mutex (`pthread_mutex_t`) para exclusión mutua
- Semáforos (`sem_t`) para sincronización y control
- Sección crítica y su protección
- Condiciones de carrera y cómo evitarlas
- Deadlocks y cómo prevenirlos
- Variables de condición (`pthread_cond_t`)
