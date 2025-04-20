#include "servidor.h"

// Manejador de señales para limpiar procesos hijo
void signal_handler(int signum) {
    if (signum == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
        
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr;

    // Manejo de señales
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
    // Configura opciones adicionales:
    // - SA_RESTART: Reinicia llamadas bloqueantes en algunas señales
    // - SA_NOCLDSTOP: Evita recibir señales SIGCHLD cuando los hijos se detienen
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creando el socket del servidor\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al asociar el socket");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 10) < 0) {
        perror("Error al escuchar conexiones");
        close(server_socket);
        return 1;
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Error aceptando conexión");
            continue;
        }
        pid_t pid=fork();
        if (pid == 0) {
            // Proceso hijo
            close(server_socket);
            printf("Cliente conectado. PID del proceso hijo: %d\n", getpid());
            // Preparar argumentos para execv
            char client_fd_str[10];
            snprintf(client_fd_str, sizeof(client_fd_str), "%d", client_socket);

            char *args[] = {"./logica_clientes", client_fd_str, NULL};

            execv(args[0], args);

            // Si execv falla
            perror("Error ejecutando logica de cliente");
            close(client_socket);
            exit(1);
        }
        printf("Conexión con cliente cerrada. PID: %d\n", pid);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
