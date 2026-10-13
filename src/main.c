#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "file_manager.h"
#include "games.h"

static void limpiar_pantalla(void) {
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
    
    logger_init();
    logger_log("SISTEMA", "MiniOS v2.0 iniciado con éxito");

    while (1) {
        limpiar_pantalla();
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
                logger_log("NAVEGACIÓN", "Ingreso al Gestor de Archivos");
                menu_gestor_archivos();
                break;
            case 2:
                logger_log("NAVEGACIÓN", "Ingreso al Centro de Juegos");
                menu_centro_juegos();
                break;
            case 3:
                limpiar_pantalla();
                logger_log("NAVEGACIÓN", "Consulta de Bitácora del Sistema");
                logger_mostrar();
                pausar();
                break;
            case 0:
                limpiar_pantalla();
                logger_log("SISTEMA", "MiniOS v2.0 apagado de forma segura");
                printf("Apagando el sistema...\n¡Hasta luego!\n");
                return 0;
            default:
                printf("\n[ERROR] Entrada inválida. Seleccione un número dentro del rango.\n");
                logger_log("ERROR", "Opción inválida ingresada en menú principal");
                pausar();
                break;
        }
    }

    return 0;
}
