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
// ammohistory.h
//

// this is the max number of items in each bucket
#define MAX_WEAPON_POSITIONS		MAX_WEAPON_SLOTS

class WeaponsResource
{
private:
	// Information about weapons & ammo
	WEAPON		rgWeapons[MAX_WEAPONS][2];	// Weapons Array

	// counts of weapons * ammo
	WEAPON*		rgSlots[MAX_WEAPON_SLOTS+1][MAX_WEAPON_POSITIONS+1][2];	// The slots currently in use by weapons.  The value is a pointer to the weapon;  if it's NULL, no weapon is there
	int			riAmmo[MAX_AMMO_TYPES][2];							// count of each ammo type

public:
	void Init( int player )
	{
		memset( rgWeapons, 0, sizeof rgWeapons);
		Reset(player);
	}

	void Reset(int player)
	{
		iOldWeaponBits[player] = 0;
		memset( rgSlots, 0, sizeof rgSlots );
		memset( riAmmo, 0, sizeof riAmmo );
	}

///// WEAPON /////
	int			iOldWeaponBits[2];

	WEAPON *GetWeapon( int iId, int player ) { return &rgWeapons[iId][player]; }
	void AddWeapon( WEAPON *wp, int player)
	{ 
		rgWeapons[ wp->iId ][player] = *wp;
		LoadWeaponSprites( &rgWeapons[ wp->iId ][player]);
	}

	void PickupWeapon( WEAPON *wp, int player)
	{
		rgSlots[ wp->iSlot ][ wp->iSlotPos ][ player ] = wp;
	}

	void DropWeapon( WEAPON *wp, int player)
	{
		rgSlots[ wp->iSlot ][ wp->iSlotPos ][ player ] = NULL;
	}

	void DropAllWeapons(int player)
	{
		for ( int i = 0; i < MAX_WEAPONS; i++ )
		{
			if ( rgWeapons[i][player].iId)
				DropWeapon( &rgWeapons[i][player], player);
		}
	}

	WEAPON* GetWeaponSlot( int slot, int pos, int player) { return rgSlots[slot][pos][player]; }

	void LoadWeaponSprites( WEAPON* wp );
	void LoadAllWeaponSprites( int player );
	WEAPON* GetFirstPos( int iSlot, int player );
	void SelectSlot( int iSlot, int fAdvance, int iDirection, int player );
	WEAPON* GetNextActivePos( int iSlot, int iSlotPos, int player);

	int HasAmmo( WEAPON *p, int player );

///// AMMO /////
	AMMO GetAmmo( int iId ) { return iId; }

	void SetAmmo( int iId, int iCount, int player) { riAmmo[ iId ][ player ] = iCount; }

	int CountAmmo( int iId, int player);

	HSPRITE* GetAmmoPicFromWeapon( int iAmmoId, wrect_t& rect, int player );

};

extern WeaponsResource gWR;


#define MAX_HISTORY 12
enum {
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};

class HistoryResource
{
private:
	struct HIST_ITEM {
		int type;
		float DisplayTime;  // the time at which this item should be removed from the history
		int iCount;
		int iId;
	};

	HIST_ITEM rgAmmoHistory[MAX_HISTORY][2];

public:

	void Init( int player )
	{
		Reset(player);
	}

	void Reset( int player )
	{
		memset( rgAmmoHistory, 0, sizeof rgAmmoHistory);
	}

	int iHistoryGap;
	int iCurrentHistorySlot;

	void AddToHistory( int iType, int iId, int iCount, int player );
	void AddToHistory( int iType, const char *szName, int iCount, int player );

	void CheckClearHistory( int player );
	int DrawAmmoHistory( float flTime, int player );
};

extern HistoryResource gHR;



