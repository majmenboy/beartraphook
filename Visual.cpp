#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"
#define _USE_MATH_DEFINES
#include <cmath> 

void CVisuals::Init()
{
}

void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

void CVisuals::Draw()
{
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Menu::Window.VisualsTab.OtherSpreadCrosshair.GetState())
		SpreadCrosshair();

	if (Menu::Window.VisualsTab.OtherNoScope.GetState() && pLocal->IsAlive() && pLocal->IsScoped())
		NoScopeCrosshair();

	if (Menu::Window.VisualsTab.OtherPlayerLine.GetState())
		bighak();

	switch (Menu::Window.VisualsTab.OtherCrosshair.GetIndex())
	{
	case 0:
		Interfaces::Engine->ClientCmd_Unrestricted("crosshair 1");
		break;
	case 1:
		Interfaces::Engine->ClientCmd_Unrestricted("crosshair 1");
		DrawRecoilCrosshair();
		break;
	case 2:
		Interfaces::Engine->ClientCmd_Unrestricted("crosshair 0");
		DrawCrosshair();
		break;
	case 3:
	//	SpreadCrosshair();
		break;
	}
 

	if (Menu::Window.VisualsTab.SniperCrosshair.GetState())
		DefaultCrosshair();
}


void CVisuals::NoScopeCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;

	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (GameUtils::IsSniper(pWeapon))
	{
		Render::Line(MidX - 1000, MidY, MidX + 1000, MidY, Color(0, 0, 0, 255));
		Render::Line(MidX, MidY - 1000, MidX, MidY + 1000, Color(0, 0, 0, 255));
	} 
}

void CVisuals::DrawCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;

	Render::Line(MidX - 3, MidY, MidX + 3, MidY, Color(169, 0, 0, 255));
	Render::Line(MidX, MidY - 3, MidX, MidY + 3, Color(169, 0, 0, 255));
}

void CVisuals::DrawRecoilCrosshair()
{
	IClientEntity *pLocal = hackManager.pLocal();

	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;

	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;

	if (Render::WorldToScreen(end, endScreen) && pLocal->IsAlive())
	{
		Render::Line(endScreen.x - 3, endScreen.y, endScreen.x + 3, endScreen.y, Color(169, 0, 0, 255));
		Render::Line(endScreen.x, endScreen.y - 3, endScreen.x, endScreen.y + 3, Color(169, 0, 0, 255));
	}
}

void CVisuals::bighak()
{
	/*	IClientEntity *pLocal = hackManager.pLocal();
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = pLocal;
	AngleVectors(QAngle(0, pLocal->GetLowerBodyYaw(), 0), &forward);
	src3D = pLocal->GetOrigin();
	dst3D = src3D + (forward * 35.f); //replace 50 with the length you want the line to have

	ray.Init(src3D, dst3D);

	Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

	if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
	return;

	Render::Line(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
	Render::Text(dst.x, dst.y, Color(255, 165.f, 0, 255), Render::Fonts::ESP, "LBY");*/

	IClientEntity *pLocal = hackManager.pLocal();

	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = hackManager.pLocal();

	AngleVectors(QAngle(0, pLocal->GetLowerBodyYaw(), 0), &forward);
	src3D = hackManager.pLocal()->GetOrigin();
	dst3D = src3D + (forward * 35.f); //replace 50 with the length you want the line to have

	ray.Init(src3D, dst3D);

	Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

	if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
		return;

	Render::Line(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
	Render::Text(dst.x, dst.y, Color(255, 165.f, 0, 255), Render::Fonts::ESP, "lby");

	AngleVectors(QAngle(0, lineRealAngle, 0), &forward);
	dst3D = src3D + (forward * 40.f); //replace 50 with the length you want the line to have

	ray.Init(src3D, dst3D);

	Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

	if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
		return;

	Render::Line(src.x, src.y, dst.x, dst.y, Color(0, 255, 0, 255));
	Render::Text(dst.x, dst.y, Color(0, 255.f, 0, 255), Render::Fonts::ESP, "real");

	AngleVectors(QAngle(0, lineFakeAngle, 0), &forward);
	dst3D = src3D + (forward * 45.f); //replace 50 with the length you want the line to have

	ray.Init(src3D, dst3D);

	Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

	if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
		return;

	Render::Line(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
	Render::Text(dst.x, dst.y, Color(255, 0.f, 0, 255), Render::Fonts::ESP, "fake");
}

void CVisuals::DefaultCrosshair()
{
	IClientEntity *pLocal = hackManager.pLocal();

	if (!pLocal->IsScoped() && pLocal->IsAlive())
	{
		ConVar* cross = Interfaces::CVar->FindVar("weapon_debug_spread_show");
		SpoofedConvar* cross_spoofed = new SpoofedConvar(cross);
		cross_spoofed->SetInt(3);
	}
}

void CVisuals::DrawFOVCrosshair()
{
	int xs;
	int ys;
	float FoV;
	std::vector<int> HitBoxesToScan;
	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pWeapon)
		return;

	if (GameUtils::IsPistol(pWeapon))
	{
		FoV = Menu::Window.LegitBotTab.WeaponPistFoV.GetValue();

	}
	else if (GameUtils::IsSniper(pWeapon))
	{
		FoV = Menu::Window.LegitBotTab.WeaponSnipFoV.GetValue();

	}
	else if (GameUtils::IsRifle)
	{
		FoV = Menu::Window.LegitBotTab.WeaponMainFoV.GetValue();
	}
	else if (GameUtils::IsShotgun)
	{
		FoV = Menu::Window.LegitBotTab.WeaponShotgunFoV.GetValue();
	}
	else if (GameUtils::IsMP)
	{
		FoV = Menu::Window.LegitBotTab.WeaponMpFoV.GetValue();
	}
	else if (GameUtils::IsMachinegun)
	{
		FoV = Menu::Window.LegitBotTab.WeaponMGFoV.GetValue();
	}

	Interfaces::Engine->GetScreenSize(xs, ys);
	Interfaces::Engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	Render::DrawLine(xs, ys, FoV * 6.25, FoV * 6.25, Color(0, 255, 255, 255));
}

void CVisuals::SpreadCrosshair()
{
	IClientEntity *pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!hackManager.pLocal()->IsAlive())
		return;

	if (!GameUtils::IsBallisticWeapon(pWeapon))
		return;

	if (pWeapon == nullptr)
		return;

	int xs;
	int ys;
	Interfaces::Engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;

	auto accuracy = pWeapon->GetInaccuracy() * 550.f;

	Render::FilledCircle((Vector2D(xs, ys)), 35, accuracy, Color(20, 20, 20, 135));

	if (pLocal->IsAlive())
	{
		if (pWeapon)
		{
			float inaccuracy = pWeapon->GetInaccuracy() * 1000;
			char buffer4[64];
			//sprintf_s(buffer4, "Inaccuracy:  %f", inaccuracy);
			//Render::Text(xs + accuracy + 4, ys, Color(255, 255, 255, 255), Render::Fonts::ESP, buffer4);
		}
	}
	else
	{

	}

}
