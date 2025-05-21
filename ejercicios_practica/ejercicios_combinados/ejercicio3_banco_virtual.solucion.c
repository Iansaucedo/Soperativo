/**
 * Ejercicio 3: Banco Virtual (SOLUCIÓN)
 * 
 * Este ejercicio simula un sistema bancario virtual con múltiples hilos
 * que realizan operaciones concurrentes en cuentas bancarias.
 * Se utilizan mutex para proteger el acceso a los recursos compartidos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define NUM_CUENTAS 5
#define NUM_CLIENTES 10
#define MAX_OPERACIONES 50
#define MAX_CANTIDAD 1000

// Estructura para representar una cuenta bancaria
typedef struct {
    int id;
    float saldo;
    pthread_mutex_t mutex;
    int num_operaciones;
} Cuenta;

// Estructura para representar un registro de transacción
typedef struct {
    int id_cuenta;
    float cantidad;
    int tipo;  // 0: depósito, 1: retiro, 2: transferencia
    int cuenta_destino;  // solo para transferencias
    time_t timestamp;
} Transaccion;

// Variables globales
Cuenta cuentas[NUM_CUENTAS];
Transaccion historial[MAX_OPERACIONES];
int indice_historial = 0;
pthread_mutex_t mutex_historial = PTHREAD_MUTEX_INITIALIZER;
int operaciones_completadas = 0;
pthread_mutex_t mutex_contador = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_contador = PTHREAD_COND_INITIALIZER;

// Prototipos de funciones
void inicializar_cuentas();
void *cliente(void *arg);
int depositar(int id_cuenta, float cantidad);
int retirar(int id_cuenta, float cantidad);
int transferir(int cuenta_origen, int cuenta_destino, float cantidad);
void registrar_transaccion(int id_cuenta, float cantidad, int tipo, int cuenta_destino);
void mostrar_estado_cuentas();
void mostrar_historial();
void liberar_recursos();

// Función para inicializar las cuentas bancarias
void inicializar_cuentas() {
    // Inicializar todas las cuentas con un saldo inicial y mutex
    for (int i = 0; i < NUM_CUENTAS; i++) {
        cuentas[i].id = i;
        cuentas[i].saldo = 1000.0;  // Saldo inicial de 1000
        cuentas[i].num_operaciones = 0;
        pthread_mutex_init(&cuentas[i].mutex, NULL);
    }
    
    // Inicializar historial de transacciones
    memset(historial, 0, sizeof(historial));
    
    // Inicializar generador de números aleatorios
    srand(time(NULL));
}

// Función para realizar un depósito en una cuenta
int depositar(int id_cuenta, float cantidad) {
    // Verificar parámetros
    if (id_cuenta < 0 || id_cuenta >= NUM_CUENTAS || cantidad <= 0) {
        return -1;  // Error en los parámetros
    }
    
    // Bloquear el mutex de la cuenta
    pthread_mutex_lock(&cuentas[id_cuenta].mutex);
    
    // Actualizar el saldo de la cuenta
    cuentas[id_cuenta].saldo += cantidad;
    cuentas[id_cuenta].num_operaciones++;
    
    // Registrar la transacción en el historial
    registrar_transaccion(id_cuenta, cantidad, 0, -1);
    
    // Desbloquear el mutex de la cuenta
    pthread_mutex_unlock(&cuentas[id_cuenta].mutex);
    
    return 0;  // Operación exitosa
}

// Función para realizar un retiro de una cuenta
int retirar(int id_cuenta, float cantidad) {
    // Verificar parámetros
    if (id_cuenta < 0 || id_cuenta >= NUM_CUENTAS || cantidad <= 0) {
        return -1;  // Error en los parámetros
    }
    
    // Bloquear el mutex de la cuenta
    pthread_mutex_lock(&cuentas[id_cuenta].mutex);
    
    // Verificar si hay saldo suficiente
    if (cuentas[id_cuenta].saldo < cantidad) {
        pthread_mutex_unlock(&cuentas[id_cuenta].mutex);
        return -2;  // Saldo insuficiente
    }
    
    // Actualizar el saldo de la cuenta
    cuentas[id_cuenta].saldo -= cantidad;
    cuentas[id_cuenta].num_operaciones++;
    
    // Registrar la transacción en el historial
    registrar_transaccion(id_cuenta, cantidad, 1, -1);
    
    // Desbloquear el mutex de la cuenta
    pthread_mutex_unlock(&cuentas[id_cuenta].mutex);
    
    return 0;  // Operación exitosa
}

// Función para transferir dinero entre cuentas
int transferir(int cuenta_origen, int cuenta_destino, float cantidad) {
    // Verificar parámetros
    if (cuenta_origen < 0 || cuenta_origen >= NUM_CUENTAS ||
        cuenta_destino < 0 || cuenta_destino >= NUM_CUENTAS ||
        cuenta_origen == cuenta_destino || cantidad <= 0) {
        return -1;  // Error en los parámetros
    }
    
    // Para evitar deadlocks, siempre bloquear las cuentas en orden ascendente
    int primera = (cuenta_origen < cuenta_destino) ? cuenta_origen : cuenta_destino;
    int segunda = (cuenta_origen < cuenta_destino) ? cuenta_destino : cuenta_origen;
    
    // Bloquear el mutex de la primera cuenta
    pthread_mutex_lock(&cuentas[primera].mutex);
    
    // Bloquear el mutex de la segunda cuenta
    pthread_mutex_lock(&cuentas[segunda].mutex);
    
    // Verificar si hay saldo suficiente en la cuenta de origen
    if (cuentas[cuenta_origen].saldo < cantidad) {
        pthread_mutex_unlock(&cuentas[segunda].mutex);
        pthread_mutex_unlock(&cuentas[primera].mutex);
        return -2;  // Saldo insuficiente
    }
    
    // Realizar la transferencia
    cuentas[cuenta_origen].saldo -= cantidad;
    cuentas[cuenta_destino].saldo += cantidad;
    
    // Incrementar el contador de operaciones para ambas cuentas
    cuentas[cuenta_origen].num_operaciones++;
    cuentas[cuenta_destino].num_operaciones++;
    
    // Registrar la transacción en el historial
    registrar_transaccion(cuenta_origen, cantidad, 2, cuenta_destino);
    
    // Desbloquear los mutexes de las cuentas
    pthread_mutex_unlock(&cuentas[segunda].mutex);
    pthread_mutex_unlock(&cuentas[primera].mutex);
    
    return 0;  // Operación exitosa
}

// Función para registrar una transacción en el historial
void registrar_transaccion(int id_cuenta, float cantidad, int tipo, int cuenta_destino) {
    // Bloquear el mutex del historial
    pthread_mutex_lock(&mutex_historial);
    
    // Crear el registro de la transacción
    historial[indice_historial].id_cuenta = id_cuenta;
    historial[indice_historial].cantidad = cantidad;
    historial[indice_historial].tipo = tipo;
    historial[indice_historial].cuenta_destino = cuenta_destino;
    historial[indice_historial].timestamp = time(NULL);
    
    // Actualizar el índice del historial (circular)
    indice_historial = (indice_historial + 1) % MAX_OPERACIONES;
    
    // Desbloquear el mutex del historial
    pthread_mutex_unlock(&mutex_historial);
    
    // Incrementar contador de operaciones completadas
    pthread_mutex_lock(&mutex_contador);
    operaciones_completadas++;
    
    // Señalizar que una operación ha sido completada
    pthread_cond_signal(&cond_contador);
    pthread_mutex_unlock(&mutex_contador);
}

// Función para mostrar el estado actual de todas las cuentas
void mostrar_estado_cuentas() {
    printf("\n=== ESTADO DE CUENTAS ===\n");
    printf("ID\tSALDO\t\tOPERACIONES\n");
    
    for (int i = 0; i < NUM_CUENTAS; i++) {
        // Bloquear el mutex de la cuenta para leer su estado
        pthread_mutex_lock(&cuentas[i].mutex);
        
        printf("%d\t%.2f\t\t%d\n", 
               cuentas[i].id, 
               cuentas[i].saldo, 
               cuentas[i].num_operaciones);
        
        // Desbloquear el mutex de la cuenta
        pthread_mutex_unlock(&cuentas[i].mutex);
    }
    printf("===========================\n");
}

// Función para mostrar el historial de transacciones
void mostrar_historial() {
    printf("\n=== HISTORIAL DE TRANSACCIONES ===\n");
    printf("HORA\t\tTIPO\t\tCUENTA\tDESTINO\tCANTIDAD\n");
    
    // Bloquear el mutex del historial
    pthread_mutex_lock(&mutex_historial);
    
    // Mostrar las últimas transacciones (hasta MAX_OPERACIONES)
    int num_transacciones = (operaciones_completadas < MAX_OPERACIONES) ? 
                            operaciones_completadas : MAX_OPERACIONES;
    
    for (int i = 0; i < num_transacciones; i++) {
        int idx = (indice_historial - i - 1);
        if (idx < 0) idx += MAX_OPERACIONES;
        
        // Obtener timestamp formateado
        struct tm *tm_info = localtime(&historial[idx].timestamp);
        char buffer[20];
        strftime(buffer, 20, "%H:%M:%S", tm_info);
        
        // Mostrar los detalles de la transacción
        printf("%s\t", buffer);
        
        switch(historial[idx].tipo) {
            case 0:
                printf("DEPÓSITO\t");
                break;
            case 1:
                printf("RETIRO\t\t");
                break;
            case 2:
                printf("TRANSFERENCIA\t");
                break;
        }
        
        printf("%d\t", historial[idx].id_cuenta);
        
        if (historial[idx].tipo == 2) {
            printf("%d\t", historial[idx].cuenta_destino);
        } else {
            printf("-\t");
        }
        
        printf("%.2f\n", historial[idx].cantidad);
    }
    
    // Desbloquear el mutex del historial
    pthread_mutex_unlock(&mutex_historial);
    
    printf("===================================\n");
}

// Función que ejecuta cada hilo de cliente
void *cliente(void *arg) {
    int id_cliente = *((int *)arg);
    int num_operaciones = 5;  // Cada cliente realiza 5 operaciones
    
    printf("Cliente %d iniciado\n", id_cliente);
    
    for (int i = 0; i < num_operaciones; i++) {
        // Realizar una operación aleatoria
        int tipo_operacion = rand() % 3;  // 0: depósito, 1: retiro, 2: transferencia
        int cuenta = rand() % NUM_CUENTAS;
        float cantidad = (rand() % MAX_CANTIDAD) + 1;  // 1-1000
        int resultado;
        
        switch(tipo_operacion) {
            case 0:  // Depósito
                resultado = depositar(cuenta, cantidad);
                if (resultado == 0) {
                    printf("Cliente %d depositó %.2f en cuenta %d\n", 
                           id_cliente, cantidad, cuenta);
                } else {
                    printf("Cliente %d: error al depositar\n", id_cliente);
                    i--;  // Reintentar
                }
                break;
                
            case 1:  // Retiro
                resultado = retirar(cuenta, cantidad);
                if (resultado == 0) {
                    printf("Cliente %d retiró %.2f de cuenta %d\n", 
                           id_cliente, cantidad, cuenta);
                } else if (resultado == -2) {
                    printf("Cliente %d: fondos insuficientes en cuenta %d\n", 
                           id_cliente, cuenta);
                    i--;  // Reintentar
                } else {
                    printf("Cliente %d: error al retirar\n", id_cliente);
                    i--;  // Reintentar
                }
                break;
                
            case 2:  // Transferencia
                int cuenta_destino;
                do {
                    cuenta_destino = rand() % NUM_CUENTAS;
                } while (cuenta_destino == cuenta);
                
                resultado = transferir(cuenta, cuenta_destino, cantidad);
                if (resultado == 0) {
                    printf("Cliente %d transfirió %.2f de cuenta %d a %d\n", 
                           id_cliente, cantidad, cuenta, cuenta_destino);
                } else if (resultado == -2) {
                    printf("Cliente %d: fondos insuficientes para transferir\n", 
                           id_cliente);
                    i--;  // Reintentar
                } else {
                    printf("Cliente %d: error al transferir\n", id_cliente);
                    i--;  // Reintentar
                }
                break;
        }
        
        // Pequeña pausa entre operaciones
        usleep((rand() % 500 + 100) * 1000);  // 100-600ms
    }
    
    printf("Cliente %d ha terminado\n", id_cliente);
    free(arg);
    return NULL;
}

// Función para liberar los recursos utilizados
void liberar_recursos() {
    // Destruir los mutex de las cuentas
    for (int i = 0; i < NUM_CUENTAS; i++) {
        pthread_mutex_destroy(&cuentas[i].mutex);
    }
    
    // Destruir el mutex y variable de condición de operaciones completadas
    pthread_mutex_destroy(&mutex_contador);
    pthread_cond_destroy(&cond_contador);
    
    // Destruir el mutex del historial
    pthread_mutex_destroy(&mutex_historial);
}

int main() {
    pthread_t hilos[NUM_CLIENTES];
    
    printf("=== SIMULACIÓN DE BANCO VIRTUAL ===\n");
    
    // Inicializar las cuentas bancarias
    inicializar_cuentas();
    
    // Mostrar estado inicial de las cuentas
    printf("Estado inicial de las cuentas:\n");
    mostrar_estado_cuentas();
    
    // Crear los hilos de clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&hilos[i], NULL, cliente, id);
    }
    
    // Esperar a que se completen todas las operaciones (NUM_CLIENTES * 5)
    pthread_mutex_lock(&mutex_contador);
    while (operaciones_completadas < NUM_CLIENTES * 5) {
        pthread_cond_wait(&cond_contador, &mutex_contador);
        
        // Mostrar estado cada 10 operaciones
        if (operaciones_completadas % 10 == 0) {
            pthread_mutex_unlock(&mutex_contador);
            mostrar_estado_cuentas();
            pthread_mutex_lock(&mutex_contador);
        }
    }
    pthread_mutex_unlock(&mutex_contador);
    
    // Esperar a que terminen todos los hilos
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // Mostrar estado final y historial
    printf("\n=== FINALIZACIÓN DE LA SIMULACIÓN ===\n");
    mostrar_estado_cuentas();
    mostrar_historial();
    
    // Calcular y mostrar el balance total
    float balance_total = 0;
    for (int i = 0; i < NUM_CUENTAS; i++) {
        balance_total += cuentas[i].saldo;
    }
    printf("\nBalance total en el banco: %.2f\n", balance_total);
    
    // Liberar recursos
    liberar_recursos();
    
    return 0;
}
