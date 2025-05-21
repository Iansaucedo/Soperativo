/**
 * Ejercicio 2: Chat Multiusuario
 * 
 * En este ejercicio implementarás un sistema de chat multiusuario que utiliza
 * memoria compartida, semáforos y tuberías para la comunicación entre usuarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define MAX_USUARIOS 5
#define MAX_MENSAJES 20
#define MAX_TEXTO 100
#define MAX_NOMBRE 20

// Estructura para representar un mensaje
typedef struct {
    char usuario[MAX_NOMBRE];
    char texto[MAX_TEXTO];
    time_t tiempo;
    int tipo; // 0: normal, 1: conexión, 2: desconexión
} Mensaje;

// Estructura para la memoria compartida
typedef struct {
    Mensaje mensajes[MAX_MENSAJES];
    int indice_actual;
    int num_usuarios;
    char usuarios[MAX_USUARIOS][MAX_NOMBRE];
    int activos[MAX_USUARIOS];  // 1 si el usuario está activo, 0 si no
} ChatShared;

// Variables globales
ChatShared *chat = NULL;
sem_t *sem_chat = NULL;
char nombre_usuario[MAX_NOMBRE];
int tuberia_notificaciones[2];
int continuar_chat = 1;
int id_usuario = -1;

// Prototipos de funciones
void inicializar_chat();
void finalizar_chat();
void conectar_usuario();
void desconectar_usuario();
void enviar_mensaje(const char *texto);
void mostrar_mensajes(int desde);
void *hilo_notificaciones(void *arg);
void manejador_sigint(int sig);

// Inicializar el sistema de chat
void inicializar_chat() {
    // TODO:
    // 1. Crear la zona de memoria compartida con mmap (MAP_SHARED | MAP_ANONYMOUS)
    // 2. Inicializar la estructura ChatShared
    // 3. Crear el semáforo con sem_open para sincronización
    // 4. Crear una tubería para notificaciones de nuevos mensajes
    
}

// Finalizar el sistema de chat y liberar recursos
void finalizar_chat() {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Desconectar al usuario actual
    // 3. Desbloquear el semáforo
    // 4. Cerrar la tubería de notificaciones
    // 5. Liberar la memoria compartida con munmap
    // 6. Cerrar y eliminar el semáforo
}

// Conectar un nuevo usuario al chat
void conectar_usuario() {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Verificar si hay espacio para más usuarios
    // 3. Buscar una posición libre en el array de usuarios
    // 4. Agregar el nombre del usuario a la lista
    // 5. Marcar al usuario como activo
    // 6. Incrementar el contador de usuarios
    // 7. Añadir un mensaje de conexión
    // 8. Desbloquear el semáforo
}

// Desconectar un usuario del chat
void desconectar_usuario() {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Verificar que el usuario está conectado
    // 3. Marcar al usuario como inactivo
    // 4. Decrementar el contador de usuarios
    // 5. Añadir un mensaje de desconexión
    // 6. Desbloquear el semáforo
}

// Enviar un mensaje al chat
void enviar_mensaje(const char *texto) {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Crear un nuevo mensaje con los datos del usuario, texto y timestamp
    // 3. Añadir el mensaje al array de mensajes (si se llena, sobreescribir los más antiguos)
    // 4. Incrementar el índice actual
    // 5. Notificar a todos los usuarios (escribir en la tubería)
    // 6. Desbloquear el semáforo
}

// Mostrar los mensajes del chat desde una posición
void mostrar_mensajes(int desde) {
    // TODO:
    // 1. Bloquear el semáforo
    // 2. Recorrer los mensajes desde la posición indicada hasta la actual
    // 3. Mostrar cada mensaje con formato adecuado según su tipo
    //    (normal, conexión o desconexión)
    // 4. Desbloquear el semáforo
}

// Función para el hilo de notificaciones
void *hilo_notificaciones(void *arg) {
    // TODO:
    // 1. En un bucle mientras continuar_chat sea verdadero:
    //   a. Leer de la tubería de notificaciones
    //   b. Cuando se reciba una notificación, actualizar la pantalla
    //      mostrando los nuevos mensajes
    //   c. Si se recibe la señal de terminación, salir del bucle
    
    return NULL;
}

// Manejador de señal SIGINT (Ctrl+C)
void manejador_sigint(int sig) {
    printf("\nDesconectando del chat...\n");
    continuar_chat = 0;
    // TODO: Señalizar al hilo de notificaciones para que termine
}

int main() {
    char comando[MAX_TEXTO];
    
    printf("=== CHAT MULTIUSUARIO ===\n");
    
    // Configurar el manejador de señal para SIGINT
    signal(SIGINT, manejador_sigint);
    
    // Inicializar el sistema de chat
    inicializar_chat();
    
    // Solicitar nombre de usuario
    printf("Ingrese su nombre de usuario: ");
    fgets(nombre_usuario, MAX_NOMBRE, stdin);
    nombre_usuario[strcspn(nombre_usuario, "\n")] = 0;
    
    // Conectar al usuario
    conectar_usuario();
    
    // Mostrar mensajes anteriores
    printf("Mensajes recientes:\n");
    mostrar_mensajes(0);
    
    // Crear hilo para recibir notificaciones
    pthread_t hilo;
    pthread_create(&hilo, NULL, hilo_notificaciones, NULL);
    
    // Bucle principal
    printf("\nEscriba un mensaje o /salir para terminar\n");
    while (continuar_chat) {
        printf("> ");
        fgets(comando, MAX_TEXTO, stdin);
        comando[strcspn(comando, "\n")] = 0;
        
        if (strcmp(comando, "/salir") == 0) {
            continuar_chat = 0;
        } else if (comando[0] != '\0') {
            enviar_mensaje(comando);
        }
    }
    
    // Esperar al hilo de notificaciones
    pthread_join(hilo, NULL);
    
    // Desconectar al usuario y finalizar
    finalizar_chat();
    
    printf("Ha salido del chat. ¡Hasta pronto!\n");
    return 0;
}
