# Ejercicios de Comunicación entre Procesos (IPC)

Esta carpeta contiene ejercicios para practicar la comunicación entre procesos (IPC) en sistemas operativos UNIX/Linux, con énfasis en tuberías (pipes).

## Lista de ejercicios

1. `ejercicio1_pipe_basico.c`: Comunicación básica entre padre e hijo con una tubería
2. `ejercicio2_pipe_bidireccional.c`: Comunicación bidireccional entre procesos
3. `ejercicio3_calculadora_pipes.c`: Implementar una calculadora distribuida usando tuberías
4. `ejercicio4_pipeline_comandos.c`: Simular un pipeline de comandos (similar a `|` en shell)
5. `ejercicio5_chat_simple.c`: Implementar un sistema de chat simple entre procesos

## Instrucciones generales

- Cada archivo contiene código parcial con comentarios `TODO` que indican qué partes debes completar
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c`
- Sigue las indicaciones en cada archivo para completar los ejercicios

## Conceptos importantes a dominar

- Creación y uso de tuberías con `pipe()`
- Redireccionamiento de descriptores de archivo
- Comunicación unidireccional vs. bidireccional
- Sincronización entre procesos mediante IPC
- Manejo de errores y cierres adecuados de tuberías
