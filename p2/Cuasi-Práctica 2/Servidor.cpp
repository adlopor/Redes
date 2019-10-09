#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <arpa/inet.h>
#include "Usuario.hpp"
#include "Partida.hpp"
#include "funciones_auxiliares.hpp"

#define TAM_MENSAJE 250
#define MAX_CLIENTES 30
#define FICHERO_USUARIOS "usuarios.txt"
#define FICHERO_FRASES "refranes.txt"
#define MAX_SALA 3

int descriptor;
std::vector<int> clientes;
fd_set set_lectura;

void salirCliente(std::string fichero_usuarios, int descriptor, fd_set &set_lectura, std::vector<int> &clientes, std::vector<Usuario> &usuarios, std::vector<Partida> &partidas, std::vector<Partida> &partidas_individuales);
void manejador (int signum);
bool registro_usuario(std::string fichero_usuarios, std::string nombre_usuario, std::string password);
int comprueba_usuario(std::string fichero_usuarios, std::string usuario, std::string password = "NULL");
void parser(int descriptor, char * linea_comandos, std::string fichero_usuarios, std::vector<int> &clientes, std::vector<Usuario> &usuarios, std::vector<Partida> &partidas, std::vector<Partida> &partidas_individuales);

int main(){

  int nuevo_descriptor;
  struct sockaddr_in cliente, entrante;
  char buffer[TAM_MENSAJE];
  socklen_t longitud_entrante;
  fd_set aux;
  int salida_select;
  int salida;
  int i, j, k;
  int recibidos;
  char identificador[TAM_MENSAJE];
  int reusar_puerto;
  int numero_usuarios = 0, numero_logueando = 0;
  std::vector<Usuario> usuarios;
  std::vector<Partida> partidas, partidas_individuales;

  srand(time(NULL));

  descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if(descriptor == -1){
    std::cerr<<"Error al abrir el socket cliente.\n";
    exit(-1);
  }

  reusar_puerto = 1;
  setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &reusar_puerto, sizeof(reusar_puerto));

  cliente.sin_family = AF_INET;
  cliente.sin_port = htons(2050);
  cliente.sin_addr.s_addr = INADDR_ANY;

  if(bind(descriptor, (struct sockaddr *)&cliente, sizeof(cliente)) == -1){
    std::cerr<<"Error en la operación bind.\n";
    exit(-1);
  }

  longitud_entrante = sizeof(entrante);

  if(listen(descriptor, 1) == -1){
    std::cerr<<"Error en la operación listen.\n";
    exit(-1);
  }

  FD_ZERO(&set_lectura);
  FD_ZERO(&aux);

  FD_SET(0, &set_lectura);
  FD_SET(descriptor, &set_lectura);

  signal(SIGINT, manejador); //<- A implementar después de probar el servidor.

  while(1){

    aux = set_lectura;
    salida_select = select(FD_SETSIZE, &aux, NULL, NULL, NULL);

    if(salida_select > 0){
      for(i = 0; i < FD_SETSIZE; i++){
        if(FD_ISSET(i, &aux)){
          if(i == descriptor){
            if((nuevo_descriptor = accept(descriptor, (struct sockaddr *)&entrante, &longitud_entrante)) == -1){
              std::cerr<<"Error aceptando peticiones.\n";
            }else{
              if(clientes.size() < MAX_CLIENTES){
                clientes.push_back(nuevo_descriptor);
                FD_SET(nuevo_descriptor, &set_lectura);

                envia_mensaje("Bienvenido invitado.\n", nuevo_descriptor);

                for(j = 0; j < clientes.size() - 1; j++){
                  envia_mensaje("Nuevo cliente conectado.\n", clientes[j]);
                }
              }else{
                envia_mensaje("Demasiados clientes conectados.\n", nuevo_descriptor);
                close(nuevo_descriptor);
              }
            }
          }else if(i == 0){
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);

            if(strcmp(buffer, "SALIR\n") == 0){
              for(j = 0; j < clientes.size(); j++){

                envia_mensaje("Desconexión del servidor.\n", clientes[j]);

                close(clientes[j]);
                FD_CLR(clientes[j], &set_lectura);
              }

              close(descriptor);
              exit(-1);
            }
          }else{
            memset(buffer, 0, sizeof(buffer));
            recibidos = recv(i, buffer, sizeof(buffer), 0);
            if(recibidos > 0){
              if(strcmp(buffer, "SALIR\n") == 0){
		            std::cout<<"El socket "<<i<<" ha escrito SALIR.\n";
                salirCliente(FICHERO_USUARIOS, i, set_lectura, clientes, usuarios, partidas, partidas_individuales);
              }else{
                parser(i, buffer, FICHERO_USUARIOS, clientes, usuarios, partidas, partidas_individuales);
              }
            }
            if(recibidos == 0){
              std::cout<<"El socket "<<i<<" ha introducido ctrl+c.\n";
              salirCliente(FICHERO_USUARIOS, i, set_lectura, clientes, usuarios, partidas, partidas_individuales);
            }
          }
        }
      }
    }
  }


  close(descriptor);

  return 0;
}

void salirCliente(std::string fichero_usuarios, int descriptor, fd_set &set_lectura, std::vector<int> &clientes, std::vector<Usuario> &usuarios, std::vector<Partida> &partidas, std::vector<Partida> &partidas_individuales){

  char buffer[TAM_MENSAJE], aux[250];
  int i, j, usr, lg;

  close(descriptor);
  FD_CLR(descriptor, &set_lectura);

  for (i = 0; i < clientes.size(); i++){
    if (clientes[i] == descriptor){
      clientes.erase(clientes.begin() + i);
      break;
    }
  }

  for(i = 0; i < usuarios.size(); i++){
    if(usuarios[i].getDescriptor() == descriptor){
      if(usuarios[i].getEstado() == 2){
        if(usuarios[i].getPartida() != -1){
          if(usuarios[i].esIndividual()){
            partidas_individuales[usuarios[i].getPartida()].elimina_jugador(descriptor);
            partidas_individuales.erase(partidas_individuales.begin() + usuarios[i].getPartida());
            for(j = 0; j < usuarios.size(); j++){
              if(usuarios[j].esIndividual() && usuarios[j].getPartida()>usuarios[i].getPartida()){
                usuarios[j].setPartida(usuarios[i].getPartida() - 1);
              }
            }
          }else{
            sprintf(buffer, "El jugador %s abandona la partida.\n", usuarios[i].getNombre().c_str());
            partidas[usuarios[i].getPartida()].mensaje_jugadores(buffer);
            bool turno = partidas[usuarios[i].getPartida()].esTurno(descriptor);
            partidas[usuarios[i].getPartida()].elimina_jugador(descriptor);
            if(turno){
              if(!partidas[usuarios[i].getPartida()].vacia()){
                partidas[usuarios[i].getPartida()].setTurno(partidas[usuarios[i].getPartida()].getTurno() - 1);
                partidas[usuarios[i].getPartida()].incrementaTurno();
                sprintf(buffer, "Turno de %s.\n", partidas[usuarios[i].getPartida()].tieneTurno().c_str());
                partidas[usuarios[i].getPartida()].mensaje_jugadores(buffer);
              }
            }
            partidas[usuarios[i].getPartida()].mensaje_jugadores(partidas[usuarios[i].getPartida()].getResuelto());
            if(partidas[usuarios[i].getPartida()].vacia()){
              partidas.erase(partidas.begin() + usuarios[i].getPartida());
              for(j = 0; j < usuarios.size(); j++){
                if(!usuarios[j].esIndividual() && usuarios[j].getPartida()>usuarios[i].getPartida()){
                  usuarios[j].setPartida(usuarios[i].getPartida() - 1);
                }
              }
            }
          }
        }
      }
      usuarios.erase(usuarios.begin() + i);
      return;
    }
  }
}

void manejador (int signum){

  int i;
  char buffer[TAM_MENSAJE];

  std::cout<<"\nSe ha recibido la señal sigint\n";

  for(i = 0; i < clientes.size(); i++){

    envia_mensaje("Desconexión del servidor.\n", clientes[i]);

    close(clientes[i]);
    FD_CLR(clientes[i], &set_lectura);
  }

  close(descriptor);
  exit(-1);
}

bool registro_usuario(std::string fichero_usuarios, std::string nombre_usuario, std::string password){

  std::ofstream fich;

  if(comprueba_usuario(fichero_usuarios, nombre_usuario) == -2){
    fich.open(fichero_usuarios.c_str(), std::ios::app);
    fich<<nombre_usuario<<" "<<password<<" 0 0 0\n";
    fich.close();
    return true;
  }

  return false;
}

int comprueba_usuario(std::string fichero_usuarios, std::string usuario, std::string password){

  std::ifstream fich(fichero_usuarios.c_str(), std::ios::in);
  char aux[TAM_MENSAJE];
  int res = 0;

  while(fich.getline(aux, 256, ' ')){
    if(usuario == aux){
      res = 1;
    }
    fich.getline(aux, 256, ' ');
    if(res == 1 && strcmp(password.c_str(), "NULL")){
      if(aux == password){
        return 0; //Coinciden usuario y contraseña.
      }
    }
    if(res == 1){
      return -1; //El usuario existe, pero la contraseña no coincide.
    }
    res = 0;
    fich.getline(aux, 256, ' ');
    fich.getline(aux, 256, ' ');
    fich.getline(aux, 256, '\n');
  }

  fich.close();
  return -2; //El usuario no existe.
}

int login_usuario(std::string fichero_usuarios, std::string usuario, std::string password, std::vector<Usuario> &usuarios, int descriptor){

  int salida, i, puntuacion, aciertos, fallos, salir = 0;
  char aux[250];

  salida = comprueba_usuario(fichero_usuarios, usuario, password);

  if(salida == -2){
    return -3; //El usuario no existe.
  }else if(salida == -1){
    return -2; //El usuario existe, pero la contraseña no coincide.
  }else if(salida == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getNombre() == usuario){
        if(usuarios[i].getEstado() == 1){
          usuarios[i].setEstado(2);
          return 0;
        }else if(usuarios[i].getEstado() == 2){
          return -1;
        }
      }
    }

    std::ifstream fichero(fichero_usuarios, std::ios::in);

    while(fichero.getline(aux, 250, ' ')){
      if(usuario == aux){
        salir = 1;
      }
      fichero.getline(aux, 250, ' ');
      fichero.getline(aux, 250, ' ');
      puntuacion = atoi(aux);
      fichero.getline(aux, 250, ' ');
      aciertos = atoi(aux);
      fichero.getline(aux, 250, '\n');
      fallos = atoi(aux);
      if(salir == 1){
        break;
      }
    }

    fichero.close();

    usuarios.push_back(Usuario(usuario, descriptor, puntuacion, aciertos, fallos, 2));
    return 0;
  }
  return -4; //Error desconocido.
}

void parser(int descriptor, char * linea_comandos, std::string fichero_usuarios, std::vector<int> &clientes, std::vector<Usuario> &usuarios, std::vector<Partida> &partidas, std::vector<Partida> &partidas_individuales){

  char opcion[TAM_MENSAJE], frase_introducida[TAM_MENSAJE], usuario[TAM_MENSAJE], password[TAM_MENSAJE], buffer[TAM_MENSAJE], letra;
  int i, j, aux = 0, salida, puntuacion, aciertos, fallos, salir = 0, encontrado = 0;
  char aux2[250];

  sscanf(linea_comandos, "%s %*s", opcion);
  if(strcmp(opcion, "REGISTRO") == 0){
    sscanf(linea_comandos, "%*s -u %s -p %s", usuario, password);
    if(!strlen(usuario)||!strlen(password)){
      envia_mensaje("-Err. Registro incorrecto.\n", descriptor);
      return;
    }
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor()==descriptor){
        envia_mensaje("-Err. Ya estás logueado.\n", descriptor);
        return;
      }
    }
    if(registro_usuario(fichero_usuarios, usuario, password)){
      envia_mensaje("+Ok. Usuario registrado.\n", descriptor);
      salida = login_usuario(fichero_usuarios, usuario, password, usuarios, descriptor);
      if(salida == 0){
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "+Ok. Logueado como %s.\n", usuario);
        envia_mensaje(buffer, descriptor);
        return;
      }
    }else{
      envia_mensaje("-Err. El usuario ya existe.\n", descriptor);
      return;
    }
  }else if(strcmp(opcion, "USUARIO") == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getEstado() == 1){
          envia_mensaje("-Err. Se espera su password.\n", descriptor);
          return;
        }else if(usuarios[i].getEstado() == 2){
          envia_mensaje("-Err. Ya estás logueado.\n", descriptor);
          return;
        }
      }
    }
    sscanf(linea_comandos, "%*s %s", usuario);
    if(comprueba_usuario(fichero_usuarios, usuario)!=-2){
      for(i = 0; i < usuarios.size(); i++){
        if(usuarios[i].getNombre() == usuario){
          memset(buffer, 0, sizeof(buffer));
          sprintf(buffer, "-Err. El usuario %s ya está logueado.\n", usuario);
          envia_mensaje(buffer, descriptor);
          return;
        }
      }
      envia_mensaje("+Ok. Usuario correcto.\n", descriptor);

      std::ifstream fichero(fichero_usuarios, std::ios::in);

      while(fichero.getline(aux2, 250, ' ')){
        if(!strcmp(usuario,aux2)){
          salir = 1;
        }
        fichero.getline(aux2, 250, ' ');
        fichero.getline(aux2, 250, ' ');
        puntuacion = atoi(aux2);
        fichero.getline(aux2, 250, ' ');
        aciertos = atoi(aux2);
        fichero.getline(aux2, 250, '\n');
        fallos = atoi(aux2);
        if(salir == 1){
          break;
        }
      }

      fichero.close();

      Usuario usr(usuario, descriptor, puntuacion, aciertos, fallos, 1);
      usuarios.push_back(usr);
      return;
    }else{
      envia_mensaje("-Err. Usuario incorrecto.\n", descriptor);
      return;
    }
  }else if(strcmp(opcion, "PASSWORD") == 0){
    sscanf(linea_comandos, "%*s %s", password);
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getEstado()==1){
          salida = login_usuario(fichero_usuarios, usuarios[i].getNombre(), password, usuarios, descriptor);
          if(salida == 0){
            envia_mensaje("+Ok. Login correcto.\n", descriptor);
            sprintf(buffer, "Puntuación: %d\n", usuarios[i].getPuntuacion());
            envia_mensaje(buffer, descriptor);
            return;
          }else if(salida == -2){
            envia_mensaje("-Err. Contraseña incorrecta.\n", descriptor);
            usuarios.erase(usuarios.begin() + i);
            return;
          }
        }else if(usuarios[i].getEstado()==2){
          envia_mensaje("-Err. Ya estás logueado.\n", descriptor);
          return;
        }
      }
    }
    envia_mensaje("-Err. No se estaba iniciando sesion.\n", descriptor);
    return;
  }else if(strcmp(opcion, "PARTIDA_GRUPO") == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getEstado() == 2){
          if(usuarios[i].getPartida() == -1){
            for(j = 0; j < partidas.size(); j++){
              if(!partidas[j].llena()){
                usuarios[i].setPartida(j);
                usuarios[i].setGrupo();
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "+Ok. Entras a la sala %d.\n", j);
                envia_mensaje(buffer, descriptor);
                envia_mensaje("Esperando jugadores...\n", descriptor);
                if(partidas[j].llena()){
                  if(!partidas[j].enCurso()){
                    partidas[j].comienza_partida();
                    partidas[j].mensaje_jugadores("+Ok. La partida da comienzo.\n");
                    sprintf(buffer, "Turno de %s.\n", partidas[j].tieneTurno().c_str());
                    partidas[j].mensaje_jugadores(buffer);
                    partidas[j].mensaje_jugadores(partidas[j].getResuelto());
                  }
                }
                return;
              }
            }
            usuarios[i].setPartida(partidas.size());
            usuarios[i].setGrupo();
            partidas.push_back(Partida(getFrase(FICHERO_FRASES), MAX_SALA));
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "+Ok. Entras a la sala %d.\n", j);
            envia_mensaje(buffer, descriptor);
            envia_mensaje("Esperando jugadores...\n", descriptor);
            return;
          }else{
            memset(buffer, 0, sizeof(buffer));
            if(usuarios[i].esIndividual()){
              sprintf(buffer, "-Err. Ya estás en una sala (sala individual %d).\n", usuarios[i].getPartida());
            }else{
              sprintf(buffer, "-Err. Ya estás en una sala (sala %d).\n", usuarios[i].getPartida());
            }
            envia_mensaje(buffer, descriptor);
            return;
          }
        }else{
          envia_mensaje("-Err. Se espera su password.\n", descriptor);
        }
      }
    }
    envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
    return;
  }else if(strcmp(opcion, "PARTIDA_INDIVIDUAL") == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getEstado() == 2){
          if(usuarios[i].getPartida() == -1){
            usuarios[i].setPartida(partidas_individuales.size());
            usuarios[i].setIndividual();
            partidas_individuales.push_back(Partida(getFrase(FICHERO_FRASES), 1));
            partidas_individuales[partidas_individuales.size()-1].add_jugador(new Usuario(usuarios[i]));
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "+Ok. Entras a la sala %d.\n", (int)partidas_individuales.size()-1);
            envia_mensaje(buffer, descriptor);
            partidas_individuales[partidas_individuales.size()-1].comienza_partida();
            partidas_individuales[partidas_individuales.size()-1].mensaje_jugadores("+Ok. La partida da comienzo.\n");
            partidas_individuales[partidas_individuales.size()-1].mensaje_jugadores(partidas_individuales[partidas_individuales.size()-1].getResuelto());
            return;
          }else{
            memset(buffer, 0, sizeof(buffer));
            if(usuarios[i].esIndividual()){
              sprintf(buffer, "-Err. Ya estás en una sala (sala individual %d).\n", usuarios[i].getPartida());
            }else{
              sprintf(buffer, "-Err. Ya estás en una sala (sala %d).\n", usuarios[i].getPartida());
            }
            envia_mensaje(buffer, descriptor);
            return;
          }
        }else{
          envia_mensaje("-Err. Se espera su password.\n", descriptor);
        }
      }
    }
    envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
    return;
  }else if(strcmp(opcion, "VOCAL") == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor()==descriptor){
        encontrado = 1;
        if(usuarios[i].getEstado()==2){
          if(usuarios[i].getPartida()!=-1){
            if(usuarios[i].esIndividual()){
              sscanf(linea_comandos, "%*s %c", &letra);
              if(!es_vocal(letra)){
                envia_mensaje("-Err. No es vocal.\n", descriptor);
                return;
              }else{
                break;
              }
            }else{
              if(!partidas[usuarios[i].getPartida()].esTurno(descriptor)){
                envia_mensaje("-Err. No es tu turno.\n", descriptor);
                return;
              }else{
                sscanf(linea_comandos, "%*s %c", &letra);
                if(!es_vocal(letra)){
                  envia_mensaje("-Err. No es vocal.\n", descriptor);
                  return;
                }else{
                  if(partidas[usuarios[i].getPartida()].getPuntuacionJugador(descriptor)>=50){
                    break;
                  }else{
                    envia_mensaje("-Err. No tienes suficientes puntos.\n", descriptor);
                    return;
                  }
                }
              }
            }
          }else{
            envia_mensaje("-Err. No estás en ninguna partida.\n", descriptor);
            return;
          }
        }else{
          envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
          return;
        }
      }
    }
    if(!encontrado){
      envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
      return;
    }
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getPartida() < 0){
          envia_mensaje("-Err. No estás en ninguna partida.\n", descriptor);
          return;
        }else{
          if(usuarios[i].esIndividual()){
            partidas_individuales[usuarios[i].getPartida()].comprueba_letra(letra, descriptor);
          }else{
            partidas[usuarios[i].getPartida()].comprueba_letra(letra, descriptor);
          }
          break;
        }
      }
    }
    if(usuarios[i].esIndividual()){
      if(partidas_individuales[usuarios[i].getPartida()].vacia()){
        int aux_partida = usuarios[i].getPartida();
        partidas_individuales.erase(partidas_individuales.begin() + usuarios[i].getPartida());
        for(j = 0; j < usuarios.size(); j++){
          if(usuarios[j].esIndividual() && usuarios[j].getPartida() == aux_partida){
            actualiza_usuario(FICHERO_USUARIOS, usuarios[j]);
            usuarios[j].setPartida(-1);
          }else if(usuarios[j].esIndividual() && usuarios[j].getPartida()>aux_partida){
            usuarios[j].setPartida(usuarios[j].getPartida() - 1);
          }
        }
      }
    }else{
      if(partidas[usuarios[i].getPartida()].vacia()){
        int aux_partida = usuarios[i].getPartida();
        partidas.erase(partidas.begin() + usuarios[i].getPartida());
        for(j = 0; j < usuarios.size(); j++){
          if(!usuarios[j].esIndividual() && usuarios[j].getPartida() == aux_partida){
            actualiza_usuario(FICHERO_USUARIOS, usuarios[j]);
            usuarios[j].setPartida(-1);
          }else if(!usuarios[j].esIndividual() && usuarios[j].getPartida()>aux_partida){
            usuarios[j].setPartida(usuarios[j].getPartida() - 1);
          }
        }
      }
    }
    return;
  }else if(strcmp(opcion, "CONSONANTE") == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor()==descriptor){
        encontrado = 1;
        if(usuarios[i].getEstado()==2){
          if(usuarios[i].getPartida()!=-1){
            if(usuarios[i].esIndividual()){
              sscanf(linea_comandos, "%*s %c", &letra);
              if(es_vocal(letra)){
                envia_mensaje("-Err. No es consonante.\n", descriptor);
                return;
              }else{
                break;
              }
            }else{
              if(!partidas[usuarios[i].getPartida()].esTurno(descriptor)){
                envia_mensaje("-Err. No es tu turno.\n", descriptor);
                return;
              }else{
                sscanf(linea_comandos, "%*s %c", &letra);
                if(es_vocal(letra)){
                  envia_mensaje("-Err. No es consonante.\n", descriptor);
                  return;
                }else{
                  break;
                }
              }
            }
          }else{
            envia_mensaje("-Err. No estás en ninguna partida.\n", descriptor);
            return;
          }
        }else{
          envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
          return;
        }
      }
    }
    if(!encontrado){
      envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
      return;
    }
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getPartida() < 0){
          envia_mensaje("-Err. No estás en ninguna partida.\n", descriptor);
          return;
        }else{
          if(usuarios[i].esIndividual()){
            partidas_individuales[usuarios[i].getPartida()].comprueba_letra(letra, descriptor);
          }else{
            partidas[usuarios[i].getPartida()].comprueba_letra(letra, descriptor);
          }
          break;
        }
      }
    }
    if(usuarios[i].esIndividual()){
      if(partidas_individuales[usuarios[i].getPartida()].vacia()){
        int aux_partida = usuarios[i].getPartida();
        partidas_individuales.erase(partidas_individuales.begin() + usuarios[i].getPartida());
        for(j = 0; j < usuarios.size(); j++){
          if(usuarios[j].esIndividual() && usuarios[j].getPartida() == aux_partida){
            actualiza_usuario(FICHERO_USUARIOS, usuarios[j]);
            usuarios[j].setPartida(-1);
          }else if(usuarios[j].esIndividual() && usuarios[j].getPartida()>aux_partida){
            usuarios[j].setPartida(usuarios[j].getPartida() - 1);
          }
        }
      }
    }else{
      if(partidas[usuarios[i].getPartida()].vacia()){
        int aux_partida = usuarios[i].getPartida();
        partidas.erase(partidas.begin() + usuarios[i].getPartida());
        for(j = 0; j < usuarios.size(); j++){
          if(!usuarios[j].esIndividual() && usuarios[j].getPartida() == aux_partida){
            actualiza_usuario(FICHERO_USUARIOS, usuarios[j]);
            usuarios[j].setPartida(-1);
          }else if(!usuarios[j].esIndividual() && usuarios[j].getPartida()>aux_partida){
            usuarios[j].setPartida(usuarios[j].getPartida() - 1);
          }
        }
      }
    }
    return;
  }else if(strcmp(opcion, "RESOLVER") == 0){
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor()==descriptor){
        encontrado = 1;
        if(usuarios[i].getEstado()==2){
          if(usuarios[i].getPartida()!=-1){
            if(!usuarios[i].esIndividual()){
              if(!partidas[usuarios[i].getPartida()].esTurno(descriptor)){
                envia_mensaje("-Err. No es tu turno.\n", descriptor);
                return;
              }else{
                break;
              }
            }else{
              break;
            }
          }else{
            envia_mensaje("-Err. No estás en ninguna partida.\n", descriptor);
            return;
          }
        }else{
          envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
          return;
        }
      }
    }
    if(!encontrado){
      envia_mensaje("-Err. Necesitas hacer login.\n", descriptor);
      return;
    }
    std::stringstream auxstream;
    auxstream << linea_comandos;
    auxstream.getline(aux2, 250, ' ');
    auxstream.getline(frase_introducida, 250, '\n');
    for(i = 0; i < usuarios.size(); i++){
      if(usuarios[i].getDescriptor() == descriptor){
        if(usuarios[i].getPartida() < 0){
          envia_mensaje("-Err. No estás en ninguna partida.\n", descriptor);
          return;
        }else{
          if(usuarios[i].esIndividual()){
            partidas_individuales[usuarios[i].getPartida()].resolver(frase_introducida, descriptor);
          }else{
            partidas[usuarios[i].getPartida()].resolver(frase_introducida, descriptor);
          }
          break;
        }
      }
    }

    if(usuarios[i].esIndividual()){
      if(partidas_individuales[usuarios[i].getPartida()].vacia()){
        int aux_partida = usuarios[i].getPartida();
        partidas_individuales.erase(partidas_individuales.begin() + aux_partida);
        for(j = 0; j < usuarios.size(); j++){
          if(usuarios[j].esIndividual() && usuarios[j].getPartida() == aux_partida){
            actualiza_usuario(FICHERO_USUARIOS, usuarios[j]);
            usuarios[j].setPartida(-1);
          }else if(usuarios[j].esIndividual() && usuarios[j].getPartida()>aux_partida){
            usuarios[j].setPartida(usuarios[j].getPartida() - 1);
          }
        }
      }
    }else{
      if(partidas[usuarios[i].getPartida()].vacia()){
        int aux_partida = usuarios[i].getPartida();
        partidas.erase(partidas.begin() + usuarios[i].getPartida());
        for(j = 0; j < usuarios.size(); j++){
          if(!usuarios[j].esIndividual() && usuarios[j].getPartida() == aux_partida){
            actualiza_usuario(FICHERO_USUARIOS, usuarios[j]);
            usuarios[j].setPartida(-1);
          }else if(!usuarios[j].esIndividual() && usuarios[j].getPartida()>aux_partida){
            usuarios[j].setPartida(usuarios[j].getPartida() - 1);
          }
        }
      }
    }
    return;
  }else{
    envia_mensaje("-Err. Opción incorrecta.\n", descriptor);
    return;
  }
}
