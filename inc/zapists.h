#ifndef _zapists_h_
#define _zapists_h_

#include <string>

class ZapisTS{

  public:

    // Konstruktori
    ZapisTS();
    ZapisTS(std::string sim, int sek, int offs, int pov, int rb, bool ext);


    // Geteri i seteri
    std::string dohvatiSimbol();
    int dohvatiSekciju();
    int dohvatiOffset();
    int dohvatiNacinPovezivanja();
    int dohvatiRedniBrojUTS();
    bool daLiJeExtern();

    void postaviSimbol(std::string sim);
    void postaviSekciju(int sek);
    void postaviOffset(int offs);
    void postaviNacinPovezivanja(int bind);
    void postaviRedniBrojUTS(int rbr);
    void postaviExtern(bool ext);

  private:
    std::string simbol;       // naziv simbola
    int sekcija;              // redni broj ulaza u kom se nalazi ova sekcija
    int offset;               // pomeraj unutar ove sekcije do tog simbola
    int nacinPovezivanja;     // 0 - lokalno vidljiv, 1 - globalno vidljiv
    int redniBroj;            // redni broj ulaza u tabeli simbola
    bool isExtern;            // da li je simbol defisan direktivom extern ili ne (ovo imam da bih radio proveru da ne sme da se pojavi u fajlu koji parsiram)
};


#endif // _zapists_h_