# Tarea Corta 2 - Calculadora de Subredes IPv4

Tecnológico de Costa Rica  
Escuela de Ingeniería en Computación  
Redes (IC 7602)  
I Semestre 2024

## Acerca de la Tarea

Aspectos generales de la tarea, tomados de la especificación brindada por el profesor.

### Objetivo General

- Implementar una calculadora de subredes IPv4 utilizando lenguaje de programación C y Kubernetes/Helm.

### Objetivos Específicos

- Desarrollar habilidades en sockets, operadores bitwise y lenguaje de programación C.
- Implementar un protocolo para intercambiar información entre un cliente y un servidor.

### Descripción

Se implementa un servidor en **C** que escucha en el puerto **TCP/9666**. Los clientes que se conectan vía **telnet** pueden enviar las siguientes primitivas:

- GET BROADCAST IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}  
   Por ejemplo:

  1. GET BROADCAST IP 10.8.2.5 MASK /29  
     a. Retorna 10.8.2.7
  2. GET BROADCAST IP 172.16.0.56 MASK 255.255.255.128  
     a. Retorna 172.16.0.127

- GET NETWORK NUMBER IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}  
   Por ejemplo:

  1. GET NETWORK NUMBER IP 10.8.2.5 MASK /29  
     a. Retorna 10.8.2.0
  2. GET NETWORK NUMBER IP 172.16.0.56 MASK 255.255.255.128  
     a. Retorna 172.16.0.0

- GET HOSTS RANGE IP {dirección IP} MASK {mascara en formato /bits o notación X.X.X.X}  
   Por ejemplo:

  1. GET HOSTS RANGE IP 10.8.2.5 MASK /29  
     a. Retorna 10.8.2.{1-6}
  2. GET HOSTS RANGE IP 172.16.0.56 MASK 255.255.255.128  
     a. Retorna 172.16.0.{1-126}

- GET RANDOM SUBNETS NETWORK NUMBER {Y.Y.Y.Y} MASK {mascara en formato /bits o notación X.X.X.X} NUMBER {número de redes} SIZE {mascara en formato /bits o notación X.X.X.X}  
   Por ejemplo:
  1. GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24  
     a. Puede retornar:  
      i. 10.20.10.0/24  
      ii. 10.33.11.0/24  
      iii. 10.42.13.0/24
- **Importante:**

  - El formato de MASK y SIZE debe coincidir

  - En caso de que se ejecute desde telnet en un sistema operativo Windows y de error por la longitud del comando enviado en la primitiva de "GET RANDOM SUBNETS", se debera ejecutar con este formato corto:
    - GET SNET NET {Y.Y.Y.Y} M {mascara en formato /bits o notación X.X.X.X} N {número de redes} S {mascara en formato /bits o notación X.X.X.X}

El entorno de ejecución así como el servidor, están automatizados mediante Kubernetes/Helm. Además, para satisfacer las operaciones propias de la calculadora, se utilizan operadores bitwise.

## Componentes

### C & GCC Compiler

Sitio: https://gcc.gnu.org/

Objetivo: Lenguaje de Programación C, utilizado mediante el compilador gcc.

### Telnet

Sitio: https://www.ibm.com/docs/en/i/7.5?topic=services-telnet

Función: Protocolo que permite la conexión a una máquina remota, dentro de la red local. Funciona como cliente al servidor.

### Docker

Sitio: https://www.docker.com/

Función: Provee un entorno de ejecución virtual, con un sistema operativo Linux por debajo

### Kubernetes

Sitio: https://kubernetes.io/es/

Función: Realiza una orquestación de contenedores para entornos de producción.

### Helm

Sitio: https://helm.sh/

Función: Facilita la gestión de Kubernetes. En este caso, mantiene la dispoibilidad del servico

## Instrucciones de Ejecución

### 1. Instalar Docker Desktop

Primeramente se debe tener instalado Docker en la computadora para poder ejecutar la aplicación. Se sugiere consultar la guía oficial y además consultar más recursos

Guía oficial para Windows: https://docs.docker.com/desktop/install/windows-install/  
Guía oficial para Linux: https://docs.docker.com/desktop/install/windows-install/

### 2. Habilitar Kubernetes

Una vez que haya instalado correctamente Docker Desktop, proceda a habilitar kubernetes:  
 i. Inicie Docker Desktop  
 ii. Diríjase a Ajustes  
 iii. En el menú a la izquierda, elija Kubernetes  
 iv. Accione el switch para activar Kubernetes

### 3. Instalar Telnet

Telnet se utilizará como cliente de la calculadora. Generalmente esta herramienta por defecto se instala en Linux y Windows. Si no es así, puede consultar alguna de las múltiples guías que se encuentran en internet
Para verificar la instalación, puede abrir la consola de la herramienta ingresando `telnet`:

```sh
   usuario@pc:~$ telnet
   telnet>
```

### 4. Instalar Helm

- Windows:

  - Visita la página de versiones de Helm en GitHub para descargar el archivo ejecutable de Helm para Windows. Asegúrate de descargar la última versión compatible con Windows.
  - El enlace de GitHub es: https://github.com/helm/helm/releases
  - Para ejecutar Helm desde cualquier ubicación en la línea de comandos, necesita agregar la carpeta que contiene el archivo ejecutable de Helm a la variable de entorno PATH.

- Linux:
  - En Ubuntu lo puedes instalar con los siguientes comandos:

```sh
curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3

chmod 700 get_helm.sh

./get_helm.sh
```

### 5. Hacer Instalacion del Servidor

En una consola de comandos, diríjase al directorio donde está ubicada la carpeta "tc2" con los scripts necesarios para hacer la instalación mediante Helm y ejecute el siguiente comando:

```sh
helm install tc2 ./tc2
```

### 6. Conectarse al Servidor

En una consola de comandos, ejecute el comando:

```sh
telnet 127.0.0.1 9666
```

Cuando se complete la conexion podra digitar las instrucciones deseadas por ejemplo:

```sh
GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24
```

Y presionar la tecla enter para enviar.

En caso de que se use telnet desde el sistema operativo Windows, antes de digitar las instrucciones debe presionar las teclas "Ctrl" + "]" y posteriormente enviar el comando de la siguiente forma:

```sh
send GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24
```

Y presionar tecla enter dos veces para ver el resultado.

## Referencias

[1] Ing. Javier Ignacio Miles Avello. Calcular Dirección de Red y Broadcast Networking (14 abr., 2020). Consultado: 13 oct., 2023. [Video en línea]. Disponible: https://www.youtube.com/watch?v=2WqMUDqKHcs

[2] The GNU library, "16 Scokets", https://www.gnu.org/software/libc/manual/html_node/Sockets.html (Consultado 14 oct., 2023)

[3] HELM Docs, "Quickstart Guide", https://helm.sh/docs/intro/quickstart/ (Consultado 15 oct., 2023)

[4] educba.com, "Docker Expose", https://www.educba.com/docker-expose/ (Consultado 16 oct., 2023)

[5] A. Murray, "How to Expose Ports in Docker", mend.io, https://www.mend.io/free-developer-tools/blog/docker-expose-port/ (Consultado 16 oct., 2023)
