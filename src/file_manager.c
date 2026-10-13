#define _XOPEN_SOURCE 700
#include "file_manager.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>

static void leer_linea(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

bool es_ruta_segura(const char *ruta_relativa, char *ruta_absoluta_out) {
    char base_abs[2048];
    if (realpath(DISK_PATH, base_abs) == NULL) {
        logger_init();
        if (realpath(DISK_PATH, base_abs) == NULL) {
            log_event(LOG_ERROR, "No se pudo obtener la ruta absoluta del disco.");
            return false;
        }
    }

    char target_path[4096];
    if (ruta_relativa == NULL || strlen(ruta_relativa) == 0 || strcmp(ruta_relativa, ".") == 0) {
        snprintf(target_path, sizeof(target_path), "%s", base_abs);
    } else if (ruta_relativa[0] == '/') {
        snprintf(target_path, sizeof(target_path), "%s%s", base_abs, ruta_relativa);
    } else {
        snprintf(target_path, sizeof(target_path), "%s/%s", base_abs, ruta_relativa);
    }

    char resolved_target[4096];
    if (realpath(target_path, resolved_target) != NULL) {
        size_t base_len = strlen(base_abs);
        if (strncmp(base_abs, resolved_target, base_len) == 0 &&
            (resolved_target[base_len] == '\0' || resolved_target[base_len] == '/')) {
            if (ruta_absoluta_out != NULL) {
                strcpy(ruta_absoluta_out, resolved_target);
            }
            return true;
        }
        log_event(LOG_WARNING, "Intento de Path Traversal detectado.");
        return false;
    } else {
        char tmp_path[4096];
        snprintf(tmp_path, sizeof(tmp_path), "%s", target_path);

        char *dir_name = dirname(tmp_path);
        char resolved_parent[2048];

        if (realpath(dir_name, resolved_parent) != NULL) {
            size_t base_len = strlen(base_abs);
            if (strncmp(base_abs, resolved_parent, base_len) == 0 &&
                (resolved_parent[base_len] == '\0' || resolved_parent[base_len] == '/')) {
                char tmp_base[4096];
                snprintf(tmp_base, sizeof(tmp_base), "%s", target_path);
                char *base_name = basename(tmp_base);

                int written = snprintf(resolved_target, sizeof(resolved_target), "%s/%s", resolved_parent, base_name);
                if (written > 0 && (size_t)written < sizeof(resolved_target)) {
                    if (ruta_absoluta_out != NULL) {
                        strcpy(ruta_absoluta_out, resolved_target);
                    }
                    return true;
                }
            }
        }
        log_event(LOG_WARNING, "Acceso a ruta fuera del disco denegado.");
        return false;
    }
}

void fm_listar_directorio(const char *subruta) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no permitida o no válida.\n");
        return;
    }

    DIR *dir = opendir(ruta_abs);
    if (dir == NULL) {
        perror("[Error] No se pudo abrir el directorio");
        log_event(LOG_ERROR, "Fallo al abrir directorio.");
        return;
    }

    printf("\n--- CONTENIDO DEL DIRECTORIO: %s ---\n", (subruta && strlen(subruta) > 0) ? subruta : "/");
    printf("%-25s %-10s %-12s\n", "Nombre", "Tipo", "Tamaño (Bytes)");
    printf("---------------------------------------------------\n");

    struct dirent *entrada;
    while ((entrada = readdir(dir)) != NULL) {
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", ruta_abs, entrada->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            const char *tipo = S_ISDIR(st.st_mode) ? "[DIR]" : "[FILE]";
            printf("%-25s %-10s %-12lld\n", entrada->d_name, tipo, (long long)st.st_size);
        } else {
            printf("%-25s %-10s %-12s\n", entrada->d_name, "[?]", "Desconocido");
        }
    }

    closedir(dir);
    log_event(LOG_INFO, "Directorio listado con éxito.");
}

void fm_crear_archivo_texto(const char *subruta) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no permitida o fuera del disco.\n");
        return;
    }

    int fd = open(ruta_abs, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("[Error] No se pudo crear el archivo");
        log_event(LOG_ERROR, "Error al crear archivo de texto.");
        return;
    }

    char buffer[1024];
    printf("Ingrese el texto para el archivo:\n> ");
    leer_linea(buffer, sizeof(buffer));

    size_t len = strlen(buffer);
    if (len > 0) {
        buffer[len] = '\n';
        len++;
    }

    ssize_t escritos = write(fd, buffer, len);
    close(fd);

    if (escritos == -1) {
        perror("[Error] Fallo al escribir en el archivo");
        log_event(LOG_ERROR, "Fallo al escribir en archivo.");
    } else {
        printf("[Éxito] Archivo creado correctamente.\n");
        log_event(LOG_INFO, "Archivo de texto creado con éxito.");
    }
}

void fm_crear_directorio(const char *subruta) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no permitida o fuera del disco.\n");
        return;
    }

    if (mkdir(ruta_abs, 0755) == -1) {
        perror("[Error] No se pudo crear el directorio");
        log_event(LOG_ERROR, "Fallo al crear directorio.");
    } else {
        printf("[Éxito] Directorio creado correctamente.\n");
        log_event(LOG_INFO, "Directorio creado con éxito.");
    }
}

void fm_leer_archivo(const char *subruta) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no permitida o no existe.\n");
        return;
    }

    struct stat st;
    if (stat(ruta_abs, &st) == -1) {
        perror("[Error] No se pudo obtener metadatos del archivo");
        log_event(LOG_ERROR, "Fallo en stat de archivo para lectura.");
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        printf("[Error] La ruta especificada es un directorio, no un archivo.\n");
        return;
    }

    int fd = open(ruta_abs, O_RDONLY);
    if (fd == -1) {
        perror("[Error] No se pudo abrir el archivo para lectura");
        log_event(LOG_ERROR, "Fallo al abrir archivo para lectura.");
        return;
    }

    unsigned char buffer[BUFFER_SIZE];
    ssize_t leidos = read(fd, buffer, sizeof(buffer));

    if (leidos == -1) {
        perror("[Error] Error al leer el archivo");
        close(fd);
        log_event(LOG_ERROR, "Error de lectura de archivo.");
        return;
    }

    bool es_binario = false;
    for (ssize_t i = 0; i < leidos; i++) {
        if (!isprint(buffer[i]) && buffer[i] != '\n' && buffer[i] != '\r' && buffer[i] != '\t') {
            es_binario = true;
            break;
        }
    }

    if (es_binario) {
        printf("\n[Aviso] El archivo contiene datos BINARIOS.\n");
        printf("--- METADATOS ---\n");
        printf("Tamaño: %lld bytes\n", (long long)st.st_size);
        printf("Permisos octal: %o\n", st.st_mode & 0777);
        printf("--- PRIMEROS 64 BYTES (HEX) ---\n");

        int limite = leidos < 64 ? (int)leidos : 64;
        for (int i = 0; i < limite; i++) {
            printf("%02X ", buffer[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n-------------------------------\n");
        close(fd);
    } else {
        printf("\n--- CONTENIDO DE '%s' ---\n", subruta);
        ssize_t written = write(STDOUT_FILENO, buffer, leidos);
        (void)written;

        while ((leidos = read(fd, buffer, sizeof(buffer))) > 0) {
            written = write(STDOUT_FILENO, buffer, leidos);
            (void)written;
        }
        printf("\n-------------------------------\n");
        close(fd);
    }

    log_event(LOG_INFO, "Archivo leído con éxito.");
}

void fm_editar_archivo(const char *subruta) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no permitida o no existe.\n");
        return;
    }

    int fd = open(ruta_abs, O_WRONLY | O_APPEND);
    if (fd == -1) {
        perror("[Error] No se pudo abrir el archivo para edición");
        log_event(LOG_ERROR, "Fallo al abrir archivo para editar.");
        return;
    }

    char buffer[1024];
    printf("Ingrese el texto a añadir al final del archivo:\n> ");
    leer_linea(buffer, sizeof(buffer));

    size_t len = strlen(buffer);
    if (len > 0) {
        buffer[len] = '\n';
        len++;
    }

    ssize_t escritos = write(fd, buffer, len);
    close(fd);

    if (escritos == -1) {
        perror("[Error] Fallo al escribir en el archivo");
        log_event(LOG_ERROR, "Error en la edición de archivo.");
    } else {
        printf("[Éxito] Texto añadido correctamente.\n");
        log_event(LOG_INFO, "Archivo editado/actualizado con éxito.");
    }
}

void fm_copiar_archivo(const char *origen, const char *destino) {
    char src_abs[2048], dst_abs[2048];

    if (!es_ruta_segura(origen, src_abs)) {
        printf("[Error de Seguridad] El archivo de origen no existe o está fuera del disco.\n");
        return;
    }

    if (!es_ruta_segura(destino, dst_abs)) {
        printf("[Error de Seguridad] La ruta de destino está fuera del disco.\n");
        return;
    }

    int fd_src = open(src_abs, O_RDONLY);
    if (fd_src == -1) {
        perror("[Error] No se pudo abrir el archivo origen");
        log_event(LOG_ERROR, "Error al abrir origen para copiar.");
        return;
    }

    int fd_dst = open(dst_abs, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst == -1) {
        perror("[Error] No se pudo crear/abrir el archivo destino");
        close(fd_src);
        log_event(LOG_ERROR, "Error al abrir destino para copiar.");
        return;
    }

    unsigned char buf_src[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    bool error_copia = false;

    while ((bytes_read = read(fd_src, buf_src, sizeof(buf_src))) > 0) {
        bytes_written = write(fd_dst, buf_src, bytes_read);
        if (bytes_written != bytes_read) {
            perror("[Error] Error al escribir en destino durante la copia");
            error_copia = true;
            break;
        }
    }

    close(fd_src);
    close(fd_dst);

    if (error_copia || bytes_read == -1) {
        log_event(LOG_ERROR, "Copia de archivo incompleta o fallida.");
        printf("[Error] Ocurrió un problema durante la copia.\n");
        return;
    }

    fd_src = open(src_abs, O_RDONLY);
    fd_dst = open(dst_abs, O_RDONLY);

    if (fd_src == -1 || fd_dst == -1) {
        if (fd_src != -1) close(fd_src);
        if (fd_dst != -1) close(fd_dst);
        printf("[Error] No se pudo verificar la copia.\n");
        return;
    }

    unsigned char buf_dst[BUFFER_SIZE];
    bool es_identico = true;

    while (1) {
        ssize_t r1 = read(fd_src, buf_src, sizeof(buf_src));
        ssize_t r2 = read(fd_dst, buf_dst, sizeof(buf_dst));

        if (r1 != r2 || (r1 > 0 && memcmp(buf_src, buf_dst, r1) != 0)) {
            es_identico = false;
            break;
        }

        if (r1 <= 0 && r2 <= 0) {
            break;
        }
    }

    close(fd_src);
    close(fd_dst);

    if (es_identico) {
        printf("[Éxito] Archivo copiado y verificado: ¡Idéntico byte a byte!\n");
        log_event(LOG_INFO, "Copia de archivo realizada y verificada correctamente.");
    } else {
        printf("[Advertencia] El archivo se copió pero difiere en el contenido.\n");
        log_event(LOG_WARNING, "Copia realizada pero con discrepancia en verificación.");
    }
}

void fm_renombrar_mover(const char *origen, const char *destino) {
    char src_abs[2048], dst_abs[2048];

    if (!es_ruta_segura(origen, src_abs)) {
        printf("[Error de Seguridad] Origen no válido o fuera del disco.\n");
        return;
    }

    if (!es_ruta_segura(destino, dst_abs)) {
        printf("[Error de Seguridad] Destino fuera del disco.\n");
        return;
    }

    if (rename(src_abs, dst_abs) == -1) {
        perror("[Error] No se pudo renombrar/mover");
        log_event(LOG_ERROR, "Error al renombrar o mover archivo/directorio.");
    } else {
        printf("[Éxito] Elemento renombrado o movido exitosamente.\n");
        log_event(LOG_INFO, "Elemento renombrado/movido con éxito.");
    }
}

void fm_cambiar_permisos(const char *subruta, mode_t modo) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no permitida.\n");
        return;
    }

    if (chmod(ruta_abs, modo) == -1) {
        perror("[Error] No se pudieron cambiar los permisos");
        log_event(LOG_ERROR, "Error al cambiar permisos con chmod.");
    } else {
        printf("[Éxito] Permisos cambiados correctamente a octal: %o\n", modo);
        log_event(LOG_INFO, "Permisos modificados correctamente.");
    }
}

static void buscar_recursivo(const char *dir_path, const char *patron, int *coincidencias) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) return;

    struct dirent *entrada;
    while ((entrada = readdir(dir)) != NULL) {
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entrada->d_name);

        if (strstr(entrada->d_name, patron) != NULL) {
            printf(" - %s\n", full_path);
            (*coincidencias)++;
        }

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            buscar_recursivo(full_path, patron, coincidencias);
        }
    }
    closedir(dir);
}

void fm_buscar_archivos(const char *patron) {
    if (patron == NULL || strlen(patron) == 0) {
        printf("[Error] El patrón de búsqueda no puede estar vacío.\n");
        return;
    }

    char base_abs[2048];
    if (!es_ruta_segura("", base_abs)) {
        printf("[Error] No se puede acceder a la raíz del disco.\n");
        return;
    }

    printf("\n--- BUSCANDO ARCHIVOS QUE CONTENGAN '%s' ---\n", patron);
    int coincidencias = 0;
    buscar_recursivo(base_abs, patron, &coincidencias);

    if (coincidencias == 0) {
        printf("No se encontraron coincidencias.\n");
    } else {
        printf("Total de coincidencias encontradas: %d\n", coincidencias);
    }
    log_event(LOG_INFO, "Búsqueda de archivos realizada.");
}

void fm_eliminar(const char *subruta) {
    char ruta_abs[2048];
    if (!es_ruta_segura(subruta, ruta_abs)) {
        printf("[Error de Seguridad] Ruta no válida o fuera del disco.\n");
        return;
    }

    struct stat st;
    if (stat(ruta_abs, &st) == -1) {
        perror("[Error] No se puede acceder al elemento a eliminar");
        log_event(LOG_ERROR, "Error en stat al eliminar.");
        return;
    }

    int res;
    if (S_ISDIR(st.st_mode)) {
        res = rmdir(ruta_abs);
    } else {
        res = unlink(ruta_abs);
    }

    if (res == -1) {
        perror("[Error] No se pudo eliminar el elemento");
        log_event(LOG_ERROR, "Fallo al eliminar archivo/directorio.");
    } else {
        printf("[Éxito] Elemento eliminado correctamente.\n");
        log_event(LOG_INFO, "Elemento eliminado del disco.");
    }
}

void menu_gestor_archivos(void) {
    char opcion_str[64];
    int opcion = -1;

    while (1) {
        limpiar_pantalla();
        printf("=======================================================\n");
        printf("       GESTOR DE ARCHIVOS (MEMORIA SECUNDARIA)         \n");
        printf("=======================================================\n");
        printf(" 1. Listar directorio\n");
        printf(" 2. Crear archivo de texto\n");
        printf(" 3. Crear directorio\n");
        printf(" 4. Leer archivo\n");
        printf(" 5. Editar / Añadir a archivo\n");
        printf(" 6. Copiar archivo (con verificación byte a byte)\n");
        printf(" 7. Renombrar / Mover archivo o directorio\n");
        printf(" 8. Cambiar permisos (chmod)\n");
        printf(" 9. Buscar archivos\n");
        printf("10. Eliminar archivo o directorio\n");
        printf(" 0. Volver al Menú Principal\n");
        printf("=======================================================\n");
        printf("Seleccione una opción (0-10): ");

        leer_linea(opcion_str, sizeof(opcion_str));
        if (sscanf(opcion_str, "%d", &opcion) != 1) {
            printf("Opción inválida. Ingrese un número.\n");
            pausar_consola();
            continue;
        }

        if (opcion == 0) {
            break;
        }

        char arg1[512] = {0};
        char arg2[512] = {0};

        switch (opcion) {
            case 1:
                printf("Ingrese subruta a listar (vacío para raíz del disco): ");
                leer_linea(arg1, sizeof(arg1));
                fm_listar_directorio(arg1);
                break;
            case 2:
                printf("Ingrese nombre/ruta del nuevo archivo: ");
                leer_linea(arg1, sizeof(arg1));
                fm_crear_archivo_texto(arg1);
                break;
            case 3:
                printf("Ingrese nombre/ruta del nuevo directorio: ");
                leer_linea(arg1, sizeof(arg1));
                fm_crear_directorio(arg1);
                break;
            case 4:
                printf("Ingrese nombre/ruta del archivo a leer: ");
                leer_linea(arg1, sizeof(arg1));
                fm_leer_archivo(arg1);
                break;
            case 5:
                printf("Ingrese nombre/ruta del archivo a editar: ");
                leer_linea(arg1, sizeof(arg1));
                fm_editar_archivo(arg1);
                break;
            case 6:
                printf("Ingrese ruta de origen: ");
                leer_linea(arg1, sizeof(arg1));
                printf("Ingrese ruta de destino: ");
                leer_linea(arg2, sizeof(arg2));
                fm_copiar_archivo(arg1, arg2);
                break;
            case 7:
                printf("Ingrese ruta origen: ");
                leer_linea(arg1, sizeof(arg1));
                printf("Ingrese ruta destino: ");
                leer_linea(arg2, sizeof(arg2));
                fm_renombrar_mover(arg1, arg2);
                break;
            case 8: {
                printf("Ingrese ruta del archivo/directorio: ");
                leer_linea(arg1, sizeof(arg1));
                printf("Ingrese modo en octal (ej. 0644, 0755): ");
                leer_linea(arg2, sizeof(arg2));
                long modo = strtol(arg2, NULL, 8);
                if (modo <= 0) {
                    printf("[Error] Modo de permisos no válido.\n");
                } else {
                    fm_cambiar_permisos(arg1, (mode_t)modo);
                }
                break;
            }
            case 9:
                printf("Ingrese texto o patrón a buscar: ");
                leer_linea(arg1, sizeof(arg1));
                fm_buscar_archivos(arg1);
                break;
            case 10:
                printf("Ingrese ruta del archivo o directorio a eliminar: ");
                leer_linea(arg1, sizeof(arg1));
                fm_eliminar(arg1);
                break;
            default:
                printf("Opción no válida.\n");
                break;
        }

        pausar_consola();
    }
}
