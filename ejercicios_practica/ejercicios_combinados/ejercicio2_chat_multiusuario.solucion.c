/**
 * Ejercicio 2: Chat Multiusuario (SOLUCIÓN)
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
#include <pthread.h>

#define MAX_USUARIOS 5
#define MAX_MENSAJES 20
#define MAX_TEXTO 100
#define MAX_NOMBRE 20
#define NOMBRE_SEMAFORO "/chat_semaforo"

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
    // 1. Crear la zona de memoria compartida con mmap
    chat = mmap(NULL, sizeof(ChatShared), PROT_READ | PROT_WRITE, 
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                
    if (chat == MAP_FAILED) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // 2. Inicializar la estructura ChatShared
    chat->indice_actual = 0;
    chat->num_usuarios = 0;
    memset(chat->mensajes, 0, sizeof(chat->mensajes));
    memset(chat->usuarios, 0, sizeof(chat->usuarios));
    memset(chat->activos, 0, sizeof(chat->activos));
    
    // 3. Crear el semáforo para sincronización
    sem_chat = sem_open(NOMBRE_SEMAFORO, O_CREAT | O_EXCL, 0644, 1);
    
    if (sem_chat == SEM_FAILED) {
        // Si ya existe, intentamos abrirlo
        sem_chat = sem_open(NOMBRE_SEMAFORO, 0);
        if (sem_chat == SEM_FAILED) {
            perror("Error al crear el semáforo");
            munmap(chat, sizeof(ChatShared));
            exit(EXIT_FAILURE);
        }
    }
    
    // 4. Crear una tubería para notificaciones de nuevos mensajes
    if (pipe(tuberia_notificaciones) == -1) {
        perror("Error al crear la tubería");
        sem_close(sem_chat);
        sem_unlink(NOMBRE_SEMAFORO);
        munmap(chat, sizeof(ChatShared));
        exit(EXIT_FAILURE);
    }
    
    // Configurar la tubería como no bloqueante
    fcntl(tuberia_notificaciones[0], F_SETFL, O_NONBLOCK);
    fcntl(tuberia_notificaciones[1], F_SETFL, O_NONBLOCK);
}

// Finalizar el sistema de chat y liberar recursos
void finalizar_chat() {
    // 1. Bloquear el semáforo
    sem_wait(sem_chat);
    
    // 2. Desconectar al usuario actual
    desconectar_usuario();
    
    // 3. Desbloquear el semáforo
    sem_post(sem_chat);
    
    // 4. Cerrar la tubería de notificaciones
    close(tuberia_notificaciones[0]);
    close(tuberia_notificaciones[1]);
    
    // 5. Liberar la memoria compartida con munmap
    munmap(chat, sizeof(ChatShared));
    
    // 6. Cerrar y eliminar el semáforo
    sem_close(sem_chat);
    sem_unlink(NOMBRE_SEMAFORO);
}

// Conectar un nuevo usuario al chat
void conectar_usuario() {
    // 1. Bloquear el semáforo
    sem_wait(sem_chat);
    
    // 2. Verificar si hay espacio para más usuarios
    if (chat->num_usuarios >= MAX_USUARIOS) {
        printf("Error: El chat está lleno. No es posible conectarse.\n");
        sem_post(sem_chat);
        exit(EXIT_FAILURE);
    }
    
    // 3. Buscar una posición libre en el array de usuarios
    int i;
    for (i = 0; i < MAX_USUARIOS; i++) {
        if (chat->activos[i] == 0) {
            // Posición libre encontrada
            id_usuario = i;
            break;
        }
    }
    
    // 4. Agregar el nombre del usuario a la lista
    strncpy(chat->usuarios[id_usuario], nombre_usuario, MAX_NOMBRE - 1);
    chat->usuarios[id_usuario][MAX_NOMBRE - 1] = '\0';
    
    // 5. Marcar al usuario como activo
    chat->activos[id_usuario] = 1;
    
    // 6. Incrementar el contador de usuarios
    chat->num_usuarios++;
    
    // 7. Añadir un mensaje de conexión
    int idx = chat->indice_actual;
    strncpy(chat->mensajes[idx].usuario, nombre_usuario, MAX_NOMBRE - 1);
    strcpy(chat->mensajes[idx].texto, "se ha conectado al chat");
    chat->mensajes[idx].tiempo = time(NULL);
    chat->mensajes[idx].tipo = 1; // Mensaje de conexión
    
    // Actualizar el índice de mensajes
    chat->indice_actual = (chat->indice_actual + 1) % MAX_MENSAJES;
    
    // 8. Desbloquear el semáforo
    sem_post(sem_chat);
    
    // Notificar a otros usuarios
    char notificacion = 1;
    write(tuberia_notificaciones[1], &notificacion, 1);
}

// Desconectar un usuario del chat
void desconectar_usuario() {
    if (id_usuario < 0) return; // Usuario no conectado
    
    // Ya estamos dentro de un bloque con el semáforo bloqueado
    
    // 2. Verificar que el usuario está conectado
    if (chat->activos[id_usuario]) {
        // 3. Marcar al usuario como inactivo
        chat->activos[id_usuario] = 0;
        
        // 4. Decrementar el contador de usuarios
        chat->num_usuarios--;
        
        // 5. Añadir un mensaje de desconexión
        int idx = chat->indice_actual;
        strncpy(chat->mensajes[idx].usuario, nombre_usuario, MAX_NOMBRE - 1);
        strcpy(chat->mensajes[idx].texto, "se ha desconectado del chat");
        chat->mensajes[idx].tiempo = time(NULL);
        chat->mensajes[idx].tipo = 2; // Mensaje de desconexión
        
        // Actualizar el índice de mensajes
        chat->indice_actual = (chat->indice_actual + 1) % MAX_MENSAJES;
        
        // Notificar a otros usuarios
        char notificacion = 1;
        write(tuberia_notificaciones[1], &notificacion, 1);
    }
    
    // No desbloqueamos el semáforo aquí, ya que esta función se llama desde
    // finalizar_chat() que ya maneja el semáforo
}

// Enviar un mensaje al chat
void enviar_mensaje(const char *texto) {
    // 1. Bloquear el semáforo
    sem_wait(sem_chat);
    
    // 2. Crear un nuevo mensaje con los datos del usuario, texto y timestamp
    int idx = chat->indice_actual;
    strncpy(chat->mensajes[idx].usuario, nombre_usuario, MAX_NOMBRE - 1);
    strncpy(chat->mensajes[idx].texto, texto, MAX_TEXTO - 1);
    chat->mensajes[idx].tiempo = time(NULL);
    chat->mensajes[idx].tipo = 0; // Mensaje normal
    
    // 3. Añadir el mensaje al array (sobreescribir los más antiguos si está lleno)
    // 4. Incrementar el índice actual (circular)
    chat->indice_actual = (chat->indice_actual + 1) % MAX_MENSAJES;
    
    // 5. Notificar a todos los usuarios
    char notificacion = 1;
    write(tuberia_notificaciones[1], &notificacion, 1);
    
    // 6. Desbloquear el semáforo
    sem_post(sem_chat);
}

// Mostrar los mensajes del chat desde una posición
void mostrar_mensajes(int desde) {
    // 1. Bloquear el semáforo
    sem_wait(sem_chat);
    
    // Calcular cuántos mensajes mostrar
    int total_mensajes = 0;
    int i;
    
    // Si hay menos mensajes que MAX_MENSAJES
    if (chat->indice_actual < MAX_MENSAJES && desde == 0) {
        total_mensajes = chat->indice_actual;
    } 
    // Si hay MAX_MENSAJES o más
    else {
        total_mensajes = MAX_MENSAJES;
    }
    
    // 2. Recorrer los mensajes desde la posición indicada
    struct tm *tm_info;
    char time_buffer[20];
    
    // Ajustar 'desde' si es necesario
    if (desde < 0) desde = 0;
    if (desde >= total_mensajes) desde = 0;
    
    // Mostrar mensajes
    for (i = desde; i < total_mensajes; i++) {
        int idx = (chat->indice_actual - total_mensajes + i) % MAX_MENSAJES;
        if (idx < 0) idx += MAX_MENSAJES; // Asegurar índice positivo
        
        tm_info = localtime(&chat->mensajes[idx].tiempo);
        strftime(time_buffer, 20, "%H:%M:%S", tm_info);
        
        // 3. Mostrar cada mensaje con formato según su tipo
        switch (chat->mensajes[idx].tipo) {
            case 0: // Mensaje normal
                printf("[%s] %s: %s\n", time_buffer, 
                       chat->mensajes[idx].usuario,
                       chat->mensajes[idx].texto);
                break;
            case 1: // Conexión
                printf("[%s] *** %s %s ***\n", time_buffer,
                       chat->mensajes[idx].usuario,
                       chat->mensajes[idx].texto);
                break;
            case 2: // Desconexión
                printf("[%s] *** %s %s ***\n", time_buffer,
                       chat->mensajes[idx].usuario,
                       chat->mensajes[idx].texto);
                break;
        }
    }
    
    // 4. Desbloquear el semáforo
    sem_post(sem_chat);
}

// Función para el hilo de notificaciones
void *hilo_notificaciones(void *arg) {
    char buffer;
    int ultimo_indice = 0;
    struct timespec ts = {0, 100000000}; // 100ms
    
    // 1. En un bucle mientras continuar_chat sea verdadero
    while (continuar_chat) {
        // a. Leer de la tubería de notificaciones
        int n = read(tuberia_notificaciones[0], &buffer, 1);
        
        if (n > 0) {
            // b. Cuando se reciba una notificación, actualizar la pantalla
            sem_wait(sem_chat);
            int nuevo_indice = chat->indice_actual;
            sem_post(sem_chat);
            
            // Mostrar mensajes nuevos
            if (nuevo_indice != ultimo_indice) {
                printf("\nNuevos mensajes:\n");
                mostrar_mensajes(ultimo_indice);
                ultimo_indice = nuevo_indice;
                printf("> ");
                fflush(stdout);
            }
        }
        
        // Pequeña pausa para no consumir demasiada CPU
        nanosleep(&ts, NULL);
    }
    
    // c. Si se recibe la señal de terminación, salir del bucle
    return NULL;
}

// Manejador de señal SIGINT (Ctrl+C)
void manejador_sigint(int sig) {
    printf("\nDesconectando del chat...\n");
    continuar_chat = 0;
    // Señalizar al hilo de notificaciones para que termine
    char notificacion = 1;
    write(tuberia_notificaciones[1], &notificacion, 1);
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
