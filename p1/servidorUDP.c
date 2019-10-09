#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


/*
 * El servidor ofrece el servicio de incrementar un número recibido de un cliente
 */
 
int main (){

/*---------------------------------------------------- 
	Descriptor del socket y buffer de datos                
-----------------------------------------------------*/

//Declaracion de variables

	int Socket_Servidor;
	struct sockaddr_in Servidor;
	
	char envio[50];
	char buffer[50];
	
	time_t tiempo;
	struct tm *stTm;
	
	tiempo = time(NULL);	
	
/* -----------------------------------------------------
	Información del Cliente
-----------------------------------------------------*/

	struct sockaddr_in Cliente;
	socklen_t Longitud_Cliente;
		
/* --------------------------------------------------
	Se abre el socket Servidor
---------------------------------------------------*/

	Socket_Servidor = socket (AF_INET, SOCK_DGRAM, 0);
	if (Socket_Servidor == -1){
	
		printf ("No se puede abrir Socket_Servidor(Descriptor del socket)\n");
		exit (-1);	
	}

/* ------------------------------------------------------------------
	Se rellenan los campos de la estructura servidor, necesaria
	para la llamada a la funcion bind().
-------------------------------------------------------------------*/

	Servidor.sin_family = AF_INET;
	Servidor.sin_port = htons(2000);
	Servidor.sin_addr.s_addr = htonl(INADDR_ANY); 

	if (bind (Socket_Servidor, (struct sockaddr *)&Servidor, sizeof (Servidor)) == -1){

		close (Socket_Servidor);
		exit (-1);
	}

/*---------------------------------------------------------------------
	Del cliente sólo necesitamos el tamaño de su estructura, el 
	resto de información (familia, puerto, ip), la obtendremos 
	nos la proporcionará el propio método recvfrom(), cuando
	recibamos la llamada de un cliente.
----------------------------------------------------------------------*/

	Longitud_Cliente = sizeof (Cliente);

/*-----------------------------------------------------------------------
	El servidor espera continuamente los mensajes de los clientes
------------------------------------------------------------------------ */

	while (1){

    /* -----------------------------------------------------------------
		Esperamos la llamada de algún cliente
	-------------------------------------------------------------------*/

     	int recibido = recvfrom (Socket_Servidor, buffer, sizeof(buffer), 0,
				 (struct sockaddr *) &Cliente, &Longitud_Cliente);

	/* -----------------------------------------------------------------
		Comprobamos si hemos recibido alguna información 
	-------------------------------------------------------------------*/
	printf("%s\n",buffer);
		if (recibido > 0){

		/*-----------------------------------------------------------------------------
			Vemos que nos ha pedido el cliente y creamos el mensaje de respuesta 
		-----------------------------------------------------------------------------*/
			
			if(strcmp(buffer,"DAY")==0){
					
					stTm = localtime(&tiempo);
				
					strftime(envio,50,"%A, %d de %B de %Y", stTm);
					printf("Linea 105\n");
			}
				
			else if(strcmp(buffer,"TIME")==0){
					
					stTm = localtime(&tiempo);
					
					strftime(envio,50,"%H:%M:%S",stTm);
					
			}
				
			else if(strcmp(buffer,"DAYTIME")==0){
					
					stTm = localtime(&tiempo);
					
					strftime(envio,50,"%A, %d de %B de %Y; %H:%M:%S",stTm);
					
			}
				
			else{
		
				printf("Error en el if-else. Saliendo del programa...\n");
				exit(-1);
		
			}
					
      
      	/* ------------------------------------------------------------------
			Devolvemos el número incrementado al cliente
		--------------------------------------------------------------------*/
			printf("envio: %s\n",envio);
			int enviado = sendto (Socket_Servidor, envio, sizeof(envio), 0,
					      (struct sockaddr *) &Cliente, Longitud_Cliente);

		}

	}
	 
	close(Socket_Servidor);

	return 0;

}
