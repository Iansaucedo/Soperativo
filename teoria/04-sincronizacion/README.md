# Sincronización en Sistemas Operativos

La sincronización en sistemas operativos se refiere a la coordinación de eventos para evitar problemas como condiciones de carrera, concurrencia peligrosa y acceso descontrolado a recursos compartidos entre procesos o hilos.

## Conceptos Básicos

### ¿Por qué es necesaria la sincronización?

Cuando múltiples hilos o procesos acceden a recursos compartidos, pueden ocurrir problemas como:

1. **Condiciones de carrera**: Ocurren cuando el resultado depende del orden específico en que se ejecutan operaciones concurrentes.
2. **Inconsistencia de datos**: Los datos pueden quedar en un estado inconsistente si múltiples hilos los modifican simultáneamente.
3. **Deadlocks (interbloqueos)**: Situación donde dos o más hilos se bloquean indefinidamente esperando recursos que están siendo usados por otros.
4. **Inanición (starvation)**: Un hilo nunca obtiene acceso a un recurso porque otros hilos lo acaparan constantemente.

### Sección crítica

Una sección crítica es un fragmento de código donde se accede a recursos compartidos y que no debe ser ejecutado por más de un hilo/proceso a la vez.

## Headers necesarios

```c
#include <pthread.h>     // Para mutex y variables de condición
#include <semaphore.h>   // Para semáforos
#include <fcntl.h>       // Para flags de semáforos nombrados
#include <sys/stat.h>    // Para permisos
```

## Mutex (Mutual Exclusion)

Los mutex son la herramienta más básica para sincronización, permitiendo acceso exclusivo a recursos compartidos.

### Creación y uso básico

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Inicialización estática

// O inicialización dinámica:
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);

// Entrar a la sección crítica
pthread_mutex_lock(&mutex);

// Sección crítica - solo un hilo a la vez
// accede a recursos compartidos aquí

// Salir de la sección crítica
pthread_mutex_unlock(&mutex);

// Destruir el mutex cuando ya no se necesita
pthread_mutex_destroy(&mutex);
```

### Intento no bloqueante

```c
// Intenta adquirir el mutex pero no bloquea si ya está tomado
if (pthread_mutex_trylock(&mutex) == 0) {
    // Sección crítica
    pthread_mutex_unlock(&mutex);
} else {
    // El mutex ya está tomado, hacer algo más
}
```

### Mutex con timeout

```c
// Estructura para tiempo absoluto
struct timespec timeout;
clock_gettime(CLOCK_REALTIME, &timeout);
timeout.tv_sec += 2;  // 2 segundos en el futuro

// Intenta adquirir el mutex con timeout
if (pthread_mutex_timedlock(&mutex, &timeout) == 0) {
    // Sección crítica
    pthread_mutex_unlock(&mutex);
} else {
    // Timeout ocurrió
}
```

## Semáforos

Los semáforos son contadores que pueden ser usados para controlar el acceso a recursos compartidos o sincronizar procesos/hilos.

### Semáforos sin nombre (entre hilos)

```c
sem_t semaforo;

// Inicializar semáforo con valor 1 (similar a un mutex)
sem_init(&semaforo, 0, 1);  // 0 significa para uso entre hilos

// Operación P (wait/decrementar)
sem_wait(&semaforo);

// Sección crítica

// Operación V (signal/incrementar)
sem_post(&semaforo);

// Destruir el semáforo
sem_destroy(&semaforo);
```

### Semáforos con nombre (entre procesos)

```c
sem_t *semaforo;

// Crear o abrir un semáforo con nombre
semaforo = sem_open("/mi_semaforo", O_CREAT, 0644, 1);

// Operación P (wait/decrementar)
sem_wait(semaforo);

// Sección crítica

// Operación V (signal/incrementar)
sem_post(semaforo);

// Cerrar el semáforo
sem_close(semaforo);

// Eliminar el semáforo del sistema (cuando ya no se necesita)
sem_unlink("/mi_semaforo");
```

### Operaciones no bloqueantes

```c
// Intento no bloqueante
if (sem_trywait(&semaforo) == 0) {
    // Semáforo adquirido
} else {
    // No se pudo adquirir, EAGAIN si ya estaba en 0
}

// Consultar el valor actual del semáforo
int valor;
sem_getvalue(&semaforo, &valor);
```

## Variables de condición

Las variables de condición permiten a los hilos esperar por una condición específica, liberando el mutex mientras esperan.

### Creación y uso básico

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicion = PTHREAD_COND_INITIALIZER;

// O inicialización dinámica:
pthread_cond_t condicion;
pthread_cond_init(&condicion, NULL);

// Hilo que espera por una condición
pthread_mutex_lock(&mutex);
while (!condicion_cumplida) {
    pthread_cond_wait(&condicion, &mutex);
}
// Procesar cuando la condición se cumple
pthread_mutex_unlock(&mutex);

// Hilo que señala que la condición se cumplió
pthread_mutex_lock(&mutex);
condicion_cumplida = true;
pthread_cond_signal(&condicion);  // Despierta un hilo
// o
pthread_cond_broadcast(&condicion);  // Despierta todos los hilos
pthread_mutex_unlock(&mutex);

// Destruir la variable de condición
pthread_cond_destroy(&condicion);
```

### Espera con timeout

```c
struct timespec timeout;
clock_gettime(CLOCK_REALTIME, &timeout);
timeout.tv_sec += 5;  // 5 segundos en el futuro

pthread_mutex_lock(&mutex);
while (!condicion_cumplida) {
    if (pthread_cond_timedwait(&condicion, &mutex, &timeout) == ETIMEDOUT) {
        // Timeout ocurrió
        break;
    }
}
pthread_mutex_unlock(&mutex);
```

## Patrones comunes de sincronización

### Exclusión mutua simple

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contador = 0;  // Variable compartida

void* incrementar(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);
        contador++;  // Sección crítica
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
```

### Patrón Productor-Consumidor

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lleno = PTHREAD_COND_INITIALIZER;
pthread_cond_t vacio = PTHREAD_COND_INITIALIZER;

#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int contador = 0;

void* productor(void* arg) {
    int item;

    while (1) {
        item = producir_item();  // Generar un nuevo item

        pthread_mutex_lock(&mutex);
        while (contador == BUFFER_SIZE) {
            pthread_cond_wait(&vacio, &mutex);  // Esperar espacio
        }

        buffer[contador++] = item;  // Añadir al buffer

        pthread_cond_signal(&lleno);  // Señalar que hay un item
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumidor(void* arg) {
    int item;

    while (1) {
        pthread_mutex_lock(&mutex);
        while (contador == 0) {
            pthread_cond_wait(&lleno, &mutex);  // Esperar items
        }

        item = buffer[--contador];  // Tomar del buffer

        pthread_cond_signal(&vacio);  // Señalar que hay espacio
        pthread_mutex_unlock(&mutex);

        consumir_item(item);  // Procesar el item
    }
    return NULL;
}
```

### Patrón Lectores-Escritores

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recurso = PTHREAD_MUTEX_INITIALIZER;
int lectores = 0;

void* lector(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        lectores++;
        if (lectores == 1) {
            pthread_mutex_lock(&recurso);  // Primer lector bloquea a escritores
        }
        pthread_mutex_unlock(&mutex);

        // Leer recurso compartido

        pthread_mutex_lock(&mutex);
        lectores--;
        if (lectores == 0) {
            pthread_mutex_unlock(&recurso);  // Último lector libera
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* escritor(void* arg) {
    while (1) {
        pthread_mutex_lock(&recurso);  // Acceso exclusivo

        // Modificar recurso compartido

        pthread_mutex_unlock(&recurso);
    }
    return NULL;
}
```

### Barrera de sincronización

```c
#define NUM_HILOS 4
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barrera = PTHREAD_COND_INITIALIZER;
int contador_barrera = 0;

void* funcion_hilo(void* arg) {
    // Fase 1 del trabajo

    pthread_mutex_lock(&mutex);
    contador_barrera++;
    if (contador_barrera == NUM_HILOS) {
        contador_barrera = 0;
        pthread_cond_broadcast(&barrera);
    } else {
        while (pthread_cond_wait(&barrera, &mutex) != 0);
    }
    pthread_mutex_unlock(&mutex);

    // Fase 2 del trabajo (todos los hilos sincronizados)

    return NULL;
}
```

## Problemas comunes de sincronización

### Deadlock (interbloqueo)

Ocurre cuando dos o más hilos se quedan bloqueados permanentemente, esperando por recursos que están siendo utilizados por otros hilos.

```c
// Hilo 1
pthread_mutex_lock(&mutex_A);
pthread_mutex_lock(&mutex_B);
// Código crítico
pthread_mutex_unlock(&mutex_B);
pthread_mutex_unlock(&mutex_A);

// Hilo 2 - Posible deadlock
pthread_mutex_lock(&mutex_B);
pthread_mutex_lock(&mutex_A);
// Código crítico
pthread_mutex_unlock(&mutex_A);
pthread_mutex_unlock(&mutex_B);
```

**Solución**: Siempre adquirir los mutex en el mismo orden en todos los hilos.

### Livelock

Similar al deadlock, pero los hilos cambian de estado constantemente sin progresar.

**Solución**: Implementar esperas aleatorias o con backoff exponencial.

### Race condition (condición de carrera)

Ocurre cuando el resultado depende del orden temporal específico en que se ejecutan instrucciones concurrentes.

```c
// Sin sincronización - posible condición de carrera
contador++;

// Con sincronización
pthread_mutex_lock(&mutex);
contador++;
pthread_mutex_unlock(&mutex);
```

### Inanición (starvation)

Un hilo nunca obtiene acceso a un recurso porque otros hilos lo monopolizan.

**Solución**: Implementar políticas justas, como FIFO.

## Consideraciones importantes

1. **Granularidad**: Mantener las secciones críticas lo más pequeñas posible.

2. **Orden de bloqueo**: Adquirir los mutex siempre en el mismo orden para evitar deadlocks.

3. **Liberación de recursos**: Asegurarse de liberar siempre los mutex incluso en caso de excepciones.

4. **Uso de trylock**: Considerar usar `pthread_mutex_trylock()` para evitar bloqueos en situaciones críticas.

5. **Rendimiento**: Demasiada sincronización puede degradar el rendimiento; buscar el equilibrio adecuado.

## Ejercicios relacionados

- **[Ejemplo de mutex](../../04-sincronizacion/ejemploMutex.c)**: Uso básico de mutex
- **[Patrón lectores-escritores](../../04-sincronizacion/leeryescribirSemaforo.c)**: Implementación del patrón lectores-escritores
- **[Sincronización básica](../../04-sincronizacion/semaforo.c)**: Uso de semáforos para sincronización

## Recursos adicionales

Para más información, consultar las páginas de manual:

- `man pthread_mutex_init`
- `man pthread_mutex_lock`
- `man sem_init`
- `man sem_wait`
- `man pthread_cond_init`
