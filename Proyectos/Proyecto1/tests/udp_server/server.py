import os
import socket

# Configura el socket UDP
UDP_IP = "0.0.0.0"  # Escucha en todas las interfaces
UDP_PORT = 12345    # Puerto de escucha

# Lee el valor de la variable de entorno
message = "Hola " + os.getenv("MY_VARIABLE", "mundo")

# Crea un socket UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Enlaza el socket al puerto
sock.bind((UDP_IP, UDP_PORT))

# Espera a recibir datos
while True:
    data, addr = sock.recvfrom(1024)  # Recibe datos del cliente
    print("Recibido mensaje:", data.decode())
    # Envía la respuesta al cliente
    sock.sendto(message.encode(), addr)
