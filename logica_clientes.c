#include "logica_clientes.h"

const char *get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');//me guarda un puntero ala direccion del ultimo caracter especificado
    if (!ext) return "application/octet-stream";

    if (strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".pdf") == 0) return "application/pdf";
    if (strcmp(ext, ".webp") == 0) return "image/webp";
    //me compara si el contenido del puntero concido con alguno de los typos
    return "application/octet-stream";
}

void handle_client(int client_socket) {
    char buffer[1024];
    char method[16], path[256], version[16];

    read(client_socket, buffer, sizeof(buffer) - 1);
    
    // Analizar la solicitud HTTP
    sscanf(buffer, "%s %s %s", method, path, version);

    printf("Solicitud recibida: %s\n", buffer);

    if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0) {
        char full_path[512] = ".";
        strcat(full_path, path);//me combina dos cadenas para hacer referencia a esta carpeta con el punto
        
        printf("Cliente solicitó: %s\n", full_path); // Mensaje en consola

        int file = open(full_path, O_RDONLY);
        if (file != -1) {
            struct stat file_stat;
            if (stat(full_path, &file_stat) == 0) {
                const char *mime_type = get_mime_type(full_path);
                char header[128];

                snprintf(header, sizeof(header),
                         "HTTP/1.1 200 OK\r\n"
                         "Content-Type: %s\r\n"
                         "Content-Length: %ld\r\n"
                         "\r\n",
                         mime_type, file_stat.st_size);
                write(client_socket, header, strlen(header));

                if (strcmp(method, "GET") == 0) {
                    char content[1024];
                    ssize_t bytes_read;

                    // Leer el archivo y enviarlo al cliente
                    while ((bytes_read = read(file, content, sizeof(content))) > 0) {
                        write(client_socket, content, bytes_read);
                    }
                }
            }    
            close(file);
        } else {
            write(client_socket, "HTTP/1.1 404 Not Found\r\n", 24);
            write(client_socket, "Content-Type: text/html\r\n\r\n", 27);
            write(client_socket, "<html><body><h1>404 Not Found</h1></body></html>", 46);
        }
    } else {
        write(client_socket, "HTTP/1.1 501 Not Implemented\r\n", 30);
        write(client_socket, "Content-Type: text/html\r\n\r\n", 27);
        write(client_socket, "<html><body><h1>501 Not Implemented</h1></body></html>", 54);
    }
    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <client_socket>\n", argv[0]);
        return 1;
    }

    int client_socket = atoi(argv[1]);
    handle_client(client_socket);

    return 0;
}

