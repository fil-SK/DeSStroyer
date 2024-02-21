#ifndef _linker_h_
#define _linker_h_

#include <fstream>
#include <string>
#include <vector>
#include <iomanip>        // Za formatirani ispis tabela
#include <unordered_map>  // Za unordered_map kod formiranja tabele simbola
#include <map>            // Za map
#include "zapists.h"
#include "sekcija.h"
#include "enumeratori.h"


class Linker{

  public:

    // Funkcije za rad
    int procitajFajl(std::string fajl);
    bool proveriPostojanjeSimbolaSaSekcijomUND();

    void postaviPremaUnetimPocetnimAdresama(std::unordered_map<std::string, uint32_t>& mapaSekcijaAdresa, std::map<uint32_t, std::string>& mapaUredjenaAdresaSekcija);
    int proveriDaLiPostojiPreklapanjeSekcija();

    void formirajMemorijskiSadrzaj();
    void prepraviMemorijskiSadrzaj();
    void formirajMemorijskiSadrzajMapa();
    void prepraviMemorijskiSadrzajMapa();
    void ispisiMemorijskiSadrzajMapa();
    int formirajIzlazniFajl(std::string nazivIzlaznogFajla);


    // Pomocne funkcije
    void ispisiSadrzajTabeleSekcijaLinkera();
    void ispisiSadrzajTabeleSimbolaLinkera();
    uint8_t konvertujStringBajtUHexaBroj(std::string bajt);
    uint8_t konvertujDecimalniUHexaBroj(int broj);
    int konvertujHexaBrojUDecimalni(int hexa);
    bool daLiJeSimboLSekcija(int redniBrojUTS, int redniBrojSekcije);

    bool postojiSimbolUTabeliSimbola(std::string nazivSimbola);
    ZapisTS& dohvatiZapisIzTabeleSimbola(std::string nazivSimbola);

    void ispisiMemorijskiSadrzaj();

    std::string izdvojNazivFajlaBezEkstenzije(std::string nazivFajla);


    // Geteri i seteri
    int dohvatiLocationCounter();
    int dohvatiPomocZaOdredjivanjePocetneAdreseSekcije();
    Sekcija& dohvatiRedIzTabeleSekcija(std::string nazivSekcije);
    int dohvatiRedniBrObradjivanogFajla();
    int dohvatiRedniBrojSekcijeZaDatiSimbol(std::string simbol);
    ZapisTS dohvatiRedIzTabeleSimbolaZaDatRedniBroj(int redniBroj);

    void inkrementirajRedniBrojObradjivanogFajla();
    void uvecajLocationCounter(int inkrement);
    void dodajNaVrednostZaPomocZaOdredjivanjePocetneAdreseSekcije(int inkrement);
    void resetujPomocZaOdredjivanjePocetneAdreseSekcije();
    std::list<ZapisTS>& dohvatiTabeluSimbolaLinkera();
    std::list<Sekcija>& dohvatiTabeluSekcijaLinkera();
    void dodajSekcijuUTabeluSekcija(Sekcija novaSekcija, std::unordered_map<std::string, int>& mapaStarihVelicinaSekcija);
    int dodajRedUTabeluSimbola(ZapisTS novZapis, std::unordered_map<int, std::string>& stara, std::unordered_map<std::string, int>& nova, std::unordered_map<std::string, bool>& ubacenSimbolIzOvogFajla, std::unordered_map<std::string, int>& mapaStarihVelicinaSekcija);

    void postaviTabeluSekcijaLinkera(std::list<Sekcija> tabelaSekcija);

    bool kljucPostojiUMapi(std::unordered_map<std::string, bool> mapa, std::string kljuc);    // Ova funkcija namenjena je ubacenSimbolIzOvogFajla mapi

    void popraviPocetneAdreseURelokacionimZapisima();
    void dodajSadrzajUmemoriju(uint8_t bajtSadrzaja);
    void izvrsiPrepravku(int adresaPocetkaPrepravke, uint8_t bajt31_24, uint8_t bajt23_16, uint8_t bajt15_8, uint8_t bajt7_0);
    void izvrsiPrepravkuUMapi(uint32_t adresaPocetkaPrepravke, uint8_t bajt31_24, uint8_t bajt23_16, uint8_t bajt15_8, uint8_t bajt7_0);


    // Konstruktori
    Linker();

  private:
    int locationCounter;
    int pomocZaOdredjivanjePocetneAdreseSekcije;
    std::list<ZapisTS> tabelaSimbolaLinkera;
    std::list<Sekcija> tabelaSekcijaLinkera;

    std::vector<uint8_t> memorija;
    std::map<uint32_t, uint8_t> memorijaMapa;
    
    int obradaFajla;      // Brojac koliko fajlova je trenutno obradjeno
};


#endif