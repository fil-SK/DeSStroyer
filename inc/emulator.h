#ifndef _emulator_h_
#define _emulator_h_

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

class Emulator{

  public:
    
    // Funkcije za rad
    void procitajUlazniFajl(std::string nazivFajla);
    void ispisiSadrzajMemorije();
    void pokreniIzvrsavanje();
    void ucitaj4BIzMemorije(uint8_t& bajt31_24, uint8_t& bajt23_16, uint8_t& bajt15_8, uint8_t& bajt7_0);
    int dohvatiSadrzajIzMemorijeSa32bitAdrese(uint32_t adresa);
    void upisi32BitPodatakUMemoriju(uint32_t adresa, int vrednost);
    void obradiInstrukciju(uint8_t bajt31_24, uint8_t bajt23_16, uint8_t bajt15_8, uint8_t bajt7_0);
    void podaciOInstrukciji(uint8_t opKod, uint8_t mod, uint8_t regANum, uint8_t regBNum, uint8_t regCNum, int disp);
    void ispisiStanjeNakonEmulacije();
    void ispisiStanjeCSRRegistara();

    void pushGPR(int registar);
    void pushCSR(int registar);
    void popGPR(int registar);
    


    // Pomocne funkcije
    uint8_t konvertujStringBajtUHexaBroj(std::string hexaString);
    uint32_t konvertujStringAdresuUHexaBroj(std::string adresa);


    // Geteri i seteri
    int& dohvatiGPRRegistar(int brojRegistra);
    int& dohvatiCSRRegistar(int brojRegistra);
    int dohvatiPC();
    int dohvatiSP();

    int upisiUGPRRegistar(int brojRegistra, int vrednost);
    int upisiUCSRRegistar(int brojRegistra, int vrednost);
    void pomeriPCNaNarednuInstrukciju();
    void upisiUMemoriju(uint32_t adresa, uint8_t podatak);

    Emulator();

  private:

    // Memorija
    std::map<uint32_t, uint8_t> memorija;

    // Registri
    int r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;   // GPR registri
    int status, handler, cause;                                                 // CSR registri
};

#endif