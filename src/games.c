// src/games.c

#include "../include/games.h"
#include "../include/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>

#define MAX_INPUT 32

#define TETRIS_PATH "/usr/games/tetris-bsd"
#define SNAKE_PATH  "/usr/games/snake"

static int leer_opcion(void);

void menu_centro_juegos(void)
{
    int opcion;

    do
    {
        limpiar_pantalla();

        printf("=================================\n");
        printf("      CENTRO DE JUEGOS\n");
        printf("=================================\n");
        printf("1. Tetris\n");
        printf("2. Snake\n");
        printf("0. Volver\n");
        printf("=================================\n");
        printf("Seleccione una opcion: ");

        opcion = leer_opcion();

        switch(opcion)
        {
            case 1:
                ejecutar_juego(TETRIS_PATH);
                pausar_consola();
                break;

            case 2:
                ejecutar_juego(SNAKE_PATH);
                pausar_consola();
                break;

            case 0:
                break;

            default:
                printf("\nOpcion invalida.\n");
                pausar_consola();
                break;
        }

    } while(opcion != 0);
}

void ejecutar_juego(const char *nombre_ejecutable)
{
    struct termios terminal_original;
    pid_t pid;
    int status;
    time_t inicio, fin;
    char mensaje[256];

    // 1. Verificación de permisos de ejecución
    if (access(nombre_ejecutable, X_OK) != 0)
    {
        snprintf(mensaje, sizeof(mensaje), "Juego no encontrado o sin permisos: %s", nombre_ejecutable);
        perror("access");
        log_event(LOG_ERROR, mensaje);
        return;
    }

    // 2. Guardar estado original de la terminal
    if (tcgetattr(STDIN_FILENO, &terminal_original) == -1)
    {
        perror("tcgetattr");
        log_event(LOG_ERROR, "Error al obtener atributos de terminal");
        return;
    }

    inicio = time(NULL);

    // 3. Crear proceso hijo
    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        log_event(LOG_ERROR, "Error al crear proceso hijo");
        return;
    }

    if (pid == 0)
    {
        // --- PROCESO HIJO ---
        // Restaurar handler por defecto de SIGINT
        signal(SIGINT, SIG_DFL);

        // Cargar ejecutable con execl
        execl(nombre_ejecutable, nombre_ejecutable, (char *)NULL);

        // Si execl retorna, ocurrió un error
        perror("execl");
        _exit(127);
    }

    // --- PROCESO PADRE ---
    // Ignorar SIGINT mientras se ejecuta el juego
    signal(SIGINT, SIG_IGN);

    snprintf(mensaje, sizeof(mensaje), "Juego iniciado PID=%d", pid);
    log_event(LOG_INFO, mensaje);

    // Esperar la finalización del hijo
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
    }

    // Restaurar el manejo de SIGINT por defecto
    signal(SIGINT, SIG_DFL);

    // Restaurar atributos originales de la terminal
    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_original) == -1)
    {
        perror("tcsetattr");
    }

    fin = time(NULL);

    // Generación del informe de ejecución
    printf("\n=================================\n");
    printf("REPORTE DE EJECUCION\n");
    printf("=================================\n");
    printf("PID hijo: %d\n", pid);

    if (WIFEXITED(status))
    {
        printf("Finalizacion normal\n");
        printf("Codigo de salida: %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("Finalizacion por senal\n");
        printf("Senal: %d\n", WTERMSIG(status));
    }

    printf("Duracion: %ld segundos\n", (long)(fin - inicio));
    printf("=================================\n");

    log_event(LOG_INFO, "Juego finalizado correctamente");
}

static int leer_opcion(void)
{
    char buffer[MAX_INPUT];
    char *fin;
    long valor;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return -1;
    }

    valor = strtol(buffer, &fin, 10);

    // Si no se leyeron dígitos
    if (fin == buffer)
    {
        return -1;
    }

    return (int)valor;
}
