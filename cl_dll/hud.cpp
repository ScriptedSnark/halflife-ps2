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
// hud.cpp
//
// implementation of CHud class
//

#include "hud.h"
#include "util.h"
#include "parsemsg.h"

extern client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

//DECLARE_MESSAGE(m_Logo, Logo)
int __MsgFunc_Logo(int player, const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Logo(player, pszName, iSize, pbuf );
}

//DECLARE_MESSAGE(m_Logo, Logo)
int __MsgFunc_ResetHUD(int player, const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_ResetHUD(player, pszName, iSize, pbuf );
}

int __MsgFunc_InitHUD(int player, const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_InitHUD( player, pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_SetFOV(int player, const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_SetFOV( player, pszName, iSize, pbuf );
}

int __MsgFunc_Concuss(int player, const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Concuss( player, pszName, iSize, pbuf );
}

int __MsgFunc_GameMode(int player, const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_GameMode( player, pszName, iSize, pbuf );
}

int __MsgFunc_HudColor(int player, const char* pszName, int iSize, void* pbuf)
{
	return gHUD.MsgFunc_HudColor(player, pszName, iSize, pbuf);
}

// This is called every time the DLL is loaded
void CHud :: Init( int player )
{
	HOOK_MESSAGE( Logo );
	HOOK_MESSAGE( ResetHUD );
	HOOK_MESSAGE( GameMode );
	HOOK_MESSAGE( InitHUD );
	HOOK_MESSAGE( SetFOV );
	HOOK_MESSAGE( Concuss );
	HOOK_MESSAGE( HudColor );

	m_iLogo = 0;
	m_iFOV = 0;

	if (player == 0)
	{
		CVAR_CREATE("zoom_sensitivity_ratio", "1.2", 0);
		CVAR_CREATE("default_fov", "90", 0);
	}

	m_pSpriteList = NULL;

	// Clear any old HUD list
	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			Q_free( pList );
		}
		m_pHudList = NULL;
	}

	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;

	m_Ammo.Init(player);
	m_Health.Init(player);
	m_Geiger.Init(player);
	m_Train.Init(player);
	m_Battery.Init(player);
	m_Flash.Init(player);
	m_Message.Init(player);
	m_Scoreboard.Init(player);
	m_MOTD.Init(player);
	m_StatusBar.Init(player);
	m_DeathNotice.Init(player);
	m_AmmoSecondary.Init(player);
	m_TextMessage.Init(player);
	m_StatusIcons.Init(player);

	m_SayText.Init(player);
	m_Menu.Init(player);

	MsgFunc_ResetHUD(player, 0, 0, NULL );
}

// CHud destructor
// cleans up memory allocated for m_rg* arrays
CHud :: ~CHud()
{
	Q_free((void*)m_rghSprites);
	Q_free((void*)m_rgrcRects);
	Q_free((void*)m_rgszSpriteNames);
	/*
	delete[] m_rghSprites;
	delete[] m_rgrcRects;
	delete[] m_rgszSpriteNames;
	*/
}

// GetSpriteIndex()
// searches through the sprite list loaded from hud.txt for a name matching SpriteName
// returns an index into the gHUD.m_rghSprites[] array
// returns 0 if sprite not found
int CHud :: GetSpriteIndex( const char *SpriteName )
{
	// look through the loaded sprite name list for SpriteName
	for ( int i = 0; i < m_iSpriteCount; i++ )
	{
		if ( strncmp( SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH ) == 0 )
			return i;
	}

	return -1; // invalid sprite
}

void CHud :: VidInit( int player )
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	// ----------
	// Load Sprites
	// ---------
//	m_hsprFont = LoadSprite("sprites/%d_font.spz");
	
	m_hsprLogo = 0;	

	if (ScreenWidth < 512)
		m_iRes = 320;
	else
		m_iRes = 640;

	// Only load this once
	if ( !m_pSpriteList )
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			for ( int j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
					m_iSpriteCount++;
				p++;
			}

			// allocated memory for sprite handle arrays
			m_rghSprites = (HSPRITE*)Q_malloc(m_iSpriteCount * sizeof(HSPRITE));
			m_rgrcRects = (wrect_t*)Q_malloc(m_iSpriteCount * sizeof(wrect_t));
			m_rgszSpriteNames = (char*)Q_malloc(m_iSpriteCount * MAX_SPRITE_NAME_LENGTH * sizeof(char));

			p = m_pSpriteList;
			int index = 0;
			for ( int j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
				{
					char sz[256];
					Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					Q_strncpy( &m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH );

					index++;
				}

				p++;
			}
		}
	}
	else
	{
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for ( int j = 0; j < m_iSpriteCountAllRes; j++ )
		{
			if ( p->iRes == m_iRes )
			{
				char sz[256];
				Sys_sprintf( sz, "sprites/%s.spz", p->szSprite );
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}

			p++;
		}
	}

	// assumption: number_1, number_2, etc, are all listed and loaded sequentially
	m_HUD_number_0 = GetSpriteIndex( "number_0" );

	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;

	m_Ammo.VidInit(player);
	m_Health.VidInit(player);
	m_Geiger.VidInit(player);
	m_Train.VidInit(player);
	m_Battery.VidInit(player);
	m_Flash.VidInit(player);
	m_Message.VidInit(player);
	m_Scoreboard.VidInit(player);
	m_MOTD.VidInit(player);
	m_StatusBar.VidInit(player);
	m_DeathNotice.VidInit(player);
	m_SayText.VidInit(player);
	m_Menu.VidInit(player);
	m_AmmoSecondary.VidInit(player);
	m_TextMessage.VidInit(player);
	m_StatusIcons.VidInit(player);
}

int CHud::MsgFunc_Logo(int player, const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iLogo = READ_BYTE();

	return 1;
}

int CHud::MsgFunc_SetFOV(int player, const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int newfov = READ_BYTE();
	int def_fov = CVAR_GET_FLOAT( "default_fov" );

	if ( newfov == 0 )
	{
		m_iFOV = def_fov;
	}
	else
	{
		m_iFOV = newfov;
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == def_fov )
	{  
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{  
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = CVAR_GET_FLOAT("sensitivity") * ((float)newfov / (float)def_fov) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	return 1;
}


void CHud::AddHudElem(CHudBase *phudelem)
{
	HUDLIST *pdl, *ptemp;

//phudelem->Think();

	if (!phudelem)
		return;
		
	pdl = (HUDLIST*)Q_malloc(sizeof(HUDLIST));
	if (!pdl)
		return;

	memset(pdl, 0, sizeof(HUDLIST));
	pdl->p = phudelem;

	if (!m_pHudList)
	{
		m_pHudList = pdl;
		return;
	}

	ptemp = m_pHudList;

	while (ptemp->pNext)
		ptemp = ptemp->pNext;

	ptemp->pNext = pdl;
}


