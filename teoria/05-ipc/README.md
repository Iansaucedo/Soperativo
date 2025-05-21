# Comunicación Entre Procesos (IPC) en UNIX/Linux

La Comunicación Entre Procesos (IPC, Inter-Process Communication) es fundamental en sistemas operativos para permitir que los procesos compartan datos y coordinen acciones. En UNIX/Linux existen varios mecanismos para implementar IPC.

## Conceptos Básicos

### ¿Qué es IPC?

IPC es un conjunto de mecanismos que permite a los procesos:

- Intercambiar datos
- Compartir información
- Sincronizar sus operaciones
- Coordinar actividades

### Importancia de IPC

- Facilita la modularidad y el diseño de programas
- Permite construir sistemas complejos a partir de componentes más simples
- Posibilita la comunicación entre procesos que no comparten memoria

### Tipos de IPC en UNIX/Linux

1. **Tuberías (Pipes)**
2. **Tuberías con nombre (Named Pipes o FIFOs)**
3. **Colas de mensajes (Message Queues)**
4. **Semáforos**
5. **Memoria compartida**
6. **Sockets**

## Headers necesarios

```c
#include <unistd.h>     // Para pipes
#include <sys/types.h>  // Tipos básicos
#include <sys/stat.h>   // Para mkfifo
#include <fcntl.h>      // Constantes de control de archivos
#include <sys/ipc.h>    // Para IPC de System V
#include <sys/msg.h>    // Para colas de mensajes
#include <sys/shm.h>    // Para memoria compartida
#include <sys/sem.h>    // Para semáforos
```

## Tuberías (Pipes)

Las tuberías son el mecanismo de IPC más simple, que permite comunicación unidireccional entre procesos relacionados (típicamente padre-hijo).

### Creación y uso básico

```c
int fd[2];  // fd[0] para lectura, fd[1] para escritura

// Crear la tubería
if (pipe(fd) == -1) {
    perror("Error al crear la tubería");
    exit(EXIT_FAILURE);
}

pid_t pid = fork();

if (pid == 0) {  // Proceso hijo
    close(fd[1]);  // Cerrar extremo de escritura

    char buffer[100];
    read(fd[0], buffer, sizeof(buffer));
    printf("Hijo recibió: %s\n", buffer);

    close(fd[0]);
    exit(EXIT_SUCCESS);
} else {  // Proceso padre
    close(fd[0]);  // Cerrar extremo de lectura

    write(fd[1], "Mensaje para el hijo", 20);

    close(fd[1]);
    wait(NULL);  // Esperar al hijo
}
```

### Comunicación bidireccional con dos tuberías

```c
int pipe_padre_hijo[2];  // Padre → Hijo
int pipe_hijo_padre[2];  // Hijo → Padre

pipe(pipe_padre_hijo);
pipe(pipe_hijo_padre);

if (fork() == 0) {  // Proceso hijo
    // Configurar tubería para recibir del padre
    close(pipe_padre_hijo[1]);
    // Configurar tubería para enviar al padre
    close(pipe_hijo_padre[0]);

    char msg[100];
    read(pipe_padre_hijo[0], msg, sizeof(msg));
    printf("Hijo recibió: %s\n", msg);

    write(pipe_hijo_padre[1], "Respuesta del hijo", 18);

    close(pipe_padre_hijo[0]);
    close(pipe_hijo_padre[1]);
    exit(0);
} else {  // Proceso padre
    // Configurar tubería para enviar al hijo
    close(pipe_padre_hijo[0]);
    // Configurar tubería para recibir del hijo
    close(pipe_hijo_padre[1]);

    write(pipe_padre_hijo[1], "Mensaje del padre", 17);

    char respuesta[100];
    read(pipe_hijo_padre[0], respuesta, sizeof(respuesta));
    printf("Padre recibió: %s\n", respuesta);

    close(pipe_padre_hijo[1]);
    close(pipe_hijo_padre[0]);
    wait(NULL);
}
```

### Envío de datos estructurados

```c
typedef struct {
    int numero;
    char texto[50];
} Mensaje;

// En el emisor:
Mensaje msg = {42, "Ejemplo de mensaje estructurado"};
write(fd[1], &msg, sizeof(Mensaje));

// En el receptor:
Mensaje msg_recibido;
read(fd[0], &msg_recibido, sizeof(Mensaje));
```

## Tuberías con nombre (Named Pipes o FIFOs)

Permiten comunicación entre procesos no relacionados.

### Creación y uso

```c
// Crear una tubería con nombre
if (mkfifo("/tmp/mi_fifo", 0666) == -1) {
    if (errno != EEXIST) {
        perror("Error al crear FIFO");
        exit(EXIT_FAILURE);
    }
}

// Escritor
int fd = open("/tmp/mi_fifo", O_WRONLY);
write(fd, "Mensaje a través de FIFO", 24);
close(fd);

// Lector
int fd = open("/tmp/mi_fifo", O_RDONLY);
char buffer[100];
read(fd, buffer, sizeof(buffer));
close(fd);
```

## Colas de mensajes

Permiten enviar mensajes estructurados entre procesos.

### Creación y uso básico

```c
// Estructura para mensajes
struct msgbuf {
    long mtype;     // Tipo de mensaje (debe ser > 0)
    char mtext[100]; // Contenido del mensaje
};

// Crear o acceder a una cola de mensajes
key_t key = ftok("/tmp/unique", 65);
int msgid = msgget(key, 0666 | IPC_CREAT);

// Enviar mensaje
struct msgbuf mensaje = {1, "Mensaje de ejemplo"};
msgsnd(msgid, &mensaje, sizeof(mensaje.mtext), 0);

// Recibir mensaje
struct msgbuf recibido;
msgrcv(msgid, &recibido, sizeof(recibido.mtext), 1, 0);

// Eliminar cola de mensajes
msgctl(msgid, IPC_RMID, NULL);
```

## Memoria compartida

Permite a múltiples procesos acceder a una región de memoria común.

### Creación y uso básico

```c
// Crear o acceder a un segmento de memoria compartida
key_t key = ftok("/tmp/shared", 65);
int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

// Adjuntar el segmento a nuestro espacio de direcciones
char *data = (char *)shmat(shmid, NULL, 0);

// Escribir datos
sprintf(data, "Datos en memoria compartida");

// En otro proceso, después de adjuntar la memoria:
printf("Leído: %s\n", data);

// Separar el segmento
shmdt(data);

// Eliminar el segmento
shmctl(shmid, IPC_RMID, NULL);
```

## Semáforos

Se utilizan para sincronización entre procesos.

### Uso básico

```c
// Crear o acceder a un conjunto de semáforos
key_t key = ftok("/tmp/sem", 65);
int semid = semget(key, 1, 0666 | IPC_CREAT);

// Inicializar el semáforo (valor 1 = disponible)
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} arg;
arg.val = 1;
semctl(semid, 0, SETVAL, arg);

// Operación P (wait/lock)
struct sembuf sb = {0, -1, 0};  // Decrementar en 1
semop(semid, &sb, 1);

// Sección crítica
printf("Acceso exclusivo a recursos\n");

// Operación V (signal/unlock)
sb.sem_op = 1;  // Incrementar en 1
semop(semid, &sb, 1);

// Eliminar el semáforo
semctl(semid, 0, IPC_RMID);
```

## Patrones comunes de uso

### Cliente-Servidor con tuberías con nombre

```c
// Servidor
int fd = open("/tmp/server_pipe", O_RDONLY);
char request[100];
read(fd, request, sizeof(request));
// Procesar solicitud
close(fd);

// Cliente
int fd = open("/tmp/server_pipe", O_WRONLY);
write(fd, "REQUEST", 7);
close(fd);
```

### Productor-Consumidor con memoria compartida y semáforos

```c
// Productor
wait(sem_vacio);
wait(sem_mutex);
// Producir dato en memoria compartida
signal(sem_mutex);
signal(sem_lleno);

// Consumidor
wait(sem_lleno);
wait(sem_mutex);
// Consumir dato de memoria compartida
signal(sem_mutex);
signal(sem_vacio);
```

## Consideraciones importantes

1. **Cierre correcto de descriptores**: Es crucial cerrar los descriptores que no se utilizan para evitar bloqueos y fugas de recursos.

2. **Limpieza de recursos**: Los recursos de IPC deben liberarse explícitamente (colas de mensajes, segmentos de memoria compartida, semáforos).

3. **Sincronización**: Al usar memoria compartida, es necesario sincronizar el acceso mediante semáforos o mutex.

4. **Condiciones de carrera**: Deben evitarse usando mecanismos de sincronización adecuados.

5. **Bloqueos y esperas**: Hay que tener cuidado con operaciones que puedan bloquear indefinidamente un proceso.

## Recursos adicionales

Para más información, consultar las páginas de manual:

- `man 2 pipe`
- `man 3 mkfifo`
- `man 2 msgget`
- `man 2 shmget`
- `man 2 semget`
