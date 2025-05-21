# Señales en Sistemas UNIX/Linux

Las señales son un mecanismo de comunicación entre procesos en sistemas operativos UNIX/Linux, que permiten notificar a un proceso la ocurrencia de un evento específico. Son una forma de comunicación asíncrona que permite a procesos o al kernel enviar notificaciones a otros procesos.

## Conceptos Básicos

### ¿Qué son las señales?

Las señales son interrupciones software que notifican a un proceso que ha ocurrido un evento. Cuando un proceso recibe una señal, puede:

- Ignorar la señal
- Ejecutar un manejador de señal personalizado
- Ejecutar el manejador por defecto (que suele terminar el proceso)

### Características principales

- **Asíncronas**: Las señales pueden ocurrir en cualquier momento durante la ejecución del programa
- **Limitadas en datos**: Solo transmiten el tipo de señal, no contienen datos adicionales
- **No son bufferizadas**: Si se envían múltiples señales del mismo tipo mientras se está procesando una, solo se garantiza la entrega de una

### Señales estándar comunes

| Señal   | Valor | Descripción                                        | Acción por defecto   |
| ------- | ----- | -------------------------------------------------- | -------------------- |
| SIGINT  | 2     | Interrupción desde teclado (Ctrl+C)                | Termina el proceso   |
| SIGQUIT | 3     | Señal de salida desde teclado (Ctrl+\\)            | Core dump + terminar |
| SIGKILL | 9     | Señal para matar un proceso (no se puede capturar) | Termina el proceso   |
| SIGSEGV | 11    | Violación de segmentación                          | Core dump + terminar |
| SIGTERM | 15    | Señal de terminación                               | Termina el proceso   |
| SIGUSR1 | 10    | Señal definida por usuario                         | Termina el proceso   |
| SIGUSR2 | 12    | Señal definida por usuario                         | Termina el proceso   |
| SIGCHLD | 17    | Cambio de estado en proceso hijo                   | Ignora               |
| SIGALRM | 14    | Señal de temporizador (alarm)                      | Termina el proceso   |

## Headers necesarios

```c
#include <signal.h>     // Para manejo de señales
#include <unistd.h>     // Para funciones como alarm(), pause()
#include <sys/types.h>  // Para tipos como pid_t
```

## Funciones principales

### Enviar señales

```c
// Envía una señal a un proceso específico
int kill(pid_t pid, int sig);

// Envía una señal a un hilo específico
int pthread_kill(pthread_t thread, int sig);

// Envía una señal al propio proceso
int raise(int sig);

// Programa una señal SIGALRM después de seconds segundos
unsigned int alarm(unsigned int seconds);
```

### Configurar manejadores de señales

#### Método simple

```c
// Configura un manejador para una señal específica
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

// Valores especiales para handler:
// SIG_DFL (manejador por defecto)
// SIG_IGN (ignorar la señal)
```

#### Método avanzado (recomendado)

```c
// Configura manejador de señales con más opciones
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

// Estructura de sigaction
struct sigaction {
    void     (*sa_handler)(int);
    void     (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t   sa_mask;
    int        sa_flags;
    void     (*sa_restorer)(void);
};
```

### Bloqueo de señales

```c
// Inicializa un conjunto de señales
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);

// Añade/elimina una señal del conjunto
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);

// Comprueba si una señal está en el conjunto
int sigismember(const sigset_t *set, int signum);

// Bloquea/desbloquea señales según el conjunto
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
// how puede ser: SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
```

## Patrones comunes de uso

### Manejador de señal básico

```c
void manejador_senales(int sig) {
    if (sig == SIGINT) {
        printf("Recibida señal SIGINT (Ctrl+C)\n");
        // Realizar limpieza
        exit(0);
    }
}

int main() {
    // Configurar manejador
    signal(SIGINT, manejador_senales);

    // Bucle principal
    while(1) {
        // Código del programa
        sleep(1);
    }

    return 0;
}
```

### Manejador avanzado con sigaction

```c
void manejador_avanzado(int sig, siginfo_t *info, void *ucontext) {
    printf("Recibida señal %d desde PID %d\n", sig, info->si_pid);
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = manejador_avanzado;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &sa, NULL);

    while(1) {
        pause(); // Esperar señales
    }

    return 0;
}
```

### Temporizador con señales

```c
volatile int contador = 0;

void manejador_alarma(int sig) {
    contador++;
    printf("Han pasado %d segundos\n", contador);
    alarm(1); // Programar la siguiente alarma
}

int main() {
    signal(SIGALRM, manejador_alarma);
    alarm(1); // Primera alarma tras 1 segundo

    while(contador < 10) {
        pause(); // Esperar señales
    }

    printf("Temporizador finalizado\n");
    return 0;
}
```

### Comunicación entre hilos con señales

```c
pthread_t hilo_id;
volatile int mensaje_listo = 0;

void manejador_senial(int sig) {
    mensaje_listo = 1;
}

void *funcion_hilo(void *arg) {
    signal(SIGUSR1, manejador_senial);

    while(1) {
        pause(); // Esperar señal
        if (mensaje_listo) {
            printf("Mensaje recibido\n");
            mensaje_listo = 0;
        }
    }

    return NULL;
}

int main() {
    pthread_create(&hilo_id, NULL, funcion_hilo, NULL);

    sleep(1); // Dar tiempo a que el hilo se inicie

    // Enviar señal al hilo
    pthread_kill(hilo_id, SIGUSR1);

    pthread_join(hilo_id, NULL);
    return 0;
}
```

## Consideraciones importantes

1. **Señales no manejables**: SIGKILL y SIGSTOP no pueden ser capturadas ni ignoradas.

2. **Reentrancia**: Los manejadores de señales deben ser funciones reentrantes, ya que pueden interrumpir cualquier parte del código.

3. **Funciones seguras en manejadores**: Dentro de un manejador de señales solo se deben llamar funciones asíncronas seguras (async-signal-safe).

4. **Variables volátiles**: Las variables compartidas entre el programa principal y los manejadores de señales deben ser declaradas como `volatile` para evitar optimizaciones del compilador.

5. **Bloqueo de señales**: Es recomendable bloquear señales durante secciones críticas del código.

6. **Señales perdidas**: No hay garantía de que todas las señales enviadas sean entregadas, especialmente si se envían múltiples señales del mismo tipo rápidamente.

## Recursos adicionales

Para más información, consultar las páginas de manual:

- `man 7 signal`
- `man 2 kill`
- `man 2 sigaction`
- `man 2 signal`
- `man 2 sigprocmask`
