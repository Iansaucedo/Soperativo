# Programación Básica en C y Punteros

Este documento cubre los conceptos fundamentales de programación en C, con énfasis en punteros y manejo de memoria.

## 1. Conceptos Básicos

### Tipos de datos y variables

```c
// Declaración e inicialización correcta de variables
int entero = 0;              // Inicializar siempre las variables
float decimal = 0.0f;        // 'f' para float explícito
char caracter = '\0';        // Carácter nulo
int *puntero = NULL;         // Puntero nulo
int array[10] = {0};         // Inicializar array a ceros
```

### Operadores básicos

```c
// Operadores aritméticos
int a = 5 + 3;    // Suma
int b = 5 - 3;    // Resta
int c = 5 * 3;    // Multiplicación
int d = 5 / 3;    // División entera
int e = 5 % 3;    // Módulo (resto)

// Operadores de comparación
if (a == b) {}    // Igualdad
if (a != b) {}    // Desigualdad
if (a > b) {}     // Mayor que
if (a >= b) {}    // Mayor o igual que
if (a < b) {}     // Menor que
if (a <= b) {}    // Menor o igual que

// Operadores lógicos
if (a && b) {}    // AND lógico
if (a || b) {}    // OR lógico
if (!a) {}        // NOT lógico
```

## 2. Punteros

### Conceptos básicos de punteros

```c
int numero = 42;
int *ptr = &numero;    // ptr almacena la dirección de numero

printf("Valor: %d\n", *ptr);         // Desreferenciación: obtiene el valor
printf("Dirección: %p\n", (void*)ptr); // Muestra la dirección

// Aritmética de punteros
ptr++;      // Avanza al siguiente entero (4 bytes en sistemas de 32 bits)
ptr--;      // Retrocede al anterior
ptr += 2;   // Avanza dos posiciones
```

### Punteros y arrays

```c
int array[5] = {1, 2, 3, 4, 5};
int *ptr = array;  // Los arrays decaen a punteros

// Acceso equivalente
printf("%d\n", array[0]);  // Usando índice
printf("%d\n", *ptr);      // Usando puntero
printf("%d\n", ptr[0]);    // Usando puntero como array

// Recorrido con punteros
for (int *p = array; p < array + 5; p++) {
    printf("%d ", *p);
}
```

## 3. Matrices y Punteros

### Matrices unidimensionales

```c
int array[5];
int *ptr = array;

// Inicialización correcta
for (int i = 0; i < 5; i++) {
    array[i] = 0;  // o ptr[i] = 0;
}

// Comparación de elementos
if (array[1] == array[2]) {
    printf("Elementos iguales\n");
}
```

### Matrices bidimensionales

```c
int matriz[3][4];
int (*ptr)[4] = matriz;  // Puntero a array de 4 enteros

// Inicialización correcta
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
        matriz[i][j] = 0;
        // o *(*(ptr + i) + j) = 0;
    }
}

// Comparación de filas
int filas_iguales = 1;
for (int j = 0; j < 4; j++) {
    if (matriz[0][j] != matriz[1][j]) {
        filas_iguales = 0;
        break;
    }
}
```

## 4. Gestión de Memoria

### Asignación dinámica

```c
// Asignación de memoria
int *ptr = (int *)malloc(sizeof(int));
if (ptr == NULL) {
    // Manejar error de asignación
    return -1;
}

// Uso
*ptr = 42;

// Liberación de memoria
free(ptr);
ptr = NULL;  // Buena práctica: evita dangling pointers
```

### Arrays dinámicos

```c
// Array dinámico
int *array = (int *)malloc(5 * sizeof(int));
if (array == NULL) {
    return -1;
}

// Inicialización
for (int i = 0; i < 5; i++) {
    array[i] = i;
}

// Redimensionar
int *temp = (int *)realloc(array, 10 * sizeof(int));
if (temp != NULL) {
    array = temp;
} else {
    // Manejar error
    free(array);
    return -1;
}

free(array);
```

## 5. Comparaciones y Búsqueda

### Comparación de strings

```c
char str1[] = "Hola";
char str2[] = "Hola";

// Método correcto de comparar strings
if (strcmp(str1, str2) == 0) {
    printf("Strings iguales\n");
}

// NO usar == para comparar strings
if (str1 == str2) {  // INCORRECTO: compara direcciones
    printf("Esto no funcionará como esperas\n");
}
```

### Búsqueda en arrays

```c
// Búsqueda lineal
int buscar(int *array, int tamano, int valor) {
    for (int i = 0; i < tamano; i++) {
        if (array[i] == valor) {
            return i;  // Retorna la posición
        }
    }
    return -1;  // No encontrado
}

// Búsqueda en matriz
int buscar_matriz(int matriz[][4], int filas, int valor) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < 4; j++) {
            if (matriz[i][j] == valor) {
                return i;  // Retorna la fila
            }
        }
    }
    return -1;
}
```

## 6. Errores Comunes

1. **No inicializar variables**

```c
int x;           // MAL: variable no inicializada
int x = 0;       // BIEN: variable inicializada
```

2. **No verificar NULL después de malloc**

```c
int *ptr = malloc(sizeof(int));  // MAL: sin verificación
if (ptr == NULL) {               // BIEN: con verificación
    // Manejar error
    return -1;
}
```

3. **Memory leaks**

```c
int *ptr = malloc(sizeof(int));
ptr = malloc(sizeof(int));  // MAL: memory leak
free(ptr);                  // BIEN: liberar antes de reasignar
ptr = malloc(sizeof(int));
```

4. **Acceso fuera de límites**

```c
int array[5];
array[5] = 10;  // MAL: acceso fuera de límites
array[4] = 10;  // BIEN: último elemento válido
```

## 7. Operaciones Avanzadas con Matrices

### Comparación de objetos en matrices

```c
// Estructura ejemplo
typedef struct {
    int id;
    char nombre[50];
    float valor;
} Objeto;

// Matriz de objetos
Objeto matriz[3][3];

// Comparación de objetos en la matriz
int comparar_objetos(Objeto obj1, Objeto obj2) {
    if (obj1.id != obj2.id) return 0;
    if (obj1.valor != obj2.valor) return 0;
    return strcmp(obj1.nombre, obj2.nombre) == 0;
}

// Buscar objeto en matriz
int buscar_objeto(Objeto matriz[][3], int filas, Objeto objetivo) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < 3; j++) {
            if (comparar_objetos(matriz[i][j], objetivo)) {
                return 1; // Encontrado
            }
        }
    }
    return 0; // No encontrado
}

// Comparar regiones de matriz
int comparar_region(Objeto matriz[][3], int fila1, int col1,
                   int fila2, int col2, int tam) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (!comparar_objetos(matriz[fila1+i][col1+j],
                                matriz[fila2+i][col2+j])) {
                return 0;
            }
        }
    }
    return 1;
}
```

### Matrices compartidas entre procesos

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Crear matriz compartida
int (*crear_matriz_compartida(int filas, int columnas))[4] {
    size_t size = filas * columnas * sizeof(int);

    // Crear memoria compartida
    int *matriz = mmap(NULL, size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS,
                      -1, 0);

    if (matriz == MAP_FAILED) {
        return NULL;
    }

    // Inicializar a cero
    memset(matriz, 0, size);

    // Retornar como matriz bidimensional
    return (int (*)[4])matriz;
}

// Ejemplo de uso
void ejemplo_matriz_compartida() {
    int (*matriz)[4] = crear_matriz_compartida(3, 4);
    if (matriz == NULL) {
        perror("Error creando matriz");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {  // Proceso hijo
        matriz[0][0] = 42;  // Modificar matriz
        exit(0);
    } else {  // Proceso padre
        wait(NULL);
        printf("Valor modificado: %d\n", matriz[0][0]);
        munmap(matriz, 3 * 4 * sizeof(int));
    }
}
```

## 8. Ejercicios Prácticos

### Ejercicio 1: Manipulación Básica de Arrays

- **Objetivo**: Practicar el manejo de arrays y punteros
- **Conceptos**: Arrays, punteros, aritmética de punteros
- **Archivo**: `ejercicios/array_manipulation.c`
- **Temas relacionados**:
  - Arrays y punteros (Sección 2)
  - Operadores básicos (Sección 1)

### Ejercicio 2: Strings y Caracteres

- **Objetivo**: Trabajar con strings y funciones de manipulación de caracteres
- **Conceptos**: Strings, arrays de caracteres, funciones de string
- **Archivo**: `ejercicios/string_operations.c`
- **Temas relacionados**:
  - Tipos de datos (Sección 1)
  - Punteros (Sección 2)

### Ejercicio 3: Estructuras y Memoria

- **Objetivo**: Comprender el uso de estructuras y asignación de memoria
- **Conceptos**: Estructuras, malloc/free, punteros a estructuras
- **Archivo**: `ejercicios/struct_memory.c`
- **Temas relacionados**:
  - Punteros (Sección 2)
  - Manejo de memoria

## 9. Entrada de Usuario y Control de Teclado

### Uso de getch() y getche()

```c
#include <conio.h>  // Para Windows
// Para Linux usar: #include <termios.h>

void ejemplo_getch() {
    printf("Presiona una tecla (no se mostrará): ");
    char c = getch();  // Espera una tecla sin eco
    printf("\nPresionaste: %c\n", c);

    printf("Presiona una tecla (se mostrará): ");
    c = getche();  // Espera una tecla con eco
    printf("\nPresionaste: %c\n", c);
}

// Menú interactivo con getch
void menu_interactivo() {
    char opcion;
    do {
        printf("\nMenú:\n");
        printf("1. Opción 1\n");
        printf("2. Opción 2\n");
        printf("3. Salir\n");

        opcion = getch();

        switch(opcion) {
            case '1':
                printf("\nSeleccionaste opción 1\n");
                break;
            case '2':
                printf("\nSeleccionaste opción 2\n");
                break;
            case '3':
                printf("\nSaliendo...\n");
                break;
            default:
                printf("\nOpción inválida\n");
        }
    } while(opcion != '3');
}

// Implementación de getch para Linux
#ifdef __linux__
int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}
#endif
```

### Control de entrada avanzado

```c
// Leer entrada con límite de tiempo
#include <time.h>

int leer_con_timeout(char *buffer, int size, int segundos) {
    time_t inicio = time(NULL);
    int pos = 0;

    while (pos < size - 1) {
        if (time(NULL) - inicio >= segundos) {
            buffer[pos] = '\0';
            return 0;  // Timeout
        }

        if (kbhit()) {  // Si hay tecla disponible
            char c = getch();
            if (c == '\r' || c == '\n') {
                buffer[pos] = '\0';
                return 1;  // Éxito
            }
            buffer[pos++] = c;
            putchar(c);  // Echo manual
        }
    }
    buffer[pos] = '\0';
    return 1;
}

// Leer contraseña oculta
void leer_password(char *password, int max_len) {
    int i = 0;
    char ch;

    while (i < max_len - 1) {
        ch = getch();
        if (ch == '\r' || ch == '\n') {
            break;
        } else if (ch == '\b' && i > 0) {  // Backspace
            printf("\b \b");  // Borrar último carácter
            i--;
        } else if (ch != '\b') {
            password[i++] = ch;
            printf("*");  // Mostrar asterisco
        }
    }
    password[i] = '\0';
    printf("\n");
}
```

## 10. Manipulación Avanzada de Matrices

### Rotación de matrices

```c
// Rotar matriz 90 grados en sentido horario
void rotar_matriz(int matriz[][4], int n) {
    // Transponer la matriz
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            int temp = matriz[i][j];
            matriz[i][j] = matriz[j][i];
            matriz[j][i] = temp;
        }
    }

    // Invertir cada fila
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n/2; j++) {
            int temp = matriz[i][j];
            matriz[i][j] = matriz[i][n-1-j];
            matriz[i][n-1-j] = temp;
        }
    }
}
```

### Operaciones con submatrices

```c
// Copiar submatriz
void copiar_submatriz(int origen[][4], int destino[][4],
                      int fila_origen, int col_origen,
                      int fila_destino, int col_destino,
                      int filas, int cols) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            destino[fila_destino+i][col_destino+j] =
                origen[fila_origen+i][col_origen+j];
        }
    }
}

// Sumar submatrices
int sumar_submatriz(int matriz[][4],
                    int fila_inicio, int col_inicio,
                    int filas, int cols) {
    int suma = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            suma += matriz[fila_inicio+i][col_inicio+j];
        }
    }
    return suma;
}
```

## Referencias

- "The C Programming Language" por Kernighan y Ritchie
- "C Programming: A Modern Approach" por K. N. King
- Manual de referencia de C (man pages)
