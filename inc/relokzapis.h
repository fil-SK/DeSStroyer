#ifndef _relokzapis_h_
#define _relokzapis_h_

#include <string>

/*
  JAKO BITNA STVAR!
  Kod vrednosti kolona relokacionih zapisa za Pocetak sekcije, Offset i Addend
  Vrednosti tih kolona su HEKSADECIMALNE VREDNOSTI !!! (nisu decimalne)

  Ovo sto sam iznad rekao je netacno - int je int, samo zavisi od toga kako ce on biti ispisan na izlazu
  std::cout ispisuje prema poslednje koriscenom formatu
  ako zelis da ispises eksplicitno prema nekom formatu onda koristis std::hex i std::dec
*/


class RelokZapis{

  public:
    RelokZapis(uint32_t sek, int offs, std::string tip, std::string simb, int add);


    //Geteri i seteri
    uint32_t dohvatiPocetnuAdresuSekcije();
    int dohvatiOffset();
    std::string dohvatiTipRelokacije();
    std::string dohvatiSimbol();
    int dohvatiAddend();
    
    void postaviPocetnuAdresuSekcije(uint32_t sek);
    void postaviOffset(int offs);
    void postaviTipRelokacije(std::string tip);
    void postaviSimbol(std::string simb);
    void postaviAddend(int add);

  private:
    uint32_t sekcijaPocetnaAdresa;     // adresa od koje krece sekcija U KOJOJ radimo relokaciju (ne sekcija u kojoj se nalazi simbol)
    int offset;                        // pomeraj do mesta na kom treba raditi prepravku
    std::string tipRelokacije;         // tip koriscene relokacije. Moguci tipovi: X86_64_32 i X86_64_PC32
    std::string simbol;                // Simbol koji moze biti ili stvarni simbol ili naziv sekcije (za simbole sa lokalnim povezivanjem)
    int addend;                        // Pomocna promenljiva preko koje uracunavamo info. o kojima linker nema pojma
};

#endif