#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

void logger_init(void) {
    mkdir(DISK_PATH, 0755);
}

void log_event(LogLevel level, const char *mensaje) {
    logger_init();
    FILE *f = fopen(LOG_FILE, "a");
    if (!f) return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    const char *lbl = (level == LOG_INFO) ? "INFO" : (level == LOG_WARNING) ? "WARN" : "ERROR";
    fprintf(f, "[%s] [%s] %s\n", timestamp, lbl, mensaje);
    fclose(f);
}

void mostrar_bitacora(void) {
    FILE *f = fopen(LOG_FILE, "r");
    if (!f) {
        printf("No hay registros en la bitácora aún.\n");
        return;
    }
    printf("\n--- CONTENIDO DE BITÁCORA (system.log) ---\n");
    char buf[256];
    while (fgets(buf, sizeof(buf), f)) {
        printf("%s", buf);
    }
    printf("------------------------------------------\n");
    fclose(f);
}

void limpiar_pantalla(void) {
    printf("\033[H\033[2J");
    fflush(stdout);
}

void pausar_consola(void) {
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
