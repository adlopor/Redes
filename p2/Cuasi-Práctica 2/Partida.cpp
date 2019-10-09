#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Usuario.hpp"
#include "Partida.hpp"
#include "funciones_auxiliares.hpp"

#define FICHERO_USUARIOS "usuarios.txt"

Partida::Partida(std::string frase_juego, int max){

  int i;

  frase = frase_juego;
  resueltas = std::vector<int>(frase.size(), 0);
  maximo_jugadores = max;

  for(i = 0; i < frase.size(); i++){
    if(!isalpha(frase[i])){
      resueltas[i] = 1;
    }
  }
}

bool Partida::esTurno(int descriptor){

  if(jugadores[getTurno()]->getDescriptor() == descriptor){
    return true;
  }else{
    return false;
  }

}

std::string Partida::tieneTurno(){
  return jugadores[getTurno()]->getNombre();
}

int Partida::add_jugador(Usuario * jugador){

  int i;

  if(jugadores.size()>=getMax()){
    return -1;
  }else{
    for(i = 0; i < jugadores.size(); i++){
      if(jugadores[i]->getDescriptor()==jugador->getDescriptor()){
        return -2;
      }
    }
    jugadores.push_back(jugador);
    return 0;
  }
}

bool Partida::elimina_jugador(int descriptor){

  int i;

  for(i = 0; i < jugadores.size(); i++){
    if(jugadores[i]->getDescriptor()==descriptor){
      jugadores.erase(jugadores.begin() + i);
      return true;
    }
  }
  return false;
}

int Partida::getPuntuacionJugador(int descriptor){

  int i;

  for(i = 0; i < jugadores.size(); i++){
    if(jugadores[i]->getDescriptor()==descriptor){
      return jugadores[i]->getPuntuacionPartida();
    }
  }
}

void Partida::comprueba_letra(char letra_introducida, int descriptor){

  int i, j, cont = 0, acierto = 0, puntos = 0;
  char buffer[250];

  for(i = 0; i < letras_probadas.size(); i++){
    if(letras_probadas[i] == toupper(letra_introducida)){
      envia_mensaje("-Err. Letra ya introducida.\n", descriptor);
      return;
    }
  }
  letras_probadas.push_back(toupper(letra_introducida));

  if(es_vocal(letra_introducida)){
    for(i = 0; i < jugadores.size(); i++){
      if(jugadores[i]->getDescriptor() == descriptor){
        jugadores[i]->aumentaPuntuacionPartida(-50);
        sprintf(buffer, "El jugador %s gasta 50 puntos.\n", jugadores[i]->getNombre().c_str());
        mensaje_jugadores(buffer);
        break;
      }
    }
  }

  for(i = 0; i < jugadores.size(); i++){
    if(jugadores[i]->getDescriptor() == descriptor){
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "El jugador %s introduce la letra %c.\n", jugadores[i]->getNombre().c_str(), letra_introducida);
      break;
    }
  }

  jugadores[i]->aumentaIntentos();
  mensaje_jugadores(buffer);

  for(j = 0; j < frase.size(); j++){
    if(toupper(frase[j]) == toupper(letra_introducida)){
      resueltas[j] = 1;
      acierto = 1;
      if(!jugadores[i]->esIndividual()){
        if(es_consonante(letra_introducida)){
          jugadores[i]->aumentaPuntuacionPartida(50);
          puntos+=50;
        }
      }
    }
    if(resueltas[j] == 1){
      cont++;
    }
  }
  if(cont == frase.size()){
    fin_partida(FICHERO_USUARIOS, descriptor, 1);
  }else if(acierto == 1){
    mensaje_jugadores("Acierto!!!\n");
    if(puntos){
      sprintf(buffer, "El jugador %s gana %d puntos.\n", jugadores[i]->getNombre().c_str(), puntos);
      mensaje_jugadores(buffer);
    }
    mensaje_jugadores(getResuelto());
  }else{
    mensaje_jugadores("No es correcto... :(\n");
    if(getMax()!=1){
      incrementaTurno();
      sprintf(buffer, "Turno de %s.\n", jugadores[getTurno()]->getNombre().c_str());
      mensaje_jugadores(buffer);
    }
    mensaje_jugadores(getResuelto());
  }
}

void Partida::resolver(std::string frase_introducida, int descriptor){

  char buffer[250];
  int i, pos_ganador = 0;

  for(i = 0; i < jugadores.size(); i++){
    if(jugadores[i]->getDescriptor() == descriptor){
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "El jugador %s cree que la respuesta es \"%s\"\n", jugadores[i]->getNombre().c_str(), frase_introducida.c_str());
      break;
    }
  }

  mensaje_jugadores(buffer);

  if(mayus(frase_introducida) == mayus(frase)){
      jugadores[i]->aumentaAciertos();
      fin_partida(FICHERO_USUARIOS, descriptor, 1);
  }else{
    jugadores[i]->aumentaFallos();
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "El jugador %s falló!\n", jugadores[i]->getNombre().c_str());
    mensaje_jugadores(buffer);
    if(jugadores[i]->esIndividual()){
      fin_partida(FICHERO_USUARIOS, jugadores[i]->getDescriptor());
    }else{
      for(i = 1; i < jugadores.size(); i++){
        if(jugadores[i]->getPuntuacion() > jugadores[pos_ganador]->getPuntuacion()){
          pos_ganador = i;
        }
      }
      fin_partida(FICHERO_USUARIOS, jugadores[pos_ganador]->getDescriptor());
    }
  }
}

void Partida::mensaje_jugadores(std::string mensaje){

  int i;
  char buffer[250];

  for(i = 0; i < jugadores.size(); i++){
    envia_mensaje(mensaje, jugadores[i]->getDescriptor());
  }
}

void Partida::fin_partida(std::string fichero_usuarios, int descriptor, int victoria){

  int i;
  char buffer[250];

  for(i = 0; i < jugadores.size(); i++){
    if(jugadores[i]->getDescriptor() == descriptor){
      if(!jugadores[i]->esIndividual()){
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "El jugador %s ha ganado!!!\n", jugadores[i]->getNombre().c_str());
        mensaje_jugadores(buffer);
      }
      break;
    }
  }

  if(getMax()==1){
    if(victoria){
      envia_mensaje("+Ok. Enhorabuena.\n", jugadores[0]->getDescriptor());
    }else{
      envia_mensaje("+Ok. Le deseamos mejor suerte la próxima vez.\n", jugadores[0]->getDescriptor());
    }
    sprintf(buffer, "Aciertos: %d\n", jugadores[0]->getAciertos());
    envia_mensaje(buffer, jugadores[0]->getDescriptor());
    sprintf(buffer, "Fallos: %d\n", jugadores[0]->getFallos());
    envia_mensaje(buffer, jugadores[0]->getDescriptor());
    if(jugadores[0]->getIntentos()<5){
      jugadores[0]->setPuntuacionPartida(150);
      jugadores[0]->aumentaPuntuacion(150);
    }else if(jugadores[0]->getIntentos()<8){
      jugadores[0]->setPuntuacionPartida(100);
      jugadores[0]->aumentaPuntuacion(100);
    }else if(jugadores[0]->getIntentos()<11){
      jugadores[0]->setPuntuacionPartida(70);
      jugadores[0]->aumentaPuntuacion(70);
    }else if(jugadores[0]->getIntentos()<15){
      jugadores[0]->setPuntuacionPartida(50);
      jugadores[0]->aumentaPuntuacion(50);
    }else{
      jugadores[0]->setPuntuacionPartida(0);
    }
    jugadores[0]->setIntentos(0);
    sprintf(buffer, "Puntuación de la partida: %d\n", jugadores[0]->getPuntuacionPartida());
    envia_mensaje(buffer, jugadores[0]->getDescriptor());
    sprintf(buffer, "Puntuación total: %d\n", jugadores[0]->getPuntuacion());
    envia_mensaje(buffer, jugadores[0]->getDescriptor());
    actualiza_fichero(FICHERO_USUARIOS, *(jugadores[0]));
    elimina_jugador(jugadores[0]->getDescriptor());
  }else{
    int aux3 = jugadores.size();
    for(i = 0; i < aux3; i++){
      if(victoria){
        if(jugadores[i]->getDescriptor()!=descriptor){
          jugadores[i]->setPuntuacionPartida(0);
        }
      }
      sprintf(buffer, "Aciertos: %d\n", jugadores[i]->getAciertos());
      envia_mensaje(buffer, jugadores[i]->getDescriptor());
      sprintf(buffer, "Fallos: %d\n", jugadores[i]->getFallos());
      envia_mensaje(buffer, jugadores[i]->getDescriptor());
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "Puntuación de la partida: %d\n", jugadores[i]->getPuntuacionPartida());
      envia_mensaje(buffer, jugadores[i]->getDescriptor());
      jugadores[i]->aumentaPuntuacion(jugadores[i]->getPuntuacionPartida());
      sprintf(buffer, "Puntuación total: %d\n", jugadores[i]->getPuntuacion());
      envia_mensaje(buffer, jugadores[i]->getDescriptor());
      jugadores[i]->setPuntuacionPartida(0);
      jugadores[i]->setIntentos(0);
    }
    for(i = 0; i < aux3; i++){
      actualiza_fichero(FICHERO_USUARIOS, *(jugadores[0]));
      elimina_jugador(jugadores[0]->getDescriptor());
    }
  }

  return;
}
