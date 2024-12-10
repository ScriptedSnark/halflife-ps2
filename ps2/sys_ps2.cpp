/*
***********************************
*                                 *
* Half-Life SDK for Playstation 2 *
*           SLUS-20066            *
*                                 *
***********************************
*/

#include "types.h"
#include "ps2mem.h"
#include "mips.h"

#include "hud.h"

// client.dll "exports"
//=======================================================================
typedef int (*_Initialize)(cl_enginefunc_t* pEnginefuncs, int iVersion);
typedef int (*_HUD_VidInit)(int player);
typedef int (*_HUD_Init)(int player);
typedef int (*_HUD_Redraw)(float flTime, int intermission, int player);
typedef int (*_HUD_UpdateClientData)(client_data_t* cdata, int player);

extern "C"
{
	int Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion);
	int HUD_VidInit(int player);
	int HUD_Init(int player);
	int HUD_Redraw(float flTime, int intermission, int player);
	int HUD_UpdateClientData(client_data_t* cdata, int player);
}
//=======================================================================

int Sys_rand()
{
	return ((int(*)())0x2E5340)();
}

void Sys_sprintf(char* buffer, const char* format, ...)
{
	((void(*)(char*, const char*, ...))0x2E5880)(buffer, format);
}

void* Q_malloc(int size)
{
	return ((void*(*)(int))0x225C58)(size);
}

void Q_free(void* ptr)
{
	((void (*)(void*))0x2261A8)(ptr);
}

/************************************************************************/
/* Sys_ReplaceClient
/************************************************************************/
void Sys_ReplaceClient()
{
	*(_Initialize*)0x46E4F8 = Initialize;
	*(_HUD_Init*)0x46E4FC = HUD_Init;
	*(_HUD_VidInit*)0x46E500 = HUD_VidInit;
	*(_HUD_Redraw*)0x46E504 = HUD_Redraw;
	*(_HUD_UpdateClientData*)0x46E508 = HUD_UpdateClientData;

	*(_HUD_VidInit*)0x46E50C = HUD_VidInit; // there is another VidInit pointer (xref -> 0x00247498, unused?)
}

typedef void (*_HookUserMsg)(void* a1, void* a2, void* a3);
_HookUserMsg ORIG_HookUserMsg = NULL;

void HOOKED_HookUserMsg(void* a1, void* a2, void* a3)
{
	ORIG_HookUserMsg(a1, a2, a3);
	Sys_ReplaceClient();
}

/************************************************************************/
/* Sys_InitELF
/* Initialize hooks, get necessary variables/functions
/************************************************************************/
extern "C"
{
	void Sys_InitELF()
	{
		// CLIENT REPLACEMENT
		ORIG_HookUserMsg = (_HookUserMsg)(0x294B28);
		makeJal(0x246F70, (void*)HOOKED_HookUserMsg); // right before Initialize
	}
}