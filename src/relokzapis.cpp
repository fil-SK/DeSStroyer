#include "../inc/relokzapis.h"

  RelokZapis::RelokZapis(uint32_t sek, int offs, std::string tip, std::string simb, int add){
    sekcijaPocetnaAdresa = sek;
    offset = offs;
    tipRelokacije = tip;
    simbol = simb;
    addend = add;
  }


  //Geteri i seteri
  uint32_t RelokZapis::dohvatiPocetnuAdresuSekcije(){
    return sekcijaPocetnaAdresa;
  }

  int RelokZapis::dohvatiOffset(){
    return offset;
  }

  std::string RelokZapis::dohvatiTipRelokacije(){
    return tipRelokacije;
  }

  std::string RelokZapis::dohvatiSimbol(){
    return simbol;
  }

  int RelokZapis::dohvatiAddend(){
    return addend;
  }
    
  void RelokZapis::postaviPocetnuAdresuSekcije(uint32_t sek){
    sekcijaPocetnaAdresa = sek;
  }

  void RelokZapis::postaviOffset(int offs){
    offset = offs;
  }

  void RelokZapis::postaviTipRelokacije(std::string tip){
    tipRelokacije = tip;
  }

  void RelokZapis::postaviSimbol(std::string simb){
    simbol = simb;
  }

  void RelokZapis::postaviAddend(int add){
    addend = add;
  }