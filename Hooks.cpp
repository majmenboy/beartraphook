/*
Syn's AyyWare Framework 2015
*/

#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "Autowall.h"
#include "CRC32.h"
#include "Resolver.h"
#include <intrin.h>
#include "Backtracking Rage.h"
#include <Windows.h>
#include "backtracking.h"
#include "EdgyLagComp.h"
#include "MemePredict.h"
#include "DLLMain.h"
#include "hitmarker.h"
#define SETANGLE 180

player_info_t GetInfo(int Index) {
	player_info_t Info;
	Interfaces::Engine->GetPlayerInfo(Index, &Info);
	return Info;
}

float Bolbilize(float Yaw)
{
	if (Yaw > 180)
	{
		Yaw -= (round(Yaw / 360) * 360.f);
	}
	else if (Yaw < -180)
	{
		Yaw += (round(Yaw / 360) * -360.f);
	}
	return Yaw;
}

Vector CalcAngle69(Vector dst, Vector src)
{
	Vector angles;

	double delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
	double hyp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	angles.x = (float)(atan(delta[2] / hyp) * 180.0 / 3.14159265);
	angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles.z = 0.0f;

	if (delta[0] >= 1.9)
	{
		angles.y += 180.0f;
	}

	return angles;
}


//Resolver.h Include --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vector LastAngleAA;
bool flipAA;
bool Resolver::didhitHS;
CUserCmd* Globals::UserCmd;
IClientEntity* Globals::Target;
int Globals::Shots;
bool Globals::change;
int Globals::choked_ticks;
int Globals::TargetID;
std::map<int, QAngle>Globals::storedshit;
int Globals::missedshots;
static ResolverSetup GetInst();
bool thirdpersonBool = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



// Funtion Typedefs
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
typedef bool(__thiscall *FireEventClientSideFn)(PVOID, IGameEvent*);
typedef void(__thiscall* RenderViewFn)(void*, CViewSetup&, CViewSetup&, int, int);

using OverrideViewFn = void(__fastcall*)(void*, void*, CViewSetup*);
typedef float(__stdcall *oGetViewModelFOV)();

#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))
// Function Pointers to the originals
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;
OverrideViewFn oOverrideView;
FireEventClientSideFn oFireEventClientSide;
RenderViewFn oRenderView;

// Hook function prototypes
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent *Event);
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __stdcall CreateMoveClient_Hooked(/*void* self, int edx,*/ float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
float __stdcall GGetViewModelFOV();
void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);

// VMT Managers
namespace Hooks
{
	// VMT Managers
	Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
	Utilities::Memory::VMTManager VMTPlaySound; // Autoaccept 
	Utilities::Memory::VMTManager VMTRenderView;
	Utilities::Memory::VMTManager VMTEventManager;
};

// Undo our hooks
void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
}


// Initialise all our hooks
void Hooks::Initialise()
{
	// Panel hooks for drawing to the screen via surface functions
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);
	//Utilities::Log("Paint Traverse Hooked");

	// No Visual Recoi	l
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);
	//Utilities::Log("InPrediction Hooked");

	// Chams
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);
	//Utilities::Log("DrawModelExecute Hooked");

	// Setup ClientMode Hooks
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)CreateMoveClient_Hooked, 24);

	oOverrideView = (OverrideViewFn)VMTClientMode.HookMethod((DWORD)&Hooked_OverrideView, 18);
	VMTClientMode.HookMethod((DWORD)&GGetViewModelFOV, 35);

	VMTEventManager.Initialise((DWORD*)Interfaces::EventManager);
	oFireEventClientSide = (FireEventClientSideFn)VMTEventManager.HookMethod((DWORD)&Hooked_FireEventClientSide, 9);

	// Setup client hooks
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod((DWORD)&Hooked_FrameStageNotify, 36);

}

int hitmarkertime = 0;
void imfinnarunuponya(IGameEvent* pEvent)
{
	int attackerid = pEvent->GetInt("attacker");
	int entityid = Interfaces::Engine->GetPlayerForUserID(attackerid);
	if (Menu::Window.VisualsTab.OtherHitmarker.GetState())
	{
		if (entityid == Interfaces::Engine->GetLocalPlayer())
		{
			hitmarkertime = 255;
			Globals::missedshots = 0;
			Globals::Shots--;
			if (Menu::Window.VisualsTab.HitmarkerSound.GetState());
			//	PlaySoundA(skeet, NULL, SND_ASYNC | SND_MEMORY);
		}
	}
}

void MovementCorrection(CUserCmd* pCmd)
{

}


//---------------------------------------------------------------------------------------------------------
//                                         Hooked Functions
//---------------------------------------------------------------------------------------------------------

void SetClanTag(const char* tag, const char* name)//190% paste
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);
}
void NoClantag()
{
	SetClanTag("", "");
}

void ClanTag()
{
	static int counter = 0;
	switch (Menu::Window.MiscTab.OtherClantag.GetIndex())
	{
	case 0:
		// No 
		break;
	case 1:
	{
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 2.5;

		if (counter % 48 == 0)
			motion++;
		int value = ServerTime % 26;
		switch (value) {
		case 0:SetClanTag("          b", "beartraphook"); break;
		case 1:SetClanTag("         b ", "beartraphook"); break;
		case 2:SetClanTag("        b e", "beartraphook"); break;
		case 3:SetClanTag("       b e ", "beartraphook"); break;
		case 4:SetClanTag("      b e a", "beartraphook"); break;
		case 5:SetClanTag("     b e a ", "beartraphook"); break;
		case 6:SetClanTag("    b e a r", "beartraphook"); break;
		case 7:SetClanTag("   b e a r ", "beartraphook"); break;
		case 8:SetClanTag("  b e a t r", "beartraphook"); break;
		case 9:SetClanTag(" a p h m e ", "beartraphook"); break;
		case 10:SetClanTag("bea r", "beartraphook"); break;
		case 11:SetClanTag("b ear  ", "beartraphook"); break;
		case 13:SetClanTag("b e ar H", "beartraphook"); break;
		case 14:SetClanTag("bea r t r H ", "beartraphook"); break;
		case 15:SetClanTag("beart r H o", "beartraphook"); break;
		case 16:SetClanTag("beartrap H o ", "beartraphook"); break;
		case 17:SetClanTag("beartraH o o", "beartraphook"); break;
		case 18:SetClanTag("beartraHo o ", "beartraphook"); break;
		case 19:SetClanTag("eartrapHoo k", "beartraphook"); break;
		case 20:SetClanTag("beartraphook ", "beartraphook"); break;
		case 21:SetClanTag("beartrapH ook ", "beartraphook"); break;
		case 22:SetClanTag("beartrapH  o", "beartraphook"); break;
		case 23:SetClanTag("beartra   pH  ", "beartraphook"); break;
		case 24:SetClanTag("    beartrapH", "beartraphook"); break;
		case 25:SetClanTag("       bear   ", "beartraphook"); break;
		case 26:SetClanTag("            b", "beartraphook"); break

			;


		}
		counter++;
	}
	break;
	case 2:
		// stainless
		SetClanTag("\r", "\r");
		break;
	case 3:
		SetClanTag("[VALV\xE1\xB4\xB1]", "Valve");
		break;
	case 4:
		time_t now = time(0);
		char timestamp[10] = "";
		strftime(timestamp, 10, "%H:%M:%S", localtime(&now));
		SetClanTag(timestamp, "Time");
		break;

	}
}


int LagCompBreak() {
	IClientEntity *pLocalPlayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	Vector velocity = pLocalPlayer->GetVelocity();
	velocity.z = 0;
	float speed = velocity.Length();
	if (speed > 0.f) {
		auto distance_per_tick = speed *
			Interfaces::Globals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, 14);
	}
	return 1;
}

void Prediction(CUserCmd* pCmd, IClientEntity* LocalPlayer)
{
	if (Interfaces::MoveHelper && Menu::Window.RageBotTab.EnginePrediction.GetState() && LocalPlayer->IsAlive())
	{
		float curtime = Interfaces::Globals->curtime;
		float frametime = Interfaces::Globals->frametime;
		int iFlags = LocalPlayer->GetFlags();

		Interfaces::Globals->curtime = (float)LocalPlayer->GetTickBase() * Interfaces::Globals->interval_per_tick;
		Interfaces::Globals->frametime = Interfaces::Globals->interval_per_tick;

		Interfaces::MoveHelper->SetHost(LocalPlayer);

		BYTE bMoveData[0x200];
		Interfaces::Prediction1->SetupMove(LocalPlayer, pCmd, nullptr, bMoveData);
		Interfaces::GameMovement->ProcessMovement(LocalPlayer, bMoveData);
		Interfaces::Prediction1->FinishMove(LocalPlayer, pCmd, bMoveData);

		Interfaces::MoveHelper->SetHost(0);

		Interfaces::Globals->curtime = curtime;
		Interfaces::Globals->frametime = frametime;
		*LocalPlayer->GetPointerFlags() = iFlags;
	}
}

void DrawAutowallCrosshair()
{
	int xs;
	int ys;
	Interfaces::Engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	float damage = 0.f;
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal->IsAlive()) {
		if (CanWallbang(damage))
		{
			Render::FilledCircle((Vector2D(xs, ys)), 35, 1.2, Color(111, 255, 15, 255));
			//Render::Text(xs - (text.right / 2), ys - 18, Color(255, 255, 255, 210), Render::Fonts::ESP, "-%.1f HP", damage);
		}
		else
		{
			Render::FilledCircle((Vector2D(xs, ys)), 35, 1.2, Color(255, 15, 15, 255));
		}
	}
}


bool __stdcall CreateMoveClient_Hooked(/*void* self, int edx,*/ float frametime, CUserCmd* pCmd)
{
	if (!pCmd->command_number)
		return true;

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pbSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pbSendPacket;

		if (Menu::Window.MiscTab.OtherClantag.GetIndex() > 0)
			ClanTag();

		//	CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
		//	CUserCmd* pCmd = &cmdlist[sequence_number % 150];


		// Backup for safety
		Vector origView = pCmd->viewangles;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
		Vector qAimAngles;
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);

		for (int i = 1; i < Interfaces::Engine->GetMaxClients(); i++) {
			IClientEntity* pBaseEntity = Interfaces::EntList->GetClientEntity(i);
			if (pBaseEntity && !pBaseEntity->IsDormant() && pBaseEntity != hackManager.pLocal()) globalsh.OldSimulationTime[i] = pBaseEntity->GetSimulationTime();
			if (pBaseEntity && !pBaseEntity->IsDormant() && pBaseEntity != hackManager.pLocal() && Menu::Window.RageBotTab.AccuracyBacktracking.GetState()) DataManager.UpdatePlayerPos();
		}

		// Do da hacks
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
			Hacks::MoveHacks(pCmd, bSendPacket);
		backtracking->legitBackTrack(pCmd, pLocal);

		if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		{
			static int ticks = 0;
			static int ticks1 = 0;
			static int iTick = 0;
			static int iTick1 = 0;
			static int iTick2 = 0;
			int ticksMax = 16;
			int value = Menu::Window.MiscTab.FakeLagChoke.GetValue();
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 2)
			{
				if (ticks >= ticksMax)
				{
					bSendPacket = true;
					ticks = 0;
				}
				else
				{
					int packetsToChoke;
					if (pLocal->GetVelocity().Length() > 0.f)
					{
						packetsToChoke = (int)((128.f / Interfaces::Globals->interval_per_tick) / pLocal->GetVelocity().Length()) + 1;
						if (packetsToChoke >= 15)
							packetsToChoke = 14;
						if (packetsToChoke < value)
							packetsToChoke = value;
					}
					else
						packetsToChoke = 0;

					bSendPacket = ticks < 18 - packetsToChoke;;
				}
				ticks++;
			}

			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 4)
			{
				if (!(pLocal->GetFlags() & FL_ONGROUND))
				{
					if (ticks1 >= ticksMax)
					{
						bSendPacket = true;
						ticks1 = 0;
					}
					else
					{
						int packetsToChoke;
						if (pLocal->GetVelocity().Length() > 0.f)
						{
							packetsToChoke = (int)((128.f / Interfaces::Globals->interval_per_tick) / pLocal->GetVelocity().Length()) + 1;
							if (packetsToChoke >= 15)
								packetsToChoke = 14;
							if (packetsToChoke < value)
								packetsToChoke = value;
						}
						else
							packetsToChoke = 0;

						bSendPacket = ticks1 < 18 - packetsToChoke;;
					}
					ticks1++;
				}
			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 1)
			{

				if (iTick < value) {
					bSendPacket = false;
					iTick++;
				}
				else {
					bSendPacket = true;
					iTick = 0;
				}
			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 3)
			{
				if (!(pLocal->GetFlags() & FL_ONGROUND))
				{
					if (iTick1 < value) {
						bSendPacket = false;
						iTick1++;
					}
					else {
						bSendPacket = true;
						iTick1 = 0;
					}
				}
			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 5)
			{
				value = LagCompBreak();
				if (iTick2 < value) {
					bSendPacket = false;
					iTick2++;
				}
				else {
					bSendPacket = true;
					iTick2 = 0;
				}
			}
		}


		if (Menu::Window.VisualsTab.NightSky.GetState())
		{
			ConVar* sky = Interfaces::CVar->FindVar("sv_skyname");
			SpoofedConvar* sky_spoofed = new SpoofedConvar(sky);
			sky_spoofed->SetString("sky_csgo_night02");
		}

		//Movement Fix
		//GameUtils::CL_FixMove(pCmd, origView);
		qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f); // if pCmd->viewangles.x > 89, set pCmd->viewangles.x instead of 0.0f on first
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
		Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
		Vector vRightNorm;			Normalize(viewright, vRightNorm);
		Vector vUpNorm;				Normalize(viewup, vUpNorm);

		// Original shit for movement correction
		float forward = pCmd->forwardmove;
		float right = pCmd->sidemove;
		float up = pCmd->upmove;
		if (forward > 450) forward = 450;
		if (right > 450) right = 450;
		if (up > 450) up = 450;
		if (forward < -450) forward = -450;
		if (right < -450) right = -450;
		if (up < -450) up = -450;
		pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
		pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
		pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

		// Angle normalisation
		if (Menu::Window.MiscTab.OtherSafeMode.GetState())
		{
			GameUtils::NormaliseViewAngle(pCmd->viewangles);

			if (pCmd->viewangles.z != 0.0f)
			{
				pCmd->viewangles.z = 0.00;
			}

			if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
			{
				Utilities::Log("Having to re-normalise!");
				GameUtils::NormaliseViewAngle(pCmd->viewangles);
				Beep(750, 800); // Why does it do this
				if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
				{
					pCmd->viewangles = origView;
					pCmd->sidemove = right;
					pCmd->forwardmove = forward;
				}
			}
		}

		if (pCmd->viewangles.x > 90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}

		if (pCmd->viewangles.x < -90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}

		switch (Menu::Window.MiscTab.OtherThirdpersonType.GetIndex())
		{
		case 0:
			if (!bSendPacket)
				LastAngleAA = pCmd->viewangles;
			break;
		case 1:
			if (bSendPacket)
				LastAngleAA = pCmd->viewangles;
			break;
		}
		//if (!bSendPacket)
		//LastAngleAA = pCmd->viewangles;

		if (bSendPacket == true) {
			lineFakeAngle = pCmd->viewangles.y;
		}
		else {
			lineRealAngle = pCmd->viewangles.y;
		}

	}

	return false;
}

std::string GetTimeString()
{
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}
std::string AYYWARE_META_CHEATVERSTRING = "0.7";
// Paint Traverse Hooked function
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	if (Menu::Window.VisualsTab.Active.GetState() && Menu::Window.VisualsTab.OtherNoScope.GetState() && strcmp("HudZoom", Interfaces::Panels->GetName(vguiPanel)) == 0)
		return;

	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	if (Menu::Window.VisualsTab.OtherPenetration.GetState()) {
		DrawAutowallCrosshair();
	}


	/*if (Menu::Window.MiscTab.OtherThirdpersonType.GetIndex == 2) {
		CBasePlayer* pLocal = Globals.g_pEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		if (pLocal)
		{
			IMaterial* mat = Globals.g_pChamsMat;
			if (mat)
			{
				Vector3D OrigAng;
				OrigAng = pLocal->GetEyeAngles();
				pLocal->SetAngle2(Vector3D(0, Chamsang.y, 0)); // paste here ur AA.y value or pLocal->GetLby() (for example)
				bool LbyColor = false; // u can make LBY INDICATOR. When LbyColor is true. Color will be Green , if false it will be White
				float NormalColor[3] = { 1, 1, 1 };
				float lbyUpdateColor[3] = { 0, 1, 0 };
				Globals.g_pEngineRenderView->SetColorModulation(LbyColor ? lbyUpdateColor : NormalColor);
				Globals.g_pModelRender->ForcedMaterialOverride(mat);
				pLocal->DrawModel(STUDIO_RENDER, 255);
				Globals.g_pModelRender->ForcedMaterialOverride(nullptr);
				pLocal->SetAngle2(OrigAng);
			}
		}
	}*/

	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, "MatSystemTopPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		
	
			Render::Textf(95, 3, Color(200, 200, 200, 255), Render::Fonts::WaterMark, ("%s"), GetTimeString().c_str());
		
			Render::Text(3, 3, Color(255, 0, 0, 255), Render::Fonts::WaterMark, ("beartrap"));
			Render::Text(56, 3, Color(200, 200, 200, 255), Render::Fonts::WaterMark, ("Hook"));
			
			Render::Text(3, 19, Color(200,200,200), Render::Fonts::WaterMark, ("Status:"));
			Render::Text(56, 19, Color(0, 200, 0), Render::Fonts::WaterMark, ("Online"));

			Render::Text(3, 33, Color(200, 200, 200), Render::Fonts::WaterMark, ("Version:"));
			Render::Text(60, 33, Color(200, 200, 200), Render::Fonts::WaterMark, (AYYWARE_META_CHEATVERSTRING.c_str()));

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
			Hacks::DrawHacks();


		if (Menu::Window.VisualsTab.lbyidicador.GetState())
		{
			CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
			CUserCmd* pCmd = cmdlist;

			IClientEntity* localplayer = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
			RECT TextSize = Render::GetTextSize(Render::Fonts::LBY, "LBY");
			RECT scrn = Render::GetViewport();
			if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
				if (pCmd->viewangles.y - *localplayer->GetLowerBodyYawTarget() >= -35 && pCmd->viewangles.y - *localplayer->GetLowerBodyYawTarget() <= 35)
					Render::Text(15, scrn.bottom - 60, Color(255, 0, 0, 255), Render::Fonts::LBY, "LBY");
				else
					Render::Text(15, scrn.bottom - 60, Color(0, 255, 0, 255), Render::Fonts::LBY, "LBY");
		}

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && Menu::Window.VisualsTab.OtherHitmarker.GetState())
			hitmarker::singleton()->on_paint();

		if (Menu::Window.VisualsTab.Active.GetState())
		{
			float AmbientRedAmount = Menu::Window.VisualsTab.AmbientRed.GetValue();
			float AmbientGreenAmount = Menu::Window.VisualsTab.AmbientGreen.GetValue();
			float AmbientBlueAmount = Menu::Window.VisualsTab.AmbientBlue.GetValue();

			ConVar* AmbientRedCvar = Interfaces::CVar->FindVar("mat_ambient_light_r");
			*(float*)((DWORD)&AmbientRedCvar->fnChangeCallback + 0xC) = NULL;
			AmbientRedCvar->SetValue(AmbientRedAmount);

			ConVar* AmbientGreenCvar = Interfaces::CVar->FindVar("mat_ambient_light_g");
			*(float*)((DWORD)&AmbientGreenCvar->fnChangeCallback + 0xC) = NULL;
			AmbientGreenCvar->SetValue(AmbientGreenAmount);

			ConVar* AmbientBlueCvar = Interfaces::CVar->FindVar("mat_ambient_light_b");
			*(float*)((DWORD)&AmbientBlueCvar->fnChangeCallback + 0xC) = NULL;
			AmbientBlueCvar->SetValue(AmbientBlueAmount);
		}

		Menu::DoUIFrame();
	}
}

// InPrediction Hooked Function
bool __stdcall Hooked_InPrediction()
{
	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD *ecxVal = Interfaces::Prediction;
	result = origFunc(ecxVal);

	// If we are in the right place where the player view is calculated
	// Calculate the change in the view and get rid of it
	if (Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)
	{
		IClientEntity* pLocalEntity = NULL;

		float* m_LocalViewAngles = NULL;

		__asm
		{
			MOV pLocalEntity, ESI
			MOV m_LocalViewAngles, EBX
		}

		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}

	return result;
}

typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void Msg(const char* msg, ...)
{

	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer[989];
	va_list list; //Normal varargs stuff http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments
	va_start(list, msg);

	vsprintf(buffer, msg, list);
	va_end(list);

	fn(buffer, list); //Calls the function, we got the address above.
}

int Kills2 = 0;
int Kills = 0;
bool RoundInfo = false;
size_t Delay = 0;
bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent *Event)
{
	if (Menu::Window.MiscTab.EnableBuyBot.GetState())
	{
		if (Menu::Window.MiscTab.BuyBot.GetIndex() == 1)
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy ak47; buy m4a1;");
		}
		else if (Menu::Window.MiscTab.BuyBot.GetIndex() == 2)
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy awp;");
		}
		else if (Menu::Window.MiscTab.BuyBot.GetIndex() == 3)
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy scar20; buy g3sg1;buy elite;");
		}
	}

	if (Menu::Window.MiscTab.EnableBuyBot.GetState())
	{
		if (Menu::Window.MiscTab.BuyBotGrenades.GetIndex() == 1)
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy flashbang; buy flashbang; buy smokegrenade; buy hegrenade;");
		}
		else if (Menu::Window.MiscTab.BuyBotGrenades.GetIndex() == 2)
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy flashbang; buy smokegrenade; buy hegrenade; buy molotov; buy incgrenade;");
		}
	}

	if (Menu::Window.MiscTab.EnableBuyBot.GetState())
	{
		if (Menu::Window.MiscTab.BuyBotKevlar.GetState())
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy vest; buy vesthelm;");
		}
	}

	if (Menu::Window.MiscTab.EnableBuyBot.GetState())
	{
		if (Menu::Window.MiscTab.BuyBotDefuser.GetState())
		{
			if (strcmp(Event->GetName(), "round_start") == 0)
				Interfaces::Engine->ClientCmd_Unrestricted("buy defuser;");
		}
	}

	if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 3)
	{
		if (!strcmp(Event->GetName(), "player_hurt"))
		{
			int deadfag = Event->GetInt("userid");
			int attackingfag = Event->GetInt("attacker");
			IClientEntity* pLocal = hackManager.pLocal();
			if (Interfaces::Engine->GetPlayerForUserID(deadfag) != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerForUserID(attackingfag) == Interfaces::Engine->GetLocalPlayer())
			{
				IClientEntity* hittedplayer = (IClientEntity*)(Interfaces::Engine->GetPlayerForUserID(deadfag));
				int hit = Event->GetInt("hitgroup");
				if (hit == 1 && hittedplayer && deadfag && attackingfag)
				{
					Resolver::didhitHS = true;
					Globals::missedshots = 0;
				}
				else
				{
					Resolver::didhitHS = false;
					Globals::missedshots++;
				}
			}
		}
	}

	if (Menu::Window.RageBotTab.FlipAA.GetState())
	{
		if (!strcmp(Event->GetName(), "player_hurt"))
		{
			int deadfag = Event->GetInt("userid");
			int attackingfag = Event->GetInt("attacker");
			IClientEntity* pLocal = hackManager.pLocal();
			if (Interfaces::Engine->GetPlayerForUserID(deadfag) == Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerForUserID(attackingfag) != Interfaces::Engine->GetLocalPlayer())
			{
				flipAA = true;

			}
			else
			{
				flipAA = false;
			}
		}
	}

	if (Menu::Window.VisualsTab.Logs.GetState())
	{
		if (!strcmp(Event->GetName(), "item_purchase"))
		{

			int nUserID = Event->GetInt("attacker");
			int nDead = Event->GetInt("userid");
			if (nUserID || nDead)
			{
				player_info_t killed_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nDead));
				player_info_t killer_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nUserID));
				std::string before = ("[beartraphook.xyz] ");
				std::string one = killed_info.name;
				std::string two = (" bought ");
				std::string three = Event->GetString("weapon");
				std::string six = "\n";
				if (Menu::Window.VisualsTab.Logs.GetState())
				{

					Msg((before + one + two + three + six).c_str());

				}

			}
		}
		if (Menu::Window.VisualsTab.Logs.GetState())
		{
			if (!strcmp(Event->GetName(), "player_hurt"))
			{

				int attackerid = Event->GetInt("attacker");
				int entityid = Interfaces::Engine->GetPlayerForUserID(attackerid);
				if (entityid == Interfaces::Engine->GetLocalPlayer())
				{

					int nUserID = Event->GetInt("attacker");
					int nDead = Event->GetInt("userid");
					if (nUserID || nDead)
					{

						player_info_t killed_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nDead));
						player_info_t killer_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nUserID));
						std::string before = ("[beartraphook.xyz] ");
						std::string two = ("Hit ");
						std::string three = killed_info.name;
						std::string foura = " for ";
						std::string fivea = Event->GetString("dmg_health");
						std::string damage = " damage";
						std::string fourb = " (";
						std::string fiveb = Event->GetString("health");
						std::string six = " health remaining)";
						std::string newline = "\n";
						if (Menu::Window.VisualsTab.Logs.GetState())
						{
							Msg((before + two + three + foura + fivea + damage + fourb + fiveb + six + newline).c_str());
						}

					}
				}

			}

		}
	}

	if (Menu::Window.MiscTab.TrashTalk.GetState()) {
		if (strcmp(Event->GetName(), "round_end") == 0) {
			RoundInfo = true;
			Delay = GetTickCount();
		}
		else if ((strcmp(Event->GetName(), "game_newmap") == 0) || (strcmp(Event->GetName(), "game_start") == 0) || (strcmp(Event->GetName(), "game_end") == 0)) {
			Kills = 0;
			Kills2 = 0;
		}
		else if (strcmp(Event->GetName(), "player_connect") == 0) {
			int PlayerID = Interfaces::Engine->GetPlayerForUserID(Event->GetInt("userid"));

			player_info_t PlayerInfo;
			if (Interfaces::Engine->GetPlayerInfo(PlayerID, &PlayerInfo)) {
				char Buffer[128];
				sprintf(Buffer, "say Hello, %s, i hope you will ragequit.", PlayerInfo.name);
				Interfaces::Engine->ClientCmd(Buffer);
			}
		}
		else if ((strcmp(Event->GetName(), "player_death") == 0) || (strcmp(Event->GetName(), "entity_killed") == 0)) {
			int PlayerID = Interfaces::Engine->GetPlayerForUserID(Event->GetInt("userid"));
			int AttackerID = Interfaces::Engine->GetPlayerForUserID(Event->GetInt("attacker"));

			player_info_t PlayerInfo;
			player_info_t AttackerInfo;

			if (Interfaces::Engine->GetPlayerInfo(PlayerID, &PlayerInfo) &&
				Interfaces::Engine->GetPlayerInfo(AttackerID, &AttackerInfo)) {

				if (PlayerID == Interfaces::Engine->GetLocalPlayer())
					Kills = 0;

				if (PlayerID != Interfaces::Engine->GetLocalPlayer() && AttackerID == Interfaces::Engine->GetLocalPlayer()) {
					Kills++;
					Kills2++;

					char Buffer[4];

					switch (Kills) {
					case 1: sprintf(Buffer, "say   1", PlayerInfo.name); break;
					default: sprintf(Buffer, "say   1", PlayerInfo.name); break;
					}

					Interfaces::Engine->ClientCmd(Buffer);


				}
			}
		}
	}
	return oFireEventClientSide(ECX, Event);
}


// DrawModelExec for chams and shit
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld) //cry for cry <3
{
	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = CreateMaterial(true);
	static IMaterial* OpenLit = CreateMaterial(false);
	static IMaterial* CoveredFlat = CreateMaterial(true, false);
	static IMaterial* OpenFlat = CreateMaterial(false, false);
	static IMaterial* Chrome = CreateMaterial("$envmap env_cube");
	bool DontDraw = false;
	const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
	IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Menu::Window.VisualsTab.Active.GetState())
	{

		if (Menu::Window.VisualsTab.Seethrough.GetState()) {
			IClientEntity *pLocal = hackManager.pLocal();
			if (pModelEntity == pLocal && pLocal->IsScoped() && Menu::Window.MiscTab.OtherThirdpersonType.GetIndex() == 0)
				Interfaces::RenderView->SetBlend(0.3);
			else
				Interfaces::RenderView->SetBlend(1);
		}

		int ChamsStyle = Menu::Window.VisualsTab.OptionsChams.GetIndex();
		int HandsStyle = Menu::Window.VisualsTab.OtherNoHands.GetIndex();

		if (Menu::Window.VisualsTab.FiltersPlayers.GetState() && strstr(ModelName, "models/player"))
		{

			if (pLocal && pModelEntity && ChamsStyle != 0)
			{
				if ((!Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() || pModelEntity->GetTeamNum() != pLocal->GetTeamNum()))
				{
					IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
					IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 /*&& pModelEntity->GetTeamNum() != local->GetTeamNum()*/)
					{
						float alpha = 1.f;

						if (pModelEntity->HasGunGameImmunity())
							alpha = 0.5f;

						if (pModelEntity->GetTeamNum() == 2)
						{
							flColor[0] = Menu::Window.ColorsTab.TChamsR.GetValue() / 255.f;
							flColor[1] = Menu::Window.ColorsTab.TChamsG.GetValue() / 255.f;
							flColor[2] = Menu::Window.ColorsTab.TChamsB.GetValue() / 255.f;
						}
						else
						{
							flColor[0] = Menu::Window.ColorsTab.CTChamsR.GetValue() / 255.f;
							flColor[1] = Menu::Window.ColorsTab.CTChamsG.GetValue() / 255.f;
							flColor[2] = Menu::Window.ColorsTab.CTChamsB.GetValue() / 255.f;
						}

						Interfaces::RenderView->SetColorModulation(flColor);
						Interfaces::RenderView->SetBlend(alpha);
						Interfaces::ModelRender->ForcedMaterialOverride(covered);
						oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

						if (pModelEntity->GetTeamNum() == 2)
						{
							flColor[0] = Menu::Window.ColorsTab.TChamsR.GetValue() / 255.f;
							flColor[1] = Menu::Window.ColorsTab.TChamsG.GetValue() / 255.f;
							flColor[2] = Menu::Window.ColorsTab.TChamsB.GetValue() / 255.f;
						}
						else
						{
							flColor[0] = Menu::Window.ColorsTab.CTChamsR.GetValue() / 255.f;
							flColor[1] = Menu::Window.ColorsTab.CTChamsG.GetValue() / 255.f;
							flColor[2] = Menu::Window.ColorsTab.CTChamsB.GetValue() / 255.f;
						}

						Interfaces::RenderView->SetColorModulation(flColor);
						Interfaces::RenderView->SetBlend(alpha);
						Interfaces::ModelRender->ForcedMaterialOverride(open);
					}
					else
					{
						color.SetColor(255, 255, 255, 255);
						ForceMaterial(color, open);
					}
				}
			}
		}
		else if (strstr(ModelName, "arms"))
		{
			if (HandsStyle != 0 && pLocal && pLocal->IsAlive())
			{
				if (HandsStyle == 1)
				{
					DontDraw = true;
				}
				else if (HandsStyle == 2)
				{
					Interfaces::RenderView->SetBlend(0.3);
				}
				else if (HandsStyle == 3)
				{
					IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
					IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

					int alpha = pLocal->HasGunGameImmunity() ? 150 : 255;

					if (pLocal->GetTeamNum() == 2)
						color.SetColor(0, 255, 255, alpha);
					else
						color.SetColor(0, 255, 255, alpha);

					ForceMaterial(color, covered);
					oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

					if (pLocal->GetTeamNum() == 2)
						color.SetColor(0, 255, 255, alpha);
					else
						color.SetColor(0, 255, 255, alpha);

					ForceMaterial(color, open);
				}
				else
				{
					static int counter = 0;
					static float colors[3] = { 1.f, 0.f, 0.f };

					if (colors[counter] >= 1.0f)
					{
						colors[counter] = 1.0f;
						counter += 1;
						if (counter > 2)
							counter = 0;
					}
					else
					{
						int prev = counter - 1;
						if (prev < 0) prev = 2;
						colors[prev] -= 0.05f;
						colors[counter] += 0.05f;
					}

					Interfaces::RenderView->SetColorModulation(colors);
					Interfaces::RenderView->SetBlend(0.3);
					Interfaces::ModelRender->ForcedMaterialOverride(OpenLit);
				}
			}
		}
		else if (Menu::Window.VisualsTab.FiltersWeapons.GetState() && strstr(ModelName, "_dropped.mdl"))
		{
			if (ChamsStyle != 0)
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;

				color.SetColor(255, 255, 255, 255);
				ForceMaterial(color, covered);
			}
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}


int RandomInt(int min, int max)
{
	return rand() % max + min;
}

// Hooked FrameStageNotify for removing visual recoil
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	DWORD eyeangles = NetVar.GetNetVar(0xBFEA4E7B);
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	IClientEntity* pEntity;

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_RENDER_START)
	{

		if (*(bool*)((DWORD)Interfaces::pInput + 0xA5))
			*(Vector*)((DWORD)pLocal + 0x31C8) = LastAngleAA;

		int thirdpersonKey = Menu::Window.MiscTab.OtherThirdpersonKey.GetKey();
		if (thirdpersonKey > 0 && GUI.GetKeyPress(thirdpersonKey))
		{
			thirdpersonBool = !thirdpersonBool;
		}
/*
		static bool nightmod = false;
		if (Menu::Window.VisualsTab.FiltersNightMod.GetState())

		if (Menu::Window.VisualsTab.OtherNightMode.GetState())
		{
			if (!nightmod)
			{
				ConVar* staticdrop = Interfaces::CVar->FindVar("r_DrawSpecificStaticProp");
				SpoofedConvar* staticdrop_spoofed = new SpoofedConvar(staticdrop);
				staticdrop_spoofed->SetInt(0);

				{
					for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						float amount = Menu::Window.VisualsTab.NightMod.GetValue(); // yes i called it nigga mode. BITE ME

						if (!strcmp(pMaterial->GetTextureGroupName(), "World textures"))  // walls
						{
							pMaterial->ColorModulation(amount, amount, amount);
						}

						if (!strcmp(pMaterial->GetTextureGroupName(), "StaticProp textures"))
						{
							pMaterial->ColorModulation(amount, amount, amount);   // values of boxes

						}
					}
				}
			}
			nightmod = true; // Update for anti fps suicide (?)
			Menu::Window.VisualsTab.OtherNightMode.SetState(false);
		}
		else
		{
			nightmod = false;
		}*/

		int FlipKey = Menu::Window.RageBotTab.FlipKey.GetKey();
		if (FlipKey > 0 && GUI.GetKeyPress(FlipKey))
		{
			flipBool = !flipBool;
		}

		if (thirdpersonBool)
		{
			static bool rekt = false;
			if (!rekt)
			{
				ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
				SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
				sv_cheats_spoofed->SetInt(1);
				rekt = true;
			}
		}
		static bool rekt1 = false;
		if (thirdpersonBool && pLocal->IsAlive() && pLocal->IsScoped() == 0)
		{
			if (!rekt1)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				rekt1 = true;
			}
		}
		else if (!thirdpersonBool)
		{
			rekt1 = false;
		}

		static bool rekt = false;
		if (!thirdpersonBool || pLocal->IsAlive() == 0)
		{
			if (!rekt)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("firstperson");
				rekt = true;
			}
		}
		else if (thirdpersonBool || pLocal->IsAlive() || pLocal->IsScoped() == 0)
		{
			rekt = false;
		}

		static bool meme = false;
		if (thirdpersonBool && pLocal->IsScoped() == 0)
		{
			if (!meme)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				meme = true;
			}
		}
		else if (pLocal->IsScoped())
		{
			meme = false;
		}

		static bool kek = false;
		if (thirdpersonBool && pLocal->IsAlive())
		{
			if (!kek)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				kek = true;
			}
		}
		else if (pLocal->IsAlive() == 0)
		{
			kek = false;
		}

	}

	if (Menu::Window.RageBotTab.PVSFix.GetState())
	{
		for (int i = 1; i < Interfaces::Globals->maxClients; i++)
		{
			if (i == Interfaces::Engine->GetLocalPlayer()) continue;
			IClientEntity* pCurEntity = Interfaces::EntList->GetClientEntity(i);
			if (!pCurEntity) continue;
			*(int*)((uintptr_t)pCurEntity + 0xA30) = Interfaces::Globals->framecount;
			*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;
		}
	}

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		{

			for (int i = 1; i < 65; i++)
			{
				if (pLocal->IsAlive())
				{
					IClientEntity* pEnt = Interfaces::EntList->GetClientEntity(i);
					if (!pEnt) continue;
					if (pEnt->IsDormant()) continue;
					if (pEnt->GetHealth() < 1) continue;
					if (pEnt->GetLifeState() != 0) continue;

					static bool isMoving;
					float PlayerIsMoving = abs(pEnt->GetVelocity().Length());
					if (PlayerIsMoving > 36) isMoving = true;
					else if (PlayerIsMoving <= 36) isMoving = false;

					bool MeetsLBYReq;
					if (pEnt->GetFlags() & FL_ONGROUND)
						MeetsLBYReq = true;
					else
						MeetsLBYReq = false;

					float newsimtime;
					float storedsimtime;
					bool lbyupdated;
					float storedlbyFGE;
					float storedanglesFGE;
					float storedsimtimeFGE;
					bool didhitHS;
					float newlby;
					float newdelta;
					float newlbydelta;
					float finaldelta;
					float finallbydelta;
					float storedlby[64];
					float storeddelta[64];
					float storedlbydelta[64];
					float StoredAngles[64];
					float StoredLBY[65];

					newlby = pEnt->GetLowerBodyYaw();
					newsimtime = pEnt->GetSimulationTime();
					newdelta = *(float*)((DWORD)pEnt + eyeangles);
					newlbydelta = pEnt->GetLowerBodyYaw();
					finaldelta = newdelta - storeddelta[pEnt->GetIndex()];
					finallbydelta = newlbydelta - storedlbydelta[pEnt->GetIndex()];
					StoredAngles[pEnt->GetIndex()] = *(float*)((DWORD)pEnt + eyeangles);
					storedlby[pEnt->GetIndex()] = pEnt->GetLowerBodyYaw();
					storedlbydelta[pEnt->GetIndex()] = pEnt->GetLowerBodyYaw();
					storedsimtime = pEnt->GetSimulationTime();
					storeddelta[pEnt->GetIndex()] = *(float*)((DWORD)pEnt + eyeangles);
					storedanglesFGE = *(float*)((DWORD)pEnt + eyeangles);
					storedlbyFGE = pEnt->GetLowerBodyYaw();
					storedsimtimeFGE = pEnt->GetSimulationTime();

					if (newlby == *storedlby)
						lbyupdated = false;
					else
						lbyupdated = true;


					if (Menu::Window.RageBotTab.LowerbodyFix.GetState()) {
						static float time_at_update[65];
						int index = pEnt->GetIndex();
						float simTime_since_lby_update = pEnt->GetSimulationTime() - time_at_update[index];

						Vector reset = Vector(0, 0, 0); reset.y = pEnt->GetEyeAnglesPointer()->y;
						static float LatestLowerBodyYawUpdateTime[55];
						float lby = pEnt->GetLowerBodyYaw();
						static bool bLowerBodyIsUpdated;
						if (pEnt->GetLowerBodyYaw() != StoredLBY[pEnt->GetIndex()]) bLowerBodyIsUpdated = true;
						else bLowerBodyIsUpdated = false;
						float bodyeyedelta = *(float*)((DWORD)pEnt + eyeangles) - pEnt->GetLowerBodyYaw();
						float curTime_since_lby_update = LatestLowerBodyYawUpdateTime[pEnt->GetIndex()] - Interfaces::Globals->curtime;
						float LastUpdatedLBY = 0.f;
						bool bClientMoving = false;
						if (pEnt->GetVelocity().Length2D() > 0.1f) bClientMoving = true;
						else bClientMoving = false;
						float flLastUpdateTime = LatestLowerBodyYawUpdateTime[pEnt->GetIndex()] - Interfaces::Globals->curtime;

						if (bLowerBodyIsUpdated || isMoving || fabsf(bodyeyedelta) >= 35.0f) // i have gay
						{
							*(float*)((DWORD)pEnt + eyeangles) = pEnt->GetLowerBodyYaw();
							LatestLowerBodyYawUpdateTime[pEnt->GetIndex()] = Interfaces::Globals->curtime;
							StoredLBY[pEnt->GetIndex()] = pEnt->GetLowerBodyYaw();
						}
						else if (Globals::missedshots >= 2) // dont know why i didn't put this in the statement below
						{
							*(float*)((DWORD)pEnt + eyeangles) = lby + SETANGLE;
						}
						else if ((curTime_since_lby_update) >= 1.1 || simTime_since_lby_update >= 1.1f || (1.1 - flLastUpdateTime) < 0.2) // if lby updated or (for some reason) is more than 1.1 seconds since updated
						{
							*(float*)((DWORD)pEnt + eyeangles) = lby + SETANGLE;
						}
						else if (simTime_since_lby_update <= 0.57f) // if it wasnt long ago that lby updated, just check nearby
						{
							*(float*)((DWORD)pEnt + eyeangles) = lby + bodyeyedelta; // meme, need a replacement
						}
						else
						{
							switch (Globals::Shots % 3) // smart attarget bruteforce that will hit (almost) any static angle after just 3 shots (thanks bolbi)
							{
							case 0: *(float*)((DWORD)pEnt + eyeangles) = Bolbilize(CalcAngle69(pEnt->GetVecOrigin(), pLocal->GetVecOrigin()).y + 70); break;
							case 1: *(float*)((DWORD)pEnt + eyeangles) = Bolbilize(CalcAngle69(pEnt->GetVecOrigin(), pLocal->GetVecOrigin()).y + SETANGLE); break;
							case 2: *(float*)((DWORD)pEnt + eyeangles) = Bolbilize(CalcAngle69(pEnt->GetVecOrigin(), pLocal->GetVecOrigin()).y - 70); break;
							}
						}
					}
				}
			}
		}
	}



	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (Menu::Window.SkinchangerTab.SkinEnable.GetState() && pLocal)
		{
			IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

			int iBayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
			int iButterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
			int iFlip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
			int iGut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
			int iKarambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
			int iM9Bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
			int iHuntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
			int iFalchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
			int iDagger = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
			int iBowie = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");

			int iGunGame = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

			for (int i = 0; i <= Interfaces::EntList->GetHighestEntityIndex(); i++)
			{

				IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

				IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

				CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)WeaponEnt;

				if (pEntity)
				{
					ULONG hOwnerEntity = *(PULONG)((DWORD)pEntity + 0x148);

					IClientEntity* pOwner = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)hOwnerEntity);

					if (pOwner)
					{
						if (pOwner == pLocal)
						{
							std::string sWeapon = Interfaces::ModelInfo->GetModelName(pEntity->GetModel());

							auto weps = pLocal->Weapons();
							for (size_t i = 0; weps[i] != nullptr; i++) {
								auto pWeapons = reinterpret_cast<CBaseCombatWeapon*>(Interfaces::EntList->GetClientEntityFromHandle(weps[i]));
							}
							if (sWeapon.find("c4_planted", 0) != std::string::npos)
								continue;

							if (sWeapon.find("thrown", 0) != std::string::npos)
								continue;

							if (sWeapon.find("smokegrenade", 0) != std::string::npos)
								continue;

							if (sWeapon.find("flashbang", 0) != std::string::npos)
								continue;

							if (sWeapon.find("fraggrenade", 0) != std::string::npos)
								continue;

							if (sWeapon.find("molotov", 0) != std::string::npos)
								continue;

							if (sWeapon.find("decoy", 0) != std::string::npos)
								continue;

							if (sWeapon.find("incendiarygrenade", 0) != std::string::npos)
								continue;

							if (sWeapon.find("ied", 0) != std::string::npos)
								continue;

							if (sWeapon.find("w_eq_", 0) != std::string::npos)
								continue;

							CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)pEntity;

							ClientClass *pClass = Interfaces::Client->GetAllClasses();









							if (Menu::Window.SkinchangerTab.SkinEnable.GetState())
							{
								IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

								static bool bUpdate = false;

								int* hMyWearables = pLocal->GetWearables();

								if (!hMyWearables)
									return;

								if (!Interfaces::EntList->GetClientEntity(hMyWearables[0] & 0xFFF))
								{
									for (ClientClass *pClass = Interfaces::Client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
									{
										if (pClass->m_ClassID != (int)CSGOClassID::CEconWearable)
											continue;

										int entry = (Interfaces::EntList->GetHighestEntityIndex() + 1), serial = RandomInt(0x0, 0xFFF);
										pClass->m_pCreateFn(entry, serial);
										hMyWearables[0] = entry | (serial << 16); //crash

										bUpdate = true;
										break;
									}


									player_info_t LocalPlayerInfo;
									Interfaces::Engine->GetPlayerInfo(Interfaces::Engine->GetLocalPlayer(), &LocalPlayerInfo);
								}
								CBaseCombatWeapon* pWeapon_g = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntity(pLocal->GetWearables()[0] & 0xFFF);



								int Glove_model = Menu::Window.SkinchangerTab.GloveModel.GetIndex();
								int Glove_skin = Menu::Window.SkinchangerTab.GloveSkin.GetIndex();

								if (!pWeapon_g)
									return;

								if (bUpdate) {
									if (Glove_model == 0)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;

										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
										pWeapon_g->PreDataUpdate(0);

										if (Glove_skin != 0 && Glove_skin != 1 && Glove_skin != 2)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10006;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 0)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10006;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 1)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10007;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 2)
										{
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10008;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}

									}
									if (Glove_model == 1)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
										pWeapon_g->PreDataUpdate(0);

										if (Glove_skin != 3 && Glove_skin != 4 && Glove_skin != 5 && Glove_skin != 6)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10037;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 3)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10037;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}

										else if (Glove_skin == 4)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10038;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}

										else if (Glove_skin == 5)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10018;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}

										else if (Glove_skin == 6)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10019;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
									}
									if (Glove_model == 2)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
										pWeapon_g->PreDataUpdate(0);

										if (Glove_skin != 7 && Glove_skin != 8 && Glove_skin != 9)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10013;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 7)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10013;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 8)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10015;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 9)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10016;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
									}
									if (Glove_model == 3)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
										pWeapon_g->PreDataUpdate(0);
										if (Glove_skin != 10 && Glove_skin != 11 && Glove_skin != 12 && Glove_skin != 13)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10030;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 10)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10030;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 11)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10033;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 12)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10034;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 13)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10035;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
									}
									if (Glove_model == 4)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
										pWeapon_g->PreDataUpdate(0);
										if (Glove_skin != 14 && Glove_skin != 15 && Glove_skin != 16 && Glove_skin != 17)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10024;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 14)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10026;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 15)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10027;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 16)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10028;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 17)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10024;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
									}
									if (Glove_model == 5)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
										pWeapon_g->PreDataUpdate(0);
										if (Glove_skin != 18 && Glove_skin != 19 && Glove_skin != 20 && Glove_skin != 21)
										{

											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10009;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 18)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10009;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 19)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10010;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 20)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10021;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
										else if (Glove_skin == 21)
										{


											*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
											*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
											*pWeapon_g->FallbackPaintKit() = 10036;
											((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
											pWeapon_g->PreDataUpdate(0);
										}
									}

									/*	*pWeapon_g->ModelIndex() = sporty; // m_nModelIndex
									*pWeapon_g->ViewModelIndex() = sporty;
									*pWeapon_g->WorldModelIndex() = sporty + 1;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
									*pWeapon_g->FallbackPaintKit() = 10018;
									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
									pWeapon_g->PreDataUpdate(0);*/

								}
							}







							if (Menu::Window.SkinchangerTab.SkinEnable.GetState())
							{
								int Model = Menu::Window.SkinchangerTab.KnifeModel.GetIndex();

								int M41S = Menu::Window.SkinchangerTab.M41SSkin.GetIndex();
								int M4A4 = Menu::Window.SkinchangerTab.M4A4Skin.GetIndex();
								int AK47 = Menu::Window.SkinchangerTab.AK47Skin.GetIndex();
								int AWP = Menu::Window.SkinchangerTab.AWPSkin.GetIndex();
								int GLOCK = Menu::Window.SkinchangerTab.GLOCKSkin.GetIndex();
								int USPS = Menu::Window.SkinchangerTab.USPSSkin.GetIndex();
								int DEAGLE = Menu::Window.SkinchangerTab.DEAGLESkin.GetIndex();
								int FIVE7 = Menu::Window.SkinchangerTab.FIVESEVENSkin.GetIndex();
								int AUG = Menu::Window.SkinchangerTab.AUGSkin.GetIndex();
								int FAMAS = Menu::Window.SkinchangerTab.FAMASSkin.GetIndex();
								int G3SG1 = Menu::Window.SkinchangerTab.G3SG1Skin.GetIndex();
								int Galil = Menu::Window.SkinchangerTab.GALILSkin.GetIndex();
								int M249 = Menu::Window.SkinchangerTab.M249Skin.GetIndex();
								int MAC10 = Menu::Window.SkinchangerTab.MAC10Skin.GetIndex();
								int P90 = Menu::Window.SkinchangerTab.P90Skin.GetIndex();
								int UMP45 = Menu::Window.SkinchangerTab.UMP45Skin.GetIndex();
								int XM1014 = Menu::Window.SkinchangerTab.XM1014Skin.GetIndex();
								int BIZON = Menu::Window.SkinchangerTab.BIZONSkin.GetIndex();
								int MAG7 = Menu::Window.SkinchangerTab.MAG7Skin.GetIndex();
								int NEGEV = Menu::Window.SkinchangerTab.NEGEVSkin.GetIndex();
								int SAWEDOFF = Menu::Window.SkinchangerTab.SAWEDOFFSkin.GetIndex();
								int TEC9 = Menu::Window.SkinchangerTab.TECNINESkin.GetIndex();
								int P2000 = Menu::Window.SkinchangerTab.P2000Skin.GetIndex();
								int MP7 = Menu::Window.SkinchangerTab.MP7Skin.GetIndex();
								int MP9 = Menu::Window.SkinchangerTab.MP9Skin.GetIndex();
								int NOVA = Menu::Window.SkinchangerTab.NOVASkin.GetIndex();
								int P250 = Menu::Window.SkinchangerTab.P250Skin.GetIndex();
								int SCAR20 = Menu::Window.SkinchangerTab.SCAR20Skin.GetIndex();
								int SG553 = Menu::Window.SkinchangerTab.SG553Skin.GetIndex();
								int SSG08 = Menu::Window.SkinchangerTab.SSG08Skin.GetIndex();
								int Magnum = Menu::Window.SkinchangerTab.DEAGLESkin.GetIndex();
								int DUAL = Menu::Window.SkinchangerTab.DUALSSkin.GetIndex();


								int weapon = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();

								switch (weapon)
								{
								case 7: // AK47 
								{
									switch (AK47)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 341;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 14;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 44;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 172;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 180;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 394;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 300;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 226;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 282;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 302;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 316;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 340;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 380;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 422;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 456;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 474;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 490;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 506;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 524;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 600;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 675;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 597;
										break;
									default:
										break;
									}
								}
								break;
								case 16: // M4A4
								{
									switch (M4A4)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 155;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 187;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 255;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 309;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 215;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 336;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 384;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 400;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 449;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 471;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 480;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 512;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 533;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 588;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 632;
										break;
									default:
										break;
									}
								}
								break;
								case 60: // M4A1-S
								{
									switch (M41S)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 60;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 430;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 77;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 254;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 189;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 301;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 217;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 257;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 321;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 326;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 360;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 383;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 440;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 445;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 497;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 548;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 587;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 631;
										break;
									default:
										break;
									}
								}
								break;
								case 9: // Awp
								{
									switch (AWP)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 174;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 344;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 84;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 30;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 51;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 72;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 181;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 259;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 395;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 212;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 227;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 251;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 279;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 424;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 446;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 451;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 475;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 525;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 640;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 662;
										break;
									default:
										break;
									}
								}
								break;
								case 61: // Usp-s
								{
									switch (USPS)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 25;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 60;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 183;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 339;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 217;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 221;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 236;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 277;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 290;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 313;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 318;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 332;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 364;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 454;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 489;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 504;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 540;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 637;
										break;
									default:
										break;
									}
								}
								break;
								case 4: // Glock
								{
									switch (GLOCK)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 2;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 3;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 38;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 40;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 48;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 437;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 99;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 159;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 399;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 208;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 230;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 278;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 293;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 353;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 367;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 381;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 479;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 495;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 532;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 607;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 586;
										break;
									default:
										break;
									}
								}
								break;
								case 1: // Deagle
								{
									switch (DEAGLE)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 37;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 347;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 468;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 469;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 232;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 17;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 40;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 61;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 90;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 185;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 231;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 237;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 397;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 328;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 273;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 296;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 351;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 425;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 470;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 509;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 527;
										break;
									default:
										break;
									}
								}
								break;
								case 2: // Dual Berettas
								{
									switch (DUAL)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 28;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 36;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 43;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 46;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 47;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 153;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 491;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 190;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 248;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 249;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 220;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 396;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 261;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 276;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 307;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 330;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 447;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 450;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 528;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 544;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 625;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 658;
										break;
									default:
										break;
									}
								}
								break;
								case 3: // Five Seven
								{
									switch (FIVE7)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 3;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 44;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 46;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 78;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 141;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 151;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 254;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 248;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 210;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 223;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 252;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 265;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 274;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 464;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 352;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 377;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 387;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 427;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 510;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 530;
										break;
									case 21: // Capillary
										*pWeapon->FallbackPaintKit() = 646;
										break;
									case 22: // Hyper Beast
										*pWeapon->FallbackPaintKit() = 660;
										break;
									default:
										break;
									}
								}
								break;
								case 8: // AUG
								{
									switch (AUG)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 9;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 33;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 280;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 305;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 375;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 442;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 444;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 455;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 507;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 541;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 601;
										break;
									default:
										break;
									}
								}
								break;
								case 10: // Famas
								{
									switch (FAMAS)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 22;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 47;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 92;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 429;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 154;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 178;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 194;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 244;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 218;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 260;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 288;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 371;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 477;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 492;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 529;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 604;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 626;
										break;
									default:
										break;
									}
								}
								break;
								case 11: // G3SG1
								{
									switch (G3SG1)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 8;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 6;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 46;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 72;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 74;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 147;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 170;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 195;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 229;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 294;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 465;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 464;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 382;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 438;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 493;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 511;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 545;
										break;
									default:
										break;
									}
								}
								break;
								case 13: // Galil
								{
									switch (Galil)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 22;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 83;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 428;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 76;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 119;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 398;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 192;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 308;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 216;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 237;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 241;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 264;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 297;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 379;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 460;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 478;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 494;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 546;
										break;
									default:
										break;
									}
								}
								break;
								case 14: // M249
								{
									switch (M249)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 22;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 75;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 202;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 243;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 266;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 401;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 452;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 472;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 496;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 547;
										break;
									default:
										break;
									}
								}
								break;
								case 17: // Mac 10
								{
									switch (MAC10)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 101;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 3;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 32;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 17;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 38;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 433;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 98;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 157;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 188;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 337;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 246;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 284;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 310;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 333;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 343;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 372;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 402;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 498;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 534;
										break;
									default:
										break;
									}
								}
								break;
								case 19: // P90
								{
									switch (P90)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 342;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 20;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 22;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 100;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 67;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 111;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 124;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 156;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 234;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 169;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 175;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 182;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 244;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 228;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 283;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 311;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 335;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 359;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 486;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 516;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 636;
										break;
									default:
										break;
									}
								}
								break;
								case 24: // UMP-45
								{
									switch (UMP45)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 37;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 15;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 17;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 436;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 70;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 93;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 169;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 175;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 193;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 392;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 281;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 333;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 362;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 441;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 488;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 556;
										break;
									default:
										break;
									}
								}
								break;
								case 25: // XM1014
								{
									switch (XM1014)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 166;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 238;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 42;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 96;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 95;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 135;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 151;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 169;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 205;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 240;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 251;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 393;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 320;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 314;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 348;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 370;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 407;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 505;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 521;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 557;
										break;
									default:
										break;
									}
								}
								break;
								case 26: // Bizon
								{
									switch (BIZON)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 13;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 164;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 25;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 70;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 148;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 149;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 159;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 171;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 203;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 224;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 236;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 267;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 306;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 323;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 349;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 376;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 457;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 459;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 508;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 526;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 542;
										break;
									default:
										break;
									}
								}
								break;
								case 27: // Mag 7
								{
									switch (MAG7)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 462;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 34;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 32;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 100;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 39;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 431;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 99;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 171;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 177;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 198;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 291;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 385;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 473;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 499;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 535;
										break;
									default:
										break;
									}
								}
								break;
								case 28: // Negev
								{
									switch (NEGEV)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 28;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 432;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 157;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 201;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 240;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 285;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 298;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 317;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 355;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 369;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 483;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 514;
										break;
									default:
										break;
									}
								}
								break;
								case 29: // Sawed Off
								{
									switch (SAWEDOFF)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 345;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 22;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 30;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 83;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 38;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 41;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 434;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 119;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 171;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 204;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 405;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 246;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 250;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 390;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 256;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 323;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 458;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 459;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 517;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 552;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 638;
										break;
									default:
										break;
									}
								}
								break;
								case 30: // Tec 9
								{
									switch (TEC9)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 101;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 2;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 463;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 17;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 36;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 439;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 159;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 179;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 248;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 206;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 216;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 242;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 272;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 289;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 303;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 374;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 459;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 520;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 539;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 555;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 614;
										break;
									default:
										break;
									}
								}
								break;
								case 32: // P2000
								{
									switch (P2000)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 104;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 32;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 21;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 25;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 36;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 485;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 38;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 71;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 95;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 184;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 211;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 338;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 246;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 275;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 327;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 346;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 357;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 389;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 442;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 443;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 515;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 550;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 591;
										break;
									default:
										break;
									}
								}
								break;
								case 33: // MP7
								{
									switch (MP7)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 2;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 102;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 5;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 28;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 11;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 15;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 22;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 36;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 141;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 235;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 245;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 209;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 213;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 250;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 354;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 365;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 423;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 442;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 481;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 500;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 536;
										break;
									default:
										break;
									}
								}
								break;
								case 34: // MP9
								{
									switch (MP9)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 482;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 33;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 100;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 39;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 61;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 148;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 141;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 199;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 329;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 262;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 366;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 368;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 386;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 403;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 448;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 549;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 609;
										break;
									default:
										break;
									}
								}
								break;
								case 35: // Nova
								{
									switch (NOVA)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 3;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 166;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 164;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 25;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 62;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 99;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 107;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 158;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 170;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 191;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 214;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 225;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 263;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 286;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 294;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 299;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 356;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 450;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 484;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 537;
										break;
									default:
										break;
									}
								}
								break;
								case 36: // P250
								{
									switch (P250)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 102;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 34;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 162;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 15;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 164;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 77;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 99;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 168;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 258;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 207;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 219;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 404;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 230;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 271;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 295;
										break;
									case 16:
										*pWeapon->FallbackPaintKit() = 464;
										break;
									case 17:
										*pWeapon->FallbackPaintKit() = 358;
										break;
									case 18:
										*pWeapon->FallbackPaintKit() = 373;
										break;
									case 19:
										*pWeapon->FallbackPaintKit() = 388;
										break;
									case 20:
										*pWeapon->FallbackPaintKit() = 426;
										break;
									case 21:
										*pWeapon->FallbackPaintKit() = 466;
										break;
									case 22:
										*pWeapon->FallbackPaintKit() = 467;
										break;
									case 23:
										*pWeapon->FallbackPaintKit() = 501;
										break;
									case 24:
										*pWeapon->FallbackPaintKit() = 551;
										break;
									default:
										break;
									}
								}
								break;
								case 38: // Scar 20
								{
									switch (SCAR20)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 165;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 100;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 46;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 70;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 116;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 157;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 196;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 232;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 391;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 298;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 312;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 406;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 453;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 502;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 518;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 597;
										break;
									default:
										break;
									}
								}
								break;
								case 39: // SG553
								{
									switch (SG553)
									{
									case 1:
										*pWeapon->FallbackPaintKit() = 39;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 98;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 410;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 347;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 287;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 298;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 363;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 378;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 487;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 519;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 553;
										break;
									default:
										break;
									}
								}
								break;
								case 40: // SSG08
								{
									switch (SSG08)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 26;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 60;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 96;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 99;
										break;
									case 4:
										*pWeapon->FallbackPaintKit() = 157;
										break;
									case 5:
										*pWeapon->FallbackPaintKit() = 200;
										break;
									case 6:
										*pWeapon->FallbackPaintKit() = 222;
										break;
									case 7:
										*pWeapon->FallbackPaintKit() = 233;
										break;
									case 8:
										*pWeapon->FallbackPaintKit() = 253;
										break;
									case 9:
										*pWeapon->FallbackPaintKit() = 304;
										break;
									case 10:
										*pWeapon->FallbackPaintKit() = 319;
										break;
									case 11:
										*pWeapon->FallbackPaintKit() = 361;
										break;
									case 12:
										*pWeapon->FallbackPaintKit() = 503;
										break;
									case 13:
										*pWeapon->FallbackPaintKit() = 538;
										break;
									case 14:
										*pWeapon->FallbackPaintKit() = 554;
										break;
									case 15:
										*pWeapon->FallbackPaintKit() = 624;
										break;
									default:
										break;
									}
								}
								break;
								case 64: // Revolver
								{
									switch (Magnum)
									{
									case 0:
										*pWeapon->FallbackPaintKit() = 27;
										break;
									case 1:
										*pWeapon->FallbackPaintKit() = 12;
										break;
									case 2:
										*pWeapon->FallbackPaintKit() = 522;
										break;
									case 3:
										*pWeapon->FallbackPaintKit() = 523;
										break;
									default:
										break;
									}
								}
								break;
								default:
									break;
								}



								if (pEntity->GetClientClass()->m_ClassID == (int)CSGOClassID::CKnife)
								{
									if (Model == 0) // Bayonet
									{
										*pWeapon->ModelIndex() = iBayonet; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iBayonet;
										*pWeapon->WorldModelIndex() = iBayonet + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 500;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 558; // Lore
										}
									}
									else if (Model == 1) // Bowie Knife
									{
										*pWeapon->ModelIndex() = iBowie; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iBowie;
										*pWeapon->WorldModelIndex() = iBowie + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 514;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 558; // Lore
										}

									}
									else if (Model == 2) // Butterfly Knife
									{
										*pWeapon->ModelIndex() = iButterfly; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iButterfly;
										*pWeapon->WorldModelIndex() = iButterfly + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 515;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 558; // Lore
										}

									}
									else if (Model == 3) // Falchion Knife
									{
										*pWeapon->ModelIndex() = iFalchion; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iFalchion;
										*pWeapon->WorldModelIndex() = iFalchion + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 512;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 558; // Lore
										}
									}
									else if (Model == 4) // Flip Knife
									{
										*pWeapon->ModelIndex() = iFlip; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iFlip;
										*pWeapon->WorldModelIndex() = iFlip + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 505;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 559; // Lore
										}

									}
									else if (Model == 5) // Gut Knife
									{
										*pWeapon->ModelIndex() = iGut; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iGut;
										*pWeapon->WorldModelIndex() = iGut + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 506;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 560; // Lore
										}


									}
									else if (Model == 6) // Huntsman Knife
									{
										*pWeapon->ModelIndex() = iHuntsman; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iHuntsman;
										*pWeapon->WorldModelIndex() = iHuntsman + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 509;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 559; // Lore
										}


									}
									else if (Model == 7) // Karambit
									{
										*pWeapon->ModelIndex() = iKarambit; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iKarambit;
										*pWeapon->WorldModelIndex() = iKarambit + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 507;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 570; // Doppler Phase 4
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 568; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 561; // Lore
										}

									}
									else if (Model == 8) // M9 Bayonet
									{
										*pWeapon->ModelIndex() = iM9Bayonet; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iM9Bayonet;
										*pWeapon->WorldModelIndex() = iM9Bayonet + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 508;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 570; // Doppler Phase 4
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 568; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 562; // Lore
										}

									}


									else if (Model == 10) // Shadow Daggers
									{
										*pWeapon->ModelIndex() = iDagger; // m_nModelIndex
										*pWeapon->ViewModelIndex() = iDagger;
										*pWeapon->WorldModelIndex() = iDagger + 1;
										*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 516;

										int Skin = Menu::Window.SkinchangerTab.KnifeSkin.GetIndex();

										if (Skin == 0)
										{
											*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
										}
										else if (Skin == 1)
										{
											*pWeapon->FallbackPaintKit() = 12; // Crimson Web
										}
										else if (Skin == 2)
										{
											*pWeapon->FallbackPaintKit() = 27; // Bone Mask
										}
										else if (Skin == 3)
										{
											*pWeapon->FallbackPaintKit() = 38; // Fade
										}
										else if (Skin == 4)
										{
											*pWeapon->FallbackPaintKit() = 40; // Night
										}
										else if (Skin == 5)
										{
											*pWeapon->FallbackPaintKit() = 42; // Blue Steel
										}
										else if (Skin == 6)
										{
											*pWeapon->FallbackPaintKit() = 43; // Stained
										}
										else if (Skin == 7)
										{
											*pWeapon->FallbackPaintKit() = 44; // Case Hardened
										}
										else if (Skin == 8)
										{
											*pWeapon->FallbackPaintKit() = 59; // Slaughter
										}
										else if (Skin == 9)
										{
											*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
										}
										else if (Skin == 10)
										{
											*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
										}
										else if (Skin == 11)
										{
											*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
										}
										else if (Skin == 12)
										{
											*pWeapon->FallbackPaintKit() = 143; // Urban Masked
										}
										else if (Skin == 13)
										{
											*pWeapon->FallbackPaintKit() = 175; // Scorched
										}
										else if (Skin == 14)
										{
											*pWeapon->FallbackPaintKit() = 323; // Rust Coat
										}
										else if (Skin == 15)
										{
											*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
										}
										else if (Skin == 16)
										{
											*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
										}
										else if (Skin == 17)
										{
											*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
										}
										else if (Skin == 18)
										{
											*pWeapon->FallbackPaintKit() = 413; // Marble Fade
										}
										else if (Skin == 19)
										{
											*pWeapon->FallbackPaintKit() = 414; // Rust Coat
										}
										else if (Skin == 20)
										{
											*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
										}
										else if (Skin == 21)
										{
											*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
										}
										else if (Skin == 22)
										{
											*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
										}
										else if (Skin == 23)
										{
											*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
										}
										else if (Skin == 24)
										{
											*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
										}
										else if (Skin == 25)
										{
											*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
										}
										else if (Skin == 26)
										{
											*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
										}
										else if (Skin == 27)
										{
											*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
										}
										else if (Skin == 28)
										{
											*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
										}
										else if (Skin == 29)
										{
											*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
										}
										else if (Skin == 30)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
										}
										else if (Skin == 31)
										{
											*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
										}
										else if (Skin == 32)
										{
											*pWeapon->FallbackPaintKit() = 561; // Lore
										}
									}
								}

								*pWeapon->OwnerXuidLow() = 0;
								*pWeapon->OwnerXuidHigh() = 0;
								*pWeapon->FallbackWear() = 0.001f;
								*pWeapon->m_AttributeManager()->m_Item()->ItemIDHigh() = 1;

							}
						}
					}
				}
			}
		}
	}
	oFrameStageNotify(curStage);
}


void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())


	{
		if (Menu::Window.VisualsTab.Active.GetState() && pLocal->IsAlive() && !pLocal->IsScoped())
		{
			if (pSetup->fov = 90)
				pSetup->fov = Menu::Window.VisualsTab.OtherFOV.GetValue();


		}

		oOverrideView(ecx, edx, pSetup);
	}

}

void GetViewModelFOV(float& fov)
{
	IClientEntity* localplayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		if (!localplayer)
			return;


		if (Menu::Window.VisualsTab.Active.GetState())
			fov += Menu::Window.VisualsTab.OtherViewmodelFOV.GetValue();
	}
}

float __stdcall GGetViewModelFOV()
{
	float fov = Hooks::VMTClientMode.GetMethod<oGetViewModelFOV>(35)();

	GetViewModelFOV(fov);

	return fov;
}

void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	static DWORD oRenderView = Hooks::VMTRenderView.GetOriginalFunction(6);

	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	__asm
	{
		PUSH whatToDraw
		PUSH nClearFlags
		PUSH hudViewSetup
		PUSH setup
		MOV ECX, ecx
		CALL oRenderView
	}
} //hooked for no reason yay

bool sendpacket = true;

void FakeLag(CUserCmd* pCmd)
{

	IClientEntity* pLocal = hackManager.pLocal();

	int choke = Menu::Window.MiscTab.FakeLagChoke.GetValue() + 1;
	if (choke > 15)
		choke = 14;

	if (Menu::Window.MiscTab.FakeLagEnable.GetState())
	{

		static int FakelagTick;
		FakelagTick++;
		int Mode = Menu::Window.MiscTab.FakeLagTyp.GetIndex();
		if (Mode == 0) sendpacket = Menu::Window.MiscTab.FakeLagEnable.GetState() ? !(FakelagTick % (choke)) : 1;
		if (Mode == 1)
		{
			static int FakelagFactor = 2;
			sendpacket = Menu::Window.MiscTab.FakeLagTyp.GetIndex() ? !(FakelagTick % (FakelagFactor + 1)) : 1;
			if (sendpacket)
				FakelagFactor = max(1, min((int)(fabs(pLocal->GetVelocity().Length2D() / 80.f)), 12));
		}
		if (Mode == 2)
		{
			static int FakelagFactor = 8;
			sendpacket = Menu::Window.MiscTab.FakeLagTyp.GetIndex() ? !(FakelagTick % (FakelagFactor + 1)) : 1;
			if (sendpacket) {
				FakelagFactor = (rand() % 7) + 6;
			}
		}
		if (Mode == 3)
		{
			static int FakelagFactor = Menu::Window.MiscTab.FakeLagChoke.GetValue();
			sendpacket = Menu::Window.MiscTab.FakeLagTyp.GetIndex() ? !(FakelagTick % (FakelagFactor + 1)) : 1;
			if (sendpacket) {
				FakelagFactor = (rand() % 10) + 1;
			}
		}
		if (Mode == 4)
		{
			static int FakelagFactor = 5;
			int Based = FakelagFactor / 3;
			sendpacket = Menu::Window.MiscTab.FakeLagTyp.GetIndex() ? !(FakelagTick % (Based)) : 1;
			if (sendpacket) {
				FakelagFactor += 1;
				if (Based >= Menu::Window.MiscTab.FakeLagChoke.GetValue()) FakelagFactor = 3;
			}
		}
		if (Mode == 5)
		{
			static int FakelagFactor = 10;
			static int m_iOldHealth = 0;
			static int iChoked;
			if (m_iOldHealth != pLocal->GetHealth()) {
				m_iOldHealth = pLocal->GetHealth();
				FakelagFactor = rand() % 7 + 6 + 1;
			}
			sendpacket = Menu::Window.MiscTab.FakeLagTyp.GetIndex() ? !(FakelagTick % (FakelagFactor + 1)) : 1;
			if (!sendpacket)
				iChoked++;
			else iChoked = 0;

			if (iChoked > 14) {
				sendpacket = 1;
				FakelagTick = -1;
			}
		}
	}
}