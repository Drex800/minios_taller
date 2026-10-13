#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdbool.h>
#include <sys/types.h>

#define BUFFER_SIZE 4096

void menu_gestor_archivos(void);

void fm_listar_directorio(const char *subruta);
void fm_crear_archivo_texto(const char *subruta);
void fm_crear_directorio(const char *subruta);
void fm_leer_archivo(const char *subruta);
void fm_editar_archivo(const char *subruta);
void fm_copiar_archivo(const char *origen, const char *destino);
void fm_renombrar_mover(const char *origen, const char *destino);
void fm_cambiar_permisos(const char *subruta, mode_t modo);
void fm_buscar_archivos(const char *patron);
void fm_eliminar(const char *subruta);

bool es_ruta_segura(const char *ruta_relativa, char *ruta_absoluta_out);

#endif
