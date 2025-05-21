# Procesos en Sistemas UNIX/Linux

Los procesos son programas en ejecución en un sistema operativo. Cada proceso tiene su propio espacio de memoria y recursos asignados por el sistema operativo.

## Conceptos Básicos

### ¿Qué es un proceso?

Un proceso es una instancia de un programa en ejecución. Incluye:

- **Código ejecutable**: Instrucciones del programa
- **Datos**: Variables y estructuras de datos
- **Stack**: Para llamadas a funciones y variables locales
- **Heap**: Memoria dinámica asignada en tiempo de ejecución
- **Contexto de ejecución**: Registros del procesador, contador de programa
- **Recursos del sistema**: Descriptores de archivo, sockets, etc.

### Ciclo de vida de un proceso

1. **Creación**: El proceso es creado (mediante `fork()` o similares)
2. **Listo**: El proceso está listo para ejecutarse cuando el planificador lo decida
3. **Ejecución**: El proceso está siendo ejecutado por la CPU
4. **Bloqueo**: El proceso espera por E/S u otro evento
5. **Terminación**: El proceso finaliza su ejecución

### Estados de un proceso

- **Nuevo**: Proceso recién creado
- **En ejecución**: Utilizando la CPU
- **Bloqueado** (o esperando): Esperando E/S u otro evento
- **Listo**: Preparado para ejecutar cuando se le asigne CPU
- **Terminado**: Proceso que ha completado su ejecución

## Headers necesarios

```c
#include <unistd.h>     // Para fork(), exec(), etc.
#include <sys/types.h>  // Para pid_t
#include <sys/wait.h>   // Para wait(), waitpid()
#include <stdlib.h>     // Para exit()
#include <stdio.h>      // Para printf(), etc.
```

## Funciones principales

### Creación y terminación de procesos

```c
// Crear un proceso hijo
pid_t fork(void);

// Reemplazar el proceso actual con otro programa
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *filename, char *const argv[], char *const envp[]);

// Esperar a que termine un proceso hijo
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

// Terminar un proceso
void exit(int status);
void _exit(int status);
```

### Identificación de procesos

```c
// Obtener el ID del proceso actual
pid_t getpid(void);

// Obtener el ID del proceso padre
pid_t getppid(void);
```

## Ejemplos de código

### Creación de procesos con fork()

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    // Crear un proceso hijo
    pid = fork();

    if (pid < 0) {
        // Error al crear el proceso
        fprintf(stderr, "Error en fork()\n");
        return 1;
    }
    else if (pid == 0) {
        // Código ejecutado por el proceso hijo
        printf("Soy el proceso hijo (PID: %d, PPID: %d)\n", getpid(), getppid());
    }
    else {
        // Código ejecutado por el proceso padre
        printf("Soy el proceso padre (PID: %d), hijo creado con PID: %d\n", getpid(), pid);
    }

    return 0;
}
```

### Ejecución de programas con exec()

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error en fork()\n");
        return 1;
    }
    else if (pid == 0) {
        // El proceso hijo ejecuta otro programa
        printf("El hijo va a ejecutar 'ls -l'\n");
        execlp("ls", "ls", "-l", NULL);

        // Este código solo se ejecuta si execlp falla
        fprintf(stderr, "Error en execlp()\n");
        return 1;
    }
    else {
        // El proceso padre espera a que el hijo termine
        wait(NULL);
        printf("El proceso hijo ha terminado\n");
    }

    return 0;
}
```

### Espera de procesos hijos

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error en fork()\n");
        return 1;
    }
    else if (pid == 0) {
        // El proceso hijo
        printf("Hijo: PID = %d\n", getpid());
        sleep(2);  // Simular trabajo
        exit(42);  // Terminar con código 42
    }
    else {
        // El proceso padre espera al hijo y obtiene su código de salida
        printf("Padre: esperando al hijo (PID = %d)\n", pid);
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Hijo terminó con código: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Hijo terminado por señal: %d\n", WTERMSIG(status));
        }
    }

    return 0;
}
```

## Patrones de uso

### Patrón Maestro-Esclavo

```c
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int num_workers = 3;

    for (int i = 0; i < num_workers; i++) {
        if ((pid = fork()) < 0) {
            perror("Error al crear proceso");
            exit(1);
        }

        if (pid == 0) {  // Proceso hijo (worker)
            printf("Worker %d: PID = %d\n", i, getpid());
            // Realizar trabajo específico del worker
            sleep(i + 1);  // Simular trabajo
            exit(i);       // Terminar con código = índice
        }
    }

    // El proceso padre (maestro) espera a que todos los trabajadores terminen
    int status;
    for (int i = 0; i < num_workers; i++) {
        pid_t terminated_pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("Worker %d (PID: %d) terminó con código: %d\n",
                   WEXITSTATUS(status), terminated_pid, WEXITSTATUS(status));
        }
    }

    printf("Todos los workers han terminado\n");
    return 0;
}
```

### Mini shell básico

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS + 1];  // +1 para NULL al final

    while (1) {
        // Mostrar prompt
        printf("minishell> ");
        fflush(stdout);

        // Leer comando
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) break;

        // Eliminar el carácter de nueva línea
        command[strlen(command) - 1] = '\0';

        // Salir si el comando es "exit"
        if (strcmp(command, "exit") == 0) break;

        // Parsear argumentos
        int argc = 0;
        char *token = strtok(command, " ");
        while (token != NULL && argc < MAX_ARGS) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;  // Terminar con NULL para execvp

        if (argc == 0) continue;  // Línea vacía

        // Crear proceso hijo para ejecutar el comando
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error en fork");
        }
        else if (pid == 0) {  // Proceso hijo
            if (execvp(args[0], args) < 0) {
                perror("Error en execvp");
                exit(1);
            }
        }
        else {  // Proceso padre
            wait(NULL);  // Esperar a que termine el hijo
        }
    }

    return 0;
}
```

## Problemas frecuentes

### Procesos zombies

Un proceso zombie es un proceso hijo que ha terminado su ejecución pero su entrada en la tabla de procesos no ha sido eliminada porque el padre no ha llamado a `wait()` o `waitpid()`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        exit(1);
    }
    else if (pid == 0) {  // Proceso hijo
        printf("Hijo: PID = %d, voy a terminar\n", getpid());
        exit(0);  // Hijo termina
    }
    else {  // Proceso padre
        printf("Padre: PID = %d, hijo PID = %d\n", getpid(), pid);
        sleep(30);  // El padre no llama a wait() durante 30 segundos
                   // El hijo se convierte en zombie durante este tiempo

        // Ejecutar "ps -l" en otra terminal para ver el proceso zombie
        printf("Padre: terminando\n");
    }

    return 0;
}
```

### Procesos huérfanos

Un proceso huérfano es un proceso cuyo proceso padre ha terminado. En sistemas UNIX, estos procesos son adoptados por el proceso init (PID 1).

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        exit(1);
    }
    else if (pid == 0) {  // Proceso hijo
        printf("Hijo: PID = %d, PPID = %d\n", getpid(), getppid());
        sleep(2);  // Dar tiempo al padre para terminar
        printf("Hijo después de 2s: PID = %d, PPID = %d\n", getpid(), getppid());
        sleep(5);  // Seguir ejecutando un tiempo
        printf("Hijo terminando\n");
    }
    else {  // Proceso padre
        printf("Padre: PID = %d, hijo PID = %d\n", getpid(), pid);
        sleep(1);  // Ejecutar brevemente
        printf("Padre terminando\n");
        exit(0);  // Padre termina, dejando al hijo huérfano
    }

    return 0;
}
```

## Consideraciones importantes

1. **Manejo de errores**: Siempre verifica el retorno de `fork()`, `exec()`, `wait()`, etc.
2. **Recursos compartidos**: Ten cuidado con descriptores de archivo y otros recursos que se duplican en el `fork()`.
3. **Procesos zombies**: Siempre utiliza `wait()` o `waitpid()` para recoger el estado de los procesos hijos.
4. **Sincronización**: La comunicación entre procesos requiere mecanismos como pipes, señales o memoria compartida.
5. **Eficiencia**: Los procesos tienen un mayor overhead que los hilos. Utiliza hilos para tareas que comparten mucha información.

## Ejercicios relacionados

### Ejercicios básicos

- [Ejercicio básico de fork](../../ejercicios_practica/01-procesos/ejercicio1_fork_basico.c)
- [Creación de jerarquía de procesos](../../ejercicios_practica/01-procesos/ejercicio2_fork_jerarquia.c)
- [Uso de exec para ejecutar comandos](../../ejercicios_practica/01-procesos/ejercicio3_exec_comandos.c)

### Ejemplos en el repositorio

- [Mini Shell](../../01-procesos/miniShell.c)
- [Master-Esclavo](../../01-procesos/master.c)
- [Proceso maestro-esclavo con exec](../../ejemplos/procesos/fork_exec_example.c)
- [Ejemplos básicos de fork](../../ejemplos/procesos/fork_examples.c)

### Ejercicios avanzados

- [Mini Shell compartida](../../01-procesos/minishellCompartida.c)

## Referencias adicionales

- Manual de las funciones (`man fork`, `man exec`, `man wait`)
- "Advanced Programming in the UNIX Environment" por W. Richard Stevens
- "The Linux Programming Interface" por Michael Kerrisk
