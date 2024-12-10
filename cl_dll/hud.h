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
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//


#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FF1010 //255,160,0
#define RGB_GREENISH 0x0000A000 //0,160,0

typedef struct rect_s
{
	int				left, right, top, bottom;
} wrect_t;

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "cl_dll.h"
#include "ammo.h"

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4
#define MIN_ALPHA	 100	

#define		HUDELEM_ACTIVE	1

typedef struct {
	int x, y;
} POSITION;

typedef struct {
	unsigned char r,g,b,a;
} RGBA;


#define HUD_ACTIVE	1
#define HUD_INTERMISSION 2

#define MAX_PLAYER_NAME_LENGTH		32

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	POSITION  m_pos;
	int   m_type;
	int	  m_iFlags; // active, moving, 
	virtual int Init( int player ) {return 0;}
	virtual int VidInit( int player ) {return 0;}
	virtual int Draw(float flTime, int player) {return 0;}
	virtual void Think(int player) {return;}
	virtual void Reset(int player) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to

};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};


//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw(float flTime, int player);
	void Think(int player);
	void Reset(int player);
	int DrawWList(float flTime, int player);
	int MsgFunc_CurWeapon(int player, const char *pszName, int iSize, void *pbuf);
	int MsgFunc_WeaponList(int player, const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoX(int player, const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoPickup( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_WeapPickup( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ItemPickup( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_HideWeapon( int player, const char *pszName, int iSize, void *pbuf );

	void /*_cdecl*/ UserCmd_Slot1( int player );
	void /*_cdecl*/ UserCmd_Slot2( int player );
	void /*_cdecl*/ UserCmd_Slot3( int player );
	void /*_cdecl*/ UserCmd_Slot4( int player );
	void /*_cdecl*/ UserCmd_Slot5( int player );
	void /*_cdecl*/ UserCmd_Slot6( int player );
	void /*_cdecl*/ UserCmd_Slot7( int player );
	void /*_cdecl*/ UserCmd_Slot8( int player );
	void /*_cdecl*/ UserCmd_Slot9( int player );
	void /*_cdecl*/ UserCmd_Slot10( int player );
	void /*_cdecl*/ UserCmd_Close( int player );
	void /*_cdecl*/ UserCmd_NextWeapon( int player );
	void /*_cdecl*/ UserCmd_PrevWeapon( int player );

private:
	float m_fFade;
	RGBA  m_rgba;
	WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

};

//
//-----------------------------------------------------
//

class CHudAmmoSecondary: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	void Reset( void );
	int Draw(float flTime, int player);

	int MsgFunc_SecAmmoVal( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_SecAmmoIcon( int player, const char *pszName, int iSize, void *pbuf );

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};


#include "health.h"


#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw(float flTime, int player);
	int MsgFunc_Geiger(int player, const char *pszName, int iSize, void *pbuf);
	
private:
	int m_iGeigerRange;

};

//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw(float flTime, int player);
	int MsgFunc_Train(int player, const char *pszName, int iSize, void *pbuf);

private:
	HSPRITE m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
class CHudMOTD : public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw( float flTime, int player );
	void Reset( void );

	int MsgFunc_MOTD( int player, const char *pszName, int iSize, void *pbuf );

protected:
	enum { MAX_MOTD_LENGTH = 241, };
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];
	float m_flActiveTill;
	int m_iLines;
};

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw( float flTime, int player );
	void Reset( void );
	void ParseStatusString( int line_num );

	int MsgFunc_StatusText( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_StatusValue( int player, const char *pszName, int iSize, void *pbuf );

protected:
	enum { 
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];  // a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	// the constructed bar that is drawn
	int m_iStatusValues[MAX_STATUSBAR_VALUES];  // an array of values for use in the status bar

	int m_bReparseString; // set to TRUE whenever the m_szStatusBar needs to be recalculated
};

//
//-----------------------------------------------------
//
class CHudScoreboard: public CHudBase
{
public:
	int Init( int player );
	void InitHUDData( void );
	int VidInit( int player );
	int Draw( float flTime, int player );
	int DrawPlayers( int xoffset, float listslot, int nameoffset = 0, char *team = NULL ); // returns the ypos where it finishes drawing
	void UserCmd_ShowScores( int player );
	void UserCmd_HideScores( int player );
	int MsgFunc_ScoreInfo( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamInfo( int player, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamScore( int player, const char *pszName, int iSize, void *pbuf );
	void DeathMsg( int killer, int victim );

	enum { 
		MAX_PLAYERS = 64,
		MAX_TEAMS = 64,
		MAX_TEAM_NAME = 16,
	};

	struct extra_player_info_t {
		short frags;
		short deaths;
		char teamname[MAX_TEAM_NAME];
	};

	struct team_info_t {
		char name[MAX_TEAM_NAME];
		short frags;
		short deaths;
		short ping;
		short packetloss;
		short ownteam;
		short players;
		int already_drawn;
		int scores_overriden;
	};

	hud_player_info_t m_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
	extra_player_info_t m_PlayerExtraInfo[MAX_PLAYERS+1];  // additional player info sent directly to the client dll
	team_info_t m_TeamInfo[MAX_TEAMS+1];

	int m_iNumTeams;

	int m_iLastKilledBy;
	int m_fLastKillTime;
	int m_iPlayerNum;
	int m_iShowscoresHeld;

	void GetAllPlayersInfo( void );
};

//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	int Init( int player );
	void InitHUDData( void );
	int VidInit( int player );
	int Draw( float flTime, int player );
	int MsgFunc_DeathMsg( int player, const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_d_skull;  // sprite index of skull icon
};

//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	int Init( int player );
	void InitHUDData( void );
	int VidInit( int player );
	void Reset( void );
	int Draw( float flTime, int player );
	int MsgFunc_ShowMenu( int player, const char *pszName, int iSize, void *pbuf );

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	int m_fWaitingForMore;
};

//
//-----------------------------------------------------
//
class CHudSayText : public CHudBase
{
public:
	int Init( int player );
	void InitHUDData( void );
	int VidInit( int player );
	int Draw( float flTime, int player );
	int MsgFunc_SayText( int player, const char *pszName, int iSize, void *pbuf );
	void SayTextPrint( const char *pszBuf, int iBufSize );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
};

//
//-----------------------------------------------------
//
class CHudBattery: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw(float flTime, int player);
	int MsgFunc_Battery(int player, const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	int	  m_iBat;	
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw(float flTime, int player);
	void Reset( int player );
	int MsgFunc_Flashlight(int player, const char *pszName,  int iSize, void *pbuf );
	int MsgFunc_FlashBat(int player, const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	HSPRITE m_hBeam;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	wrect_t *m_prcBeam;
	float m_flBat;	
	int	  m_iBat;	
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage: public CHudBase
{
public:
	int Init( int player );
	char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	char *BufferedLocaliseTextString( const char *msg );
	char *LookupString( const char *msg_name, int *msg_dest = NULL );
	int MsgFunc_TextMsg(int player, const char *pszName, int iSize, void *pbuf);
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	int Draw(float flTime, int player);
	int MsgFunc_HudText(int player, const char *pszName, int iSize, void *pbuf);
	int MsgFunc_GameTitle(int player, const char *pszName, int iSize, void *pbuf);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( const char *pName, float time );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH	24

class CHudStatusIcons: public CHudBase
{
public:
	int Init( int player );
	int VidInit( int player );
	void Reset( void );
	int Draw(float flTime, int player);
	int MsgFunc_StatusIcon(int player, const char *pszName, int iSize, void *pbuf);

	enum { 
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

	
	//had to make these public so CHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon( char *pszIconName, unsigned char red, unsigned char green, unsigned char blue );
	void DisableIcon( char *pszIconName );

private:

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		HSPRITE spr;
		wrect_t rc;
		unsigned char r, g, b;
	} icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];

};


//
//-----------------------------------------------------
//

class CHud
{
private:
	HUDLIST						*m_pHudList;
	HSPRITE						m_hsprLogo;
	int							m_iLogo;
	client_sprite_t				*m_pSpriteList;
	int							m_iSpriteCount;
	int							m_iSpriteCountAllRes;
	float						m_flMouseSensitivity;
	int							m_iConcussionEffect; 

public:

	float m_flTime;	   // the current client time
	float m_fOldTime;  // the time at which the HUD was last redrawn
	double m_flTimeDelta; // the difference between flTime and fOldTime
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;
	int		m_Teamplay;
	int		m_iRes;

	int m_iFontHeight;
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudString(int x, int y, int iMaxX, char *szString, int r, int g, int b );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b );
	int GetNumWidth(int iNumber, int iFlags);

private:
	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	HSPRITE *m_rghSprites;	/*[HUD_SPRITE_COUNT]*/			// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/

public:
	HSPRITE GetSprite( int index ) 
	{
		return (index < 0) ? 0 : m_rghSprites[index];
	}

	wrect_t& GetSpriteRect( int index )
	{
		return m_rgrcRects[index];
	}

	
	int GetSpriteIndex( const char *SpriteName );	// gets a sprite index, for use in the m_rghSprites[] array

	CHudAmmo	m_Ammo;
	CHudHealth	m_Health;
	CHudGeiger	m_Geiger;
	CHudBattery	m_Battery;
	CHudTrain	m_Train;
	CHudFlashlight m_Flash;
	CHudMessage m_Message;
	CHudScoreboard m_Scoreboard;
	CHudMOTD    m_MOTD;
	CHudStatusBar    m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText m_SayText;
	CHudMenu	m_Menu;
	CHudAmmoSecondary	m_AmmoSecondary;
	CHudTextMessage m_TextMessage;
	CHudStatusIcons m_StatusIcons;

	void Init( int player );
	void VidInit( int player );
	void Think(int player);
	int Redraw( float flTime, int intermission, int player );
	int UpdateClientData( client_data_t *cdata, int player );

	CHud() : m_iSpriteCount(0), m_pHudList(NULL) {}  
	~CHud();			// destructor, frees allocated memory

	// user messages
	int /*_cdecl*/ MsgFunc_Damage(int player, const char *pszName, int iSize, void *pbuf );
	int /*_cdecl*/ MsgFunc_GameMode(int player, const char *pszName, int iSize, void *pbuf );
	int /*_cdecl*/ MsgFunc_Logo(int player, const char *pszName,  int iSize, void *pbuf);
	int /*_cdecl*/ MsgFunc_ResetHUD(int player, const char *pszName,  int iSize, void *pbuf);
	void /*_cdecl*/ MsgFunc_InitHUD( int player, const char *pszName, int iSize, void *pbuf );
	int /*_cdecl*/ MsgFunc_SetFOV(int player, const char *pszName,  int iSize, void *pbuf);
	int  /*_cdecl*/ MsgFunc_Concuss( int player, const char *pszName, int iSize, void *pbuf );
	// Screen information
	SCREENINFO	m_scrinfo;

	int	m_iWeaponBits;
	int	m_fPlayerDead;
	int m_iIntermission;

	// sprite indexes
	int m_HUD_number_0;


	void AddHudElem(CHudBase *p);

};

extern CHud gHUD;