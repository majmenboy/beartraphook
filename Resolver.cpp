#include "Resolver.h"
#include "Ragebot.h"
#include "Hooks.h"

void LowerBodyYawFix(IClientEntity* pEntity)
{
	if (Menu::Window.RageBotTab.LowerbodyFix.GetState())
	{
		if (!pEntity) return;
		if (pEntity->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer) return;
		if (!pEntity->IsAlive() || !pEntity->GetActiveWeaponHandle()) return;
		if (Interfaces::Engine->GetLocalPlayer()) return;

		auto EyeAngles = pEntity->GetEyeAnglesXY();
		if (pEntity->GetVelocity().Length() > 1 && (pEntity->GetFlags() & (int)pEntity->GetFlags() & FL_ONGROUND))
			EyeAngles->y = pEntity->GetLowerBodyYaw();
	}
}

void PitchCorrection()
{
	CUserCmd* pCmd;
	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		IClientEntity* pLocal = hackManager.pLocal();
		IClientEntity *player = (IClientEntity*)Interfaces::EntList->GetClientEntity(i);

		if (!player || player->IsDormant() || player->GetHealth() < 1 || (DWORD)player == (DWORD)pLocal)
			continue;

		if (!player)
			continue;

		if (pLocal)
			continue;

		if (pLocal && player && pLocal->IsAlive())
		{
			if (Menu::Window.RageBotTab.AdvancedResolver.GetState())
			{
				Vector* eyeAngles = player->GetEyeAnglesXY();
				if (eyeAngles->x < -179.f) eyeAngles->x += 360.f;
				else if (eyeAngles->x > 90.0 || eyeAngles->x < -90.0) eyeAngles->x = 89.f;
				else if (eyeAngles->x > 89.0 && eyeAngles->x < 91.0) eyeAngles->x -= 90.f;
				else if (eyeAngles->x > 179.0 && eyeAngles->x < 181.0) eyeAngles->x -= 180;
				else if (eyeAngles->x > -179.0 && eyeAngles->x < -181.0) eyeAngles->x += 180;
				else if (fabs(eyeAngles->x) == 0) eyeAngles->x = std::copysign(89.0f, eyeAngles->x);
			}
		}
	}
}

void ResolverSetup::Resolve(IClientEntity* pEntity)
{
	bool MeetsLBYReq;
	if (pEntity->GetFlags() & FL_ONGROUND)
		MeetsLBYReq = true;
	else
		MeetsLBYReq = false;

	bool IsMoving;
	if (pEntity->GetVelocity().Length2D() >= 0.5)
		IsMoving = true;
	else
		IsMoving = false;

	ResolverSetup::NewANgles[pEntity->GetIndex()] = *pEntity->GetEyeAnglesXY();
	ResolverSetup::newlby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
	ResolverSetup::newsimtime = pEntity->GetSimulationTime();
	ResolverSetup::newdelta[pEntity->GetIndex()] = pEntity->GetEyeAnglesXY()->y;
	ResolverSetup::newlbydelta[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
	ResolverSetup::finaldelta[pEntity->GetIndex()] = ResolverSetup::newdelta[pEntity->GetIndex()] - ResolverSetup::storeddelta[pEntity->GetIndex()];
	ResolverSetup::finallbydelta[pEntity->GetIndex()] = ResolverSetup::newlbydelta[pEntity->GetIndex()] - ResolverSetup::storedlbydelta[pEntity->GetIndex()];
	if (newlby == storedlby)
		ResolverSetup::lbyupdated = false;
	else
		ResolverSetup::lbyupdated = true;

	if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 0)
	{

	}
	else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 1)//level 1
	{
		if (MeetsLBYReq && lbyupdated)
			pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
		else if (!MeetsLBYReq && lbyupdated)
		{
			switch (Globals::Shots % 2)
			{
			case 1:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
				break;
			case 2:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
				break;
			case 3:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 15;
				break;
			case 4:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
				break;
			case 5:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
				break;
			case 6:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
				break;
			case 7:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
				break;
			case 8:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 180;
				break;
			case 9:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
				break;
			case 10:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 47;
				break;
			case 11:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 77;
				break;
			case 12:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
				break;
			case 13:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
				break;
			case 14:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 58;
				break;
			case 15:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
				break;
			case 16:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
				break;
			case 17:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 32;
				break;
			case 18:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
				break;
			case 19:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
				break;
			case 20:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
				break;
			case 21:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
				break;
			case 22:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
				break;
			case 23:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 34;
				break;
			case 25:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
				break;
			case 26:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
				break;
			case 27:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
				break;
			case 28:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 91;
				break;
			case 29:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 150;
				break;
			case 30:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 75;
				break;
			case 31:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
				break;
			case 32:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 50;
				break;
			case 33:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
				break;
			case 34:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 140;
				break;
			case 35:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
				break;
			case 36:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 150;
				break;
			case 37:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
				break;
			case 39:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 176;
				break;
			case 40:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 120;
				break;
			case 41:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
				break;
			case 42:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 123;
				break;
			case 44:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 56;
				break;
			case 45:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
				break;
			case 46:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
				break;
			case 47:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 58;
				break;
			case 48:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
				break;
			case 49:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
				break;
			case 50:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 123;
				break;
			case 51:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20;
				break;
			case 100:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
				break;
			case 52:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
				break;
			case 53:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 45;
				break;
			case 54:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 45;
				break;
			case 55:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
				break;
			case 56:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
				break;
			case 57:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
				break;
			case 58:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
				break;
			case 59:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 34;
				break;
			case 60:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 64;
				break;
			case 61:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
				break;
			case 62:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 42;
				break;
			case 63:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 17;
				break;
			case 64:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 42;
				break;
			case 65:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
				break;
			case 66:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 170;
				break;
			case 67:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 58;
				break;
			case 68:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 140;
				break;
			case 69:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
				break;
			case 70:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
				break;
			case 71:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
				break;
			case 72:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
				break;
			case 73:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 74;
				break;
			case 74:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 74;
				break;
			case 75:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
				break;
			case 76:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 45;
				break;
			case 77:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 74;
				break;
			case 78:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
				break;
			case 79:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 80;
				break;
			case 80:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
				break;
			case 81:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 47;
				break;
			case 82:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 145;
				break;
			case 83:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 178;
				break;
			case 84:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 147;
				break;
			case 85:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
				break;
			case 86:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 170;
				break;
			case 87:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 160;
				break;
			case 88:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 160;
				break;
			case 89:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 150;
				break;
			case 90:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 150;
				break;
			case 91:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 140;
				break;
			case 92:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 140;
				break;
			case 93:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 130;
				break;
			case 94:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
				break;
			case 95:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
				break;
			case 96:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 120;
				break;
			case 97:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 110;
				break;
			case 98:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 110;
				break;
			case 99:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 100;
				break;
			case 156:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 100;
				break;
			case 101:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 80;
				break;
			case 102:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 80;
				break;
			case 103:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
				break;
			case 104:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
				break;
			case 105:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 60;
				break;
			case 106:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 60;
				break;
			case 107:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 50;
				break;
			case 108:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 50;
				break;
			case 109:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
				break;
			case 110:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
				break;
			case 111:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 30;
				break;
			case 112:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 30;
				break;
			case 113:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 20;
				break;
			case 114:
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20;
				break;
			}
		}
			else if (MeetsLBYReq && !lbyupdated)
			{
				switch (Globals::Shots % 3)
				{
				case 1:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
					break;
				case 2:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
					break;
				case 3:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 15;
					break;
				case 4:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 5:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 6:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
					break;
				case 7:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 8:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 180;
					break;
				case 9:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
					break;
				case 10:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 47;
					break;
				case 11:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 77;
					break;
				case 12:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 13:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 14:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 58;
					break;
				case 15:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
					break;
				case 16:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
					break;
				case 17:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 32;
					break;
				case 18:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
					break;
				case 19:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
					break;
				case 20:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 21:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
					break;
				case 22:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
					break;
				case 23:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 34;
					break;
				case 25:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 26:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 27:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 28:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 91;
					break;
				case 29:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 150;
					break;
				case 30:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 75;
					break;
				case 31:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 32:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 50;
					break;
				case 33:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
					break;
				case 34:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 140;
					break;
				case 35:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 36:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 150;
					break;
				case 37:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
					break;
				case 38:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 176;
					break;
				case 39:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 120;
					break;
				case 40:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
					break;
				case 41:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 123;
					break;
				case 42:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 56;
					break;
				case 43:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
					break;
				case 44:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
					break;
				case 45:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 58;
					break;
				case 46:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
					break;
				case 47:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 48:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 123;
					break;
				case 49:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20;
					break;
				}
			}
			else
				pEntity->GetEyeAnglesXY()->y = rand() % 360 - rand() % 5;
		}
		else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 2) //level 2 .
		{
			if (Resolver::didhitHS)
			{
				if (MeetsLBYReq && lbyupdated)
				{
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
				}
				switch (Globals::Shots % 76)
				{
				case 1:
					*pEntity->GetEyeAnglesXY() = StoredAngles[pEntity->GetIndex()];
					break;
				case 2:
					*pEntity->GetEyeAnglesXY() = StoredAngles[pEntity->GetIndex()];
					break;
				case 3:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 5;
					break;
				case 4:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 5;
					break;
				case 5:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 10;
					break;
				case 6:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 10;
					break;
				case 7:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 15;
					break;
				case 8:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 15;
					break;
				case 9:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 20;
					break;
				case 10:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 20;
					break;
				case 11:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 25;
					break;
				case 12:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 25;
					break;
				case 13:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 30;
					break;
				case 14:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 30;
					break;
				case 15:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 35;
					break;
				case 16:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 35;
					break;
				case 17:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 40;
					break;
				case 18:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 40;
					break;
				case 19:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 45;
					break;
				case 20:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 45;
					break;
				case 21:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 50;
					break;
				case 22:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 50;
					break;
				case 23:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 55;
					break;
				case 24:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 55;
					break;
				case 25:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 60;
					break;
				case 26:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 60;
					break;
				case 27:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 65;
					break;
				case 28:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 65;
					break;
				case 29:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 70;
					break;
				case 30:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 70;
					break;
				case 31:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 75;
					break;
				case 32:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 75;
					break;
				case 33:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 80;
					break;
				case 34:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 80;
					break;
				case 35:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 85;
					break;
				case 36:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 85;
					break;
				case 37:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 90;
					break;
				case 38:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 90;
					break;
				case 39:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 95;
					break;
				case 40:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 95;
					break;
				case 41:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 100;
					break;
				case 42:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 100;
					break;
				case 43:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 105;
					break;
				case 44:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 105;
					break;
				case 45:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 110;
					break;
				case 46:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 110;
					break;
				case 47:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 115;
					break;
				case 48:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 115;
					break;
				case 49:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 120;
					break;
				case 50:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 120;
					break;
				case 51:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 125;
					break;
				case 52:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 125;
					break;
				case 53:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 130;
					break;
				case 54:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 130;
					break;
				case 55:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 135;
					break;
				case 56:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 135;
					break;
				case 57:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 140;
					break;
				case 58:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 140;
					break;
				case 59:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 145;
					break;
				case 60:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 145;
					break;
				case 61:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 150;
					break;
				case 62:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 150;
					break;
				case 63:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 155;
					break;
				case 64:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 155;
					break;
				case 65:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 160;
					break;
				case 66:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 160;
					break;
				case 67:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 165;
					break;
				case 68:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 165;
					break;
				case 69:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 170;
					break;
				case 70:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 170;
					break;
				case 71:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 175;
					break;
				case 72:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 175;
					break;
				case 73:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 180;
					break;
				case 74:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 180;
					break;
				case 75:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 0;
					break;
				case 76:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 0;
					break;

				}
			}
			else if (MeetsLBYReq && lbyupdated && !Resolver::didhitHS)
			{
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
			}
			else if (!MeetsLBYReq || !lbyupdated && !Resolver::didhitHS)
			{
				pEntity->GetEyeAnglesXY()->y = rand() % 180 - rand() % 35;
			}
			else
				pEntity->GetEyeAnglesXY()->y = rand() % 180;
		}
		else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 3)//level 3
		{
			if (Globals::missedshots > 3 && Globals::missedshots < 21)
			{
				if (MeetsLBYReq && lbyupdated)
				{
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
				}
				else if (!MeetsLBYReq && lbyupdated)
				{
					switch (Globals::Shots % 4)
					{
					case 1:
						pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
						break;
					case 2:
						pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
						break;
					case 3:
						pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 15;
						break;
					case 4:
						pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
						break;
					}
				}
				else
					pEntity->GetEyeAnglesXY()->y = rand() % 180 - rand() % 35;
			}

			else if (Globals::missedshots >= 2 && Globals::missedshots <= 3)
			{
				if (MeetsLBYReq && lbyupdated)
				{
					pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetIndex()];
				}
				else
					pEntity->GetEyeAnglesXY()->y = ResolverSetup::finaldelta[pEntity->GetIndex()];
			}
			else
			{
				if (MeetsLBYReq && lbyupdated)
				{
					bool timer;
					if (timer)
						pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetIndex()] + rand() % 35;
					else
						pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetIndex()] - rand() % 35;
					timer = !timer;
				}
				else
				{
					bool timer;
					if (timer)
						pEntity->GetEyeAnglesXY()->y = ResolverSetup::finaldelta[pEntity->GetIndex()] + rand() % 35;
					else
						pEntity->GetEyeAnglesXY()->y = ResolverSetup::finaldelta[pEntity->GetIndex()] - rand() % 35;
					timer = !timer;
				}
			}
		}
		else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 4)//level 3
		{
			if (MeetsLBYReq && lbyupdated)
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
			else if (!MeetsLBYReq && lbyupdated)
			{
				switch (Globals::Shots % 3)
				{
				case 1:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
					break;
				case 2:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
					break;
				case 3:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 15;
					break;
				case 4:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 5:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 6:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
					break;
				case 7:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 8:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 180;
					break;
				case 9:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
					break;
				case 10:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 47;
					break;
				case 11:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 77;
					break;
				case 12:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 13:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 14:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 58;
					break;
				case 15:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
					break;
				case 16:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
					break;
				case 17:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 32;
					break;
				case 18:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
					break;
				case 19:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
					break;
				case 20:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 21:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
					break;
				case 22:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
					break;
				case 23:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 34;
					break;
				case 25:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 26:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 27:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 28:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 91;
					break;
				case 29:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 150;
					break;
				case 30:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 75;
					break;
				case 31:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 32:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 50;
					break;
				case 33:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
					break;
				case 34:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 140;
					break;
				case 35:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 36:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 150;
					break;
				case 37:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
					break;
				case 39:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 176;
					break;
				case 40:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 120;
					break;
				case 41:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
					break;
				case 42:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 123;
					break;
				case 44:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 56;
					break;
				case 45:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
					break;
				case 46:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
					break;
				case 47:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 58;
					break;
				case 48:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
					break;
				case 49:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 50:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 123;
					break;
				case 51:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20;
					break;
				case 100:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 52:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 53:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 45;
					break;
				case 54:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 45;
					break;
				case 55:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
					break;
				case 56:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
					break;
				case 57:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
					break;
				case 58:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
					break;
				case 59:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 34;
					break;
				case 60:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 64;
					break;
				case 61:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
					break;
				case 62:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 42;
					break;
				case 63:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 17;
					break;
				case 64:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 42;
					break;
				case 65:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
					break;
				case 66:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 170;
					break;
				case 67:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 58;
					break;
				case 68:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 140;
					break;
				case 69:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
					break;
				case 70:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 71:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
					break;
				case 72:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 73:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 74;
					break;
				case 74:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 74;
					break;
				case 75:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 78;
					break;
				case 76:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 45;
					break;
				case 77:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 74;
					break;
				case 78:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 79:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 80;
					break;
				case 80:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
					break;
				case 81:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 47;
					break;
				case 82:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 145;
					break;
				case 83:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 178;
					break;
				case 84:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 147;
					break;
				case 85:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
					break;
				case 86:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 170;
					break;
				case 87:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 160;
					break;
				case 88:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 160;
					break;
				case 89:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 150;
					break;
				case 90:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 150;
					break;
				case 91:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 140;
					break;
				case 92:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 140;
					break;
				case 93:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 130;
					break;
				case 94:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
					break;
				case 95:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
					break;
				case 96:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 120;
					break;
				case 97:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 110;
					break;
				case 98:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 110;
					break;
				case 99:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 100;
					break;
				case 156:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 100;
					break;
				case 101:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 80;
					break;
				case 102:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 80;
					break;
				case 103:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
					break;
				case 104:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
					break;
				case 105:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 60;
					break;
				case 106:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 60;
					break;
				case 107:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 50;
					break;
				case 108:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 50;
					break;
				case 109:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
					break;
				case 110:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 111:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 30;
					break;
				case 112:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 30;
					break;
				case 113:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 20;
					break;
				case 114:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20;
					break;
				}
			}
			else if (MeetsLBYReq && !lbyupdated)
			{
				switch (Globals::Shots % 3)
				{
				case 1:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
					break;
				case 2:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 40;
					break;
				case 3:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 15;
					break;
				case 4:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 5:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 6:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
					break;
				case 7:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 8:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 180;
					break;
				case 9:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 15;
					break;
				case 10:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 47;
					break;
				case 11:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 77;
					break;
				case 12:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 13:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 14:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 58;
					break;
				case 15:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
					break;
				case 16:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
					break;
				case 17:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 32;
					break;
				case 18:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
					break;
				case 19:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 180;
					break;
				case 20:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 21:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 14;
					break;
				case 22:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 25;
					break;
				case 23:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 34;
					break;
				case 25:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 26:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 90;
					break;
				case 27:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 47;
					break;
				case 28:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 91;
					break;
				case 29:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 150;
					break;
				case 30:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 75;
					break;
				case 31:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 32:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 50;
					break;
				case 33:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 70;
					break;
				case 34:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 140;
					break;
				case 35:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 78;
					break;
				case 36:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 150;
					break;
				case 37:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 130;
					break;
				case 38:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 176;
					break;
				case 39:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 120;
					break;
				case 40:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 120;
					break;
				case 41:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 123;
					break;
				case 42:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 56;
					break;
				case 43:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 24;
					break;
				case 44:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 70;
					break;
				case 45:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 58;
					break;
				case 46:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 170;
					break;
				case 47:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 40;
					break;
				case 48:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 123;
					break;
				case 49:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20;
					break;
				}
			}
			else
				pEntity->GetEyeAnglesXY()->y = rand() % 360 - rand() % 5;
		}
		else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 5)
		{
			pEntity->GetEyeAnglesXY()->y = 167.f - rand() % 12 + rand() % 6;
			if (Resolver::didhitHS)
			{
				switch (Globals::Shots % 2)
				{
				case 1:pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 85; break;
				case 2:pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 20; break;
				case 3:pEntity->GetEyeAnglesXY()->y + -45; break;
				case 4:pEntity->GetEyeAnglesXY()->y + +74; break;
				case 5:pEntity->GetEyeAnglesXY()->y + -15; break;
				case 6:pEntity->GetEyeAnglesXY()->y -= 174; break;
				case 7:pEntity->GetEyeAnglesXY()->y + 270; break;
				case 8:pEntity->GetEyeAnglesXY()->y -= 90; break;
				case 9:pEntity->GetEyeAnglesXY()->y += 180; break;
				case 10:pEntity->GetEyeAnglesXY()->y += +90; break;
				}
			}
			else if (MeetsLBYReq && lbyupdated && !Resolver::didhitHS)
			{
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
			}
			else if (!MeetsLBYReq || !lbyupdated && !Resolver::didhitHS)
			{
				pEntity->GetEyeAnglesXY()->y = rand() % 180 - rand() % 35;
			}
			else if (!IsMoving)
			{
				pEntity->GetEyeAnglesXY()->y = 179.f + rand() % 1 - rand() % 2;

			}
			else if (Globals::missedshots >= 1 && Globals::missedshots <= 2)
			{
				if (MeetsLBYReq && lbyupdated)
				{
					pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetIndex()];
				}
				else
					pEntity->GetEyeAnglesXY()->y = ResolverSetup::finaldelta[pEntity->GetIndex()];
			}

			else
			{
				if (MeetsLBYReq && lbyupdated)
				{
					bool timer;
					if (timer)
						pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetIndex()] + rand() % 35;
					else
						pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetEyeAnglesXY()->y = 180];
					pEntity->GetEyeAnglesXY()->y = ResolverSetup::finallbydelta[pEntity->GetIndex()] - rand() % 35;
					timer = !timer;
				}
				if (Resolver::didhitHS)
				{
					switch (Globals::Shots % 1)
					{
					case 1:pEntity->GetEyeAnglesXY()->y - 0; break;
					case 2:pEntity->GetEyeAnglesXY()->y + 90; break;
					case 3:pEntity->GetEyeAnglesXY()->y - 90; break;
					case 4:pEntity->GetEyeAnglesXY()->y + 179; break;
					case 5:pEntity->GetEyeAnglesXY()->y - 179; break;
					case 6:pEntity->GetEyeAnglesXY()->y + 35; break;
					case 7:pEntity->GetEyeAnglesXY()->y - 35; break;

					}
				}
				else
					pEntity->GetEyeAnglesXY()->y = rand() % 180;
			}
		}
		else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 6) //level 6 datz qick math(er) .
		{
			if (Resolver::didhitHS)
			{
				if (MeetsLBYReq && lbyupdated)
				{
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
				}
				switch (Globals::Shots % 363)  //76
				{
				case 1:
					*pEntity->GetEyeAnglesXY() = StoredAngles[pEntity->GetIndex()];
					break;
				case 2:
					*pEntity->GetEyeAnglesXY() = StoredAngles[pEntity->GetIndex()];
					break;
				case 3:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 1;
					break;
				case 4:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 1;
					break;
				case 5:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 2;
					break;
				case 6:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 2;
					break;
				case 7:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 3;
					break;
				case 8:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 3;
					break;
				case 9:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 4;
					break;
				case 10:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 4;
					break;
				case 11:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 5;
					break;
				case 12:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 5;
					break;
				case 13:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 6;
					break;
				case 14:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 6;
					break;
				case 15:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 7;
					break;
				case 16:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 7;
					break;
				case 17:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 8;
					break;
				case 18:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 8;
					break;
				case 19:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 9;
					break;
				case 20:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 9;
					break;
				case 21:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 10;
					break;
				case 22:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 10;
					break;
				case 23:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 11;
					break;
				case 24:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 11;
					break;
				case 25:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 12;
					break;
				case 26:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 12;
					break;
				case 27:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 13;
					break;
				case 28:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 13;
					break;
				case 29:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 14;
					break;
				case 30:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 14;
					break;
				case 31:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 15;
					break;
				case 32:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 15;
					break;
				case 33:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 16;
					break;
				case 34:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 16;
					break;
				case 35:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 17;
					break;
				case 36:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 17;
					break;
				case 37:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 18;
					break;
				case 38:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 18;
					break;
				case 39:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 19;
					break;
				case 40:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 19;
					break;
				case 41:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 20;
					break;
				case 42:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 20;
					break;
				case 43:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 21;
					break;
				case 44:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 21;
					break;
				case 45:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 22;
					break;
				case 46:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 22;
					break;
				case 47:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 23;
					break;
				case 48:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 23;
					break;
				case 49:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 24;
					break;
				case 50:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 24;
					break;
				case 51:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 25;
					break;
				case 52:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 25;
					break;
				case 53:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 26;
					break;
				case 54:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 26;
					break;
				case 55:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 27;
					break;
				case 56:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 27;
					break;
				case 57:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 28;
					break;
				case 58:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 28;
					break;
				case 59:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 29;
					break;
				case 60:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 29;
					break;
				case 61:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 30;
					break;
				case 62:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 30;
					break;
				case 63:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 31;
					break;
				case 64:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 31;
					break;
				case 65:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 32;
					break;
				case 66:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 32;
					break;
				case 67:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 33;
					break;
				case 68:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 33;
					break;
				case 69:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 34;
					break;
				case 70:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 34;
					break;
				case 71:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 35;
					break;
				case 72:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 35;
					break;
				case 73:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 36;
					break;
				case 74:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 36;
					break;
				case 75:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 37;
					break;
				case 76:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 37;
					break;
				case 77:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 37;
					break;
				case 78:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 37;
					break;
				case 79:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 38;
					break;
				case 80:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 38;
					break;
				case 81:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 39;
					break;
				case 82:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 39;
					break;
				case 83:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 40;
					break;
				case 84:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 41;
					break;
				case 85:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 41;
					break;
				case 86:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 42;
					break;
				case 87:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 42;
					break;
				case 88:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 43;
					break;
				case 89:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 43;
					break;
				case 90:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 44;
					break;
				case 91:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 44;
					break;
				case 92:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 45;
					break;
				case 93:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 45;
					break;
				case 94:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 46;
					break;
				case 95:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 46;
					break;
				case 96:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 47;
					break;
				case 97:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 47;
					break;
				case 98:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 48;
					break;
				case 99:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 48;
					break;
				case 100:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 49;
					break;
				case 101:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 49;
					break;
				case 102:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 50;
					break;
				case 103:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 50;
					break;
				case 104:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 51;
					break;
				case 105:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 51;
					break;
				case 106:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 52;
					break;
				case 107:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 52;
					break;
				case 108:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 53;
					break;
				case 109:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 53;
					break;
				case 110:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 54;
					break;
				case 111:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 54;
					break;
				case 112:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 55;
					break;
				case 113:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 55;
					break;
				case 114:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 56;
					break;
				case 115:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 56;
					break;
				case 116:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 57;
					break;
				case 117:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 57;
					break;
				case 118:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 58;
					break;
				case 119:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 58;
					break;
				case 120:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 59;
					break;
				case 121:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 59;
					break;
				case 122:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 60;
					break;
				case 123:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 61;
					break;
				case 124:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 61;
					break;
				case 125:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 62;
					break;
				case 126:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 62;
					break;
				case 127:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 63;
					break;
				case 128:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 63;
					break;
				case 129:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 64;
					break;
				case 130:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 64;
					break;
				case 131:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 65;
					break;
				case 132:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 65;
					break;
				case 133:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 66;
					break;
				case 134:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 66;
					break;
				case 135:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 67;
					break;
				case 136:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 67;
					break;
				case 137:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 68;
					break;
				case 138:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 68;
					break;
				case 139:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 69;
					break;
				case 140:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 69;
					break;
				case 141:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 70;
					break;
				case 142:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 70;
					break;
				case 143:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 71;
					break;
				case 144:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 71;
					break;
				case 145:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 72;
					break;
				case 146:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 72;
					break;
				case 147:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 73;
					break;
				case 148:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 73;
					break;
				case 149:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 74;
					break;
				case 150:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 74;
					break;
				case 151:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 75;
					break;
				case 152:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 75;
					break;
				case 153:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 76;
					break;
				case 154:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 76;
					break;
				case 155:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 77;
					break;
				case 156:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 77;
					break;
				case 157:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 78;
					break;
				case 158:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 78;
					break;
				case 159:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 79;
					break;
				case 160:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 79;
					break;
				case 161:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 80;
					break;
				case 162:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 80;
					break;
				case 163:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 81;
					break;
				case 164:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 81;
					break;
				case 165:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 82;
					break;
				case 166:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 82;
					break;
				case 167:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 83;
					break;
				case 168:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 83;
					break;
				case 169:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 84;
					break;
				case 170:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 85;
					break;
				case 171:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 85;
					break;
				case 172:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 86;
					break;
				case 173:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 86;
					break;
				case 174:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 87;
					break;
				case 175:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 87;
					break;
				case 176:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 88;
					break;
				case 177:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 88;
					break;
				case 178:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 89;
					break;
				case 179:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 89;
					break;
				case 180:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 90;
					break;
				case 181:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 90;
					break;
				case 182:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() - 90;
					break;
				case 183:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 91;
					break;
				case 184:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 91;
					break;
				case 185:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 92;
					break;
				case 186:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 92;
					break;
				case 187:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 93;
					break;
				case 188:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 93;
					break;
				case 189:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 94;
					break;
				case 190:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 94;
					break;
				case 191:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 95;
					break;
				case 192:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 95;
					break;
				case 193:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 96;
					break;
				case 194:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 96;
					break;
				case 195:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 97;
					break;
				case 196:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 97;
					break;
				case 197:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 98;
					break;
				case 198:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 98;
					break;
				case 199:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 99;
					break;
				case 200:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 99;
					break;
				case 201:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 100;
					break;
				case 202:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 100;
					break;
				case 203:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 101;
					break;
				case 204:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 101;
					break;
				case 205:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 102;
					break;
				case 226:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 102;
					break;
				case 227:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 103;
					break;
				case 228:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 103;
					break;
				case 229:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 104;
					break;
				case 230:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 104;
					break;
				case 231:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 105;
					break;
				case 232:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 105;
					break;
				case 233:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 106;
					break;
				case 234:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 106;
					break;
				case 235:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 107;
					break;
				case 236:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 107;
					break;
				case 237:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 108;
					break;
				case 238:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 108;
					break;
				case 239:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 109;
					break;
				case 240:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 109;
					break;
				case 241:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 120;
					break;
				case 242:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 120;
					break;
				case 243:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 121;
					break;
				case 244:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 121;
					break;
				case 245:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 122;
					break;
				case 246:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 122;
					break;
				case 247:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 123;
					break;
				case 248:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 123;
					break;
				case 249:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 124;
					break;
				case 250:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 124;
					break;
				case 251:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 125;
					break;
				case 252:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 125;
					break;
				case 253:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 126;
					break;
				case 254:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 126;
					break;
				case 255:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 127;
					break;
				case 256:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 127;
					break;
				case 257:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 128;
					break;
				case 258:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 128;
					break;
				case 259:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 129;
					break;
				case 260:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 129;
					break;
				case 261:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 130;
					break;
				case 262:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 130;
					break;
				case 263:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 131;
					break;
				case 264:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 131;
					break;
				case 265:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 132;
					break;
				case 266:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 132;
					break;
				case 267:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 133;
					break;
				case 268:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 133;
					break;
				case 269:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 134;
					break;
				case 270:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 134;
					break;
				case 271:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 135;
					break;
				case 272:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 135;
					break;
				case 273:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 136;
					break;
				case 274:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 136;
					break;
				case 275:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 137;
					break;
				case 276:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 137;
					break;
				case 277:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 137;
					break;
				case 278:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 137;
					break;
				case 279:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw() + 135;
					break;
				case 280:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 138;
					break;
				case 281:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 139;
					break;
				case 282:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 139;
					break;
				case 283:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 140;
					break;
				case 284:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 141;
					break;
				case 285:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 141;
					break;
				case 286:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 142;
					break;
				case 287:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 142;
					break;
				case 288:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 143;
					break;
				case 289:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 143;
					break;
				case 290:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 144;
					break;
				case 291:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 144;
					break;
				case 292:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 145;
					break;
				case 293:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 145;
					break;
				case 294:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 146;
					break;
				case 295:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 146;
					break;
				case 296:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 147;
					break;
				case 297:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 147;
					break;
				case 298:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 148;
					break;
				case 299:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 148;
					break;
				case 300:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 149;
					break;
				case 301:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 149;
					break;
				case 302:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 150;
					break;
				case 303:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 150;
					break;
				case 304:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 151;
					break;
				case 305:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 151;
					break;
				case 306:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 152;
					break;
				case 307:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 152;
					break;
				case 308:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 153;
					break;
				case 309:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 153;
					break;
				case 310:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 154;
					break;
				case 311:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 154;
					break;
				case 312:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 155;
					break;
				case 313:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 155;
					break;
				case 314:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 156;
					break;
				case 315:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 156;
					break;
				case 316:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 157;
					break;
				case 317:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 157;
					break;
				case 318:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 158;
					break;
				case 319:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 158;
					break;
				case 320:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 159;
					break;
				case 321:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 159;
					break;
				case 322:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 160;
					break;
				case 323:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 161;
					break;
				case 324:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 161;
					break;
				case 325:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 162;
					break;
				case 326:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 162;
					break;
				case 327:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 163;
					break;
				case 328:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 163;
					break;
				case 329:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 164;
					break;
				case 330:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 164;
					break;
				case 331:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 165;
					break;
				case 332:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 165;
					break;
				case 333:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 166;
					break;
				case 334:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 166;
					break;
				case 335:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 167;
					break;
				case 336:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 167;
					break;
				case 337:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 168;
					break;
				case 338:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 168;
					break;
				case 339:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 169;
					break;
				case 340:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 169;
					break;
				case 341:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 170;
					break;
				case 342:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 170;
					break;
				case 343:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 171;
					break;
				case 344:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 171;
					break;
				case 345:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 172;
					break;
				case 346:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 172;
					break;
				case 347:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 173;
					break;
				case 348:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 173;
					break;
				case 349:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 174;
					break;
				case 350:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 174;
					break;
				case 351:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 175;
					break;
				case 352:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 175;
					break;
				case 353:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 176;
					break;
				case 354:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 176;
					break;
				case 355:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 177;
					break;
				case 356:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 177;
					break;
				case 357:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 178;
					break;
				case 358:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 178;
					break;
				case 359:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 179;
					break;
				case 360:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 179;
					break;
				case 361:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y - 180;
					break;
				case 362:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 180;
					break;
				case 363:
					pEntity->GetEyeAnglesXY()->y = pEntity->GetEyeAnglesXY()->y + 138;
					break;

				}
			}
			else if (MeetsLBYReq && lbyupdated && !Resolver::didhitHS)
			{
				pEntity->GetEyeAnglesXY()->y = pEntity->GetLowerBodyYaw();
			}
			else if (!MeetsLBYReq || !lbyupdated && !Resolver::didhitHS)
			{
				pEntity->GetEyeAnglesXY()->y = rand() % 180 - rand() % 35;
			}
			else
				pEntity->GetEyeAnglesXY()->y = rand() % 180;
			LowerBodyYawFix(pEntity);
			PitchCorrection();
		}
		}
		

void ResolverSetup::StoreFGE(IClientEntity* pEntity)
{
	ResolverSetup::storedanglesFGE = pEntity->GetEyeAnglesXY()->y;
	ResolverSetup::storedlbyFGE = pEntity->GetLowerBodyYaw();
	ResolverSetup::storedsimtimeFGE = pEntity->GetSimulationTime();
}

void ResolverSetup::StoreThings(IClientEntity* pEntity)
{
	ResolverSetup::StoredAngles[pEntity->GetIndex()] = *pEntity->GetEyeAnglesXY();
	ResolverSetup::storedlby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
	ResolverSetup::storedsimtime = pEntity->GetSimulationTime();
	ResolverSetup::storeddelta[pEntity->GetIndex()] = pEntity->GetEyeAnglesXY()->y;
	ResolverSetup::storedlby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
}

void ResolverSetup::CM(IClientEntity* pEntity)
{
	for (int x = 1; x < Interfaces::Engine->GetMaxClients(); x++)
	{

		pEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(x);

		if (!pEntity
			|| pEntity == hackManager.pLocal()
			|| pEntity->IsDormant()
			|| !pEntity->IsAlive())
			continue;

		ResolverSetup::StoreThings(pEntity);
	}
}


void AngleCorrect(IClientEntity* pEntity)
{
	if (Menu::Window.RageBotTab.AccuracyResolver2.GetState())
	{
		auto delta = pEntity->GetLowerBodyYaw();
		switch (Globals::Shots % 0)

			if (fabsf(pEntity->GetVelocity().Length2D() < 32)) // Fakewalk fix
			{
				pEntity->GetEyeAnglesPointer()->y = pEntity->GetLowerBodyYaw();
			}
			else
			{
				pEntity->GetEyeAnglesPointer()->y = Globals::Shots * 45;
			}
	}
}
	
void ResolverSetup::FSN(IClientEntity* pEntity, ClientFrameStage_t stage)
{
	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = 1; i < Interfaces::Engine->GetMaxClients(); i++)
		{

			pEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(i);

			if (!pEntity
				|| pEntity == hackManager.pLocal()
				|| pEntity->IsDormant()
				|| !pEntity->IsAlive())
				continue;

			ResolverSetup::Resolve(pEntity);
		}
	}
}