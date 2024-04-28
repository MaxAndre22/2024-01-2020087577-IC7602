# ¿Porque un proxy TCP es más rápido que un proxy HTTP?
### Máximo Agrazal Quirós
### I Semestre 2024
### Redes IC-7602


Cuando hablamos de un proxy, nos referimos a un programa que actúa como intermediario entre un cliente y un servidor (destino), redirigiendo nuestras solicitudes hacia el servidor web. Los proxies tienen usos variados, desde ocultar los datos del usuario hasta proporcionar una navegación más segura. La distinción clave entre un Proxy TCP y un Proxy HTTP radica en las capas del Modelo OSI en las que operan. Mientras que el Proxy TCP se encuentra en la capa 4, encargada de manejar conexiones TCP completas, el Proxy HTTP opera en la capa de aplicación (capa 7), gestionando solicitudes y respuestas HTTP entre clientes y servidores web. Esta diferencia de capas contribuye a la disparidad en la velocidad y funcionalidad entre ambos tipos de proxies. Teniendo eso claro, ahora veremos algunas otras razones de por qué ocurre esto:

### Razones para la Superioridad del Proxy TCP

**Manejo eficiente de múltiples conexiones**: un proxy TCP puede manejar eficientemente múltiples conexiones simultáneas. Debido a que TCP es un protocolo orientado a la conexión, puede manejar una amplia variedad de solicitudes sin sacrificar la velocidad o la estabilidad. Por otro lado, un proxy HTTP se limita a procesar una conexión a la vez, lo que puede provocar cuellos de botella y retrasos, especialmente en entornos con uso intensivo de datos.

**Menor sobrecarga de procesamiento**: debido a su posición en un nivel inferior del modelo OSI, el proxy TCP está sujeto a una menor sobrecarga de procesamiento relacionada con la traducción y manipulación de datos en comparación con un proxy HTTP. Esta menor sobrecarga permite que el proxy TCP optimice la velocidad de transmisión, reduciendo el tiempo de procesamiento requerido para cada solicitud.

**Flexibilidad en la gestión de conexiones**: Los servidores proxy TCP ofrecen una mayor flexibilidad en la gestión de conexiones, permitiéndole especificar rangos de puertos y direcciones IP para aceptar o rechazar conexiones entrantes. Esta característica de configuración avanzada no sólo mejora la seguridad sino que también ayuda a distribuir los recursos del servidor de manera más eficiente, lo que resulta en un rendimiento más rápido.

**Eliminación de la conversión de protocolo**: un proxy HTTP debe traducir el flujo de bytes TCP en tramas HTTP, un proceso que puede ser relativamente lento y consumir muchos recursos. Por el contrario, un proxy TCP evita esta conversión manteniendo los datos en su forma original, lo que resulta en una transmisión más rápida y eficiente.

**Mayor rendimiento general**: debido a su capacidad para manejar múltiples conexiones, reducir la sobrecarga de procesamiento y eliminar la necesidad de conversión de protocolo, los servidores proxy TCP generalmente ofrecen un mejor rendimiento general que los servidores proxy HTTP. Este aumento de la eficiencia conduce a tiempos de respuesta más rápidos y una mejor experiencia de usuario.

### Pruebas de carga realizadas

A pesar de que no se pudo realizar pruebas de carga como tal, los resultados esperados de dicha prueba probablemente incluyan un flujo más lento en el proxy HTTP, dado que el host solo podría aceptar una conexión a la vez, lo que provocaría bottle neck en este proxy, mientras el TCP pueda manejar múltiples simultáneamente sin problema.


