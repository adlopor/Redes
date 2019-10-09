#ifndef TABLERO_HPP
#define TABLERO_HPP

#include <iostream>
#include <ctype.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <vector>

#include "barcos.hpp"

namespace redes{

class Tablero{
	private:
		
		std::vector <std::vector<char>> _tablero;
		
		
		std::vector <redes::Barcos> _vbarquitos;
		
	public:

	Tablero(){
		
		_tablero.resize(10);
		int i;
		for(i=0;i<10;i++)		
			_tablero[i].resize(10,'0');
		
		
		_vbarquitos.resize(5);
		
		for(i=0;i<5;i++)
			_vbarquitos[i].setDimension[i];
		
		
	}  

	
};//Fin de la clase Tablero

}//Fin del espacio de nombres redes

#endif
