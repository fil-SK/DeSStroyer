all: build

buildAssembler:
	bison -d misc/parser.y
	flex misc/lexer.l
	g++ src/zapists.cpp src/sekcija.cpp src/relokzapis.cpp parser.tab.c lex.yy.c src/assembler.cpp src/main.cpp -lfl -o asembler


buildLinker:
	g++ -g src/zapists.cpp src/sekcija.cpp src/relokzapis.cpp src/linker.cpp src/mainLinker.cpp -o linker


buildEmulator:
	g++ -g src/emulator.cpp src/mainEmulator.cpp -o emulator


clean:
	rm izlaz
	rm parser.tab.c
	rm parser.tab.h
	rm lex.yy.c

run:
	./izlaz


testInstr:
	./asembler -o testEmu1.o testEmu1.s
	./asembler -o testEmu2.o testEmu2.s
	./linker -hex testEmu1.o testEmu2.o -place=prva@0x40000000 -place=druga@0xF0000000 -o testInstr.hex
	./emulator testInstr.hex
	

proveraInstrukcija:
	./asembler -o instrukcije.o testInstrukcije.s
	./linker instrukcije.o -place=prva@0x40000000 -place=druga@0x30000 -place=treca@0x6 -o instrukcije.hex -hex
	./emulator instrukcije.hex


proveraInstrukcijaSimbol:
	./asembler -o simb2.o testSimb2Sekc.s
	./linker -hex simb2.o -o simb2.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator simb2.hex


proveraInstrukcijaBEQ:
	./asembler -o beqSimbTest.o testBEQSimbol.s
	./linker -hex beqSimbTest.o -o beqSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator beqSimbTest.hex


proveraInstrukcijaBNE:
	./asembler -o bneSimbTest.o testBNESimbol.s
	./linker -hex bneSimbTest.o -o bneSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator bneSimbTest.hex


proveraInstrukcijaBGT:
	./asembler -o bgtSimbTest.o testBGTSimbol.s
	./linker -hex bgtSimbTest.o -o bgtSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator bgtSimbTest.hex


proveraInstrukcijaJMP:
	./asembler -o jmpSimbTest.o testJMPSimbol.s
	./linker -hex jmpSimbTest.o -o jmpSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator jmpSimbTest.hex


proveraInstrukcijaLD:
	./asembler -o ldSimbTest.o testLDSimbol.s
	./linker -hex ldSimbTest.o -o ldSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator ldSimbTest.hex


proveraInstrukcijaLDMEM:
	./asembler -o ldMEMSimbTest.o testLDSimbolMEM.s
	./linker -hex ldMEMSimbTest.o -o ldMEMSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator ldMEMSimbTest.hex


proveraInstrukcijaSTMEM:
	./asembler -o stMEMSimbTest.o testSTSimbolMEM.s
	./linker -hex stMEMSimbTest.o -o stMEMSimbTest.hex -place=prva@0x40000000 -place=druga@0x41000000
	./emulator stMEMSimbTest.hex

proveraCALL:
	./asembler -o testCall.o testInstrukcijeCALLiRET.s
	./linker -hex testCall.o -o testCall.hex -place=prva@0x40000000 -place=druga@0x30000
	./emulator testCall.hex


nivoAtest:
	./asembler -o main.o main.s
	./asembler -o math.o math.s
	./asembler -o handler.o handler.s
	./asembler -o isr_timer.o isr_timer.s
	./asembler -o isr_terminal.o isr_terminal.s
	./asembler -o isr_software.o isr_software.s
	./linker -hex -place=my_code@0x40000000 -place=math@0xF0000000 -o program.hex handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o
	./emulator program.hex