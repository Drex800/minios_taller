#include <stdio.h>
#include <stdlib.h>

int main() {
    int opcion = 0;

    while (1) {
        // Formato visual del menú principal
        printf("_______________________________________________________ \n");
        printf("   MINI SISTEMA OPERATIVO (CLI)\n");
        printf("_______________________________________________________ \n");
        printf("--- GESTIÓN DE MEMORIA SECUNDARIA (CRUD) ---\n");
        printf("1. Create\n");
        printf("2. Read\n");
        printf("3. Update\n");
        printf("4. Delete\n");
        printf("--- GESTIÓN DE PROCESOS ---\n");
        printf("5. Iniciar Reto BSDGames\n");
        printf("--- CONTROL DEL SISTEMA ---\n");
        printf("6. Apagar Sistema\n");
        printf("_______________________________________________________ \n");
        printf("Seleccione una opción (1-6): ");

        // Captura de opción
        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n'); // Limpia el buffer en caso de error
            printf("Opción no válida.\n");
            continue;
        }

        // Mensajes simulados de ejecución
        switch (opcion) {
            case 1:
                printf("Ejecutando Create...\n");
                break;
            case 2:
                printf("Ejecutando Read...\n");
                break;
            case 3:
                printf("Ejecutando Update...\n");
                break;
            case 4:
                printf("Ejecutando Delete...\n");
                break;
            case 5:
                printf("Ejecutando Iniciar Reto BSDGames ...\n");
                break;
            case 6:
                printf("Apagando Sistema...\n");
                return 0; // Finaliza la ejecución de forma segura
            default:
                printf("Opción no válida. Intente de nuevo.\n");
                break;
        }
    }

    return 0;
}