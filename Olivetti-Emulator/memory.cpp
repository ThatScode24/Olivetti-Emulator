#include "memory.h"
#include <stdlib.h>

Memory::Memory() {
	Data = (Byte*)calloc(MAX_MEM, sizeof(Byte));
}

Memory::~Memory() {
	free(Data);
}