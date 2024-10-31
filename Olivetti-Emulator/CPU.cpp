#include "CPU.h"
#include <stdio.h>
#include "screen.h"
#include <thread>
#include <string>
#include <atomic>

extern ev_queue scr;

bool CPU::LED_ER =       false;
bool CPU::LED_HLT =      false;
bool CPU::LED_KES =      false;
bool CPU::LED_KEYB =     false;

bool CPU::LED_1 =		 false; 
bool CPU::LED_2 =		 false;
bool CPU::LED_3 =		 false;
bool CPU::LED_4 =		 false;
bool CPU::LED_5 =        false;
bool CPU::LED_6=         false;
bool CPU::LED_7 =        false;
bool CPU::LED_8 =        false;

bool CPU::LED_END =      false;

// optiuni default deci trebuie sa aprindem ledurile
bool CPU::LED_MAGNCARD = !CPU::LED_KEYB;
bool CPU::LED_KEP =		 !CPU::LED_KES;


void CPU::Debug() {
	printf("                   =======================================Debug=======================================\n\n"
		"R_0: %d\nR_1: %d\nR_2: %d\nR_3: %d\nR_4: %d\nR_5: %d\nR_6: %d\nR_7: %d\n"
		"R_8: %d\nR_9: %d\nR_10: %d\nR_11: %d\nR_12: %d\nR_13: %d\nR_14: %d\nR_15: %d\n"
		"M: %d\nA: %d\nR: %d\nWR: %d\nRA: %d\nCR: %d\n",
		R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7, R_8, R_9,
		R_10, R_11, R_12, R_13, R_14, R_15, M, A, R, WR, RA, CR);
}



void CPU::handleHalt() {
	std::unique_lock<std::mutex> lock(haltMutex);
	isHalted = LED_HLT = true;
	char inter;

	printf("CPU halted.\n");

	do {
		scanf_s(" %c", &inter, 1);
	} while (inter != 'r');

	isHalted = LED_HLT = false;
	haltAcknowledged = true;
	haltCondVar.notify_one(); // Notify the main thread to resume
}


void CPU::Reset(Memory& memorie)      // Initializare
{
	PC = 0xFF0;   
	SP = 0x0100;

	// flaguri la 0
	C = Z = V = N;   // am scos decimal flag 

	// registri la 0
	R_0 = R_1 = R_2 = R_3 = R_4 = R_5 = R_6 = R_7 = R_8 = R_9 = R_10 = R_11 = R_12 = R_13 = R_14 = R_15 = 0;

	M = A = R = WR = RA = CR = 0;

	outputMode = 0;

	isHalted = LED_HLT = false;
	haltAcknowledged = false;	

	x = 300;
	y = 250;

}

Byte CPU::Fetch(unsigned int& NumCycles, Memory& memorie)  // luam urmatoarea instrucitune din memorie
{
	Byte Data = memorie[PC];
	PC++;  // incrementam adresa in registru PC
	NumCycles--;
	return Data;
}

Byte CPU::Read(unsigned int& NumCycles, Byte Address, Memory& memorie)  // luam urmatoarea instrucitune din memorie
{
	Byte Data = memorie[Address];
	NumCycles--;
	return Data;
}

void CPU::SET_FLAGS_LI(Byte Register) {
	Z = (Register == 0);
	N = (Register & 0b10000000) > 0;
}

void CPU::Exec(unsigned int NumCycles, Memory& memorie)
{
	while (NumCycles > 0)
	{
		Byte Instruction = Fetch(NumCycles, memorie);   // determinam instructiunea urmatoare (instructiune = 1byte)

		Byte* Regs[] = { &R_0, &R_1, &R_2, &R_3, &R_4, &R_5, &R_6, &R_7, &R_8, &R_9, &R_10, &R_11, &R_12, &R_13, &R_14, &R_15, &WR, &RA, &CR, &M, &A, &R };
		bool* Leds[] = { &LED_ER, &LED_1, &LED_2, &LED_3, &LED_4, &LED_5, &LED_6, &LED_7, &LED_8 };

		printf("Executing...%d\n", NumCycles);
		switch (Instruction) {

		case INSTRUCTION_LI:                           //        Genera la constante k nel registro RR
		{
			Byte Register = Fetch(NumCycles, memorie);   // registrul cu care vom lucra
			Byte Constant = Fetch(NumCycles, memorie);  // constanta de care avem nevoie

			*Regs[Register] = Constant;
			SET_FLAGS_LI(Register);
		} break;

		case INSTRUCTION_RMC:                          //       Load into RR value at 0x....
		{
			// functioneaza doar pentru M, A, R si 0...15
			Byte Register = Fetch(NumCycles, memorie);
			Byte Adresa = Fetch(NumCycles, memorie);
			Byte Valoare = Read(NumCycles, Adresa, memorie);

			if ((Register >= 0 && Register <= 15) || (Register == 0x13 || Register == 0x14 || Register == 0x15)) {
				*Regs[Register] = Valoare; SET_FLAGS_LI(Register);
			}
		} break;

		case INSTRUCTION_PR:                           //        Stampa decimale di Registro RR.
		{											  //      putem printa continutul oricarui registru	 
			Byte Register = Fetch(NumCycles, memorie); 
			if (!outputMode) {
				std::string final(1, static_cast<char>(*Regs[Register]));
				scr.rendertext(final, font, textColor, x, y);
				x += 12;
			} else {
				std::string outputMethod = "Output is on secondary screen (KES).";
				scr.rendertext(outputMethod, font, textColor, 210, 250);
			}
		} break;

		case INSTRUCTION_LAX:                         //        Trasferisce dal Registro RR (numerico) al Registro Ausiliario.
		{
			Byte Register = Fetch(NumCycles, memorie);
			RA = *Regs[Register]; SET_FLAGS_LI(Register);    // setem RR = Aux reg 
		} break;

		case INSTRUCTION_LCR:                         //         Scambia il contenuto di due registri.
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			if ((Register1 >= 0 && Register1 <= 15 && Register2 >= 0 && Register2 <= 15) ||
				((Register1 == 0x11 || Register1 == 0x15 || Register1 == 0x14 || Register1 == 0x13) &&
					(Register2 == 0x11 || Register2 == 0x15 || Register2 == 0x14 || Register2 == 0x13))) {
				Byte aux = *Regs[Register1];
				*Regs[Register1] = *Regs[Register2];
				*Regs[Register2] = aux;
			}
		} break;

		case INSTRUCTION_LR:                        //           Transferice, nel Registro RA, il Registro RB  (MOV)
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			*Regs[Register1] = *Regs[Register2];
		} break;

		case INSTRUCTION_SAX:                      //              Trasferisce dal Registro Ausiliario al Registro RR (numerico).
		{
			Byte Register = Fetch(NumCycles, memorie);
			*Regs[Register] = RA; SET_FLAGS_LI(Register);    // setem RR = Aux reg 
		} break;

		case INSTRUCTION_LRZ:                     //               Azzera nn + 1 registri.
		{
			Byte NumRegisters = Fetch(NumCycles, memorie);
			for (int i = 0; i <= NumRegisters; i++) *Regs[i] = 0;    //  Reseteaza NumRegisters+1 registri.
			Z = 0;                                                   //  Setam flagul Zero
		} break;                                                     // trebuie adaugate securitati aici 

		case INSTRUCTION_LZ:
		{
			Byte Register = Fetch(NumCycles, memorie);
			Byte Urmatori = Fetch(NumCycles, memorie);

			for (int i = Register; i <= Register + Urmatori; i++) {
				*Regs[i] = 0;
				if (i >= 15) break;
			}
		} break;

		case INSTRUCTION_ARI:                    //                 Addiziona k a RR (Risultato in RR)
		{
			Byte Register = Fetch(NumCycles, memorie);
			Byte Constant = Fetch(NumCycles, memorie);
			*Regs[Register] += Constant;
		} break;

		case INSTRUCTION_AR:
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			*Regs[Register1] += *Regs[Register2];
		} break;

		case INSTRUCTION_SRI:      // se pare ca exista probleme de underflow. (e normal, am definite Byte = unsigned char)
		{
			Byte Register = Fetch(NumCycles, memorie);
			Byte Integer = Fetch(NumCycles, memorie);
			*Regs[Register] -= Integer;
		} break;

		case INSTRUCTION_SR:
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			*Regs[Register1] -= *Regs[Register2];
		} break;

		case INSTRUCTION_FD:
		{
			Byte Constante = Fetch(NumCycles, memorie);
			WR = Constante;
		} break;
		case INSTRUCTION_KAC:
		{
			Byte NumeroPointer = Fetch(NumCycles, memorie);	 // aici o sa se implementeze logica cu cei 2 pointeri
			if (NumeroPointer == 1) {
				char alfanumerico;
				scanf_s("%c", &alfanumerico, 1);
				WR = alfanumerico;
			}
			else {
				printf("De implementat scrierea in memoria pointata de catre pointer 2.\n");
			}
		} break;

		case INSTRUCTION_HLT:
		{
			// Start the halt thread
			std::thread haltThread([&]() {handleHalt(); });

			// Main thread waits until halt is acknowledged, but keeps UI responsive
			std::unique_lock<std::mutex> lock(haltMutex);
			haltCondVar.wait(lock, [&]() { return haltAcknowledged; });

			haltThread.join();  // Ensure haltThread completes before proceeding
			haltAcknowledged = false; // Reset for future halts if necessary
		} break;

		case INSTRUCTION_ON:
		{
			Byte Led = Fetch(NumCycles, memorie);
			*Leds[Led] = true;
		} break;

		case INSTRUCTION_OFF:
		{
			Byte Led = Fetch(NumCycles, memorie);
			*Leds[Led] = false;
		} break;

		case INSTRUCTION_KEP: outputMode = LED_KES =0; LED_KEP = 1; ; break;

		case INSTRUCTION_KES: outputMode = LED_KES =1 ; LED_KEP = 0 ; break;

		case INSTRUCTION_END: LED_END = 1; break;

		default: printf("Instruction %d not handled.", Instruction); LED_ER = true; //   aprindem si ledul de eroare
		}
	}
};