#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include "Hooks.h"
#include <iostream>
#include "UTIL Functions.h"

#define TICK_INTERVAL			( Interfaces::Globals->interval_per_tick )
#define TIME_TO_TICKS( dt )	( ( int )( 0.5f + ( float )( dt ) / Interfaces::Globals->interval_per_tick ) )
#define TICKS_TO_TIME( t ) ( Interfaces::Globals->interval_per_tick *( t ) )

void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

float hitchance(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	float hitchance = 75;
	if (!pWeapon) return 0;
	if (Menu::Window.RageBotTab.AccuracyHitchance.GetValue() > 1)
	{
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;

	}
	return hitchance;
}

void AngleVectors3(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

float RandomNumber(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

float InterpolationFix()
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	return max(cl_interp, cl_interp_ratio / cl_updaterate);
}

bool CanOpenFire()
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return;

	if (!Menu::Window.RageBotTab.Active.GetState())
		return;

	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);
			}
			ChokedPackets = -1;
		}
	}

	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
		pCmd->tick_count = TIME_TO_TICKS(InterpolationFix());
	    PositionAdjustment(pCmd);

	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}

	LastAngle = pCmd->viewangles;
}

Vector BestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* pLocal = hackManager.pLocal();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;
}

void CRageBot::PositionAdjustment(CUserCmd* pCmd)
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();

	if (!pLocal)
		return;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (!pWeapon)
		return;

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	float flSimTime = pLocal->GetSimulationTime();
	float flOldAnimTime = pLocal->GetAnimTime();

	int iTargetTickDiff = (int)(0.5f + (flSimTime - flOldAnimTime) / Interfaces::Globals->interval_per_tick);
}


void CRageBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	bool FindNewTarget = true;

	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			return;
		}
	}
	else
		return;

	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View;
				Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					FindNewTarget = false;
			}
		}
	}

	if (GameUtils::IsRevolver(pWeapon) && Menu::Window.RageBotTab.AutoRevolver.GetState())
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			pCmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}

	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		case 3:
			TargetID = GetTargetThreat(pCmd);
			break;
		case 4:
			TargetID = GetTargetNextShot();
			break;
		}

		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	Globals::Target = pTarget;
	Globals::TargetID = TargetID;

	if (TargetID >= 0 && pTarget)
	{
		HitBox = HitScan(pTarget);

		if (!CanOpenFire())
			return;

		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		int StopKey = Menu::Window.RageBotTab.AimbotStopKey.GetKey();
		if (StopKey >= 0 && GUI.GetKeyState(StopKey))
		{
			TargetID = -1;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}

		float pointscale = Menu::Window.RageBotTab.TargetPointscale.GetValue() - 5.f;

		Vector Point;
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox) + Vector(0, 0, pointscale);

		if (Menu::Window.RageBotTab.Accuracylagcomp.GetState())
			pCmd->tick_count = TIME_TO_TICKS(InterpolationFix());
		    PositionAdjustment(pCmd);
		    StartLagCompensation(pLocal, pCmd);

		if (Menu::Window.RageBotTab.TargetMultipoint.GetState())
		{
			Point = BestPoint(pTarget, AimPoint);
		}
		else
		{
			Point = AimPoint;
		}

		if (GameUtils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) // Autoscope
		{
			pCmd->buttons |= IN_ATTACK2;
		}
		else
		{
			if ((Menu::Window.RageBotTab.AccuracyHitchance.GetValue() * 1.5 <= hitchance(pLocal, pWeapon)) || Menu::Window.RageBotTab.AccuracyHitchance.GetValue() == 0 || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(pLocal, Point, pCmd, bSendPacket))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{
						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (IsAbleToShoot(pLocal) && pCmd->buttons & IN_ATTACK)
			Globals::Shots += 1;
		if (Menu::Window.RageBotTab.AccuracyBacktracking.GetState()) pCmd->tick_count += TICKS_TO_TIME(Interfaces::Globals->interval_per_tick);
	}


if (Menu::Window.RageBotTab.AutoMinimumDamage.GetState())
{
	if (GameUtils::IsSSG08(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(85);
	}

	else if (GameUtils::IsAK47(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsDEAGLE(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(30);
	}

	else if (GameUtils::IsAutoSniper(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsDuals(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsMP9(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsMP7(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsM4A1(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsM4A1S(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsFAMAS(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsMAC10(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsAWP(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(85);
	}

	else if (GameUtils::IsBizon(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsMP7(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsMP9(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsUMP(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsUSPS(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsGALIL(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

	else if (GameUtils::IsGLOCK(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsFSEVEN(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsP250(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}

	else if (GameUtils::IsTEC9(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(3);
	}
	else if (GameUtils::IsRevolver(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(40);
	}
	else if (GameUtils::IsCZ75(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(5);
	}

}
else
{
	Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue();
}

if (Menu::Window.RageBotTab.AutoHitChance.GetState())
{
	if (GameUtils::IsSSG08(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(85);
	}

	else if (GameUtils::IsAK47(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(30);
	}

	else if (GameUtils::IsDEAGLE(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(70);
	}

	else if (GameUtils::IsAutoSniper(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(50);
	}

	else if (GameUtils::IsDuals(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(25);
	}

	else if (GameUtils::IsMP9(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(10);
	}

	else if (GameUtils::IsMP7(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(10);
	}

	else if (GameUtils::IsM4A1(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(30);
	}

	else if (GameUtils::IsM4A1S(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(30);
	}

	else if (GameUtils::IsFAMAS(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(30);
	}

	else if (GameUtils::IsMAC10(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(10);
	}

	else if (GameUtils::IsAWP(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(90);
	}

	else if (GameUtils::IsBizon(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(10);
	}

	else if (GameUtils::IsMP7(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(10);
	}

	else if (GameUtils::IsMP9(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(10);
	}

	else if (GameUtils::IsUMP(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(20);
	}

	else if (GameUtils::IsUSPS(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(65);
	}

	else if (GameUtils::IsGALIL(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(30);
	}

	else if (GameUtils::IsGLOCK(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(5);
	}

	else if (GameUtils::IsFSEVEN(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(20);
	}

	else if (GameUtils::IsP250(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(20);
	}

	else if (GameUtils::IsTEC9(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(20);
	}
	else if (GameUtils::IsRevolver(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(60);
	}
	else if (GameUtils::IsCZ75(pWeapon))
	{
		Menu::Window.RageBotTab.AccuracyHitchance.SetValue(40);
	}

}
else
{
	Menu::Window.RageBotTab.AccuracyHitchance.GetValue();
}

} //posunout if add



bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	CONST FLOAT MaxDegrees = 180.0f;

	Vector Angles = View;

	Vector Origin = ViewOffSet;

	Vector Delta(0, 0, 0);

	Vector Forward(0, 0, 0);

	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);

	VectorSubtract(AimPos, Origin, Delta);

	Normalize(Delta, Delta);

	FLOAT DotProduct = Forward.Dot(Delta);

	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetNextShot()
{
	int target = -1;
	int minfov = 361;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minfov && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minfov = fov;
					target = i;
				}
				else
					minfov = 361;
			}
		}
	}

	return target;
}

float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	Vector ang, aim;

	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}

double inline __declspec (naked) __fastcall FASTSQRT(double n)
{
	_asm fld qword ptr[esp + 4]
		_asm fsqrt
	_asm ret 8
}

float VectorDistance(Vector v1, Vector v2)
{
	return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

int CRageBot::GetTargetThreat(CUserCmd* pCmd)
{
	auto iBestTarget = -1;
	float flDistance = 8192.f;

	IClientEntity* pLocal = hackManager.pLocal();

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			auto vecHitbox = pEntity->GetBonePos(NewHitBox);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				QAngle TempTargetAbs;
				CalcAngle(pLocal->GetEyePosition(), vecHitbox, TempTargetAbs);
				float flTempFOVs = GetFov(pCmd->viewangles, TempTargetAbs);
				float flTempDistance = VectorDistance(pLocal->GetOrigin(), pEntity->GetOrigin());
				if (flTempDistance < flDistance && flTempFOVs < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					flDistance = flTempDistance;
					iBestTarget = i;
				}
			}
		}
	}
	return iBestTarget;
}

int CRageBot::GetTargetHealth()
{
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int huso = (pEntity->GetHealth());
	int health = Menu::Window.RageBotTab.BaimIfUnderXHealth.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();
	int TargetHitbox = Menu::Window.RageBotTab.TargetHitbox.GetIndex();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	int iSmart = Menu::Window.RageBotTab.AccuracySmart.GetValue();

	if (iSmart > 0 && pLocal->GetShotsFired() + 1 > iSmart)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
	}
	else
	{
		if (HitScanMode == 0)
		{
			switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
			{
			case 0:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				break;
			case 1:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
				break;
			case 2:
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
				break;
			case 3:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				break;
			case 4:
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
				break;
			}
		}

		else if (Menu::Window.RageBotTab.AWPAtBody.GetState() && GameUtils::AWP(pWeapon))
		{
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
		}

		else if (Menu::Window.RageBotTab.PreferBodyAim.GetState())
		{
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
		}
		else
		{
			switch (HitScanMode)
			{
			case 1:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			case 2:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			case 3:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
			case 4:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			}
		}
	}
#pragma endregion Get the list of shit to scan

	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				c = Color(0, 255, 0, 255);
				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamageAWall.GetValue())
				{
					return HitBoxID;
				}

				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{
					return HitBoxID;
				}

			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

void CRageBot::aimAtPlayer(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (!pLocal || !pWeapon)
		return;

	Vector eye_position = pLocal->GetEyePosition();

	float best_dist = pWeapon->GetCSWpnData()->flRange;

	IClientEntity* target = nullptr;

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			if (Globals::TargetID != -1)
				target = Interfaces::EntList->GetClientEntity(Globals::TargetID);
			else
				target = pEntity;

			Vector target_position = target->GetEyePosition();

			float temp_dist = eye_position.DistTo(target_position);

			if (best_dist > temp_dist)
			{
				best_dist = temp_dist;
				CalcAngle(eye_position, target_position, pCmd->viewangles);
			}
		}
	}
}

void CRageBot::StartLagCompensation(IClientEntity* pEntity, CUserCmd* pCmd)
{
	float flSimTime = pEntity->GetSimulationTime();
	pCmd->tick_count = TIME_TO_TICKS(flSimTime + 0.031f);
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	bool ReturnValue = false;

	if (point.Length() == 0) return ReturnValue;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	}


	IsLocked = true;

	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle;

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}

	switch (Menu::Window.RageBotTab.AimbotSilentAim.GetIndex())
	{

	case 0: /*Disabled*/
		Interfaces::Engine->SetViewAngles(angles);
		break;

	case 1: /*Client*/
		pCmd->viewangles = angles;
		break;

	case 2:
		// pSilent Aim
		CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
		QAngle oldView = pCmd->viewangles;
		float oldSidemove = pCmd->sidemove;
		float oldForwardmove = pCmd->forwardmove;
		float flServerTime = (float)pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

		static int ChokedPackets = -1;
		ChokedPackets++;

		if (ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles = angles;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = oldView;
			pCmd->sidemove = oldSidemove;
			pCmd->forwardmove = oldForwardmove;
			ChokedPackets = -1;
			ReturnValue = false;
		}
	}
	return ReturnValue;
}

float CurrentVelocity(IClientEntity* LocalPlayer)
{
	int vel = LocalPlayer->GetVelocity().Length2D();
	return vel;
}

float GetLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float Latency = nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING);
		return Latency;
	}
	else
	{
		return 0.0f;
	}
}

float OldLBY;
float LBYBreakerTimer;
float LastLBYUpdateTime;
bool bSwitch;

bool NextLBYUpdate()
{
	IClientEntity* LocalPlayer = hackManager.pLocal();

	float flServerTime = (float)(LocalPlayer->GetTickBase()  * Interfaces::Globals->interval_per_tick);


	if (OldLBY != LocalPlayer->GetLowerBodyYaw())
	{
		LBYBreakerTimer++;
		OldLBY = LocalPlayer->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (CurrentVelocity(LocalPlayer) > 0.5)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}

	if ((LastLBYUpdateTime + 1 - (GetLatency() * 2) < flServerTime) && (LocalPlayer->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 1.1 - (GetLatency() * 2) < flServerTime)
		{
			LastLBYUpdateTime += 1.1;
		}
		return true;
	}
	return false;
}



namespace AntiAims
{
	static float pDance = 0.0f;

	void JitterPitch(CUserCmd *pCmd)
	{
		pDance += 45.0f;
		if (pDance > 100)
			pDance = 0.0f;
		else if (pDance > 75.f)
			pCmd->viewangles.x = -89.f;
		else if (pDance < 75.f)
			pCmd->viewangles.x = 89.f;
	}
	void AngelDown(CUserCmd *pCmd)
	{
		{
			pCmd->viewangles.x = 36000088.0f;

		}
	}
	void AngelUp(CUserCmd *pCmd)
	{
		{
			pCmd->viewangles.x = 35999912.0f;

		}
	}

	void FakePitch(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = 89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 51;
			ChokedPackets = -1;
		}
	}

	enum ADAPTIVE_SIDE {
		ADAPTIVE_UNKNOWN,
		ADAPTIVE_LEFT,
		ADAPTIVE_RIGHT
	};

	enum ADAPTIVE_SIDE2 {
		ADAPTIVE_UNKNOWN2,
		ADAPTIVE_LEFT2,
		ADAPTIVE_RIGHT2
	};

	void adaptive2(CUserCmd * pCmd, bool& bSendPacket) {
		auto fov_to_player = [](Vector view_offset, Vector view, IClientEntity* m_entity, int hitbox)
		{
			CONST FLOAT MaxDegrees = 180.0f;
			Vector Angles = view;
			Vector Origin = view_offset;
			Vector Delta(0, 0, 0);
			Vector Forward(0, 0, 0);
			AngleVectors3(Angles, &Forward);
			Vector AimPos = GetHitboxPosition(m_entity, hitbox);
			VectorSubtract(AimPos, Origin, Delta);
			Normalize(Delta, Delta);
			FLOAT DotProduct = Forward.Dot(Delta);
			return (acos(DotProduct) * (MaxDegrees / PI));
		};

		auto m_local = hackManager.pLocal();

		int target = -1;
		float mfov = 20;

		Vector viewoffset = m_local->GetOrigin() + m_local->GetViewOffset();
		Vector view; Interfaces::Engine->GetViewAngles(view);

		for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++) {
			IClientEntity* m_entity = Interfaces::EntList->GetClientEntity(i);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {

				float fov = fov_to_player(viewoffset, view, m_entity, 0);
				if (fov < mfov) {
					mfov = fov;
					target = i;
				}
			}
		}

		ADAPTIVE_SIDE2 side = ADAPTIVE_UNKNOWN2;

		Vector at_target_angle;

		if (target) {
			auto m_entity = Interfaces::EntList->GetClientEntity(target);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {
				Vector pos_enemy;
				if (Render::WorldToScreen(m_entity->GetOrigin(), pos_enemy)) {
					CalcAngle(m_local->GetOrigin(), m_entity->GetOrigin(), at_target_angle);

					POINT mouse = GUI.GetMouse();

					if (mouse.x > pos_enemy.x) side = ADAPTIVE_RIGHT2;
					else if (mouse.x < pos_enemy.x) side = ADAPTIVE_LEFT2;
					else side = ADAPTIVE_UNKNOWN2;
				}
			}
		}

		if (side == ADAPTIVE_RIGHT) {
			pCmd->viewangles.y = at_target_angle.y + 89;
		}
		else if (side == ADAPTIVE_LEFT) {
			pCmd->viewangles.y = at_target_angle.y - 88;
		}

		if (side == ADAPTIVE_UNKNOWN) {
			pCmd->viewangles.y -= 180;
		}
	}


	void adaptive(CUserCmd * pCmd, bool& bSendPacket) {
		auto fov_to_player = [](Vector view_offset, Vector view, IClientEntity* m_entity, int hitbox)
		{
			CONST FLOAT MaxDegrees = 180.0f;
			Vector Angles = view;
			Vector Origin = view_offset;
			Vector Delta(0, 0, 0);
			Vector Forward(0, 0, 0);
			AngleVectors3(Angles, &Forward);
			Vector AimPos = GetHitboxPosition(m_entity, hitbox);
			VectorSubtract(AimPos, Origin, Delta);
			Normalize(Delta, Delta);
			FLOAT DotProduct = Forward.Dot(Delta);
			return (acos(DotProduct) * (MaxDegrees / PI));
		};

		auto m_local = hackManager.pLocal();

		int target = -1;
		float mfov = 20;

		Vector viewoffset = m_local->GetOrigin() + m_local->GetViewOffset();
		Vector view; Interfaces::Engine->GetViewAngles(view);

		for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++) {
			IClientEntity* m_entity = Interfaces::EntList->GetClientEntity(i);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {

				float fov = fov_to_player(viewoffset, view, m_entity, 0);
				if (fov < mfov) {
					mfov = fov;
					target = i;
				}
			}
		}

		ADAPTIVE_SIDE side = ADAPTIVE_UNKNOWN;

		Vector at_target_angle;

		if (target) {
			auto m_entity = Interfaces::EntList->GetClientEntity(target);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {
				Vector pos_enemy;
				if (Render::WorldToScreen(m_entity->GetOrigin(), pos_enemy)) {
					CalcAngle(m_local->GetOrigin(), m_entity->GetOrigin(), at_target_angle);

					POINT mouse = GUI.GetMouse();

					if (mouse.x > pos_enemy.x) side = ADAPTIVE_RIGHT;
					else if (mouse.x < pos_enemy.x) side = ADAPTIVE_LEFT;
					else side = ADAPTIVE_UNKNOWN;
				}
			}
		}

		if (side == ADAPTIVE_RIGHT) {
			pCmd->viewangles.y = at_target_angle.y - 88;
		}
		else if (side == ADAPTIVE_LEFT) {
			pCmd->viewangles.y = at_target_angle.y + 89;
		}


		if (side == ADAPTIVE_UNKNOWN) {
			pCmd->viewangles.y -= 177;
		}
	}

	void SideJitterALT(CUserCmd *pCmd)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y -= 74;
		}
		else
		{
			pCmd->viewangles.y -= 103;
		}
		Fast2 = !Fast2;
	}

	void SideJitter(CUserCmd *pCmd)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y += 76;
		}
		else
		{
			pCmd->viewangles.y += 104;
		}
		Fast2 = !Fast2;
	}

	void NewBackJitter(CUserCmd *pCmd)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y += 163;
		}
		else
		{
			pCmd->viewangles.y -= 164;
		}
		Fast2 = !Fast2;
	}

	void LbyBreakFakeLeft(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;
		int random = rand() % 100;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 0.9f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (random > 10 && random < 50)
			yaw = 159.3;
		else if (random < 50)
			yaw = -162.7;
		else if (LBYUpdated && random < 10 && random > 7)
			yaw = -91.2;
		else if (LBYUpdated && random < 6 && random > 3)
			yaw = -89.7;
		else if (LBYUpdated && random < 3)
			yaw = -90.3;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void LbyBreakFakeRight(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;
		int random = rand() % 100;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.069f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 1.1f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (random > 10 && random < 50)
			yaw = -159.3;
		else if (random < 50)
			yaw = 153 - 7;
		else if (LBYUpdated && random < 10 && random > 7)
			yaw = 91.2;
		else if (LBYUpdated && random < 6 && random > 3)
			yaw = 89.7;
		else if (LBYUpdated && random < 3)
			yaw = 90.3;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void LbyBreakRealRight(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;
		int random = rand() % 100;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.04f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 1.1f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated && random > 91)
			yaw = -178.1;
		else
			yaw = 91.8;

		/*			yaw = -91.1;
		else
		yaw = -179.1;*/

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void LbyBreakRealLeft(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;
		int random = rand() % 100;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.05f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 1.2f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated && random < 3)
			yaw = 177.9;
		else
			yaw = -92.1;
		/*			yaw = -91.1;
		else
		yaw = 178.3;*/

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void LBY180L(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;
		int random = rand() % 100;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.001f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 0.01f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated && random < 3)
			yaw = 90;
		else
			yaw = -91.1;
		/*			yaw = -91.1;
		else
		yaw = 178.3;*/

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void LBY180R(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;
		int random = rand() % 100;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.001f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 0.002f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated && random < 3)
			yaw = -90;
		else
			yaw = 88.92;
		/*			yaw = -91.1;
		else
		yaw = 178.3;*/

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void AntiCorrection(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;
		int random = rand() % 100;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 1.f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated && random > 60 && random < 95)
			yaw = 91.3;
		else if (random < 100 && random > 95)
			yaw = 179;
		else
			yaw = -179;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void FakeLowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true;
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y -= 90.f;
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y += 90.f;
		}
	}

	void FakeLowerbody2(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true;
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y += 90.f;
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y -= 90.f;
		}
	}

	void Lowerbody(CUserCmd *pCmd)
	{
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y -= 90.f;
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y += 90.f;
		}
	}

	void Lowerbody2(CUserCmd *pCmd)
	{
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y += 90.f;
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y -= 90.f;
		}
	}

	void AntiCorrectionALT(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;
		int random = rand() % 100;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 0.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 1.f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated && random < 40 && random > 5)
			yaw = -91;
		else if (random < 5)
			yaw = -179;
		else
			yaw = 179;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}



	void StaticJitter(CUserCmd *pCmd)
	{
		static bool down = true;
		if (down)
		{
			pCmd->viewangles.x = 179.0f;
			down = !down;
		}
		else
		{
			pCmd->viewangles.x = 89.0f;
			down = !down;
		}
	}

	// Yaws

	void FastSpin(CUserCmd *pCmd)
	{
		static int y2 = -179;
		int spinBotSpeedFast = 100;

		y2 += spinBotSpeedFast;

		if (y2 >= 177)
			y2 = -179;

		pCmd->viewangles.y = y2;
	}

	void FakeEdge(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		CTraceFilter filter;
		filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		for (int y = 0; y < 360; y++)
		{
			Vector qTmp(10.0f, pCmd->viewangles.y, 0.0f);
			qTmp.y += y;

			if (qTmp.y > 180.0)
				qTmp.y -= 360.0;
			else if (qTmp.y < -180.0)
				qTmp.y += 360.0;

			GameUtils::NormaliseViewAngle(qTmp);

			Vector vForward;

			VectorAngles(qTmp, vForward);

			float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
			vForward *= fLength;

			trace_t tr;

			Vector vTraceEnd = vEyePos + vForward;

			Ray_t ray;

			ray.Init(vEyePos, vTraceEnd);
			Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector angles;

				Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

				VectorAngles(vNegative, angles);

				GameUtils::NormaliseViewAngle(angles);

				qTmp.y = angles.y;

				GameUtils::NormaliseViewAngle(qTmp);

				trace_t trLeft, trRight;

				Vector vLeft, vRight;
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

				vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
				vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

				vTraceEnd = vEyePos + vLeft;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

				vTraceEnd = vEyePos + vRight;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);

				if ((trLeft.fraction == 1.0f) && (trRight.fraction != 1.0f))
					qTmp.y -= 90.f;
				else if ((trLeft.fraction != 1.0f) && (trRight.fraction == 1.0f))
					qTmp.y += 90.f;

				if (qTmp.y > 180.0)
					qTmp.y -= 360.0;
				else if (qTmp.y < -180.0)
					qTmp.y += 360.0;

				pCmd->viewangles.y = qTmp.y;

				int offset = Menu::Window.RageBotTab.AntiAimOffset.GetValue();

				static int ChokedPackets = -1;
				ChokedPackets++;
				if (ChokedPackets < 1)
				{
					bSendPacket = false; // +=180?
				}
				else
				{
					bSendPacket = true;
					pCmd->viewangles.y -= offset;
					ChokedPackets = -1;
				}
				return;
			}
		}
		pCmd->viewangles.y += 360.0f;
	}

	void BackJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		if (random < 98)

			pCmd->viewangles.y -= 178;

		if (random < 15)
		{
			float change = -69 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -89 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void FakeSideways(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 178;
			ChokedPackets = -1;
		}
	}

	void FastSpint(CUserCmd *pCmd)
	{
		int r1 = rand() % 100;
		int r2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (r1 == 1) dir = !dir;

		if (dir)
			current_y += 14 + rand() % 10;
		else
			current_y -= 16 + rand() % 10;

		pCmd->viewangles.y = current_y;

		if (r1 == r2)
			pCmd->viewangles.y += r1;
	}

	void BackwardJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		if (random < 98)

			pCmd->viewangles.y -= 180;

		if (random < 15)
		{
			float change = -69 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -91 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void Jitter(CUserCmd *pCmd)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 92;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 89;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 179;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 178;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}
	}

	void Backwards(CUserCmd *pCmd)
	{
		pCmd->viewangles.y -= 179;
	}

	void FakeStatic(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			static int y2 = -179;
			int spinBotSpeedFast = 360.0f / 1.618033988749895f;;

			y2 += spinBotSpeedFast;

			if (y2 >= 179)
				y2 = -179;

			pCmd->viewangles.y = y2;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 178;
			ChokedPackets = -1;
		}
	}

	void TJitter(CUserCmd *pCmd)
	{
		static bool Turbo = true;
		if (Turbo)
		{
			pCmd->viewangles.y -= 29;
			Turbo = !Turbo;
		}
		else
		{
			pCmd->viewangles.y += 31;
			Turbo = !Turbo;
		}
	}

	void TFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y = -44;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = 89;
			ChokedPackets = -1;
		}
	}

	void FakeJitter(CUserCmd* pCmd, bool &bSendPacket)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 133;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y += 223;
		}

		static int iChoked = -1;
		iChoked++;
		if (iChoked < 1)
		{
			bSendPacket = false;
			if (jitterangle <= 1)
			{
				pCmd->viewangles.y += 44;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				pCmd->viewangles.y -= 46;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else
		{
			bSendPacket = true;
			iChoked = -1;
		}
	}

	void Fake1(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 34;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 36;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 91;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 89;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}
	}

	void Fake2(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 23;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 244;
			ChokedPackets = -1;
		}
	}

	void FakeTank(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y = -92;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = 278;
			ChokedPackets = -1;

		}
	}

	void Up(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -89.0f;
	}

	void Zero(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 1080.f;
	}

	void Static(CUserCmd *pCmd)
	{
		static bool aa1 = false;
		aa1 = !aa1;
		if (aa1)
		{
			static bool turbo = false;
			turbo = !turbo;
			if (turbo)
			{
				pCmd->viewangles.y -= 89;
			}
			else
			{
				pCmd->viewangles.y += 86;
			}
		}
		else
		{
			pCmd->viewangles.y -= 180;
		}
	}

	void UpsideDown(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 130089.0f;
	}

	void UpsideDown2(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -250078.0f;
	}
	void HalfUp(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -45.0f;
	}

	void UpsideDownJitter(CUserCmd *pCmd)
	{
		pDance += 45.0f;
		if (pDance > 100)
			pDance = 0.0f;
		else if (pDance > 75.f)
			pCmd->viewangles.x = -130089.0f;
		else if (pDance < 75.f)
			pCmd->viewangles.x = 130089.0f;
	}
	void TrueDown(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 90.0f;
	}
	void TrueUp(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -90.0f;
	}



	void fakelowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 45.60f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 85.40f;
			bSendPacket = true;
		}
	}

	void FakeSideLBY(CUserCmd *pCmd, bool &bSendPacket)
	{
		int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i;
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		static bool isMoving;
		float PlayerIsMoving = abs(pLocal->GetVelocity().Length());
		if (PlayerIsMoving > 0.1) isMoving = true;
		else if (PlayerIsMoving <= 0.1) isMoving = false;

		int flip = (int)floorf(Interfaces::Globals->curtime / 1.1) % 2;
		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		if (std::fmod(flTime, 1) == 0.f)
			bFlipYaw = !bFlipYaw;

		if (PlayerIsMoving <= 0.1)
		{
			if (bSendPacket)
			{
				pCmd->viewangles.y += 179.f;
			}
			else
			{
				if (flip)
				{
					pCmd->viewangles.y += bFlipYaw ? 86 : -89.f;

				}
				else
				{
					pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? 89.f : -90.f;
				}
			}
		}
		else if (PlayerIsMoving > 0.1)
		{
			if (bSendPacket)
			{
				pCmd->viewangles.y += 177.f;
			}
			else
			{
				pCmd->viewangles.y += 93.f;
			}
		}
	}

	void LBYJitter(CUserCmd* cmd, bool& packet)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;
		if (ySwitch)
		{
			if (jbool)
			{
				if (jboolt)
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 44.f;
					packet = false;
				}
				else
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 54.f;
					packet = false;
				}
			}
			else
			{
				if (jboolt)
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 192.f;
					packet = false;
				}
				else
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 192.f;
					packet = false;
				}
			}
		}
		else
		{
			cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			packet = true;
		}
	}

	void LBYThing(CUserCmd *pCmd, bool &bSendPacket)
	{
		pDance += 45.0f;
		if (pDance > 100)
			pDance = 0.0f;
		else if (pDance > 75.f)
			pCmd->viewangles.y += 1.0f;
		else if (pDance < 75.f)
			pCmd->viewangles.y += 46.0f;
		else if (pDance > 75.f)
			pCmd->viewangles.y += 91.0f;
		else if (pDance < 75.f)
			pCmd->viewangles.y += 136.0f;
		else if (pDance > 75.f)
			pCmd->viewangles.y += 1.0f;


	}

	void LBYSpin(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();
		static int skeet = 65;
		int SpinSpeed = 175;
		static int ChokedPackets = -1;
		ChokedPackets++;
		skeet += SpinSpeed;

		if
			(pCmd->command_number % 9)
		{
			bSendPacket = true;
			if (skeet >= pLocal->GetLowerBodyYaw() + 145);
			skeet = pLocal->GetLowerBodyYaw() - 0;
			ChokedPackets = -1;
		}
		else if
			(pCmd->command_number % 9)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 174;
			ChokedPackets = -1;
		}
		pCmd->viewangles.y = skeet;
	}

	void SlowSpin(CUserCmd *pCmd)
	{
		int r1 = rand() % 100;
		int r2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (r1 == 1) dir = !dir;

		if (dir)
			current_y += 4 + rand() % 10;
		else
			current_y -= 4 + rand() % 10;

		pCmd->viewangles.y = current_y;

		if (r1 == r2)
			pCmd->viewangles.y += r1;

	}

	void EdgeDetect(CUserCmd* pCmd, bool &bSendPacket)
	{

		IClientEntity* pLocal = hackManager.pLocal();

		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;

		bool bEdge = false;

		Vector angle;
		Vector eyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		for (float i = 0; i < 360; i++)
		{
			Vector vecDummy(10.f, pCmd->viewangles.y, 0.f);
			vecDummy.y += i;

			Vector forward = vecDummy.Forward();

			float flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
			forward *= flLength;

			Ray_t ray;
			CGameTrace tr;

			ray.Init(eyePos, (eyePos + forward));
			Interfaces::Trace->EdgeTraceRay(ray, traceFilter, tr, true);

			if (tr.fraction != 1.0f)
			{
				Vector negate = tr.plane.normal;
				negate *= -1;

				Vector vecAng = negate.Angle();

				vecDummy.y = vecAng.y;

				trace_t leftTrace, rightTrace;

				Vector left = (vecDummy + Vector(0, 45, 0)).Forward(); // or 45
				Vector right = (vecDummy - Vector(0, 45, 0)).Forward();

				left *= (flLength * cosf(rad(30)) * 2);
				right *= (flLength * cosf(rad(30)) * 2);

				ray.Init(eyePos, (eyePos + left));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, leftTrace, true);

				ray.Init(eyePos, (eyePos + right));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, rightTrace, true);

				if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				{
					vecDummy.y -= 45; // left
				}
				else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				{
					vecDummy.y += 45; // right     
				}

				angle.y = vecDummy.y;
				angle.y += 360;
				bEdge = true;
			}
		}

		if (bEdge)
		{
			static bool turbo = true;

			if (Menu::Window.RageBotTab.AntiAimEdge.GetState())
			{
				pCmd->viewangles.y = angle.y;
			}
		}
	}
}

void CorrectMovement(Vector old_angles, CUserCmd* cmd, float old_forwardmove, float old_sidemove)
{
	float delta_view, first_function, second_function;

	if (old_angles.y < 0.f) first_function = 360.0f + old_angles.y;
	else first_function = old_angles.y;

	if (cmd->viewangles.y < 0.0f) second_function = 360.0f + cmd->viewangles.y;
	else second_function = cmd->viewangles.y;

	if (second_function < first_function) delta_view = abs(second_function - first_function);
	else delta_view = 360.0f - abs(first_function - second_function);

	delta_view = 360.0f - delta_view;

	cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;
}

float GetOutgoingLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float OutgoingLatency = nci->GetAvgLatency(FLOW_OUTGOING);
		return OutgoingLatency;
	}
	else
	{
		return 0.0f;
	}
}
float GetIncomingLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		return IncomingLatency;
	}
	else
	{
		return 0.0f;
	}
}

void DoLBYBreak(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	if (!bSendPacket)
	{
		if (NextLBYUpdate())
			pCmd->viewangles.y += 57;
		else
			pCmd->viewangles.y -= 89;
	}
	else
	{
		if (NextLBYUpdate())
			pCmd->viewangles.y -= 35;
		else
			pCmd->viewangles.y += 69;
	}
}

void DoLBYBreakReal(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	if (!bSendPacket)
	{
		if (NextLBYUpdate())
			pCmd->viewangles.y -= 50;
		else
			pCmd->viewangles.y += 130;
	}
	else
	{
		if (NextLBYUpdate())
			pCmd->viewangles.y += 14;
		else
			pCmd->viewangles.y -= 67;
	}
}

void DoRealAA(CUserCmd* pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	static bool switch2;
	static bool cock;
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	if (!Menu::Window.RageBotTab.AntiAimEnable.GetState())
		return;
	switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
	{
	case 0:
		break;
	case 1:
		// Fast Spin
		AntiAims::FastSpint(pCmd);
		break;
	case 2:
		// Slow Spin
		AntiAims::SlowSpin(pCmd);
		break;
	case 3:
		AntiAims::Jitter(pCmd);
		break;
	case 4:
		// 180 Jitter
		AntiAims::BackJitter(pCmd);
		break;
	case 5:
		// Backward
		AntiAims::Backwards(pCmd);
		break;
	case 6:
		AntiAims::BackwardJitter(pCmd);
		break;
	case 7:
		//Sideways-switch
		if (switch2)
			pCmd->viewangles.y = 69;
		else
			pCmd->viewangles.y = -79;

		switch2 = !switch2;
		break;
	case 8:
		//Sideways
		pCmd->viewangles.y -= 98;
		break;
	case 9:
		pCmd->viewangles.y += 67;
		break;
	case 10:
		pCmd->viewangles.y = pLocal->GetLowerBodyYaw() + rand() % 180 - rand() % 50;
		break;
	case 11:
		AntiAims::LBYJitter(pCmd, bSendPacket);
		break;
	case 12:
		AntiAims::FakeSideLBY(pCmd, bSendPacket);
		break;
	case 13:
		AntiAims::LBYSpin(pCmd, bSendPacket);
		break;
	case 14:
		AntiAims::LBYThing(pCmd, bSendPacket);
		break;
	case 15:
		AntiAims::Fake1(pCmd, bSendPacket);
		break;
	case 16:
		AntiAims::Fake2(pCmd, bSendPacket);
		break;
	case 17:
		AntiAims::FakeTank(pCmd, bSendPacket);
		break;
	case 18:
		if (flipBool)
		{
			pCmd->viewangles.y += 89;
		}
		else if (!flipBool)
		{
			pCmd->viewangles.y -= 91;
		}
		break;

	case 19:
		AntiAims::adaptive(pCmd, bSendPacket);
		break;
	case 20:
		if (flipBool)
		{
			AntiAims::SideJitter(pCmd);
		}
		else if (!flipBool)
		{
			AntiAims::SideJitterALT(pCmd);
		}
		break;
	case 21:
		// 180 Jitter
		AntiAims::NewBackJitter(pCmd);
		break;
	case 22:
		if (flipBool)
		{
			//AAYaw::LowerbodyNew(pCmd);
			AntiAims::AntiCorrectionALT(pCmd);
		}
		else if (!flipBool)
		{
			AntiAims::AntiCorrection(pCmd);
			//AAYaw::LowerbodyNewALT(pCmd);
		}
		break;

	case 23:

		pCmd->viewangles.y += RandomNumber(-180, 180);

		break;

	case 24:

		pCmd->viewangles.y += (Menu::Window.RageBotTab.SpinSpeed.GetValue() * 3) * Interfaces::Globals->curtime;

		break;

	case 25:

		if (flipBool)

		{
			AntiAims::LbyBreakRealRight(pCmd);
		}

		else if (!flipBool)
		{
			AntiAims::LbyBreakRealLeft(pCmd);
		}
		break;

	case 26:

		if (flipBool)

		{
			AntiAims::LBY180R(pCmd);
		}

		else if (!flipBool)
		{
			AntiAims::LBY180L(pCmd);
		}
		break;
	case 27:
		if (flipBool)

		{
			AntiAims::Lowerbody(pCmd);
		}

		else if (!flipBool)
		{
			AntiAims::Lowerbody2(pCmd);
		}
		break;

	}

	static auto RandomReal = false;
	if (RandomReal)
		pCmd->viewangles.y += Menu::Window.RageBotTab.AntiAimOffset.GetValue();
	else
		pCmd->viewangles.y -= Menu::Window.RageBotTab.AntiAimOffset.GetValue();
	RandomReal = !RandomReal;
}

void DoFakeAA(CUserCmd* pCmd, bool& bSendPacket, IClientEntity* pLocal)
{
	static bool switch2;
	static bool cock;
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	if (!Menu::Window.RageBotTab.AntiAimEnable.GetState())
		return;
	switch (Menu::Window.RageBotTab.FakeYaw.GetIndex())
	{
	case 0:
		break;
	case 1:
		// Fast Spin 
		AntiAims::FastSpint(pCmd);
		break;
	case 2:
		// Slow Spin 
		AntiAims::SlowSpin(pCmd);
		break;
	case 3:
		AntiAims::Jitter(pCmd);
		break;
	case 4:
		// 180 Jitter 
		AntiAims::BackJitter(pCmd);
		break;
	case 5:
		// Backward
		AntiAims::Backwards(pCmd);
		break;
	case 6:
		AntiAims::BackwardJitter(pCmd);
		break;
	case 7:
		//Sideways-switch
		if (switch2)
			pCmd->viewangles.y = 91;
		else
			pCmd->viewangles.y = -89;

		switch2 = !switch2;
		break;
	case 8:
		pCmd->viewangles.y -= 89;
		break;
	case 9:
		pCmd->viewangles.y += 91;
		break;
	case 10:
		pCmd->viewangles.y = pLocal->GetLowerBodyYaw() + rand() % 180 - rand() % 50;
		break;
	case 11:
		AntiAims::FakeJitter(pCmd, bSendPacket);
		break;
	case 12:
		AntiAims::FakeSideLBY(pCmd, bSendPacket);
		break;
	case 13:
		AntiAims::LBYSpin(pCmd, bSendPacket);
		break;
	case 14:
		AntiAims::LBYThing(pCmd, bSendPacket);
		break;
	case 15:
		AntiAims::Fake1(pCmd, bSendPacket);
		break;
	case 16:
		AntiAims::Fake2(pCmd, bSendPacket);
		break;
	case 17:
		AntiAims::FakeTank(pCmd, bSendPacket);
		break;
	case 18:
		if (flipBool)
		{
			pCmd->viewangles.y -= 93;
		}
		else if (!flipBool)
		{
			pCmd->viewangles.y += 92;
		}
		break;

	case 19:
		AntiAims::adaptive2(pCmd, bSendPacket);
		break;
	case 20:
		if (flipBool)
		{
			AntiAims::SideJitterALT(pCmd);
		}
		else if (!flipBool)
		{
			AntiAims::SideJitter(pCmd);
		}
		break;
	case 21:
		// 180 Jitter
		AntiAims::NewBackJitter(pCmd);
		break;
	case 22:
		if (flipBool)
		{
			//AAYaw::LowerbodyNew(pCmd);
			AntiAims::AntiCorrection(pCmd);
		}
		else if (!flipBool)
		{
			AntiAims::AntiCorrectionALT(pCmd);
			//AAYaw::LowerbodyNewALT(pCmd);
		}
		break;
	case 23:
		pCmd->viewangles.y += RandomNumber(-180, 180);
		break;
	case 24:
		pCmd->viewangles.y += (Menu::Window.RageBotTab.SpinSpeed.GetValue() * 3) * Interfaces::Globals->curtime;
		break;
	case 25:
		if (flipBool)
		{
			AntiAims::LbyBreakFakeRight(pCmd);
		}
		else if (!flipBool)
		{
			AntiAims::LbyBreakFakeLeft(pCmd);
		}
		break;

	case 26:
		if (flipBool)
		{
			pCmd->viewangles.y += RandomNumber(-170, 10);
		}
		else if (!flipBool)
		{
			pCmd->viewangles.y += RandomNumber(170, -10);
		}

		break;
	case 27:
		if (flipBool)
		{
			AntiAims::FakeLowerbody(pCmd, bSendPacket);
		}
		else if (!flipBool)
		{
			AntiAims::FakeLowerbody2(pCmd, bSendPacket);
		}
		break;
	}


	static auto RandomFake = false;
	if (RandomFake)
		pCmd->viewangles.y += Menu::Window.RageBotTab.AddFakeYaw.GetValue();
	else
		pCmd->viewangles.y -= Menu::Window.RageBotTab.AddFakeYaw.GetValue();
	RandomFake = !RandomFake;
}

void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocal = hackManager.pLocal();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (IsAimStepping || pCmd->buttons & IN_ATTACK)
		return;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();

		if (!GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (Menu::Window.RageBotTab.AntiAimKnife.GetState())
			{
				if (!CanOpenFire() || pCmd->buttons & IN_ATTACK2)
					return;
			}
			else
			{
				return;
			}
		}
	}
	if (Menu::Window.RageBotTab.AntiAimTarget.GetState())
	{
		aimAtPlayer(pCmd);
	}
	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex())
	{
	case 0:
		break;
	case 1:
		pCmd->viewangles.x = 46.f;
		break;
	case 2:
		pCmd->viewangles.x = -44.f;
		break;
	case 3:
		AntiAims::JitterPitch(pCmd);
		break;
	case 4:
		pCmd->viewangles.x = 89.000000;
		break;
	case 5:
		AntiAims::Up(pCmd);
		break;
	case 6:
		AntiAims::Zero(pCmd);
		break;
	case 7:
		AntiAims::UpsideDown(pCmd);
		break;
	case 8:
		AntiAims::UpsideDown2(pCmd);
		break;
	case 9:
		AntiAims::UpsideDownJitter(pCmd);
		break;
	case 10:
		AntiAims::AngelDown(pCmd);
		break;
	case 11:
		AntiAims::AngelUp(pCmd);
		break;
	case 12:
		AntiAims::TrueDown(pCmd);
		break;
	case 13:
		AntiAims::TrueUp(pCmd);
		break;
	case 14:
		static auto Random = false;
		if (Random)
			pCmd->viewangles.x += Menu::Window.RageBotTab.CustomPitch.GetValue();
		else
			pCmd->viewangles.x -= Menu::Window.RageBotTab.CustomPitch.GetValue();
		Random = !Random;

	}

	if (Menu::Window.RageBotTab.LBY.GetState() && pLocal->GetVelocity().Length2D() == 0)
	{
#define RandomInt(min, max) (rand() % (max - min + 1) + min)
		static bool fakeantiaim;
		int rand2;
		{
			int var1;
			int var2;
			float var3;

			pCmd->viewangles.y += 179.9;
			var1 = rand() % 100;
			var2 = rand() % (10 - 6 + 1) + 10;
			var3 = var2 - (rand() % var2);
			if (var1 < 60 + (rand() % 14))
				pCmd->viewangles.y -= var3;
			else if (var1 < 100 + (rand() % 14))
				pCmd->viewangles.y += var3;
		}

		if (fakeantiaim)
		{
			rand2 = RandomInt(1, 100);

			if (rand2 < 2.0)
			{
				bSendPacket = true;
				pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 92.3 - 0 - 31.3;
			}

			else
			{
				bSendPacket = true;
				pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 91.7;
			}
			fakeantiaim = false;
		}
		else
		{
			bSendPacket = false;
			pCmd->viewangles.y += 154.4;
			fakeantiaim = true;
		}
	}
	else
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = true;
			DoFakeAA(pCmd, bSendPacket, pLocal);
		}
		else
		{
			bSendPacket = false;
			DoRealAA(pCmd, pLocal, bSendPacket);
			ChokedPackets = -1;
		}

		if (flipAA)
		{
			pCmd->viewangles.y -= 24;
		}
	}
}

