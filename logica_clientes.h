#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
const char *get_mime_type(const char *path);
void handle_client(int client_socket);