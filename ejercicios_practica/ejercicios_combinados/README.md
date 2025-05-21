# Ejercicios Combinados

Esta carpeta contiene ejercicios avanzados que combinan múltiples conceptos de sistemas operativos: procesos, memoria compartida, hilos, sincronización, IPC y señales.

## Lista de ejercicios

1. `ejercicio1_gestor_tareas.c`: Un gestor de tareas que utiliza procesos, hilos y señales
2. `ejercicio2_chat_multiusuario.c`: Un sistema de chat multiusuario que utiliza memoria compartida, semáforos y tuberías
3. `ejercicio3_banco_virtual.c`: Simulación de un banco con múltiples cajeros usando hilos, mutex y variables de condición
4. `ejercicio4_sistema_archivos.c`: Sistema de archivos distribuido con procesos, memoria compartida y señales

## Instrucciones generales

- Cada ejercicio combina varios conceptos vistos en las secciones anteriores
- Son ejercicios más complejos, diseñados para prepararte para el examen final
- Lee cuidadosamente los comentarios y los TODOs en cada archivo
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c -pthread`

## Beneficios de los ejercicios combinados

- Ponen a prueba tu comprensión integral de los sistemas operativos
- Te preparan para implementar soluciones a problemas del mundo real
- Desarrollan tu capacidad para combinar diferentes mecanismos de concurrencia
- Ayudan a identificar y resolver problemas complejos de sincronización
