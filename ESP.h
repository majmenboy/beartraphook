/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Hacks.h"

class CEsp : public CHack
{
public:
	void Init();
	void BacktrackingCross(IClientEntity* pEntity);
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:

	// Other shit
	IClientEntity *BombCarrier;

	struct ESPBox
	{
		int x, y, w, h;
	};

	// Draw a player
	void DrawPlayer(IClientEntity* pEntity, player_info_t pinfo);

	// Get player info
	Color GetPlayerColor(IClientEntity* pEntity);
	bool GetBox(IClientEntity* pEntity, ESPBox &result);

	// Draw shit about player
	void SpecList();
	void DrawGlow(IClientEntity *pEntity, int r, int g, int b, int a);
	void DrawBox(ESPBox size, Color color);
	//void DrawWeapon(IClientEntity * pEntity, CEsp::ESPBox size);
	//void DrawIcon(IClientEntity * pEntity, CEsp::ESPBox size);
	void DrawName(player_info_t pinfo, ESPBox size);
	void DrawMoney(IClientEntity * pEntity, CEsp::ESPBox size);
	void Barrel(CEsp::ESPBox size, Color color, IClientEntity * pEntity);
	void DrawHealth(ESPBox box, IClientEntity* pEntity);
	void Armor(ESPBox box, IClientEntity* pEntity);
	void DrawInfo(IClientEntity* pEntity, ESPBox size);
//	void Drawbeartrap(IClientEntity* pEntity, ESPBox size);
	void DrawGlow();
	void DrawCross(IClientEntity* pEntity);
	void DrawSkeleton(IClientEntity* pEntity);
	void HealthSkeleton(IClientEntity* pEntity);
	void DrawChicken(IClientEntity* pEntity, ClientClass* cClass);
	void DrawDrop(IClientEntity* pEntity, ClientClass* cClass);
	void BombTimer(IClientEntity * pEntity, ClientClass * cClass);
	void DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass);
	void DrawBomb(IClientEntity* pEntity, ClientClass* cClass);
};

char* const itemNames[] =
{
	"knife", //0 - default
	"deagle",
	"elite",
	"fiveseven",
	"glock",
	"none",
	"none",
	"ak47",
	"aug",
	"awp",

	"famas", //10
	"g3sg1",
	"none",
	"galil",
	"m249",
	"none",
	"m4a4",
	"mac-10",
	"none",
	"p90",

	"none", //20
	"none",
	"none",
	"none",
	"ump45",
	"xm1014",
	"bizon",
	"mag7",
	"negev",
	"sawed-off",

	"tec9", //30
	"taser",
	"p2000",
	"mp7",
	"mp9",
	"nova",
	"p250",
	"none",
	"scar20",
	"sg556",

	"ssg08", //40
	"knife",
	"knife",
	"flash",
	"nade",
	"smoke",
	"molotov",
	"decoy",
	"incendiary",
	"c4",

	"none", //50
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"knife",

	"m4a1", //60
	"usp",
	"none",
	"cz75",
	"revolver"
};
