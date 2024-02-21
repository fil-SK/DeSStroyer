#include "../inc/zapists.h"


// Konstruktori
ZapisTS::ZapisTS(std::string sim, int sek, int offs, int pov, int rb, bool ext){
  this->simbol = sim;
  this->sekcija = sek;
  this->offset = offs;
  this->nacinPovezivanja = pov;
  this->redniBroj = rb;
  this->isExtern = ext;
}

ZapisTS::ZapisTS(){}

// Geteri i seteri
std::string ZapisTS::dohvatiSimbol(){
  return simbol;
}

void ZapisTS::postaviSimbol(std::string sim){
  simbol = sim;
}

int ZapisTS::dohvatiSekciju(){
  return sekcija;
}

void ZapisTS::postaviSekciju(int sek){
  sekcija = sek;
}

int ZapisTS::dohvatiOffset(){
  return offset;
}

void ZapisTS::postaviOffset(int offs){
  offset = offs;
}

int ZapisTS::dohvatiNacinPovezivanja(){
  return nacinPovezivanja;
}

void ZapisTS::postaviNacinPovezivanja(int bind){
  nacinPovezivanja = bind;
}

int ZapisTS::dohvatiRedniBrojUTS(){
  return redniBroj;
}

void ZapisTS::postaviRedniBrojUTS(int rbr){
  redniBroj = rbr;
}

bool ZapisTS::daLiJeExtern(){
  return isExtern;
}

void ZapisTS::postaviExtern(bool ext){
  isExtern = ext;
}