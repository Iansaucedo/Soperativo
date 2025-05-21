# Ejercicios para práctica de Sistemas Operativos

Este directorio contiene ejercicios para practicar los diferentes conceptos de Sistemas Operativos vistos en clase.
Cada carpeta contiene ejercicios relacionados con un tema específico.

## Estructura

- `01-procesos`: Ejercicios sobre creación y gestión de procesos
- `02-memoria`: Ejercicios sobre gestión de memoria y memoria compartida
- `03-hilos`: Ejercicios sobre hilos (threads) y paralelismo
- `04-sincronizacion`: Ejercicios sobre semáforos y mutex
- `05-ipc`: Ejercicios sobre comunicación entre procesos (tuberías)
- `06-signal`: Ejercicios sobre manejo de señales

## Cómo usar estos ejercicios

1. Revisa el contenido de cada carpeta según el tema que deseas practicar
2. Cada ejercicio incluye:

   - Un archivo `.c` con el código a completar (contiene comentarios con instrucciones)
   - Un archivo de solución (con extensión `.solucion.c`)
   - Un archivo README con instrucciones específicas

3. Intenta resolver los ejercicios por tu cuenta antes de ver las soluciones
4. Compila los programas usando:

   ```
   gcc -o nombre_ejecutable archivo.c -pthread
   ```

5. Ejecuta los programas y observa su comportamiento

## Recomendaciones para el examen final

- Estudia todos los conceptos de cada tema
- Practica la implementación de los ejemplos básicos de cada concepto
- Asegúrate de entender las diferencias entre procesos e hilos
- Domina los mecanismos de comunicación y sincronización
- Practica la combinación de múltiples conceptos en un mismo programa
