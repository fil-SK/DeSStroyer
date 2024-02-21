#ifndef _sekcija_h_
#define _sekcija_h_

#include <string>       // Za koriscenje std::string
#include <vector>       // Za koriscenje std::vector
#include <cstdint>      // Za koriscenje uint8_t tipa
#include <list>         // Za koriscenje std::list
#include <iostream>
#include <iomanip>      // Za lep formatiran ispis

#include "relokzapis.h"

class Sekcija{

  public:
    
    // Dodatne funkcije
    void dodajUBazenLiterala(int literal);

    
    // Konstruktori
    // Pocetna adresa je unsigned int
    Sekcija(std::string naz, int vel, uint32_t adr, int rbr);


    // Geteri i seteri
    std::string dohvatiNazivSekcije();
    int dohvatiVelicinuSekcije();
    uint32_t dohvatiPocetnuAdresuSekcije();
    int dohvatiRedniBrojSekcije();
    std::list<RelokZapis> dohvatiTabeluRelokZapisa();
    std::list<RelokZapis>& dohvatiTabeluRelokZapisaPoReferenci();
    std::vector<uint8_t> dohvatiBazenLiterala();
    std::vector<uint8_t>& dohvatiSadrzaj();
    void ispisiSadrzaj();
    void ispisiSadrzajBazena();
    void ispisiRelokacioneZapise();

    void postaviNazivSekcije(std::string sekNaziv);
    void postaviVelicinuSekcije(int sekVelicina);
    void postaviPocetnuAdresuSekcije(uint32_t adr);
    void postaviRedniBrojSekcije(int rbr);
    void upisiUSadrzaj(uint8_t bajt);
    void dodajRelokacioniZapis(RelokZapis rz);


  private:
    std::string naziv;                                     // Naziv sekcije
    int velicina;                                          // Velicina sekcije (u bajtovima)
    uint32_t pocetnaAdresa;                                // Pocetna adresa sekcije
    int redniBroj;                                         // Redni broj ove konkretne sekcije u tabeli sekcija
    std::vector<uint8_t> sadrzaj;                          // Sadrzaj sekcije
    std::vector<uint8_t> bazenLiterala;                    // Bazen literala za ovu sekciju
    std::list<RelokZapis> tabelaRelokacionihZapisa;        // Relokacioni zapisi za ovu sekciju

};

#endif

/*
  Bazen literala ce kao elemente da ima vrednosti koje treba da se smeste u memoriju a koje predstavljaju te literale

  Fora sa bazenom literala je sto cu ja samo tu da punim vrednosti koje treba smestiti u memoriju
  A znacu gde ih smestam zato sto znam velicinu sekcije (saznacu velicinu sekcije na kraju prvog prolaza)
  Samim tim ja mogu da izracunam odmah pomeraj do tok konkretnog literala kada se na njega naidje i taj pomeraj odmah ugradim u instrukciju, a onda vrednost literala ubacim u bazen
*/