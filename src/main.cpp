
#include <iostream>
#include "../inc/assembler.h"
#include "../parser.tab.h"

#include <iomanip>
#include <cstdint>
#include <vector>


extern FILE* yyin;
extern int yyparse();

Assembler asembler;

// Asembler se pokrece sledecom komandom
// ./asembler -o nazivIzlaznogFajla.o nazivUlaznogFajla.s

int main(int argc, char* argv[]){

  std::string komanda = argv[0];
  std::string opcija = argv[1];
  std::string nazivUlazneDatoteke = argv[3];
  std::string nazivIzlazneDatoteke = argv[2];
  

  if(argc != 4){
    std::cout << "Nedovoljan broj argumenata!";
    return -1;
  }
  
  if(komanda != "./asembler"){
    std::cout << "Greska! Asembler se mora pokrenuti komandom asembler!";
    return -1;
  }

  if(opcija != "-o"){
    std::cout << "Mora se zadati naziv izlazne datoteke!";
    return -1;
  }

 std::cout << "Uneto je redom:" << komanda << " " << opcija << " " << nazivIzlazneDatoteke <<" " << nazivUlazneDatoteke << std::endl;

  asembler.prviProlaz(nazivUlazneDatoteke, nazivIzlazneDatoteke);

  std::cout << std::endl << std::endl;
  std::cout << "Sadrzaj tabele simbola je" << std::endl;
  asembler.ispisiSadrzajTabeleSimbola();
  std::cout << std::endl;


  std::cout << std::endl << std::endl;
  std::cout << "Sadrzaj tabele sekcija je:" << std::endl;
  asembler.ispisiSadrzajTabeleSekcija();
  std::cout << std::endl << std::endl;

  


  std::cout << std::endl << std::endl;
  // Postoji problem da ako imam gresku u prvom prolazu, on zapamti cursor dokle je stao pa mi
  // ovaj drugi prolaz ne krene od pocetka nego nastavi tamo gde je stao u prvom
  // To ispravi tako sto ces u metodi za drugi prolaz da imas postavljanje kursora na pocetak fajla
  asembler.drugiProlaz(nazivUlazneDatoteke, nazivIzlazneDatoteke);


  std::cout << std::endl << std::endl;
  std::cout << "Sadrzaj svake od sekcija:" << std::endl;
  for(Sekcija iterator : asembler.dohvatiTabeluSekcija()){
    iterator.ispisiSadrzaj();
    std::cout << std::endl;
  }


  asembler.uvecajVelicinuSekcijaZaBazenLiterala();

  asembler.formirajIzlazniFajl(nazivIzlazneDatoteke);

}