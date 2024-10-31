/*

. CPU::Debug() pentru a afisa continutul registrilor.


. Cateva precizari despre instructiuni.

																	========LCR=========

=>  Se pot interschimba registri 0, 1 ... 15 intre ei | RA, M, A, R intre ei dar !NU! se poate face LCR Registru_numeric, (RA, M, A, R).
	Daca vrem, de pilda, sa interschimbam 2 si M, scriem in felul urmator:    (COD TESTAT)

	machine code pentru el:           0x03 0x02 0x04 0x11 0x13 0x06 0x02 0x02 0x02 0x02 0x13   ( 11 cicluri )

				LAX 2        Mutam in RA valoarea din registrul 2
				LCR RA, M    Interschimbam registrul M si RA
				SAX 2        Mutam in registrul 2 continutul registrului RA


																	========LAX========

=> LAX Load Auxiliar Register (RA)
=> Muta in Registrul Auxiliar (RA) valoarea continuta in RR, unde RR este un registru intre 0 si 15.

																	========SAX========
=> SAX Store Auxiliar Register (RA)
=> Muta valoare registrului RR in Registrul Auxiliar (RA), unde RR este un registru intre 0 si 15.

																	========LI=========

=> Load Integer
=> se poate genera o constantra in oricare registru in afara de WR si CR.

																	========LRZ========

=> Load Register Zero (LRZ)
=> Sintaxa LRZ k, unde 0 <= k <=15  | Reseteaza k+1 registri.

																	========LZ========
=> Load Zero
=> Sintaxa LZ RR, nn unde 0 < RR <= 15 si nn 0 < nn << 15
=> Reseteaza registru RR si inca nn registri.

																	========ARI=======

=> Add Register Integer
=> Sintaxa ARI RR, k unde RR este orice registru in afara de CR si WR

																	========AR========
=> Add Register
=> Sintaxa AR RA, RB unde 0 <= RA <= RB <= 15 sau RA, RB apartin { M, A, R, RA }

																	========FD=======

-> Genereaza K in WR
*/

#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "CPU.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "screen.h"
#include <string>


const int WIDTH  = 800;
const int HEIGHT = 600;


ev_queue scr;

int main(void) {

	Memory memorie;
	CPU cpu;
	cpu.Reset(memorie);

	const std::string fileName = "C:/Users/miaif/Desktop/cod/A5BAL8P101-C6502/assembler/output.bin";
	std::ifstream cod(fileName, std::ios::binary);

	std::vector<Byte> instr((std::istreambuf_iterator<char>(cod)),
		std::istreambuf_iterator<char>());
	int start = 0xFF0;

	for (const Byte& byte : instr) {
		memorie[start] = byte;
		start++;
	}

	//init grafice
	SDL_Window* window = SDL_CreateWindow("Olivetti: Assembleur a deux addresses.",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_SHOWN);
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Set the background color to #383840
	SDL_SetRenderDrawColor(renderer, 56, 56, 64, 255); 
	SDL_RenderClear(renderer);


	if (TTF_Init() < 0) {
		std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	
	TTF_Font* font = TTF_OpenFont("./olivettiScreen/VT323.ttf", 20);
	SDL_Color textColor = { 0, 255, 0, 255 };    // verde 

	std::string availavbleBytes = std::to_string(2000 - instr.size()) + " Octets libres.";     // spatiu ramas pentru alte instructiuni

	screen::renderText(renderer, availavbleBytes, font, textColor, 5, 2);
	SDL_RenderPresent(renderer);     // inchidem fontul pentru a putea schimba marimea lui (de la 20 la 28)
	SDL_Delay(16);

	TTF_CloseFont(font);

	font = TTF_OpenFont("./olivettiScreen/VT323.ttf", 28);

	cpu.renderer = renderer;
	cpu.textColor = textColor;
	cpu.font = font;

	std::thread cput([&]() {cpu.Exec(instr.size(), memorie); });
	
	bool running = true;
	SDL_Event e;
	while (running) {
		while (SDL_PollEvent(&e) != 0) {
			while (scr.handle(renderer));
			SDL_RenderPresent(renderer);
			if (e.type == SDL_QUIT) {
				running = false;
				std::exit(EXIT_SUCCESS);             // altfel arunca un debug error
			}
			/*else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_SPACE) {
					CPU::LED_ER = !CPU::LED_ER;
				}
			}*/
		} 

		//   primul rand
		screen::renderRedLED(renderer, 3.5, 7.5, "C.U.", 20, CPU::LED_ER, 230, 50);         // EROARE
		screen::renderRedLED(renderer, 3.5, 7.5, "HLT", 20, CPU::LED_HLT, 283, 50);         // HLT
		screen::renderYellowLED(renderer, 4, 7.5, "KES", 20, CPU::LED_KES, 336, 50);           // KES 
		screen::renderYellowLED(renderer, 4, 7.5, "KEP", 20, CPU::LED_KEP, 389, 50);           // KEP
		screen::renderYellowLED(renderer, 3, 10, "KEYB", 16, CPU::LED_KEYB, 442, 50);          // KEYB
		screen::renderYellowTwoLED(renderer, 3.5, 4.5, "MAGN", "CARD", 12, CPU::LED_MAGNCARD, 495, 50);    // magnetic card
		
		//  al doilea rand
		screen::renderWhiteLED (renderer, 17, -8,   "1", 40,	  CPU::LED_1, 150, 150);
		screen::renderWhiteLED (renderer, 12, -8,   "2", 40,      CPU::LED_2, 203, 150);
		screen::renderWhiteLED (renderer, 10, -8,   "3", 40,      CPU::LED_3, 256, 150);
		screen::renderWhiteLED (renderer, 10, -8,   "4", 40,      CPU::LED_4, 309, 150);
		screen::renderWhiteLED (renderer, 10, -8,   "5", 40,      CPU::LED_5, 362, 150);
		screen::renderWhiteLED (renderer, 10, -8,   "6", 40,      CPU::LED_6, 415, 150);
		screen::renderWhiteLED (renderer, 13, -8,   "7", 40,      CPU::LED_7, 468, 150);
		screen::renderWhiteLED (renderer, 11.5, -8, "8", 40,      CPU::LED_8, 521, 150);
		screen::renderGreenLED( renderer, 573, 150,				  CPU::LED_END        );
	}

	cput.join();

	TTF_CloseFont(font);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();

	//cpu.Debug();
	
	
	
	
	printf("\n");
	return 0;
}


