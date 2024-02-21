#include "../inc/sekcija.h"


// Konstruktor
Sekcija::Sekcija(std::string naz, int vel, uint32_t adr, int rbr){
  naziv = naz;
  velicina = vel;
  pocetnaAdresa = adr;
  redniBroj = rbr;
}


// Geteri i seteri
std::string Sekcija::dohvatiNazivSekcije(){
  return naziv;
}


void Sekcija::postaviNazivSekcije(std::string sekNaziv){
  naziv = sekNaziv;
}


int Sekcija::dohvatiVelicinuSekcije(){
  return velicina;
}


void Sekcija::postaviVelicinuSekcije(int sekVelicina){
  velicina = sekVelicina;
}


uint32_t Sekcija::dohvatiPocetnuAdresuSekcije(){
  return pocetnaAdresa;
}


void Sekcija::postaviPocetnuAdresuSekcije(uint32_t adr){
  pocetnaAdresa = adr;
}


int Sekcija::dohvatiRedniBrojSekcije(){
  return redniBroj;
}


void Sekcija::postaviRedniBrojSekcije(int rbr){
  redniBroj = rbr;
}


void Sekcija::upisiUSadrzaj(uint8_t bajt){
  sadrzaj.push_back(bajt);
}


std::vector<uint8_t>& Sekcija::dohvatiSadrzaj(){
  return sadrzaj;
}


void Sekcija::ispisiSadrzaj(){

  std::cout << "Sadrzaj sekcije " << this->dohvatiNazivSekcije() << std::endl;
  
  for(uint8_t iterator : this->dohvatiSadrzaj()){
    if (iterator < 0x10){
      std::cout << "0" << std::hex << static_cast<int>(iterator) << " ";
    }
    else{
      std::cout << std::hex << static_cast<int>(iterator) << " ";
    }
  }
}


void Sekcija::ispisiSadrzajBazena(){
  
  std::cout << "Sadrzaj bazena literala za sekciju " << this->dohvatiNazivSekcije() << std::endl;

  for(uint8_t iterator : this->dohvatiBazenLiterala()){
    if (iterator < 0x10){
      std::cout << "0" << std::hex << static_cast<int>(iterator) << " ";
    }
    else{
      std::cout << std::hex << static_cast<int>(iterator) << " ";
    }
  }
}


void Sekcija::ispisiRelokacioneZapise(){

  const int sirinaKarakteraRZ = 18;
  const char separator = ' ';

  std::cout << "Relokacioni zapisi sekcije " << this->dohvatiNazivSekcije() << std::endl;
  std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Pocetak sekcije";
  std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Offset";
  std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Tip relokacije";
  std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Simbol";
  std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Addend";
  std::cout << std::endl;

  for(RelokZapis iteratorRZ : this->dohvatiTabeluRelokZapisa()){
    std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiPocetnuAdresuSekcije();
    std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiOffset();
    std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiTipRelokacije();
    std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiSimbol();
    std::cout << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiAddend();
    std::cout << std::endl;
  }
  
}


std::list<RelokZapis> Sekcija::dohvatiTabeluRelokZapisa(){
  return tabelaRelokacionihZapisa;
}


std::list<RelokZapis>& Sekcija::dohvatiTabeluRelokZapisaPoReferenci(){
  return tabelaRelokacionihZapisa;
}


std::vector<uint8_t> Sekcija::dohvatiBazenLiterala(){
  return bazenLiterala;
}

void Sekcija::dodajUBazenLiterala(int literal){
  
  // Prosledjuje mi se podatak tipa int - dakle 32bitna vrednost. Ja cu da izdvojim svaki bajt i da ga PUSH-ujem u bazen kako treba
  uint8_t literal31_24  = (literal & 0xFF000000) >> 24;
  uint8_t literal23_16  = (literal & 0x00FF0000) >> 16;
  uint8_t literal15_8   = (literal & 0x0000FF00) >> 8;
  uint8_t literal7_0    = (literal & 0x000000FF);
  /*
  std::cout << "PROSLEDJENA MI JE VREDNOST " << literal << ", a ja sam u bazen dodao sledece:" << std::endl;
  if (literal31_24 < 0x10){
    std::cout  << "0" << std::hex << static_cast<int>(literal31_24) << " ";
  }
  else{
    std::cout  << std::hex << static_cast<int>(literal31_24) << " ";
  }

  if (literal23_16 < 0x10){
    std::cout  << "0" << std::hex << static_cast<int>(literal23_16) << " ";
  }
  else{
    std::cout  << std::hex << static_cast<int>(literal23_16) << " ";
  }

  if (literal15_8 < 0x10){
    std::cout  << "0" << std::hex << static_cast<int>(literal15_8) << " ";
  }
  else{
    std::cout  << std::hex << static_cast<int>(literal15_8) << " ";
  }

  if (literal7_0 < 0x10){
    std::cout  << "0" << std::hex << static_cast<int>(literal7_0) << " ";
  }
  else{
    std::cout  << std::hex << static_cast<int>(literal7_0) << " ";
  }
  */

  // |-------| |-------|   |-------|  |-------|     Little Endian - Nizi bajt na nizoj adresi
  // 31     24 23      16 15       8  7       0


  bazenLiterala.push_back(literal7_0);
  bazenLiterala.push_back(literal15_8);
  bazenLiterala.push_back(literal23_16);
  bazenLiterala.push_back(literal31_24);
}


void Sekcija::dodajRelokacioniZapis(RelokZapis rz){
  tabelaRelokacionihZapisa.push_back(rz);
}