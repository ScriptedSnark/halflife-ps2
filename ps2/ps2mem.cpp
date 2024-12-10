#include "ps2mem.h"

void makeJal(unsigned int addr, void* func)
{
	*(int*)addr = ((0x0C000000 | (((unsigned int)func & 0x0fffffff) >> 2)));
}

void patchInt(unsigned int addr, int data)
{
	*(int*)(addr) = data;
}

void makeJmp(unsigned int addr, void* func)
{
	*(int*)addr = (0x08000000 | (((unsigned int)(func) & 0x0FFFFFFC) >> 2));
}

void NOP(unsigned int addr)
{
	patchInt(addr, 0);
}

void patchShort(unsigned int addr, short data)
{
	*(short*)(addr) = data;
}