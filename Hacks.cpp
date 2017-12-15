/*
Syn's Payhake Framework for Insanity & Razor
*/

#define _CRT_SECURE_NO_WARNINGS

#include "Hacks.h"
#include "Interfaces.h"
#include "RenderManager.h"

#include "ESP.h"
#include "Visuals.h"
#include "RageBot.h"
#include "MiscHacks.h"
#include "LegitBot.h"

CEsp Esp;
CVisuals Visuals;
CMiscHacks MiscHacks;
CRageBot RageBot;
CLegitBot LegitBot;

// Initialise and register ALL hackmanager hacks in here nigga
void Hacks::SetupHacks()
{
	Esp.Init();
	Visuals.Init();
	MiscHacks.Init();
	RageBot.Init();
	LegitBot.Init();

	hackManager.RegisterHack(&Esp);
	hackManager.RegisterHack(&Visuals);
	hackManager.RegisterHack(&MiscHacks);
	hackManager.RegisterHack(&RageBot);
	hackManager.RegisterHack(&LegitBot);

	//--------------------------------
	hackManager.Ready();
}

using PlaySoundFn = void(__stdcall*)(const char*);
extern PlaySoundFn oPlaySound;

namespace G // Global Stuff
{
	extern bool			Aimbotting;
	extern bool			InAntiAim;
	extern bool			Return;
	extern CUserCmd*	UserCmd;
	extern HMODULE		Dll;
	extern HWND			Window;
	extern bool			PressedKeys[256];
	extern bool			d3dinit;
	extern float		FOV;
	extern int			ChamMode;
	extern bool			SendPacket;
	extern int			BestTarget;
}

HWND				G::Window;

PlaySoundFn oPlaySound;
void __stdcall Hooked__PlaySoundCSGO(const char* fileName)
{
	IClientEntity* pLocal = hackManager.pLocal();

	oPlaySound(fileName);

	if (Interfaces::Engine->IsInGame() || !Menu::Window.MiscTab.OtherAutoAccept.GetState())
		return;

	if (!strcmp(fileName, "weapons/hegrenade/beep.wav"))
	{
		//Accept the game
		GameUtils::IsReady = (IsReadyFn)(Offsets::Functions::dwIsReady);
		GameUtils::IsReady();

		//This will flash the CSGO window on the taskbar
		//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = G::Window;
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
		FlashWindowEx(&fi);
	}
}

// Only gets called in game, use a seperate draw UI call for menus in the hook
void Hacks::DrawHacks()
{
	IClientEntity *pLocal = hackManager.pLocal();

	void Hooked__PlaySoundCSGO(const char* fileName);

	// Spectator List
	/*if (Menu::Window.MiscTab.OtherSpectators.GetState())
		SpecList();*/

	// Check the master visuals switch, just to be sure
	if (!Menu::Window.VisualsTab.Active.GetState())
		return;

	if (Menu::Window.VisualsTab.OptionsCompRank.GetState() && GUI.GetKeyState(VK_TAB))
	{
		GameUtils::ServerRankRevealAll();
	}

	hackManager.Draw();
	//--------------------------------
		
}

// Game Cmd Changes
void Hacks::MoveHacks(CUserCmd *pCmd, bool &bSendPacket)
{
	Vector origView = pCmd->viewangles;
	IClientEntity *pLocal = hackManager.pLocal();
	hackManager.Move(pCmd, bSendPacket);
	// ------------------------------

	// Put it in here so it's applied AFTER the aimbot
	int AirStuckKey = Menu::Window.MiscTab.OtherAirStuck.GetKey();
	if (AirStuckKey > 0 && GUI.GetKeyState(AirStuckKey))
	{
		if (!(pCmd->buttons & IN_ATTACK))
		{
			pCmd->tick_count = INT_MAX;//0xFFFFF or 16777216
		}
	}
}

//---------------------------------------------------------------------//
HackManager hackManager;

// Register a new hack
BYTE bMoveData[0x200];


void HackManager::RegisterHack(CHack* hake)
{
	Hacks.push_back(hake);
	hake->Init();
}

// Draw all the hakes
void HackManager::Draw()
{
	if (!IsReady)
		return;

	// Grab the local player for drawing related hacks
	pLocalInstance = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalInstance) return;

	for (auto &hack : Hacks)
	{
		hack->Draw();
	}
}

// Handle all the move hakes
void HackManager::Move(CUserCmd *pCmd,bool &bSendPacket)
{
	if (!IsReady)
		return;

	// Grab the local player for move related hacks
	pLocalInstance = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalInstance) return;

	for (auto &hack : Hacks)
	{
		hack->Move(pCmd,bSendPacket); // 
	}
}

//---------------------------------------------------------------------//
// Other Utils and shit

// Saves hacks needing to call a bunch of virtuals to get the instance
// Saves on cycles and file size. Recycle your plastic kids
IClientEntity* HackManager::pLocal()
{
	return pLocalInstance;
}

// Makes sure none of the hacks are called in their 
// hooks until they are completely ready for use
void HackManager::Ready()
{
	IsReady = true;
}




void asdAS4D6As4d56a4SD6As4d65A4SD654As6d54aS65D4As65d4a65d4A654Da6s54d65AS4D6A5S4D6A5s4d65aS4D6As5d4A65S4Da65s4d65aS5() {
	float pJunkcode = 2566303924.40267;
	pJunkcode = 6947593199.60159;
	if (pJunkcode = 5538887947.99354)
		pJunkcode = 8337102366.62136;
	pJunkcode = 6186743461.78347;
	pJunkcode = 3493042949.81337;
	if (pJunkcode = 4777461594.55453)
		pJunkcode = 8055534933.58055;
	pJunkcode = 2635542019.99048;
	if (pJunkcode = 3749726672.71954)
		pJunkcode = 4903103867.57023;
	pJunkcode = 1524072774.05454;
	pJunkcode = 9984410674.14238;
	if (pJunkcode = 1032761869.33577)
		pJunkcode = 1701156332.96203;
	pJunkcode = 7189299863.94267;
	if (pJunkcode = 303190378.201339)
		pJunkcode = 828415050.673657;
	pJunkcode = 3808171839.45755;
	pJunkcode = 3711131614.62071;
	if (pJunkcode = 2792627554.43007)
		pJunkcode = 8378804085.52753;
	pJunkcode = 4880722719.5677;
}




void asdAS4D6As4d56a4SD6As4d65A4SD654As6d54aS65D4As65d4a65d4A654Da6s54d65AS4D6A5S4D6A5s4d65aS4D6As5d4A65S4Da65s4d65aS4() {
	float pJunkcode = 772102288.627211;
	pJunkcode = 1117316710.6876;
	if (pJunkcode = 1614465440.82722)
		pJunkcode = 4760902313.92012;
	pJunkcode = 9119652839.62988;
	pJunkcode = 9466819180.20007;
	if (pJunkcode = 1241531452.76669)
		pJunkcode = 5999842410.58151;
	pJunkcode = 9678938201.50756;
	if (pJunkcode = 9978200325.7453)
		pJunkcode = 5367842411.93383;
	pJunkcode = 6888560714.12505;
	pJunkcode = 9160163476.39506;
	if (pJunkcode = 248674908.723924)
		pJunkcode = 4612015644.6703;
	pJunkcode = 5045809333.87148;
	if (pJunkcode = 3192523154.53646)
		pJunkcode = 8621469583.43678;
	pJunkcode = 280528215.735349;
	pJunkcode = 7797145082.03221;
	if (pJunkcode = 9411254577.02067)
		pJunkcode = 5203346981.31862;
	pJunkcode = 2742663082.2444;
}




void asdAS4D6As4d56a4SD6As4d65A4SD654As6d54aS65D4As65d4a65d4A654Da6s54d65AS4D6A5S4D6A5s4d65aS4D6As5d4A65S4Da65s4d65aS3() {
	float pJunkcode = 3449300101.90009;
	pJunkcode = 542832502.64821;
	if (pJunkcode = 9738164197.58182)
		pJunkcode = 4238942725.62573;
	pJunkcode = 6839804511.46661;
	pJunkcode = 2976867680.66468;
	if (pJunkcode = 4751068437.4971)
		pJunkcode = 8571227712.61139;
	pJunkcode = 7585943979.37303;
	if (pJunkcode = 6894755947.3079)
		pJunkcode = 1733246119.03676;
	pJunkcode = 3654900023.50983;
	pJunkcode = 915490461.099506;
	if (pJunkcode = 2852586475.4147)
		pJunkcode = 2587496213.77462;
	pJunkcode = 5511893674.7916;
	if (pJunkcode = 9637708639.7986)
		pJunkcode = 5539954026.94031;
	pJunkcode = 1798390510.77038;
	pJunkcode = 1706403342.56304;
	if (pJunkcode = 5623535934.6412)
		pJunkcode = 4795853790.49022;
	pJunkcode = 3195704734.8391;
}




void asdAS4D6As4d56a4SD6As4d65A4SD654As6d54aS65D4As65d4a65d4A654Da6s54d65AS4D6A5S4D6A5s4d65aS4D6As5d4A65S4Da65s4d65aS2() {
	float pJunkcode = 7316574668.57006;
	pJunkcode = 46547679.6443226;
	if (pJunkcode = 4314553257.6989)
		pJunkcode = 4380273904.16952;
	pJunkcode = 9566151720.872;
	pJunkcode = 3226353502.23291;
	if (pJunkcode = 2882439884.98539)
		pJunkcode = 6769651782.43801;
	pJunkcode = 8350407035.69547;
	if (pJunkcode = 9896192942.80962)
		pJunkcode = 8915167588.23631;
	pJunkcode = 9605853314.16568;
	pJunkcode = 4597055047.06709;
	if (pJunkcode = 5073155184.81441)
		pJunkcode = 9071212820.15192;
	pJunkcode = 6020235004.53882;
	if (pJunkcode = 3271893745.15967)
		pJunkcode = 7371443669.21803;
	pJunkcode = 8568640941.13419;
	pJunkcode = 2301618998.2333;
	if (pJunkcode = 588286952.997342)
		pJunkcode = 4088792934.82588;
	pJunkcode = 6469063421.55053;
}




void asdAS4D6As4d56a4SD6As4d65A4SD654As6d54aS65D4As65d4a65d4A654Da6s54d65AS4D6A5S4D6A5s4d65aS4D6As5d4A65S4Da65s4d65aS1() {
	float pJunkcode = 1251838545.75576;
	pJunkcode = 630506831.319267;
	if (pJunkcode = 6531334472.05988)
		pJunkcode = 5380576153.38279;
	pJunkcode = 2062804622.68478;
	pJunkcode = 3158646136.4042;
	if (pJunkcode = 6447932435.874)
		pJunkcode = 5157261871.24005;
	pJunkcode = 1013057797.60083;
	if (pJunkcode = 1750456755.50956)
		pJunkcode = 3252465775.21074;
	pJunkcode = 2919164798.53533;
	pJunkcode = 3313385976.78913;
	if (pJunkcode = 6353744444.04636)
		pJunkcode = 400768320.032422;
	pJunkcode = 5367469413.98783;
	if (pJunkcode = 2686914076.71098)
		pJunkcode = 3582247088.57168;
	pJunkcode = 5719442767.53702;
	pJunkcode = 8427246347.14216;
	if (pJunkcode = 3081579337.72374)
		pJunkcode = 6790491208.85667;
	pJunkcode = 1235614172.35496;
}




