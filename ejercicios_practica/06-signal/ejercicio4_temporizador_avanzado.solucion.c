/**
 * Ejercicio 4: Temporizador avanzado con señales (SOLUCIÓN)
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
#include <string.h>

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

// Función que muestra todos los temporizadores activos
void mostrar_temporizadores() {
    printf("\n=== TEMPORIZADORES ACTIVOS (%d) ===\n", num_temporizadores);
    
    if (num_temporizadores == 0) {
        printf("No hay temporizadores activos\n");
        return;
    }
    
    printf("%-3s %-10s %-12s %-7s %-20s\n", 
           "ID", "Intervalo", "Repeticiones", "Activo", "Descripción");
    
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (temporizadores[i].activo) {
            time_t tiempo_actual = time(NULL);
            int tiempo_restante = temporizadores[i].segundos - 
                                  (tiempo_actual - temporizadores[i].tiempo_inicio);
            if (tiempo_restante < 0) tiempo_restante = 0;
            
            printf("%-3d %-2ds (-%ds) %-2d/%-7d %-7s %-20s\n", 
                   temporizadores[i].id,
                   temporizadores[i].segundos,
                   tiempo_restante,
                   temporizadores[i].contador,
                   temporizadores[i].repeticiones,
                   temporizadores[i].activo ? "Sí" : "No",
                   temporizadores[i].descripcion);
        }
    }
}

// Función que encuentra el próximo temporizador a activar
// Esta función recorre todos los temporizadores y encuentra el que vence antes
int encontrar_siguiente_temporizador() {
    int indice_siguiente = -1;
    time_t tiempo_actual = time(NULL);
    time_t menor_tiempo_restante = (time_t)-1;
    
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (temporizadores[i].activo) {
            time_t tiempo_transcurrido = tiempo_actual - temporizadores[i].tiempo_inicio;
            time_t tiempo_restante = temporizadores[i].segundos - tiempo_transcurrido;
            
            // Si el temporizador ya debería haber expirado, lo consideramos con prioridad
            if (tiempo_restante <= 0) {
                return i;
            }
            
            // Actualizamos el temporizador con menor tiempo restante
            if (tiempo_restante < menor_tiempo_restante || menor_tiempo_restante == (time_t)-1) {
                menor_tiempo_restante = tiempo_restante;
                indice_siguiente = i;
            }
        }
    }
    
    return indice_siguiente; // Retorna -1 si no hay temporizadores activos
}

// Manejador de la señal SIGALRM
// Este manejador:
// 1. Identifica qué temporizador ha vencido
// 2. Muestra un mensaje con su información
// 3. Actualiza el contador de repeticiones
// 4. Si no ha alcanzado el máximo de repeticiones, programa la próxima alarma
// 5. Si un temporizador ha terminado todas sus repeticiones, lo marca como inactivo
// 6. Programa el siguiente temporizador que deba activarse
void manejador_alarma(int sig) {
    time_t tiempo_actual = time(NULL);
    
    // Identificar el temporizador que ha vencido
    int temporizador_expirado = -1;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (temporizadores[i].activo) {
            time_t tiempo_transcurrido = tiempo_actual - temporizadores[i].tiempo_inicio;
            if (tiempo_transcurrido >= temporizadores[i].segundos) {
                temporizador_expirado = i;
                break;
            }
        }
    }
    
    if (temporizador_expirado >= 0) {
        // Mostrar mensaje
        printf("\n[ALARMA] Temporizador %d: %s\n", 
               temporizadores[temporizador_expirado].id,
               temporizadores[temporizador_expirado].descripcion);
        
        // Actualizar contador
        temporizadores[temporizador_expirado].contador++;
        
        // Verificar si ha alcanzado el máximo de repeticiones
        if (temporizadores[temporizador_expirado].repeticiones > 0 && 
            temporizadores[temporizador_expirado].contador >= 
            temporizadores[temporizador_expirado].repeticiones) {
            
            // Desactivar el temporizador
            temporizadores[temporizador_expirado].activo = 0;
            printf("Temporizador %d finalizado\n", temporizadores[temporizador_expirado].id);
            
            // Decrementar contador de temporizadores activos
            num_temporizadores--;
        } else {
            // Reiniciar el tiempo de inicio para la próxima activación
            temporizadores[temporizador_expirado].tiempo_inicio = tiempo_actual;
        }
    }
    
    // Encontrar el próximo temporizador a activar
    siguiente_alarma = encontrar_siguiente_temporizador();
    
    // Programar la próxima alarma si hay temporizadores activos
    if (siguiente_alarma >= 0) {
        time_t tiempo_transcurrido = tiempo_actual - 
                                    temporizadores[siguiente_alarma].tiempo_inicio;
        unsigned int segundos_restantes = temporizadores[siguiente_alarma].segundos - 
                                         tiempo_transcurrido;
        
        // Asegurar que no sea cero
        if (segundos_restantes == 0) segundos_restantes = 1;
        
        alarm(segundos_restantes);
    }
    
    printf("> "); // Restaurar el prompt
    fflush(stdout);
}

// Función para crear un nuevo temporizador
// Esta función inicializa un nuevo temporizador con los datos proporcionados
int crear_temporizador(int segundos, int repeticiones, const char* descripcion) {
    // Verificar si hay espacio para más temporizadores
    if (num_temporizadores >= MAX_TIMERS) {
        return -1;
    }
    
    // Buscar una posición libre
    int indice = -1;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!temporizadores[i].activo) {
            indice = i;
            break;
        }
    }
    
    if (indice < 0) {
        return -1;
    }
    
    // Inicializar el nuevo temporizador
    temporizadores[indice].id = indice;
    temporizadores[indice].segundos = segundos;
    temporizadores[indice].repeticiones = repeticiones;
    temporizadores[indice].contador = 0;
    strncpy(temporizadores[indice].descripcion, descripcion, 49);
    temporizadores[indice].descripcion[49] = '\0';
    temporizadores[indice].tiempo_inicio = time(NULL);
    temporizadores[indice].activo = 1;
    
    // Incrementar contador de temporizadores activos
    num_temporizadores++;
    
    return indice;
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
    
    // Inicializar array de temporizadores
    for (int i = 0; i < MAX_TIMERS; i++) {
        temporizadores[i].activo = 0;
    }
    
    // Bucle principal del programa
    while (1) {
        procesar_comando();
    }
    
    return 0;
}
