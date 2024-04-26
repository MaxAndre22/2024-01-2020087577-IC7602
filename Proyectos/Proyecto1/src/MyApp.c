#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>

#define CONFIG_FILE "config.json"

// Prototipos de funciones
int modeTCP(cJSON *json);
int modeUDP(cJSON *json);
int modeHTTP(cJSON *json);
int servidorTcpUdp(const char *ip, const char *port, const char *weight);
int servidorHttp(const char *ip, const char *port, const char *weight, const char *type, const char *path);

int main(int argc, char *argv[]) {
    // Abrir archivo "config.json"
    FILE *fp = fopen(CONFIG_FILE, "r");
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

    // Dependiendo del mode, la funcion que se utiliza cambia
    cJSON *mode = cJSON_GetObjectItemCaseSensitive(json, "mode"); 
    if (mode == NULL) {
        printf("Error: No se encontró la clave 'mode' en el archivo JSON.\n");
        cJSON_Delete(json);
        return 1;
    }

    printf("Mode: %s\n", mode->valuestring);

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
        printf("Error: El mode seleccionado (%s) no existe.\n", mode->valuestring);
        cJSON_Delete(json);
        return 1;
    }

    // Eliminar JSON 
    cJSON_Delete(json); 
    return 0; 
}

int modeTCP(cJSON *json) {
    // Implementación para TCP
    return 0;
}

int modeUDP(cJSON *json) {
    // Implementación para UDP
    cJSON *udp = cJSON_GetObjectItemCaseSensitive(json, "udp");
    if (udp == NULL) {
        printf("Error: No se encontró la configuración UDP.\n");
        return 1;
    }

    cJSON *ports = cJSON_GetObjectItemCaseSensitive(udp, "ports");
    if (ports == NULL) {
        printf("Error: No se encontraron los puertos UDP.\n");
        return 1;
    }

    printf("Puertos UDP:\n");

    cJSON *port; // Puerto que se está iterando en la lista de puertos

    const char *portNumber; // Número de puerto que se está leyendo
    cJSON *portContent; // Contenido del puerto, contiene IP, Port y Weight

    cJSON *portData; // Dato que se está iterando en la lista de elementos de un puerto

    cJSON *weightExists; // Validación de la existencia del elemento weight

    const char *serverIp; // IP del servidor dentro de un puerto N
    const char *serverPort; // Puerto del servidor dentro de un puerto N
    const char *serverWeight; // Weight del servidor dentro de un puerto N

    // Loops para extraer los datos de los puertos
    cJSON_ArrayForEach(port, ports) {
        portNumber = port->string;
        portContent = cJSON_GetObjectItemCaseSensitive(ports, portNumber);

        printf("  Puerto: %s\n", portNumber);

        if (portContent == NULL || !cJSON_IsArray(portContent)) {
            printf("  PORT CONTENT: NULL or not an array\n");
            continue;
        }

        cJSON_ArrayForEach(portData, portContent) {

            if (!cJSON_IsObject(portData)) {
                printf("  PORT DATA: NULL or not an object\n");
                continue;
            }

            serverIp = cJSON_GetObjectItemCaseSensitive(portData, "ip")->valuestring;
            serverPort = cJSON_GetObjectItemCaseSensitive(portData, "port")->valuestring;
            weightExists = cJSON_GetObjectItemCaseSensitive(portData, "weight");
            if (weightExists && cJSON_IsString(weightExists)) {
                serverWeight = weightExists->valuestring;
            } else {
                serverWeight = NULL;
            }

            printf("    IP: %s, Port: %s", serverIp, serverPort);
            if (serverWeight) {
                printf(", Weight: %s", serverWeight);
            }
            printf("\n");

            if (servidorTcpUdp(serverIp, serverPort, serverWeight) == 1) {
                printf("Error: El servidor con el IP: %s y Port: %s falló.\n", serverIp, serverPort);
                return 1;
            }
        }
    }

    return 0;
}

int modeHTTP(cJSON *json) {
    // Implementación para HTTP
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

    printf("Puertos HTTP:\n");

    cJSON *port; // Puerto que se está iterando en la lista de puertos

    const char *portNumber; // Número de puerto que se está leyendo
    cJSON *portContent; // Contenido del puerto, contiene hostnames

    cJSON *dns; // DNS que se está iterando en la lista de hostnames del puerto
    cJSON *hostname; // Hostname que se está leyendo

    cJSON *hostnameContent; // Contenido del hostname: contiene IP, Port, Weight, Type y Path
    cJSON *hostnameData; // Dato que se está iterando en la lista de elementos de un DNS

    cJSON *weightExists; // Validación de la existencia del elemento weight

    const char *serverIp; // IP del servidor dentro de un hostname
    const char *serverPort; // Puerto del servidor dentro de un hostname
    const char *serverWeight; // Weight del servidor dentro de un hostname
    const char *serverType; // Tipo de servidor dentro de un hostname
    const char *serverPath; // Path de servidor dentro de un hostname, depende del serverType

    // Loops para extraer los datos de los puertos
    cJSON_ArrayForEach(port, ports) {
        portNumber = port->string;
        portContent = cJSON_GetObjectItemCaseSensitive(ports, portNumber);

        printf("  Puerto: %s\n", portNumber);

        if (portContent == NULL) {
            printf("  PORT CONTENT: NULL or not an array\n");
            continue;
        }

        cJSON_ArrayForEach(dns, portContent) {
            hostname = dns->string;
            hostnameContent = cJSON_GetObjectItemCaseSensitive(portContent, hostname);

            printf("    Hostname: %s\n", hostname);

            if (hostnameContent == NULL || !cJSON_IsArray(hostnameContent)) {
                printf("    HOSTNAME: NULL or not an array\n");
                continue;
            }

            cJSON_ArrayForEach(hostnameData, hostnameContent) {

                if (!cJSON_IsObject(hostnameData)) {
                    printf("    HOSTNAME DATA: NULL or not an object\n");
                    continue;
                }

                serverIp = cJSON_GetObjectItemCaseSensitive(hostnameData, "ip")->valuestring;
                serverPort = cJSON_GetObjectItemCaseSensitive(hostnameData, "port")->valuestring;
                weightExists = cJSON_GetObjectItemCaseSensitive(hostnameData, "weight");
                if (weightExists && cJSON_IsString(weightExists)) {
                    serverWeight = weightExists->valuestring;
                } else {
                    serverWeight = NULL;
                }
                serverType = cJSON_GetObjectItemCaseSensitive(hostnameData, "type")->valuestring;
                if (strcmp(serverType, "path") == 0) {
                    serverPath = cJSON_GetObjectItemCaseSensitive(hostnameData, "path")->valuestring;;
                } else {
                    serverPath = NULL;
                }

                printf("      IP: %s, Port: %s, Type: %s", serverIp, serverPort, serverType);
                if (serverWeight) {
                    printf(", Weight: %s", serverWeight);
                }
                if (serverPath) {
                    printf(", Path: %s", serverPath);
                }
                printf("\n");

                if (servidorHttp(serverIp, serverPort, serverWeight, serverType, serverPath) == 1) {
                    printf("Error: El servidor con el IP: %s y Port: %s falló.\n", serverIp, serverPort);
                    return 1;
                }
            }
        }
    }

    return 0;
}

int servidorTcpUdp(const char *ip, const char *port, const char *weight) {
    printf("  SERVER TCP/UDP!\n");
    return 0;
}

int servidorHttp(const char *ip, const char *port, const char *weight, const char *type, const char *path) {
    printf("  SERVER HTTP!\n");
    return 0;
}


