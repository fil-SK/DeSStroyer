
%{
  #include <cstdio>
  #include <iostream>
  #include <cstdlib>
  #include <cstring>
  #include <string>

  
  #include "inc/zapists.h"
  #include "inc/assembler.h"

  extern int yylex();
  extern int yyparse();
  extern FILE* yyin;

  extern int main();
  extern Assembler asembler;

  void yyerror(const char* s);


  int flagGlobalExtern = -1;    // 0 - global, 1 - extern; Flag koji prati da li se trenutno obradjuje direktiva global ili extern. Nakon obrade, ovaj flag ce biti resetovan na -1
  int locationCounter = 0;


%}



%union{
  int numValue;
  char* stringValue;
}


/* Definisanje terminalnih simbola */
%token ZAREZ DVE_TACKE DOLAR PROCENAT LEVA_UGLASTA DESNA_UGLASTA PLUS

%token DIR_GLOBAL DIR_EXTERN DIR_SECTION DIR_WORD DIR_SKIP DIR_END
%token INSTR_HALT INSTR_INT INSTR_IRET INSTR_CALL INSTR_RET
%token INSTR_JMP INSTR_BEQ INSTR_BNE INSTR_BGT
%token INSTR_PUSH INSTR_POP
%token INSTR_XCHG INSTR_ADD INSTR_SUB INSTR_MUL INSTR_DIV
%token INSTR_NOT INSTR_AND INSTR_OR INSTR_XOR INSTR_SHL INSTR_SHR
%token INSTR_LD INSTR_ST
%token INSTR_CSRRD INSTR_CSRWR

/* Definisanje terminalnih simbola za koje uzimam vrednost */
%token <stringValue>  GPRX PROCENAT_CSRX IDENT
%token <numValue>     HEX_BROJ DEC_BROJ


/* Definisanje neterminalnih simbola */
%type <stringValue> labela nazivSekcije simbol
%type <numValue>    literal

%%

/* Gramatika asemblera */

ulazniFajl      : listaAsmLinija
                ;


listaAsmLinija  : %empty
                | listaAsmLinija asmLinija
                ;


asmLinija       : asmDirektiva
                | asmInstrukcija
                ;


asmDirektiva    : labela direktiva
                ;

direktiva       : DIR_GLOBAL {flagGlobalExtern = 0;} listaSimbola { flagGlobalExtern = -1; }

                | DIR_EXTERN {flagGlobalExtern = 1;} listaSimbola { flagGlobalExtern = -1; }

                | DIR_SECTION nazivSekcije
                  {
                    if(asembler.obradaDirektiveSection($2) != 0){
                      YYABORT;   // Prekini yyparse metodu
                    }
                  }

                | DIR_WORD listaSimbolaIliLiterala { std::cout << ".word" << std::endl; }

                | DIR_SKIP literal  { asembler.obradaDirektiveSkip($2); }

                | DIR_END 
                  { // Zavrsava asembliranje ulaznog fajla
                    std::cout << "END direktiva" << std::endl;
                    asembler.obradaDirektiveEnd();
                    YYACCEPT;
                  }
                ;


listaSimbola    : simbol
                  {
                    if(asembler.obradaDirektivaGlobalExtern($1, flagGlobalExtern) != 0){
                      YYABORT;   // Prekini yyparse metodu
                    }
                  }

                | listaSimbola ZAREZ simbol
                  {
                    if(asembler.obradaDirektivaGlobalExtern($3, flagGlobalExtern) != 0){
                      YYABORT;   // Prekini yyparse metodu
                    }
                  }
                ;              


nazivSekcije  : IDENT { $$ = $1; std::cout << "Naziv sekcije je " << $1 << std::endl;  }
              ;


/* Koristi se samo kod word direktive */
listaSimbolaIliLiterala : simbol    { asembler.obradaDirektiveWordSimbol($1); }

                        | literal   { asembler.obradaDirektiveWordLiteral($1); }

                        | listaSimbolaIliLiterala ZAREZ simbol    { asembler.obradaDirektiveWordSimbol($3); }

                        | listaSimbolaIliLiterala ZAREZ literal   { asembler.obradaDirektiveWordLiteral($3); }
                        ;                                


asmInstrukcija  : labela instrukcija
                ;


labela        : %empty
              | IDENT DVE_TACKE
                {
                  $$ = $1;    // Ovim smo neterminalu labela dodelili parsiranu vrednost

                  if(asembler.obradaLabele($1) != 0){
                    YYABORT;   // Prekini yyparse metodu tj. prekini parsiranje
                  }

                  std::cout << "Naziv labele je " << $1 << std::endl;

                }
              ;


instrukcija : INSTR_HALT                 { asembler.obradiInstrukcijuHalt(); }

            | INSTR_INT                  { asembler.obradiInstrukcijuInt(); }

            | INSTR_IRET                 { asembler.obradiInstrukcijuIret(); }

            | INSTR_CALL literal         { asembler.obradiInstrukcijuCallLiteral($2); }

            | INSTR_CALL simbol          { asembler.obradiInstrukcijuCallSimbol($2); }

            | INSTR_RET                  { asembler.obradiInstrukcijuRet(); }

            | INSTR_JMP literal          { asembler.obradiInstrukcijuJmpLiteral($2); }

            | INSTR_JMP simbol           { asembler.obradiInstrukcijuJmpSimbol($2); }

            | INSTR_BEQ PROCENAT GPRX ZAREZ PROCENAT GPRX ZAREZ literal   { asembler.obradiInstrukcijuBeqLiteral($3, $6, $8); }

            | INSTR_BEQ PROCENAT GPRX ZAREZ PROCENAT GPRX ZAREZ simbol    { asembler.obradiInstrukcijuBeqSimbol($3, $6, $8); }

            | INSTR_BNE PROCENAT GPRX ZAREZ PROCENAT GPRX ZAREZ literal   { asembler.obradiInstrukcijuBneLiteral($3, $6, $8); }

            | INSTR_BNE PROCENAT GPRX ZAREZ PROCENAT GPRX ZAREZ simbol    { asembler.obradiInstrukcijuBneSimbol($3, $6, $8); }

            | INSTR_BGT PROCENAT GPRX ZAREZ PROCENAT GPRX ZAREZ literal   { asembler.obradiInstrukcijuBgtLiteral($3, $6, $8); }

            | INSTR_BGT PROCENAT GPRX ZAREZ PROCENAT GPRX ZAREZ simbol    { asembler.obradiInstrukcijuBgtSimbol($3, $6, $8); }

            | INSTR_PUSH PROCENAT GPRX                          { asembler.obradiInstrukcijuPush($3); }

            | INSTR_POP PROCENAT GPRX                           { asembler.obradiInstrukcijuPop($3); }

            | INSTR_XCHG PROCENAT GPRX ZAREZ PROCENAT GPRX      { asembler.obradiInstrukcijuXchg($3, $6); }

            | INSTR_ADD PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuAdd($3, $6); }

            | INSTR_SUB PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuSub($3, $6); }

            | INSTR_MUL PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuMul($3, $6); }

            | INSTR_DIV PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuDiv($3, $6); }

            | INSTR_NOT PROCENAT GPRX                           { asembler.obradiInstrukcijuNot($3); }

            | INSTR_AND PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuAnd($3, $6); }

            | INSTR_OR PROCENAT GPRX ZAREZ PROCENAT GPRX        { asembler.obradiInstrukcijuOr($3, $6); }

            | INSTR_XOR PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuXor($3, $6); }

            | INSTR_SHL PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuShl($3, $6); }

            | INSTR_SHR PROCENAT GPRX ZAREZ PROCENAT GPRX       { asembler.obradiInstrukcijuShr($3, $6); }



            | INSTR_LD DOLAR literal ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdLiteralVrednost($3, $6); }

            | INSTR_LD DOLAR simbol ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdSimbolVrednost($3, $6); }

            | INSTR_LD literal ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdLiteralMem($2, $5); }

            | INSTR_LD simbol ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdSimbolMem($2, $5); }

            | INSTR_LD PROCENAT GPRX ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdGPRVrednost($3, $6); }

            | INSTR_LD LEVA_UGLASTA PROCENAT GPRX DESNA_UGLASTA ZAREZ PROCENAT GPRX   { asembler.obradiInstrukcijuLdGPRMem($4, $8); }

            | INSTR_LD LEVA_UGLASTA PROCENAT GPRX PLUS literal DESNA_UGLASTA ZAREZ PROCENAT GPRX
              {
                if(asembler.obradiInstrukcijuLdGPRMemOffsLiteral($4, $6, $10) == -4){
                  std::cout << "Literal se ne moze zapisati na 12 bita!" << std::endl;
                  YYABORT;
                }
              }



            | INSTR_ST PROCENAT GPRX ZAREZ literal   { asembler.obradiInstrukcijuStLiteralMem($3, $5); }

            | INSTR_ST PROCENAT GPRX ZAREZ simbol   { asembler.obradiInstrukcijuStSimbolMem($3, $5); }

            | INSTR_ST PROCENAT GPRX ZAREZ LEVA_UGLASTA PROCENAT GPRX DESNA_UGLASTA   { asembler.obradiInstrukcijuStGPRMem($3, $7); }

            | INSTR_ST PROCENAT GPRX ZAREZ LEVA_UGLASTA PROCENAT GPRX PLUS literal DESNA_UGLASTA
              {
                if(asembler.obradiInstrukcijuStGPRMemOffsLiteral($3, $7, $9) == -4){
                  std::cout << "Literal se ne moze zapisati na 12 bita!" << std::endl;
                  YYABORT;
                }
              }



            | INSTR_CSRRD PROCENAT_CSRX ZAREZ PROCENAT GPRX     { asembler.obradiInstrukcijuCsrRd($2, $5); }

            | INSTR_CSRWR PROCENAT GPRX ZAREZ PROCENAT_CSRX     { asembler.obradiInstrukcijuCsrWr($3, $5); }
            ;               


simbol  : IDENT
          {
            $$ = $1;
            std::cout << "Simbol je " << $1 << std::endl;
          }
        ;

literal : HEX_BROJ  { $$ = $1; std::cout << "Literal je " << $1 << std::endl; }
        | DEC_BROJ  { $$ = $1; }
        ;                    


%%





void yyerror(const char* s){
  printf("Greska u parsiranju, %s.", s);
  exit(1);
}
