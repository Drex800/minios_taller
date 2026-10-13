#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

void log_init(void);
void logger_init(void); // Alias de compatibilidad
void log_event(LogLevel level, const char *mensaje);
void logger_mostrar(void);

void limpiar_pantalla(void);
void pausar_consola(void);

#endif
