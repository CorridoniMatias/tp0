#include "tp0.h"

int main() {
  configure_logger();
  int socket = connect_to_server(IP, PUERTO);
  wait_hello(socket);
  Alumno alumno = read_hello();
  send_hello(socket, alumno);
  void * content = wait_content(socket);
  send_md5(socket, content);
  wait_confirmation(socket);
  exit_gracefully(0);
}

void configure_logger() {

  /*
    1.  Creemos el logger con la funcion de las commons log_create.
        Tiene que: guardarlo en el archivo tp0.log, mostrar 'tp0' al loggear,
        mostrarse por pantalla y mostrar solo los logs de nivel info para arriba
        (info, warning y error!)
  */
  logger = log_create("/tmp/tp0.log", "tp0", true, LOG_LEVEL_INFO);
}

int connect_to_server(char * ip, char * port) {
  struct addrinfo hints;
  struct addrinfo *server_info;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

  getaddrinfo(ip, port, &hints, &server_info);  // Carga en server_info los datos de la conexion

  // 2. Creemos el socket con el nombre "server_socket" usando la "server_info" que creamos anteriormente
  int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

  // 3. Conectemosnos al server a traves del socket! Para eso vamos a usar connect()
  int retorno = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen);

  freeaddrinfo(server_info);  // No lo necesitamos mas
  /*
    3.1 Recuerden chequear por si no se pudo contectar (usando el retorno de connect()).
        Si hubo un error, lo loggeamos y podemos terminar el programa con la funcioncita
        exit_gracefully pasandole 1 como parametro para indicar error ;).
        Pss, revisen los niveles de log de las commons.
  */

  if(retorno == -1) //Hubo error
  {
	  /*char* error = "Error al Conectar: ";
	  string_append(*error, errno);*/
	  log_info(logger, "Error al comentar");
	  exit_gracefully(EXIT_FAILURE);
  }

  // 4 Logeamos que pudimos conectar y retornamos el socket
  log_info(logger, "Conectado!");
  return server_socket;
}

void  wait_hello(int socket) {
  char * hola = "SYSTEM UTNSO 0.1";

  /*
    5.  Ya conectados al servidor, vamos a hacer un handshake!
        Para esto, vamos a, primero recibir un mensaje del
        servidor y luego mandar nosotros un mensaje.
        Deberìamos recibir lo mismo que está contenido en la
        variable "hola". Entonces, vamos por partes:
        5.1.  Reservemos memoria para un buffer para recibir el mensaje.
  */

  char * buffer = (char*)malloc(16);

  /*
        5.2.  Recibamos el mensaje en el buffer.
        Recuerden el prototipo de recv:
        conexión - donde guardar - cant de bytes - flags(si no se pasa ninguno puede ir NULL)
        Nota: Palabra clave MSG_WAITALL.
  */

  int result_recv = recv(socket, buffer, 16, MSG_WAITALL);

  if(result_recv == -1)
  {
	  log_info(logger, "Error al recibir.");
	  free(buffer);
	  exit_gracefully(EXIT_FAILURE);
	  return;
  }

  if(strcmp(buffer, hola) == 0)
	  log_info(logger, "Recibido OK");
  else
	  log_info(logger, "Recibi cualquier cosa");

  free(buffer);
  /*
        5.3.  Chequiemos errores al recibir! (y logiemos, por supuesto)
        5.4.  Comparemos lo recibido con "hola".
              Pueden usar las funciones de las commons para comparar!
        No se olviden de loggear y devolver la memoria que pedimos!
        (si, también si falló algo, tenemos que devolverla, atenti.)
  */

}

Alumno read_hello() {
  /*
    6.    Ahora nos toca mandar a nosotros un mensaje de hola.
          que van a ser nuestros datos, definamos una variable de tipo Alumno.
          Alumno es esa estructura que definimos en el .h.
          Recuerden definir el nombre y apellido como cadenas varias, dado
          que como se va a enviar toda la estructura completa, para evitar problemas
          con otros otros lenguajes
  */

  Alumno alumno = { .nombre = "", .apellido = "" };

  /*
    7.    Pero como conseguir los datos? Ingresemoslos por consola!
          Para eso, primero leamos por consola usando la biblioteca realine.
          Vamos a recibir, primero el legajo, despues el nombre y
          luego el apellido
  */
  char * tmp = readline("Legajo: ");

  alumno.legajo = atoi(tmp);

  tmp = readline("Nombre: ");

  memcpy(alumno.nombre, tmp, strlen(tmp));

  tmp = readline("Apellido: ");

  memcpy(alumno.apellido, tmp, strlen(tmp));

  free(tmp);

  /*
    8.    Realine nos va a devolver un cacho de memoria ya reservada
          con lo que leyo del teclado hasta justo antes del enter (/n).
          Ahora, nos toca copiar el legajo al la estructura alumno. Como
          el legajo es numero, conviertanlo a numero con la funcion atoi
          y asignenlo.
          Recuerden liberar la memoria pedida por readline con free()!
  */


  /*
    9.    Para el nombre y el apellido no hace falta convertirlos porque
          ambos son cadenas de caracteres, por los que solo hace falta
          copiarlos usando memcpy a la estructura y liberar la memoria
          pedida por readline.
  */
  // Usemos memcpy(destino, origen, cant de bytes).
  // Para la cant de bytes nos conviene usar strlen dado que son cadenas
  // de caracteres que cumplen el formato de C (terminar en \0)

  // 9.1. Faltaría armar el del apellido

  // 10. Finalmente retornamos la estructura
  return alumno;
}

void send_hello(int socket, Alumno alumno) {

  log_info(logger, "Enviando info de Estudiante");


  /*
    11.   Ahora SI nos toca mandar el hola con los datos del alumno.
          Pero nos falta algo en nuestra estructura, el id_mensaje del protocolo.
          Segun definimos, el tipo de id para un mensaje de tamaño fijo con
          la informacion del alumno es el id 99
  */
  alumno.id_mensaje = 99;

  /*
    11.1. Como algo extra, podes probar enviando caracteres invalidos en el nombre
          o un id de otra operacion a ver que responde el servidor y como se
          comporta nuestro cliente.
  */  

  // alumno.id = 33;
  // alumno.nombre[2] = -4;

  /*
    12.   Finalmente, enviemos la estructura por el socket!
          Recuerden que nuestra estructura esta definida como __attribute__((packed))
          por lo que no tiene padding y la podemos mandar directamente sin necesidad
          de un buffer y usando el tamaño del tipo Alumno!
  */
  int resultado = send(socket, &alumno, sizeof(Alumno), 0);

  if(resultado == -1)
	  log_info(logger, "Error al enviar.");
  else if(resultado == sizeof(Alumno))
	  log_info(logger, "Enviado OK");
  else
	  log_info(logger, "U FOKIN WOT M8");

  //close(socket);
  //log_info(logger, "Socket cerrado.");

  /*
    12.1. Recuerden que al salir tenemos que cerrar el socket (ademas de loggear)!
  */
}

void * wait_content(int socket) {
  /*
    13.   Ahora tenemos que recibir un contenido de tamaño variable
          Para eso, primero tenemos que confirmar que el id corresponde al de una
          respuesta de contenido variable (18) y despues junto con el id de operacion
          vamos a haber recibido el tamaño del contenido que sigue. Por lo que:
  */

  log_info(logger, "Esperando el encabezado del contenido(%ld bytes)", sizeof(ContentHeader));
  // 13.1. Reservamos el suficiente espacio para guardar un ContentHeader
  ContentHeader * header = (ContentHeader*)malloc(sizeof(ContentHeader));

  // 13.2. Recibamos el header en la estructura y chequiemos si el id es el correcto.
  //      No se olviden de validar los errores, liberando memoria y cerrando el socket!

  log_info(logger, "Esperando el contenido (%d bytes)", header->len);


  int resultado = recv(socket, header, sizeof(ContentHeader), 0);

    if(resultado == -1)
  	  log_info(logger, "Error al recibir.");
    else if(resultado == sizeof(ContentHeader))
  	  log_info(logger, "Recibido OK");
    else
  	  log_info(logger, "U FOKIN WOT M8");

    if (header->id != 18) {
    	_exit_with_error(socket, "Id incorrecto, deberia ser 18", header);
    }


    log_info(logger, "Esperando el contenido (%d bytes)", header->len);

    void * buf = calloc(sizeof(char), header->len + 1);

    if (recv(socket, buf, header->len, MSG_WAITALL) <= 0) {
        free(buf);
        _exit_with_error(socket, "Error recibiendo el contenido", header);
    }
    log_info(logger, "Contenido recibido '%s'", (char*) buf);
    free(header);
    return buf;


  /*
      14.   Ahora, recibamos el contenido variable. Ya tenemos el tamaño,
            por lo que reecibirlo es lo mismo que veniamos haciendo:
      14.1. Reservamos memoria
      14.2. Recibimos el contenido en un buffer (si hubo error, fallamos, liberamos y salimos
  */

  /*
      15.   Finalmente, no te olvides de liberar la memoria que pedimos
            para el header y retornar el contenido recibido.
  */
}


void _exit_with_error(int socket, char* error_msg, void * buffer) {
   if (buffer != NULL) {
       free(buffer);
   }
   log_error(logger, error_msg);
   close(socket);
   exit_gracefully(1);
}


void send_md5(int socket, void * content) {
  /*
    16.   Ahora calculemos el MD5 del contenido, para eso vamos
          a armar el digest:
  */

  void * digest = malloc(MD5_DIGEST_LENGTH);
  MD5_CTX context;
  MD5_Init(&context);
  MD5_Update(&context, content, strlen(content) + 1);
  MD5_Final(digest, &context);

  free(content);

  /*
    17.   Luego, nos toca enviar a nosotros un contenido variable.
          A diferencia de recibirlo, para mandarlo es mejor enviarlo todo de una,
          siguiendo la logida de 1 send - N recv.
          Asi que:
  */

  //      17.1. Creamos un ContentHeader para guardar un mensaje de id 33 y el tamaño del md5


  ContentHeader header = { .id = 33, .len = MD5_DIGEST_LENGTH };
  int message_size = sizeof(ContentHeader) + MD5_DIGEST_LENGTH;
  void * buf = malloc(message_size);
  memcpy(buf, &header, sizeof(ContentHeader));
  memcpy(buf + sizeof(ContentHeader), digest, MD5_DIGEST_LENGTH);

  log_info(logger, "Enviando MD5");
  int result_send =  send(socket, buf, message_size, 0);
  free(buf);

  if (result_send <= 0) {
      _exit_with_error(socket, "No se pudo enviar el md5", NULL);
  }



  /*
          17.2. Creamos un buffer del tamaño del mensaje completo y copiamos el header y la info de "digest" allí.
          Recuerden revisar la función memcpy(ptr_destino, ptr_origen, tamaño)!
  */

  /*
    18.   Con todo listo, solo nos falta enviar el paquete que armamos y liberar la memoria que usamos.
          Si, TODA la que usamos, eso incluye a la del contenido del mensaje que recibimos en la función
          anterior y el digest del MD5. Obviamente, validando tambien los errores.
  */
}

void wait_confirmation(int socket) {
  //int result = 1; // Dejemos creado un resultado por defecto
  /*
    19.   Ahora nos toca recibir la confirmacion del servidor.
          Si el resultado obvenido es distinto de 0, entonces hubo un error
  */

  int result = 0;
  log_info(logger, "Esperando confirmacion");

  if (recv(socket, &result, sizeof(int), 0) <= 0) {
      _exit_with_error(socket, "No se pudo recibir confirmacion", NULL);
  }



  log_info(logger, "Los MD5 concidieron!");
  close(socket);
  exit_gracefully(0);
}

void exit_gracefully(int return_nr) {
  /*
    20.   Siempre llamamos a esta funcion para cerrar el programa.
          Asi solo necesitamos destruir el logger y usar la llamada al
          sistema exit() para terminar la ejecucion
  */

	log_destroy(logger);
	  exit(return_nr);

}
