# Sistemas Operativos - Guías y Ejercicios

Este repositorio contiene material didáctico para el estudio de Sistemas Operativos con énfasis en sistemas UNIX/Linux, incluyendo tanto contenido teórico como ejercicios prácticos.

## Estructura del Repositorio

### 📚 [Teoría](./teoria/README.md)

- **[Programación Básica en C](./teoria/00-programacion-basica-c/README.md)** - Fundamentos de C y punteros
- **[Procesos](./teoria/01-procesos/README.md)** - Creación y gestión de procesos
- **[Memoria](./teoria/02-memoria/README.md)** - Gestión de memoria y memoria compartida
- **[Hilos](./teoria/03-hilos/README.md)** - Programación con threads y concurrencia
- **[Sincronización](./teoria/04-sincronizacion/README.md)** - Mecanismos de sincronización
- **[IPC](./teoria/05-ipc/README.md)** - Comunicación entre procesos
- **[Señales](./teoria/06-signal/README.md)** - Manejo de señales UNIX/Linux

### 💻 [Ejercicios Prácticos](./ejercicios_practica/README.md)

- **[Procesos](./ejercicios_practica/01-procesos/README.md)** - Ejercicios de fork, exec, etc.
- **[Memoria](./ejercicios_practica/02-memoria/README.md)** - Ejercicios de gestión de memoria
- **[Hilos](./ejercicios_practica/03-hilos/README.md)** - Ejercicios de threading
- **[Sincronización](./ejercicios_practica/04-sincronizacion/README.md)** - Mutex, semáforos, etc.
- **[IPC](./ejercicios_practica/05-ipc/README.md)** - Ejercicios de comunicación
- **[Señales](./ejercicios_practica/06-signal/README.md)** - Manejo de señales
- **[Ejercicios Combinados](./ejercicios_practica/ejercicios_combinados/README.md)** - Ejercicios avanzados

## Cómo usar este repositorio

1. Comienza con la sección de teoría para entender los conceptos fundamentales
2. Realiza los ejercicios prácticos de cada tema
3. Cada ejercicio incluye:
   - Archivo `.c` con código a completar
   - Archivo `.solucion.c` con la implementación
   - README con instrucciones específicas

## Compilación de ejercicios

### Programas básicos:

```bash
gcc -o programa programa.c
```

### Programas con hilos:

```bash
gcc -o programa programa.c -pthread
```

## Referencias y recursos adicionales

- "Advanced Programming in the UNIX Environment" por W. Richard Stevens
- "The Linux Programming Interface" por Michael Kerrisk
- Manual de referencia de POSIX (man pages)

## Requisitos previos

- Conocimientos básicos de programación en C
- Sistema operativo Linux o acceso a un entorno UNIX
- Compilador GCC instalado
- Conocimientos básicos de terminal y comandos UNIX
