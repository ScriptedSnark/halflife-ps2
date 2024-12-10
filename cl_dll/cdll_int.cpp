/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  cdll_int.c
//
// this implementation handles the linking of the engine to the DLL
//

#include "hud.h"
#include "util.h"

// PS2 client doesn't have any exports because of static linking - ScriptedSnark
#define DLLEXPORT
//#define DLLEXPORT __declspec( dllexport )

cl_enginefunc_t gEngfuncs;
CHud gHUD;

/*
========================== 
    Initialize

Called when the DLL is first loaded.
==========================
*/
extern "C" 
{
int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );
int DLLEXPORT HUD_VidInit( int player );
int DLLEXPORT HUD_Init( int player );
int DLLEXPORT HUD_Redraw( float flTime, int intermission, int player );
int DLLEXPORT HUD_UpdateClientData( client_data_t *cdata, int player );
}


int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	gEngfuncs = *pEnginefuncs;

	//!!! mwh UNDONE We need to think about our versioning strategy. Do we want to try to be compatible
	// with previous versions, especially when we're only 'bonus' functionality? Should it be the engine
	// that decides if the DLL is compliant?

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	Q_memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	return 1;
}


/*
==========================
	HUD_VidInit

Called when the game initializes
and whenever the vid_mode is changed
so the HUD can reinitialize itself.
==========================
*/

int DLLEXPORT HUD_VidInit( int player )
{
	gHUD.VidInit(player);
	return 1;
}

/*
==========================
	HUD_Init

Called whenever the client connects
to a server.  Reinitializes all 
the hud variables.
==========================
*/

int DLLEXPORT HUD_Init( int player )
{
	//gEngfuncs.pfnClientCmd(";bind v \"toggleconsole\";\n");
	gHUD.Init(player);
	return 1;
}


/*
==========================
	HUD_Redraw

called every screen frame to
redraw the HUD.
===========================
*/

int DLLEXPORT HUD_Redraw( float time, int intermission, int player )
{
	gHUD.Redraw( time, intermission, player );

	return 1;
}


/*
==========================
	HUD_UpdateClientData

called every time shared client
dll/engine data gets changed,
and gives the cdll a chance
to modify the data.

returns 1 if anything has been changed, 0 otherwise.
==========================
*/

int DLLEXPORT HUD_UpdateClientData(client_data_t *pcldata, int player)
{
	return gHUD.UpdateClientData(pcldata, player);
}