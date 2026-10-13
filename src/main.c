#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "file_manager.h"
#include "games.h"

static void limpiar_pantalla_local(void) {
    printf("\033[H\033[2J");
    fflush(stdout);
}

static void pausar(void) {
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    getchar();
}

static int leer_opcion(void) {
    char buffer[64];
    int opcion = -1;
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d", &opcion) != 1) {
            return -1;
        }
    }
    return opcion;
}

int main(void) {
    int opcion = -1;
    log_init();
    log_event(LOG_INFO, "MiniOS v2.0 iniciado");

    while (1) {
        limpiar_pantalla_local();
        printf("========= MINI OS v2.0 =========\n");
        printf("1. Gestor de Archivos\n");
        printf("2. Centro de Juegos\n");
        printf("3. Ver Bitácora del Sistema\n");
        printf("0. Apagar Sistema\n");
        printf("=================================\n");
        printf("Seleccione una opción: ");
        fflush(stdout);

        opcion = leer_opcion();

        switch (opcion) {
            case 1:
                log_event(LOG_INFO, "Ingreso a Gestor de Archivos");
                menu_gestor_archivos();
                break;
            case 2:
                log_event(LOG_INFO, "Ingreso a Centro de Juegos");
                menu_centro_juegos();
                break;
            case 3:
                limpiar_pantalla_local();
                log_event(LOG_INFO, "Consulta de Bitácora");
                logger_mostrar();
                pausar();
                break;
            case 0:
                limpiar_pantalla_local();
                log_event(LOG_INFO, "MiniOS v2.0 apagado");
                printf("Apagando el sistema...\n¡Hasta luego!\n");
                return 0;
            default:
                printf("\n[ERROR] Opción inválida.\n");
                pausar();
                break;
        }
    }
    return 0;
}
