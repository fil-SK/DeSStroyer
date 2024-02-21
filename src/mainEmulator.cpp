#include <iostream>

#include "../inc/emulator.h"
#include "../inc/enumeratori.h"

Emulator emulator;


int main(int argc, char* argv[]){

  if(argc > 2){
    std::cout << "Greska! Previse argumenata!";
    return EmulatorInfo::NEISPRAVAN_BROJ_ARGUMENATA;
  }

  std::string komanda = argv[0];
  std::string nazivFajla = argv[1];


  std::cout << "Uspesno!" << std::endl;
  emulator.procitajUlazniFajl(nazivFajla);
  std::cout << "Zavrsio citanje ulaznog fajla!" << std::endl;

  std::cout << std::endl;
  emulator.ispisiSadrzajMemorije();
  std::cout << std::endl;
  std::cout << "Ispisao sadrzaj memorije!" << std::endl;
  
  emulator.pokreniIzvrsavanje();
  std::cout << "Zavrsio izvrsavanje!" << std::endl;

  emulator.ispisiStanjeNakonEmulacije();
  std::cout << "Ispisao sadrzaj nakon emulacije!" << std::endl;

  emulator.ispisiStanjeCSRRegistara();

  std::cout << std::endl;
  emulator.ispisiSadrzajMemorije();
  std::cout << std::endl;
}