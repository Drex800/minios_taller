#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "file_manager.h"
#include "games.h"

int main(void) {
    logger_init();
    log_event(LOG_INFO, "Sistema iniciado.");

    char opcion_str[64];
    int opcion = -1;

    while (1) {
        limpiar_pantalla();
        printf("_______________________________________________________ \n");
        printf("               MINI SISTEMA OPERATIVO v2.0              \n");
        printf("_______________________________________________________ \n");
        printf(" 1. Gestor de Archivos (Memoria Secundaria)\n");
        printf(" 2. Centro de Juegos (Gestión de Procesos)\n");
        printf(" 3. Ver Bitácora del Sistema (Log)\n");
        printf(" 0. Apagar Sistema\n");
        printf("_______________________________________________________ \n");
        printf("Seleccione una opción (0-3): ");

        if (fgets(opcion_str, sizeof(opcion_str), stdin) != NULL) {
            if (sscanf(opcion_str, "%d", &opcion) != 1) {
                printf("Opción inválida. Ingrese un número.\n");
                pausar_consola();
                continue;
            }
        }

        switch (opcion) {
            case 1:
                menu_gestor_archivos();
                break;
            case 2:
                menu_centro_juegos();
                break;
            case 3:
                limpiar_pantalla();
                mostrar_bitacora();
                pausar_consola();
                break;
            case 0:
                printf("\nApagando Mini OS v2.0...\n¡Hasta luego!\n");
                log_event(LOG_INFO, "Sistema apagado.");
                return 0;
            default:
                printf("Opción fuera de rango.\n");
                pausar_consola();
                break;
        }
    }

    return 0;
}
