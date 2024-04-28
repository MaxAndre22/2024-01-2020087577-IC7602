#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <unistd.h>
#include <arpa/inet.h>

// Función para obtener el valor de una variable de entorno
const char *get_environment_variable(const char *var_name) {
    // Abrir el archivo de variables de entorno
    FILE *env_file = fopen("variables.env", "r");
    if (env_file == NULL) {
        printf("Error: No se pudo abrir el archivo de variables de entorno.\n");
        return NULL;
    }

    // Buscar la variable de entorno
    char line[256];
    const char *value = NULL;
    while (fgets(line, sizeof(line), env_file)) {
        // Eliminar el salto de línea del final
        line[strcspn(line, "\n")] = 0;
        char *delimiter = strchr(line, '=');
        if (delimiter != NULL) {
            *delimiter = '\0';
            if (strcmp(line, var_name) == 0) {
                value = strdup(delimiter + 1); // strdup para asignar memoria para el valor
                break;
            }
        }
    }

    fclose(env_file);
    return value;
}

// Función para manejar el modo TCP
int modeTCP(cJSON *json) {
    cJSON *tcp = cJSON_GetObjectItemCaseSensitive(json, "tcp"); 
    if (tcp == NULL) {
        printf("Error: No se encontró la configuración TCP.\n");
        return 1;
    }

    cJSON *ports = cJSON_GetObjectItemCaseSensitive(tcp, "ports");
    if (ports == NULL) {
        printf("Error: No se encontraron los puertos TCP.\n");
        return 1;
    }

    cJSON *port;
    const char *portNumber;
    cJSON *portContent;
    cJSON *portData;
    const char *serverIp;
    const char *serverPort;
    const char *serverWeight;

    // Loops para extraer los datos de los puertos
    cJSON_ArrayForEach(port, ports) {
    portNumber = port->string;
    portContent = cJSON_GetObjectItemCaseSensitive(ports, portNumber);
    printf("Port: %s\n", portNumber);

    if (portContent == NULL || !cJSON_IsArray(portContent)) {
        printf("PORT CONTENT: NULL or not an array\n");
        continue;
    }

    cJSON_ArrayForEach(portData, portContent) {
        if (!cJSON_IsObject(portData)) {
            printf("PORT DATA: NULL or not an object\n");
            continue;
        }

        serverIp = cJSON_GetObjectItemCaseSensitive(portData, "ip")->valuestring;
        serverPort = cJSON_GetObjectItemCaseSensitive(portData, "port")->valuestring;
        serverWeight = cJSON_GetObjectItemCaseSensitive(portData, "weight") != NULL ? cJSON_GetObjectItemCaseSensitive(portData, "weight")->valuestring : NULL;

        // Revisar por variable de entorno solo para serverIp
        if (serverIp[0] == '$') {
            const char *env_var = getenv(serverIp + 1);
            if (env_var != NULL) {
                serverIp = env_var;
            } else {
                printf("Error: Variable de entorno '%s' no encontrada.\n", serverIp + 1);
                continue;
            }
        }

        printf("  IP: %s, Port: %s", serverIp, serverPort);
        if (serverWeight != NULL && serverWeight[0] != '\0') {
            printf(", Weight: %s", serverWeight);
        }
        printf("\n");

        //Realizar cualquier operación adicional
        }
    }


    return 0;
}


int modeUDP(cJSON *json) {
    cJSON *udp = cJSON_GetObjectItemCaseSensitive(json, "tcp"); 
    if (udp == NULL) {
        printf("Error: No se encontró la configuración UDP.\n");
        return 1;
    }

    cJSON *ports = cJSON_GetObjectItemCaseSensitive(udp, "ports");
    if (ports == NULL) {
        printf("Error: No se encontraron los puertos UDP.\n");
        return 1;
    }

    cJSON *port;
    const char *portNumber;
    cJSON *portContent;
    cJSON *portData;
    const char *serverIp;
    const char *serverPort;
    const char *serverWeight;

    // Loops para extraer los datos de los puertos
    cJSON_ArrayForEach(port, ports) {
    portNumber = port->string;
    portContent = cJSON_GetObjectItemCaseSensitive(ports, portNumber);
    printf("Port: %s\n", portNumber);

    if (portContent == NULL || !cJSON_IsArray(portContent)) {
        printf("PORT CONTENT: NULL or not an array\n");
        continue;
    }

    cJSON_ArrayForEach(portData, portContent) {
        if (!cJSON_IsObject(portData)) {
            printf("PORT DATA: NULL or not an object\n");
            continue;
        }

        serverIp = cJSON_GetObjectItemCaseSensitive(portData, "ip")->valuestring;
        serverPort = cJSON_GetObjectItemCaseSensitive(portData, "port")->valuestring;
        serverWeight = cJSON_GetObjectItemCaseSensitive(portData, "weight") != NULL ? cJSON_GetObjectItemCaseSensitive(portData, "weight")->valuestring : NULL;

        // Revisar por variable de entorno solo para serverIp
        if (serverIp[0] == '$') {
            const char *env_var = getenv(serverIp + 1);
            if (env_var != NULL) {
                serverIp = env_var;
            } else {
                printf("Error: Variable de entorno '%s' no encontrada.\n", serverIp + 1);
                continue;
            }
        }

        printf("  IP: %s, Port: %s", serverIp, serverPort);
        if (serverWeight != NULL && serverWeight[0] != '\0') {
            printf(", Weight: %s", serverWeight);
        }
        printf("\n");

        //Realizar cualquier operación adicional
        }
    }

    return 0;
}

int modeHTTP(cJSON *json) {
    cJSON *http = cJSON_GetObjectItemCaseSensitive(json, "http"); 
    if (http == NULL) {
        printf("Error: No se encontró la configuración HTTP.\n");
        return 1;
    }

    cJSON *ports = cJSON_GetObjectItemCaseSensitive(http, "ports");
    if (ports == NULL) {
        printf("Error: No se encontraron los puertos HTTP.\n");
        return 1;
    }

    cJSON *port;
    const char *portNumber;
    cJSON *portContent;
    cJSON *dns;
    const char *hostname;
    cJSON *hostnameContent;
    cJSON *hostnameData;
    const char *serverIp;
    const char *serverPort;
    const char *serverWeight;
    const char *serverType;
    const char *serverPath;

    // Loops para extraer los datos de los puertos HTTP
    cJSON_ArrayForEach(port, ports) {
    portNumber = port->string;
    portContent = cJSON_GetObjectItemCaseSensitive(ports, portNumber);
    printf("Port: %s\n", portNumber);

    if (portContent == NULL || !cJSON_IsObject(portContent)) {
        printf("PORT CONTENT: NULL or not an object\n");
        continue;
    }

    cJSON_ArrayForEach(dns, portContent) {
        hostname = dns->string;
        hostnameContent = cJSON_GetObjectItemCaseSensitive(portContent, hostname);
        printf("Hostname: %s\n", hostname);

        if (hostnameContent == NULL || !cJSON_IsArray(hostnameContent)) {
            printf("HOSTNAME: NULL or not an array\n");
            continue;
        }

        cJSON_ArrayForEach(hostnameData, hostnameContent) {
            if (!cJSON_IsObject(hostnameData)) {
                printf("HOSTNAME DATA: NULL or not an object\n");
                continue;
            }

            serverIp = cJSON_GetObjectItemCaseSensitive(hostnameData, "ip")->valuestring;
            serverPort = cJSON_GetObjectItemCaseSensitive(hostnameData, "port")->valuestring;
            serverWeight = cJSON_GetObjectItemCaseSensitive(hostnameData, "weight") != NULL ? cJSON_GetObjectItemCaseSensitive(hostnameData, "weight")->valuestring : NULL;
            serverType = cJSON_GetObjectItemCaseSensitive(hostnameData, "type")->valuestring;
            serverPath = cJSON_GetObjectItemCaseSensitive(hostnameData, "path") != NULL ? cJSON_GetObjectItemCaseSensitive(hostnameData, "path")->valuestring : NULL;

            // Revisar por variable de entorno solo para serverIp
            if (serverIp[0] == '$') {
                const char *env_var = getenv(serverIp + 1);
                if (env_var != NULL) {
                    serverIp = env_var;
                } else {
                    printf("Error: Variable de entorno '%s' no encontrada.\n", serverIp + 1);
                    continue;
                }
            }

            printf("  IP: %s, Port: %s, Type: %s", serverIp, serverPort, serverType);
            if (serverWeight != NULL) {
                printf(", Weight: %s", serverWeight);
            }
            if (serverPath != NULL) {
                printf(", Path: %s", serverPath);
            }
            printf("\n");

            //Realizar cualquier operación adicional que necesites con los datos del servidor
            }
        }
    }


    return 0;
}


int main() {
    // Abrir archivo "config.json"
    FILE *fp = fopen("config.json", "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo.\n");
        return 1;
    }

    // Guardar contenido de "config.json" en un buffer
    char buffer[4000];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    // Parsear buffer
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        printf("Error: Ocurrió un error al parsear el archivo de configuración.\n");
        cJSON_Delete(json);
        return 1; 
    }

    // Obtener el modo del JSON
    cJSON *mode = cJSON_GetObjectItemCaseSensitive(json, "mode"); 
    if (mode == NULL || !cJSON_IsString(mode)) {
        printf("Error: No se encontró el modo o no es una cadena válida.\n");
        cJSON_Delete(json);
        return 1;
    }

    // Imprimir el modo
    printf("Mode: %s\n", mode->valuestring);

    // Llamar a la función correspondiente según el modo
    if (strcmp(mode->valuestring, "tcp") == 0) {
        if (modeTCP(json) == 1) {
            printf("Error: Algo falló en la lectura TCP.\n");
            cJSON_Delete(json);
            return 1;
        }
    } else if (strcmp(mode->valuestring, "udp") == 0) {
        if (modeUDP(json) == 1) {
            printf("Error: Algo falló en la lectura UDP.\n");
            cJSON_Delete(json);
            return 1;
        }
    } else if (strcmp(mode->valuestring, "http") == 0) {
        if (modeHTTP(json) == 1) {
            printf("Error: Algo falló en la lectura HTTP.\n");
            cJSON_Delete(json);
            return 1;
        }
    } else {
        printf("Error: El modo seleccionado (%s) no existe.\n", mode->valuestring);
        cJSON_Delete(json);
        return 1;
    }

    // Eliminar JSON 
    cJSON_Delete(json); 
    return 0; 
}
#define BUFFER_SIZE 1024
void forwardData(int port, const char *hostname, const char *ip, int destPort) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    // Accept incoming connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    while (1) {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        }

        // Connect to destination IP and port
        int dest_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (dest_socket == -1) {
            perror("Error creating destination socket");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(destPort);
        inet_pton(AF_INET, ip, &dest_addr.sin_addr);

        if (connect(dest_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            perror("Error connecting to destination");
            exit(EXIT_FAILURE);
        }

        // Forward received data to destination
        if (send(dest_socket, buffer, bytes_received, 0) == -1) {
            perror("Error sending data to destination");
            exit(EXIT_FAILURE);
        }

        close(dest_socket);
    }

    close(client_socket);
    close(server_socket);
}

