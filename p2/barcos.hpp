//fichero barcos.hpp

#ifndef BARCOS_HPP
#define BARCOS_HPP

#include <iostream>
#include <ctype.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <vector>

namespace redes{

//Declaracion de la estructura posicion	
struct posicion{
		
	int x;
	int y;
};
	
class Barco{
	private:
		
		std::vector <char> _estatus;
		int _colocacion;	//0 horizontal, 1 vertical
		int _tocados; //numero de puntos del barco tocados
		bool _hundido //si es true el barco esta hundido, en caso contrario false.
		std::vector <struct posicion> _pos;	
	
	public:

	Barco(int dimension){
	
		this->setDimension(dimension);
			
		this->setColocacion(rand()%2);	
	
		this->setHundido(false);
		
		_pos.resize(this->getDimension)
	}  
  
  	//Observador y modificador de las dimensiones del barco
  	
	inline int getDimension() const{return this->_estatus.size();}
	
	inline void setDimension(int dimension){
		
		this->_estatus.resize(dimension,'B');	
	} 	
	
	
	//Observador y modificador de la colocacion del barco
	
	inline int getColocacion() const{return _colocacion;}
	
	inline void setColocacion(int colocacion){_colocacion=colocacion;}
	
	
	//Observador y modificador de la variable _tocados
	
	inline int getTocados() const{return _tocados;}
	
	inline void setTocados(int tocados){_tocados=tocados;}
	
	//Observador y modificador de la variable _hundido
	
	inline bool getHundido() const{return _hundido;}
	
	inline void setHundido(bool hundido){_hundido=hundido}
	
	inline void setHundido(){
	
		if( getTocados() == getDimension() )//Han tocado todos los puntos del barco, ergo esta hundido
			
			setHundido(true);
		
		else
			setHundido(false);	
	}
	
	//Observador y modificador de la variable _pos
	
	inline struct posicion getPos(int i){return _pos[i];}
	
	inline void setPos (int x, int y,int i){
		
		_pos[i].x=x;
		_pos[i].y=y;
	}
	
	//codificada en el .cpp
	bool tienePos(int x,int y);
	
	
};//Fin de la clase Barco

}//Fin del espacio de nombres redes

#endif
