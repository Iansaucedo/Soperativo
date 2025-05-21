# Hilos (Threads) en Sistemas UNIX/Linux

Los hilos (threads) son subunidades de ejecución dentro de un proceso que comparten la misma memoria y recursos. Son fundamentales para la programación concurrente y paralela, permitiendo que un programa ejecute múltiples tareas simultáneamente.

## Conceptos Básicos

### ¿Qué son los hilos?

Un hilo es una secuencia de instrucciones dentro de un programa que puede ser ejecutada independientemente. Los hilos de un mismo proceso comparten:

- Espacio de memoria (código, datos, heap)
- Descriptores de archivos
- Variables globales
- Señales

Pero tienen su propio:

- Contador de programa
- Registros
- Pila (stack)
- Estado

### Ventajas de los hilos sobre procesos

1. **Menor overhead**: Crear y cambiar entre hilos consume menos recursos que entre procesos
2. **Comunicación simplificada**: Los hilos comparten variables y memoria, facilitando la comunicación
3. **Paralelismo eficiente**: Aprovechan mejor los sistemas multiprocesador o multinúcleo
4. **Mayor rendimiento**: Reducen la latencia de tareas concurrentes

### Desventajas de los hilos

1. **Sincronización requerida**: El acceso a recursos compartidos debe ser sincronizado
2. **Mayor complejidad**: Más difíciles de programar y depurar
3. **Fallos compartidos**: Un error en un hilo puede afectar a todo el proceso

## Headers necesarios

```c
#include <pthread.h>  // API POSIX de hilos
#include <unistd.h>   // Para sleep(), etc.
#include <stdlib.h>   // Para malloc(), etc.
#include <stdio.h>    // Para printf(), etc.
```

## Funciones principales

### Creación y terminación de hilos

```c
// Crear un hilo
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                  void *(*start_routine) (void *), void *arg);

// Esperar a que un hilo termine
int pthread_join(pthread_t thread, void **retval);

// Separar un hilo (no se puede hacer join)
int pthread_detach(pthread_t thread);

// Terminar un hilo
void pthread_exit(void *retval);

// Cancelar un hilo
int pthread_cancel(pthread_t thread);
```

### Atributos de hilos

```c
// Inicializar atributos con valores por defecto
int pthread_attr_init(pthread_attr_t *attr);

// Destruir objeto de atributos
int pthread_attr_destroy(pthread_attr_t *attr);

// Configurar estado de separación (joinable/detached)
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
// donde detachstate puede ser PTHREAD_CREATE_JOINABLE o PTHREAD_CREATE_DETACHED

// Configurar tamaño de pila
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
```

### Identificadores de hilos

```c
// Obtener ID del hilo actual
pthread_t pthread_self(void);

// Comparar dos IDs de hilo
int pthread_equal(pthread_t t1, pthread_t t2);
```

### Cancelación de hilos

```c
// Configurar estado de cancelación (on/off)
int pthread_setcancelstate(int state, int *oldstate);
// donde state puede ser PTHREAD_CANCEL_ENABLE o PTHREAD_CANCEL_DISABLE

// Configurar tipo de cancelación
int pthread_setcanceltype(int type, int *oldtype);
// donde type puede ser PTHREAD_CANCEL_DEFERRED o PTHREAD_CANCEL_ASYNCHRONOUS

// Crear un punto de cancelación
void pthread_testcancel(void);
```

## Ejemplo básico de uso de hilos

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Función que ejecutará el hilo
void *funcion_hilo(void *arg) {
    int id = *((int*)arg);
    printf("Hilo %d: ejecutando\n", id);

    // Hacer algo en el hilo...

    printf("Hilo %d: terminando\n", id);
    pthread_exit(NULL);
}

int main() {
    pthread_t hilo1, hilo2;
    int id1 = 1, id2 = 2;

    // Crear dos hilos
    pthread_create(&hilo1, NULL, funcion_hilo, &id1);
    pthread_create(&hilo2, NULL, funcion_hilo, &id2);

    // Esperar a que los hilos terminen
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Hilos terminados. Finalizando programa.\n");
    return 0;
}
```

## Patrones comunes de uso

### Hilos con retorno de valor

```c
void *calcular_suma(void *arg) {
    int *array = (int*)arg;
    int suma = 0;

    for (int i = 0; i < 5; i++) {
        suma += array[i];
    }

    // Asignar memoria para el resultado
    int *resultado = malloc(sizeof(int));
    *resultado = suma;

    return (void*)resultado;
}

int main() {
    pthread_t hilo;
    int numeros[5] = {1, 2, 3, 4, 5};
    void *resultado;

    pthread_create(&hilo, NULL, calcular_suma, numeros);

    // Recoger resultado
    pthread_join(hilo, &resultado);
    printf("La suma es: %d\n", *((int*)resultado));

    // Liberar memoria asignada en el hilo
    free(resultado);
    return 0;
}
```

### Hilos con argumentos múltiples

```c
struct Args {
    int a;
    int b;
    char *mensaje;
};

void *funcion_hilo(void *arguments) {
    struct Args *args = (struct Args*)arguments;
    printf("Mensaje: %s, a=%d, b=%d\n", args->mensaje, args->a, args->b);
    return NULL;
}

int main() {
    pthread_t hilo;
    struct Args args = {5, 10, "Hola desde el hilo"};

    pthread_create(&hilo, NULL, funcion_hilo, &args);
    pthread_join(hilo, NULL);

    return 0;
}
```

### Hilos separados (detached)

```c
void *hilo_background(void *arg) {
    // Este hilo se ejecuta de forma independiente
    pthread_detach(pthread_self());

    while (1) {
        printf("Hilo en segundo plano trabajando...\n");
        sleep(2);
    }

    return NULL;
}

int main() {
    pthread_t hilo;

    // Crear hilo en modo detached desde el inicio
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&hilo, &attr, hilo_background, NULL);
    pthread_attr_destroy(&attr);

    printf("Hilo separado iniciado. El programa principal continúa...\n");
    sleep(10);  // El programa principal hace otras cosas

    return 0;  // El programa termina pero el hilo se termina forzosamente
}
```

### Múltiples hilos con un array

```c
#define NUM_HILOS 5

void *funcion_hilo(void *arg) {
    int id = *((int*)arg);
    printf("Hilo %d ejecutándose\n", id);
    sleep(id);  // Diferentes tiempos de espera
    printf("Hilo %d terminado\n", id);
    return NULL;
}

int main() {
    pthread_t hilos[NUM_HILOS];
    int ids[NUM_HILOS];

    // Crear múltiples hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, funcion_hilo, &ids[i]);
    }

    // Esperar a que todos terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("Todos los hilos han terminado\n");
    return 0;
}
```

## Sincronización básica entre hilos

### Uso de mutex

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contador = 0;

void *incrementar(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        contador++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;

    pthread_create(&hilo1, NULL, incrementar, NULL);
    pthread_create(&hilo2, NULL, incrementar, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Valor final del contador: %d\n", contador);

    pthread_mutex_destroy(&mutex);
    return 0;
}
```

### Uso de variables de condición

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicion = PTHREAD_COND_INITIALIZER;
int listo = 0;

void *productor(void *arg) {
    sleep(2);  // Simular procesamiento

    pthread_mutex_lock(&mutex);
    listo = 1;
    printf("Productor: datos listos\n");
    pthread_cond_signal(&condicion);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *consumidor(void *arg) {
    pthread_mutex_lock(&mutex);
    while (listo == 0) {
        printf("Consumidor: esperando datos...\n");
        pthread_cond_wait(&condicion, &mutex);
    }
    printf("Consumidor: procesando datos\n");
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t hilo_prod, hilo_cons;

    pthread_create(&hilo_cons, NULL, consumidor, NULL);
    pthread_create(&hilo_prod, NULL, productor, NULL);

    pthread_join(hilo_prod, NULL);
    pthread_join(hilo_cons, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condicion);

    return 0;
}
```

## Problemas comunes y soluciones

### Condiciones de carrera

Ocurren cuando múltiples hilos acceden a datos compartidos simultáneamente.

**Solución**: Usar mutex para proteger acceso a variables compartidas.

```c
// Incorrecto (condición de carrera)
contador++;

// Correcto
pthread_mutex_lock(&mutex);
contador++;
pthread_mutex_unlock(&mutex);
```

### Deadlocks (interbloqueos)

Ocurren cuando dos o más hilos se bloquean indefinidamente esperando recursos que otros tienen.

**Solución**: Adquirir los mutex siempre en el mismo orden.

```c
// Potencial deadlock
// Hilo 1:
pthread_mutex_lock(&mutex_A);
pthread_mutex_lock(&mutex_B);

// Hilo 2:
pthread_mutex_lock(&mutex_B);
pthread_mutex_lock(&mutex_A);

// Solución: siempre el mismo orden
// Ambos hilos:
pthread_mutex_lock(&mutex_A);
pthread_mutex_lock(&mutex_B);
```

### Variables globales compartidas

Cuando se usan variables globales entre hilos, deben ser protegidas.

**Solución**: Usar mutex o hacer las variables thread-local.

```c
// Variable thread-local (cada hilo tiene su propia copia)
__thread int contador_local;

// O usar mutex para variables compartidas
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contador_global;
```

### Cancelación segura

La cancelación de hilos puede ocurrir en cualquier punto, dejando recursos sin liberar.

**Solución**: Usar manejadores de limpieza.

```c
void limpiar_recursos(void *arg) {
    free(arg);
    pthread_mutex_unlock(&mutex);
}

void *funcion_hilo(void *arg) {
    void *recurso = malloc(1000);

    pthread_cleanup_push(limpiar_recursos, recurso);

    pthread_mutex_lock(&mutex);
    // Código cancelable...

    pthread_cleanup_pop(1);  // 1 = ejecutar la función de limpieza
    return NULL;
}
```

## Consideraciones importantes

1. **Recursos compartidos**: Proteger siempre el acceso a recursos compartidos con mutex u otros mecanismos de sincronización.

2. **Granularidad de bloqueo**: Mantener las secciones críticas (código entre lock/unlock) lo más pequeñas posible.

3. **Variables volatile**: Usar `volatile` para variables que pueden cambiar por factores externos al hilo.

4. **Thread-safety**: Asegurarse de que las funciones utilizadas son thread-safe (seguras para uso concurrente).

5. **Terminación adecuada**: Liberar siempre todos los recursos antes de terminar un hilo.

6. **No usar exit()**: Usar `pthread_exit()` para terminar un hilo en lugar de `exit()` que termina todo el proceso.

7. **Paso seguro de argumentos**: Tener cuidado con el ciclo de vida de los argumentos pasados a los hilos.

## Ejercicios relacionados

- **[Hilos básicos](../../03-hilos/thread1.c)**: Ejemplo básico de creación de hilos
- **[Tres hilos](../../03-hilos/treshilos.c)**: Creación de múltiples hilos
- **[Suma Resta Hilos](../../03-hilos/sumaRestaHilos.c)**: Operaciones concurrentes con hilos

## Recursos adicionales

Para más información, consultar las páginas de manual:

- `man pthread_create`
- `man pthread_join`
- `man pthread_mutex_init`
- `man pthread_cond_init`
