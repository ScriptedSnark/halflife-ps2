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
// Ammo.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "util.h"
#include "parsemsg.h"

#include "ammohistory.h"

WEAPON *gpActiveSel[2];	// NULL means off, 1 means just the menu bar, otherwise
						// this points to the active weapon menu item
WEAPON *gpLastSel[2];		// Last weapon menu selection 

client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

WeaponsResource gWR;

void WeaponsResource :: LoadAllWeaponSprites( int player )
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if ( rgWeapons[i][player].iId)
			LoadWeaponSprites( &rgWeapons[i][player] );
	}
}

int WeaponsResource :: CountAmmo( int iId, int player ) 
{ 
	if ( iId < 0 )
		return 0;

	return riAmmo[iId][player];
}

int WeaponsResource :: HasAmmo( WEAPON *p, int player )
{
	if ( !p )
		return FALSE;

	// weapons with no max ammo can always be selected
	if ( p->iMax1 == -1 )
		return TRUE;

	return (p->iAmmoType == -1) || p->iClip > 0 || CountAmmo(p->iAmmoType, player) 
		|| CountAmmo(p->iAmmo2Type, player) || ( p->iFlags & WEAPON_FLAGS_SELECTONEMPTY );
}


void WeaponsResource :: LoadWeaponSprites( WEAPON *pWeapon )
{
	int i, iRes;

	if (ScreenWidth < 512)
		iRes = 320;
	else
		iRes = 640;

	char sz[128];

	if ( !pWeapon )
		return;

	memset(&pWeapon->rcActive, 0, sizeof(wrect_t));
	memset(&pWeapon->rcInactive, 0, sizeof(wrect_t));
	memset(&pWeapon->rcAmmo, 0, sizeof(wrect_t));
	memset(&pWeapon->rcAmmo2, 0, sizeof(wrect_t));
	pWeapon->hInactive = 0;
	pWeapon->hActive = 0;
	pWeapon->hAmmo = 0;
	pWeapon->hAmmo2 = 0;

	Sys_sprintf(sz, "sprites/%s.txt", pWeapon->szName);
	client_sprite_t* pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t* p;

	p = GetSpriteList(pList, "crosshair", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = 0;

	p = GetSpriteList(pList, "scrosshair", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hSCrosshair = SPR_Load(sz);
		pWeapon->rcSCrosshair = p->rc;
	}
	else
		pWeapon->hSCrosshair = 0;

	p = GetSpriteList(pList, "autoaimold", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAutoaimOLD = SPR_Load(sz);
		pWeapon->rcAutoaimOLD = p->rc;
	}
	else
		pWeapon->hAutoaimOLD = 0;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList(pList, "zoom", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p)
	{
		Sys_sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo2 = 0;
}

// Returns the first weapon for a given slot.
WEAPON *WeaponsResource :: GetFirstPos( int iSlot, int player )
{
	WEAPON *pret = NULL;

	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if ( rgSlots[iSlot][i][player] && HasAmmo(rgSlots[iSlot][i][player], player))
		{
			pret = rgSlots[iSlot][i][player];
			break;
		}
	}

	return pret;
}


WEAPON* WeaponsResource :: GetNextActivePos( int iSlot, int iSlotPos, int player )
{
	if ( iSlotPos >= MAX_WEAPON_POSITIONS || iSlot >= MAX_WEAPON_SLOTS )
		return NULL;

	WEAPON *p = gWR.rgSlots[ iSlot ][ iSlotPos+1 ][player];
	
	if ( !p || !gWR.HasAmmo(p, player) )
		return GetNextActivePos( iSlot, iSlotPos + 1, player );

	return p;
}


int giBucketHeight, giBucketWidth, giABHeight, giABWidth; // Ammo Bar width and height

HSPRITE ghsprBuckets;					// Sprite for top row of weapons menu

DECLARE_MESSAGE(m_Ammo, CurWeapon );	// Current weapon and clip
DECLARE_MESSAGE(m_Ammo, WeaponList);	// new weapon type
DECLARE_MESSAGE(m_Ammo, AmmoX);			// update known ammo type's count
DECLARE_MESSAGE(m_Ammo, AmmoPickup);	// flashes an ammo pickup record
DECLARE_MESSAGE(m_Ammo, WeapPickup);    // flashes a weapon pickup record
DECLARE_MESSAGE(m_Ammo, HideWeapon);	// hides the weapon, ammo, and crosshair displays temporarily
DECLARE_MESSAGE(m_Ammo, ItemPickup);

DECLARE_COMMAND(m_Ammo, Slot1);
DECLARE_COMMAND(m_Ammo, Slot2);
DECLARE_COMMAND(m_Ammo, Slot3);
DECLARE_COMMAND(m_Ammo, Slot4);
DECLARE_COMMAND(m_Ammo, Slot5);
DECLARE_COMMAND(m_Ammo, Slot6);
DECLARE_COMMAND(m_Ammo, Slot7);
DECLARE_COMMAND(m_Ammo, Slot8);
DECLARE_COMMAND(m_Ammo, Slot9);
DECLARE_COMMAND(m_Ammo, Slot10);
DECLARE_COMMAND(m_Ammo, Close);
DECLARE_COMMAND(m_Ammo, NextWeapon);
DECLARE_COMMAND(m_Ammo, PrevWeapon);

// width of ammo fonts
#define AMMO_SMALL_WIDTH 10
#define AMMO_LARGE_WIDTH 20

#define HISTORY_DRAW_TIME	"5"

int CHudAmmo::Init(int player)
{
	gHUD.AddHudElem(this);

	HOOK_MESSAGE(CurWeapon);
	HOOK_MESSAGE(WeaponList);
	HOOK_MESSAGE(AmmoPickup);
	HOOK_MESSAGE(WeapPickup);
	HOOK_MESSAGE(ItemPickup);
	HOOK_MESSAGE(HideWeapon);
	HOOK_MESSAGE(AmmoX);

	if (player == 0)
	{
		HOOK_COMMAND("slot1", Slot1);
		HOOK_COMMAND("slot2", Slot2);
		HOOK_COMMAND("slot3", Slot3);
		HOOK_COMMAND("slot4", Slot4);
		HOOK_COMMAND("slot5", Slot5);
		HOOK_COMMAND("slot6", Slot6);
		HOOK_COMMAND("slot7", Slot7);
		HOOK_COMMAND("slot8", Slot8);
		HOOK_COMMAND("slot9", Slot9);
		HOOK_COMMAND("slot10", Slot10);
		HOOK_COMMAND("cancelselect", Close);
		HOOK_COMMAND("invnext", NextWeapon);
		HOOK_COMMAND("invprev", PrevWeapon);
	}

	Reset(player);

	if (player == 0)
	{
		CVAR_CREATE("hud_drawhistory_time", HISTORY_DRAW_TIME, 0);
		CVAR_CREATE("hud_fastswitch", "0", FCVAR_ARCHIVE);		// controls whether or not weapons can be selected in one keypress
	}

	m_iFlags |= HUD_ACTIVE; //!!!

	gWR.Init(player);
	gHR.Init(player);

	return 1;
};

void CHudAmmo::Reset(int player)
{
	m_fFade = 0;
	m_iFlags |= HUD_ACTIVE; //!!!

	gpActiveSel[player] = NULL;
	gHUD.m_iHideHUDDisplay = 0;

	gWR.Reset(player);
	gHR.Reset(player);

	//	VidInit();

}

int CHudAmmo::VidInit(int player)
{
	// Load sprites for buckets (top row of weapon menu)
	m_HUD_bucket0 = gHUD.GetSpriteIndex( "bucket1" );
	m_HUD_selection = gHUD.GetSpriteIndex( "selection" );

	ghsprBuckets = gHUD.GetSprite(m_HUD_bucket0);
	giBucketWidth = gHUD.GetSpriteRect(m_HUD_bucket0).right - gHUD.GetSpriteRect(m_HUD_bucket0).left;
	giBucketHeight = gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = max( gHR.iHistoryGap, gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top);

	// If we've already loaded weapons, let's get new sprites
	gWR.LoadAllWeaponSprites(player);

	if (ScreenWidth >= 512)
	{
		giABWidth = 20;
		giABHeight = 4;
	}
	else
	{
		giABWidth = 10;
		giABHeight = 2;
	}

	return 1;
}

//
// Think:
//  Used for selection of weapon menu item.
//
void CHudAmmo::Think(int player)
{
	if ( gHUD.m_fPlayerDead )
		return;

	if ( gHUD.m_iWeaponBits[player] != gWR.iOldWeaponBits[player] )
	{
		gWR.iOldWeaponBits[player] = gHUD.m_iWeaponBits[player];

		for (int i = MAX_WEAPONS-1; i > 0; i-- )
		{
			WEAPON *p = gWR.GetWeapon(i, player);

			if ( p )
			{
				if ( gHUD.m_iWeaponBits[player] & ( 1 << p->iId ) )
					gWR.PickupWeapon( p, player );
				else
					gWR.DropWeapon( p, player );
			}
		}
	}

	if (!gpActiveSel[player])
		return;

	// has the player selected one?
	if (gHUD.m_iKeyBits[player] & IN_ATTACK)
	{
		if (gpActiveSel[player] != (WEAPON *)1)
			ServerCmd(gpActiveSel[player]->szName, player);

		gpLastSel[player] = gpActiveSel[player];
		gpActiveSel[player] = NULL;
		gHUD.m_iKeyBits[player] &= ~IN_ATTACK;

		PlaySound("common/wpn_select.wav", player);
	}

}

//
// Helper function to return a Ammo pointer from id
//

HSPRITE* WeaponsResource :: GetAmmoPicFromWeapon( int iAmmoId, wrect_t& rect, int player )
{
	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		if ( rgWeapons[i][player].iAmmoType == iAmmoId)
		{
			rect = rgWeapons[i][player].rcAmmo;
			return &rgWeapons[i][player].hAmmo;
		}
		else if ( rgWeapons[i][player].iAmmo2Type == iAmmoId)
		{
			rect = rgWeapons[i][player].rcAmmo2;
			return &rgWeapons[i][player].hAmmo2;
		}
	}

	return NULL;
}


// Menu Selection Code

void WeaponsResource :: SelectSlot( int iSlot, int fAdvance, int iDirection, int player )
{
	if ( gHUD.m_Menu.m_fMenuDisplayed && (fAdvance == FALSE) && (iDirection == 1) )	
	{ // menu is overriding slot use commands
		gHUD.m_Menu.SelectMenuItem( iSlot + 1, player );  // slots are one off the key numbers
		return;
	}

	if ( iSlot > MAX_WEAPON_SLOTS )
		return;

	if ( gHUD.m_fPlayerDead || gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL ) )
		return;

	if (!(gHUD.m_iWeaponBits[player] & (1<<(WEAPON_SUIT)) ))
		return;

	WEAPON *p = NULL;

	if ( (gpActiveSel[player] == NULL) || (gpActiveSel[player] == (WEAPON*)1) || (iSlot != gpActiveSel[player]->iSlot))
	{
		PlaySound( "common/wpn_hudon.wav", player );
		p = GetFirstPos( iSlot, player );

		if ( p && CVAR_GET_FLOAT( "hud_fastswitch" ) > 0 ) // check for fast weapon switch mode
		{
			// if fast weapon switch is on, then weapons can be selected in a single keypress
			// but only if there is only one item in the bucket
			WEAPON *p2 = GetNextActivePos( p->iSlot, p->iSlotPos, player );
			if ( !p2 )
			{	// only one active item in bucket, so change directly to weapon
				ServerCmd( p->szName, player );
				return;
			}
		}
	}
	else
	{
		PlaySound("common/wpn_moveselect.wav", player);
		if ( gpActiveSel[player])
			p = GetNextActivePos( gpActiveSel[player]->iSlot, gpActiveSel[player]->iSlotPos, player);
		if ( !p )
			p = GetFirstPos( iSlot, player );
	}

	
	if ( !p )  // if no selection found,  just display the weapon list
		gpActiveSel[player] = (WEAPON*)1;
	else 
		gpActiveSel[player] = p;
}


//------------------------------------------------------------------------
// Message Handlers
//------------------------------------------------------------------------

//
// AmmoX  -- Update the count of a known type of ammo
// 
int CHudAmmo::MsgFunc_AmmoX(int player, const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	gWR.SetAmmo( iIndex, abs(iCount), player );

	return 1;
}

int CHudAmmo::MsgFunc_AmmoPickup( int player, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	// Add ammo to the history
	gHR.AddToHistory( HISTSLOT_AMMO, iIndex, abs(iCount), player );

	return 1;
}

int CHudAmmo::MsgFunc_WeapPickup( int player, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_BYTE();

	// Add the weapon to the history
	gHR.AddToHistory( HISTSLOT_WEAP, iIndex, 0 , player );

	return 1;
}

int CHudAmmo::MsgFunc_ItemPickup( int player, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	const char *szName = READ_STRING();

	// Add the weapon to the history
	gHR.AddToHistory( HISTSLOT_ITEM, szName, 0, player );

	return 1;
}


int CHudAmmo::MsgFunc_HideWeapon( int player, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	
	gHUD.m_iHideHUDDisplay = READ_BYTE();
	static wrect_t nullrc;

	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL ) )
	{
		gpActiveSel[player] = NULL;
		SetCrosshair( 0, &nullrc, 0, 0, 0, player );
	}
	else
	{
		if (m_pWeapon)
			SetCrosshair(m_pWeapon->hCrosshair, &m_pWeapon->rcCrosshair, 255, 255, 255, player);
		else
			SetCrosshair(0, &nullrc, 0, 0, 0, player);
	}

	return 1;
}

// 
//  CurWeapon: Update hud state with the current weapon and clip count. Ammo
//  counts are updated with AmmoX. Server assures that the Weapon ammo type 
//  numbers match a real ammo type.
//
int CHudAmmo::MsgFunc_CurWeapon(int player, const char *pszName, int iSize, void *pbuf )
{
	static wrect_t nullrc;
	int fOnTarget = FALSE;

	BEGIN_READ( pbuf, iSize );

	int iState = READ_BYTE();
	int iId = READ_CHAR();
	int iClip = READ_CHAR();

	// detect if we're also on target
	if ( iState > 1 )
	{
		fOnTarget = TRUE;
	}

	if ( iId < 1 )
	{
		SetCrosshair(0, &nullrc, 0, 0, 0, player);
		return 0;
	}

	// Is player dead???
	if ((iId == -1) && (iClip == -1))
	{
		gHUD.m_fPlayerDead = TRUE;
		gpActiveSel[player] = NULL;
		return 1;
	}

	gHUD.m_fPlayerDead = FALSE;

	WEAPON *pWeapon = gWR.GetWeapon( iId, player );

	if ( !pWeapon )
		return 0;

	if ( iClip < -1 )
		pWeapon->iClip = abs(iClip);
	else
		pWeapon->iClip = iClip;


	if ( iState == 0 )	// we're not the current weapon, so update no more
		return 1;

	m_pWeapon = pWeapon;

	if ( !(gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
	{
		if ( gHUD.m_iFOV >= 90 )
		{ // normal crosshairs
			if (fOnTarget && m_pWeapon->hAutoaim)
				SetCrosshair(m_pWeapon->hAutoaim, &m_pWeapon->rcAutoaim, 255, 255, 255, player);
			else
				SetCrosshair(m_pWeapon->hCrosshair, &m_pWeapon->rcCrosshair, 255, 255, 255, player);
		}
		else
		{ // zoomed crosshairs
			if (fOnTarget && m_pWeapon->hZoomedAutoaim)
				SetCrosshair(m_pWeapon->hZoomedAutoaim, &m_pWeapon->rcZoomedAutoaim, 255, 255, 255, player);
			else
				SetCrosshair(m_pWeapon->hZoomedCrosshair, &m_pWeapon->rcZoomedCrosshair, 255, 255, 255, player);
		}
	}

	m_fFade = 200.0f; //!!!
	m_iFlags |= HUD_ACTIVE;
	
	return 1;
}

//
// WeaponList -- Tells the hud about a new weapon type.
//
int CHudAmmo::MsgFunc_WeaponList(int player, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	
	WEAPON Weapon{};

	Q_strcpy( Weapon.szName, READ_STRING() );
	Weapon.iAmmoType = (int)READ_CHAR();	
	
	Weapon.iMax1 = READ_BYTE();
	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;

	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = READ_BYTE();
	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;

	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_CHAR();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;

	gWR.AddWeapon( &Weapon, player );

	return 1;

}

//------------------------------------------------------------------------
// Command Handlers
//------------------------------------------------------------------------

void CHudAmmo::UserCmd_Slot1(int player)
{
	gWR.SelectSlot(0, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot2(int player)
{
	gWR.SelectSlot(1, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot3(int player)
{
	gWR.SelectSlot(2, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot4(int player)
{
	gWR.SelectSlot(3, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot5(int player)
{
	gWR.SelectSlot(4, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot6(int player)
{
	gWR.SelectSlot(5, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot7(int player)
{
	gWR.SelectSlot(6, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot8(int player)
{
	gWR.SelectSlot(7, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot9(int player)
{
	gWR.SelectSlot(8, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Slot10(int player)
{
	gWR.SelectSlot(9, FALSE, 1, player);
}

void CHudAmmo::UserCmd_Close(int player)
{
	char cmd[32];

	if (gpActiveSel[player])
	{
		gpLastSel[player] = gpActiveSel[player];
		gpActiveSel[player] = NULL;
		PlaySound("common/wpn_hudoff.wav", player);
	}
	else
	{
		Sys_sprintf(cmd, "pause %d", player);
		ClientCmd(cmd);
	}
}


// Selects the next item in the weapon menu
void CHudAmmo::UserCmd_NextWeapon(int player)
{
	if ( gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		return;

	if ( !gpActiveSel[player] || gpActiveSel[player] == (WEAPON*)1)
		gpActiveSel[player] = m_pWeapon;

	int pos = 0;
	int slot = 0;
	if ( gpActiveSel[player])
	{
		pos = gpActiveSel[player]->iSlotPos + 1;
		slot = gpActiveSel[player]->iSlot;
	}

	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot < MAX_WEAPON_SLOTS; slot++ )
		{
			for ( ; pos < MAX_WEAPON_POSITIONS; pos++ )
			{
				WEAPON *wsp = gWR.GetWeaponSlot( slot, pos, player );

				if ( wsp && gWR.HasAmmo(wsp, player) )
				{
					gpActiveSel[player] = wsp;
					return;
				}
			}

			pos = 0;
		}

		slot = 0;  // start looking from the first slot again
	}

	gpActiveSel[player] = NULL;
}

// Selects the previous item in the menu
void CHudAmmo::UserCmd_PrevWeapon(int player)
{
	if ( gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		return;

	if ( !gpActiveSel[player] || gpActiveSel[player] == (WEAPON*)1)
		gpActiveSel[player] = m_pWeapon;

	int pos = MAX_WEAPON_POSITIONS-1;
	int slot = MAX_WEAPON_SLOTS-1;
	if ( gpActiveSel[player])
	{
		pos = gpActiveSel[player]->iSlotPos - 1;
		slot = gpActiveSel[player]->iSlot;
	}
	
	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot >= 0; slot-- )
		{
			for ( ; pos >= 0; pos-- )
			{
				WEAPON *wsp = gWR.GetWeaponSlot( slot, pos, player );

				if ( wsp && gWR.HasAmmo(wsp, player) )
				{
					gpActiveSel[player] = wsp;
					return;
				}
			}

			pos = MAX_WEAPON_POSITIONS-1;
		}
		
		slot = MAX_WEAPON_SLOTS-1;
	}

	gpActiveSel[player] = NULL;
}



//-------------------------------------------------------------------------
// Drawing code
//-------------------------------------------------------------------------

int CHudAmmo::Draw(float flTime, int player)
{
	int a, x, y, r, g, b;
	int AmmoWidth;

	if (!(gHUD.m_iWeaponBits[player] & (1<<(WEAPON_SUIT)) ))
		return 1;

	if ( (gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
		return 1;

	// Draw Weapon Menu
	DrawWList(flTime, player);

	// Draw ammo pickup history
	gHR.DrawAmmoHistory( flTime, player);

	if (!(m_iFlags & HUD_ACTIVE))
		return 0;

	if (!m_pWeapon)
		return 0;

	WEAPON *pw = m_pWeapon; // shorthand

	// SPR_Draw Ammo
	if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
		return 0;


	int iFlags = DHN_DRAWZERO; // draw 0 values

	AmmoWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	a = (int) max( MIN_ALPHA, m_fFade );

	if (m_fFade > 0)
		m_fFade -= (gHUD.m_flTimeDelta * 20);

	UnpackRGB(r,g,b, RGB_YELLOWISH);

	ScaleColors(r, g, b, a );

	// Does this weapon have a clip?
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/2;

	// Does weapon have any ammo at all?
	if (m_pWeapon->iAmmoType > 0)
	{
		int iIconWidth = m_pWeapon->rcAmmo.right - m_pWeapon->rcAmmo.left;
		
		if (pw->iClip >= 0)
		{
			// room for the number and the '|' and the current ammo
			
			x = ScreenWidth - (8 * AmmoWidth) - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, pw->iClip, r, g, b);

			wrect_t rc{};
			rc.top = 0;
			rc.left = 0;
			rc.right = AmmoWidth;
			rc.bottom = 100;

			int iBarWidth =  AmmoWidth/10;

			x += AmmoWidth/2;

			UnpackRGB(r,g,b, RGB_YELLOWISH);

			// draw the | bar
			FillRGBA(x, y, iBarWidth, gHUD.m_iFontHeight, r, g, b, a);

			x += iBarWidth + AmmoWidth/2;;

			// GL Seems to need this
			ScaleColors(r, g, b, a );
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType, player), r, g, b);		


		}
		else
		{
			// SPR_Draw a bullets only line
			x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType, player), r, g, b);
		}

		// Draw the ammo Icon
		int iOffset = (m_pWeapon->rcAmmo.bottom - m_pWeapon->rcAmmo.top)/8;
		SPR_Set(m_pWeapon->hAmmo, r, g, b);
		SPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo);
	}

	// Does weapon have seconday ammo?
	if (pw->iAmmo2Type > 0) 
	{
		int iIconWidth = m_pWeapon->rcAmmo2.right - m_pWeapon->rcAmmo2.left;

		// Do we have secondary ammo?
		if ((pw->iAmmo2Type != 0) && (gWR.CountAmmo(pw->iAmmo2Type, player) > 0))
		{
			y -= gHUD.m_iFontHeight + gHUD.m_iFontHeight/4;
			x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags|DHN_3DIGITS, gWR.CountAmmo(pw->iAmmo2Type, player), r, g, b);

			// Draw the ammo Icon
			SPR_Set(m_pWeapon->hAmmo2, r, g, b);
			int iOffset = (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top)/8;
			SPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo2);
		}
	}
	return 1;
}


//
// Draws the ammo bar on the hud
//
int DrawBar(int x, int y, int width, int height, float f, int player)
{
	int r, g, b;

	if (f < 0)
		f = 0;
	if (f > 1)
		f = 1;

	if (f)
	{
		int w = f * width;

		// Always show at least one pixel if we have ammo.
		if (w <= 0)
			w = 1;
		UnpackRGB(r, g, b, RGB_GREENISH);
		FillRGBA(x, y, w, height, r, g, b, 255);
		x += w;
		width -= w;
	}

	UnpackRGB(r, g, b, RGB_YELLOWISH);

	FillRGBA(x, y, width, height, r, g, b, 128);

	return (x + width);
}



void DrawAmmoBar(WEAPON *p, int x, int y, int width, int height, int player)
{
	if ( !p )
		return;
	
	if (p->iAmmoType != -1)
	{
		if (!gWR.CountAmmo(p->iAmmoType, player))
			return;

		float f = (float)gWR.CountAmmo(p->iAmmoType, player)/(float)p->iMax1;
		
		x = DrawBar(x, y, width, height, f, player);


		// Do we have secondary ammo too?

		if (p->iAmmo2Type != -1)
		{
			f = (float)gWR.CountAmmo(p->iAmmo2Type, player)/(float)p->iMax2;

			x += 5; //!!!

			DrawBar(x, y, width, height, f, player);
		}
	}
}




//
// Draw Weapon Menu
//
int CHudAmmo::DrawWList(float flTime, int player)
{
	int r,g,b,x,y,a,i;

	if ( !gpActiveSel[player])
		return 0;

	int iActiveSlot;

	if ( gpActiveSel[player] == (WEAPON*)1)
		iActiveSlot = -1;	// current slot has no weapons
	else 
		iActiveSlot = gpActiveSel[player]->iSlot;

	x = 10; //!!!
	y = 10; //!!!
	

	// Ensure that there are available choices in the active slot
	if ( iActiveSlot > 0 )
	{
		if ( !gWR.GetFirstPos( iActiveSlot, player) )
		{
			gpActiveSel[player] = (WEAPON*)1;
			iActiveSlot = -1;
		}
	}
		
	// Draw top line
	for ( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		int iWidth;

		UnpackRGB(r,g,b, RGB_YELLOWISH);
	
		if ( iActiveSlot == i )
			a = 255;
		else
			a = 192;

		ScaleColors(r, g, b, 255);
		SPR_Set(gHUD.GetSprite(m_HUD_bucket0 + i), r, g, b );

		// make active slot wide enough to accomodate gun pictures
		if ( i == iActiveSlot )
		{
			WEAPON *p = gWR.GetFirstPos(iActiveSlot, player);
			if ( p )
				iWidth = p->rcActive.right - p->rcActive.left;
			else
				iWidth = giBucketWidth;
		}
		else
			iWidth = giBucketWidth;

		SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_bucket0 + i));
		
		x += iWidth + 5;
	}


	a = 128; //!!!
	x = 10;

	// Draw all of the buckets
	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		y = giBucketHeight + 10;

		// If this is the active slot, draw the bigger pictures,
		// otherwise just draw boxes
		if ( i == iActiveSlot )
		{
			WEAPON *p = gWR.GetFirstPos( i, player );
			int iWidth = giBucketWidth;
			if ( p )
				iWidth = p->rcActive.right - p->rcActive.left;

			for ( int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++ )
			{
				p = gWR.GetWeaponSlot( i, iPos, player );

				if ( !p || !p->iId )
					continue;

				UnpackRGB( r,g,b, RGB_YELLOWISH );
			
				// if active, then we must have ammo.

				if ( gpActiveSel[player] == p)
				{
					SPR_Set(p->hActive, r, g, b );
					SPR_DrawAdditive(0, x, y, &p->rcActive);

					SPR_Set(gHUD.GetSprite(m_HUD_selection), r, g, b );
					SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_selection));
				}
				else
				{
					// Draw Weapon if Red if no ammo

					if ( gWR.HasAmmo(p, player) )
						ScaleColors(r, g, b, 192);
					else
					{
						UnpackRGB(r,g,b, RGB_REDISH);
						ScaleColors(r, g, b, 128);
					}

					SPR_Set( p->hInactive, r, g, b );
					SPR_DrawAdditive( 0, x, y, &p->rcInactive );
				}

				// Draw Ammo Bar

				DrawAmmoBar(p, x + giABWidth/2, y, giABWidth, giABHeight, player);
				
				y += p->rcActive.bottom - p->rcActive.top + 5;
			}

			x += iWidth + 5;

		}
		else
		{
			// Draw Row of weapons.

			UnpackRGB(r,g,b, RGB_YELLOWISH);

			for ( int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++ )
			{
				WEAPON *p = gWR.GetWeaponSlot( i, iPos, player );
				
				if ( !p || !p->iId )
					continue;

				if ( gWR.HasAmmo(p, player) )
				{
					UnpackRGB(r,g,b, RGB_YELLOWISH);
					a = 128;
				}
				else
				{
					UnpackRGB(r,g,b, RGB_REDISH);
					a = 96;
				}

				FillRGBA( x, y, giBucketWidth, giBucketHeight, r, g, b, a );

				y += giBucketHeight + 5;
			}

			x += giBucketWidth + 5;
		}
	}	

	return 1;

}


/* =================================
	GetSpriteList

Finds and returns the matching 
sprite name 'psz' and resolution 'iRes'
in the given sprite list 'pList'
iCount is the number of items in the pList
================================= */
client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount)
{
	if (!pList)
		return NULL;

	int i = iCount;
	client_sprite_t *p = pList;

	while(i--)
	{
		if ((!strcmp((char*)psz, p->szName)) && (p->iRes == iRes))
			return p;
		p++;
	}

	return NULL;
}
