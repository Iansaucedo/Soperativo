/**
 * Ejercicio 3: Banco Virtual
 * 
 * En este ejercicio implementarás la simulación de un banco virtual con
 * múltiples cajeros (hilos) y clientes que realizan diferentes operaciones.
 * Utilizarás hilos, mutex y variables de condición para sincronización.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_CUENTAS 10
#define NUM_CAJEROS 3
#define NUM_CLIENTES 15
#define MAX_OPERACIONES 50
#define MAX_MONTO 1000

// Tipos de operaciones bancarias
typedef enum {
    DEPOSITO,
    RETIRO,
    CONSULTA,
    TRANSFERENCIA
} TipoOperacion;

// Estructura para representar una cuenta
typedef struct {
    int id;
    float saldo;
    pthread_mutex_t mutex;
    int num_operaciones;
} Cuenta;

// Estructura para representar una operación
typedef struct {
    int id;
    TipoOperacion tipo;
    int cuenta_origen;
    int cuenta_destino;  // Solo para transferencias
    float monto;
    int completada;
    int cajero_asignado;
    time_t tiempo_inicio;
    time_t tiempo_fin;
} Operacion;

// Estructura para representar el banco
typedef struct {
    Cuenta cuentas[NUM_CUENTAS];
    Operacion operaciones[MAX_OPERACIONES];
    int siguiente_operacion;
    int total_operaciones;
    
    pthread_mutex_t mutex_banco;
    pthread_cond_t hay_operaciones;
    pthread_cond_t fin_operaciones;
    
    int cajeros_activos;
    int banco_abierto;
} Banco;

// Variables globales
Banco banco;
pthread_t cajeros[NUM_CAJEROS];
pthread_t clientes[NUM_CLIENTES];

// Prototipos de funciones
void inicializar_banco();
void *funcion_cajero(void *arg);
void *funcion_cliente(void *arg);
int crear_operacion(TipoOperacion tipo, int cuenta_origen, int cuenta_destino, float monto);
void procesar_operacion(int id_operacion, int id_cajero);
void finalizar_banco();
void imprimir_estado_cuentas();
void imprimir_estadisticas();

// Nombre de las operaciones para impresión
const char* nombre_operacion[] = {
    "DEPOSITO",
    "RETIRO",
    "CONSULTA",
    "TRANSFERENCIA"
};

// Inicializar el banco
void inicializar_banco() {
    // TODO:
    // 1. Inicializar el mutex del banco
    // 2. Inicializar las variables de condición
    // 3. Inicializar el array de cuentas con saldos aleatorios
    // 4. Inicializar cada mutex de cuenta
    // 5. Inicializar el array de operaciones
    // 6. Configurar los valores iniciales de contador de operaciones, etc.
}

// Función que ejecutan los cajeros
void *funcion_cajero(void *arg) {
    int id_cajero = *((int *)arg);
    printf("Cajero %d iniciando turno\n", id_cajero);
    
    // TODO:
    // 1. Mientras el banco esté abierto:
    //   a. Bloquear el mutex del banco
    //   b. Si no hay operaciones pendientes, esperar en la variable de condición
    //   c. Cuando hay una operación, tomarla y desbloquear el mutex
    //   d. Procesar la operación
    //   e. Volver a bloquear el mutex para marcarla como completada
    //   f. Si no quedan operaciones pendientes y el banco está cerrado, señalizar fin_operaciones
    //   g. Desbloquear el mutex
    
    printf("Cajero %d terminando turno\n", id_cajero);
    return NULL;
}

// Función que ejecutan los clientes
void *funcion_cliente(void *arg) {
    int id_cliente = *((int *)arg);
    
    // TODO:
    // 1. Generar operaciones aleatorias (entre 1 y 3 por cliente):
    //   a. Elegir aleatoriamente el tipo de operación
    //   b. Elegir cuentas aleatorias
    //   c. Generar montos aleatorios
    //   d. Crear la operación y añadirla al banco
    //   e. Señalizar que hay una nueva operación
    //   f. Esperar un tiempo aleatorio entre operaciones
    
    printf("Cliente %d ha finalizado todas sus operaciones\n", id_cliente);
    return NULL;
}

// Crear una nueva operación bancaria
int crear_operacion(TipoOperacion tipo, int cuenta_origen, int cuenta_destino, float monto) {
    // TODO:
    // 1. Bloquear el mutex del banco
    // 2. Verificar si hay espacio para más operaciones
    // 3. Inicializar la nueva operación
    // 4. Incrementar contador de operaciones
    // 5. Desbloquear el mutex
    // 6. Retornar el ID de la operación
    
    return -1; // Error
}

// Procesar una operación bancaria
void procesar_operacion(int id_operacion, int id_cajero) {
    // TODO:
    // 1. Verificar que la operación existe
    // 2. Según el tipo de operación:
    //   a. DEPOSITO: Bloquear mutex de cuenta, aumentar saldo, desbloquear
    //   b. RETIRO: Bloquear mutex de cuenta, verificar fondos, reducir saldo, desbloquear
    //   c. CONSULTA: Bloquear mutex de cuenta, obtener saldo, desbloquear
    //   d. TRANSFERENCIA: Bloquear ambas cuentas en orden correcto (para evitar deadlocks),
    //      verificar fondos, reducir saldo origen, aumentar saldo destino, desbloquear
    // 3. Registrar el cajero que procesó la operación y el tiempo de finalización
    // 4. Simular un tiempo de procesamiento (sleep)
    // 5. Mostrar información de la operación procesada
}

// Finalizar el banco
void finalizar_banco() {
    // TODO:
    // 1. Bloquear el mutex del banco
    // 2. Marcar el banco como cerrado
    // 3. Señalizar a los cajeros que no habrá más operaciones
    // 4. Esperar a que todos los cajeros terminen (usando la variable de condición)
    // 5. Desbloquear el mutex
    // 6. Hacer join de todos los hilos cajeros
    // 7. Destruir mutex y variables de condición
    // 8. Destruir los mutex de las cuentas
}

// Imprimir el estado actual de todas las cuentas
void imprimir_estado_cuentas() {
    // TODO: 
    // 1. Bloquear el mutex del banco
    // 2. Para cada cuenta, bloquear su mutex, imprimir su ID y saldo, desbloquear
    // 3. Imprimir el total de fondos en el banco
    // 4. Desbloquear el mutex del banco
}

// Imprimir estadísticas de las operaciones
void imprimir_estadisticas() {
    // TODO:
    // 1. Contar operaciones por tipo
    // 2. Contar operaciones por cajero
    // 3. Calcular tiempos promedio de procesamiento
    // 4. Mostrar información detallada
}

int main() {
    int id_cajeros[NUM_CAJEROS];
    int id_clientes[NUM_CLIENTES];
    
    printf("=== BANCO VIRTUAL ===\n");
    
    // Inicializar semilla para números aleatorios
    srand(time(NULL));
    
    // Inicializar el banco
    inicializar_banco();
    
    printf("Banco abierto con %d cuentas\n", NUM_CUENTAS);
    
    // Crear hilos cajeros
    for (int i = 0; i < NUM_CAJEROS; i++) {
        id_cajeros[i] = i;
        pthread_create(&cajeros[i], NULL, funcion_cajero, &id_cajeros[i]);
    }
    
    // Crear hilos clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        id_clientes[i] = i;
        pthread_create(&clientes[i], NULL, funcion_cliente, &id_clientes[i]);
    }
    
    // Esperar a que todos los clientes terminen
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pthread_join(clientes[i], NULL);
    }
    
    printf("\nTodos los clientes han terminado. Cerrando el banco...\n\n");
    
    // Cerrar el banco y esperar a que los cajeros terminen
    finalizar_banco();
    
    // Imprimir estado final y estadísticas
    imprimir_estado_cuentas();
    imprimir_estadisticas();
    
    printf("\nBanco cerrado con éxito.\n");
    return 0;
}
