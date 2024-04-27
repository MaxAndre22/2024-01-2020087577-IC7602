import socket

def pruebasUDP(host, port, message):
    # Crear un socket UDP
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        # Enviar el mensaje al servidor UDP
        s.sendto(message.encode(), (host, port))
        print(f"Mensaje enviado al servidor UDP en {host}:{port}: {message}")

        # Esperar la respuesta del servidor
        response, server_address = s.recvfrom(1024)
        print(f"Respuesta recibida del servidor UDP en {server_address}: {response.decode()}")

if __name__ == "__main__":
    # Definir la dirección y el puerto del servidor UDP
    SERVER_HOST = 'localhost'  # Cambia esto por la dirección del servidor UDP
    SERVER_PORT = 5050  # Cambia esto por el puerto del servidor UDP

    # Definir el mensaje a enviar
    MESSAGE = "Hola desde el cliente UDP"
    

    # Llamar a la función del cliente UDP
    pruebasUDP(SERVER_HOST, SERVER_PORT, MESSAGE)
    print(MESSAGE)