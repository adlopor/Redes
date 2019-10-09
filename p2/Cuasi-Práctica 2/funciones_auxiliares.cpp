#include <iostream>
#include <ctype.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include "Usuario.hpp"
#include "funciones_auxiliares.hpp"

#define TAM_MENSAJE 250

void envia_mensaje(std::string mensaje, int descriptor){

  char buffer[250];

  memset(buffer, 0, sizeof(buffer));
  strcpy(buffer, mensaje.c_str());
  send(descriptor, buffer, strlen(buffer), 0);

}

bool es_vocal(char letra){

  std::string vocales = "AEIOU";
  int i;

  for(i = 0; i < vocales.size(); i++){
    if(vocales[i] == toupper(letra)){
      return true;
    }
  }
  return false;
}

bool es_consonante(char letra){

  if(isalpha(letra) && !es_vocal(letra)){
    return true;
  }else{
    return false;
  }

}

std::string getFrase(std::string fichero_frases){

  std::ifstream fichero(fichero_frases.c_str(), std::ios::in);
  char aux[TAM_MENSAJE];
  int i, count=0, pos;

  while(fichero.getline(aux, 256, '\n')){
    count++;
  }

  pos = rand()%count + 1;

  fichero.clear();
  fichero.seekg(0, std::ios::beg);

  for(i = 0; i < pos; i++){
    fichero.getline(aux, 256, '\n');
  }

  fichero.close();
  return aux;
}

std::string resuelto(std::string frase, std::vector<int>descubiertas){
  int i;
  std::stringstream aux;

  for(i = 0; i < frase.size(); i++){
    if(isalpha(frase[i])){
      if(descubiertas[i]==1){
        aux<<frase[i];
      }else{
        aux<<"_";
      }
    }else{
      aux<<frase[i];
    }
  }
  aux<<"\n";
  return aux.str();
}

void actualiza_fichero(std::string fichero_usuarios, Usuario jugador){

  char aux[250];

  std::ifstream fichero(fichero_usuarios, std::ios::in);
  std::ofstream fichero2("aux.txt", std::ios::out);

  while(fichero.getline(aux, 250, ' ')){
    fichero2 << aux << " ";
    if(!strcmp(aux, jugador.getNombre().c_str())){
      fichero.getline(aux, 250, ' ');
      fichero2 << aux << " ";
      fichero2 << jugador.getPuntuacion() << " ";
      fichero2 << jugador.getAciertos() << " ";
      fichero2 << jugador.getFallos() << "\n";
      fichero.getline(aux, 250, ' ');
      fichero.getline(aux, 250, ' ');
      fichero.getline(aux, 250, '\n');
    }else{
      fichero.getline(aux, 250, ' ');
      fichero2 << aux << " ";
      fichero.getline(aux, 250, ' ');
      fichero2 << aux << " ";
      fichero.getline(aux, 250, ' ');
      fichero2 << aux << " ";
      fichero.getline(aux, 250, '\n');
      fichero2 << aux << "\n";
    }
    memset(aux, 0, sizeof(aux));
  }

  fichero.close();
  fichero2.close();

  remove(fichero_usuarios.c_str());
  rename("aux.txt", fichero_usuarios.c_str());
}

void actualiza_usuario(std::string fichero_usuarios, Usuario &jugador){

  char aux[250];
  int salir;

  std::ifstream fichero(fichero_usuarios, std::ios::in);

  while(fichero.getline(aux, 250, ' ')){
    if(jugador.getNombre() == aux){
      salir = 1;
    }
    fichero.getline(aux, 250, ' ');
    fichero.getline(aux, 250, ' ');
    jugador.setPuntuacion(atoi(aux));
    fichero.getline(aux, 250, ' ');
    jugador.setAciertos(atoi(aux));
    fichero.getline(aux, 250, '\n');
    jugador.setFallos(atoi(aux));
    if(salir == 1){
      break;
    }
  }

  fichero.close();
}

std::string mayus(std::string frase){

  std::stringstream stream;
  int i;

  for(i = 0; i < frase.size(); i++){
    if(isalpha(frase[i])){
      stream<<toupper(frase[i]);
    }else{
      stream<<frase[i];
    }
  }

  return stream.str();
}
