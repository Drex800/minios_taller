#include "../include/logger.h"
#include <stdio.h>

void logger_init(void)
{
}

void log_event(LogLevel level, const char *mensaje)
{
    (void)level;
    printf("[LOG] %s\n", mensaje);
}

void mostrar_bitacora(void)
{
}

void limpiar_pantalla(void)
{
    printf("\033[H\033[2J");
    fflush(stdout);
}

void pausar_consola(void)
{
    printf("\nPulse ENTER para continuar...");
    getchar();
}
