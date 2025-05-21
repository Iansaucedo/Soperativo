# Gestión de Memoria en Sistemas UNIX/Linux

La gestión de memoria es un aspecto fundamental de los sistemas operativos que se encarga de administrar la memoria principal (RAM) del sistema, asignándola a los procesos y liberándola cuando ya no es necesaria.

## Conceptos Básicos

### Espacio de direcciones de un proceso

Cada proceso en un sistema UNIX/Linux tiene su propio espacio de direcciones virtual, que típicamente se divide en:

1. **Text/Code**: Segmento que contiene el código ejecutable del programa (solo lectura)
2. **Data**: Segmento que contiene variables globales y estáticas inicializadas
3. **BSS**: Segmento que contiene variables globales y estáticas no inicializadas
4. **Heap**: Región para memoria asignada dinámicamente (crece hacia arriba)
5. **Stack**: Pila para variables locales y control de funciones (crece hacia abajo)

### Tipos de memoria

- **Memoria física**: RAM real del sistema
- **Memoria virtual**: Abstracción que permite a los procesos usar más memoria que la disponible físicamente
- **Memoria compartida**: Regiones de memoria accesibles por múltiples procesos
- **Memoria mapeada**: Mapeo de archivos a memoria

## Headers necesarios

```c
#include <stdlib.h>      // Para malloc(), calloc(), realloc(), free()
#include <unistd.h>      // Para brk(), sbrk()
#include <sys/mman.h>    // Para mmap(), munmap()
#include <fcntl.h>       // Para open(), flags
#include <sys/stat.h>    // Para fstat()
#include <string.h>      // Para memcpy(), memset()
```

## Funciones principales

### Gestión de memoria dinámica en C

```c
// Asignar memoria dinámica
void *malloc(size_t size);  // Asigna 'size' bytes
void *calloc(size_t nmemb, size_t size);  // Asigna e inicializa a 0
void *realloc(void *ptr, size_t size);  // Redimensiona memoria asignada

// Liberar memoria
void free(void *ptr);  // Libera memoria asignada con malloc/calloc/realloc
```

### Mapeo de memoria (mmap)

```c
// Mapear archivos o memoria anónima
void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);

// Liberar mapeo de memoria
int munmap(void *addr, size_t length);

// Cambiar protección de un mapeo de memoria
int mprotect(void *addr, size_t length, int prot);

// Sincronizar contenido de memoria mapeada con el almacenamiento físico
int msync(void *addr, size_t length, int flags);
```

### Memoria compartida

```c
// Crear un objeto de memoria compartida
int shm_open(const char *name, int oflag, mode_t mode);

// Eliminar un objeto de memoria compartida
int shm_unlink(const char *name);
```

## Ejemplos de código

### Uso básico de malloc y free

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Asignar memoria para un arreglo de 10 enteros
    int *array = (int *)malloc(10 * sizeof(int));

    if (array == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        return 1;
    }

    // Usar la memoria
    for (int i = 0; i < 10; i++) {
        array[i] = i * 10;
    }

    // Imprimir los valores
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }

    // Liberar la memoria cuando ya no se necesita
    free(array);
    array = NULL;  // Buena práctica para evitar uso después de liberar

    return 0;
}
```

### Memoria compartida con mmap anónimo

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main() {
    // Crear un mapeo de memoria anónimo compartido
    int *shared_value = mmap(NULL, sizeof(int),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS,
                            -1, 0);

    if (shared_value == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Inicializar la variable compartida
    *shared_value = 0;

    // Crear un proceso hijo
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {  // Proceso hijo
        printf("Hijo: Valor inicial = %d\n", *shared_value);
        (*shared_value)++;  // Incrementar el valor compartido
        printf("Hijo: Valor modificado = %d\n", *shared_value);
        exit(0);
    }
    else {  // Proceso padre
        wait(NULL);  // Esperar a que el hijo termine
        printf("Padre: Valor después de que el hijo terminó = %d\n", *shared_value);

        // Liberar el mapeo de memoria
        if (munmap(shared_value, sizeof(int)) == -1) {
            perror("munmap");
        }
    }

    return 0;
}
```

### Mapeo de archivos en memoria

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo>\n", argv[0]);
        return 1;
    }

    // Abrir el archivo
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Obtener el tamaño del archivo
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    // Mapear el archivo a memoria
    char *mapped_file = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (mapped_file == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // El archivo ya está mapeado, podemos cerrar el descriptor
    close(fd);

    // Imprimir el contenido del archivo
    printf("Contenido del archivo:\n%s\n", mapped_file);

    // Modificar el contenido (convirtiéndolo a mayúsculas)
    for (off_t i = 0; i < sb.st_size; i++) {
        if (mapped_file[i] >= 'a' && mapped_file[i] <= 'z') {
            mapped_file[i] = mapped_file[i] - 'a' + 'A';
        }
    }

    // Sincronizar los cambios con el archivo
    if (msync(mapped_file, sb.st_size, MS_SYNC) == -1) {
        perror("msync");
    }

    // Liberar el mapeo
    if (munmap(mapped_file, sb.st_size) == -1) {
        perror("munmap");
        return 1;
    }

    printf("El archivo ha sido modificado (convertido a mayúsculas)\n");

    return 0;
}
```

## Patrones de uso

### Patrón Productor-Consumidor con memoria compartida

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct {
    int buffer[5];  // Buffer circular de tamaño 5
    int in;         // Índice para insertar
    int out;        // Índice para extraer
    int count;      // Número de elementos en el buffer
} shared_data;

int main() {
    // Crear estructura de datos compartida
    shared_data *data = mmap(NULL, sizeof(shared_data),
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS,
                           -1, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Inicializar la estructura compartida
    data->in = 0;
    data->out = 0;
    data->count = 0;

    // Crear proceso productor
    pid_t producer = fork();

    if (producer < 0) {
        perror("fork productor");
        exit(1);
    }
    else if (producer == 0) {  // Proceso productor
        for (int i = 1; i <= 10; i++) {
            // Esperar hasta que haya espacio en el buffer
            while (data->count >= 5) {
                usleep(1000);  // Espera activa (no ideal en producción)
            }

            // Producir un elemento y colocarlo en el buffer
            data->buffer[data->in] = i;
            printf("Productor: insertó %d en posición %d\n", i, data->in);

            // Actualizar índice y contador
            data->in = (data->in + 1) % 5;
            data->count++;

            sleep(1);  // Simular tiempo de producción
        }
        exit(0);
    }

    // Crear proceso consumidor
    pid_t consumer = fork();

    if (consumer < 0) {
        perror("fork consumidor");
        kill(producer, SIGTERM);  // Terminar productor si no se puede crear consumidor
        exit(1);
    }
    else if (consumer == 0) {  // Proceso consumidor
        for (int i = 1; i <= 10; i++) {
            // Esperar hasta que haya elementos en el buffer
            while (data->count <= 0) {
                usleep(1000);  // Espera activa (no ideal en producción)
            }

            // Consumir un elemento del buffer
            int item = data->buffer[data->out];
            printf("Consumidor: extrajo %d de posición %d\n", item, data->out);

            // Actualizar índice y contador
            data->out = (data->out + 1) % 5;
            data->count--;

            sleep(2);  // Simular tiempo de consumo
        }
        exit(0);
    }

    // Proceso padre espera a que ambos hijos terminen
    waitpid(producer, NULL, 0);
    waitpid(consumer, NULL, 0);

    // Liberar memoria compartida
    if (munmap(data, sizeof(shared_data)) == -1) {
        perror("munmap");
        return 1;
    }

    return 0;
}
```

### Sección crítica con memoria compartida

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Estructura para implementar un spin-lock básico
typedef struct {
    int value;  // 0 = libre, 1 = ocupado
} lock_t;

// Función para adquirir el lock
void acquire_lock(lock_t *lock) {
    while (__sync_lock_test_and_set(&lock->value, 1)) {
        // Espera activa hasta obtener el lock
        usleep(100);
    }
}

// Función para liberar el lock
void release_lock(lock_t *lock) {
    __sync_lock_release(&lock->value);
}

int main() {
    // Datos compartidos
    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    lock_t *lock = mmap(NULL, sizeof(lock_t), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (counter == MAP_FAILED || lock == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Inicializar
    *counter = 0;
    lock->value = 0;

    // Crear varios procesos que incrementan el contador
    const int NUM_PROCESSES = 4;
    const int ITERATIONS = 10000;

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {  // Proceso hijo
            for (int j = 0; j < ITERATIONS; j++) {
                acquire_lock(lock);  // Entrar en sección crítica
                (*counter)++;        // Incrementar contador
                release_lock(lock);  // Salir de sección crítica
            }
            exit(0);
        }
    }

    // Proceso padre espera a que todos los hijos terminen
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // Verificar resultado
    printf("Valor final del contador: %d\n", *counter);
    printf("Valor esperado: %d\n", NUM_PROCESSES * ITERATIONS);

    // Liberar recursos
    munmap(counter, sizeof(int));
    munmap(lock, sizeof(lock_t));

    return 0;
}
```

## Problemas frecuentes

### Fugas de memoria (Memory leaks)

Las fugas de memoria ocurren cuando se asigna memoria dinámica pero no se libera correctamente.

```c
#include <stdio.h>
#include <stdlib.h>

void function_with_leak() {
    int *array = malloc(100 * sizeof(int));
    // Usar array...

    // Oops! Olvidamos liberar la memoria con free(array)
    // La memoria permanece asignada pero inaccesible (memory leak)
}

void function_without_leak() {
    int *array = malloc(100 * sizeof(int));
    // Usar array...

    free(array);  // Correctamente liberamos la memoria
}

int main() {
    for (int i = 0; i < 1000; i++) {
        function_with_leak();  // Llamada repetida causará un memory leak creciente
        // function_without_leak();  // Esta versión no produciría memory leak
    }
    return 0;
}
```

### Uso después de liberación (Use after free)

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    printf("Valor: %d\n", *ptr);  // Correcto: 42

    free(ptr);  // Liberamos la memoria

    // Error: uso después de liberar
    *ptr = 100;  // Comportamiento indefinido
    printf("Valor después de free: %d\n", *ptr);  // Comportamiento indefinido

    return 0;
}
```

### Desbordamiento de buffer (Buffer overflow)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buffer = malloc(10);  // Asignar 10 bytes

    // Buffer overflow: intentamos escribir más de 10 bytes
    strcpy(buffer, "Este texto es demasiado largo para el buffer de 10 bytes");

    // Esto puede corromper la memoria y causar comportamientos inesperados
    printf("Buffer: %s\n", buffer);

    free(buffer);
    return 0;
}
```

## Consideraciones importantes

1. **Liberación de memoria**: Siempre libera la memoria que has asignado (con `free()` para `malloc()`/`calloc()`/`realloc()` o `munmap()` para `mmap()`).
2. **Verificación de errores**: Comprueba siempre si la asignación de memoria tuvo éxito (NULL para malloc/calloc/realloc, MAP_FAILED para mmap).
3. **Fragmentación**: La asignación y liberación repetida de bloques de memoria puede causar fragmentación.
4. **Alineación de memoria**: Algunos sistemas requieren que los datos estén alineados a ciertos límites.
5. **Compartir memoria**: Es una forma eficiente de comunicación entre procesos, pero requiere sincronización.
6. **Permisos**: Usa los permisos adecuados (PROT_READ, PROT_WRITE, PROT_EXEC) al mapear memoria.
7. **Limpieza**: Asegúrate de liberar toda la memoria compartida y los objetos de sincronización antes de que un proceso termine.

## Ejercicios relacionados

### Ejercicios básicos

- [Ejercicio básico de mmap](../../ejercicios_practica/02-memoria/ejercicio1_mmap_basico.c)
- [Uso de memoria compartida](../../02-memoria/memoriaCompartida.c)
- [Calculadora con archivos](../../02-memoria/calculadoraEnFicheros.c)

### Ejemplos en el repositorio

- [Memoria compartida](../../02-memoria/memoriaCompartida.c)
- [Sección crítica pura](../../02-memoria/seccionCriticaPura.c)
- [Mapeo de memoria](../../ejemplos/memoria/mmap_example.c)
- [Memoria compartida](../../ejemplos/memoria/shared_memory.c)

### Ejercicios avanzados

- [Calculadora](../../02-memoria/calculadora.c)

## Referencias adicionales

- Manual de las funciones (`man malloc`, `man mmap`, `man msync`, `man shm_open`)
- "Advanced Programming in the UNIX Environment" por W. Richard Stevens
- "The Linux Programming Interface" por Michael Kerrisk
