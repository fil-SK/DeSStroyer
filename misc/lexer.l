
%{
  #include <iostream>
  #include <cstdio>
  
  #include "parser.tab.h"
%}

%option noyywrap

%%

[ \r\t]                     { /* Ignorisi beline */ }
#[^\n\r]*                   { /* Jednolinijski komentar, ignorisi do kraja reda */ }
[\n]                        {}

r([0-9]|1[0-5])             { yylval.stringValue = strdup(yytext);  return GPRX; }
(sp|pc)                     { yylval.stringValue = strdup(yytext);  return GPRX; }
(%status|%handler|%cause)   { yylval.stringValue = strdup(yytext);  return PROCENAT_CSRX; }


.global     { return DIR_GLOBAL; }
.extern     { return DIR_EXTERN; }
.section    { return DIR_SECTION; }
.word       { return DIR_WORD; }
.skip       { return DIR_SKIP; }
.end        { return DIR_END; }


\$          { return DOLAR; }
%           { return PROCENAT; }
,           { return ZAREZ; }
:           { return DVE_TACKE; }
\[          { return LEVA_UGLASTA; }
\]          { return DESNA_UGLASTA; }
\+          { return PLUS; }


halt        { return INSTR_HALT; }
int         { return INSTR_INT; }
iret        { return INSTR_IRET; }
call        { return INSTR_CALL; }
ret         { return INSTR_RET; }
jmp         { return INSTR_JMP; }
beq         { return INSTR_BEQ; }
bne         { return INSTR_BNE; }
bgt         { return INSTR_BGT; }
push        { return INSTR_PUSH; }
pop         { return INSTR_POP; }
xchg        { return INSTR_XCHG; }
add         { return INSTR_ADD; }
sub         { return INSTR_SUB; }
mul         { return INSTR_MUL; }
div         { return INSTR_DIV; }
not         { return INSTR_NOT; }
and         { return INSTR_AND; }
or          { return INSTR_OR; }
xor         { return INSTR_XOR; }
shl         { return INSTR_SHL; }
shr         { return INSTR_SHR; }
ld          { return INSTR_LD; }
st          { return INSTR_ST; }
csrrd       { return INSTR_CSRRD; }
csrwr       { return INSTR_CSRWR; }




[a-zA-Z_][a-zA-Z0-9_]*      { yylval.stringValue = strdup(yytext);  return IDENT; }
[0-9]+                      { yylval.numValue = atoi(yytext);       return DEC_BROJ; }
0[xX][0-9A-Fa-f]+           {
                              char* hexBroj = strdup(yytext + 2);     // Da preskocimo 0x na pocetku
                              yylval.numValue = static_cast<int>(strtol(hexBroj, nullptr, 16));
                              free(hexBroj);
                              return HEX_BROJ;
                            }

<<EOF>>                     { printf("EOF"); return 0; }
.                           { printf("Leksicka greska: neocekivan karakter!"); }


%%


