#ifndef USUARIO_HPP
#define USUARIO_HPP

class Usuario{
  private:
    std::string nombre_usuario;
    int descriptor_usuario;
    int estado_usuario;
    bool individual = true;
    int numero_partida = -1;
    int puntuacion_jugador = 0;
    int puntuacion_partida = 0;
    int aciertos_jugador = 0;
    int fallos_jugador = 0;
    int intentos_jugador = 0;

  public:
    inline Usuario(std::string nombre, int descriptor, int puntuacion, int aciertos, int fallos, int estado = 0){
      nombre_usuario = nombre;
      descriptor_usuario = descriptor;
      puntuacion_jugador = puntuacion;
      aciertos_jugador = aciertos;
      fallos_jugador = fallos;
      estado_usuario = estado;
    }
    inline std::string getNombre(){return nombre_usuario;}
    inline int getDescriptor(){return descriptor_usuario;}
    inline void setEstado(int estado){estado_usuario = estado;}
    inline int getEstado(){return estado_usuario;}
    inline void setIndividual(){individual = true;}
    inline void setGrupo(){individual = false;}
    inline bool esIndividual(){return individual;}
    inline void setPartida(int partida){numero_partida = partida;}
    inline int getPartida(){return numero_partida;}
    inline void aumentaPuntuacion(int puntos){puntuacion_jugador += puntos;}
    inline int getPuntuacion(){return puntuacion_jugador;}
    inline void setPuntuacion(int puntos){puntuacion_jugador = puntos;}
    inline void setPuntuacionPartida(int puntos){puntuacion_partida = puntos;}
    inline void aumentaPuntuacionPartida(int puntos){puntuacion_partida += puntos;}
    inline int getPuntuacionPartida(){return puntuacion_partida;}
    inline void aumentaAciertos(){aciertos_jugador++;}
    inline int getAciertos(){return aciertos_jugador;}
    inline void setAciertos(int aciertos){aciertos_jugador = aciertos;}
    inline void aumentaFallos(){fallos_jugador++;}
    inline int getFallos(){return fallos_jugador;}
    inline void setFallos(int fallos){fallos_jugador = fallos;}
    inline void setIntentos(int intentos){intentos_jugador = intentos;}
    inline void aumentaIntentos(){intentos_jugador++;}
    inline int getIntentos(){return intentos_jugador;}
};

#endif
