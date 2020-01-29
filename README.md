# Excel-Microcontroladores
El ejemplo que proponemos hace uso de xlApp, este servicio puede crear objetos que interactúen con programas del paquete Office, un pequeño programa escrito en C++ sirve como interfaz entre un microcontroadr y Excel, básicamente recibe comandos desde el controlador y los traduce en acciones sobre el sistema Windows.
En el ejemplo creamos un objeto para Excel (pero podríamos haber hecho lo mismo con cualquier programa Office). 

ATENCIÓN: 
----------
El programa es solo un ejemplo, no se han contemplado detalles de control para la captura de errores en la comunicación, o desconexión de una de las partes, por ejemplo si cierra el Excel antes de cortar la comuicación el programa entra en un bucle infinito de error de comunicación.
Mas info en www.firtec.com.ar
