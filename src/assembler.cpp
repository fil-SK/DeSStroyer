
#include <cstdio>
#include <iostream>
#include <iomanip>                // Za formatirani ispis tabele simbola
#include <fstream>                // Za upis u fajl
#include "../inc/assembler.h"
#include "../parser.tab.h"

extern FILE* yyin;
extern int yyparse();



// ---------- Prvi prolaz asemblera START ----------

void Assembler::prviProlaz(std::string nazivUlazneDatoteke, std::string nazivIzlazneDatoteke){

    // Postavi flag
    this->postaviProlazAsemblera(ASMProlaz::PRVI_PROLAZ);

    std::string putanja = "tests/" + nazivUlazneDatoteke;
    const char* putanjaCString = putanja.c_str();

    // Ucitaj ulazni fajl i usmeri da parser cita iz fajla
    FILE* ulazniFajl = fopen(putanjaCString, "r");
    yyin = ulazniFajl;


    // Parsiraj ulazni fajl
    yyparse();


    // Oslobodi resurse
    fclose(ulazniFajl);
}

// ---------- Prvi prolaz asemblera END ----------

// ---------- Drugi prolaz asemblera START ----------

void Assembler::drugiProlaz(std::string nazivUlazneDatoteke, std::string nazivIzlazneDatoteke){

    // Postavi flag
    this->postaviProlazAsemblera(ASMProlaz::DRUGI_PROLAZ);


    // Resetuj flag-ove za ispravno funkcionisanje prolaza
    this->resetujLocationCounter();
    this->postaviTrenutnuSekciju(-1);     // Nisam siguran da li mi je ovo korektna vrednost

    std::string putanja = "tests/" + nazivUlazneDatoteke;
    const char* putanjaCString = putanja.c_str();

    // Ucitaj ulazni fajl i usmeri da parser cita iz fajla
    FILE* ulazniFajl = fopen(putanjaCString, "r");
    yyin = ulazniFajl;


    // Parsiraj ulazni fajl
    yyparse();


    // Oslobodi resurse
    fclose(ulazniFajl);
}

// ---------- Drugi prolaz asemblera END ----------

// ---------- Formiranje izlaznog fajla START ----------

void Assembler::formirajIzlazniFajl(std::string nazivIzlazneDatoteke){

  std::fstream izlazniFajl;
  izlazniFajl.open(nazivIzlazneDatoteke, std::ios::out);

  

  if(izlazniFajl.is_open()){

    // Ispisujemo tabelu sekcija
    izlazniFajl << "Tabela sekcija" << std::endl;

    const char separator = ' ';
    const int sirinaKaraktera = 15;

    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Naziv sekcije";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Velicina";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Pocetna adresa";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Redni broj";
    izlazniFajl << std::endl;

    for(Sekcija iterator : this->dohvatiTabeluSekcija()){
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiNazivSekcije();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiVelicinuSekcije();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiPocetnuAdresuSekcije();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiRedniBrojSekcije();
      izlazniFajl << std::endl;
    }
    izlazniFajl << std::endl;

  // -----------------------

  // Ispisujemo tabelu simbola

    izlazniFajl << "Tabela simbola" << std::endl;

    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Simbol";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Sekcija";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Offset";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Povezivanje";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Redni broj";
    izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "isExtern";
    izlazniFajl << std::endl;


    // Sadrzaj
    for(ZapisTS iterator : this->dohvatiTabeluSimbola()){    
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiSimbol();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiSekciju();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiOffset();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiNacinPovezivanja();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiRedniBrojUTS();
      izlazniFajl << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.daLiJeExtern();
      izlazniFajl << std::endl;
    }
    izlazniFajl << std::endl;
    // --------------------------

    // Sada za svaku sekciju ispisujemo njen: sadrzaj, bazen literala i relokacione zapise

    for(Sekcija iteratorSekcija : this->dohvatiTabeluSekcija()){

      izlazniFajl << "Sadrzaj sekcije " << iteratorSekcija.dohvatiNazivSekcije() << std::endl;
      int cnt = 0;      // Brojac, da bi imalo 8 bajtova u jednom redu
      int prvi = 1;     // Obican flag kako ne bih za prvi upis imao nov red, a da za ostale imam

      for(uint8_t iteratorSadrzaja : iteratorSekcija.dohvatiSadrzaj()){
        if(cnt % 8 == 0 && prvi == 0){
          izlazniFajl << std::endl;
        }

        if (iteratorSadrzaja < 0x10){
          izlazniFajl << "0" << std::hex << static_cast<int>(iteratorSadrzaja) << " ";
        }
        else{
          izlazniFajl << std::hex << static_cast<int>(iteratorSadrzaja) << " ";
        }
        cnt++;
        prvi = 0;
      }
      izlazniFajl << std::endl << std::endl;

      izlazniFajl << "Bazen literala sekcije " << iteratorSekcija.dohvatiNazivSekcije() << std::endl;
      for(uint8_t iteratorBazen : iteratorSekcija.dohvatiBazenLiterala()){
        
        if (iteratorBazen < 0x10){
          izlazniFajl << "0" << std::hex << static_cast<int>(iteratorBazen) << " ";
        }
        else{
          izlazniFajl << std::hex << static_cast<int>(iteratorBazen) << " ";
        }

      }
      izlazniFajl << std::endl << std::endl;

    // Relokacioni zapisi
    const int sirinaKarakteraRZ = 18;
      izlazniFajl << "Relokacioni zapisi sekcije " << iteratorSekcija.dohvatiNazivSekcije() << std::endl;
      izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Pocetak sekcije";
      izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Offset";
      izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Tip relokacije";
      izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Simbol";
      izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << "Addend";
      izlazniFajl << std::endl;

      for(RelokZapis iteratorRZ : iteratorSekcija.dohvatiTabeluRelokZapisa()){
        izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiPocetnuAdresuSekcije();
        izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiOffset();
        izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiTipRelokacije();
        izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiSimbol();
        izlazniFajl << std::left << std::setw(sirinaKarakteraRZ) << std::setfill(separator) << iteratorRZ.dohvatiAddend();
        izlazniFajl << std::endl;
      }
      izlazniFajl << std::endl;

    }

    izlazniFajl << "FajlZavrsen";

    izlazniFajl.close();
  }

}

// ---------- Formiranje izlaznog fajla END ----------



// ---------- Funkcije za asemblersku obradu START ----------

// --- Pomocne funkcije START ---

uint32_t Assembler::formiraj32BitniLiteral(uint8_t hex31_24, uint8_t hex23_16, uint8_t hex15_8, uint8_t hex7_0){

  // Funkcija radi sledece
  //                  ____ ____   ____ ____   ____ ____   ____ ____
  // hex31_24         ____ ____   ____ ____   ____ ____   XXXX XXXX
  // hex31_24 << 24   XXXX XXXX   ____ ____   ____ ____   ____ ____   itd. i ostali, samo shift na odgovarajuce mesto

  uint32_t hex31_0 = 0;       // Inicijalno su svih 32 bita nule

  hex31_0 |= static_cast<uint32_t>(hex31_24)  << 24;
  hex31_0 |= static_cast<uint32_t>(hex23_16)  << 16;
  hex31_0 |= static_cast<uint32_t>(hex15_8)   << 8;
  hex31_0 |= static_cast<uint32_t>(hex7_0)    << 0;   // Logicno ne treba shift ovde ali ovako mi lepse izgleda jer je uniformno

  return hex31_0;
}


int Assembler::obradaLabele(std::string nazivLabele){

  // Labelu obradjujemo samo u prvom prolazu
  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    return 0;
  }


  // Ako simbol ne postoji u tabeli simbola, dodajemo ga
  if(!this->postojiSimbol(nazivLabele)){

    int redniBrojSimbola = this->dohvatiTabeluSimbola().back().dohvatiRedniBrojUTS();
    redniBrojSimbola++;
    ZapisTS novZapis(nazivLabele, this->dohvatiTrenutnuSekciju(), this->dohvatiLocationCounter(), Povezivanje::LOKALNO, redniBrojSimbola, externDir::NIJE_EXTERN);
    this->dodajZapisUTabeluSimbola(novZapis);

    return InfoKod::DODAT_SIMBOL_U_TS;
  }


  // Ako postoji u tabeli simbola, dohvatamo ceo njegov red zbog ostalih podataka iz reda koje koristimo za proveru
  ZapisTS redIzTabeleSimbola;
  for(ZapisTS iterator : this->dohvatiTabeluSimbola()){
    if(iterator.dohvatiSimbol() == nazivLabele){
      redIzTabeleSimbola = iterator;
      break;
    }
  }


  // U tabeli simbola je mogao da se nadje u sledecim slucajevima
  //    1. Normalna definicija, pa je onda ovo visestruka definicija sto je greska. Visestruka je definicija zato sto u ovom delu obradjujemo one simbole koji VEC POSTOJE u tabeli simbola.
  //    2. Uveden preko extern, pa je ova definicija greska jer ne sme biti definisan u ovom vec u spoljasnjem fajlu
  //    3. Uveden preko global, pa moramo da proverimo da li je ovo njegova prva definicija (ispravno) ili druga (neispravno)

  // Proveramo da li je uveden preko extern - greska ako jeste
  if(redIzTabeleSimbola.daLiJeExtern()){
    std::cout << "Greska! Simbol " << nazivLabele << " je uveden preko extern i ne sme biti definisan u ovom fajlu!" << std::endl;
    return InfoKod::SIMBOL_UVEDEN_PREKO_EXTERN;
  }


  // Proveravamo da li je lokalno povezivanje - to bi znacilo da smo naisli na visestruku definiciju
  if(redIzTabeleSimbola.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){
    std::cout << "Greska! Simbol " << nazivLabele << " je visestruko definisan!" << std::endl;
    return InfoKod::VISESTRUKA_DEFINICIJA_SIMBOLA;
  }

  else{
    // Else - globalno povezivanje. Proveravamo da li je ovo prva definicija globalnog simbola.
    // Ako sekcija nije UND (ako je !=0) tada ovo nije prva nego neka kasnija definicija - greska
    if(redIzTabeleSimbola.dohvatiSekciju() != 0){
      std::cout << "Greska! Simbol " << nazivLabele << " je visestruko definisan!" << std::endl;
      return InfoKod::VISESTRUKA_DEFINICIJA_SIMBOLA;
    }
    else{
      // Jeste prva definicija - azuriramo offset na kom smo nasli def. simbola i sekciju u kojoj smo ga nasli

      // ovde je BIO problem! Ovo azurira lokalni podatak a ne pravi! Moram da dohvatim po referenci ali kada to uradim program mi pukne

      int redUTS = redIzTabeleSimbola.dohvatiRedniBrojUTS();
      
      this->globalniSimbolAzurirajOffset(redUTS, this->dohvatiLocationCounter());
      this->globalniSimbolAzurirajSekciju(redUTS, this->dohvatiTrenutnuSekciju());

      // Ove dve linije su bile ranije, ali nisu tacne
      //redIzTabeleSimbola.postaviOffset(this->dohvatiLocationCounter());
      //redIzTabeleSimbola.postaviSekciju(this->dohvatiTrenutnuSekciju());

      return InfoKod::AZURIRAN_SIMBOL_U_TS;
    }
  }
}

void Assembler::globalniSimbolAzurirajOffset(int redniBrojUTS, int offset){
  
  for(ZapisTS& iterator : tabelaSimbola){
    if(iterator.dohvatiRedniBrojUTS() == redniBrojUTS){
      iterator.postaviOffset(offset);
    }
  }

}

void Assembler::globalniSimbolAzurirajSekciju(int redniBrojUTS, int sekcija){
  
  for(ZapisTS& iterator : tabelaSimbola){
    if(iterator.dohvatiRedniBrojUTS() == redniBrojUTS){
      iterator.postaviSekciju(sekcija);
    }
  }

}


// --- Pomocne funkcije END ---

// --- Obrada direktiva START ---

int Assembler::obradaDirektiveSection(std::string nazivSekcije){

  // Direktivu obradjujemo samo u prvom prolazu
  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // U drugom prolazu samo azuriramo flag koji prati koja je trenutna sekcija
    // tako sto procitamo redni broj sekcije u tabeli simbola i postavimo ga u flag
    for(ZapisTS iterator : this->dohvatiTabeluSimbola()){
      if(iterator.dohvatiSimbol() == nazivSekcije){
        std::cout << "TRENUTNA SEKCIJA JE " << nazivSekcije << std::endl;
        this->postaviTrenutnuSekciju(iterator.dohvatiRedniBrojUTS());
        std::cout << "NJEN REDNI BR JE " << iterator.dohvatiRedniBrojUTS() << std::endl;
      }
    }

    // I azuriramo LC
    this->resetujLocationCounter();
    
    return 0;
  }


  // Da li taj simbol (naziv sekcije) vec postoji u tabeli simbola
  if(this->postojiSimbol(nazivSekcije)){
    std::cout << "Greska! Sekcija " << nazivSekcije << " je vec ranije iskoriscena!" << std::endl;
    return InfoKod::SEKCIJA_VEC_ISKORISCENA;
  }


  // Ako ne postoji, dodajemo simbol (naziv sekcije) u tabelu simbola
  // Sekcija uzima prvi naredni broj u tabeli simbola, a posto je simbol = sekcija, tada ce redni broj simbola biti jednak rednom broju sekcije
  int rbr = this->dohvatiTabeluSimbola().back().dohvatiRedniBrojUTS();
  int redniBrojSekcije = rbr + 1;
  int redniBrojSimbola = redniBrojSekcije;
  int offset = 0;

  ZapisTS novZapis(nazivSekcije, redniBrojSekcije, offset, Povezivanje::LOKALNO, redniBrojSimbola, externDir::NIJE_EXTERN);
  this->dodajZapisUTabeluSimbola(novZapis);


  // Posto ovde zapocinje nova sekcija, onda treba za "staru" sekciju postaviti njenu velicinu na LC, resetovati LC i postaviti da je trenutna sekcija sada ova nova sekcija
  // Treba i dodati tu sekciju u tabelu sekcija
  
  if(this->dohvatiTrenutnuSekciju() == -1){
    // Ovo je prva sekcija na koju smo naisli - ne postavljamo velicinu za prethodnu sekciju jer prethodne sekcije nema
    // Posto je ovo prva sekcija onda nju dodajemo u tabelu sekcija
    Sekcija prvaSekcija(nazivSekcije, 0, 0, 1);
    this->dodajSekcijuUTabeluSekcija(prvaSekcija);
  }
  else{
    
    // Nije prva sekcija - moramo da postavimo velicinu za trenutnu tekucu sekciju (dok ne oznacimo da je ova nova zapravo tekuca)
    //Sekcija trenutnaSekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());
    //trenutnaSekcija.postaviVelicinuSekcije(this->dohvatiLocationCounter());

    this->postaviVelicinuSekcijeUTabeli(this->dohvatiNazivTrenutneSekcije(), this->dohvatiLocationCounter());
    std::cout << "TEST SECTION sekcije! NAZIV SEKCIJE: " << this->dohvatiNazivTrenutneSekcije() << " I NJENA VELICINA " << this->dohvatiLocationCounter() << std::endl;
    std::cout << "LOCATION COUNTER JE " << this->dohvatiLocationCounter() << std::endl << std::endl;
   // std::cout << "TRENUTNA VELICINA SEKCIJE JE " << trenutnaSekcija.dohvatiVelicinuSekcije() << std::endl;

    // Potrebno je da ovu NOVU sekciju, upravo otvorenu, dodamo u tabelu sekcija
    // Kada dodajemo sekciju postavljamo da su joj velicina i pocetna adresa jednake 0
    int rbr = this->dohvatiTabeluSekcija().back().dohvatiRedniBrojSekcije();
    rbr++;
    Sekcija novaSekcija(nazivSekcije, 0, 0, rbr);
    this->dodajSekcijuUTabeluSekcija(novaSekcija);
  }
  
  // Ovo radimo u svakom slucaju, jer smo otvorili novu sekciju
  std::cout << "Trenutna sekcija je " << this->dohvatiTrenutnuSekciju() << std::endl;
  this->resetujLocationCounter();
  this->postaviTrenutnuSekciju(redniBrojSekcije);   // Postavljamo da je sekcija koju smo sad otvorili tekuca

  return InfoKod::DODAT_SIMBOL_U_TS;
}


int Assembler::obradaDirektivaGlobalExtern(std::string simbol, int flagGlobalExtern){

  // Posto se ista funkcija poziva za obradu obe direktive, na osnovu flag-a znamo za koju radimo

  // Bez obzira da li se obradjuje global ili extern, sekcija se postavlja na UND a povezivanje na globalno
  // Za global - sekciju nalazimo dublje u kodu; Za extern - sekciju nalazimo tokom linkovanja


  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    return 0;
  }


  // Da li simbol vec postoji u tabeli simbola
  if(this->postojiSimbol(simbol)){
    std::cout << "Greska! Simbol " << simbol << " je visestruko definisan!" << std::endl;
    return InfoKod::VISESTRUKA_DEFINICIJA_SIMBOLA;
  }

  
  // Ako ne postoji - dodajemo ga
  // Sekcija = 0 (UND), dok ne pronadjemo sekciju u kojoj je simbol definisan
  // LC = -1, pravu vrednost saznacemo kada pronadjemo definiciju tog simbola
  int redniBrojSimbola = this->dohvatiTabeluSimbola().back().dohvatiRedniBrojUTS();
  redniBrojSimbola++;


  if(flagGlobalExtern == 0){
    // Simbol uveden preko global direktive
    ZapisTS novZapis = ZapisTS(simbol, 0, -1, Povezivanje::GLOBALNO, redniBrojSimbola, externDir::NIJE_EXTERN);
    this->dodajZapisUTabeluSimbola(novZapis);
    return InfoKod::DODAT_SIMBOL_U_TS;
  }
  else{
    // Simbol uveden preko extern direktive
    ZapisTS novZapis(simbol, 0, -1, Povezivanje::GLOBALNO, redniBrojSimbola, externDir::JESTE_EXTERN);
    this->dodajZapisUTabeluSimbola(novZapis);
    return InfoKod::DODAT_SIMBOL_U_TS;
  }

}


void Assembler::obradaDirektiveWordLiteral(int literal){
  // Alocira 4B za svaki navedeni argument i inicijalizuje vrednoscu argumenta

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije()); 


    uint8_t literal31_24  = (literal & 0xFF000000)  >> 24;
    uint8_t literal23_16  = (literal & 0xFF0000)    >> 16;
    uint8_t literal15_8   = (literal & 0xFF00)      >> 8;
    uint8_t literal7_0    = (literal & 0xFF)        >> 0;


    sekcija.upisiUSadrzaj(literal7_0);    
    sekcija.upisiUSadrzaj(literal15_8);
    sekcija.upisiUSadrzaj(literal23_16);
    sekcija.upisiUSadrzaj(literal31_24);

    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradaDirektiveWordSimbol(std::string simbol){
  // Alocira 4B za svaki navedeni argument i inicijalizuje vrednoscu argumenta

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije()); 


    // U sadrzaj upisem 4B vrednosti 0, a onda napravim relokacioni zapis da linker tu vrednost prepravi sa stvarnom vrednoscu tog simbola
    sekcija.upisiUSadrzaj(0x00);
    sekcija.upisiUSadrzaj(0x00);
    sekcija.upisiUSadrzaj(0x00);
    sekcija.upisiUSadrzaj(0x00);


    // Napravi relokacioni zapis
    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);
    std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
    uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
    int offsetDoMestaPrepravke = this->dohvatiLocationCounter();
    std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
    int addend = 0;

    if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

      // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
      addend = redIzTS.dohvatiOffset();
      RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

      sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
    }

    else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

      // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
      RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

      sekcija.dodajRelokacioniZapis(globalSimbolRelok);
    }

    this->uvecajLocationCounter(4);
  }

  

}


void Assembler::obradaDirektiveSkip(int literal){
  // Alocira broj bajtova datih kao literal i inicijalizuje ih nulama

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(literal);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
  
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije()); 


    // Mozda se unese hexa vrednost koja se protumaci kao negativan broj, sto je greska, jer bih hexa vrednost trebalo da ovde tumacim kao unsigned int, ne mogu da idem unazad skip
    uint32_t unsignedLiteralVrednost = static_cast<uint32_t>(literal);


    // Unesi nule u sadrzaj
    for(int i = 0; i < unsignedLiteralVrednost; i++){   // ranije bilo i<literal
      sekcija.upisiUSadrzaj(0x00);
    }

    this->uvecajLocationCounter(unsignedLiteralVrednost);   // ranije bilo this->uvecajLocationCounter(literal)
  }

}


void Assembler::obradaDirektiveEnd(){
  // Za tekucu sekciju (posto je ona poslednja a nijedna druga nije otvorena) postavljamo velicinu
  Sekcija& trenutnaSekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());
  trenutnaSekcija.postaviVelicinuSekcije(this->dohvatiLocationCounter());

  std::cout << "TRENUTNA SEKCIJA JE " << this->dohvatiTrenutnuSekciju() << std::endl;
}


// ---Obrada direktiva END ---

// --- Obrada instrukcija START ---

void Assembler::obradiInstrukcijuHalt(){
  
  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }
  
  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    sekcija.upisiUSadrzaj(0x00);
    sekcija.upisiUSadrzaj(0x00);
    sekcija.upisiUSadrzaj(0x00);
    sekcija.upisiUSadrzaj(0x00);


    // Kao i u prvom prolazu, moramo da uvecamo LC
    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuInt(){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());
    
    sekcija.upisiUSadrzaj(0x00);    // bajt7_0
    sekcija.upisiUSadrzaj(0x00);    // bajt15_8
    sekcija.upisiUSadrzaj(0x00);    // bajt23_16
    sekcija.upisiUSadrzaj(0x10);    // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuIret(){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(8);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
     // Instrukcija IRET realizuje se preko dve instrukcije POP     --->   POP PC i POP STATUS

     // Medjutim, ne sme se realizovati tako da se sekvencijalno izvrse: POP PC; POP STATUS;
     // zato sto bi POP PC u pc smestilo drugu adresu, pa se POP STATUS nikad ne bi izvrsilo

     // Zato treba realizovati kao:
     // status = mem[sp + 4];   ---> jer mora da se preskoci PC koji je trenutno na vrhu steka
     // pc = mem[sp]            ---> onda se dohvati PC koji je na vrhu
     // sp = sp + 8;            ---> azuriramo SP za ove dve dohvacene vrednosti


    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // Hardkodujem registre u sadrzaj jer znam da koristim samo SP i PC

    // POP STATUS       --->    csr[A] <= mem32[gpr[B]+ gpr[C]+ D];
    //                          csrA = status;  gprB = sp tj. R14 ; D = 4

    sekcija.upisiUSadrzaj(0x04);      // bajt7_0      D je 4
    sekcija.upisiUSadrzaj(0x00);      // bajt15_9     RegC se ne koristi
    sekcija.upisiUSadrzaj(0x0E);      // bajt23_16    RegA je CsrA i to 0 (jer status je 0 indeks), RegB je SP tj. R14
    sekcija.upisiUSadrzaj(0x96);      // bajt31_24


    // POP PC, sp+=8      --->    : gpr[A] <= mem32[gpr[B]]; gpr[B] <= gpr[B] + D;

    sekcija.upisiUSadrzaj(0x08);      // bajt7_0      D je 8, jer treba da azuriramo SP za obe uzete vrednosti sa steka
    sekcija.upisiUSadrzaj(0x00);      // bajt15_9     RegC se ne koristi
    sekcija.upisiUSadrzaj(0xFE);      // bajt23_16    RegA je PC tj. R15, RegB je SP tj. R14
    sekcija.upisiUSadrzaj(0x93);      // bajt31_24
    

    this->uvecajLocationCounter(8);
  }

}


void Assembler::obradiInstrukcijuCallLiteral(int literal){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    if(literal >= 0 && literal <= 4095){
      // U tom slucaju moze se smestiti na 12 bita
      // Time sto uslovljavam da mora biti pozitivan broj cuvam se od toga da mi se ne prosledi negativni 32bitni broj

      uint8_t hexa11_8 = (literal & 0xF00) >> 8;
      uint8_t hexa7_0 = literal & 0xFF;
      
      std::cout << "OVO SE PRIKAZE: " << std::endl;
      std::cout << std::hex << static_cast<int>(hexa11_8) << " " << 
      static_cast<int>(hexa7_0) << " " << std::endl;


      // call operand   ;     RegA = 0, RegB = 0, RegC = 0      (RegC svakako 0, a ne vidim sto ova dva isto ne bi bila, jer svakako koristim displacemenet za adresu skoka)
      // Npr. call 0x200 - treba da se u PC upise 0x200, sto je vrednost smestena u D polje

      std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(hexa11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju
      sekcija.upisiUSadrzaj(hexa7_0);         // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
      sekcija.upisiUSadrzaj(0x00);            // bajt23_16
      sekcija.upisiUSadrzaj(0x20);            // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      std::cout << "Adresa naredne instrukcije je " << this->dohvatiLocationCounter() << std::endl;

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;   // Nemam vise 4*brElemenataUBazenu zato sto je sada svaki bajt zasebni element

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

      // Literal je smesten u bazen, pa koristim memind adr. da pomerajem dodjem do adrese u bazenu, a taj sadrzaj je adresa koja treba da se smesti u PC
      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8       // RegC ne koristim pa mi odgovara sto je AND napravio nulu za RegC
      sekcija.upisiUSadrzaj(0xF0);            // bajt23_16      // RegA = R15 tj. PC, RegB = 0
      sekcija.upisiUSadrzaj(0x21);            // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();
    }
  }
}

void Assembler::obradiInstrukcijuCallSimbol(std::string simbol){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);


    // Dodatno, posto je u pitanju simbol, moramo da imamo i relokacioni zapis - i za masinski sadrzaj instrukcije i za bazen literala

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);

    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();

      if(pomeraj >= 0){
        // Pomeraj je pozitivan - skace se unapred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;

        std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

        // Koristi se memind adresiranje - adresa na koju se skace instrukcijom nalazi se u memoriji
        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8     
        sekcija.upisiUSadrzaj(0xF0);            // bajt23_16      // RegA = PC tj. R15, RegB = 0
        sekcija.upisiUSadrzaj(0x20);            // bajt31_24

        // Koristimo 0x20 instrukciju, jer je samo potrebno da se taj pomeraj unutar sekcije doda na PC i da se odatle dalje izvrsava kod

      }
      else{
        // Pomeraj je negativan - skace se unazad
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]

        
        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);

        // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8     
        sekcija.upisiUSadrzaj(0xF0);            // bajt23_16     // RegA = PC tj. R15, RegB = 0
        sekcija.upisiUSadrzaj(0x21);            // bajt31_24


        // Instrukcija i odrediste skoka su u istoj sekciji, ali zbog negativnog pomeraja moram da radim tako da postoji relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;

        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }

    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);

      // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8     
      sekcija.upisiUSadrzaj(0xF0);            // bajt23_16     // RegA = PC tj. R15, RegB = 0
      sekcija.upisiUSadrzaj(0x21);            // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;

      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }
    }

  }
}


void Assembler::obradiInstrukcijuRet(){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }
  
  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Instrukcija RET realizuje se preko instrukcije POP     --->    RET = POP PC
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());
    

    // Hardkodujem registre u sadrzaj jer znam da koristim samo SP i PC
    // gpr[A] <= mem32[gpr[B]]; gpr[B] <= gpr[B]+D    ;   pop pc  --->      gprA = pc, grpB = sp, D = 4
    sekcija.upisiUSadrzaj(0x04);      // bajt7_0      // Zato sto je za POP D = 4 i to staje na najnizu hexa cifru
    sekcija.upisiUSadrzaj(0x00);      // bajt15_8      // RegC = 0 jer ga ne koristim ; D11_8 = 0
    sekcija.upisiUSadrzaj(0xFE);      // bajt23_16      // R14 je SP, R15 je PC
    sekcija.upisiUSadrzaj(0x93);      // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuJmpLiteral(int literal){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    if(literal >= 0 && literal <= 4095){
      // U tom slucaju moze se smestiti na 12 bita
      // Time sto uslovljavam da mora biti pozitivan broj cuvam se od toga da mi se ne prosledi negativni 32bitni broj

      uint8_t hexa11_8 = (literal & 0xF00) >> 8;
      uint8_t hexa7_0 = literal & 0xFF;
      
      std::cout << "OVO SE PRIKAZE: " << std::endl;
      std::cout << std::hex << static_cast<int>(hexa11_8) << " " << 
      static_cast<int>(hexa7_0) << " " << std::endl;


      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju
      sekcija.upisiUSadrzaj(hexa7_0);     // bajt7_0
      sekcija.upisiUSadrzaj(hexa11_8);    // bajt15_8     // RegC = 0 zbog AND operacije, a to sam i hteo jer RegC ne koristim, D_11_8 = hexa_11_8
      sekcija.upisiUSadrzaj(0x00);        // bajt23_16
      sekcija.upisiUSadrzaj(0x30);        // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      std::cout << "Adresa naredne instrukcije je " << this->dohvatiLocationCounter() << std::endl;

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      // Koristim memind adresiranje, adresa koju treba upisati u PC je u bazenu
      sekcija.upisiUSadrzaj(pomeraj7_0);    // bajt7_0
      sekcija.upisiUSadrzaj(pomeraj11_8);   // bajt15_8       // RegC ne koristim pa mi odgovara sto je AND napravio nulu za RegC
      sekcija.upisiUSadrzaj(0xF0);          // bajt23_16      // RegA = PC tj. R15
      sekcija.upisiUSadrzaj(0x38);          // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();
    }
  }
}


void Assembler::obradiInstrukcijuJmpSimbol(std::string simbol){

  
  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);


    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);

  
    // Dodatno, posto je u pitanju simbol, moramo da imamo i relokacioni zapis - i za masinski sadrzaj instrukcije i za bazen literala

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);

    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();


      if(pomeraj >= 0){
        // Pomeraj je pozitivan - skace se unapred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Koristi se memind adresiranje - adresa na koju se skace instrukcijom nalazi se u memoriji
        sekcija.upisiUSadrzaj(pomeraj7_0);    // bajt7_0
        sekcija.upisiUSadrzaj(pomeraj11_8);   // bajt15_8     // RegC = 0, to je postignuto AND-ovanjem
        sekcija.upisiUSadrzaj(0xF0);          // bajt23_16     // RegA = PC tj. R15, RegB = 0
        sekcija.upisiUSadrzaj(0x30);          // bajt31_24
      }

      else{
        
        std::cout << "Pomeraj je negativan i iznosi " << std::dec << pomeraj << std::endl;

        // Pomeraj je negativan - skace se unazad
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]



        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);


        // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
        sekcija.upisiUSadrzaj(pomeraj7_0);    // bajt7_0
        sekcija.upisiUSadrzaj(pomeraj11_8);   // bajt15_8      // RegC = 0, to je postignuto AND-ovanjem
        sekcija.upisiUSadrzaj(0xF0);          // bajt23_16     // RegA = PC tj. R15, RegB = 0
        sekcija.upisiUSadrzaj(0x38);          // bajt31_24


        // Instrukcija i odrediste skoka su u ISTOJ SEKCIJI, ali posto je negativan pomeraj moram da radim kao da nisu u istoj sekciji, i da imam relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;
      
        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }

    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);


      // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
      sekcija.upisiUSadrzaj(pomeraj7_0);    // bajt7_0
      sekcija.upisiUSadrzaj(pomeraj11_8);   // bajt15_8      // RegC = 0, to je postignuto AND-ovanjem
      sekcija.upisiUSadrzaj(0xF0);          // bajt23_16     // RegA = PC tj. R15, RegB = 0
      sekcija.upisiUSadrzaj(0x38);          // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;
    
      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }
      
    }

  }
}


void Assembler::obradiInstrukcijuBeqLiteral(std::string reg1, std::string reg2, int literal){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // beq %gpr1, %gpr2 , operand   ;      RegA = 0, RegB = reg1, RegC = reg2
    std::string hexaReg1 = this->konvertujRegistarUHexa(reg1);
    std::string hexaReg2 = this->konvertujRegistarUHexa(reg2);


    if(literal >= 0 && literal <= 4095){
      // U tom slucaju moze se smestiti na 12 bita
      // Time sto uslovljavam da mora biti pozitivan broj cuvam se od toga da mi se ne prosledi negativni 32bitni broj

      uint8_t hexa11_8 = (literal & 0xF00) >> 8;
      uint8_t hexa7_0 = literal & 0xFF;
      
      std::cout << "OVO SE PRIKAZE: " << std::endl;
      std::cout << std::hex << static_cast<int>(hexa11_8) << " " << 
      static_cast<int>(hexa7_0) << " " << std::endl;


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0x0" + hexaReg1;
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(hexa11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju
      sekcija.upisiUSadrzaj(hexa7_0);           // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16
      sekcija.upisiUSadrzaj(0x31);              // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



      sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16
      sekcija.upisiUSadrzaj(0x39);              // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();
    }
  }
}


void Assembler::obradiInstrukcijuBeqSimbol(std::string reg1, std::string reg2, std::string simbol){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // beq %gpr1, %gpr2 , operand   ;      RegA = PC, RegB = reg1, RegC = reg2
    std::string hexaReg1 = this->konvertujRegistarUHexa(reg1);
    std::string hexaReg2 = this->konvertujRegistarUHexa(reg2);


    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);



    // Dodatno, posto je u pitanju simbol, moramo da imamo i relokacioni zapis - i za masinski sadrzaj instrukcije i za bazen literala

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);

    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();

      
      if(pomeraj >= 0){
        // Pomeraj je pozitivan - skace se unapred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

        std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


        // Koristi se memind adresiranje - adresa na koju se skace instrukcijom nalazi se u memoriji
        sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8          // RegC = Reg2
        sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16          // RegA = PC tj. R15, RegB = Reg1
        sekcija.upisiUSadrzaj(0x31);              // bajt31_24

      }
      else{
        // Pomeraj je negativan - skace se unazad
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]


        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);


        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

        std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

        // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
        sekcija.upisiUSadrzaj(pomeraj7_0);            // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);         // bajt15_8      // RegC = Reg2
        sekcija.upisiUSadrzaj(vrednost2bajt);         // bajt23_16      // RegA = PC tj. R15, RegB = Reg1
        sekcija.upisiUSadrzaj(0x39);                  // bajt31_24


        // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;

        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }

    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

      // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
      sekcija.upisiUSadrzaj(pomeraj7_0);            // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);         // bajt15_8      // RegC = Reg2
      sekcija.upisiUSadrzaj(vrednost2bajt);         // bajt23_16      // RegA = PC tj. R15, RegB = Reg1
      sekcija.upisiUSadrzaj(0x39);                  // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;

      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }

    }

  }
}


void Assembler::obradiInstrukcijuBneLiteral(std::string reg1, std::string reg2, int literal){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // bne %gpr1, %gpr2, operand   ;      RegA = 0, RegB = reg1, RegC = reg2
    std::string hexaReg1 = this->konvertujRegistarUHexa(reg1);
    std::string hexaReg2 = this->konvertujRegistarUHexa(reg2);


    if(literal >= 0 && literal <= 4095){
      // U tom slucaju moze se smestiti na 12 bita
      // Time sto uslovljavam da mora biti pozitivan broj cuvam se od toga da mi se ne prosledi negativni 32bitni broj

      uint8_t hexa11_8 = (literal & 0xF00) >> 8;
      uint8_t hexa7_0 = literal & 0xFF;
      
      std::cout << "OVO SE PRIKAZE: " << std::endl;
      std::cout << std::hex << static_cast<int>(hexa11_8) << " " << 
      static_cast<int>(hexa7_0) << " " << std::endl;


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0x0" + hexaReg1;
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(hexa11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju
      sekcija.upisiUSadrzaj(hexa7_0);               // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);         // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);         // bajt23_16
      sekcija.upisiUSadrzaj(0x32);                  // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0xF" + hexaReg1;        // RegA = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



      sekcija.upisiUSadrzaj(pomeraj7_0);          // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);       // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);       // bajt23_16
      sekcija.upisiUSadrzaj(0x3A);                // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();
    }
  }
}


void Assembler::obradiInstrukcijuBneSimbol(std::string reg1, std::string reg2, std::string simbol){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // bne %gpr1, %gpr2, operand   ;      RegA = PC tj. R15, RegB = reg1, RegC = reg2
    std::string hexaReg1 = this->konvertujRegistarUHexa(reg1);
    std::string hexaReg2 = this->konvertujRegistarUHexa(reg2);

    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);



    // Dodatno, posto je u pitanju simbol, moramo da imamo i relokacioni zapis - i za masinski sadrzaj instrukcije i za bazen literala

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);

     // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();


      if(pomeraj >= 0){
        // Pomeraj je pozitivan - skace se unapred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

        std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


        // Koristi se memind adresiranje - adresa na koju se skace instrukcijom nalazi se u memoriji
        sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8          // RegC = Reg2
        sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16          // RegA = PC tj. R15, RegB = Reg1
        sekcija.upisiUSadrzaj(0x32);              // bajt31_24

      }
      else{
        // Pomeraj je negativan - skace se unazad
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]


        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);


        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

        std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

        // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
        sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8           // RegC = Reg2
        sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16          // RegA = PC tj. R15, RegB = Reg1
        sekcija.upisiUSadrzaj(0x3A);              // bajt31_24


        // Instrukcija i odrediste skoka su u ISTOJ SEKCIJI, ali posto je negativan pomeraj moram da radim kao da nisu u istoj sekciji, i da imam relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;

        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }
      
    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

      // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
      sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8           // RegC = Reg2
      sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16          // RegA = PC tj. R15, RegB = Reg1
      sekcija.upisiUSadrzaj(0x3A);              // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;

      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }

    }

  }
}


void Assembler::obradiInstrukcijuBgtLiteral(std::string reg1, std::string reg2, int literal){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // bgt %gpr1, %gpr2, operand   ;      RegA = 0, RegB = reg1, RegC = reg2
    std::string hexaReg1 = this->konvertujRegistarUHexa(reg1);
    std::string hexaReg2 = this->konvertujRegistarUHexa(reg2);


    if(literal >= 0 && literal <= 4095){
      // U tom slucaju moze se smestiti na 12 bita
      // Time sto uslovljavam da mora biti pozitivan broj cuvam se od toga da mi se ne prosledi negativni 32bitni broj

      uint8_t hexa11_8 = (literal & 0xF00) >> 8;
      uint8_t hexa7_0 = literal & 0xFF;
      
      std::cout << "OVO SE PRIKAZE: " << std::endl;
      std::cout << std::hex << static_cast<int>(hexa11_8) << " " << 
      static_cast<int>(hexa7_0) << " " << std::endl;


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0x0" + hexaReg1;
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(hexa11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju
      sekcija.upisiUSadrzaj(hexa7_0);             // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);       // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);       // bajt23_16
      sekcija.upisiUSadrzaj(0x33);                // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0xF" + hexaReg1;        // RegA = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
      sekcija.upisiUSadrzaj(0x3B);            // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();
    }
  }
}


void Assembler::obradiInstrukcijuBgtSimbol(std::string reg1, std::string reg2, std::string simbol){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
     // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

     // bgt %gpr1, %gpr2, operand   ;
    std::string hexaReg1 = this->konvertujRegistarUHexa(reg1);
    std::string hexaReg2 = this->konvertujRegistarUHexa(reg2);

    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);



    // Dodatno, posto je u pitanju simbol, moramo da imamo i relokacioni zapis - i za masinski sadrzaj instrukcije i za bazen literala

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);

    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();


      if(pomeraj >= 0){
        // Pomeraj je pozitivan - skace se unapred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

        std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


        // Koristi se memind adresiranje - adresa na koju se skace instrukcijom nalazi se u memoriji
        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8             // RegC = Reg2
        sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16            // RegA = PC tj. R15, RegB = Reg1
        sekcija.upisiUSadrzaj(0x33);            // bajt31_24

      }
      else{
        // Pomeraj je negativan - skace se unazad
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]


        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);

        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

        std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

        // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
        sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8           // RegC = Reg2
        sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16          // RegA = PC tj. R15, RegB = Reg1
        sekcija.upisiUSadrzaj(0x3B);              // bajt31_24


        // Instrukcija i odrediste skoka su u ISTOJ SEKCIJI, ali posto je negativan pomeraj moram da radim kao da nisu u istoj sekciji, i da imam relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;

        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }

    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);

      // Napravi sadrzaj
      std::string sadrzaj2bajt = "0xF" + hexaReg1;      // RegA = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaReg2 + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 

      // Formiramo masinski sadrzaj - koristimo memind adresiranje, jer se u PC smesta simbol sa te adrese iz bazena
      sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8           // RegC = Reg2
      sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16          // RegA = PC tj. R15, RegB = Reg1
      sekcija.upisiUSadrzaj(0x3B);              // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;

      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }

    }


  }
}


void Assembler::obradiInstrukcijuPush(std::string reg){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Instrukcija PUSH realizuje se preko instrukcije STORE
    // Reg prosledjen kao argument moze biti samo neki od GPR registara


    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // gpr[A] <= gpr[A] + D ; mem32[gpr[A]] <= gpr[C]    ;   push rX    ;  gprA = SP, gprC = rX 
    
    // S tim da ja ovde necu koristiti D = -4, nego D = 4, ali cu u emulatoru na osnovu procitanog operacionog koda znati da je ovo PUSH instrukcija i da ovu D vrednost treba da koristim negativnu
    std::string hexaReg = this->konvertujRegistarUHexa(reg);

    std::string sadrzaj3bajt = "0x" + hexaReg + "0";     // SP = E tj. R14
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x04);            // bajt7_0       // Zato sto je za PUSH D = 4 i to staje na najnizu hexa cifru (ali cu u emulatoru znati da za ovaj OP kod koristim -D)
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(0xE0);            // bajt23_16     // R14 je SP (vrednost E) ; RegB = 0 jer ne koristim
    sekcija.upisiUSadrzaj(0x81);            // bajt31_24


    this->uvecajLocationCounter(4);

  }
}


void Assembler::obradiInstrukcijuPop(std::string reg){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Instrukcija POP realizuje se preko instrukcije LOAD
    // Reg prosledjen kao argument moze biti samo neki od GPR registara

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // gpr[A] <= mem32[gpr[B]]; gpr[B] <= gpr[B]+D    ;   pop rX  --->      gprA = rX, grpB = sp, D = 4
    std::string hexaReg = this->konvertujRegistarUHexa(reg);

    std::string sadrzaj2bajt = "0x" + hexaReg + "E";
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



    sekcija.upisiUSadrzaj(0x04);            // bajt7_0        // Zato sto je za POP D = 4 i to staje na najnizu hexa cifru
    sekcija.upisiUSadrzaj(0x00);            // bajt15_8       // RegC = 0 jer ga ne koristim ; D11_8 = 0
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16      // R14 je SP
    sekcija.upisiUSadrzaj(0x93);            // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuXchg(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // xchg %gprSrc , gprDst     ;   temp <= gprDst ; gprDst <= gprSrc ; gprSrc <= temp;      ---> RegB = dst, RegC = src

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Obradi registre
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    // Formiraj vrednost
    std::string sadrzaj2bajt = "0x0" + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);  

    
    // Unesi sadrzaj
    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x40);            // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuAdd(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // add %gprS, %gprD   ;   Dst = Dst + Src     --->        RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x50);            // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuSub(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // sub %gprS, %gprD ;     Dst = Dst - Src     --->      RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);



    sekcija.upisiUSadrzaj(0x00);              // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16
    sekcija.upisiUSadrzaj(0x51);              // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuMul(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // mul %gprS, %gprD ;   Dst = Dst * Src       --->      RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);



    sekcija.upisiUSadrzaj(0x00);              // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16
    sekcija.upisiUSadrzaj(0x52);              // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuDiv(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // div %gprS, %gprD ;   Dst = Dst / Src     --->      RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);



    sekcija.upisiUSadrzaj(0x00);                // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);       // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);       // bajt23_16
    sekcija.upisiUSadrzaj(0x53);                // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuNot(std::string reg){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // not %gpr ;   dst = not(dst)        --->    RegA = dst, RegB = dst

    std::string hexaReg = this->konvertujRegistarUHexa(reg);    // Npr.   od R12 -> C
    std::string sadrzaj = "0x" + hexaReg + hexaReg;             // Da napravimo oblik 0xCC
    uint8_t vrednost = this->konvertujHexaZapisUInt8(sadrzaj);       // Vraca vrednost CC

    // Ne moze da se ispise direktno CC nego mora da se uradi ovako kako je ispod, preko std::hex i cast-om
    //std::cout << "OVA VREDNOST JE " << std::hex << static_cast<int>(val) << std::endl;

    
    // Na mesto RegA i RegB koristim isti registar, a to je onaj registar koji je specificiran u instrukciji
    // RegC ne koristim tj. ima vrednost 0
    sekcija.upisiUSadrzaj(0x00);        // bajt7_0
    sekcija.upisiUSadrzaj(0x00);        // bajt15_8
    sekcija.upisiUSadrzaj(vrednost);    // bajt23_16
    sekcija.upisiUSadrzaj(0x60);        // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuAnd(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // and %gprS, %gprD ;  Dst = Dst AND Src    --->      RegA = dst, RegB = dst, RegC = src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x61);            // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuOr(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // or %gprS, %gprD ;  Dst = Dst OR Src      --->        RegA = Dst , RegB = Dst , RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x62);            // bajt31_24


    this->uvecajLocationCounter(4);

  }
}


void Assembler::obradiInstrukcijuXor(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // xor %gprS, %gprD ;   Dst = Dst XOR Src     --->      RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x63);            // bajt31_24


    this->uvecajLocationCounter(4);    
  }
}


void Assembler::obradiInstrukcijuShl(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // shl %gprS, %gprD ;   Dst = Dst << Src        --->        RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x70);            // bajt31_24


    this->uvecajLocationCounter(4);    
  }
}


void Assembler::obradiInstrukcijuShr(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // shr %gprS, %gprD ;    Dst = Dst >> Src       --->        RegA = Dst, RegB = Dst, RegC = Src
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x71);            // bajt31_24


    this->uvecajLocationCounter(4);    

  }
}


void Assembler::obradiInstrukcijuLdLiteralVrednost(int literal, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Obradi registar
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);
    

    if(literal >= 0 && literal <= 4095){

      // U tom slucaju moze se smestiti na 12 bita

      uint8_t hexa11_8 = (literal & 0xF00) >> 8;
      uint8_t hexa7_0 = literal & 0xFF;

      // ld $literal, %gpr   ;     RegA = regDst, RegB = 0, RegC = 0, D = literal
      // Npr. $0x200, %r2   --->  r2 = 0x200

      std::string sadrzaj2bajt = "0x" + hexaRegDst + "0";
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x0" + this->konvertuj4BitLiteralUHexa(hexa11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju
      sekcija.upisiUSadrzaj(hexa7_0);         // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
      sekcija.upisiUSadrzaj(0x91);            // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{

      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";   // RegB = PC tj. R15 (F)
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 


      // Koristim memind adresiranje, adresa koju treba upisati u PC je u bazenu
      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(pomeraj11_8);     // bajt15_8       // RegC ne koristim pa mi odgovara sto je AND napravio nulu za RegC
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
      sekcija.upisiUSadrzaj(0x92);            // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();
    }

  }
}


void Assembler::obradiInstrukcijuLdSimbolVrednost(std::string simbol, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

     // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);



    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();

      if(pomeraj >= 0){
        // Pomeraj je pozitivan - uzima se simbol koji je definisan napred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Napravi sadrzaj
        std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";      // RegB = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(pomeraj11_8);     // bajt15_8       // RegC = 0 jer se ne koristi
        sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16      // RegA = RegDST, RegB = PC tj. 15
        sekcija.upisiUSadrzaj(0x91);            // bajt31_24

      }
      else{
        // Pomeraj je negativan - uzima se simbol definisan ranije
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]


        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);


        // Napravi sadrzaj - ovde radimo memind adresiranje
        std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";      // RegB = PC tj. R15
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(pomeraj11_8);     // bajt15_8       // RegC = 0 jer se ne koristi
        sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16      // RegA = RegDST, RegB=PC tj. R15
        sekcija.upisiUSadrzaj(0x92);            // bajt31_24


        // Posto je u pitanju simbol - postoji relokacioni zapis ; Iako smo u istoj sekciji, posto se koristio negativan pomeraj, morali smo da radimo preko relok. zapisa jer nema te instrukcije koja bi omogucila sabiranje sa negativnom vrednoscu dohvacene iz bazena literala
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;

        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }

    }

    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);


      // Napravi sadrzaj - ovde radimo memind adresiranje
      std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";      // RegB = PC tj. R15
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(pomeraj11_8);     // bajt15_8       // RegC = 0 jer se ne koristi
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16      // RegA = RegDST, RegB=PC tj. R15
      sekcija.upisiUSadrzaj(0x92);            // bajt31_24


      // Posto je u pitanju simbol - postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;

      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }

    }

  }

}


void Assembler::obradiInstrukcijuLdLiteralMem(int literal, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){

    if(literal >= 0 && literal <= 4095){
      this->uvecajLocationCounter(4);
    }
    else{
      this->uvecajLocationCounter(8);
    }
    
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // PITANJE! Da li ovde treba da stavim da mi u oba slucaja instrukcija bude velicine 8B, iako samo u ovom drugom slucaju imam 2 instrukcije, ali kao stavim da je svakako 8B da bi bilo uniformno?
    
    // Radimo mem[literal], imamo dva slucaja - literal moze u 12 bita --> odmah ga ukodujemo
    //                                        - literal veci od 12 bita ---> ide u bazen pa ukodujemo PC+pomeraj do literala u bazenu, tada moramo mem[mem[PC+pomeraj]]


    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Obradi registar
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);


    if(literal >= 0 && literal <= 4095){
      // Moze u 12 bita - odmah ga ukodujemo u sadrzaj i radimo memind sa tom vrednoscu

      uint8_t hexaLiteral11_8 = (literal & 0xF00) >> 8;
      uint8_t hexaLiteral7_0 = literal & 0xFF;


      // Formiramo sadrzaj
      std::string sadrzaj2bajt = "0x" + hexaRegDst + "0";
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



      sekcija.upisiUSadrzaj(hexaLiteral7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(hexaLiteral11_8);     // bajt15_8     // RegC = 0, napravilo ga AND-ovanje, ne koristi se u ovom slucaju
      sekcija.upisiUSadrzaj(vrednost2bajt);       // bajt23_16
      sekcija.upisiUSadrzaj(0x92);                // bajt31_24


      this->uvecajLocationCounter(4);

    }

    else{

      // Posto hocu da se uradi cuvanje vrednosti iz MEMORIJE od ovog literala, sa prvim memind iz bazena dohvatam vrednost literala (adresu)
      // A sa drugim memind dohvatam sadrzaj iz memorije

      // PRVI MEMIND DEO
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      // Koristim memind adresiranje - ovim cemo pregaziti vrednost iz regDst, ali to i nije bitno, jer je bitno da se na kraju instrukcije tamo nadje trazena vrednost iz memorije

      std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";     // Koristimo PC tj. F (R15)
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



      sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
      sekcija.upisiUSadrzaj(pomeraj11_8);       // bajt15_8     // RegC ne koristim pa mi odgovara sto je AND napravio nulu za RegC
      sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16
      sekcija.upisiUSadrzaj(0x92);              // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literal);

      //sekcija.ispisiSadrzajBazena();



      // DRUGI DEO - U ovom trenutku u gprA se nalazi vrednost literala (adresa) - sada treba odraditi mem[gprA]
      // RegB je zapravo RegA, posto se radi mem[regB]

      sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
      vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);


      sekcija.upisiUSadrzaj(0x00);            // bajt7_0
      sekcija.upisiUSadrzaj(0x00);            // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
      sekcija.upisiUSadrzaj(0x92);            // bajt31_24


      // Posto smo izvrsili jos jednu instrukciju moramo da uvecamo LC za jos 4
      this->uvecajLocationCounter(4);
    }

  }

}


void Assembler::obradiInstrukcijuLdSimbolMem(std::string simbol, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(8);     // Jer uvek izvrsavamo dve instrukcije
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    

    // Slicno kao za literal mem - i ovde moram da imam dva memind adresiranja - prvo da dodjem do vrednosti simbola, a drugo da dohvatim sadrzaj iz memorije sa te adrese

    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);


    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Obradi registar
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);


    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbol);

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbol);


    // Deo kada je simbol U SEKCIJI (a pomeraj pozitivan) moze da se uradi jednim memind adresiranjem
    // ali kako bi mi instrukcija u oba slucaja bila uniformne velicine od 8B
    // odlucio sam da je napisem kao dve instrukcije od 4B
    

    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();


      if(pomeraj >= 0){
        // Pomeraj je pozitivan

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Radimo PC + pomeraj do simbola unutar sekcije - smestimo tu adresu u dst registar
        std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);



        sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
        sekcija.upisiUSadrzaj(pomeraj11_8);       // bajt15_8    // RegC = 0 jer se ne koristi
        sekcija.upisiUSadrzaj(vrednost2bajt);     // bajt23_16   // RegA = RegDst, RegB = PC tj. R15
        sekcija.upisiUSadrzaj(0x91);              // bajt31_24


        // DRUGI DEO - kad smo dosli do simbola, odradimo jedan memind da uzmemo vrednost sa te adrese iz memorije
        // RegB = RegA, jer se u RegA nalazi vrednost simbola, a RegC i D su 0

        sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;
        vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);


        sekcija.upisiUSadrzaj(0x00);            // bajt7_0
        sekcija.upisiUSadrzaj(0x00);            // bajt15_8
        sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
        sekcija.upisiUSadrzaj(0x92);            // bajt31_24


        // Jer smo izvrsili jos jednu instrukciju
        this->uvecajLocationCounter(4);

      }
      else{
        // Pomeraj je negativan
        // Posto se koristi negativan broj, to znaci da se on mora zapisati na svih 32 bita
        // Ne stavljam negativni pomeraj u bazen literala, vec radim sa relokacionim zapisima, kao kad simbol nije iz iste sekcije
        
        // Za pojasnjenje zasto se ovako radi, pogledati assembler.h --> linije [196, 205]


        // PRVI DEO

        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);


        // Formiramo masinski sadrzaj

        std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";   // RegB = PC
        uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);



        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(pomeraj11_8);     // bajt15_8      // RegC = 0, napravljen AND-ovanjem
        sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16     // RegA = RegDST, RegB = PC tj. R15
        sekcija.upisiUSadrzaj(0x92);            // bajt31_24


        // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;
      
        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }



        // DRUGI DEO - vrednost iz RegA iskoristimo da uzmemo sadrzaj iz memorije
        sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;   // RegB = RegA
        vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);



        sekcija.upisiUSadrzaj(0x00);            // bajt7_0
        sekcija.upisiUSadrzaj(0x00);            // bajt15_8
        sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
        sekcija.upisiUSadrzaj(0x92);            // bajt31_24


        // Jer smo izvrsili jos jednu instrukciju
        this->uvecajLocationCounter(4);

      }
      
    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);


      // Formiramo masinski sadrzaj

      std::string sadrzaj2bajt = "0x" + hexaRegDst + "F";   // RegB = PC
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);



      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(pomeraj11_8);     // bajt15_8      // RegC = 0, napravljen AND-ovanjem
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16     // RegA = RegDST, RegB = PC tj. R15
      sekcija.upisiUSadrzaj(0x92);            // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;
    
      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbol, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }



      // DRUGI DEO - vrednost iz RegA iskoristimo da uzmemo sadrzaj iz memorije
      sadrzaj2bajt = "0x" + hexaRegDst + hexaRegDst;   // RegB = RegA
      vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt);



      sekcija.upisiUSadrzaj(0x00);            // bajt7_0
      sekcija.upisiUSadrzaj(0x00);            // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
      sekcija.upisiUSadrzaj(0x92);            // bajt31_24


      // Jer smo izvrsili jos jednu instrukciju
      this->uvecajLocationCounter(4);
      
    }

  }

}


void Assembler::obradiInstrukcijuLdGPRVrednost(std::string regSrc, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);


    // Formiramo sadrzaj
    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegSrc;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 



    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(0x00);            // bajt15_8       // RegC i D se ne koriste
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16      // RegA = dst, RegB = src
    sekcija.upisiUSadrzaj(0x91);            // bajt31_24


    this->uvecajLocationCounter(4);
  }

}


void Assembler::obradiInstrukcijuLdGPRMem(std::string srcAddr, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    std::string hexaRegSrc = this->konvertujRegistarUHexa(srcAddr);
    std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);


    // Formiramo sadrzaj
    std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegSrc;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 


    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(0x00);            // bajt15_8    // RegC i D se ne koriste
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16   // RegA = dst, RegB = src
    sekcija.upisiUSadrzaj(0x92);            // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


int Assembler::obradiInstrukcijuLdGPRMemOffsLiteral(std::string srcAddr, int literalOffs, std::string regDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Identicno kao obradiInstrukcijuLdGPRMem, samo sto se dodaje offset dat literalom

    if(literalOffs >= 0 && literalOffs <= 4095){    // Moze na 12 bita

      // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
      Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

      std::string hexaRegSrc = this->konvertujRegistarUHexa(srcAddr);
      std::string hexaRegDst = this->konvertujRegistarUHexa(regDst);

      uint8_t hexaLiteral11_8 = (literalOffs & 0xF00) >> 8;
      uint8_t hexaLiteral7_0 = literalOffs & 0xFF;


      // Formiramo sadrzaj
      std::string sadrzaj2bajt = "0x" + hexaRegDst + hexaRegSrc;
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 


      sekcija.upisiUSadrzaj(hexaLiteral7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(hexaLiteral11_8);     // bajt15_8     // RegC ima vrednost 0 koja je formirana AND-ovanjem
      sekcija.upisiUSadrzaj(vrednost2bajt);       // bajt23_16    // RegA = dst, RegB = src
      sekcija.upisiUSadrzaj(0x92);                // bajt31_24


      this->uvecajLocationCounter(4);

      return 0;     // Uspesna obrada
    }
    else{
      // Literal mora da moze da stane u 12 bita, zato sto se pomeraj navodi u polju Displacement, koje je sirine 12 bita
      return InfoKod::LITERAL_NE_MOZE_U_12_BITA;
    }

  }

  
}


void Assembler::obradiInstrukcijuStLiteralMem(std::string regSrc, int literalAddrDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Obradi registar
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);


    // Da li moze da stane na 12 bita
    if(literalAddrDst >= 0 && literalAddrDst <= 4095){
      // U tom slucaju moze se smestiti na 12 bita
      // Time sto uslovljavam da mora biti pozitivan broj cuvam se od toga da mi se ne prosledi negativni 32bitni broj

      uint8_t hexa11_8 = (literalAddrDst & 0xF00) >> 8;
      uint8_t hexa7_0 = literalAddrDst & 0xFF;


      // Pomeraj jeste do 12 bita - moze se ukodovati u instrukciju

      std::string sadrzaj3bajt = "0x" + hexaRegSrc + this->konvertuj4BitLiteralUHexa(hexa11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);


      sekcija.upisiUSadrzaj(hexa7_0);         // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8      // RegC = src
      sekcija.upisiUSadrzaj(0x00);            // bajt23_16     // RegA i RegB su 0 jer se ne koriste
      sekcija.upisiUSadrzaj(0x80);            // bajt31_24

      this->uvecajLocationCounter(4);
    }

    else{
      std::cout << "Ne moze na 12 bita - mora u bazen!" << std::endl;

      // Prvo uvecam LC, zato sto hocu da pomeraj odredim u odnosu na narednu instrukciju
      this->uvecajLocationCounter(4);


      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten ili kako vec
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();

      int adresaSmestanjaUBazen = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;

      uint8_t pomeraj11_8 = (adresaSmestanjaUBazen & 0xF00) >> 8;
      uint8_t pomeraj7_0 = adresaSmestanjaUBazen & 0xFF;


      // Koristim ono duplo memind adresiranje
      std::string sadrzaj3bajt = "0x" + hexaRegSrc + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);


      sekcija.upisiUSadrzaj(pomeraj7_0);        // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);     // bajt15_8     // RegC ne koristim pa mi odgovara sto je AND napravio nulu za RegC
      sekcija.upisiUSadrzaj(0xF0);              // bajt23_16    // RegA = PC tj. R15, RegB se ne koristi pa je 0
      sekcija.upisiUSadrzaj(0x82);              // bajt31_24


      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(literalAddrDst);

      //sekcija.ispisiSadrzajBazena();
    }
    
  }
}


void Assembler::obradiInstrukcijuStSimbolMem(std::string regSrc, std::string simbolAddrDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Prvo uvecamo LC, kako bismo radili sa adresom naredne instrukcije
    this->uvecajLocationCounter(4);


    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    // Obradi registar
    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);


    // Dohvati vrednost simbola iz tabele simbola - to je njegova adresa tj. offset
    int vrednostSimbola = this->dohvatiVrednostSimbolaIzTS(simbolAddrDst);

  
    // Dodatno, posto je u pitanju simbol, moramo da imamo i relokacioni zapis - i za masinski sadrzaj instrukcije i za bazen literala

    ZapisTS redIzTS = this->dohvatiRedIzTabeleSimbola(simbolAddrDst);



    // Da li je simbol u istoj sekciji kao i instrukcija
    if(redIzTS.dohvatiSekciju() == this->dohvatiTrenutnuSekciju()){
      // U ovom slucaju relokacioni zapis NE POSTOJI - u istoj smo sekciji pa je pomeraj fiksan. POMERAJ enkodujemo u sadrzaj.

      int pomeraj = vrednostSimbola - this->dohvatiLocationCounter();


      if(pomeraj >= 0){
        // Pomeraj je pozitivan - simbol je definisan napred

        uint8_t pomeraj11_8 = (pomeraj & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomeraj & 0xFF;


        // Radimo PC + pomeraj do simbola unutar sekcije
        std::string sadrzaj3bajt = "0x" + hexaRegSrc + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);


        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8      // RegC = regSRC
        sekcija.upisiUSadrzaj(0xF0);            // bajt23_16     // RegA = PC tj. R15, RegB = 0 jer se ne koristi
        sekcija.upisiUSadrzaj(0x80);            // bajt31_24

      }
      else{
        // Pomeraj je negativan - simbol je definisan ranije

        // MEDJUTIM, meni ove instrukcije ne dozvoljavaju da u bazen prvo smestim negativni pomeraj, pa da onda uradim mem[PC + negPomeraj]
        // Zato sto bi 0x82 instrukcija radila bukvalno kao mem[ mem[negPomeraj]] u mom slucaju, a to je neispravno
        // Zato ovde, i u slucaju da je pomeraj negativan, ja radim tako da u bazen smestam VREDNOST simbola, pa cu imati relokacioni zapis za njega

        // Efektivno radim isti slucaj kao dole ispod, kada simbol nije u istoj sekciji
        // Prosto moram, jer nikako drugacije ne mogu u PC da dodam negativni pomeraj, pa mi je ovaj pristup jedino sto preostaje
        
        // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
        int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
        int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
        uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
        uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

        // Dodaj u bazen
        sekcija.dodajUBazenLiterala(vrednostSimbola);



        // Formiramo masinski sadrzaj - koristimo duplo memind adresiranje
        // Radimo PC + pomeraj do simbola u bazenu literala
        std::string sadrzaj3bajt = "0x" + hexaRegSrc + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
        uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);


        sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
        sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8      // RegC = regSRC
        sekcija.upisiUSadrzaj(0xF0);            // bajt23_16     // RegA = PC tj. R15, RegB = 0
        sekcija.upisiUSadrzaj(0x82);            // bajt31_24


        // Instrukcija i odrediste skoka su u ISTOJ SEKCIJI, ali posto je negativan pomeraj moram da radim tako da imam relokacioni zapis
        // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

        // Formiramo relokacioni zapis za simbol
        std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
        uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
        int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
        std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
        int addend = 0;
      
        if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
          addend = redIzTS.dohvatiOffset();
          RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

          sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
        }

        else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

          // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
          RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbolAddrDst, addend);

          sekcija.dodajRelokacioniZapis(globalSimbolRelok);
        }

      }
      
    }
    else{

      // Simbol uvek dodajemo u bazen literala (preventivno, jer iako mozda i sada moze da stane u 12 bita, mozda nece moci kada linker uradi relokaciju)
      // Zato je bolje da ga ubacimo u bazen, jer ce se tada dohvatati sa ovog offset-a, a linker ce njegovu vrednost izmeniti u memoriji

      // Moras da vidis na kom je poslednje mestu u bazenu smesten simbol - dohvati indeks pa *4 kako bi znao gde je poslednji smesten
      int brElemenataUBazenu = sekcija.dohvatiBazenLiterala().size();
      int pomerajDoElemUBazenu = sekcija.dohvatiVelicinuSekcije() - this->dohvatiLocationCounter() + brElemenataUBazenu;
      uint8_t pomeraj11_8 = (pomerajDoElemUBazenu & 0xF00) >> 8;
      uint8_t pomeraj7_0 = pomerajDoElemUBazenu & 0xFF;

      // Dodaj u bazen
      sekcija.dodajUBazenLiterala(vrednostSimbola);



      // Formiramo masinski sadrzaj - koristimo duplo memind adresiranje
      // Radimo PC + pomeraj do simbola u bazenu literala
      std::string sadrzaj3bajt = "0x" + hexaRegSrc + this->konvertuj4BitLiteralUHexa(pomeraj11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt);


      sekcija.upisiUSadrzaj(pomeraj7_0);      // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8      // RegC = regSRC
      sekcija.upisiUSadrzaj(0xF0);            // bajt23_16     // RegA = PC tj. R15, RegB = 0
      sekcija.upisiUSadrzaj(0x82);            // bajt31_24


      // Instrukcija i odrediste skoka su u razl. sekcijama pa postoji relokacioni zapis
      // Da li je simbol globalan ili lokalan - ovo nam je bitno pri formiranju relokacionog zapisa

      // Formiramo relokacioni zapis za simbol
      std::string nazivSekcije = this->dohvatiNazivSekcijePremaRednomBrojuUTS(this->dohvatiTrenutnuSekciju());
      uint32_t pocetnaAdresaSekcije = this->dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(nazivSekcije);   // Pocetna adresa sekcije je u odnosu na sekciju u kojoj je instrukcija, a ne simbol; To je zato jer ce linker menjati vrednost simbola u bazenu, a bazen je odmah ispod OVE sekcije u kojoj je instrukcija
      int offsetDoMestaPrepravke = this->dohvatiLocationCounter() + pomerajDoElemUBazenu;
      std::string nazivSekcijeSimbola = this->dohvatiNazivSekcijePremaRednomBrojuUTS(redIzTS.dohvatiSekciju());
      int addend = 0;
    
      if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::LOKALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ;  Addend = pomeraj do simbola unutar ove sekcije
        addend = redIzTS.dohvatiOffset();
        RelokZapis lokalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", nazivSekcijeSimbola, addend);

        sekcija.dodajRelokacioniZapis(lokalSimbolRelok);
      }

      else if(redIzTS.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO){

        // Apsolutno adresiranje - menjamo konkretnu vrednost ; Addend = 0 jer koristimo konkretan simbol
        RelokZapis globalSimbolRelok(pocetnaAdresaSekcije, offsetDoMestaPrepravke, "X86_64_32", simbolAddrDst, addend);

        sekcija.dodajRelokacioniZapis(globalSimbolRelok);
      }
      
    }
  }
}


void Assembler::obradiInstrukcijuStGPRMem(std::string regSrc, std::string addrDst){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

    std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
    std::string hexaRegDst = this->konvertujRegistarUHexa(addrDst);


    // Formiramo sadrzaj - RegC = src, RegA = dst
    std::string sadrzaj2bajt = "0x" + hexaRegDst + "0";
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

    std::string sadrzaj3bajt = "0x" + hexaRegSrc + "0";
    uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x80);            // bajt31_24


    this->uvecajLocationCounter(4);

  }
}


int Assembler::obradiInstrukcijuStGPRMemOffsLiteral(std::string regSrc, std::string addrDst, int literalOffs){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){
    
    // Identicno kao obradiInstrukcijuStGPRMem, samo se doda literal koji predstavlja dodatni pomeraj

    if(literalOffs >= 0 && literalOffs <= 4095){

      // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
      Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());

      std::string hexaRegSrc = this->konvertujRegistarUHexa(regSrc);
      std::string hexaRegDst = this->konvertujRegistarUHexa(addrDst);

      uint8_t hexaLiteral11_8 = (literalOffs & 0xF00) >> 8;
      uint8_t hexaLiteral7_0 = literalOffs & 0xFF;


      // Formiramo sadrzaj - RegC = src, RegA = dst
      std::string sadrzaj2bajt = "0x" + hexaRegDst + "0";
      uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 

      std::string sadrzaj3bajt = "0x" + hexaRegSrc + this->konvertuj4BitLiteralUHexa(hexaLiteral11_8);
      uint8_t vrednost3bajt = this->konvertujHexaZapisUInt8(sadrzaj3bajt); 


      sekcija.upisiUSadrzaj(hexaLiteral7_0);  // bajt7_0
      sekcija.upisiUSadrzaj(vrednost3bajt);   // bajt15_8
      sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
      sekcija.upisiUSadrzaj(0x80);            // bajt31_24


      this->uvecajLocationCounter(4);

    }

    else{
      // Literal mora da stane u 12 bita, zato sto se on ubacuje u polje Displacement
      return InfoKod::LITERAL_NE_MOZE_U_12_BITA;
    }

  }
}


void Assembler::obradiInstrukcijuCsrRd(std::string regCsr, std::string regGpr){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // csrrd %csr, %gpr ; gpr <= csr ;   Dst = Src       --->          RegA = Dst, RegB = Src, RegC = 0, D = 0
    std::string hexaRegGpr = this->konvertujRegistarUHexa(regGpr);
    std::string hexaRegCsr;

    if(regCsr == "%status"){
      hexaRegCsr = "0";
    }
    else if(regCsr == "%handler"){
      hexaRegCsr = "1";
    }
    else if(regCsr == "%cause"){
      hexaRegCsr = "2";
    }

    
    std::string sadrzaj2bajt = "0x" + hexaRegGpr + hexaRegCsr;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 


    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(0x00);            // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x90);            // bajt31_24


    this->uvecajLocationCounter(4);
  }
}


void Assembler::obradiInstrukcijuCsrWr(std::string regGpr, std::string regCsr){

  if(this->dohvatiTekuciProlaz() == ASMProlaz::PRVI_PROLAZ){
    this->uvecajLocationCounter(4);
  }

  if(this->dohvatiTekuciProlaz() == ASMProlaz::DRUGI_PROLAZ){

    // Nadji tekucu sekciju u tabeli sekcija i za nju upisi sadrzaj
    Sekcija& sekcija = this->dohvatiRedIzTabeleSekcija(this->dohvatiNazivTrenutneSekcije());


    // csrwr %gpr, %csr   ;   csr = gpr       --->         RegA = Dst, RegB = Src, RegC = 0, D = 0
    std::string hexaRegGpr = this->konvertujRegistarUHexa(regGpr);
    std::string hexaRegCsr;

    if(regCsr == "%status"){
      hexaRegCsr = "0";
    }
    else if(regCsr == "%handler"){
      hexaRegCsr = "1";
    }
    else if(regCsr == "%cause"){
      hexaRegCsr = "2";
    } 


    std::string sadrzaj2bajt = "0x" + hexaRegCsr + hexaRegGpr;
    uint8_t vrednost2bajt = this->konvertujHexaZapisUInt8(sadrzaj2bajt); 


    sekcija.upisiUSadrzaj(0x00);            // bajt7_0
    sekcija.upisiUSadrzaj(0x00);            // bajt15_8
    sekcija.upisiUSadrzaj(vrednost2bajt);   // bajt23_16
    sekcija.upisiUSadrzaj(0x94);            // bajt31_24


    this->uvecajLocationCounter(4); 
  }
}

// --- Obrada instrukcija END ---

// ---------- Funkcije za asemblersku obradu END ----------



// ---------- Funkcije za tabelu simbola START ----------

std::list<ZapisTS> Assembler::dohvatiTabeluSimbola(){
  return tabelaSimbola;
}

void Assembler::postaviTabeluSimbola(std::list<ZapisTS> ts){
  tabelaSimbola = ts;
}

void Assembler::dodajZapisUTabeluSimbola(ZapisTS zapis){
  tabelaSimbola.push_back(zapis);
}


void Assembler::dodajSekcijuUTabeluSekcija(Sekcija sekcija){
  tabelaSekcija.push_back(sekcija);
}


void Assembler::ispisiSadrzajTabeleSimbola(){

  // Za lep ispis tabele simbola

  const char separator = ' ';
  const int sirinaKaraktera = 15;

  // Zaglavlje
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Simbol";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Sekcija";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Offset";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Povezivanje";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Redni broj";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "isExtern";
  std::cout << std::endl;


  // Sadrzaj
  for(ZapisTS iterator : this->dohvatiTabeluSimbola()){    
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiSimbol();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiSekciju();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiOffset();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiNacinPovezivanja();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiRedniBrojUTS();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.daLiJeExtern();
    std::cout << std::endl;
  }
}


void Assembler::ispisiSadrzajTabeleSekcija(){

  const char separator = ' ';
  const int sirinaKaraktera = 15;

  // Zaglavlje
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Naziv sekcije";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Velicina";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Pocetna adresa";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Redni broj";
  std::cout << std::endl;


  // Sadrzaj
  for(Sekcija iterator : this->dohvatiTabeluSekcija()){
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiNazivSekcije();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiVelicinuSekcije();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiPocetnuAdresuSekcije();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiRedniBrojSekcije();
    std::cout << std::endl;
  }

}


bool Assembler::postojiSimbol(std::string simbol){

  for(ZapisTS iterator : this->dohvatiTabeluSimbola()){  
    std::string simbolTS = iterator.dohvatiSimbol();

    if(simbolTS == simbol){
      return true;   // Simbol vec postoji u TS - Greska! Visestruka definicija
    }
  }

  // Ako nismo vratili true unutar for-a onda taj simbol ne postoji i vracamo false
  return false;
}

// ---------- Funkcije za tabelu simbola END ----------



// ---------- Konstruktori START ----------

Assembler::Assembler(){
  // Dodamo undefined sekciju u tabelu simbola
  ZapisTS undefinedSekcija("UND", 0, 0, Povezivanje::LOKALNO, 0, externDir::NIJE_EXTERN);
  this->dodajZapisUTabeluSimbola(undefinedSekcija);

  locationCounter = 0;
  trenutnaSekcija = -1;
}

// ---------- Konstruktori END ----------



// ---------- Geteri i seteri START ----------

std::list<Sekcija>& Assembler::dohvatiTabeluSekcija(){
  return tabelaSekcija;
}


void Assembler::postaviTabeluSekcija(std::list<Sekcija> ts){
  tabelaSekcija = ts;
}


ASMProlaz Assembler::dohvatiTekuciProlaz(){
  return trenutniProlaz;
}


void Assembler::postaviProlazAsemblera(ASMProlaz prolaz){
  trenutniProlaz = prolaz;
}


int Assembler::dohvatiLocationCounter(){
  return locationCounter;
}


void Assembler::resetujLocationCounter(){
  locationCounter = 0;
}


void Assembler::uvecajLocationCounter(int inkrement){
  locationCounter += inkrement;
}


int Assembler::dohvatiTrenutnuSekciju(){
  return trenutnaSekcija;
}


void Assembler::postaviTrenutnuSekciju(int sekcija){
  trenutnaSekcija = sekcija;
}


std::string Assembler::dohvatiNazivTrenutneSekcije(){
  int redniBrSekcije = this->dohvatiTrenutnuSekciju();

  for(ZapisTS iterator : this->dohvatiTabeluSimbola()){
    
    if(iterator.dohvatiSekciju() == redniBrSekcije && iterator.dohvatiRedniBrojUTS() == redniBrSekcije){
      return iterator.dohvatiSimbol();
    }
  }
}

void Assembler::postaviVelicinuSekcijeUTabeli(std::string nazivSekcije, int velicina){

  this->dohvatiRedIzTabeleSekcija(nazivSekcije).postaviVelicinuSekcije(velicina);

}


Sekcija& Assembler::dohvatiRedIzTabeleSekcija(std::string nazivSekcije){

  for(Sekcija& iterator : this->dohvatiTabeluSekcija()){
    if(iterator.dohvatiNazivSekcije() == nazivSekcije){
      return iterator;
    }
  }
}


ZapisTS Assembler::dohvatiRedIzTabeleSimbola(std::string simbol){

  for(ZapisTS iterator : this->dohvatiTabeluSimbola()){
    if(iterator.dohvatiSimbol() == simbol){
      return iterator;
    }
  }
}


int Assembler::dohvatiVrednostSimbolaIzTS(std::string simbol){

  for(ZapisTS iterator : tabelaSimbola){
    if(iterator.dohvatiSimbol() == simbol){
      return iterator.dohvatiOffset();
    }
  }
}


std::string Assembler::dohvatiNazivSekcijePremaRednomBrojuUTS(int redniBrojSekcijeUTS){

  for(ZapisTS iterator : tabelaSimbola){
    if(iterator.dohvatiRedniBrojUTS() == redniBrojSekcijeUTS){
      return iterator.dohvatiSimbol();
    }
  }
}


uint32_t Assembler::dohvatiPocetnuAdresuSekcijePremaNazivuIzTabeleSekcija(std::string nazivSekcije){

  for(Sekcija iterator : tabelaSekcija){
    if(iterator.dohvatiNazivSekcije() == nazivSekcije){
      return iterator.dohvatiPocetnuAdresuSekcije();
    }
  }
}

// ---------- Geteri i seteri END ----------


// ---------- Pomocne funkcije START ----------


std::string Assembler::konvertujRegistarUHexa(std::string reg){

  // Vracamo hardkodovanu vrednost ukoliko je unesen registar SP ili PC
  if(reg == "sp"){
    return "E";
  }

  if(reg == "pc"){
    return "F";
  }


  // Ova funkcija radi: R12 -> 12 -> C
  reg = reg.substr(1);        // Ukloni R iz stringa npr. 'R6' tako da ostane samo broj registra
  int broj = std::stoi(reg);  // Konvertujemo string u decimalni broj
  
  std::stringstream sTok;
  sTok << std::hex << broj;   // Konvertujemo broj u hexa zapis

  std::string hexaBroj = sTok.str();
  return hexaBroj;
}


std::string Assembler::konvertuj4BitLiteralUHexa(uint8_t literal4bit){

  // Nisam siguran, ali mislim da sam ovo radio zato sto mi npr. 1111 nije tumacio koa F, nego kao odvojene cetiri hexa cifre 1
  
  std::stringstream sTok;
  sTok << std::hex << static_cast<int>(literal4bit);
  std::string hexaBroj = sTok.str();

  std::cout << " PRETVORENA VREDNOST JE " << std::hex << hexaBroj << std::endl;
  return hexaBroj;
}


uint8_t Assembler::konvertujHexaZapisUInt8(std::string hexa){

  // Funkcija za prosledjenu string hexa vrednost (npr. 0xCC) 
  // Ova funkcija ce od toga da napravi uint8_t vrednost 0xAB

  int decimalniBroj = std::stoi(hexa, nullptr, 16);     // Da bih koristio osnovu 16 tj. hexa
  uint8_t hexaBroj = static_cast<int>(decimalniBroj);

  // Ovo bi za hexa vrednost C ispisalo 221
  //std::cout << "KONVERTOVANO JE OVAKO " << decimalniBroj << std::endl;
  // Ne mozes direktno da ispises hexa vrednost, moras da koristis std::hex ali to moze samo kroz ispis (npr. std::cout)

  return hexaBroj;
}


void Assembler::uvecajVelicinuSekcijaZaBazenLiterala(){

  // Posto bazen literala takodje pripada sadrzaju sekcije, potrebno je da se velicina sekcije uveca za velicinu bazena literala

  for(Sekcija& iterator : tabelaSekcija){
    int velicinaBazena = iterator.dohvatiBazenLiterala().size();
    iterator.postaviVelicinuSekcije(iterator.dohvatiVelicinuSekcije() + velicinaBazena);
  }
}


// ---------- Pomocne funkcije END ----------