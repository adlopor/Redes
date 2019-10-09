#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define TAM_MENSAJE 250

int main(int argc, char ** argv){

  int descriptor;
  struct sockaddr_in servidor;
  char buffer[TAM_MENSAJE];
  socklen_t longitud_socket;
  fd_set set_lectura, aux;
  int salir = 0;

  descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if(descriptor == -1){
    std::cerr<<"Error al abrir el socket cliente.\n";
    exit(-1);
  }

  servidor.sin_family = AF_INET;
  servidor.sin_port = htons(2050);
  servidor.sin_addr.s_addr = inet_addr(argv[1]);

  longitud_socket = sizeof(servidor);

  if(connect(descriptor, (struct sockaddr *)&servidor, longitud_socket) == -1){
    std::cerr<<"Error al establecer conexión con el servidor.\n";
    exit(-1);
  }

  FD_ZERO(&aux);
  FD_ZERO(&set_lectura);

  FD_SET(0, &set_lectura);
  FD_SET(descriptor, &set_lectura);

  do{
    aux = set_lectura;
    if(select(descriptor+1, &aux, NULL, NULL, NULL) == -1){
      std::cerr<<"Ha ocurrido un error en la función select.\n";
      exit(-1);
    }

    if(FD_ISSET(descriptor, &aux)){
      memset(buffer, 0, sizeof(buffer));
      recv(descriptor, buffer, sizeof(buffer), 0);

      std::cout<<buffer<<"\n";

      if(strcmp(buffer, "Demasiados clientes conectados.\n") == 0){
        salir = 1;
      }else if(strcmp(buffer, "Desconexión del servidor.\n") == 0){
        salir = 1;
      }
    }else{
      memset(buffer, 0, sizeof(buffer));
      fgets(buffer, sizeof(buffer), stdin);

      if(strcmp(buffer, "SALIR\n") == 0){
        salir = 1;
      }

      send(descriptor, buffer, sizeof(buffer), 0);
    }
  }while(salir == 0);

  close(descriptor);

  return 0;
}
