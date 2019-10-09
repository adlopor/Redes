#ifndef PARTIDA_HPP
#define PARTIDA_HPP

#include <string>
#include <vector>
#include "funciones_auxiliares.hpp"
#include "Usuario.hpp"

class Partida{
  private:
    std::vector<Usuario *> jugadores;
    std::string frase;
    int maximo_jugadores;
    int turno = 0;
    bool partida_en_curso = false;
    std::vector<int> resueltas;
    std::vector<char> letras_probadas;

  public:
    Partida(std::string frase_juego, int max);
    bool esTurno(int descriptor);
    inline void comienza_partida(){partida_en_curso = true;}
    inline bool enCurso(){return partida_en_curso;}
    inline int getMax(){return maximo_jugadores;}
    int getPuntuacionJugador(int descriptor);
    inline int getTurno(){return turno;}
    std::string tieneTurno();
    inline std::string getResuelto(){return resuelto(frase, resueltas);}
    inline void incrementaTurno(){turno = (turno+1)%jugadores.size();}
    inline void setTurno(int valor_turno){turno = valor_turno;}
    inline bool llena(){return jugadores.size() == maximo_jugadores;}
    inline bool vacia(){return jugadores.size() == 0;}
    int add_jugador(Usuario * jugador);
    bool elimina_jugador(int descriptor);
    void comprueba_letra(char letra_introducida, int descriptor);
    void resolver(std::string frase_introducida, int descriptor);
    void mensaje_jugadores(std::string mensaje);
    void fin_partida(std::string fichero_usuarios, int descriptor, int victoria = 0);
};

#endif
