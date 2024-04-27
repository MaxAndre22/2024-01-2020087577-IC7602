import socket

# Configura el socket UDP
UDP_IP = "127.0.0.1"  # Dirección IP del servidor
UDP_PORT = 12345      # Puerto del servidor

# Crea un socket UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Envía un mensaje al servidor
sock.sendto(b"Mensaje de prueba", (UDP_IP, UDP_PORT))

# Espera a recibir la respuesta del servidor
data, addr = sock.recvfrom(1024)
print("Recibido mensaje del servidor:", data.decode())

# Cierra el socket
sock.close()
