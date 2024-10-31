#pragma once

using Byte = unsigned char;   // 8 biti
using Word = unsigned short;  // 16 biti

struct Memory {

	static constexpr unsigned int MAX_MEM = 1024 * 64;
	Byte* Data;

	Memory();
	~Memory();

	Byte operator[] (unsigned int Adresa) const
	{
		return Data[Adresa];
	}
	Byte& operator[] (unsigned int Adresa)
	{
		return Data[Adresa];
	}
};