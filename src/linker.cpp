
#include "../inc/linker.h"
#include <sstream>                // Da bih koristio stringstream


int Linker::procitajFajl(std::string fajl){

  std::ifstream ulazniFajl;
  
  
  ulazniFajl.open(fajl);

  if(ulazniFajl.is_open()){


    this->inkrementirajRedniBrojObradjivanogFajla();   // Inkrementiramo za svaki fajl koji obradjujemo

    std::cout << "Trenutno se obradjuje " << this->dohvatiRedniBrObradjivanogFajla() << ". fajl po redu navodjenja!" << std::endl << std::endl;


    std::string jednaLinija;                    // Promenljiva u kojoj cuvamo procitanu liniju
    
    // Pomocne mape, za ispravku vrednosti sekcije tabele simbola
    std::unordered_map<int, std::string> mapaStariRedniBrojSekcije;     // Da bih ispravno promenio vrednost kolone Sekcija simbola - ovde cuvam vrednost sekcije koju je taj simbol imao u svom fajlu; <redniBrojSekcije, nazivSekcije>
    std::unordered_map<std::string, int> mapaNoviRedniBrojSekcije;      // Da bih ispravno promenio vrednost kolone Sekcija simbola - ovde cuvam vrednost sekcije koju taj simbol ima u linkerovoj tabeli simbola; <nazivSekcije, redniBrojULinkerovojTS>
    std::unordered_map<std::string, bool> ubacenSimbolIzOvogFajla;      // Da bih znao koji simbol je ubacen tokom obrade ovog fajla. Ne zelim da azuriram simbole koje sam obradio u prethodnim fajlovima. Dakle, obradjujem samo i-ti fajl, a ne i simbole obradjene pre i-tog fajla

    bool flag_obradiMapeZaTS = true;    //    Pogledaj deo jednaLinija.find("Sadrzaj sekcije") - tamo sam objasnio cemu mi ovo sluzi


    std::unordered_map<std::string, int> mapaStarihVelicinaSekcija;     // Ovo koristim kako bi azurirao offset vrednosti za simbole i relokacione zapise

    

    while(std::getline(ulazniFajl, jednaLinija)){


      // Da li smo dosli do kraja
      if(jednaLinija == "FajlZavrsen"){
        std::cout << "Naisao na FajlZavrsen - Zavrseno citanje ulaznog fajla" << std::endl << std::endl << std::endl;

        std::cout << "STARE VREDNOSTI SEKCIJA IZ FAJL ZAVRSEN" << std::endl;
        for(std::pair<int, std::string> iterator : mapaStariRedniBrojSekcije){
          std::cout << iterator.first << " -> " << iterator.second << std::endl;
        }

        std::cout << "NOVE VREDNOSTI SEKCIJA IZ FAJL ZAVRSEN" << std::endl;
        for(std::pair<std::string, int> iterator : mapaNoviRedniBrojSekcije){
          std::cout << iterator.first << " -> " << iterator.second << std::endl;
        }

        mapaStariRedniBrojSekcije.clear();    // Brisemo sadrzaj starih vrednosti sekcija jer smo zavrsili rad sa ovim fajlom
        flag_obradiMapeZaTS = true;           // Hocemo za naredni fajl da mogu da se obrade
        this->resetujPomocZaOdredjivanjePocetneAdreseSekcije();   // Resetuj kako bi ti pracenje zauzeca memorije i u novom fajlu krenuo od pocetka (jer se linkerova tabela sekcija uvek obradjuje od pocetka)
        return 0;
      }


      // Da li obradjujemo tabelu sekcija
      if(jednaLinija == "Tabela sekcija"){
        std::cout << "Obradjujemo tabelu sekcija" << std::endl;

        std::getline(ulazniFajl, jednaLinija);            // Dohvati naredni red - Ovo treba da dohvati nazive kolona

        while(std::getline(ulazniFajl, jednaLinija)){     // Dohvatamo sadrzaje tabele sekcija

          if(jednaLinija == ""){
            break;                                        // Napustamo while kada dodjemo do praznog reda
          }
          
          else{
            // Obradi tabelu sekcija
            std::stringstream stringTok(jednaLinija);   // Dohvata jedan po jedan string iz linije, razdvojen belinama
            std::string chunk;                          // Promenljiva u kojoj cuvamo dohvaceni string


            std::list<Sekcija> tabSekcija = this->dohvatiTabeluSekcijaLinkera();
            
            int counter = 0;                  // Da bismo znali koji deo fajla se trenutno obradjuje
            std::string nazivSekcijeIzFajla;
            int velicina;
            uint32_t pocetnaAdr;
            int redniBrojUTabSekcija;

            while(stringTok >> chunk){
              
              if(counter == 0){
                nazivSekcijeIzFajla = chunk;
                counter++;
              }

              else if(counter == 1){
                velicina = std::stoi(chunk);
                counter++;
              }

              else if(counter == 2){
                pocetnaAdr = static_cast<uint32_t>(std::stoul(chunk));
                
                // Ovo nije dobro zato sto radi signed konverziju, a adresa je unsigned int
                
                // pocetnaAdr = std::stoi(chunk);
                counter++;
              }

              else if(counter == 3){
                redniBrojUTabSekcija = std::stoi(chunk);    // Koristim zbog konstruktora, ali necu koristiti ovu vrednost nego cu sam odrediti na osnovu poslednje unete u tabeli sekcija linkera

                Sekcija novaSekcija(nazivSekcijeIzFajla, velicina, pocetnaAdr, redniBrojUTabSekcija);
                this->dodajSekcijuUTabeluSekcija(novaSekcija, mapaStarihVelicinaSekcija);
                
                counter = 0;    // Da bi moglo da se krene opet sa unosom - pocetak state masine
              }
              
            }
          }                  
        }
      }


      // Da li obradjujemo tabelu simbola
      if(jednaLinija == "Tabela simbola"){


        // Pre nego sto krenemo obradu tabele simbola linkera - potrebno je da azuriramo tabelu sekcija linkera
        // Konkretno, potrebno je za sve sekcije postaviti njihove pocetne adrese
        for(Sekcija& iterator : tabelaSekcijaLinkera){

          // Vrednost prethodne ukupne zauzete memorije postavi kao pocetnu adresu ove sekcije
          iterator.postaviPocetnuAdresuSekcije(this->dohvatiPomocZaOdredjivanjePocetneAdreseSekcije());

          // Uvecaj tu pomocnu promenljivu za velicinu ove obradjene sekcije
          this->dodajNaVrednostZaPomocZaOdredjivanjePocetneAdreseSekcije(iterator.dohvatiVelicinuSekcije());

        }


        
        std::cout << "Obradjujemo tabelu simbola" << std::endl;

        std::getline(ulazniFajl, jednaLinija);            // Dohvati naredni red - Ovo treba da dohvati nazive kolona
        std::getline(ulazniFajl, jednaLinija);            // Dohvati naredni red - Ovo treba da dohvati red za UND sekciju - ona ne treba da postoji u linkerovoj tabeli simbola

        while(std::getline(ulazniFajl, jednaLinija)){     // Dohvatamo sadrzaje tabele simbola

          if(jednaLinija == ""){
            break;                                        // Napustamo while kada dodjemo do praznog reda
          }
          
          else{
            // Obradi tabelu simbola
            std::stringstream stringTok(jednaLinija);   // Dohvata jedan po jedan string iz linije, razdvojen belinama
            std::string chunk;                          
            

            std::list<ZapisTS> tabSimbola = this->dohvatiTabeluSimbolaLinkera();

            std::string simbol;       // naziv simbola
            int sekcija;              // redni broj na kom je smestena ta sekcija u tabeli simbola LINKERA
            int offset;               // pomeraj do tog simbola u linkerovoj tabeli simbola
            int nacinPovezivanja;
            int redniBroj;
            bool isExtern;

            int counter = 0;          // Da bismo znali koji deo fajla se trenutno obradjuje


            while(stringTok >> chunk){

              if(counter == 0){
                simbol = chunk;
                counter++;
              }

              else if(counter == 1){
                sekcija = std::stoi(chunk);
                counter++;
              }

              else if(counter == 2){
                offset = std::stoi(chunk);
                counter++;
              }

              else if(counter == 3){
                nacinPovezivanja = std::stoi(chunk);
                counter++;
              }

              else if(counter == 4){
                redniBroj = std::stoi(chunk);
                counter++;
              }

              else if(counter == 5){
                isExtern = std::stoi(chunk);

                // Ovde treba izvrsiti obradu i dodati to u tabelu simbola

                // Provera da li je u pitanju lokalni simbol u tom fajlu - sa takvim simbolima linker ne radi, pa mu samim tim nije ni potreban u linkerovoj tabeli simbola
                if(nacinPovezivanja == Povezivanje::LOKALNO && !this->daLiJeSimboLSekcija(redniBroj, sekcija)){
                  // Povezivanje je lokalno i NIJE sekcija -> Jeste lokalan simbol - ne dodajemo ga u tabelu simbola
                }
                
                else{
                  // Sve potrebne provere da li simbol zapravo treba dodati uradicu u ovoj metodi
                  ZapisTS novZapisUTS(simbol, sekcija, offset, nacinPovezivanja, redniBroj, isExtern);
                  if(this->dodajRedUTabeluSimbola(novZapisUTS, mapaStariRedniBrojSekcije, mapaNoviRedniBrojSekcije, ubacenSimbolIzOvogFajla, mapaStarihVelicinaSekcija) == LinkerInfo::VISESTRUKA_DEFINICIJA_GLOBALNOG_SIMBOLA_U_LINKERU){
                    return LinkerInfo::VISESTRUKA_DEFINICIJA_GLOBALNOG_SIMBOLA_U_LINKERU;
                  }
                }
                

                counter = 0;
              }
            }
          }                  
        }        
      }

      if(jednaLinija.find("Sadrzaj sekcije") != std::string::npos){

        // Pre nego sto predjem na obradu sadrzaja sekcije, hocu da zavrsim obradu tabele simbola
        // Treba izvrsiti prepravku vrednosti za sekciju i offset simbola
        if(flag_obradiMapeZaTS){
          // Ovo se obradjuje jednom po fajlu zato sto ja obradjujem finalan sadrzaj tabele simbola koja je dotad obradjena
          // Posto u fajlu postoji N pojavljivanja "sadrzaj sekcije X", ne zelim da za svako to pojavljivanje radim izmenu tabele simbola linkera, vec samo u prvom tom pojavljivanju
          // Jer je u prvom pojavljivanju tabela simbola linkera u potpunosti obradjena za taj fajl i meni je dovoljno da je samo u tom prvom pojavljivanju obradim tj. uradim azuriranja

          std::cout << "STARE VREDNOSTI SEKCIJA" << std::endl;
          for(std::pair<int, std::string> iterator : mapaStariRedniBrojSekcije){
            std::cout << iterator.first << " -> " << iterator.second << std::endl;
          }

          std::cout << "NOVE VREDNOSTI SEKCIJA" << std::endl;
          for(std::pair<std::string, int> iterator : mapaNoviRedniBrojSekcije){
            std::cout << iterator.first << " -> " << iterator.second << std::endl;
          }

          // Menjam vrednosti za sekciju za celu tabelu simbola
          // Offset ce morati da se menja samo za one simbole iz tog istog fajla

          for(ZapisTS& iterator : tabelaSimbolaLinkera){
            
            std::string obradjivaniSimbol = iterator.dohvatiSimbol();

            std::cout << std::endl << "Trenutno se obradjuje simbol: " << obradjivaniSimbol << std::endl;
            
            if(this->kljucPostojiUMapi(ubacenSimbolIzOvogFajla, obradjivaniSimbol)){
              // Ako smo ovaj simbol obradili u ovom fajlu --- njega prepravljamo (ne prepravljamo ranije obradjene simbole)

              
              int trenutniRbrSekcije = iterator.dohvatiSekciju();
              std::cout << "TRENUTNA SEKCIJA " << trenutniRbrSekcije << std::endl;

              if(trenutniRbrSekcije == 0){
                // Ostavljamo da bude 0 - to je UND sekcija; Provericemo na kraju da li je ostao neki nerazresen simbol - ako jeste to je GRESKA
              }
              else{

                // Azuriranje sekcije
                std::string nazivSekcije = mapaStariRedniBrojSekcije.at(trenutniRbrSekcije);
                std::cout << "TRENUTNA SEKCIJA NAZIV " << nazivSekcije << std::endl;
                int noviRbrSekcije = mapaNoviRedniBrojSekcije.at(nazivSekcije);
                std::cout << "SEKCIJA NOVI REDNI BROJ " << noviRbrSekcije << std::endl;
                
                iterator.postaviSekciju(noviRbrSekcije);

                std::cout << "Za simbol " << obradjivaniSimbol << " azuriran redni br sekcije je " << iterator.dohvatiSekciju() << std::endl;

                // Azuriranje offset-a simbola u tabeli simbola linkera
                std::cout << "MAPA STARIH VELICINA SEKCIJA: " << std::endl;
                for(std::pair<std::string, int> iterator : mapaStarihVelicinaSekcija){
                  std::cout << "size(" << iterator.first << ") -> " << iterator.second << std::endl;
                }

                std::cout << "Simbol: " << obradjivaniSimbol << std::endl;
                std::cout << "Stari offset: " << iterator.dohvatiOffset() << std::endl;
                iterator.postaviOffset(iterator.dohvatiOffset() + mapaStarihVelicinaSekcija.at(nazivSekcije));
                std::cout << "Novi offset: " << iterator.dohvatiOffset() << std::endl << std::endl;
              }
              
            }
 
          }

          flag_obradiMapeZaTS = false;
        }
        




        std::string nazivSekcije = jednaLinija.substr(16);
        std::cout << "Obrada sekcije " << nazivSekcije << std::endl;

        while(std::getline(ulazniFajl, jednaLinija)){     // Dohvatamo sadrzaje sekcije

          if(jednaLinija == ""){
            break;                                        // Napustamo while kada dodjemo do praznog reda
          }
          else{
            // Obradi sadrzaj sekcije
            // Prvi chunk odmah dohvata bajt koji predstavlja sadrzaj
            std::stringstream stringTok(jednaLinija);   // Dohvata jedan po jedan string iz linije, razdvojen belinama
            std::string chunk;


            // Dohvati red iz tabele sekcija koji odgovara nazivu ove sekcije, kako bi u nju smestio sadrzaj
            Sekcija& obradjivanaSekcija = this->dohvatiRedIzTabeleSekcija(nazivSekcije);

            uint8_t bajtSadrzaja;
            while(stringTok >> chunk){
              bajtSadrzaja = this->konvertujStringBajtUHexaBroj(chunk);
              obradjivanaSekcija.upisiUSadrzaj(bajtSadrzaja);
            }
          }  
        }
      }


      if(jednaLinija.find("Bazen literala sekcije") != std::string::npos){

        std::string nazivSekcije = jednaLinija.substr(23);
        std::cout << "Obrada bazena literala sekcije " << nazivSekcije << std::endl;

        while(std::getline(ulazniFajl, jednaLinija)){     // Dohvatamo bazen literala

          if(jednaLinija == ""){
            break;                                        // Napustamo while kada dodjemo do praznog reda
          }
          else{
            // Obradi bazen literala
            // Prvi chunk odmah dohvata bajt koji predstavlja sadrzaj
            std::stringstream stringTok(jednaLinija);   // Dohvata jedan po jedan string iz linije, razdvojen belinama
            std::string chunk;   

            // Bazen literala je takodje sadrzaj, i njega dodajemo u sadrzaj

            // Dohvati red iz tabele sekcija koji odgovara nazivu ove sekcije, kako bi u nju smestio sadrzaj
            
            Sekcija& obradjivanaSekcija = this->dohvatiRedIzTabeleSekcija(nazivSekcije);
            
            uint8_t bajtSadrzaja;
            while(stringTok >> chunk){
              bajtSadrzaja = this->konvertujStringBajtUHexaBroj(chunk);
              obradjivanaSekcija.upisiUSadrzaj(bajtSadrzaja);
            }
          }  
        }

      }


      if(jednaLinija.find("Relokacioni zapisi sekcije") != std::string::npos){

        std::string nazivSekcije = jednaLinija.substr(27);
        std::cout << "Obrada relokacionih zapisa sekcije " << nazivSekcije << std::endl;

        std::getline(ulazniFajl, jednaLinija);            // Dohvati naredni red - Ovo treba da dohvati nazive kolona

        while(std::getline(ulazniFajl, jednaLinija)){     // Dohvatamo sadrzaje relokacionih zapisa

          if(jednaLinija == ""){
            break;                                        // Napustamo while kada dodjemo do praznog reda
          }
          else{
            // Obradi sadrzaj relokacionih zapisa
            // Prvi chunk odmah dohvata bajt koji predstavlja sadrzaj
            std::stringstream stringTok(jednaLinija);   // Dohvata jedan po jedan string iz linije, razdvojen belinama
            std::string chunk;   


            // Dohvati red iz tabele sekcija koji odgovara nazivu ove sekcije, kako bi u nju smestio relokacione zapise
            
            Sekcija& obradjivanaSekcija = this->dohvatiRedIzTabeleSekcija(nazivSekcije);

            uint32_t pocetnaAdr;
            int offsetPrepravke;
            std::string tipRelokacije;
            std::string simbol;
            int addend;

            int counter = 0;
            
            while(stringTok >> chunk){

              if(counter == 0){
                pocetnaAdr = static_cast<uint32_t>(std::stoul(chunk));
                
                // Adresa je unsigned, pa ne mozes da koristis ovo sto si ranije
                // pocetnaAdr = std::stoi(chunk);
                

                counter++;
              }

              else if(counter == 1){
                std::istringstream(chunk) >> std::hex >> offsetPrepravke;
                //offsetPrepravke = std::stoi(chunk);   // Ovo ne moze jer se ovde offset sacuvao heksadecimalno

                //offsetPrepravke = this->konvertujStringBajtUHexaBroj(chunk);
                int decimalnaVrAdreseSekcije = mapaStarihVelicinaSekcija.at(nazivSekcije);
                offsetPrepravke = offsetPrepravke + decimalnaVrAdreseSekcije;

                counter++;
              }

              else if(counter == 2){
                tipRelokacije = chunk;
                counter++;
              }

              else if(counter == 3){
                simbol = chunk;
                counter++;
              }

              else if(counter == 4){
                //addend = std::stoi(chunk);          // Ovo ne moze jer se ovde offset sacuvao heksadecimalno
                addend = this->konvertujStringBajtUHexaBroj(chunk);

                // Ovde sad ubacujemo relokacione zapise tamo gde treba
                RelokZapis novRelokZapis(pocetnaAdr, offsetPrepravke, tipRelokacije, simbol, addend);
                obradjivanaSekcija.dodajRelokacioniZapis(novRelokZapis);

                counter = 0;
              }
              
            }
          }  
        }
      }
      
        
    }


      ulazniFajl.close();
  }

}


void Linker::formirajMemorijskiSadrzaj(){

  // Prolazimo kroz tabelu sekcija i sadrzaj svake sekcije ubacujemo u vektor

  for(Sekcija iteratorSekcija : tabelaSekcijaLinkera){

    std::vector<uint8_t> sadrzajSekcije = iteratorSekcija.dohvatiSadrzaj();

    for(uint8_t sadrzaj : sadrzajSekcije){
      this->dodajSadrzajUmemoriju(sadrzaj);
    }
  }
}


void Linker::formirajMemorijskiSadrzajMapa(){

  // Prolazimo kroz tabelu sekcija i sadrzaj svake sekcije ubacujemo u mapu

  for(Sekcija iteratorSekcija : tabelaSekcijaLinkera){

    uint32_t pocetnaAdresa = iteratorSekcija.dohvatiPocetnuAdresuSekcije();
    std::vector<uint8_t> sadrzajSekcije = iteratorSekcija.dohvatiSadrzaj();

    for(int i = 0; i < sadrzajSekcije.size(); i++){
      memorijaMapa.insert({pocetnaAdresa + i, sadrzajSekcije[i]});    // Dodajemo u memoriju
    }
  }
}


void Linker::dodajSadrzajUmemoriju(uint8_t bajtSadrzaja){
  memorija.push_back(bajtSadrzaja);
}


void Linker::ispisiMemorijskiSadrzaj(){

  int novRedCount = 0;

  for(uint8_t iterator : memorija){
    
    if(novRedCount % 8 == 0){
      std::cout << std::endl;
    }

    if (iterator < 0x10) {
        std::cout << "0" << std::hex << static_cast<int>(iterator) << " ";
    }
    else {
        std::cout << std::hex << static_cast<int>(iterator) << " ";
    }

    novRedCount++;
  }
}


void Linker::ispisiMemorijskiSadrzajMapa(){

  /*
  std::cout <<"Parovi: adresa -> sadrzaj:" << std::endl;
  for(std::pair<uint32_t, uint8_t> iterator : memorijaMapa){
      std::cout << std::hex << iterator.first << " -> ";

      if (iterator.second < 0x10) {
        std::cout << "0" << std::hex << static_cast<int>(iterator.second) << " ";
      }
      else {
          std::cout << std::hex << static_cast<int>(iterator.second) << " ";
      }
      
      std::cout << std::endl;
  }
  */



  int novRedCount = 0;

  for(std::pair<uint32_t, uint8_t> iterator : memorijaMapa){
      
      if(novRedCount % 8 == 0){
        std::cout << std::endl;
      }

      if (iterator.second < 0x10) {
        std::cout << "0" << std::hex << static_cast<int>(iterator.second) << " ";
      } else {
          std::cout << std::hex << static_cast<int>(iterator.second) << " ";
      }

      novRedCount++;
  }
}


void Linker::popraviPocetneAdreseURelokacionimZapisima(){

  // Za svaku sekciju dohvatimo njene relokacione zapise i u polje koje predstavlja pocetak te sekcije u relokacionom zapisu
  // postavimo vrednost pocetne adrese koja je sacuvana u linkerovoj tabeli simbola

  for(Sekcija& iteratorSekcije : tabelaSekcijaLinkera){

    uint32_t pocetnaAdresaSekcije = iteratorSekcije.dohvatiPocetnuAdresuSekcije();

    for(RelokZapis& iteratorRZ : iteratorSekcije.dohvatiTabeluRelokZapisaPoReferenci()){

      iteratorRZ.postaviPocetnuAdresuSekcije(pocetnaAdresaSekcije);
    }
    
  }
}


bool Linker::proveriPostojanjeSimbolaSaSekcijomUND(){

  bool postojeNerazreseniSimboli = false;

  for(ZapisTS iterator : tabelaSimbolaLinkera){

    if(iterator.dohvatiSekciju() == 0){
      std::cout << "GRESKA! Simbol " << iterator.dohvatiSimbol() << " nije definisan - i dalje u sekciji UNDEFINED!" << std::endl;
      postojeNerazreseniSimboli = true;
    }
  }

  return postojeNerazreseniSimboli;
}


void Linker::prepraviMemorijskiSadrzaj(){

  // Prolazim kroz sve sekcije iz tabele sekcija linkera - zato sto svaka sekcija ima svoje relokacione zapise koje treba iskoristiti za prepravku
  // Za svaki relokacioni zapis menjamo deo memorije
  // Posto sam simbole stavljao u bazen literala, ja cu u relok. zapisima menjati konacnu vrednost tog simbola u bazenu (tj. u memoriji) - zato koristim apsolutno adresiranje
  for(Sekcija iteratorSekcija : tabelaSekcijaLinkera){

    std::list<RelokZapis> relokacijeOveSekcije = iteratorSekcija.dohvatiTabeluRelokZapisa();


    for(RelokZapis iteratorRZ : relokacijeOveSekcije){

      // Prepravi memorijski sadrzaj
      
      // Radis apsolutno adresiranje - u memoriju se upisuje vrednost Simbol + Addend
      if(iteratorRZ.dohvatiTipRelokacije() == "X86_64_32"){

        // Pokupi podatke iz relokacionog zapisa
        std::string simbol = iteratorRZ.dohvatiSimbol();
        uint32_t pocetnaAdresaSekcijePrepravke = iteratorRZ.dohvatiPocetnuAdresuSekcije();   // hexa vrednost
        int pomerajDoMestaPrepravke = iteratorRZ.dohvatiOffset();                       // hexa vrednost
        int addend = iteratorRZ.dohvatiAddend();                                        // hexa vrednost


        // Pokupi podatke o simbolu
        ZapisTS redIzTabeleSimbolaZaSimbol = this->dohvatiZapisIzTabeleSimbola(simbol);
        int redniBrojSekcijeSimbola = redIzTabeleSimbolaZaSimbol.dohvatiSekciju();      // hexa vrednost
        int offsetDoSimbolaUSekciji = redIzTabeleSimbolaZaSimbol.dohvatiOffset();       // hexa vrednost


        // Pokupi podatke o sekciji
        ZapisTS redIzTabeleSimbolaZaSekciju = this->dohvatiRedIzTabeleSimbolaZaDatRedniBroj(redniBrojSekcijeSimbola);
        std::string nazivSekcije = redIzTabeleSimbolaZaSekciju.dohvatiSimbol();
        Sekcija redIzTabeleSekcija = this->dohvatiRedIzTabeleSekcija(nazivSekcije);
        uint32_t pocetnaAdresaSekcijeSimbola = redIzTabeleSekcija.dohvatiPocetnuAdresuSekcije();   // hexa vrednost


        // Formiraj vrednost simbola koju ces upisati u memoriju
        // Odmah sam i razdvojio na pojedinacne bajtove
        uint32_t vrednostSimbola = pocetnaAdresaSekcijeSimbola + offsetDoSimbolaUSekciji + addend;
        uint8_t vrednost31_24 = (vrednostSimbola & 0xFF000000) >> 24;
        uint8_t vrednost23_16 = (vrednostSimbola & 0x00FF0000) >> 16;
        uint8_t vrednost15_8  = (vrednostSimbola & 0x0000FF00) >> 8;
        uint8_t vrednost7_0   = (vrednostSimbola & 0x000000FF);


        // Formiranje adrese od koje se radi prepravka
        uint32_t adresaPrepravke = pocetnaAdresaSekcijePrepravke + pomerajDoMestaPrepravke;


        // Izmeni sadrzaj memorije
        this->izvrsiPrepravku(adresaPrepravke, vrednost31_24, vrednost23_16, vrednost15_8, vrednost7_0);



        std::cout << "Simbol:" << simbol << std::endl;
        std::cout << "Relok. zapis: pocetna adr. sekcije prepravke: " << pocetnaAdresaSekcijePrepravke << std::endl;
        std::cout << "Relok. zapis: pomeraj do mesta prepravke: " << pomerajDoMestaPrepravke << std::endl;
        std::cout << "Relok. zapis: addend: " << addend << std::endl;

        std::cout << "Simbol je originalno u sekciji sa rednim brojem: " << redniBrojSekcijeSimbola << std::endl;
        std::cout << "Simbol je originalno u sekciji na offsetu: " << offsetDoSimbolaUSekciji << std::endl;
        std::cout << "Simbol je originalno u sekciji: " << nazivSekcije << std::endl;

        std::cout << "Originalna sekcija krece od adrese " << pocetnaAdresaSekcijeSimbola << std::endl;
        std::cout << "Formirana vrednost simbola: " << vrednostSimbola << std::endl;
        
        std::cout << "Finalna adresa pocevsi od koje se radi prepravka je: " << adresaPrepravke;
        
        std::cout << std::endl << std::endl;
        
      }
      
    }
  }
}


void Linker::prepraviMemorijskiSadrzajMapa(){

  // Prolazim kroz sve sekcije iz tabele sekcija linkera - zato sto svaka sekcija ima svoje relokacione zapise koje treba iskoristiti za prepravku
  // Za svaki relokacioni zapis menjamo deo memorije
  // Posto sam simbole stavljao u bazen literala, ja cu u relok. zapisima menjati konacnu vrednost tog simbola u bazenu (tj. u memoriji) - zato koristim apsolutno adresiranje
  for(Sekcija iteratorSekcija : tabelaSekcijaLinkera){

    std::cout << "Trenutno sam u sekciji: " << iteratorSekcija.dohvatiNazivSekcije() << std::endl;
    std::list<RelokZapis> relokacijeOveSekcije = iteratorSekcija.dohvatiTabeluRelokZapisa();

    std::cout << "Dohvatio sam tabelu relok zapisa sekcije " << iteratorSekcija.dohvatiNazivSekcije() << std::endl;

    for(RelokZapis iteratorRZ : relokacijeOveSekcije){

      // Prepravi memorijski sadrzaj
      std::cout << "Obradjujem relok zapis - simbol: " << iteratorRZ.dohvatiSimbol() << std::endl;
      // Radis apsolutno adresiranje - u memoriju se upisuje vrednost Simbol + Addend
      if(iteratorRZ.dohvatiTipRelokacije() == "X86_64_32"){

        // Pokupi podatke iz relokacionog zapisa
        std::string simbol = iteratorRZ.dohvatiSimbol();
        uint32_t pocetnaAdresaSekcijePrepravke = iteratorRZ.dohvatiPocetnuAdresuSekcije();   // hexa vrednost
        int pomerajDoMestaPrepravke = iteratorRZ.dohvatiOffset();                       // hexa vrednost
        int addend = iteratorRZ.dohvatiAddend();                                        // hexa vrednost


        // Pokupi podatke o simbolu
        ZapisTS redIzTabeleSimbolaZaSimbol = this->dohvatiZapisIzTabeleSimbola(simbol);
        int redniBrojSekcijeSimbola = redIzTabeleSimbolaZaSimbol.dohvatiSekciju();      // hexa vrednost
        int offsetDoSimbolaUSekciji = redIzTabeleSimbolaZaSimbol.dohvatiOffset();       // hexa vrednost


        // Pokupi podatke o sekciji
        ZapisTS redIzTabeleSimbolaZaSekciju = this->dohvatiRedIzTabeleSimbolaZaDatRedniBroj(redniBrojSekcijeSimbola);
        std::string nazivSekcije = redIzTabeleSimbolaZaSekciju.dohvatiSimbol();
        std::cout << "Naziv sekcije je: " <<nazivSekcije << std::endl;
        Sekcija redIzTabeleSekcija = this->dohvatiRedIzTabeleSekcija(nazivSekcije);
        uint32_t pocetnaAdresaSekcijeSimbola = redIzTabeleSekcija.dohvatiPocetnuAdresuSekcije();   // hexa vrednost


        // Formiraj vrednost simbola koju ces upisati u memoriju
        // Odmah sam i razdvojio na pojedinacne bajtove
        uint32_t vrednostSimbola = pocetnaAdresaSekcijeSimbola + offsetDoSimbolaUSekciji + addend;
        uint8_t vrednost31_24 = (vrednostSimbola & 0xFF000000) >> 24;
        uint8_t vrednost23_16 = (vrednostSimbola & 0x00FF0000) >> 16;
        uint8_t vrednost15_8  = (vrednostSimbola & 0x0000FF00) >> 8;
        uint8_t vrednost7_0   = (vrednostSimbola & 0x000000FF);


        // Formiranje adrese od koje se radi prepravka
        uint32_t adresaPrepravke = pocetnaAdresaSekcijePrepravke + pomerajDoMestaPrepravke;


        // Izmeni sadrzaj memorije
        this->izvrsiPrepravkuUMapi(adresaPrepravke, vrednost31_24, vrednost23_16, vrednost15_8, vrednost7_0);



        std::cout << "Simbol:" << simbol << std::endl;
        std::cout << "Relok. zapis: pocetna adr. sekcije prepravke: " << pocetnaAdresaSekcijePrepravke << std::endl;
        std::cout << "Relok. zapis: pomeraj do mesta prepravke: " << pomerajDoMestaPrepravke << std::endl;
        std::cout << "Relok. zapis: addend: " << addend << std::endl;

        std::cout << "Simbol je originalno u sekciji sa rednim brojem: " << redniBrojSekcijeSimbola << std::endl;
        std::cout << "Simbol je originalno u sekciji na offsetu: " << offsetDoSimbolaUSekciji << std::endl;
        std::cout << "Simbol je originalno u sekciji: " << nazivSekcije << std::endl;

        std::cout << "Originalna sekcija krece od adrese " << pocetnaAdresaSekcijeSimbola << std::endl;
        std::cout << "Formirana vrednost simbola: " << vrednostSimbola << std::endl;
        
        std::cout << "Finalna adresa pocevsi od koje se radi prepravka je: " << adresaPrepravke;
        
        std::cout << std::endl << "------------------------------" << std::endl;
      }
      
    }
  }
}


int Linker::dohvatiRedniBrojSekcijeZaDatiSimbol(std::string simbol){

  for(ZapisTS iterator : tabelaSimbolaLinkera){
    if(iterator.dohvatiSimbol() == simbol){
      return iterator.dohvatiSekciju();
    }
  }
}


ZapisTS Linker::dohvatiRedIzTabeleSimbolaZaDatRedniBroj(int redniBroj){

  for(ZapisTS iterator : tabelaSimbolaLinkera){
    if(iterator.dohvatiRedniBrojUTS() == redniBroj){
      return iterator;
    }
  }
}



int Linker::konvertujHexaBrojUDecimalni(int hexa){

  std::stringstream sTok;
  sTok << std::hex << static_cast<int>(hexa);
  std::string hexaBroj = sTok.str();


  int decimalniBroj = std::stoi(hexaBroj, nullptr, 16);   // hexaBroj string tumaci kao broj sa osnovom 16 (tj. kao heksa broj) i njega konvertuje u decimalni broj sa osnovom 10
  
  return decimalniBroj;
}


void Linker::izvrsiPrepravku(int adresaPocetkaPrepravke, uint8_t bajt31_24, uint8_t bajt23_16, uint8_t bajt15_8, uint8_t bajt7_0){

  // Primer poziva funkcije: linker.izvrsiPrepravku(14, 0x19, 0x24, 0x37, 0x45);
  // Pocevsi od adrese 14, formirace sledeci sadrzaj: 45 37 24 19

  memorija[adresaPocetkaPrepravke] = bajt7_0;
  memorija[adresaPocetkaPrepravke + 1] = bajt15_8;
  memorija[adresaPocetkaPrepravke + 2] = bajt23_16;
  memorija[adresaPocetkaPrepravke + 3] = bajt31_24;

}

void Linker::izvrsiPrepravkuUMapi(uint32_t adresaPocetkaPrepravke, uint8_t bajt31_24, uint8_t bajt23_16, uint8_t bajt15_8, uint8_t bajt7_0){

  memorijaMapa[adresaPocetkaPrepravke] = bajt7_0;
  memorijaMapa[adresaPocetkaPrepravke + 1] = bajt15_8;
  memorijaMapa[adresaPocetkaPrepravke + 2] = bajt23_16;
  memorijaMapa[adresaPocetkaPrepravke + 3] = bajt31_24;
}


// Geteri i seteri

int Linker::dohvatiLocationCounter(){
  return locationCounter;
}


void Linker::uvecajLocationCounter(int inkrement){
  locationCounter += inkrement;
}


std::list<ZapisTS>& Linker::dohvatiTabeluSimbolaLinkera(){
  return tabelaSimbolaLinkera;
}


std::list<Sekcija>& Linker::dohvatiTabeluSekcijaLinkera(){
  return tabelaSekcijaLinkera;
}


void Linker::dodajSekcijuUTabeluSekcija(Sekcija novaSekcija, std::unordered_map<std::string, int>& mapaStarihVelicinaSekcija){

  // Proveravamo da li ova sekcija vec postoji u tabeli sekcija - ako postoji ne dodajemo je, znaci da je vec ubacena jer je postojala u ranije obradjenom fajlu
  // Ovo je moguce zato sto se jedan naziv sekcije moze naci u vise fajlova, a pri linkovanju fajlova potrebno je takve sekcije spojiti
  for(Sekcija& iterator : tabelaSekcijaLinkera){
    if(iterator.dohvatiNazivSekcije() == novaSekcija.dohvatiNazivSekcije()){
      std::cout << "Ova sekcija je vec ranije ubacena u tabelu!" << std::endl;


      // Pre nego sto postavim novu velicinu za ovu sekciju, sacuvam u hes mapi tekucu velicinu
      mapaStarihVelicinaSekcija[iterator.dohvatiNazivSekcije()] = iterator.dohvatiVelicinuSekcije();

      
      // Zatim azuriramo velicinu te sekcije za novu procitanu velicinu
      int novaVelicina = iterator.dohvatiVelicinuSekcije() + novaSekcija.dohvatiVelicinuSekcije();

      iterator.postaviVelicinuSekcije(novaVelicina);

      return;
    }
  }

  // Ako gore nismo izasli iz funkcije to znaci da ova sekcija ne postoji u tabeli - dodajemo je prvi put

  int redniBroj;

  if(tabelaSekcijaLinkera.size() == 0){
    redniBroj = 0;
  }
  else{
    Sekcija poslednjaSekcija = tabelaSekcijaLinkera.back();
    redniBroj = poslednjaSekcija.dohvatiRedniBrojSekcije();     // Dohvati poslednji unet red 
  }
  
  novaSekcija.postaviRedniBrojSekcije(redniBroj + 1);
  tabelaSekcijaLinkera.push_back(novaSekcija);


  // Postavljam velicinu te sekcije u mapi koja cuva staru velicinu
  // Iako velicina sekcije nije 0, pri odredjivanju offseta u PRVOM POJAVLJIVANJU SEKCIJE - tada koristim offset vrednost koja mi je data
  // Tek kada radim spajanje istoimenih sekcija u vise fajlova tada mi treba da koristim njenu velicinu da bi: novOffsetSimbola = velicinaStareSekcije + offsetSimbola
  mapaStarihVelicinaSekcija[novaSekcija.dohvatiNazivSekcije()] = 0;
}


void Linker::ispisiSadrzajTabeleSekcijaLinkera(){

  const char separator = ' ';
  const int sirinaKaraktera = 15;

  // Zaglavlje
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Naziv sekcije";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Velicina";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Pocetna adresa";
  std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << "Redni broj";
  std::cout << std::endl;


  // Sadrzaj
  for(Sekcija iterator : this->dohvatiTabeluSekcijaLinkera()){
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiNazivSekcije();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiVelicinuSekcije();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << std::hex << iterator.dohvatiPocetnuAdresuSekcije();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiRedniBrojSekcije();
    std::cout << std::endl;
  }
}


int Linker::dodajRedUTabeluSimbola(ZapisTS novZapis, std::unordered_map<int, std::string>& mapaStariBrojSekcije, std::unordered_map<std::string, int>& mapaNoviBrojSekcije, std::unordered_map<std::string, bool>& ubacenSimbolIzOvogFajla, std::unordered_map<std::string, int>& mapaStarihVelicinaSekcija){
  
  // Vidi koje sve provere treba da odradis ovde
  // U tabelu simbola ubacuju se samo globalni simboli, extern simboli i nazivi sekcija

  // Pre nego sto se simbol ubaci u tabelu, moramo da proverimo da li takav simbol vec postoji u tabeli simbola linkera
  if(this->postojiSimbolUTabeliSimbola(novZapis.dohvatiSimbol())){

    // Dohvatam taj postojeci zapis kako bih proverio sta treba da uradim
    ZapisTS& postojeciZapis = this->dohvatiZapisIzTabeleSimbola(novZapis.dohvatiSimbol());


    // Ako je taj zapis sekcija onda ga ubacujem u mape
    // Posto ovde obradjujemo kada postoji - znaci da nekad nije postojao, i tada kada nije postojao mi smo mu ubacili ispravnu vrednost za sekciju u tabelu simbola
    if(postojeciZapis.dohvatiSekciju() == postojeciZapis.dohvatiRedniBrojUTS()){

      std::cout << std::endl << "SEKCIJA VEC POSTOJI U TS: " << novZapis.dohvatiSimbol() << std::endl;
      std::cout << "Stari rbr: " << novZapis.dohvatiSekciju() << ", nov rbr: " << postojeciZapis.dohvatiRedniBrojUTS() << std::endl;
      mapaStariBrojSekcije[novZapis.dohvatiSekciju()] = novZapis.dohvatiSimbol();
      mapaNoviBrojSekcije[novZapis.dohvatiSimbol()] = postojeciZapis.dohvatiRedniBrojUTS();
      // NE SMEM DA KORISTIM OVO ISPOD JER NECE DA URADI OVERRIDE PODATAKA
      // mapaStariBrojSekcije.insert({novZapis.dohvatiSekciju(), novZapis.dohvatiSimbol()});
      // mapaNoviBrojSekcije.insert({novZapis.dohvatiSimbol(), postojeciZapis.dohvatiRedniBrojUTS()});
    }



    // Ako je u TS globalni simbol, a pokusavamo da unesemo isti taj globalni simbol
    // To je GRESKA, jer imamo visestruku definiciju globalnog simbola
    if(postojeciZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO && postojeciZapis.daLiJeExtern() == false
          && novZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO &&       novZapis.daLiJeExtern() == false){

          std::cout << "GRESKA! Visestruka definicija globalnog simbola! Linker obustavlja rad!";
          return LinkerInfo::VISESTRUKA_DEFINICIJA_GLOBALNOG_SIMBOLA_U_LINKERU;
    }

    // Ako je u TS globalni simbol, a pokusavamo da unesem extern-ovan taj simbol
    else if(postojeciZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO && postojeciZapis.daLiJeExtern() == false
               && novZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO &&       novZapis.daLiJeExtern() == true){

      // NISTA NE RADIMO OVDE - jer mi vec imamo njegovu definiciju a ovaj extern ce koristiti tu definiciju
      std::cout << "Info: Detektovan extern simbol ("<< postojeciZapis.dohvatiSimbol() << ") za koji vec postoji definicija global u tabeli simbola!" << std::endl;
    }

    // Ako je u TS extern simbol, a pokusavamo da unesemo taj globalni simbol
    // Tada u TS azuriramo podatke (sekciju i offset) - naisli smo na definiciju simbola za koji smo rekli da je extern
    else if(postojeciZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO && postojeciZapis.daLiJeExtern() == true
               && novZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO &&       novZapis.daLiJeExtern() == false){

          std::cout << "Simbol " << novZapis.dohvatiSimbol() << " je azuriran! Izmene:" << std::endl;
          std::cout << "Sekcija: " << postojeciZapis.dohvatiSekciju() << " -> " << novZapis.dohvatiSekciju() << std::endl;
          std::cout << "ILI!!!" << std::endl;
          std::cout << "Sekcija nova: " << mapaNoviBrojSekcije[mapaStariBrojSekcije[novZapis.dohvatiSekciju()]] << std::endl;
          std::cout << "Offset: " << postojeciZapis.dohvatiOffset() << " -> " << novZapis.dohvatiOffset() << std::endl;

          // mapaStariBrojSekcije[novZapis.dohvatiSekciju()] -- ovo dohvati naziv sekcije simbola
          // a onda preko mapaNoviBrojSekcije dohvatim nov redni broj u TS gde je ta sekcija smestena
          postojeciZapis.postaviSekciju(mapaNoviBrojSekcije[mapaStariBrojSekcije[novZapis.dohvatiSekciju()]]);
          // postojeciZapis.postaviOffset(novZapis.dohvatiOffset());  


          // mapaStariBrojSekcije[novZapis.dohvatiSekciju()] -- ovo dohvati naziv sekcije simbola
          int novOffset = novZapis.dohvatiOffset() + mapaStarihVelicinaSekcija.at(mapaStariBrojSekcije[novZapis.dohvatiSekciju()]);
          std::cout << "AZURIRAN NOV OFFSET iznosi: " << novOffset << std::endl;
          postojeciZapis.postaviOffset(novOffset);  

    }

    // Ako je u TS extern simbol, a pokusavamo da unesemo isto taj extern simbol
    else if(postojeciZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO && postojeciZapis.daLiJeExtern() == true
               && novZapis.dohvatiNacinPovezivanja() == Povezivanje::GLOBALNO &&       novZapis.daLiJeExtern() == true){

        // NE RADIMO NISTA - simbol je bio UNDEFINED i jos uvek je UNDEFINED
        std::cout << " Simbol " << postojeciZapis.dohvatiSimbol() << ": bio UND, ostao UND" << std::endl;
    }

  }

  else{
    // Ako ne postoji - onda ga svakako ubacujemo u tabelu simbola

    int redniBroj;
    
    if(tabelaSimbolaLinkera.size() == 0){
      redniBroj = 1;
    }
    else{
      redniBroj = tabelaSimbolaLinkera.back().dohvatiRedniBrojUTS();
      redniBroj++;
    }


    // Proveri da li je simbol zapravo sekcija - ako jeste evidentiraj u mapama ; Ako if prodje onda JESTE sekcija
    if(novZapis.dohvatiSekciju() == novZapis.dohvatiRedniBrojUTS()){
   
      mapaStariBrojSekcije[novZapis.dohvatiSekciju()] = novZapis.dohvatiSimbol();
      mapaNoviBrojSekcije[novZapis.dohvatiSimbol()] = redniBroj;
      // mapaStariBrojSekcije.insert({novZapis.dohvatiSekciju(), novZapis.dohvatiSimbol()});
      // mapaNoviBrojSekcije.insert({novZapis.dohvatiSimbol(), redniBroj});
      
      std::cout << std::endl << "DODAJEMO SEKCIJU " << novZapis.dohvatiSimbol() << std::endl;
      std::cout << "Stari rbr: " << novZapis.dohvatiSekciju() << ", nov rbr: " << redniBroj << std::endl;

    }


    novZapis.postaviRedniBrojUTS(redniBroj);    // Postavi da je redni broj za 1 veci od poslednjeg dodatog (ili 1 ako je prvi zapis u TS)
    tabelaSimbolaLinkera.push_back(novZapis);


    ubacenSimbolIzOvogFajla[novZapis.dohvatiSimbol()] = true;
    //ubacenSimbolIzOvogFajla.insert({novZapis.dohvatiSimbol(), true});
  }
  
  
}


void Linker::ispisiSadrzajTabeleSimbolaLinkera(){

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
  for(ZapisTS iterator : this->dohvatiTabeluSimbolaLinkera()){    
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiSimbol();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiSekciju();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiOffset();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiNacinPovezivanja();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.dohvatiRedniBrojUTS();
    std::cout << std::left << std::setw(sirinaKaraktera) << std::setfill(separator) << iterator.daLiJeExtern();
    std::cout << std::endl;
  }

}


Sekcija& Linker::dohvatiRedIzTabeleSekcija(std::string nazivSekcije){

  for(Sekcija& iterator : tabelaSekcijaLinkera){
    if(iterator.dohvatiNazivSekcije() == nazivSekcije){
      return iterator;
    }
  }
}


uint8_t Linker::konvertujStringBajtUHexaBroj(std::string bajt){

  std::string temp = "0x";
  temp += bajt;

  // Isti princip kao i konverzija kod asemblera
  int decimalniBroj = std::stoi(temp, nullptr, 16);
  uint8_t hexaBroj = static_cast<int>(decimalniBroj);

  return hexaBroj;
}


bool Linker::daLiJeSimboLSekcija(int redniBrojUTS, int redniBrojSekcije){
  // U tabeli simbola, sekcija se uvek moze prepoznati tako sto je redni broj samog simbola u tabeli simbola jednak vrednosti iz kolone sekcija
  if(redniBrojUTS == redniBrojSekcije){
    return true;
  }
  else{
    return false;
  }
}


bool Linker::postojiSimbolUTabeliSimbola(std::string nazivSimbola){

  for(ZapisTS iterator : tabelaSimbolaLinkera){
    if(iterator.dohvatiSimbol() == nazivSimbola){
      return true;
    }
  }

  return false;
}


ZapisTS& Linker::dohvatiZapisIzTabeleSimbola(std::string nazivSimbola){
  
  for(ZapisTS& iterator : tabelaSimbolaLinkera){
    if(iterator.dohvatiSimbol() == nazivSimbola){
      return iterator;
    }
  }
}


bool Linker::kljucPostojiUMapi(std::unordered_map<std::string, bool> mapa, std::string kljuc){

  // find metoda vraca pokazivac na iterator gde se kljuc nalazi u mapi, ukoliko kljuc postoji
  // odnosno vraca mapa.end() ukoliko kljuc ne postoji u mapi

  if(mapa.find(kljuc) != mapa.end()){
    return true;
  }
  else{
    return false;
  }

}


int Linker::dohvatiRedniBrObradjivanogFajla(){
  return obradaFajla;
}

void Linker::inkrementirajRedniBrojObradjivanogFajla(){
  obradaFajla++;
}


uint8_t Linker::konvertujDecimalniUHexaBroj(int broj){

  std::stringstream stringTok;

  stringTok << std::hex << broj;
  std::string hexaBrojString = stringTok.str();
  uint8_t hexaBroj = std::stoul(hexaBrojString, nullptr, 16);

  return hexaBroj;
}


int Linker::dohvatiPomocZaOdredjivanjePocetneAdreseSekcije(){
  return pomocZaOdredjivanjePocetneAdreseSekcije;
}


void Linker::dodajNaVrednostZaPomocZaOdredjivanjePocetneAdreseSekcije(int inkrement){
  pomocZaOdredjivanjePocetneAdreseSekcije += inkrement;
}


void Linker::resetujPomocZaOdredjivanjePocetneAdreseSekcije(){
  pomocZaOdredjivanjePocetneAdreseSekcije = 0;
}


void Linker::postaviPremaUnetimPocetnimAdresama(std::unordered_map<std::string, uint32_t>& mapaSekcijaAdresa, std::map<uint32_t, std::string>& mapaUredjenaAdresaSekcija){

  // Sekcije se smestaju po adresama, rastuce, tako da se prvo pojave one sekcije za koje je postavljena eksplicitna adresa
  // A sekcije za koje nije postavljena adresa pozicioniraju se posle poslednje sekcije cija adresa jeste eksplicitno postavljena
  // Samim tim, trenutni redosled sekcija u tabeli sekcija linkera nece biti adekvatan kasnije, zato ovde formiram novi redosled koji cu dodeliti linkerovoj tabeli sekcija
  std::list<Sekcija> novRasporedTabeleSekcijaLinkera;
  int redniBroj = 0;

  std::cout << "ISPIS UREDJENE MAPE SA POCETNIM ADRESAMA SEKCIJA:" << std::endl;
   for(std::pair<uint32_t, std::string> iterator : mapaUredjenaAdresaSekcija){
      std::cout << std::dec << iterator.first << " -> " << iterator.second;
      std::cout << std::endl;
   }

   std::cout << std::endl << std::endl << std::endl;
  
  for(std::pair<uint32_t, std::string> iterator : mapaUredjenaAdresaSekcija){

    // Dohvati ovu postojecu sekciju iz tabele sekcija
    std::string nazivSekcije = iterator.second;
    std::cout << "DOHVATAM SEKCIJU  " << nazivSekcije << std::endl;
    Sekcija kopijaPostojeceSekcije = this->dohvatiRedIzTabeleSekcija(nazivSekcije);
    std::cout << "OVDE SAM!" << std::endl;
    redniBroj++;  
    kopijaPostojeceSekcije.postaviPocetnuAdresuSekcije(iterator.first);
    kopijaPostojeceSekcije.postaviRedniBrojSekcije(redniBroj);

    novRasporedTabeleSekcijaLinkera.push_back(kopijaPostojeceSekcije);      // Takav red dodaj u novu tabelu sekcija linkera
  }
  
  // Nakon ovoga, u novoj tabeli sekcija linkera nalaze se redom spakovane sve sekcije sa zadatim pocetnim adresama
  // Sad je potrebno da u novu tabelu sekcija linkera smestimo one sekcije bez pocetnih adresa
  for(Sekcija iteratorSekcije : tabelaSekcijaLinkera){

    if(mapaSekcijaAdresa.count( iteratorSekcije.dohvatiNazivSekcije() ) > 0){
      // Count ce da vrati broj pojavljivanja tog key-a - ako je >0 znaci da postoji u hes mapi
      // Ovu sekciju smo vec obradili - jer se ona nalazi u mapi sekcija sa pocetnim adresama
    }
    else{
      // Ova sekcija ne postoji - nju treba da dodamo u novu linkerovu tabelu sekcija
      uint32_t pocetnaAdrPoslednjeDodateSekcije = novRasporedTabeleSekcijaLinkera.back().dohvatiPocetnuAdresuSekcije();
      int velicinaPoslednjeDodateSekcije = novRasporedTabeleSekcijaLinkera.back().dohvatiVelicinuSekcije();
      uint32_t novaAdresaSekcije = pocetnaAdrPoslednjeDodateSekcije + velicinaPoslednjeDodateSekcije;

      Sekcija kopijaPostojeceSekcije = iteratorSekcije;
      redniBroj++;
      kopijaPostojeceSekcije.postaviPocetnuAdresuSekcije(novaAdresaSekcije);
      kopijaPostojeceSekcije.postaviRedniBrojSekcije(redniBroj);

      novRasporedTabeleSekcijaLinkera.push_back(kopijaPostojeceSekcije);
    }
  }


  // Nakon ovoga, u novoj tabeli sekcija linkera nalaze se sve potrebne sekcije, sa azuriranim adresama
  // Sada je potrebno da se prebaci da linker za svoju tabelu sekcija koristi OVU NOVU tabelu sekcija
  this->postaviTabeluSekcijaLinkera(novRasporedTabeleSekcijaLinkera);

}


int Linker::proveriDaLiPostojiPreklapanjeSekcija(){

  // Prolazimo kroz tabelu sekcija linkera i proveravamo za svaku sekciju da li se njena adresa pocetka poklapa sa adresama sekcije prethodnika
  // Dovoljno je proveriti za i-tu sekciju da li se poklapa sa prethodnikom, jer u trenutku smestanja i-te sekcije je (i-1). sekcija poslednja ubacena, tako da i-ta sekcija ne treba da proverava da li preklapa prostor sledbenicke sekcije
  // "Sledbenik" tj. i-ta sekcija u narednoj iteraciji, proverice svog prethodnika, sto je i-ta sekcija iz prethodne iteracije, tako da je sve pokriveno

  // Ovu proveru ne treba raditi za prvu sekciju - prva uvek mora da moze da se smesti, a druga mora da pazi da ne pregazi prvu
  // Sekcija (i) preklapa se sa sekcijom (i-1) ukoliko je pocetna adresa sekcije (i) manja od base+size sekcije (i-1)
  
  int brojac = 0;   // Brojac, da bih preskocio proveru kod prve tj. nulte sekcije
  std::string nazivPrethodneSekcije;    // Cuvam naziv sekcije prethodnika
  uint32_t pocetnaAdresaPrethodneSekcije;    // Cuvam pocetnu adresu sekcije prethodnika
  int velicinaPrethodneSekcije;         // Cuvam velicinu sekcije prethodnika

  for(Sekcija iterator : tabelaSekcijaLinkera){
    if(brojac == 0){
      // Za prvu sekciju ne radimo proveru, vec nju evidentiramo kao prethodnika s kojim ce se porediti naredna sekcija
      nazivPrethodneSekcije = iterator.dohvatiNazivSekcije();
      pocetnaAdresaPrethodneSekcije = iterator.dohvatiPocetnuAdresuSekcije();
      velicinaPrethodneSekcije = iterator.dohvatiVelicinuSekcije();

      brojac++;
    }
    else{
      brojac++;

      // Za sve ostale sekcije - radimo poredjenje
      uint32_t pocetnaAdresaTekuceSekcije = iterator.dohvatiPocetnuAdresuSekcije();

      // Ako je pocetna adresa tekuce sekcije niza od krajnje adrese 
      if(pocetnaAdresaTekuceSekcije < (pocetnaAdresaPrethodneSekcije + velicinaPrethodneSekcije)){
        std::cout << "GRESKA! Sekcija " << iterator.dohvatiNazivSekcije() << " preklapa se sa sekcijom " << nazivPrethodneSekcije << std::endl;
        std::cout << "Sekcija " << nazivPrethodneSekcije << " zauzima adrese [" << pocetnaAdresaPrethodneSekcije << ", " << pocetnaAdresaPrethodneSekcije + velicinaPrethodneSekcije - 1;
        std::cout << "], a sekcija " << iterator.dohvatiNazivSekcije() << " je postavljena da pocne od adrese " << pocetnaAdresaTekuceSekcije << "!" << std::endl;
        return LinkerInfo::PREKLOPLJENE_SEKCIJE;
      }
      else{
        // Nema preklapanja - postavljamo da je ova sekcija prethodnik, kog ce koristiti naredna sekcija za poredjenje
        nazivPrethodneSekcije = iterator.dohvatiNazivSekcije();
        pocetnaAdresaPrethodneSekcije = iterator.dohvatiPocetnuAdresuSekcije();
        velicinaPrethodneSekcije = iterator.dohvatiVelicinuSekcije();
      }
    }

  }

  return LinkerInfo::USPESNO;
}


void Linker::postaviTabeluSekcijaLinkera(std::list<Sekcija> tabelaSekcija){
  tabelaSekcijaLinkera = tabelaSekcija;
}


int Linker::formirajIzlazniFajl(std::string nazivIzlaznogFajla){

  std::string nazivBezEkstenzije = this->izdvojNazivFajlaBezEkstenzije(nazivIzlaznogFajla);
  if(nazivBezEkstenzije == "greska"){
    return LinkerInfo::NEISPRAVAN_NAZIV_FAJLA;
  }

  std::string nazivIzlazniBinarni = nazivBezEkstenzije + ".o";


  // Upis u .hex fajl
  std::fstream izlazniFajl;           // .hex izlazni fajl
  izlazniFajl.open(nazivIzlaznogFajla, std::ios::out);

  if(izlazniFajl.is_open()){

    // Prolazimo kroz tabelu sekcija i sadrzaj svake sekcije ubacujemo u izlazni fajl
    
    int prviRed = 1;            // Obican flag kako ne bih za prvi upis imao nov red, a da za ostale imam
    int lc = 0;

    
    for(std::pair<uint32_t, uint8_t> iterator : memorijaMapa){
      
      lc = iterator.first;    // Dohvati key tj. adresu
      //std::cout << std::endl << "Adresa: " << std::hex << lc << std::endl;


      // Ako obradjujem prvi red, tada samo ispisem "adresa:"
      // Dok ako obradjujem ostale redove, tada dodajem novi red svaki put kada adresa bude deljiva sa 8
      if(prviRed){
        izlazniFajl << std::hex << lc << ": ";
      }

      if(lc % 8 == 0 && prviRed == 0){
        izlazniFajl << std::endl;
        izlazniFajl << lc << ": ";
      }

      
      // Ispis u izlazni fajl
      if (iterator.second < 0x10) {
        izlazniFajl << "0" << std::hex << static_cast<int>(iterator.second) << " ";
      } else {
          izlazniFajl << std::hex << static_cast<int>(iterator.second) << " ";
      }


      prviRed = 0;    // Postavi flag na 0 nakon sto obradim prvi red
    }

    std::cout << std::endl << std::endl << "Zavrseno formiranje izlaznog fajla!" << std::endl;

    izlazniFajl.close();
  }

  /*
  std::cout << "ISPIS CELOG SADRZAJA MEMORIJE: " << std::endl;
  for(std::pair<uint32_t, uint8_t> iterator : memorijaMapa){

    std::cout << iterator.first << " -> ";
    if (iterator.second < 0x10) {
        std::cout << "0" << std::hex << static_cast<int>(iterator.second) << std::endl;
      } else {
          std::cout << std::hex << static_cast<int>(iterator.second) << std::endl;
      }
  }
  */

  /*
  // Upis u .o fajl
  std::fstream izlazniFajlBinarni;    // .o izlazni fajl, za laksi unos u emulator

  izlazniFajlBinarni.open(nazivIzlazniBinarni, std::ios::out);
  if(izlazniFajlBinarni.is_open()){


    izlazniFajlBinarni.close();
  }

  */

}


std::string Linker::izdvojNazivFajlaBezEkstenzije(std::string nazivFajla){

  int pozicijaEkstenzije = nazivFajla.find(".hex");

  if(pozicijaEkstenzije != std::string::npos){
    return nazivFajla.substr(0, pozicijaEkstenzije);
  }
  else{
    std::cout << "Neispravan naziv fajla! Fajl mora da sadrzi ekstenziju .hex!";
    return "greska";
  }

}



Linker::Linker(){

  locationCounter = 0;
  obradaFajla = 0;
  pomocZaOdredjivanjePocetneAdreseSekcije = 0;
}
