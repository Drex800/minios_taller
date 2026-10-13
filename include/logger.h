#ifndef LOGGER_H
#define LOGGER_H

#define DISK_PATH "./disk"
#define LOG_FILE "./disk/system.log"

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

void logger_init(void);
void log_event(LogLevel level, const char *mensaje);
void mostrar_bitacora(void);
void limpiar_pantalla(void);
void pausar_consola(void);

#endif
