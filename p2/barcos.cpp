//fichero barcos.cpp

#include "barcos.hpp"


//funcion booleana tienePos
bool redes::Barcos::tienePos(int x,int y){

	int i;
	
	for(i=0; i< getDimension(); i++){
	
		/*si encuentra la posicion que buscamos entre las posiciones 
		del barco devolvemos true*/
		if(getPos(i).x == x and getPos(i).y == y)
			return true;		

	}
		/*si no encuentra la posicion entre las posiciones 
		que pertenecen al barco devuelve false*/
		return false;
}

//fin del fichero barcos.cpp
