#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int handle_tcp_udp(const char *ip, const char *port, const char *weight) {
    printf("Handling TCP/UDP request for IP: %s, Port: %s", ip, port);
    if (weight) {
        printf(", Weight: %s\n", weight);
    } else {
        printf("\n");
    }
    //Implementar la lógica para manejar las solicitudes TCP/UDP
    return 0;
}

int handle_http(const char *ip, const char *port, const char *weight, const char *type, const char *path) {
    printf("Handling HTTP request for IP: %s, Port: %s, Type: %s", ip, port, type);
    if (weight) {
        printf(", Weight: %s", weight);
    }
    if (path) {
        printf(", Path: %s\n", path);
    } else {
        printf("\n");
    }
    //Implementar la lógica para manejar las solicitudes HTTP
    return 0;
}

int main(int argc, char *argv[]) {
    
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

    // Obtener el modo (TCP, UDP, HTTP)
    cJSON *mode = cJSON_GetObjectItemCaseSensitive(json, "mode"); 
    if (!cJSON_IsString(mode) || (mode->valuestring == NULL)) {
        printf("Error: No se encontró el modo en la configuración.\n");
        cJSON_Delete(json);
        return 1;
    }

    // Procesar según el modo
    if (strcmp(mode->valuestring, "tcp") == 0 || strcmp(mode->valuestring, "udp") == 0) {
        cJSON *tcp_udp = cJSON_GetObjectItemCaseSensitive(json, "tcp");
        if (tcp_udp == NULL) {
            printf("Error: No se encontró la configuración TCP/UDP.\n");
            cJSON_Delete(json);
            return 1;
        }

        cJSON *ports = cJSON_GetObjectItemCaseSensitive(tcp_udp, "ports");
        if (ports == NULL) {
            printf("Error: No se encontraron los puertos TCP/UDP.\n");
            cJSON_Delete(json);
            return 1;
        }

        cJSON *port;
        cJSON_ArrayForEach(port, ports) {
            const char *port_number = port->string;
            cJSON *port_content = cJSON_GetObjectItemCaseSensitive(ports, port_number);
            if (port_content == NULL || !cJSON_IsArray(port_content)) {
                printf("Error: La configuración del puerto %s no es válida.\n", port_number);
                continue;
            }

            cJSON *port_data;
            cJSON_ArrayForEach(port_data, port_content) {
                const char *server_ip = cJSON_GetObjectItemCaseSensitive(port_data, "ip")->valuestring;
                const char *server_port = cJSON_GetObjectItemCaseSensitive(port_data, "port")->valuestring;
                const char *server_weight = cJSON_GetObjectItemCaseSensitive(port_data, "weight") ? cJSON_GetObjectItemCaseSensitive(port_data, "weight")->valuestring : NULL;
                
                // Revisar si hay variables de entorno
                if (server_ip[0] == '$') server_ip = getenv(server_ip + 1);
                if (server_port[0] == '$') server_port = getenv(server_port + 1);
                if (server_weight && server_weight[0] == '$') server_weight = getenv(server_weight + 1);
                
                if (handle_tcp_udp(server_ip, server_port, server_weight) == 1) {
                    printf("Error: Falló el manejo de la solicitud para el servidor con IP: %s y puerto: %s\n", server_ip, server_port);
                    cJSON_Delete(json);
                    return 1;
                }
            }
        }
    } else if (strcmp(mode->valuestring, "http") == 0) {
        cJSON *http = cJSON_GetObjectItemCaseSensitive(json, "http");
        if (http == NULL) {
            printf("Error: No se encontró la configuración HTTP.\n");
            cJSON_Delete(json);
            return 1;
        }

        cJSON *ports = cJSON_GetObjectItemCaseSensitive(http, "ports");
        if (ports == NULL) {
            printf("Error: No se encontraron los puertos HTTP.\n");
            cJSON_Delete(json);
            return 1;
        }

        cJSON *port;
        cJSON_ArrayForEach(port, ports) {
            const char *port_number = port->string;
            cJSON *port_content = cJSON_GetObjectItemCaseSensitive(ports, port_number);
            if (port_content == NULL || !cJSON_IsObject(port_content)) {
                printf("Error: La configuración del puerto %s no es válida.\n", port_number);
                continue;
            }

            cJSON *hostname;
            cJSON_ArrayForEach(hostname, port_content) {
                const char *hostname_str = hostname->string;
                cJSON *hostname_content = cJSON_GetObjectItemCaseSensitive(port_content, hostname_str);
                if (hostname_content == NULL || !cJSON_IsArray(hostname_content)) {
                    printf("Error: La configuración del hostname %s en el puerto %s no es válida.\n", hostname_str, port_number);
                    continue;
                }

                cJSON *hostname_data;
                cJSON_ArrayForEach(hostname_data, hostname_content) {
                    const char *server_ip = cJSON_GetObjectItemCaseSensitive(hostname_data, "ip")->valuestring;
                    const char *server_port = cJSON_GetObjectItemCaseSensitive(hostname_data, "port")->valuestring;
                    const char *server_weight = cJSON_GetObjectItemCaseSensitive(hostname_data, "weight") ? cJSON_GetObjectItemCaseSensitive(hostname_data, "weight")->valuestring : NULL;
                    const char *server_type = cJSON_GetObjectItemCaseSensitive(hostname_data, "type")->valuestring;
                    const char *server_path = cJSON_GetObjectItemCaseSensitive(hostname_data, "path") ? cJSON_GetObjectItemCaseSensitive(hostname_data, "path")->valuestring : NULL;
                    
                    // Revisar si hay variables de entorno
                    if (server_ip[0] == '$') server_ip = getenv(server_ip + 1);
                    if (server_port[0] == '$') server_port = getenv(server_port + 1);
                    if (server_weight && server_weight[0] == '$') server_weight = getenv(server_weight + 1);
                    if (server_type[0] == '$') server_type = getenv(server_type + 1);
                    if (server_path && server_path[0] == '$') server_path = getenv(server_path + 1);
                    
                    if (handle_http(server_ip, server_port, server_weight, server_type, server_path) == 1) {
                        printf("Error: Falló el manejo de la solicitud HTTP para el servidor con IP: %s y puerto: %s\n", server_ip, server_port);
                        cJSON_Delete(json);
                        return 1;
                    }
                }
            }
        }
    } else {
        printf("Error: El modo seleccionado (%s) no es compatible.\n", mode->valuestring);
        cJSON_Delete(json);
        return 1;
    }

    // Eliminar JSON
    cJSON_Delete(json);
    return 0;
}

