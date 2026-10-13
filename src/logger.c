#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include "logger.h"

static void obtener_ruta_bitacora(char *buffer, size_t size) {
    const char *homedir = getenv("HOME");
    if (!homedir) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    snprintf(buffer, size, "%s/miniSO/disk/bitacora.txt", homedir);
}

void log_init(void) {
    const char *homedir = getenv("HOME");
    if (!homedir) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    
    char ruta_disk[512];
    snprintf(ruta_disk, sizeof(ruta_disk), "%s/miniSO/disk", homedir);
    
    mkdir(ruta_disk, 0755);
}

void logger_init(void) {
    log_init();
}

void log_event(LogLevel level, const char *mensaje) {
    log_init();
    
    char ruta_file[512];
    obtener_ruta_bitacora(ruta_file, sizeof(ruta_file));
    
    FILE *f = fopen(ruta_file, "a");
    if (!f) {
        perror("Error al abrir bitacora.txt");
        return;
    }
    
    time_t ahora = time(NULL);
    struct tm *tm_info = localtime(&ahora);
    char tiempo_str[26];
    strftime(tiempo_str, sizeof(tiempo_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    const char *level_str = "INFO";
    if (level == LOG_WARNING) level_str = "WARNING";
    else if (level == LOG_ERROR) level_str = "ERROR";

    fprintf(f, "[%s] [%s] %s\n", tiempo_str, level_str, mensaje);
    fclose(f);
}

void logger_mostrar(void) {
    char ruta_file[512];
    obtener_ruta_bitacora(ruta_file, sizeof(ruta_file));
    
    FILE *f = fopen(ruta_file, "r");
    if (!f) {
        printf("\nLa bitácora está vacía o aún no se ha generado ningún evento.\n");
        return;
    }
    
    printf("\n========= BITÁCORA DEL SISTEMA =========\n");
    char linea[512];
    while (fgets(linea, sizeof(linea), f) != NULL) {
        printf("%s", linea);
    }
    printf("=======================================\n");
    fclose(f);
}

void limpiar_pantalla(void) {
    printf("\033[H\033[2J");
    fflush(stdout);
}

void pausar_consola(void) {
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    getchar();
}
