# Ejercicios de Procesos

Esta carpeta contiene ejercicios para practicar la creación y gestión de procesos en sistemas operativos UNIX/Linux.

## Lista de ejercicios

1. `ejercicio1_fork_basico.c`: Implementar un programa que cree procesos hijos básicos
2. `ejercicio2_fork_jerarquia.c`: Crear una jerarquía de procesos con fork
3. `ejercicio3_exec_comandos.c`: Implementar un programa que ejecute comandos externos
4. `ejercicio4_zombies.c`: Manejar procesos zombis y evitarlos
5. `ejercicio5_minishell.c`: Implementar un mini-shell con manejo de comandos

## Instrucciones generales

- Cada archivo contiene código parcial con comentarios `TODO` que indican qué partes debes completar
- Compila los programas con: `gcc -o nombre_ejecutable archivo.c`
- Sigue las indicaciones en cada archivo para completar los ejercicios

## Conceptos importantes a dominar

- Uso de `fork()` para crear procesos
- Uso de `exec()` para ejecutar programas diferentes
- Uso de `wait()` y `waitpid()` para esperar la terminación de procesos hijos
- Gestión de procesos zombis y huérfanos
- Comunicación básica entre procesos padre e hijo
