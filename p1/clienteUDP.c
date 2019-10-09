#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

/* -------------------------------------------------------------------------------------- 
Enviamos el tipo de servicio que deseamos al servidor
---------------------------------------------------------------------------------------- */
 
int main (){


/*---------------------------------------------------- 
Descriptor del socket y buffer para datos 
-----------------------------------------------------*/

	int Socket_Cliente;
	char buffer[50];
	char mensaje_recibido[50];
/* -----------------------------------------------------
Información del Servidor
------------------------------------------------------*/
	
	struct sockaddr_in Servidor;  
	socklen_t Longitud_Servidor;

/* --------------------------------------------------
Se abre el socket cliente
---------------------------------------------------*/

	Socket_Cliente = socket (AF_INET, SOCK_DGRAM, 0);
	if (Socket_Cliente == -1){
	
		printf ("No se puede abrir el socket cliente\n");
    		exit (-1);	
	
	}

/*---------------------------------------------------------------------
Necesitamos una estructura con la información del Servidor
para poder solicitarle un servicio.
----------------------------------------------------------------------*/

		Servidor.sin_family = AF_INET;
		Servidor.sin_port = htons(2000);
		Servidor.sin_addr.s_addr = inet_addr("127.0.0.1");
  	 	Longitud_Servidor = sizeof(Servidor);

/*------------------------------------------------------------------------
Se pide por pantalla que quiere imprimirse: d(el dia), 
t(la hora) o f (el dia y la hora)
----------------------------------------------------------------------- */

	char opcion;
	
	printf("[CLIENTE]: Introduce que quieres que el servidor te envie el DAY(dia), la TIME(tiempo) o DAYTIME(ambas cosas):\n");
	scanf("%s",buffer);

	
/*-----------------------------------------------------------------------
Se envía el mensaje al Servidor
-----------------------------------------------------------------------*/

	int enviado = sendto (Socket_Cliente,buffer, sizeof(buffer), 0,
			      (struct sockaddr *) &Servidor, Longitud_Servidor);
					  
	if (enviado < 0)
		printf("Error al solicitar el servicio\n");
	
    	else{
   
		/*----------------------------------------------------------------------
		  Esperamos la respuesta del Servidor
----------------------------------------------------------------------- */ 
		
		// ***Aqui metemos lo de esperar 5 segundos***
			
		struct timeval timeout;
		fd_set lectura;
		int salida;
	
		//Inicializar la estructua timeval
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		printf("socket cliente: %d\n",Socket_Cliente);
		//Inicializar los conjuntos fd_set
		FD_ZERO(&lectura);
		FD_SET(Socket_Cliente,&lectura);
	
		int i=0;
		while(i<3){
		
			salida = select(Socket_Cliente+1,&lectura,NULL,NULL,&timeout);
		
			if(salida == -1){
		
				printf("Se ha producido un error en select\n");
				break;
			}
			
			else if(salida == 0){
		
				printf("Se ha agotado el tiempo\n");
			
				i++;
				
				if(i<3)
				{
					printf("Reenviando intento %d...\n",i);
					int enviado = sendto (Socket_Cliente,buffer, sizeof(buffer), 0,
			      (struct sockaddr *) &Servidor, Longitud_Servidor);
				}	
				if(i==3){
					printf("Agotado ultimo intento\n");
					
					exit(-1);
				}
				
			}
		
			else{
		
				printf("Se ha escrito algo en el buffer\n");
		
				break;
			}
	
		}

	}
			
			
		//Si se pasa el tiempo se vuelve a mandar el mensaje otra vez asi 3 veces
		
		//Si despues del tercer intento no recibe respuesta se imprime mensaje de error	
		
		int recibido = recvfrom (Socket_Cliente, mensaje_recibido,
					 sizeof(mensaje_recibido), 0,(struct sockaddr *) &Servidor,
					 &Longitud_Servidor);
			
   		if (recibido > 0)
      		printf ("Leido %s\n", mensaje_recibido);
      		
   		else
      		printf ("Error al leer del servidor\n");
		
	
		
	//Cerramos el socket
	close(Socket_Cliente);					  

return 0;
}
