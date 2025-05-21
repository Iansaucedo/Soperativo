/**
 * Ejercicio 3: Productor-Consumidor
 * 
 * En este ejercicio implementarás el clásico problema del productor-consumidor
 * utilizando mutex y variables de condición.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCTORES 2
#define NUM_CONSUMIDORES 3
#define NUM_ITEMS 20

// Estructura para el buffer compartido
typedef struct {
    int buffer[BUFFER_SIZE];
    int contador;        // Número de elementos en el buffer
    int in;              // Índice para insertar
    int out;             // Índice para extraer
    
    // TODO: Declarar mutex y variables de condición
    // Necesitas un mutex para proteger el buffer
    // Una variable de condición para productores (buffer lleno)
    // Una variable de condición para consumidores (buffer vacío)
    
} BufferCompartido;

// Buffer global compartido
BufferCompartido buffer_compartido;

// Estructura para datos de los hilos
typedef struct {
    int id;
    int num_operaciones;
} DatosHilo;

// TODO: Inicializar el buffer compartido
void inicializar_buffer() {
    buffer_compartido.contador = 0;
    buffer_compartido.in = 0;
    buffer_compartido.out = 0;
    
    // TODO: Inicializar mutex y variables de condición
}

// TODO: Implementar función para producir un elemento
void producir(int id_productor, int item) {
    // TODO:
    // 1. Bloquear mutex
    // 2. Verificar si el buffer está lleno
    //    Si está lleno, esperar en la variable de condición de productores
    // 3. Insertar el elemento en el buffer
    // 4. Incrementar contador e índice de inserción
    // 5. Señalizar a los consumidores que hay un elemento disponible
    // 6. Desbloquear mutex
    
    printf("Productor %d: produce %d (contador: %d)\n", 
           id_productor, item, buffer_compartido.contador);
}

// TODO: Implementar función para consumir un elemento
int consumir(int id_consumidor) {
    int item;
    
    // TODO:
    // 1. Bloquear mutex
    // 2. Verificar si el buffer está vacío
    //    Si está vacío, esperar en la variable de condición de consumidores
    // 3. Extraer el elemento del buffer
    // 4. Decrementar contador e incrementar índice de extracción
    // 5. Señalizar a los productores que hay espacio disponible
    // 6. Desbloquear mutex
    
    printf("Consumidor %d: consume %d (contador: %d)\n", 
           id_consumidor, item, buffer_compartido.contador);
    
    return item;
}

// Función para el hilo productor
void* funcion_productor(void* arg) {
    DatosHilo* datos = (DatosHilo*)arg;
    
    for (int i = 0; i < datos->num_operaciones; i++) {
        // Crear un elemento (número aleatorio)
        int item = rand() % 100;
        
        // Producir el elemento
        producir(datos->id, item);
        
        // Simular tiempo de producción variable
        usleep((rand() % 100) * 1000);
    }
    
    printf("Productor %d: ha terminado\n", datos->id);
    return NULL;
}

// Función para el hilo consumidor
void* funcion_consumidor(void* arg) {
    DatosHilo* datos = (DatosHilo*)arg;
    
    for (int i = 0; i < datos->num_operaciones; i++) {
        // Consumir un elemento
        int item = consumir(datos->id);
        
        // Simular tiempo de consumo variable
        usleep((rand() % 200) * 1000);
    }
    
    printf("Consumidor %d: ha terminado\n", datos->id);
    return NULL;
}

int main() {
    pthread_t productores[NUM_PRODUCTORES];
    pthread_t consumidores[NUM_CONSUMIDORES];
    DatosHilo datos_productores[NUM_PRODUCTORES];
    DatosHilo datos_consumidores[NUM_CONSUMIDORES];
    
    printf("Problema productor-consumidor\n");
    
    // Inicializar semilla para números aleatorios
    srand(time(NULL));
    
    // Inicializar el buffer compartido
    inicializar_buffer();
    
    // Crear hilos productores
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        datos_productores[i].id = i;
        datos_productores[i].num_operaciones = NUM_ITEMS / NUM_PRODUCTORES;
        
        if (pthread_create(&productores[i], NULL, funcion_productor, 
                          &datos_productores[i]) != 0) {
            perror("Error al crear productor");
            exit(1);
        }
    }
    
    // Crear hilos consumidores
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        datos_consumidores[i].id = i;
        datos_consumidores[i].num_operaciones = NUM_ITEMS / NUM_CONSUMIDORES;
        
        if (pthread_create(&consumidores[i], NULL, funcion_consumidor, 
                          &datos_consumidores[i]) != 0) {
            perror("Error al crear consumidor");
            exit(1);
        }
    }
    
    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(productores[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(consumidores[i], NULL);
    }
    
    // TODO: Destruir mutex y variables de condición
    
    printf("Programa terminado correctamente\n");
    return 0;
}
