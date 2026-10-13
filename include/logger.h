#ifndef LOGGER_H
#define LOGGER_H

void logger_init(void);
void logger_log(const char *categoria, const char *mensaje);
void logger_mostrar(void);

#endif
