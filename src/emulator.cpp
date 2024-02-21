
#include "../inc/emulator.h"
#include "../inc/enumeratori.h"

int& Emulator::dohvatiGPRRegistar(int brojRegistra){

  /*  Ovaj deo ne mogu da imam zato sto sam promenio da vratim referencu, pa ne mogu da vratim int enum
  if(brojRegistra < 0 || brojRegistra > 15){
    std::cout << "Neispravan indeks registra!" << std::endl;
    return EmulatorInfo::NEISPRAVAN_INDEKS_REGISTRA;
  }
  */

  switch(brojRegistra){
    case 0: return r0;
    case 1: return r1;
    case 2: return r2;
    case 3: return r3;
    case 4: return r4;
    case 5: return r5;
    case 6: return r6;
    case 7: return r7;
    case 8: return r8;
    case 9: return r9;
    case 10: return r10;
    case 11: return r11;
    case 12: return r12;
    case 13: return r13;
    case 14: return r14;
    case 15: return r15;
  }
}


int Emulator::upisiUGPRRegistar(int brojRegistra, int vrednost){

  if(brojRegistra < 0 || brojRegistra > 15){
    std::cout << "Neispravan indeks registra!" << std::endl;
    return EmulatorInfo::NEISPRAVAN_INDEKS_REGISTRA;
  }

  switch(brojRegistra){
    case 0: r0 = 0; break;    // R0 je uvek ozicen na vrednost 0, tako da, iako se pokusa upis u njega, taj upis je invalidiran i upise se 0
    case 1: r1 = vrednost; break;
    case 2: r2 = vrednost; break;
    case 3: r3 = vrednost; break;
    case 4: r4 = vrednost; break;
    case 5: r5 = vrednost; break;
    case 6: r6 = vrednost; break;
    case 7: r7 = vrednost; break;
    case 8: r8 = vrednost; break;
    case 9: r9 = vrednost; break;
    case 10: r10 = vrednost; break;
    case 11: r11 = vrednost; break;
    case 12: r12 = vrednost; break;
    case 13: r13 = vrednost; break;
    case 14: r14 = vrednost; break;
    case 15: r15 = vrednost; break;
  }

  return EmulatorInfo::USPESNO_EMULATOR;
}


int Emulator::dohvatiPC(){
  return r15;
}

int Emulator::dohvatiSP(){
  return r14;
}


int& Emulator::dohvatiCSRRegistar(int brojRegistra){

  /*  Ne mogu da imam ovo posto vracam registar po referenci
  if(brojRegistra < 0 || brojRegistra > 2){
    std::cout << "Neispravan indeks registra!" << std::endl;
    return EmulatorInfo::NEISPRAVAN_INDEKS_REGISTRA;
  }
  */


  switch(brojRegistra){
    case 0: return status;
    case 1: return handler;
    case 2: return cause;
  }
}


int Emulator::upisiUCSRRegistar(int brojRegistra, int vrednost){

  if(brojRegistra < 0 || brojRegistra > 2){
    std::cout << "Neispravan indeks registra!" << std::endl;
    return EmulatorInfo::NEISPRAVAN_INDEKS_REGISTRA;
  }

  switch(brojRegistra){
    case 0: status = vrednost; break;
    case 1: handler = vrednost; break;
    case 2: cause = vrednost; break;
  }

  return EmulatorInfo::USPESNO_EMULATOR;
}


void Emulator::upisiUMemoriju(uint32_t adresa, uint8_t podatak){

  memorija.insert({adresa, podatak});
}


void Emulator::pushGPR(int registar){
  
  // R14 je SP ; SP pokazuje na poslednju zauzetu adresu, SP raste nanize
  // Little Endian - nizi bajt na nizoj adresi
  
  // Kao argument funkcije prosledice se redni broj registra
  int vrednostRegistra = this->dohvatiGPRRegistar(registar);


  // Unesi u memoriju - odradi PUSH
  uint8_t bajt31_24 = vrednostRegistra >> 24;
  uint8_t bajt23_16 = vrednostRegistra >> 16;
  uint8_t bajt15_8  = vrednostRegistra >> 8;
  uint8_t bajt7_0   = vrednostRegistra & 0xFF;

  // Upis u memoriju
  // Ne koristim metodu upisiUMemoriju, jer ona radi sa insert pa ovo ne bi izvrsilo overwrite
  r14 -= 1;
  memorija[r14] = bajt31_24;

  r14 -= 1;
  memorija[r14] = bajt23_16;

  r14 -= 1;
  memorija[r14] = bajt15_8;

  r14 -= 1;
  memorija[r14] = bajt7_0;
}


void Emulator::pushCSR(int registar){

  // R14 je SP ; SP pokazuje na poslednju zauzetu adresu, SP raste nanize
  // Little Endian - nizi bajt na nizoj adresi

  // Kao argument funkcije prosledice se redni broj registra
  int vrednostRegistra = this->dohvatiCSRRegistar(registar);


  // Unesi u memoriju - odradi PUSH
  uint8_t bajt31_24 = vrednostRegistra >> 24;
  uint8_t bajt23_16 = vrednostRegistra >> 16;
  uint8_t bajt15_8  = vrednostRegistra >> 8;
  uint8_t bajt7_0   = vrednostRegistra & 0xFF;

  r14 -= 1;
  memorija[r14] = bajt31_24;

  r14 -= 1;
  memorija[r14] = bajt23_16;

  r14 -= 1;
  memorija[r14] = bajt15_8;

  r14 -= 1;
  memorija[r14] = bajt7_0;
}


void Emulator::popGPR(int registar){

  
  uint8_t bajt7_0 = memorija[r14];
  r14 += 1;

  uint8_t bajt15_8 = memorija[r14];
  r14 += 1;

  uint8_t bajt23_16 = memorija[r14];
  r14 += 1;

  uint8_t bajt31_24 = memorija[r14];
  r14 += 1;


  int vrednost = (bajt31_24 << 24) | (bajt23_16 << 16) | (bajt15_8 <<8) | bajt7_0;
  this->upisiUGPRRegistar(registar, vrednost);
  
}


void Emulator::pomeriPCNaNarednuInstrukciju(){
  
  // R15 je PC
  r15 += 4;   // Instrukcije su po 4B, tako da se inkrementiranjem za 4 prelazi na narednu instrukciju
}


void Emulator::procitajUlazniFajl(std::string nazivFajla){

  std::ifstream ulazniFajl;
  
  ulazniFajl.open(nazivFajla);

  if(ulazniFajl.is_open()){

    uint32_t adresa;   // Adresa na kojoj smestamo jedan bajt sadrzaja
    std::string jednaLinija;


    // Citaj iz ulaznog fajla
    while(std::getline(ulazniFajl, jednaLinija)){

      std::stringstream stringTok(jednaLinija);   // Dohvata jedan po jedan string iz linije, razdvojen belinama
      std::string chunk;


      // Sacuvamo adresu sa pocetka reda
      stringTok >> chunk;                                                   // Ovo ce u chunk ucitati adresu na kojoj je sadrzaj
      std::string adresaBezDvotacke = chunk.substr(0, chunk.length() - 1);
      adresa = this->konvertujStringAdresuUHexaBroj(adresaBezDvotacke);
      

      // Sacuvamo sadrzaj
      int temp;
      while(stringTok >> chunk){

        temp = this->konvertujStringBajtUHexaBroj(chunk);
        this->upisiUMemoriju(adresa, temp);         // Ovde sme upis u memoriju preko insert jer se ovde tek ucitava fajl
        adresa++;
      }

    }

  }
}


uint8_t Emulator::konvertujStringBajtUHexaBroj(std::string bajt){

  std::string temp = "0x";
  temp += bajt;

  // Isti princip kao i konverzija kod asemblera
  int decimalniBroj = std::stoi(temp, nullptr, 16);
  uint8_t hexaBroj = static_cast<int>(decimalniBroj);

  return hexaBroj;
}


uint32_t Emulator::konvertujStringAdresuUHexaBroj(std::string adresa){
  
  std::string temp = "0x";
  temp += adresa;

  // Isti princip kao i konverzija kod mainLinker-a za adresu

  unsigned long longBroj = std::stoul(temp, nullptr, 16);
  uint32_t decimalniBroj = static_cast<uint32_t>(longBroj);


  // Ne moze ovo jer je adresa unsigned int podatak
  // long longBroj = std::stol(temp, nullptr, 16);
  // int decimalniBroj = static_cast<int>(longBroj);

  /*
  Ovo nije moglo jer sam dobijao gresku  std::out_of_range, pa sam radio sa long i onda to cast-ovao u int
  int decimalniBroj = std::stoi(temp, nullptr, 16);
  int hexaBroj = static_cast<int>(decimalniBroj);
  */

  return decimalniBroj;
}


void Emulator::ispisiSadrzajMemorije(){

  std::cout << "Sadrzaj memorije emulatora:" << std::endl;

    for(std::pair<uint32_t, uint8_t> iterator : memorija){
      std::cout << std::hex << iterator.first << " -> ";

      if (iterator.second < 0x10) {
        std::cout << "0" << std::hex << static_cast<int>(iterator.second) << " ";
      } else {
          std::cout << std::hex << static_cast<int>(iterator.second) << " ";
      }
      
      std::cout << std::endl;
    }
}


void Emulator::pokreniIzvrsavanje(){

  // Ucitaj sadrzaj sa adrese iz PC registra
  // Ovo ce kao bocni efekat pomeriti i PC na narednu instrukciju

  uint8_t bajt31_24, bajt23_16, bajt15_8, bajt7_0;
  
  this->ucitaj4BIzMemorije(bajt31_24, bajt23_16, bajt15_8, bajt7_0);    // Prosledjujem po referenci
  
   

  // Nastavi da citas sve dok ne ucitas 4B nula, sto je HALT instrukcija
  while(!(bajt31_24 == 0 && bajt23_16 == 0 && bajt15_8 == 0 && bajt7_0 == 0)){
    
    // Analiziraj sadrzaj u ova cetiri bajta
    this->obradiInstrukciju(bajt31_24, bajt23_16, bajt15_8, bajt7_0);

    std::cout << "Stanje registara:" << std::endl;
    this->ispisiStanjeNakonEmulacije();
    std::cout << std::endl;
    this->ispisiStanjeCSRRegistara();
    std::cout << std::endl;
    std::cout << "------------------" << std::endl;

    // Predji na izvrsavanje naredne instrukcije
    this->ucitaj4BIzMemorije(bajt31_24, bajt23_16, bajt15_8, bajt7_0);
  }

  std::cout << "KRAJ IZVRSAVANJA!" << std::endl;
}


void Emulator::ucitaj4BIzMemorije(uint8_t& bajt31_24, uint8_t& bajt23_16, uint8_t& bajt15_8, uint8_t& bajt7_0){

  // Little Endian - Nizi bajt je na nizoj adresi
  
  bajt7_0 = memorija[this->dohvatiPC()];
  bajt15_8 = memorija[this->dohvatiPC() + 1];
  bajt23_16 = memorija[this->dohvatiPC() + 2];
  bajt31_24 = memorija[this->dohvatiPC() + 3];

  this->pomeriPCNaNarednuInstrukciju();

}


int Emulator::dohvatiSadrzajIzMemorijeSa32bitAdrese(uint32_t adresa){

  uint8_t sadrzaj31_24 = memorija[adresa + 3];
  uint8_t sadrzaj23_16 = memorija[adresa + 2];
  uint8_t sadrzaj15_8 = memorija[adresa + 1];
  uint8_t sadrzaj7_0 = memorija[adresa];

  int vrednost = (sadrzaj31_24 << 24) | (sadrzaj23_16 << 16) | (sadrzaj15_8 << 8) | sadrzaj7_0;

  return vrednost;
}


void Emulator::upisi32BitPodatakUMemoriju(uint32_t adresa, int vrednost){

  uint8_t bajt31_24 = vrednost >> 24;
  uint8_t bajt23_16 = vrednost >> 16;
  uint8_t bajt15_8 = vrednost >> 8;
  uint8_t bajt7_0 = vrednost & 0xFF;

  memorija[adresa] = bajt7_0;
  memorija[adresa + 1] = bajt15_8;
  memorija[adresa + 2] = bajt23_16;
  memorija[adresa + 3] = bajt31_24;
  
  /*
  Ovo nisam mogao da koristim zato sto, u slucaju kada vec postoji value za ovaj key, nece se uraditi overwrite podataka
  memorija.insert({adresa,     bajt7_0});
  memorija.insert({adresa + 1, bajt15_8});
  memorija.insert({adresa + 2, bajt23_16});
  memorija.insert({adresa + 3, bajt31_24});
  */
}


void Emulator::obradiInstrukciju(uint8_t bajt31_24, uint8_t bajt23_16, uint8_t bajt15_8, uint8_t bajt7_0){

  // Little Endian - ali ovde u bajtovima je vec smesten sadrzaj koji treba tu da bude

  uint8_t operacioniKod, modifikatorInstrukcije;
  uint8_t regABroj, regBBroj, regCBroj;
  int displacement_12bit = 0;
  int temp;                                         // Za obradu displacement-a

  
  // Izvuci podatke s kojima radis
  operacioniKod = (bajt31_24 >> 4);
  modifikatorInstrukcije = (bajt31_24 & 0xF);
  regABroj = (bajt23_16 >> 4);
  regBBroj = (bajt23_16 & 0xF);
  regCBroj = (bajt15_8 >> 4);
  
  temp = (0xF & bajt15_8) << 8;                     // Ovo su visa 4 bita od 12 bita displacement-a
  displacement_12bit = 0xFF & bajt7_0;              // U displacement smesti 8 bita
  displacement_12bit = temp | displacement_12bit;   // Finalna displacement vrednost (heksadecimalna)

  


  // Dohvati registre sa zadatih indeksa
  int regA = this->dohvatiGPRRegistar(regABroj);
  int regB = this->dohvatiGPRRegistar(regBBroj);
  int regC = this->dohvatiGPRRegistar(regCBroj);

  int csrA = this->dohvatiCSRRegistar(regABroj);
  int csrB = this->dohvatiCSRRegistar(regBBroj);


  // Da vidim sta sam to procitao
  podaciOInstrukciji(operacioniKod, modifikatorInstrukcije, regABroj, regBBroj, regCBroj, displacement_12bit);
  


  // Obrada instrukcije
  switch(operacioniKod){


    case OPKodovi::INT_INSTR:
    {
      // obradiInstrukcijuInt
      this->pushCSR(Registri::REGISTAR_STATUS);                               // push status
      this->pushGPR(Registri::REGISTAR_PC);                                   // push pc

      this->upisiUCSRRegistar(Registri::REGISTAR_CAUSE, 4);                   // cause = 4

      int statusReg = this->dohvatiCSRRegistar(Registri::REGISTAR_STATUS);
      statusReg = statusReg & (~0x1);
      this->upisiUCSRRegistar(Registri::REGISTAR_STATUS, statusReg);          // status = status & (~0x1)

      std::cout << "HANDLER: " << handler << ", a PC: " << r15 << std::endl;
      this->upisiUGPRRegistar(Registri::REGISTAR_PC, this->dohvatiCSRRegistar(Registri::REGISTAR_HANDLER)); // pc = handler
      std::cout << "HANDLER: " << handler << ", a PC: " << r15 << std::endl;

      break;
    }


    case OPKodovi::CALL_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::CALL_LITERAL_12bit){
        // obradiInstrukcijuCallLiteral - kada literal moze da stane u 12 bita
        // obradiInstrukcijuCallSimbol - kada je simbol u istoimenoj sekciji pa ne postoji relokacioni zapis
        this->pushGPR(Registri::REGISTAR_PC);                                               // push pc
        this->upisiUGPRRegistar(Registri::REGISTAR_PC, regA + regB + displacement_12bit);   // pc = regA + regB + D
      }

      else if(modifikatorInstrukcije == Modifikatori::CALL_SIMBOL_ili_CALL_LITERAL_BAZEN){
        // obradiInstrukcijuCallLiteral - kada literal ne moze u 12 bita pa mora u bazen literala
        // obradiInstrukcijuCallSimbol - kada simbol nije u istoj sekciji, dakle formiran je relok. zapis, pa se simbol nasao u bazenu literala
        this->pushGPR(Registri::REGISTAR_PC);                                               // push pc
        int adr = regA + regB + displacement_12bit;
        std::cout << "Pomeraj do simbola u bazenu literala je: " << adr << std::endl;
        std::cout << "Simbol je: " << this->dohvatiSadrzajIzMemorijeSa32bitAdrese(adr) << std::endl;
        this->upisiUGPRRegistar(Registri::REGISTAR_PC, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(adr));

        std::cout << "PC: " <<this->dohvatiPC() << std::endl;
      }

      break;
    }


    case OPKodovi::SKOK_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::JMP_LITERAL_12bit){
        //obradiInstrukcijuJmpLiteral - kada literal moze u 12 bita
        // obradiInstrukcijuJmpSimbol - kada je simbol u istoj sekciji, ne postoji relok. zapis, vec se na PC doda pomeraj do simbola
        this->upisiUGPRRegistar(Registri::REGISTAR_PC, regA + displacement_12bit);

        std::cout << "PC:" << std::hex << regA << ", pomeraj: " << displacement_12bit << std::endl;
        std::cout << "Finalna adresa skoka: " << std::hex << regA + displacement_12bit << std::endl;
      }

      else if(modifikatorInstrukcije == Modifikatori::JMP_SIMBOL_ili_JMP_LITERAL_BAZEN){
        // obradiInstrukcijuJmpLiteral - kada literal ne moze u 12 bita pa mora u bazen literala
        // obradiInstrukcijuJmpSimbol - simbol u drugoj sekciji, postoji relok. zapis, pa se simbol stavlja u bazen literala
        // I za JMP simbol, jer simbol svakako mora da se nadje u bazenu literala
        
        /*
        Ovo mi je stajalo ranije, ali ovo nije tacno, pogresno je bilo implementirano u asembleru
        std::cout << "PC:" << std::hex << regA << ", pomeraj do literala u bazenu: " << displacement_12bit << std::endl;
        std::cout << "Finalna adresa skoka (zapravo msm da je ovo taj pomeraj koji TEK TREBA da se doda na PC): " << std::hex << this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + displacement_12bit) << std::endl;

        int finalnaAdresaSkoka = regA + this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + displacement_12bit);
        std::cout << "Finalna adresa: " << finalnaAdresaSkoka << std::endl;

        this->upisiUGPRRegistar(Registri::REGISTAR_PC, finalnaAdresaSkoka);
        */

       this->upisiUGPRRegistar(Registri::REGISTAR_PC, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + displacement_12bit));
      }

      else if(modifikatorInstrukcije == Modifikatori::BEQ_LITERAL_12bit){
        // obradiInstrukcijuBeqLiteral - kada literal moze u 12 bita
        // obradiInstrukcijuBeqSimbol - kada je simbol u istoj sekciji, ne postoji relok. zapis, vec se na PC doda pomeraj do simbola
        if(regB == regC){
          this->upisiUGPRRegistar(Registri::REGISTAR_PC, regA + displacement_12bit);
        }
      }

      else if(modifikatorInstrukcije == Modifikatori::BEQ_SIMBOL_ili_BEQ_LITERAL_BAZEN){
        // obradiInstrukcijuBeqLiteral - kada literal ne moze u 12 pa mora u bazen literala
        // obradiInstrukcijuBeqSimbol - simbol u drugoj sekciji, postoji relok. zapis, pa se simbol stavlja u bazen literala
        if(regB == regC){
          this->upisiUGPRRegistar(Registri::REGISTAR_PC, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + displacement_12bit));
        }
      }

      else if(modifikatorInstrukcije == Modifikatori::BNE_LITERAL_12bit){
        // obradiInstrukcijuBneLiteral - kada literal moze u 12 bita
        // obradiInstrukcijuBneSimbol - kada je simbol u istoj sekciji, ne postoji relok. zapis, vec se na PC doda pomeraj do simbola
        if(regB != regC){
          this->upisiUGPRRegistar(Registri::REGISTAR_PC, regA + displacement_12bit);
        }
      }

      else if(modifikatorInstrukcije == Modifikatori::BNE_SIMBOL_ili_BNE_LITERAL_BAZEN){
        // obradiInstrukcijuBneLiteral - kada literal ne moze u 12 bita pa mora u bazen literala
        // obradiInstrukcijuBneSimbol - simbol u drugoj sekciji, postoji relok. zapis, pa se simbol stavlja u bazen literala
        if(regB != regC){
          this->upisiUGPRRegistar(Registri::REGISTAR_PC, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + displacement_12bit));
        }
      }

      else if(modifikatorInstrukcije == Modifikatori::BGT_LITERAL_12bit){
        // obradiInstrukcijuBgtLiteral - kada literal moze u 12 bita
        // obradiInstrukcijuBgtSimbol - kada je simbol u istoj sekciji, ne postoji relok. zapis, vec se na PC doda pomeraj do simbola
        if(regB > regC){
          this->upisiUGPRRegistar(Registri::REGISTAR_PC, regA + displacement_12bit);
        }
      }

      else if(modifikatorInstrukcije == Modifikatori::BGT_SIMBOL_ili_BGT_LITERAL_BAZEN){
        // obradiInstrukcijuBgtLiteral - kada literal ne moze u 12 bita pa mora u bazen literala
        // obradiInstrukcijuBgtSimbol - simbol u drugoj sekciji, postoji relok. zapis, pa se simbol stavlja u bazen literala
        if(regB > regC){
          this->upisiUGPRRegistar(Registri::REGISTAR_PC, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + displacement_12bit));
        }
      }

      break;
    }


    case OPKodovi::ARITMETICKA_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::SABIRANJE){
        // obradiInstrukcijuAdd
        this->upisiUGPRRegistar(regABroj, regB + regC);
      }

      else if(modifikatorInstrukcije == Modifikatori::ODUZIMANJE){
        // obradiInstrukcijuSub
        this->upisiUGPRRegistar(regABroj, regB - regC);
      }

      else if(modifikatorInstrukcije == Modifikatori::MNOZENJE){
        // obradiInstrukcijuMul
        this->upisiUGPRRegistar(regABroj, regB * regC);
      }

      else if(modifikatorInstrukcije == Modifikatori::DELJENJE){
        // obradiInstrukcijuDiv
        this->upisiUGPRRegistar(regABroj, regB / regC);
      }

      break;
    }


    case OPKodovi::LOGICKA_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::LOGICKO_NOT){
        // obradiInstrukcijuNot
        this->upisiUGPRRegistar(regABroj, ~regB);
      }

      else if(modifikatorInstrukcije == Modifikatori::LOGICKO_AND){
        // obradiInstrukcijuAnd
        this->upisiUGPRRegistar(regABroj, regB & regC);
      }

      else if(modifikatorInstrukcije == Modifikatori::LOGICKO_OR){
        //obradiInstrukcijuOr
        this->upisiUGPRRegistar(regABroj, regB | regC);
      }

      else if(modifikatorInstrukcije == Modifikatori::LOGICKO_XOR){
        // obradiInstrukcijuXor
        this->upisiUGPRRegistar(regABroj, regB ^ regC);
      }

      break;
    }


    case OPKodovi::POMERACKA_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::POMERANJE_ULEVO){
        // obradiInstrukcijuShl
        this->upisiUGPRRegistar(regABroj, regB << regC);
      }

      else if(modifikatorInstrukcije == Modifikatori::POMERANJE_UDESNO){
        // obradiInstrukcijuShr

        // Shift-uje se levi operand za broj bita desnog operanda:  a >> b, a se shiftuje za b broj bita
        int rezultat;
        if((regB & 0x80000000) == 0x80000000){
          rezultat = (regB >> regC) | 0x80000000;   // Uradi shift ali i zadrzi bit 1 kao znak
        }
        else{
          rezultat = regB >> regC;
        }

        this->upisiUGPRRegistar(regABroj, rezultat);    // Na ovaj nacin, ocuvan je bit koji predstavlja znak
      }

      break;
    }


    case OPKodovi::XCHG_INSTR:
    {
      // obradiInstrukcijuXchg
      int privremena = regB;
      this->upisiUGPRRegistar(regBBroj, regC);
      this->upisiUGPRRegistar(regCBroj, privremena);
      
      break;
    }


    case OPKodovi::LOAD_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::LOAD_LITERAL_VREDNOST_12bit_ili_LOAD_VREDNOST_REG){
        // LD $literal, gpr ; obradiInstrukcijuLdLiteralVrednost - kada literal moze da stane u 12 bita
        // LD %reg, gpr ; gpr = reg;   obradiInstrukcijuLdGPRVrednost
        // LD $simbol, gpr ; obradiInstrukcijuLdSimbolVrednost - kada se koristi simbol koji je u istoj sekciji, nema relok. zapisa

        // obradiInstrukcijuLdSimbolMem - prvi deo ove instrukcije kada je simbol u istoj sekciji -- kada se prvo u registar smesti adresa sa koje treba procitati vrednost

        this->upisiUGPRRegistar(regABroj, regB + displacement_12bit);
      }

      else if(modifikatorInstrukcije == Modifikatori::LOAD_LITERAL_VREDNOST_BAZEN_ili_LOAD_MEM_REG){
        // LD $literal, gpr ;         obradiInstrukcijuLdLiteralVrednost - kada literal moze da stane u 12 bita
        // gpr = mem[reg];            obradiInstrukcijuLdGPRMem
        // gpr = mem[reg + offset]    obradiInstrukcijuLdGPRMemOffsLiteral
        //                            obradiInstrukcijuLdLiteralMem
        // LD $simbol, gpr ;          obradiInstrukcijuLdSimbolVrednost - simbol u drugoj sekciji, postoji relok. zapis, pa je simbol u bazenu literala

        // LD simbol, gpr ;           obradiInstrukcijuLdSimbolMem - drugi deo ove instrukcije kada je simbol u istoj sekciji -- odradis memind 
        // LD simbol, gpr ;           obradiInstrukcijuLdSimbolMem - celokupna instrukcija, kada je simbol u drugoj sekciji -- radis duplo memind adresiranje
        this->upisiUGPRRegistar(regABroj, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regB + regC + displacement_12bit));
      }

      else if(modifikatorInstrukcije == Modifikatori::CSR_WR){
        // obradiInstrukcijuCsrWr
        this->upisiUCSRRegistar(regABroj, regB);
      }

      else if(modifikatorInstrukcije == Modifikatori::CSR_RD){
        // obradiInstrukcijuCsrRd
        this->upisiUGPRRegistar(regABroj, csrB);
      }

      else if(modifikatorInstrukcije == Modifikatori::POP_SA_STEKA){
        // regB ce ovde biti postavljeno na SP

        // obradiInstrukcijuPop
        // obradiInstrukcijuRet
        // obradiInstrukcijuIret - U drugom delu instrukcije, kada je potrebno skinuti PC sa steka
        this->upisiUGPRRegistar(regABroj, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regB));
        this->upisiUGPRRegistar(regBBroj, regB + displacement_12bit);     // Azuriraj SP : SP+=4  ili SP+=8 u onoj drugoj situaciji kod IRET realizacije

        std::cout << "PC: " << std::hex << this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regB) << std::endl;
        std::cout << "SP: " << std::hex << regB + displacement_12bit << std::endl;
      }

      else if(modifikatorInstrukcije == Modifikatori::POP_STATUS_ZA_IRET){
        // obradiInstrukcijuIret
        // Ovde je obradjen prvi deo realizacije IRET instrukcije - skidamo status sa steka, ali tako sto preskocimo PC koji je trenutno na vrhu
        

        uint32_t adresaSaSteka = regB + regC + displacement_12bit;
        std::cout << "Adresa sa koje citamo status: " << std::hex << adresaSaSteka << std::endl;
        std::cout << "Memorija sta je ovo: " << this->dohvatiSadrzajIzMemorijeSa32bitAdrese(adresaSaSteka) << std::endl;

        this->upisiUCSRRegistar(regABroj, this->dohvatiSadrzajIzMemorijeSa32bitAdrese(adresaSaSteka));
      }

      break;
    }


    case OPKodovi::STORE_INSTR:
    {
      if(modifikatorInstrukcije == Modifikatori::STORE_U_MEMORIJU_LITERAL_12bit_ili_REG){
        // Radi se store mem[literal] = reg, gde je literal sirine 12 bita ;  obradiInstrukcijuStLiteralMem
        // Ili moze mem[reg] = reg ;                                          obradiInstrukcijuStGPRMem
        // Ili moze mem[reg + offset] = reg ;                                 obradiInstrukcijuStGPRMemOffsLiteral
        // obradiInstrukcijuStSimbolMem - kada je simbol u istoj sekciji kao i st instrukcija
        this->upisi32BitPodatakUMemoriju(regA + regB + displacement_12bit, regC);

        std::cout << "Adresa upisa: " << std::hex << regA + regB + displacement_12bit << std::endl;
        std::cout << "Upisana vrednost: " << std::hex << regC << std::endl;
      }

      else if(modifikatorInstrukcije == Modifikatori::STORE_MEM_MEM){
        // Radi se store  mem[ mem[literal]] = reg ;    obradiInstrukcijuStLiteralMem
        // Tj. literal je veci od 12 bita, pa je u bazenu, pa zato mora indirektno da se dohvati prvo
        // obradiInstrukcijuStSimbolMem - kada simbol nije u istoj sekciji kao st instrukcija, pa postoji relok. zapis
        int adr = this->dohvatiSadrzajIzMemorijeSa32bitAdrese(regA + regB + displacement_12bit);
        this->upisi32BitPodatakUMemoriju(adr, regC);

        std::cout << "PC: " << std::hex << regA << ", offset: " << std::hex << displacement_12bit << std::endl;
        std::cout << "Simbol je na adresi (u bazenu): " << std::hex << regA + regB + displacement_12bit << std::endl;
        std::cout << "Sadrzaj u bazenu - vrednost simbola: " << std::hex << adr << std::endl;
        std::cout << "mem[" << std::hex << adr << "] = " << std::hex << regC << std::endl;
      }

      else if(modifikatorInstrukcije == Modifikatori::PUSH_NA_STEK){

        // RegA ce u ovom slucaju biti SP registar tj. R14
        // Iako treba da se radi gpr + D, posto meni D treba da bude -4 (a ne 4 kako je u kodu) onda ovde oduzimam
        // Mislim da ovo nije problem jer nece nikad onaj ko pise kod da ukoduje da je D=-4, vec ce on samo reci STA ide na stek, a ja to realizujem
        
        // obradiInstrukcijuPush
        
        int adr = regA - displacement_12bit;
        this->upisi32BitPodatakUMemoriju(adr, regC);    // Upisemo podatak na stek
        this->upisiUGPRRegistar(Registri::REGISTAR_SP, regA - displacement_12bit);    // Azuriramo SP registar
      }

      break;
    }
      
  }

}


void Emulator::podaciOInstrukciji(uint8_t opKod, uint8_t mod, uint8_t regANum, uint8_t regBNum, uint8_t regCNum, int disp){

  std::cout << std::endl << "Obradjuje se instrukcija sa sledecim informacijama: " << std::endl;
  
  std::cout << "OP kod: ";
  if (opKod < 0x10) { std::cout << "0" << std::hex << static_cast<int>(opKod) << " "; }
  else { std::cout << std::hex << static_cast<int>(opKod) << " "; }

  std::cout << std::endl << "Mod: ";
  if (mod < 0x10) { std::cout << "0" << std::hex << static_cast<int>(mod) << " "; }
  else { std::cout << std::hex << static_cast<int>(mod) << " "; }

  std::cout << std::endl << "RegA[broj]:";
  if (regANum < 0x10) { std::cout << "0" << std::hex << static_cast<int>(regANum) << " "; }
  else { std::cout << std::hex << static_cast<int>(regANum) << " "; }

  std::cout << std::endl << "RegB[broj]:";
  if (regBNum < 0x10) { std::cout << "0" << std::hex << static_cast<int>(regBNum) << " "; }
  else { std::cout << std::hex << static_cast<int>(regBNum) << " "; }

  std::cout << std::endl << "RegC[broj]:";
  if (regCNum < 0x10) { std::cout << "0" << std::hex << static_cast<int>(regCNum) << " "; }
  else { std::cout << std::hex << static_cast<int>(regCNum) << " "; }

  std::cout << std::endl << "Displ: " << std::dec << disp << std::endl << std::endl;

}


void Emulator::ispisiStanjeNakonEmulacije(){

  const char separator = ' ';
  const int sirinaKaraktera = 8;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "Emulated processor executed halt instruction" << std::endl;
  std::cout << "Emulated processor state:" << std::endl;

  std::cout << "r0=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r0;
  std::cout << "   ";

  std::cout << "r1=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r1;
  std::cout << "   ";

  std::cout << "r2=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r2;
  std::cout << "   ";

  std::cout << "r3=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r3 << std::endl;

  std::cout << "r4=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r4;
  std::cout << "   ";

  std::cout << "r5=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r5;
  std::cout << "   ";

  std::cout << "r6=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r6;
  std::cout << "   ";

  std::cout << "r7=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r7 << std::endl;

  std::cout << "r8=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r8;
  std::cout << "   ";

  std::cout << "r9=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r9;
  std::cout << "   ";

  std::cout << "r10=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r10;
  std::cout << "  ";

  std::cout << "r11=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r11 << std::endl;

  std::cout << "r12=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r12;
  std::cout << "  ";

  std::cout << "r13=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r13;
  std::cout << "  ";

  std::cout << "r14=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r14;
  std::cout << "  ";

  std::cout << "r15=";
  std::cout << "0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << r15 << std::endl;
}


void Emulator::ispisiStanjeCSRRegistara(){
  const int sirinaKaraktera = 8;

  std::cout << "Stanje CSR registara:" << std::endl;
  std::cout << "status = 0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << status << std::endl;
  std::cout << "handler = 0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << handler << std::endl;
  std::cout << "cause = 0x" << std::setfill('0') << std::setw(sirinaKaraktera) << std::hex << cause << std::endl;
}



Emulator::Emulator(){
  r0 = 0; r1 = 0; r2 = 0; r3 = 0; r4 = 0; r5 = 0; r6 = 0; r7 = 0; r8 = 0; r9 = 0;
  r10 = 0; r11 = 0; r12 = 0; r13 = 0;
  r14 = 0xFFFFFF00;   // SP
  r15 = 0x40000000;   // PC

  status = 0;
  handler = 0;
  cause = 0;
}