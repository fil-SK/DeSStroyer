#include <iostream>
#include <vector>
#include <map>                // Za koriscenje std::map
#include <fstream>            // Da bih mogao da koristim stringstream za citanje
#include "../inc/linker.h"
#include "../inc/enumeratori.h"

int proveriArgumente(std::vector<std::string> argumenti, std::string& nazivIzlaznogFajla, std::unordered_map<std::string, uint32_t>& mapaPocetnihAdresaSekcija, std::map<uint32_t, std::string>& mapaUredjenaAdresaSekcija, std::vector<std::string>& ulazniFajlovi);
uint32_t konvertujHexaBrojStringUDecimalni(std::string hexa);


// Linker se pokrece sledecom komandom
// ./linker ulazniFajl1.o ulazniFajl2.o ... ulazniFajlN.o -o nazivIzlaznogFajla.hex -hex

Linker linker;


int main(int argc, char* argv[]){

  
  std::vector<std::string> argumenti;           // Sacuvam sve argumente lokalno ovde
  std::string nazivIzlaznogFajla;               // Unosi se sa ekstenzijom - nazivIzlaza.hex
  std::unordered_map<std::string, uint32_t> mapaSekcijaAdresa;           // Cuvamo vrednosti opcionog parametra place ; key: nazivSekcije, value: pocetna adresa sekcije
  std::map<uint32_t, std::string> mapaUredjenaAdresaSekcija;             // Efektivno isto kao mapa iznad, ali ovde cuvam adrese po uredjenom poretku, kako bih ih lakse dohvatao
  std::vector<std::string> ulazniFajlovi;

  for(int i = 1; i < argc; i++){
    argumenti.push_back(argv[i]);
  }


  int povratnaVr = proveriArgumente(argumenti, nazivIzlaznogFajla, mapaSekcijaAdresa, mapaUredjenaAdresaSekcija, ulazniFajlovi);

  if(povratnaVr == LinkerInfo::NE_POSTOJI_OPCIJA_HEX){
    std::cout << "Opcija -hex je obavezna! Obustavak rada linkera!";
    return LinkerInfo::OBUSTAVAK_RADA_LINKERA;
  }
  else if(povratnaVr == LinkerInfo::NE_POSTOJI_O_OPCIJA){
    std::cout << "Opcija -o je obavezna! Obustavak rada linkera!";
    return LinkerInfo::OBUSTAVAK_RADA_LINKERA;
  }
  else if(povratnaVr == LinkerInfo::PREKLOPLJENE_SEKCIJE){
    std::cout << "Obustava rada linkera!";
    return LinkerInfo::PREKLOPLJENE_SEKCIJE;
  }



  // Ispisujem sacuvane argumente
  std::cout << "Uneti argumenti su :" << std::endl;
  for(std::string iterator : argumenti){
    std::cout << iterator << std::endl;
  }


  std::cout << "Ulazni fajlovi su :" << std::endl;
  for(std::string iterator : ulazniFajlovi){
    std::cout << iterator << std::endl;
  }


  std::cout << "Sekcija -> Adresa: " << std::endl;
  for(std::pair<std::string, uint32_t> iterator : mapaSekcijaAdresa){
    std::cout << "size(" << iterator.first << ") -> " << iterator.second << std::endl;
  }

  std::cout << "UREDJENO: Adresa -> Sekcija: " << std::endl;
  for(std::pair<uint32_t, std::string> iterator : mapaUredjenaAdresaSekcija){
    std::cout << "adr(" << iterator.first << ") -> " << iterator.second << std::endl;
  }


  // PRVI DEO: Procitaj svaki fajl
  for(int i = 0; i < ulazniFajlovi.size(); i++){
    if(linker.procitajFajl(ulazniFajlovi[i]) == LinkerInfo::VISESTRUKA_DEFINICIJA_GLOBALNOG_SIMBOLA_U_LINKERU){
      return 1;   // Bitno mi je samo da se prekine program
    }
  }
  

  std::cout << std::endl << std::endl << "--- SAD VRSIMO ISPIS PARSIRANOG ---" << std::endl << std::endl;

  linker.ispisiSadrzajTabeleSekcijaLinkera();


  std::cout << std::endl << std::endl << "Tabela simbola linkera" << std::endl << std::endl;
  linker.ispisiSadrzajTabeleSimbolaLinkera();


  // DEO 1.5 : Provera UND simbola

  // OBAVEZNO OVO OTKOMENTARISI NA KRAJU
  // Provera da li u tabeli simbola postoje nerazreseni simboli - simboli sa sekcijom UND (tj. sekcijom sa rednim brojem 0)
  // Ukoliko ih ima, to je GRESKA! U tom slucaju linker prekida sa svojim radom!
  if(linker.proveriPostojanjeSimbolaSaSekcijomUND()){
    std::cout << "Postoje nerazreseni simboli! Obustavljanje rada linkera!" << std::endl;
    return LinkerInfo::POSTOJE_UND_SIMBOLI;
  }
  

 
  // DEO 1.6 : Postavljanje da sekcije krecu od zadatih adresa

  // Potrebno je proveriti da li je korisnik specificirao da neke sekcije krenu od odredjene adrese
  // Pocetne adrese sekcija inkorporiram nakon sto sam formirao inicijalnu tabelu sekcija i simbola, parsirajuci sve fajlove
  if(mapaSekcijaAdresa.size() > 0){

    // U ovom slucaju potrebno je prepraviti i tabelu sekcija i tabelu simbola linkera
    // tako da se koriste specificirane pocetne adrese, i da simboli budu sa offset-ima u odnosu na te adrese
    linker.postaviPremaUnetimPocetnimAdresama(mapaSekcijaAdresa, mapaUredjenaAdresaSekcija);

    if(linker.proveriDaLiPostojiPreklapanjeSekcija() == LinkerInfo::PREKLOPLJENE_SEKCIJE){
      std::cout << "Obustava rada linkera!";
      return LinkerInfo::PREKLOPLJENE_SEKCIJE;
    }

    // Ukoliko nema preklopljenih sekcija - nastavljamo dalje
    std::cout << "TABELA SEKCIJA NAKON IZMENE :" << std::endl;
    linker.ispisiSadrzajTabeleSekcijaLinkera();
  }

  // Nakon ovoga, konacan izgled tabele SEKCIJA linkera je formiran (sa ili bez zadatih pocetnih adresa - oba slucaja pokrivena)
  // Tabelu simbola linkera nije potrebno azurirati, jer tabela simbola ne sadrzi podatke o pocetnoj adresi sekcije
  // Samim tim ono sto je dosad smesteno u nju je relevantno, bilo da je promenjena pocetna adresa sekcije ili ne



  // Nakon sto je formirana konacna vrednost za tabelu sekcija linkera potrebno je da prepravimo relokacione zapise
  // Tako da se za pocetak sekcije uzima pocetna adresa te konkretne sekcije iz linkerove tabele sekcija
  linker.popraviPocetneAdreseURelokacionimZapisima();



  std::cout << std::endl << std::endl << "Masinski sadrzaji sekcija:" << std::endl << std::endl;
  
  for(Sekcija iterator : linker.dohvatiTabeluSekcijaLinkera()){
    std::cout << "Masinski sadrzaj:" << std::endl;
    std::cout << "Sekcija : " << iterator.dohvatiNazivSekcije() << std::endl;
    iterator.ispisiSadrzaj();
    std::cout << std::endl << "Relokacioni zapisi:" << std::endl << std::endl;
    iterator.ispisiRelokacioneZapise();
  }

  std::cout << std::endl << std::endl << "--- KRAJ RADA PRVOG DELA ---" << std::endl;



  // DRUGI DEO - Smestanje sadrzaja u memoriju - formiram jedan vektor koji ce mi biti MEMORIJA i onda redom idem i smestam ovo sto imam u taj vektor
  // Prolazim redom kroz tabelu sekcija i redom smestam sadrzaj iz tih sekcija u vektor
  std::cout << std::endl << "--- FORMIRANJE INICIJALNOG MEMORIJSKOG SADRZAJA ---" << std::endl << std::endl;

  //linker.formirajMemorijskiSadrzaj();
  linker.formirajMemorijskiSadrzajMapa();

  
  //linker.ispisiMemorijskiSadrzaj();
  linker.ispisiMemorijskiSadrzajMapa();
  
  std::cout << std::endl << std::endl;

  // TRECI DEO - Prepravka memorijskog sadrzaja, koristeci relokacione zapise i tabelu simbola
  //linker.prepraviMemorijskiSadrzaj();
  linker.prepraviMemorijskiSadrzajMapa();

  std::cout << "--- MEM. SADRZAJ NAKON PREPRAVKE ---" << std::endl << std::endl;
  //linker.ispisiMemorijskiSadrzaj();   // Ispisi sadrzaj nakon prepravke
  linker.ispisiMemorijskiSadrzajMapa();

  
  // CETVRTI DEO - Formiranje izlaznog sadrzaja

  if(linker.formirajIzlazniFajl(nazivIzlaznogFajla) == LinkerInfo::NEISPRAVAN_NAZIV_FAJLA){
    std::cout << "Izlazni fajl nece biti generisan! Obustava rada linkera!" << std::endl;
    return LinkerInfo::NEISPRAVAN_NAZIV_FAJLA;
  }
  else{
    std::cout << "Linker zavrsio sa radom!" << std::endl;
  }


}


int proveriArgumente(std::vector<std::string> argumenti, std::string& nazivIzlaznogFajla, std::unordered_map<std::string, uint32_t>& mapaSekcijaAdresa, std::map<uint32_t, std::string>& mapaAdresaSekcija, std::vector<std::string>& ulazniFajlovi){

  // Moram da proverim da li se nalaze obavezne opcije -hex i -o
  bool postojiHexOpcija = false;
  bool postojiOpcijaIzlazniFajl = false;
  
  std::string placeOpcija = "-place=";      // Koristim da proverim da li argument pocinje sa ovom opcijom


  for(int i = 0; i < argumenti.size(); i++){

    // Proveri da li postoji -hex opcija
    if(argumenti[i] == "-hex"){
      postojiHexOpcija = true;
    }

    // Proveri da li postoji opcija za izlazni fajl
    else if(argumenti[i] == "-o"){
      postojiOpcijaIzlazniFajl = true;

      nazivIzlaznogFajla = argumenti[i + 1];
    }

    else if(argumenti[i - 1] == "-o"){
      // Ovde ne radim nista, ali mi je trebao ovaj case kako bi mi za finalni else ostalo da pokriva input fajlove, pa da njih sacuvam
    }

    // Proveri da li postoji postavljanje adresa sekcija
    else if(argumenti[i].compare(0, placeOpcija.length(), placeOpcija) == 0){
      
      // Ovo ce da izdvoji podstring <sekcija>@<adresa>
      std::string deoNakonPlaceOpcije = argumenti[i].substr(placeOpcija.length());    


      int pozicijaEtSimbola = deoNakonPlaceOpcije.find("@");

      // Iz podstringa <sekcija>@<adresa> izdvoj posebno <sekcija> i posebno <adresa> deo
      std::string sekcija = deoNakonPlaceOpcije.substr(0, pozicijaEtSimbola);
      std::string adresa = deoNakonPlaceOpcije.substr(pozicijaEtSimbola + 1);

      std::cout << "Adresa je: " << std::hex << adresa << std::endl;
      uint32_t adresaBroj = konvertujHexaBrojStringUDecimalni(adresa);


      // Pre nego sto dodam ovaj argument proveri da li je ranije vec dodata sekcija koja pocinje od iste te adrese
      // U tom slucaju, to je efektivno preklapanje, i to je greska usled koje linker mora da prekine svoj rad
      if(mapaAdresaSekcija.count(adresaBroj) > 0){
        std::cout << "GRESKA! Nije moguce dodati sekciju " << sekcija << " zato sto se specificirana pocetna adresa poklapa sa ranije postavljenom adresom - za sekciju " << mapaAdresaSekcija.at(adresaBroj) << std::endl;
        return LinkerInfo::PREKLOPLJENE_SEKCIJE;
      }
      else{
        // Nema preklapanja (direktnog - da se smestaju na IDENTICNU adresu), pa zasad mozemo sacuvati ove sekcije i nastaviti dalje
        mapaSekcijaAdresa.insert({sekcija, adresaBroj});
        mapaAdresaSekcija.insert({adresaBroj, sekcija});
      }

      
    }

    // Ulazni fajlovi linkera
    else{
      ulazniFajlovi.push_back(argumenti[i]);
    }

  }

  if(!postojiHexOpcija){
    return LinkerInfo::NE_POSTOJI_OPCIJA_HEX;
  }

  if(!postojiOpcijaIzlazniFajl){
    return LinkerInfo::NE_POSTOJI_O_OPCIJA;
  }
  
}


uint32_t konvertujHexaBrojStringUDecimalni(std::string hexa){

  unsigned long longBroj = std::stoul(hexa, nullptr, 16);   // hexaBroj string tumaci kao broj sa osnovom 16 (tj. kao heksa broj) i njega konvertuje u decimalni broj sa osnovom 10

  uint32_t decimalniBroj = static_cast<uint32_t>(longBroj);
  std::cout << "Broj je: " << decimalniBroj << std::endl;
  return decimalniBroj;
}