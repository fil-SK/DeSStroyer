#ifndef _enumeratori_h_
#define _enumeratori_h_


// Za pracenje koji prolaz asemblera je u toku
enum ASMProlaz{
  PRVI_PROLAZ,
  DRUGI_PROLAZ
};


// Za razne povratne informacije i kodove greski
enum InfoKod{
  DODAT_SIMBOL_U_TS = 0,
  AZURIRAN_SIMBOL_U_TS = 0,
  SEKCIJA_VEC_ISKORISCENA = -1,
  SIMBOL_UVEDEN_PREKO_EXTERN = -2,
  VISESTRUKA_DEFINICIJA_SIMBOLA = -3,
  LITERAL_NE_MOZE_U_12_BITA = -4
};


// Za nacin povezivanja
enum Povezivanje{
  LOKALNO = 0,
  GLOBALNO = 1
};


// Da li je simbol uveden preko extern direktive
enum externDir{
  NIJE_EXTERN = false,
  JESTE_EXTERN = true
};


enum TipLiterala{
  LITERAL_DECIMALNI,
  LITERAL_HEXA,
  LITERAL_BINARNI,
  LITERAL_OKTALNI
};

enum LinkerInfo{
  NEISPRAVAN_NAZIV_FAJLA = -5,
  PREKLOPLJENE_SEKCIJE = -4,
  NE_POSTOJI_O_OPCIJA = -3,
  NE_POSTOJI_OPCIJA_HEX = -2,
  OBUSTAVAK_RADA_LINKERA = -1,
  USPESNO = 0,
  VISESTRUKA_DEFINICIJA_GLOBALNOG_SIMBOLA_U_LINKERU = 1,
  POSTOJE_UND_SIMBOLI = 2
};


enum EmulatorInfo{
  USPESNO_EMULATOR = 0,
  NEISPRAVAN_INDEKS_REGISTRA = -1,
  NEISPRAVAN_BROJ_ARGUMENATA = -2
};


enum OPKodovi{
  HALT_INSTR = 0,
  INT_INSTR = 1,
  CALL_INSTR = 2,
  SKOK_INSTR = 3,
  XCHG_INSTR = 4,
  ARITMETICKA_INSTR = 5,
  LOGICKA_INSTR = 6,
  POMERACKA_INSTR = 7,
  STORE_INSTR = 8,
  LOAD_INSTR = 9
};

enum Modifikatori{
  CALL_LITERAL_12bit = 0,
  CALL_SIMBOL_ili_CALL_LITERAL_BAZEN = 1,

  JMP_LITERAL_12bit = 0,
  JMP_SIMBOL_ili_JMP_LITERAL_BAZEN = 8,
  BEQ_LITERAL_12bit = 1,
  BEQ_SIMBOL_ili_BEQ_LITERAL_BAZEN = 9,
  BNE_LITERAL_12bit = 2,
  BNE_SIMBOL_ili_BNE_LITERAL_BAZEN = 10,
  BGT_LITERAL_12bit = 3,
  BGT_SIMBOL_ili_BGT_LITERAL_BAZEN = 11,

  PUSH_NA_STEK = 1,
  POP_SA_STEKA = 3,

  SABIRANJE = 0,
  ODUZIMANJE = 1,
  MNOZENJE = 2,
  DELJENJE = 3,

  LOGICKO_NOT = 0,
  LOGICKO_AND = 1,
  LOGICKO_OR = 2,
  LOGICKO_XOR = 3,

  POMERANJE_ULEVO = 0,
  POMERANJE_UDESNO = 1,

  CSR_RD = 0,
  CSR_WR = 4,

  LOAD_LITERAL_VREDNOST_12bit_ili_LOAD_VREDNOST_REG = 1,
  LOAD_LITERAL_VREDNOST_BAZEN_ili_LOAD_MEM_REG = 2,

  STORE_U_MEMORIJU_LITERAL_12bit_ili_REG = 0,
  STORE_MEM_MEM = 2,
  POP_STATUS_ZA_IRET = 6
};

enum Registri{
  REGISTAR_STATUS = 0,
  REGISTAR_HANDLER = 1,
  REGISTAR_CAUSE = 2,
  REGISTAR_SP = 14,
  REGISTAR_PC = 15
};


#endif

/*
  // Zapis vodece nule kod 8bitnog broja - tj. za ispisivanje heksadecimalnih vrednosti na neki izlaz
  //Imam puno 8 bitnih hexa vrednosti koje ubacujem i posle kada ih vadim ih zapisujem sa vodecom nulom
  //Broj svakako jeste 8 bitni, iako mu se nula ne ispise, ali ovako izgleda lepse kada ima nula

  uint8_t val = 0x03;

  std::vector<uint8_t> niz;
  niz.push_back(0x01);
  niz.push_back(val);
  niz.push_back(0x1F);
  niz.push_back(0x07);

  // Ako je manji od 0x10 -->  manji od 0001 0000 --> dakle ako mu je u visoj heksa cifri 0
  for(uint8_t iterator : niz){
    if (iterator < 0x10) {
        std::cout << "0" << std::hex << static_cast<int>(iterator) << " ";
    } else {
        std::cout << std::hex << static_cast<int>(iterator) << " ";
    }
  }
  //A ovako ako hoces da ti sam upise nule
  //std::cout << "Vrednost je " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(val);
*/



  //Za bazen literala, ja cu da prosledjujem bajt po bajt tamo i onda hocu da formiram 32bitnu vrednost

/*
  std::cout << "Formiram 32bitni broj" << std::endl << std::endl << std::endl;
  uint32_t test = 0;
  test = asembler.formiraj32BitniLiteral(0x00, 0x08, 0x1F, 0x07);
  std::cout << test;

  std::cout << "Zapis sa vodecim nulama: 0x" << std::setfill('0') << std::setw(8) << std::hex << test << std::endl;

  Fora ovde je sto ti 32bitni broj ima 8 hexa cifri, pa ako je blank onda ce se na tom mestu iskoristiti fill 0 cifra

*/


 /*
 if(pom.substr(0, 2) == "0x"){
    // Obradjujemo hexa podatak
    std::string brojBez0x = pom.substr(2);      // Ovo ce da napravi podstring koji sadrzi broj, ali bez 0x

    std::cout << "LITERAL JE " << brojBez0x << std::endl;

    int decimalnaVrednost = std::stoi(brojBez0x, nullptr, 16);
    if (decimalnaVrednost > 0x7FFFFFFF) {
        decimalnaVrednost -= 0x100000000; // Subtract 2^32 to get the correct negative value
    }
    std::cout << "DECIMALNA VREDNOST JE " << decimalnaVrednost << std::endl;
    return true;
  }
 */



/*

  Ovako sam inicijalno hteo da radim ispis za JMP sa literalom

 std::cout << "Heksa vrednost literala je ";
      if(literal < 256 && literal > 15){    // 0XX - treba dopisati jednu vodecu nulu
        std::cout << "0" << std::hex << literal << std::endl;
      }
      else if(literal < 16){                // 00X - treba dopisati dve vodece nule
        std::cout << "00" << std::hex << literal << std::endl;
      }
      else{   // Broj je vec sam po sebi sa tri heksa cifre
        std::cout << std::hex << literal << std::endl;
      }
*/


/*

Hexa ispis moras da radis sa cast-om
  uint8_t hexaBr = linker.konvertujDecimalniUHexaBroj(13);
  std::cout << "32 decimalno je hexa " << static_cast<unsigned int>(hexaBr) << std::endl;
  
*/


/*
Prepravka za relok zapise - pocetnu adresu sekcije
pocetnaAdr = obradjivanaSekcija.dohvatiPocetnuAdresuSekcije();
                std::cout << "STAVLJENA JE POCETNA ADRESA " << pocetnaAdr << std::endl;
*/


/*
  Ispis iz hes mape std::map

  for(std::pair<int, uint8_t> iterator : memorija){
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