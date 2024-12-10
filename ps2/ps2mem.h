#pragma once

#define LOWORD(l)           ((uint16_t)((*(uint32_t*)(&l)) & 0xffff))
#define HIWORD(l)           ((uint16_t)(((*(uint32_t*)(&l)) >> 16) & 0xffff))

void makeJal(unsigned int addr, void* func);
void patchInt(unsigned int addr, int data);
void makeJmp(unsigned int addr, void* func);
void NOP(unsigned int addr);
void patchShort(unsigned int addr, short data);