#ifndef _assembler_h_
#define _assembler_h_

#include <list>
#include <string>
#include "enumeratori.h"
#include "zapists.h"
#include "sekcija.h"
#include "relokzapis.h"



class Assembler{

  public:

    // Funkcije za rad
    void prviProlaz(std::string nazivUlazneDatoteke, std::string nazivIzlazneDatoteke);
    void drugiProlaz(std::string nazivUlazneDatoteke, std::string nazivIzlazneDatoteke);
    void uvecajVelicinuSekcijaZaBazenLiterala();
    void formirajIzlazniFajl(std::string nazivIzlazneDatoteke);


    // Funkcije za asembliranje

    int obradaLabele(std::string nazivLabele);
    int obradaDirektiveSection(std::string nazivSekcije);
    int obradaDirektivaGlobalExtern(std::string simbol, int flagGlobalExtern);
    void obradaDirektiveWordSimbol(std::string simbol);
    void obradaDirektiveWordLiteral(int literal);
    void obradaDirektiveSkip(int literal);
    void obradaDirektiveEnd();


    void obradiInstrukcijuHalt();
    void obradiInstrukcijuInt();
    void obradiInstrukcijuIret();
    void obradiInstrukcijuCallLiteral(int literal);
    void obradiInstrukcijuCallSimbol(std::string simbol);
    void obradiInstrukcijuRet();

    void obradiInstrukcijuJmpLiteral(int literal);
    void obradiInstrukcijuJmpSimbol(std::string simbol);
    void obradiInstrukcijuBeqLiteral(std::string reg1, std::string reg2, int literal);
    void obradiInstrukcijuBeqSimbol(std::string reg1, std::string reg2, std::string simbol);
    void obradiInstrukcijuBneLiteral(std::string reg1, std::string reg2, int literal);
    void obradiInstrukcijuBneSimbol(std::string reg1, std::string reg2, std::string simbol);
    void obradiInstrukcijuBgtLiteral(std::string reg1, std::string reg2, int literal);
    void obradiInstrukcijuBgtSimbol(std::string reg1, std::string reg2, std::string simbol);

    void obradiInstrukcijuPush(std::string reg);
    void obradiInstrukcijuPop(std::string reg);
    void obradiInstrukcijuXchg(std::string regSrc, std::string regDst);

    void obradiInstrukcijuAdd(std::string regSrc, std::string regDst);
    void obradiInstrukcijuSub(std::string regSrc, std::string regDst);
    void obradiInstrukcijuMul(std::string regSrc, std::string regDst);
    void obradiInstrukcijuDiv(std::string regSrc, std::string regDst);

    void obradiInstrukcijuNot(std::string reg);
    void obradiInstrukcijuAnd(std::string regSrc, std::string regDst);
    void obradiInstrukcijuOr(std::string regSrc, std::string regDst);
    void obradiInstrukcijuXor(std::string regSrc, std::string regDst);
    void obradiInstrukcijuShl(std::string regSrc, std::string regDst);
    void obradiInstrukcijuShr(std::string regSrc, std::string regDst);


    /*
      Instrukcija LD ne radi sa specijalnim CSR registrima, tako da varijante sa njima nisu obradjene
      Dodatno, nije obradjen slucaj [%gpr + simbol], zato sto se konacna vrednost simbola ne moze znati tokom asembliranja, vec tek nakon linkovanja
        A i malo nema smisla koristiti simbol kao pomeraj, buduci da nikad ne znamo koju ce vrednost (adresu) simbol imati, jer je linker taj koji rasporedjuje po memoriji pa simbol moze imati razlicitu vrednost

      INSTR_LD LEVA_UGLASTA PROCENAT GPRX PLUS simbol DESNA_UGLASTA ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdGPRMemOffsSimbol(); }

    */
    void obradiInstrukcijuLdLiteralVrednost(int literal, std::string regDst);
    void obradiInstrukcijuLdSimbolVrednost(std::string simbol, std::string regDst);
    void obradiInstrukcijuLdLiteralMem(int literal, std::string regDst);
    void obradiInstrukcijuLdSimbolMem(std::string simbol, std::string regDst);
    void obradiInstrukcijuLdGPRVrednost(std::string regSrc, std::string regDst);
    void obradiInstrukcijuLdGPRMem(std::string srcAddr, std::string regDst);
    int obradiInstrukcijuLdGPRMemOffsLiteral(std::string srcAddr, int literalOffs, std::string regDst);
    // void obradiInstrukcijuLdGPRMemOffsSimbol();


    /*
      Instrukcija STORE takodje ne radi sa CSR registrima - za to se koriste posebne instrukcije CSRRD i CSRWR
      Dodatno, STORE kao operand ne moze da koristi $literal i $simbol, zato sto nije moguce raditi upis u neposrednu vrednost
      Kao operand se ne moze koristiti %gpr, buduci da se STORE instrukcija ne moze koristiti za upis u registar
      Iz istog razloga kao i za LOAD, ne moze se koristiti [%gpr + simbol] kao operand
    */

    void obradiInstrukcijuStLiteralMem(std::string regSrc, int literalAddrDst);
    void obradiInstrukcijuStSimbolMem(std::string regSrc, std::string simbolAddrDst);
    void obradiInstrukcijuStGPRMem(std::string regSrc, std::string addrDst);
    int obradiInstrukcijuStGPRMemOffsLiteral(std::string regSrc, std::string addrDst, int literalOffs);
    // void obradiInstrukcijuStGPRMemOffsSimbol();


    void obradiInstrukcijuCsrRd(std::string regCsr, std::string regGpr);
    void obradiInstrukcijuCsrWr(std::string regGpr, std::string regCsr);


    // Pomocne funkcije
    void dodajZapisUTabeluSimbola(ZapisTS zapis);
    void dodajSekcijuUTabeluSekcija(Sekcija sekcija);
    void ispisiSadrzajTabeleSimbola();
    bool postojiSimbol(std::string simbol);
    void ispisiSadrzajTabeleSekcija();


    uint32_t formiraj32BitniLiteral(uint8_t hex31_24, uint8_t hex23_16, uint8_t hex15_8, uint8_t hex7_0);
    std::string konvertujRegistarUHexa(std::string reg);
    uint8_t konvertujHexaZapisUInt8(std::string hexa);    // Hexa string "0xAA" u uint8_t hexa vrednost 0xAA
    std::string konvertuj4BitLiteralUHexa(uint8_t literal4bit);
    // Trebaju ti metode koje ce iz svih tipova literala da konvertuju u 32bitnu vrednost (e sad nzm da li da koristim uint32_t ili int obican)




    // Konstruktori
    Assembler();


    // Geteri i seteri
    std::list<ZapisTS> dohvatiTabeluSimbola();
    std::list<Sekcija>& dohvatiTabeluSekcija();
    Sekcija& dohvatiRedIzTabeleSekcija(std::string nazivSekcije);
    ZapisTS dohvatiRedIzTabeleSimbola(std::string simbol);
    int dohvatiLocationCounter();
    ASMProlaz dohvatiTekuciProlaz();
    int dohvatiTrenutnuSekciju();
    std::string dohvatiNazivTrenutneSekcije();
    int dohvatiVrednostSimbolaIzTS(std::string simbol);
    std::string dohvatiNazivSekcijePremaRednomBrojuUTS(int redniBrojSekcijeUTS);
    uint32_t dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(std::string nazivSekcije);

    void postaviTabeluSimbola(std::list<ZapisTS> ts);
    void postaviTabeluSekcija(std::list<Sekcija> ts);
    void resetujLocationCounter();
    void uvecajLocationCounter(int inkrement);
    void postaviProlazAsemblera(ASMProlaz prolaz);
    void postaviTrenutnuSekciju(int sekcija);
    void postaviVelicinuSekcijeUTabeli(std::string nazivSekcije, int velicina);

    void globalniSimbolAzurirajOffset(int redniBrojUTS, int offset);
    void globalniSimbolAzurirajSekciju(int redniBrojUTS, int sekcija);    


  private:
    std::list<ZapisTS> tabelaSimbola;
    std::list<Sekcija> tabelaSekcija;
    

    int locationCounter;
    ASMProlaz trenutniProlaz;
    int trenutnaSekcija;          // Cuva redni broj sekcije u tabeli simbola
};

#endif // _assembler_h_


/*

Napomene:

Pojasnjenje za rad sa instrukcijama koje koriste pomeraj, ali je pomeraj NEGATIVAN

Inicijalno, ja sam hteo da radim na ovaj nacin (kao primer uzet kod koji je ranije koriscen za obradiInstrukcijuCallSimbol )


else{
        // Pomeraj je negativan - skace se unazad
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita, samim tim moram da koristim bazen literala
        

        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);

        // Dodaj u bazen POMERAJ
        sekcija.dodajUBazenLiterala(pomeraj);

        // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8     
        sekcija.upisiUSadrzaj(0xF0);            // bajt23_16     // RegA = PC tj. R15, RegB = 0
        sekcija.upisiUSadrzaj(0x21);            // bajt31_24
      }


        // Medjutim, postoji problem. Recimo da je negativni pomeraj jednak 0xFFFFFFFA i to je vrednost koju smestim u bazen literala.
        // Ako bih tada za realizaciju ove CALL instrukcije koristio 0x21 opkod, to bi znacilo da
        // Preko PC+pomeraj ja dolazim do adrese U BAZENU LITERALA na kojoj se pomeraj nalazi, i kada odradim mem[] od te adrese, ja zapravo dobijam TAJ LITERAL
        // Instrukcija 0x21 tada TAJ LITERAL smesta u PC registar --- TO NIJE ISPRAVNO PONASANJE
        
        // Ono sto bi meni trebalo, ako bih zeleo da radim na taj nacin, jeste da taj DOHVACENI LITERAL saberem sa PC vrednoscu, kako bih onda skocio unazad na mesto gde je simbol definisan
        // Medjutim, ne postoji takva instrukcija koja bi omogucila da se ovaj dohvaceni literal sabere sa PC-jem
        // Zato NE RADIM NA OVAJ NACIN
        
        // Ukoliko je pomeraj negativan -- radi se na identican nacin kao da simbol nije definisan u istoj sekciji -- u bazen smestamo vrednost simbola i formiramo relokacioni zapis

*/