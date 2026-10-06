#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void crear_archivo();
void leer_archivo();
void actualizar_archivo();
void eliminar_archivo();
void listar_directorio();
void iniciar_bsdgames();
void limpiar_buffer();
void limpiar_pantalla();
void pausar();

int main() {
    int opcion = 0;

    while (1) {
        limpiar_pantalla();
        printf("_______________________________________________________ \n");
        printf("   MINI SISTEMA OPERATIVO (CLI)\n");
        printf("_______________________________________________________ \n");
        printf("--- GESTIÓN DE MEMORIA SECUNDARIA (CRUD) ---\n");
        printf("1. Create (Crear archivo)\n");
        printf("2. Read (Leer archivo)\n");
        printf("3. Update (Añadir texto)\n");
        printf("4. Delete (Eliminar archivo)\n");
        printf("7. Dir (Listar directorio)\n");
        printf("--- GESTIÓN DE PROCESOS ---\n");
        printf("5. Iniciar Reto BSDGames\n");
        printf("--- CONTROL DEL SISTEMA ---\n");
        printf("6. Apagar Sistema\n");
        printf("_______________________________________________________ \n");
        printf("Seleccione una opción (1-7): ");

        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            limpiar_pantalla();
            printf("Opción no válida. Ingrese un número.\n");
            pausar();
            continue;
        }
        limpiar_buffer();
        limpiar_pantalla();

        switch (opcion) {
            case 1:
                crear_archivo();
                pausar();
                break;
            case 2:
                leer_archivo();
                pausar();
                break;
            case 3:
                actualizar_archivo();
                pausar();
                break;
            case 4:
                eliminar_archivo();
                pausar();
                break;
            case 5:
                iniciar_bsdgames();
                pausar();
                break;
            case 6:
                printf("Apagando Sistema...\n¡Hasta luego!\n");
                return 0;
            case 7:
                listar_directorio();
                pausar();
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
                pausar();
                break;
        }
    }

    return 0;
}

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpiar_pantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    printf("\nPresione ENTER para continuar...");
    getchar();
}

void crear_archivo() {
    char nombre[100];
    char contenido[500];

    printf("--- CREAR ARCHIVO ---\n");
    printf("Ingrese el nombre del archivo (ej. notas.txt): ");
    if (scanf("%99s", nombre) != 1) return;
    limpiar_buffer();

    FILE *archivo = fopen(nombre, "w");
    if (archivo == NULL) {
        printf("[Error] No se pudo crear el archivo.\n");
        return;
    }

    printf("Ingrese el texto que desea guardar en el archivo:\n> ");
    if (fgets(contenido, sizeof(contenido), stdin) != NULL) {
        fputs(contenido, archivo);
    }
    
    fclose(archivo);
    printf("[Éxito] Archivo '%s' guardado correctamente.\n", nombre);
}

void leer_archivo() {
    char nombre[100];
    char ch;

    printf("--- LEER ARCHIVO ---\n");
    printf("Ingrese el nombre del archivo a leer: ");
    if (scanf("%99s", nombre) != 1) return;
    limpiar_buffer();

    FILE *archivo = fopen(nombre, "r");
    if (archivo == NULL) {
        printf("[Error] El archivo '%s' no existe o está vacío.\n", nombre);
        return;
    }

    printf("\n--- Contenido de '%s' ---\n", nombre);
    int caracteres_leidos = 0;
    while ((ch = fgetc(archivo)) != EOF) {
        putchar(ch);
        caracteres_leidos++;
    }
    
    if (caracteres_leidos == 0) {
        printf("(El archivo está vacío)");
    }
    
    printf("\n---------------------------\n");
    fclose(archivo);
}

void actualizar_archivo() {
    char nombre[100];
    char contenido[500];

    printf("--- ACTUALIZAR ARCHIVO ---\n");
    printf("Ingrese el nombre del archivo a modificar: ");
    if (scanf("%99s", nombre) != 1) return;
    limpiar_buffer();

    FILE *archivo = fopen(nombre, "a");
    if (archivo == NULL) {
        printf("[Error] No se pudo abrir el archivo '%s'.\n", nombre);
        return;
    }

    printf("Ingrese el texto adicional a añadir:\n> ");
    if (fgets(contenido, sizeof(contenido), stdin) != NULL) {
        fputs(contenido, archivo);
    }

    fclose(archivo);
    printf("[Éxito] Archivo '%s' actualizado correctamente.\n", nombre);
}

void eliminar_archivo() {
    char nombre[100];

    printf("--- ELIMINAR ARCHIVO ---\n");
    printf("Ingrese el nombre del archivo que desea eliminar: ");
    if (scanf("%99s", nombre) != 1) return;
    limpiar_buffer();

    if (remove(nombre) == 0) {
        printf("[Éxito] El archivo '%s' fue eliminado correctamente.\n", nombre);
    } else {
        printf("[Error] No se pudo eliminar el archivo. Verifique si existe.\n");
    }
}

void listar_directorio() {
    printf("--- LISTAR DIRECTORIO (DIR) ---\n");
#ifdef _WIN32
    system("dir");
#else
    system("ls -la");
#endif
}

void iniciar_bsdgames() {
    printf("--- INICIANDO PROCESO: BSDGAMES ---\n");
    printf("Intentando ejecutar un juego BSD (Tetris)...\n");

    int resultado = system("tetris-bsd || snake || bsdgames-tetris");

    if (resultado != 0) {
        printf("\n[Aviso] No se detectó 'bsdgames' instalado en el sistema operativo host.\n");
        printf("Iniciando mini-juego integrado 'Adivina el Número'...\n\n");

        int secreto = rand() % 10 + 1;
        int intento = 0;

        printf("¡Adivina el número del 1 al 10 en 3 intentos!\n");
        for (int i = 1; i <= 3; i++) {
            printf("Intento %d: ", i);
            if (scanf("%d", &intento) == 1) {
                if (intento == secreto) {
                    printf("¡Felicidades! ¡Completaste el reto BSDGames!\n");
                    limpiar_buffer();
                    return;
                } else if (intento < secreto) {
                    printf("Es mayor...\n");
                } else {
                    printf("Es menor...\n");
                }
            } else {
                limpiar_buffer();
            }
        }
        printf("¡Agotaste los intentos! El número era %d.\n", secreto);
        limpiar_buffer();
    }
}