
2020087577 - Agrazal Quirós Máximo
2020207809 - Araya Sambucci Daniel
2019157553 - Arias Siles Andrés
2020425920 - Casasola Mayorga Manuel
2020041924 - Hernández Calvo David
2020096838 - Quesada Jiménez Kevin

// Para compilar el programa, ejecute:
// gcc main.c -o server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>
#include <time.h>

// Server escucha por el puerto 9666
#define PORT 9666
#define MAX_CONNECTIONS 5
#define BUFF_SIZE 2048


/*
--------------
Auxiliares
--------------
*/

int generarNumero(int min, int max){
    
    int numero = (rand() % (max - min + 1)) + min;
    return numero;
}

int validarOcteto(int valor) {
    if((valor >= 0) && (valor <= 255)){
        return 1;
    }
    else{
        return 0;
    }
}

int validarLongitudMascara(int valor) {
    if((valor >= 0) && (valor <= 32)){
        return 1;
    }
    else{
        return 0;
    }
}

/*
--------------------------------------------------
Funciones especializadas para calculos requeridos
--------------------------------------------------
*/

// Función para calcular la dirección de broadcast
uint32_t calcularBroadcast(const char *cadena, int client_socket) {
    // Parsea la cadena para extraer la dirección IP y la máscara 
    uint32_t ip = 0;
    int octetos[4];
    int octetos2[4];
    uint32_t broadcast = 0;
    int longitudMascara;

    if (sscanf(cadena, "GET BROADCAST IP %u.%u.%u.%u MASK /%d", 
            &octetos[0], &octetos[1],
            &octetos[2], &octetos[3], &longitudMascara) != 5) {
        // Manejo de errores en caso de que la cadena no esté en el formato esperado
        
        if (sscanf(cadena, "GET BROADCAST IP %u.%u.%u.%u MASK %u.%u.%u.%u", 
               &octetos[0], &octetos[1],
               &octetos[2], &octetos[3], 
               &octetos2[0], &octetos2[1],
               &octetos2[2], &octetos2[3]) != 8) {
            // Manejo de errores en caso de que la cadena no esté en el formato esperado
            
            char* mensaje = "Formato de cadena incorrecto.\nGET BROADCAST IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            //fprintf(stderr, "Formato de cadena incorrecto.\n");

            return -2;
        }
        else{

            // Valida que los octetos de la IP sean valores entre 0 y 255
            if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET BROADCAST IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }
            
            // Obtiene la IP
            ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

             // Valida que los octetos de la IP de la mascara sean valores entre 0 y 255
            if(validarOcteto(octetos2[0]) == 0 || validarOcteto(octetos2[1]) == 0 || validarOcteto(octetos2[2]) == 0 || validarOcteto(octetos2[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara en formato X.X.X.X).\nGET BROADCAST IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la máscara de subred
            uint32_t mascara = (octetos2[0] << 24) | (octetos2[1] << 16) | (octetos2[2] << 8) | octetos2[3];
        
            // Calcula la dirección de broadcast
            broadcast = ip | ~mascara;
        }
    }
    else{
        
        // Valida que los octetos de la IP sean valores entre 0 y 255
        if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
            char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET BROADCAST IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Obtiene la IP
        ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

        // Valida que la longitud de mascara sea un numero entre 0 y 32
        if(validarLongitudMascara(longitudMascara) == 0){
            char* mensaje = "Formato de mascara incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET BROADCAST IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Calcula la máscara de subred
        uint32_t mascara = 0xFFFFFFFF << (32 - longitudMascara);
    
        // Calcula la dirección de broadcast
        broadcast = ip | ~mascara;
    }

    return broadcast;
}

// Función para calcular el network number
uint32_t calcularNetworkNumber(const char *cadena, int client_socket) {
    // Parsea la cadena para extraer la dirección IP y la máscara
    uint32_t ip = 0;
    int octetos[4];
    int octetos2[4];
    uint32_t network_number = 0;
    int longitudMascara;

    if (sscanf(cadena, "GET NETWORK NUMBER IP %u.%u.%u.%u MASK /%d", 
            &octetos[0], &octetos[1],
            &octetos[2], &octetos[3], &longitudMascara) != 5) {
        
        if (sscanf(cadena, "GET NETWORK NUMBER IP %u.%u.%u.%u MASK %u.%u.%u.%u", 
               &octetos[0], &octetos[1],
               &octetos[2], &octetos[3], 
               &octetos2[0], &octetos2[1],
               &octetos2[2], &octetos2[3]) != 8) {
            // Manejo de errores en caso de que la cadena no esté en el formato esperado
            
            char* mensaje = "Formato de cadena incorrecto.\nGET NETWORK NUMBER IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            //fprintf(stderr, "Formato de cadena incorrecto.\n");
            return -2;
        }
        else{

            // Valida que los octetos de la IP sean valores entre 0 y 255
            if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET NETWORK NUMBER IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la IP
            ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

            // Valida que los octetos de la IP de la mascara sean valores entre 0 y 255
            if(validarOcteto(octetos2[0]) == 0 || validarOcteto(octetos2[1]) == 0 || validarOcteto(octetos2[2]) == 0 || validarOcteto(octetos2[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara en formato X.X.X.X).\nGET NETWORK NUMBER IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la máscara de subred
            uint32_t mascara = (octetos2[0] << 24) | (octetos2[1] << 16) | (octetos2[2] << 8) | octetos2[3];
        
            // Calcula la dirección de network_number
            network_number = ip & mascara;
        }
    }
    else{

        // Valida que los octetos de la IP sean valores entre 0 y 255
        if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
            char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET NETWORK NUMBER IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Obtiene la IP
        ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

        // Valida que la longitud de mascara sea un numero entre 0 y 32
        if(validarLongitudMascara(longitudMascara) == 0){
            char* mensaje = "Formato de mascara incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET NETWORK NUMBER IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Calcula la máscara de subred
        uint32_t mascara = 0xFFFFFFFF << (32 - longitudMascara);
    
        // Calcula la dirección de network_number
        network_number = ip & mascara;
    }

    return network_number;
}

// Función para calcular el host range
uint32_t calcularHostRange(const char *cadena, uint32_t *start, uint32_t *end, int client_socket) {
    // Parsea la cadena para extraer la dirección IP y la máscara
    uint32_t ip = 0;
    int octetos[4];
    int octetos2[4];
    int longitudMascara;

    if (sscanf(cadena, "GET HOSTS RANGE IP %u.%u.%u.%u MASK /%d", 
            &octetos[0], &octetos[1],
            &octetos[2], &octetos[3], &longitudMascara) != 5) {
        
        if (sscanf(cadena, "GET HOSTS RANGE IP %u.%u.%u.%u MASK %u.%u.%u.%u", 
               &octetos[0], &octetos[1],
               &octetos[2], &octetos[3], 
               &octetos2[0], &octetos2[1],
               &octetos2[2], &octetos2[3]) != 8) {
            // Manejo de errores en caso de que la cadena no esté en el formato esperado
            
            char* mensaje = "Formato de cadena incorrecto.\nGET HOSTS RANGE IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            //fprintf(stderr, "Formato de cadena incorrecto.\n");
            return -2;
        }
        else{

            // Valida que los octetos de la IP sean valores entre 0 y 255
            if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET HOSTS RANGE IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la IP
            ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

            // Valida que los octetos de la IP de la mascara sean valores entre 0 y 255
            if(validarOcteto(octetos2[0]) == 0 || validarOcteto(octetos2[1]) == 0 || validarOcteto(octetos2[2]) == 0 || validarOcteto(octetos2[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara en formato X.X.X.X).\nGET HOSTS RANGE IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la máscara de subred
            uint32_t mascara = (octetos2[0] << 24) | (octetos2[1] << 16) | (octetos2[2] << 8) | octetos2[3];
        
            // Calcula host range
            *start = (ip & mascara) + 1;
            *end = (ip | ~mascara) - 1;
        }
    }
    else{

        // Valida que los octetos de la IP sean valores entre 0 y 255
        if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
            char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET HOSTS RANGE IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Obtiene la IP
        ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

        // Valida que la longitud de mascara sea un numero entre 0 y 32
        if(validarLongitudMascara(longitudMascara) == 0){
            char* mensaje = "Formato de mascara incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET HOSTS RANGE IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Calcula la máscara de subred
        uint32_t mascara = 0xFFFFFFFF << (32 - longitudMascara);
    
        // Calcula host range
        *start = (ip & mascara) + 1;
        *end = (ip | ~mascara) - 1;
    }

    return 1;
}

// Función para generar subnet random
uint32_t generarSubnet(const char *cadena, int client_socket) {
    // Parsea la cadena para extraer la dirección IP y la máscara
    srand(time(NULL));
    uint32_t ip = 0;
    int octetos[4];
    int octetos2[4];
    int octetos3[4];
    uint32_t subnet = 0;
    int longitudMascara;
    int size;
    int number;

    if (sscanf(cadena, "GET RANDOM SUBNETS NETWORK NUMBER %u.%u.%u.%u MASK /%d NUMBER %d SIZE /%d", 
            &octetos[0], &octetos[1],
            &octetos[2], &octetos[3], &longitudMascara,
            &number, &size) != 7) {
        
        if (sscanf(cadena, "GET RANDOM SUBNETS NETWORK NUMBER %u.%u.%u.%u MASK %u.%u.%u.%u NUMBER %d SIZE %u.%u.%u.%u", 
               &octetos[0], &octetos[1],
               &octetos[2], &octetos[3], 
               &octetos2[0], &octetos2[1],
               &octetos2[2], &octetos2[3], &number,
               &octetos3[0], &octetos3[1],
               &octetos3[2], &octetos3[3]) != 13) {
            // Manejo de errores en caso de que la cadena no esté en el formato esperado
            
            char* mensaje = "Formato de cadena incorrecto.\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            //fprintf(stderr, "Formato de cadena incorrecto.\n");
            return -2;
        }
        else{

            // Valida que los octetos de la IP sean valores entre 0 y 255
            if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la IP
            ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

            // Valida que los octetos de la IP de la mascara sean valores entre 0 y 255
            if(validarOcteto(octetos2[0]) == 0 || validarOcteto(octetos2[1]) == 0 || validarOcteto(octetos2[2]) == 0 || validarOcteto(octetos2[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara en formato X.X.X.X).\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Valida que los octetos de la IP de la mascara del SIZE sean valores entre 0 y 255
            if(validarOcteto(octetos3[0]) == 0 || validarOcteto(octetos3[1]) == 0 || validarOcteto(octetos3[2]) == 0 || validarOcteto(octetos3[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara del SIZE en formato X.X.X.X).\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la máscara de subred
            uint32_t mascara = (octetos2[0] << 24) | (octetos2[1] << 16) | (octetos2[2] << 8) | octetos2[3];
            uint32_t mascara2 = (octetos3[0] << 24) | (octetos3[1] << 16) | (octetos3[2] << 8) | octetos3[3];

            // Valida que el size sea mayor que la mascara de la red
            if(mascara2 <= mascara){
                char* mensaje = "Error: El numero del SIZE debe ser mayor que la mascara de la subred\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            int start = (ip & mascara) + 1;
            int end = (ip | ~mascara) - 1;
            
            for (int i = 1; i <= number; i++) {
                uint32_t randomSubnet = (generarNumero((start >> 24) & 0xFF, (end >> 24) & 0xFF) << 24) | 
                    (generarNumero((start >> 16) & 0xFF, (end >> 16) & 0xFF) << 16) | 
                    (generarNumero((start >> 8) & 0xFF, (end >> 8) & 0xFF) << 8) | 
                    generarNumero((start) & 0xFF, (end) & 0xFF);
                
                randomSubnet = randomSubnet & mascara2;
                
                char mensaje[128];
                sprintf(mensaje, "Subred %d: %u.%u.%u.%u mask %u.%u.%u.%u\n", i, (randomSubnet >> 24) & 0xFF, (randomSubnet >> 16) & 0xFF, (randomSubnet >> 8) & 0xFF, randomSubnet & 0xFF, (mascara2 >> 24) & 0xFF, (mascara2 >> 16) & 0xFF, (mascara2 >> 8) & 0xFF, mascara2 & 0xFF);
                write(client_socket, mensaje, strlen(mensaje));
            }
        }
    }
    else{
        
        // Valida que los octetos de la IP sean valores entre 0 y 255
        if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
            char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Obtiene la IP
        ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

        if(validarLongitudMascara(longitudMascara) == 0){
            char* mensaje = "Formato de mascara incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        if(validarLongitudMascara(size) == 0){
            char* mensaje = "Formato de size incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Calcula la máscara de subred
        uint32_t mascara = 0xFFFFFFFF << (32 - longitudMascara);
        uint32_t mascara2 = 0xFFFFFFFF << (32 - size);

        // Valida que el size sea mayor que la mascara de la red
        if(mascara2 <= mascara){
            char* mensaje = "Error: El numero del SIZE debe ser mayor que la mascara de la subred\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }
        
        int start = (ip & mascara) + 1;
        int end = (ip | ~mascara) - 1;

        char mensajeRandom[128];
        bzero(mensajeRandom,128);

        for (int i = 1; i <= number; i++) {
            uint32_t randomSubnet = (generarNumero((start >> 24) & 0xFF, (end >> 24) & 0xFF) << 24) | 
                (generarNumero((start >> 16) & 0xFF, (end >> 16) & 0xFF) << 16) | 
                (generarNumero((start >> 8) & 0xFF, (end >> 8) & 0xFF) << 8) | 
                generarNumero((start) & 0xFF, (end) & 0xFF);
            
            randomSubnet = randomSubnet & mascara2;

            char mensaje[128];
            sprintf(mensaje, "Subred %d: %u.%u.%u.%u/%u \n ", i, (randomSubnet >> 24) & 0xFF, (randomSubnet >> 16) & 0xFF, (randomSubnet >> 8) & 0xFF, randomSubnet & 0xFF, size);
            write(client_socket, mensaje, strlen(mensaje));
        }
    }

    return 1;
}

// Función para generar subnet random
uint32_t generarSubnetWin(const char *cadena, int client_socket) {
    // Parsea la cadena para extraer la dirección IP y la máscara
    srand(time(NULL));
    uint32_t ip = 0;
    int octetos[4];
    int octetos2[4];
    int octetos3[4];
    uint32_t subnet = 0;
    int longitudMascara;
    int size;
    int number;

    if (sscanf(cadena, "GET SNET NET %u.%u.%u.%u M /%d N %d S /%d", 
            &octetos[0], &octetos[1],
            &octetos[2], &octetos[3], &longitudMascara,
            &number, &size) != 7) {
        
        if (sscanf(cadena, "GET SNET NET %u.%u.%u.%u M %u.%u.%u.%u N %d S %u.%u.%u.%u", 
               &octetos[0], &octetos[1],
               &octetos[2], &octetos[3], 
               &octetos2[0], &octetos2[1],
               &octetos2[2], &octetos2[3], &number,
               &octetos3[0], &octetos3[1],
               &octetos3[2], &octetos3[3]) != 13) {
            // Manejo de errores en caso de que la cadena no esté en el formato esperado
            
            char* mensaje = "Formato de cadena incorrecto.\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            //fprintf(stderr, "Formato de cadena incorrecto.\n");
            return -2;
        }
        else{

            // Valida que los octetos de la IP sean valores entre 0 y 255
            if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la IP
            ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

            // Valida que los octetos de la IP de la mascara sean valores entre 0 y 255
            if(validarOcteto(octetos2[0]) == 0 || validarOcteto(octetos2[1]) == 0 || validarOcteto(octetos2[2]) == 0 || validarOcteto(octetos2[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara en formato X.X.X.X).\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Valida que los octetos de la IP de la mascara del SIZE sean valores entre 0 y 255
            if(validarOcteto(octetos3[0]) == 0 || validarOcteto(octetos3[1]) == 0 || validarOcteto(octetos3[2]) == 0 || validarOcteto(octetos3[3]) == 0){
                char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto de la mascara del SIZE en formato X.X.X.X).\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            // Obtiene la máscara de subred
            uint32_t mascara = (octetos2[0] << 24) | (octetos2[1] << 16) | (octetos2[2] << 8) | octetos2[3];
            uint32_t mascara2 = (octetos3[0] << 24) | (octetos3[1] << 16) | (octetos3[2] << 8) | octetos3[3];

            // Valida que el size sea mayor que la mascara de la red
            if(mascara2 <= mascara){
                char* mensaje = "Error: El numero del SIZE debe ser mayor que la mascara de la subred\n";
                write(client_socket, mensaje, strlen(mensaje));
                return -2;
            }

            int start = (ip & mascara) + 1;
            int end = (ip | ~mascara) - 1;
            
            for (int i = 1; i <= number; i++) {
                uint32_t randomSubnet = (generarNumero((start >> 24) & 0xFF, (end >> 24) & 0xFF) << 24) | 
                    (generarNumero((start >> 16) & 0xFF, (end >> 16) & 0xFF) << 16) | 
                    (generarNumero((start >> 8) & 0xFF, (end >> 8) & 0xFF) << 8) | 
                    generarNumero((start) & 0xFF, (end) & 0xFF);
                
                randomSubnet = randomSubnet & mascara2;
                
                char mensaje[128];
                sprintf(mensaje, "Subred %d: %u.%u.%u.%u mask %u.%u.%u.%u\n", i, (randomSubnet >> 24) & 0xFF, (randomSubnet >> 16) & 0xFF, (randomSubnet >> 8) & 0xFF, randomSubnet & 0xFF, (mascara2 >> 24) & 0xFF, (mascara2 >> 16) & 0xFF, (mascara2 >> 8) & 0xFF, mascara2 & 0xFF);
                write(client_socket, mensaje, strlen(mensaje));
            }
        }
    }
    else{
        
        // Valida que los octetos de la IP sean valores entre 0 y 255
        if(validarOcteto(octetos[0]) == 0 || validarOcteto(octetos[1]) == 0 || validarOcteto(octetos[2]) == 0 || validarOcteto(octetos[3]) == 0){
            char* mensaje = "Formato de direccion IP incorrecto (Utilice valores entre 0 y 255 para cada octeto).\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Obtiene la IP
        ip = (octetos[0] << 24) | (octetos[1] << 16) | (octetos[2] << 8) | octetos[3];

        if(validarLongitudMascara(longitudMascara) == 0){
            char* mensaje = "Formato de mascara incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        if(validarLongitudMascara(size) == 0){
            char* mensaje = "Formato de size incorrecto (Utilice valores entre 0 y 32 para la mascara en formato bits).\nGET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }

        // Calcula la máscara de subred
        uint32_t mascara = 0xFFFFFFFF << (32 - longitudMascara);
        uint32_t mascara2 = 0xFFFFFFFF << (32 - size);

        // Valida que el size sea mayor que la mascara de la red
        if(mascara2 <= mascara){
            char* mensaje = "Error: El numero del SIZE debe ser mayor que la mascara de la subred\n";
            write(client_socket, mensaje, strlen(mensaje));
            return -2;
        }
        
        int start = (ip & mascara) + 1;
        int end = (ip | ~mascara) - 1;

        for (int i = 1; i <= number; i++) {
            uint32_t randomSubnet = (generarNumero((start >> 24) & 0xFF, (end >> 24) & 0xFF) << 24) | 
                (generarNumero((start >> 16) & 0xFF, (end >> 16) & 0xFF) << 16) | 
                (generarNumero((start >> 8) & 0xFF, (end >> 8) & 0xFF) << 8) | 
                generarNumero((start) & 0xFF, (end) & 0xFF);
            
            randomSubnet = randomSubnet & mascara2;

            char mensaje[128];
            sprintf(mensaje, "Subred %d: %u.%u.%u.%u/%u \n ", i, (randomSubnet >> 24) & 0xFF, (randomSubnet >> 16) & 0xFF, (randomSubnet >> 8) & 0xFF, randomSubnet & 0xFF, size);
            write(client_socket, mensaje, strlen(mensaje));
        }
    }

    return 1;
}

-----------------------------------
"Menu" de la Calculadora
-----------------------------------
*/
void calculadora(int client_socket){
    while(1){
        // se recibe el mensaje del cliente
        char buffer[BUFF_SIZE];
        int bytes_received = read(client_socket, buffer, BUFF_SIZE);
        if (bytes_received == -1) {
            perror("Error al recibir datos del cliente");
        }
        printf("Recibí %d bytes del cliente con mensaje: %s\n", bytes_received, buffer);

        // Se recibe la entrada de usuario
        char *cadena = buffer;
        //const char *cadena = "GET RANDOM SUBNETS NETWORK NUMBER 10.5.3.0 MASK 255.0.0.0 NUMBER 3 SIZE 255.240.0.0"; //"GET BROADCAST 10.8.2.5 MASK /29";
        uint32_t result = 0;

        uint32_t rangeStart = 0;
        uint32_t rangeEnd = 0;

        if (strncmp(cadena, "GET BROADCAST", 13) == 0) {
            result = calcularBroadcast(cadena, client_socket);
            if (result != -2) {
                char mensaje[32];
                sprintf(mensaje, "El Broadcast es: %u.%u.%u.%u\n", 
                    (result >> 24) & 0xFF, (result >> 16) & 0xFF, 
                    (result >> 8) & 0xFF, result & 0xFF);
                
                write(client_socket, mensaje, strlen(mensaje));
            }
        } else if (strncmp(cadena, "GET HOSTS RANGE", 15) == 0) {
            result = calcularHostRange(cadena, &rangeStart, &rangeEnd, client_socket);
            if (result != -2) {
                char mensaje[128];
                sprintf(mensaje, "El Host Range es: %u.%u.%u.%u - %u.%u.%u.%u\n",
                    (rangeStart >> 24) & 0xFF, (rangeStart >> 16) & 0xFF, (rangeStart >> 8) & 0xFF, rangeStart & 0xFF,
                    (rangeEnd >> 24) & 0xFF, (rangeEnd >> 16) & 0xFF, (rangeEnd >> 8) & 0xFF, rangeEnd & 0xFF);
                write(client_socket, mensaje, strlen(mensaje));
            }
        } else if (strncmp(cadena, "GET NETWORK NUMBER", 18) == 0) {
            result = calcularNetworkNumber(cadena, client_socket);
            if (result != -2) {
                char mensaje[32];
                sprintf(mensaje,"El Network Number es: %u.%u.%u.%u\n", 
                    (result >> 24) & 0xFF, (result >> 16) & 0xFF, 
                    (result >> 8) & 0xFF, result & 0xFF);
                write(client_socket, mensaje, strlen(mensaje));
            }
        } else if (strncmp(cadena, "GET RANDOM SUBNETS", 18) == 0) {
            result = generarSubnet(cadena, client_socket);
        } else if (strncmp(cadena, "GET SNET", 8) == 0) {
            result = generarSubnetWin(cadena, client_socket);
        } else if (strncmp(cadena, "EXIT", 4) == 0) {
            return 0;
        } else {
            char* mensaje = "Entrada no valida.\n";
            write(client_socket, mensaje, strlen(mensaje));
        }
        bzero(cadena,BUFF_SIZE);
    }

}

/*
-----------------------------------
main
- Se establece la configuracion 
  de los sockets y del servidor
-----------------------------------
*/

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del servidor");
        exit(1);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Vincular el socket a la dirección y puerto
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr) ) == -1) {
        perror("Error al vincular el socket al puerto");
        exit(1);
    }

    // Escuchar por conexiones entrantes
    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        perror("Error al escuchar por conexiones entrantes");
        exit(1);
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        // Aceptar una conexión entrante
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error al aceptar la conexión");
            continue;
        }

        printf("Cliente conectado desde %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Manejar la conexión con el cliente hacia la calculadora
        calculadora(client_socket);

        // Cerrar la conexión con el cliente
        close(client_socket);
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;
}
