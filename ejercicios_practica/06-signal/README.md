# Ejercicios de Señales (Signals)

Esta carpeta contiene ejercicios para practicar el manejo de señales en sistemas operativos UNIX/Linux.

## Lista de ejercicios

1. `ejercicio1_manejador_basico.c`: Implementar un manejador básico de señales
2. `ejercicio2_multiples_senales.c`: Manejar múltiples señales con diferentes comportamientos
3. `ejercicio3_comunicacion_hilos.c`: Implementar comunicación entre hilos usando señales
4. `ejercicio4_temporizador_avanzado.c`: Crear un sistema de temporizadores con señales
5. `ejercicio5_control_procesos.c`: Gestionar procesos hijos mediante señales

## Instrucciones generales

- Cada archivo contiene código parcial con comentarios `TODO` que indican qué partes debes completar
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c -pthread`
- Para probar las señales en la terminal puedes usar:
  - `kill -SIGINT pid`: Envía la señal SIGINT (equivalente a Ctrl+C)
  - `kill -SIGUSR1 pid`: Envía la señal de usuario 1
  - `kill -SIGUSR2 pid`: Envía la señal de usuario 2

## Conceptos importantes a dominar

- Configuración de manejadores de señales con `signal()` y `sigaction()`
- Envío de señales con `kill()` y `pthread_kill()`
- Suspensión de ejecución con `pause()`
- Manejo seguro de señales
- Comunicación entre procesos e hilos mediante señales
