#include <string>
#include <vector>

void envia_mensaje(std::string mensaje, int descriptor);
bool es_vocal(char letra);
bool es_consonante(char letra);
std::string getFrase(std::string fichero_frases);
std::string resuelto(std::string frase, std::vector<int> descubiertas);
void actualiza_fichero(std::string fichero_usuarios, Usuario jugador);
void actualiza_usuario(std::string fichero_usuarios, Usuario &jugador);
std::string mayus(std::string frase);
