void parser(char * linea_comandos, std::string fichero_usuarios){

  char opcion[256], resto[256];
  int aux;

  sscanf(linea_comandos, "%s %s", opcion, resto);
  if(opcion == "REGISTRO"){
    sscanf(resto, " -u %s -p %s", usuario, password);
    if(registro_usuario(fichero_usuarios, usuario, password)){
      std::cout<<"+Ok. Usuario registrado.\n";
    }else{
      std::cout<<"-Err. El usuario ya existe.\n";
    }
  }
  return;
}
