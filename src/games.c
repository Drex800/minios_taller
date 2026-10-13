#include "games.h"
#include "logger.h"
#include <stdio.h>

void menu_centro_juegos(void) {
    limpiar_pantalla();
    printf("=======================================================\n");
    printf("            CENTRO DE JUEGOS (EN DESARROLLO)           \n");
    printf("=======================================================\n");
    printf(" Módulo asignado al Integrante 3.\n");
    log_event(LOG_INFO, "Acceso al módulo de juegos.");
    pausar_consola();
}

void ejecutar_juego(const char *nombre_ejecutable) {
    (void)nombre_ejecutable;
}
