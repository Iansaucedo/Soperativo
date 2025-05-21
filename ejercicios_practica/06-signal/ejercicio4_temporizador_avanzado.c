/**
 * Ejercicio 4: Temporizador avanzado con señales
 *
 * En este ejercicio implementarás un sistema de temporizadores usando
 * señales SIGALRM y estructuras de datos para gestionar múltiples
 * temporizadores simultáneos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define MAX_TIMERS 5

// Estructura para representar un temporizador
typedef struct {
    int id;
    int segundos;
    int repeticiones;
    int contador;
    char descripcion[50];
    time_t tiempo_inicio;
    int activo;
} Temporizador;

// Array global de temporizadores
Temporizador temporizadores[MAX_TIMERS];
int num_temporizadores = 0;
int siguiente_alarma = 0;

// TODO: Implementa la función que muestra todos los temporizadores activos
void mostrar_temporizadores() {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementa la función que encuentra el próximo temporizador a activar
// Esta función debe recorrer todos los temporizadores y encontrar el que vence antes
int encontrar_siguiente_temporizador() {
    // TU CÓDIGO AQUÍ
    return -1; // Retorna -1 si no hay temporizadores activos
}

// TODO: Implementa el manejador de la señal SIGALRM
// Este manejador debe:
// 1. Identificar qué temporizador ha vencido
// 2. Mostrar un mensaje con su información
// 3. Actualizar el contador de repeticiones
// 4. Si no ha alcanzado el máximo de repeticiones, programar la próxima alarma
// 5. Si un temporizador ha terminado todas sus repeticiones, marcarlo como inactivo
// 6. Programar el siguiente temporizador que deba activarse

void manejador_alarma(int sig) {
    // TU CÓDIGO AQUÍ
}

// TODO: Implementa la función para crear un nuevo temporizador
// Esta función debe inicializar un nuevo temporizador con los datos proporcionados
int crear_temporizador(int segundos, int repeticiones, const char* descripcion) {
    // TU CÓDIGO AQUÍ
    return -1; // Retorna -1 si no se pudo crear
}

// Función para procesar comandos del usuario
void procesar_comando() {
    char comando[100];
    int segundos, repeticiones;
    char descripcion[50];
    
    printf("\nComandos disponibles:\n");
    printf("  nuevo <segundos> <repeticiones> <descripcion> - Crea un nuevo temporizador\n");
    printf("  lista - Muestra los temporizadores activos\n");
    printf("  salir - Termina el programa\n");
    printf("> ");
    
    fgets(comando, sizeof(comando), stdin);
    
    if (strncmp(comando, "nuevo ", 6) == 0) {
        if (sscanf(comando, "nuevo %d %d %49[^\n]", &segundos, &repeticiones, descripcion) == 3) {
            int id = crear_temporizador(segundos, repeticiones, descripcion);
            if (id >= 0) {
                printf("Temporizador creado con ID %d\n", id);
                
                // Primera programación de alarma si es necesario
                if (num_temporizadores == 1) {
                    siguiente_alarma = 0;
                    alarm(temporizadores[0].segundos);
                } else {
                    // Comprobar si este nuevo temporizador debe activarse antes que el actual
                    int sig = encontrar_siguiente_temporizador();
                    if (sig != siguiente_alarma) {
                        siguiente_alarma = sig;
                        alarm(temporizadores[sig].segundos);
                    }
                }
            } else {
                printf("No se pudo crear el temporizador. Límite alcanzado.\n");
            }
        } else {
            printf("Formato incorrecto. Use: nuevo <segundos> <repeticiones> <descripcion>\n");
        }
    } else if (strncmp(comando, "lista", 5) == 0) {
        mostrar_temporizadores();
    } else if (strncmp(comando, "salir", 5) == 0) {
        exit(0);
    } else {
        printf("Comando desconocido\n");
    }
}

int main() {
    // Configurar el manejador de la señal SIGALRM
    if (signal(SIGALRM, manejador_alarma) == SIG_ERR) {
        perror("Error al configurar manejador SIGALRM");
        return 1;
    }
    
    printf("Sistema de temporizadores con señales\n");
    printf("Puede crear hasta %d temporizadores simultáneos\n", MAX_TIMERS);
    
    // Bucle principal del programa
    while (1) {
        procesar_comando();
    }
    
    return 0;
}
