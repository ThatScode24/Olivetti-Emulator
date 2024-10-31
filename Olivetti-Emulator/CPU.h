#pragma once
#include "memory.h"
#include <SDL.h>
#include <mutex>
#include <condition_variable>
#include <SDL_ttf.h>
#include <thread>
#include <queue>
#include "screen.h"
#include <variant>


using Byte = unsigned char;   // 8 biti
using Word = unsigned short;  // 16 biti

struct CPU {
	//registri

	Word PC;  // PC
	Word SP; // stack pointer
	Byte CR; // registrul de flaguri
	Byte WR; // Work Register
	Byte RA; // Registru Auxiliar

	Byte M, A, R; // Registri I/O 

	Byte R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7, R_8, R_9, R_10, R_11, R_12, R_13, R_14, R_15; // cei 16 registri

	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Color textColor;
	 
	int x, y;          // coordonate pentru fereastra
	std::mutex haltMutex;
	std::condition_variable haltCondVar;
	bool isHalted;
	bool haltAcknowledged;

	// leduri
	
	static bool LED_ER;
	static bool LED_HLT;
	static bool LED_KEP;
	static bool LED_KES;
	static bool LED_KEYB;
	static bool LED_MAGNCARD;
	static bool LED_END;

	static bool LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8;
	
	//Flags 

	Byte C : 1;   // Carry Flag - overflow sau underflow   INOP
	Byte Z : 1;   // Zero Flag  
	Byte V : 1;   //Overflow Flag                          INOP
	Byte N : 1;   // Negative Flag                         INOP

	//opcodes instructiuni 
	enum Instructions {
		INSTRUCTION_LI =  0x00, //      3 cycles
		INSTRUCTION_RMC = 0x01,  //     4 cycles
		INSTRUCTION_PR =  0x02,  //     2 cycles 
		INSTRUCTION_LAX = 0x03,  //     2 cycles 
		INSTRUCTION_LCR = 0x04,   //    3 cycles
		INSTRUCTION_LR =  0x05,   //    3 cycles
		INSTRUCTION_SAX = 0x06,   //    2 cycles
		INSTRUCTION_LRZ = 0x07,    //   2 cycles
		INSTRUCTION_LZ =  0x08,     //  3 cycles 
		INSTRUCTION_ARI = 0x09,     //  3 cycles 
		INSTRUCTION_AR =  0x0A,     //  3 cycles 
		INSTRUCTION_SRI = 0x0B,     //  3 cycles
		INSTRUCTION_SR =  0x0C,     //  3 cycles  
		INSTRUCTION_FD =  0x0D,      // 2 cycles
		INSTRUCTION_KAC = 0x0E,      // 2 cycles ??
		INSTRUCTION_KEP = 0x0F,      // 1 cycle
		INSTRUCTION_KES = 0x10,   	 // 1 cycle    
		INSTRUCTION_HLT = 0x11,      // 1 cycle
		INSTRUCTION_ON =  0x12,      // 2 cycles 
		INSTRUCTION_END = 0x13,      // 1 cycle
		INSTRUCTION_OFF = 0x14       // 2 cycles
	};

	//opcodes registri
	enum Registers {
		REG_0 =  0x00,
		REG_1 =  0x01,
		REG_2 =  0x02,
		REG_3 =  0x03,
		REG_4 =  0x04,
		REG_5 =  0x05,
		REG_6 =  0x06,
		REG_7 =  0x07,
		REG_8 =  0x08,
		REG_9 =  0x09,
		REG_10 = 0x0A,
		REG_11 = 0x0B,
		REG_12 = 0x0C,
		REG_13 = 0x0D,
		REG_14 = 0x0E,
		REG_15 = 0x0F,
		REG_WR = 0x10,
		REG_RA = 0x11,
		REG_CR = 0x12,
		REG_M =  0x13,
		REG_A =  0x14,
		REG_R =  0x15
	};

	int outputMode;

	void Reset(Memory& memorie);
	Byte Fetch(unsigned int& NumCycles, Memory& memorie);
	Byte Read(unsigned int& NumCycles, Byte Address, Memory& memorie);
	void SET_FLAGS_LI(Byte Register);
	void handleHalt();
	void Exec(unsigned int NumCycles, Memory& memorie);
	void Debug();
};


struct Rendertext {
	std::string text; TTF_Font* font; SDL_Color color; int x; int y;
};
   
using ev_cpu = std::variant<Rendertext>;

class ev_queue
{
	std::queue<ev_cpu> q;
	std::mutex mut;
public:
	void rendertext(std::string& text, TTF_Font* font, SDL_Color color, int x, int y)
	{
		mut.lock();
		q.emplace(Rendertext{text,font,color,x,y});
		mut.unlock();
	}

	size_t handle(SDL_Renderer* r)
	{
		mut.lock();
		if (q.size() == 0) {
			mut.unlock();
			return 0;
		}
		ev_cpu ev = q.front();
		q.pop();
		mut.unlock();
		switch (ev.index())
		{
			case 0:
				screen::renderText(r, std::get<Rendertext>(ev).text, std::get<Rendertext>(ev).font, std::get<Rendertext>(ev).color, std::get<Rendertext>(ev).x, std::get<Rendertext>(ev).y);
				break;
		default:
			break;
		}
		return q.size();
	}
};