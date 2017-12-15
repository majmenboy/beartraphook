/*
Syn's AyyWare Framework 2015
*/

#include "ESP.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "GlowManager.h"
#include "EdgyLagComp.h"
#include "MemePredict.h"
#include "SDK.h"
#include "MiscClasses.h"

DWORD GlowManager = *(DWORD*)(Utilities::Memory::FindPatternV2("client.dll", "0F 11 05 ?? ?? ?? ?? 83 C8 01 C7 05 ?? ?? ?? ?? 00 00 00 00") + 3);
IClientEntity *BombCarrier;
void CEsp::Init()
{
	BombCarrier = nullptr;
}

// Yeah dude we're defo gunna do some sick moves for the esp yeah
void CEsp::Move(CUserCmd *pCmd,bool &bSendPacket) 
{

}

// Main ESP Drawing loop
void CEsp::Draw()
{
	IClientEntity *pLocal = hackManager.pLocal();

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
			player_info_t pinfo;

			if (pEntity &&  pEntity != pLocal && !pEntity->IsDormant())
			{
				if (Menu::Window.VisualsTab.OtherRadar.GetState())
				{
					DWORD m_bSpotted = NetVar.GetNetVar(0x839EB159);
					*(char*)((DWORD)(pEntity)+m_bSpotted) = 1;
				}

				if (Menu::Window.VisualsTab.FiltersPlayers.GetState() && Interfaces::Engine->GetPlayerInfo(i, &pinfo) && pEntity->IsAlive())
				{
					DrawPlayer(pEntity, pinfo);
				}

				ClientClass* cClass = (ClientClass*)pEntity->GetClientClass();

				if (Menu::Window.VisualsTab.FiltersWeapons.GetState() && cClass->m_ClassID != (int)CSGOClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)CSGOClassID::CDEagle || cClass->m_ClassID == (int)CSGOClassID::CAK47)))
				{
					DrawDrop(pEntity, cClass);
				}

				if (Menu::Window.VisualsTab.FiltersC4.GetState())
				{
					if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
						DrawBombPlanted(pEntity, cClass);

					if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
						BombTimer(pEntity, cClass);

					if (cClass->m_ClassID == (int)CSGOClassID::CC4)
						DrawBomb(pEntity, cClass);
				}

				if (Menu::Window.VisualsTab.FiltersChickens.GetState())
				{
					if (cClass->m_ClassID == (int)CSGOClassID::CChicken)
						DrawChicken(pEntity, cClass);


				}
			}
		}

		if (Menu::Window.MiscTab.OtherSpectators.GetState())
		{
			SpecList();
		}

}

void CEsp::BacktrackingCross(IClientEntity* pEntity)
{
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		player_info_t pinfo;
		if (pEntity && pEntity != hackManager.pLocal() && !pEntity->IsDormant())
		{
			if (Interfaces::Engine->GetPlayerInfo(i, &pinfo) && pEntity->IsAlive())
			{
				if (Menu::Window.LegitBotTab.AimbotBacktrack.GetState())
				{
					if (hackManager.pLocal()->IsAlive())
					{
						for (int t = 0; t < 12; ++t)
						{
							Vector screenbacktrack[64][12];

							if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > hackManager.pLocal()->GetSimulationTime())
							{
								if (Render::WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
								{

									Interfaces::Surface->DrawSetColor(Color(255, 0, 0, 255));
									Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);

								}
							}
						}
					}
					else
					{
						memset(&headPositions[0][0], 0, sizeof(headPositions));
					}
				}
				if (Menu::Window.RageBotTab.AccuracyBacktracking.GetState())
				{
					if (hackManager.pLocal()->IsAlive())
					{
					Vector screenbacktrack[64];

					if (backtracking->records[i].tick_count + 12 > Interfaces::Globals->tickcount)
					{
					if (Render::WorldToScreen(backtracking->records[i].headPosition, screenbacktrack[i]))
					{

					Interfaces::Surface->DrawSetColor(Color(255, 0, 0, 255));
					Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i].x, screenbacktrack[i].y, screenbacktrack[i].x + 2, screenbacktrack[i].y + 2);

					}
					}
					}
					if (hackManager.pLocal()->IsAlive())
					{
						for (int t = 0; t < 12; ++t)
						{
							Vector screenbacktrack[64];

							if (backtracking->records[i].tick_count + 12 > Interfaces::Globals->tickcount)
							{
								if (Render::WorldToScreen(backtracking->records[i].headPosition, screenbacktrack[i]))
								{

									Interfaces::Surface->DrawSetColor(Color(255, 0, 0, 255));
									Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i].x, screenbacktrack[i].y, screenbacktrack[i].x + 2, screenbacktrack[i].y + 2);

								}
							}
						}
					}
					else
					{
						memset(&backtracking->records[0], 0, sizeof(backtracking->records));
					}
				}
			}
		}
	}
}

void CEsp::SpecList()
{
	IClientEntity *pLocal = hackManager.pLocal();

	RECT scrn = Render::GetViewport();
	int ayy = 0;

	// Loop through all active entitys
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something
		if (pEntity &&  pEntity != pLocal)
		{
			if (Interfaces::Engine->GetPlayerInfo(i, &pinfo) && !pEntity->IsAlive() && !pEntity->IsDormant())
			{
				HANDLE obs = pEntity->GetObserverTargetHandle();

				if (obs)
				{
					IClientEntity *pTarget = Interfaces::EntList->GetClientEntityFromHandle(obs);
					player_info_t pinfo2;
					if (pTarget)
					{
						if (Interfaces::Engine->GetPlayerInfo(pTarget->GetIndex(), &pinfo2))
						{
							char buf[500]; sprintf_s(buf, "%s is spectating %s", pinfo.name, pinfo2.name);
							RECT TextSize = Render::GetTextSize(Render::Fonts::ESP, buf);
							Render::Clear(scrn.right - 260, (scrn.bottom / 2) + (16 * ayy), 260, 16, Color(0, 0, 0, 0));
							Render::Text(scrn.right - TextSize.right - 4, (scrn.bottom / 2) + (16 * ayy), pTarget->GetIndex() == pLocal->GetIndex() ? Color(240, 0, 0, 200) : Color(255, 255, 255, 200), Render::Fonts::ESP, buf);
							ayy++;
						}
					}
				}
			}
		}
	}

	Render::Outline(scrn.right - 261, (scrn.bottom / 2) - 1, 262, (16 * ayy) + 2, Color(23, 23, 23, 0));
	Render::Outline(scrn.right - 260, (scrn.bottom / 2), 260, (16 * ayy), Color(90, 90, 90, 0));
}

//  Yeah m8
void CEsp::DrawPlayer(IClientEntity* pEntity, player_info_t pinfo)
{
	ESPBox Box;
	Color Color;

	// Show own team false? well gtfo teammate lol
	if (Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && (pEntity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
		return;

	if (GetBox(pEntity, Box))
	{
		Color = GetPlayerColor(pEntity);

		if (Menu::Window.VisualsTab.OptionsBox.GetIndex())
			DrawBox(Box, Color);

		if (Menu::Window.VisualsTab.OptionsName.GetState())
			DrawName(pinfo, Box);

		if (Menu::Window.VisualsTab.OptionsHealth.GetState())
			DrawHealth(Box, pEntity);

		if (Menu::Window.VisualsTab.OptionsInfo.GetState() || Menu::Window.VisualsTab.OptionsWeapon.GetState())
			DrawInfo(pEntity, Box);
//		    Drawbeartrap(pEntity, Box);

		if (Menu::Window.VisualsTab.OptionsMoney.GetState())
			DrawMoney(pEntity, Box);

		if (Menu::Window.VisualsTab.OptionsAimSpot.GetState())
			DrawCross(pEntity);

		if (Menu::Window.VisualsTab.OptionsArmor.GetState())
			Armor(Box, pEntity);

		if (Menu::Window.VisualsTab.OptionsSkeleton.GetIndex() == 1)
			DrawSkeleton(pEntity);

		if (Menu::Window.VisualsTab.OptionsSkeleton.GetIndex() == 2)
			HealthSkeleton(pEntity);

		if (Menu::Window.VisualsTab.OptionsGlow.GetState())
			DrawGlow();

		if (Menu::Window.VisualsTab.OptionsBarrels.GetState())
			Barrel(Box, Color, pEntity);

		if (Menu::Window.VisualsTab.BacktrackingLol.GetState())
	    	BacktrackingCross(pEntity);

	}
}


// Gets the 2D bounding box for the entity
// Returns false on failure nigga don't fail me
bool CEsp::GetBox(IClientEntity* pEntity, CEsp::ESPBox &result)
{
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetOrigin();
	min = pEntity->collisionProperty()->GetMins() + vOrigin;
	max = pEntity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return true;
}

void CEsp::DrawGlow()
{
	int GlowR = Menu::Window.ColorsTab.GlowR.GetValue();
	int GlowG = Menu::Window.ColorsTab.GlowG.GetValue();
	int GlowB = Menu::Window.ColorsTab.GlowB.GetValue();

	CGlowObjectManager* GlowObjectManager = (CGlowObjectManager*)GlowManager;

	for (int i = 0; i < GlowObjectManager->size; ++i)
	{
		CGlowObjectManager::GlowObjectDefinition_t* glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[i];
		IClientEntity* Entity = glowEntity->getEntity();

		if (glowEntity->IsEmpty() || !Entity)
			continue;

		switch (Entity->GetClientClass()->m_ClassID)
		{
		case 35:
			if (Menu::Window.VisualsTab.OptionsGlow.GetState())
			{
				if (!Menu::Window.VisualsTab.FiltersPlayers.GetState() && !(Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
					break;
				if (Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && (Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
					break;

				if (GameUtils::IsVisible(hackManager.pLocal(), Entity, 0))
				{
					glowEntity->set((Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()) ? Color(GlowR, GlowG, GlowB) : Color(GlowR, GlowG, GlowB));
				}

				else
				{
					glowEntity->set((Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()) ? Color(GlowR, GlowG, GlowB) : Color(GlowR, GlowG, GlowB));
				}
			}
		}
	}
}

void CEsp::Barrel(CEsp::ESPBox size, Color color, IClientEntity* pEntity)
{
	Vector src3D, src;
	src3D = pEntity->GetOrigin() - Vector(0, 0, 0);

	if (!Render::WorldToScreen(src3D, src))
		return;

	int ScreenWidth, ScreenHeight;
	Interfaces::Engine->GetScreenSize(ScreenWidth, ScreenHeight);

	int x = (int)(ScreenWidth * 0.5f);
	int y = 0;


	y = ScreenHeight;

	Render::Line((int)(src.x), (int)(src.y), x, y, Color(0, 0, 0, 255));
}

void CEsp::DrawMoney(IClientEntity* pEntity, CEsp::ESPBox size)
{
	ESPBox ArmorBar = size;

	int MoneyEnemy = 100;
	MoneyEnemy = pEntity->GetMoney();
	char nameBuffer[512];
	sprintf_s(nameBuffer, "%d $", MoneyEnemy);

	RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, nameBuffer);
	Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 27, Color(255, 255, 0, 255), Render::Fonts::ESP, nameBuffer);
}

// Get an entities color depending on team and vis ect
Color CEsp::GetPlayerColor(IClientEntity* pEntity)
{
	int TeamNum = pEntity->GetTeamNum();
	bool IsVis = GameUtils::IsVisible(hackManager.pLocal(), pEntity, (int)CSGOHitboxID::Head);

	Color color;

	if (TeamNum == TEAM_CS_T)
	{
		if (IsVis)
			color = Color(Menu::Window.ColorsTab.TColorVisR.GetValue(), Menu::Window.ColorsTab.TColorVisG.GetValue(), Menu::Window.ColorsTab.TColorVisB.GetValue(), 255);
		else
			color = Color(Menu::Window.ColorsTab.TColorNoVisR.GetValue(), Menu::Window.ColorsTab.TColorNoVisG.GetValue(), Menu::Window.ColorsTab.TColorNoVisB.GetValue(), 255);
	}
	else
	{
		if (IsVis)
			color = Color(Menu::Window.ColorsTab.CTColorVisR.GetValue(), Menu::Window.ColorsTab.CTColorVisG.GetValue(), Menu::Window.ColorsTab.CTColorVisB.GetValue(), 255);
		else
			color = Color(Menu::Window.ColorsTab.CTColorNoVisR.GetValue(), Menu::Window.ColorsTab.CTColorNoVisG.GetValue(), Menu::Window.ColorsTab.CTColorNoVisB.GetValue(), 255);
	}

	return color;
}

// 2D  Esp box
void CEsp::DrawBox(CEsp::ESPBox size, Color color)
{
	{
		{
			int VertLine;
			int HorzLine;
			// Corner Box
			//bool IsVis = GameUtils::IsVisible(hackManager.pLocal(), pEntity, (int)CSGOHitboxID::Chest);  da dream
			int xd = Menu::Window.VisualsTab.OptionsBox.GetIndex();
			if (Menu::Window.VisualsTab.OptionsBox.GetIndex() == 1)
			{
				// Corner Box
				int VertLine = (((float)size.w) * (0.20f));
				int HorzLine = (((float)size.h) * (0.20f));

				Render::Clear(size.x, size.y - 1, VertLine, 1, Color(10, 10, 10, 240));
				Render::Clear(size.x + size.w - VertLine, size.y - 1, VertLine, 1, Color(10, 10, 10, 240));
				Render::Clear(size.x, size.y + size.h - 1, VertLine, 1, Color(10, 10, 10, 240));
				Render::Clear(size.x + size.w - VertLine, size.y + size.h - 1, VertLine, 1, Color(10, 10, 10, 240));

				Render::Clear(size.x - 1, size.y, 1, HorzLine, Color(10, 10, 10, 240));
				Render::Clear(size.x - 1, size.y + size.h - HorzLine, 1, HorzLine, Color(10, 10, 10, 240));
				Render::Clear(size.x + size.w - 1, size.y, 1, HorzLine, Color(10, 10, 10, 240));
				Render::Clear(size.x + size.w - 1, size.y + size.h - HorzLine, 1, HorzLine, Color(10, 10, 10, 240));

				Render::Clear(size.x, size.y, VertLine, 1, color);
				Render::Clear(size.x + size.w - VertLine, size.y, VertLine, 1, color);
				Render::Clear(size.x, size.y + size.h, VertLine, 1, color);
				Render::Clear(size.x + size.w - VertLine, size.y + size.h, VertLine, 1, color);

				Render::Clear(size.x, size.y, 1, HorzLine, color);
				Render::Clear(size.x, size.y + size.h - HorzLine, 1, HorzLine, color);
				Render::Clear(size.x + size.w, size.y, 1, HorzLine, color);
				Render::Clear(size.x + size.w, size.y + size.h - HorzLine, 1, HorzLine, color);
			}
			if (Menu::Window.VisualsTab.OptionsBox.GetIndex() == 2)
			{
				{
					Render::Outline(size.x, size.y, size.w, size.h, color);
					Render::Outline(size.x - 1, size.y - 1, size.w + 2, size.h + 2, Color(10, 10, 10, 240));
					Render::Outline(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(10, 10, 10, 240));
				}
			}

			if (Menu::Window.VisualsTab.OptionsBox.GetIndex() == 3)
			{




				int VertLine = (((float)size.w) * (0.20f));
				int HorzLine = (((float)size.h) * (0.20f));

				int BVertLine = (((float)size.w) * (0.25f));
				int BHorzLine = (((float)size.h) * (1.0f));


		/*		Render::Clear(size.x, size.y - 1, VertLine, 1, Color(190, 10, 255, 255));
				Render::Clear(size.x + size.w - VertLine, size.y - 1, VertLine, 1, Color(190, 10, 255, 255));
				Render::Clear(size.x, size.y + size.h - 1, VertLine, 1, Color(190, 10, 255, 255));
				Render::Clear(size.x + size.w - VertLine, size.y + size.h - 1, VertLine, 1, Color(190, 10, 255, 255));
				Render::Clear(size.x - 1, size.y, 1, BHorzLine, Color(190, 10, 255, 255));
				Render::Clear(size.x + size.w - 1, size.y, 1, BHorzLine, Color(190, 10, 255, 255));*/

				Render::Clear(size.x, size.y, VertLine, 1, color);	Render::Clear(size.x + size.w - VertLine, size.y, VertLine, 1, color);
				Render::Clear(size.x, size.y + size.h, VertLine, 1, color);
				Render::Clear(size.x + size.w - VertLine, size.y + size.h, VertLine, 1, color);
				Render::Clear(size.x, size.y, 1, BHorzLine, color);

				Render::Clear(size.x, size.y + size.h - BHorzLine, 1, BHorzLine, color);
				Render::Clear(size.x + size.w, size.y, 1, BHorzLine, color);
				Render::Clear(size.x + size.w, size.y + size.h - BHorzLine, 1, BHorzLine, color);

			}

		}
	}
}


// Unicode Conversions
static wchar_t* CharToWideChar(const char* text)
{
	size_t size = strlen(text) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs_s(NULL, wa, size/4, text, size);
	return wa;
}

// Player name
void CEsp::DrawName(player_info_t pinfo, CEsp::ESPBox size)
{
	if (strlen(pinfo.name) > 16)
	{
		pinfo.name[16] = 0;
		strcat(pinfo.name, "...");
		puts(pinfo.name);
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16, Color(255, 255, 255, 255), Render::Fonts::ESP, pinfo.name);
	}
	else
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16, Color(255, 255, 255, 255), Render::Fonts::ESP, pinfo.name);
	}
}

void CEsp::Armor(ESPBox box, IClientEntity* pEntity)
{
	float armor = pEntity->ArmorValue();

	if (armor > 100)
		armor = 100;

	Interfaces::Surface->DrawSetColor(0, 0, 0, 150);
	if (Menu::Window.VisualsTab.OptionsArmor.GetState())
	{
		Interfaces::Surface->DrawOutlinedRect(box.x + box.w + 1, box.y - 1, box.x + box.w + 5, box.y + box.h + 1);

		int height = armor * box.h / 100;

		if (armor > 0) {
			Interfaces::Surface->DrawSetColor(50, 50, 190, 255);
			Interfaces::Surface->DrawFilledRect(box.x + box.w + 2,
				box.y + box.h - height,
				box.x + box.w + 4,
				box.y + box.h);
		}
	}
}

// Draw a health bar. For Tf2 when a bar is bigger than max health a second bar is displayed
void CEsp::DrawHealth(ESPBox box, IClientEntity* pEntity)
{
	float health = pEntity->GetHealth();

	if (health > 100)
		health = 100;

	int colors[2] = { 255 - (health * 2.55), health * 2.55 };

	Interfaces::Surface->DrawSetColor(0, 0, 0, 150);
	if (Menu::Window.VisualsTab.OptionsHealth.GetState())
	{
		Interfaces::Surface->DrawOutlinedRect(box.x - 5, box.y - 1, box.x - 1, box.y + box.h + 1);

		int height = health * box.h / 100;

		if (health > 0)
		{
			Interfaces::Surface->DrawSetColor(colors[0], colors[1], 0, 255);
			Interfaces::Surface->DrawFilledRect(box.x - 4,
				box.y + box.h - height,
				box.x - 2,
				box.y + box.h);
		}
	}
}

// Cleans the internal class name up to something human readable and nice
std::string CleanItemName(std::string name)
{
	std::string Name = name;
	// Tidy up the weapon Name
	if (Name[0] == 'C')
		Name.erase(Name.begin());

	// Remove the word Weapon
	auto startOfWeap = Name.find("Weapon");
	if (startOfWeap != std::string::npos)
		Name.erase(Name.begin() + startOfWeap, Name.begin() + startOfWeap + 6);

	return Name;
}

// Anything else: weapons, class state? idk
void CEsp::DrawInfo(IClientEntity* pEntity, CEsp::ESPBox size)
{
	std::vector<std::string> Info;

	// Player Weapon ESP
	IClientEntity* pWeapon = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	if (Menu::Window.VisualsTab.OptionsWeapon.GetState() && pWeapon)

		if (Menu::Window.VisualsTab.OptionsWeapon.GetState() && pWeapon)
		{
			ClientClass* cClass = (ClientClass*)pWeapon->GetClientClass();
			if (cClass)
			{
				// Draw it
				Info.push_back(CleanItemName(cClass->m_pNetworkName));
			}
		}

	std::vector<std::string> weapon;

	if (Menu::Window.VisualsTab.OptionsInfo.GetState() && pEntity == BombCarrier)
	{
		Info.push_back("Has Bomb");
	}

	// Is Scoped
	if (Menu::Window.VisualsTab.IsScoped.GetState() && pEntity->IsScoped())
	{
		Info.push_back("Zoom");
	}


	//Resolver Info
	if (Menu::Window.VisualsTab.ResolverInfo.GetState())
	{
		if (pEntity->GetVelocity().Length2D() < 1)
			Info.push_back("Fake");
	}

	static RECT Size = Render::GetTextSize(Render::Fonts::Menu, " Text");
	int i = 0;
	for (auto Text : Info)
	{
	/*	Render::Text(size.x + (size.w / 2) - (Size.right / 2), size.y + size.h + 8,
			Color(255, 255, 255, 255), Render::Fonts::ESP, Text.c_str());
		i++;*/
				Render::Text(size.x + size.w + 3, size.y + (i*(Size.bottom)), Color(250, 250, 250, 255), Render::Fonts::ESP, Text.c_str());
		i++;
	}
}

/*
void CEsp::Drawbeartrap(IClientEntity* pEntity, CEsp::ESPBox size)
{
	std::vector<std::string> Info;

	// Player Weapon ESP
	IClientEntity* pWeapon = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	if (Menu::Window.VisualsTab.OptionsWeapon.GetState() && pWeapon)
	{
		ClientClass* cClass = (ClientClass*)pWeapon->GetClientClass();
		if (cClass)
		{
			// Draw it
			Info.push_back(CleanItemName(cClass->m_pNetworkName));
		}
	}

	static RECT Size = Render::GetTextSize(Render::Fonts::Menu, " Text");
	int i = 0;
	for (auto Text : Info)
	{
			Render::Text(size.x + (size.w / 2) - (Size.right / 2), size.y + size.h - 16,
		Color(255, 255, 255, 255), Render::Fonts::ESP, Text.c_str());
		i++;
	}
}*/

// Little cross on their heads
void CEsp::DrawCross(IClientEntity* pEntity)
{
	Vector cross = pEntity->GetHeadPos(), screen;
	static int Scale = 2;
	if (Render::WorldToScreen(cross, screen))
	{
		Render::Clear(screen.x - Scale, screen.y - (Scale * 2), (Scale * 2), (Scale * 4), Color(20, 20, 20, 160));
		Render::Clear(screen.x - (Scale * 2), screen.y - Scale, (Scale * 4), (Scale * 2), Color(20, 20, 20, 160));
		Render::Clear(screen.x - Scale - 1, screen.y - (Scale * 2) - 1, (Scale * 2) - 2, (Scale * 4) - 2, Color(250, 250, 250, 160));
		Render::Clear(screen.x - (Scale * 2) - 1, screen.y - Scale - 1, (Scale * 4) - 2, (Scale * 2) - 2, Color(250, 250, 250, 160));
	}
}

// Draws a dropped CS:GO Item
void CEsp::DrawDrop(IClientEntity* pEntity, ClientClass* cClass)
{
	Vector Box;
	CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)pEntity;
	IClientEntity* plr = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)Weapon->GetOwnerHandle());
	if (!plr && Render::WorldToScreen(Weapon->GetOrigin(), Box))
	{
		ESPBox Box;
		if (Menu::Window.VisualsTab.OptionsBox.GetIndex())
		if (GetBox(pEntity, Box))
		{
			DrawBox(Box, Color(255, 255, 255, 255));
		}

		if (Menu::Window.VisualsTab.OptionsInfo.GetState())
		{
			std::string ItemName = CleanItemName(cClass->m_pNetworkName);
			RECT TextSize = Render::GetTextSize(Render::Fonts::ESP, ItemName.c_str());
			Render::Text(Box.x, Box.y, Color(255, 255, 255, 255), Render::Fonts::ESP, ItemName.c_str());
		}
	}
}

// Draws a chicken
void CEsp::DrawChicken(IClientEntity* pEntity, ClientClass* cClass)
{
	ESPBox Box;

	if (GetBox(pEntity, Box))
	{
		player_info_t pinfo; strcpy_s(pinfo.name, "Chicken");
		if (Menu::Window.VisualsTab.OptionsBox.GetIndex())
			DrawBox(Box, Color(255,255,255,255));

		if (Menu::Window.VisualsTab.OptionsName.GetState())
			DrawName(pinfo, Box);
	}
}

// Draw the planted bomb and timer
void CEsp::DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass)
{
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntity;

	float flBlow = Bomb->GetC4BlowTime();
	float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
	char buffer[64];
	/*sprintf_s(buffer, "%.1fs", TimeRemaining);*/
	float TimeRemaining2;
	bool exploded = true;
	if (TimeRemaining < 0)
	{
		!exploded;

		TimeRemaining2 = 0;
	}
	else
	{
		exploded = true;
		TimeRemaining2 = TimeRemaining;
	}
	if (exploded)
	{
		sprintf_s(buffer, "Bomb", TimeRemaining2);
	}
	else
	{
		sprintf_s(buffer, "Bomb", TimeRemaining2);
	}
}

void CEsp::BombTimer(IClientEntity* pEntity, ClientClass* cClass)
{
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntity;

	if (Render::WorldToScreen(vOrig, vScreen))
	{

		ESPBox Box;
		GetBox(pEntity, Box);
		DrawBox(Box, Color(250, 42, 42, 255));
		float flBlow = Bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
		float TimeRemaining2;
		bool exploded = true;
		if (TimeRemaining < 0)
		{
			!exploded;

			TimeRemaining2 = 0;
		}
		else
		{
			exploded = true;
			TimeRemaining2 = TimeRemaining;
		}
		char buffer[64];
		if (exploded)
		{
			sprintf_s(buffer, "Bomb", TimeRemaining2);
		}
		else
		{
			sprintf_s(buffer, "Bomb", TimeRemaining2);
		}
		Render::Text(vScreen.x, vScreen.y, Color(169, 0, 50, 255), Render::Fonts::ESP, buffer);
	}
}


void CEsp::DrawBomb(IClientEntity* pEntity, ClientClass* cClass)
{
	BombCarrier = nullptr;
	C_BaseCombatWeapon *BombWeapon = (C_BaseCombatWeapon *)pEntity;
	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	bool adopted = true;
	HANDLE parent = BombWeapon->GetOwnerHandle();
	if (parent || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
	{
		IClientEntity* pParentEnt = (Interfaces::EntList->GetClientEntityFromHandle(parent));
		if (pParentEnt && pParentEnt->IsAlive())
		{
			BombCarrier = pParentEnt;
			adopted = false;
		}
	}

	if (adopted)
	{
		if (Render::WorldToScreen(vOrig, vScreen))
		{
			Render::Text(vScreen.x, vScreen.y, Color(112, 230, 20, 255), Render::Fonts::ESP, " ");
		}
	}
}

void DrawBoneArray(int* boneNumbers, int amount, IClientEntity* pEntity, Color color)
{
	Vector LastBoneScreen;
	for (int i = 0; i < amount; i++)
	{
		Vector Bone = pEntity->GetBonePos(boneNumbers[i]);
		Vector BoneScreen;

		if (Render::WorldToScreen(Bone, BoneScreen))
		{
			if (i>0)
			{
				Render::Line(LastBoneScreen.x, LastBoneScreen.y, BoneScreen.x, BoneScreen.y, color);
			}
		}
		LastBoneScreen = BoneScreen;
	}
}

void DrawBoneTest(IClientEntity *pEntity)
{
	for (int i = 0; i < 127; i++)
	{
		Vector BoneLoc = pEntity->GetBonePos(i);
		Vector BoneScreen;
		if (Render::WorldToScreen(BoneLoc, BoneScreen))
		{
			char buf[10];
			_itoa_s(i, buf, 10);
			Render::Text(BoneScreen.x, BoneScreen.y, Color(255, 255, 255, 180), Render::Fonts::ESP, buf);
		}
	}
}

void CEsp::DrawSkeleton(IClientEntity* pEntity)
{
	studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());

	if (!pStudioHdr)
		return;

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = pEntity->GetBonePos(j);
			vParent = pEntity->GetBonePos(pBone->parent);

			if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
			{
				Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(0, 0, 0,255));
			}
		}
	}
}

void CEsp::HealthSkeleton(IClientEntity* pEntity)
{
	if (Menu::Window.VisualsTab.OptionsSkeleton.GetIndex() == 2)
	{
		studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());

		if (!pStudioHdr)
			return;
		int HPEnemy = 100;
		HPEnemy = pEntity->GetHealth();
		char nameBuffer[512];
		sprintf(nameBuffer, "%d", HPEnemy);


		RECT scrn = Render::GetViewport();
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())


			float health = pEntity->GetHealth();



		Vector vParent, vChild, sParent, sChild;

		for (int j = 0; j < pStudioHdr->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioHdr->GetBone(j);

			if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
			{
				vChild = pEntity->GetBonePos(j);
				vParent = pEntity->GetBonePos(pBone->parent);
				if (pEntity->GetHealth() > 85)
				{

					if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
					{
						Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(250, 250, 250, 255));
					}
				}
				else if (pEntity->GetHealth() < 85 & pEntity->GetHealth() > 60)
				{

					if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
					{
						Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(20, 250, 140, 255));
					}
				}
				else if (pEntity->GetHealth() < 60 & pEntity->GetHealth() > 35)
				{

					if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
					{
						Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(252, 150, 40, 255));
					}
				}
				else if (pEntity->GetHealth() < 60 & pEntity->GetHealth() > 30)
				{

					if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
					{
						Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(252, 150, 40, 255));
					}
				}
				else if (pEntity->GetHealth() < 30 & pEntity->GetHealth() > 0)
				{

					if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
					{
						Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(252, 1, 80, 255));
					}
				}
			}
		}
	}
}

void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3750() {
	float pJunkcode = 2210609338.75329;
	pJunkcode = 9028265334.71835;
	if (pJunkcode = 7079023288.84429)
		pJunkcode = 5955112989.93458;
	pJunkcode = 4084901470.30295;
	pJunkcode = 7788513307.51434;
	if (pJunkcode = 1458550909.4147)
		pJunkcode = 7489536931.13972;
	pJunkcode = 8095018362.0862;
	if (pJunkcode = 3797172664.30118)
		pJunkcode = 4656358803.1421;
	pJunkcode = 5653789014.96968;
	pJunkcode = 5048742265.17477;
	if (pJunkcode = 1889363797.81097)
		pJunkcode = 4520560994.40091;
	pJunkcode = 9844138885.38787;
	if (pJunkcode = 9867906761.16475)
		pJunkcode = 142368857.331224;
	pJunkcode = 4740566346.13738;
	pJunkcode = 7680342962.54831;
	if (pJunkcode = 8830473373.52665)
		pJunkcode = 6182645969.81259;
	pJunkcode = 2172400691.8397;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3749() {
	float pJunkcode = 9699024074.42711;
	pJunkcode = 5739073726.52439;
	if (pJunkcode = 2663460754.31639)
		pJunkcode = 2464818359.76357;
	pJunkcode = 1623062635.6837;
	pJunkcode = 7969455518.45027;
	if (pJunkcode = 8032911516.07451)
		pJunkcode = 3342565236.62936;
	pJunkcode = 3366077613.59785;
	if (pJunkcode = 5100060010.03416)
		pJunkcode = 4181162311.66737;
	pJunkcode = 3049389055.96326;
	pJunkcode = 1941790196.60287;
	if (pJunkcode = 8808097070.68136)
		pJunkcode = 9279471918.18836;
	pJunkcode = 944543048.366499;
	if (pJunkcode = 2311620653.06862)
		pJunkcode = 9140708463.4373;
	pJunkcode = 6020147724.18826;
	pJunkcode = 1556104727.90403;
	if (pJunkcode = 5357813099.42292)
		pJunkcode = 5180210850.25113;
	pJunkcode = 5339448104.81932;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3748() {
	float pJunkcode = 6544896253.4426;
	pJunkcode = 4494064.02237079;
	if (pJunkcode = 1598845922.79367)
		pJunkcode = 7700248067.56384;
	pJunkcode = 517737295.928498;
	pJunkcode = 2007322666.57248;
	if (pJunkcode = 6885834837.04258)
		pJunkcode = 4494440225.51786;
	pJunkcode = 4964508799.07487;
	if (pJunkcode = 9036098875.98339)
		pJunkcode = 8315999238.85276;
	pJunkcode = 9028821247.24543;
	pJunkcode = 2101607670.97427;
	if (pJunkcode = 1570771321.55206)
		pJunkcode = 5698916509.17217;
	pJunkcode = 827264711.159568;
	if (pJunkcode = 7412822150.24576)
		pJunkcode = 5761533221.25502;
	pJunkcode = 9268196054.51863;
	pJunkcode = 4963252292.17727;
	if (pJunkcode = 5299179684.00713)
		pJunkcode = 3928431138.12094;
	pJunkcode = 3094242229.51635;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3747() {
	float pJunkcode = 5197315847.86069;
	pJunkcode = 6868223058.33703;
	if (pJunkcode = 6633015840.6556)
		pJunkcode = 3763752695.13675;
	pJunkcode = 3527615295.73964;
	pJunkcode = 4093075942.07012;
	if (pJunkcode = 6569406655.96958)
		pJunkcode = 2346818205.88519;
	pJunkcode = 5794424122.99712;
	if (pJunkcode = 6963655298.65146)
		pJunkcode = 9112822685.95942;
	pJunkcode = 6592185993.94182;
	pJunkcode = 9256073631.75094;
	if (pJunkcode = 9223885394.22308)
		pJunkcode = 660459688.848274;
	pJunkcode = 6514828544.03393;
	if (pJunkcode = 8510301669.37056)
		pJunkcode = 8280659569.96081;
	pJunkcode = 259533918.906183;
	pJunkcode = 6526542557.99714;
	if (pJunkcode = 1887129763.55977)
		pJunkcode = 5760183715.74344;
	pJunkcode = 2635866405.71252;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3746() {
	float pJunkcode = 839456300.234887;
	pJunkcode = 3352375397.16018;
	if (pJunkcode = 6703615590.73649)
		pJunkcode = 1039328468.33813;
	pJunkcode = 6593704714.41642;
	pJunkcode = 7958788261.01195;
	if (pJunkcode = 8977497935.46102)
		pJunkcode = 6772332563.00409;
	pJunkcode = 4110765550.62492;
	if (pJunkcode = 9942480417.94674)
		pJunkcode = 5372295574.76748;
	pJunkcode = 814355752.433621;
	pJunkcode = 47219589.566384;
	if (pJunkcode = 5107482767.64592)
		pJunkcode = 8773301852.17853;
	pJunkcode = 3831688502.79533;
	if (pJunkcode = 4142741073.87563)
		pJunkcode = 8595445142.61263;
	pJunkcode = 6127048011.64874;
	pJunkcode = 4671611078.74561;
	if (pJunkcode = 6375664298.62086)
		pJunkcode = 2880547326.05457;
	pJunkcode = 3704979461.66104;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3745() {
	float pJunkcode = 2951964163.82526;
	pJunkcode = 2451940530.18338;
	if (pJunkcode = 5379530137.25195)
		pJunkcode = 8784274771.28578;
	pJunkcode = 6186611406.21958;
	pJunkcode = 3218477339.41646;
	if (pJunkcode = 863267423.871338)
		pJunkcode = 8494116739.41466;
	pJunkcode = 805297306.058222;
	if (pJunkcode = 2149523249.32851)
		pJunkcode = 7360598868.54786;
	pJunkcode = 2430475056.36421;
	pJunkcode = 7941704820.9761;
	if (pJunkcode = 2115651604.87377)
		pJunkcode = 2490738379.41658;
	pJunkcode = 5545609848.13878;
	if (pJunkcode = 6968671500.88693)
		pJunkcode = 1652100452.32236;
	pJunkcode = 7787895460.32709;
	pJunkcode = 752418871.092116;
	if (pJunkcode = 5252146664.66495)
		pJunkcode = 4580607710.28669;
	pJunkcode = 2827454246.66457;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3744() {
	float pJunkcode = 490463430.844105;
	pJunkcode = 975876265.815437;
	if (pJunkcode = 9503996067.06057)
		pJunkcode = 76698967.4333484;
	pJunkcode = 9208534636.89875;
	pJunkcode = 2053574102.21466;
	if (pJunkcode = 7693422230.53035)
		pJunkcode = 4306856491.42769;
	pJunkcode = 3501475183.48015;
	if (pJunkcode = 1101975666.96059)
		pJunkcode = 6347180015.0039;
	pJunkcode = 9362281490.12994;
	pJunkcode = 5252809652.60597;
	if (pJunkcode = 2059789610.08721)
		pJunkcode = 877334270.405357;
	pJunkcode = 6020467073.83384;
	if (pJunkcode = 1610601654.93441)
		pJunkcode = 2469688578.64533;
	pJunkcode = 9194580646.90574;
	pJunkcode = 2162864758.82695;
	if (pJunkcode = 4751261999.31704)
		pJunkcode = 3917487504.15919;
	pJunkcode = 6808902804.04759;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3743() {
	float pJunkcode = 2587632525.44011;
	pJunkcode = 5953021441.9558;
	if (pJunkcode = 6582803511.08686)
		pJunkcode = 5135422209.72219;
	pJunkcode = 4837043838.90578;
	pJunkcode = 2412412894.5655;
	if (pJunkcode = 8910750399.70567)
		pJunkcode = 9609688096.63525;
	pJunkcode = 7127224198.29897;
	if (pJunkcode = 7200746609.62615)
		pJunkcode = 7092310283.92098;
	pJunkcode = 1833637269.52311;
	pJunkcode = 9763035921.71478;
	if (pJunkcode = 3599895942.85816)
		pJunkcode = 5411387951.81956;
	pJunkcode = 1279790940.07122;
	if (pJunkcode = 2782825666.28929)
		pJunkcode = 3041409508.06168;
	pJunkcode = 4991893611.50808;
	pJunkcode = 1490583454.64811;
	if (pJunkcode = 724028533.700381)
		pJunkcode = 2188676115.17692;
	pJunkcode = 7518347186.03955;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3742() {
	float pJunkcode = 9585853152.33385;
	pJunkcode = 9204015073.75785;
	if (pJunkcode = 7175201722.95479)
		pJunkcode = 6202689605.34746;
	pJunkcode = 4868187049.82315;
	pJunkcode = 1447788152.78628;
	if (pJunkcode = 735197571.082477)
		pJunkcode = 8146145243.60582;
	pJunkcode = 9976417913.2339;
	if (pJunkcode = 5275822685.01487)
		pJunkcode = 1206635729.26498;
	pJunkcode = 8283149468.56085;
	pJunkcode = 2137847808.0323;
	if (pJunkcode = 7246551196.509)
		pJunkcode = 1595547988.86873;
	pJunkcode = 1861604227.24969;
	if (pJunkcode = 102749561.862161)
		pJunkcode = 3169480159.45166;
	pJunkcode = 4507003511.0289;
	pJunkcode = 4953570590.01287;
	if (pJunkcode = 1848929542.70291)
		pJunkcode = 2599395912.97586;
	pJunkcode = 2174188482.68518;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3741() {
	float pJunkcode = 2694705962.66213;
	pJunkcode = 7743791488.7119;
	if (pJunkcode = 4617913716.74602)
		pJunkcode = 4369970881.0194;
	pJunkcode = 841123690.901948;
	pJunkcode = 7301461510.21325;
	if (pJunkcode = 4183575884.48908)
		pJunkcode = 2970249174.9177;
	pJunkcode = 7194824656.35013;
	if (pJunkcode = 437321424.911919)
		pJunkcode = 2617360887.5792;
	pJunkcode = 1205928727.45923;
	pJunkcode = 2155451202.59022;
	if (pJunkcode = 8827709622.26212)
		pJunkcode = 5997040454.51873;
	pJunkcode = 2867317182.99592;
	if (pJunkcode = 58728146.5110181)
		pJunkcode = 2125963214.73216;
	pJunkcode = 5566567728.29115;
	pJunkcode = 3369200721.39738;
	if (pJunkcode = 6257168746.62283)
		pJunkcode = 8780523031.69144;
	pJunkcode = 5989688479.04312;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3740() {
	float pJunkcode = 5272167962.65124;
	pJunkcode = 6324452213.57925;
	if (pJunkcode = 9569169487.82855)
		pJunkcode = 1624310472.05483;
	pJunkcode = 2444011627.09695;
	pJunkcode = 9455508944.08615;
	if (pJunkcode = 3452720873.71591)
		pJunkcode = 546311360.472138;
	pJunkcode = 5721969110.75326;
	if (pJunkcode = 9623926020.7588)
		pJunkcode = 8626043672.64084;
	pJunkcode = 4191272548.31583;
	pJunkcode = 4229784076.92827;
	if (pJunkcode = 2821254906.39024)
		pJunkcode = 7517552033.10534;
	pJunkcode = 9940627081.27392;
	if (pJunkcode = 2264413697.0052)
		pJunkcode = 2962992160.79878;
	pJunkcode = 1588503374.70874;
	pJunkcode = 6746149512.54669;
	if (pJunkcode = 2777343094.96853)
		pJunkcode = 1783682408.4903;
	pJunkcode = 2390671905.68767;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3739() {
	float pJunkcode = 1907678098.70548;
	pJunkcode = 1482387744.28201;
	if (pJunkcode = 1332918515.30442)
		pJunkcode = 7989360844.65271;
	pJunkcode = 2781831580.1859;
	pJunkcode = 3079017045.81562;
	if (pJunkcode = 4737512081.01413)
		pJunkcode = 9896734087.03666;
	pJunkcode = 925137511.425744;
	if (pJunkcode = 913094941.785365)
		pJunkcode = 627590202.654617;
	pJunkcode = 1551056306.88629;
	pJunkcode = 7985586641.78251;
	if (pJunkcode = 3939066260.59518)
		pJunkcode = 2223065304.38066;
	pJunkcode = 1993209587.31988;
	if (pJunkcode = 2398281540.61844)
		pJunkcode = 2406033363.80137;
	pJunkcode = 3703004064.74392;
	pJunkcode = 2066085597.39524;
	if (pJunkcode = 1413079502.17936)
		pJunkcode = 5017502903.14802;
	pJunkcode = 8683552424.84916;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3738() {
	float pJunkcode = 1025876153.51615;
	pJunkcode = 728684585.456288;
	if (pJunkcode = 3168498925.47128)
		pJunkcode = 2271001190.02057;
	pJunkcode = 1736429894.91041;
	pJunkcode = 5889174737.4639;
	if (pJunkcode = 3537383703.16868)
		pJunkcode = 8474424355.66114;
	pJunkcode = 6680792592.79855;
	if (pJunkcode = 9193498647.15876)
		pJunkcode = 9684507948.40614;
	pJunkcode = 7308340646.30468;
	pJunkcode = 4694985430.43221;
	if (pJunkcode = 3420757969.7599)
		pJunkcode = 5001000837.74382;
	pJunkcode = 1195856921.4525;
	if (pJunkcode = 8862003675.48794)
		pJunkcode = 8725632012.46969;
	pJunkcode = 1539091154.98904;
	pJunkcode = 7280534628.82278;
	if (pJunkcode = 7267410711.35471)
		pJunkcode = 6632093125.74399;
	pJunkcode = 6298523944.81903;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3737() {
	float pJunkcode = 2624270216.84656;
	pJunkcode = 3903512478.12788;
	if (pJunkcode = 2314548715.93184)
		pJunkcode = 6499187556.90142;
	pJunkcode = 3538353912.72833;
	pJunkcode = 6165645537.72772;
	if (pJunkcode = 7547288243.88694)
		pJunkcode = 6920392144.58797;
	pJunkcode = 2615502668.49661;
	if (pJunkcode = 5335279760.8638)
		pJunkcode = 7410695596.52976;
	pJunkcode = 348544453.935452;
	pJunkcode = 3121083508.90401;
	if (pJunkcode = 3175249388.99425)
		pJunkcode = 5512413687.52074;
	pJunkcode = 9638347991.00905;
	if (pJunkcode = 5136081411.8748)
		pJunkcode = 6694852638.41748;
	pJunkcode = 8780252643.51229;
	pJunkcode = 7483114681.03629;
	if (pJunkcode = 8171892325.37106)
		pJunkcode = 3352155069.39711;
	pJunkcode = 7951621558.66826;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3736() {
	float pJunkcode = 1513326138.45867;
	pJunkcode = 2809240227.25375;
	if (pJunkcode = 8038698042.43264)
		pJunkcode = 3740419121.0013;
	pJunkcode = 3345934164.56421;
	pJunkcode = 3963323241.14234;
	if (pJunkcode = 651710927.415088)
		pJunkcode = 7336673952.40058;
	pJunkcode = 8978996064.59941;
	if (pJunkcode = 5120143822.42312)
		pJunkcode = 8761906687.28002;
	pJunkcode = 4766761973.98038;
	pJunkcode = 1952075685.45256;
	if (pJunkcode = 9615771323.10044)
		pJunkcode = 202705383.934216;
	pJunkcode = 1440301651.43836;
	if (pJunkcode = 8086442645.64181)
		pJunkcode = 8490085077.32166;
	pJunkcode = 1295069602.1077;
	pJunkcode = 5877090500.86821;
	if (pJunkcode = 6620575427.9597)
		pJunkcode = 726330879.779296;
	pJunkcode = 692506045.457351;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3735() {
	float pJunkcode = 5141930669.16593;
	pJunkcode = 985433660.942292;
	if (pJunkcode = 5839694005.68413)
		pJunkcode = 4656801465.64833;
	pJunkcode = 7093850692.35315;
	pJunkcode = 2176795854.81172;
	if (pJunkcode = 7585916514.94496)
		pJunkcode = 4035623210.68275;
	pJunkcode = 9293544867.15252;
	if (pJunkcode = 2259960434.05537)
		pJunkcode = 7516804477.26363;
	pJunkcode = 5934826274.65367;
	pJunkcode = 2067000973.55835;
	if (pJunkcode = 484269564.363465)
		pJunkcode = 9790473858.5847;
	pJunkcode = 9729661654.42037;
	if (pJunkcode = 7969195572.62106)
		pJunkcode = 5507020880.82868;
	pJunkcode = 1089492686.2638;
	pJunkcode = 5895807203.59666;
	if (pJunkcode = 3963160270.63475)
		pJunkcode = 1731837195.73911;
	pJunkcode = 929210980.171046;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3734() {
	float pJunkcode = 547120043.059754;
	pJunkcode = 8913023709.76538;
	if (pJunkcode = 7110768989.59322)
		pJunkcode = 7566547031.46883;
	pJunkcode = 7040435024.6757;
	pJunkcode = 9366206048.86961;
	if (pJunkcode = 4008596169.22372)
		pJunkcode = 3899302015.53013;
	pJunkcode = 1083321015.30646;
	if (pJunkcode = 6314038534.11571)
		pJunkcode = 8525543302.39178;
	pJunkcode = 8127501376.55925;
	pJunkcode = 2552043443.87193;
	if (pJunkcode = 8755184700.9883)
		pJunkcode = 9649811502.65952;
	pJunkcode = 5449254979.05921;
	if (pJunkcode = 4308208949.20231)
		pJunkcode = 9157797132.79711;
	pJunkcode = 9925473599.38117;
	pJunkcode = 9820930349.37087;
	if (pJunkcode = 8095396491.56827)
		pJunkcode = 7371621731.65703;
	pJunkcode = 1945807656.16864;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3733() {
	float pJunkcode = 323772990.331632;
	pJunkcode = 6508240997.46804;
	if (pJunkcode = 1440946266.89701)
		pJunkcode = 6177203221.44689;
	pJunkcode = 7518954583.892;
	pJunkcode = 8903076203.81135;
	if (pJunkcode = 814297065.728461)
		pJunkcode = 7540714415.56539;
	pJunkcode = 8820036217.98408;
	if (pJunkcode = 5275329361.39551)
		pJunkcode = 8961266549.27136;
	pJunkcode = 4305015259.49899;
	pJunkcode = 2416749808.54959;
	if (pJunkcode = 9033523571.30072)
		pJunkcode = 4526109321.09448;
	pJunkcode = 4946170214.18706;
	if (pJunkcode = 8290798195.36576)
		pJunkcode = 1746907450.82226;
	pJunkcode = 5847294553.32575;
	pJunkcode = 2923019220.07329;
	if (pJunkcode = 2134235255.53315)
		pJunkcode = 2804599456.02018;
	pJunkcode = 3962161185.70743;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3732() {
	float pJunkcode = 9916813787.67757;
	pJunkcode = 4826376993.57358;
	if (pJunkcode = 5053221453.55751)
		pJunkcode = 9990277856.8855;
	pJunkcode = 2203735913.94043;
	pJunkcode = 4754059978.6722;
	if (pJunkcode = 6955991188.87976)
		pJunkcode = 9226635040.74826;
	pJunkcode = 9525250984.92106;
	if (pJunkcode = 4959362068.62926)
		pJunkcode = 8979694996.57574;
	pJunkcode = 2922191026.47285;
	pJunkcode = 9622190269.37008;
	if (pJunkcode = 3006238334.69324)
		pJunkcode = 9456502736.82586;
	pJunkcode = 4809108217.54216;
	if (pJunkcode = 3496554627.11662)
		pJunkcode = 5742915314.56561;
	pJunkcode = 5143421657.45989;
	pJunkcode = 2001629439.17686;
	if (pJunkcode = 3337440124.29284)
		pJunkcode = 7134982522.04847;
	pJunkcode = 558795409.850902;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3731() {
	float pJunkcode = 5381872520.48797;
	pJunkcode = 1595057175.13861;
	if (pJunkcode = 9690536264.64145)
		pJunkcode = 2273967269.09112;
	pJunkcode = 1014215257.0048;
	pJunkcode = 1857271601.96317;
	if (pJunkcode = 3287217843.64189)
		pJunkcode = 500411304.666874;
	pJunkcode = 7455454963.55461;
	if (pJunkcode = 2765983974.22152)
		pJunkcode = 6662475071.88844;
	pJunkcode = 292403752.014031;
	pJunkcode = 8585265134.30291;
	if (pJunkcode = 8216979465.76994)
		pJunkcode = 6327392499.92902;
	pJunkcode = 6168378809.43749;
	if (pJunkcode = 7380995108.59658)
		pJunkcode = 416220646.381023;
	pJunkcode = 6066103840.39528;
	pJunkcode = 1195162457.45153;
	if (pJunkcode = 5094377648.73602)
		pJunkcode = 6050963664.64075;
	pJunkcode = 6533115305.9953;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3730() {
	float pJunkcode = 5788838720.72262;
	pJunkcode = 9280443073.03098;
	if (pJunkcode = 5616659822.98958)
		pJunkcode = 1649137770.21694;
	pJunkcode = 3559802329.54512;
	pJunkcode = 6245269990.37626;
	if (pJunkcode = 2652610489.16565)
		pJunkcode = 555438845.099598;
	pJunkcode = 1695369734.42712;
	if (pJunkcode = 4608744807.30469)
		pJunkcode = 5339212473.25108;
	pJunkcode = 2752835931.08342;
	pJunkcode = 3425726565.58973;
	if (pJunkcode = 8795561671.01765)
		pJunkcode = 594251269.385338;
	pJunkcode = 3992377276.60146;
	if (pJunkcode = 1971591024.22682)
		pJunkcode = 6286117562.30969;
	pJunkcode = 9059089961.99529;
	pJunkcode = 2310348372.05267;
	if (pJunkcode = 8371419781.84034)
		pJunkcode = 3762114757.25158;
	pJunkcode = 2918946400.91899;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3729() {
	float pJunkcode = 309070494.557381;
	pJunkcode = 8166793736.83622;
	if (pJunkcode = 5877659535.59915)
		pJunkcode = 5773185300.55807;
	pJunkcode = 6676239562.91561;
	pJunkcode = 8585597915.81401;
	if (pJunkcode = 5762414869.90838)
		pJunkcode = 4302324506.89617;
	pJunkcode = 3411753516.27082;
	if (pJunkcode = 1372704201.52903)
		pJunkcode = 694538857.83703;
	pJunkcode = 9607998843.38938;
	pJunkcode = 5409110227.33499;
	if (pJunkcode = 6863437588.96205)
		pJunkcode = 3979026628.59164;
	pJunkcode = 6733126724.05738;
	if (pJunkcode = 6500002709.78792)
		pJunkcode = 1305686533.70567;
	pJunkcode = 5550734988.41471;
	pJunkcode = 4341039527.4562;
	if (pJunkcode = 7145795257.60306)
		pJunkcode = 4573768373.86747;
	pJunkcode = 1960487295.18717;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3728() {
	float pJunkcode = 5064156468.21502;
	pJunkcode = 8721908109.82873;
	if (pJunkcode = 9606019432.5121)
		pJunkcode = 3715889028.18369;
	pJunkcode = 2222396654.05502;
	pJunkcode = 9741197584.99004;
	if (pJunkcode = 5235224847.06075)
		pJunkcode = 6529940810.6583;
	pJunkcode = 771437328.630362;
	if (pJunkcode = 8591137439.26898)
		pJunkcode = 689316353.416206;
	pJunkcode = 211288806.995841;
	pJunkcode = 555326647.462726;
	if (pJunkcode = 7705125718.32542)
		pJunkcode = 5864173418.76234;
	pJunkcode = 5919661926.66344;
	if (pJunkcode = 3245161354.91853)
		pJunkcode = 4824100402.9629;
	pJunkcode = 560814380.587235;
	pJunkcode = 2186728389.90508;
	if (pJunkcode = 6257647700.07857)
		pJunkcode = 6177336739.17259;
	pJunkcode = 4633826847.58317;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3727() {
	float pJunkcode = 502482321.844815;
	pJunkcode = 5870750529.96314;
	if (pJunkcode = 379968541.947577)
		pJunkcode = 1451073080.92152;
	pJunkcode = 9298548303.41146;
	pJunkcode = 4668631483.99137;
	if (pJunkcode = 6243907385.94366)
		pJunkcode = 8792857221.12504;
	pJunkcode = 7730464384.30133;
	if (pJunkcode = 7489919973.29924)
		pJunkcode = 7772490270.07093;
	pJunkcode = 5439879890.83154;
	pJunkcode = 119215888.119964;
	if (pJunkcode = 5177912576.33945)
		pJunkcode = 4028637774.49567;
	pJunkcode = 3796491962.78193;
	if (pJunkcode = 714478321.327053)
		pJunkcode = 8239994758.94178;
	pJunkcode = 7237790474.68526;
	pJunkcode = 2590120631.26212;
	if (pJunkcode = 9793935722.2699)
		pJunkcode = 1975777966.50816;
	pJunkcode = 6421971552.52337;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3726() {
	float pJunkcode = 3211361926.82235;
	pJunkcode = 3873423327.55965;
	if (pJunkcode = 6719756099.37811)
		pJunkcode = 8062083756.14103;
	pJunkcode = 148610402.394276;
	pJunkcode = 9212335594.07067;
	if (pJunkcode = 8824900713.94584)
		pJunkcode = 68248773.3903178;
	pJunkcode = 1016768016.92169;
	if (pJunkcode = 3473500339.85853)
		pJunkcode = 3428684814.64403;
	pJunkcode = 8162487011.37149;
	pJunkcode = 9534355407.90388;
	if (pJunkcode = 9449814221.93275)
		pJunkcode = 5033348402.01615;
	pJunkcode = 4257044053.80365;
	if (pJunkcode = 1459090621.7476)
		pJunkcode = 2364169119.08969;
	pJunkcode = 6177879387.47055;
	pJunkcode = 9190634653.99136;
	if (pJunkcode = 4214338616.68668)
		pJunkcode = 1434501565.15914;
	pJunkcode = 6373314830.41921;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3725() {
	float pJunkcode = 5178326037.51836;
	pJunkcode = 4180047259.61276;
	if (pJunkcode = 6722476612.09619)
		pJunkcode = 9041155695.11847;
	pJunkcode = 8463126201.0085;
	pJunkcode = 932537065.367591;
	if (pJunkcode = 160889870.18893)
		pJunkcode = 2198507102.20055;
	pJunkcode = 4076474389.12678;
	if (pJunkcode = 6242970955.11599)
		pJunkcode = 1589942228.08633;
	pJunkcode = 9127634140.72867;
	pJunkcode = 173732968.769321;
	if (pJunkcode = 1925903529.03203)
		pJunkcode = 3458203624.31448;
	pJunkcode = 601284116.402266;
	if (pJunkcode = 4725307830.73502)
		pJunkcode = 3003068060.53359;
	pJunkcode = 3575190632.76357;
	pJunkcode = 841910473.658224;
	if (pJunkcode = 8445630948.2182)
		pJunkcode = 5952147604.69242;
	pJunkcode = 6166568790.47274;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3724() {
	float pJunkcode = 5861522328.46304;
	pJunkcode = 1913318187.69469;
	if (pJunkcode = 8272962223.94709)
		pJunkcode = 6184406105.31078;
	pJunkcode = 9795222298.94441;
	pJunkcode = 3446820333.66005;
	if (pJunkcode = 9040609461.26445)
		pJunkcode = 3481112964.30486;
	pJunkcode = 749286811.043863;
	if (pJunkcode = 8431170477.47773)
		pJunkcode = 6928117534.77226;
	pJunkcode = 6154796634.62263;
	pJunkcode = 8878138174.83074;
	if (pJunkcode = 6354917009.35143)
		pJunkcode = 9458490427.07803;
	pJunkcode = 6715256803.94206;
	if (pJunkcode = 3845599067.97925)
		pJunkcode = 2477491172.3707;
	pJunkcode = 6024955566.03605;
	pJunkcode = 7219715691.26357;
	if (pJunkcode = 4587567197.75802)
		pJunkcode = 4584080577.43806;
	pJunkcode = 9843606355.80849;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3723() {
	float pJunkcode = 7781540316.00071;
	pJunkcode = 8475944233.29246;
	if (pJunkcode = 4229680666.23201)
		pJunkcode = 7783492984.21769;
	pJunkcode = 4501278521.1945;
	pJunkcode = 5207743867.22017;
	if (pJunkcode = 8177475278.29169)
		pJunkcode = 9175259195.91106;
	pJunkcode = 5647511516.55818;
	if (pJunkcode = 11934144.0607834)
		pJunkcode = 366693430.428102;
	pJunkcode = 715220917.925647;
	pJunkcode = 52396161.6171614;
	if (pJunkcode = 6609075511.27384)
		pJunkcode = 3011034174.24138;
	pJunkcode = 4313518461.56458;
	if (pJunkcode = 9433160089.37096)
		pJunkcode = 1352906113.72035;
	pJunkcode = 2541838639.34663;
	pJunkcode = 9029952452.92857;
	if (pJunkcode = 5380042254.77292)
		pJunkcode = 6991802439.88299;
	pJunkcode = 7861209581.11511;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3722() {
	float pJunkcode = 6479831710.17972;
	pJunkcode = 6460626305.47397;
	if (pJunkcode = 9706032502.41033)
		pJunkcode = 1701624599.59809;
	pJunkcode = 8181523210.42737;
	pJunkcode = 6062464785.049;
	if (pJunkcode = 6075578949.33894)
		pJunkcode = 187700248.602574;
	pJunkcode = 1507921348.09321;
	if (pJunkcode = 9239374262.97577)
		pJunkcode = 5542263576.7273;
	pJunkcode = 1321446938.48111;
	pJunkcode = 4063153820.20702;
	if (pJunkcode = 8532323197.74209)
		pJunkcode = 4658580108.6693;
	pJunkcode = 9883721433.56392;
	if (pJunkcode = 2944138908.91996)
		pJunkcode = 8362624878.49162;
	pJunkcode = 1963273528.35383;
	pJunkcode = 2143498006.42696;
	if (pJunkcode = 1287563280.0853)
		pJunkcode = 7168099493.0999;
	pJunkcode = 6585361318.72163;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3721() {
	float pJunkcode = 7634305477.50164;
	pJunkcode = 8610659541.7058;
	if (pJunkcode = 5841807150.81745)
		pJunkcode = 4941622632.4867;
	pJunkcode = 5755743342.16915;
	pJunkcode = 2906996073.20057;
	if (pJunkcode = 6069017486.52921)
		pJunkcode = 8517540087.49194;
	pJunkcode = 1147441274.4426;
	if (pJunkcode = 7427990598.34262)
		pJunkcode = 2994113455.58683;
	pJunkcode = 1791281892.10652;
	pJunkcode = 6336412017.4101;
	if (pJunkcode = 7052365715.654)
		pJunkcode = 8317399009.66934;
	pJunkcode = 7873227722.40864;
	if (pJunkcode = 9267319404.6892)
		pJunkcode = 2320575199.80831;
	pJunkcode = 9819697970.47159;
	pJunkcode = 5956624758.15568;
	if (pJunkcode = 3909807040.52662)
		pJunkcode = 6195878320.08368;
	pJunkcode = 4557993142.58492;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3720() {
	float pJunkcode = 9021576689.44138;
	pJunkcode = 7674435444.62783;
	if (pJunkcode = 8577294468.80156)
		pJunkcode = 1458667498.1936;
	pJunkcode = 9920691731.86023;
	pJunkcode = 4203577738.98277;
	if (pJunkcode = 4308060262.71186)
		pJunkcode = 6847719738.2178;
	pJunkcode = 6485546578.96724;
	if (pJunkcode = 5442947145.4465)
		pJunkcode = 1784100158.36679;
	pJunkcode = 9539786016.60123;
	pJunkcode = 3985509611.96112;
	if (pJunkcode = 6197422388.57696)
		pJunkcode = 8335837547.75291;
	pJunkcode = 5750146824.67087;
	if (pJunkcode = 6819394928.02713)
		pJunkcode = 9904337503.12313;
	pJunkcode = 1478522808.21733;
	pJunkcode = 9575073040.68281;
	if (pJunkcode = 2588344721.64557)
		pJunkcode = 8280621402.41508;
	pJunkcode = 2838025890.82692;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3719() {
	float pJunkcode = 6233059854.19735;
	pJunkcode = 1855557517.57747;
	if (pJunkcode = 6963270102.83002)
		pJunkcode = 8344022319.84476;
	pJunkcode = 2399770577.81111;
	pJunkcode = 2861411227.35344;
	if (pJunkcode = 9403866748.31278)
		pJunkcode = 1943030193.64462;
	pJunkcode = 4851801382.27863;
	if (pJunkcode = 9967785372.71087)
		pJunkcode = 3969258100.09189;
	pJunkcode = 2255470081.55492;
	pJunkcode = 4729357127.24463;
	if (pJunkcode = 442821283.735091)
		pJunkcode = 4524165479.39894;
	pJunkcode = 2784496172.19921;
	if (pJunkcode = 9535231138.06146)
		pJunkcode = 3278362414.24433;
	pJunkcode = 3067597138.25204;
	pJunkcode = 7486957307.4918;
	if (pJunkcode = 6680645571.03056)
		pJunkcode = 4505531270.07089;
	pJunkcode = 2783231646.69444;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3718() {
	float pJunkcode = 5429267899.85042;
	pJunkcode = 4795316522.85462;
	if (pJunkcode = 7635768351.77652)
		pJunkcode = 3763230021.17996;
	pJunkcode = 9545177668.46485;
	pJunkcode = 1052818145.11988;
	if (pJunkcode = 6262278725.11886)
		pJunkcode = 4072075344.90674;
	pJunkcode = 9713990651.06145;
	if (pJunkcode = 9239516355.76833)
		pJunkcode = 9782132874.99126;
	pJunkcode = 6980556166.36385;
	pJunkcode = 5630060419.65788;
	if (pJunkcode = 8542763016.84059)
		pJunkcode = 9926118368.50652;
	pJunkcode = 6162816168.45909;
	if (pJunkcode = 8485881073.38137)
		pJunkcode = 8562209701.52951;
	pJunkcode = 2426997318.44448;
	pJunkcode = 7266817129.53449;
	if (pJunkcode = 8983181342.15829)
		pJunkcode = 8048470063.35713;
	pJunkcode = 1314299912.45834;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3717() {
	float pJunkcode = 1796264967.52928;
	pJunkcode = 2108222460.78133;
	if (pJunkcode = 8362117568.87536)
		pJunkcode = 4534625256.54063;
	pJunkcode = 9164136139.79296;
	pJunkcode = 5285482384.90348;
	if (pJunkcode = 1389816386.54869)
		pJunkcode = 6143682796.17753;
	pJunkcode = 3051241552.12586;
	if (pJunkcode = 9725122740.91819)
		pJunkcode = 5147390451.25119;
	pJunkcode = 5102866417.77434;
	pJunkcode = 7446699643.34778;
	if (pJunkcode = 5315194076.53519)
		pJunkcode = 3143898728.04793;
	pJunkcode = 9537623793.58733;
	if (pJunkcode = 1317265705.15006)
		pJunkcode = 3270759645.42058;
	pJunkcode = 9129140232.59544;
	pJunkcode = 5977259379.47505;
	if (pJunkcode = 9741057273.33409)
		pJunkcode = 5311565111.96809;
	pJunkcode = 4846129235.15868;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3716() {
	float pJunkcode = 1850524770.56192;
	pJunkcode = 9748678891.93824;
	if (pJunkcode = 1093715200.05516)
		pJunkcode = 6941598948.4771;
	pJunkcode = 7269316667.22679;
	pJunkcode = 501316077.828507;
	if (pJunkcode = 4132800271.04879)
		pJunkcode = 8281334492.68127;
	pJunkcode = 846553019.043558;
	if (pJunkcode = 6222847145.15075)
		pJunkcode = 4130482833.03848;
	pJunkcode = 1614785002.34657;
	pJunkcode = 8487967203.44526;
	if (pJunkcode = 1921861280.23553)
		pJunkcode = 9040843653.9537;
	pJunkcode = 5096520595.8937;
	if (pJunkcode = 2834582406.60894)
		pJunkcode = 9545280653.08529;
	pJunkcode = 7146626559.4615;
	pJunkcode = 7334598134.30277;
	if (pJunkcode = 6554628027.6645)
		pJunkcode = 3188567434.05507;
	pJunkcode = 7529765636.09787;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3715() {
	float pJunkcode = 2027140963.87892;
	pJunkcode = 6237167788.51817;
	if (pJunkcode = 350732530.205488)
		pJunkcode = 4565330610.5282;
	pJunkcode = 9219233787.15971;
	pJunkcode = 5782310820.11455;
	if (pJunkcode = 1111082939.762)
		pJunkcode = 1190187496.09556;
	pJunkcode = 8041932272.20222;
	if (pJunkcode = 6777509940.64084)
		pJunkcode = 8536469723.35617;
	pJunkcode = 8094406321.63714;
	pJunkcode = 9735528525.17355;
	if (pJunkcode = 812675865.870877)
		pJunkcode = 4165351939.60718;
	pJunkcode = 4987199.05079364;
	if (pJunkcode = 1542777041.42267)
		pJunkcode = 3823948961.30603;
	pJunkcode = 8831887586.68363;
	pJunkcode = 4169287259.67159;
	if (pJunkcode = 3086535421.35991)
		pJunkcode = 7403894851.40381;
	pJunkcode = 6298808824.23252;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3714() {
	float pJunkcode = 3680683354.31548;
	pJunkcode = 1077236892.45225;
	if (pJunkcode = 401184193.312687)
		pJunkcode = 5416477203.93896;
	pJunkcode = 7705161697.28499;
	pJunkcode = 5276372602.21283;
	if (pJunkcode = 8381931710.19638)
		pJunkcode = 8777254501.65444;
	pJunkcode = 9654902801.33981;
	if (pJunkcode = 6437834858.10104)
		pJunkcode = 6679144440.73519;
	pJunkcode = 6846522630.48001;
	pJunkcode = 9898831839.89005;
	if (pJunkcode = 4202074077.84361)
		pJunkcode = 3138729196.7254;
	pJunkcode = 6379257111.93544;
	if (pJunkcode = 4449298582.33865)
		pJunkcode = 9208231713.29082;
	pJunkcode = 6020552142.31541;
	pJunkcode = 7348908896.43649;
	if (pJunkcode = 70143211.2456338)
		pJunkcode = 3344790716.76825;
	pJunkcode = 526668173.341648;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3713() {
	float pJunkcode = 985181798.91219;
	pJunkcode = 556434605.238576;
	if (pJunkcode = 3718633782.98597)
		pJunkcode = 1856282062.99678;
	pJunkcode = 9299487809.90117;
	pJunkcode = 4403537240.98709;
	if (pJunkcode = 6410495444.86104)
		pJunkcode = 3456724102.81505;
	pJunkcode = 8362583260.43026;
	if (pJunkcode = 7857126128.69164)
		pJunkcode = 8036050775.17427;
	pJunkcode = 4855359168.90145;
	pJunkcode = 634976226.943061;
	if (pJunkcode = 6680438402.72387)
		pJunkcode = 4166470002.13192;
	pJunkcode = 276058615.222637;
	if (pJunkcode = 6944242034.13892)
		pJunkcode = 7606040599.64762;
	pJunkcode = 3109023991.69056;
	pJunkcode = 9429215330.44665;
	if (pJunkcode = 2081929652.7278)
		pJunkcode = 5543994045.24155;
	pJunkcode = 9541818458.78389;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3712() {
	float pJunkcode = 2583362170.08652;
	pJunkcode = 3992460910.86061;
	if (pJunkcode = 2748223010.77293)
		pJunkcode = 7888920460.2908;
	pJunkcode = 9648103371.69384;
	pJunkcode = 7403053316.74091;
	if (pJunkcode = 8072909173.85396)
		pJunkcode = 1698916638.65448;
	pJunkcode = 4031241707.74799;
	if (pJunkcode = 3078831907.72412)
		pJunkcode = 6803502733.11209;
	pJunkcode = 9058548098.32875;
	pJunkcode = 9064933952.70993;
	if (pJunkcode = 5947403075.84841)
		pJunkcode = 8289961666.51163;
	pJunkcode = 6433598807.67143;
	if (pJunkcode = 2451295400.17415)
		pJunkcode = 3772429547.54475;
	pJunkcode = 6640927111.65372;
	pJunkcode = 9778747798.08446;
	if (pJunkcode = 7385961129.34525)
		pJunkcode = 7125678229.92663;
	pJunkcode = 1578969998.15089;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3711() {
	float pJunkcode = 6589777794.79763;
	pJunkcode = 1009196197.9659;
	if (pJunkcode = 2608198669.62366)
		pJunkcode = 8236535603.0499;
	pJunkcode = 9958369388.18961;
	pJunkcode = 9033956045.23987;
	if (pJunkcode = 6213366974.51952)
		pJunkcode = 7562874041.39558;
	pJunkcode = 4670023182.65025;
	if (pJunkcode = 3267939619.55224)
		pJunkcode = 7518398652.73866;
	pJunkcode = 2162778621.66342;
	pJunkcode = 8933184986.46313;
	if (pJunkcode = 3021310005.37255)
		pJunkcode = 1525000543.66888;
	pJunkcode = 1383034160.58694;
	if (pJunkcode = 2134519178.02227)
		pJunkcode = 3079792333.83796;
	pJunkcode = 2638857951.54419;
	pJunkcode = 9435440666.00239;
	if (pJunkcode = 608911613.921382)
		pJunkcode = 2674455806.08329;
	pJunkcode = 8819759964.85966;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3710() {
	float pJunkcode = 6360927877.43279;
	pJunkcode = 6989525467.43388;
	if (pJunkcode = 9269647876.43338)
		pJunkcode = 4766575019.82802;
	pJunkcode = 4184573137.05756;
	pJunkcode = 6215613243.89869;
	if (pJunkcode = 2346235902.26856)
		pJunkcode = 4773831444.8913;
	pJunkcode = 2133177630.58251;
	if (pJunkcode = 3836001000.07399)
		pJunkcode = 2877798052.0547;
	pJunkcode = 230574313.955198;
	pJunkcode = 5027552322.71224;
	if (pJunkcode = 6776545377.16783)
		pJunkcode = 4202487646.12355;
	pJunkcode = 2799424518.42418;
	if (pJunkcode = 4861119259.93266)
		pJunkcode = 7300297606.12151;
	pJunkcode = 403180146.641768;
	pJunkcode = 7418544564.99898;
	if (pJunkcode = 6402799838.07484)
		pJunkcode = 6709947608.22631;
	pJunkcode = 1269424587.61933;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3709() {
	float pJunkcode = 3738237494.07559;
	pJunkcode = 1596251591.38107;
	if (pJunkcode = 8139005698.35683)
		pJunkcode = 5093524316.66238;
	pJunkcode = 7441745508.48461;
	pJunkcode = 5358407335.49071;
	if (pJunkcode = 9071847410.02332)
		pJunkcode = 4293686869.76132;
	pJunkcode = 7601953699.0368;
	if (pJunkcode = 2995532118.09291)
		pJunkcode = 1656687400.63428;
	pJunkcode = 8901791329.11181;
	pJunkcode = 1555618961.89179;
	if (pJunkcode = 8255974632.34381)
		pJunkcode = 2374836516.225;
	pJunkcode = 6961556311.808;
	if (pJunkcode = 8847245715.35241)
		pJunkcode = 4950207934.47283;
	pJunkcode = 2554661088.38374;
	pJunkcode = 5176856408.89406;
	if (pJunkcode = 5258158766.73638)
		pJunkcode = 4006608811.91604;
	pJunkcode = 9325861260.37;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3708() {
	float pJunkcode = 2928317216.74707;
	pJunkcode = 6021616249.20635;
	if (pJunkcode = 4633412136.1162)
		pJunkcode = 1316601445.96644;
	pJunkcode = 7453272721.96215;
	pJunkcode = 7517453439.62693;
	if (pJunkcode = 759517454.855593)
		pJunkcode = 2844745080.85058;
	pJunkcode = 5155285964.42892;
	if (pJunkcode = 4206128687.49537)
		pJunkcode = 3007885851.1353;
	pJunkcode = 2192296688.61154;
	pJunkcode = 2968033031.19425;
	if (pJunkcode = 2722297820.00303)
		pJunkcode = 58530181.9076306;
	pJunkcode = 8033067171.11644;
	if (pJunkcode = 5604040805.3647)
		pJunkcode = 3410409349.51251;
	pJunkcode = 5172618754.27735;
	pJunkcode = 1974431802.08291;
	if (pJunkcode = 8397069760.22839)
		pJunkcode = 2069851031.95108;
	pJunkcode = 8390803307.19216;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3707() {
	float pJunkcode = 8496220881.07702;
	pJunkcode = 6846294488.99868;
	if (pJunkcode = 4977696143.6326)
		pJunkcode = 1779849968.27359;
	pJunkcode = 3184752653.86607;
	pJunkcode = 6379571500.00963;
	if (pJunkcode = 2478737306.76685)
		pJunkcode = 7136983795.50455;
	pJunkcode = 4145711083.08813;
	if (pJunkcode = 4035442752.44039)
		pJunkcode = 533276839.76402;
	pJunkcode = 2551025884.01778;
	pJunkcode = 1496303084.77468;
	if (pJunkcode = 522980279.927939)
		pJunkcode = 7506243491.98155;
	pJunkcode = 4879508093.82747;
	if (pJunkcode = 3441828450.34212)
		pJunkcode = 9117016410.52958;
	pJunkcode = 7447444761.47703;
	pJunkcode = 3592117553.96445;
	if (pJunkcode = 6740073398.98652)
		pJunkcode = 3599068964.62153;
	pJunkcode = 6730817891.3816;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3706() {
	float pJunkcode = 2453982809.36236;
	pJunkcode = 5374531881.42303;
	if (pJunkcode = 5023615589.37996)
		pJunkcode = 7052556949.19079;
	pJunkcode = 7765229805.43637;
	pJunkcode = 2144823972.99587;
	if (pJunkcode = 2270265242.99867)
		pJunkcode = 616963920.671506;
	pJunkcode = 9957707720.38514;
	if (pJunkcode = 8084452873.90451)
		pJunkcode = 6425774784.42792;
	pJunkcode = 4501327159.02958;
	pJunkcode = 9757267236.99046;
	if (pJunkcode = 1366562125.37885)
		pJunkcode = 4427503505.11489;
	pJunkcode = 7927092019.98204;
	if (pJunkcode = 1302933401.74061)
		pJunkcode = 4489141087.98533;
	pJunkcode = 3439742827.84396;
	pJunkcode = 5636195976.61523;
	if (pJunkcode = 2935053254.22763)
		pJunkcode = 4254351412.96319;
	pJunkcode = 8201055613.18886;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3705() {
	float pJunkcode = 6278462870.44334;
	pJunkcode = 8837636855.85474;
	if (pJunkcode = 8948954033.82563)
		pJunkcode = 6745613350.84559;
	pJunkcode = 8222049192.90681;
	pJunkcode = 1562109025.57033;
	if (pJunkcode = 6743512238.27961)
		pJunkcode = 6198185639.37541;
	pJunkcode = 2441695199.56152;
	if (pJunkcode = 1540110151.39389)
		pJunkcode = 2105277974.20875;
	pJunkcode = 5720371962.06065;
	pJunkcode = 9639831149.28676;
	if (pJunkcode = 3229902860.47072)
		pJunkcode = 8304502903.0664;
	pJunkcode = 4435454582.77008;
	if (pJunkcode = 4655997390.73307)
		pJunkcode = 9315276468.39268;
	pJunkcode = 1171913586.25719;
	pJunkcode = 5662728127.52034;
	if (pJunkcode = 7949013224.96078)
		pJunkcode = 8217281688.56465;
	pJunkcode = 269658921.526401;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3704() {
	float pJunkcode = 6158988978.52089;
	pJunkcode = 7844447029.72;
	if (pJunkcode = 526957350.765847)
		pJunkcode = 1460659356.07261;
	pJunkcode = 6708362433.89395;
	pJunkcode = 7219305112.4992;
	if (pJunkcode = 7712445495.9733)
		pJunkcode = 4101927478.57826;
	pJunkcode = 4064315756.89949;
	if (pJunkcode = 320271620.337895)
		pJunkcode = 6081351268.51327;
	pJunkcode = 6712206571.40523;
	pJunkcode = 9954490778.11797;
	if (pJunkcode = 7170507051.96622)
		pJunkcode = 543259668.101769;
	pJunkcode = 2891392017.29544;
	if (pJunkcode = 7975875526.41702)
		pJunkcode = 9075661154.12631;
	pJunkcode = 4507863988.60354;
	pJunkcode = 1707556208.26435;
	if (pJunkcode = 2395238933.20893)
		pJunkcode = 4620490946.91885;
	pJunkcode = 9794660751.9721;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3703() {
	float pJunkcode = 7767449869.98865;
	pJunkcode = 3138153121.26621;
	if (pJunkcode = 1668496699.58554)
		pJunkcode = 926406185.931095;
	pJunkcode = 2269052973.48098;
	pJunkcode = 6055458238.31857;
	if (pJunkcode = 3624873287.22809)
		pJunkcode = 3594053505.10839;
	pJunkcode = 5317322125.00897;
	if (pJunkcode = 4534657137.72185)
		pJunkcode = 3888017167.01571;
	pJunkcode = 8533591672.01509;
	pJunkcode = 1323121106.78043;
	if (pJunkcode = 1318684761.13599)
		pJunkcode = 6670698472.52608;
	pJunkcode = 5345163180.82404;
	if (pJunkcode = 925838073.200289)
		pJunkcode = 4115325732.99371;
	pJunkcode = 9509766381.5831;
	pJunkcode = 2432509689.46132;
	if (pJunkcode = 9565160803.56198)
		pJunkcode = 8141659026.13625;
	pJunkcode = 6531179271.03317;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3702() {
	float pJunkcode = 8948973873.12638;
	pJunkcode = 3551161414.18759;
	if (pJunkcode = 5178845619.51513)
		pJunkcode = 9949367793.83225;
	pJunkcode = 3165703726.0862;
	pJunkcode = 6373404010.64883;
	if (pJunkcode = 9284864473.42585)
		pJunkcode = 5380244643.1143;
	pJunkcode = 8111730790.20159;
	if (pJunkcode = 3631083356.23119)
		pJunkcode = 368530636.755228;
	pJunkcode = 5560060696.08498;
	pJunkcode = 8651828845.11847;
	if (pJunkcode = 7456278386.50287)
		pJunkcode = 2449107307.60613;
	pJunkcode = 1836389619.18503;
	if (pJunkcode = 4389898239.76531)
		pJunkcode = 2420932261.90101;
	pJunkcode = 7722792095.27092;
	pJunkcode = 1715672656.88706;
	if (pJunkcode = 2483316550.78981)
		pJunkcode = 316661477.611804;
	pJunkcode = 3051763678.9361;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3701() {
	float pJunkcode = 4429130467.47328;
	pJunkcode = 3563388453.7022;
	if (pJunkcode = 8436077278.34439)
		pJunkcode = 7409060960.1501;
	pJunkcode = 1169207334.27342;
	pJunkcode = 1143150763.14175;
	if (pJunkcode = 1410561103.06524)
		pJunkcode = 1442812742.07166;
	pJunkcode = 6115305373.77245;
	if (pJunkcode = 5378196641.01533)
		pJunkcode = 8825044734.55685;
	pJunkcode = 7574841335.31333;
	pJunkcode = 4098150137.18237;
	if (pJunkcode = 9784300345.95539)
		pJunkcode = 7816796802.32045;
	pJunkcode = 5533532442.72587;
	if (pJunkcode = 3232246740.79281)
		pJunkcode = 4044976606.29629;
	pJunkcode = 2945658341.84919;
	pJunkcode = 8984919417.17361;
	if (pJunkcode = 5438991792.83385)
		pJunkcode = 8348879255.87112;
	pJunkcode = 3488383158.83472;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3700() {
	float pJunkcode = 5664231360.2554;
	pJunkcode = 844587114.536853;
	if (pJunkcode = 2009528812.64806)
		pJunkcode = 5201011114.94715;
	pJunkcode = 354923649.426806;
	pJunkcode = 6115764941.94265;
	if (pJunkcode = 2793395589.8504)
		pJunkcode = 9001638741.66369;
	pJunkcode = 1690506514.36452;
	if (pJunkcode = 6300744132.77031)
		pJunkcode = 6739593677.19735;
	pJunkcode = 3516997658.67188;
	pJunkcode = 3400313641.35093;
	if (pJunkcode = 367988260.866397)
		pJunkcode = 45904677.6858919;
	pJunkcode = 5335422940.96459;
	if (pJunkcode = 9576676708.56825)
		pJunkcode = 7700996792.20369;
	pJunkcode = 9455916109.58742;
	pJunkcode = 2967129464.08329;
	if (pJunkcode = 8224449243.05997)
		pJunkcode = 3839141136.9365;
	pJunkcode = 4612704562.70079;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3699() {
	float pJunkcode = 2280155116.9598;
	pJunkcode = 3011573865.61974;
	if (pJunkcode = 9049513616.97185)
		pJunkcode = 9981611465.45371;
	pJunkcode = 6671959840.8789;
	pJunkcode = 2344269485.53957;
	if (pJunkcode = 3705508652.48237)
		pJunkcode = 3781067652.66451;
	pJunkcode = 4466792009.75496;
	if (pJunkcode = 1575638506.68087)
		pJunkcode = 8112405456.8109;
	pJunkcode = 6260372799.51447;
	pJunkcode = 5742282077.42673;
	if (pJunkcode = 8262241104.0063)
		pJunkcode = 7753296562.65974;
	pJunkcode = 7647159481.76032;
	if (pJunkcode = 5680699341.92253)
		pJunkcode = 9668957084.45793;
	pJunkcode = 4126956594.65238;
	pJunkcode = 3260515085.46044;
	if (pJunkcode = 2146817975.22367)
		pJunkcode = 9587219619.38821;
	pJunkcode = 2219182910.8489;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3698() {
	float pJunkcode = 5037571048.51964;
	pJunkcode = 29855980.2338527;
	if (pJunkcode = 5805638492.05221)
		pJunkcode = 8868194689.82172;
	pJunkcode = 3701538776.03941;
	pJunkcode = 8260993814.78171;
	if (pJunkcode = 1128726420.37097)
		pJunkcode = 1148504678.19203;
	pJunkcode = 8502796828.13624;
	if (pJunkcode = 2422024194.10392)
		pJunkcode = 3300025463.8436;
	pJunkcode = 8771896761.23494;
	pJunkcode = 8912606427.8738;
	if (pJunkcode = 8989147984.50116)
		pJunkcode = 5846368648.66622;
	pJunkcode = 5606648349.78419;
	if (pJunkcode = 5882482704.53494)
		pJunkcode = 6763453904.3255;
	pJunkcode = 8366081559.41852;
	pJunkcode = 2317943443.57231;
	if (pJunkcode = 3349304263.62548)
		pJunkcode = 3491130658.68283;
	pJunkcode = 2437421985.07622;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3697() {
	float pJunkcode = 1011574343.63115;
	pJunkcode = 6508449441.6739;
	if (pJunkcode = 5961925257.73386)
		pJunkcode = 4972704251.00153;
	pJunkcode = 8331365823.84596;
	pJunkcode = 8231960882.8811;
	if (pJunkcode = 8598119122.26361)
		pJunkcode = 398495196.235364;
	pJunkcode = 7303766049.53489;
	if (pJunkcode = 9798931004.88123)
		pJunkcode = 9218058749.21793;
	pJunkcode = 7553232280.92867;
	pJunkcode = 4640527262.33148;
	if (pJunkcode = 3078482807.27641)
		pJunkcode = 5303136387.02401;
	pJunkcode = 3185959333.1759;
	if (pJunkcode = 6015292590.25305)
		pJunkcode = 3306670705.26602;
	pJunkcode = 5231249506.13722;
	pJunkcode = 9999656456.16417;
	if (pJunkcode = 8957541875.96868)
		pJunkcode = 6515492055.83866;
	pJunkcode = 4433378999.82887;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3696() {
	float pJunkcode = 3344659210.67919;
	pJunkcode = 5430845702.83831;
	if (pJunkcode = 8863917964.28542)
		pJunkcode = 5013715601.3568;
	pJunkcode = 3754656248.85201;
	pJunkcode = 5547378039.49668;
	if (pJunkcode = 1171491848.03166)
		pJunkcode = 3725930403.52961;
	pJunkcode = 3325642359.65632;
	if (pJunkcode = 1938362095.35168)
		pJunkcode = 2897054096.91903;
	pJunkcode = 341371437.287848;
	pJunkcode = 4161255264.86369;
	if (pJunkcode = 3666183563.52912)
		pJunkcode = 8072316013.45299;
	pJunkcode = 9526619556.74338;
	if (pJunkcode = 3175353570.35222)
		pJunkcode = 8808418329.51913;
	pJunkcode = 8981578281.95476;
	pJunkcode = 9181278809.02394;
	if (pJunkcode = 325833118.129265)
		pJunkcode = 937394907.096105;
	pJunkcode = 7519348778.52783;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3695() {
	float pJunkcode = 3760118922.66494;
	pJunkcode = 4205373031.55657;
	if (pJunkcode = 4473256227.16222)
		pJunkcode = 1181645807.50946;
	pJunkcode = 530163042.768821;
	pJunkcode = 3805644290.19064;
	if (pJunkcode = 73369608.6706898)
		pJunkcode = 1512229430.94702;
	pJunkcode = 7759246324.94974;
	if (pJunkcode = 8701089756.70089)
		pJunkcode = 1846112508.67842;
	pJunkcode = 9105817389.51704;
	pJunkcode = 508552053.580857;
	if (pJunkcode = 5035150241.72218)
		pJunkcode = 9202740480.75673;
	pJunkcode = 8726197338.24556;
	if (pJunkcode = 6261234359.74078)
		pJunkcode = 5663358405.41134;
	pJunkcode = 8166664626.55052;
	pJunkcode = 9741752952.69886;
	if (pJunkcode = 640353196.653121)
		pJunkcode = 9365302162.78386;
	pJunkcode = 8783879193.64976;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3694() {
	float pJunkcode = 2667104408.0219;
	pJunkcode = 7536830515.63442;
	if (pJunkcode = 4228131182.15212)
		pJunkcode = 1265174988.91005;
	pJunkcode = 7165701210.11365;
	pJunkcode = 9032185984.2706;
	if (pJunkcode = 5604685900.44969)
		pJunkcode = 5315517866.21674;
	pJunkcode = 9132341207.49675;
	if (pJunkcode = 4262949425.04705)
		pJunkcode = 6130042521.38487;
	pJunkcode = 9361352600.6114;
	pJunkcode = 9538474918.44854;
	if (pJunkcode = 3878635953.07506)
		pJunkcode = 1008755894.01051;
	pJunkcode = 4143716270.61779;
	if (pJunkcode = 6537615705.03757)
		pJunkcode = 8997201660.28178;
	pJunkcode = 2264880196.47435;
	pJunkcode = 8442297424.15698;
	if (pJunkcode = 4044347019.90539)
		pJunkcode = 2342697431.08278;
	pJunkcode = 5866326931.66897;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3693() {
	float pJunkcode = 255080697.865455;
	pJunkcode = 9998776646.85451;
	if (pJunkcode = 158789763.441273)
		pJunkcode = 3746211335.35517;
	pJunkcode = 8217843822.26432;
	pJunkcode = 5933360795.02994;
	if (pJunkcode = 1503872803.61907)
		pJunkcode = 5176066177.95043;
	pJunkcode = 3977484394.78548;
	if (pJunkcode = 3489962922.23702)
		pJunkcode = 2247799126.63074;
	pJunkcode = 6251125033.45103;
	pJunkcode = 4464134344.38403;
	if (pJunkcode = 4427105706.29674)
		pJunkcode = 5014373432.15064;
	pJunkcode = 4715575716.58378;
	if (pJunkcode = 3092790205.04097)
		pJunkcode = 6936501165.52822;
	pJunkcode = 7261804973.85515;
	pJunkcode = 6689315617.8669;
	if (pJunkcode = 1009159436.7032)
		pJunkcode = 4863927546.59734;
	pJunkcode = 3053737234.18125;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3692() {
	float pJunkcode = 3277160563.79376;
	pJunkcode = 9772904473.2588;
	if (pJunkcode = 912139039.525687)
		pJunkcode = 8020267024.70366;
	pJunkcode = 2689924712.13623;
	pJunkcode = 2396287261.32494;
	if (pJunkcode = 1489943744.88206)
		pJunkcode = 2538372167.98908;
	pJunkcode = 7425354891.01011;
	if (pJunkcode = 459105326.376543)
		pJunkcode = 9648683815.01223;
	pJunkcode = 1462173145.24349;
	pJunkcode = 5910638986.60613;
	if (pJunkcode = 1560150036.0646)
		pJunkcode = 5860876984.25175;
	pJunkcode = 3446305883.19936;
	if (pJunkcode = 8570256029.2485)
		pJunkcode = 107784048.105229;
	pJunkcode = 4475568827.00014;
	pJunkcode = 4563951386.81045;
	if (pJunkcode = 5298334628.98009)
		pJunkcode = 6551455469.45908;
	pJunkcode = 6150891985.0322;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3691() {
	float pJunkcode = 733047068.645968;
	pJunkcode = 4309984341.94;
	if (pJunkcode = 8515025599.89352)
		pJunkcode = 9615690827.36699;
	pJunkcode = 7568129071.93206;
	pJunkcode = 1426571164.98664;
	if (pJunkcode = 1410563678.14641)
		pJunkcode = 5873675826.82597;
	pJunkcode = 5515923786.24802;
	if (pJunkcode = 4039108603.99749)
		pJunkcode = 1616276828.82246;
	pJunkcode = 7463414590.66554;
	pJunkcode = 7031400936.79865;
	if (pJunkcode = 7010820662.26464)
		pJunkcode = 9278044852.90838;
	pJunkcode = 6554493476.18108;
	if (pJunkcode = 7889796206.74636)
		pJunkcode = 4740711966.27892;
	pJunkcode = 572229124.018776;
	pJunkcode = 8368439858.79817;
	if (pJunkcode = 1251766646.0437)
		pJunkcode = 3595419380.77882;
	pJunkcode = 2221208167.94975;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3690() {
	float pJunkcode = 3861920505.17643;
	pJunkcode = 4640770695.97657;
	if (pJunkcode = 404787614.285314)
		pJunkcode = 7732206568.73641;
	pJunkcode = 1390020274.30442;
	pJunkcode = 5250100862.79356;
	if (pJunkcode = 6638549268.66528)
		pJunkcode = 3499591797.98497;
	pJunkcode = 5444593846.98947;
	if (pJunkcode = 6385353472.24591)
		pJunkcode = 7278930891.08885;
	pJunkcode = 1379157820.59138;
	pJunkcode = 6175140269.44179;
	if (pJunkcode = 5952192205.28451)
		pJunkcode = 1031872367.38334;
	pJunkcode = 9748918295.32519;
	if (pJunkcode = 4176519673.15071)
		pJunkcode = 198566378.674716;
	pJunkcode = 7357553080.70357;
	pJunkcode = 5605764044.55976;
	if (pJunkcode = 1030869680.60198)
		pJunkcode = 3613123613.28011;
	pJunkcode = 5743945710.24478;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3689() {
	float pJunkcode = 7363942553.23676;
	pJunkcode = 5050810562.91799;
	if (pJunkcode = 747657210.740662)
		pJunkcode = 3957029912.73368;
	pJunkcode = 4289242621.92387;
	pJunkcode = 3918291029.59962;
	if (pJunkcode = 3369810158.82588)
		pJunkcode = 5224076633.84317;
	pJunkcode = 8723194379.16987;
	if (pJunkcode = 4090468029.78189)
		pJunkcode = 2870900652.91021;
	pJunkcode = 2888115072.84313;
	pJunkcode = 5356740445.40453;
	if (pJunkcode = 47665979.6539392)
		pJunkcode = 1659291943.89679;
	pJunkcode = 3413538033.55019;
	if (pJunkcode = 6738798049.29337)
		pJunkcode = 7505063420.2916;
	pJunkcode = 6303397984.41344;
	pJunkcode = 8941845689.89917;
	if (pJunkcode = 8251213251.38634)
		pJunkcode = 9361387247.79301;
	pJunkcode = 2013992999.90244;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3688() {
	float pJunkcode = 376027971.604662;
	pJunkcode = 6147335618.6621;
	if (pJunkcode = 3040441701.29024)
		pJunkcode = 5764854009.6074;
	pJunkcode = 6447312027.57529;
	pJunkcode = 8555586597.4233;
	if (pJunkcode = 7975238617.5756)
		pJunkcode = 6045541051.26776;
	pJunkcode = 9503250161.97161;
	if (pJunkcode = 250336413.491173)
		pJunkcode = 2598967539.14335;
	pJunkcode = 8912429038.30864;
	pJunkcode = 3545951785.1726;
	if (pJunkcode = 6816168709.87988)
		pJunkcode = 8812031957.6851;
	pJunkcode = 2146338327.93955;
	if (pJunkcode = 332313619.5489)
		pJunkcode = 2741504081.51698;
	pJunkcode = 6417791180.65708;
	pJunkcode = 1340067145.51427;
	if (pJunkcode = 794808108.730608)
		pJunkcode = 9460357395.8487;
	pJunkcode = 5084520021.49656;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3687() {
	float pJunkcode = 2796763893.80536;
	pJunkcode = 5187122011.87733;
	if (pJunkcode = 3715976126.92817)
		pJunkcode = 7718930884.45928;
	pJunkcode = 4825398162.3814;
	pJunkcode = 3583762069.82353;
	if (pJunkcode = 5160465109.23258)
		pJunkcode = 3535052307.10086;
	pJunkcode = 9459983142.95809;
	if (pJunkcode = 4280062359.77389)
		pJunkcode = 318773621.583364;
	pJunkcode = 8953624486.01284;
	pJunkcode = 9053553348.89426;
	if (pJunkcode = 1348045831.95109)
		pJunkcode = 9568020114.55197;
	pJunkcode = 3457147321.82866;
	if (pJunkcode = 3223401201.94207)
		pJunkcode = 7151863077.81863;
	pJunkcode = 6437039043.93442;
	pJunkcode = 8153418823.45938;
	if (pJunkcode = 4473056679.32629)
		pJunkcode = 1086390996.341;
	pJunkcode = 158658122.311788;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3686() {
	float pJunkcode = 8657048939.76623;
	pJunkcode = 1999331062.82072;
	if (pJunkcode = 6377243382.16449)
		pJunkcode = 3258563695.47092;
	pJunkcode = 3173376159.47017;
	pJunkcode = 1606522421.36003;
	if (pJunkcode = 216327309.527348)
		pJunkcode = 4389922276.96058;
	pJunkcode = 5069762962.40661;
	if (pJunkcode = 5419848965.39441)
		pJunkcode = 9526243681.03941;
	pJunkcode = 1960160509.0052;
	pJunkcode = 9445842101.02482;
	if (pJunkcode = 223660856.641801)
		pJunkcode = 8192226707.59297;
	pJunkcode = 1775110070.38477;
	if (pJunkcode = 2134752735.09022)
		pJunkcode = 7257214419.03262;
	pJunkcode = 852260914.623038;
	pJunkcode = 3776119238.39305;
	if (pJunkcode = 2052434458.83338)
		pJunkcode = 4772945849.798;
	pJunkcode = 3456726240.17898;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3685() {
	float pJunkcode = 7715490976.48422;
	pJunkcode = 8571439656.22683;
	if (pJunkcode = 5288378757.50469)
		pJunkcode = 8973143012.20674;
	pJunkcode = 8826636926.33463;
	pJunkcode = 6407607316.71047;
	if (pJunkcode = 9292873644.07399)
		pJunkcode = 4237066797.62148;
	pJunkcode = 1720364187.02166;
	if (pJunkcode = 439103341.499005)
		pJunkcode = 9832347609.6765;
	pJunkcode = 5948354244.70844;
	pJunkcode = 9696741791.16413;
	if (pJunkcode = 5449413495.56113)
		pJunkcode = 5369844537.54155;
	pJunkcode = 1353581737.25404;
	if (pJunkcode = 2249689711.17744)
		pJunkcode = 2125253407.34521;
	pJunkcode = 8153408974.82164;
	pJunkcode = 4254404926.22319;
	if (pJunkcode = 5329064897.92569)
		pJunkcode = 2675422062.21815;
	pJunkcode = 8588329741.65993;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3684() {
	float pJunkcode = 6455888030.39851;
	pJunkcode = 7830335601.22017;
	if (pJunkcode = 6574082733.28889)
		pJunkcode = 6068556034.77794;
	pJunkcode = 7136950012.116;
	pJunkcode = 6103519960.67806;
	if (pJunkcode = 8897489262.54168)
		pJunkcode = 2712281739.12529;
	pJunkcode = 9079880617.86674;
	if (pJunkcode = 238677551.899299)
		pJunkcode = 7924829259.3733;
	pJunkcode = 9993562279.78331;
	pJunkcode = 6943086518.85138;
	if (pJunkcode = 8154083683.34005)
		pJunkcode = 6597794222.70436;
	pJunkcode = 4776285874.42579;
	if (pJunkcode = 2402693343.83015)
		pJunkcode = 4003589764.40376;
	pJunkcode = 6042043881.49683;
	pJunkcode = 2769042379.03222;
	if (pJunkcode = 6820324655.7276)
		pJunkcode = 6461604633.75698;
	pJunkcode = 3104124191.97134;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3683() {
	float pJunkcode = 5574128471.60398;
	pJunkcode = 560277804.451739;
	if (pJunkcode = 6418903455.0699)
		pJunkcode = 2125253239.70882;
	pJunkcode = 5158136346.45301;
	pJunkcode = 3159041553.38868;
	if (pJunkcode = 6465648433.19006)
		pJunkcode = 8552072.96753531;
	pJunkcode = 9617074689.02478;
	if (pJunkcode = 7898340600.79749)
		pJunkcode = 2437059555.20745;
	pJunkcode = 6550855512.79959;
	pJunkcode = 9936964186.63643;
	if (pJunkcode = 4011373547.92954)
		pJunkcode = 7440405637.40872;
	pJunkcode = 3424563520.63447;
	if (pJunkcode = 9018127631.31353)
		pJunkcode = 5794810976.55617;
	pJunkcode = 2698711771.499;
	pJunkcode = 8291436957.52056;
	if (pJunkcode = 7414775724.16488)
		pJunkcode = 4389333610.1461;
	pJunkcode = 6789398336.02737;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3682() {
	float pJunkcode = 9840808588.3271;
	pJunkcode = 1112507220.41533;
	if (pJunkcode = 9772170160.69167)
		pJunkcode = 4415748795.09045;
	pJunkcode = 3036547375.30731;
	pJunkcode = 6441888988.12648;
	if (pJunkcode = 3999308459.1345)
		pJunkcode = 3105772951.71661;
	pJunkcode = 4591064251.11479;
	if (pJunkcode = 5070422411.35724)
		pJunkcode = 1795023685.27522;
	pJunkcode = 4864109387.83989;
	pJunkcode = 5549532106.5347;
	if (pJunkcode = 51795206.1241951)
		pJunkcode = 6802919768.26305;
	pJunkcode = 3855939234.23051;
	if (pJunkcode = 2287504205.36431)
		pJunkcode = 9681358070.00692;
	pJunkcode = 1586651467.60179;
	pJunkcode = 4711526559.77116;
	if (pJunkcode = 248961171.137615)
		pJunkcode = 9230108620.35428;
	pJunkcode = 7687959170.22241;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3681() {
	float pJunkcode = 4370330775.38781;
	pJunkcode = 6508466661.6561;
	if (pJunkcode = 2138477652.86264)
		pJunkcode = 1865504839.59713;
	pJunkcode = 8695081826.91287;
	pJunkcode = 6811018987.59042;
	if (pJunkcode = 8162169205.37764)
		pJunkcode = 1657035334.61214;
	pJunkcode = 1201891917.53218;
	if (pJunkcode = 4311188101.58276)
		pJunkcode = 2116898346.95182;
	pJunkcode = 8162019515.39715;
	pJunkcode = 758670786.328333;
	if (pJunkcode = 3678839482.11369)
		pJunkcode = 8951018406.08345;
	pJunkcode = 8156951168.93219;
	if (pJunkcode = 4711653108.94539)
		pJunkcode = 5763066069.75179;
	pJunkcode = 7730772588.45546;
	pJunkcode = 4011150097.93876;
	if (pJunkcode = 1328253809.32003)
		pJunkcode = 2768944016.05385;
	pJunkcode = 3503637089.84755;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3680() {
	float pJunkcode = 3651499316.83823;
	pJunkcode = 5626177623.33313;
	if (pJunkcode = 980174297.470869)
		pJunkcode = 1943889581.58275;
	pJunkcode = 7946152458.79382;
	pJunkcode = 4653490337.34276;
	if (pJunkcode = 2005872078.6478)
		pJunkcode = 4637872651.66805;
	pJunkcode = 7310254315.77884;
	if (pJunkcode = 436396255.989864)
		pJunkcode = 7572297623.10477;
	pJunkcode = 7202754427.88618;
	pJunkcode = 6428923845.59976;
	if (pJunkcode = 2507728564.7273)
		pJunkcode = 3971728122.13865;
	pJunkcode = 1184363537.92917;
	if (pJunkcode = 1578797432.78843)
		pJunkcode = 3352734884.07904;
	pJunkcode = 3387524239.9649;
	pJunkcode = 8204887729.77421;
	if (pJunkcode = 5239730955.0533)
		pJunkcode = 7913641260.4532;
	pJunkcode = 860887501.726041;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3679() {
	float pJunkcode = 4746301980.88235;
	pJunkcode = 8766479356.23136;
	if (pJunkcode = 884130375.26298)
		pJunkcode = 7138092735.81589;
	pJunkcode = 508084210.692328;
	pJunkcode = 223262198.687171;
	if (pJunkcode = 4226061752.86329)
		pJunkcode = 768764899.615957;
	pJunkcode = 7945769106.97264;
	if (pJunkcode = 8664002608.85797)
		pJunkcode = 4277158862.62623;
	pJunkcode = 388871107.437583;
	pJunkcode = 768595919.809544;
	if (pJunkcode = 5290770835.61633)
		pJunkcode = 6343596896.66037;
	pJunkcode = 3992455239.16285;
	if (pJunkcode = 1605464319.11134)
		pJunkcode = 7226346855.88605;
	pJunkcode = 2654022020.14044;
	pJunkcode = 754428572.933216;
	if (pJunkcode = 8384767335.56184)
		pJunkcode = 8966186199.93275;
	pJunkcode = 7876196781.68162;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3678() {
	float pJunkcode = 3308683156.80953;
	pJunkcode = 3069261867.69146;
	if (pJunkcode = 1123870543.71343)
		pJunkcode = 3720253741.86863;
	pJunkcode = 3529568848.70422;
	pJunkcode = 4623198456.17553;
	if (pJunkcode = 5350896298.69492)
		pJunkcode = 3189135383.80605;
	pJunkcode = 4161965840.58408;
	if (pJunkcode = 1364093847.21617)
		pJunkcode = 6160815360.65963;
	pJunkcode = 8405545500.19418;
	pJunkcode = 3652716980.98797;
	if (pJunkcode = 80795274.3081006)
		pJunkcode = 9577243503.49079;
	pJunkcode = 8756752273.50001;
	if (pJunkcode = 3684984938.93101)
		pJunkcode = 8987382431.08807;
	pJunkcode = 2115282339.16567;
	pJunkcode = 7462473612.70087;
	if (pJunkcode = 1743397263.62022)
		pJunkcode = 1324663622.30885;
	pJunkcode = 1196988543.89832;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3677() {
	float pJunkcode = 6960521147.80494;
	pJunkcode = 5585065858.78203;
	if (pJunkcode = 4215264761.15938)
		pJunkcode = 417891394.432392;
	pJunkcode = 4319935070.23792;
	pJunkcode = 1566611766.86545;
	if (pJunkcode = 4612653293.90576)
		pJunkcode = 5424360430.7665;
	pJunkcode = 7445406610.34408;
	if (pJunkcode = 2142126289.0941)
		pJunkcode = 5444101880.75919;
	pJunkcode = 8913583403.4243;
	pJunkcode = 6965747526.48893;
	if (pJunkcode = 2210611632.1122)
		pJunkcode = 239448257.860402;
	pJunkcode = 1920054101.77623;
	if (pJunkcode = 6154185984.15861)
		pJunkcode = 7176442707.1886;
	pJunkcode = 2405443800.59787;
	pJunkcode = 7808611185.06182;
	if (pJunkcode = 3641006994.95462)
		pJunkcode = 3875392661.62744;
	pJunkcode = 1960730663.64033;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3676() {
	float pJunkcode = 1892152471.01836;
	pJunkcode = 4367360700.98338;
	if (pJunkcode = 7191318890.5578)
		pJunkcode = 636100519.953514;
	pJunkcode = 5725463724.06929;
	pJunkcode = 5727387938.33785;
	if (pJunkcode = 9568280260.61353)
		pJunkcode = 6464785606.37771;
	pJunkcode = 2309906079.74346;
	if (pJunkcode = 3080645163.00245)
		pJunkcode = 9221321994.05217;
	pJunkcode = 2232139282.73298;
	pJunkcode = 7509533195.42554;
	if (pJunkcode = 881165653.581517)
		pJunkcode = 8232545351.80106;
	pJunkcode = 3216328755.48527;
	if (pJunkcode = 7950151161.71051)
		pJunkcode = 7966020986.48102;
	pJunkcode = 6356614213.68426;
	pJunkcode = 8482794670.96575;
	if (pJunkcode = 400849914.715527)
		pJunkcode = 552479133.88348;
	pJunkcode = 7378813793.66082;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3675() {
	float pJunkcode = 1612793109.54331;
	pJunkcode = 4462761837.76317;
	if (pJunkcode = 6952799727.34342)
		pJunkcode = 8921132773.07393;
	pJunkcode = 1125290819.71974;
	pJunkcode = 4079840085.40461;
	if (pJunkcode = 3958652577.08137)
		pJunkcode = 4262609013.75207;
	pJunkcode = 4648256403.37994;
	if (pJunkcode = 862367928.927614)
		pJunkcode = 433272475.316504;
	pJunkcode = 5274828938.84082;
	pJunkcode = 7260933120.28902;
	if (pJunkcode = 2426685833.73939)
		pJunkcode = 8577892544.20826;
	pJunkcode = 3936670585.28607;
	if (pJunkcode = 4199539273.64639)
		pJunkcode = 9687282088.4701;
	pJunkcode = 6239106838.60698;
	pJunkcode = 2029509393.61857;
	if (pJunkcode = 3497756731.35006)
		pJunkcode = 2662429664.80884;
	pJunkcode = 7950653626.16953;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3674() {
	float pJunkcode = 7713078327.99092;
	pJunkcode = 9569811993.86267;
	if (pJunkcode = 4300867946.9252)
		pJunkcode = 5933619679.16203;
	pJunkcode = 4165477857.81668;
	pJunkcode = 130299999.045762;
	if (pJunkcode = 1789031257.19643)
		pJunkcode = 1936107178.72888;
	pJunkcode = 4200601420.12292;
	if (pJunkcode = 5165740186.28701)
		pJunkcode = 9016982197.49204;
	pJunkcode = 7720122169.1609;
	pJunkcode = 1544049545.95381;
	if (pJunkcode = 3739965208.23087)
		pJunkcode = 5429679051.37245;
	pJunkcode = 2680900309.9171;
	if (pJunkcode = 679919390.968307)
		pJunkcode = 3699788377.42137;
	pJunkcode = 4198258140.49788;
	pJunkcode = 2190067024.54782;
	if (pJunkcode = 1492287997.72945)
		pJunkcode = 528754140.425716;
	pJunkcode = 1615883188.31802;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3673() {
	float pJunkcode = 757573133.533719;
	pJunkcode = 3595389792.95651;
	if (pJunkcode = 1213445410.34167)
		pJunkcode = 4337608432.03288;
	pJunkcode = 3294809776.63584;
	pJunkcode = 6570526664.93903;
	if (pJunkcode = 7173109143.54657)
		pJunkcode = 1494690099.10006;
	pJunkcode = 9375102591.0911;
	if (pJunkcode = 5291401846.91653)
		pJunkcode = 3314239676.73152;
	pJunkcode = 1437514777.99384;
	pJunkcode = 9304064611.55511;
	if (pJunkcode = 8754835737.90929)
		pJunkcode = 5449123524.19238;
	pJunkcode = 3300304126.74108;
	if (pJunkcode = 6332180928.55189)
		pJunkcode = 4177564250.40257;
	pJunkcode = 827722479.566893;
	pJunkcode = 8223399301.44373;
	if (pJunkcode = 7573890632.10994)
		pJunkcode = 9545403751.74388;
	pJunkcode = 582833992.781726;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3672() {
	float pJunkcode = 5304663698.86344;
	pJunkcode = 1834548268.51389;
	if (pJunkcode = 2896405041.4244)
		pJunkcode = 7046333904.66567;
	pJunkcode = 2841794573.33777;
	pJunkcode = 7661089797.77023;
	if (pJunkcode = 7326213225.31936)
		pJunkcode = 5774406478.0871;
	pJunkcode = 4985307243.05346;
	if (pJunkcode = 4553691979.38864)
		pJunkcode = 8475756047.47832;
	pJunkcode = 2381520197.60229;
	pJunkcode = 3959872401.87788;
	if (pJunkcode = 214079913.259134)
		pJunkcode = 3892433994.66247;
	pJunkcode = 8777363700.46261;
	if (pJunkcode = 9491145366.63568)
		pJunkcode = 930377338.273228;
	pJunkcode = 3124355852.66881;
	pJunkcode = 2188375980.8522;
	if (pJunkcode = 6390174564.35313)
		pJunkcode = 7657464635.28952;
	pJunkcode = 7618233483.24837;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3671() {
	float pJunkcode = 1352791378.25484;
	pJunkcode = 8050191244.34988;
	if (pJunkcode = 7428570731.99934)
		pJunkcode = 944757378.472171;
	pJunkcode = 1005298575.13403;
	pJunkcode = 3944803613.61311;
	if (pJunkcode = 2218770676.6752)
		pJunkcode = 2366183496.12336;
	pJunkcode = 4028197703.3686;
	if (pJunkcode = 6199979218.33281)
		pJunkcode = 2683054849.59659;
	pJunkcode = 7005256768.6699;
	pJunkcode = 5141784499.54957;
	if (pJunkcode = 3635068718.08541)
		pJunkcode = 376392350.246427;
	pJunkcode = 8640883609.29791;
	if (pJunkcode = 2458728821.32274)
		pJunkcode = 1130626795.34832;
	pJunkcode = 6225551243.4726;
	pJunkcode = 4415503771.22797;
	if (pJunkcode = 883974019.407824)
		pJunkcode = 5406604508.05093;
	pJunkcode = 1926112988.36935;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3670() {
	float pJunkcode = 7683025296.07925;
	pJunkcode = 7793111236.00961;
	if (pJunkcode = 3696493349.1938)
		pJunkcode = 5848604510.61969;
	pJunkcode = 9749931650.29996;
	pJunkcode = 8454091503.12126;
	if (pJunkcode = 7043000497.01861)
		pJunkcode = 4956990619.10636;
	pJunkcode = 4080838194.78129;
	if (pJunkcode = 5614631250.41352)
		pJunkcode = 8853961336.118;
	pJunkcode = 3968019814.33825;
	pJunkcode = 8330349807.64355;
	if (pJunkcode = 5677490457.84335)
		pJunkcode = 7336814704.10204;
	pJunkcode = 290508934.648561;
	if (pJunkcode = 1821623903.53832)
		pJunkcode = 4265706049.43685;
	pJunkcode = 7228151268.34497;
	pJunkcode = 7388330373.984;
	if (pJunkcode = 2955535698.9601)
		pJunkcode = 5307668341.28785;
	pJunkcode = 744550659.362636;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3669() {
	float pJunkcode = 8421833587.06347;
	pJunkcode = 855943469.283477;
	if (pJunkcode = 4244168929.68849)
		pJunkcode = 1036006320.2684;
	pJunkcode = 2260690152.91985;
	pJunkcode = 4821208244.55419;
	if (pJunkcode = 9669333120.81344)
		pJunkcode = 7658747747.4787;
	pJunkcode = 5046994886.33998;
	if (pJunkcode = 3325698962.54806)
		pJunkcode = 3600257385.53487;
	pJunkcode = 9274674367.3521;
	pJunkcode = 2654156045.43187;
	if (pJunkcode = 5846838756.97426)
		pJunkcode = 7084905235.23089;
	pJunkcode = 5937558556.8431;
	if (pJunkcode = 6335102288.2879)
		pJunkcode = 4863352833.18379;
	pJunkcode = 6901364094.52313;
	pJunkcode = 5576899310.16242;
	if (pJunkcode = 7855362733.59704)
		pJunkcode = 2177393897.62246;
	pJunkcode = 954313066.967475;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3668() {
	float pJunkcode = 4880606540.56664;
	pJunkcode = 5754790019.64721;
	if (pJunkcode = 4445868283.18817)
		pJunkcode = 2755193506.36701;
	pJunkcode = 4591828.6319754;
	pJunkcode = 3438864515.62036;
	if (pJunkcode = 7521113652.08223)
		pJunkcode = 878092885.592045;
	pJunkcode = 6516930504.09498;
	if (pJunkcode = 6812046675.47628)
		pJunkcode = 4761957561.69366;
	pJunkcode = 7053182654.33367;
	pJunkcode = 9362284251.47376;
	if (pJunkcode = 4671376569.40985)
		pJunkcode = 5328019382.37572;
	pJunkcode = 7074880230.04382;
	if (pJunkcode = 7810680896.07292)
		pJunkcode = 430031616.924783;
	pJunkcode = 1419259820.38033;
	pJunkcode = 3036749379.48197;
	if (pJunkcode = 8808129217.28686)
		pJunkcode = 2654703478.35171;
	pJunkcode = 3701802945.37426;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3667() {
	float pJunkcode = 8764830768.77905;
	pJunkcode = 5392764450.26573;
	if (pJunkcode = 2320006337.37033)
		pJunkcode = 7626006035.13621;
	pJunkcode = 5996442022.8847;
	pJunkcode = 224660786.735907;
	if (pJunkcode = 4287135714.26112)
		pJunkcode = 5593039476.54642;
	pJunkcode = 8716410823.75844;
	if (pJunkcode = 1911880645.14051)
		pJunkcode = 6199185439.08565;
	pJunkcode = 8034280262.51807;
	pJunkcode = 7647665073.10361;
	if (pJunkcode = 308135777.346114)
		pJunkcode = 7705537370.42833;
	pJunkcode = 8617818462.92719;
	if (pJunkcode = 1080321206.39788)
		pJunkcode = 9023406324.19169;
	pJunkcode = 2236210348.69007;
	pJunkcode = 956991444.745337;
	if (pJunkcode = 3653848505.6459)
		pJunkcode = 7939176498.71918;
	pJunkcode = 5362605597.88474;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3666() {
	float pJunkcode = 2022189948.91518;
	pJunkcode = 5513995648.82235;
	if (pJunkcode = 294532622.371763)
		pJunkcode = 4849246682.90186;
	pJunkcode = 3680091541.99046;
	pJunkcode = 350637028.687108;
	if (pJunkcode = 5130838353.3703)
		pJunkcode = 8130875787.60093;
	pJunkcode = 680844431.834789;
	if (pJunkcode = 2901763682.88948)
		pJunkcode = 9511044962.93327;
	pJunkcode = 1397045198.17378;
	pJunkcode = 9227029829.09629;
	if (pJunkcode = 2819968329.71572)
		pJunkcode = 1598092453.11275;
	pJunkcode = 3156710175.94713;
	if (pJunkcode = 4408400931.36288)
		pJunkcode = 898587835.533334;
	pJunkcode = 6826671630.1084;
	pJunkcode = 4866051362.25234;
	if (pJunkcode = 1751325964.14603)
		pJunkcode = 5781744136.9496;
	pJunkcode = 799245711.647944;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3665() {
	float pJunkcode = 3975095881.27535;
	pJunkcode = 9964562467.71055;
	if (pJunkcode = 4111684493.71782)
		pJunkcode = 4095464530.08532;
	pJunkcode = 6412426590.42855;
	pJunkcode = 5060825630.99256;
	if (pJunkcode = 5367680565.987)
		pJunkcode = 5785751510.39275;
	pJunkcode = 7513103741.04005;
	if (pJunkcode = 2706886008.10958)
		pJunkcode = 7814450616.99805;
	pJunkcode = 9532787.74077739;
	pJunkcode = 2484190287.93701;
	if (pJunkcode = 8810252718.62364)
		pJunkcode = 5850899746.12186;
	pJunkcode = 6224625518.09087;
	if (pJunkcode = 9501242858.26216)
		pJunkcode = 6294792624.26794;
	pJunkcode = 7388101908.87131;
	pJunkcode = 7823532837.47272;
	if (pJunkcode = 8957940352.31722)
		pJunkcode = 597594160.129683;
	pJunkcode = 7564924513.80298;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3664() {
	float pJunkcode = 4017958652.1264;
	pJunkcode = 3817118551.52442;
	if (pJunkcode = 7286946671.73286)
		pJunkcode = 6912653944.18977;
	pJunkcode = 2490360499.06081;
	pJunkcode = 2238226916.63819;
	if (pJunkcode = 8304497543.35858)
		pJunkcode = 4959746863.986;
	pJunkcode = 1059159976.32319;
	if (pJunkcode = 1793303419.29775)
		pJunkcode = 235175562.582243;
	pJunkcode = 7052904205.63824;
	pJunkcode = 6828732065.23941;
	if (pJunkcode = 5930267109.74462)
		pJunkcode = 6601620323.00605;
	pJunkcode = 2990455154.65199;
	if (pJunkcode = 1796232471.68131)
		pJunkcode = 9316249334.52341;
	pJunkcode = 8599929818.75876;
	pJunkcode = 7305286246.141;
	if (pJunkcode = 5020550001.81573)
		pJunkcode = 4897425855.50502;
	pJunkcode = 9202897081.08161;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3663() {
	float pJunkcode = 2994266810.285;
	pJunkcode = 4675147033.05731;
	if (pJunkcode = 867072401.988258)
		pJunkcode = 6425028108.67724;
	pJunkcode = 5749311790.57453;
	pJunkcode = 8384457150.03875;
	if (pJunkcode = 3628286177.80544)
		pJunkcode = 9513600691.41119;
	pJunkcode = 5426683885.06048;
	if (pJunkcode = 6261519653.58868)
		pJunkcode = 4640289106.9043;
	pJunkcode = 8221833405.29589;
	pJunkcode = 5237841357.69866;
	if (pJunkcode = 6100083617.30451)
		pJunkcode = 6684250863.94284;
	pJunkcode = 494090220.794741;
	if (pJunkcode = 7479272881.1213)
		pJunkcode = 4602261223.23816;
	pJunkcode = 6437054808.73965;
	pJunkcode = 4823388807.45449;
	if (pJunkcode = 8717864368.60273)
		pJunkcode = 8510339350.30511;
	pJunkcode = 3412428902.31963;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3662() {
	float pJunkcode = 986022143.822407;
	pJunkcode = 3380216299.30857;
	if (pJunkcode = 9358291343.65384)
		pJunkcode = 7309697597.66579;
	pJunkcode = 1403898315.70097;
	pJunkcode = 7143815357.78632;
	if (pJunkcode = 3781608214.82077)
		pJunkcode = 2928704901.5032;
	pJunkcode = 6723013197.54482;
	if (pJunkcode = 459886067.238036)
		pJunkcode = 2470777209.34272;
	pJunkcode = 3803162705.88974;
	pJunkcode = 5059920625.00261;
	if (pJunkcode = 9764227048.09315)
		pJunkcode = 4467848479.87716;
	pJunkcode = 3529898456.76537;
	if (pJunkcode = 4862864298.84288)
		pJunkcode = 1116750129.96711;
	pJunkcode = 7668006577.54369;
	pJunkcode = 1908140039.39622;
	if (pJunkcode = 2873336942.79064)
		pJunkcode = 5606874500.58344;
	pJunkcode = 4229865997.57101;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3661() {
	float pJunkcode = 5442828011.85412;
	pJunkcode = 487922617.348069;
	if (pJunkcode = 2674239874.11881)
		pJunkcode = 9749935387.19442;
	pJunkcode = 7338860745.78659;
	pJunkcode = 339106818.709149;
	if (pJunkcode = 1514881464.201)
		pJunkcode = 8346746634.64923;
	pJunkcode = 8850111086.86345;
	if (pJunkcode = 8813169126.10139)
		pJunkcode = 505651289.837613;
	pJunkcode = 6625871596.93121;
	pJunkcode = 8140230831.63893;
	if (pJunkcode = 1950832966.6478)
		pJunkcode = 9134012267.53806;
	pJunkcode = 1869705895.93095;
	if (pJunkcode = 9327361682.86729)
		pJunkcode = 3252454387.57784;
	pJunkcode = 8038251303.10261;
	pJunkcode = 4522456847.85239;
	if (pJunkcode = 808712508.620741)
		pJunkcode = 5688686347.31505;
	pJunkcode = 2403890677.4114;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3660() {
	float pJunkcode = 2363305011.72161;
	pJunkcode = 2829377983.63509;
	if (pJunkcode = 9076270100.28706)
		pJunkcode = 325289894.745366;
	pJunkcode = 4328264670.85503;
	pJunkcode = 146147490.924264;
	if (pJunkcode = 7011428225.42519)
		pJunkcode = 2373950841.97007;
	pJunkcode = 2900416005.08017;
	if (pJunkcode = 2283622198.32754)
		pJunkcode = 2088231234.90025;
	pJunkcode = 3489740356.99384;
	pJunkcode = 4916406997.64917;
	if (pJunkcode = 3342447823.63823)
		pJunkcode = 8172978315.79536;
	pJunkcode = 5891179903.80942;
	if (pJunkcode = 2316651593.33645)
		pJunkcode = 9281920676.02508;
	pJunkcode = 3481200368.05444;
	pJunkcode = 9878112885.82659;
	if (pJunkcode = 1968769647.95425)
		pJunkcode = 1944727414.27545;
	pJunkcode = 7999236681.39471;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3659() {
	float pJunkcode = 4182903120.08711;
	pJunkcode = 5608935622.55797;
	if (pJunkcode = 3294060723.70807)
		pJunkcode = 509296731.664589;
	pJunkcode = 8951304337.88094;
	pJunkcode = 9644976610.81804;
	if (pJunkcode = 1605029442.38397)
		pJunkcode = 7026432182.64556;
	pJunkcode = 1037466889.54296;
	if (pJunkcode = 3947200774.36688)
		pJunkcode = 1098861548.66232;
	pJunkcode = 7250815647.33652;
	pJunkcode = 9500531807.57201;
	if (pJunkcode = 2725188388.19435)
		pJunkcode = 3446732869.37512;
	pJunkcode = 7697975949.5851;
	if (pJunkcode = 1660818240.60476)
		pJunkcode = 1141634012.3947;
	pJunkcode = 5658587075.69995;
	pJunkcode = 5475700098.74006;
	if (pJunkcode = 2773446792.27321)
		pJunkcode = 5055590346.62551;
	pJunkcode = 3178670836.01218;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3658() {
	float pJunkcode = 5417034729.96627;
	pJunkcode = 8397437493.92618;
	if (pJunkcode = 9088547211.85917)
		pJunkcode = 2829316635.70228;
	pJunkcode = 1904471563.864;
	pJunkcode = 435082117.099379;
	if (pJunkcode = 5621341341.50661)
		pJunkcode = 1578057550.96799;
	pJunkcode = 1001518016.05082;
	if (pJunkcode = 3439300884.77723)
		pJunkcode = 8459817780.49624;
	pJunkcode = 378941024.026388;
	pJunkcode = 9361181995.66335;
	if (pJunkcode = 5485825704.38561)
		pJunkcode = 9900324048.20438;
	pJunkcode = 9018382424.65212;
	if (pJunkcode = 4071282455.06376)
		pJunkcode = 4825162435.35355;
	pJunkcode = 8041213752.37972;
	pJunkcode = 6979177366.39364;
	if (pJunkcode = 4127617575.61332)
		pJunkcode = 9592842195.03154;
	pJunkcode = 3231116501.34718;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3657() {
	float pJunkcode = 8243026735.41041;
	pJunkcode = 2319595693.41402;
	if (pJunkcode = 4524251332.51193)
		pJunkcode = 5108027003.83299;
	pJunkcode = 3260592526.4864;
	pJunkcode = 1736956377.93844;
	if (pJunkcode = 2176648125.24565)
		pJunkcode = 9325374970.49404;
	pJunkcode = 6847967248.68669;
	if (pJunkcode = 2658779738.70851)
		pJunkcode = 4482183705.28196;
	pJunkcode = 3827171680.0437;
	pJunkcode = 2786685494.10074;
	if (pJunkcode = 2003966537.21012)
		pJunkcode = 6145116783.44333;
	pJunkcode = 836443001.253714;
	if (pJunkcode = 1881847890.24682)
		pJunkcode = 5640823190.256;
	pJunkcode = 4375083150.54605;
	pJunkcode = 9126791283.60094;
	if (pJunkcode = 3468407253.05678)
		pJunkcode = 8845506022.18031;
	pJunkcode = 8940028856.04964;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3656() {
	float pJunkcode = 875870418.251552;
	pJunkcode = 563063336.805163;
	if (pJunkcode = 9592848369.63846)
		pJunkcode = 9311742844.69242;
	pJunkcode = 4278829850.49076;
	pJunkcode = 6876926958.41042;
	if (pJunkcode = 6785225659.13691)
		pJunkcode = 9206190027.82404;
	pJunkcode = 2251537844.13741;
	if (pJunkcode = 214693485.718507)
		pJunkcode = 1191447425.56939;
	pJunkcode = 3695135412.96892;
	pJunkcode = 4970229460.63087;
	if (pJunkcode = 9552183425.9073)
		pJunkcode = 9234872012.9484;
	pJunkcode = 1267407868.33281;
	if (pJunkcode = 5725604384.96771)
		pJunkcode = 886130142.319866;
	pJunkcode = 1329791444.76696;
	pJunkcode = 266917810.487259;
	if (pJunkcode = 4230303421.44014)
		pJunkcode = 6477081234.78574;
	pJunkcode = 924945239.474666;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3655() {
	float pJunkcode = 360319045.169741;
	pJunkcode = 3560105518.57733;
	if (pJunkcode = 9979719812.22505)
		pJunkcode = 5703908287.71736;
	pJunkcode = 6916192643.5473;
	pJunkcode = 800352393.524626;
	if (pJunkcode = 8367624384.60859)
		pJunkcode = 3800491799.57251;
	pJunkcode = 9854963120.33295;
	if (pJunkcode = 7076673743.80929)
		pJunkcode = 8519996295.83475;
	pJunkcode = 9832828274.42035;
	pJunkcode = 1736494388.35376;
	if (pJunkcode = 8063983663.13347)
		pJunkcode = 2190454893.23832;
	pJunkcode = 8250190522.74359;
	if (pJunkcode = 4741172726.53009)
		pJunkcode = 8116745278.82065;
	pJunkcode = 8776099333.28109;
	pJunkcode = 931917558.11446;
	if (pJunkcode = 9412289522.16512)
		pJunkcode = 2460040636.58664;
	pJunkcode = 2276372995.5986;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3654() {
	float pJunkcode = 6706312611.07756;
	pJunkcode = 3828925424.40463;
	if (pJunkcode = 8543160589.81527)
		pJunkcode = 5956051758.35854;
	pJunkcode = 8373364824.92999;
	pJunkcode = 136822400.249023;
	if (pJunkcode = 4253026182.13176)
		pJunkcode = 8585389944.24963;
	pJunkcode = 7853721179.88813;
	if (pJunkcode = 2715658233.98038)
		pJunkcode = 234036.93284896;
	pJunkcode = 1557922590.85472;
	pJunkcode = 2126441965.62726;
	if (pJunkcode = 4470457633.14031)
		pJunkcode = 6562380391.00913;
	pJunkcode = 5128919426.63278;
	if (pJunkcode = 4074908891.11535)
		pJunkcode = 9050127031.10797;
	pJunkcode = 4394424394.27582;
	pJunkcode = 939122078.761409;
	if (pJunkcode = 3215646400.88499)
		pJunkcode = 8628788467.02393;
	pJunkcode = 1874033063.72176;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3653() {
	float pJunkcode = 8880173601.8724;
	pJunkcode = 3518163192.56182;
	if (pJunkcode = 8924717591.06416)
		pJunkcode = 7946244595.94305;
	pJunkcode = 7219089296.94887;
	pJunkcode = 1453429086.94227;
	if (pJunkcode = 8113599512.16947)
		pJunkcode = 4091638511.27354;
	pJunkcode = 3234307939.60069;
	if (pJunkcode = 605188309.649195)
		pJunkcode = 9492279750.42532;
	pJunkcode = 5032384632.48769;
	pJunkcode = 6458422403.86334;
	if (pJunkcode = 1808807384.63841)
		pJunkcode = 8264064103.14963;
	pJunkcode = 3483139902.42615;
	if (pJunkcode = 7145582880.92695)
		pJunkcode = 3395950587.73187;
	pJunkcode = 3582965282.43454;
	pJunkcode = 9744380611.15652;
	if (pJunkcode = 5506117178.67729)
		pJunkcode = 6752990851.28941;
	pJunkcode = 260557753.483423;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3652() {
	float pJunkcode = 8018409364.40903;
	pJunkcode = 9503046453.49369;
	if (pJunkcode = 8514659835.92327)
		pJunkcode = 7952583595.95404;
	pJunkcode = 3068582456.04108;
	pJunkcode = 4999602636.19009;
	if (pJunkcode = 5222901945.14387)
		pJunkcode = 660842523.790596;
	pJunkcode = 2183696972.22441;
	if (pJunkcode = 4843384079.8917)
		pJunkcode = 9464574782.76056;
	pJunkcode = 4782424779.45431;
	pJunkcode = 2872682788.35591;
	if (pJunkcode = 9888468593.3679)
		pJunkcode = 3075575028.41602;
	pJunkcode = 4821313427.07392;
	if (pJunkcode = 9056743026.96571)
		pJunkcode = 213503577.03223;
	pJunkcode = 5033920195.76262;
	pJunkcode = 4777717507.79732;
	if (pJunkcode = 3607455023.77682)
		pJunkcode = 7126749687.20749;
	pJunkcode = 2306422069.42892;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3651() {
	float pJunkcode = 110402875.38488;
	pJunkcode = 6928756243.37656;
	if (pJunkcode = 5325258669.52071)
		pJunkcode = 5541107651.66193;
	pJunkcode = 8722058922.0417;
	pJunkcode = 8278520233.9703;
	if (pJunkcode = 29266004.9242371)
		pJunkcode = 8185412130.13681;
	pJunkcode = 6763729994.47009;
	if (pJunkcode = 5121085429.02424)
		pJunkcode = 508787608.315753;
	pJunkcode = 6766562853.83517;
	pJunkcode = 2684735464.91197;
	if (pJunkcode = 9062438123.97269)
		pJunkcode = 3736671772.54898;
	pJunkcode = 4948396204.21997;
	if (pJunkcode = 5784904003.68355)
		pJunkcode = 5733693390.11689;
	pJunkcode = 9969682967.71814;
	pJunkcode = 6687369468.84275;
	if (pJunkcode = 3074372679.71286)
		pJunkcode = 9357400302.70625;
	pJunkcode = 862851313.106191;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3650() {
	float pJunkcode = 9251600725.07615;
	pJunkcode = 5862546805.64643;
	if (pJunkcode = 8981829776.11612)
		pJunkcode = 2957957662.9143;
	pJunkcode = 1456892647.94588;
	pJunkcode = 5543187451.12392;
	if (pJunkcode = 3112888655.34611)
		pJunkcode = 3715430491.44292;
	pJunkcode = 4336807614.38575;
	if (pJunkcode = 6123665893.98983)
		pJunkcode = 8762961339.01308;
	pJunkcode = 8496321248.70719;
	pJunkcode = 9824190516.21478;
	if (pJunkcode = 8629653726.89217)
		pJunkcode = 115668406.478377;
	pJunkcode = 8876728855.72495;
	if (pJunkcode = 7930425995.73019)
		pJunkcode = 4970192231.38325;
	pJunkcode = 5505559336.00348;
	pJunkcode = 2471811241.80017;
	if (pJunkcode = 219450661.796608)
		pJunkcode = 2644602159.02773;
	pJunkcode = 3330901967.59748;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3649() {
	float pJunkcode = 4186007026.53547;
	pJunkcode = 4289947847.24957;
	if (pJunkcode = 7594163659.41419)
		pJunkcode = 794970303.919912;
	pJunkcode = 3044351375.31571;
	pJunkcode = 5145695489.05503;
	if (pJunkcode = 7254345285.01602)
		pJunkcode = 6243589472.84879;
	pJunkcode = 8283186471.965;
	if (pJunkcode = 8313140007.02441)
		pJunkcode = 7618517359.17183;
	pJunkcode = 4089235257.40276;
	pJunkcode = 7028628123.85609;
	if (pJunkcode = 7380905227.55374)
		pJunkcode = 837787710.502002;
	pJunkcode = 5912638967.86507;
	if (pJunkcode = 1865117746.22492)
		pJunkcode = 3658258837.38266;
	pJunkcode = 723871439.779466;
	pJunkcode = 5183737561.2787;
	if (pJunkcode = 1122109109.03325)
		pJunkcode = 4769044706.60201;
	pJunkcode = 5129430789.79001;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3648() {
	float pJunkcode = 3101356927.32753;
	pJunkcode = 2784582004.35766;
	if (pJunkcode = 1274388200.92322)
		pJunkcode = 5747352090.94243;
	pJunkcode = 9664474093.9888;
	pJunkcode = 8204755017.63448;
	if (pJunkcode = 8624039535.39648)
		pJunkcode = 4877623700.18568;
	pJunkcode = 2097759591.67961;
	if (pJunkcode = 3369433382.07632)
		pJunkcode = 2761273401.05902;
	pJunkcode = 5204013533.86115;
	pJunkcode = 8168417998.38615;
	if (pJunkcode = 9373520934.79459)
		pJunkcode = 2732726861.47882;
	pJunkcode = 4002309429.85638;
	if (pJunkcode = 8673255438.93726)
		pJunkcode = 7592500846.15181;
	pJunkcode = 8635613919.8907;
	pJunkcode = 9163409196.242;
	if (pJunkcode = 3554643224.27456)
		pJunkcode = 4139037275.9597;
	pJunkcode = 8349622031.74752;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3647() {
	float pJunkcode = 5781122285.11533;
	pJunkcode = 5876405170.72205;
	if (pJunkcode = 6237359487.71159)
		pJunkcode = 3629983924.62825;
	pJunkcode = 4195096404.15256;
	pJunkcode = 8472520467.30451;
	if (pJunkcode = 516274025.845482)
		pJunkcode = 3438162170.38092;
	pJunkcode = 3786202923.30044;
	if (pJunkcode = 4691797105.09328)
		pJunkcode = 6338406480.63796;
	pJunkcode = 7913870002.63159;
	pJunkcode = 3273101877.31819;
	if (pJunkcode = 5171148031.35996)
		pJunkcode = 6745936474.63817;
	pJunkcode = 1378288653.86401;
	if (pJunkcode = 7138860910.93321)
		pJunkcode = 8327878903.17748;
	pJunkcode = 7629595726.22309;
	pJunkcode = 3849215101.26172;
	if (pJunkcode = 8589073469.80414)
		pJunkcode = 7006291213.23347;
	pJunkcode = 1092734622.65062;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3646() {
	float pJunkcode = 6625107996.89029;
	pJunkcode = 4010127681.28596;
	if (pJunkcode = 2890251330.84331)
		pJunkcode = 4864798215.06734;
	pJunkcode = 8120173484.84458;
	pJunkcode = 2458728988.95913;
	if (pJunkcode = 3730916196.05881)
		pJunkcode = 7744563258.69841;
	pJunkcode = 4002541922.89893;
	if (pJunkcode = 5915407368.71105)
		pJunkcode = 7787645151.26186;
	pJunkcode = 1772946492.31007;
	pJunkcode = 4900670807.01853;
	if (pJunkcode = 958203092.611417)
		pJunkcode = 1621433831.65333;
	pJunkcode = 1295780291.91396;
	if (pJunkcode = 213970220.854935)
		pJunkcode = 9410516853.51872;
	pJunkcode = 6442804983.61291;
	pJunkcode = 3255851050.0216;
	if (pJunkcode = 9560244985.25467)
		pJunkcode = 1088283925.46883;
	pJunkcode = 667776998.516017;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3645() {
	float pJunkcode = 864161365.859742;
	pJunkcode = 4525117602.85506;
	if (pJunkcode = 6228829542.98571)
		pJunkcode = 7487165316.30871;
	pJunkcode = 2822325914.31245;
	pJunkcode = 392526053.316391;
	if (pJunkcode = 3392327173.79181)
		pJunkcode = 5033933266.74424;
	pJunkcode = 2663694777.04097;
	if (pJunkcode = 7292399396.79188)
		pJunkcode = 3658000807.73049;
	pJunkcode = 5720886740.80926;
	pJunkcode = 9379990097.47527;
	if (pJunkcode = 6736983913.92401)
		pJunkcode = 7466911525.27987;
	pJunkcode = 5265628644.14259;
	if (pJunkcode = 309348254.080996)
		pJunkcode = 6060649289.96227;
	pJunkcode = 5944368836.28856;
	pJunkcode = 4210208412.3375;
	if (pJunkcode = 9584529807.41178)
		pJunkcode = 6333421994.27561;
	pJunkcode = 3136803413.194;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3644() {
	float pJunkcode = 662007857.448191;
	pJunkcode = 7087547324.38753;
	if (pJunkcode = 2242562603.61705)
		pJunkcode = 1611308165.47242;
	pJunkcode = 7725905920.20551;
	pJunkcode = 3976661979.98383;
	if (pJunkcode = 7046231581.27765)
		pJunkcode = 7695447406.14128;
	pJunkcode = 1754147985.35303;
	if (pJunkcode = 913139314.2057)
		pJunkcode = 5856484370.74979;
	pJunkcode = 8835337287.41763;
	pJunkcode = 151387718.236451;
	if (pJunkcode = 1349164001.57685)
		pJunkcode = 2751356940.43255;
	pJunkcode = 4377914761.95136;
	if (pJunkcode = 6621719371.67996)
		pJunkcode = 2798155226.92379;
	pJunkcode = 1126059479.11995;
	pJunkcode = 9258608098.3471;
	if (pJunkcode = 7446974650.84071)
		pJunkcode = 1104282252.84313;
	pJunkcode = 3075075078.50281;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3643() {
	float pJunkcode = 4058538832.38109;
	pJunkcode = 6155994340.72919;
	if (pJunkcode = 5715607160.85522)
		pJunkcode = 4519398165.97599;
	pJunkcode = 9473994741.66116;
	pJunkcode = 9857486958.31651;
	if (pJunkcode = 9289411035.33819)
		pJunkcode = 6890853095.15207;
	pJunkcode = 234641037.664205;
	if (pJunkcode = 3516203488.27316)
		pJunkcode = 9720168920.37325;
	pJunkcode = 3678842762.66465;
	pJunkcode = 6298105249.65958;
	if (pJunkcode = 8787519415.4659)
		pJunkcode = 5109806580.4955;
	pJunkcode = 4601230548.16502;
	if (pJunkcode = 2825501520.24051)
		pJunkcode = 719017993.849545;
	pJunkcode = 4194153679.97553;
	pJunkcode = 646777653.968975;
	if (pJunkcode = 9893804645.82982)
		pJunkcode = 1046250395.94564;
	pJunkcode = 2667106941.19398;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3642() {
	float pJunkcode = 2063379540.98725;
	pJunkcode = 3216774873.1637;
	if (pJunkcode = 1071217363.15756)
		pJunkcode = 2380575478.08659;
	pJunkcode = 1383055808.9637;
	pJunkcode = 617555062.450981;
	if (pJunkcode = 8290727829.9923)
		pJunkcode = 4256808038.06869;
	pJunkcode = 5271739837.94503;
	if (pJunkcode = 635182640.886011)
		pJunkcode = 626703524.909651;
	pJunkcode = 90288480.9967047;
	pJunkcode = 210220735.03869;
	if (pJunkcode = 1776332365.48971)
		pJunkcode = 2166800151.06814;
	pJunkcode = 4200789214.78549;
	if (pJunkcode = 2347380788.94351)
		pJunkcode = 1543315435.94731;
	pJunkcode = 4349753648.69263;
	pJunkcode = 1629571761.01642;
	if (pJunkcode = 989040807.167964)
		pJunkcode = 57525220.3936306;
	pJunkcode = 2676811227.70497;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3641() {
	float pJunkcode = 401148905.853217;
	pJunkcode = 75436784.1814279;
	if (pJunkcode = 4672031201.14263)
		pJunkcode = 8153408530.11956;
	pJunkcode = 8005259409.75046;
	pJunkcode = 8866014466.85076;
	if (pJunkcode = 8643136968.29928)
		pJunkcode = 2486988043.77699;
	pJunkcode = 1599398566.4112;
	if (pJunkcode = 6386669878.88419)
		pJunkcode = 5111485850.06321;
	pJunkcode = 1120561324.60005;
	pJunkcode = 4907859136.78443;
	if (pJunkcode = 8124481411.70341)
		pJunkcode = 6697171200.98856;
	pJunkcode = 1203473275.80844;
	if (pJunkcode = 5386904098.70538)
		pJunkcode = 7914237834.11728;
	pJunkcode = 3989573101.68483;
	pJunkcode = 3593802746.6845;
	if (pJunkcode = 1347281908.27882)
		pJunkcode = 2172320347.44465;
	pJunkcode = 7823391212.66508;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3640() {
	float pJunkcode = 7681997023.79432;
	pJunkcode = 2429193965.12481;
	if (pJunkcode = 4963083801.31032)
		pJunkcode = 8501125188.35242;
	pJunkcode = 5805165382.58719;
	pJunkcode = 7825203331.74123;
	if (pJunkcode = 5401273487.0089)
		pJunkcode = 4645685126.62712;
	pJunkcode = 6413023608.7947;
	if (pJunkcode = 2269031401.93757)
		pJunkcode = 9481506835.4975;
	pJunkcode = 4842670093.23649;
	pJunkcode = 8256078303.80778;
	if (pJunkcode = 4468518764.65758)
		pJunkcode = 9376803893.20229;
	pJunkcode = 6046442066.58038;
	if (pJunkcode = 7411437843.88588)
		pJunkcode = 696974872.980055;
	pJunkcode = 1294228216.45492;
	pJunkcode = 3140629569.37448;
	if (pJunkcode = 8109392830.70356)
		pJunkcode = 7016486285.53519;
	pJunkcode = 3759640975.02752;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3639() {
	float pJunkcode = 2909195504.50755;
	pJunkcode = 2151481137.55632;
	if (pJunkcode = 812353647.78045)
		pJunkcode = 3641223925.75257;
	pJunkcode = 7203870539.30964;
	pJunkcode = 3369133434.01425;
	if (pJunkcode = 436915097.592582)
		pJunkcode = 8150280074.25363;
	pJunkcode = 8041049482.69012;
	if (pJunkcode = 3720848455.23446)
		pJunkcode = 3684538693.19701;
	pJunkcode = 8660503536.8797;
	pJunkcode = 2226437375.49777;
	if (pJunkcode = 937739518.6139)
		pJunkcode = 5124270334.11522;
	pJunkcode = 7523246415.3294;
	if (pJunkcode = 6432050133.83407)
		pJunkcode = 9648569044.62247;
	pJunkcode = 6482760310.87634;
	pJunkcode = 1118089735.61873;
	if (pJunkcode = 1774055248.68704)
		pJunkcode = 6079221904.55951;
	pJunkcode = 236579691.944884;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3638() {
	float pJunkcode = 6136416839.12302;
	pJunkcode = 9456720396.37644;
	if (pJunkcode = 3232657503.49155)
		pJunkcode = 3604412304.60972;
	pJunkcode = 8388814108.21251;
	pJunkcode = 6433954017.53414;
	if (pJunkcode = 803742208.487642)
		pJunkcode = 1211086503.28011;
	pJunkcode = 290579563.118014;
	if (pJunkcode = 7028101550.02502)
		pJunkcode = 498842586.639847;
	pJunkcode = 1567727314.84438;
	pJunkcode = 2763405889.56877;
	if (pJunkcode = 2120358187.35895)
		pJunkcode = 5321676803.79352;
	pJunkcode = 8325585800.38268;
	if (pJunkcode = 2011568488.80648)
		pJunkcode = 9078686.37941863;
	pJunkcode = 6719463776.44337;
	pJunkcode = 222839175.249341;
	if (pJunkcode = 9714752416.41524)
		pJunkcode = 9204713172.20882;
	pJunkcode = 2394659882.20387;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3637() {
	float pJunkcode = 2788686527.74366;
	pJunkcode = 8199484666.99486;
	if (pJunkcode = 4665516066.51109)
		pJunkcode = 9924073232.52411;
	pJunkcode = 6759969307.2836;
	pJunkcode = 4726497355.25458;
	if (pJunkcode = 1715314087.31176)
		pJunkcode = 4199962313.3822;
	pJunkcode = 2159125654.82529;
	if (pJunkcode = 6497342641.62877)
		pJunkcode = 8168903722.4893;
	pJunkcode = 7493077278.92172;
	pJunkcode = 9808411161.54276;
	if (pJunkcode = 271047544.523242)
		pJunkcode = 4193669986.13529;
	pJunkcode = 3612068572.72376;
	if (pJunkcode = 9881808695.40302)
		pJunkcode = 9869621441.59093;
	pJunkcode = 2499015123.34955;
	pJunkcode = 9609936559.37194;
	if (pJunkcode = 983138716.475522)
		pJunkcode = 6929074026.08758;
	pJunkcode = 6000182756.68457;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3636() {
	float pJunkcode = 3311088871.6756;
	pJunkcode = 3413351379.74636;
	if (pJunkcode = 7527169397.82137)
		pJunkcode = 836898182.938456;
	pJunkcode = 6882980522.54824;
	pJunkcode = 897546827.539556;
	if (pJunkcode = 2239399959.2413)
		pJunkcode = 2200838929.00075;
	pJunkcode = 4675303987.28123;
	if (pJunkcode = 8477799642.13728)
		pJunkcode = 3389535853.32647;
	pJunkcode = 5624548381.58553;
	pJunkcode = 1238845503.41201;
	if (pJunkcode = 5619537203.78511)
		pJunkcode = 3631851927.15664;
	pJunkcode = 2353811879.53806;
	if (pJunkcode = 5684523295.54715)
		pJunkcode = 4970265826.08544;
	pJunkcode = 4233872339.58472;
	pJunkcode = 5031835404.46154;
	if (pJunkcode = 4388686459.18709)
		pJunkcode = 8700337951.74242;
	pJunkcode = 8287750649.66768;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3635() {
	float pJunkcode = 2950471643.52099;
	pJunkcode = 2285142103.89826;
	if (pJunkcode = 7326077556.26002)
		pJunkcode = 1155915405.56473;
	pJunkcode = 5189905623.39715;
	pJunkcode = 9429916411.42834;
	if (pJunkcode = 9149381797.80945)
		pJunkcode = 4809253041.41086;
	pJunkcode = 9352703817.9851;
	if (pJunkcode = 366008628.802221)
		pJunkcode = 520153553.260685;
	pJunkcode = 2015219178.58368;
	pJunkcode = 182950616.16189;
	if (pJunkcode = 8790832688.12026)
		pJunkcode = 6637849151.59281;
	pJunkcode = 6963201895.77505;
	if (pJunkcode = 2738570495.96613)
		pJunkcode = 3215643257.70273;
	pJunkcode = 5076730480.12654;
	pJunkcode = 3414426892.89647;
	if (pJunkcode = 4520730477.11649)
		pJunkcode = 2888020814.62795;
	pJunkcode = 8871640406.86324;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3634() {
	float pJunkcode = 1233200024.44731;
	pJunkcode = 422031100.50512;
	if (pJunkcode = 78617104.8828812)
		pJunkcode = 4292329343.31428;
	pJunkcode = 8425150196.14758;
	pJunkcode = 7536253324.9276;
	if (pJunkcode = 5482857182.94604)
		pJunkcode = 9860803264.72382;
	pJunkcode = 3946935890.2491;
	if (pJunkcode = 762730331.940378)
		pJunkcode = 9363883204.58581;
	pJunkcode = 8530271321.37995;
	pJunkcode = 1828961776.60287;
	if (pJunkcode = 736058574.849112)
		pJunkcode = 2208759983.44467;
	pJunkcode = 7914992712.4095;
	if (pJunkcode = 6820576294.24252)
		pJunkcode = 8738835908.79235;
	pJunkcode = 9427204841.64584;
	pJunkcode = 2323641951.51411;
	if (pJunkcode = 7508348048.07832)
		pJunkcode = 1503407961.88752;
	pJunkcode = 1096836938.82419;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3633() {
	float pJunkcode = 323755132.399846;
	pJunkcode = 2846233878.23009;
	if (pJunkcode = 4176082344.74119)
		pJunkcode = 1470403954.47812;
	pJunkcode = 5351385294.0359;
	pJunkcode = 9357301089.90452;
	if (pJunkcode = 3992963552.26914)
		pJunkcode = 7446643443.24903;
	pJunkcode = 6729632494.90807;
	if (pJunkcode = 5058314598.56634)
		pJunkcode = 2116695960.93873;
	pJunkcode = 3999110757.62711;
	pJunkcode = 4472270439.05999;
	if (pJunkcode = 9195532481.65475)
		pJunkcode = 8607820363.38024;
	pJunkcode = 2322733571.84188;
	if (pJunkcode = 6454433726.53391)
		pJunkcode = 4528490430.66419;
	pJunkcode = 9035731298.28057;
	pJunkcode = 5880636953.40864;
	if (pJunkcode = 5108002161.05174)
		pJunkcode = 7114842644.60118;
	pJunkcode = 3381973096.04871;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3632() {
	float pJunkcode = 3535999161.6522;
	pJunkcode = 3672091344.54559;
	if (pJunkcode = 7445771661.85968)
		pJunkcode = 8581667287.24016;
	pJunkcode = 5103578388.13633;
	pJunkcode = 2158792812.77883;
	if (pJunkcode = 3301505888.71689)
		pJunkcode = 1267501181.26502;
	pJunkcode = 3844249473.99291;
	if (pJunkcode = 4556545823.56978)
		pJunkcode = 435777519.398456;
	pJunkcode = 226170857.638587;
	pJunkcode = 8240341661.4902;
	if (pJunkcode = 4593061182.3228)
		pJunkcode = 8517966954.92525;
	pJunkcode = 409448844.142953;
	if (pJunkcode = 269417918.608926)
		pJunkcode = 2030520296.91118;
	pJunkcode = 2804893306.30867;
	pJunkcode = 4835109613.01966;
	if (pJunkcode = 1696005977.37956)
		pJunkcode = 7135419005.2912;
	pJunkcode = 2234717546.66353;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3631() {
	float pJunkcode = 1170866145.21706;
	pJunkcode = 6517151067.01465;
	if (pJunkcode = 2206168282.99204)
		pJunkcode = 8871181071.52136;
	pJunkcode = 6032218489.33762;
	pJunkcode = 5200718240.2093;
	if (pJunkcode = 2495892707.05012)
		pJunkcode = 4348657697.873;
	pJunkcode = 4956066376.841;
	if (pJunkcode = 3911929228.7117)
		pJunkcode = 2287900479.15699;
	pJunkcode = 7665793348.83075;
	pJunkcode = 795673622.381711;
	if (pJunkcode = 6838852285.56603)
		pJunkcode = 9953110199.33943;
	pJunkcode = 9536122062.5949;
	if (pJunkcode = 4994649295.27148)
		pJunkcode = 7215572797.7712;
	pJunkcode = 1710420532.04796;
	pJunkcode = 4438069407.73099;
	if (pJunkcode = 1178997271.33677)
		pJunkcode = 5257358386.80649;
	pJunkcode = 2291095828.92619;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3630() {
	float pJunkcode = 397419063.761935;
	pJunkcode = 8874362453.91993;
	if (pJunkcode = 4785290791.24321)
		pJunkcode = 4431228211.01354;
	pJunkcode = 1019130956.56967;
	pJunkcode = 2600433873.247;
	if (pJunkcode = 4757526289.37822)
		pJunkcode = 3293823695.16993;
	pJunkcode = 7485103272.33404;
	if (pJunkcode = 8560401135.41345)
		pJunkcode = 7643762202.32151;
	pJunkcode = 6586796537.64921;
	pJunkcode = 8017881496.05253;
	if (pJunkcode = 8653175931.54386)
		pJunkcode = 6143520018.91744;
	pJunkcode = 9931143951.39481;
	if (pJunkcode = 9757937382.07389)
		pJunkcode = 3507840802.75244;
	pJunkcode = 3407031508.90975;
	pJunkcode = 8435394281.13795;
	if (pJunkcode = 3679477110.3946)
		pJunkcode = 6722760080.5701;
	pJunkcode = 6532503274.85938;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3629() {
	float pJunkcode = 6674804390.55369;
	pJunkcode = 6754555150.23501;
	if (pJunkcode = 9392205567.75089)
		pJunkcode = 4089398800.66801;
	pJunkcode = 5019589894.67668;
	pJunkcode = 6296684638.38049;
	if (pJunkcode = 7620102396.13548)
		pJunkcode = 7487127735.49033;
	pJunkcode = 7384836400.94584;
	if (pJunkcode = 286248122.927616)
		pJunkcode = 9549565911.77934;
	pJunkcode = 1216218113.85492;
	pJunkcode = 9646643386.81835;
	if (pJunkcode = 9087156598.18023)
		pJunkcode = 1871692554.75368;
	pJunkcode = 1313229821.8797;
	if (pJunkcode = 5080892523.74784)
		pJunkcode = 3065169963.60015;
	pJunkcode = 7709715493.17309;
	pJunkcode = 9175089955.33693;
	if (pJunkcode = 7231567413.86216)
		pJunkcode = 5827182161.26109;
	pJunkcode = 9280579757.22177;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3628() {
	float pJunkcode = 3826719243.4048;
	pJunkcode = 6337021410.60121;
	if (pJunkcode = 9211977892.94498)
		pJunkcode = 1046694576.49248;
	pJunkcode = 3987477905.28634;
	pJunkcode = 9722314917.58185;
	if (pJunkcode = 99159761.3459811)
		pJunkcode = 5479995736.92041;
	pJunkcode = 2267229199.01937;
	if (pJunkcode = 4193550205.28019)
		pJunkcode = 806410141.587278;
	pJunkcode = 7528622151.04939;
	pJunkcode = 5149881621.09383;
	if (pJunkcode = 7870162165.11209)
		pJunkcode = 3793057237.56517;
	pJunkcode = 1011097622.99896;
	if (pJunkcode = 7257326099.79064)
		pJunkcode = 7758059987.84982;
	pJunkcode = 6355478996.36926;
	pJunkcode = 2653565236.59689;
	if (pJunkcode = 2410392033.26324)
		pJunkcode = 1436419548.94199;
	pJunkcode = 2037432441.07288;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3627() {
	float pJunkcode = 666875703.809415;
	pJunkcode = 2227723481.85909;
	if (pJunkcode = 8527384394.62348)
		pJunkcode = 2095552151.07859;
	pJunkcode = 3764644350.7511;
	pJunkcode = 659120341.636089;
	if (pJunkcode = 9152666493.11638)
		pJunkcode = 7270888265.34007;
	pJunkcode = 2352313794.63706;
	if (pJunkcode = 1410641307.76331)
		pJunkcode = 5800951218.01922;
	pJunkcode = 4989414038.84382;
	pJunkcode = 3532748515.15811;
	if (pJunkcode = 4485731009.97403)
		pJunkcode = 1260017722.98318;
	pJunkcode = 3803675838.54216;
	if (pJunkcode = 6643395289.70022)
		pJunkcode = 2211026567.09436;
	pJunkcode = 1079341844.35715;
	pJunkcode = 9764092629.32186;
	if (pJunkcode = 2968778903.68724)
		pJunkcode = 4637234085.08903;
	pJunkcode = 6014819895.22243;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3626() {
	float pJunkcode = 9740126851.80524;
	pJunkcode = 6677211635.10179;
	if (pJunkcode = 9146595790.48626)
		pJunkcode = 4926021011.96575;
	pJunkcode = 238377943.766573;
	pJunkcode = 490396543.925679;
	if (pJunkcode = 3480288721.80093)
		pJunkcode = 9014690119.14183;
	pJunkcode = 6463276166.43977;
	if (pJunkcode = 2182166701.13709)
		pJunkcode = 1337997320.41731;
	pJunkcode = 2973781899.90066;
	pJunkcode = 6342326652.89217;
	if (pJunkcode = 2074405617.12717)
		pJunkcode = 4918139438.21925;
	pJunkcode = 701952774.95765;
	if (pJunkcode = 6447234686.6655)
		pJunkcode = 5891878989.45244;
	pJunkcode = 2826495590.04592;
	pJunkcode = 1653282610.15403;
	if (pJunkcode = 9665523968.08483)
		pJunkcode = 5472088093.38745;
	pJunkcode = 4623430183.21292;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3625() {
	float pJunkcode = 7496622679.07817;
	pJunkcode = 8183188016.70013;
	if (pJunkcode = 6242306884.52331)
		pJunkcode = 6379659900.26439;
	pJunkcode = 9765176137.79632;
	pJunkcode = 6523928266.21307;
	if (pJunkcode = 4168216458.96658)
		pJunkcode = 597353294.580914;
	pJunkcode = 8536488931.69219;
	if (pJunkcode = 3391156403.59305)
		pJunkcode = 6562143169.22349;
	pJunkcode = 4962258911.54368;
	pJunkcode = 9225115973.35947;
	if (pJunkcode = 3372829802.37864)
		pJunkcode = 8209664402.89544;
	pJunkcode = 9536137536.36474;
	if (pJunkcode = 7205548598.17011)
		pJunkcode = 1473354720.43036;
	pJunkcode = 6552970716.13521;
	pJunkcode = 8000744875.70117;
	if (pJunkcode = 7930036695.14556)
		pJunkcode = 1774284691.67866;
	pJunkcode = 7521058043.3674;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3624() {
	float pJunkcode = 6354565676.08017;
	pJunkcode = 6151949253.75552;
	if (pJunkcode = 9621038337.31031)
		pJunkcode = 4471921150.02134;
	pJunkcode = 6692360951.69598;
	pJunkcode = 6742887166.42975;
	if (pJunkcode = 5691986011.15437)
		pJunkcode = 2875653756.82107;
	pJunkcode = 2008087789.31058;
	if (pJunkcode = 8466558497.82593)
		pJunkcode = 657337810.382251;
	pJunkcode = 1147123694.29221;
	pJunkcode = 8536202012.70264;
	if (pJunkcode = 990823277.886441)
		pJunkcode = 9074531089.30193;
	pJunkcode = 654143650.365734;
	if (pJunkcode = 1248946962.43299)
		pJunkcode = 8725051850.87356;
	pJunkcode = 2487527655.99372;
	pJunkcode = 1766588102.35697;
	if (pJunkcode = 9316669112.33452)
		pJunkcode = 5774973405.72178;
	pJunkcode = 2300022594.5913;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3623() {
	float pJunkcode = 3889123080.55895;
	pJunkcode = 3473595238.35157;
	if (pJunkcode = 716866341.241158)
		pJunkcode = 8276148179.09378;
	pJunkcode = 9722728828.11942;
	pJunkcode = 5911278179.49332;
	if (pJunkcode = 4040436053.68276)
		pJunkcode = 3139091424.03353;
	pJunkcode = 1745423764.02427;
	if (pJunkcode = 8053950760.40995)
		pJunkcode = 4137965418.21414;
	pJunkcode = 9429341937.82796;
	pJunkcode = 3861816775.50538;
	if (pJunkcode = 1012973266.95266)
		pJunkcode = 5838936002.83591;
	pJunkcode = 6620339589.17425;
	if (pJunkcode = 17512074.8746492)
		pJunkcode = 5941435204.17204;
	pJunkcode = 7194709369.08152;
	pJunkcode = 1940548744.38415;
	if (pJunkcode = 550616104.567901)
		pJunkcode = 6498619064.66046;
	pJunkcode = 4389817096.76914;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3622() {
	float pJunkcode = 6981548643.62359;
	pJunkcode = 7749118628.50592;
	if (pJunkcode = 7425106323.50066)
		pJunkcode = 1375036615.05658;
	pJunkcode = 9211070202.44456;
	pJunkcode = 7492596257.95054;
	if (pJunkcode = 9962785144.82612)
		pJunkcode = 4823410583.88683;
	pJunkcode = 4466166577.0212;
	if (pJunkcode = 9530185052.31173)
		pJunkcode = 9598933116.47256;
	pJunkcode = 8526982870.55229;
	pJunkcode = 8512844615.57396;
	if (pJunkcode = 9983947178.1585)
		pJunkcode = 5063019453.46542;
	pJunkcode = 4499469496.40666;
	if (pJunkcode = 2173441366.88899)
		pJunkcode = 3817671111.32377;
	pJunkcode = 5744358656.87312;
	pJunkcode = 7339843539.71669;
	if (pJunkcode = 2161337887.03304)
		pJunkcode = 5647103931.96594;
	pJunkcode = 5335535760.25307;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3621() {
	float pJunkcode = 4990795486.07198;
	pJunkcode = 3763637086.56506;
	if (pJunkcode = 2938547597.46281)
		pJunkcode = 9934163508.80109;
	pJunkcode = 4172802753.03093;
	pJunkcode = 4551118535.00421;
	if (pJunkcode = 9198856829.95206)
		pJunkcode = 3945343665.87536;
	pJunkcode = 7756230285.5793;
	if (pJunkcode = 2778737326.79949)
		pJunkcode = 5481324259.94422;
	pJunkcode = 858724847.979755;
	pJunkcode = 942574990.197878;
	if (pJunkcode = 5113892731.39914)
		pJunkcode = 1272060402.05287;
	pJunkcode = 1817211408.82651;
	if (pJunkcode = 7692166566.47125)
		pJunkcode = 4607022054.72081;
	pJunkcode = 7302151275.73881;
	pJunkcode = 1548023552.77109;
	if (pJunkcode = 613075305.967155)
		pJunkcode = 3159399845.89829;
	pJunkcode = 9381920025.35069;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3620() {
	float pJunkcode = 5826800655.76845;
	pJunkcode = 5099602185.74804;
	if (pJunkcode = 7002019360.79382)
		pJunkcode = 8601763823.69655;
	pJunkcode = 4977974808.85836;
	pJunkcode = 6719585242.97547;
	if (pJunkcode = 3214469588.79145)
		pJunkcode = 1949316225.21389;
	pJunkcode = 6996922057.47127;
	if (pJunkcode = 3564784722.78092)
		pJunkcode = 8363348345.91583;
	pJunkcode = 5211963207.56159;
	pJunkcode = 8406502943.7773;
	if (pJunkcode = 7510725807.24856)
		pJunkcode = 1831520825.21257;
	pJunkcode = 7858653866.60689;
	if (pJunkcode = 3786186425.08601)
		pJunkcode = 6974542495.02958;
	pJunkcode = 7384002954.73861;
	pJunkcode = 7780481950.65603;
	if (pJunkcode = 9821794812.96627)
		pJunkcode = 3550561718.29581;
	pJunkcode = 7731367597.51325;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3619() {
	float pJunkcode = 4955410902.26972;
	pJunkcode = 2010767373.13911;
	if (pJunkcode = 2086222485.32695)
		pJunkcode = 9389959617.34651;
	pJunkcode = 7613318153.88052;
	pJunkcode = 2637907059.74986;
	if (pJunkcode = 777235813.050768)
		pJunkcode = 5407911985.72331;
	pJunkcode = 7755852046.66792;
	if (pJunkcode = 1069327761.14635)
		pJunkcode = 2354750107.80033;
	pJunkcode = 1252709579.76622;
	pJunkcode = 9169370364.79118;
	if (pJunkcode = 9576846393.84446)
		pJunkcode = 2851755508.82903;
	pJunkcode = 2192367107.53551;
	if (pJunkcode = 2495730174.25975)
		pJunkcode = 5892670389.19421;
	pJunkcode = 9165670536.59802;
	pJunkcode = 7976548751.81907;
	if (pJunkcode = 5618943789.60128)
		pJunkcode = 4431722081.77944;
	pJunkcode = 4050973867.8909;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3618() {
	float pJunkcode = 3773669118.51069;
	pJunkcode = 2221857637.87878;
	if (pJunkcode = 9602957700.58477)
		pJunkcode = 3747891498.98361;
	pJunkcode = 7827990619.62015;
	pJunkcode = 2293236771.43208;
	if (pJunkcode = 2237213722.31496)
		pJunkcode = 1406074885.59567;
	pJunkcode = 104969614.773024;
	if (pJunkcode = 7956925741.59038)
		pJunkcode = 5182562944.75618;
	pJunkcode = 3979475984.91194;
	pJunkcode = 5007026085.30194;
	if (pJunkcode = 9184393821.38295)
		pJunkcode = 7031425164.91314;
	pJunkcode = 4556652736.0039;
	if (pJunkcode = 7671084502.68034)
		pJunkcode = 7735096475.69531;
	pJunkcode = 586600258.534657;
	pJunkcode = 7399431963.33143;
	if (pJunkcode = 3704747159.53771)
		pJunkcode = 7377967292.76995;
	pJunkcode = 636029868.201229;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3617() {
	float pJunkcode = 1983635430.88261;
	pJunkcode = 7889176394.48813;
	if (pJunkcode = 9310882772.23904)
		pJunkcode = 5967138919.33527;
	pJunkcode = 1238816381.24633;
	pJunkcode = 2170129318.91158;
	if (pJunkcode = 4743242486.43513)
		pJunkcode = 5355757264.97943;
	pJunkcode = 3335703481.17167;
	if (pJunkcode = 6365474851.45125)
		pJunkcode = 7767699522.47804;
	pJunkcode = 4421416234.42186;
	pJunkcode = 8396516881.45425;
	if (pJunkcode = 3743577476.67948)
		pJunkcode = 8037815448.49633;
	pJunkcode = 4971598113.94311;
	if (pJunkcode = 5049296899.4744)
		pJunkcode = 9832367609.62879;
	pJunkcode = 3573981937.16053;
	pJunkcode = 4209827728.72881;
	if (pJunkcode = 6326858398.75852)
		pJunkcode = 7350075398.89223;
	pJunkcode = 1966353209.01598;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3616() {
	float pJunkcode = 7542860366.20833;
	pJunkcode = 4424268083.26433;
	if (pJunkcode = 5561050062.75842)
		pJunkcode = 2681000218.87548;
	pJunkcode = 6683653307.74328;
	pJunkcode = 9043435345.0932;
	if (pJunkcode = 4735040589.8207)
		pJunkcode = 913877950.39465;
	pJunkcode = 302343916.891279;
	if (pJunkcode = 2897834651.51787)
		pJunkcode = 4356631618.31421;
	pJunkcode = 1764751065.99452;
	pJunkcode = 6122543850.1009;
	if (pJunkcode = 2892037768.95628)
		pJunkcode = 3636630132.28818;
	pJunkcode = 1852593489.72439;
	if (pJunkcode = 5835166868.63814)
		pJunkcode = 4681754844.99976;
	pJunkcode = 8915545098.39507;
	pJunkcode = 3091027999.69906;
	if (pJunkcode = 2226715176.93048)
		pJunkcode = 8770809117.72961;
	pJunkcode = 6846239052.5768;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3615() {
	float pJunkcode = 7605674977.70265;
	pJunkcode = 8480432022.35317;
	if (pJunkcode = 8983137512.22793)
		pJunkcode = 3945707080.6079;
	pJunkcode = 4320598442.34566;
	pJunkcode = 7277274832.17589;
	if (pJunkcode = 8158053448.02543)
		pJunkcode = 531914477.472865;
	pJunkcode = 5561088055.68292;
	if (pJunkcode = 8652917096.30571)
		pJunkcode = 5339910739.33843;
	pJunkcode = 8736083130.72633;
	pJunkcode = 1522187407.19039;
	if (pJunkcode = 8242712715.20497)
		pJunkcode = 1657284873.01565;
	pJunkcode = 7559958958.71095;
	if (pJunkcode = 765528467.29051)
		pJunkcode = 506490887.368937;
	pJunkcode = 3689356656.09167;
	pJunkcode = 8679693758.85101;
	if (pJunkcode = 9084693195.44247)
		pJunkcode = 5858683832.32444;
	pJunkcode = 6680992766.94265;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3614() {
	float pJunkcode = 7942400672.63382;
	pJunkcode = 3702571141.44612;
	if (pJunkcode = 8909517841.52859)
		pJunkcode = 3356071420.93866;
	pJunkcode = 857363682.426141;
	pJunkcode = 2804265307.80688;
	if (pJunkcode = 8420738469.76921)
		pJunkcode = 5174716011.20643;
	pJunkcode = 1217819919.11384;
	if (pJunkcode = 7357842805.27431)
		pJunkcode = 8558098156.04152;
	pJunkcode = 9640178992.46157;
	pJunkcode = 3015729592.60884;
	if (pJunkcode = 1231427712.02823)
		pJunkcode = 8665553343.22592;
	pJunkcode = 651699877.383243;
	if (pJunkcode = 4880156974.70086)
		pJunkcode = 502422824.897117;
	pJunkcode = 6702826912.75772;
	pJunkcode = 6186510742.89745;
	if (pJunkcode = 4281031058.27778)
		pJunkcode = 2686463088.2642;
	pJunkcode = 11426277.98485;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3613() {
	float pJunkcode = 3453350266.85931;
	pJunkcode = 2086747359.51132;
	if (pJunkcode = 5416967298.22959)
		pJunkcode = 1612494290.69857;
	pJunkcode = 3383321029.96917;
	pJunkcode = 9054725881.60337;
	if (pJunkcode = 4309965764.56872)
		pJunkcode = 4200383346.13793;
	pJunkcode = 1082774841.9878;
	if (pJunkcode = 5369323875.2214)
		pJunkcode = 6590823079.84756;
	pJunkcode = 6005851262.36966;
	pJunkcode = 5712050645.475;
	if (pJunkcode = 9897883069.16167)
		pJunkcode = 6515737789.8275;
	pJunkcode = 3006333535.86309;
	if (pJunkcode = 6608717100.0218)
		pJunkcode = 2330542198.1489;
	pJunkcode = 7684245854.28495;
	pJunkcode = 6590232406.053;
	if (pJunkcode = 9116023095.08792)
		pJunkcode = 7353776507.64112;
	pJunkcode = 5808425244.42601;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3612() {
	float pJunkcode = 7439668472.69472;
	pJunkcode = 4677642126.05706;
	if (pJunkcode = 433800294.779061)
		pJunkcode = 9173444098.96076;
	pJunkcode = 4358699359.59775;
	pJunkcode = 758658707.195334;
	if (pJunkcode = 7640354140.60343)
		pJunkcode = 7531605589.79912;
	pJunkcode = 3459185961.90107;
	if (pJunkcode = 260748730.909068)
		pJunkcode = 3376396115.17437;
	pJunkcode = 2069390448.64942;
	pJunkcode = 7298500609.24444;
	if (pJunkcode = 600831954.500774)
		pJunkcode = 1664934722.05304;
	pJunkcode = 7393666803.6278;
	if (pJunkcode = 5411321088.18397)
		pJunkcode = 254235670.777817;
	pJunkcode = 609492636.982274;
	pJunkcode = 5439153906.53324;
	if (pJunkcode = 3234628632.66565)
		pJunkcode = 6464154122.43604;
	pJunkcode = 3565737619.22704;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3611() {
	float pJunkcode = 5950969360.70474;
	pJunkcode = 3138535914.29944;
	if (pJunkcode = 2529530219.35097)
		pJunkcode = 8144623922.43895;
	pJunkcode = 2092517010.47316;
	pJunkcode = 3814251871.51826;
	if (pJunkcode = 1538476471.9386)
		pJunkcode = 8678381326.59208;
	pJunkcode = 7367986492.36683;
	if (pJunkcode = 6355153090.27831)
		pJunkcode = 7028338760.16924;
	pJunkcode = 8045957764.34487;
	pJunkcode = 5759322530.37072;
	if (pJunkcode = 3072868783.4171)
		pJunkcode = 7485469408.82959;
	pJunkcode = 4641937033.1242;
	if (pJunkcode = 4478166557.70614)
		pJunkcode = 4764467383.41264;
	pJunkcode = 1215496401.95621;
	pJunkcode = 1235634018.64055;
	if (pJunkcode = 5113820889.89416)
		pJunkcode = 7417150270.30438;
	pJunkcode = 4019108612.13033;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3610() {
	float pJunkcode = 9816692538.7592;
	pJunkcode = 2102091676.80444;
	if (pJunkcode = 1814150203.09118)
		pJunkcode = 1069912013.53597;
	pJunkcode = 3348806511.61157;
	pJunkcode = 9407292833.27076;
	if (pJunkcode = 3103179712.26881)
		pJunkcode = 7946902571.09044;
	pJunkcode = 3161476208.81435;
	if (pJunkcode = 6095012563.04624)
		pJunkcode = 7058825337.03031;
	pJunkcode = 6634975307.45939;
	pJunkcode = 3416025089.3165;
	if (pJunkcode = 599059388.298831)
		pJunkcode = 123709770.29768;
	pJunkcode = 3093269428.57755;
	if (pJunkcode = 2294106364.23526)
		pJunkcode = 8680629442.29979;
	pJunkcode = 4793314315.75699;
	pJunkcode = 4394353099.91738;
	if (pJunkcode = 379157191.147468)
		pJunkcode = 499188210.960813;
	pJunkcode = 9442979299.93839;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3609() {
	float pJunkcode = 5055838443.82175;
	pJunkcode = 872068562.36234;
	if (pJunkcode = 2573807384.96508)
		pJunkcode = 6435755859.56845;
	pJunkcode = 1321276999.42204;
	pJunkcode = 7361748071.85977;
	if (pJunkcode = 4175282078.89717)
		pJunkcode = 9445335934.59559;
	pJunkcode = 517904904.376134;
	if (pJunkcode = 5027804689.97979)
		pJunkcode = 1522878648.84747;
	pJunkcode = 1017412394.89547;
	pJunkcode = 897500082.598688;
	if (pJunkcode = 1824786210.31302)
		pJunkcode = 4174666478.50317;
	pJunkcode = 7288219024.9256;
	if (pJunkcode = 5584108054.31501)
		pJunkcode = 6181849932.13078;
	pJunkcode = 5345338877.72735;
	pJunkcode = 473640143.854663;
	if (pJunkcode = 8666594556.10861)
		pJunkcode = 9852917858.62325;
	pJunkcode = 4669119564.31706;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3608() {
	float pJunkcode = 4455908999.62995;
	pJunkcode = 6321371431.63916;
	if (pJunkcode = 2216999209.42291)
		pJunkcode = 4048059236.8932;
	pJunkcode = 8932189410.25852;
	pJunkcode = 6894557773.15903;
	if (pJunkcode = 6018549592.96015)
		pJunkcode = 3782375779.9279;
	pJunkcode = 2516455060.78869;
	if (pJunkcode = 5551001150.75993)
		pJunkcode = 945785855.646019;
	pJunkcode = 411951855.633713;
	pJunkcode = 1460360360.27836;
	if (pJunkcode = 2410487995.78169)
		pJunkcode = 3586268878.74411;
	pJunkcode = 9681376235.27208;
	if (pJunkcode = 594381965.231862)
		pJunkcode = 2135197199.68761;
	pJunkcode = 6896154654.09908;
	pJunkcode = 5425259588.10916;
	if (pJunkcode = 6415539262.86298)
		pJunkcode = 9470899531.35037;
	pJunkcode = 3830743394.45635;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3607() {
	float pJunkcode = 2846678061.10522;
	pJunkcode = 5008721429.33653;
	if (pJunkcode = 5623632481.55871)
		pJunkcode = 2967860302.85196;
	pJunkcode = 9748872674.94509;
	pJunkcode = 1934222058.66275;
	if (pJunkcode = 1288320584.44857)
		pJunkcode = 1684058702.04036;
	pJunkcode = 9835252778.53147;
	if (pJunkcode = 9402486334.84234)
		pJunkcode = 9389637019.74593;
	pJunkcode = 8990727307.85795;
	pJunkcode = 2841356064.48869;
	if (pJunkcode = 4854775142.35529)
		pJunkcode = 7057756778.46198;
	pJunkcode = 6185501515.96872;
	if (pJunkcode = 5743313183.23225)
		pJunkcode = 2838834578.07111;
	pJunkcode = 7076363439.54376;
	pJunkcode = 4157788935.53141;
	if (pJunkcode = 8890088272.04912)
		pJunkcode = 3773253703.9022;
	pJunkcode = 8791869426.04273;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3606() {
	float pJunkcode = 9107720944.92919;
	pJunkcode = 4992750442.9913;
	if (pJunkcode = 901815494.88786)
		pJunkcode = 2900224298.9019;
	pJunkcode = 7567033104.81445;
	pJunkcode = 489914854.737237;
	if (pJunkcode = 2464134577.92571)
		pJunkcode = 8731803778.88664;
	pJunkcode = 4959231554.38882;
	if (pJunkcode = 2548047721.15768)
		pJunkcode = 5013768525.56116;
	pJunkcode = 4706874347.35418;
	pJunkcode = 1693145583.73791;
	if (pJunkcode = 2951575538.44274)
		pJunkcode = 7187191347.43627;
	pJunkcode = 102717373.347568;
	if (pJunkcode = 14860034.6358168)
		pJunkcode = 4235622703.27852;
	pJunkcode = 5057803735.98999;
	pJunkcode = 9185536531.11315;
	if (pJunkcode = 2269676268.85081)
		pJunkcode = 2748219893.20179;
	pJunkcode = 6274877007.05397;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3605() {
	float pJunkcode = 6607434430.20641;
	pJunkcode = 4580395801.5955;
	if (pJunkcode = 414992789.007631)
		pJunkcode = 9310061437.76077;
	pJunkcode = 1609530577.16371;
	pJunkcode = 8982963102.86521;
	if (pJunkcode = 3391825102.81265)
		pJunkcode = 5932637278.71179;
	pJunkcode = 4175098386.66952;
	if (pJunkcode = 5633440483.77105)
		pJunkcode = 1777802389.92215;
	pJunkcode = 3394952059.26423;
	pJunkcode = 9105192413.12678;
	if (pJunkcode = 4206496246.57192)
		pJunkcode = 4481089160.74536;
	pJunkcode = 3607788426.93951;
	if (pJunkcode = 8019351497.20214)
		pJunkcode = 5699163989.37338;
	pJunkcode = 1580021150.47437;
	pJunkcode = 5390199469.19057;
	if (pJunkcode = 5985314536.04376)
		pJunkcode = 2963667409.47888;
	pJunkcode = 7917732975.95358;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3604() {
	float pJunkcode = 4859137381.07524;
	pJunkcode = 4148620445.18106;
	if (pJunkcode = 4552837848.71388)
		pJunkcode = 9731313880.97925;
	pJunkcode = 6922952732.19282;
	pJunkcode = 3967215187.61989;
	if (pJunkcode = 7841801091.91131)
		pJunkcode = 8474638967.48907;
	pJunkcode = 3465542454.3034;
	if (pJunkcode = 7064339389.06759)
		pJunkcode = 9086597400.44562;
	pJunkcode = 3697476960.35129;
	pJunkcode = 8388661826.85287;
	if (pJunkcode = 8321755674.47942)
		pJunkcode = 8683393864.10986;
	pJunkcode = 9486875020.59523;
	if (pJunkcode = 6042690224.54158)
		pJunkcode = 3700885739.18059;
	pJunkcode = 467795857.448067;
	pJunkcode = 9552113863.79152;
	if (pJunkcode = 1299195301.22898)
		pJunkcode = 3140606589.21975;
	pJunkcode = 6660576163.72947;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3603() {
	float pJunkcode = 5185178498.46971;
	pJunkcode = 3039772145.26244;
	if (pJunkcode = 2696221700.60601)
		pJunkcode = 9791854896.03503;
	pJunkcode = 2394780359.21563;
	pJunkcode = 2985332922.40173;
	if (pJunkcode = 6514081477.13168)
		pJunkcode = 864080078.510013;
	pJunkcode = 8411133081.74171;
	if (pJunkcode = 5013276764.2198)
		pJunkcode = 9730834634.15984;
	pJunkcode = 5229459212.38735;
	pJunkcode = 5654907422.08032;
	if (pJunkcode = 2229312779.58091)
		pJunkcode = 9593182627.28107;
	pJunkcode = 9143656835.91446;
	if (pJunkcode = 5295234817.9818)
		pJunkcode = 5865203565.31521;
	pJunkcode = 1234733434.06031;
	pJunkcode = 2939047400.69418;
	if (pJunkcode = 7366418654.43083)
		pJunkcode = 6027043260.70622;
	pJunkcode = 7184990640.23012;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3602() {
	float pJunkcode = 1938965346.53184;
	pJunkcode = 7060786110.00095;
	if (pJunkcode = 6747897291.84618)
		pJunkcode = 8499037179.36403;
	pJunkcode = 5741771573.40598;
	pJunkcode = 238164347.070095;
	if (pJunkcode = 6770777761.12547)
		pJunkcode = 4869534043.11721;
	pJunkcode = 698957692.463868;
	if (pJunkcode = 9485473026.26824)
		pJunkcode = 2610590496.28072;
	pJunkcode = 3719809889.60971;
	pJunkcode = 5681497717.20062;
	if (pJunkcode = 2213831398.58684)
		pJunkcode = 3469301427.8735;
	pJunkcode = 5034124672.57399;
	if (pJunkcode = 5397211070.62372)
		pJunkcode = 835482882.473246;
	pJunkcode = 293237230.817228;
	pJunkcode = 9481398067.42172;
	if (pJunkcode = 2316996479.59166)
		pJunkcode = 3587918153.03995;
	pJunkcode = 798631596.698596;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3601() {
	float pJunkcode = 6509890029.62244;
	pJunkcode = 3099977726.89261;
	if (pJunkcode = 9165954724.51143)
		pJunkcode = 8331158697.44837;
	pJunkcode = 9475102720.39156;
	pJunkcode = 7868559500.71669;
	if (pJunkcode = 6195349329.49469)
		pJunkcode = 4789756892.34634;
	pJunkcode = 3665911075.23856;
	if (pJunkcode = 6478212792.03963)
		pJunkcode = 379952439.541289;
	pJunkcode = 699041403.55636;
	pJunkcode = 9004103587.937;
	if (pJunkcode = 677180945.813375)
		pJunkcode = 4349395181.56179;
	pJunkcode = 9073207504.56669;
	if (pJunkcode = 7868667214.08021)
		pJunkcode = 9233257898.05645;
	pJunkcode = 2428881814.53073;
	pJunkcode = 8897332967.22188;
	if (pJunkcode = 9352829524.32083)
		pJunkcode = 7857992694.72673;
	pJunkcode = 94139777.9786749;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3600() {
	float pJunkcode = 9818869134.26501;
	pJunkcode = 3358538614.12139;
	if (pJunkcode = 7593733530.05618)
		pJunkcode = 7888908523.18307;
	pJunkcode = 59777804.0767108;
	pJunkcode = 9387768269.83865;
	if (pJunkcode = 9137100413.83776)
		pJunkcode = 7695434523.7506;
	pJunkcode = 7518419276.67083;
	if (pJunkcode = 1646526561.07977)
		pJunkcode = 963793428.517923;
	pJunkcode = 6677418354.04983;
	pJunkcode = 927409156.762999;
	if (pJunkcode = 8466681449.80268)
		pJunkcode = 6014129223.99472;
	pJunkcode = 7279931.6485683;
	if (pJunkcode = 228827745.362944)
		pJunkcode = 5775007040.10021;
	pJunkcode = 5434264125.41643;
	pJunkcode = 1829383484.56072;
	if (pJunkcode = 8943281760.9784)
		pJunkcode = 5228014519.67561;
	pJunkcode = 2177031814.66789;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3599() {
	float pJunkcode = 8715711027.98906;
	pJunkcode = 3286873111.05338;
	if (pJunkcode = 8301210561.6776)
		pJunkcode = 7237181409.78207;
	pJunkcode = 142133484.202413;
	pJunkcode = 2437279282.27348;
	if (pJunkcode = 5366980951.82369)
		pJunkcode = 1440883829.32769;
	pJunkcode = 6305389472.09543;
	if (pJunkcode = 455549241.728704)
		pJunkcode = 7300194374.70306;
	pJunkcode = 7573738143.5331;
	pJunkcode = 8437004652.15059;
	if (pJunkcode = 2032763278.75454)
		pJunkcode = 6893521673.18614;
	pJunkcode = 4318242119.6784;
	if (pJunkcode = 2913762969.74604)
		pJunkcode = 4135621908.08907;
	pJunkcode = 7975761678.0274;
	pJunkcode = 4210030727.08038;
	if (pJunkcode = 6453087864.78546)
		pJunkcode = 4331542216.00439;
	pJunkcode = 5674283387.49675;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3598() {
	float pJunkcode = 5060208770.99737;
	pJunkcode = 8950108005.46144;
	if (pJunkcode = 4801693296.9315)
		pJunkcode = 9445726851.00874;
	pJunkcode = 1068222820.82547;
	pJunkcode = 8791416866.00482;
	if (pJunkcode = 4993370674.34049)
		pJunkcode = 2663319360.00878;
	pJunkcode = 1742966938.68643;
	if (pJunkcode = 3860778191.25437)
		pJunkcode = 4246408088.4909;
	pJunkcode = 9353543208.29922;
	pJunkcode = 8433197459.83639;
	if (pJunkcode = 1771788257.58781)
		pJunkcode = 1927361064.55364;
	pJunkcode = 1890160852.95251;
	if (pJunkcode = 4058032370.2599)
		pJunkcode = 6961675859.83479;
	pJunkcode = 2284739280.64507;
	pJunkcode = 5741421297.17527;
	if (pJunkcode = 7575666206.45371)
		pJunkcode = 9838516062.94652;
	pJunkcode = 6574806892.21319;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3597() {
	float pJunkcode = 7163566714.62382;
	pJunkcode = 5285309675.18745;
	if (pJunkcode = 1555882181.28711)
		pJunkcode = 8960919856.26843;
	pJunkcode = 9554110890.45914;
	pJunkcode = 1807832728.05793;
	if (pJunkcode = 4338111974.48699)
		pJunkcode = 4484693135.84938;
	pJunkcode = 5672589612.725;
	if (pJunkcode = 3752534123.53057)
		pJunkcode = 6421115868.88374;
	pJunkcode = 1939851966.06973;
	pJunkcode = 2019411962.52746;
	if (pJunkcode = 4036078958.1403)
		pJunkcode = 5793960070.55357;
	pJunkcode = 9989471560.78812;
	if (pJunkcode = 2597258609.57655)
		pJunkcode = 8118885862.77093;
	pJunkcode = 369172733.011512;
	pJunkcode = 6108555105.82526;
	if (pJunkcode = 2317889543.81731)
		pJunkcode = 2377240657.17166;
	pJunkcode = 6308080660.0872;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3596() {
	float pJunkcode = 6871801189.49804;
	pJunkcode = 1916229412.41414;
	if (pJunkcode = 8631115318.28304)
		pJunkcode = 2625699228.57126;
	pJunkcode = 5098960943.97167;
	pJunkcode = 13642410.066883;
	if (pJunkcode = 2316295391.62512)
		pJunkcode = 7615918177.94542;
	pJunkcode = 4971261080.98698;
	if (pJunkcode = 2796621994.2603)
		pJunkcode = 4970096366.6527;
	pJunkcode = 2010662055.57896;
	pJunkcode = 5845800254.21315;
	if (pJunkcode = 7988746103.39519)
		pJunkcode = 8026631600.90509;
	pJunkcode = 3886892566.67188;
	if (pJunkcode = 1220374949.1068)
		pJunkcode = 8382383785.95139;
	pJunkcode = 4503787437.21134;
	pJunkcode = 8926106221.04585;
	if (pJunkcode = 9567261552.91579)
		pJunkcode = 4428387039.6776;
	pJunkcode = 5992602132.1619;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3595() {
	float pJunkcode = 1978758201.54028;
	pJunkcode = 1830715811.9992;
	if (pJunkcode = 1565748262.52271)
		pJunkcode = 2712774755.41127;
	pJunkcode = 7985524616.3193;
	pJunkcode = 4426227198.49736;
	if (pJunkcode = 8594833581.7899)
		pJunkcode = 6571881395.11801;
	pJunkcode = 9777165205.81812;
	if (pJunkcode = 6447625039.63413)
		pJunkcode = 9724201825.76945;
	pJunkcode = 2568601998.082;
	pJunkcode = 5822057589.08625;
	if (pJunkcode = 640602660.551575)
		pJunkcode = 4148042150.86802;
	pJunkcode = 736268.563551531;
	if (pJunkcode = 9680091903.06252)
		pJunkcode = 7050617559.17263;
	pJunkcode = 86179040.276685;
	pJunkcode = 180044774.433089;
	if (pJunkcode = 9001764080.13068)
		pJunkcode = 8367213687.10178;
	pJunkcode = 9390438659.27701;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3594() {
	float pJunkcode = 6552294000.63672;
	pJunkcode = 1154738833.28435;
	if (pJunkcode = 2158481532.96266)
		pJunkcode = 8848982835.77163;
	pJunkcode = 1816117723.42639;
	pJunkcode = 4910337289.19596;
	if (pJunkcode = 8386567738.71717)
		pJunkcode = 9097973706.25058;
	pJunkcode = 7551892901.12052;
	if (pJunkcode = 8281324383.27582)
		pJunkcode = 5982282841.29739;
	pJunkcode = 6061757315.25834;
	pJunkcode = 5174981058.30403;
	if (pJunkcode = 5966326304.2774)
		pJunkcode = 6732985294.82552;
	pJunkcode = 8015774988.85448;
	if (pJunkcode = 2302892191.93625)
		pJunkcode = 25349124.8624063;
	pJunkcode = 6790566787.25611;
	pJunkcode = 8770970016.06519;
	if (pJunkcode = 1681699985.89802)
		pJunkcode = 2528940707.3697;
	pJunkcode = 6977977795.33227;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3593() {
	float pJunkcode = 7882341574.93824;
	pJunkcode = 1773012098.67276;
	if (pJunkcode = 9762845016.46592)
		pJunkcode = 4512889243.2039;
	pJunkcode = 6373449803.32189;
	pJunkcode = 7682807850.73071;
	if (pJunkcode = 2087970455.54567)
		pJunkcode = 6311966236.38204;
	pJunkcode = 927184596.181152;
	if (pJunkcode = 584180601.909067)
		pJunkcode = 2527272266.64694;
	pJunkcode = 9597766630.3148;
	pJunkcode = 8074768843.45694;
	if (pJunkcode = 9977979145.83059)
		pJunkcode = 4815132636.23147;
	pJunkcode = 5358341009.68852;
	if (pJunkcode = 8468576022.69842)
		pJunkcode = 3559151311.30779;
	pJunkcode = 2793375857.65067;
	pJunkcode = 9967636355.60422;
	if (pJunkcode = 705918865.612207)
		pJunkcode = 5380827351.8522;
	pJunkcode = 1112121742.87163;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3592() {
	float pJunkcode = 4492388949.85278;
	pJunkcode = 7405632905.46458;
	if (pJunkcode = 636754138.883123)
		pJunkcode = 7879832465.6739;
	pJunkcode = 9162527710.56847;
	pJunkcode = 1799779161.70662;
	if (pJunkcode = 7905258486.50544)
		pJunkcode = 9296574613.12589;
	pJunkcode = 423999838.532422;
	if (pJunkcode = 3085415077.11768)
		pJunkcode = 2590985725.82226;
	pJunkcode = 4740751598.31475;
	pJunkcode = 1922516675.64519;
	if (pJunkcode = 9429189072.06925)
		pJunkcode = 427369206.512786;
	pJunkcode = 8155705502.52096;
	if (pJunkcode = 3046429053.71175)
		pJunkcode = 6668210885.91121;
	pJunkcode = 78247436.3817916;
	pJunkcode = 2490350811.07461;
	if (pJunkcode = 8935948458.33367)
		pJunkcode = 6539689965.51394;
	pJunkcode = 6877758292.86483;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3591() {
	float pJunkcode = 9159935467.62586;
	pJunkcode = 8350922026.31785;
	if (pJunkcode = 7469905378.44014)
		pJunkcode = 3752983095.68414;
	pJunkcode = 3113581943.56408;
	pJunkcode = 2305638369.31953;
	if (pJunkcode = 1681940886.37104)
		pJunkcode = 836853167.911971;
	pJunkcode = 7886481087.76347;
	if (pJunkcode = 3412845658.01921)
		pJunkcode = 1586335668.58801;
	pJunkcode = 4493083979.62655;
	pJunkcode = 1640030236.64782;
	if (pJunkcode = 8794625233.20514)
		pJunkcode = 1570344528.62381;
	pJunkcode = 6903392573.96786;
	if (pJunkcode = 5337484994.2524)
		pJunkcode = 6161636238.79441;
	pJunkcode = 373243556.827152;
	pJunkcode = 641918846.002952;
	if (pJunkcode = 5494118622.90164)
		pJunkcode = 5829712495.15465;
	pJunkcode = 1263824223.79729;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3590() {
	float pJunkcode = 422265081.320605;
	pJunkcode = 1852898534.39827;
	if (pJunkcode = 2685212150.61991)
		pJunkcode = 381074720.915097;
	pJunkcode = 2707677435.79075;
	pJunkcode = 602254400.466982;
	if (pJunkcode = 9125327175.33598)
		pJunkcode = 3778845709.18773;
	pJunkcode = 5835560995.75375;
	if (pJunkcode = 3242308686.52038)
		pJunkcode = 3152042994.99775;
	pJunkcode = 5515817735.27868;
	pJunkcode = 4796355861.46938;
	if (pJunkcode = 8653535646.63446)
		pJunkcode = 5956638329.71818;
	pJunkcode = 5797618592.6753;
	if (pJunkcode = 9123308432.7714)
		pJunkcode = 6857110942.82146;
	pJunkcode = 7014351166.06547;
	pJunkcode = 1792308506.65384;
	if (pJunkcode = 3837528491.19122)
		pJunkcode = 8584241718.81664;
	pJunkcode = 6276173201.8662;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3589() {
	float pJunkcode = 6199541058.7262;
	pJunkcode = 6356104238.18379;
	if (pJunkcode = 7537141008.49089)
		pJunkcode = 7433944849.56254;
	pJunkcode = 8897425069.44686;
	pJunkcode = 558125122.742198;
	if (pJunkcode = 996291069.265743)
		pJunkcode = 42452904.7798853;
	pJunkcode = 7872936626.47913;
	if (pJunkcode = 4963049896.85104)
		pJunkcode = 3950192681.08244;
	pJunkcode = 7764375444.26157;
	pJunkcode = 7585122714.74326;
	if (pJunkcode = 579164197.112967)
		pJunkcode = 2093875694.07693;
	pJunkcode = 4572760259.85809;
	if (pJunkcode = 5937321076.61803)
		pJunkcode = 7864735428.34963;
	pJunkcode = 8618884590.76791;
	pJunkcode = 7784592315.70446;
	if (pJunkcode = 7809543464.56063)
		pJunkcode = 6909860837.93124;
	pJunkcode = 4993732938.90116;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3588() {
	float pJunkcode = 2982973859.3452;
	pJunkcode = 9807424535.25859;
	if (pJunkcode = 8756914608.38631)
		pJunkcode = 1895132997.08587;
	pJunkcode = 3056921850.56512;
	pJunkcode = 7892417362.30916;
	if (pJunkcode = 6053442417.74934)
		pJunkcode = 8657806060.19513;
	pJunkcode = 9469798712.09067;
	if (pJunkcode = 8967749593.1762)
		pJunkcode = 9460343894.1347;
	pJunkcode = 1100165950.67266;
	pJunkcode = 7604092292.30498;
	if (pJunkcode = 7088719121.35916)
		pJunkcode = 1503067504.02687;
	pJunkcode = 6040681875.433;
	if (pJunkcode = 1001491741.37544)
		pJunkcode = 2222742767.31544;
	pJunkcode = 4262684689.9394;
	pJunkcode = 41736487.3970666;
	if (pJunkcode = 3463646584.55395)
		pJunkcode = 639002855.287886;
	pJunkcode = 818966311.864996;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3587() {
	float pJunkcode = 4672910304.9825;
	pJunkcode = 3708573206.28888;
	if (pJunkcode = 1677196124.69871)
		pJunkcode = 7931629098.7971;
	pJunkcode = 7592742491.67544;
	pJunkcode = 8700598572.77375;
	if (pJunkcode = 3560773360.98873)
		pJunkcode = 1579085858.17717;
	pJunkcode = 1213764599.33571;
	if (pJunkcode = 7777477333.84273)
		pJunkcode = 158764508.553909;
	pJunkcode = 3218459721.29784;
	pJunkcode = 9126816824.86713;
	if (pJunkcode = 1909033750.88459)
		pJunkcode = 1872254208.82702;
	pJunkcode = 4585013257.40157;
	if (pJunkcode = 4236628307.39867)
		pJunkcode = 8634319094.10897;
	pJunkcode = 256664559.04611;
	pJunkcode = 3861600678.2328;
	if (pJunkcode = 7817181788.59641)
		pJunkcode = 2959242234.93702;
	pJunkcode = 9024147495.88099;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3586() {
	float pJunkcode = 1276042734.81071;
	pJunkcode = 9639406919.79826;
	if (pJunkcode = 5322389803.25667)
		pJunkcode = 6366553884.96548;
	pJunkcode = 9573867762.99944;
	pJunkcode = 558950813.438248;
	if (pJunkcode = 7592435300.32448)
		pJunkcode = 1379953334.40948;
	pJunkcode = 8894796670.59517;
	if (pJunkcode = 15436512.8879931)
		pJunkcode = 8446951182.55398;
	pJunkcode = 5192186879.88727;
	pJunkcode = 2347727460.99189;
	if (pJunkcode = 8069802883.24364)
		pJunkcode = 4123048459.62208;
	pJunkcode = 7642136769.64516;
	if (pJunkcode = 172728414.704873)
		pJunkcode = 6528648166.47789;
	pJunkcode = 7423560401.69399;
	pJunkcode = 8007010162.26754;
	if (pJunkcode = 1196897964.37052)
		pJunkcode = 399606881.592542;
	pJunkcode = 9944181210.16391;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3585() {
	float pJunkcode = 7819418176.74187;
	pJunkcode = 4845839310.35559;
	if (pJunkcode = 3150329278.48079)
		pJunkcode = 1078318170.43144;
	pJunkcode = 6793056960.59356;
	pJunkcode = 9131377244.7208;
	if (pJunkcode = 9144658337.59969)
		pJunkcode = 2964006083.87464;
	pJunkcode = 3085496927.91193;
	if (pJunkcode = 1920842132.49228)
		pJunkcode = 987970120.548831;
	pJunkcode = 8886980805.65582;
	pJunkcode = 4724432398.59768;
	if (pJunkcode = 5075200865.61508)
		pJunkcode = 860199719.897724;
	pJunkcode = 1809883747.61187;
	if (pJunkcode = 1967135640.21104)
		pJunkcode = 5470907004.23775;
	pJunkcode = 2169358892.34445;
	pJunkcode = 6501187826.86747;
	if (pJunkcode = 3513063572.59755)
		pJunkcode = 880414211.835224;
	pJunkcode = 2566966400.12139;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3584() {
	float pJunkcode = 89041510.7469073;
	pJunkcode = 8027971115.75367;
	if (pJunkcode = 894480879.091439)
		pJunkcode = 7141611633.12521;
	pJunkcode = 8876976701.46662;
	pJunkcode = 2961822538.44362;
	if (pJunkcode = 824134248.313558)
		pJunkcode = 5565930491.16181;
	pJunkcode = 2792464884.271;
	if (pJunkcode = 1067417244.16758)
		pJunkcode = 8611447837.84612;
	pJunkcode = 9658712075.13914;
	pJunkcode = 8444965259.46872;
	if (pJunkcode = 1922081847.81177)
		pJunkcode = 6397670336.92457;
	pJunkcode = 1361155099.86156;
	if (pJunkcode = 5029341533.81044)
		pJunkcode = 7953590769.33704;
	pJunkcode = 7681332652.8531;
	pJunkcode = 1663657279.23333;
	if (pJunkcode = 4635035457.03974)
		pJunkcode = 6773891967.8985;
	pJunkcode = 7880774060.6336;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3583() {
	float pJunkcode = 5574513960.78909;
	pJunkcode = 6279361850.83649;
	if (pJunkcode = 8011711913.56518)
		pJunkcode = 84102977.709151;
	pJunkcode = 9201926757.43608;
	pJunkcode = 9455671651.49774;
	if (pJunkcode = 591395585.860507)
		pJunkcode = 6577624145.0963;
	pJunkcode = 2544864222.34972;
	if (pJunkcode = 4371300407.53325)
		pJunkcode = 7694558097.43635;
	pJunkcode = 8101432591.25416;
	pJunkcode = 342493078.38379;
	if (pJunkcode = 3309524898.68964)
		pJunkcode = 6514020790.43129;
	pJunkcode = 5675784864.70631;
	if (pJunkcode = 7231606240.31202)
		pJunkcode = 9184192992.99128;
	pJunkcode = 1513698836.05615;
	pJunkcode = 6925153912.04045;
	if (pJunkcode = 3196138656.97037)
		pJunkcode = 9314125623.64114;
	pJunkcode = 7880622761.80945;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3582() {
	float pJunkcode = 9831490256.65785;
	pJunkcode = 6792366543.14872;
	if (pJunkcode = 2813373754.06299)
		pJunkcode = 2348920671.18368;
	pJunkcode = 8300627312.77801;
	pJunkcode = 7866080801.15862;
	if (pJunkcode = 6636393965.3089)
		pJunkcode = 6273982459.71195;
	pJunkcode = 6062563271.42638;
	if (pJunkcode = 3911081454.24994)
		pJunkcode = 4178487468.22334;
	pJunkcode = 2698040075.77899;
	pJunkcode = 6999618272.22637;
	if (pJunkcode = 8968489197.93094)
		pJunkcode = 7804628537.98576;
	pJunkcode = 2382498616.68834;
	if (pJunkcode = 7905481899.24369)
		pJunkcode = 9029149248.79121;
	pJunkcode = 1688026164.05369;
	pJunkcode = 5463412011.64117;
	if (pJunkcode = 3785946788.87075)
		pJunkcode = 6192562050.9289;
	pJunkcode = 8813858398.03093;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3581() {
	float pJunkcode = 3659831082.75867;
	pJunkcode = 4356145072.32711;
	if (pJunkcode = 9429636821.87416)
		pJunkcode = 7218688573.80708;
	pJunkcode = 1830266555.79508;
	pJunkcode = 587353176.412469;
	if (pJunkcode = 9461251545.05431)
		pJunkcode = 4688837100.51349;
	pJunkcode = 4824004447.42931;
	if (pJunkcode = 2730602552.22778)
		pJunkcode = 752859109.85558;
	pJunkcode = 1025836146.62673;
	pJunkcode = 1407639764.28377;
	if (pJunkcode = 9001267748.36948)
		pJunkcode = 3328009345.86042;
	pJunkcode = 4227427987.08932;
	if (pJunkcode = 3371541458.53721)
		pJunkcode = 9342904928.67303;
	pJunkcode = 9086137941.70472;
	pJunkcode = 5673279620.39201;
	if (pJunkcode = 5545244878.1131)
		pJunkcode = 3607566103.83778;
	pJunkcode = 3864877452.72044;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3580() {
	float pJunkcode = 1002395029.09242;
	pJunkcode = 1508487209.37421;
	if (pJunkcode = 8386371178.06852)
		pJunkcode = 4803505481.19958;
	pJunkcode = 6075860238.53981;
	pJunkcode = 8985908551.01873;
	if (pJunkcode = 3958100934.62559)
		pJunkcode = 3618552370.99234;
	pJunkcode = 5788137146.1449;
	if (pJunkcode = 6881008671.60637)
		pJunkcode = 5938606451.89844;
	pJunkcode = 1412290246.78638;
	pJunkcode = 7629714880.77002;
	if (pJunkcode = 5909954778.69246)
		pJunkcode = 466537611.322958;
	pJunkcode = 7645349621.11883;
	if (pJunkcode = 8364688573.21905)
		pJunkcode = 7923828090.63256;
	pJunkcode = 1551637148.3411;
	pJunkcode = 7783964235.71276;
	if (pJunkcode = 3136974590.8997)
		pJunkcode = 6013544743.43334;
	pJunkcode = 2095287108.63756;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3579() {
	float pJunkcode = 5540363446.21967;
	pJunkcode = 1304987161.68382;
	if (pJunkcode = 5450691418.00717)
		pJunkcode = 891578371.464114;
	pJunkcode = 3599885123.32635;
	pJunkcode = 1350085366.08424;
	if (pJunkcode = 4358712861.31176)
		pJunkcode = 9016693462.44164;
	pJunkcode = 4214342837.86404;
	if (pJunkcode = 4729723002.97246)
		pJunkcode = 3635178489.65124;
	pJunkcode = 4685811186.89419;
	pJunkcode = 3042014756.90878;
	if (pJunkcode = 2776182350.35703)
		pJunkcode = 4348751497.4164;
	pJunkcode = 5019835528.54935;
	if (pJunkcode = 2015362039.70968)
		pJunkcode = 7146087431.52767;
	pJunkcode = 5066927877.20284;
	pJunkcode = 8418754388.35592;
	if (pJunkcode = 9574988319.11543)
		pJunkcode = 8669336114.17812;
	pJunkcode = 459737245.707164;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3578() {
	float pJunkcode = 3790075980.41676;
	pJunkcode = 7573908504.01142;
	if (pJunkcode = 3935035608.97707)
		pJunkcode = 1794197815.30137;
	pJunkcode = 2691060353.19877;
	pJunkcode = 4503378369.50273;
	if (pJunkcode = 8473895330.14791)
		pJunkcode = 5143872920.64408;
	pJunkcode = 5573570139.9907;
	if (pJunkcode = 3149308959.61111)
		pJunkcode = 3655169699.23434;
	pJunkcode = 8380467085.05257;
	pJunkcode = 527718731.952921;
	if (pJunkcode = 9335588662.07614)
		pJunkcode = 9001395347.59941;
	pJunkcode = 6285103071.13274;
	if (pJunkcode = 8994471406.02698)
		pJunkcode = 3397422584.22015;
	pJunkcode = 6495036528.38772;
	pJunkcode = 6636169176.55531;
	if (pJunkcode = 3145478843.02099)
		pJunkcode = 7297952042.4507;
	pJunkcode = 8698637622.85359;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3577() {
	float pJunkcode = 7046777337.83362;
	pJunkcode = 6646060286.8348;
	if (pJunkcode = 2749504721.33565)
		pJunkcode = 7555433349.0187;
	pJunkcode = 3405019385.65417;
	pJunkcode = 154763565.828831;
	if (pJunkcode = 5885809502.18613)
		pJunkcode = 9934590730.13345;
	pJunkcode = 7486560942.89607;
	if (pJunkcode = 4025282704.50215)
		pJunkcode = 4616646448.52369;
	pJunkcode = 4642998734.89865;
	pJunkcode = 8044380385.10451;
	if (pJunkcode = 58894076.2665001)
		pJunkcode = 6152747317.04368;
	pJunkcode = 2229537435.62236;
	if (pJunkcode = 5785926820.80105)
		pJunkcode = 3542675657.94768;
	pJunkcode = 6818263738.64198;
	pJunkcode = 268166841.267647;
	if (pJunkcode = 2304484383.71403)
		pJunkcode = 8367950469.97734;
	pJunkcode = 6644554271.80137;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3576() {
	float pJunkcode = 142373483.629849;
	pJunkcode = 9680484162.89505;
	if (pJunkcode = 9171607921.73479)
		pJunkcode = 730031909.366556;
	pJunkcode = 1679739266.4779;
	pJunkcode = 2696481485.78368;
	if (pJunkcode = 6003368008.68974)
		pJunkcode = 7351546552.542;
	pJunkcode = 424678069.743245;
	if (pJunkcode = 1416425058.80792)
		pJunkcode = 8962720191.90354;
	pJunkcode = 3673370382.23926;
	pJunkcode = 5292454335.67453;
	if (pJunkcode = 7206228587.23471)
		pJunkcode = 1727759373.10431;
	pJunkcode = 9183912642.08852;
	if (pJunkcode = 6069563336.21651)
		pJunkcode = 1344804913.02401;
	pJunkcode = 6535395216.73775;
	pJunkcode = 9952392659.72413;
	if (pJunkcode = 8364689877.05761)
		pJunkcode = 9859918163.22655;
	pJunkcode = 9453644841.43203;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3575() {
	float pJunkcode = 6099143682.4107;
	pJunkcode = 3549285679.43085;
	if (pJunkcode = 9202135292.44499)
		pJunkcode = 1666694920.41487;
	pJunkcode = 6719152762.05148;
	pJunkcode = 2050801892.29744;
	if (pJunkcode = 8112409431.19024)
		pJunkcode = 6584034444.12229;
	pJunkcode = 6570744862.32302;
	if (pJunkcode = 6545849347.7928)
		pJunkcode = 8858701331.79471;
	pJunkcode = 437374812.444623;
	pJunkcode = 1318964660.35183;
	if (pJunkcode = 5629193879.23391)
		pJunkcode = 3189616740.05;
	pJunkcode = 8402319016.62264;
	if (pJunkcode = 6893936270.56724)
		pJunkcode = 6270112119.70207;
	pJunkcode = 9319584961.02432;
	pJunkcode = 8497636477.23419;
	if (pJunkcode = 5414086457.60528)
		pJunkcode = 346178929.893174;
	pJunkcode = 3103287784.18793;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3574() {
	float pJunkcode = 11932674.9141137;
	pJunkcode = 5885713874.94046;
	if (pJunkcode = 3309845989.89102)
		pJunkcode = 9038021614.12044;
	pJunkcode = 2800146432.88352;
	pJunkcode = 3887990196.18366;
	if (pJunkcode = 3276644376.43383)
		pJunkcode = 2002690158.41582;
	pJunkcode = 5533556004.95138;
	if (pJunkcode = 9995350256.89144)
		pJunkcode = 2798865094.8461;
	pJunkcode = 4196936925.9549;
	pJunkcode = 1594149845.52207;
	if (pJunkcode = 2747529790.07516)
		pJunkcode = 7566148555.12028;
	pJunkcode = 789541229.561165;
	if (pJunkcode = 4419217527.21094)
		pJunkcode = 7068994353.51526;
	pJunkcode = 3583052828.20939;
	pJunkcode = 5803894165.59664;
	if (pJunkcode = 9515259218.30429)
		pJunkcode = 8224517536.26142;
	pJunkcode = 263429741.975176;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3573() {
	float pJunkcode = 1462877769.87214;
	pJunkcode = 2220946650.52941;
	if (pJunkcode = 4133748541.19629)
		pJunkcode = 5616629550.65202;
	pJunkcode = 7389419004.6814;
	pJunkcode = 4173163299.31863;
	if (pJunkcode = 9232842932.80661)
		pJunkcode = 2421752478.80337;
	pJunkcode = 564509829.279777;
	if (pJunkcode = 7331139290.40083)
		pJunkcode = 4679560440.45611;
	pJunkcode = 552205507.062462;
	pJunkcode = 7486469862.7874;
	if (pJunkcode = 1859528960.955)
		pJunkcode = 1923495767.60507;
	pJunkcode = 3673316589.58528;
	if (pJunkcode = 114521559.389018)
		pJunkcode = 5745483722.87358;
	pJunkcode = 7199390747.90156;
	pJunkcode = 5352454432.34673;
	if (pJunkcode = 6632542952.37748)
		pJunkcode = 2783997970.82665;
	pJunkcode = 3322647780.0717;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3572() {
	float pJunkcode = 671947603.234472;
	pJunkcode = 5660144806.55878;
	if (pJunkcode = 37074519.473668)
		pJunkcode = 6726717067.63753;
	pJunkcode = 9174098162.59245;
	pJunkcode = 5901906192.53881;
	if (pJunkcode = 2817375125.19217)
		pJunkcode = 5868151784.12568;
	pJunkcode = 6408114537.85197;
	if (pJunkcode = 2598029166.52753)
		pJunkcode = 5521792980.62779;
	pJunkcode = 6326478145.88221;
	pJunkcode = 5114888966.50788;
	if (pJunkcode = 792628370.042543)
		pJunkcode = 436749780.175569;
	pJunkcode = 4158470652.18212;
	if (pJunkcode = 9814293920.50018)
		pJunkcode = 7960446315.26357;
	pJunkcode = 1258443589.36954;
	pJunkcode = 3714907442.63255;
	if (pJunkcode = 476282819.738653)
		pJunkcode = 2687926379.30177;
	pJunkcode = 3841520793.65752;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3571() {
	float pJunkcode = 3956090599.49147;
	pJunkcode = 1076054727.63574;
	if (pJunkcode = 9647717649.68041)
		pJunkcode = 2140623049.37419;
	pJunkcode = 4297864757.34948;
	pJunkcode = 3564192570.52655;
	if (pJunkcode = 5832089732.78947)
		pJunkcode = 5018148575.64723;
	pJunkcode = 7674534745.90432;
	if (pJunkcode = 8729473111.57145)
		pJunkcode = 7725989482.28788;
	pJunkcode = 1102767956.10653;
	pJunkcode = 1114363164.76528;
	if (pJunkcode = 2701738963.34575)
		pJunkcode = 2550177359.84885;
	pJunkcode = 5865156540.98036;
	if (pJunkcode = 694960644.956505)
		pJunkcode = 6379763574.05664;
	pJunkcode = 6990921722.5429;
	pJunkcode = 5929375044.35244;
	if (pJunkcode = 6944876319.00523)
		pJunkcode = 7819912706.41191;
	pJunkcode = 5630977940.16978;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3570() {
	float pJunkcode = 2864329013.17482;
	pJunkcode = 5620699194.0281;
	if (pJunkcode = 6245216723.91429)
		pJunkcode = 5206361204.62821;
	pJunkcode = 7144329472.97423;
	pJunkcode = 5345987563.02496;
	if (pJunkcode = 3466765454.87814)
		pJunkcode = 4373706709.12667;
	pJunkcode = 6657304611.58094;
	if (pJunkcode = 8562268.51946335)
		pJunkcode = 9230129106.91774;
	pJunkcode = 8643980293.41194;
	pJunkcode = 4567239048.35012;
	if (pJunkcode = 4264788805.99065)
		pJunkcode = 5332702449.20106;
	pJunkcode = 2295946171.25467;
	if (pJunkcode = 3513455930.21796)
		pJunkcode = 6363433724.77243;
	pJunkcode = 7898815661.36379;
	pJunkcode = 9675707117.24852;
	if (pJunkcode = 6867103196.04942)
		pJunkcode = 3108809498.60506;
	pJunkcode = 3341342005.55459;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3569() {
	float pJunkcode = 3508444910.7408;
	pJunkcode = 8781078532.1124;
	if (pJunkcode = 1125180826.96702)
		pJunkcode = 8090588908.1599;
	pJunkcode = 4353246562.36195;
	pJunkcode = 1766555769.48882;
	if (pJunkcode = 2560125410.62114)
		pJunkcode = 6900146362.85317;
	pJunkcode = 1093541572.99557;
	if (pJunkcode = 9118732613.65295)
		pJunkcode = 6969329652.98384;
	pJunkcode = 6131257477.74131;
	pJunkcode = 7555204320.46148;
	if (pJunkcode = 4793227428.88628)
		pJunkcode = 281595030.419598;
	pJunkcode = 5629355944.03935;
	if (pJunkcode = 3110706197.22332)
		pJunkcode = 5510373692.38754;
	pJunkcode = 7715210574.35923;
	pJunkcode = 6888464288.27826;
	if (pJunkcode = 2849267167.85042)
		pJunkcode = 7320767077.66025;
	pJunkcode = 5035149452.43419;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3568() {
	float pJunkcode = 9068806295.04332;
	pJunkcode = 9289407228.59524;
	if (pJunkcode = 161168235.066165)
		pJunkcode = 6590091335.37675;
	pJunkcode = 3576259552.21579;
	pJunkcode = 3938793495.23893;
	if (pJunkcode = 2170972059.62516)
		pJunkcode = 4774706471.90737;
	pJunkcode = 1387733215.73269;
	if (pJunkcode = 7410949630.84805)
		pJunkcode = 8190157064.10209;
	pJunkcode = 5443827965.23106;
	pJunkcode = 1341847211.11625;
	if (pJunkcode = 5690431113.81614)
		pJunkcode = 7531524844.93937;
	pJunkcode = 5406846260.67558;
	if (pJunkcode = 4412740429.74128)
		pJunkcode = 6945918879.96387;
	pJunkcode = 2401193296.18645;
	pJunkcode = 1984328046.22675;
	if (pJunkcode = 4309703101.63307)
		pJunkcode = 846891258.050362;
	pJunkcode = 724383536.345889;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3567() {
	float pJunkcode = 4400497262.20926;
	pJunkcode = 595710970.21028;
	if (pJunkcode = 5757784929.84804)
		pJunkcode = 9466363612.02032;
	pJunkcode = 8299614921.71247;
	pJunkcode = 3464223942.89716;
	if (pJunkcode = 2552905073.94673)
		pJunkcode = 1840252706.59289;
	pJunkcode = 7319265851.90527;
	if (pJunkcode = 9890927646.87272)
		pJunkcode = 4404147390.87023;
	pJunkcode = 2144438290.56325;
	pJunkcode = 9577760307.84575;
	if (pJunkcode = 9313369709.26291)
		pJunkcode = 817725662.90396;
	pJunkcode = 2748851551.76469;
	if (pJunkcode = 6708427395.32221)
		pJunkcode = 8061271227.22965;
	pJunkcode = 6414542662.21379;
	pJunkcode = 6514887014.20876;
	if (pJunkcode = 8134148538.81528)
		pJunkcode = 2502085125.33863;
	pJunkcode = 5536601355.07844;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3566() {
	float pJunkcode = 8921139092.0344;
	pJunkcode = 5444990716.82263;
	if (pJunkcode = 3265989086.37091)
		pJunkcode = 1288019099.71962;
	pJunkcode = 7366866669.66003;
	pJunkcode = 1876233975.81682;
	if (pJunkcode = 2242632617.41976)
		pJunkcode = 7820759002.41386;
	pJunkcode = 9775180158.16733;
	if (pJunkcode = 4883543909.57762)
		pJunkcode = 4390387435.33864;
	pJunkcode = 3353358032.76703;
	pJunkcode = 7332914694.77994;
	if (pJunkcode = 1843769385.04836)
		pJunkcode = 5539272971.52202;
	pJunkcode = 8302538192.96895;
	if (pJunkcode = 1761823305.80811)
		pJunkcode = 2315195636.39082;
	pJunkcode = 5874294083.79105;
	pJunkcode = 5389476993.61498;
	if (pJunkcode = 7892829428.84647)
		pJunkcode = 7726238468.88829;
	pJunkcode = 1732913691.42471;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3565() {
	float pJunkcode = 4292025898.17092;
	pJunkcode = 3026563634.28257;
	if (pJunkcode = 9944105459.47152)
		pJunkcode = 9494116982.36356;
	pJunkcode = 5427536127.49761;
	pJunkcode = 5430328623.74594;
	if (pJunkcode = 9953448431.36138)
		pJunkcode = 2590567019.02139;
	pJunkcode = 535114257.964913;
	if (pJunkcode = 7200622460.91186)
		pJunkcode = 6906842821.8484;
	pJunkcode = 8977995987.82346;
	pJunkcode = 874174089.353179;
	if (pJunkcode = 9764403320.0824)
		pJunkcode = 6746671851.2307;
	pJunkcode = 5893207968.81975;
	if (pJunkcode = 4247098564.14283)
		pJunkcode = 9637440130.54395;
	pJunkcode = 7832436674.20533;
	pJunkcode = 4466680869.15864;
	if (pJunkcode = 7550179629.30564)
		pJunkcode = 1220538026.71542;
	pJunkcode = 967115869.951333;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3564() {
	float pJunkcode = 6045026370.30703;
	pJunkcode = 1624555565.7658;
	if (pJunkcode = 6838491140.90956)
		pJunkcode = 2558530881.24708;
	pJunkcode = 5024437631.65631;
	pJunkcode = 6945898623.90044;
	if (pJunkcode = 1764241348.93348)
		pJunkcode = 5356777267.2026;
	pJunkcode = 590911037.540353;
	if (pJunkcode = 659955594.591049)
		pJunkcode = 4798446235.99346;
	pJunkcode = 9210528031.44464;
	pJunkcode = 4924911906.3463;
	if (pJunkcode = 1186489809.92291)
		pJunkcode = 2474647891.61023;
	pJunkcode = 308264077.389274;
	if (pJunkcode = 1008981621.06222)
		pJunkcode = 8294274627.11864;
	pJunkcode = 2128819310.36311;
	pJunkcode = 6177814572.72413;
	if (pJunkcode = 6266311926.20703)
		pJunkcode = 1999084246.18021;
	pJunkcode = 1467583851.77644;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3563() {
	float pJunkcode = 8833351173.41315;
	pJunkcode = 4175034293.69087;
	if (pJunkcode = 7112566712.89464)
		pJunkcode = 9421733164.36196;
	pJunkcode = 1088364598.15181;
	pJunkcode = 2621088259.36205;
	if (pJunkcode = 9463831587.79319)
		pJunkcode = 2508082363.65042;
	pJunkcode = 6911049219.26377;
	if (pJunkcode = 9155540679.50743)
		pJunkcode = 7524543180.57099;
	pJunkcode = 4286417345.77702;
	pJunkcode = 4240149532.99113;
	if (pJunkcode = 4909591558.72824)
		pJunkcode = 7438530757.13189;
	pJunkcode = 6435385932.62793;
	if (pJunkcode = 2981201602.80492)
		pJunkcode = 4259942093.4556;
	pJunkcode = 7917261237.84733;
	pJunkcode = 148442447.03245;
	if (pJunkcode = 5688470326.87581)
		pJunkcode = 3995359122.80551;
	pJunkcode = 1876831606.52144;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3562() {
	float pJunkcode = 4934584008.99927;
	pJunkcode = 7351696922.38117;
	if (pJunkcode = 1002818371.50504)
		pJunkcode = 2966814137.711;
	pJunkcode = 2673017986.46342;
	pJunkcode = 928093592.848425;
	if (pJunkcode = 4693548067.15584)
		pJunkcode = 659009526.999411;
	pJunkcode = 5790126449.8973;
	if (pJunkcode = 5656925696.65999)
		pJunkcode = 1805050064.14935;
	pJunkcode = 4867192325.77018;
	pJunkcode = 1886863438.23472;
	if (pJunkcode = 532234220.5983)
		pJunkcode = 4462510830.21299;
	pJunkcode = 8867690909.88272;
	if (pJunkcode = 579216609.09503)
		pJunkcode = 3699491269.86475;
	pJunkcode = 1457710299.0802;
	pJunkcode = 5546980324.49673;
	if (pJunkcode = 6351767147.25018)
		pJunkcode = 5410958914.42635;
	pJunkcode = 3054777350.44258;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3561() {
	float pJunkcode = 9826100341.69346;
	pJunkcode = 2904183078.74736;
	if (pJunkcode = 8830979146.47603)
		pJunkcode = 18839941.3226059;
	pJunkcode = 9679976902.17273;
	pJunkcode = 7901119821.17528;
	if (pJunkcode = 664029354.371745)
		pJunkcode = 4569835614.91454;
	pJunkcode = 4970070054.72479;
	if (pJunkcode = 3372363875.66715)
		pJunkcode = 3761976121.9595;
	pJunkcode = 2152200851.78809;
	pJunkcode = 5979497655.85816;
	if (pJunkcode = 7252247047.87973)
		pJunkcode = 8410140921.12849;
	pJunkcode = 5510432984.44632;
	if (pJunkcode = 7145088060.22152)
		pJunkcode = 7343833565.12207;
	pJunkcode = 9572669935.82216;
	pJunkcode = 6054788232.93212;
	if (pJunkcode = 1606058054.59824)
		pJunkcode = 2833221310.90382;
	pJunkcode = 8012472244.69655;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3560() {
	float pJunkcode = 8177829032.97743;
	pJunkcode = 3991557623.14363;
	if (pJunkcode = 5777287314.05483)
		pJunkcode = 5707080363.96817;
	pJunkcode = 6046405063.17623;
	pJunkcode = 7531056662.1215;
	if (pJunkcode = 8227843514.35694)
		pJunkcode = 2731573599.96937;
	pJunkcode = 2574765787.80086;
	if (pJunkcode = 8910692707.17741)
		pJunkcode = 7855612109.02074;
	pJunkcode = 9853291475.89257;
	pJunkcode = 7602270159.28299;
	if (pJunkcode = 7189948274.50287)
		pJunkcode = 1844995347.19927;
	pJunkcode = 5681574054.62037;
	if (pJunkcode = 1982910897.10472)
		pJunkcode = 5678818938.95806;
	pJunkcode = 3791190657.61779;
	pJunkcode = 3721205339.13276;
	if (pJunkcode = 9478203923.70318)
		pJunkcode = 8107377513.04349;
	pJunkcode = 9666163631.28522;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3559() {
	float pJunkcode = 3441551317.12667;
	pJunkcode = 6933634522.77459;
	if (pJunkcode = 7252512104.29046)
		pJunkcode = 9050147277.85827;
	pJunkcode = 3344412391.7104;
	pJunkcode = 9904971569.13081;
	if (pJunkcode = 4096001972.34214)
		pJunkcode = 9711368741.1349;
	pJunkcode = 2022685654.3682;
	if (pJunkcode = 5092224610.79916)
		pJunkcode = 7953723027.46156;
	pJunkcode = 9711528768.69259;
	pJunkcode = 3230877361.05623;
	if (pJunkcode = 5908294233.17786)
		pJunkcode = 8838476926.02754;
	pJunkcode = 9254760901.47177;
	if (pJunkcode = 7847178985.94979)
		pJunkcode = 1181884607.87107;
	pJunkcode = 5587422081.33311;
	pJunkcode = 2247427970.35672;
	if (pJunkcode = 8961448162.34216)
		pJunkcode = 2797901539.51969;
	pJunkcode = 3955746772.6051;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3558() {
	float pJunkcode = 5609839259.51743;
	pJunkcode = 7256259485.33874;
	if (pJunkcode = 254989089.236507)
		pJunkcode = 6138196082.84104;
	pJunkcode = 5243419493.40144;
	pJunkcode = 8640113250.251;
	if (pJunkcode = 8989262556.98685)
		pJunkcode = 5872192459.00277;
	pJunkcode = 4638487565.21852;
	if (pJunkcode = 9420102891.18493)
		pJunkcode = 9822912158.3798;
	pJunkcode = 3431691061.25553;
	pJunkcode = 1256865683.93719;
	if (pJunkcode = 2326577548.70161)
		pJunkcode = 5913416179.0493;
	pJunkcode = 5262637084.34122;
	if (pJunkcode = 646291026.491965)
		pJunkcode = 5998599542.76663;
	pJunkcode = 65355641.7592505;
	pJunkcode = 2131588989.43654;
	if (pJunkcode = 4435521644.30974)
		pJunkcode = 4931668458.32972;
	pJunkcode = 430691855.163397;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3557() {
	float pJunkcode = 4799206115.4379;
	pJunkcode = 1571616006.38207;
	if (pJunkcode = 797967758.934227)
		pJunkcode = 8083999324.34625;
	pJunkcode = 2396163866.97439;
	pJunkcode = 4653261648.71488;
	if (pJunkcode = 8959028102.92359)
		pJunkcode = 6320677230.73419;
	pJunkcode = 5346678052.64658;
	if (pJunkcode = 4651475813.62725)
		pJunkcode = 8509697831.46305;
	pJunkcode = 6506117975.75759;
	pJunkcode = 3198766159.43305;
	if (pJunkcode = 6479953556.22198)
		pJunkcode = 3147132638.84272;
	pJunkcode = 6312619792.448;
	if (pJunkcode = 3708749378.16195)
		pJunkcode = 8428850804.31559;
	pJunkcode = 6388542121.2162;
	pJunkcode = 4349282483.34404;
	if (pJunkcode = 4854918830.02181)
		pJunkcode = 4166700490.52267;
	pJunkcode = 3233970375.796;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3556() {
	float pJunkcode = 1444574742.52465;
	pJunkcode = 3966081397.54245;
	if (pJunkcode = 3821542832.00556)
		pJunkcode = 8357891739.60879;
	pJunkcode = 2952200668.49968;
	pJunkcode = 1919540151.89642;
	if (pJunkcode = 9295502045.25397)
		pJunkcode = 5506104724.69071;
	pJunkcode = 6892043937.47989;
	if (pJunkcode = 2639913676.61578)
		pJunkcode = 771308239.299214;
	pJunkcode = 6421995987.85506;
	pJunkcode = 1240006755.58924;
	if (pJunkcode = 1433383674.92736)
		pJunkcode = 4486704285.8826;
	pJunkcode = 1693032799.37368;
	if (pJunkcode = 734284313.657034)
		pJunkcode = 3624672943.11932;
	pJunkcode = 7725297116.07004;
	pJunkcode = 2516749560.66818;
	if (pJunkcode = 747974534.7799)
		pJunkcode = 1931322012.03071;
	pJunkcode = 7820071842.23715;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3555() {
	float pJunkcode = 1606299590.69256;
	pJunkcode = 9156225725.60304;
	if (pJunkcode = 5103337550.52696)
		pJunkcode = 1450938562.03406;
	pJunkcode = 6436839521.7096;
	pJunkcode = 5275054505.24099;
	if (pJunkcode = 694021008.082936)
		pJunkcode = 7587978572.88932;
	pJunkcode = 292524301.202573;
	if (pJunkcode = 4739935125.29171)
		pJunkcode = 3947046197.32005;
	pJunkcode = 9047348534.82623;
	pJunkcode = 9335676203.19441;
	if (pJunkcode = 440291496.988069)
		pJunkcode = 7050434653.90468;
	pJunkcode = 8868570514.30346;
	if (pJunkcode = 4817639298.47022)
		pJunkcode = 7285783438.18667;
	pJunkcode = 9191434221.35043;
	pJunkcode = 2790537347.54238;
	if (pJunkcode = 71254763.8906722)
		pJunkcode = 6248306460.43138;
	pJunkcode = 8085530687.39837;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3554() {
	float pJunkcode = 254265586.888053;
	pJunkcode = 2255291879.41893;
	if (pJunkcode = 757086815.718369)
		pJunkcode = 3912642905.70525;
	pJunkcode = 3904218055.02434;
	pJunkcode = 8796783055.76388;
	if (pJunkcode = 1328852741.53494)
		pJunkcode = 3145109875.33313;
	pJunkcode = 8486063201.00275;
	if (pJunkcode = 8897935498.96255)
		pJunkcode = 7947175383.01235;
	pJunkcode = 9267168750.79949;
	pJunkcode = 4641011724.50516;
	if (pJunkcode = 3778157915.718)
		pJunkcode = 4685531227.14299;
	pJunkcode = 9409770436.20747;
	if (pJunkcode = 2439433535.57414)
		pJunkcode = 4039206089.21312;
	pJunkcode = 4994153813.33363;
	pJunkcode = 4141892687.0754;
	if (pJunkcode = 8807481356.20149)
		pJunkcode = 2507008587.39954;
	pJunkcode = 9421643315.91507;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3553() {
	float pJunkcode = 3331449547.19916;
	pJunkcode = 8910182333.54052;
	if (pJunkcode = 1574743202.64682)
		pJunkcode = 895094318.838673;
	pJunkcode = 1052776000.86652;
	pJunkcode = 8993924068.56675;
	if (pJunkcode = 5465608271.82649)
		pJunkcode = 6672587986.06253;
	pJunkcode = 2249191398.04729;
	if (pJunkcode = 8549408477.12163)
		pJunkcode = 1927237816.88493;
	pJunkcode = 365337531.45098;
	pJunkcode = 7397327523.64883;
	if (pJunkcode = 4892387290.10987)
		pJunkcode = 4005806490.198;
	pJunkcode = 3366809998.34654;
	if (pJunkcode = 3524448039.03496)
		pJunkcode = 811153757.744295;
	pJunkcode = 804360446.826455;
	pJunkcode = 9293253682.74825;
	if (pJunkcode = 5631489869.09981)
		pJunkcode = 4304636771.46132;
	pJunkcode = 9694897616.28552;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3552() {
	float pJunkcode = 5441080137.09502;
	pJunkcode = 7745832249.85027;
	if (pJunkcode = 500581809.498752)
		pJunkcode = 7059057001.20406;
	pJunkcode = 9013386171.14671;
	pJunkcode = 5339693128.6818;
	if (pJunkcode = 2462250680.20823)
		pJunkcode = 4562231106.86328;
	pJunkcode = 3266015130.54627;
	if (pJunkcode = 2416078217.60232)
		pJunkcode = 6479255199.33158;
	pJunkcode = 9116338142.06623;
	pJunkcode = 4467944370.21882;
	if (pJunkcode = 9576904735.96371)
		pJunkcode = 5903619480.65167;
	pJunkcode = 5106303059.16813;
	if (pJunkcode = 8508840755.51009)
		pJunkcode = 127604783.124136;
	pJunkcode = 9070553247.80229;
	pJunkcode = 3101159945.25964;
	if (pJunkcode = 1052507519.55127)
		pJunkcode = 6280859910.12638;
	pJunkcode = 5482374844.1666;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3551() {
	float pJunkcode = 7968951084.26371;
	pJunkcode = 3090275153.99802;
	if (pJunkcode = 4727609015.00064)
		pJunkcode = 1975581247.53626;
	pJunkcode = 4878724016.53622;
	pJunkcode = 9112372456.53294;
	if (pJunkcode = 6547336873.9297)
		pJunkcode = 6690334642.21115;
	pJunkcode = 8364046155.65969;
	if (pJunkcode = 1681120885.99901)
		pJunkcode = 5899253186.06247;
	pJunkcode = 7287130268.50091;
	pJunkcode = 4019890028.19394;
	if (pJunkcode = 5355720613.14603)
		pJunkcode = 263282359.323293;
	pJunkcode = 5675180121.09665;
	if (pJunkcode = 9383134923.50005)
		pJunkcode = 6802707270.51622;
	pJunkcode = 4133488949.26612;
	pJunkcode = 3417023876.22358;
	if (pJunkcode = 1727333334.5398)
		pJunkcode = 1214789323.31712;
	pJunkcode = 9521209613.88731;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3550() {
	float pJunkcode = 143144028.939414;
	pJunkcode = 6667858893.73582;
	if (pJunkcode = 150200745.51523)
		pJunkcode = 829310771.47039;
	pJunkcode = 2927557015.99956;
	pJunkcode = 5641227154.36794;
	if (pJunkcode = 4763009307.72934)
		pJunkcode = 1403865440.34285;
	pJunkcode = 11039529.1985482;
	if (pJunkcode = 4513323507.5928)
		pJunkcode = 8680947320.47042;
	pJunkcode = 8976289691.54491;
	pJunkcode = 4096264525.84849;
	if (pJunkcode = 2803212905.19535)
		pJunkcode = 5109513789.57585;
	pJunkcode = 6086503824.32332;
	if (pJunkcode = 2478484632.16592)
		pJunkcode = 1638420904.04947;
	pJunkcode = 9684227485.74579;
	pJunkcode = 7691804162.2136;
	if (pJunkcode = 439303976.64317)
		pJunkcode = 2809239235.40513;
	pJunkcode = 5934054288.13683;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3549() {
	float pJunkcode = 9616865110.94502;
	pJunkcode = 8522502164.12894;
	if (pJunkcode = 1975132915.66056)
		pJunkcode = 9397636278.89269;
	pJunkcode = 9837462086.41558;
	pJunkcode = 5143542551.23434;
	if (pJunkcode = 5748557810.9996)
		pJunkcode = 8590181848.99924;
	pJunkcode = 4571505059.12734;
	if (pJunkcode = 7805597960.58571)
		pJunkcode = 4424918649.81473;
	pJunkcode = 6088704652.60019;
	pJunkcode = 6699105224.31405;
	if (pJunkcode = 3797064436.38708)
		pJunkcode = 3240733449.30053;
	pJunkcode = 7393196525.35509;
	if (pJunkcode = 5084153382.09185)
		pJunkcode = 4084779223.79599;
	pJunkcode = 2046756330.35401;
	pJunkcode = 8825404198.19288;
	if (pJunkcode = 2400983748.37437)
		pJunkcode = 3491362388.04851;
	pJunkcode = 3116357359.52721;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3548() {
	float pJunkcode = 9274440456.38511;
	pJunkcode = 1666105464.3124;
	if (pJunkcode = 7924723951.12628)
		pJunkcode = 3880604779.57712;
	pJunkcode = 4446565077.80057;
	pJunkcode = 3158746492.39296;
	if (pJunkcode = 9779065445.75505)
		pJunkcode = 2333546167.69948;
	pJunkcode = 4562989943.23687;
	if (pJunkcode = 2862223055.4516)
		pJunkcode = 8385710995.7087;
	pJunkcode = 8578679785.63632;
	pJunkcode = 9931382125.44825;
	if (pJunkcode = 5357435985.07231)
		pJunkcode = 7169807521.62112;
	pJunkcode = 909850872.691176;
	if (pJunkcode = 2536021812.71154)
		pJunkcode = 8182523331.44069;
	pJunkcode = 1839004371.96917;
	pJunkcode = 3400725766.09532;
	if (pJunkcode = 4067780794.03463)
		pJunkcode = 1415256013.86698;
	pJunkcode = 4637914197.55264;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3547() {
	float pJunkcode = 3970011644.27787;
	pJunkcode = 4698609340.29659;
	if (pJunkcode = 1234279798.35559)
		pJunkcode = 9518346095.68967;
	pJunkcode = 9513019640.4109;
	pJunkcode = 6068847624.30349;
	if (pJunkcode = 7774974999.88237)
		pJunkcode = 9996319989.15305;
	pJunkcode = 8889197279.99548;
	if (pJunkcode = 952533879.128245)
		pJunkcode = 9193851449.57669;
	pJunkcode = 4356440503.51985;
	pJunkcode = 2533985298.36165;
	if (pJunkcode = 3919169064.75752)
		pJunkcode = 1862836166.10345;
	pJunkcode = 6442790671.65636;
	if (pJunkcode = 3664246410.99108)
		pJunkcode = 1338762955.3777;
	pJunkcode = 8882808424.45732;
	pJunkcode = 8403822330.84761;
	if (pJunkcode = 8732848130.29502)
		pJunkcode = 5499586709.97291;
	pJunkcode = 5246855881.01237;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3546() {
	float pJunkcode = 9950123333.35689;
	pJunkcode = 6555914238.79856;
	if (pJunkcode = 3376288429.75025)
		pJunkcode = 6105822891.15605;
	pJunkcode = 7324537525.00972;
	pJunkcode = 8977672598.60661;
	if (pJunkcode = 599669573.106217)
		pJunkcode = 7483738937.98705;
	pJunkcode = 1066495576.98338;
	if (pJunkcode = 3501754549.51914)
		pJunkcode = 1213058563.76747;
	pJunkcode = 3087303494.03276;
	pJunkcode = 7075590004.83481;
	if (pJunkcode = 6240445382.56148)
		pJunkcode = 7791649691.18441;
	pJunkcode = 5154988917.40767;
	if (pJunkcode = 3986276047.85092)
		pJunkcode = 9837713373.35974;
	pJunkcode = 7745103066.49092;
	pJunkcode = 1256810131.10116;
	if (pJunkcode = 5890439547.01922)
		pJunkcode = 3450544015.11411;
	pJunkcode = 8192553137.54762;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3545() {
	float pJunkcode = 713388975.846731;
	pJunkcode = 4414681766.20415;
	if (pJunkcode = 9335522920.67302)
		pJunkcode = 4559432771.2808;
	pJunkcode = 9681423005.82406;
	pJunkcode = 8191036940.93197;
	if (pJunkcode = 4381476879.18085)
		pJunkcode = 795627163.819671;
	pJunkcode = 5564647937.76058;
	if (pJunkcode = 6112015090.58595)
		pJunkcode = 4233267642.54073;
	pJunkcode = 4663157732.20719;
	pJunkcode = 94708004.7953561;
	if (pJunkcode = 7074980481.25984)
		pJunkcode = 5179796255.19831;
	pJunkcode = 9035063055.40448;
	if (pJunkcode = 4068744423.86609)
		pJunkcode = 4645906474.17822;
	pJunkcode = 1799317912.51598;
	pJunkcode = 3949876646.91207;
	if (pJunkcode = 925893900.773737)
		pJunkcode = 2174725163.59342;
	pJunkcode = 4815439576.12785;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3544() {
	float pJunkcode = 6523956261.4897;
	pJunkcode = 8229452749.1674;
	if (pJunkcode = 5469659437.94747)
		pJunkcode = 9856210505.01715;
	pJunkcode = 6553754516.36077;
	pJunkcode = 284252579.376371;
	if (pJunkcode = 9766714678.9454)
		pJunkcode = 518960724.907333;
	pJunkcode = 5723989224.67662;
	if (pJunkcode = 7615448039.3697)
		pJunkcode = 4756991848.27837;
	pJunkcode = 1802358326.68276;
	pJunkcode = 758708762.954845;
	if (pJunkcode = 8828940930.15115)
		pJunkcode = 498432603.915959;
	pJunkcode = 1794423970.75718;
	if (pJunkcode = 8188033544.13905)
		pJunkcode = 6394673547.79924;
	pJunkcode = 2574638826.52052;
	pJunkcode = 8804570483.05279;
	if (pJunkcode = 9984915939.52603)
		pJunkcode = 6066140149.97106;
	pJunkcode = 9159067057.59052;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3543() {
	float pJunkcode = 3539222848.95567;
	pJunkcode = 247979383.025839;
	if (pJunkcode = 7818349420.26948)
		pJunkcode = 4556110646.49389;
	pJunkcode = 3890593228.3904;
	pJunkcode = 2859808327.80144;
	if (pJunkcode = 3001186377.01339)
		pJunkcode = 7855950608.79525;
	pJunkcode = 5308314931.13062;
	if (pJunkcode = 5757856228.86304)
		pJunkcode = 9160379431.64237;
	pJunkcode = 5820786339.49973;
	pJunkcode = 3915719189.40228;
	if (pJunkcode = 9599575918.19864)
		pJunkcode = 1752099126.44585;
	pJunkcode = 3783444741.28921;
	if (pJunkcode = 3497736640.59473)
		pJunkcode = 1100661290.58523;
	pJunkcode = 344464789.628679;
	pJunkcode = 7878988814.60161;
	if (pJunkcode = 7559437418.62805)
		pJunkcode = 772697176.61428;
	pJunkcode = 8749288349.51383;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3542() {
	float pJunkcode = 4201478216.63418;
	pJunkcode = 235478011.220415;
	if (pJunkcode = 6408354621.0976)
		pJunkcode = 7195957163.54355;
	pJunkcode = 2223007674.71605;
	pJunkcode = 1400635280.41222;
	if (pJunkcode = 5049982357.67613)
		pJunkcode = 6476914159.51494;
	pJunkcode = 9001760096.4382;
	if (pJunkcode = 5428568108.7376)
		pJunkcode = 766370837.80707;
	pJunkcode = 8434889539.61801;
	pJunkcode = 8218153304.64589;
	if (pJunkcode = 4326653091.46343)
		pJunkcode = 6906512985.6155;
	pJunkcode = 5059452502.72011;
	if (pJunkcode = 5112509158.44846)
		pJunkcode = 6113815121.21597;
	pJunkcode = 7405694982.15001;
	pJunkcode = 7244453466.93845;
	if (pJunkcode = 7807026699.90128)
		pJunkcode = 8277197492.07356;
	pJunkcode = 4458978501.03816;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3541() {
	float pJunkcode = 8985003065.40245;
	pJunkcode = 3140884597.86967;
	if (pJunkcode = 8260113688.82554)
		pJunkcode = 7684494752.4106;
	pJunkcode = 4889244184.68634;
	pJunkcode = 6294073869.28884;
	if (pJunkcode = 5218336284.32482)
		pJunkcode = 3157213981.49724;
	pJunkcode = 5085292680.88723;
	if (pJunkcode = 867683238.714927)
		pJunkcode = 4385390760.41398;
	pJunkcode = 3272072559.63413;
	pJunkcode = 4762538382.19392;
	if (pJunkcode = 7155770209.3589)
		pJunkcode = 8066492954.0039;
	pJunkcode = 9816847125.11916;
	if (pJunkcode = 1191119880.36711)
		pJunkcode = 647487796.628695;
	pJunkcode = 9199648762.87067;
	pJunkcode = 5960675761.29684;
	if (pJunkcode = 7184003969.70858)
		pJunkcode = 3825471602.60945;
	pJunkcode = 443234279.257379;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3540() {
	float pJunkcode = 4433779795.14744;
	pJunkcode = 642911728.383953;
	if (pJunkcode = 375485856.483544)
		pJunkcode = 5364706258.76379;
	pJunkcode = 5857407732.92412;
	pJunkcode = 7358174697.38123;
	if (pJunkcode = 7405626099.89292)
		pJunkcode = 4663531160.88557;
	pJunkcode = 3101789126.52927;
	if (pJunkcode = 9188802358.0134)
		pJunkcode = 6465529294.94111;
	pJunkcode = 4545770608.29821;
	pJunkcode = 2215613729.60832;
	if (pJunkcode = 277110757.066455)
		pJunkcode = 9232344081.84305;
	pJunkcode = 9332425624.22053;
	if (pJunkcode = 8944282156.72913)
		pJunkcode = 1829982330.62915;
	pJunkcode = 7482445963.19043;
	pJunkcode = 8530955745.82396;
	if (pJunkcode = 957641350.063317)
		pJunkcode = 5785062690.15014;
	pJunkcode = 6842413138.53777;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3539() {
	float pJunkcode = 1797289963.91981;
	pJunkcode = 1365965947.52291;
	if (pJunkcode = 2493299486.22858)
		pJunkcode = 4523414812.97094;
	pJunkcode = 7323751745.74349;
	pJunkcode = 1350569420.80836;
	if (pJunkcode = 7211624178.55343)
		pJunkcode = 4190526373.14603;
	pJunkcode = 6680744658.105;
	if (pJunkcode = 6842608667.75697)
		pJunkcode = 3942027219.77107;
	pJunkcode = 8074017208.46315;
	pJunkcode = 7690195579.32243;
	if (pJunkcode = 3367645658.75105)
		pJunkcode = 8951265909.56749;
	pJunkcode = 5625822066.55623;
	if (pJunkcode = 808049357.699902)
		pJunkcode = 9693298176.56229;
	pJunkcode = 9479394440.07136;
	pJunkcode = 3375598098.45188;
	if (pJunkcode = 1745008489.11278)
		pJunkcode = 6530842664.15286;
	pJunkcode = 48658845.7369581;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3538() {
	float pJunkcode = 8175037097.84533;
	pJunkcode = 5145291832.27647;
	if (pJunkcode = 810359652.537502)
		pJunkcode = 2275204833.07697;
	pJunkcode = 5257661007.73762;
	pJunkcode = 6989648719.75916;
	if (pJunkcode = 4701420511.7041)
		pJunkcode = 5149055145.76636;
	pJunkcode = 5288016497.6006;
	if (pJunkcode = 1243880574.05415)
		pJunkcode = 8021433161.61894;
	pJunkcode = 7753449284.06489;
	pJunkcode = 8766237524.44508;
	if (pJunkcode = 8708343869.779)
		pJunkcode = 754174340.710864;
	pJunkcode = 1670407668.25124;
	if (pJunkcode = 6821121417.50547)
		pJunkcode = 9858476939.65671;
	pJunkcode = 1053005292.51974;
	pJunkcode = 2594601391.23599;
	if (pJunkcode = 9655208171.97334)
		pJunkcode = 3554810651.11611;
	pJunkcode = 4230420990.42625;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3537() {
	float pJunkcode = 6431962911.69059;
	pJunkcode = 5102374693.68551;
	if (pJunkcode = 8727992016.15261)
		pJunkcode = 7231092064.58875;
	pJunkcode = 1922176567.02701;
	pJunkcode = 5754147685.90604;
	if (pJunkcode = 5072566171.08573)
		pJunkcode = 988272930.070893;
	pJunkcode = 6006777153.0595;
	if (pJunkcode = 4460160863.75874)
		pJunkcode = 4641651424.95808;
	pJunkcode = 9494185024.11043;
	pJunkcode = 9734347491.90266;
	if (pJunkcode = 2632582618.43602)
		pJunkcode = 9158349871.87426;
	pJunkcode = 8020954864.73905;
	if (pJunkcode = 5568850996.4179)
		pJunkcode = 3441806692.53606;
	pJunkcode = 8782891212.30479;
	pJunkcode = 9704879629.72417;
	if (pJunkcode = 1065464096.32427)
		pJunkcode = 4755672717.54881;
	pJunkcode = 5894082467.31554;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3536() {
	float pJunkcode = 4822184640.36219;
	pJunkcode = 9030531229.19621;
	if (pJunkcode = 4556904011.30664)
		pJunkcode = 4085676194.88077;
	pJunkcode = 3832550645.09242;
	pJunkcode = 3817728214.79638;
	if (pJunkcode = 2076184132.09257)
		pJunkcode = 5312865011.07881;
	pJunkcode = 7319966392.72527;
	if (pJunkcode = 7460441964.46277)
		pJunkcode = 5333159968.48209;
	pJunkcode = 4386931858.01796;
	pJunkcode = 9769080989.94738;
	if (pJunkcode = 8966150842.62478)
		pJunkcode = 7618677912.92151;
	pJunkcode = 3283887281.15685;
	if (pJunkcode = 5839617672.92095)
		pJunkcode = 6257124900.39449;
	pJunkcode = 6670449418.40546;
	pJunkcode = 6325379093.78942;
	if (pJunkcode = 1501554136.27525)
		pJunkcode = 338221977.979591;
	pJunkcode = 576984437.470354;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3535() {
	float pJunkcode = 800347047.786506;
	pJunkcode = 8580857775.78374;
	if (pJunkcode = 8116039562.2272)
		pJunkcode = 1573154002.71289;
	pJunkcode = 8271745372.36817;
	pJunkcode = 5580743135.68375;
	if (pJunkcode = 8040361267.80114)
		pJunkcode = 4120040222.32848;
	pJunkcode = 1311119913.05997;
	if (pJunkcode = 9566563314.76783)
		pJunkcode = 7280072865.08157;
	pJunkcode = 2752639596.27824;
	pJunkcode = 7639985894.68334;
	if (pJunkcode = 5863089169.89384)
		pJunkcode = 1455343906.58831;
	pJunkcode = 4004624632.71486;
	if (pJunkcode = 2279197591.88677)
		pJunkcode = 7851539310.82098;
	pJunkcode = 9995239304.88606;
	pJunkcode = 1960561099.43485;
	if (pJunkcode = 5846162912.25367)
		pJunkcode = 9628708749.74451;
	pJunkcode = 47911266.6124164;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3534() {
	float pJunkcode = 4440417553.46781;
	pJunkcode = 7065496736.71397;
	if (pJunkcode = 2461106147.43233)
		pJunkcode = 377558424.298065;
	pJunkcode = 398349045.245268;
	pJunkcode = 7518965938.92894;
	if (pJunkcode = 8043743953.56114)
		pJunkcode = 6052330012.95267;
	pJunkcode = 5233739439.66151;
	if (pJunkcode = 4876960996.29521)
		pJunkcode = 1972949426.10833;
	pJunkcode = 1117007081.62419;
	pJunkcode = 3112531987.97815;
	if (pJunkcode = 307763419.677983)
		pJunkcode = 9362904929.85332;
	pJunkcode = 35526006.287845;
	if (pJunkcode = 1056359759.48792)
		pJunkcode = 974000123.609139;
	pJunkcode = 6653378881.71484;
	pJunkcode = 7187667912.07349;
	if (pJunkcode = 8823360345.09449)
		pJunkcode = 6007810920.09267;
	pJunkcode = 1135922363.62278;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3533() {
	float pJunkcode = 9637910085.1853;
	pJunkcode = 7981496595.66115;
	if (pJunkcode = 398476865.662098)
		pJunkcode = 6930487268.35048;
	pJunkcode = 7071667280.78226;
	pJunkcode = 3820541223.2193;
	if (pJunkcode = 8103042897.07487)
		pJunkcode = 1248463638.78977;
	pJunkcode = 6349609122.13081;
	if (pJunkcode = 441915581.58848)
		pJunkcode = 8554520937.5675;
	pJunkcode = 3997624134.86407;
	pJunkcode = 3693908070.14611;
	if (pJunkcode = 8501889340.19645)
		pJunkcode = 1978028044.95857;
	pJunkcode = 5279450110.58681;
	if (pJunkcode = 7227526308.25269)
		pJunkcode = 1393990940.03537;
	pJunkcode = 5179744176.16074;
	pJunkcode = 5515396788.66942;
	if (pJunkcode = 8814625667.4461)
		pJunkcode = 4693472567.91803;
	pJunkcode = 4710497898.66295;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3532() {
	float pJunkcode = 6559530295.36386;
	pJunkcode = 6702132886.47399;
	if (pJunkcode = 6677173197.47521)
		pJunkcode = 5192923804.7881;
	pJunkcode = 3621098118.12038;
	pJunkcode = 8089412422.026;
	if (pJunkcode = 4143541796.06314)
		pJunkcode = 1084323588.00194;
	pJunkcode = 9724279.71699889;
	if (pJunkcode = 9578463803.25707)
		pJunkcode = 1422051129.20426;
	pJunkcode = 3787485148.41374;
	pJunkcode = 257844998.604804;
	if (pJunkcode = 9824464939.30864)
		pJunkcode = 6238271031.52063;
	pJunkcode = 2367533665.19564;
	if (pJunkcode = 9064494261.093)
		pJunkcode = 858057785.543219;
	pJunkcode = 9243676518.13684;
	pJunkcode = 2078985601.54417;
	if (pJunkcode = 1253478099.46944)
		pJunkcode = 25074045.1927003;
	pJunkcode = 1712968770.43897;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3531() {
	float pJunkcode = 673147958.927258;
	pJunkcode = 3667467206.56687;
	if (pJunkcode = 6236881393.39234)
		pJunkcode = 4517391267.38781;
	pJunkcode = 8774930169.59771;
	pJunkcode = 2372804439.5828;
	if (pJunkcode = 9950693285.43139)
		pJunkcode = 3282583186.47988;
	pJunkcode = 3830572889.62447;
	if (pJunkcode = 3023438025.90693)
		pJunkcode = 4833830917.58362;
	pJunkcode = 57688526.173999;
	pJunkcode = 2489077184.31099;
	if (pJunkcode = 8878638031.61263)
		pJunkcode = 1204609191.6084;
	pJunkcode = 3492544890.46607;
	if (pJunkcode = 5055944944.14975)
		pJunkcode = 1083897444.66469;
	pJunkcode = 4053367429.11891;
	pJunkcode = 8384143395.25759;
	if (pJunkcode = 6914347758.54233)
		pJunkcode = 8757187699.7022;
	pJunkcode = 4476772917.2591;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3530() {
	float pJunkcode = 1151964846.88708;
	pJunkcode = 1431357253.68494;
	if (pJunkcode = 8945515739.3508)
		pJunkcode = 5614228906.78666;
	pJunkcode = 7353224013.0337;
	pJunkcode = 2850664249.49995;
	if (pJunkcode = 2377041165.21452)
		pJunkcode = 7998199421.93681;
	pJunkcode = 1763205020.78883;
	if (pJunkcode = 7619058477.78776)
		pJunkcode = 2932818642.56221;
	pJunkcode = 5984653357.17875;
	pJunkcode = 4643870071.58592;
	if (pJunkcode = 1176282050.80629)
		pJunkcode = 6238321452.8206;
	pJunkcode = 5692916791.23883;
	if (pJunkcode = 1185147391.70524)
		pJunkcode = 9197946617.92098;
	pJunkcode = 7804256119.78227;
	pJunkcode = 2641372425.17291;
	if (pJunkcode = 2040617746.62856)
		pJunkcode = 564262500.416974;
	pJunkcode = 3553351464.84174;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3529() {
	float pJunkcode = 3747249507.50007;
	pJunkcode = 634865067.720476;
	if (pJunkcode = 8338574311.88741)
		pJunkcode = 7836948223.39561;
	pJunkcode = 478182542.796727;
	pJunkcode = 7747769074.10039;
	if (pJunkcode = 2740268955.24359)
		pJunkcode = 6598468770.57124;
	pJunkcode = 2426718362.18333;
	if (pJunkcode = 1693195008.53268)
		pJunkcode = 3384368312.68604;
	pJunkcode = 5925453291.19397;
	pJunkcode = 5294010224.86139;
	if (pJunkcode = 2828201374.10218)
		pJunkcode = 7908770147.51072;
	pJunkcode = 7137287126.56204;
	if (pJunkcode = 3672053104.82308)
		pJunkcode = 6016734777.73211;
	pJunkcode = 3118961204.30477;
	pJunkcode = 2815439874.86146;
	if (pJunkcode = 6176345002.3069)
		pJunkcode = 8317281333.58221;
	pJunkcode = 6816122193.11076;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3528() {
	float pJunkcode = 1608779277.44269;
	pJunkcode = 913335646.682597;
	if (pJunkcode = 1556047200.68388)
		pJunkcode = 7636030611.91918;
	pJunkcode = 9240356889.26948;
	pJunkcode = 8846887597.46405;
	if (pJunkcode = 1514693294.68484)
		pJunkcode = 3674841745.43451;
	pJunkcode = 3617704917.83366;
	if (pJunkcode = 2997949644.33942)
		pJunkcode = 5087012956.17783;
	pJunkcode = 4596735944.21953;
	pJunkcode = 4616602611.60848;
	if (pJunkcode = 3730728359.48714)
		pJunkcode = 8941501287.92686;
	pJunkcode = 7442622253.77365;
	if (pJunkcode = 7671175193.96573)
		pJunkcode = 3865625194.8248;
	pJunkcode = 2776701611.05072;
	pJunkcode = 6405187746.23135;
	if (pJunkcode = 4513882132.56975)
		pJunkcode = 9680239488.27504;
	pJunkcode = 8027525091.20656;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3527() {
	float pJunkcode = 5951589224.1852;
	pJunkcode = 3718633245.15256;
	if (pJunkcode = 9746737115.42986)
		pJunkcode = 7775802413.50795;
	pJunkcode = 1032817568.85365;
	pJunkcode = 7667036486.72647;
	if (pJunkcode = 7983042941.21536)
		pJunkcode = 5960948698.94604;
	pJunkcode = 6865044644.49948;
	if (pJunkcode = 207037052.296363)
		pJunkcode = 8765952211.97092;
	pJunkcode = 9899497556.57893;
	pJunkcode = 3899403533.33218;
	if (pJunkcode = 6640134531.47239)
		pJunkcode = 5707191346.24123;
	pJunkcode = 261388089.104364;
	if (pJunkcode = 1064183482.38291)
		pJunkcode = 2960987814.00893;
	pJunkcode = 9337945212.91514;
	pJunkcode = 4835177950.45849;
	if (pJunkcode = 9417893450.58867)
		pJunkcode = 502669685.775006;
	pJunkcode = 1809434803.39769;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3526() {
	float pJunkcode = 4735648530.16312;
	pJunkcode = 532081722.708329;
	if (pJunkcode = 3430780807.31535)
		pJunkcode = 7375629400.79214;
	pJunkcode = 8178017467.91787;
	pJunkcode = 93673401.9085323;
	if (pJunkcode = 2075571744.72932)
		pJunkcode = 230455829.953813;
	pJunkcode = 730935408.957304;
	if (pJunkcode = 8165506943.63137)
		pJunkcode = 2552911411.53347;
	pJunkcode = 4118881768.7476;
	pJunkcode = 1357730858.90384;
	if (pJunkcode = 8727047385.11168)
		pJunkcode = 2297684281.19423;
	pJunkcode = 939349372.747678;
	if (pJunkcode = 4341491326.14552)
		pJunkcode = 298053505.706608;
	pJunkcode = 8150521840.84798;
	pJunkcode = 728791015.935789;
	if (pJunkcode = 5136518672.76412)
		pJunkcode = 7447839640.6281;
	pJunkcode = 1558443199.62435;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3525() {
	float pJunkcode = 1543848365.99139;
	pJunkcode = 198517468.430516;
	if (pJunkcode = 5236441531.00329)
		pJunkcode = 8377127719.32892;
	pJunkcode = 2130406636.02909;
	pJunkcode = 9504680084.05503;
	if (pJunkcode = 293346955.817391)
		pJunkcode = 6119216791.682;
	pJunkcode = 9598278601.15393;
	if (pJunkcode = 2852898786.66031)
		pJunkcode = 8022103122.768;
	pJunkcode = 4326686311.40746;
	pJunkcode = 1509068993.78369;
	if (pJunkcode = 966137611.516818)
		pJunkcode = 8876276379.50524;
	pJunkcode = 9641118692.19879;
	if (pJunkcode = 8452649061.85933)
		pJunkcode = 7440952383.48503;
	pJunkcode = 7021178204.49307;
	pJunkcode = 38122931.4078738;
	if (pJunkcode = 553620763.290292)
		pJunkcode = 9858468031.64536;
	pJunkcode = 2120476641.09266;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3524() {
	float pJunkcode = 1055210120.78706;
	pJunkcode = 9415890908.21827;
	if (pJunkcode = 5884058903.33574)
		pJunkcode = 8726583246.52164;
	pJunkcode = 2714965502.22208;
	pJunkcode = 1622882147.17366;
	if (pJunkcode = 3024615056.85909)
		pJunkcode = 6134793764.99749;
	pJunkcode = 9583547353.41102;
	if (pJunkcode = 8002452000.84853)
		pJunkcode = 9889400330.37673;
	pJunkcode = 3531056556.44721;
	pJunkcode = 6977792019.29119;
	if (pJunkcode = 5010303164.79467)
		pJunkcode = 1132721623.87807;
	pJunkcode = 7876877613.58472;
	if (pJunkcode = 9691888354.54734)
		pJunkcode = 4602448396.24914;
	pJunkcode = 7234328909.02031;
	pJunkcode = 3699246651.12354;
	if (pJunkcode = 9002534699.9453)
		pJunkcode = 1047229294.65802;
	pJunkcode = 76260295.6044462;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3523() {
	float pJunkcode = 8266212612.57576;
	pJunkcode = 2943738511.73781;
	if (pJunkcode = 6695041906.88329)
		pJunkcode = 7879809834.76165;
	pJunkcode = 3775079280.45498;
	pJunkcode = 78218572.6555409;
	if (pJunkcode = 2832193189.9573)
		pJunkcode = 7428706047.15964;
	pJunkcode = 9625603434.68484;
	if (pJunkcode = 4687156899.63252)
		pJunkcode = 4504828943.45771;
	pJunkcode = 5803201936.7475;
	pJunkcode = 3387206126.63767;
	if (pJunkcode = 3982528587.64102)
		pJunkcode = 5653023808.41339;
	pJunkcode = 7654244363.57744;
	if (pJunkcode = 1575941407.00597)
		pJunkcode = 4322105460.86912;
	pJunkcode = 1613937856.52126;
	pJunkcode = 5431821507.26239;
	if (pJunkcode = 7011820892.70727)
		pJunkcode = 4428658948.22565;
	pJunkcode = 108140957.614639;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3522() {
	float pJunkcode = 3430011484.35444;
	pJunkcode = 3841486125.52137;
	if (pJunkcode = 5605780908.31464)
		pJunkcode = 3652453377.42593;
	pJunkcode = 4491437368.88663;
	pJunkcode = 2867102948.34986;
	if (pJunkcode = 6246176793.80081)
		pJunkcode = 2934977834.15144;
	pJunkcode = 1720642735.83326;
	if (pJunkcode = 6625367861.22959)
		pJunkcode = 5221765682.42972;
	pJunkcode = 7903819495.75915;
	pJunkcode = 609230097.445631;
	if (pJunkcode = 3556160389.45601)
		pJunkcode = 4734477881.03509;
	pJunkcode = 5395145347.96167;
	if (pJunkcode = 9753726409.58212)
		pJunkcode = 6607848238.29952;
	pJunkcode = 9883795635.94801;
	pJunkcode = 4793149093.79959;
	if (pJunkcode = 2518577193.09498)
		pJunkcode = 8123013692.27129;
	pJunkcode = 3979086239.62522;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3521() {
	float pJunkcode = 6271780723.40465;
	pJunkcode = 6405442062.2719;
	if (pJunkcode = 9973125075.92078)
		pJunkcode = 9434123055.64172;
	pJunkcode = 6461421812.30722;
	pJunkcode = 3838434598.4591;
	if (pJunkcode = 5192426483.50657)
		pJunkcode = 2428990801.46514;
	pJunkcode = 9720608772.64171;
	if (pJunkcode = 2879923811.82614)
		pJunkcode = 3423994874.72683;
	pJunkcode = 5600489782.40444;
	pJunkcode = 7230936956.69336;
	if (pJunkcode = 3609380190.31319)
		pJunkcode = 696980742.581885;
	pJunkcode = 4164154305.6774;
	if (pJunkcode = 2811798216.49461)
		pJunkcode = 1208931479.31772;
	pJunkcode = 7369818343.6427;
	pJunkcode = 4237224531.82027;
	if (pJunkcode = 9742809727.82615)
		pJunkcode = 202582625.204961;
	pJunkcode = 1448849316.72731;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3520() {
	float pJunkcode = 8283417204.82549;
	pJunkcode = 9053502671.48315;
	if (pJunkcode = 3319765803.24522)
		pJunkcode = 3970982531.36792;
	pJunkcode = 7240283751.62416;
	pJunkcode = 2753570697.66577;
	if (pJunkcode = 7254232933.71246)
		pJunkcode = 8578730546.86563;
	pJunkcode = 4306333917.58708;
	if (pJunkcode = 4498904054.21663)
		pJunkcode = 7717938609.76018;
	pJunkcode = 266356317.065457;
	pJunkcode = 4800351295.47652;
	if (pJunkcode = 2750683705.71738)
		pJunkcode = 5271440176.26179;
	pJunkcode = 4730182850.53664;
	if (pJunkcode = 6412861134.21142)
		pJunkcode = 82130370.0752482;
	pJunkcode = 9791656049.01233;
	pJunkcode = 2313405661.61574;
	if (pJunkcode = 6429281518.78604)
		pJunkcode = 1734649282.16189;
	pJunkcode = 1430568876.0547;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3519() {
	float pJunkcode = 8538061188.70961;
	pJunkcode = 7060775818.98941;
	if (pJunkcode = 9453091748.45585)
		pJunkcode = 3313100624.73415;
	pJunkcode = 3467497716.22781;
	pJunkcode = 1626560473.67189;
	if (pJunkcode = 7653585480.39962)
		pJunkcode = 9317139595.49615;
	pJunkcode = 2680559351.47557;
	if (pJunkcode = 504330792.405836)
		pJunkcode = 5029078095.5565;
	pJunkcode = 942082141.548283;
	pJunkcode = 9609397301.05426;
	if (pJunkcode = 5986532006.946)
		pJunkcode = 8724532769.45768;
	pJunkcode = 7916404404.03582;
	if (pJunkcode = 4891473990.77533)
		pJunkcode = 8983510593.99213;
	pJunkcode = 7275349123.14954;
	pJunkcode = 2551871358.13579;
	if (pJunkcode = 2052678160.23103)
		pJunkcode = 3969322188.41396;
	pJunkcode = 7725998.10476523;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3518() {
	float pJunkcode = 7923184879.12864;
	pJunkcode = 3815294139.11322;
	if (pJunkcode = 5746240792.08025)
		pJunkcode = 7870254006.56936;
	pJunkcode = 5971489011.40201;
	pJunkcode = 3521859383.97669;
	if (pJunkcode = 9706549891.16436)
		pJunkcode = 8079067592.22359;
	pJunkcode = 5358287014.47392;
	if (pJunkcode = 7223586231.5058)
		pJunkcode = 8036579337.35916;
	pJunkcode = 8935527881.92144;
	pJunkcode = 5643923429.6584;
	if (pJunkcode = 7070673958.35574)
		pJunkcode = 2929020330.31996;
	pJunkcode = 7890556549.51915;
	if (pJunkcode = 7142821523.13751)
		pJunkcode = 8830233241.38707;
	pJunkcode = 3614127063.73834;
	pJunkcode = 3229044473.69435;
	if (pJunkcode = 7286401962.90431)
		pJunkcode = 4957767082.9117;
	pJunkcode = 987383290.749756;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3517() {
	float pJunkcode = 7151650149.33941;
	pJunkcode = 9032774767.49924;
	if (pJunkcode = 9351556225.84513)
		pJunkcode = 7226672207.8456;
	pJunkcode = 2765067352.42692;
	pJunkcode = 4984965837.58157;
	if (pJunkcode = 1366501641.23909)
		pJunkcode = 8039350434.35703;
	pJunkcode = 3762609799.1439;
	if (pJunkcode = 4086363206.04737)
		pJunkcode = 591282391.718446;
	pJunkcode = 5630706080.51567;
	pJunkcode = 9007237790.00575;
	if (pJunkcode = 6499814863.90311)
		pJunkcode = 6395157956.4224;
	pJunkcode = 2612691681.02348;
	if (pJunkcode = 1432999980.84855)
		pJunkcode = 4950479260.9501;
	pJunkcode = 496016546.816123;
	pJunkcode = 8897794367.75093;
	if (pJunkcode = 8907884825.634)
		pJunkcode = 4115293444.36294;
	pJunkcode = 1982584206.38235;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3516() {
	float pJunkcode = 8169170022.20323;
	pJunkcode = 8301712737.4295;
	if (pJunkcode = 2171557455.20181)
		pJunkcode = 5407900328.00956;
	pJunkcode = 7595673246.03398;
	pJunkcode = 5622756530.21423;
	if (pJunkcode = 6161757721.62449)
		pJunkcode = 8862566589.16247;
	pJunkcode = 252191300.800565;
	if (pJunkcode = 5213248270.85206)
		pJunkcode = 3266559804.45057;
	pJunkcode = 213619329.957476;
	pJunkcode = 3622018176.46091;
	if (pJunkcode = 5487259484.43421)
		pJunkcode = 613219729.371151;
	pJunkcode = 53379218.6432557;
	if (pJunkcode = 2033286052.8275)
		pJunkcode = 1466777443.92146;
	pJunkcode = 5672567463.76736;
	pJunkcode = 2302720192.34659;
	if (pJunkcode = 3772533666.03908)
		pJunkcode = 3954929640.67792;
	pJunkcode = 2618394775.68309;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3515() {
	float pJunkcode = 5071622715.82779;
	pJunkcode = 9534014603.12905;
	if (pJunkcode = 5894400210.44574)
		pJunkcode = 8991542621.39598;
	pJunkcode = 9440485347.79738;
	pJunkcode = 3926808338.73283;
	if (pJunkcode = 7641138592.74827)
		pJunkcode = 3833025260.95664;
	pJunkcode = 6159401277.88651;
	if (pJunkcode = 949332578.267298)
		pJunkcode = 5308067008.55562;
	pJunkcode = 8221616753.89191;
	pJunkcode = 4751851194.6518;
	if (pJunkcode = 7540163017.57934)
		pJunkcode = 8855548361.43546;
	pJunkcode = 9416634752.77663;
	if (pJunkcode = 4121597056.79829)
		pJunkcode = 6142862886.60855;
	pJunkcode = 826159607.858864;
	pJunkcode = 804351801.911107;
	if (pJunkcode = 5755458683.94255)
		pJunkcode = 9691904908.64056;
	pJunkcode = 1297101071.068;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3514() {
	float pJunkcode = 1072208843.9121;
	pJunkcode = 103853540.602102;
	if (pJunkcode = 4590724242.61279)
		pJunkcode = 9004896042.39104;
	pJunkcode = 1090848445.54393;
	pJunkcode = 3977274276.54404;
	if (pJunkcode = 4239694418.8266)
		pJunkcode = 1267921501.56611;
	pJunkcode = 8331281563.27865;
	if (pJunkcode = 1569804315.71002)
		pJunkcode = 3006105168.53829;
	pJunkcode = 5177540737.87846;
	pJunkcode = 2610528475.47408;
	if (pJunkcode = 143768218.369967)
		pJunkcode = 7510603849.44871;
	pJunkcode = 862549178.786245;
	if (pJunkcode = 4826078633.40062)
		pJunkcode = 4767576004.63439;
	pJunkcode = 8001642934.09419;
	pJunkcode = 6885895500.46014;
	if (pJunkcode = 6400037190.41653)
		pJunkcode = 8732657103.97543;
	pJunkcode = 1026825755.44161;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3513() {
	float pJunkcode = 5055455257.30867;
	pJunkcode = 4443031707.88317;
	if (pJunkcode = 2124491893.446)
		pJunkcode = 2625880934.77339;
	pJunkcode = 1773165022.63855;
	pJunkcode = 421994371.838378;
	if (pJunkcode = 5978272119.78307)
		pJunkcode = 7967720924.21828;
	pJunkcode = 674249788.661849;
	if (pJunkcode = 5714837463.04072)
		pJunkcode = 746187665.934547;
	pJunkcode = 1661259827.44417;
	pJunkcode = 9700761474.2403;
	if (pJunkcode = 4399100462.28199)
		pJunkcode = 4584741323.2424;
	pJunkcode = 9700507898.5938;
	if (pJunkcode = 7635720724.41634)
		pJunkcode = 1631917063.91951;
	pJunkcode = 9837831196.12871;
	pJunkcode = 1851719973.76409;
	if (pJunkcode = 7919272113.14314)
		pJunkcode = 1554615772.20127;
	pJunkcode = 6493740978.50993;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3512() {
	float pJunkcode = 5489113198.28889;
	pJunkcode = 5508543717.70788;
	if (pJunkcode = 2897983996.91245)
		pJunkcode = 6526590304.09976;
	pJunkcode = 9456134586.36539;
	pJunkcode = 9274009910.26441;
	if (pJunkcode = 716457199.027488)
		pJunkcode = 8026052626.73338;
	pJunkcode = 8644062656.42848;
	if (pJunkcode = 6336070672.47357)
		pJunkcode = 1698234076.837;
	pJunkcode = 9472256482.71148;
	pJunkcode = 1330158226.19694;
	if (pJunkcode = 9359693307.71315)
		pJunkcode = 3252241573.1845;
	pJunkcode = 4000441033.84813;
	if (pJunkcode = 3493549907.85887)
		pJunkcode = 2764106388.47967;
	pJunkcode = 4131452136.89599;
	pJunkcode = 8802007974.61434;
	if (pJunkcode = 8005127712.74218)
		pJunkcode = 8481461603.13323;
	pJunkcode = 5403165317.18708;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3511() {
	float pJunkcode = 4543268600.29749;
	pJunkcode = 2712017721.12884;
	if (pJunkcode = 4940777346.95963)
		pJunkcode = 2405699082.87593;
	pJunkcode = 1789492750.85681;
	pJunkcode = 5489408401.30989;
	if (pJunkcode = 6374804060.85939)
		pJunkcode = 7897445410.84931;
	pJunkcode = 298115936.291079;
	if (pJunkcode = 7911440245.91368)
		pJunkcode = 4742911700.2627;
	pJunkcode = 6869079400.88079;
	pJunkcode = 5229997178.50789;
	if (pJunkcode = 3182045281.9781)
		pJunkcode = 9515833044.64195;
	pJunkcode = 2272981655.61012;
	if (pJunkcode = 3918646369.84618)
		pJunkcode = 2881590427.17491;
	pJunkcode = 4578390225.47214;
	pJunkcode = 9512038858.11745;
	if (pJunkcode = 6961140594.52271)
		pJunkcode = 5602814559.16325;
	pJunkcode = 9735864913.19525;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3510() {
	float pJunkcode = 287568031.303755;
	pJunkcode = 2285895680.68021;
	if (pJunkcode = 2858692835.70131)
		pJunkcode = 3756995356.19363;
	pJunkcode = 2105412572.25784;
	pJunkcode = 9175005492.20898;
	if (pJunkcode = 3668202254.87148)
		pJunkcode = 1131236966.18601;
	pJunkcode = 9872142322.65489;
	if (pJunkcode = 1875033804.0584)
		pJunkcode = 4063058910.07256;
	pJunkcode = 2163584207.63444;
	pJunkcode = 4567702553.64713;
	if (pJunkcode = 1842633236.42009)
		pJunkcode = 6688128228.383;
	pJunkcode = 9116828064.0639;
	if (pJunkcode = 8270287792.60918)
		pJunkcode = 8023428747.07928;
	pJunkcode = 4442540477.39037;
	pJunkcode = 2001005273.02914;
	if (pJunkcode = 5678176134.9037)
		pJunkcode = 744294774.059245;
	pJunkcode = 5516599861.46864;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3509() {
	float pJunkcode = 6220396785.45521;
	pJunkcode = 3835019326.04809;
	if (pJunkcode = 9703321901.19362)
		pJunkcode = 2257371469.33544;
	pJunkcode = 5268404753.93409;
	pJunkcode = 5864302759.54807;
	if (pJunkcode = 8547183690.81112)
		pJunkcode = 2085382911.07846;
	pJunkcode = 7106471712.30153;
	if (pJunkcode = 2271588534.37023)
		pJunkcode = 12286019.8220159;
	pJunkcode = 4106694188.97587;
	pJunkcode = 9272933661.37683;
	if (pJunkcode = 7370517724.97769)
		pJunkcode = 5015636891.30173;
	pJunkcode = 7227853131.68719;
	if (pJunkcode = 942997939.130646)
		pJunkcode = 4231238019.90897;
	pJunkcode = 3117248051.23233;
	pJunkcode = 6329723071.7846;
	if (pJunkcode = 7238459315.93012)
		pJunkcode = 1470089415.06553;
	pJunkcode = 7414381583.07957;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3508() {
	float pJunkcode = 9300903336.20985;
	pJunkcode = 8422876907.04242;
	if (pJunkcode = 86798251.8378559)
		pJunkcode = 524301436.263848;
	pJunkcode = 8154704352.40648;
	pJunkcode = 5150931260.03326;
	if (pJunkcode = 1911295221.62446)
		pJunkcode = 2073204742.22724;
	pJunkcode = 6131163082.15742;
	if (pJunkcode = 8078133014.70931)
		pJunkcode = 7800810521.48319;
	pJunkcode = 203927644.114908;
	pJunkcode = 1532423126.65939;
	if (pJunkcode = 9171821520.75955)
		pJunkcode = 5013573205.88744;
	pJunkcode = 5929631497.75692;
	if (pJunkcode = 8660194080.10924)
		pJunkcode = 7744212400.39691;
	pJunkcode = 3201443084.36222;
	pJunkcode = 9385107347.19958;
	if (pJunkcode = 6515203115.89934)
		pJunkcode = 9783031501.48536;
	pJunkcode = 2077916125.63228;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3507() {
	float pJunkcode = 680694723.228035;
	pJunkcode = 2212589634.37817;
	if (pJunkcode = 7200254133.50186)
		pJunkcode = 8490166499.71181;
	pJunkcode = 1192149521.68253;
	pJunkcode = 5838365217.23605;
	if (pJunkcode = 768304125.395081)
		pJunkcode = 5055779503.33373;
	pJunkcode = 3400760147.85264;
	if (pJunkcode = 3983040085.83867)
		pJunkcode = 8984542163.126;
	pJunkcode = 8979653213.20628;
	pJunkcode = 9406277862.46749;
	if (pJunkcode = 1464361140.02361)
		pJunkcode = 8461664791.22372;
	pJunkcode = 3575148225.41422;
	if (pJunkcode = 7140697812.25524)
		pJunkcode = 4641648686.89709;
	pJunkcode = 941077347.67067;
	pJunkcode = 8866481646.17859;
	if (pJunkcode = 318445156.709238)
		pJunkcode = 5502447674.04393;
	pJunkcode = 4082648204.4329;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3506() {
	float pJunkcode = 1987976936.24108;
	pJunkcode = 5936127458.97669;
	if (pJunkcode = 3305539855.81021)
		pJunkcode = 5452634475.07129;
	pJunkcode = 1011755670.32313;
	pJunkcode = 165452001.330718;
	if (pJunkcode = 5458646435.11745)
		pJunkcode = 4849097086.0527;
	pJunkcode = 4279858851.52832;
	if (pJunkcode = 6430648842.6486)
		pJunkcode = 6492818568.60341;
	pJunkcode = 1304290187.79361;
	pJunkcode = 2544252908.32501;
	if (pJunkcode = 2877704464.38507)
		pJunkcode = 9136821494.82914;
	pJunkcode = 1634257312.11988;
	if (pJunkcode = 2706913926.55287)
		pJunkcode = 6385150965.16209;
	pJunkcode = 104682912.313817;
	pJunkcode = 9882081311.74915;
	if (pJunkcode = 6009070809.98568)
		pJunkcode = 3370939085.1198;
	pJunkcode = 2278879462.26156;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3505() {
	float pJunkcode = 3781334816.1715;
	pJunkcode = 5429384912.37685;
	if (pJunkcode = 7646447273.91344)
		pJunkcode = 8689159794.47522;
	pJunkcode = 3345018583.51218;
	pJunkcode = 1462023629.88424;
	if (pJunkcode = 4643406561.63234)
		pJunkcode = 8556839553.68909;
	pJunkcode = 3359016068.16278;
	if (pJunkcode = 4793635146.19066)
		pJunkcode = 6215728261.08646;
	pJunkcode = 490317054.010544;
	pJunkcode = 7145339600.94855;
	if (pJunkcode = 434816920.991658)
		pJunkcode = 9389568728.87276;
	pJunkcode = 7634106278.90818;
	if (pJunkcode = 4246114277.78323)
		pJunkcode = 439789575.019025;
	pJunkcode = 7246505834.56833;
	pJunkcode = 1631093065.88531;
	if (pJunkcode = 4647205865.72321)
		pJunkcode = 6015580726.93468;
	pJunkcode = 2321758919.91797;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3504() {
	float pJunkcode = 743188383.217958;
	pJunkcode = 7046970024.2191;
	if (pJunkcode = 3091149005.2311)
		pJunkcode = 3648626445.92716;
	pJunkcode = 1394904588.61239;
	pJunkcode = 4871673728.35019;
	if (pJunkcode = 679460858.884108)
		pJunkcode = 4739629135.33486;
	pJunkcode = 724145879.171305;
	if (pJunkcode = 1371431920.51307)
		pJunkcode = 5204671231.94284;
	pJunkcode = 228204761.983065;
	pJunkcode = 2636345100.72883;
	if (pJunkcode = 2838757281.3987)
		pJunkcode = 5865349569.62347;
	pJunkcode = 6006102253.62187;
	if (pJunkcode = 3423867315.07771)
		pJunkcode = 6515687505.89624;
	pJunkcode = 4803692237.44787;
	pJunkcode = 6913995866.48312;
	if (pJunkcode = 2260727608.01093)
		pJunkcode = 7745582238.80527;
	pJunkcode = 9878918187.74707;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3503() {
	float pJunkcode = 7420827928.58535;
	pJunkcode = 5089595450.59888;
	if (pJunkcode = 3678131050.74209)
		pJunkcode = 9272605189.51785;
	pJunkcode = 1005379040.59981;
	pJunkcode = 6642391483.01467;
	if (pJunkcode = 2963946237.68447)
		pJunkcode = 9604477987.99393;
	pJunkcode = 3567055741.81;
	if (pJunkcode = 7755635772.44582)
		pJunkcode = 5295686516.55494;
	pJunkcode = 8893207896.73706;
	pJunkcode = 5802212774.96298;
	if (pJunkcode = 9968377357.32885)
		pJunkcode = 6727097776.85733;
	pJunkcode = 1819704094.38149;
	if (pJunkcode = 6738059098.78619)
		pJunkcode = 2034006798.4887;
	pJunkcode = 2275588166.27189;
	pJunkcode = 6145648593.58321;
	if (pJunkcode = 8452430801.6117)
		pJunkcode = 5415178601.68196;
	pJunkcode = 5221941817.05027;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3502() {
	float pJunkcode = 7149209875.7665;
	pJunkcode = 7535715857.05966;
	if (pJunkcode = 2789299106.02612)
		pJunkcode = 4886158108.23036;
	pJunkcode = 1287178552.24877;
	pJunkcode = 5871763503.09947;
	if (pJunkcode = 3502662761.55499)
		pJunkcode = 3463655860.43403;
	pJunkcode = 2740073325.9082;
	if (pJunkcode = 7742001423.13378)
		pJunkcode = 3178361004.38813;
	pJunkcode = 8688291701.08638;
	pJunkcode = 2937350594.49776;
	if (pJunkcode = 8646134872.67225)
		pJunkcode = 9912939503.55603;
	pJunkcode = 4790743413.85777;
	if (pJunkcode = 5523141210.24021)
		pJunkcode = 5062868995.15149;
	pJunkcode = 9711744200.07619;
	pJunkcode = 4715329044.29683;
	if (pJunkcode = 2499683473.32667)
		pJunkcode = 9390993158.53622;
	pJunkcode = 1369597349.80247;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3501() {
	float pJunkcode = 8720230258.18546;
	pJunkcode = 7038948266.87223;
	if (pJunkcode = 22894537.0046677)
		pJunkcode = 9587026891.09363;
	pJunkcode = 2148254125.46451;
	pJunkcode = 1995196611.68307;
	if (pJunkcode = 1437260701.76646)
		pJunkcode = 2589658958.32395;
	pJunkcode = 2387286751.94753;
	if (pJunkcode = 1886417634.87452)
		pJunkcode = 9676777121.68069;
	pJunkcode = 4842935924.96552;
	pJunkcode = 8817602242.41709;
	if (pJunkcode = 324064617.109708)
		pJunkcode = 9385662398.2619;
	pJunkcode = 81389587.2092308;
	if (pJunkcode = 1484771682.71575)
		pJunkcode = 1510479316.37952;
	pJunkcode = 6931263252.52949;
	pJunkcode = 6457434294.4628;
	if (pJunkcode = 8782900031.84138)
		pJunkcode = 9485420819.17509;
	pJunkcode = 9464884258.15729;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3500() {
	float pJunkcode = 1653635617.46087;
	pJunkcode = 6835053393.58127;
	if (pJunkcode = 2236084223.26337)
		pJunkcode = 7821268176.98493;
	pJunkcode = 2818134832.34366;
	pJunkcode = 315654364.764171;
	if (pJunkcode = 1370356046.47907)
		pJunkcode = 9928710928.09563;
	pJunkcode = 8769823152.67785;
	if (pJunkcode = 4991666138.24401)
		pJunkcode = 3164129352.65939;
	pJunkcode = 1084124745.63581;
	pJunkcode = 406885318.244757;
	if (pJunkcode = 3226911817.54821)
		pJunkcode = 3821595567.61898;
	pJunkcode = 3233238868.81006;
	if (pJunkcode = 8059347061.85503)
		pJunkcode = 1521665182.59225;
	pJunkcode = 5243182569.63605;
	pJunkcode = 5618376761.85042;
	if (pJunkcode = 5356165191.82929)
		pJunkcode = 4533456130.10978;
	pJunkcode = 3011480687.72794;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3499() {
	float pJunkcode = 2871925754.07348;
	pJunkcode = 8297581420.16098;
	if (pJunkcode = 3438606555.81668)
		pJunkcode = 626959975.985848;
	pJunkcode = 4156657473.73713;
	pJunkcode = 5931431221.05356;
	if (pJunkcode = 6696113890.35614)
		pJunkcode = 5114927727.76582;
	pJunkcode = 4069921687.64656;
	if (pJunkcode = 7268772407.75688)
		pJunkcode = 4720767275.79321;
	pJunkcode = 5703676854.04364;
	pJunkcode = 1203517862.43083;
	if (pJunkcode = 480476995.995752)
		pJunkcode = 5471029937.58824;
	pJunkcode = 358886853.010254;
	if (pJunkcode = 6630357886.57756)
		pJunkcode = 8438112223.43144;
	pJunkcode = 3042570781.19345;
	pJunkcode = 9398408095.44486;
	if (pJunkcode = 4488694406.86238)
		pJunkcode = 3165627737.90893;
	pJunkcode = 42226961.1978453;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3498() {
	float pJunkcode = 8459042939.50425;
	pJunkcode = 7233997948.22664;
	if (pJunkcode = 3771367864.39657)
		pJunkcode = 8966045394.68077;
	pJunkcode = 3765911767.98355;
	pJunkcode = 576354264.352098;
	if (pJunkcode = 1568991197.743)
		pJunkcode = 5611946013.51349;
	pJunkcode = 8663055807.85713;
	if (pJunkcode = 8253773836.66291)
		pJunkcode = 7977216433.57892;
	pJunkcode = 130929578.451814;
	pJunkcode = 5867888289.99295;
	if (pJunkcode = 887673465.76254)
		pJunkcode = 4128638308.9174;
	pJunkcode = 639743170.168989;
	if (pJunkcode = 9465156192.31645)
		pJunkcode = 8428933437.41298;
	pJunkcode = 2290408596.5727;
	pJunkcode = 3657592601.48665;
	if (pJunkcode = 7232732954.73696)
		pJunkcode = 9556038119.85439;
	pJunkcode = 9172329412.44659;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3497() {
	float pJunkcode = 9598475185.08541;
	pJunkcode = 2807298420.47768;
	if (pJunkcode = 2773858297.69428)
		pJunkcode = 9458436494.72707;
	pJunkcode = 6403889798.22192;
	pJunkcode = 7236043300.73939;
	if (pJunkcode = 7613777265.70722)
		pJunkcode = 484245475.945961;
	pJunkcode = 4104869988.43844;
	if (pJunkcode = 4432528594.40715)
		pJunkcode = 9559301949.0877;
	pJunkcode = 629486731.308205;
	pJunkcode = 3632340030.76517;
	if (pJunkcode = 2487728468.0874)
		pJunkcode = 1446924762.52941;
	pJunkcode = 8071809753.89243;
	if (pJunkcode = 1672387984.83067)
		pJunkcode = 2550020533.6805;
	pJunkcode = 6081982184.35385;
	pJunkcode = 872015922.208529;
	if (pJunkcode = 3038075145.81852)
		pJunkcode = 8813847522.62032;
	pJunkcode = 8133365069.20593;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3496() {
	float pJunkcode = 8698894281.14699;
	pJunkcode = 9122912955.25156;
	if (pJunkcode = 3477690318.2211)
		pJunkcode = 2924693353.44836;
	pJunkcode = 3128618666.71327;
	pJunkcode = 6486117329.64286;
	if (pJunkcode = 2648845853.21093)
		pJunkcode = 9463626438.10799;
	pJunkcode = 8390806415.45017;
	if (pJunkcode = 4569942545.97492)
		pJunkcode = 6025524859.20642;
	pJunkcode = 7378610145.71826;
	pJunkcode = 8318122910.15421;
	if (pJunkcode = 4795130241.5761)
		pJunkcode = 3774074477.26423;
	pJunkcode = 1103197466.14122;
	if (pJunkcode = 7976079975.28899)
		pJunkcode = 7688916676.66918;
	pJunkcode = 1355357414.04228;
	pJunkcode = 1575629542.79073;
	if (pJunkcode = 3788397551.35926)
		pJunkcode = 997228086.820765;
	pJunkcode = 8039124243.97697;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3495() {
	float pJunkcode = 7512685683.83018;
	pJunkcode = 6454738389.36936;
	if (pJunkcode = 5037018903.1547)
		pJunkcode = 6714936946.73075;
	pJunkcode = 5145904058.98818;
	pJunkcode = 3436620998.27188;
	if (pJunkcode = 1654607093.60656)
		pJunkcode = 1213222549.40652;
	pJunkcode = 5078380592.60432;
	if (pJunkcode = 6477665841.0744)
		pJunkcode = 57418131.0099973;
	pJunkcode = 7838983159.16953;
	pJunkcode = 2491902183.39215;
	if (pJunkcode = 6341218532.1365)
		pJunkcode = 4291275601.93994;
	pJunkcode = 1029308147.65677;
	if (pJunkcode = 7018994954.75401)
		pJunkcode = 4243243602.44318;
	pJunkcode = 2918956112.18802;
	pJunkcode = 9214216215.89375;
	if (pJunkcode = 7474846116.36204)
		pJunkcode = 6742317088.62796;
	pJunkcode = 6591015274.9653;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3494() {
	float pJunkcode = 9444402528.20774;
	pJunkcode = 1984269552.76909;
	if (pJunkcode = 3249254369.60091)
		pJunkcode = 1395300289.64741;
	pJunkcode = 8382743850.28448;
	pJunkcode = 8903921414.83335;
	if (pJunkcode = 6469427308.92455)
		pJunkcode = 1656457368.58703;
	pJunkcode = 4854063944.9833;
	if (pJunkcode = 4495262367.91039)
		pJunkcode = 8684419957.12176;
	pJunkcode = 7511884339.99107;
	pJunkcode = 5587510258.07269;
	if (pJunkcode = 9084776168.46921)
		pJunkcode = 4393976169.50113;
	pJunkcode = 1444324566.17155;
	if (pJunkcode = 1805555746.29569)
		pJunkcode = 7255546108.69372;
	pJunkcode = 8180471508.62907;
	pJunkcode = 8685182774.85954;
	if (pJunkcode = 4347680759.57503)
		pJunkcode = 2504636173.96741;
	pJunkcode = 3302728351.45821;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3493() {
	float pJunkcode = 1440195349.01443;
	pJunkcode = 8129374508.29543;
	if (pJunkcode = 2989035549.19123)
		pJunkcode = 8457737010.14017;
	pJunkcode = 6209559372.42748;
	pJunkcode = 5709279846.49737;
	if (pJunkcode = 2219935016.15589)
		pJunkcode = 7578300393.41733;
	pJunkcode = 4335148128.24;
	if (pJunkcode = 49068819.1477134)
		pJunkcode = 7335384309.37715;
	pJunkcode = 5162203661.54594;
	pJunkcode = 1448945861.31655;
	if (pJunkcode = 6390517990.7748)
		pJunkcode = 7509739288.06542;
	pJunkcode = 1374932592.02186;
	if (pJunkcode = 4776558032.10014)
		pJunkcode = 5234484189.92202;
	pJunkcode = 6509702104.57601;
	pJunkcode = 3725397517.72291;
	if (pJunkcode = 2404695911.81167)
		pJunkcode = 5059430386.35843;
	pJunkcode = 3651898631.36871;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3492() {
	float pJunkcode = 9332506448.24191;
	pJunkcode = 6872708207.12464;
	if (pJunkcode = 1240614232.60332)
		pJunkcode = 5317670366.3202;
	pJunkcode = 3590190576.02378;
	pJunkcode = 2210202622.61067;
	if (pJunkcode = 7962398243.0865)
		pJunkcode = 7754122069.42142;
	pJunkcode = 3279385485.14469;
	if (pJunkcode = 6909060458.00135)
		pJunkcode = 9941460702.10239;
	pJunkcode = 1351866560.90354;
	pJunkcode = 2970220832.72688;
	if (pJunkcode = 9269551750.93512)
		pJunkcode = 1526141160.27252;
	pJunkcode = 5605753842.02298;
	if (pJunkcode = 7363523834.79448)
		pJunkcode = 8937733429.62826;
	pJunkcode = 7677171461.38344;
	pJunkcode = 1455214507.5955;
	if (pJunkcode = 3116856278.01098)
		pJunkcode = 4910336823.53933;
	pJunkcode = 4958314536.78609;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3491() {
	float pJunkcode = 7629178579.37194;
	pJunkcode = 8477075783.5615;
	if (pJunkcode = 1043334363.32148)
		pJunkcode = 6750095086.3701;
	pJunkcode = 4132746604.12211;
	pJunkcode = 4461530110.78318;
	if (pJunkcode = 8362695837.57728)
		pJunkcode = 6967853884.67685;
	pJunkcode = 993981221.458508;
	if (pJunkcode = 9869292494.76219)
		pJunkcode = 3986190690.66224;
	pJunkcode = 8501925170.1459;
	pJunkcode = 6560505899.55558;
	if (pJunkcode = 1520723943.85987)
		pJunkcode = 8719529586.98161;
	pJunkcode = 4158030948.92378;
	if (pJunkcode = 531948561.209946)
		pJunkcode = 5599642501.53871;
	pJunkcode = 2591689980.25388;
	pJunkcode = 2570305602.86522;
	if (pJunkcode = 8609204055.07336)
		pJunkcode = 3117033562.8034;
	pJunkcode = 3131317265.63031;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3490() {
	float pJunkcode = 9963464481.97144;
	pJunkcode = 8341232254.32403;
	if (pJunkcode = 2970315230.63904)
		pJunkcode = 4550136630.47828;
	pJunkcode = 1533975544.37607;
	pJunkcode = 9513883361.28397;
	if (pJunkcode = 9415534971.93122)
		pJunkcode = 9488802657.44002;
	pJunkcode = 6070305292.53724;
	if (pJunkcode = 9982181745.81614)
		pJunkcode = 3785154273.88135;
	pJunkcode = 2428368605.04496;
	pJunkcode = 6296460131.34916;
	if (pJunkcode = 4471694037.64115)
		pJunkcode = 1596657352.9276;
	pJunkcode = 5090538051.3769;
	if (pJunkcode = 341399455.847314)
		pJunkcode = 4776703072.49917;
	pJunkcode = 6229069633.21619;
	pJunkcode = 3681667312.38716;
	if (pJunkcode = 8353194891.67548)
		pJunkcode = 6886486001.96175;
	pJunkcode = 4734414328.21814;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3489() {
	float pJunkcode = 4824039560.26754;
	pJunkcode = 2186123122.43171;
	if (pJunkcode = 1338785553.69399)
		pJunkcode = 176516354.445677;
	pJunkcode = 5776469886.43569;
	pJunkcode = 149295725.555583;
	if (pJunkcode = 8149291950.88339)
		pJunkcode = 5320703388.18796;
	pJunkcode = 4147266494.97694;
	if (pJunkcode = 1550999878.61579)
		pJunkcode = 5246783504.00227;
	pJunkcode = 9084861863.25894;
	pJunkcode = 5532272198.93381;
	if (pJunkcode = 4449339539.1676)
		pJunkcode = 3795022573.97079;
	pJunkcode = 4669804931.71575;
	if (pJunkcode = 1692732276.22568)
		pJunkcode = 425483483.421313;
	pJunkcode = 5543726572.0729;
	pJunkcode = 6000121641.58008;
	if (pJunkcode = 7306748638.46127)
		pJunkcode = 1868653511.94568;
	pJunkcode = 6676657671.34771;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3488() {
	float pJunkcode = 6411375866.50917;
	pJunkcode = 3120476232.12228;
	if (pJunkcode = 4178645553.99288)
		pJunkcode = 2069719300.01855;
	pJunkcode = 7963533038.98226;
	pJunkcode = 4991960719.6134;
	if (pJunkcode = 2633382751.56789)
		pJunkcode = 6179335234.98688;
	pJunkcode = 4316395074.02667;
	if (pJunkcode = 7266547283.84531)
		pJunkcode = 7670977778.83717;
	pJunkcode = 3352426784.69766;
	pJunkcode = 7583082763.84743;
	if (pJunkcode = 8332292194.16213)
		pJunkcode = 1674559087.91651;
	pJunkcode = 2488479050.69721;
	if (pJunkcode = 402821649.243247)
		pJunkcode = 8561057280.53028;
	pJunkcode = 4138773204.41274;
	pJunkcode = 9376450061.6832;
	if (pJunkcode = 6490866235.6592)
		pJunkcode = 2762455023.38555;
	pJunkcode = 4832017771.7346;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3487() {
	float pJunkcode = 9379576782.97819;
	pJunkcode = 9390775149.74316;
	if (pJunkcode = 6969473673.59485)
		pJunkcode = 6489868123.95424;
	pJunkcode = 2039556170.58141;
	pJunkcode = 5217224282.32113;
	if (pJunkcode = 5156967583.23433)
		pJunkcode = 7492890.39546269;
	pJunkcode = 3339128306.52846;
	if (pJunkcode = 8537800812.14801)
		pJunkcode = 5854678326.16438;
	pJunkcode = 150061723.72815;
	pJunkcode = 2121187875.71718;
	if (pJunkcode = 9141119724.38864)
		pJunkcode = 44714685.1814208;
	pJunkcode = 8078752049.32096;
	if (pJunkcode = 6565666907.03348)
		pJunkcode = 9985914328.29669;
	pJunkcode = 1350769199.14432;
	pJunkcode = 2944386414.73228;
	if (pJunkcode = 8434407459.278)
		pJunkcode = 562110573.071175;
	pJunkcode = 7983028529.14264;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3486() {
	float pJunkcode = 7614390060.52002;
	pJunkcode = 6854659994.07029;
	if (pJunkcode = 7659000976.21102)
		pJunkcode = 5400611712.1034;
	pJunkcode = 2169352338.22737;
	pJunkcode = 3495945464.42554;
	if (pJunkcode = 31192484.6123079)
		pJunkcode = 5327627830.34128;
	pJunkcode = 9608339948.51274;
	if (pJunkcode = 6139456922.67358)
		pJunkcode = 2631874135.84219;
	pJunkcode = 6905600608.28109;
	pJunkcode = 7179417424.18964;
	if (pJunkcode = 5598053231.75629)
		pJunkcode = 7383081955.77168;
	pJunkcode = 1188239207.37919;
	if (pJunkcode = 1966817431.42421)
		pJunkcode = 1501317876.07056;
	pJunkcode = 6871971484.78444;
	pJunkcode = 6199556795.59203;
	if (pJunkcode = 8353315438.53574)
		pJunkcode = 224429218.021769;
	pJunkcode = 3918604209.29485;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3485() {
	float pJunkcode = 1464275282.25406;
	pJunkcode = 7059204721.457;
	if (pJunkcode = 5397215941.39208)
		pJunkcode = 5287798719.30758;
	pJunkcode = 1036125495.76988;
	pJunkcode = 9306279339.6824;
	if (pJunkcode = 7940880520.26509)
		pJunkcode = 4324575364.17344;
	pJunkcode = 8316897564.99835;
	if (pJunkcode = 1222693923.784)
		pJunkcode = 347594656.434201;
	pJunkcode = 3330213987.86509;
	pJunkcode = 8435031539.27924;
	if (pJunkcode = 1200171111.39301)
		pJunkcode = 4377477447.51154;
	pJunkcode = 3985868966.16784;
	if (pJunkcode = 2326349768.10417)
		pJunkcode = 7483924038.82602;
	pJunkcode = 3581198122.50608;
	pJunkcode = 519142729.129705;
	if (pJunkcode = 1074484063.16399)
		pJunkcode = 1219555684.47226;
	pJunkcode = 3870374899.44226;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3484() {
	float pJunkcode = 9660003687.89012;
	pJunkcode = 7537377140.64001;
	if (pJunkcode = 2757677046.42576)
		pJunkcode = 7087914769.37821;
	pJunkcode = 7965563762.89195;
	pJunkcode = 8261454128.00252;
	if (pJunkcode = 7776385951.11465)
		pJunkcode = 3548118315.51034;
	pJunkcode = 7294527128.61915;
	if (pJunkcode = 1270529632.71296)
		pJunkcode = 1937000180.09089;
	pJunkcode = 2794489109.94241;
	pJunkcode = 2174895782.51117;
	if (pJunkcode = 4545148402.5649)
		pJunkcode = 5604331929.23362;
	pJunkcode = 8036164292.94769;
	if (pJunkcode = 2652281973.0693)
		pJunkcode = 2135305706.99569;
	pJunkcode = 1373772238.56193;
	pJunkcode = 6470030916.33202;
	if (pJunkcode = 6785658894.42465)
		pJunkcode = 3732576387.67436;
	pJunkcode = 7757643891.05437;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3483() {
	float pJunkcode = 9623641255.43118;
	pJunkcode = 4463373482.40409;
	if (pJunkcode = 1194618025.68868)
		pJunkcode = 7655737552.09898;
	pJunkcode = 3804987579.301;
	pJunkcode = 7972787128.66274;
	if (pJunkcode = 4237092692.78671)
		pJunkcode = 3095890665.63026;
	pJunkcode = 1847076280.53516;
	if (pJunkcode = 5999513659.32856)
		pJunkcode = 9731419589.69098;
	pJunkcode = 9698220888.91602;
	pJunkcode = 7616678986.37484;
	if (pJunkcode = 9097099245.00732)
		pJunkcode = 1915457421.24073;
	pJunkcode = 2007509541.5632;
	if (pJunkcode = 6808602795.45017)
		pJunkcode = 1767325229.81844;
	pJunkcode = 9253202020.44106;
	pJunkcode = 1825826931.92797;
	if (pJunkcode = 1732365366.77242)
		pJunkcode = 396098310.219012;
	pJunkcode = 526198449.200331;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3482() {
	float pJunkcode = 723848997.458152;
	pJunkcode = 8977231696.28245;
	if (pJunkcode = 8855938155.61612)
		pJunkcode = 6836090147.80173;
	pJunkcode = 709292028.694111;
	pJunkcode = 837180596.700095;
	if (pJunkcode = 3073598183.30678)
		pJunkcode = 4472537667.7589;
	pJunkcode = 2263515619.98591;
	if (pJunkcode = 7672777995.72984)
		pJunkcode = 4939051392.98642;
	pJunkcode = 2524033319.77568;
	pJunkcode = 3339503611.80306;
	if (pJunkcode = 3779227929.4633)
		pJunkcode = 5275521570.48294;
	pJunkcode = 2040503358.07725;
	if (pJunkcode = 6094498343.08558)
		pJunkcode = 1178091280.48305;
	pJunkcode = 3358722194.59146;
	pJunkcode = 2098707961.94329;
	if (pJunkcode = 1525002529.69441)
		pJunkcode = 3398787575.14298;
	pJunkcode = 8661710546.80574;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3481() {
	float pJunkcode = 6917745205.61753;
	pJunkcode = 3241159764.93074;
	if (pJunkcode = 45444860.389392)
		pJunkcode = 4468776053.91571;
	pJunkcode = 2155908081.59338;
	pJunkcode = 3882711859.53282;
	if (pJunkcode = 6104948252.96327)
		pJunkcode = 8795064687.33718;
	pJunkcode = 6988994248.67792;
	if (pJunkcode = 983570931.97524)
		pJunkcode = 2013906404.06713;
	pJunkcode = 8774394948.52301;
	pJunkcode = 3606494879.38524;
	if (pJunkcode = 1386435677.50109)
		pJunkcode = 1275160769.51084;
	pJunkcode = 4787492597.399;
	if (pJunkcode = 1243401960.52776)
		pJunkcode = 8204914644.72747;
	pJunkcode = 766312998.596977;
	pJunkcode = 8564544357.19374;
	if (pJunkcode = 2910986052.04028)
		pJunkcode = 6179187850.00671;
	pJunkcode = 5567456035.83432;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3480() {
	float pJunkcode = 2826135847.01591;
	pJunkcode = 7329282300.65175;
	if (pJunkcode = 2698487965.2809)
		pJunkcode = 1827665255.83101;
	pJunkcode = 340784058.013949;
	pJunkcode = 626828921.583718;
	if (pJunkcode = 7173972608.3085)
		pJunkcode = 5643843981.65236;
	pJunkcode = 3682492679.45185;
	if (pJunkcode = 6882062683.06155)
		pJunkcode = 3085349282.16405;
	pJunkcode = 3693462702.17486;
	pJunkcode = 1236117963.93537;
	if (pJunkcode = 494390275.957832)
		pJunkcode = 4618802697.16302;
	pJunkcode = 9731036938.68303;
	if (pJunkcode = 3146535306.15834)
		pJunkcode = 1060110090.47095;
	pJunkcode = 6188825170.43765;
	pJunkcode = 3492040891.66846;
	if (pJunkcode = 3621800619.35479)
		pJunkcode = 7402544982.68006;
	pJunkcode = 4381444327.45408;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3479() {
	float pJunkcode = 2797990342.56744;
	pJunkcode = 9286682643.70992;
	if (pJunkcode = 1611325259.72733)
		pJunkcode = 5894559993.5337;
	pJunkcode = 6135279321.46425;
	pJunkcode = 4425899481.00213;
	if (pJunkcode = 4889986748.68897)
		pJunkcode = 9784092793.48198;
	pJunkcode = 2765747164.54201;
	if (pJunkcode = 9596694758.59954)
		pJunkcode = 6425971722.25844;
	pJunkcode = 2427258481.96578;
	pJunkcode = 2401828512.36601;
	if (pJunkcode = 3324056216.75818)
		pJunkcode = 9706906358.29997;
	pJunkcode = 6370874852.53826;
	if (pJunkcode = 5522696887.66809)
		pJunkcode = 4565273860.95462;
	pJunkcode = 2577299418.54337;
	pJunkcode = 4880872556.23003;
	if (pJunkcode = 8111762096.29686)
		pJunkcode = 804019206.662666;
	pJunkcode = 4923409332.51538;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3478() {
	float pJunkcode = 9107107377.12638;
	pJunkcode = 7313859147.69106;
	if (pJunkcode = 8513769158.18785)
		pJunkcode = 3122321550.18791;
	pJunkcode = 6336300532.22787;
	pJunkcode = 7458664285.35102;
	if (pJunkcode = 6663550741.03352)
		pJunkcode = 3735420408.82888;
	pJunkcode = 1875909997.54433;
	if (pJunkcode = 6875388093.62998)
		pJunkcode = 4898015509.51156;
	pJunkcode = 1993352720.85501;
	pJunkcode = 1994882114.17958;
	if (pJunkcode = 6309220298.81192)
		pJunkcode = 4960697257.52774;
	pJunkcode = 7214176361.0561;
	if (pJunkcode = 9666465907.63044)
		pJunkcode = 8500632109.2028;
	pJunkcode = 9103095104.97548;
	pJunkcode = 5420878031.62389;
	if (pJunkcode = 319758450.432938)
		pJunkcode = 1836718989.76131;
	pJunkcode = 5780400196.03475;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3477() {
	float pJunkcode = 5106053956.15357;
	pJunkcode = 8085127449.71078;
	if (pJunkcode = 7968024753.52836)
		pJunkcode = 7502264928.71414;
	pJunkcode = 988745578.53585;
	pJunkcode = 4089101998.11649;
	if (pJunkcode = 5507117851.49372)
		pJunkcode = 2404891597.02585;
	pJunkcode = 5306057211.25491;
	if (pJunkcode = 2588847702.98345)
		pJunkcode = 1905782529.30455;
	pJunkcode = 3974835360.3602;
	pJunkcode = 7916570629.48327;
	if (pJunkcode = 7863438893.60807)
		pJunkcode = 1418147932.46252;
	pJunkcode = 3065894897.84274;
	if (pJunkcode = 1790542033.56891)
		pJunkcode = 4326974760.07903;
	pJunkcode = 6579343859.27066;
	pJunkcode = 2059092652.49499;
	if (pJunkcode = 1737237392.4019)
		pJunkcode = 7892800409.12525;
	pJunkcode = 9151171099.7107;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3476() {
	float pJunkcode = 448540676.376781;
	pJunkcode = 9031829768.58957;
	if (pJunkcode = 4215069902.48572)
		pJunkcode = 3548960304.18846;
	pJunkcode = 5686909909.17823;
	pJunkcode = 9369615889.39602;
	if (pJunkcode = 720295597.320789)
		pJunkcode = 2905477050.04917;
	pJunkcode = 360727239.772168;
	if (pJunkcode = 6650095748.68591)
		pJunkcode = 5354894081.93976;
	pJunkcode = 9323167902.41833;
	pJunkcode = 7666179673.86951;
	if (pJunkcode = 5162114933.00325)
		pJunkcode = 8193084391.22783;
	pJunkcode = 8817650433.2218;
	if (pJunkcode = 3085638063.78012)
		pJunkcode = 9972449475.66992;
	pJunkcode = 662575087.759683;
	pJunkcode = 7001445781.25417;
	if (pJunkcode = 789224308.314913)
		pJunkcode = 4385186488.49153;
	pJunkcode = 9476722942.37022;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3475() {
	float pJunkcode = 9926135418.52405;
	pJunkcode = 2416229339.47695;
	if (pJunkcode = 6004314574.53398)
		pJunkcode = 5682521640.25262;
	pJunkcode = 1913039953.20131;
	pJunkcode = 6497441172.24354;
	if (pJunkcode = 5202750100.32117)
		pJunkcode = 5647944556.27013;
	pJunkcode = 9668816351.38273;
	if (pJunkcode = 2956948440.64213)
		pJunkcode = 8393867125.2612;
	pJunkcode = 4014011313.46436;
	pJunkcode = 7877188215.8705;
	if (pJunkcode = 8418298289.32762)
		pJunkcode = 9162817411.85638;
	pJunkcode = 8952547503.71606;
	if (pJunkcode = 6900367610.28809)
		pJunkcode = 1825070987.28206;
	pJunkcode = 3859870333.11756;
	pJunkcode = 5912786581.01698;
	if (pJunkcode = 3192911635.56543)
		pJunkcode = 4111835217.45603;
	pJunkcode = 7823292959.11601;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3474() {
	float pJunkcode = 2766614533.83478;
	pJunkcode = 2238034667.96994;
	if (pJunkcode = 5165249009.34472)
		pJunkcode = 1265329065.376;
	pJunkcode = 1778409503.72406;
	pJunkcode = 3287080172.25904;
	if (pJunkcode = 6033153192.57919)
		pJunkcode = 2789610315.9938;
	pJunkcode = 8610881394.49434;
	if (pJunkcode = 6314228605.83921)
		pJunkcode = 4612674907.35827;
	pJunkcode = 4789503433.114;
	pJunkcode = 8607506962.49812;
	if (pJunkcode = 3154966487.44114)
		pJunkcode = 8818274652.92003;
	pJunkcode = 2430728690.21779;
	if (pJunkcode = 5741192457.20899)
		pJunkcode = 6056065887.6256;
	pJunkcode = 5841702671.43923;
	pJunkcode = 3049844717.27433;
	if (pJunkcode = 6823635811.97257)
		pJunkcode = 4221146684.26315;
	pJunkcode = 8844575400.41911;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3473() {
	float pJunkcode = 4312912740.07599;
	pJunkcode = 9182002595.42294;
	if (pJunkcode = 4292503755.00529)
		pJunkcode = 7643576321.50769;
	pJunkcode = 5696638363.26835;
	pJunkcode = 8291944190.30348;
	if (pJunkcode = 7465791853.91147)
		pJunkcode = 563831432.760851;
	pJunkcode = 1502863453.29132;
	if (pJunkcode = 1401418363.52656)
		pJunkcode = 8517444462.57455;
	pJunkcode = 1185935983.53753;
	pJunkcode = 4919381002.19557;
	if (pJunkcode = 2768751595.09265)
		pJunkcode = 8518438835.05677;
	pJunkcode = 9515989847.52747;
	if (pJunkcode = 921231246.006809)
		pJunkcode = 5260565696.96626;
	pJunkcode = 2511323501.43612;
	pJunkcode = 6467362834.22227;
	if (pJunkcode = 5715861951.86553)
		pJunkcode = 9761283036.49034;
	pJunkcode = 9387815010.12295;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3472() {
	float pJunkcode = 7389128984.4187;
	pJunkcode = 2339973637.81373;
	if (pJunkcode = 9162235632.10346)
		pJunkcode = 9001005977.16628;
	pJunkcode = 2686485346.65115;
	pJunkcode = 8473819712.16766;
	if (pJunkcode = 9761805410.0986)
		pJunkcode = 2895524428.85705;
	pJunkcode = 1055870659.3742;
	if (pJunkcode = 3221994406.69522)
		pJunkcode = 7798487770.33072;
	pJunkcode = 4562216583.03297;
	pJunkcode = 5119342748.66485;
	if (pJunkcode = 8645591872.80353)
		pJunkcode = 3401547107.55844;
	pJunkcode = 7136593314.48036;
	if (pJunkcode = 374269822.132004)
		pJunkcode = 5540805403.2561;
	pJunkcode = 3906933140.5144;
	pJunkcode = 7430831823.22907;
	if (pJunkcode = 2619598172.11368)
		pJunkcode = 5602404758.04545;
	pJunkcode = 1660684522.64671;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3471() {
	float pJunkcode = 9250129927.6537;
	pJunkcode = 7372200666.24793;
	if (pJunkcode = 3473190398.80517)
		pJunkcode = 4978479776.22177;
	pJunkcode = 7685213186.08663;
	pJunkcode = 1522367870.08931;
	if (pJunkcode = 9555012846.39816)
		pJunkcode = 1594424836.71702;
	pJunkcode = 4565337383.50389;
	if (pJunkcode = 1991476911.59321)
		pJunkcode = 6585472058.85325;
	pJunkcode = 7129224146.96194;
	pJunkcode = 1694133660.54248;
	if (pJunkcode = 9625079529.06626)
		pJunkcode = 741894959.062687;
	pJunkcode = 7735810138.71916;
	if (pJunkcode = 7728501916.34056)
		pJunkcode = 8165047135.648;
	pJunkcode = 51377097.6921023;
	pJunkcode = 2741086399.16071;
	if (pJunkcode = 6981210781.79865)
		pJunkcode = 5917824920.56867;
	pJunkcode = 2946540798.4168;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3470() {
	float pJunkcode = 3839968287.46609;
	pJunkcode = 908752235.032789;
	if (pJunkcode = 9852531936.37747)
		pJunkcode = 4502897779.26394;
	pJunkcode = 359785966.115541;
	pJunkcode = 1713704310.01133;
	if (pJunkcode = 1927049696.26271)
		pJunkcode = 2887155112.38591;
	pJunkcode = 5250363425.61303;
	if (pJunkcode = 1266552245.22854)
		pJunkcode = 8738125198.03155;
	pJunkcode = 4279747887.88152;
	pJunkcode = 5266731335.34195;
	if (pJunkcode = 5102710036.30807)
		pJunkcode = 3409263920.34759;
	pJunkcode = 5093351448.6231;
	if (pJunkcode = 7108927236.1291)
		pJunkcode = 4609577606.24921;
	pJunkcode = 6361348698.31475;
	pJunkcode = 659562659.556409;
	if (pJunkcode = 4869943092.19955)
		pJunkcode = 5715755633.14362;
	pJunkcode = 7036369716.56285;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3469() {
	float pJunkcode = 4712481414.30342;
	pJunkcode = 8401858386.75532;
	if (pJunkcode = 6351617135.96662)
		pJunkcode = 4456097720.94922;
	pJunkcode = 4253097804.77812;
	pJunkcode = 8656410685.17715;
	if (pJunkcode = 3070897756.68746)
		pJunkcode = 4881204788.2663;
	pJunkcode = 1483289607.0897;
	if (pJunkcode = 414045121.885466)
		pJunkcode = 330068309.422391;
	pJunkcode = 9895151422.59355;
	pJunkcode = 2885734221.71285;
	if (pJunkcode = 4519343525.82695)
		pJunkcode = 5713974151.94548;
	pJunkcode = 2869256398.39284;
	if (pJunkcode = 5987398333.16792)
		pJunkcode = 6787744344.62986;
	pJunkcode = 9622540196.35831;
	pJunkcode = 8709702595.29187;
	if (pJunkcode = 1113368012.3082)
		pJunkcode = 3225722369.862;
	pJunkcode = 7845633113.03706;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3468() {
	float pJunkcode = 3916434084.08792;
	pJunkcode = 3702889890.39465;
	if (pJunkcode = 3042836557.04368)
		pJunkcode = 430040576.158357;
	pJunkcode = 5666400169.98235;
	pJunkcode = 4760268268.86698;
	if (pJunkcode = 8756672781.2566)
		pJunkcode = 9494540031.41251;
	pJunkcode = 1027221556.59281;
	if (pJunkcode = 905601758.264794)
		pJunkcode = 9332948600.85413;
	pJunkcode = 697666412.657382;
	pJunkcode = 391111581.719983;
	if (pJunkcode = 6010791003.78638)
		pJunkcode = 6945805276.04397;
	pJunkcode = 5440169613.074;
	if (pJunkcode = 8969846410.05976)
		pJunkcode = 126937895.308836;
	pJunkcode = 1794140206.59131;
	pJunkcode = 1566646060.14801;
	if (pJunkcode = 6227573154.83056)
		pJunkcode = 7216621477.45797;
	pJunkcode = 6468848663.04076;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3467() {
	float pJunkcode = 9982904870.98272;
	pJunkcode = 9756066753.2421;
	if (pJunkcode = 421809566.691372)
		pJunkcode = 73251674.1441286;
	pJunkcode = 2214289215.88811;
	pJunkcode = 6515537445.71873;
	if (pJunkcode = 2566847543.5947)
		pJunkcode = 5783343036.51114;
	pJunkcode = 8217121758.79485;
	if (pJunkcode = 8865915654.51373)
		pJunkcode = 3723483252.20826;
	pJunkcode = 940874177.026152;
	pJunkcode = 8957468556.0039;
	if (pJunkcode = 2363329651.94222)
		pJunkcode = 9620893408.66887;
	pJunkcode = 7167324775.50692;
	if (pJunkcode = 1593691245.91766)
		pJunkcode = 2933368669.18948;
	pJunkcode = 4468172262.57386;
	pJunkcode = 1629862796.41057;
	if (pJunkcode = 9476754164.6473)
		pJunkcode = 5167116082.88813;
	pJunkcode = 1592673781.52313;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3466() {
	float pJunkcode = 6704259554.27586;
	pJunkcode = 7521707327.03378;
	if (pJunkcode = 7824675761.40497)
		pJunkcode = 486968908.331122;
	pJunkcode = 1562700258.15285;
	pJunkcode = 8383032906.63795;
	if (pJunkcode = 1916294418.07978)
		pJunkcode = 1102936777.58969;
	pJunkcode = 9123917020.37654;
	if (pJunkcode = 1589008302.49134)
		pJunkcode = 6408515493.82207;
	pJunkcode = 2178222240.23302;
	pJunkcode = 8494621795.25283;
	if (pJunkcode = 9834003812.94301)
		pJunkcode = 939200518.620842;
	pJunkcode = 5766557768.05731;
	if (pJunkcode = 4271195493.84947)
		pJunkcode = 5151270546.76748;
	pJunkcode = 9927696425.27727;
	pJunkcode = 4400624339.90377;
	if (pJunkcode = 801774953.576708)
		pJunkcode = 9909634791.99883;
	pJunkcode = 6628250899.75318;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3465() {
	float pJunkcode = 7636747269.11517;
	pJunkcode = 9738304919.0156;
	if (pJunkcode = 6808000990.13385)
		pJunkcode = 8016473548.30551;
	pJunkcode = 8697873273.1055;
	pJunkcode = 7600887892.49916;
	if (pJunkcode = 9142922739.87766)
		pJunkcode = 6310726961.10806;
	pJunkcode = 6318498632.90487;
	if (pJunkcode = 4662069501.9745)
		pJunkcode = 5033582308.32657;
	pJunkcode = 8348865896.18239;
	pJunkcode = 2688199137.67317;
	if (pJunkcode = 6501600107.60683)
		pJunkcode = 6847618420.64811;
	pJunkcode = 8465035612.05277;
	if (pJunkcode = 7912661316.34169)
		pJunkcode = 579772591.470608;
	pJunkcode = 1406022676.17424;
	pJunkcode = 7728733110.20111;
	if (pJunkcode = 201719207.008696)
		pJunkcode = 4177551745.19375;
	pJunkcode = 2962016205.0363;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3464() {
	float pJunkcode = 3138388054.34951;
	pJunkcode = 9618084970.66578;
	if (pJunkcode = 5628185683.73461)
		pJunkcode = 5753130468.30952;
	pJunkcode = 707957612.785644;
	pJunkcode = 7626020517.05743;
	if (pJunkcode = 792925819.856785)
		pJunkcode = 2389287881.05006;
	pJunkcode = 3583434326.71718;
	if (pJunkcode = 3529125485.38476)
		pJunkcode = 6587223263.05726;
	pJunkcode = 4409393552.97617;
	pJunkcode = 5895324767.02933;
	if (pJunkcode = 9647995174.04742)
		pJunkcode = 7435145198.27048;
	pJunkcode = 1890913130.55246;
	if (pJunkcode = 5901376454.57094)
		pJunkcode = 6088430723.10236;
	pJunkcode = 1615075695.4831;
	pJunkcode = 1830904898.85892;
	if (pJunkcode = 4261512089.71008)
		pJunkcode = 4298130009.336;
	pJunkcode = 7783656704.43246;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3463() {
	float pJunkcode = 2711789973.12736;
	pJunkcode = 339638272.621708;
	if (pJunkcode = 65078159.3013303)
		pJunkcode = 6127661532.88589;
	pJunkcode = 3711782276.63071;
	pJunkcode = 5694039165.74396;
	if (pJunkcode = 8734340224.52348)
		pJunkcode = 6717000692.6804;
	pJunkcode = 7854725706.01318;
	if (pJunkcode = 6884228549.83859)
		pJunkcode = 2284341856.68051;
	pJunkcode = 1694414544.62209;
	pJunkcode = 8199664736.41393;
	if (pJunkcode = 4166200256.55892)
		pJunkcode = 7196378824.93573;
	pJunkcode = 337759022.157401;
	if (pJunkcode = 129894000.014349)
		pJunkcode = 450538061.600001;
	pJunkcode = 9494532240.97708;
	pJunkcode = 4289317916.27276;
	if (pJunkcode = 7793090043.97635)
		pJunkcode = 6562706669.62537;
	pJunkcode = 5646720417.16493;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3462() {
	float pJunkcode = 4043383327.21028;
	pJunkcode = 9362802084.92987;
	if (pJunkcode = 3930690902.22926)
		pJunkcode = 2959459668.64413;
	pJunkcode = 393350200.36071;
	pJunkcode = 5190112943.04225;
	if (pJunkcode = 3130639788.78323)
		pJunkcode = 3077361024.15519;
	pJunkcode = 8253364990.1412;
	if (pJunkcode = 947870786.644083)
		pJunkcode = 5018959209.33613;
	pJunkcode = 2373058082.74951;
	pJunkcode = 9106640030.16216;
	if (pJunkcode = 8491908891.36408)
		pJunkcode = 8815479325.47938;
	pJunkcode = 660470759.834668;
	if (pJunkcode = 1575783141.95863)
		pJunkcode = 6759620961.19963;
	pJunkcode = 2121829976.63003;
	pJunkcode = 4851616274.45481;
	if (pJunkcode = 7301840510.47321)
		pJunkcode = 4174006177.40091;
	pJunkcode = 8972061194.06585;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3461() {
	float pJunkcode = 9207969923.46135;
	pJunkcode = 6152957579.63868;
	if (pJunkcode = 5277613998.55276)
		pJunkcode = 4697630404.33168;
	pJunkcode = 2163632689.47454;
	pJunkcode = 8189425475.62625;
	if (pJunkcode = 9770432562.97441)
		pJunkcode = 4256419552.38315;
	pJunkcode = 1241326372.92999;
	if (pJunkcode = 3369699570.03266)
		pJunkcode = 6373776801.37763;
	pJunkcode = 6626612426.36289;
	pJunkcode = 446721237.178397;
	if (pJunkcode = 4870098567.96368)
		pJunkcode = 6149058813.61978;
	pJunkcode = 3245650999.85051;
	if (pJunkcode = 2062876236.01801)
		pJunkcode = 7883479805.05165;
	pJunkcode = 1896293606.65695;
	pJunkcode = 4124229222.81213;
	if (pJunkcode = 5481701495.36555)
		pJunkcode = 6964252293.73479;
	pJunkcode = 369734501.170726;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3460() {
	float pJunkcode = 20422014.3817998;
	pJunkcode = 2363466580.60274;
	if (pJunkcode = 2353038109.55633)
		pJunkcode = 4809909000.26504;
	pJunkcode = 4429209249.59034;
	pJunkcode = 8836110952.62662;
	if (pJunkcode = 7759994325.49354)
		pJunkcode = 1235389630.39937;
	pJunkcode = 1970893202.84155;
	if (pJunkcode = 9671603741.70611)
		pJunkcode = 4982289774.11716;
	pJunkcode = 1054146765.93158;
	pJunkcode = 9642709566.1683;
	if (pJunkcode = 2634248081.28463)
		pJunkcode = 4799741224.75501;
	pJunkcode = 2113814645.34466;
	if (pJunkcode = 265813554.681622)
		pJunkcode = 2986919696.26461;
	pJunkcode = 2597814149.57833;
	pJunkcode = 3011799860.42401;
	if (pJunkcode = 8486487227.93061)
		pJunkcode = 6266313055.42436;
	pJunkcode = 3075689072.38143;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3459() {
	float pJunkcode = 4498078226.15188;
	pJunkcode = 3551424512.51219;
	if (pJunkcode = 1108283253.77529)
		pJunkcode = 754231292.845076;
	pJunkcode = 676269332.155838;
	pJunkcode = 8908000322.4481;
	if (pJunkcode = 3416440699.50077)
		pJunkcode = 7271600401.01017;
	pJunkcode = 6409904505.6422;
	if (pJunkcode = 7428500599.4542)
		pJunkcode = 6773013827.96786;
	pJunkcode = 2566427712.23308;
	pJunkcode = 7523586128.13944;
	if (pJunkcode = 3622413237.21807)
		pJunkcode = 5378450856.9397;
	pJunkcode = 398603859.538409;
	if (pJunkcode = 6233055640.00484)
		pJunkcode = 8140605145.06491;
	pJunkcode = 8294046245.82414;
	pJunkcode = 9771689151.36191;
	if (pJunkcode = 6065842518.55073)
		pJunkcode = 4406523257.14627;
	pJunkcode = 3633373511.41952;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3458() {
	float pJunkcode = 3639070627.048;
	pJunkcode = 5551968110.03631;
	if (pJunkcode = 7480868139.24799)
		pJunkcode = 8105837404.95985;
	pJunkcode = 6177701525.26391;
	pJunkcode = 5468592094.74294;
	if (pJunkcode = 3235861323.55487)
		pJunkcode = 8647796596.29811;
	pJunkcode = 2610376496.79125;
	if (pJunkcode = 2829870192.00682)
		pJunkcode = 2360072812.21493;
	pJunkcode = 1151608400.70602;
	pJunkcode = 6695215096.22565;
	if (pJunkcode = 3234193900.29184)
		pJunkcode = 4639511164.63916;
	pJunkcode = 6718779473.07008;
	if (pJunkcode = 5221812858.10298)
		pJunkcode = 1891476055.8684;
	pJunkcode = 5545369308.54966;
	pJunkcode = 421435439.528044;
	if (pJunkcode = 8985774928.52028)
		pJunkcode = 9383804081.39142;
	pJunkcode = 2721711469.83029;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3457() {
	float pJunkcode = 1376801695.82819;
	pJunkcode = 6151166869.12612;
	if (pJunkcode = 7117786018.25441)
		pJunkcode = 3035603549.85235;
	pJunkcode = 5639555635.65484;
	pJunkcode = 1565521699.61735;
	if (pJunkcode = 6848649791.87791)
		pJunkcode = 6014617658.21945;
	pJunkcode = 4146672121.54044;
	if (pJunkcode = 9725660732.64984)
		pJunkcode = 9728618865.28998;
	pJunkcode = 5649369337.50434;
	pJunkcode = 2994615428.7781;
	if (pJunkcode = 9357689906.20626)
		pJunkcode = 4943859670.22317;
	pJunkcode = 488006884.900235;
	if (pJunkcode = 2217372142.17698)
		pJunkcode = 4287001048.69182;
	pJunkcode = 269327183.086161;
	pJunkcode = 9571390739.80523;
	if (pJunkcode = 8048159814.97039)
		pJunkcode = 7058462427.53522;
	pJunkcode = 7281802500.07045;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3456() {
	float pJunkcode = 6206108130.37004;
	pJunkcode = 210640820.183179;
	if (pJunkcode = 4339037443.75757)
		pJunkcode = 7380127136.27847;
	pJunkcode = 1815234484.55564;
	pJunkcode = 5476468788.40795;
	if (pJunkcode = 3780658710.68319)
		pJunkcode = 1293642427.39841;
	pJunkcode = 275259047.863567;
	if (pJunkcode = 1083386176.96707)
		pJunkcode = 7371937137.19095;
	pJunkcode = 395690744.253037;
	pJunkcode = 515449964.947639;
	if (pJunkcode = 6934236328.09091)
		pJunkcode = 5589431408.32062;
	pJunkcode = 2611360242.98916;
	if (pJunkcode = 6959707741.13081)
		pJunkcode = 8746843729.03627;
	pJunkcode = 7306122181.28293;
	pJunkcode = 5534047456.63108;
	if (pJunkcode = 8106966624.61746)
		pJunkcode = 4138222232.50251;
	pJunkcode = 5206139044.50629;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3455() {
	float pJunkcode = 6179349207.01408;
	pJunkcode = 2431789991.52754;
	if (pJunkcode = 9978450958.44189)
		pJunkcode = 8485123776.00294;
	pJunkcode = 2110362707.13055;
	pJunkcode = 6965637806.14533;
	if (pJunkcode = 709300941.362022)
		pJunkcode = 9268250564.28381;
	pJunkcode = 5345127548.77866;
	if (pJunkcode = 9030560444.49322)
		pJunkcode = 2391855642.09531;
	pJunkcode = 4540032228.5074;
	pJunkcode = 222271702.796402;
	if (pJunkcode = 6659041850.7426)
		pJunkcode = 7462378709.55126;
	pJunkcode = 3695733302.11298;
	if (pJunkcode = 7420305249.97199)
		pJunkcode = 3729200696.07695;
	pJunkcode = 2751726296.9437;
	pJunkcode = 6170141638.75928;
	if (pJunkcode = 7391663660.56034)
		pJunkcode = 8568742077.0978;
	pJunkcode = 3945477525.85869;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3454() {
	float pJunkcode = 2243614517.28912;
	pJunkcode = 9922503215.31508;
	if (pJunkcode = 5982597143.22509)
		pJunkcode = 6297433081.29809;
	pJunkcode = 8975146984.143;
	pJunkcode = 2780029267.83839;
	if (pJunkcode = 8800330793.51661)
		pJunkcode = 9044734045.1381;
	pJunkcode = 9379159102.9502;
	if (pJunkcode = 345965368.120943)
		pJunkcode = 4779466309.21312;
	pJunkcode = 5595530457.79813;
	pJunkcode = 8288102313.55515;
	if (pJunkcode = 3276649882.82349)
		pJunkcode = 3808405145.07092;
	pJunkcode = 3721102747.99218;
	if (pJunkcode = 6065979109.6102)
		pJunkcode = 5654423092.61879;
	pJunkcode = 4160639799.51012;
	pJunkcode = 2502962154.67821;
	if (pJunkcode = 7160452014.25476)
		pJunkcode = 2358669472.14028;
	pJunkcode = 7076912686.19618;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3453() {
	float pJunkcode = 194742853.454198;
	pJunkcode = 201813469.880438;
	if (pJunkcode = 1354503932.95851)
		pJunkcode = 3729747940.40586;
	pJunkcode = 9214107042.69669;
	pJunkcode = 1123847442.48799;
	if (pJunkcode = 4345612224.34693)
		pJunkcode = 4784298185.92791;
	pJunkcode = 1257026200.43434;
	if (pJunkcode = 5620646225.58636)
		pJunkcode = 8496498323.95413;
	pJunkcode = 7824850848.29809;
	pJunkcode = 8985019880.26338;
	if (pJunkcode = 6756568046.4806)
		pJunkcode = 2754675118.77951;
	pJunkcode = 2985912350.58866;
	if (pJunkcode = 6505277807.79689)
		pJunkcode = 8733671739.50594;
	pJunkcode = 1493638539.32703;
	pJunkcode = 6627563667.39969;
	if (pJunkcode = 1527958545.92516)
		pJunkcode = 32482414.0145657;
	pJunkcode = 5567343980.22272;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3452() {
	float pJunkcode = 2464717002.61306;
	pJunkcode = 4203466980.22488;
	if (pJunkcode = 165782214.745493)
		pJunkcode = 7958919182.70194;
	pJunkcode = 7266129357.01931;
	pJunkcode = 4275704754.33742;
	if (pJunkcode = 5666506691.26489)
		pJunkcode = 3544082376.36119;
	pJunkcode = 603032549.949326;
	if (pJunkcode = 3933388928.38865)
		pJunkcode = 1141238772.35974;
	pJunkcode = 4834133028.62073;
	pJunkcode = 8754690944.30827;
	if (pJunkcode = 7156005310.07858)
		pJunkcode = 7577995823.7132;
	pJunkcode = 5708303781.30558;
	if (pJunkcode = 1405239036.60022)
		pJunkcode = 6883187395.16298;
	pJunkcode = 6173682224.02617;
	pJunkcode = 4724572998.96074;
	if (pJunkcode = 8760039297.0901)
		pJunkcode = 8750732821.03867;
	pJunkcode = 9662341305.13052;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3451() {
	float pJunkcode = 1717732936.0231;
	pJunkcode = 3220260923.05429;
	if (pJunkcode = 6937540634.52684)
		pJunkcode = 8510209646.30721;
	pJunkcode = 5444871601.85651;
	pJunkcode = 3127236188.05567;
	if (pJunkcode = 9369579025.68886)
		pJunkcode = 9853464756.03796;
	pJunkcode = 5287450210.24378;
	if (pJunkcode = 1729143162.58532)
		pJunkcode = 2821121118.58369;
	pJunkcode = 7041478060.6041;
	pJunkcode = 8527795741.7213;
	if (pJunkcode = 1018135832.05199)
		pJunkcode = 9356287390.34385;
	pJunkcode = 6346076271.42055;
	if (pJunkcode = 9926679456.80705)
		pJunkcode = 5539734646.78848;
	pJunkcode = 199920743.31324;
	pJunkcode = 2821477122.39095;
	if (pJunkcode = 8357418467.16395)
		pJunkcode = 5181176969.01728;
	pJunkcode = 2959248523.62982;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3450() {
	float pJunkcode = 9353953686.94742;
	pJunkcode = 3941934323.60154;
	if (pJunkcode = 3213226471.85027)
		pJunkcode = 1833142898.17633;
	pJunkcode = 3917833894.48531;
	pJunkcode = 8381323665.08121;
	if (pJunkcode = 6438677063.3095)
		pJunkcode = 3519046622.35179;
	pJunkcode = 1936193362.45808;
	if (pJunkcode = 4345831578.88765)
		pJunkcode = 5214495173.58165;
	pJunkcode = 8678201157.05683;
	pJunkcode = 9036148587.157;
	if (pJunkcode = 2828848702.01071)
		pJunkcode = 2757725803.00327;
	pJunkcode = 3412866077.06247;
	if (pJunkcode = 1191370564.28593)
		pJunkcode = 6657105245.3511;
	pJunkcode = 165580019.651616;
	pJunkcode = 4753018423.53188;
	if (pJunkcode = 5337798523.19009)
		pJunkcode = 1033013826.82548;
	pJunkcode = 7572694395.15005;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3449() {
	float pJunkcode = 9012957285.0919;
	pJunkcode = 1899405138.23264;
	if (pJunkcode = 2878310004.26756)
		pJunkcode = 2600332665.10661;
	pJunkcode = 6366885518.63298;
	pJunkcode = 9319759647.45274;
	if (pJunkcode = 7919429966.08437)
		pJunkcode = 4904904957.56908;
	pJunkcode = 6726604478.84909;
	if (pJunkcode = 1468776652.19039)
		pJunkcode = 7886645047.7914;
	pJunkcode = 4488186212.49852;
	pJunkcode = 7666027089.83307;
	if (pJunkcode = 3869574393.78508)
		pJunkcode = 6708387926.2662;
	pJunkcode = 1590049063.6871;
	if (pJunkcode = 3456571369.76848)
		pJunkcode = 1297447559.18202;
	pJunkcode = 5148652073.38687;
	pJunkcode = 5571052967.8818;
	if (pJunkcode = 2003123766.22886)
		pJunkcode = 2955105096.96061;
	pJunkcode = 3727015178.5901;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3448() {
	float pJunkcode = 9945651416.20244;
	pJunkcode = 5046172471.53833;
	if (pJunkcode = 7492239255.30981)
		pJunkcode = 6590021903.64483;
	pJunkcode = 2092296666.4121;
	pJunkcode = 4402132203.59402;
	if (pJunkcode = 6898975990.79083)
		pJunkcode = 7933322109.89198;
	pJunkcode = 4229094821.29671;
	if (pJunkcode = 5958875844.7527)
		pJunkcode = 8322249168.06345;
	pJunkcode = 7655209422.97476;
	pJunkcode = 9298387218.81322;
	if (pJunkcode = 8440771176.34295)
		pJunkcode = 8948955025.67425;
	pJunkcode = 8877618723.21784;
	if (pJunkcode = 6583886004.42986)
		pJunkcode = 2437646731.92072;
	pJunkcode = 1457168398.16114;
	pJunkcode = 9226588489.05489;
	if (pJunkcode = 7683671087.32089)
		pJunkcode = 7641600542.75213;
	pJunkcode = 5029133583.20061;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3447() {
	float pJunkcode = 7269843092.04773;
	pJunkcode = 5537792064.69412;
	if (pJunkcode = 9571368683.97892)
		pJunkcode = 5143725107.25982;
	pJunkcode = 9226553536.86819;
	pJunkcode = 3736267098.31069;
	if (pJunkcode = 3514389436.72199)
		pJunkcode = 7344976146.79669;
	pJunkcode = 3086175990.31984;
	if (pJunkcode = 787697005.788615)
		pJunkcode = 4949080342.28515;
	pJunkcode = 8518933243.65608;
	pJunkcode = 8281233529.01061;
	if (pJunkcode = 1545827103.99484)
		pJunkcode = 3833605010.44665;
	pJunkcode = 3068202033.2001;
	if (pJunkcode = 3282773453.77917)
		pJunkcode = 4873168870.30129;
	pJunkcode = 1414439633.97526;
	pJunkcode = 5057777936.28437;
	if (pJunkcode = 5563613670.14653)
		pJunkcode = 3218552037.72487;
	pJunkcode = 6983267165.71698;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3446() {
	float pJunkcode = 4694084356.91251;
	pJunkcode = 8906155588.78154;
	if (pJunkcode = 5178550572.4891)
		pJunkcode = 9655423663.8923;
	pJunkcode = 964373625.038295;
	pJunkcode = 5769606044.83629;
	if (pJunkcode = 9268304594.42267)
		pJunkcode = 6843227674.42923;
	pJunkcode = 6282800389.78105;
	if (pJunkcode = 8201794556.71119)
		pJunkcode = 9785980989.21016;
	pJunkcode = 9493754200.92404;
	pJunkcode = 718779540.402122;
	if (pJunkcode = 2040719862.79938)
		pJunkcode = 6411805211.23576;
	pJunkcode = 9491364602.43396;
	if (pJunkcode = 682455529.271771)
		pJunkcode = 7115013463.75128;
	pJunkcode = 8433294532.9459;
	pJunkcode = 5415406463.76932;
	if (pJunkcode = 1121570549.2005)
		pJunkcode = 166962656.632469;
	pJunkcode = 6842187693.20832;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3445() {
	float pJunkcode = 8648482140.64632;
	pJunkcode = 3094043845.822;
	if (pJunkcode = 5435025392.51763)
		pJunkcode = 2925797662.80451;
	pJunkcode = 3211211128.57909;
	pJunkcode = 8481643672.54753;
	if (pJunkcode = 4864179180.45569)
		pJunkcode = 7142040076.80622;
	pJunkcode = 7200523019.93838;
	if (pJunkcode = 9145497881.5805)
		pJunkcode = 6852563100.35339;
	pJunkcode = 8979614836.11506;
	pJunkcode = 201166025.557751;
	if (pJunkcode = 5341461294.42858)
		pJunkcode = 4002360740.56051;
	pJunkcode = 1076752854.0261;
	if (pJunkcode = 1530702220.40406)
		pJunkcode = 2607269104.90301;
	pJunkcode = 9835417104.09986;
	pJunkcode = 4642369083.31582;
	if (pJunkcode = 8177236710.75839)
		pJunkcode = 991410997.089681;
	pJunkcode = 3768031308.91069;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3444() {
	float pJunkcode = 67958753.1276174;
	pJunkcode = 3080787511.90615;
	if (pJunkcode = 7898757014.23944)
		pJunkcode = 5255276070.47041;
	pJunkcode = 1021771730.24632;
	pJunkcode = 7998652990.12133;
	if (pJunkcode = 4060464099.03364)
		pJunkcode = 9494547123.36299;
	pJunkcode = 4137688901.99373;
	if (pJunkcode = 3538156567.44826)
		pJunkcode = 8700119894.05542;
	pJunkcode = 7474336888.24047;
	pJunkcode = 244940431.020884;
	if (pJunkcode = 7666408602.31056)
		pJunkcode = 5861732877.43687;
	pJunkcode = 6865326282.94283;
	if (pJunkcode = 1699010626.78881)
		pJunkcode = 4798160399.65559;
	pJunkcode = 7054407389.47936;
	pJunkcode = 4187116968.00857;
	if (pJunkcode = 652291075.041514)
		pJunkcode = 848556346.045385;
	pJunkcode = 6177223696.36894;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3443() {
	float pJunkcode = 1234232506.26818;
	pJunkcode = 9724051716.698;
	if (pJunkcode = 9356255031.86458)
		pJunkcode = 1476080646.40678;
	pJunkcode = 8047957515.10377;
	pJunkcode = 6844413035.97851;
	if (pJunkcode = 9435340500.93285)
		pJunkcode = 6679714716.44104;
	pJunkcode = 8781866381.22236;
	if (pJunkcode = 3398776217.77776)
		pJunkcode = 3191278624.32741;
	pJunkcode = 4352955717.88702;
	pJunkcode = 628521008.350186;
	if (pJunkcode = 729678920.685976)
		pJunkcode = 4594468538.68589;
	pJunkcode = 7423020815.08838;
	if (pJunkcode = 374310499.566974)
		pJunkcode = 958616521.194371;
	pJunkcode = 296102935.27275;
	pJunkcode = 6908254415.68683;
	if (pJunkcode = 8450598435.78525)
		pJunkcode = 661085172.80426;
	pJunkcode = 5968546394.4408;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3442() {
	float pJunkcode = 4561757047.45873;
	pJunkcode = 6646544113.38718;
	if (pJunkcode = 1444798006.25278)
		pJunkcode = 3368132069.69773;
	pJunkcode = 1502957124.77915;
	pJunkcode = 9083229150.04117;
	if (pJunkcode = 6370087739.16578)
		pJunkcode = 2704493906.71511;
	pJunkcode = 8339989931.32741;
	if (pJunkcode = 2316718838.81048)
		pJunkcode = 5609497788.85361;
	pJunkcode = 5549833300.22825;
	pJunkcode = 348144613.212979;
	if (pJunkcode = 4447596968.23752)
		pJunkcode = 4933632553.76042;
	pJunkcode = 2103878075.3366;
	if (pJunkcode = 1669775255.32459)
		pJunkcode = 7471692850.31083;
	pJunkcode = 841191565.012431;
	pJunkcode = 8732168378.7153;
	if (pJunkcode = 5484943999.85346)
		pJunkcode = 4198017030.47938;
	pJunkcode = 3899415822.01066;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3441() {
	float pJunkcode = 1253842457.15666;
	pJunkcode = 667383118.198416;
	if (pJunkcode = 2592234365.45107)
		pJunkcode = 6378569858.6274;
	pJunkcode = 3475428677.51656;
	pJunkcode = 8287342506.28748;
	if (pJunkcode = 9553479504.30536)
		pJunkcode = 4551809408.79256;
	pJunkcode = 7282705601.52477;
	if (pJunkcode = 6051947089.53559)
		pJunkcode = 6516769095.86538;
	pJunkcode = 6600584171.81093;
	pJunkcode = 1384063594.68517;
	if (pJunkcode = 5906095968.34075)
		pJunkcode = 7716717255.28164;
	pJunkcode = 2687991426.8765;
	if (pJunkcode = 983171903.823588)
		pJunkcode = 2806251193.6396;
	pJunkcode = 3794215451.33289;
	pJunkcode = 6534636773.84402;
	if (pJunkcode = 2431639353.9358)
		pJunkcode = 9853674240.98639;
	pJunkcode = 5119836074.61249;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3440() {
	float pJunkcode = 8411129561.37759;
	pJunkcode = 8270648252.7504;
	if (pJunkcode = 1411522325.45193)
		pJunkcode = 4025868728.71526;
	pJunkcode = 5975692745.26145;
	pJunkcode = 8473719663.51228;
	if (pJunkcode = 9642925425.956)
		pJunkcode = 6879803053.99365;
	pJunkcode = 3141300482.79135;
	if (pJunkcode = 2555911076.0885)
		pJunkcode = 3054207547.3782;
	pJunkcode = 6149509201.36951;
	pJunkcode = 959194997.113493;
	if (pJunkcode = 4451808702.07788)
		pJunkcode = 7034555464.77933;
	pJunkcode = 1091612460.01925;
	if (pJunkcode = 4890643867.02816)
		pJunkcode = 324500988.594855;
	pJunkcode = 1834317619.4716;
	pJunkcode = 3498115289.28394;
	if (pJunkcode = 5096085288.43396)
		pJunkcode = 286852882.835255;
	pJunkcode = 2163246976.77424;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3439() {
	float pJunkcode = 7514578701.43278;
	pJunkcode = 1673736337.84209;
	if (pJunkcode = 2252462032.85225)
		pJunkcode = 7906637877.85958;
	pJunkcode = 5840506581.16118;
	pJunkcode = 8703063777.60267;
	if (pJunkcode = 673550931.190572)
		pJunkcode = 5565524112.62024;
	pJunkcode = 2218473818.24488;
	if (pJunkcode = 965086852.673194)
		pJunkcode = 416290995.456499;
	pJunkcode = 5045001047.092;
	pJunkcode = 4803213605.29521;
	if (pJunkcode = 5440837876.90464)
		pJunkcode = 8428194768.62807;
	pJunkcode = 4949287547.84437;
	if (pJunkcode = 8849307424.05431)
		pJunkcode = 7745958996.92856;
	pJunkcode = 4665734042.70818;
	pJunkcode = 1983503684.98038;
	if (pJunkcode = 7367594642.31214)
		pJunkcode = 9254723108.77962;
	pJunkcode = 1846797859.77913;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3438() {
	float pJunkcode = 4840979254.4298;
	pJunkcode = 622739760.209964;
	if (pJunkcode = 4785311629.37743)
		pJunkcode = 5274947895.48368;
	pJunkcode = 999000194.350926;
	pJunkcode = 3382518894.51378;
	if (pJunkcode = 6395679678.11138)
		pJunkcode = 4015199429.37261;
	pJunkcode = 7770757827.17257;
	if (pJunkcode = 6812877909.81452)
		pJunkcode = 8326973894.85925;
	pJunkcode = 2998066568.39109;
	pJunkcode = 8818453215.9399;
	if (pJunkcode = 7264922568.28017)
		pJunkcode = 1831364832.56958;
	pJunkcode = 8130696987.09617;
	if (pJunkcode = 9774620734.58925)
		pJunkcode = 8872115283.49518;
	pJunkcode = 9111534116.27452;
	pJunkcode = 3745249386.54414;
	if (pJunkcode = 1952545677.34644)
		pJunkcode = 2158887350.38799;
	pJunkcode = 4700980033.12747;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3437() {
	float pJunkcode = 1652141017.99974;
	pJunkcode = 8414420773.54906;
	if (pJunkcode = 2277940388.83251)
		pJunkcode = 3112046350.02148;
	pJunkcode = 9323614208.68769;
	pJunkcode = 8556937874.75837;
	if (pJunkcode = 9441743812.78111)
		pJunkcode = 2962828591.92242;
	pJunkcode = 1896909144.1873;
	if (pJunkcode = 2394714021.77203)
		pJunkcode = 7292129404.42086;
	pJunkcode = 1079532095.35845;
	pJunkcode = 5709455331.52691;
	if (pJunkcode = 6216678105.15337)
		pJunkcode = 1668073878.70834;
	pJunkcode = 2862853619.72143;
	if (pJunkcode = 5320442855.6314)
		pJunkcode = 754339492.819773;
	pJunkcode = 7100270323.13803;
	pJunkcode = 1011568024.67068;
	if (pJunkcode = 6071070892.57338)
		pJunkcode = 7475041738.71258;
	pJunkcode = 8837987369.57033;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3436() {
	float pJunkcode = 1235287258.11741;
	pJunkcode = 2897440424.28608;
	if (pJunkcode = 4929258925.07928)
		pJunkcode = 685185257.334578;
	pJunkcode = 170166071.574519;
	pJunkcode = 9092766778.04405;
	if (pJunkcode = 73540499.9975714)
		pJunkcode = 3620468997.38611;
	pJunkcode = 7495894559.74422;
	if (pJunkcode = 3238102854.87782)
		pJunkcode = 2117408562.26546;
	pJunkcode = 7914884123.61152;
	pJunkcode = 6386562256.32371;
	if (pJunkcode = 4714836039.65226)
		pJunkcode = 4493410730.88428;
	pJunkcode = 608021494.973916;
	if (pJunkcode = 2320066807.54038)
		pJunkcode = 7148417232.72153;
	pJunkcode = 5526444922.96393;
	pJunkcode = 8418065696.16889;
	if (pJunkcode = 4513082760.78647)
		pJunkcode = 504314978.70666;
	pJunkcode = 8511780974.33965;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3435() {
	float pJunkcode = 4919622263.55247;
	pJunkcode = 1670156977.34757;
	if (pJunkcode = 5346846818.25095)
		pJunkcode = 9219183407.76884;
	pJunkcode = 8220871642.74751;
	pJunkcode = 4624105715.94357;
	if (pJunkcode = 8223700604.5666)
		pJunkcode = 3195222328.53945;
	pJunkcode = 3758634909.90731;
	if (pJunkcode = 2110307203.18846)
		pJunkcode = 7663222181.50724;
	pJunkcode = 31162277.4666778;
	pJunkcode = 605552259.88686;
	if (pJunkcode = 4809679079.97537)
		pJunkcode = 7904128486.87298;
	pJunkcode = 6846354211.78984;
	if (pJunkcode = 168012970.774011)
		pJunkcode = 8099611734.13134;
	pJunkcode = 1539090267.91316;
	pJunkcode = 3393889058.30778;
	if (pJunkcode = 2646571204.38841)
		pJunkcode = 2386456928.54888;
	pJunkcode = 3494740023.76235;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3434() {
	float pJunkcode = 9853441203.12559;
	pJunkcode = 2006481790.8137;
	if (pJunkcode = 3340622696.4441)
		pJunkcode = 2979236035.89927;
	pJunkcode = 5219651424.37706;
	pJunkcode = 8539886865.37856;
	if (pJunkcode = 1773665687.33469)
		pJunkcode = 5295527189.81048;
	pJunkcode = 7334067376.54688;
	if (pJunkcode = 5452153002.41319)
		pJunkcode = 2790635244.86412;
	pJunkcode = 7759102821.21759;
	pJunkcode = 7933654788.95861;
	if (pJunkcode = 1524865731.41774)
		pJunkcode = 2322764626.48257;
	pJunkcode = 6728767928.86821;
	if (pJunkcode = 4004869696.15816)
		pJunkcode = 3732406397.39306;
	pJunkcode = 9269931498.57398;
	pJunkcode = 5485680507.99161;
	if (pJunkcode = 3813577193.26753)
		pJunkcode = 500294732.185957;
	pJunkcode = 560647431.043732;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3433() {
	float pJunkcode = 6967030873.83471;
	pJunkcode = 7151781781.15577;
	if (pJunkcode = 5046228504.00069)
		pJunkcode = 6127559956.87676;
	pJunkcode = 9222951724.73923;
	pJunkcode = 4883380717.88313;
	if (pJunkcode = 9731838501.38076)
		pJunkcode = 7223198386.09814;
	pJunkcode = 3047310395.03173;
	if (pJunkcode = 9587248850.9832)
		pJunkcode = 7425685858.78862;
	pJunkcode = 4477943363.8265;
	pJunkcode = 1196384882.94033;
	if (pJunkcode = 2670292330.56785)
		pJunkcode = 863688372.808878;
	pJunkcode = 4150874260.52593;
	if (pJunkcode = 9601588942.4998)
		pJunkcode = 8077169773.70113;
	pJunkcode = 8389352146.50983;
	pJunkcode = 9485001700.26811;
	if (pJunkcode = 3347530749.8123)
		pJunkcode = 1223685481.37189;
	pJunkcode = 7150928174.34471;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3432() {
	float pJunkcode = 4355278536.5597;
	pJunkcode = 6863707874.69674;
	if (pJunkcode = 8186173183.03596)
		pJunkcode = 629592225.817878;
	pJunkcode = 4964667213.13233;
	pJunkcode = 1543621977.70727;
	if (pJunkcode = 4127172289.13198)
		pJunkcode = 9682499189.51972;
	pJunkcode = 2139965300.07027;
	if (pJunkcode = 2536110653.01182)
		pJunkcode = 3581068960.99815;
	pJunkcode = 3264602998.87443;
	pJunkcode = 1284872532.13904;
	if (pJunkcode = 3869514333.39286)
		pJunkcode = 9053979636.58507;
	pJunkcode = 2694217111.67471;
	if (pJunkcode = 393216240.261205)
		pJunkcode = 4567822739.6235;
	pJunkcode = 2768435027.74544;
	pJunkcode = 3803273753.35473;
	if (pJunkcode = 272053497.885865)
		pJunkcode = 2656483266.888;
	pJunkcode = 2378282042.34731;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3431() {
	float pJunkcode = 9216497586.46974;
	pJunkcode = 2652928455.81104;
	if (pJunkcode = 4742360774.91813)
		pJunkcode = 2124515118.07045;
	pJunkcode = 4990954442.61942;
	pJunkcode = 6607785071.97758;
	if (pJunkcode = 935935439.099478)
		pJunkcode = 5376787340.53581;
	pJunkcode = 6546625702.16883;
	if (pJunkcode = 5540135933.37479)
		pJunkcode = 7874309768.72128;
	pJunkcode = 6099492873.66145;
	pJunkcode = 3264871354.46239;
	if (pJunkcode = 6623902304.77251)
		pJunkcode = 8792754765.02489;
	pJunkcode = 5747166894.64385;
	if (pJunkcode = 5449576934.05364)
		pJunkcode = 5303302207.35019;
	pJunkcode = 4613562902.9115;
	pJunkcode = 8191952938.74668;
	if (pJunkcode = 8351588264.54215)
		pJunkcode = 9268683678.50083;
	pJunkcode = 8784746034.42226;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3430() {
	float pJunkcode = 5711618481.19751;
	pJunkcode = 4358265416.51193;
	if (pJunkcode = 6428262960.09843)
		pJunkcode = 8292947480.11018;
	pJunkcode = 4238297737.64177;
	pJunkcode = 7916730002.79339;
	if (pJunkcode = 5596354714.27175)
		pJunkcode = 9654355189.14216;
	pJunkcode = 4544413025.97237;
	if (pJunkcode = 1724993575.27889)
		pJunkcode = 7558923666.65231;
	pJunkcode = 8736954239.24185;
	pJunkcode = 1708967804.43106;
	if (pJunkcode = 8617266624.89563)
		pJunkcode = 1418191224.55947;
	pJunkcode = 1299414776.84042;
	if (pJunkcode = 7072499993.5803)
		pJunkcode = 2968371936.09004;
	pJunkcode = 334095885.37846;
	pJunkcode = 2355834084.2597;
	if (pJunkcode = 1501885981.81651)
		pJunkcode = 5427711968.75447;
	pJunkcode = 9292499498.2844;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3429() {
	float pJunkcode = 9855363838.81817;
	pJunkcode = 1143344045.56772;
	if (pJunkcode = 7493021556.12101)
		pJunkcode = 2966372739.46253;
	pJunkcode = 3498903333.96968;
	pJunkcode = 401860031.048428;
	if (pJunkcode = 1042161232.2436)
		pJunkcode = 2850283530.96702;
	pJunkcode = 1567608383.81263;
	if (pJunkcode = 9876257388.50701)
		pJunkcode = 9884962096.49466;
	pJunkcode = 218038506.841938;
	pJunkcode = 2951006263.89863;
	if (pJunkcode = 5349737605.30088)
		pJunkcode = 2699018052.49125;
	pJunkcode = 2551171271.33101;
	if (pJunkcode = 5007405758.43573)
		pJunkcode = 7557892879.82157;
	pJunkcode = 3329622943.87351;
	pJunkcode = 8445609963.40214;
	if (pJunkcode = 4908459112.07019)
		pJunkcode = 3673755827.18699;
	pJunkcode = 4678332664.57263;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3428() {
	float pJunkcode = 422689722.915226;
	pJunkcode = 1647266077.35975;
	if (pJunkcode = 1425215845.75202)
		pJunkcode = 2025723716.96899;
	pJunkcode = 6519135434.80676;
	pJunkcode = 4198425374.09193;
	if (pJunkcode = 6896360020.31461)
		pJunkcode = 1856398961.43733;
	pJunkcode = 8077713586.14067;
	if (pJunkcode = 6904397896.36575)
		pJunkcode = 6230595126.66648;
	pJunkcode = 1678471886.49798;
	pJunkcode = 7073884174.21288;
	if (pJunkcode = 7473946791.38299)
		pJunkcode = 6974829970.47879;
	pJunkcode = 1488335345.92525;
	if (pJunkcode = 8010090634.54596)
		pJunkcode = 469348554.558716;
	pJunkcode = 9796317430.20837;
	pJunkcode = 2441875017.52601;
	if (pJunkcode = 2367410487.37542)
		pJunkcode = 7400136660.13686;
	pJunkcode = 6812991103.95658;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3427() {
	float pJunkcode = 5873143704.69614;
	pJunkcode = 754385304.119097;
	if (pJunkcode = 1188615930.57824)
		pJunkcode = 3680772979.24719;
	pJunkcode = 520053069.216357;
	pJunkcode = 5507429138.29475;
	if (pJunkcode = 22748488.4590312)
		pJunkcode = 5680641239.61644;
	pJunkcode = 4652432386.43248;
	if (pJunkcode = 6048651423.35844)
		pJunkcode = 574209920.224529;
	pJunkcode = 4875945310.709;
	pJunkcode = 5871519149.78254;
	if (pJunkcode = 571760855.05443)
		pJunkcode = 5618561748.60351;
	pJunkcode = 8497361453.44328;
	if (pJunkcode = 6853871146.12686)
		pJunkcode = 7967071342.53166;
	pJunkcode = 9951099289.11937;
	pJunkcode = 5739354941.22021;
	if (pJunkcode = 275703160.890198)
		pJunkcode = 8112673356.05536;
	pJunkcode = 9338134029.69402;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3426() {
	float pJunkcode = 9904487702.99762;
	pJunkcode = 63514766.0578748;
	if (pJunkcode = 3885061099.56273)
		pJunkcode = 4894593922.79664;
	pJunkcode = 1056323559.34145;
	pJunkcode = 4961640303.00784;
	if (pJunkcode = 7453872720.53074)
		pJunkcode = 9792931356.79638;
	pJunkcode = 3271579941.48456;
	if (pJunkcode = 2807642934.20758)
		pJunkcode = 1198852278.68369;
	pJunkcode = 8715272232.76115;
	pJunkcode = 5919706073.24031;
	if (pJunkcode = 433931591.322638)
		pJunkcode = 9246362553.63799;
	pJunkcode = 6446556148.12131;
	if (pJunkcode = 7882653292.47164)
		pJunkcode = 5996381480.53786;
	pJunkcode = 4946859742.22775;
	pJunkcode = 757470304.908256;
	if (pJunkcode = 6884080501.29773)
		pJunkcode = 9447768373.49569;
	pJunkcode = 116519251.945614;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3425() {
	float pJunkcode = 6793923843.27516;
	pJunkcode = 3880152054.2311;
	if (pJunkcode = 6764058878.43519)
		pJunkcode = 3716479066.35695;
	pJunkcode = 5338795119.18273;
	pJunkcode = 7631653751.58103;
	if (pJunkcode = 4687429755.79181)
		pJunkcode = 5816958143.74321;
	pJunkcode = 7621782215.17801;
	if (pJunkcode = 1719036136.63933)
		pJunkcode = 9195087146.27946;
	pJunkcode = 2565383994.11771;
	pJunkcode = 4607879358.84552;
	if (pJunkcode = 8175370396.23529)
		pJunkcode = 1200390715.06003;
	pJunkcode = 6997897277.49627;
	if (pJunkcode = 8331222781.11388)
		pJunkcode = 5621955081.60237;
	pJunkcode = 2228703097.68268;
	pJunkcode = 4073455639.63474;
	if (pJunkcode = 2933661241.25052)
		pJunkcode = 4294655368.03286;
	pJunkcode = 9903311514.88397;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3424() {
	float pJunkcode = 8262013078.93912;
	pJunkcode = 7705321864.53966;
	if (pJunkcode = 2295678597.96985)
		pJunkcode = 1139770626.8518;
	pJunkcode = 9050815013.16864;
	pJunkcode = 8157940724.19656;
	if (pJunkcode = 5618673457.30092)
		pJunkcode = 1824756249.9654;
	pJunkcode = 3486789497.91027;
	if (pJunkcode = 3189077181.38379)
		pJunkcode = 4452359152.45268;
	pJunkcode = 6044706259.31286;
	pJunkcode = 8500196508.37937;
	if (pJunkcode = 3168986586.70601)
		pJunkcode = 2720166563.15507;
	pJunkcode = 1530423806.63287;
	if (pJunkcode = 4430087387.1927)
		pJunkcode = 967595549.831414;
	pJunkcode = 2431201890.48586;
	pJunkcode = 925357874.113065;
	if (pJunkcode = 9814707602.866)
		pJunkcode = 1734305411.03814;
	pJunkcode = 1247154687.31415;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3423() {
	float pJunkcode = 7263379773.35777;
	pJunkcode = 2971332166.5133;
	if (pJunkcode = 4875775883.87222)
		pJunkcode = 4536852282.65954;
	pJunkcode = 328250500.022215;
	pJunkcode = 5307263583.23136;
	if (pJunkcode = 1182140535.08356)
		pJunkcode = 9634014553.1517;
	pJunkcode = 6981587861.22502;
	if (pJunkcode = 7755263768.67673)
		pJunkcode = 854725898.265056;
	pJunkcode = 7321990953.66945;
	pJunkcode = 1419744515.38233;
	if (pJunkcode = 8962643509.89789)
		pJunkcode = 7659966864.47714;
	pJunkcode = 8764525346.92329;
	if (pJunkcode = 405557703.251381)
		pJunkcode = 4532739081.76223;
	pJunkcode = 5588997102.02263;
	pJunkcode = 3462059261.21624;
	if (pJunkcode = 9484826021.99106)
		pJunkcode = 840819849.95045;
	pJunkcode = 2027704853.10409;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3422() {
	float pJunkcode = 1810779093.55501;
	pJunkcode = 3740008630.71168;
	if (pJunkcode = 4350106534.92875)
		pJunkcode = 2948758620.83241;
	pJunkcode = 6220737937.14424;
	pJunkcode = 7537316523.78811;
	if (pJunkcode = 7435350529.56176)
		pJunkcode = 1262328676.72492;
	pJunkcode = 6012665474.28537;
	if (pJunkcode = 6398829540.21262)
		pJunkcode = 6169662070.63679;
	pJunkcode = 4575031716.60441;
	pJunkcode = 1948500161.96867;
	if (pJunkcode = 1406771235.50599)
		pJunkcode = 1293849118.40706;
	pJunkcode = 8147805526.02442;
	if (pJunkcode = 9963115518.89243)
		pJunkcode = 1552399586.56872;
	pJunkcode = 3232449976.62925;
	pJunkcode = 7269056178.9076;
	if (pJunkcode = 8557548113.11627)
		pJunkcode = 5292077593.84922;
	pJunkcode = 8896681229.54506;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3421() {
	float pJunkcode = 3908563120.32775;
	pJunkcode = 375387304.914234;
	if (pJunkcode = 3229971728.75811)
		pJunkcode = 1785717805.26428;
	pJunkcode = 6285692394.52307;
	pJunkcode = 3132585672.3831;
	if (pJunkcode = 6751294743.57794)
		pJunkcode = 6173976162.78942;
	pJunkcode = 1927935316.96712;
	if (pJunkcode = 488469379.722361)
		pJunkcode = 8225115627.96611;
	pJunkcode = 6593951682.39533;
	pJunkcode = 9981385498.58884;
	if (pJunkcode = 9537148742.33416)
		pJunkcode = 393825298.179545;
	pJunkcode = 5074847138.86874;
	if (pJunkcode = 6187119055.76518)
		pJunkcode = 5562723811.96677;
	pJunkcode = 1218509998.95762;
	pJunkcode = 2568317616.92109;
	if (pJunkcode = 7761854709.87945)
		pJunkcode = 8718021895.58431;
	pJunkcode = 9071001623.91538;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3420() {
	float pJunkcode = 6464644617.19137;
	pJunkcode = 9451127978.9848;
	if (pJunkcode = 3092149368.38587)
		pJunkcode = 1466080220.90496;
	pJunkcode = 4157759797.06775;
	pJunkcode = 7385818237.95157;
	if (pJunkcode = 1568868692.79661)
		pJunkcode = 5448284185.10049;
	pJunkcode = 5156481915.00998;
	if (pJunkcode = 6520756292.40672)
		pJunkcode = 7662528870.00644;
	pJunkcode = 1878349705.34444;
	pJunkcode = 2347610758.12712;
	if (pJunkcode = 5928893506.5024)
		pJunkcode = 7652559962.47366;
	pJunkcode = 3907842300.16149;
	if (pJunkcode = 3379178804.69407)
		pJunkcode = 3092293803.43128;
	pJunkcode = 1174243766.9612;
	pJunkcode = 6785486808.68851;
	if (pJunkcode = 2119176793.20417)
		pJunkcode = 1669337163.23819;
	pJunkcode = 7805806099.78648;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3419() {
	float pJunkcode = 5213123293.26896;
	pJunkcode = 1872040027.73147;
	if (pJunkcode = 5772749597.29019)
		pJunkcode = 3427737522.37545;
	pJunkcode = 8917416351.20674;
	pJunkcode = 5771076341.67787;
	if (pJunkcode = 9337137133.35286)
		pJunkcode = 7055408297.45239;
	pJunkcode = 5954705752.25653;
	if (pJunkcode = 5738493034.07971)
		pJunkcode = 2727348248.00085;
	pJunkcode = 7785543482.2362;
	pJunkcode = 221919596.535137;
	if (pJunkcode = 1278110185.05886)
		pJunkcode = 1820915644.45967;
	pJunkcode = 8732437311.71748;
	if (pJunkcode = 792046785.865412)
		pJunkcode = 634539643.584147;
	pJunkcode = 5793228731.20402;
	pJunkcode = 9163669228.21769;
	if (pJunkcode = 3203629402.77848)
		pJunkcode = 8750369126.91215;
	pJunkcode = 2669007046.09049;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3418() {
	float pJunkcode = 7254625305.30258;
	pJunkcode = 7928218424.74531;
	if (pJunkcode = 3732603609.96099)
		pJunkcode = 3145857962.0234;
	pJunkcode = 6628279931.11582;
	pJunkcode = 5160680733.86369;
	if (pJunkcode = 2277732417.26813)
		pJunkcode = 637978471.235822;
	pJunkcode = 7543687735.59654;
	if (pJunkcode = 7589076640.11834)
		pJunkcode = 3122242321.04049;
	pJunkcode = 1533691361.11923;
	pJunkcode = 2039055287.02665;
	if (pJunkcode = 4147420711.13987)
		pJunkcode = 1783513249.40608;
	pJunkcode = 2029117937.04374;
	if (pJunkcode = 4984378362.84806)
		pJunkcode = 3333783802.39869;
	pJunkcode = 5869717107.51587;
	pJunkcode = 5627577394.14971;
	if (pJunkcode = 1667330103.99847)
		pJunkcode = 2138091863.32899;
	pJunkcode = 5654932963.34651;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3417() {
	float pJunkcode = 5881331214.84898;
	pJunkcode = 3756084797.24836;
	if (pJunkcode = 6653638396.81167)
		pJunkcode = 1280232338.31969;
	pJunkcode = 9034118640.89388;
	pJunkcode = 8258658372.15777;
	if (pJunkcode = 9615289126.34628)
		pJunkcode = 4158281984.41336;
	pJunkcode = 3601471589.85585;
	if (pJunkcode = 9070579264.03825)
		pJunkcode = 7507974156.07159;
	pJunkcode = 9322595975.95971;
	pJunkcode = 6327806212.56252;
	if (pJunkcode = 3597168040.16935)
		pJunkcode = 1751315703.40217;
	pJunkcode = 7422048496.10419;
	if (pJunkcode = 3762560348.73801)
		pJunkcode = 6259167314.6139;
	pJunkcode = 1430515989.10287;
	pJunkcode = 2899091204.98113;
	if (pJunkcode = 7908306418.69852)
		pJunkcode = 8944130410.87462;
	pJunkcode = 3919147854.09799;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3416() {
	float pJunkcode = 2410185612.33544;
	pJunkcode = 4338971194.78873;
	if (pJunkcode = 6139340024.23302)
		pJunkcode = 2740013817.31909;
	pJunkcode = 5619363602.33664;
	pJunkcode = 8375644572.69262;
	if (pJunkcode = 656092919.289309)
		pJunkcode = 9878201707.5006;
	pJunkcode = 8818886113.62659;
	if (pJunkcode = 1838240764.9434)
		pJunkcode = 2637958461.25703;
	pJunkcode = 6360024240.47333;
	pJunkcode = 4702198370.15105;
	if (pJunkcode = 2296988832.32949)
		pJunkcode = 7942479941.36206;
	pJunkcode = 9218578810.84102;
	if (pJunkcode = 4219104745.01351)
		pJunkcode = 3636092603.88488;
	pJunkcode = 547514093.342024;
	pJunkcode = 4061559905.49971;
	if (pJunkcode = 6064261996.12812)
		pJunkcode = 876133514.247868;
	pJunkcode = 6113740045.72569;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3415() {
	float pJunkcode = 1122853072.33405;
	pJunkcode = 9001778117.34981;
	if (pJunkcode = 628856143.755547)
		pJunkcode = 6970765512.18929;
	pJunkcode = 6409846778.1897;
	pJunkcode = 4179604224.58122;
	if (pJunkcode = 7989999665.01459)
		pJunkcode = 5375128331.6881;
	pJunkcode = 7950383473.14697;
	if (pJunkcode = 3625103254.08342)
		pJunkcode = 9020189789.37407;
	pJunkcode = 3396723207.54173;
	pJunkcode = 2733348541.02013;
	if (pJunkcode = 4710395389.01228)
		pJunkcode = 9983345365.85733;
	pJunkcode = 5395742163.76719;
	if (pJunkcode = 1205484153.43254)
		pJunkcode = 4175132982.62889;
	pJunkcode = 7689370780.35882;
	pJunkcode = 6643228889.63154;
	if (pJunkcode = 9675915715.12107)
		pJunkcode = 1111489788.61676;
	pJunkcode = 7707914873.48743;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3414() {
	float pJunkcode = 4782153746.34073;
	pJunkcode = 4607974941.85381;
	if (pJunkcode = 2669760546.03909)
		pJunkcode = 5432306302.38055;
	pJunkcode = 7558961913.35967;
	pJunkcode = 3268678160.28158;
	if (pJunkcode = 8399147450.26585)
		pJunkcode = 9940204206.84622;
	pJunkcode = 626844709.671781;
	if (pJunkcode = 1649692784.02673)
		pJunkcode = 4388754016.65107;
	pJunkcode = 8155490338.88992;
	pJunkcode = 3535652524.52806;
	if (pJunkcode = 6316367397.01146)
		pJunkcode = 6246778887.82424;
	pJunkcode = 1041840634.63825;
	if (pJunkcode = 206640303.533918)
		pJunkcode = 2291958059.69805;
	pJunkcode = 2138850341.14698;
	pJunkcode = 8524870430.88877;
	if (pJunkcode = 2157946947.50926)
		pJunkcode = 5693553055.14581;
	pJunkcode = 9201933004.21978;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3413() {
	float pJunkcode = 162885485.916953;
	pJunkcode = 5711510351.07131;
	if (pJunkcode = 5859904325.33822)
		pJunkcode = 156097090.00485;
	pJunkcode = 440672813.889774;
	pJunkcode = 707813752.826172;
	if (pJunkcode = 271733109.825997)
		pJunkcode = 8300439527.42857;
	pJunkcode = 2864831838.51773;
	if (pJunkcode = 6131334150.43383)
		pJunkcode = 5856444142.67347;
	pJunkcode = 912268303.478072;
	pJunkcode = 6116826054.66146;
	if (pJunkcode = 4734295017.67623)
		pJunkcode = 337821920.726785;
	pJunkcode = 5146632557.83228;
	if (pJunkcode = 6887218584.6145)
		pJunkcode = 6035793020.97288;
	pJunkcode = 2378869631.16669;
	pJunkcode = 1064283333.13422;
	if (pJunkcode = 1499987741.87481)
		pJunkcode = 3879067153.44332;
	pJunkcode = 7267908041.94938;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3412() {
	float pJunkcode = 114162677.823778;
	pJunkcode = 7343743940.19036;
	if (pJunkcode = 560326891.645459)
		pJunkcode = 7858193688.4265;
	pJunkcode = 2667211818.70861;
	pJunkcode = 3634388552.07129;
	if (pJunkcode = 2666895363.11899)
		pJunkcode = 7245135043.89216;
	pJunkcode = 4880016020.43877;
	if (pJunkcode = 5600641896.12769)
		pJunkcode = 9316856003.62323;
	pJunkcode = 9629865580.51203;
	pJunkcode = 9615918638.23212;
	if (pJunkcode = 5221723843.18146)
		pJunkcode = 1550333374.96721;
	pJunkcode = 8378135307.14631;
	if (pJunkcode = 4186045731.91461)
		pJunkcode = 7364552214.18044;
	pJunkcode = 2259087432.64565;
	pJunkcode = 6076600830.48661;
	if (pJunkcode = 1263313058.54412)
		pJunkcode = 1474256722.93504;
	pJunkcode = 2022816464.3006;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3411() {
	float pJunkcode = 6489433182.03495;
	pJunkcode = 4321620928.84695;
	if (pJunkcode = 5044954299.82285)
		pJunkcode = 3412710789.88928;
	pJunkcode = 6465853086.95094;
	pJunkcode = 1201099947.36104;
	if (pJunkcode = 8059590849.39916)
		pJunkcode = 2685992286.12778;
	pJunkcode = 6089748966.75604;
	if (pJunkcode = 8690242827.97991)
		pJunkcode = 6945194984.13556;
	pJunkcode = 3919964396.96953;
	pJunkcode = 4605305118.18825;
	if (pJunkcode = 8824976168.94627)
		pJunkcode = 3588706838.00357;
	pJunkcode = 1608337406.55274;
	if (pJunkcode = 2525058623.49959)
		pJunkcode = 117696804.433198;
	pJunkcode = 1936290314.49687;
	pJunkcode = 4311791909.22259;
	if (pJunkcode = 4822012936.46448)
		pJunkcode = 2041481681.70369;
	pJunkcode = 1305166143.79465;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3410() {
	float pJunkcode = 2281256576.49737;
	pJunkcode = 7637355102.35657;
	if (pJunkcode = 3479083793.36003)
		pJunkcode = 474791205.136529;
	pJunkcode = 1189608398.52484;
	pJunkcode = 4157829489.56738;
	if (pJunkcode = 8405705183.16596)
		pJunkcode = 1391722561.27938;
	pJunkcode = 5734580072.51842;
	if (pJunkcode = 487328204.330764)
		pJunkcode = 6524319815.91923;
	pJunkcode = 2486491905.2633;
	pJunkcode = 9087595095.3879;
	if (pJunkcode = 8497837948.23201)
		pJunkcode = 9880889826.81518;
	pJunkcode = 5228061730.27311;
	if (pJunkcode = 4830402660.33746)
		pJunkcode = 7707205552.71166;
	pJunkcode = 9866931105.7508;
	pJunkcode = 3435473867.13198;
	if (pJunkcode = 4007173131.87359)
		pJunkcode = 9692155955.77155;
	pJunkcode = 4743753414.20341;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3409() {
	float pJunkcode = 8310741931.25536;
	pJunkcode = 7785891064.6433;
	if (pJunkcode = 2230348441.83657)
		pJunkcode = 8122949680.78256;
	pJunkcode = 6237090736.31549;
	pJunkcode = 2906274242.55942;
	if (pJunkcode = 4342960328.46166)
		pJunkcode = 2441243566.18031;
	pJunkcode = 5517957248.2187;
	if (pJunkcode = 9759129763.39687)
		pJunkcode = 9913858467.60348;
	pJunkcode = 6146120955.66934;
	pJunkcode = 2228966975.98214;
	if (pJunkcode = 6231439653.17324)
		pJunkcode = 9363451445.42959;
	pJunkcode = 4533414607.50802;
	if (pJunkcode = 15435667.7212085)
		pJunkcode = 2165916826.05091;
	pJunkcode = 5335749356.94955;
	pJunkcode = 2674375445.39026;
	if (pJunkcode = 4787182239.58296)
		pJunkcode = 3247324402.14467;
	pJunkcode = 7074582570.6841;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3408() {
	float pJunkcode = 6321486956.39266;
	pJunkcode = 1144373307.37299;
	if (pJunkcode = 443991970.115655)
		pJunkcode = 8994722651.06204;
	pJunkcode = 1584694643.39938;
	pJunkcode = 3004104635.47625;
	if (pJunkcode = 693833290.253706)
		pJunkcode = 6514533343.34121;
	pJunkcode = 6576120002.00252;
	if (pJunkcode = 5232130963.87136)
		pJunkcode = 6474480018.63985;
	pJunkcode = 4515757033.80113;
	pJunkcode = 2698416787.33662;
	if (pJunkcode = 3731644846.24132)
		pJunkcode = 9314320442.73399;
	pJunkcode = 7838613730.4816;
	if (pJunkcode = 905063338.12904)
		pJunkcode = 9431386179.74963;
	pJunkcode = 6263180785.83062;
	pJunkcode = 2645122721.2317;
	if (pJunkcode = 9573579274.74705)
		pJunkcode = 9768506863.26119;
	pJunkcode = 3076245464.53485;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3407() {
	float pJunkcode = 4030616808.19108;
	pJunkcode = 9302186732.44958;
	if (pJunkcode = 5778614554.19462)
		pJunkcode = 6035718217.89173;
	pJunkcode = 2345787854.4434;
	pJunkcode = 2294785598.93613;
	if (pJunkcode = 5412730642.44636)
		pJunkcode = 9246889430.14587;
	pJunkcode = 1936594397.58981;
	if (pJunkcode = 1075887584.84473)
		pJunkcode = 2644264967.74834;
	pJunkcode = 9687432763.31436;
	pJunkcode = 4760108548.64266;
	if (pJunkcode = 7692532539.98698)
		pJunkcode = 3367765826.10115;
	pJunkcode = 6267008816.27904;
	if (pJunkcode = 7059293461.6411)
		pJunkcode = 7111437171.93396;
	pJunkcode = 7267049832.12253;
	pJunkcode = 8382747941.07798;
	if (pJunkcode = 1427409395.81575)
		pJunkcode = 2438047957.97167;
	pJunkcode = 1806576612.31191;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3406() {
	float pJunkcode = 583047582.493355;
	pJunkcode = 1329576495.33797;
	if (pJunkcode = 470725477.932712)
		pJunkcode = 1618374681.79202;
	pJunkcode = 3547867845.79356;
	pJunkcode = 4259597295.67516;
	if (pJunkcode = 6350495688.11818)
		pJunkcode = 6135285752.18232;
	pJunkcode = 4791602024.14932;
	if (pJunkcode = 7699069756.05595)
		pJunkcode = 4515399041.64005;
	pJunkcode = 780732240.099371;
	pJunkcode = 856839374.014579;
	if (pJunkcode = 3487519156.23938)
		pJunkcode = 9989302944.19388;
	pJunkcode = 3497383558.78282;
	if (pJunkcode = 7614205506.8276)
		pJunkcode = 735301261.172707;
	pJunkcode = 8114210740.04771;
	pJunkcode = 623289228.049279;
	if (pJunkcode = 7204247306.74606)
		pJunkcode = 4431475220.90155;
	pJunkcode = 5877512157.60383;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3405() {
	float pJunkcode = 2365571146.01264;
	pJunkcode = 4024555926.2593;
	if (pJunkcode = 1785724815.72485)
		pJunkcode = 8027730699.56354;
	pJunkcode = 6600347709.0456;
	pJunkcode = 2022107008.48441;
	if (pJunkcode = 7482257532.90656)
		pJunkcode = 2534568742.83702;
	pJunkcode = 7794474809.008;
	if (pJunkcode = 2533814805.17119)
		pJunkcode = 8814685918.75754;
	pJunkcode = 9624660619.70476;
	pJunkcode = 9739323351.97593;
	if (pJunkcode = 7158386603.58265)
		pJunkcode = 2127316042.7038;
	pJunkcode = 1315809543.31551;
	if (pJunkcode = 1681328056.63399)
		pJunkcode = 221686439.931888;
	pJunkcode = 8528703029.42874;
	pJunkcode = 8254587725.29165;
	if (pJunkcode = 2838771742.36537)
		pJunkcode = 2365645865.27559;
	pJunkcode = 3377676819.91878;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3404() {
	float pJunkcode = 5324026297.60629;
	pJunkcode = 4115242196.52245;
	if (pJunkcode = 3359710907.01733)
		pJunkcode = 9089566110.47612;
	pJunkcode = 3091600196.23851;
	pJunkcode = 9994704470.30636;
	if (pJunkcode = 3608677067.42718)
		pJunkcode = 4747453251.7097;
	pJunkcode = 1428284071.26106;
	if (pJunkcode = 320571989.819211)
		pJunkcode = 3691425105.1733;
	pJunkcode = 7822718965.14196;
	pJunkcode = 3902170616.36989;
	if (pJunkcode = 1949049720.61103)
		pJunkcode = 2327769578.45384;
	pJunkcode = 9844084538.6025;
	if (pJunkcode = 8602860242.50109)
		pJunkcode = 5864919386.71494;
	pJunkcode = 8198073226.82311;
	pJunkcode = 4321832653.60378;
	if (pJunkcode = 7908138714.79127)
		pJunkcode = 490823129.636732;
	pJunkcode = 853086372.490772;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3403() {
	float pJunkcode = 3914306578.10412;
	pJunkcode = 8153244646.92497;
	if (pJunkcode = 8480723139.23723)
		pJunkcode = 9696024549.56907;
	pJunkcode = 8486738826.86472;
	pJunkcode = 9174455209.47056;
	if (pJunkcode = 443441948.144949)
		pJunkcode = 5469667021.1657;
	pJunkcode = 4948025944.33496;
	if (pJunkcode = 6953779813.48021)
		pJunkcode = 3583917343.02702;
	pJunkcode = 945828491.167121;
	pJunkcode = 2829567061.18168;
	if (pJunkcode = 9876581413.34517)
		pJunkcode = 8600931616.13595;
	pJunkcode = 1276830050.74382;
	if (pJunkcode = 5613600114.34031)
		pJunkcode = 9428004378.73723;
	pJunkcode = 9676331786.3054;
	pJunkcode = 994626367.765723;
	if (pJunkcode = 8172539941.9867)
		pJunkcode = 6970633801.21131;
	pJunkcode = 5495296550.24281;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3402() {
	float pJunkcode = 4422939597.49306;
	pJunkcode = 4702338688.79184;
	if (pJunkcode = 6872782800.6603)
		pJunkcode = 1032345383.71704;
	pJunkcode = 210783727.874842;
	pJunkcode = 5095887794.14377;
	if (pJunkcode = 8444258432.28421)
		pJunkcode = 8137485797.44268;
	pJunkcode = 7514995678.31926;
	if (pJunkcode = 9961478679.95691)
		pJunkcode = 3344887019.21604;
	pJunkcode = 6820321407.7726;
	pJunkcode = 4761625117.78363;
	if (pJunkcode = 8293263267.48253)
		pJunkcode = 7640639557.85033;
	pJunkcode = 1074545387.81397;
	if (pJunkcode = 4654758578.78745)
		pJunkcode = 5587047611.91215;
	pJunkcode = 308383376.289763;
	pJunkcode = 9255812749.95191;
	if (pJunkcode = 3254640256.63544)
		pJunkcode = 6890706466.86393;
	pJunkcode = 4368811272.61008;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3401() {
	float pJunkcode = 2298630663.10409;
	pJunkcode = 4947228791.40552;
	if (pJunkcode = 7760979470.98962)
		pJunkcode = 1561857154.2271;
	pJunkcode = 8599206730.8447;
	pJunkcode = 608931485.818095;
	if (pJunkcode = 9000479158.86548)
		pJunkcode = 6034033849.38392;
	pJunkcode = 6148614775.09821;
	if (pJunkcode = 1287268728.98358)
		pJunkcode = 5051638782.12954;
	pJunkcode = 9591343542.02944;
	pJunkcode = 4425740612.92945;
	if (pJunkcode = 1673034851.73913)
		pJunkcode = 4174392157.85378;
	pJunkcode = 7361101719.50188;
	if (pJunkcode = 3214261135.27245)
		pJunkcode = 5505479699.4065;
	pJunkcode = 4453730345.92184;
	pJunkcode = 8736908160.18997;
	if (pJunkcode = 4777140852.59562)
		pJunkcode = 8786719307.94514;
	pJunkcode = 9299858614.60444;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3400() {
	float pJunkcode = 7802037033.10892;
	pJunkcode = 8507357192.11111;
	if (pJunkcode = 361099604.425149)
		pJunkcode = 1686711438.43585;
	pJunkcode = 3496580161.39796;
	pJunkcode = 3906878884.53208;
	if (pJunkcode = 1708309254.19774)
		pJunkcode = 9888174803.61477;
	pJunkcode = 1031138085.08389;
	if (pJunkcode = 6330048048.89056)
		pJunkcode = 3717392524.01474;
	pJunkcode = 9281697987.38289;
	pJunkcode = 2041625923.5016;
	if (pJunkcode = 8469679191.19583)
		pJunkcode = 5645078085.80945;
	pJunkcode = 104947265.583036;
	if (pJunkcode = 1695164621.99445)
		pJunkcode = 9382398291.96289;
	pJunkcode = 4940901144.10318;
	pJunkcode = 9841519960.32032;
	if (pJunkcode = 7266393957.08654)
		pJunkcode = 196757486.599017;
	pJunkcode = 7947901064.63384;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3399() {
	float pJunkcode = 4371066356.86927;
	pJunkcode = 5540857829.20787;
	if (pJunkcode = 2194455540.27143)
		pJunkcode = 7104335787.24585;
	pJunkcode = 149642425.543366;
	pJunkcode = 3625894875.01209;
	if (pJunkcode = 198874429.162155)
		pJunkcode = 7432617600.10962;
	pJunkcode = 9806263858.1673;
	if (pJunkcode = 1297636788.06701)
		pJunkcode = 3477305466.98557;
	pJunkcode = 2710065027.30075;
	pJunkcode = 6631566782.41295;
	if (pJunkcode = 6610925986.48666)
		pJunkcode = 2735346212.62218;
	pJunkcode = 6475710167.04387;
	if (pJunkcode = 535188134.389366)
		pJunkcode = 1917901941.60212;
	pJunkcode = 9030137029.90382;
	pJunkcode = 5577039184.10114;
	if (pJunkcode = 1420618920.74679)
		pJunkcode = 7724546435.67233;
	pJunkcode = 4779004191.57286;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3398() {
	float pJunkcode = 3932174910.33032;
	pJunkcode = 6668125717.31347;
	if (pJunkcode = 6645909667.86934)
		pJunkcode = 700447425.813205;
	pJunkcode = 5537128718.19749;
	pJunkcode = 1562455483.41667;
	if (pJunkcode = 4912552534.20211)
		pJunkcode = 1128284479.63252;
	pJunkcode = 4571037360.59237;
	if (pJunkcode = 295459518.879918)
		pJunkcode = 6776243952.97424;
	pJunkcode = 8279924977.99969;
	pJunkcode = 457768954.710227;
	if (pJunkcode = 9071559925.26098)
		pJunkcode = 1862840769.11925;
	pJunkcode = 6294324611.41474;
	if (pJunkcode = 6862564389.64826)
		pJunkcode = 9106649860.17363;
	pJunkcode = 7849583059.37623;
	pJunkcode = 7292617026.07478;
	if (pJunkcode = 5751009714.34686)
		pJunkcode = 430979691.4965;
	pJunkcode = 8766289328.74524;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3397() {
	float pJunkcode = 3817110809.98294;
	pJunkcode = 7920457507.28839;
	if (pJunkcode = 2412269844.84856)
		pJunkcode = 7186710456.84895;
	pJunkcode = 1479621557.63331;
	pJunkcode = 2872892217.42554;
	if (pJunkcode = 4561202552.85609)
		pJunkcode = 4227844400.53128;
	pJunkcode = 2957527850.20279;
	if (pJunkcode = 1797812824.13925)
		pJunkcode = 635642709.637092;
	pJunkcode = 2583748616.19601;
	pJunkcode = 9879403571.92087;
	if (pJunkcode = 7031132888.54406)
		pJunkcode = 9596710232.36938;
	pJunkcode = 8724193499.02144;
	if (pJunkcode = 7353807576.25151)
		pJunkcode = 8675693989.58064;
	pJunkcode = 5833894322.1979;
	pJunkcode = 2037647893.41103;
	if (pJunkcode = 469254356.878902)
		pJunkcode = 8171169165.28023;
	pJunkcode = 2609906705.12973;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3396() {
	float pJunkcode = 2028085182.81374;
	pJunkcode = 6054811769.54652;
	if (pJunkcode = 2808101154.30184)
		pJunkcode = 2247090881.52179;
	pJunkcode = 7673863234.11868;
	pJunkcode = 1460694601.62299;
	if (pJunkcode = 4814010161.61016)
		pJunkcode = 8090410938.85223;
	pJunkcode = 4078457944.34807;
	if (pJunkcode = 2952509578.1236)
		pJunkcode = 535432909.125549;
	pJunkcode = 5762911364.64939;
	pJunkcode = 3755885577.70035;
	if (pJunkcode = 7526018795.21129)
		pJunkcode = 3224565937.2309;
	pJunkcode = 3424784591.11988;
	if (pJunkcode = 1127626939.87408)
		pJunkcode = 2890549104.28891;
	pJunkcode = 4785963895.57453;
	pJunkcode = 5012304435.92247;
	if (pJunkcode = 237154319.211954)
		pJunkcode = 8108113935.03516;
	pJunkcode = 4634831220.04154;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3395() {
	float pJunkcode = 7551242390.44892;
	pJunkcode = 2235765504.58252;
	if (pJunkcode = 7433033950.68793)
		pJunkcode = 6744872374.73207;
	pJunkcode = 5976816928.84259;
	pJunkcode = 8886953560.08636;
	if (pJunkcode = 7188449772.83918)
		pJunkcode = 1595803878.82868;
	pJunkcode = 6843174349.76017;
	if (pJunkcode = 4010707600.74063)
		pJunkcode = 4779192097.99303;
	pJunkcode = 9148208580.5851;
	pJunkcode = 5415079605.41056;
	if (pJunkcode = 5852221626.55383)
		pJunkcode = 7233452638.70104;
	pJunkcode = 5562674782.98013;
	if (pJunkcode = 7853769612.83429)
		pJunkcode = 2871151795.50081;
	pJunkcode = 4537565745.45103;
	pJunkcode = 5093704.03535552;
	if (pJunkcode = 7075906988.9447)
		pJunkcode = 1739344996.22132;
	pJunkcode = 2735189451.64575;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3394() {
	float pJunkcode = 6004113252.54628;
	pJunkcode = 3107122985.74846;
	if (pJunkcode = 4350093228.79053)
		pJunkcode = 8188837258.17041;
	pJunkcode = 6077090549.92365;
	pJunkcode = 4843624281.87977;
	if (pJunkcode = 9895298127.71503)
		pJunkcode = 5399132428.08886;
	pJunkcode = 6875962571.88693;
	if (pJunkcode = 4385070912.5158)
		pJunkcode = 2562280576.60881;
	pJunkcode = 3669189260.6658;
	pJunkcode = 7261495528.7261;
	if (pJunkcode = 4566347786.21562)
		pJunkcode = 898518832.205522;
	pJunkcode = 1215728555.06942;
	if (pJunkcode = 1748461323.7159)
		pJunkcode = 6594956783.60632;
	pJunkcode = 3055781299.15341;
	pJunkcode = 868898127.551156;
	if (pJunkcode = 365878847.970976)
		pJunkcode = 3513881552.01979;
	pJunkcode = 8572424797.06634;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3393() {
	float pJunkcode = 4454814666.96712;
	pJunkcode = 4219562150.20866;
	if (pJunkcode = 9925257590.58335)
		pJunkcode = 492658412.801974;
	pJunkcode = 7218137867.32113;
	pJunkcode = 4200765005.29726;
	if (pJunkcode = 2176393385.45757)
		pJunkcode = 2711777283.98417;
	pJunkcode = 698758237.759263;
	if (pJunkcode = 5015465708.93945)
		pJunkcode = 8455411097.17918;
	pJunkcode = 7401304112.53212;
	pJunkcode = 3251210635.01536;
	if (pJunkcode = 5117847404.15311)
		pJunkcode = 2247493872.41137;
	pJunkcode = 7314471660.7816;
	if (pJunkcode = 445699616.798436)
		pJunkcode = 3857231098.43607;
	pJunkcode = 2523806000.1783;
	pJunkcode = 3596916278.25542;
	if (pJunkcode = 6567450185.66621)
		pJunkcode = 2297038126.74041;
	pJunkcode = 2515180400.26735;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3392() {
	float pJunkcode = 5410163721.91132;
	pJunkcode = 595561790.123803;
	if (pJunkcode = 2034373437.89341)
		pJunkcode = 6644893167.38403;
	pJunkcode = 4059035450.52111;
	pJunkcode = 7167668390.51953;
	if (pJunkcode = 551561089.507874)
		pJunkcode = 4943871113.73487;
	pJunkcode = 5339516432.94502;
	if (pJunkcode = 8152424907.32066)
		pJunkcode = 7099999655.56489;
	pJunkcode = 9920275791.05976;
	pJunkcode = 5154003331.86607;
	if (pJunkcode = 917856620.763085)
		pJunkcode = 7759133223.93901;
	pJunkcode = 9517575284.95582;
	if (pJunkcode = 2766040497.95163)
		pJunkcode = 2969075296.46094;
	pJunkcode = 2947957360.81148;
	pJunkcode = 6957188247.7236;
	if (pJunkcode = 4489873095.55214)
		pJunkcode = 460780565.686113;
	pJunkcode = 689598299.19293;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3391() {
	float pJunkcode = 2460218240.35386;
	pJunkcode = 6948904339.66338;
	if (pJunkcode = 1709911471.56277)
		pJunkcode = 8431030375.36726;
	pJunkcode = 8510744406.38185;
	pJunkcode = 1381354849.11014;
	if (pJunkcode = 1169610842.04186)
		pJunkcode = 5728316629.95231;
	pJunkcode = 1677817829.85114;
	if (pJunkcode = 9176135645.50813)
		pJunkcode = 3201490062.13141;
	pJunkcode = 3153540155.57035;
	pJunkcode = 4182797294.96123;
	if (pJunkcode = 2672619088.69563)
		pJunkcode = 7109083696.757;
	pJunkcode = 9731869730.64269;
	if (pJunkcode = 1219608478.29276)
		pJunkcode = 5155777327.63387;
	pJunkcode = 6750331134.70102;
	pJunkcode = 2579311357.86458;
	if (pJunkcode = 8863743901.83689)
		pJunkcode = 4887291791.20677;
	pJunkcode = 4098193922.87535;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3390() {
	float pJunkcode = 751797147.313423;
	pJunkcode = 8857551504.50291;
	if (pJunkcode = 2328309693.57973)
		pJunkcode = 9698488543.69075;
	pJunkcode = 1886604125.69853;
	pJunkcode = 7810910208.68068;
	if (pJunkcode = 9308547788.28689)
		pJunkcode = 5922274362.80584;
	pJunkcode = 1324841447.26296;
	if (pJunkcode = 9785267410.0045)
		pJunkcode = 7844576577.72293;
	pJunkcode = 3318314721.72451;
	pJunkcode = 7218386988.96196;
	if (pJunkcode = 5552363687.67233)
		pJunkcode = 2083366559.66068;
	pJunkcode = 9604247660.25471;
	if (pJunkcode = 5244777776.54051)
		pJunkcode = 2603270150.53174;
	pJunkcode = 4775455254.7543;
	pJunkcode = 4201347169.2169;
	if (pJunkcode = 4775571980.90191)
		pJunkcode = 4035079246.90481;
	pJunkcode = 8049984842.04834;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3389() {
	float pJunkcode = 1804925279.81375;
	pJunkcode = 6810882873.829;
	if (pJunkcode = 6724840008.08767)
		pJunkcode = 1355065959.55716;
	pJunkcode = 159871465.168003;
	pJunkcode = 1370663347.25934;
	if (pJunkcode = 6830520335.71122)
		pJunkcode = 7082324186.67369;
	pJunkcode = 3945572354.50324;
	if (pJunkcode = 8009074490.28798)
		pJunkcode = 6345395278.86591;
	pJunkcode = 117850061.343477;
	pJunkcode = 1232549083.04332;
	if (pJunkcode = 3541582752.52241)
		pJunkcode = 6425320643.48575;
	pJunkcode = 3392190568.76264;
	if (pJunkcode = 922026689.976415)
		pJunkcode = 4418495019.03939;
	pJunkcode = 2875560774.50507;
	pJunkcode = 2024235087.22587;
	if (pJunkcode = 4034522566.04429)
		pJunkcode = 369747309.056352;
	pJunkcode = 7620757998.76233;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3388() {
	float pJunkcode = 8310027402.11018;
	pJunkcode = 737586180.052219;
	if (pJunkcode = 5178114836.62525)
		pJunkcode = 3889531102.86842;
	pJunkcode = 9340381351.45717;
	pJunkcode = 8695315742.53642;
	if (pJunkcode = 3380273984.85197)
		pJunkcode = 371226367.227431;
	pJunkcode = 193639333.386038;
	if (pJunkcode = 4285829207.48281)
		pJunkcode = 2158084767.90223;
	pJunkcode = 1603738956.52204;
	pJunkcode = 3182425478.97561;
	if (pJunkcode = 9275466409.70002)
		pJunkcode = 3692390427.66661;
	pJunkcode = 8342195842.2464;
	if (pJunkcode = 5997477193.87261)
		pJunkcode = 2734102036.31216;
	pJunkcode = 5142449412.98076;
	pJunkcode = 1075490638.17822;
	if (pJunkcode = 4600466892.2453)
		pJunkcode = 6125852694.3607;
	pJunkcode = 8484913827.72617;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3387() {
	float pJunkcode = 8317053937.15148;
	pJunkcode = 4516674747.56053;
	if (pJunkcode = 5565069259.22342)
		pJunkcode = 9711547520.68511;
	pJunkcode = 4998046861.08911;
	pJunkcode = 628104650.787025;
	if (pJunkcode = 8104352682.07586)
		pJunkcode = 5424985579.44971;
	pJunkcode = 6424398159.07417;
	if (pJunkcode = 1402972832.46069)
		pJunkcode = 9173323524.1611;
	pJunkcode = 4519133572.89361;
	pJunkcode = 4629520937.01755;
	if (pJunkcode = 3139378526.95744)
		pJunkcode = 480794964.969424;
	pJunkcode = 5371102774.35352;
	if (pJunkcode = 7906736247.8228)
		pJunkcode = 1206152589.55613;
	pJunkcode = 2707473487.49965;
	pJunkcode = 3044487575.22361;
	if (pJunkcode = 7944689759.14018)
		pJunkcode = 3578296827.91428;
	pJunkcode = 1261847415.94057;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3386() {
	float pJunkcode = 1658483736.27894;
	pJunkcode = 8562860438.04458;
	if (pJunkcode = 156913746.962265)
		pJunkcode = 8261496614.51351;
	pJunkcode = 1478473884.00343;
	pJunkcode = 4732883745.14088;
	if (pJunkcode = 9293344141.20532)
		pJunkcode = 162649605.222415;
	pJunkcode = 5763084458.53214;
	if (pJunkcode = 4977550106.72838)
		pJunkcode = 7020200563.05359;
	pJunkcode = 9809183636.99906;
	pJunkcode = 2554129394.65801;
	if (pJunkcode = 4852576174.37666)
		pJunkcode = 2114041201.26517;
	pJunkcode = 7166953407.35913;
	if (pJunkcode = 4545277459.30009)
		pJunkcode = 866077857.213086;
	pJunkcode = 7729107726.3039;
	pJunkcode = 5244472906.48786;
	if (pJunkcode = 10687432.2504161)
		pJunkcode = 3936289305.63689;
	pJunkcode = 4810868101.58577;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3385() {
	float pJunkcode = 7138192558.6278;
	pJunkcode = 7358112969.93828;
	if (pJunkcode = 2096339641.63295)
		pJunkcode = 3720411185.03202;
	pJunkcode = 5408535967.93666;
	pJunkcode = 8032418101.65211;
	if (pJunkcode = 1405496533.07555)
		pJunkcode = 7957541847.22183;
	pJunkcode = 5534060273.82984;
	if (pJunkcode = 4262064398.05484)
		pJunkcode = 7762510196.09215;
	pJunkcode = 1382419023.52129;
	pJunkcode = 4840176776.71678;
	if (pJunkcode = 5523832233.03867)
		pJunkcode = 1323270463.81643;
	pJunkcode = 6578086781.94369;
	if (pJunkcode = 3765323981.2601)
		pJunkcode = 1966469613.86051;
	pJunkcode = 8273378469.75266;
	pJunkcode = 4806817154.32341;
	if (pJunkcode = 5657800148.59012)
		pJunkcode = 6718955505.24617;
	pJunkcode = 9767534972.6811;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3384() {
	float pJunkcode = 2105991227.45477;
	pJunkcode = 8626601198.66815;
	if (pJunkcode = 6550720954.33131)
		pJunkcode = 3826794409.69812;
	pJunkcode = 1573279140.94753;
	pJunkcode = 7930493739.45682;
	if (pJunkcode = 8711686986.36681)
		pJunkcode = 1825526965.23964;
	pJunkcode = 4812874613.67895;
	if (pJunkcode = 6080726680.9463)
		pJunkcode = 7341003602.14123;
	pJunkcode = 7566653413.05438;
	pJunkcode = 2814440389.46944;
	if (pJunkcode = 8253980420.57053)
		pJunkcode = 3888134603.28967;
	pJunkcode = 626487921.233095;
	if (pJunkcode = 1552835122.20022)
		pJunkcode = 7884891145.10721;
	pJunkcode = 3054652046.89984;
	pJunkcode = 3567611791.70579;
	if (pJunkcode = 4411682434.59361)
		pJunkcode = 7525779920.34462;
	pJunkcode = 5615534000.29709;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3383() {
	float pJunkcode = 3303259113.87068;
	pJunkcode = 3825203419.31444;
	if (pJunkcode = 6615257810.84378)
		pJunkcode = 7538805018.49081;
	pJunkcode = 4486195490.82169;
	pJunkcode = 1314678735.76879;
	if (pJunkcode = 6422015412.72141)
		pJunkcode = 9547942958.72454;
	pJunkcode = 5893659730.25654;
	if (pJunkcode = 2575711117.32674)
		pJunkcode = 7732455275.94284;
	pJunkcode = 4176708203.55076;
	pJunkcode = 1176684122.02402;
	if (pJunkcode = 6792322594.47592)
		pJunkcode = 8169091337.87407;
	pJunkcode = 1035778060.04462;
	if (pJunkcode = 9656601588.90519)
		pJunkcode = 7824487142.53016;
	pJunkcode = 5670563137.93206;
	pJunkcode = 8461224242.7986;
	if (pJunkcode = 3723140701.22106)
		pJunkcode = 5483006381.65879;
	pJunkcode = 2808588256.74861;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3382() {
	float pJunkcode = 9932716566.96783;
	pJunkcode = 4807111025.56644;
	if (pJunkcode = 2079415656.39713)
		pJunkcode = 8790631589.64775;
	pJunkcode = 6585190823.20288;
	pJunkcode = 5187717626.28875;
	if (pJunkcode = 7173233695.05385)
		pJunkcode = 9085967713.93854;
	pJunkcode = 8250365351.19729;
	if (pJunkcode = 8630747300.5347)
		pJunkcode = 2115373519.39052;
	pJunkcode = 9107307725.89171;
	pJunkcode = 5139970038.22057;
	if (pJunkcode = 2676673384.02916)
		pJunkcode = 6334008244.33218;
	pJunkcode = 5290475821.18627;
	if (pJunkcode = 6109782374.14301)
		pJunkcode = 2684043946.18919;
	pJunkcode = 90428625.0162646;
	pJunkcode = 3979985555.29114;
	if (pJunkcode = 138299360.636981)
		pJunkcode = 6383291037.11967;
	pJunkcode = 1863902410.35833;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3381() {
	float pJunkcode = 7908898638.47309;
	pJunkcode = 1818231799.87297;
	if (pJunkcode = 1051755130.19373)
		pJunkcode = 2807392590.2181;
	pJunkcode = 7061598976.51339;
	pJunkcode = 327832538.27196;
	if (pJunkcode = 30390735.6519229)
		pJunkcode = 3112200431.14399;
	pJunkcode = 2641779953.88636;
	if (pJunkcode = 915651669.096753)
		pJunkcode = 6801034648.19691;
	pJunkcode = 7313810242.10343;
	pJunkcode = 1606773666.39509;
	if (pJunkcode = 988878141.665463)
		pJunkcode = 5642397577.6525;
	pJunkcode = 1730676851.59231;
	if (pJunkcode = 9359057861.03355)
		pJunkcode = 4964556400.82393;
	pJunkcode = 3319700972.20082;
	pJunkcode = 2782821880.50088;
	if (pJunkcode = 4627843450.91474)
		pJunkcode = 1036550717.10701;
	pJunkcode = 4685294268.45335;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3380() {
	float pJunkcode = 6713117311.95658;
	pJunkcode = 3850360984.44185;
	if (pJunkcode = 5850623746.7803)
		pJunkcode = 2880639763.55232;
	pJunkcode = 4409324421.59279;
	pJunkcode = 6231981328.24884;
	if (pJunkcode = 7278084474.01617)
		pJunkcode = 7511062972.91683;
	pJunkcode = 8124107058.69662;
	if (pJunkcode = 6407187168.70233)
		pJunkcode = 2826789857.09709;
	pJunkcode = 4539703726.38075;
	pJunkcode = 2881317023.86907;
	if (pJunkcode = 3955767496.65345)
		pJunkcode = 9888921679.5978;
	pJunkcode = 195867968.348604;
	if (pJunkcode = 9503421947.35923)
		pJunkcode = 3277983562.99448;
	pJunkcode = 1777894038.13191;
	pJunkcode = 1669854961.77006;
	if (pJunkcode = 2841001830.17662)
		pJunkcode = 5038155708.35882;
	pJunkcode = 8672695514.77831;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3379() {
	float pJunkcode = 1527910839.40335;
	pJunkcode = 151560928.533354;
	if (pJunkcode = 6763338586.7892)
		pJunkcode = 7645172964.96286;
	pJunkcode = 3011816556.5425;
	pJunkcode = 3192475501.56516;
	if (pJunkcode = 7828783029.83681)
		pJunkcode = 6610467244.85698;
	pJunkcode = 4574855481.86769;
	if (pJunkcode = 7502533966.48907)
		pJunkcode = 2014608493.19542;
	pJunkcode = 6169875569.54538;
	pJunkcode = 6820609083.45417;
	if (pJunkcode = 4764021659.21407)
		pJunkcode = 2459098203.44501;
	pJunkcode = 9710458405.70482;
	if (pJunkcode = 5010950457.77895)
		pJunkcode = 6422079102.90706;
	pJunkcode = 1872642729.4368;
	pJunkcode = 5433625412.15557;
	if (pJunkcode = 790900569.740788)
		pJunkcode = 4735380400.41863;
	pJunkcode = 6955864248.55396;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3378() {
	float pJunkcode = 6068890909.41559;
	pJunkcode = 4399367504.71825;
	if (pJunkcode = 8165440459.50593)
		pJunkcode = 7344733057.73751;
	pJunkcode = 228389893.089712;
	pJunkcode = 8475816068.28973;
	if (pJunkcode = 1723069826.66696)
		pJunkcode = 7525135540.04256;
	pJunkcode = 1364048627.30077;
	if (pJunkcode = 578200471.821894)
		pJunkcode = 5253999985.03978;
	pJunkcode = 7500686231.66552;
	pJunkcode = 1851606989.16751;
	if (pJunkcode = 6727241864.98855)
		pJunkcode = 7980766853.51385;
	pJunkcode = 9899127592.38588;
	if (pJunkcode = 8747156752.73652)
		pJunkcode = 4138715830.85928;
	pJunkcode = 7753937194.42592;
	pJunkcode = 4839652973.54267;
	if (pJunkcode = 6188656286.09085)
		pJunkcode = 7172642930.45625;
	pJunkcode = 8013600723.96011;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3377() {
	float pJunkcode = 9810774578.48878;
	pJunkcode = 2357932025.70402;
	if (pJunkcode = 9335381512.39571)
		pJunkcode = 2164633038.65961;
	pJunkcode = 7722391304.60892;
	pJunkcode = 6979222949.52121;
	if (pJunkcode = 4023616943.6333)
		pJunkcode = 8020143048.28231;
	pJunkcode = 4398827736.50655;
	if (pJunkcode = 715543168.612038)
		pJunkcode = 3192818967.56765;
	pJunkcode = 6476268908.43499;
	pJunkcode = 5370348554.96543;
	if (pJunkcode = 3980324113.27273)
		pJunkcode = 48623460.4895951;
	pJunkcode = 1481726965.88171;
	if (pJunkcode = 9453954391.33382)
		pJunkcode = 4961859378.15458;
	pJunkcode = 4970249937.88121;
	pJunkcode = 7415281720.62944;
	if (pJunkcode = 2069300737.57125)
		pJunkcode = 5144482916.86053;
	pJunkcode = 6069380260.98389;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3376() {
	float pJunkcode = 8375910485.91155;
	pJunkcode = 6097305640.22992;
	if (pJunkcode = 6156579550.04319)
		pJunkcode = 4022117950.70186;
	pJunkcode = 9780249398.69303;
	pJunkcode = 8614776204.76016;
	if (pJunkcode = 7121852588.29673)
		pJunkcode = 2225849437.43591;
	pJunkcode = 2961188851.94608;
	if (pJunkcode = 7046075129.96289)
		pJunkcode = 5694808479.39174;
	pJunkcode = 1100473130.3822;
	pJunkcode = 1970115290.84409;
	if (pJunkcode = 5185687402.95993)
		pJunkcode = 8187982277.67229;
	pJunkcode = 5540689268.49242;
	if (pJunkcode = 9480686269.35267)
		pJunkcode = 3568072510.04787;
	pJunkcode = 2786281138.83723;
	pJunkcode = 8563207650.25465;
	if (pJunkcode = 4696264253.92383)
		pJunkcode = 6409751046.17128;
	pJunkcode = 7799176537.02281;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3375() {
	float pJunkcode = 5619842190.25192;
	pJunkcode = 3197003795.76788;
	if (pJunkcode = 3637864315.6069)
		pJunkcode = 3915305711.92538;
	pJunkcode = 1517725713.52727;
	pJunkcode = 8326499034.52529;
	if (pJunkcode = 4688097032.4304)
		pJunkcode = 1420642762.36628;
	pJunkcode = 3938457989.63655;
	if (pJunkcode = 4657980724.76746)
		pJunkcode = 750887903.848136;
	pJunkcode = 7464713700.48827;
	pJunkcode = 3503281395.70194;
	if (pJunkcode = 8066475196.18829)
		pJunkcode = 1277417127.34091;
	pJunkcode = 2970867154.81709;
	if (pJunkcode = 7464873548.76815)
		pJunkcode = 9406084666.188;
	pJunkcode = 7242070082.6361;
	pJunkcode = 1348241950.22594;
	if (pJunkcode = 8646738922.45352)
		pJunkcode = 6905831825.42447;
	pJunkcode = 6774988081.69795;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3374() {
	float pJunkcode = 7835453176.90412;
	pJunkcode = 3579339384.21635;
	if (pJunkcode = 1276880302.07912)
		pJunkcode = 7654896941.76448;
	pJunkcode = 9122156097.91865;
	pJunkcode = 500890884.430779;
	if (pJunkcode = 2795225182.69161)
		pJunkcode = 4917643665.24602;
	pJunkcode = 9512506116.6069;
	if (pJunkcode = 3773693537.5444)
		pJunkcode = 3785824416.6365;
	pJunkcode = 1203080117.25862;
	pJunkcode = 7144333170.28788;
	if (pJunkcode = 7498070456.76508)
		pJunkcode = 211899701.929589;
	pJunkcode = 3411009471.48011;
	if (pJunkcode = 6923573755.15829)
		pJunkcode = 7185207543.08868;
	pJunkcode = 2647777355.17796;
	pJunkcode = 401708175.764607;
	if (pJunkcode = 5883951415.81569)
		pJunkcode = 5440836968.87421;
	pJunkcode = 4892134841.35241;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3373() {
	float pJunkcode = 9047927404.22521;
	pJunkcode = 7329812639.31668;
	if (pJunkcode = 2649130306.54861)
		pJunkcode = 716708295.052432;
	pJunkcode = 5517386334.56326;
	pJunkcode = 3366153934.71962;
	if (pJunkcode = 906500061.127543)
		pJunkcode = 2293784656.03885;
	pJunkcode = 9055891075.56397;
	if (pJunkcode = 5946110608.61751)
		pJunkcode = 8639039534.53548;
	pJunkcode = 3183641615.77161;
	pJunkcode = 6281333948.57638;
	if (pJunkcode = 6265396685.08434)
		pJunkcode = 5563594908.84088;
	pJunkcode = 3026632442.0346;
	if (pJunkcode = 9838731345.32)
		pJunkcode = 2966396127.0668;
	pJunkcode = 6614205233.61101;
	pJunkcode = 804334540.019808;
	if (pJunkcode = 2556250982.14604)
		pJunkcode = 9374929418.04673;
	pJunkcode = 3989968469.77536;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3372() {
	float pJunkcode = 3567799358.19662;
	pJunkcode = 8659358815.51286;
	if (pJunkcode = 3095051664.13943)
		pJunkcode = 859804745.287042;
	pJunkcode = 1326552591.02704;
	pJunkcode = 5564946118.65573;
	if (pJunkcode = 7879173414.85969)
		pJunkcode = 5217020646.01999;
	pJunkcode = 1445847458.92956;
	if (pJunkcode = 4728245602.53899)
		pJunkcode = 1578744541.18003;
	pJunkcode = 1820463273.0127;
	pJunkcode = 6239698951.28053;
	if (pJunkcode = 2394865579.03561)
		pJunkcode = 3402113122.50612;
	pJunkcode = 1845888888.72297;
	if (pJunkcode = 6116992561.83116)
		pJunkcode = 4538175464.60179;
	pJunkcode = 1307790072.34269;
	pJunkcode = 6940691991.38587;
	if (pJunkcode = 6720199942.32389)
		pJunkcode = 7604470433.42847;
	pJunkcode = 3464847287.16092;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3371() {
	float pJunkcode = 961535354.743167;
	pJunkcode = 5125630130.63834;
	if (pJunkcode = 7331374391.12051)
		pJunkcode = 8624052555.15587;
	pJunkcode = 6265282773.83107;
	pJunkcode = 2308763074.32131;
	if (pJunkcode = 838084685.346481)
		pJunkcode = 6334811450.79782;
	pJunkcode = 5038265750.00549;
	if (pJunkcode = 5750506528.12007)
		pJunkcode = 2063081986.40026;
	pJunkcode = 6425544072.52198;
	pJunkcode = 3318946161.4288;
	if (pJunkcode = 9622537579.36804)
		pJunkcode = 6241675901.16252;
	pJunkcode = 7115543555.6182;
	if (pJunkcode = 9552927493.98084)
		pJunkcode = 4118366764.15552;
	pJunkcode = 5732856849.62135;
	pJunkcode = 800501247.651462;
	if (pJunkcode = 4362431925.78032)
		pJunkcode = 6122361652.63103;
	pJunkcode = 6928870247.76116;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3370() {
	float pJunkcode = 5747642199.67989;
	pJunkcode = 3569687262.88941;
	if (pJunkcode = 3110800897.8123)
		pJunkcode = 7662602162.03182;
	pJunkcode = 1161743510.40341;
	pJunkcode = 2247417223.00168;
	if (pJunkcode = 5975037987.80138)
		pJunkcode = 5328563120.3102;
	pJunkcode = 2510808978.79865;
	if (pJunkcode = 4418709556.36227)
		pJunkcode = 9886331711.38783;
	pJunkcode = 3693302388.23834;
	pJunkcode = 6355406691.53593;
	if (pJunkcode = 3827326732.06029)
		pJunkcode = 7992810603.59378;
	pJunkcode = 5114313619.80132;
	if (pJunkcode = 4828929259.89444)
		pJunkcode = 6408371903.94344;
	pJunkcode = 7720569374.14756;
	pJunkcode = 5408581190.18035;
	if (pJunkcode = 222699026.573215)
		pJunkcode = 4206204461.47059;
	pJunkcode = 414838612.425506;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3369() {
	float pJunkcode = 4989719048.59346;
	pJunkcode = 6144294161.42447;
	if (pJunkcode = 8964164276.93337)
		pJunkcode = 1542773472.1646;
	pJunkcode = 7376992096.02778;
	pJunkcode = 5298896753.36663;
	if (pJunkcode = 749325216.074476)
		pJunkcode = 8470254370.266;
	pJunkcode = 6789024118.061;
	if (pJunkcode = 634536970.715087)
		pJunkcode = 1139244278.8642;
	pJunkcode = 6440623815.35899;
	pJunkcode = 3943968984.63804;
	if (pJunkcode = 1346354462.29584)
		pJunkcode = 1623771341.79271;
	pJunkcode = 7892963950.06222;
	if (pJunkcode = 9970629889.7897)
		pJunkcode = 6571691439.80867;
	pJunkcode = 2599446744.05367;
	pJunkcode = 6081576664.94877;
	if (pJunkcode = 8866306761.8461)
		pJunkcode = 5410976402.16111;
	pJunkcode = 5079584366.21678;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3368() {
	float pJunkcode = 6403626166.72047;
	pJunkcode = 8447196741.92159;
	if (pJunkcode = 9472278305.70948)
		pJunkcode = 3334340215.50665;
	pJunkcode = 7265599709.85448;
	pJunkcode = 1680173405.1395;
	if (pJunkcode = 2962514557.74727)
		pJunkcode = 3287987685.80995;
	pJunkcode = 5958178759.1049;
	if (pJunkcode = 3283403340.5186)
		pJunkcode = 9601191388.15138;
	pJunkcode = 7977446654.21711;
	pJunkcode = 157204170.017951;
	if (pJunkcode = 4054413096.00728)
		pJunkcode = 9645760799.86689;
	pJunkcode = 1898372751.87138;
	if (pJunkcode = 4209205183.06617)
		pJunkcode = 5156014912.63167;
	pJunkcode = 1080622206.84393;
	pJunkcode = 8252801575.8858;
	if (pJunkcode = 5691702749.89767)
		pJunkcode = 4393075207.73902;
	pJunkcode = 871983684.799989;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3367() {
	float pJunkcode = 3835548942.94524;
	pJunkcode = 8430393315.18744;
	if (pJunkcode = 214737352.901398)
		pJunkcode = 8361952677.53417;
	pJunkcode = 6040816482.79522;
	pJunkcode = 4539708636.72992;
	if (pJunkcode = 4905825306.94501)
		pJunkcode = 1074247013.67131;
	pJunkcode = 398715989.655069;
	if (pJunkcode = 5604266450.9265)
		pJunkcode = 8996470674.83127;
	pJunkcode = 6389556104.82742;
	pJunkcode = 3241018477.72415;
	if (pJunkcode = 2429179420.33995)
		pJunkcode = 1206844089.65264;
	pJunkcode = 8894758954.73637;
	if (pJunkcode = 4426976991.81918)
		pJunkcode = 4814512835.61466;
	pJunkcode = 2007961605.6768;
	pJunkcode = 7589600836.77231;
	if (pJunkcode = 8754210887.24632)
		pJunkcode = 474943155.879959;
	pJunkcode = 9146354454.6243;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3366() {
	float pJunkcode = 3813259717.88989;
	pJunkcode = 6575674829.60706;
	if (pJunkcode = 2372110480.81927)
		pJunkcode = 3059911934.23497;
	pJunkcode = 515349930.26196;
	pJunkcode = 4083881663.65556;
	if (pJunkcode = 1600137023.32235)
		pJunkcode = 465360685.143543;
	pJunkcode = 3418894414.25233;
	if (pJunkcode = 5900357646.75702)
		pJunkcode = 3056603169.1368;
	pJunkcode = 9994407672.4114;
	pJunkcode = 1573296314.36407;
	if (pJunkcode = 6055683432.19343)
		pJunkcode = 7618191404.18694;
	pJunkcode = 9482094608.25126;
	if (pJunkcode = 3592560249.06664)
		pJunkcode = 4263637716.76938;
	pJunkcode = 3124148649.43787;
	pJunkcode = 9274259043.54666;
	if (pJunkcode = 8632646322.77955)
		pJunkcode = 6265728146.45888;
	pJunkcode = 3510992171.25288;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3365() {
	float pJunkcode = 2870736760.40248;
	pJunkcode = 3417646971.36105;
	if (pJunkcode = 3612002461.12363)
		pJunkcode = 5113339158.79663;
	pJunkcode = 3225512230.666;
	pJunkcode = 818508354.866736;
	if (pJunkcode = 4626932240.05019)
		pJunkcode = 6754152594.73439;
	pJunkcode = 8602372720.96335;
	if (pJunkcode = 6750202704.27401)
		pJunkcode = 7940936261.69206;
	pJunkcode = 5520803993.46451;
	pJunkcode = 6969038978.47357;
	if (pJunkcode = 2428626420.49509)
		pJunkcode = 2714696853.27045;
	pJunkcode = 9032777743.04511;
	if (pJunkcode = 9990796146.68511)
		pJunkcode = 754556290.905587;
	pJunkcode = 3503306219.85692;
	pJunkcode = 1148098339.23127;
	if (pJunkcode = 1036731168.36452)
		pJunkcode = 262355842.325292;
	pJunkcode = 8236519156.0577;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3364() {
	float pJunkcode = 710204250.033551;
	pJunkcode = 8984407292.66778;
	if (pJunkcode = 2394193797.16912)
		pJunkcode = 9505340252.44515;
	pJunkcode = 9348324361.61123;
	pJunkcode = 2133142946.14837;
	if (pJunkcode = 6842210249.61551)
		pJunkcode = 8912437622.68863;
	pJunkcode = 7891047062.90074;
	if (pJunkcode = 743708482.093578)
		pJunkcode = 4250415301.28251;
	pJunkcode = 5869807174.82137;
	pJunkcode = 3131973983.5048;
	if (pJunkcode = 640037900.548509)
		pJunkcode = 2285440785.37429;
	pJunkcode = 2241536119.45359;
	if (pJunkcode = 1393005247.39275)
		pJunkcode = 9667538917.87616;
	pJunkcode = 6587667606.58392;
	pJunkcode = 6972256563.34689;
	if (pJunkcode = 1078548204.80698)
		pJunkcode = 962379373.6842;
	pJunkcode = 3990535264.6979;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3363() {
	float pJunkcode = 6476163728.24354;
	pJunkcode = 7518770293.29558;
	if (pJunkcode = 4535978033.29004)
		pJunkcode = 4659210819.62097;
	pJunkcode = 2858316026.35579;
	pJunkcode = 3103595979.02893;
	if (pJunkcode = 3915040019.89335)
		pJunkcode = 7400162888.24658;
	pJunkcode = 4176885073.90878;
	if (pJunkcode = 1321615147.6258)
		pJunkcode = 4974397613.66717;
	pJunkcode = 7270258883.83809;
	pJunkcode = 6250234248.61458;
	if (pJunkcode = 2695338859.87168)
		pJunkcode = 3691858761.88029;
	pJunkcode = 6472863652.2981;
	if (pJunkcode = 2398949622.84299)
		pJunkcode = 2728471334.96063;
	pJunkcode = 6953401020.43744;
	pJunkcode = 8049557392.54424;
	if (pJunkcode = 5144903167.31313)
		pJunkcode = 5277802494.02856;
	pJunkcode = 9501067864.50036;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3362() {
	float pJunkcode = 1010552513.70571;
	pJunkcode = 2749816620.47515;
	if (pJunkcode = 7076062082.87039)
		pJunkcode = 9637606933.40562;
	pJunkcode = 7271342087.30745;
	pJunkcode = 4552675856.08521;
	if (pJunkcode = 5532932174.07643)
		pJunkcode = 2431382455.82924;
	pJunkcode = 5335194403.67512;
	if (pJunkcode = 3592799636.15561)
		pJunkcode = 41817509.3225162;
	pJunkcode = 3601346346.84847;
	pJunkcode = 7338941034.30285;
	if (pJunkcode = 4636390450.31471)
		pJunkcode = 3715623564.32341;
	pJunkcode = 1075572174.65425;
	if (pJunkcode = 9751778863.58808)
		pJunkcode = 5238313773.3353;
	pJunkcode = 1787893993.32054;
	pJunkcode = 4978775596.2378;
	if (pJunkcode = 7191272520.47052)
		pJunkcode = 3049745283.28571;
	pJunkcode = 197954391.77617;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3361() {
	float pJunkcode = 3128937662.45981;
	pJunkcode = 5745174280.07281;
	if (pJunkcode = 1378638178.05925)
		pJunkcode = 517728190.013086;
	pJunkcode = 5734229093.1462;
	pJunkcode = 9225315404.78125;
	if (pJunkcode = 1075512777.82272)
		pJunkcode = 9843923481.94366;
	pJunkcode = 8050466361.27212;
	if (pJunkcode = 2180147653.56741)
		pJunkcode = 3257570889.92326;
	pJunkcode = 2279311198.13495;
	pJunkcode = 3508143083.75423;
	if (pJunkcode = 7153340897.34307)
		pJunkcode = 531383300.626002;
	pJunkcode = 8999022543.01572;
	if (pJunkcode = 1182841983.93398)
		pJunkcode = 7370120463.99295;
	pJunkcode = 9822588189.42044;
	pJunkcode = 8457005333.18956;
	if (pJunkcode = 3267720195.16303)
		pJunkcode = 8974129140.23831;
	pJunkcode = 1546164912.26926;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3360() {
	float pJunkcode = 1897847439.96978;
	pJunkcode = 2348853591.01775;
	if (pJunkcode = 350104934.496682)
		pJunkcode = 3883962815.80338;
	pJunkcode = 6975725055.65422;
	pJunkcode = 8084964604.93047;
	if (pJunkcode = 6059973012.18102)
		pJunkcode = 7032857904.21917;
	pJunkcode = 5649078409.21121;
	if (pJunkcode = 1393355048.65369)
		pJunkcode = 2032858419.38903;
	pJunkcode = 7572940264.17931;
	pJunkcode = 3298947226.6022;
	if (pJunkcode = 4166538558.42937)
		pJunkcode = 9775783968.13544;
	pJunkcode = 916754234.56882;
	if (pJunkcode = 6469647396.87446)
		pJunkcode = 8366038344.15491;
	pJunkcode = 7411155141.41713;
	pJunkcode = 7138593004.7039;
	if (pJunkcode = 1389936362.97958)
		pJunkcode = 3759229208.83874;
	pJunkcode = 8151235534.13951;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3359() {
	float pJunkcode = 2787117716.5853;
	pJunkcode = 87223112.2910931;
	if (pJunkcode = 9433369264.65773)
		pJunkcode = 4429247349.61586;
	pJunkcode = 271824846.510518;
	pJunkcode = 4033663403.94963;
	if (pJunkcode = 7510571265.12598)
		pJunkcode = 9562690292.57576;
	pJunkcode = 2729713683.56837;
	if (pJunkcode = 8780132231.69245)
		pJunkcode = 9635721270.84951;
	pJunkcode = 1035022771.97456;
	pJunkcode = 3533924943.08493;
	if (pJunkcode = 7923314599.42452)
		pJunkcode = 6163167660.0536;
	pJunkcode = 8370836099.88009;
	if (pJunkcode = 150909402.730299)
		pJunkcode = 1892726779.31064;
	pJunkcode = 6597994876.47479;
	pJunkcode = 8825216724.83338;
	if (pJunkcode = 260504147.092103)
		pJunkcode = 4087334011.64751;
	pJunkcode = 6463640309.92494;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3358() {
	float pJunkcode = 4375414854.06205;
	pJunkcode = 1133295177.80661;
	if (pJunkcode = 7529199975.04923)
		pJunkcode = 1518955554.59791;
	pJunkcode = 4290423268.69976;
	pJunkcode = 8638763195.26459;
	if (pJunkcode = 601748692.709531)
		pJunkcode = 5423702276.3413;
	pJunkcode = 7197987058.58444;
	if (pJunkcode = 3929222062.89294)
		pJunkcode = 9953203139.74634;
	pJunkcode = 8270248572.67946;
	pJunkcode = 9384363351.30281;
	if (pJunkcode = 1773771251.69286)
		pJunkcode = 6566046137.79348;
	pJunkcode = 5899736647.07439;
	if (pJunkcode = 3738622559.51318)
		pJunkcode = 5969133675.92681;
	pJunkcode = 3014445947.24282;
	pJunkcode = 4251319997.6108;
	if (pJunkcode = 9689771640.15595)
		pJunkcode = 8104696155.0632;
	pJunkcode = 9545465253.34338;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3357() {
	float pJunkcode = 3132279023.52212;
	pJunkcode = 3178692074.61111;
	if (pJunkcode = 808172945.297959)
		pJunkcode = 3766022179.82724;
	pJunkcode = 9813930079.69182;
	pJunkcode = 5066553233.16065;
	if (pJunkcode = 4369509794.80858)
		pJunkcode = 1397299747.04264;
	pJunkcode = 8247727757.95542;
	if (pJunkcode = 1421472266.79014)
		pJunkcode = 3505901256.73931;
	pJunkcode = 7954823944.50915;
	pJunkcode = 8729298434.45616;
	if (pJunkcode = 2254165169.65057)
		pJunkcode = 7938872771.85356;
	pJunkcode = 5713811258.27123;
	if (pJunkcode = 670199928.707721)
		pJunkcode = 5278105594.58602;
	pJunkcode = 3464844861.08988;
	pJunkcode = 8974260406.51421;
	if (pJunkcode = 1866164125.56794)
		pJunkcode = 3050999967.13759;
	pJunkcode = 8060929791.49007;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3356() {
	float pJunkcode = 6891444232.27498;
	pJunkcode = 229298740.746863;
	if (pJunkcode = 3146711147.4152)
		pJunkcode = 5086571942.09608;
	pJunkcode = 4687429006.08463;
	pJunkcode = 2998800559.65515;
	if (pJunkcode = 3316505666.66899)
		pJunkcode = 8544848022.34371;
	pJunkcode = 6720430144.33582;
	if (pJunkcode = 3196181415.89048)
		pJunkcode = 392760921.207758;
	pJunkcode = 2379083146.37326;
	pJunkcode = 9101978134.41554;
	if (pJunkcode = 5723102842.62362)
		pJunkcode = 5224481492.01584;
	pJunkcode = 8655043871.2086;
	if (pJunkcode = 958791456.74909)
		pJunkcode = 9715530218.9834;
	pJunkcode = 1408111760.82945;
	pJunkcode = 5936756840.47868;
	if (pJunkcode = 8460633762.25154)
		pJunkcode = 5480240367.30304;
	pJunkcode = 9083612255.06433;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3355() {
	float pJunkcode = 2620814078.40964;
	pJunkcode = 871188243.158666;
	if (pJunkcode = 2594752128.94389)
		pJunkcode = 5317016614.67845;
	pJunkcode = 3479018294.09837;
	pJunkcode = 3883498816.91033;
	if (pJunkcode = 1199221025.18465)
		pJunkcode = 9476256724.62333;
	pJunkcode = 7208915782.22087;
	if (pJunkcode = 1186747942.01954)
		pJunkcode = 155618471.821888;
	pJunkcode = 7842871350.12635;
	pJunkcode = 5629569985.43791;
	if (pJunkcode = 8106236956.97522)
		pJunkcode = 199472681.518381;
	pJunkcode = 9513924166.77452;
	if (pJunkcode = 6306381611.7541)
		pJunkcode = 211070458.273447;
	pJunkcode = 6905936207.0148;
	pJunkcode = 1321405360.00056;
	if (pJunkcode = 7069927389.70609)
		pJunkcode = 5777322533.99409;
	pJunkcode = 8809210139.03873;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3354() {
	float pJunkcode = 6479945563.22591;
	pJunkcode = 865271260.767176;
	if (pJunkcode = 5206748658.8843)
		pJunkcode = 7415838082.51518;
	pJunkcode = 5963889930.57877;
	pJunkcode = 9205949427.69955;
	if (pJunkcode = 7563690700.68128)
		pJunkcode = 9796488621.88378;
	pJunkcode = 2753662562.40586;
	if (pJunkcode = 8093256762.09791)
		pJunkcode = 6085073064.05797;
	pJunkcode = 6584626475.30593;
	pJunkcode = 7813552272.2262;
	if (pJunkcode = 6276948939.24719)
		pJunkcode = 6661697986.4315;
	pJunkcode = 8849504061.5363;
	if (pJunkcode = 7174764364.27759)
		pJunkcode = 3907952013.52024;
	pJunkcode = 1120939896.45592;
	pJunkcode = 6222921736.35813;
	if (pJunkcode = 5281896828.84749)
		pJunkcode = 5323945971.8375;
	pJunkcode = 5140470369.97222;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3353() {
	float pJunkcode = 7181215107.91024;
	pJunkcode = 5770340685.69135;
	if (pJunkcode = 1747822985.30863)
		pJunkcode = 2332896469.5987;
	pJunkcode = 6058819960.21706;
	pJunkcode = 5034880160.8764;
	if (pJunkcode = 752687010.149742)
		pJunkcode = 2530181998.93688;
	pJunkcode = 5477172651.68801;
	if (pJunkcode = 1749902465.79583)
		pJunkcode = 5390516506.85098;
	pJunkcode = 5244178034.08307;
	pJunkcode = 6254194031.95008;
	if (pJunkcode = 9588182608.9418)
		pJunkcode = 6899388492.71709;
	pJunkcode = 6502770631.00757;
	if (pJunkcode = 3865797674.04079)
		pJunkcode = 3509463583.51429;
	pJunkcode = 2639672873.93066;
	pJunkcode = 5027768247.69187;
	if (pJunkcode = 7951334465.05748)
		pJunkcode = 2544589780.6296;
	pJunkcode = 7282687128.92624;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3352() {
	float pJunkcode = 7155257579.61563;
	pJunkcode = 5606653767.69909;
	if (pJunkcode = 9293038898.62736)
		pJunkcode = 9863024535.32677;
	pJunkcode = 5891856232.8129;
	pJunkcode = 3905530952.93991;
	if (pJunkcode = 7982655216.87842)
		pJunkcode = 9241602825.76155;
	pJunkcode = 9026297537.31744;
	if (pJunkcode = 2541219220.56826)
		pJunkcode = 3654497030.29197;
	pJunkcode = 40098665.9260515;
	pJunkcode = 2430888550.1391;
	if (pJunkcode = 6363756913.75693)
		pJunkcode = 4809131637.7424;
	pJunkcode = 4880874374.61917;
	if (pJunkcode = 4123926317.83046)
		pJunkcode = 8084357344.44673;
	pJunkcode = 2850538803.93194;
	pJunkcode = 5436445077.14002;
	if (pJunkcode = 5483569090.4444)
		pJunkcode = 7396944372.06;
	pJunkcode = 8123997117.16612;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3351() {
	float pJunkcode = 654326932.815558;
	pJunkcode = 6812731311.79406;
	if (pJunkcode = 8869349907.08901)
		pJunkcode = 319038144.817254;
	pJunkcode = 3208362481.12594;
	pJunkcode = 8055720984.35904;
	if (pJunkcode = 6780580219.69562)
		pJunkcode = 3525927066.93836;
	pJunkcode = 8001051540.86013;
	if (pJunkcode = 5717061555.52286)
		pJunkcode = 9199797779.97733;
	pJunkcode = 9579223405.63581;
	pJunkcode = 7356888237.12088;
	if (pJunkcode = 2181912312.91977)
		pJunkcode = 2800198174.32003;
	pJunkcode = 1129351929.93807;
	if (pJunkcode = 3906277139.75112)
		pJunkcode = 4423443829.11899;
	pJunkcode = 8627142843.47607;
	pJunkcode = 6166717339.59448;
	if (pJunkcode = 9215476997.51921)
		pJunkcode = 993901277.528159;
	pJunkcode = 1828282246.21005;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3350() {
	float pJunkcode = 2623645713.09779;
	pJunkcode = 4278913382.30545;
	if (pJunkcode = 6813861569.86595)
		pJunkcode = 2776331197.49902;
	pJunkcode = 5303672846.74535;
	pJunkcode = 5924416197.04417;
	if (pJunkcode = 2321567909.89636)
		pJunkcode = 4251365645.93031;
	pJunkcode = 46145627.0528546;
	if (pJunkcode = 9579365230.67581)
		pJunkcode = 3059237176.82261;
	pJunkcode = 1764453755.87726;
	pJunkcode = 1773583501.26766;
	if (pJunkcode = 1712922072.06377)
		pJunkcode = 8377667885.67703;
	pJunkcode = 6257732754.59044;
	if (pJunkcode = 459115999.226517)
		pJunkcode = 4888231241.81769;
	pJunkcode = 9360108729.30648;
	pJunkcode = 984481423.400298;
	if (pJunkcode = 8861772898.39008)
		pJunkcode = 7012042421.86445;
	pJunkcode = 1539707584.25184;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3349() {
	float pJunkcode = 4662290148.71237;
	pJunkcode = 2740971081.62441;
	if (pJunkcode = 1502295787.59088)
		pJunkcode = 3837255192.65813;
	pJunkcode = 6256970181.32239;
	pJunkcode = 6889933721.32678;
	if (pJunkcode = 5257486814.90522)
		pJunkcode = 8853804440.10115;
	pJunkcode = 1180713856.29858;
	if (pJunkcode = 6679348565.9758)
		pJunkcode = 9626300247.9235;
	pJunkcode = 124503589.125645;
	pJunkcode = 8908903130.53875;
	if (pJunkcode = 2043463593.15707)
		pJunkcode = 6985350634.55322;
	pJunkcode = 499858707.615;
	if (pJunkcode = 4640142492.58586)
		pJunkcode = 6808167508.94497;
	pJunkcode = 6481098512.74542;
	pJunkcode = 6480245362.27786;
	if (pJunkcode = 3339865692.42935)
		pJunkcode = 6163464397.4132;
	pJunkcode = 3480647957.26521;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3348() {
	float pJunkcode = 6170182251.00232;
	pJunkcode = 4701358064.82164;
	if (pJunkcode = 9847702213.32459)
		pJunkcode = 8985257833.12993;
	pJunkcode = 5948745908.50048;
	pJunkcode = 628350629.245598;
	if (pJunkcode = 9568279734.42154)
		pJunkcode = 2817217614.50857;
	pJunkcode = 2721629460.71278;
	if (pJunkcode = 1091164472.72945)
		pJunkcode = 3133578410.41055;
	pJunkcode = 2146822450.18389;
	pJunkcode = 824317703.056335;
	if (pJunkcode = 1691090503.65815)
		pJunkcode = 5333179505.10602;
	pJunkcode = 6243013367.12191;
	if (pJunkcode = 5053906245.87026)
		pJunkcode = 1045283241.09348;
	pJunkcode = 2876202391.13502;
	pJunkcode = 557700311.182907;
	if (pJunkcode = 6553311956.299)
		pJunkcode = 5790825458.70698;
	pJunkcode = 7417090277.43237;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3347() {
	float pJunkcode = 3482893621.29076;
	pJunkcode = 122668063.818956;
	if (pJunkcode = 882678036.469368)
		pJunkcode = 7320927189.03613;
	pJunkcode = 1187448240.69809;
	pJunkcode = 7604353700.29601;
	if (pJunkcode = 4966818879.71041)
		pJunkcode = 8263169919.01978;
	pJunkcode = 1345948868.38569;
	if (pJunkcode = 7883021238.0432)
		pJunkcode = 5046118166.66206;
	pJunkcode = 4997039394.34244;
	pJunkcode = 7400305123.06441;
	if (pJunkcode = 8759122821.97734)
		pJunkcode = 9184629010.57739;
	pJunkcode = 1896462097.5239;
	if (pJunkcode = 6830649962.87578)
		pJunkcode = 2785161281.20618;
	pJunkcode = 3202143005.8589;
	pJunkcode = 507941563.768378;
	if (pJunkcode = 2273942403.02704)
		pJunkcode = 715061654.044833;
	pJunkcode = 6243123357.54636;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3346() {
	float pJunkcode = 5354958796.57;
	pJunkcode = 3873076683.45067;
	if (pJunkcode = 9771342581.75798)
		pJunkcode = 2326631350.66872;
	pJunkcode = 8971983040.58525;
	pJunkcode = 5990750674.41359;
	if (pJunkcode = 2083032916.68482)
		pJunkcode = 3054342063.93738;
	pJunkcode = 2618246489.65735;
	if (pJunkcode = 7396059668.69915)
		pJunkcode = 5552374227.81016;
	pJunkcode = 7968446724.5822;
	pJunkcode = 1628729171.64124;
	if (pJunkcode = 2720759991.3086)
		pJunkcode = 4023894076.84875;
	pJunkcode = 5505123222.95776;
	if (pJunkcode = 2973239838.33006)
		pJunkcode = 835053023.196079;
	pJunkcode = 6360972647.98954;
	pJunkcode = 1023267347.16719;
	if (pJunkcode = 4604532172.41876)
		pJunkcode = 1814407257.19271;
	pJunkcode = 1219658764.5523;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3345() {
	float pJunkcode = 8986184880.97649;
	pJunkcode = 8738634321.38038;
	if (pJunkcode = 4808780507.03177)
		pJunkcode = 643561135.449338;
	pJunkcode = 1238189763.41645;
	pJunkcode = 5059216398.51038;
	if (pJunkcode = 5813845489.93504)
		pJunkcode = 3598103344.10797;
	pJunkcode = 7738664164.50638;
	if (pJunkcode = 4150534515.11992)
		pJunkcode = 1612244361.14349;
	pJunkcode = 6795284035.60642;
	pJunkcode = 9843558577.1099;
	if (pJunkcode = 6740965008.03521)
		pJunkcode = 1098895041.01548;
	pJunkcode = 5426803663.5873;
	if (pJunkcode = 4773356635.77958)
		pJunkcode = 8521343538.25511;
	pJunkcode = 6530496557.87728;
	pJunkcode = 841529943.716225;
	if (pJunkcode = 8707108652.70256)
		pJunkcode = 726063197.065168;
	pJunkcode = 8729566973.97849;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3344() {
	float pJunkcode = 2316478636.82241;
	pJunkcode = 4672368140.96744;
	if (pJunkcode = 3453982046.49295)
		pJunkcode = 2812511879.51846;
	pJunkcode = 510923307.527974;
	pJunkcode = 9312301495.28049;
	if (pJunkcode = 783872793.724567)
		pJunkcode = 4809321867.78915;
	pJunkcode = 3364017972.4114;
	if (pJunkcode = 3189932599.59914)
		pJunkcode = 6037581265.83358;
	pJunkcode = 2873250961.62208;
	pJunkcode = 3122668687.89293;
	if (pJunkcode = 5677704811.23185)
		pJunkcode = 4656653261.11249;
	pJunkcode = 8673698641.60518;
	if (pJunkcode = 9113786497.39697)
		pJunkcode = 2351043047.9597;
	pJunkcode = 4561292750.54544;
	pJunkcode = 8499969547.3376;
	if (pJunkcode = 2878043755.77586)
		pJunkcode = 8388832741.46259;
	pJunkcode = 7506947497.28688;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3343() {
	float pJunkcode = 6122807514.19831;
	pJunkcode = 7489784041.1439;
	if (pJunkcode = 73335410.847735)
		pJunkcode = 3537763010.76202;
	pJunkcode = 9666870647.06067;
	pJunkcode = 6408186444.54887;
	if (pJunkcode = 4540618399.40235)
		pJunkcode = 3524057234.37941;
	pJunkcode = 5050593692.6554;
	if (pJunkcode = 9980596329.34233)
		pJunkcode = 4538468406.85985;
	pJunkcode = 8844216344.23263;
	pJunkcode = 3075214817.4011;
	if (pJunkcode = 648360718.876792)
		pJunkcode = 4342591055.7687;
	pJunkcode = 6090509204.75609;
	if (pJunkcode = 6515583138.27561)
		pJunkcode = 8589006492.08267;
	pJunkcode = 6051392676.42285;
	pJunkcode = 5065292910.20697;
	if (pJunkcode = 3538295728.93233)
		pJunkcode = 3147930990.83798;
	pJunkcode = 4508136332.54054;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3342() {
	float pJunkcode = 8317103455.07759;
	pJunkcode = 4045378931.29688;
	if (pJunkcode = 3636659100.78718)
		pJunkcode = 1330334025.54471;
	pJunkcode = 8936383393.26811;
	pJunkcode = 8647146493.07606;
	if (pJunkcode = 9548378641.03788)
		pJunkcode = 7662875264.98913;
	pJunkcode = 1446974334.00603;
	if (pJunkcode = 6796854169.2296)
		pJunkcode = 9998592817.25814;
	pJunkcode = 213027892.48604;
	pJunkcode = 1398636325.92615;
	if (pJunkcode = 2328354426.88396)
		pJunkcode = 3181173053.55839;
	pJunkcode = 8525036754.12411;
	if (pJunkcode = 7460790107.9861)
		pJunkcode = 1108574747.84123;
	pJunkcode = 8796668022.27813;
	pJunkcode = 8015731782.90401;
	if (pJunkcode = 1872563898.4258)
		pJunkcode = 6876301136.85337;
	pJunkcode = 9904829858.17669;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3341() {
	float pJunkcode = 2858602391.21394;
	pJunkcode = 311067444.391651;
	if (pJunkcode = 3433149979.77733)
		pJunkcode = 9046280004.1973;
	pJunkcode = 9202135043.31869;
	pJunkcode = 3575558333.8654;
	if (pJunkcode = 8796325266.40201)
		pJunkcode = 6159791891.62285;
	pJunkcode = 2343501403.55358;
	if (pJunkcode = 3237994249.78185)
		pJunkcode = 4552033326.76826;
	pJunkcode = 2475164300.15705;
	pJunkcode = 8373018341.47254;
	if (pJunkcode = 1637167589.22808)
		pJunkcode = 6246100551.84067;
	pJunkcode = 8232890277.63708;
	if (pJunkcode = 4971954324.96757)
		pJunkcode = 1303113848.34155;
	pJunkcode = 2840470378.59234;
	pJunkcode = 8321041659.88482;
	if (pJunkcode = 7634137647.8671)
		pJunkcode = 8488533500.1152;
	pJunkcode = 81363580.2864091;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3340() {
	float pJunkcode = 2954511796.86265;
	pJunkcode = 9937672217.54333;
	if (pJunkcode = 9980369810.67435)
		pJunkcode = 9629507413.72971;
	pJunkcode = 2099858110.53816;
	pJunkcode = 6002276572.41115;
	if (pJunkcode = 5589103905.02743)
		pJunkcode = 9060075084.20854;
	pJunkcode = 903084765.433699;
	if (pJunkcode = 3455627241.63671)
		pJunkcode = 7889137779.91204;
	pJunkcode = 5283415442.22106;
	pJunkcode = 281072170.200157;
	if (pJunkcode = 6859845020.1172)
		pJunkcode = 4047405471.28175;
	pJunkcode = 8235966992.06649;
	if (pJunkcode = 9303351181.35958)
		pJunkcode = 8374992301.83896;
	pJunkcode = 731647451.496078;
	pJunkcode = 4599579003.7931;
	if (pJunkcode = 7320754521.2292)
		pJunkcode = 9629731641.36707;
	pJunkcode = 6354320640.57627;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3339() {
	float pJunkcode = 5351291426.97229;
	pJunkcode = 861129193.81533;
	if (pJunkcode = 2018115190.30101)
		pJunkcode = 4409825603.16779;
	pJunkcode = 6704823802.05663;
	pJunkcode = 2193105552.80524;
	if (pJunkcode = 2790829751.96703)
		pJunkcode = 830283875.086004;
	pJunkcode = 9448260076.62998;
	if (pJunkcode = 1606731033.20227)
		pJunkcode = 5124385777.37881;
	pJunkcode = 7812602702.8967;
	pJunkcode = 6399014391.92529;
	if (pJunkcode = 9382036888.867)
		pJunkcode = 3995158247.84819;
	pJunkcode = 7381217787.92564;
	if (pJunkcode = 9933833083.51255)
		pJunkcode = 777317300.632849;
	pJunkcode = 3785074062.19844;
	pJunkcode = 5423135686.30768;
	if (pJunkcode = 4877498480.46113)
		pJunkcode = 6493408520.63019;
	pJunkcode = 5557296765.54265;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3338() {
	float pJunkcode = 8444559353.56864;
	pJunkcode = 2892223391.33067;
	if (pJunkcode = 2997462921.4028)
		pJunkcode = 3722599792.15061;
	pJunkcode = 7136394115.8869;
	pJunkcode = 6408806198.60002;
	if (pJunkcode = 5330574398.39899)
		pJunkcode = 5576073987.33511;
	pJunkcode = 8506519187.12547;
	if (pJunkcode = 8068143101.40593)
		pJunkcode = 1624811506.94799;
	pJunkcode = 1972822273.90877;
	pJunkcode = 597700644.159696;
	if (pJunkcode = 9826794870.88635)
		pJunkcode = 2755500937.53113;
	pJunkcode = 3361997544.16981;
	if (pJunkcode = 2651132576.50989)
		pJunkcode = 5816317965.99224;
	pJunkcode = 5401636515.24643;
	pJunkcode = 1569336633.47303;
	if (pJunkcode = 9049226674.69948)
		pJunkcode = 6809400323.23319;
	pJunkcode = 7448695419.72734;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3337() {
	float pJunkcode = 4337479109.87341;
	pJunkcode = 3520700578.82506;
	if (pJunkcode = 4168469559.64332)
		pJunkcode = 8479815569.80753;
	pJunkcode = 6608552297.15536;
	pJunkcode = 4059041185.08251;
	if (pJunkcode = 436039125.293555)
		pJunkcode = 2549646839.57784;
	pJunkcode = 1028889303.4872;
	if (pJunkcode = 1371419629.50631)
		pJunkcode = 4847637455.07625;
	pJunkcode = 947983638.528521;
	pJunkcode = 981765881.566744;
	if (pJunkcode = 6649747519.0161)
		pJunkcode = 939369768.508101;
	pJunkcode = 3922504390.9099;
	if (pJunkcode = 8391589580.05443)
		pJunkcode = 4353735685.7585;
	pJunkcode = 2720850256.51817;
	pJunkcode = 1369895870.62697;
	if (pJunkcode = 5207651141.01531)
		pJunkcode = 6573014319.07411;
	pJunkcode = 7736835414.50367;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3336() {
	float pJunkcode = 1944932964.42535;
	pJunkcode = 2926525023.11815;
	if (pJunkcode = 7056766599.21772)
		pJunkcode = 1796482440.81721;
	pJunkcode = 415953175.540662;
	pJunkcode = 8704482787.02294;
	if (pJunkcode = 782162220.389862)
		pJunkcode = 9061535872.34172;
	pJunkcode = 5565339107.24088;
	if (pJunkcode = 7913127559.69969)
		pJunkcode = 4629553588.86049;
	pJunkcode = 6747904150.96835;
	pJunkcode = 5295875209.93481;
	if (pJunkcode = 3262527241.31199)
		pJunkcode = 3533489069.85237;
	pJunkcode = 9530509419.40751;
	if (pJunkcode = 8325483891.42906)
		pJunkcode = 4185233091.24563;
	pJunkcode = 3763993635.19058;
	pJunkcode = 2509721686.86407;
	if (pJunkcode = 1508956169.83866)
		pJunkcode = 4331401482.9292;
	pJunkcode = 9584088250.84005;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3335() {
	float pJunkcode = 1938143173.87163;
	pJunkcode = 2061550709.49462;
	if (pJunkcode = 7648762837.65582)
		pJunkcode = 2487435183.57172;
	pJunkcode = 6040019572.00723;
	pJunkcode = 4895641466.28123;
	if (pJunkcode = 5040847555.25472)
		pJunkcode = 822763669.511155;
	pJunkcode = 2543884990.69285;
	if (pJunkcode = 92652230.8872195)
		pJunkcode = 6755457010.71442;
	pJunkcode = 1675434554.9781;
	pJunkcode = 273380779.93475;
	if (pJunkcode = 9947976791.33003)
		pJunkcode = 4803684246.78008;
	pJunkcode = 8268263161.8165;
	if (pJunkcode = 3794901254.12052)
		pJunkcode = 7593535693.50837;
	pJunkcode = 3687718399.23171;
	pJunkcode = 1816295646.16839;
	if (pJunkcode = 9812195213.0507)
		pJunkcode = 15387723.7145169;
	pJunkcode = 5519447545.01256;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3334() {
	float pJunkcode = 8777342126.59091;
	pJunkcode = 8183693375.21511;
	if (pJunkcode = 5620794867.83943)
		pJunkcode = 5696074872.1796;
	pJunkcode = 4506757255.50463;
	pJunkcode = 8136820895.65291;
	if (pJunkcode = 1506906267.86063)
		pJunkcode = 8916601656.99202;
	pJunkcode = 1663602771.79643;
	if (pJunkcode = 3922376174.68494)
		pJunkcode = 9739111124.30994;
	pJunkcode = 1039523999.88562;
	pJunkcode = 8423892301.59323;
	if (pJunkcode = 4847419711.70748)
		pJunkcode = 6513980268.99129;
	pJunkcode = 3961083781.9914;
	if (pJunkcode = 5661840776.90155)
		pJunkcode = 152258393.691305;
	pJunkcode = 1512476627.09768;
	pJunkcode = 7755082504.8484;
	if (pJunkcode = 6325569163.18464)
		pJunkcode = 3281939253.20819;
	pJunkcode = 9197095569.89311;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3333() {
	float pJunkcode = 4861313538.86381;
	pJunkcode = 6879074194.26566;
	if (pJunkcode = 7294871071.91968)
		pJunkcode = 1865461337.9547;
	pJunkcode = 9474454803.42739;
	pJunkcode = 6886393357.2468;
	if (pJunkcode = 8826092040.55656)
		pJunkcode = 8243039897.19508;
	pJunkcode = 5875446798.15395;
	if (pJunkcode = 746453048.30492)
		pJunkcode = 794312263.580594;
	pJunkcode = 913645578.422824;
	pJunkcode = 4691488088.36833;
	if (pJunkcode = 9218651111.01778)
		pJunkcode = 6595143332.63741;
	pJunkcode = 1337704378.15925;
	if (pJunkcode = 4375812024.24374)
		pJunkcode = 1333151532.21068;
	pJunkcode = 3193176580.21857;
	pJunkcode = 3703034653.72799;
	if (pJunkcode = 3942157435.99126)
		pJunkcode = 3498475444.42006;
	pJunkcode = 4361767072.8845;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3332() {
	float pJunkcode = 5215238298.70052;
	pJunkcode = 1906711442.10592;
	if (pJunkcode = 1923427846.92893)
		pJunkcode = 9933131336.64188;
	pJunkcode = 2938936707.12823;
	pJunkcode = 9949801126.90788;
	if (pJunkcode = 4984175536.78945)
		pJunkcode = 1820912128.75211;
	pJunkcode = 5213613664.62528;
	if (pJunkcode = 3077342816.98093)
		pJunkcode = 9442639489.34046;
	pJunkcode = 2366521816.62007;
	pJunkcode = 1078305776.98022;
	if (pJunkcode = 172214523.032139)
		pJunkcode = 2682286218.13578;
	pJunkcode = 878901258.518001;
	if (pJunkcode = 984988192.854866)
		pJunkcode = 556007397.87592;
	pJunkcode = 7372680353.11286;
	pJunkcode = 4108292548.37572;
	if (pJunkcode = 9660029883.40388)
		pJunkcode = 836112270.960079;
	pJunkcode = 1540034240.04996;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3331() {
	float pJunkcode = 4193477777.04786;
	pJunkcode = 8945316629.2321;
	if (pJunkcode = 5046440179.86357)
		pJunkcode = 3760894515.69238;
	pJunkcode = 5167645667.18932;
	pJunkcode = 2583473997.52637;
	if (pJunkcode = 5353436115.68577)
		pJunkcode = 130751788.42195;
	pJunkcode = 7951799562.90017;
	if (pJunkcode = 5611122974.73195)
		pJunkcode = 4970301483.74194;
	pJunkcode = 6311323953.8631;
	pJunkcode = 9523538122.88401;
	if (pJunkcode = 8808371503.08835)
		pJunkcode = 8677875164.81941;
	pJunkcode = 8274615342.23843;
	if (pJunkcode = 371118712.070991)
		pJunkcode = 4363564325.87271;
	pJunkcode = 6576488871.90249;
	pJunkcode = 8070870585.00377;
	if (pJunkcode = 7456008461.65206)
		pJunkcode = 6040345575.88606;
	pJunkcode = 3825184886.18054;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3330() {
	float pJunkcode = 8689598440.693;
	pJunkcode = 4531680918.97817;
	if (pJunkcode = 4736809000.03721)
		pJunkcode = 3781081473.35331;
	pJunkcode = 5242156420.74536;
	pJunkcode = 1918679176.06918;
	if (pJunkcode = 6422541858.4969)
		pJunkcode = 90503637.6428957;
	pJunkcode = 5050646491.13246;
	if (pJunkcode = 2369055780.30707)
		pJunkcode = 1602113458.65376;
	pJunkcode = 3126658371.04068;
	pJunkcode = 543252443.439144;
	if (pJunkcode = 3136758288.7382)
		pJunkcode = 555343059.530673;
	pJunkcode = 695570797.167927;
	if (pJunkcode = 7245242740.9577)
		pJunkcode = 7282704518.87311;
	pJunkcode = 7456324062.76177;
	pJunkcode = 9224983219.31064;
	if (pJunkcode = 1020134560.69459)
		pJunkcode = 1481261875.02387;
	pJunkcode = 1155100560.0116;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3329() {
	float pJunkcode = 988036928.305631;
	pJunkcode = 5629234333.15369;
	if (pJunkcode = 1830513360.79418)
		pJunkcode = 9635840974.87127;
	pJunkcode = 8592772424.89111;
	pJunkcode = 5392191240.9231;
	if (pJunkcode = 4404966541.4189)
		pJunkcode = 9261936167.2409;
	pJunkcode = 5600352972.48636;
	if (pJunkcode = 7521155451.74968)
		pJunkcode = 1317726067.26482;
	pJunkcode = 6558194249.65719;
	pJunkcode = 7895390428.55745;
	if (pJunkcode = 7516217815.10094)
		pJunkcode = 3722473818.06232;
	pJunkcode = 9591629406.30671;
	if (pJunkcode = 700426736.689105)
		pJunkcode = 8443932549.4761;
	pJunkcode = 1339810282.33196;
	pJunkcode = 565450828.198991;
	if (pJunkcode = 4534218060.77168)
		pJunkcode = 6861402613.60732;
	pJunkcode = 774406117.822494;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3328() {
	float pJunkcode = 8842095317.86085;
	pJunkcode = 5220907623.94128;
	if (pJunkcode = 8489549919.1106)
		pJunkcode = 4496685456.48274;
	pJunkcode = 6052662661.75162;
	pJunkcode = 6513851519.58948;
	if (pJunkcode = 7864452819.01221)
		pJunkcode = 3360272146.65627;
	pJunkcode = 9114688590.70469;
	if (pJunkcode = 894865748.953237)
		pJunkcode = 85553053.4115382;
	pJunkcode = 9369793786.52691;
	pJunkcode = 73164826.8542296;
	if (pJunkcode = 5775399109.01352)
		pJunkcode = 3944917052.22491;
	pJunkcode = 1544831362.48212;
	if (pJunkcode = 8871776529.9378)
		pJunkcode = 788867168.305471;
	pJunkcode = 3167871068.99476;
	pJunkcode = 6862514005.60082;
	if (pJunkcode = 6361415110.26342)
		pJunkcode = 8817863352.38788;
	pJunkcode = 1921338897.31414;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3327() {
	float pJunkcode = 7855765326.3502;
	pJunkcode = 157480690.894929;
	if (pJunkcode = 236827835.70679)
		pJunkcode = 849833609.587289;
	pJunkcode = 8040039138.18547;
	pJunkcode = 914827244.986739;
	if (pJunkcode = 5604616994.90977)
		pJunkcode = 7470878905.80329;
	pJunkcode = 5079457563.25969;
	if (pJunkcode = 3591969149.19626)
		pJunkcode = 2292671964.86335;
	pJunkcode = 3269053756.59158;
	pJunkcode = 4155394580.35885;
	if (pJunkcode = 4923862956.57869)
		pJunkcode = 7321490451.95313;
	pJunkcode = 7233313307.2523;
	if (pJunkcode = 6415406578.66265)
		pJunkcode = 7962274937.21071;
	pJunkcode = 909099160.864037;
	pJunkcode = 720062133.384164;
	if (pJunkcode = 6489403661.73285)
		pJunkcode = 785058579.02138;
	pJunkcode = 6828732302.72429;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3326() {
	float pJunkcode = 6837286196.17068;
	pJunkcode = 9051323018.94156;
	if (pJunkcode = 947104741.905853)
		pJunkcode = 9858502196.87235;
	pJunkcode = 5407853417.76059;
	pJunkcode = 3948158813.99049;
	if (pJunkcode = 6257027710.87083)
		pJunkcode = 4769845873.4916;
	pJunkcode = 1158865792.00682;
	if (pJunkcode = 6562787502.95989)
		pJunkcode = 4689374158.60359;
	pJunkcode = 1280155747.11708;
	pJunkcode = 716289984.059705;
	if (pJunkcode = 3216395709.92391)
		pJunkcode = 8027830932.15329;
	pJunkcode = 6675862439.25187;
	if (pJunkcode = 5000254890.74577)
		pJunkcode = 7878409162.89949;
	pJunkcode = 5886942950.00594;
	pJunkcode = 6886267141.01706;
	if (pJunkcode = 25759275.2226813)
		pJunkcode = 3109432533.20717;
	pJunkcode = 3773357605.8662;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3325() {
	float pJunkcode = 6329547889.43187;
	pJunkcode = 7794500089.50648;
	if (pJunkcode = 8988708420.93981)
		pJunkcode = 8590780185.17366;
	pJunkcode = 7775657831.7807;
	pJunkcode = 4439302755.19606;
	if (pJunkcode = 8215947077.08039)
		pJunkcode = 3647015353.14674;
	pJunkcode = 6980492632.17317;
	if (pJunkcode = 9165384080.93683)
		pJunkcode = 9528064584.7279;
	pJunkcode = 4924760328.12818;
	pJunkcode = 8672693489.17196;
	if (pJunkcode = 9516660404.71702)
		pJunkcode = 6958094096.55205;
	pJunkcode = 9780982393.4519;
	if (pJunkcode = 4845362098.45565)
		pJunkcode = 3681213781.45517;
	pJunkcode = 119343680.597396;
	pJunkcode = 3888398996.13971;
	if (pJunkcode = 9347180895.18901)
		pJunkcode = 7538821395.63451;
	pJunkcode = 2943185542.16063;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3324() {
	float pJunkcode = 6740494347.92406;
	pJunkcode = 8670750436.76441;
	if (pJunkcode = 9085072698.6116)
		pJunkcode = 2916521184.35323;
	pJunkcode = 8681782324.29908;
	pJunkcode = 230504821.687923;
	if (pJunkcode = 6411897266.89507)
		pJunkcode = 2803785602.31568;
	pJunkcode = 432133050.793838;
	if (pJunkcode = 8221955605.23719)
		pJunkcode = 9506182068.83032;
	pJunkcode = 6225931345.0105;
	pJunkcode = 3062532149.17138;
	if (pJunkcode = 7475556771.24406)
		pJunkcode = 7187559106.74517;
	pJunkcode = 8030970032.9298;
	if (pJunkcode = 7728678020.69342)
		pJunkcode = 7673924901.02627;
	pJunkcode = 5671049714.18684;
	pJunkcode = 8409163074.80009;
	if (pJunkcode = 8566925066.29874)
		pJunkcode = 4823380698.04427;
	pJunkcode = 6516022690.19555;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3323() {
	float pJunkcode = 4222388264.53745;
	pJunkcode = 9008842170.34584;
	if (pJunkcode = 5548775717.1867)
		pJunkcode = 559078077.395404;
	pJunkcode = 9737728436.10686;
	pJunkcode = 3472473979.09407;
	if (pJunkcode = 5774075009.7277)
		pJunkcode = 1403065418.96857;
	pJunkcode = 7394028274.24391;
	if (pJunkcode = 8558278076.45047)
		pJunkcode = 8797732641.39137;
	pJunkcode = 2586522288.27505;
	pJunkcode = 8719088929.12718;
	if (pJunkcode = 904899929.904213)
		pJunkcode = 4498369843.61682;
	pJunkcode = 2086518100.45407;
	if (pJunkcode = 4343939485.61347)
		pJunkcode = 537990982.1846;
	pJunkcode = 2622332170.24779;
	pJunkcode = 3836511781.16043;
	if (pJunkcode = 8206592165.75453)
		pJunkcode = 747047256.433308;
	pJunkcode = 2374197521.24188;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3322() {
	float pJunkcode = 526205650.580124;
	pJunkcode = 6153098792.34021;
	if (pJunkcode = 6160013285.73961)
		pJunkcode = 1805319586.20717;
	pJunkcode = 327161326.834845;
	pJunkcode = 8260174785.30376;
	if (pJunkcode = 908630617.162279)
		pJunkcode = 4647864443.89594;
	pJunkcode = 7533282072.41189;
	if (pJunkcode = 3108020494.66666)
		pJunkcode = 6977362017.98875;
	pJunkcode = 8415429914.33132;
	pJunkcode = 3377269321.47301;
	if (pJunkcode = 9958184005.62839)
		pJunkcode = 1273133090.99552;
	pJunkcode = 3470887032.93823;
	if (pJunkcode = 4933565443.32681)
		pJunkcode = 6130286728.01996;
	pJunkcode = 620908057.944212;
	pJunkcode = 7062326835.07962;
	if (pJunkcode = 1289450032.27792)
		pJunkcode = 8808619888.99171;
	pJunkcode = 300573157.437063;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3321() {
	float pJunkcode = 3045087492.30229;
	pJunkcode = 1773226102.81879;
	if (pJunkcode = 8038024889.20738)
		pJunkcode = 1460268141.63922;
	pJunkcode = 7938182577.92389;
	pJunkcode = 608384402.140728;
	if (pJunkcode = 9529764315.24796)
		pJunkcode = 8738925976.09785;
	pJunkcode = 6495961362.037;
	if (pJunkcode = 7375771582.05946)
		pJunkcode = 5264795388.85458;
	pJunkcode = 5898936588.44757;
	pJunkcode = 2045316468.82982;
	if (pJunkcode = 2644783129.49238)
		pJunkcode = 5429682764.98408;
	pJunkcode = 9566349040.54096;
	if (pJunkcode = 363368339.408463)
		pJunkcode = 8914548376.67514;
	pJunkcode = 9749453411.62715;
	pJunkcode = 2211180711.08052;
	if (pJunkcode = 963514309.276946)
		pJunkcode = 1816708564.85734;
	pJunkcode = 9159407280.29457;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3320() {
	float pJunkcode = 676676404.525784;
	pJunkcode = 1574245180.55206;
	if (pJunkcode = 2769880288.55826)
		pJunkcode = 7902000107.78299;
	pJunkcode = 8814794961.57073;
	pJunkcode = 9058134581.27104;
	if (pJunkcode = 11556871.3869153)
		pJunkcode = 3301754148.89295;
	pJunkcode = 2500543594.67399;
	if (pJunkcode = 6443754115.22518)
		pJunkcode = 7295785314.2089;
	pJunkcode = 1583245198.66173;
	pJunkcode = 2293147076.65188;
	if (pJunkcode = 941338425.045492)
		pJunkcode = 2932759394.74081;
	pJunkcode = 4050000056.47721;
	if (pJunkcode = 185016189.813818)
		pJunkcode = 8225333047.70353;
	pJunkcode = 8134993968.6959;
	pJunkcode = 2578975004.76713;
	if (pJunkcode = 6562105581.42027)
		pJunkcode = 3829180015.18259;
	pJunkcode = 1918379784.46679;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3319() {
	float pJunkcode = 4165330111.95262;
	pJunkcode = 7233941187.01165;
	if (pJunkcode = 6570004405.41665)
		pJunkcode = 9043017134.21665;
	pJunkcode = 2364075696.72816;
	pJunkcode = 2501408651.98159;
	if (pJunkcode = 8280119233.64802)
		pJunkcode = 8460378987.53921;
	pJunkcode = 3991345218.52812;
	if (pJunkcode = 9028941475.1309)
		pJunkcode = 7577364935.81202;
	pJunkcode = 6866591525.55881;
	pJunkcode = 464960127.309859;
	if (pJunkcode = 3090003846.14702)
		pJunkcode = 781972679.514923;
	pJunkcode = 9569686846.3149;
	if (pJunkcode = 2913073399.79626)
		pJunkcode = 1088417371.01776;
	pJunkcode = 5673789570.28136;
	pJunkcode = 3031850392.33357;
	if (pJunkcode = 5117562324.50727)
		pJunkcode = 8928939555.38069;
	pJunkcode = 9177937077.76461;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3318() {
	float pJunkcode = 6891900580.42958;
	pJunkcode = 147910469.256182;
	if (pJunkcode = 4163696290.47206)
		pJunkcode = 64286673.4130848;
	pJunkcode = 4135188814.82661;
	pJunkcode = 944945310.503451;
	if (pJunkcode = 4646461492.64457)
		pJunkcode = 3960743526.69138;
	pJunkcode = 9594023205.0196;
	if (pJunkcode = 2433600767.18431)
		pJunkcode = 6827308194.36391;
	pJunkcode = 128245116.870073;
	pJunkcode = 2323393530.68651;
	if (pJunkcode = 7134058310.05079)
		pJunkcode = 8832884541.23186;
	pJunkcode = 2457317313.63079;
	if (pJunkcode = 1449055039.17017)
		pJunkcode = 5974445013.66306;
	pJunkcode = 1001859952.37127;
	pJunkcode = 382658547.171506;
	if (pJunkcode = 4659030033.09068)
		pJunkcode = 4369671789.76554;
	pJunkcode = 5296118403.76674;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3317() {
	float pJunkcode = 7913387493.88748;
	pJunkcode = 136346878.682655;
	if (pJunkcode = 4721721997.61333)
		pJunkcode = 6140022073.82822;
	pJunkcode = 4385396220.23797;
	pJunkcode = 1453416137.03137;
	if (pJunkcode = 1256481696.49471)
		pJunkcode = 6077194004.85728;
	pJunkcode = 2087028511.34349;
	if (pJunkcode = 352030482.871492)
		pJunkcode = 7999125347.55089;
	pJunkcode = 372319822.127519;
	pJunkcode = 8824905386.81013;
	if (pJunkcode = 561725970.961941)
		pJunkcode = 5147073921.1565;
	pJunkcode = 1471585330.00664;
	if (pJunkcode = 1885988904.81468)
		pJunkcode = 5503500712.27676;
	pJunkcode = 9885101590.92322;
	pJunkcode = 725335147.579715;
	if (pJunkcode = 1420782580.4262)
		pJunkcode = 8036308804.82645;
	pJunkcode = 6427783147.50621;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3316() {
	float pJunkcode = 7759771683.41701;
	pJunkcode = 6997870388.15414;
	if (pJunkcode = 1223930360.88081)
		pJunkcode = 6541048742.24904;
	pJunkcode = 7294190016.50139;
	pJunkcode = 7888382375.42782;
	if (pJunkcode = 1552285172.40629)
		pJunkcode = 9438706057.51369;
	pJunkcode = 7235332010.23607;
	if (pJunkcode = 2816268119.68413)
		pJunkcode = 1805696477.0426;
	pJunkcode = 8707689438.27857;
	pJunkcode = 9539743506.80742;
	if (pJunkcode = 6983118572.35785)
		pJunkcode = 5450975596.60744;
	pJunkcode = 209597069.471847;
	if (pJunkcode = 2903669180.08977)
		pJunkcode = 6357648313.66193;
	pJunkcode = 1174593442.49485;
	pJunkcode = 6548363600.23462;
	if (pJunkcode = 807782794.889971)
		pJunkcode = 5686719111.03038;
	pJunkcode = 7894212794.57995;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3315() {
	float pJunkcode = 9988177528.95095;
	pJunkcode = 4429656682.74874;
	if (pJunkcode = 9601098659.61824)
		pJunkcode = 9496376655.66884;
	pJunkcode = 5725534040.5488;
	pJunkcode = 7334747344.65693;
	if (pJunkcode = 4459192814.84585)
		pJunkcode = 4983753817.19019;
	pJunkcode = 5278145610.78858;
	if (pJunkcode = 2803046868.33884)
		pJunkcode = 2080884642.41528;
	pJunkcode = 5457108895.13013;
	pJunkcode = 5674767670.39262;
	if (pJunkcode = 5340687289.29025)
		pJunkcode = 7599219847.58453;
	pJunkcode = 9741255684.96784;
	if (pJunkcode = 3155186901.35069)
		pJunkcode = 4121012084.96917;
	pJunkcode = 3536213573.24778;
	pJunkcode = 8845828999.29105;
	if (pJunkcode = 5500864148.13605)
		pJunkcode = 4329323315.59369;
	pJunkcode = 5068769125.42952;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3314() {
	float pJunkcode = 8536241945.08701;
	pJunkcode = 5014554386.31731;
	if (pJunkcode = 127258295.01012)
		pJunkcode = 3311889219.00952;
	pJunkcode = 9636974734.68102;
	pJunkcode = 6713241439.71633;
	if (pJunkcode = 3850199690.29813)
		pJunkcode = 3519680353.08671;
	pJunkcode = 2272234060.18759;
	if (pJunkcode = 5459243949.40792)
		pJunkcode = 3023697133.55421;
	pJunkcode = 293920817.079303;
	pJunkcode = 6089920328.39612;
	if (pJunkcode = 1959999312.64937)
		pJunkcode = 3569615381.80362;
	pJunkcode = 4294590327.44298;
	if (pJunkcode = 728170870.733073)
		pJunkcode = 4012580802.32531;
	pJunkcode = 1121191709.59207;
	pJunkcode = 1223108903.00353;
	if (pJunkcode = 9315774516.09855)
		pJunkcode = 9984100192.92733;
	pJunkcode = 7140983988.52246;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3313() {
	float pJunkcode = 9389548610.17804;
	pJunkcode = 6935573964.01498;
	if (pJunkcode = 6672219195.32419)
		pJunkcode = 5475697835.64883;
	pJunkcode = 6087867755.87732;
	pJunkcode = 6635455234.13748;
	if (pJunkcode = 4053197445.82246)
		pJunkcode = 4965580607.92648;
	pJunkcode = 8808880167.76541;
	if (pJunkcode = 7933157849.51552)
		pJunkcode = 8052933093.45478;
	pJunkcode = 9430526193.44274;
	pJunkcode = 3235859174.54952;
	if (pJunkcode = 9165901052.92818)
		pJunkcode = 1740103413.50394;
	pJunkcode = 9231847656.95053;
	if (pJunkcode = 3235663072.57265)
		pJunkcode = 2103686103.73405;
	pJunkcode = 9591073870.9615;
	pJunkcode = 5617966872.25786;
	if (pJunkcode = 951241104.563974)
		pJunkcode = 8752993942.53607;
	pJunkcode = 1337177299.09074;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3312() {
	float pJunkcode = 8670326689.23052;
	pJunkcode = 1518567250.51846;
	if (pJunkcode = 4588720652.51497)
		pJunkcode = 7392084236.97258;
	pJunkcode = 1285912054.68817;
	pJunkcode = 9908997771.39982;
	if (pJunkcode = 7581973925.98055)
		pJunkcode = 5884128102.23934;
	pJunkcode = 5732915315.13961;
	if (pJunkcode = 7206032261.74266)
		pJunkcode = 2068587372.56764;
	pJunkcode = 5073640613.22559;
	pJunkcode = 2799021837.19626;
	if (pJunkcode = 6163155159.50135)
		pJunkcode = 5903689175.47958;
	pJunkcode = 3058838013.63054;
	if (pJunkcode = 3836449208.55069)
		pJunkcode = 1983554890.91177;
	pJunkcode = 4864684885.88486;
	pJunkcode = 3379327116.33093;
	if (pJunkcode = 5599050579.78438)
		pJunkcode = 9082138216.67173;
	pJunkcode = 2206839401.29783;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3311() {
	float pJunkcode = 9659964316.622;
	pJunkcode = 4145836586.86322;
	if (pJunkcode = 3260416687.55782)
		pJunkcode = 4671225880.59589;
	pJunkcode = 2184689768.45893;
	pJunkcode = 7675530114.89173;
	if (pJunkcode = 854971881.380196)
		pJunkcode = 4675510545.57774;
	pJunkcode = 4087167271.64949;
	if (pJunkcode = 4874953983.62115)
		pJunkcode = 6990819091.8215;
	pJunkcode = 494145391.79234;
	pJunkcode = 6391815661.71855;
	if (pJunkcode = 4741528781.55959)
		pJunkcode = 3781548177.71138;
	pJunkcode = 6241149494.96783;
	if (pJunkcode = 7126911491.24522)
		pJunkcode = 5179630821.36714;
	pJunkcode = 2431235862.46535;
	pJunkcode = 3775126281.50699;
	if (pJunkcode = 4214333990.38806)
		pJunkcode = 233033760.939877;
	pJunkcode = 27801181.8763573;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3310() {
	float pJunkcode = 1005656553.32541;
	pJunkcode = 4402184692.40942;
	if (pJunkcode = 4634050714.33664)
		pJunkcode = 4704398842.20059;
	pJunkcode = 51783287.6427337;
	pJunkcode = 6323213175.79016;
	if (pJunkcode = 5413653066.32257)
		pJunkcode = 1704467864.06058;
	pJunkcode = 5799654427.16651;
	if (pJunkcode = 4672820319.16691)
		pJunkcode = 8402128665.50517;
	pJunkcode = 9768566932.96655;
	pJunkcode = 2610250876.602;
	if (pJunkcode = 194185848.962315)
		pJunkcode = 2257072489.83917;
	pJunkcode = 7653022801.88169;
	if (pJunkcode = 1165693894.45981)
		pJunkcode = 6230742295.11631;
	pJunkcode = 6027982443.56458;
	pJunkcode = 6961722590.80596;
	if (pJunkcode = 3732963050.31419)
		pJunkcode = 6457874889.45359;
	pJunkcode = 9118820601.8016;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3309() {
	float pJunkcode = 4686642234.96977;
	pJunkcode = 5250334282.4928;
	if (pJunkcode = 9468791648.13698)
		pJunkcode = 634771314.742744;
	pJunkcode = 3867739592.57445;
	pJunkcode = 675728900.38344;
	if (pJunkcode = 7812282619.84192)
		pJunkcode = 3901149957.57087;
	pJunkcode = 1832517076.61929;
	if (pJunkcode = 4566209430.31752)
		pJunkcode = 4559482480.12612;
	pJunkcode = 9843389250.3893;
	pJunkcode = 1429960837.92438;
	if (pJunkcode = 1469528240.61852)
		pJunkcode = 1963113098.00315;
	pJunkcode = 8621746148.55114;
	if (pJunkcode = 6189262810.83712)
		pJunkcode = 444873174.066928;
	pJunkcode = 5026565533.44825;
	pJunkcode = 1684865559.25398;
	if (pJunkcode = 1450395781.02396)
		pJunkcode = 231873630.995129;
	pJunkcode = 5396820908.57432;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3308() {
	float pJunkcode = 8883351096.03811;
	pJunkcode = 8137123602.73051;
	if (pJunkcode = 1980038384.74925)
		pJunkcode = 4948506811.63945;
	pJunkcode = 5488551807.31155;
	pJunkcode = 811568194.473855;
	if (pJunkcode = 7986233017.42322)
		pJunkcode = 3591823999.36792;
	pJunkcode = 3754953801.11071;
	if (pJunkcode = 5436721013.61793)
		pJunkcode = 716601408.229433;
	pJunkcode = 1591644959.76337;
	pJunkcode = 2030088309.58321;
	if (pJunkcode = 6655711725.97478)
		pJunkcode = 789965009.69102;
	pJunkcode = 9413937678.88505;
	if (pJunkcode = 5038164648.96613)
		pJunkcode = 7934795128.49656;
	pJunkcode = 5531932048.40654;
	pJunkcode = 9605412996.24059;
	if (pJunkcode = 125961576.334187)
		pJunkcode = 1126585028.50644;
	pJunkcode = 7026179282.20117;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3307() {
	float pJunkcode = 943991384.95617;
	pJunkcode = 5264000636.38507;
	if (pJunkcode = 6349128336.31623)
		pJunkcode = 1210190393.66009;
	pJunkcode = 3645111178.41128;
	pJunkcode = 6076930859.96702;
	if (pJunkcode = 7421036635.8872)
		pJunkcode = 2026214591.23448;
	pJunkcode = 4932105560.89578;
	if (pJunkcode = 3681097527.94905)
		pJunkcode = 4923342743.61719;
	pJunkcode = 5164520643.21276;
	pJunkcode = 4228002600.38669;
	if (pJunkcode = 1360848388.13694)
		pJunkcode = 6857967459.98646;
	pJunkcode = 8612174394.90208;
	if (pJunkcode = 940566103.255881)
		pJunkcode = 6092008686.62699;
	pJunkcode = 5760617737.99089;
	pJunkcode = 3677758227.41733;
	if (pJunkcode = 8387797363.25747)
		pJunkcode = 8157665968.15822;
	pJunkcode = 903419113.879354;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3306() {
	float pJunkcode = 1214852687.54313;
	pJunkcode = 2312378839.85121;
	if (pJunkcode = 7259841311.48512)
		pJunkcode = 9449339794.65946;
	pJunkcode = 11040476.8097915;
	pJunkcode = 6797141092.87571;
	if (pJunkcode = 905855834.492174)
		pJunkcode = 4058203762.16766;
	pJunkcode = 2150541649.69287;
	if (pJunkcode = 261641247.988182)
		pJunkcode = 7851507923.23579;
	pJunkcode = 5905051647.20004;
	pJunkcode = 3003896608.03307;
	if (pJunkcode = 8793469180.08419)
		pJunkcode = 4564523748.65801;
	pJunkcode = 8991495599.38942;
	if (pJunkcode = 505404221.742524)
		pJunkcode = 3439611892.18427;
	pJunkcode = 2122713182.90481;
	pJunkcode = 5735799303.60273;
	if (pJunkcode = 757548979.924287)
		pJunkcode = 3513739915.57073;
	pJunkcode = 9447956978.4008;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3305() {
	float pJunkcode = 4487984357.33399;
	pJunkcode = 5120247598.65983;
	if (pJunkcode = 9726241725.44305)
		pJunkcode = 2871266693.94614;
	pJunkcode = 8508549498.92904;
	pJunkcode = 147153416.34749;
	if (pJunkcode = 1667753879.47176)
		pJunkcode = 8620209395.52352;
	pJunkcode = 5452156909.27232;
	if (pJunkcode = 5999339974.06189)
		pJunkcode = 4728819505.70999;
	pJunkcode = 7209071854.02548;
	pJunkcode = 4242472288.80017;
	if (pJunkcode = 362281028.847616)
		pJunkcode = 1586044237.38572;
	pJunkcode = 9123186682.18875;
	if (pJunkcode = 9679981910.3098)
		pJunkcode = 4033108953.58436;
	pJunkcode = 9678062077.87013;
	pJunkcode = 1548272825.75033;
	if (pJunkcode = 9299221070.14172)
		pJunkcode = 182599343.425994;
	pJunkcode = 1542999357.53956;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3304() {
	float pJunkcode = 7567490374.96918;
	pJunkcode = 8771136539.53288;
	if (pJunkcode = 4319015100.37215)
		pJunkcode = 9084856743.36429;
	pJunkcode = 4664645463.23302;
	pJunkcode = 4097632997.55464;
	if (pJunkcode = 3334324203.9034)
		pJunkcode = 9651714159.35436;
	pJunkcode = 7134766559.8163;
	if (pJunkcode = 763523845.76325)
		pJunkcode = 1391172718.58648;
	pJunkcode = 9084240244.25299;
	pJunkcode = 9096989396.60815;
	if (pJunkcode = 8974575735.21478)
		pJunkcode = 9535306441.72349;
	pJunkcode = 7239697077.82005;
	if (pJunkcode = 7926042183.13855)
		pJunkcode = 6283079183.06239;
	pJunkcode = 1593442645.81226;
	pJunkcode = 5017859691.58671;
	if (pJunkcode = 3545072450.8327)
		pJunkcode = 9297743699.97593;
	pJunkcode = 3663521066.97065;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3303() {
	float pJunkcode = 8350302491.12532;
	pJunkcode = 784455185.815952;
	if (pJunkcode = 2122273263.11614)
		pJunkcode = 2491892232.30995;
	pJunkcode = 7739199034.01033;
	pJunkcode = 8692028488.44631;
	if (pJunkcode = 3498879345.66836)
		pJunkcode = 9086928608.0373;
	pJunkcode = 7417583978.2336;
	if (pJunkcode = 2741708302.2172)
		pJunkcode = 9427923750.29163;
	pJunkcode = 2597825090.18087;
	pJunkcode = 3441406132.37409;
	if (pJunkcode = 4931655880.94413)
		pJunkcode = 9686570502.27482;
	pJunkcode = 2905771272.86296;
	if (pJunkcode = 6961493287.51133)
		pJunkcode = 3391585091.74379;
	pJunkcode = 3874967964.15403;
	pJunkcode = 9768328828.76159;
	if (pJunkcode = 2319250213.44661)
		pJunkcode = 9955987922.39258;
	pJunkcode = 6782370096.68281;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3302() {
	float pJunkcode = 5755717763.65043;
	pJunkcode = 118519580.11874;
	if (pJunkcode = 6035895297.79524)
		pJunkcode = 9390768434.97455;
	pJunkcode = 816088139.453189;
	pJunkcode = 7682685473.8399;
	if (pJunkcode = 9028226056.58155)
		pJunkcode = 2291468885.07927;
	pJunkcode = 8873444132.47863;
	if (pJunkcode = 2986163274.32065)
		pJunkcode = 1123705708.25103;
	pJunkcode = 8043294697.35703;
	pJunkcode = 7353392122.06221;
	if (pJunkcode = 8568701674.40024)
		pJunkcode = 1876324562.32946;
	pJunkcode = 769965369.394618;
	if (pJunkcode = 2826627938.97349)
		pJunkcode = 2377235018.06986;
	pJunkcode = 1866195589.98647;
	pJunkcode = 1389299856.93114;
	if (pJunkcode = 7177794543.31162)
		pJunkcode = 8215683862.34164;
	pJunkcode = 7823876357.02572;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3301() {
	float pJunkcode = 2915552942.19284;
	pJunkcode = 5851809515.77914;
	if (pJunkcode = 4827704096.34466)
		pJunkcode = 263620130.345183;
	pJunkcode = 6931157024.61063;
	pJunkcode = 9507403195.13711;
	if (pJunkcode = 5572216690.36745)
		pJunkcode = 376188909.521063;
	pJunkcode = 7536196978.14701;
	if (pJunkcode = 8875350658.78007)
		pJunkcode = 1473491290.53528;
	pJunkcode = 4404513478.47183;
	pJunkcode = 6897249694.55999;
	if (pJunkcode = 8365637721.1293)
		pJunkcode = 7517889224.38473;
	pJunkcode = 4437354904.19692;
	if (pJunkcode = 4997699236.77291)
		pJunkcode = 8328816239.70729;
	pJunkcode = 1017127990.45173;
	pJunkcode = 3978392017.7657;
	if (pJunkcode = 1186465553.86902)
		pJunkcode = 9000280675.05495;
	pJunkcode = 2217730641.90378;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3300() {
	float pJunkcode = 2548073774.64616;
	pJunkcode = 3525636541.43821;
	if (pJunkcode = 7783307955.5555)
		pJunkcode = 9715718151.01468;
	pJunkcode = 3234282295.89004;
	pJunkcode = 1211901269.67161;
	if (pJunkcode = 8247144651.08111)
		pJunkcode = 9918016217.87065;
	pJunkcode = 4700260414.35531;
	if (pJunkcode = 3387211549.20914)
		pJunkcode = 3822790025.77054;
	pJunkcode = 3263849580.41574;
	pJunkcode = 7446257821.35177;
	if (pJunkcode = 68519324.5492908)
		pJunkcode = 764553743.189822;
	pJunkcode = 8486436080.2063;
	if (pJunkcode = 2732873298.84774)
		pJunkcode = 3074924748.24445;
	pJunkcode = 5339023574.98229;
	pJunkcode = 6050107580.53642;
	if (pJunkcode = 6467626912.75407)
		pJunkcode = 4315333442.10072;
	pJunkcode = 6850606220.27218;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3299() {
	float pJunkcode = 9520215916.6072;
	pJunkcode = 3364356986.73649;
	if (pJunkcode = 419504193.844359)
		pJunkcode = 3074179485.76165;
	pJunkcode = 185913633.540085;
	pJunkcode = 9191852101.61077;
	if (pJunkcode = 6682166116.87915)
		pJunkcode = 2533440247.27548;
	pJunkcode = 2440544154.57764;
	if (pJunkcode = 815353538.178943)
		pJunkcode = 604934340.522839;
	pJunkcode = 9414234164.79007;
	pJunkcode = 1144940584.25014;
	if (pJunkcode = 7868369054.13961)
		pJunkcode = 1379235580.59213;
	pJunkcode = 2654093151.51905;
	if (pJunkcode = 6169226325.45981)
		pJunkcode = 3221273279.19558;
	pJunkcode = 8901095444.85812;
	pJunkcode = 4451050114.83059;
	if (pJunkcode = 9952443167.17057)
		pJunkcode = 132809133.921215;
	pJunkcode = 1915260443.8294;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3298() {
	float pJunkcode = 5529438827.34644;
	pJunkcode = 688613440.075676;
	if (pJunkcode = 5795819281.48478)
		pJunkcode = 5114618722.68229;
	pJunkcode = 1712566859.87278;
	pJunkcode = 6791943191.42296;
	if (pJunkcode = 7773005731.00651)
		pJunkcode = 9713143761.34729;
	pJunkcode = 1579179948.75596;
	if (pJunkcode = 9320877202.41175)
		pJunkcode = 2177668558.2012;
	pJunkcode = 721574353.292195;
	pJunkcode = 9560699605.82317;
	if (pJunkcode = 9855306455.95157)
		pJunkcode = 4178130887.00185;
	pJunkcode = 6049705940.05107;
	if (pJunkcode = 3384216997.56391)
		pJunkcode = 7417492087.88239;
	pJunkcode = 5590085262.40683;
	pJunkcode = 4850067004.57696;
	if (pJunkcode = 3248180313.95605)
		pJunkcode = 3888100277.41115;
	pJunkcode = 5571500242.71695;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3297() {
	float pJunkcode = 9621226364.8012;
	pJunkcode = 6359786676.43008;
	if (pJunkcode = 7354746160.7411)
		pJunkcode = 4922607832.68129;
	pJunkcode = 505036776.751845;
	pJunkcode = 6954600181.72097;
	if (pJunkcode = 5271436879.41284)
		pJunkcode = 7482569709.11174;
	pJunkcode = 3750344023.98247;
	if (pJunkcode = 8984400026.09606)
		pJunkcode = 4215647945.34417;
	pJunkcode = 7097871688.58103;
	pJunkcode = 7467876132.99025;
	if (pJunkcode = 9506816269.87843)
		pJunkcode = 4067334217.68442;
	pJunkcode = 6892896287.01805;
	if (pJunkcode = 2611103077.13004)
		pJunkcode = 2616884800.24002;
	pJunkcode = 7164297.46576256;
	pJunkcode = 4154329029.93235;
	if (pJunkcode = 3369881918.84094)
		pJunkcode = 7590635167.25;
	pJunkcode = 5541634528.16981;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3296() {
	float pJunkcode = 6636897428.6014;
	pJunkcode = 5271076905.8828;
	if (pJunkcode = 98547320.4322198)
		pJunkcode = 7670366888.55999;
	pJunkcode = 4075516288.9678;
	pJunkcode = 1091508222.78248;
	if (pJunkcode = 5221680711.73605)
		pJunkcode = 36217201.3792646;
	pJunkcode = 81414804.8440635;
	if (pJunkcode = 4392195687.13647)
		pJunkcode = 9908819457.50625;
	pJunkcode = 8294640572.69497;
	pJunkcode = 946798500.494424;
	if (pJunkcode = 7325689061.26135)
		pJunkcode = 4038817750.20942;
	pJunkcode = 8680716920.55442;
	if (pJunkcode = 288115345.481154)
		pJunkcode = 2871307159.50735;
	pJunkcode = 4281173255.84308;
	pJunkcode = 2136462575.01574;
	if (pJunkcode = 5346760026.83984)
		pJunkcode = 9621480527.17506;
	pJunkcode = 8204598161.19845;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3295() {
	float pJunkcode = 9236895956.89755;
	pJunkcode = 8468120119.2459;
	if (pJunkcode = 5660206478.12294)
		pJunkcode = 7014487140.12991;
	pJunkcode = 3295330057.11755;
	pJunkcode = 4818455084.64973;
	if (pJunkcode = 7763260932.36865)
		pJunkcode = 9501112264.8601;
	pJunkcode = 38167830.0202009;
	if (pJunkcode = 5384487869.31259)
		pJunkcode = 5103130277.44753;
	pJunkcode = 2907409185.13702;
	pJunkcode = 5893328150.13955;
	if (pJunkcode = 1727361851.35186)
		pJunkcode = 6724399236.14736;
	pJunkcode = 7080127763.50854;
	if (pJunkcode = 2399467784.58703)
		pJunkcode = 8878900450.07855;
	pJunkcode = 7720877396.6956;
	pJunkcode = 4257269294.24418;
	if (pJunkcode = 6522821921.93744)
		pJunkcode = 129276003.816984;
	pJunkcode = 1764037321.48076;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3294() {
	float pJunkcode = 8913134759.55865;
	pJunkcode = 1664567849.82002;
	if (pJunkcode = 3406838801.56973)
		pJunkcode = 4102780785.04079;
	pJunkcode = 6612964067.77112;
	pJunkcode = 9708133191.22877;
	if (pJunkcode = 1334999944.56462)
		pJunkcode = 5145890615.48125;
	pJunkcode = 4253170933.82368;
	if (pJunkcode = 5855489451.12103)
		pJunkcode = 8191335657.33224;
	pJunkcode = 489860575.472086;
	pJunkcode = 6102259733.18399;
	if (pJunkcode = 1217277985.5988)
		pJunkcode = 39179627.3735387;
	pJunkcode = 4491761777.8915;
	if (pJunkcode = 8369734891.7971)
		pJunkcode = 3596351199.27756;
	pJunkcode = 1534415962.41733;
	pJunkcode = 677674597.720658;
	if (pJunkcode = 1389921559.75529)
		pJunkcode = 8470027146.12823;
	pJunkcode = 9803385704.38176;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3293() {
	float pJunkcode = 157441526.844008;
	pJunkcode = 5726749527.7538;
	if (pJunkcode = 2165327786.71112)
		pJunkcode = 5131324165.94821;
	pJunkcode = 391288491.658816;
	pJunkcode = 9109628602.77625;
	if (pJunkcode = 2979663224.63566)
		pJunkcode = 540847413.088323;
	pJunkcode = 3832069274.87877;
	if (pJunkcode = 8079580506.0174)
		pJunkcode = 1015613495.81106;
	pJunkcode = 1274382666.59416;
	pJunkcode = 3747463222.93899;
	if (pJunkcode = 9159785786.95851)
		pJunkcode = 4223540143.0467;
	pJunkcode = 717977449.184114;
	if (pJunkcode = 3233983910.10597)
		pJunkcode = 6327316339.45879;
	pJunkcode = 1108193973.42949;
	pJunkcode = 4044594786.48542;
	if (pJunkcode = 2852808460.91538)
		pJunkcode = 2696416752.52717;
	pJunkcode = 6999738544.34922;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3292() {
	float pJunkcode = 6415572366.39286;
	pJunkcode = 8677974028.37867;
	if (pJunkcode = 3139749317.69101)
		pJunkcode = 8337059328.30727;
	pJunkcode = 1288466525.89318;
	pJunkcode = 4062594990.34115;
	if (pJunkcode = 8464121218.42515)
		pJunkcode = 7230762123.5831;
	pJunkcode = 2477405472.9244;
	if (pJunkcode = 8522035015.06879)
		pJunkcode = 3710362980.83161;
	pJunkcode = 4570973544.67942;
	pJunkcode = 2148227436.35473;
	if (pJunkcode = 2782958876.68162)
		pJunkcode = 7794037976.52279;
	pJunkcode = 7974774674.56136;
	if (pJunkcode = 7625236369.91768)
		pJunkcode = 9837959405.36883;
	pJunkcode = 7230994518.83776;
	pJunkcode = 6822103866.84966;
	if (pJunkcode = 5459241181.07925)
		pJunkcode = 6540055014.70125;
	pJunkcode = 4944439987.81426;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3291() {
	float pJunkcode = 4653656187.93662;
	pJunkcode = 2391075082.8399;
	if (pJunkcode = 5132365690.82085)
		pJunkcode = 8832545901.76036;
	pJunkcode = 8258212657.27233;
	pJunkcode = 5439716415.0865;
	if (pJunkcode = 4587730552.43234)
		pJunkcode = 2388339261.66008;
	pJunkcode = 9778163273.28571;
	if (pJunkcode = 86595258.1428574)
		pJunkcode = 5193328160.05161;
	pJunkcode = 1273410312.68572;
	pJunkcode = 8789651126.32908;
	if (pJunkcode = 8759288467.68227)
		pJunkcode = 909779345.504425;
	pJunkcode = 4450330989.65446;
	if (pJunkcode = 2357657597.9536)
		pJunkcode = 7006435501.59705;
	pJunkcode = 1836751082.81629;
	pJunkcode = 220163498.279937;
	if (pJunkcode = 5786569582.94632)
		pJunkcode = 9155703172.71698;
	pJunkcode = 1510274355.28526;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3290() {
	float pJunkcode = 8538537301.65989;
	pJunkcode = 1443472160.7546;
	if (pJunkcode = 6297817401.68506)
		pJunkcode = 6802371662.46938;
	pJunkcode = 122974617.220328;
	pJunkcode = 6865343637.96545;
	if (pJunkcode = 4121876965.32726)
		pJunkcode = 6782318832.54434;
	pJunkcode = 6272505119.81384;
	if (pJunkcode = 1799332126.68463)
		pJunkcode = 1343409987.36467;
	pJunkcode = 9330385137.81958;
	pJunkcode = 8689452848.49087;
	if (pJunkcode = 4840496624.61496)
		pJunkcode = 8168189489.03608;
	pJunkcode = 7920853175.72745;
	if (pJunkcode = 8247220273.71792)
		pJunkcode = 356555885.346537;
	pJunkcode = 2708350216.49071;
	pJunkcode = 1329581855.04579;
	if (pJunkcode = 3902756093.43646)
		pJunkcode = 9670955743.25203;
	pJunkcode = 1866100724.08939;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3289() {
	float pJunkcode = 4677333197.80687;
	pJunkcode = 5470943646.75802;
	if (pJunkcode = 92441809.9689621)
		pJunkcode = 3906482347.6434;
	pJunkcode = 2485764663.6921;
	pJunkcode = 1662223173.22509;
	if (pJunkcode = 4681396086.83353)
		pJunkcode = 362569486.83232;
	pJunkcode = 7887621753.26105;
	if (pJunkcode = 1961560058.63488)
		pJunkcode = 8042580845.74224;
	pJunkcode = 1331266756.73045;
	pJunkcode = 8853550226.50506;
	if (pJunkcode = 9400139052.89857)
		pJunkcode = 6926316984.9351;
	pJunkcode = 5745859915.22406;
	if (pJunkcode = 9652259711.02142)
		pJunkcode = 2846744403.20538;
	pJunkcode = 4598691813.82584;
	pJunkcode = 2468932100.82258;
	if (pJunkcode = 6742371372.54968)
		pJunkcode = 6035633547.54658;
	pJunkcode = 6260250536.70953;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3288() {
	float pJunkcode = 1266056476.91187;
	pJunkcode = 7307614233.60228;
	if (pJunkcode = 9987847126.94524)
		pJunkcode = 9167987923.38611;
	pJunkcode = 2785832888.45143;
	pJunkcode = 3767187657.83839;
	if (pJunkcode = 4378288935.68035)
		pJunkcode = 8360307617.43083;
	pJunkcode = 5108804617.8102;
	if (pJunkcode = 5827735035.37865)
		pJunkcode = 9784981331.52518;
	pJunkcode = 4323541438.81702;
	pJunkcode = 1125107371.96182;
	if (pJunkcode = 9445918368.59607)
		pJunkcode = 2445818470.28347;
	pJunkcode = 5744364638.23254;
	if (pJunkcode = 6309300454.61603)
		pJunkcode = 2662732676.89154;
	pJunkcode = 1891223139.12603;
	pJunkcode = 4241642453.76011;
	if (pJunkcode = 8448085401.03551)
		pJunkcode = 8803867772.2729;
	pJunkcode = 1754965161.51758;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3287() {
	float pJunkcode = 5138132582.76715;
	pJunkcode = 6248686075.3581;
	if (pJunkcode = 1260031487.79319)
		pJunkcode = 7137782415.25238;
	pJunkcode = 8236231145.10336;
	pJunkcode = 717710660.530717;
	if (pJunkcode = 8804661490.98469)
		pJunkcode = 4300136409.67159;
	pJunkcode = 218489341.622033;
	if (pJunkcode = 5674925802.72783)
		pJunkcode = 291462909.089788;
	pJunkcode = 3588862642.05562;
	pJunkcode = 7425355470.75261;
	if (pJunkcode = 8116146144.04511)
		pJunkcode = 9368225312.96962;
	pJunkcode = 7623625714.8545;
	if (pJunkcode = 2957196679.86364)
		pJunkcode = 7015121716.0316;
	pJunkcode = 6683655237.89001;
	pJunkcode = 2183065190.26249;
	if (pJunkcode = 4717683334.37289)
		pJunkcode = 5352866396.43958;
	pJunkcode = 3555762327.54186;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3286() {
	float pJunkcode = 9122487610.51542;
	pJunkcode = 8930778156.34943;
	if (pJunkcode = 1204426249.08791)
		pJunkcode = 9292424304.05168;
	pJunkcode = 9867413207.04535;
	pJunkcode = 7009328679.79378;
	if (pJunkcode = 7609513850.96571)
		pJunkcode = 1083511950.82301;
	pJunkcode = 5976455812.12552;
	if (pJunkcode = 8001358694.95856)
		pJunkcode = 1907926060.8613;
	pJunkcode = 1234169414.45129;
	pJunkcode = 3639867721.77036;
	if (pJunkcode = 9869061089.02788)
		pJunkcode = 5403282981.6328;
	pJunkcode = 5265907449.06534;
	if (pJunkcode = 9670581783.72509)
		pJunkcode = 9752646202.61317;
	pJunkcode = 9764528872.75144;
	pJunkcode = 6481422626.05833;
	if (pJunkcode = 4252778066.30926)
		pJunkcode = 1378753346.58543;
	pJunkcode = 913387078.457452;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3285() {
	float pJunkcode = 7097785309.27604;
	pJunkcode = 5133568952.21101;
	if (pJunkcode = 5322417025.5433)
		pJunkcode = 3308536386.49611;
	pJunkcode = 9996216638.99216;
	pJunkcode = 9676950785.9928;
	if (pJunkcode = 1212570243.62861)
		pJunkcode = 6243021127.28966;
	pJunkcode = 6647556306.54865;
	if (pJunkcode = 2282955699.33553)
		pJunkcode = 9836965035.21489;
	pJunkcode = 1212717356.19965;
	pJunkcode = 4834274858.3173;
	if (pJunkcode = 9947068649.14267)
		pJunkcode = 1254100137.56636;
	pJunkcode = 2663301760.51642;
	if (pJunkcode = 8838302402.57894)
		pJunkcode = 9953646649.74763;
	pJunkcode = 3687863686.42875;
	pJunkcode = 6361732208.45919;
	if (pJunkcode = 7118121477.29113)
		pJunkcode = 2790054044.85371;
	pJunkcode = 9677918252.8349;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3284() {
	float pJunkcode = 1443888974.66126;
	pJunkcode = 74591954.9978584;
	if (pJunkcode = 9693308465.24554)
		pJunkcode = 2469393471.08394;
	pJunkcode = 759941065.020777;
	pJunkcode = 3251159633.97288;
	if (pJunkcode = 6267524016.44691)
		pJunkcode = 6882456921.93476;
	pJunkcode = 4350047552.53163;
	if (pJunkcode = 916671482.728988)
		pJunkcode = 9486236100.11716;
	pJunkcode = 4573906508.57867;
	pJunkcode = 2509521896.88669;
	if (pJunkcode = 7611823058.49508)
		pJunkcode = 7559852190.63039;
	pJunkcode = 5752620942.1677;
	if (pJunkcode = 9516549243.16615)
		pJunkcode = 837950892.883363;
	pJunkcode = 7664886338.189;
	pJunkcode = 5205104090.04871;
	if (pJunkcode = 4762581001.41704)
		pJunkcode = 7452204556.87368;
	pJunkcode = 2364688130.65707;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3283() {
	float pJunkcode = 5165186085.16422;
	pJunkcode = 1373227541.3748;
	if (pJunkcode = 9426093915.30897)
		pJunkcode = 6813966140.04721;
	pJunkcode = 3030163045.9516;
	pJunkcode = 7473554929.68688;
	if (pJunkcode = 4990620748.42133)
		pJunkcode = 9213704184.51925;
	pJunkcode = 4644506903.59399;
	if (pJunkcode = 766957062.252526)
		pJunkcode = 4297652965.07245;
	pJunkcode = 9500570070.57735;
	pJunkcode = 8014912071.23909;
	if (pJunkcode = 8722872782.73105)
		pJunkcode = 7059750622.36653;
	pJunkcode = 8510109828.15187;
	if (pJunkcode = 5741400289.07638)
		pJunkcode = 8307062505.80748;
	pJunkcode = 3457045685.28417;
	pJunkcode = 2540415823.1834;
	if (pJunkcode = 8456858446.46199)
		pJunkcode = 4971014723.01825;
	pJunkcode = 5812755357.49501;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3282() {
	float pJunkcode = 1854467084.78891;
	pJunkcode = 6922378868.60263;
	if (pJunkcode = 2625072177.68072)
		pJunkcode = 9270133835.69312;
	pJunkcode = 630676681.903579;
	pJunkcode = 8896317609.92361;
	if (pJunkcode = 784110914.227498)
		pJunkcode = 332164686.260437;
	pJunkcode = 7760083515.03629;
	if (pJunkcode = 5983878478.93423)
		pJunkcode = 9813263059.16438;
	pJunkcode = 7285736534.92255;
	pJunkcode = 213628058.691018;
	if (pJunkcode = 5441126414.05097)
		pJunkcode = 5633139385.53711;
	pJunkcode = 2022544278.68686;
	if (pJunkcode = 8438510075.8001)
		pJunkcode = 4879399747.17094;
	pJunkcode = 2046813748.14486;
	pJunkcode = 504182171.107315;
	if (pJunkcode = 6602682327.4573)
		pJunkcode = 9235498633.10656;
	pJunkcode = 4530097660.82286;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3281() {
	float pJunkcode = 5415283197.47433;
	pJunkcode = 1081847226.04016;
	if (pJunkcode = 2606548131.07006)
		pJunkcode = 507192692.446686;
	pJunkcode = 4433847953.30847;
	pJunkcode = 5095530519.0939;
	if (pJunkcode = 1883640773.04756)
		pJunkcode = 4123489063.92599;
	pJunkcode = 3526804664.37903;
	if (pJunkcode = 4890779284.63292)
		pJunkcode = 6402500455.78558;
	pJunkcode = 6433312470.75268;
	pJunkcode = 1913544862.35764;
	if (pJunkcode = 5468858834.50239)
		pJunkcode = 3648069916.40504;
	pJunkcode = 3874594779.94537;
	if (pJunkcode = 6980317372.98709)
		pJunkcode = 3562954298.74262;
	pJunkcode = 8355553246.93393;
	pJunkcode = 3404847306.90288;
	if (pJunkcode = 5333201153.48278)
		pJunkcode = 714942296.937265;
	pJunkcode = 5440805532.39507;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3280() {
	float pJunkcode = 8092131016.23877;
	pJunkcode = 55194376.1289206;
	if (pJunkcode = 6543830053.42511)
		pJunkcode = 740918824.022407;
	pJunkcode = 1747659723.76564;
	pJunkcode = 1689453944.1166;
	if (pJunkcode = 766941609.437238)
		pJunkcode = 1978574495.34292;
	pJunkcode = 4158102557.60044;
	if (pJunkcode = 3222144750.92327)
		pJunkcode = 9434028683.34067;
	pJunkcode = 4161194214.95113;
	pJunkcode = 9725535419.79397;
	if (pJunkcode = 6929721381.9355)
		pJunkcode = 2372931312.38838;
	pJunkcode = 4424085592.43078;
	if (pJunkcode = 8785532952.21896)
		pJunkcode = 2070727085.74002;
	pJunkcode = 7811407453.12887;
	pJunkcode = 3280224495.94866;
	if (pJunkcode = 476964410.662066)
		pJunkcode = 4227136264.85164;
	pJunkcode = 3489745597.95922;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3279() {
	float pJunkcode = 1408109721.25341;
	pJunkcode = 8301831603.26933;
	if (pJunkcode = 153233080.539463)
		pJunkcode = 4614387795.00643;
	pJunkcode = 5348477624.60482;
	pJunkcode = 4172230135.07222;
	if (pJunkcode = 5152828175.18185)
		pJunkcode = 4631254134.27966;
	pJunkcode = 4956271561.45045;
	if (pJunkcode = 3298353237.33243)
		pJunkcode = 4482884467.28886;
	pJunkcode = 2631591605.66639;
	pJunkcode = 3967971115.96782;
	if (pJunkcode = 3067436221.29019)
		pJunkcode = 1080011961.50118;
	pJunkcode = 6988108914.35234;
	if (pJunkcode = 1408987563.16381)
		pJunkcode = 7169089982.00582;
	pJunkcode = 1253044973.61101;
	pJunkcode = 2002348862.37323;
	if (pJunkcode = 6991143749.95267)
		pJunkcode = 5381686735.13376;
	pJunkcode = 8719819758.58271;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3278() {
	float pJunkcode = 7397820553.90451;
	pJunkcode = 8276413167.9843;
	if (pJunkcode = 3468024243.49322)
		pJunkcode = 4221230078.70913;
	pJunkcode = 2892303707.78632;
	pJunkcode = 5336378449.74442;
	if (pJunkcode = 8857367463.03278)
		pJunkcode = 3557415119.87355;
	pJunkcode = 9910859026.56364;
	if (pJunkcode = 2462537736.56648)
		pJunkcode = 1231428948.34658;
	pJunkcode = 1633590545.3753;
	pJunkcode = 148284598.747793;
	if (pJunkcode = 642498444.154553)
		pJunkcode = 9377776321.61579;
	pJunkcode = 7837763453.11546;
	if (pJunkcode = 2611195160.72875)
		pJunkcode = 1935299823.26267;
	pJunkcode = 8966576755.462;
	pJunkcode = 4894512584.22468;
	if (pJunkcode = 6008149610.7864)
		pJunkcode = 6286858580.3323;
	pJunkcode = 7897075351.19665;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3277() {
	float pJunkcode = 6775457046.81896;
	pJunkcode = 9624482266.23037;
	if (pJunkcode = 8175453981.60049)
		pJunkcode = 1769478025.61385;
	pJunkcode = 4178320683.98794;
	pJunkcode = 2405312573.90279;
	if (pJunkcode = 7074859939.05614)
		pJunkcode = 7602773257.03503;
	pJunkcode = 9243052345.00427;
	if (pJunkcode = 38400045.9970557)
		pJunkcode = 1066652356.58607;
	pJunkcode = 6129404185.30587;
	pJunkcode = 7439836560.76869;
	if (pJunkcode = 7608385625.48501)
		pJunkcode = 422443018.032309;
	pJunkcode = 9813304802.95304;
	if (pJunkcode = 3599649996.99555)
		pJunkcode = 1708334930.50435;
	pJunkcode = 4272861613.27393;
	pJunkcode = 4847270284.82299;
	if (pJunkcode = 8845857227.396)
		pJunkcode = 4599320175.5398;
	pJunkcode = 7408103002.0164;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3276() {
	float pJunkcode = 6088021625.12607;
	pJunkcode = 7727369518.57645;
	if (pJunkcode = 9307101214.32236)
		pJunkcode = 9256584654.97883;
	pJunkcode = 3234969146.4051;
	pJunkcode = 6620665707.13896;
	if (pJunkcode = 6040353969.34683)
		pJunkcode = 4848206827.40825;
	pJunkcode = 6529329668.92654;
	if (pJunkcode = 7586790696.79424)
		pJunkcode = 4289939062.63723;
	pJunkcode = 2862580567.98635;
	pJunkcode = 1997998048.53871;
	if (pJunkcode = 3897473826.64366)
		pJunkcode = 8016377129.44357;
	pJunkcode = 4415400733.05702;
	if (pJunkcode = 2090128930.02369)
		pJunkcode = 3494448336.44891;
	pJunkcode = 4713531865.81367;
	pJunkcode = 308245730.518026;
	if (pJunkcode = 5336606784.47326)
		pJunkcode = 1238971982.73775;
	pJunkcode = 1647817158.69929;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3275() {
	float pJunkcode = 803220633.480502;
	pJunkcode = 6435250379.98275;
	if (pJunkcode = 2308865556.03258)
		pJunkcode = 1274853349.98814;
	pJunkcode = 6019323043.96709;
	pJunkcode = 2695161356.22064;
	if (pJunkcode = 969582737.174422)
		pJunkcode = 7630276383.50495;
	pJunkcode = 7355334732.09597;
	if (pJunkcode = 856516720.535203)
		pJunkcode = 5070215145.26265;
	pJunkcode = 8462999172.20794;
	pJunkcode = 2507059944.66933;
	if (pJunkcode = 1402945205.0528)
		pJunkcode = 9826994537.46473;
	pJunkcode = 9995121568.26356;
	if (pJunkcode = 378400431.602453)
		pJunkcode = 6599696520.84361;
	pJunkcode = 147128219.667205;
	pJunkcode = 1591551509.46245;
	if (pJunkcode = 430794348.516086)
		pJunkcode = 4041999926.55255;
	pJunkcode = 5049481266.90418;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3274() {
	float pJunkcode = 3765967437.23374;
	pJunkcode = 4586721958.79662;
	if (pJunkcode = 4226820680.03992)
		pJunkcode = 3882143805.00909;
	pJunkcode = 7578268568.11491;
	pJunkcode = 3593889202.82283;
	if (pJunkcode = 7420340076.4314)
		pJunkcode = 9334923892.9985;
	pJunkcode = 6397960364.17212;
	if (pJunkcode = 4705174102.28354)
		pJunkcode = 3834082899.87699;
	pJunkcode = 3381025587.2498;
	pJunkcode = 7006247513.68697;
	if (pJunkcode = 4121517168.74674)
		pJunkcode = 3289587109.2352;
	pJunkcode = 9844853302.77546;
	if (pJunkcode = 4252222510.00949)
		pJunkcode = 9955639380.73283;
	pJunkcode = 8423520826.34441;
	pJunkcode = 6750089896.62695;
	if (pJunkcode = 679968669.081245)
		pJunkcode = 656855017.587595;
	pJunkcode = 7500132542.64885;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3273() {
	float pJunkcode = 8155732620.03484;
	pJunkcode = 9736822154.21774;
	if (pJunkcode = 2534448256.51212)
		pJunkcode = 6148450884.91877;
	pJunkcode = 4740767782.21095;
	pJunkcode = 2813508042.45042;
	if (pJunkcode = 2866610427.98819)
		pJunkcode = 4797613416.09439;
	pJunkcode = 4550925816.02277;
	if (pJunkcode = 1135839006.42933)
		pJunkcode = 4067301170.03334;
	pJunkcode = 5700004068.85931;
	pJunkcode = 9988584103.0683;
	if (pJunkcode = 8723933178.33864)
		pJunkcode = 4310436361.81622;
	pJunkcode = 9296847140.99726;
	if (pJunkcode = 8888536317.6608)
		pJunkcode = 9066659657.55684;
	pJunkcode = 7216374220.77195;
	pJunkcode = 7645935871.1754;
	if (pJunkcode = 9928297331.87629)
		pJunkcode = 9948454517.78054;
	pJunkcode = 464533744.15944;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3272() {
	float pJunkcode = 8871092447.75141;
	pJunkcode = 3527037772.08833;
	if (pJunkcode = 2787648064.56336)
		pJunkcode = 3882378970.9207;
	pJunkcode = 234381988.224009;
	pJunkcode = 3432311651.16033;
	if (pJunkcode = 6719263690.77403)
		pJunkcode = 3263657497.0556;
	pJunkcode = 3460508287.03513;
	if (pJunkcode = 2343399250.13023)
		pJunkcode = 2231387785.10789;
	pJunkcode = 2029834454.54273;
	pJunkcode = 9711694318.93792;
	if (pJunkcode = 5951395899.85013)
		pJunkcode = 9375084379.26027;
	pJunkcode = 3678547438.57293;
	if (pJunkcode = 8655187777.73374)
		pJunkcode = 8459877454.39345;
	pJunkcode = 5614720344.49666;
	pJunkcode = 7673980020.80164;
	if (pJunkcode = 2266741025.56235)
		pJunkcode = 4852179155.53337;
	pJunkcode = 9926975621.40899;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3271() {
	float pJunkcode = 8234436837.39334;
	pJunkcode = 3049100719.04928;
	if (pJunkcode = 4936568474.57927)
		pJunkcode = 9665013161.38727;
	pJunkcode = 9748902903.04527;
	pJunkcode = 3042933534.69358;
	if (pJunkcode = 8361957231.65601)
		pJunkcode = 3777479856.80013;
	pJunkcode = 8485897052.38865;
	if (pJunkcode = 5582357017.74739)
		pJunkcode = 109902653.062539;
	pJunkcode = 3034272279.45442;
	pJunkcode = 7061211643.32802;
	if (pJunkcode = 9423154355.38354)
		pJunkcode = 6272478393.45152;
	pJunkcode = 994090764.852585;
	if (pJunkcode = 8539100375.98596)
		pJunkcode = 2823405110.8065;
	pJunkcode = 4739648243.5547;
	pJunkcode = 9683808585.69742;
	if (pJunkcode = 2465500749.28811)
		pJunkcode = 971080761.540067;
	pJunkcode = 1246907351.4665;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3270() {
	float pJunkcode = 1355045389.1755;
	pJunkcode = 4114132154.93318;
	if (pJunkcode = 3318554946.99541)
		pJunkcode = 3250827373.99722;
	pJunkcode = 7186490301.37883;
	pJunkcode = 6914320180.02838;
	if (pJunkcode = 9243537389.24873)
		pJunkcode = 9064947594.12092;
	pJunkcode = 4831082353.7101;
	if (pJunkcode = 81119331.7422183)
		pJunkcode = 1313152238.82847;
	pJunkcode = 838264896.790825;
	pJunkcode = 7097658424.82904;
	if (pJunkcode = 7754091783.11416)
		pJunkcode = 1240606349.03656;
	pJunkcode = 2340832930.29225;
	if (pJunkcode = 1798556820.03602)
		pJunkcode = 710197674.961926;
	pJunkcode = 187550921.834258;
	pJunkcode = 5581695529.22072;
	if (pJunkcode = 7971415979.43097)
		pJunkcode = 6573052086.15514;
	pJunkcode = 1493623351.93603;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3269() {
	float pJunkcode = 5548997879.6369;
	pJunkcode = 9631997356.56713;
	if (pJunkcode = 4770346857.16254)
		pJunkcode = 6150703403.40992;
	pJunkcode = 9443620588.28042;
	pJunkcode = 9067389793.184;
	if (pJunkcode = 9038822522.5706)
		pJunkcode = 9374914926.81238;
	pJunkcode = 9434441681.19124;
	if (pJunkcode = 4036441385.68078)
		pJunkcode = 3698034588.82306;
	pJunkcode = 8000381516.84743;
	pJunkcode = 5714082431.08181;
	if (pJunkcode = 8916069634.97838)
		pJunkcode = 6478647624.52961;
	pJunkcode = 1995069755.17547;
	if (pJunkcode = 4507014130.32836)
		pJunkcode = 1950503894.09177;
	pJunkcode = 1597486887.61914;
	pJunkcode = 4018054395.78623;
	if (pJunkcode = 436176754.767309)
		pJunkcode = 9268878525.53307;
	pJunkcode = 6291900933.84415;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3268() {
	float pJunkcode = 293111363.829964;
	pJunkcode = 4359618025.62496;
	if (pJunkcode = 4611605505.95144)
		pJunkcode = 3766722585.60682;
	pJunkcode = 793053910.354461;
	pJunkcode = 5307044910.87761;
	if (pJunkcode = 5572562605.72381)
		pJunkcode = 6668244026.69362;
	pJunkcode = 8661779750.3659;
	if (pJunkcode = 1674297216.59714)
		pJunkcode = 8671156258.8463;
	pJunkcode = 7232472312.75108;
	pJunkcode = 8994735228.44764;
	if (pJunkcode = 6286164160.56872)
		pJunkcode = 2559477936.03077;
	pJunkcode = 3669703585.3992;
	if (pJunkcode = 363771351.25259)
		pJunkcode = 7955207983.17387;
	pJunkcode = 1208404418.87547;
	pJunkcode = 7539939053.03798;
	if (pJunkcode = 1126687959.57636)
		pJunkcode = 179147845.257399;
	pJunkcode = 4756387118.63842;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3267() {
	float pJunkcode = 3372989562.18377;
	pJunkcode = 6734285142.71401;
	if (pJunkcode = 3126268963.35501)
		pJunkcode = 5676304905.37485;
	pJunkcode = 663282187.353322;
	pJunkcode = 1942916149.67921;
	if (pJunkcode = 3524335743.6102)
		pJunkcode = 3708592833.71586;
	pJunkcode = 6265561886.08719;
	if (pJunkcode = 7075584635.81385)
		pJunkcode = 7607125353.75356;
	pJunkcode = 3266206487.48207;
	pJunkcode = 6439989637.59371;
	if (pJunkcode = 7113936786.4596)
		pJunkcode = 9977694950.93234;
	pJunkcode = 2644160593.14287;
	if (pJunkcode = 8868733333.47265)
		pJunkcode = 6123822179.99649;
	pJunkcode = 8139988808.93344;
	pJunkcode = 4371044564.13895;
	if (pJunkcode = 3443824790.16621)
		pJunkcode = 2177427958.07671;
	pJunkcode = 6057496335.901;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3266() {
	float pJunkcode = 8639380832.90635;
	pJunkcode = 4080554565.65585;
	if (pJunkcode = 4864153140.9369)
		pJunkcode = 2678942675.47215;
	pJunkcode = 5075752126.23242;
	pJunkcode = 9816858407.97932;
	if (pJunkcode = 3979102912.46088)
		pJunkcode = 189900343.470248;
	pJunkcode = 4123324030.55953;
	if (pJunkcode = 7452547196.33564)
		pJunkcode = 6105701957.80078;
	pJunkcode = 3741625951.64953;
	pJunkcode = 5521219266.04772;
	if (pJunkcode = 464140788.170252)
		pJunkcode = 6164020868.72824;
	pJunkcode = 9651087618.35783;
	if (pJunkcode = 6487306525.8745)
		pJunkcode = 957550942.828477;
	pJunkcode = 7560560414.78479;
	pJunkcode = 2544929865.96494;
	if (pJunkcode = 8295523031.59086)
		pJunkcode = 2551390095.02316;
	pJunkcode = 878419501.809348;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3265() {
	float pJunkcode = 2504136468.52393;
	pJunkcode = 5116521095.32659;
	if (pJunkcode = 4174517105.16919)
		pJunkcode = 1508556205.71718;
	pJunkcode = 1960624517.21138;
	pJunkcode = 5055589175.49908;
	if (pJunkcode = 4338914235.66967)
		pJunkcode = 5230087895.40439;
	pJunkcode = 2830648157.55479;
	if (pJunkcode = 8873029823.80464)
		pJunkcode = 2478548857.85671;
	pJunkcode = 7783570006.15268;
	pJunkcode = 3084073892.89009;
	if (pJunkcode = 7343100747.31272)
		pJunkcode = 8265906615.63406;
	pJunkcode = 8881607328.37049;
	if (pJunkcode = 316135101.684803)
		pJunkcode = 3366884349.74075;
	pJunkcode = 5752692501.95042;
	pJunkcode = 5569685949.02434;
	if (pJunkcode = 336097154.177979)
		pJunkcode = 1246483715.6902;
	pJunkcode = 8850140285.86247;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3264() {
	float pJunkcode = 6587382703.88759;
	pJunkcode = 6841420728.79824;
	if (pJunkcode = 3024731741.0976)
		pJunkcode = 9216855823.10054;
	pJunkcode = 2400953026.67816;
	pJunkcode = 7286630563.05743;
	if (pJunkcode = 1200206359.27167)
		pJunkcode = 9031314831.5177;
	pJunkcode = 2733578957.23411;
	if (pJunkcode = 3888868773.83154)
		pJunkcode = 6673851391.6631;
	pJunkcode = 6539596894.72317;
	pJunkcode = 3080677067.4665;
	if (pJunkcode = 6494956887.13418)
		pJunkcode = 2215037470.21476;
	pJunkcode = 9126323221.85379;
	if (pJunkcode = 109373592.625062)
		pJunkcode = 4453888282.68126;
	pJunkcode = 887670257.388355;
	pJunkcode = 8376736579.40059;
	if (pJunkcode = 6643617487.07467)
		pJunkcode = 2594744659.81154;
	pJunkcode = 6156441887.97347;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3263() {
	float pJunkcode = 6676877577.69153;
	pJunkcode = 2438482045.41104;
	if (pJunkcode = 5217710078.60106)
		pJunkcode = 876275020.313072;
	pJunkcode = 1528390058.28337;
	pJunkcode = 9476448931.38248;
	if (pJunkcode = 9724824527.42707)
		pJunkcode = 1926801172.99065;
	pJunkcode = 4483638768.16687;
	if (pJunkcode = 2853356501.51712)
		pJunkcode = 6737464387.74865;
	pJunkcode = 3161262090.58244;
	pJunkcode = 897603421.118162;
	if (pJunkcode = 3415173354.4451)
		pJunkcode = 4596891091.69572;
	pJunkcode = 5096564539.90994;
	if (pJunkcode = 892332912.155284)
		pJunkcode = 5114559074.39618;
	pJunkcode = 749643673.98761;
	pJunkcode = 617786843.038887;
	if (pJunkcode = 4623623537.81566)
		pJunkcode = 4100068428.29859;
	pJunkcode = 9375063501.54524;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3262() {
	float pJunkcode = 1591896505.14697;
	pJunkcode = 4257413391.68853;
	if (pJunkcode = 8735361400.20136)
		pJunkcode = 2121619383.41882;
	pJunkcode = 4552743557.90274;
	pJunkcode = 4714993510.75505;
	if (pJunkcode = 4555416147.56868)
		pJunkcode = 3903236057.36708;
	pJunkcode = 6790584742.97579;
	if (pJunkcode = 5005319027.67479)
		pJunkcode = 9023662351.52035;
	pJunkcode = 2820360629.12906;
	pJunkcode = 9016429989.26346;
	if (pJunkcode = 6631345898.1902)
		pJunkcode = 3136923962.38254;
	pJunkcode = 7590776938.73949;
	if (pJunkcode = 4660697139.7072)
		pJunkcode = 7145037454.98719;
	pJunkcode = 788660162.978599;
	pJunkcode = 9726604648.90784;
	if (pJunkcode = 7159292652.64345)
		pJunkcode = 5180477936.92477;
	pJunkcode = 1743330241.66137;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3261() {
	float pJunkcode = 53824558.6751088;
	pJunkcode = 4471195684.93018;
	if (pJunkcode = 5121118935.3471)
		pJunkcode = 2768915240.80236;
	pJunkcode = 859030623.734553;
	pJunkcode = 1925140657.74374;
	if (pJunkcode = 2134920089.755)
		pJunkcode = 1338753143.9925;
	pJunkcode = 5381381399.42447;
	if (pJunkcode = 2779465148.11319)
		pJunkcode = 2977474563.96656;
	pJunkcode = 9800361138.83841;
	pJunkcode = 8611599986.49362;
	if (pJunkcode = 9376965292.11875)
		pJunkcode = 1606356759.35711;
	pJunkcode = 6276509000.83226;
	if (pJunkcode = 7070026944.76544)
		pJunkcode = 6459259377.41955;
	pJunkcode = 3661228064.29203;
	pJunkcode = 7616126384.6664;
	if (pJunkcode = 2523621313.77373)
		pJunkcode = 3469104867.22485;
	pJunkcode = 3578975939.21613;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3260() {
	float pJunkcode = 4470405670.51689;
	pJunkcode = 850175953.357292;
	if (pJunkcode = 4485090948.63722)
		pJunkcode = 3614068570.28068;
	pJunkcode = 5515647672.8206;
	pJunkcode = 7863115436.87101;
	if (pJunkcode = 6402480195.06558)
		pJunkcode = 5646854875.51703;
	pJunkcode = 9068176846.02112;
	if (pJunkcode = 7851587413.15093)
		pJunkcode = 7737614227.54671;
	pJunkcode = 4876297365.74804;
	pJunkcode = 8859153193.34768;
	if (pJunkcode = 4199484773.19434)
		pJunkcode = 9833016090.03748;
	pJunkcode = 6716655203.39986;
	if (pJunkcode = 5394018100.3599)
		pJunkcode = 1897354323.56762;
	pJunkcode = 5641083976.59543;
	pJunkcode = 2131535681.06546;
	if (pJunkcode = 4789637213.9357)
		pJunkcode = 8341903202.66515;
	pJunkcode = 5987455422.67084;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3259() {
	float pJunkcode = 6387905657.07687;
	pJunkcode = 4017654198.83643;
	if (pJunkcode = 8784346205.3412)
		pJunkcode = 8900067566.05305;
	pJunkcode = 3762676723.31486;
	pJunkcode = 9729495233.39714;
	if (pJunkcode = 1945156041.89082)
		pJunkcode = 1916786642.74949;
	pJunkcode = 6601804289.97112;
	if (pJunkcode = 9238591496.50793)
		pJunkcode = 9897367338.14525;
	pJunkcode = 8483060381.62405;
	pJunkcode = 4430189619.77767;
	if (pJunkcode = 4897076105.46631)
		pJunkcode = 3454311412.41265;
	pJunkcode = 9556758027.33365;
	if (pJunkcode = 9517178256.7994)
		pJunkcode = 3317925402.51361;
	pJunkcode = 1714194944.46365;
	pJunkcode = 6014530817.9144;
	if (pJunkcode = 8979428517.58401)
		pJunkcode = 6368130672.82192;
	pJunkcode = 3623428606.15777;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3258() {
	float pJunkcode = 2543525385.03155;
	pJunkcode = 5448008535.00141;
	if (pJunkcode = 7157760761.07106)
		pJunkcode = 5550762185.09022;
	pJunkcode = 6956393478.95611;
	pJunkcode = 8774162136.50567;
	if (pJunkcode = 1598285626.1094)
		pJunkcode = 7512013438.63535;
	pJunkcode = 5427804797.40379;
	if (pJunkcode = 4867833236.93033)
		pJunkcode = 5446605164.65907;
	pJunkcode = 467597527.304224;
	pJunkcode = 3542968567.60977;
	if (pJunkcode = 3262446503.43708)
		pJunkcode = 8228660690.5215;
	pJunkcode = 5012806767.66935;
	if (pJunkcode = 2689529874.89425)
		pJunkcode = 9084881253.20104;
	pJunkcode = 3130769716.29632;
	pJunkcode = 7636928344.35256;
	if (pJunkcode = 6033146601.20958)
		pJunkcode = 3507645760.14441;
	pJunkcode = 7286196917.99186;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3257() {
	float pJunkcode = 5963713151.0238;
	pJunkcode = 6333480762.47068;
	if (pJunkcode = 873498405.284126)
		pJunkcode = 462832779.649309;
	pJunkcode = 3254379153.64951;
	pJunkcode = 5798158526.1951;
	if (pJunkcode = 6571929439.24088)
		pJunkcode = 6696408923.41247;
	pJunkcode = 3504419518.71433;
	if (pJunkcode = 2173017181.63787)
		pJunkcode = 4303855436.88762;
	pJunkcode = 6254088095.0666;
	pJunkcode = 2710310556.40067;
	if (pJunkcode = 3280132733.65303)
		pJunkcode = 8075010563.48563;
	pJunkcode = 5600948926.82711;
	if (pJunkcode = 7553972216.29962)
		pJunkcode = 2866849328.46598;
	pJunkcode = 5924818931.8226;
	pJunkcode = 6335488636.6095;
	if (pJunkcode = 2975494689.76093)
		pJunkcode = 9697880405.44425;
	pJunkcode = 4409218365.96687;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3256() {
	float pJunkcode = 9668812542.31149;
	pJunkcode = 7629994488.95046;
	if (pJunkcode = 2550006829.40586)
		pJunkcode = 4436958148.44963;
	pJunkcode = 575942900.9563;
	pJunkcode = 1635562077.34239;
	if (pJunkcode = 4347902111.7827)
		pJunkcode = 964263848.815903;
	pJunkcode = 5265972121.78649;
	if (pJunkcode = 5553435969.16542)
		pJunkcode = 45133662.0631297;
	pJunkcode = 8081073978.58948;
	pJunkcode = 9647981299.80812;
	if (pJunkcode = 459110998.074304)
		pJunkcode = 2711727041.96201;
	pJunkcode = 742275435.454174;
	if (pJunkcode = 1588796249.44659)
		pJunkcode = 7435436794.78087;
	pJunkcode = 5245487970.42246;
	pJunkcode = 6692497980.47268;
	if (pJunkcode = 196896731.90128)
		pJunkcode = 1548689674.58123;
	pJunkcode = 479212973.40014;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3255() {
	float pJunkcode = 4393732870.89647;
	pJunkcode = 6801822169.01894;
	if (pJunkcode = 5249566535.77959)
		pJunkcode = 5272086330.71561;
	pJunkcode = 4061211585.02685;
	pJunkcode = 4593584110.06245;
	if (pJunkcode = 312558283.675485)
		pJunkcode = 5216906581.10002;
	pJunkcode = 2461254186.66006;
	if (pJunkcode = 401525058.622888)
		pJunkcode = 4515256119.97869;
	pJunkcode = 2005769089.37079;
	pJunkcode = 4461665188.4586;
	if (pJunkcode = 9853835649.21599)
		pJunkcode = 9649988885.23983;
	pJunkcode = 195859449.160546;
	if (pJunkcode = 7523451148.25191)
		pJunkcode = 6879120501.4893;
	pJunkcode = 7872012633.34006;
	pJunkcode = 7095514120.28228;
	if (pJunkcode = 8537778362.84185)
		pJunkcode = 3961830357.6259;
	pJunkcode = 8458634346.76541;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3254() {
	float pJunkcode = 4904704662.35426;
	pJunkcode = 4191267342.2747;
	if (pJunkcode = 5950520027.66728)
		pJunkcode = 7094810704.03259;
	pJunkcode = 7339299601.54987;
	pJunkcode = 2799207976.44951;
	if (pJunkcode = 6204651693.79807)
		pJunkcode = 7731200934.34858;
	pJunkcode = 5726717548.78469;
	if (pJunkcode = 1282674909.90808)
		pJunkcode = 4035280990.31176;
	pJunkcode = 5000615902.69009;
	pJunkcode = 8178309986.42839;
	if (pJunkcode = 5687384501.75962)
		pJunkcode = 9826994935.60114;
	pJunkcode = 7863993595.42791;
	if (pJunkcode = 8963079145.64556)
		pJunkcode = 1458000996.8733;
	pJunkcode = 9273313974.78849;
	pJunkcode = 8128707031.4245;
	if (pJunkcode = 361986084.266046)
		pJunkcode = 458549267.16762;
	pJunkcode = 5784948986.11407;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3253() {
	float pJunkcode = 5127052131.90247;
	pJunkcode = 757604949.523008;
	if (pJunkcode = 8666740602.32597)
		pJunkcode = 1484415494.97412;
	pJunkcode = 719039828.488956;
	pJunkcode = 7678232676.5467;
	if (pJunkcode = 1001459848.55281)
		pJunkcode = 5172581122.23675;
	pJunkcode = 9611159101.2749;
	if (pJunkcode = 6442027348.68114)
		pJunkcode = 8638286977.54156;
	pJunkcode = 6567595875.65625;
	pJunkcode = 5765606927.48521;
	if (pJunkcode = 4870489486.70511)
		pJunkcode = 4511920924.21017;
	pJunkcode = 416038318.527293;
	if (pJunkcode = 5113852996.91885)
		pJunkcode = 4390368217.68949;
	pJunkcode = 5846709662.98654;
	pJunkcode = 8904734833.1489;
	if (pJunkcode = 8268905972.85571)
		pJunkcode = 7223471011.7574;
	pJunkcode = 2194846067.86129;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3252() {
	float pJunkcode = 4747759735.26257;
	pJunkcode = 9359540849.40399;
	if (pJunkcode = 7997739648.87769)
		pJunkcode = 898586827.386729;
	pJunkcode = 6875128536.62406;
	pJunkcode = 579091945.830156;
	if (pJunkcode = 5421219120.44928)
		pJunkcode = 2601112979.89228;
	pJunkcode = 3896981916.29217;
	if (pJunkcode = 5556167657.64264)
		pJunkcode = 4262556289.78006;
	pJunkcode = 4806257798.26554;
	pJunkcode = 9931650441.4554;
	if (pJunkcode = 4461267194.06467)
		pJunkcode = 8643133827.44531;
	pJunkcode = 3893322353.42787;
	if (pJunkcode = 7382876824.71275)
		pJunkcode = 4540971625.56781;
	pJunkcode = 9948836309.65201;
	pJunkcode = 2330107035.04271;
	if (pJunkcode = 7170434928.73899)
		pJunkcode = 8518301019.32036;
	pJunkcode = 9959241181.22038;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3251() {
	float pJunkcode = 4908287468.70785;
	pJunkcode = 3077073509.12516;
	if (pJunkcode = 4024142774.74204)
		pJunkcode = 904459772.630659;
	pJunkcode = 9284167106.05579;
	pJunkcode = 2005358862.17718;
	if (pJunkcode = 662968064.703419)
		pJunkcode = 9313510235.12091;
	pJunkcode = 6153747885.08737;
	if (pJunkcode = 6982659190.4503)
		pJunkcode = 8466903058.12149;
	pJunkcode = 6075828417.89396;
	pJunkcode = 8917662227.22086;
	if (pJunkcode = 4100234239.31163)
		pJunkcode = 7476774565.77766;
	pJunkcode = 799187455.675167;
	if (pJunkcode = 836220750.328755)
		pJunkcode = 5757492918.90324;
	pJunkcode = 6138257237.52635;
	pJunkcode = 6518867954.65327;
	if (pJunkcode = 9420959818.12776)
		pJunkcode = 9887392884.642;
	pJunkcode = 2163133961.90998;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3250() {
	float pJunkcode = 2535781892.44574;
	pJunkcode = 8141632679.28409;
	if (pJunkcode = 3793920790.80186)
		pJunkcode = 4719381239.51896;
	pJunkcode = 5383035339.59976;
	pJunkcode = 4575992573.45063;
	if (pJunkcode = 3238914859.9255)
		pJunkcode = 5214183504.94219;
	pJunkcode = 5603578017.1789;
	if (pJunkcode = 734930312.115883)
		pJunkcode = 251713721.031889;
	pJunkcode = 2660669308.12376;
	pJunkcode = 7791254886.53839;
	if (pJunkcode = 296388888.024782)
		pJunkcode = 4554363810.14908;
	pJunkcode = 3126867830.37799;
	if (pJunkcode = 3066201164.86529)
		pJunkcode = 7661641102.62496;
	pJunkcode = 9571131096.65283;
	pJunkcode = 388162019.490131;
	if (pJunkcode = 5211081260.88799)
		pJunkcode = 92717501.9771374;
	pJunkcode = 3768949781.69039;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3249() {
	float pJunkcode = 8835300353.86197;
	pJunkcode = 2230599603.05344;
	if (pJunkcode = 4790778678.03441)
		pJunkcode = 9107463329.71141;
	pJunkcode = 5935287447.01097;
	pJunkcode = 8042721958.32759;
	if (pJunkcode = 4151391646.95167)
		pJunkcode = 7275962080.94217;
	pJunkcode = 6418783434.40163;
	if (pJunkcode = 9003658217.63746)
		pJunkcode = 1538196363.17729;
	pJunkcode = 652879758.153979;
	pJunkcode = 2206618475.16599;
	if (pJunkcode = 586331388.396121)
		pJunkcode = 4168575987.79454;
	pJunkcode = 6172228595.36369;
	if (pJunkcode = 2495268312.73065)
		pJunkcode = 1292338057.98404;
	pJunkcode = 6901930670.58706;
	pJunkcode = 6877208471.12648;
	if (pJunkcode = 1196922399.70222)
		pJunkcode = 2495137735.62656;
	pJunkcode = 4453825169.90982;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3248() {
	float pJunkcode = 3327639987.02099;
	pJunkcode = 7748384634.91358;
	if (pJunkcode = 3526752813.5132)
		pJunkcode = 1376358879.65527;
	pJunkcode = 7134157569.41818;
	pJunkcode = 6281859947.32151;
	if (pJunkcode = 7410030850.73496)
		pJunkcode = 1096004542.67266;
	pJunkcode = 1806558554.14777;
	if (pJunkcode = 2831233359.88394)
		pJunkcode = 515334684.663873;
	pJunkcode = 2948581913.45421;
	pJunkcode = 8445199640.74891;
	if (pJunkcode = 4429689509.21292)
		pJunkcode = 7396808600.5562;
	pJunkcode = 7285612660.94566;
	if (pJunkcode = 3825293747.38766)
		pJunkcode = 6861230364.89136;
	pJunkcode = 4045121066.95281;
	pJunkcode = 2608795182.75237;
	if (pJunkcode = 7880651190.14676)
		pJunkcode = 3021075698.59742;
	pJunkcode = 3059874504.755;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3247() {
	float pJunkcode = 634162086.859734;
	pJunkcode = 5406473455.97708;
	if (pJunkcode = 4503621519.09727)
		pJunkcode = 1747348665.13637;
	pJunkcode = 1623463465.92651;
	pJunkcode = 8894868249.00414;
	if (pJunkcode = 8990490232.75415)
		pJunkcode = 4810086012.64833;
	pJunkcode = 9179457486.24449;
	if (pJunkcode = 200622867.365781)
		pJunkcode = 3655154099.73721;
	pJunkcode = 1777769523.87716;
	pJunkcode = 8413804158.02361;
	if (pJunkcode = 1834454315.30381)
		pJunkcode = 986016479.109495;
	pJunkcode = 9384786356.11438;
	if (pJunkcode = 7207556088.14221)
		pJunkcode = 5971921937.02808;
	pJunkcode = 5294431401.97068;
	pJunkcode = 2511818226.68193;
	if (pJunkcode = 5544968266.43308)
		pJunkcode = 7495528015.79912;
	pJunkcode = 6159597263.44922;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3246() {
	float pJunkcode = 5114324802.5453;
	pJunkcode = 1322824599.92086;
	if (pJunkcode = 4474192145.76759)
		pJunkcode = 2371486039.93414;
	pJunkcode = 3354414463.30841;
	pJunkcode = 6912659871.99867;
	if (pJunkcode = 5400005024.37731)
		pJunkcode = 7736070973.95253;
	pJunkcode = 4321960865.17217;
	if (pJunkcode = 8269275748.45782)
		pJunkcode = 9453036326.00367;
	pJunkcode = 6566774331.63249;
	pJunkcode = 5524665274.12464;
	if (pJunkcode = 8708992934.58677)
		pJunkcode = 6412667316.28033;
	pJunkcode = 2558196113.71045;
	if (pJunkcode = 4145190646.61145)
		pJunkcode = 2277345237.74945;
	pJunkcode = 4698664385.5669;
	pJunkcode = 6009594522.35676;
	if (pJunkcode = 6052168963.27867)
		pJunkcode = 7306234669.95519;
	pJunkcode = 2067520988.61577;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3245() {
	float pJunkcode = 590237563.012013;
	pJunkcode = 9318376761.34559;
	if (pJunkcode = 8594365866.95694)
		pJunkcode = 8546048119.59707;
	pJunkcode = 3092278590.42916;
	pJunkcode = 5157070491.02142;
	if (pJunkcode = 1664457999.09256)
		pJunkcode = 3403410996.0105;
	pJunkcode = 3501585648.87436;
	if (pJunkcode = 1549455628.51642)
		pJunkcode = 4986990440.43485;
	pJunkcode = 4496089916.57639;
	pJunkcode = 333781939.678077;
	if (pJunkcode = 4561354906.3925)
		pJunkcode = 7927512472.99527;
	pJunkcode = 4697163699.97361;
	if (pJunkcode = 8930039427.02915)
		pJunkcode = 6443976575.69562;
	pJunkcode = 2957696059.34748;
	pJunkcode = 3298617548.69255;
	if (pJunkcode = 6151675734.03553)
		pJunkcode = 9177300902.33238;
	pJunkcode = 5886039829.92535;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3244() {
	float pJunkcode = 1469778545.0272;
	pJunkcode = 4045237737.22162;
	if (pJunkcode = 5160577958.78874)
		pJunkcode = 2033883301.69369;
	pJunkcode = 1706848913.09493;
	pJunkcode = 7191833827.62967;
	if (pJunkcode = 8660698919.41707)
		pJunkcode = 6509987761.63608;
	pJunkcode = 8933952349.00962;
	if (pJunkcode = 4390166178.59059)
		pJunkcode = 5831238798.84747;
	pJunkcode = 7349058127.7452;
	pJunkcode = 427445802.371962;
	if (pJunkcode = 3734657074.21224)
		pJunkcode = 7881856728.59784;
	pJunkcode = 6844519634.0944;
	if (pJunkcode = 434819347.062704)
		pJunkcode = 288260951.652991;
	pJunkcode = 2667095739.82372;
	pJunkcode = 9307903773.52529;
	if (pJunkcode = 7637289689.24138)
		pJunkcode = 9345036325.21798;
	pJunkcode = 5357241072.84814;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3243() {
	float pJunkcode = 4333351212.85284;
	pJunkcode = 3147593708.75555;
	if (pJunkcode = 6606524290.35211)
		pJunkcode = 1144972539.93642;
	pJunkcode = 2600856286.67464;
	pJunkcode = 7876081955.41307;
	if (pJunkcode = 5550727989.59555)
		pJunkcode = 9378295600.93574;
	pJunkcode = 1116274051.94107;
	if (pJunkcode = 3906753057.12563)
		pJunkcode = 1070573746.53237;
	pJunkcode = 1104823310.9237;
	pJunkcode = 9842159427.94493;
	if (pJunkcode = 6508160073.33048)
		pJunkcode = 6939952505.37356;
	pJunkcode = 4838937215.06398;
	if (pJunkcode = 3066755713.01844)
		pJunkcode = 9536521290.9789;
	pJunkcode = 5131881442.84922;
	pJunkcode = 1685624558.60739;
	if (pJunkcode = 3685570974.78553)
		pJunkcode = 2195584399.04515;
	pJunkcode = 2636949539.33339;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3242() {
	float pJunkcode = 2642529870.60719;
	pJunkcode = 2234516487.77184;
	if (pJunkcode = 3131287095.73721)
		pJunkcode = 5117366776.66179;
	pJunkcode = 8274509007.21853;
	pJunkcode = 654440732.311239;
	if (pJunkcode = 2863795242.62052)
		pJunkcode = 7847383958.68546;
	pJunkcode = 5567317954.67364;
	if (pJunkcode = 9273827486.95107)
		pJunkcode = 5744606088.18038;
	pJunkcode = 461921517.562528;
	pJunkcode = 3513521836.92918;
	if (pJunkcode = 5065487261.3149)
		pJunkcode = 4967414142.55107;
	pJunkcode = 8176599629.62402;
	if (pJunkcode = 8221139223.01718)
		pJunkcode = 6892790822.77605;
	pJunkcode = 6387386044.81242;
	pJunkcode = 9923911069.93077;
	if (pJunkcode = 1422106558.64129)
		pJunkcode = 5102013535.05934;
	pJunkcode = 445992715.051465;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3241() {
	float pJunkcode = 6055490051.97957;
	pJunkcode = 9182470030.86192;
	if (pJunkcode = 9149225120.65122)
		pJunkcode = 8694178557.82215;
	pJunkcode = 5533644416.84755;
	pJunkcode = 6245630257.26998;
	if (pJunkcode = 117558823.388687)
		pJunkcode = 2320410152.47214;
	pJunkcode = 8336012287.73184;
	if (pJunkcode = 3477947791.4136)
		pJunkcode = 1463914493.82491;
	pJunkcode = 5224717826.72559;
	pJunkcode = 2282994001.92168;
	if (pJunkcode = 8816871135.89586)
		pJunkcode = 4577912969.33483;
	pJunkcode = 8550080012.1901;
	if (pJunkcode = 4484232883.16393)
		pJunkcode = 8335624044.18776;
	pJunkcode = 867690374.902792;
	pJunkcode = 4739306637.85045;
	if (pJunkcode = 9187803566.44976)
		pJunkcode = 8732229719.66326;
	pJunkcode = 2094482807.87884;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3240() {
	float pJunkcode = 862361717.068161;
	pJunkcode = 5272427813.02084;
	if (pJunkcode = 6292733786.33918)
		pJunkcode = 1388129740.97995;
	pJunkcode = 5468157814.05607;
	pJunkcode = 677848012.906477;
	if (pJunkcode = 5994144312.41754)
		pJunkcode = 3089535355.99578;
	pJunkcode = 9001243.02517551;
	if (pJunkcode = 939404334.199794)
		pJunkcode = 5243221249.40408;
	pJunkcode = 8909410223.62468;
	pJunkcode = 7227206853.83436;
	if (pJunkcode = 9052645206.70691)
		pJunkcode = 7864913206.73808;
	pJunkcode = 7263516371.40209;
	if (pJunkcode = 9977260598.06868)
		pJunkcode = 6620320164.30791;
	pJunkcode = 9652683034.80777;
	pJunkcode = 4582665127.96273;
	if (pJunkcode = 673080282.720843)
		pJunkcode = 9742263065.56804;
	pJunkcode = 9480810529.82456;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3239() {
	float pJunkcode = 7641699934.83166;
	pJunkcode = 7962693557.86509;
	if (pJunkcode = 2374636442.19708)
		pJunkcode = 8039934225.74659;
	pJunkcode = 5651394385.0597;
	pJunkcode = 5440850272.68415;
	if (pJunkcode = 7055457324.11074)
		pJunkcode = 2219644304.39309;
	pJunkcode = 3579671590.64151;
	if (pJunkcode = 5009793959.95593)
		pJunkcode = 3937646992.73548;
	pJunkcode = 3710828120.5834;
	pJunkcode = 7455686380.93034;
	if (pJunkcode = 9832593574.16537)
		pJunkcode = 3745929652.67444;
	pJunkcode = 9165258123.14652;
	if (pJunkcode = 5410203398.18454)
		pJunkcode = 2826527203.47458;
	pJunkcode = 1232409614.22588;
	pJunkcode = 5643141459.4634;
	if (pJunkcode = 9218568429.02644)
		pJunkcode = 8480654124.268;
	pJunkcode = 3910604703.35007;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3238() {
	float pJunkcode = 6554867745.36968;
	pJunkcode = 6372661879.70687;
	if (pJunkcode = 7700823316.48251)
		pJunkcode = 1090862210.35393;
	pJunkcode = 1058075229.20209;
	pJunkcode = 2101473259.18783;
	if (pJunkcode = 4039253171.75504)
		pJunkcode = 2264731125.81718;
	pJunkcode = 9228801082.14653;
	if (pJunkcode = 4724246392.05657)
		pJunkcode = 2170695578.32899;
	pJunkcode = 6936878822.36881;
	pJunkcode = 4534968738.8411;
	if (pJunkcode = 2739739831.94246)
		pJunkcode = 4246093663.16419;
	pJunkcode = 2410875268.4317;
	if (pJunkcode = 473704057.555503)
		pJunkcode = 5765457961.60078;
	pJunkcode = 9739875285.4671;
	pJunkcode = 5737043985.15019;
	if (pJunkcode = 2746605491.93101)
		pJunkcode = 2461086790.08619;
	pJunkcode = 3496581928.56488;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3237() {
	float pJunkcode = 1768116424.67129;
	pJunkcode = 4539579274.98964;
	if (pJunkcode = 9657176297.66217)
		pJunkcode = 174157608.035659;
	pJunkcode = 1500695400.25641;
	pJunkcode = 2284056057.59517;
	if (pJunkcode = 8135727359.26291)
		pJunkcode = 166741220.606614;
	pJunkcode = 8882989862.90688;
	if (pJunkcode = 6340407467.71522)
		pJunkcode = 2108049250.48443;
	pJunkcode = 8164607206.5462;
	pJunkcode = 4427060637.71975;
	if (pJunkcode = 6143211859.00069)
		pJunkcode = 771505717.291422;
	pJunkcode = 6668693005.83204;
	if (pJunkcode = 5354909606.93182)
		pJunkcode = 3168378366.9696;
	pJunkcode = 8977165559.0712;
	pJunkcode = 9387456806.0881;
	if (pJunkcode = 7619523834.06988)
		pJunkcode = 2402910786.8504;
	pJunkcode = 4302294795.67485;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3236() {
	float pJunkcode = 1380027355.18748;
	pJunkcode = 3510871379.92289;
	if (pJunkcode = 4768055942.9539)
		pJunkcode = 7292957449.01118;
	pJunkcode = 650639227.992783;
	pJunkcode = 7460892552.44485;
	if (pJunkcode = 7211625799.0385)
		pJunkcode = 506698484.07973;
	pJunkcode = 269882022.274711;
	if (pJunkcode = 2299819032.79521)
		pJunkcode = 2627886245.47246;
	pJunkcode = 3458448941.54063;
	pJunkcode = 1706266404.58938;
	if (pJunkcode = 160344772.537093)
		pJunkcode = 2910421254.78469;
	pJunkcode = 5028186861.36141;
	if (pJunkcode = 9797109614.58156)
		pJunkcode = 8155432038.67976;
	pJunkcode = 1134378697.92248;
	pJunkcode = 1776089690.86493;
	if (pJunkcode = 2077919669.27924)
		pJunkcode = 6149107570.19729;
	pJunkcode = 6225958243.66577;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3235() {
	float pJunkcode = 3809050757.03476;
	pJunkcode = 6091017059.19061;
	if (pJunkcode = 9950526987.80717)
		pJunkcode = 3170470178.04438;
	pJunkcode = 7808129705.41887;
	pJunkcode = 3763068587.33925;
	if (pJunkcode = 6317465536.41725)
		pJunkcode = 1726291623.40451;
	pJunkcode = 4790299750.18979;
	if (pJunkcode = 2722747288.08092)
		pJunkcode = 9429203090.66211;
	pJunkcode = 5131271320.90548;
	pJunkcode = 4946806971.69008;
	if (pJunkcode = 8196409023.57569)
		pJunkcode = 8655206853.35761;
	pJunkcode = 1064966246.52246;
	if (pJunkcode = 3896576063.9426)
		pJunkcode = 6174502277.9487;
	pJunkcode = 8025722094.34381;
	pJunkcode = 6249685421.05313;
	if (pJunkcode = 3949212250.35974)
		pJunkcode = 5392544902.47752;
	pJunkcode = 1842127763.81924;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3234() {
	float pJunkcode = 6237666381.0423;
	pJunkcode = 9219926619.03419;
	if (pJunkcode = 9028288820.11051)
		pJunkcode = 148031360.702348;
	pJunkcode = 853020819.678593;
	pJunkcode = 7916987289.72324;
	if (pJunkcode = 3268799519.71266)
		pJunkcode = 5030613819.48297;
	pJunkcode = 9838842984.16892;
	if (pJunkcode = 1399697585.3268)
		pJunkcode = 7353438850.7051;
	pJunkcode = 1350451644.60505;
	pJunkcode = 4166104789.96478;
	if (pJunkcode = 7102518988.32056)
		pJunkcode = 3025981770.71146;
	pJunkcode = 4933342917.66443;
	if (pJunkcode = 9377945103.51813)
		pJunkcode = 528215138.219174;
	pJunkcode = 1689609652.70904;
	pJunkcode = 1865242933.35351;
	if (pJunkcode = 1213527463.69656)
		pJunkcode = 6485212533.1984;
	pJunkcode = 6966974259.30156;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3233() {
	float pJunkcode = 4919559730.52354;
	pJunkcode = 815881273.823306;
	if (pJunkcode = 7197335667.82181)
		pJunkcode = 1444441452.97069;
	pJunkcode = 8313270514.28;
	pJunkcode = 3527515749.99342;
	if (pJunkcode = 5235678305.81596)
		pJunkcode = 3039860028.63835;
	pJunkcode = 3337066192.70529;
	if (pJunkcode = 2941895822.30386)
		pJunkcode = 28653277.631648;
	pJunkcode = 4336159113.02251;
	pJunkcode = 9527580350.72597;
	if (pJunkcode = 2064545414.80653)
		pJunkcode = 1780062591.74771;
	pJunkcode = 8721592199.05737;
	if (pJunkcode = 6484784212.9164)
		pJunkcode = 5053203139.28222;
	pJunkcode = 916370275.065736;
	pJunkcode = 5545983796.02431;
	if (pJunkcode = 9866714261.09932)
		pJunkcode = 8042506545.57026;
	pJunkcode = 6918406563.76034;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3232() {
	float pJunkcode = 7415103213.48837;
	pJunkcode = 5325594775.82015;
	if (pJunkcode = 2769774191.02325)
		pJunkcode = 6906610789.80592;
	pJunkcode = 213903732.072928;
	pJunkcode = 5107843178.89319;
	if (pJunkcode = 5634530169.18071)
		pJunkcode = 8689649867.81129;
	pJunkcode = 8776148047.94951;
	if (pJunkcode = 4602204806.60908)
		pJunkcode = 5667384395.80658;
	pJunkcode = 2069189808.84869;
	pJunkcode = 8767054642.40256;
	if (pJunkcode = 8541445865.1517)
		pJunkcode = 833274692.165048;
	pJunkcode = 21954760.4341091;
	if (pJunkcode = 4311105687.34399)
		pJunkcode = 7531742220.4394;
	pJunkcode = 606268048.633228;
	pJunkcode = 5866145753.98712;
	if (pJunkcode = 6035940936.80161)
		pJunkcode = 1125908808.93226;
	pJunkcode = 9118935465.32268;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3231() {
	float pJunkcode = 1772339317.9728;
	pJunkcode = 9818242246.35428;
	if (pJunkcode = 4066870314.25098)
		pJunkcode = 7968238885.72996;
	pJunkcode = 9625164797.78018;
	pJunkcode = 5106686681.07016;
	if (pJunkcode = 7578613798.95602)
		pJunkcode = 6604577659.37335;
	pJunkcode = 5520380548.60743;
	if (pJunkcode = 5680073103.6028)
		pJunkcode = 4076245791.30194;
	pJunkcode = 8419856793.17645;
	pJunkcode = 9458039052.13624;
	if (pJunkcode = 2747062256.84829)
		pJunkcode = 4356816933.35522;
	pJunkcode = 4576686245.83533;
	if (pJunkcode = 7312229978.12024)
		pJunkcode = 4008062505.77045;
	pJunkcode = 2456975603.15381;
	pJunkcode = 4490157474.38476;
	if (pJunkcode = 2281339072.22606)
		pJunkcode = 9803963884.60892;
	pJunkcode = 8579099954.599;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3230() {
	float pJunkcode = 2694629879.02525;
	pJunkcode = 9440359424.93132;
	if (pJunkcode = 2754817439.74382)
		pJunkcode = 7542930978.37981;
	pJunkcode = 2294789643.16396;
	pJunkcode = 6541606836.37744;
	if (pJunkcode = 5157671723.58008)
		pJunkcode = 8991408661.29648;
	pJunkcode = 9423265617.05059;
	if (pJunkcode = 364667742.594871)
		pJunkcode = 8532571800.17577;
	pJunkcode = 7546961439.0787;
	pJunkcode = 3962982687.82209;
	if (pJunkcode = 1247000429.24212)
		pJunkcode = 6744140930.60978;
	pJunkcode = 2720100493.46403;
	if (pJunkcode = 721779309.729891)
		pJunkcode = 4130415913.52409;
	pJunkcode = 6151706504.62568;
	pJunkcode = 5798720475.9604;
	if (pJunkcode = 4774734781.50223)
		pJunkcode = 5804793935.27777;
	pJunkcode = 5587682106.32394;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3229() {
	float pJunkcode = 8866496994.22113;
	pJunkcode = 7255882128.84668;
	if (pJunkcode = 2229635882.41893)
		pJunkcode = 3460086155.32975;
	pJunkcode = 6243712480.70434;
	pJunkcode = 5088689827.61389;
	if (pJunkcode = 6273661843.48033)
		pJunkcode = 6611359787.54721;
	pJunkcode = 4392032388.34095;
	if (pJunkcode = 4503853928.32163)
		pJunkcode = 7470348762.71415;
	pJunkcode = 2874527624.46693;
	pJunkcode = 9946275712.73402;
	if (pJunkcode = 7726680719.2884)
		pJunkcode = 792997838.311281;
	pJunkcode = 5746686562.84449;
	if (pJunkcode = 708937349.679962)
		pJunkcode = 4963902218.4498;
	pJunkcode = 462648086.259895;
	pJunkcode = 6342545348.52875;
	if (pJunkcode = 8987913293.61671)
		pJunkcode = 6091614261.49113;
	pJunkcode = 2152756158.96155;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3228() {
	float pJunkcode = 6161952764.23252;
	pJunkcode = 9398842846.44493;
	if (pJunkcode = 5397495067.61791)
		pJunkcode = 6984736535.90185;
	pJunkcode = 3042956840.80794;
	pJunkcode = 716700737.445317;
	if (pJunkcode = 442202325.956783)
		pJunkcode = 301441809.613857;
	pJunkcode = 2544372558.79625;
	if (pJunkcode = 3074387890.3867)
		pJunkcode = 2210328701.4717;
	pJunkcode = 9777162919.44407;
	pJunkcode = 8705153008.93971;
	if (pJunkcode = 7395199966.44279)
		pJunkcode = 3439322230.47718;
	pJunkcode = 6903930570.35608;
	if (pJunkcode = 7071502100.73395)
		pJunkcode = 3300814912.48408;
	pJunkcode = 362907618.738099;
	pJunkcode = 7094368781.92041;
	if (pJunkcode = 6711680768.23588)
		pJunkcode = 4775439401.47431;
	pJunkcode = 9257569874.97998;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3227() {
	float pJunkcode = 142702819.281875;
	pJunkcode = 6595975735.77379;
	if (pJunkcode = 2945921745.17889)
		pJunkcode = 6158944487.35811;
	pJunkcode = 6824447167.42924;
	pJunkcode = 7231763343.54607;
	if (pJunkcode = 9064985279.71204)
		pJunkcode = 816653607.254333;
	pJunkcode = 9985731157.60446;
	if (pJunkcode = 7119502703.33239)
		pJunkcode = 3750047444.94613;
	pJunkcode = 5360890139.81198;
	pJunkcode = 7760443169.59153;
	if (pJunkcode = 6550363534.92789)
		pJunkcode = 4016213678.40811;
	pJunkcode = 2710600697.7341;
	if (pJunkcode = 8010364119.34171)
		pJunkcode = 3892814498.99335;
	pJunkcode = 3359229019.92482;
	pJunkcode = 4480946225.1143;
	if (pJunkcode = 5096327536.98292)
		pJunkcode = 9496583104.53604;
	pJunkcode = 9298327177.04726;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3226() {
	float pJunkcode = 3859191710.75516;
	pJunkcode = 8100107120.60957;
	if (pJunkcode = 6414767315.92696)
		pJunkcode = 159044344.906098;
	pJunkcode = 7137819739.95961;
	pJunkcode = 2391921916.67527;
	if (pJunkcode = 1008930335.9692)
		pJunkcode = 8213309858.69212;
	pJunkcode = 3862035082.31868;
	if (pJunkcode = 4011047367.10118)
		pJunkcode = 3748021766.42603;
	pJunkcode = 2754413156.65709;
	pJunkcode = 1324941160.64556;
	if (pJunkcode = 4354511823.6042)
		pJunkcode = 250337200.450879;
	pJunkcode = 369834961.932222;
	if (pJunkcode = 4412917365.29122)
		pJunkcode = 468968548.480426;
	pJunkcode = 5507536660.73905;
	pJunkcode = 2972539530.33838;
	if (pJunkcode = 844980305.68264)
		pJunkcode = 7489298431.12663;
	pJunkcode = 7632493058.07696;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3225() {
	float pJunkcode = 4387202707.19718;
	pJunkcode = 3755501201.43459;
	if (pJunkcode = 7021762086.68567)
		pJunkcode = 4391586596.62217;
	pJunkcode = 3306729764.49649;
	pJunkcode = 3191532744.79217;
	if (pJunkcode = 9746909904.30751)
		pJunkcode = 9642241693.01209;
	pJunkcode = 4431155477.77611;
	if (pJunkcode = 8158842414.7113)
		pJunkcode = 4675764617.14855;
	pJunkcode = 8748151383.65817;
	pJunkcode = 3968725400.54784;
	if (pJunkcode = 5381116692.25622)
		pJunkcode = 8506978005.58849;
	pJunkcode = 4139736282.44109;
	if (pJunkcode = 9446323040.50609)
		pJunkcode = 2262125152.98882;
	pJunkcode = 3975620499.34857;
	pJunkcode = 6905973720.31296;
	if (pJunkcode = 5097824793.01512)
		pJunkcode = 9116611831.75089;
	pJunkcode = 4366947330.6075;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3224() {
	float pJunkcode = 9631495532.38598;
	pJunkcode = 2148171813.6702;
	if (pJunkcode = 3422232424.91518)
		pJunkcode = 8985139516.76493;
	pJunkcode = 6057028241.55789;
	pJunkcode = 7218444213.5053;
	if (pJunkcode = 1973474966.18168)
		pJunkcode = 106450088.54026;
	pJunkcode = 5769784305.17936;
	if (pJunkcode = 6844825179.34916)
		pJunkcode = 3922586516.44157;
	pJunkcode = 7279608279.46117;
	pJunkcode = 7286759885.21689;
	if (pJunkcode = 6604242340.03362)
		pJunkcode = 4983534008.63426;
	pJunkcode = 301935068.041287;
	if (pJunkcode = 8182958410.72869)
		pJunkcode = 357029330.084333;
	pJunkcode = 8804888282.0618;
	pJunkcode = 1332201099.08813;
	if (pJunkcode = 7851764306.79178)
		pJunkcode = 6251637095.09321;
	pJunkcode = 2913110435.79638;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3223() {
	float pJunkcode = 6857022903.45059;
	pJunkcode = 5662654125.3686;
	if (pJunkcode = 3087114027.66288)
		pJunkcode = 4100543519.13257;
	pJunkcode = 7779240179.46251;
	pJunkcode = 8131487474.15095;
	if (pJunkcode = 4014968377.53733)
		pJunkcode = 1825630622.7339;
	pJunkcode = 7624629009.31776;
	if (pJunkcode = 3262573115.47496)
		pJunkcode = 215005661.923694;
	pJunkcode = 1708489321.28852;
	pJunkcode = 3888002154.24594;
	if (pJunkcode = 6222326452.56292)
		pJunkcode = 9395762045.87858;
	pJunkcode = 8757004196.06548;
	if (pJunkcode = 2156213065.73191)
		pJunkcode = 7937285064.34913;
	pJunkcode = 2072405156.2419;
	pJunkcode = 7694109986.74755;
	if (pJunkcode = 8070902359.08397)
		pJunkcode = 9630726954.47568;
	pJunkcode = 1602066497.18256;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3222() {
	float pJunkcode = 9280584106.4547;
	pJunkcode = 5502308526.52956;
	if (pJunkcode = 8543388531.06425)
		pJunkcode = 2439981543.57992;
	pJunkcode = 6062097563.5735;
	pJunkcode = 5546316526.31317;
	if (pJunkcode = 6734510625.29598)
		pJunkcode = 291518145.279315;
	pJunkcode = 973683588.857889;
	if (pJunkcode = 839948585.439959)
		pJunkcode = 3908928393.03025;
	pJunkcode = 7695573326.67906;
	pJunkcode = 1134432830.5058;
	if (pJunkcode = 857875646.280424)
		pJunkcode = 4511667844.48798;
	pJunkcode = 9392870497.48006;
	if (pJunkcode = 3981887608.96065)
		pJunkcode = 4440241758.77661;
	pJunkcode = 5249990574.34887;
	pJunkcode = 5140549745.80148;
	if (pJunkcode = 8303900403.9217)
		pJunkcode = 6287383920.1733;
	pJunkcode = 7155650312.08964;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3221() {
	float pJunkcode = 6191430146.76082;
	pJunkcode = 8707870129.34925;
	if (pJunkcode = 2018684131.90062)
		pJunkcode = 2075783595.21343;
	pJunkcode = 1182164283.57172;
	pJunkcode = 8019395313.1628;
	if (pJunkcode = 2739205572.44871)
		pJunkcode = 8335234594.593;
	pJunkcode = 421745641.349117;
	if (pJunkcode = 5484339612.47113)
		pJunkcode = 8680333715.41508;
	pJunkcode = 8189831125.41519;
	pJunkcode = 4668334800.18228;
	if (pJunkcode = 6806009076.37604)
		pJunkcode = 8150390332.43063;
	pJunkcode = 4150641103.92268;
	if (pJunkcode = 8604363598.63515)
		pJunkcode = 3287239161.40245;
	pJunkcode = 8637340639.86908;
	pJunkcode = 2097082701.55989;
	if (pJunkcode = 6304186392.31135)
		pJunkcode = 3157571163.41578;
	pJunkcode = 3841436074.41842;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3220() {
	float pJunkcode = 1322975202.58835;
	pJunkcode = 9299509244.07588;
	if (pJunkcode = 7584052805.7707)
		pJunkcode = 5348396877.41678;
	pJunkcode = 6977977371.58474;
	pJunkcode = 8869127223.10339;
	if (pJunkcode = 815768903.893481)
		pJunkcode = 6782014309.40588;
	pJunkcode = 3729586036.25195;
	if (pJunkcode = 5050519962.91278)
		pJunkcode = 4654425014.97321;
	pJunkcode = 6680135830.68669;
	pJunkcode = 8390146710.3884;
	if (pJunkcode = 3123391622.14029)
		pJunkcode = 3146178275.5782;
	pJunkcode = 4472114809.62917;
	if (pJunkcode = 5011043838.23138)
		pJunkcode = 6670827047.30665;
	pJunkcode = 6044830924.90602;
	pJunkcode = 9291433763.92353;
	if (pJunkcode = 5942966809.3656)
		pJunkcode = 897536822.906847;
	pJunkcode = 4676662314.65768;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3219() {
	float pJunkcode = 5112642161.62359;
	pJunkcode = 83191615.5061945;
	if (pJunkcode = 5466234766.25737)
		pJunkcode = 8787875363.39437;
	pJunkcode = 7057366562.86207;
	pJunkcode = 4628326949.17919;
	if (pJunkcode = 1334443992.45672)
		pJunkcode = 1029326159.25524;
	pJunkcode = 1067758877.81122;
	if (pJunkcode = 1867196439.75242)
		pJunkcode = 4755979755.23308;
	pJunkcode = 5219707980.70314;
	pJunkcode = 1171248835.80582;
	if (pJunkcode = 8250453334.68937)
		pJunkcode = 6336869448.21636;
	pJunkcode = 269728611.697744;
	if (pJunkcode = 5044113051.53877)
		pJunkcode = 4807279290.58993;
	pJunkcode = 4613387296.81123;
	pJunkcode = 1903532010.80863;
	if (pJunkcode = 6525217965.11529)
		pJunkcode = 4094233259.44882;
	pJunkcode = 7000738060.00893;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3218() {
	float pJunkcode = 6967978724.89125;
	pJunkcode = 974128595.945252;
	if (pJunkcode = 1079066748.38946)
		pJunkcode = 2517057869.59505;
	pJunkcode = 3149777240.21687;
	pJunkcode = 8153028393.65741;
	if (pJunkcode = 5290737031.27323)
		pJunkcode = 4835971799.55414;
	pJunkcode = 6078711130.45791;
	if (pJunkcode = 9315840853.54215)
		pJunkcode = 4991968405.27609;
	pJunkcode = 5307117883.9282;
	pJunkcode = 7317482519.72203;
	if (pJunkcode = 9817324255.52918)
		pJunkcode = 6236537063.5968;
	pJunkcode = 4319280026.39902;
	if (pJunkcode = 1265416180.41847)
		pJunkcode = 1980189085.20463;
	pJunkcode = 5520622904.25741;
	pJunkcode = 3457876561.06681;
	if (pJunkcode = 2814069780.34195)
		pJunkcode = 5102988999.55407;
	pJunkcode = 2154478855.66663;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3217() {
	float pJunkcode = 594699638.513573;
	pJunkcode = 1193884991.34899;
	if (pJunkcode = 8324487910.10318)
		pJunkcode = 8760277731.91125;
	pJunkcode = 7649322952.73399;
	pJunkcode = 5099295173.67489;
	if (pJunkcode = 1229258578.66993)
		pJunkcode = 234699540.434996;
	pJunkcode = 6606120251.73512;
	if (pJunkcode = 9503646181.98143)
		pJunkcode = 2327947824.82721;
	pJunkcode = 3927096130.82855;
	pJunkcode = 2491494957.35551;
	if (pJunkcode = 8551773584.40122)
		pJunkcode = 5950053493.27388;
	pJunkcode = 7897352288.83632;
	if (pJunkcode = 6746798079.10185)
		pJunkcode = 7080277402.2668;
	pJunkcode = 7563661878.86413;
	pJunkcode = 6721017311.73595;
	if (pJunkcode = 8320688827.19921)
		pJunkcode = 8356863707.13703;
	pJunkcode = 5720771553.65682;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3216() {
	float pJunkcode = 2131715082.26727;
	pJunkcode = 470528549.415327;
	if (pJunkcode = 1005544451.14815)
		pJunkcode = 3341683078.082;
	pJunkcode = 3716047134.90778;
	pJunkcode = 4809619196.53267;
	if (pJunkcode = 8580232054.34287)
		pJunkcode = 3969090875.81097;
	pJunkcode = 4237772539.82604;
	if (pJunkcode = 230853964.044734)
		pJunkcode = 7653163686.2953;
	pJunkcode = 8013240722.49067;
	pJunkcode = 9324842692.36925;
	if (pJunkcode = 7578411855.31672)
		pJunkcode = 6828831182.58152;
	pJunkcode = 1569056880.93904;
	if (pJunkcode = 4353546468.51492)
		pJunkcode = 5060458593.45143;
	pJunkcode = 2384501352.30625;
	pJunkcode = 4332703787.15641;
	if (pJunkcode = 7757850407.44178)
		pJunkcode = 8771073666.57461;
	pJunkcode = 5783965463.43135;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3215() {
	float pJunkcode = 7233650016.57707;
	pJunkcode = 6274694811.91238;
	if (pJunkcode = 9353534463.2677)
		pJunkcode = 1150344142.95381;
	pJunkcode = 6614532881.25775;
	pJunkcode = 5870280642.842;
	if (pJunkcode = 30790257.3996034)
		pJunkcode = 3366806859.82085;
	pJunkcode = 7311980134.71163;
	if (pJunkcode = 6600421599.43975)
		pJunkcode = 4027511458.20716;
	pJunkcode = 8144418800.69315;
	pJunkcode = 213413081.322629;
	if (pJunkcode = 9622127128.65924)
		pJunkcode = 8018557534.02062;
	pJunkcode = 3948802728.63592;
	if (pJunkcode = 7767929680.25259)
		pJunkcode = 6255752074.79885;
	pJunkcode = 5492443211.29912;
	pJunkcode = 9570385254.42752;
	if (pJunkcode = 3625252930.09421)
		pJunkcode = 5566407954.51632;
	pJunkcode = 266164599.245769;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3214() {
	float pJunkcode = 2009650204.67511;
	pJunkcode = 2835694438.88031;
	if (pJunkcode = 9272484570.48081)
		pJunkcode = 9377302455.45874;
	pJunkcode = 8050415746.72465;
	pJunkcode = 4311067431.32349;
	if (pJunkcode = 3404379764.36288)
		pJunkcode = 570747684.014031;
	pJunkcode = 1849329816.48605;
	if (pJunkcode = 2232085552.94265)
		pJunkcode = 6060074525.3265;
	pJunkcode = 4905667470.30177;
	pJunkcode = 2379888399.39979;
	if (pJunkcode = 5233608117.50682)
		pJunkcode = 3770117078.09069;
	pJunkcode = 4540579046.76049;
	if (pJunkcode = 84040239.7913038)
		pJunkcode = 6749058765.21031;
	pJunkcode = 6505913596.73411;
	pJunkcode = 5361908982.55014;
	if (pJunkcode = 7495451720.28848)
		pJunkcode = 1139168476.94958;
	pJunkcode = 4825325270.82073;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3213() {
	float pJunkcode = 4494023923.8335;
	pJunkcode = 6513771540.73489;
	if (pJunkcode = 7609312389.28103)
		pJunkcode = 3989788914.90687;
	pJunkcode = 9622906945.19232;
	pJunkcode = 1196063037.37521;
	if (pJunkcode = 8122857508.70909)
		pJunkcode = 8736617380.90696;
	pJunkcode = 3259121570.7407;
	if (pJunkcode = 55826851.9192155)
		pJunkcode = 9478007619.16568;
	pJunkcode = 5295918064.31453;
	pJunkcode = 3762132906.21876;
	if (pJunkcode = 9333173166.09254)
		pJunkcode = 2356861103.92829;
	pJunkcode = 2842509295.73045;
	if (pJunkcode = 2662521941.65506)
		pJunkcode = 5493565471.71838;
	pJunkcode = 8201881366.74856;
	pJunkcode = 7943897644.61548;
	if (pJunkcode = 6573252092.66286)
		pJunkcode = 958512647.169773;
	pJunkcode = 1378265836.6891;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3212() {
	float pJunkcode = 6663940726.13341;
	pJunkcode = 6392769964.4477;
	if (pJunkcode = 7939835952.32637)
		pJunkcode = 6006680047.35402;
	pJunkcode = 3383052334.45187;
	pJunkcode = 1299867392.75713;
	if (pJunkcode = 9636568374.76572)
		pJunkcode = 4737141963.15439;
	pJunkcode = 6842449066.2751;
	if (pJunkcode = 460928120.63095)
		pJunkcode = 8826323399.72521;
	pJunkcode = 806957094.880788;
	pJunkcode = 1718424899.44796;
	if (pJunkcode = 1197075423.78418)
		pJunkcode = 7867220158.16107;
	pJunkcode = 273550937.852439;
	if (pJunkcode = 5946832339.14249)
		pJunkcode = 8195752731.77702;
	pJunkcode = 2365948313.91377;
	pJunkcode = 2940906669.83247;
	if (pJunkcode = 2757926386.92521)
		pJunkcode = 4159592137.28309;
	pJunkcode = 1231221342.32266;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3211() {
	float pJunkcode = 9376601181.22955;
	pJunkcode = 9774751176.65291;
	if (pJunkcode = 4695782576.37681)
		pJunkcode = 2390907623.40239;
	pJunkcode = 684845989.22662;
	pJunkcode = 3794949922.22327;
	if (pJunkcode = 8256019179.38538)
		pJunkcode = 999709319.550909;
	pJunkcode = 5739366657.14103;
	if (pJunkcode = 6726250591.45121)
		pJunkcode = 7901834040.6588;
	pJunkcode = 857962263.070291;
	pJunkcode = 902053776.042329;
	if (pJunkcode = 5842003245.80946)
		pJunkcode = 8738131028.05256;
	pJunkcode = 3553312268.19486;
	if (pJunkcode = 3749155966.2813)
		pJunkcode = 9674662456.17847;
	pJunkcode = 744562249.556173;
	pJunkcode = 9958163323.48913;
	if (pJunkcode = 8585710185.62766)
		pJunkcode = 4415434041.47581;
	pJunkcode = 2370584652.09621;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3210() {
	float pJunkcode = 330891639.239324;
	pJunkcode = 2255273669.91639;
	if (pJunkcode = 2960001022.41666)
		pJunkcode = 8333753247.71958;
	pJunkcode = 9054739280.87291;
	pJunkcode = 7420903113.50493;
	if (pJunkcode = 3644386826.23947)
		pJunkcode = 6643690455.46869;
	pJunkcode = 5894007962.25463;
	if (pJunkcode = 9204242526.06738)
		pJunkcode = 2311049380.85758;
	pJunkcode = 5131102988.36178;
	pJunkcode = 698593816.731264;
	if (pJunkcode = 1584883960.75914)
		pJunkcode = 8913665212.30946;
	pJunkcode = 9172932128.12162;
	if (pJunkcode = 8778800765.71874)
		pJunkcode = 6581864870.71848;
	pJunkcode = 5935214166.627;
	pJunkcode = 4971770628.08335;
	if (pJunkcode = 3739577029.87871)
		pJunkcode = 266763228.783865;
	pJunkcode = 6880987437.66401;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3209() {
	float pJunkcode = 1026906996.22568;
	pJunkcode = 6622758214.37041;
	if (pJunkcode = 7981040296.4005)
		pJunkcode = 9644564141.48775;
	pJunkcode = 8452639501.92871;
	pJunkcode = 1128518085.59442;
	if (pJunkcode = 1805161155.85173)
		pJunkcode = 9416462464.47986;
	pJunkcode = 4981130615.06649;
	if (pJunkcode = 6249784841.07206)
		pJunkcode = 9796594670.52484;
	pJunkcode = 3817500904.51213;
	pJunkcode = 8866509527.36935;
	if (pJunkcode = 4118060341.138)
		pJunkcode = 8533875792.40828;
	pJunkcode = 4813789582.39251;
	if (pJunkcode = 8609223761.66197)
		pJunkcode = 2569887059.04418;
	pJunkcode = 1693198012.01795;
	pJunkcode = 6551555518.11446;
	if (pJunkcode = 9979475980.44354)
		pJunkcode = 7217696573.8454;
	pJunkcode = 7368571496.79191;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3208() {
	float pJunkcode = 2250897419.58843;
	pJunkcode = 8804043457.53479;
	if (pJunkcode = 8063683933.93003)
		pJunkcode = 508720132.341689;
	pJunkcode = 1937575133.3176;
	pJunkcode = 561575587.153073;
	if (pJunkcode = 2224698145.65401)
		pJunkcode = 9389576302.77786;
	pJunkcode = 1349028854.05294;
	if (pJunkcode = 801825991.87171)
		pJunkcode = 139403271.856711;
	pJunkcode = 2627777209.64412;
	pJunkcode = 4587314765.29855;
	if (pJunkcode = 8968521460.9506)
		pJunkcode = 2649739515.80536;
	pJunkcode = 1406396919.75172;
	if (pJunkcode = 5022942240.57891)
		pJunkcode = 7365644924.02992;
	pJunkcode = 3892004736.08235;
	pJunkcode = 5920703756.54118;
	if (pJunkcode = 3379129503.2983)
		pJunkcode = 6675232328.99726;
	pJunkcode = 6274430018.59765;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3207() {
	float pJunkcode = 8216136811.20284;
	pJunkcode = 7188627330.04073;
	if (pJunkcode = 9559242088.92783)
		pJunkcode = 3241335601.53104;
	pJunkcode = 836729510.46542;
	pJunkcode = 2296548295.54578;
	if (pJunkcode = 8414045263.38554)
		pJunkcode = 7456351611.00804;
	pJunkcode = 6061883205.52843;
	if (pJunkcode = 9685334409.76392)
		pJunkcode = 1483252319.635;
	pJunkcode = 4461695321.09917;
	pJunkcode = 2055592970.50654;
	if (pJunkcode = 4007239783.63542)
		pJunkcode = 3198220735.82157;
	pJunkcode = 4135652769.48227;
	if (pJunkcode = 9480143236.88799)
		pJunkcode = 2611959512.80512;
	pJunkcode = 6202754047.56857;
	pJunkcode = 1122210403.32011;
	if (pJunkcode = 9174109038.00306)
		pJunkcode = 6353825093.4465;
	pJunkcode = 6989730840.63426;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3206() {
	float pJunkcode = 5758942333.37321;
	pJunkcode = 2648901257.03684;
	if (pJunkcode = 6665470452.40247)
		pJunkcode = 2594271291.90709;
	pJunkcode = 7340177955.68256;
	pJunkcode = 8740152478.41045;
	if (pJunkcode = 4956497644.72948)
		pJunkcode = 8104204351.81274;
	pJunkcode = 6609538350.68188;
	if (pJunkcode = 9419488277.98298)
		pJunkcode = 26415024.531376;
	pJunkcode = 2777094988.45916;
	pJunkcode = 2512284355.95406;
	if (pJunkcode = 7594963364.14197)
		pJunkcode = 5545303078.20708;
	pJunkcode = 4379089695.1204;
	if (pJunkcode = 2833178931.49387)
		pJunkcode = 5165488491.89329;
	pJunkcode = 967310365.412822;
	pJunkcode = 5405793071.10434;
	if (pJunkcode = 5251177446.95392)
		pJunkcode = 9946420860.23408;
	pJunkcode = 348044410.890913;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3205() {
	float pJunkcode = 5156399354.08937;
	pJunkcode = 9095589516.36226;
	if (pJunkcode = 7948711514.38837)
		pJunkcode = 2645924961.45983;
	pJunkcode = 9930604825.78926;
	pJunkcode = 9050897494.92762;
	if (pJunkcode = 2987982438.78162)
		pJunkcode = 4101522105.85501;
	pJunkcode = 4329796078.14452;
	if (pJunkcode = 2886424750.91529)
		pJunkcode = 8084006162.51388;
	pJunkcode = 5055436153.7454;
	pJunkcode = 4062391456.48446;
	if (pJunkcode = 155554697.818037)
		pJunkcode = 7001269313.68913;
	pJunkcode = 62997640.399841;
	if (pJunkcode = 555901873.098566)
		pJunkcode = 4793796144.64243;
	pJunkcode = 378071373.016122;
	pJunkcode = 471596032.316841;
	if (pJunkcode = 1110849194.10311)
		pJunkcode = 85713437.1965493;
	pJunkcode = 6186285344.13367;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3204() {
	float pJunkcode = 3350966059.42813;
	pJunkcode = 2657700481.67909;
	if (pJunkcode = 1968888972.46586)
		pJunkcode = 9945953168.68395;
	pJunkcode = 9726401208.18248;
	pJunkcode = 4978633142.56135;
	if (pJunkcode = 4005547338.31334)
		pJunkcode = 3621957228.99281;
	pJunkcode = 1696024747.99834;
	if (pJunkcode = 6256606086.73117)
		pJunkcode = 6364599610.23313;
	pJunkcode = 9313692863.32317;
	pJunkcode = 8445327270.24652;
	if (pJunkcode = 6791424652.49706)
		pJunkcode = 3211431870.77656;
	pJunkcode = 3229627766.83132;
	if (pJunkcode = 2701224834.18814)
		pJunkcode = 2282614272.90909;
	pJunkcode = 1517399411.62819;
	pJunkcode = 116854410.633807;
	if (pJunkcode = 5211250296.5732)
		pJunkcode = 7280767669.01187;
	pJunkcode = 5356207261.57759;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3203() {
	float pJunkcode = 4610691880.65726;
	pJunkcode = 2599693162.55776;
	if (pJunkcode = 5474151774.14517)
		pJunkcode = 5794663072.36886;
	pJunkcode = 7089984422.18222;
	pJunkcode = 6011303356.46395;
	if (pJunkcode = 8956098228.65608)
		pJunkcode = 2171929347.21331;
	pJunkcode = 4212541729.23892;
	if (pJunkcode = 4033542628.91762)
		pJunkcode = 9319337830.06558;
	pJunkcode = 1415698431.90347;
	pJunkcode = 8720417691.96415;
	if (pJunkcode = 2579151758.71099)
		pJunkcode = 3158596339.08412;
	pJunkcode = 5445111398.72332;
	if (pJunkcode = 1947973418.17293)
		pJunkcode = 975708771.453676;
	pJunkcode = 7505431857.1309;
	pJunkcode = 6359293292.27536;
	if (pJunkcode = 3508987219.10942)
		pJunkcode = 8674974764.28834;
	pJunkcode = 2106821851.78282;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3202() {
	float pJunkcode = 5094206815.61622;
	pJunkcode = 9681912112.92263;
	if (pJunkcode = 807674527.395062)
		pJunkcode = 4641917242.7174;
	pJunkcode = 6215626200.79443;
	pJunkcode = 1901302509.72473;
	if (pJunkcode = 1598144648.56448)
		pJunkcode = 5010357881.77706;
	pJunkcode = 7832370883.90827;
	if (pJunkcode = 4203344279.70271)
		pJunkcode = 4214469112.30086;
	pJunkcode = 8529477446.67319;
	pJunkcode = 2031638159.20356;
	if (pJunkcode = 4981032433.6942)
		pJunkcode = 8904396263.52589;
	pJunkcode = 1589396799.81829;
	if (pJunkcode = 5562062619.13207)
		pJunkcode = 2826352964.07652;
	pJunkcode = 1187089161.22878;
	pJunkcode = 4448420786.99391;
	if (pJunkcode = 387400542.843448)
		pJunkcode = 9541068958.82574;
	pJunkcode = 8802616152.44164;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3201() {
	float pJunkcode = 732976982.666489;
	pJunkcode = 3583960991.35129;
	if (pJunkcode = 6471103051.14328)
		pJunkcode = 3483304702.9643;
	pJunkcode = 8242101273.12468;
	pJunkcode = 4878077836.47128;
	if (pJunkcode = 9261464182.33664)
		pJunkcode = 9843219360.22417;
	pJunkcode = 7625083343.50743;
	if (pJunkcode = 8671732373.8863)
		pJunkcode = 6306844362.68738;
	pJunkcode = 5235896130.67464;
	pJunkcode = 7016083848.777;
	if (pJunkcode = 9059673862.81416)
		pJunkcode = 5012859570.80298;
	pJunkcode = 1412371461.95933;
	if (pJunkcode = 9375054330.4379)
		pJunkcode = 8612235370.30958;
	pJunkcode = 3594893873.30144;
	pJunkcode = 2008585320.13759;
	if (pJunkcode = 8427146114.55241)
		pJunkcode = 2512806936.77952;
	pJunkcode = 4285710499.96624;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3200() {
	float pJunkcode = 3822111170.57983;
	pJunkcode = 350074985.790482;
	if (pJunkcode = 1061180644.37794)
		pJunkcode = 6018156271.43051;
	pJunkcode = 3875030641.53651;
	pJunkcode = 2463602062.31603;
	if (pJunkcode = 2666877419.04073)
		pJunkcode = 6370351561.58644;
	pJunkcode = 1991867311.12749;
	if (pJunkcode = 3447338451.16671)
		pJunkcode = 9202645731.27381;
	pJunkcode = 2487351204.72666;
	pJunkcode = 6271431797.57818;
	if (pJunkcode = 8812327274.79198)
		pJunkcode = 6916089952.29054;
	pJunkcode = 9262786614.57171;
	if (pJunkcode = 5180562048.48196)
		pJunkcode = 1873662584.97813;
	pJunkcode = 6878073558.70176;
	pJunkcode = 7172144761.67965;
	if (pJunkcode = 4704653912.60488)
		pJunkcode = 8221120701.5247;
	pJunkcode = 8002493081.07648;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3199() {
	float pJunkcode = 2134491445.84163;
	pJunkcode = 6218743115.36078;
	if (pJunkcode = 817692249.712428)
		pJunkcode = 7585784431.44167;
	pJunkcode = 4903527684.95261;
	pJunkcode = 2069002510.11043;
	if (pJunkcode = 7638104751.32483)
		pJunkcode = 822013147.436708;
	pJunkcode = 6298513614.22667;
	if (pJunkcode = 3947573658.227)
		pJunkcode = 6023966380.9687;
	pJunkcode = 3041720799.51927;
	pJunkcode = 569176085.900744;
	if (pJunkcode = 5264684590.51588)
		pJunkcode = 9080836820.47495;
	pJunkcode = 3950957841.07307;
	if (pJunkcode = 7147743632.4659)
		pJunkcode = 5387322290.95567;
	pJunkcode = 2420695261.30228;
	pJunkcode = 776499272.316656;
	if (pJunkcode = 3852784736.51751)
		pJunkcode = 8047665792.86609;
	pJunkcode = 790514125.959579;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3198() {
	float pJunkcode = 3755747612.95383;
	pJunkcode = 4530889246.82728;
	if (pJunkcode = 436906897.379316)
		pJunkcode = 3417526790.04125;
	pJunkcode = 5920277827.40597;
	pJunkcode = 5713515563.98249;
	if (pJunkcode = 8232528767.44016)
		pJunkcode = 2526391123.23102;
	pJunkcode = 9611209513.26173;
	if (pJunkcode = 5649724293.58419)
		pJunkcode = 9566130521.85389;
	pJunkcode = 3265037712.62197;
	pJunkcode = 5683044460.89125;
	if (pJunkcode = 8217642658.59988)
		pJunkcode = 7579956049.5373;
	pJunkcode = 9181458922.6804;
	if (pJunkcode = 3431662188.21615)
		pJunkcode = 263652169.849655;
	pJunkcode = 271715736.177108;
	pJunkcode = 6566155131.71274;
	if (pJunkcode = 2468057071.47822)
		pJunkcode = 2726360976.78225;
	pJunkcode = 3128107645.81366;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3197() {
	float pJunkcode = 2844299927.08138;
	pJunkcode = 5527449325.68998;
	if (pJunkcode = 4039148666.76571)
		pJunkcode = 8081594119.3742;
	pJunkcode = 5431519256.52828;
	pJunkcode = 2726573467.54423;
	if (pJunkcode = 7870604694.90618)
		pJunkcode = 9722278233.15249;
	pJunkcode = 9536090877.57035;
	if (pJunkcode = 7154537769.92426)
		pJunkcode = 4001241781.64676;
	pJunkcode = 7828694932.25885;
	pJunkcode = 3666107980.47312;
	if (pJunkcode = 2008676323.41292)
		pJunkcode = 8088922472.08893;
	pJunkcode = 5098794094.54435;
	if (pJunkcode = 4900187008.40555)
		pJunkcode = 5650395141.80913;
	pJunkcode = 8780005605.68488;
	pJunkcode = 5565498047.49034;
	if (pJunkcode = 6033527121.83845)
		pJunkcode = 5702999775.33298;
	pJunkcode = 6821293237.81733;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3196() {
	float pJunkcode = 7972264415.12514;
	pJunkcode = 2362347174.65863;
	if (pJunkcode = 2989732746.59662)
		pJunkcode = 9777151822.84656;
	pJunkcode = 2612970793.2796;
	pJunkcode = 2348730336.36419;
	if (pJunkcode = 7379204239.76077)
		pJunkcode = 6163918154.18865;
	pJunkcode = 4565633706.42909;
	if (pJunkcode = 1150495935.37399)
		pJunkcode = 3359994228.8094;
	pJunkcode = 226851603.395215;
	pJunkcode = 5862421844.36131;
	if (pJunkcode = 7291433550.43485)
		pJunkcode = 9879624158.12338;
	pJunkcode = 703429071.784909;
	if (pJunkcode = 3920352312.2257)
		pJunkcode = 8226940943.75117;
	pJunkcode = 9608592783.7652;
	pJunkcode = 432954937.075215;
	if (pJunkcode = 4940668320.44442)
		pJunkcode = 8131598204.96943;
	pJunkcode = 1177962125.95997;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3195() {
	float pJunkcode = 8209589949.05677;
	pJunkcode = 6257895087.14845;
	if (pJunkcode = 8081370585.56522)
		pJunkcode = 5367919133.52029;
	pJunkcode = 3178968374.30118;
	pJunkcode = 4832814994.51253;
	if (pJunkcode = 7204982850.97498)
		pJunkcode = 6926065660.73841;
	pJunkcode = 7005732737.26664;
	if (pJunkcode = 4623315578.13127)
		pJunkcode = 9502615492.93859;
	pJunkcode = 3202920226.35627;
	pJunkcode = 7852855084.16624;
	if (pJunkcode = 9312589734.40658)
		pJunkcode = 8292960735.02617;
	pJunkcode = 3774479705.63392;
	if (pJunkcode = 2319655211.31627)
		pJunkcode = 6913706030.15479;
	pJunkcode = 5973747106.13131;
	pJunkcode = 1526840222.63271;
	if (pJunkcode = 8473189934.85754)
		pJunkcode = 4946655284.04264;
	pJunkcode = 4543610094.24414;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3194() {
	float pJunkcode = 5770597346.31303;
	pJunkcode = 4759338934.64636;
	if (pJunkcode = 2729414771.5923)
		pJunkcode = 419429944.894602;
	pJunkcode = 705779471.299823;
	pJunkcode = 3093370999.93011;
	if (pJunkcode = 2156900570.49453)
		pJunkcode = 8389774052.37174;
	pJunkcode = 6444547147.09343;
	if (pJunkcode = 3130897946.49098)
		pJunkcode = 1274198846.31093;
	pJunkcode = 671483185.250461;
	pJunkcode = 4427801387.9898;
	if (pJunkcode = 2296098680.78606)
		pJunkcode = 6812572124.74659;
	pJunkcode = 8865163120.80264;
	if (pJunkcode = 3226674241.86354)
		pJunkcode = 1346926849.75451;
	pJunkcode = 6066564137.31811;
	pJunkcode = 3525712664.65591;
	if (pJunkcode = 9878523008.24747)
		pJunkcode = 9058814493.5023;
	pJunkcode = 8147142805.83596;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3193() {
	float pJunkcode = 7520609685.88058;
	pJunkcode = 9397914050.05772;
	if (pJunkcode = 9285048112.10299)
		pJunkcode = 8714024480.20821;
	pJunkcode = 5825704835.33267;
	pJunkcode = 7964106760.54718;
	if (pJunkcode = 7665929683.77907)
		pJunkcode = 5890682691.95719;
	pJunkcode = 7194048981.83278;
	if (pJunkcode = 393501110.361571)
		pJunkcode = 4082372202.76307;
	pJunkcode = 4789430928.04832;
	pJunkcode = 6000784610.89483;
	if (pJunkcode = 5355144311.84337)
		pJunkcode = 9438357702.11659;
	pJunkcode = 1970199297.62854;
	if (pJunkcode = 8400135322.1815)
		pJunkcode = 7301090039.62099;
	pJunkcode = 1186311724.20108;
	pJunkcode = 2516704398.95986;
	if (pJunkcode = 7446135889.16304)
		pJunkcode = 7558216997.79105;
	pJunkcode = 8795649824.4744;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3192() {
	float pJunkcode = 1414676092.08402;
	pJunkcode = 658888551.735051;
	if (pJunkcode = 6186068408.67915)
		pJunkcode = 4922206585.67579;
	pJunkcode = 4553554224.1876;
	pJunkcode = 697953457.374212;
	if (pJunkcode = 1211128775.58904)
		pJunkcode = 946035200.802032;
	pJunkcode = 9631533318.09328;
	if (pJunkcode = 4458008601.14017)
		pJunkcode = 541186648.600316;
	pJunkcode = 1373350870.53342;
	pJunkcode = 5740259055.47771;
	if (pJunkcode = 7398361814.54571)
		pJunkcode = 7515732929.17973;
	pJunkcode = 9180177782.54704;
	if (pJunkcode = 6845869230.40902)
		pJunkcode = 8659856823.63792;
	pJunkcode = 9283629193.48576;
	pJunkcode = 5861468279.69792;
	if (pJunkcode = 6173448226.9127)
		pJunkcode = 4875480864.78559;
	pJunkcode = 9821455982.57555;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3191() {
	float pJunkcode = 483639204.982556;
	pJunkcode = 3296118367.22758;
	if (pJunkcode = 3392310412.48139)
		pJunkcode = 2916954491.81774;
	pJunkcode = 8668454404.9894;
	pJunkcode = 9960655946.18046;
	if (pJunkcode = 3486986724.4479)
		pJunkcode = 6343640572.92404;
	pJunkcode = 133689960.690616;
	if (pJunkcode = 2559360576.59015)
		pJunkcode = 834081062.767493;
	pJunkcode = 4353839375.84873;
	pJunkcode = 1795394261.80674;
	if (pJunkcode = 9999608637.88477)
		pJunkcode = 1484703822.57497;
	pJunkcode = 3086313445.17236;
	if (pJunkcode = 9494648811.12971)
		pJunkcode = 7171162023.62835;
	pJunkcode = 1615999774.76864;
	pJunkcode = 8990294726.81778;
	if (pJunkcode = 7456042079.73251)
		pJunkcode = 1666519251.45096;
	pJunkcode = 3060567578.77093;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3190() {
	float pJunkcode = 1120117199.932;
	pJunkcode = 2338012496.97436;
	if (pJunkcode = 4771788111.00006)
		pJunkcode = 8260265970.18518;
	pJunkcode = 8975108721.13766;
	pJunkcode = 50577253.2673366;
	if (pJunkcode = 25111795.9757469)
		pJunkcode = 5125344499.18938;
	pJunkcode = 685136428.580288;
	if (pJunkcode = 1047424071.84177)
		pJunkcode = 5587470982.26417;
	pJunkcode = 5587136456.869;
	pJunkcode = 7328792897.86122;
	if (pJunkcode = 1166846981.34802)
		pJunkcode = 5776750842.69208;
	pJunkcode = 8450718526.30201;
	if (pJunkcode = 9606428078.80146)
		pJunkcode = 2713584720.8829;
	pJunkcode = 3720846140.92101;
	pJunkcode = 5368341525.99339;
	if (pJunkcode = 5914133781.52824)
		pJunkcode = 3062826690.95997;
	pJunkcode = 4361807938.9104;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3189() {
	float pJunkcode = 6340775029.12005;
	pJunkcode = 8389149625.45631;
	if (pJunkcode = 7968216299.05509)
		pJunkcode = 6009701707.2;
	pJunkcode = 4275872292.93656;
	pJunkcode = 3732484667.28782;
	if (pJunkcode = 1815894697.18314)
		pJunkcode = 2305025271.82992;
	pJunkcode = 4711271207.64462;
	if (pJunkcode = 7385971741.65986)
		pJunkcode = 8412898297.55374;
	pJunkcode = 1541481249.40344;
	pJunkcode = 6833773329.11473;
	if (pJunkcode = 8657222364.26518)
		pJunkcode = 3563599358.90335;
	pJunkcode = 6232491180.3503;
	if (pJunkcode = 4247743133.03582)
		pJunkcode = 2907369827.8386;
	pJunkcode = 4171678120.09114;
	pJunkcode = 4460149075.66113;
	if (pJunkcode = 3924957914.74224)
		pJunkcode = 9591457667.48476;
	pJunkcode = 802332821.861641;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3188() {
	float pJunkcode = 581724172.379354;
	pJunkcode = 4311851212.92279;
	if (pJunkcode = 629200319.884392)
		pJunkcode = 1076972860.4861;
	pJunkcode = 9991444948.39688;
	pJunkcode = 7513854137.38721;
	if (pJunkcode = 4848340279.94202)
		pJunkcode = 2088565981.48232;
	pJunkcode = 338522417.309402;
	if (pJunkcode = 8069441750.22859)
		pJunkcode = 3552196142.80171;
	pJunkcode = 5092639531.81162;
	pJunkcode = 1575834315.29406;
	if (pJunkcode = 9119691363.40293)
		pJunkcode = 4329468798.36652;
	pJunkcode = 6584277295.69659;
	if (pJunkcode = 1867203268.60691)
		pJunkcode = 68261574.2910943;
	pJunkcode = 3986685939.77177;
	pJunkcode = 6235525887.89507;
	if (pJunkcode = 3702917918.26725)
		pJunkcode = 9371616438.75605;
	pJunkcode = 7774070939.17538;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3187() {
	float pJunkcode = 4038120541.16262;
	pJunkcode = 7375531517.44009;
	if (pJunkcode = 228068082.448837)
		pJunkcode = 1197781328.96856;
	pJunkcode = 4199895773.37796;
	pJunkcode = 5412824297.6362;
	if (pJunkcode = 1422741127.55814)
		pJunkcode = 437016610.738063;
	pJunkcode = 5159335384.33753;
	if (pJunkcode = 4645414319.357)
		pJunkcode = 7573261949.0929;
	pJunkcode = 5890100919.18913;
	pJunkcode = 3556899130.4177;
	if (pJunkcode = 9095689290.28023)
		pJunkcode = 8556642559.97978;
	pJunkcode = 2854236818.39252;
	if (pJunkcode = 4097558257.33713)
		pJunkcode = 1774439522.50835;
	pJunkcode = 7566015430.87443;
	pJunkcode = 5076826696.42785;
	if (pJunkcode = 3603828226.84133)
		pJunkcode = 4931934646.28606;
	pJunkcode = 1731786867.57047;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3186() {
	float pJunkcode = 4743944070.32598;
	pJunkcode = 9504722989.65697;
	if (pJunkcode = 2774423926.14696)
		pJunkcode = 8067713172.28026;
	pJunkcode = 1960074385.81136;
	pJunkcode = 5382409674.03769;
	if (pJunkcode = 5797506213.43234)
		pJunkcode = 793773771.268063;
	pJunkcode = 1630406753.20366;
	if (pJunkcode = 5543706357.91857)
		pJunkcode = 5642452613.60968;
	pJunkcode = 5469547834.0228;
	pJunkcode = 1531359194.38969;
	if (pJunkcode = 9421644447.46069)
		pJunkcode = 3155983984.43163;
	pJunkcode = 6125866645.43335;
	if (pJunkcode = 4338744033.5146)
		pJunkcode = 6191967023.24485;
	pJunkcode = 5303776473.97194;
	pJunkcode = 3702822409.76402;
	if (pJunkcode = 7986437172.93152)
		pJunkcode = 8701659298.99755;
	pJunkcode = 4813238363.68426;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3185() {
	float pJunkcode = 4417355424.55206;
	pJunkcode = 2221203686.00468;
	if (pJunkcode = 3580055117.08392)
		pJunkcode = 6746153521.85028;
	pJunkcode = 3035321371.24731;
	pJunkcode = 6567220242.09372;
	if (pJunkcode = 7585470252.91298)
		pJunkcode = 5002980285.87363;
	pJunkcode = 2586787803.35756;
	if (pJunkcode = 4338060014.19186)
		pJunkcode = 8845975625.2509;
	pJunkcode = 4747040048.97021;
	pJunkcode = 8111090484.39505;
	if (pJunkcode = 8636885209.05804)
		pJunkcode = 7904770867.17981;
	pJunkcode = 8313479950.33448;
	if (pJunkcode = 6530943769.84879)
		pJunkcode = 2738700041.64078;
	pJunkcode = 7763863336.05374;
	pJunkcode = 9064670393.38526;
	if (pJunkcode = 8814827906.77735)
		pJunkcode = 3292879688.10889;
	pJunkcode = 3676698027.38551;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3184() {
	float pJunkcode = 708364820.196014;
	pJunkcode = 5061386502.76276;
	if (pJunkcode = 6263590775.53936)
		pJunkcode = 8378558575.05387;
	pJunkcode = 6857014640.37368;
	pJunkcode = 8844699984.52236;
	if (pJunkcode = 9725355569.23351)
		pJunkcode = 5129218818.23517;
	pJunkcode = 2151927632.41662;
	if (pJunkcode = 6811739204.73135)
		pJunkcode = 2156008104.63765;
	pJunkcode = 9285645717.1965;
	pJunkcode = 6618957371.28437;
	if (pJunkcode = 6033415885.78252)
		pJunkcode = 6057078993.47407;
	pJunkcode = 2275109026.5535;
	if (pJunkcode = 7138425219.30674)
		pJunkcode = 2789476691.16707;
	pJunkcode = 2605352788.1709;
	pJunkcode = 3738081787.81144;
	if (pJunkcode = 8387706995.60345)
		pJunkcode = 4148165952.66813;
	pJunkcode = 5156517663.46952;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3183() {
	float pJunkcode = 9695264639.85695;
	pJunkcode = 1245294966.48893;
	if (pJunkcode = 71689025.9512822)
		pJunkcode = 384347690.988068;
	pJunkcode = 5310639889.49552;
	pJunkcode = 7367219700.24971;
	if (pJunkcode = 6316955775.11883)
		pJunkcode = 9048618257.05901;
	pJunkcode = 7111058155.37601;
	if (pJunkcode = 3326442162.17199)
		pJunkcode = 8276288905.18413;
	pJunkcode = 6234456118.6195;
	pJunkcode = 2863081940.48057;
	if (pJunkcode = 3447862547.7045)
		pJunkcode = 3565238114.01591;
	pJunkcode = 8766936228.24967;
	if (pJunkcode = 6826828013.90295)
		pJunkcode = 163520827.82381;
	pJunkcode = 1817189932.74271;
	pJunkcode = 1690751335.66637;
	if (pJunkcode = 4781595091.09609)
		pJunkcode = 6377070933.21618;
	pJunkcode = 5991572407.02828;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3182() {
	float pJunkcode = 5239433316.64812;
	pJunkcode = 2721727302.15573;
	if (pJunkcode = 5045257123.31461)
		pJunkcode = 2189053127.08308;
	pJunkcode = 8961852587.45416;
	pJunkcode = 898732193.745402;
	if (pJunkcode = 7324302445.24459)
		pJunkcode = 5884605053.37156;
	pJunkcode = 3973702794.95971;
	if (pJunkcode = 2183879747.1085)
		pJunkcode = 9826938803.0226;
	pJunkcode = 2798726764.55911;
	pJunkcode = 1562005845.37412;
	if (pJunkcode = 3083136181.50669)
		pJunkcode = 2892193291.28606;
	pJunkcode = 1920707245.73608;
	if (pJunkcode = 3198761491.22533)
		pJunkcode = 871485066.66474;
	pJunkcode = 9236428523.78685;
	pJunkcode = 5496889366.00048;
	if (pJunkcode = 5259091937.96764)
		pJunkcode = 122130828.779328;
	pJunkcode = 6582896537.64024;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3181() {
	float pJunkcode = 6123382527.96038;
	pJunkcode = 1147620000.44229;
	if (pJunkcode = 1680195770.62747)
		pJunkcode = 5011439837.28665;
	pJunkcode = 9217569639.79108;
	pJunkcode = 5579894462.5047;
	if (pJunkcode = 3456391295.69284)
		pJunkcode = 1200106766.95979;
	pJunkcode = 7538358321.06988;
	if (pJunkcode = 5048020499.72555)
		pJunkcode = 9077754010.60023;
	pJunkcode = 6957142184.9697;
	pJunkcode = 9690044032.98689;
	if (pJunkcode = 3839588486.27671)
		pJunkcode = 2740965426.22463;
	pJunkcode = 1513493560.64366;
	if (pJunkcode = 3816396653.36306)
		pJunkcode = 6302289481.81933;
	pJunkcode = 7711096609.78504;
	pJunkcode = 5190170027.8886;
	if (pJunkcode = 7616194961.9184)
		pJunkcode = 3009607849.35544;
	pJunkcode = 1973265087.7534;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3180() {
	float pJunkcode = 56179624.069462;
	pJunkcode = 3475872187.51785;
	if (pJunkcode = 8546399334.12589)
		pJunkcode = 3651697269.8002;
	pJunkcode = 2368191572.82282;
	pJunkcode = 9498313000.29264;
	if (pJunkcode = 8536474619.73564)
		pJunkcode = 2931171158.71611;
	pJunkcode = 1630492608.64494;
	if (pJunkcode = 4598305891.58006)
		pJunkcode = 813861257.687837;
	pJunkcode = 3040405112.32048;
	pJunkcode = 5029579386.56081;
	if (pJunkcode = 7521685126.85391)
		pJunkcode = 1772557859.9427;
	pJunkcode = 5249893245.12822;
	if (pJunkcode = 1326312703.35719)
		pJunkcode = 327613237.281757;
	pJunkcode = 8734640207.50979;
	pJunkcode = 9726778348.14421;
	if (pJunkcode = 9353502635.637)
		pJunkcode = 3314423536.59556;
	pJunkcode = 9555862672.07737;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3179() {
	float pJunkcode = 5693376252.30801;
	pJunkcode = 4390249396.08705;
	if (pJunkcode = 4433920754.0661)
		pJunkcode = 1074782900.635;
	pJunkcode = 2040829611.08238;
	pJunkcode = 1538338400.09105;
	if (pJunkcode = 2979694111.63997)
		pJunkcode = 7528552346.79217;
	pJunkcode = 757729703.590925;
	if (pJunkcode = 2937224892.8186)
		pJunkcode = 4902874813.4019;
	pJunkcode = 43938081.6790907;
	pJunkcode = 4411565252.10251;
	if (pJunkcode = 373819040.902422)
		pJunkcode = 2229759432.76446;
	pJunkcode = 2526123438.18861;
	if (pJunkcode = 6379213435.67177)
		pJunkcode = 6099513334.6138;
	pJunkcode = 9896771220.82467;
	pJunkcode = 981999473.558942;
	if (pJunkcode = 31400956.757566)
		pJunkcode = 7981825926.65661;
	pJunkcode = 1775345504.04894;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3178() {
	float pJunkcode = 7923218038.53731;
	pJunkcode = 2881181706.38049;
	if (pJunkcode = 2026919066.853)
		pJunkcode = 2694052821.03057;
	pJunkcode = 4976351536.82877;
	pJunkcode = 4050682960.55232;
	if (pJunkcode = 1512132599.97896)
		pJunkcode = 8006136578.78702;
	pJunkcode = 2952176892.07212;
	if (pJunkcode = 2985130708.68159)
		pJunkcode = 7847221989.33963;
	pJunkcode = 3274852674.07265;
	pJunkcode = 9057656666.22959;
	if (pJunkcode = 7998564950.75045)
		pJunkcode = 4582639800.89859;
	pJunkcode = 8033381282.12492;
	if (pJunkcode = 5570486524.53001)
		pJunkcode = 6207857902.67991;
	pJunkcode = 3609650848.57319;
	pJunkcode = 7494180640.66663;
	if (pJunkcode = 5580810446.34971)
		pJunkcode = 5135135327.98518;
	pJunkcode = 3109368065.37494;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3177() {
	float pJunkcode = 4159497266.72944;
	pJunkcode = 3339058870.13998;
	if (pJunkcode = 795418896.56333)
		pJunkcode = 9235150242.78521;
	pJunkcode = 3757974597.16364;
	pJunkcode = 6305748507.32736;
	if (pJunkcode = 8245968532.81595)
		pJunkcode = 1528973591.2335;
	pJunkcode = 637357424.987526;
	if (pJunkcode = 5759062128.19801)
		pJunkcode = 1899814282.77459;
	pJunkcode = 9573338984.28422;
	pJunkcode = 632095947.434999;
	if (pJunkcode = 5190624506.43183)
		pJunkcode = 5002859063.81125;
	pJunkcode = 4027322799.75154;
	if (pJunkcode = 2901255865.70749)
		pJunkcode = 5070215620.23242;
	pJunkcode = 7476185484.52878;
	pJunkcode = 3744836896.2593;
	if (pJunkcode = 2933069619.84471)
		pJunkcode = 1582125892.83379;
	pJunkcode = 1553050579.19494;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3176() {
	float pJunkcode = 806711742.730383;
	pJunkcode = 928114149.260385;
	if (pJunkcode = 3107327849.05484)
		pJunkcode = 3327008158.49341;
	pJunkcode = 1629650142.66877;
	pJunkcode = 7930771233.55616;
	if (pJunkcode = 1055775730.61348)
		pJunkcode = 5243222678.96993;
	pJunkcode = 2956661228.28891;
	if (pJunkcode = 7180339361.45468)
		pJunkcode = 1918812032.56247;
	pJunkcode = 3113857777.59753;
	pJunkcode = 4906080770.82915;
	if (pJunkcode = 390466898.741116)
		pJunkcode = 9523825821.84841;
	pJunkcode = 7244095018.43387;
	if (pJunkcode = 5300110613.10171)
		pJunkcode = 5496068206.14344;
	pJunkcode = 3198358630.71961;
	pJunkcode = 7282640598.18742;
	if (pJunkcode = 1671072921.61177)
		pJunkcode = 1941656418.10945;
	pJunkcode = 5507435168.54811;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3175() {
	float pJunkcode = 7774983786.82299;
	pJunkcode = 5212628379.43221;
	if (pJunkcode = 4628692925.0232)
		pJunkcode = 4777021011.20516;
	pJunkcode = 578087780.588994;
	pJunkcode = 3641155043.49548;
	if (pJunkcode = 910409167.051926)
		pJunkcode = 6350288594.31655;
	pJunkcode = 1436923752.62852;
	if (pJunkcode = 5694290340.93054)
		pJunkcode = 8735343665.66858;
	pJunkcode = 6733091061.74432;
	pJunkcode = 4551749229.6579;
	if (pJunkcode = 5370696942.9585)
		pJunkcode = 4460353303.3462;
	pJunkcode = 8890494366.54015;
	if (pJunkcode = 2648284597.27401)
		pJunkcode = 2522928684.45989;
	pJunkcode = 7943569442.83735;
	pJunkcode = 6347194366.54125;
	if (pJunkcode = 8911576400.06338)
		pJunkcode = 6499920381.69562;
	pJunkcode = 8271676722.93939;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3174() {
	float pJunkcode = 5192365438.25057;
	pJunkcode = 8910037900.82339;
	if (pJunkcode = 854016982.610557)
		pJunkcode = 8057140615.43091;
	pJunkcode = 1148382052.17491;
	pJunkcode = 4881840567.89039;
	if (pJunkcode = 8861083964.0616)
		pJunkcode = 1250399291.91347;
	pJunkcode = 4016768480.34413;
	if (pJunkcode = 7814227672.24471)
		pJunkcode = 513703016.845232;
	pJunkcode = 1709306702.342;
	pJunkcode = 2882759441.84817;
	if (pJunkcode = 9966348479.7477)
		pJunkcode = 4719714053.62601;
	pJunkcode = 4200010220.13636;
	if (pJunkcode = 2607343246.75174)
		pJunkcode = 3731412916.64329;
	pJunkcode = 3861889580.91958;
	pJunkcode = 5790401650.63773;
	if (pJunkcode = 218666675.308399)
		pJunkcode = 7047947116.18376;
	pJunkcode = 7064026449.18552;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3173() {
	float pJunkcode = 7358361744.66492;
	pJunkcode = 3443860424.53987;
	if (pJunkcode = 7589830761.71854)
		pJunkcode = 2448571287.93031;
	pJunkcode = 3526140272.48327;
	pJunkcode = 4680450780.59048;
	if (pJunkcode = 8927673127.66858)
		pJunkcode = 6639268859.49803;
	pJunkcode = 6087866030.6195;
	if (pJunkcode = 4056679232.62659)
		pJunkcode = 6748091927.00466;
	pJunkcode = 3502305260.66166;
	pJunkcode = 1272377926.32446;
	if (pJunkcode = 5721950789.49313)
		pJunkcode = 1629241293.81878;
	pJunkcode = 2310077362.22191;
	if (pJunkcode = 468083754.316531)
		pJunkcode = 2947414752.09434;
	pJunkcode = 2096508074.29282;
	pJunkcode = 9101213745.08663;
	if (pJunkcode = 538152728.015256)
		pJunkcode = 2028494134.10819;
	pJunkcode = 5155341554.51749;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3172() {
	float pJunkcode = 5423780804.6755;
	pJunkcode = 4764027477.59367;
	if (pJunkcode = 9585740416.86358)
		pJunkcode = 3626694745.04797;
	pJunkcode = 8817599073.62372;
	pJunkcode = 6200756173.40589;
	if (pJunkcode = 6223628221.28501)
		pJunkcode = 8313519091.10257;
	pJunkcode = 988754044.173395;
	if (pJunkcode = 1553933487.44948)
		pJunkcode = 6583599315.94035;
	pJunkcode = 7983345499.28284;
	pJunkcode = 2574599387.73218;
	if (pJunkcode = 3613770817.78963)
		pJunkcode = 7576123113.39627;
	pJunkcode = 1545788240.29244;
	if (pJunkcode = 2986962350.41198)
		pJunkcode = 974246244.092985;
	pJunkcode = 3635720697.90368;
	pJunkcode = 9466080883.94046;
	if (pJunkcode = 3874385136.67369)
		pJunkcode = 3392466461.00317;
	pJunkcode = 3794030017.54943;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3171() {
	float pJunkcode = 6253403193.32455;
	pJunkcode = 4736313702.03374;
	if (pJunkcode = 9200528392.90253)
		pJunkcode = 1130842187.15111;
	pJunkcode = 7365970199.15612;
	pJunkcode = 3048026239.65689;
	if (pJunkcode = 1399159730.96385)
		pJunkcode = 9244272211.70988;
	pJunkcode = 6251115967.11643;
	if (pJunkcode = 4470725513.7585)
		pJunkcode = 6565061038.86306;
	pJunkcode = 6525488816.62279;
	pJunkcode = 8086654037.45415;
	if (pJunkcode = 6614154290.77562)
		pJunkcode = 9791022525.49461;
	pJunkcode = 4514496119.46353;
	if (pJunkcode = 4981526418.54598)
		pJunkcode = 4012621544.95221;
	pJunkcode = 6786800151.30616;
	pJunkcode = 7608217409.35547;
	if (pJunkcode = 3990835394.36606)
		pJunkcode = 3857363740.72731;
	pJunkcode = 7674768731.37637;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3170() {
	float pJunkcode = 4322164778.53902;
	pJunkcode = 5241939191.92716;
	if (pJunkcode = 2069863842.16499)
		pJunkcode = 3695455933.74093;
	pJunkcode = 5432402276.54041;
	pJunkcode = 969329566.649188;
	if (pJunkcode = 9758743526.83286)
		pJunkcode = 4393066639.65702;
	pJunkcode = 7949112875.47974;
	if (pJunkcode = 3093038700.37363)
		pJunkcode = 8880024491.63442;
	pJunkcode = 5480930295.80842;
	pJunkcode = 291641183.402548;
	if (pJunkcode = 2352155077.90278)
		pJunkcode = 2771350450.35941;
	pJunkcode = 9759521292.14849;
	if (pJunkcode = 9591844565.28119)
		pJunkcode = 3729207138.43644;
	pJunkcode = 5902822660.66672;
	pJunkcode = 1190612123.72048;
	if (pJunkcode = 1859006058.82646)
		pJunkcode = 2919480311.17027;
	pJunkcode = 9601189895.72188;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3169() {
	float pJunkcode = 1552377123.29286;
	pJunkcode = 5160612966.85423;
	if (pJunkcode = 6985977885.67612)
		pJunkcode = 851894081.970829;
	pJunkcode = 1974020534.14672;
	pJunkcode = 7383603386.42527;
	if (pJunkcode = 2602840170.18385)
		pJunkcode = 1135179368.88777;
	pJunkcode = 8620768779.52079;
	if (pJunkcode = 2397871935.07642)
		pJunkcode = 4755986025.29961;
	pJunkcode = 5497634307.18021;
	pJunkcode = 7167455559.82821;
	if (pJunkcode = 1250505119.36763)
		pJunkcode = 1713979284.90829;
	pJunkcode = 1076742814.46915;
	if (pJunkcode = 2192478359.88942)
		pJunkcode = 5271764166.17569;
	pJunkcode = 2914385706.3279;
	pJunkcode = 8828195367.31982;
	if (pJunkcode = 5401482024.34608)
		pJunkcode = 9298950223.29079;
	pJunkcode = 8771334329.51503;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3168() {
	float pJunkcode = 2963176709.83464;
	pJunkcode = 1230917748.16917;
	if (pJunkcode = 7982683198.18535)
		pJunkcode = 110888254.90212;
	pJunkcode = 8893017808.71558;
	pJunkcode = 7461647083.82288;
	if (pJunkcode = 1086542283.52373)
		pJunkcode = 1128312675.1415;
	pJunkcode = 2331839349.8855;
	if (pJunkcode = 2699018567.04182)
		pJunkcode = 4639762030.16407;
	pJunkcode = 902231375.152975;
	pJunkcode = 2520780442.78633;
	if (pJunkcode = 2548827188.44829)
		pJunkcode = 2435930705.74687;
	pJunkcode = 7852404181.86594;
	if (pJunkcode = 9489524208.68719)
		pJunkcode = 2010983966.33601;
	pJunkcode = 5620547201.37557;
	pJunkcode = 1855454470.09339;
	if (pJunkcode = 5521827848.9963)
		pJunkcode = 940956514.431583;
	pJunkcode = 5384783945.43977;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3167() {
	float pJunkcode = 932778829.633657;
	pJunkcode = 1942745067.43311;
	if (pJunkcode = 2329017100.50675)
		pJunkcode = 6582404524.8443;
	pJunkcode = 2689079419.62431;
	pJunkcode = 7080593892.78067;
	if (pJunkcode = 8155944833.73113)
		pJunkcode = 6699386061.82799;
	pJunkcode = 4191744023.32608;
	if (pJunkcode = 846069620.895282)
		pJunkcode = 8120780815.17682;
	pJunkcode = 1245820571.75421;
	pJunkcode = 3444092968.80465;
	if (pJunkcode = 3967571645.44237)
		pJunkcode = 7695010489.83788;
	pJunkcode = 5621702791.16817;
	if (pJunkcode = 5667090545.5181)
		pJunkcode = 5629173513.74116;
	pJunkcode = 3782511090.43163;
	pJunkcode = 9051575525.9061;
	if (pJunkcode = 8154185108.01078)
		pJunkcode = 30379254.8876943;
	pJunkcode = 7393442203.46514;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3166() {
	float pJunkcode = 1990744263.74853;
	pJunkcode = 8471227022.19469;
	if (pJunkcode = 5461743417.25172)
		pJunkcode = 2923641151.06918;
	pJunkcode = 7003764774.55763;
	pJunkcode = 6844886052.3122;
	if (pJunkcode = 9733869462.77533)
		pJunkcode = 2030182274.43471;
	pJunkcode = 7257962917.82901;
	if (pJunkcode = 1574626660.43358)
		pJunkcode = 4053538972.36507;
	pJunkcode = 6853737220.9516;
	pJunkcode = 7066781392.55488;
	if (pJunkcode = 6764513499.00369)
		pJunkcode = 7719089172.78945;
	pJunkcode = 5321618049.56815;
	if (pJunkcode = 3151658576.82289)
		pJunkcode = 2552738950.94375;
	pJunkcode = 964716292.439656;
	pJunkcode = 3016972709.55002;
	if (pJunkcode = 4672987978.83677)
		pJunkcode = 5666708676.81328;
	pJunkcode = 9940942929.18164;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3165() {
	float pJunkcode = 7237602924.49242;
	pJunkcode = 4191785110.53889;
	if (pJunkcode = 5396710508.37204)
		pJunkcode = 7855443953.42656;
	pJunkcode = 5141361422.56123;
	pJunkcode = 1153611059.49058;
	if (pJunkcode = 7197403702.58383)
		pJunkcode = 1163567261.58219;
	pJunkcode = 9951578556.89333;
	if (pJunkcode = 3259199468.11016)
		pJunkcode = 3390018606.54031;
	pJunkcode = 3845292384.19402;
	pJunkcode = 8606021170.93216;
	if (pJunkcode = 4139279091.44799)
		pJunkcode = 1419784692.23642;
	pJunkcode = 1843794090.4609;
	if (pJunkcode = 2627849025.53798)
		pJunkcode = 3375683434.68601;
	pJunkcode = 7821701521.70197;
	pJunkcode = 1468921196.66511;
	if (pJunkcode = 7204605394.36675)
		pJunkcode = 1271134543.959;
	pJunkcode = 2290353539.66098;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3164() {
	float pJunkcode = 9452788033.23164;
	pJunkcode = 7437468517.52404;
	if (pJunkcode = 1266360757.90889)
		pJunkcode = 4124070901.88599;
	pJunkcode = 9705431484.05371;
	pJunkcode = 265290526.825795;
	if (pJunkcode = 5928665881.89992)
		pJunkcode = 7914082223.31273;
	pJunkcode = 2905799277.45273;
	if (pJunkcode = 6437291702.23736)
		pJunkcode = 4289159851.45777;
	pJunkcode = 8691592086.69348;
	pJunkcode = 3295569162.48426;
	if (pJunkcode = 4393731986.14887)
		pJunkcode = 1496269001.0376;
	pJunkcode = 4591364124.6718;
	if (pJunkcode = 3392085260.51562)
		pJunkcode = 4651517499.20883;
	pJunkcode = 7350465982.36088;
	pJunkcode = 4157911324.06365;
	if (pJunkcode = 5632940780.65585)
		pJunkcode = 8120045920.5389;
	pJunkcode = 3679862068.73116;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3163() {
	float pJunkcode = 9693499750.00456;
	pJunkcode = 9537015680.95195;
	if (pJunkcode = 904267291.134097)
		pJunkcode = 6273398251.5597;
	pJunkcode = 7872929897.74081;
	pJunkcode = 5383398584.36763;
	if (pJunkcode = 5013079709.97512)
		pJunkcode = 4110916306.95909;
	pJunkcode = 696524913.634426;
	if (pJunkcode = 5747256470.68161)
		pJunkcode = 9643232079.47355;
	pJunkcode = 9449871013.41542;
	pJunkcode = 8108272535.35528;
	if (pJunkcode = 9226555902.40388)
		pJunkcode = 134212101.470562;
	pJunkcode = 3949134839.60146;
	if (pJunkcode = 9434613587.64958)
		pJunkcode = 8607381856.85944;
	pJunkcode = 3863728160.93377;
	pJunkcode = 8819916281.134;
	if (pJunkcode = 6281657463.52052)
		pJunkcode = 9921996513.38072;
	pJunkcode = 8041359696.15261;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3162() {
	float pJunkcode = 5998908498.95622;
	pJunkcode = 5103932277.86249;
	if (pJunkcode = 2886417963.9699)
		pJunkcode = 2047317118.30602;
	pJunkcode = 4462085774.18397;
	pJunkcode = 5432744857.7953;
	if (pJunkcode = 8823005293.55503)
		pJunkcode = 3081854124.02977;
	pJunkcode = 1927690719.18046;
	if (pJunkcode = 7639196618.33581)
		pJunkcode = 8229778508.5765;
	pJunkcode = 7162768339.34573;
	pJunkcode = 1907729686.47531;
	if (pJunkcode = 4670386834.86767)
		pJunkcode = 1174781893.73328;
	pJunkcode = 9918645194.25136;
	if (pJunkcode = 8180663505.84273)
		pJunkcode = 1108897485.13879;
	pJunkcode = 5432670769.49708;
	pJunkcode = 9208603684.46394;
	if (pJunkcode = 9617839557.98002)
		pJunkcode = 8389724231.76883;
	pJunkcode = 5307231974.45927;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3161() {
	float pJunkcode = 3797135067.18482;
	pJunkcode = 384282056.685978;
	if (pJunkcode = 8465429825.31486)
		pJunkcode = 3261275111.58672;
	pJunkcode = 2788327292.27938;
	pJunkcode = 5324805329.50795;
	if (pJunkcode = 3271479732.17765)
		pJunkcode = 5914090966.72934;
	pJunkcode = 4191116388.03646;
	if (pJunkcode = 6144329390.67687)
		pJunkcode = 4888617762.43224;
	pJunkcode = 5067818943.76155;
	pJunkcode = 391226023.821815;
	if (pJunkcode = 7372069437.22456)
		pJunkcode = 9679317725.63124;
	pJunkcode = 5779115607.71405;
	if (pJunkcode = 8588270666.13149)
		pJunkcode = 12329772.9190316;
	pJunkcode = 4212022424.30786;
	pJunkcode = 5683754708.32371;
	if (pJunkcode = 7759346808.99407)
		pJunkcode = 6970678939.6368;
	pJunkcode = 9395590153.38994;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3160() {
	float pJunkcode = 5195744263.7171;
	pJunkcode = 6941311975.93704;
	if (pJunkcode = 728958026.014654)
		pJunkcode = 382125361.016281;
	pJunkcode = 865798111.663941;
	pJunkcode = 2506561112.33204;
	if (pJunkcode = 328905797.643978)
		pJunkcode = 1099912840.68279;
	pJunkcode = 4041586239.53016;
	if (pJunkcode = 5263652278.65968)
		pJunkcode = 4161959642.69433;
	pJunkcode = 4641250422.42231;
	pJunkcode = 6240734282.91998;
	if (pJunkcode = 2573764374.59039)
		pJunkcode = 8509315615.84405;
	pJunkcode = 44612822.7934664;
	if (pJunkcode = 3226558840.50905)
		pJunkcode = 1178837539.47104;
	pJunkcode = 992803044.99104;
	pJunkcode = 2162002744.58543;
	if (pJunkcode = 393012520.141863)
		pJunkcode = 7642001480.09597;
	pJunkcode = 4845810837.78091;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3159() {
	float pJunkcode = 183649869.441303;
	pJunkcode = 7293153192.43244;
	if (pJunkcode = 2854881503.69967)
		pJunkcode = 8969336676.70074;
	pJunkcode = 8171082110.77313;
	pJunkcode = 1252274342.15497;
	if (pJunkcode = 2120989932.06834)
		pJunkcode = 9490520972.31621;
	pJunkcode = 5465549817.94965;
	if (pJunkcode = 2135127274.35967)
		pJunkcode = 6504047689.66056;
	pJunkcode = 6511465152.75642;
	pJunkcode = 2520432760.26305;
	if (pJunkcode = 8715853614.37764)
		pJunkcode = 6906823634.46694;
	pJunkcode = 6576653807.48107;
	if (pJunkcode = 1665476611.33069)
		pJunkcode = 3076390393.17629;
	pJunkcode = 1868435666.13245;
	pJunkcode = 742483292.932679;
	if (pJunkcode = 6230561813.59112)
		pJunkcode = 1074558200.35616;
	pJunkcode = 9653243215.07788;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3158() {
	float pJunkcode = 310519860.133406;
	pJunkcode = 1611722592.88884;
	if (pJunkcode = 3591674276.79147)
		pJunkcode = 1127131949.20398;
	pJunkcode = 5151684939.25966;
	pJunkcode = 7706719179.01752;
	if (pJunkcode = 8006760269.96498)
		pJunkcode = 8324414361.96666;
	pJunkcode = 4255056279.73329;
	if (pJunkcode = 4552181286.83436)
		pJunkcode = 8047193929.03255;
	pJunkcode = 3045765781.72019;
	pJunkcode = 2976648458.8361;
	if (pJunkcode = 9857355326.50016)
		pJunkcode = 5672083469.5786;
	pJunkcode = 100885543.026243;
	if (pJunkcode = 2916720578.52247)
		pJunkcode = 3000617158.34808;
	pJunkcode = 2432728056.80998;
	pJunkcode = 2621957623.99349;
	if (pJunkcode = 1621776287.18093)
		pJunkcode = 7122232347.577;
	pJunkcode = 5573507441.65366;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3157() {
	float pJunkcode = 4968248196.44021;
	pJunkcode = 1155311802.81381;
	if (pJunkcode = 5000892973.0419)
		pJunkcode = 8937162709.22032;
	pJunkcode = 5827749871.19891;
	pJunkcode = 2459419292.3181;
	if (pJunkcode = 4574107849.19263)
		pJunkcode = 4323097274.56816;
	pJunkcode = 8259421420.39559;
	if (pJunkcode = 5619434745.35667)
		pJunkcode = 6739898260.87117;
	pJunkcode = 8888425291.15036;
	pJunkcode = 5760935986.35854;
	if (pJunkcode = 8940147919.79353)
		pJunkcode = 8451299428.29218;
	pJunkcode = 9692406916.75608;
	if (pJunkcode = 1147406119.69527)
		pJunkcode = 4329835482.00861;
	pJunkcode = 3199072342.6374;
	pJunkcode = 368538373.640146;
	if (pJunkcode = 579159188.975904)
		pJunkcode = 6231806562.65879;
	pJunkcode = 6075077760.77909;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3156() {
	float pJunkcode = 2535817761.97599;
	pJunkcode = 7387754999.33804;
	if (pJunkcode = 7030416126.57534)
		pJunkcode = 4512317780.07364;
	pJunkcode = 5789467282.66893;
	pJunkcode = 5494098283.02002;
	if (pJunkcode = 9003995781.44215)
		pJunkcode = 3065550342.20374;
	pJunkcode = 4239049561.2265;
	if (pJunkcode = 1347364532.06307)
		pJunkcode = 1939448518.83665;
	pJunkcode = 2064812168.53568;
	pJunkcode = 5090164294.41059;
	if (pJunkcode = 6155623903.89466)
		pJunkcode = 723732685.744949;
	pJunkcode = 3063749585.14938;
	if (pJunkcode = 7385333433.52027)
		pJunkcode = 4496898599.16401;
	pJunkcode = 3689053031.67051;
	pJunkcode = 7056074188.7625;
	if (pJunkcode = 689286886.664618)
		pJunkcode = 7357691143.23799;
	pJunkcode = 1397522200.52823;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3155() {
	float pJunkcode = 4971958229.49842;
	pJunkcode = 9839672078.81494;
	if (pJunkcode = 9049497668.23225)
		pJunkcode = 784605516.074308;
	pJunkcode = 2730702191.10532;
	pJunkcode = 2335015984.91472;
	if (pJunkcode = 225082839.279661)
		pJunkcode = 7747360679.26561;
	pJunkcode = 6929768129.20466;
	if (pJunkcode = 6787172485.69146)
		pJunkcode = 4762760956.75024;
	pJunkcode = 8710811160.74963;
	pJunkcode = 141121109.434853;
	if (pJunkcode = 2374160375.81245)
		pJunkcode = 1894082356.98469;
	pJunkcode = 5253306594.37735;
	if (pJunkcode = 7518862088.18717)
		pJunkcode = 3657680135.62007;
	pJunkcode = 8177835079.52878;
	pJunkcode = 2773909089.19127;
	if (pJunkcode = 421772633.135713)
		pJunkcode = 8727652482.62037;
	pJunkcode = 7218636215.37553;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3154() {
	float pJunkcode = 5064540111.0716;
	pJunkcode = 7127399839.32348;
	if (pJunkcode = 744499134.456455)
		pJunkcode = 2888605642.10352;
	pJunkcode = 2099926997.45182;
	pJunkcode = 4920070923.7161;
	if (pJunkcode = 4402966287.75084)
		pJunkcode = 9161262402.76056;
	pJunkcode = 892344595.480147;
	if (pJunkcode = 3826921822.66539)
		pJunkcode = 8174972597.41714;
	pJunkcode = 54573599.9614707;
	pJunkcode = 4335327836.77518;
	if (pJunkcode = 3096348487.5881)
		pJunkcode = 4365800569.66462;
	pJunkcode = 8596820410.5773;
	if (pJunkcode = 3344906881.79962)
		pJunkcode = 4316871908.74474;
	pJunkcode = 1774484728.45405;
	pJunkcode = 6231781268.19061;
	if (pJunkcode = 8657433835.23914)
		pJunkcode = 6243289452.60994;
	pJunkcode = 5424050040.35449;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3153() {
	float pJunkcode = 3573839856.00939;
	pJunkcode = 7656332023.32336;
	if (pJunkcode = 4657107236.74656)
		pJunkcode = 4488390635.75938;
	pJunkcode = 2443302303.9929;
	pJunkcode = 8030872815.46674;
	if (pJunkcode = 1544770992.42824)
		pJunkcode = 6860709015.72769;
	pJunkcode = 4168398498.1132;
	if (pJunkcode = 1875503209.22493)
		pJunkcode = 2584807631.1317;
	pJunkcode = 6443969337.06329;
	pJunkcode = 9168659735.52028;
	if (pJunkcode = 6966177469.58429)
		pJunkcode = 5717943970.18137;
	pJunkcode = 765336383.930368;
	if (pJunkcode = 4004813773.1251)
		pJunkcode = 4012135746.344;
	pJunkcode = 7095908449.95853;
	pJunkcode = 9652984782.63271;
	if (pJunkcode = 7447926534.48366)
		pJunkcode = 6817190095.10324;
	pJunkcode = 8078795811.73111;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3152() {
	float pJunkcode = 2012789377.62839;
	pJunkcode = 9473737424.46363;
	if (pJunkcode = 1786966107.29204)
		pJunkcode = 8019065942.58653;
	pJunkcode = 7892391893.21975;
	pJunkcode = 9385725601.83638;
	if (pJunkcode = 8536959112.177)
		pJunkcode = 3711928094.67632;
	pJunkcode = 8552750287.54259;
	if (pJunkcode = 5662747279.97756)
		pJunkcode = 3196944194.04744;
	pJunkcode = 6300031168.23141;
	pJunkcode = 2048979805.84111;
	if (pJunkcode = 8164872127.91651)
		pJunkcode = 1899815854.36572;
	pJunkcode = 4069612580.11857;
	if (pJunkcode = 2342600749.12484)
		pJunkcode = 9112675855.11064;
	pJunkcode = 8287551318.36208;
	pJunkcode = 3955920867.6496;
	if (pJunkcode = 6664793434.22717)
		pJunkcode = 4691297641.79125;
	pJunkcode = 6158528623.39098;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3151() {
	float pJunkcode = 2121887797.21386;
	pJunkcode = 4480491930.50544;
	if (pJunkcode = 1352212483.24475)
		pJunkcode = 5563858971.0747;
	pJunkcode = 288762072.69263;
	pJunkcode = 632976394.694247;
	if (pJunkcode = 2762221780.63583)
		pJunkcode = 7845356629.41261;
	pJunkcode = 9340432154.59558;
	if (pJunkcode = 4326182394.09976)
		pJunkcode = 4613225238.99063;
	pJunkcode = 4228329970.96774;
	pJunkcode = 5212959167.93292;
	if (pJunkcode = 9381280238.75129)
		pJunkcode = 3303206105.84813;
	pJunkcode = 629218501.447535;
	if (pJunkcode = 5821051272.51146)
		pJunkcode = 4107527565.33461;
	pJunkcode = 7549014598.32486;
	pJunkcode = 1075809678.16214;
	if (pJunkcode = 2018259932.6798)
		pJunkcode = 3417695816.41332;
	pJunkcode = 2573626018.69229;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3150() {
	float pJunkcode = 1295152111.80609;
	pJunkcode = 5140994359.40899;
	if (pJunkcode = 6796171844.897)
		pJunkcode = 9063260736.67842;
	pJunkcode = 9248512620.68555;
	pJunkcode = 480690127.0356;
	if (pJunkcode = 4233196601.96516)
		pJunkcode = 6237080002.93016;
	pJunkcode = 7697144060.9993;
	if (pJunkcode = 2567805264.24352)
		pJunkcode = 5911797817.36888;
	pJunkcode = 2265514660.61845;
	pJunkcode = 5704091411.84393;
	if (pJunkcode = 6724027928.53493)
		pJunkcode = 376126723.406323;
	pJunkcode = 2733567069.02033;
	if (pJunkcode = 2382047467.59002)
		pJunkcode = 4734256659.21128;
	pJunkcode = 237374360.5962;
	pJunkcode = 5367106441.62909;
	if (pJunkcode = 8466388724.07496)
		pJunkcode = 9588426347.59199;
	pJunkcode = 2878912331.11931;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3149() {
	float pJunkcode = 64528284.0124629;
	pJunkcode = 2829162959.70104;
	if (pJunkcode = 8088196306.18455)
		pJunkcode = 6838082077.85752;
	pJunkcode = 1930810043.51987;
	pJunkcode = 7712102972.9255;
	if (pJunkcode = 366813069.257931)
		pJunkcode = 3479274056.00275;
	pJunkcode = 293731895.527682;
	if (pJunkcode = 3008675900.47284)
		pJunkcode = 2061263746.2677;
	pJunkcode = 1892024170.9752;
	pJunkcode = 1528236303.12049;
	if (pJunkcode = 794561445.756787)
		pJunkcode = 3409748547.82937;
	pJunkcode = 3420358888.05773;
	if (pJunkcode = 7963609087.69489)
		pJunkcode = 3389966054.86126;
	pJunkcode = 3269364864.1148;
	pJunkcode = 346386307.74535;
	if (pJunkcode = 283786734.154787)
		pJunkcode = 3308169577.12674;
	pJunkcode = 5072103490.00096;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3148() {
	float pJunkcode = 8162175873.58059;
	pJunkcode = 2761984430.79462;
	if (pJunkcode = 8229398211.46282)
		pJunkcode = 5854659339.01527;
	pJunkcode = 7498493710.70294;
	pJunkcode = 414533479.276858;
	if (pJunkcode = 84524171.1164219)
		pJunkcode = 1528999938.08566;
	pJunkcode = 3308022918.57091;
	if (pJunkcode = 6457181885.28616)
		pJunkcode = 318622332.07235;
	pJunkcode = 8402899208.48645;
	pJunkcode = 2587913528.26216;
	if (pJunkcode = 189831198.117163)
		pJunkcode = 1192530708.20038;
	pJunkcode = 7297855488.54834;
	if (pJunkcode = 5916427631.70193)
		pJunkcode = 2360142227.64886;
	pJunkcode = 6589907671.08849;
	pJunkcode = 682780562.25653;
	if (pJunkcode = 2476807721.14905)
		pJunkcode = 7171778515.75479;
	pJunkcode = 7971847801.45083;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3147() {
	float pJunkcode = 8426843393.50511;
	pJunkcode = 4845977077.19805;
	if (pJunkcode = 339593595.19628)
		pJunkcode = 6331783639.62775;
	pJunkcode = 3879809819.89041;
	pJunkcode = 973768219.622227;
	if (pJunkcode = 7521846674.7805)
		pJunkcode = 7796042048.57522;
	pJunkcode = 1026844372.39371;
	if (pJunkcode = 6023011510.13846)
		pJunkcode = 2149358956.35609;
	pJunkcode = 7808500689.39994;
	pJunkcode = 1438954658.1443;
	if (pJunkcode = 2063161709.14371)
		pJunkcode = 148459052.347899;
	pJunkcode = 2859202180.23705;
	if (pJunkcode = 9926681906.16093)
		pJunkcode = 8883333070.46994;
	pJunkcode = 9119744117.64261;
	pJunkcode = 2791782525.01415;
	if (pJunkcode = 47963494.6601099)
		pJunkcode = 9994804495.6789;
	pJunkcode = 8071115282.90661;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3146() {
	float pJunkcode = 8399048733.38843;
	pJunkcode = 3566406111.22944;
	if (pJunkcode = 1684714637.6117)
		pJunkcode = 195812834.603535;
	pJunkcode = 2011445734.73379;
	pJunkcode = 8495630111.82285;
	if (pJunkcode = 2154154698.11634)
		pJunkcode = 5155983292.21817;
	pJunkcode = 4985117916.38057;
	if (pJunkcode = 4199920632.45719)
		pJunkcode = 3093883147.71876;
	pJunkcode = 4833196355.65162;
	pJunkcode = 7414419890.32229;
	if (pJunkcode = 2109540110.72281)
		pJunkcode = 4235148001.26782;
	pJunkcode = 9532078479.69216;
	if (pJunkcode = 3502847187.19185)
		pJunkcode = 8088612065.37894;
	pJunkcode = 9808740476.24024;
	pJunkcode = 1393919012.38392;
	if (pJunkcode = 520440830.805831)
		pJunkcode = 5979385493.14553;
	pJunkcode = 5947225565.21252;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3145() {
	float pJunkcode = 4624235140.54749;
	pJunkcode = 1218458350.65244;
	if (pJunkcode = 2527602142.46065)
		pJunkcode = 3242347566.52076;
	pJunkcode = 9074541990.32365;
	pJunkcode = 3158516027.28503;
	if (pJunkcode = 6762211944.54105)
		pJunkcode = 1658074088.82782;
	pJunkcode = 714191050.766757;
	if (pJunkcode = 2817882788.70747)
		pJunkcode = 8442039292.06035;
	pJunkcode = 8613203666.02048;
	pJunkcode = 1980619659.26473;
	if (pJunkcode = 1988246362.83929)
		pJunkcode = 2989308819.78493;
	pJunkcode = 3085405454.32341;
	if (pJunkcode = 5654192930.18767)
		pJunkcode = 9622506371.06066;
	pJunkcode = 8607321728.94701;
	pJunkcode = 209309491.578174;
	if (pJunkcode = 7197944502.22496)
		pJunkcode = 9817344469.68352;
	pJunkcode = 4394412012.46601;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3144() {
	float pJunkcode = 6938561996.46737;
	pJunkcode = 4927868278.80438;
	if (pJunkcode = 110906857.884514)
		pJunkcode = 3242646532.04734;
	pJunkcode = 8125015317.29813;
	pJunkcode = 8720583102.51249;
	if (pJunkcode = 760417967.259056)
		pJunkcode = 1152511138.94818;
	pJunkcode = 5256000459.89474;
	if (pJunkcode = 339564985.252893)
		pJunkcode = 2542467799.66172;
	pJunkcode = 7035310641.0918;
	pJunkcode = 5966830896.78722;
	if (pJunkcode = 8557323964.64054)
		pJunkcode = 7795195596.5783;
	pJunkcode = 5845136206.9033;
	if (pJunkcode = 3355875544.80526)
		pJunkcode = 1095047937.27147;
	pJunkcode = 1614781540.18952;
	pJunkcode = 9162544094.69702;
	if (pJunkcode = 6847878229.10861)
		pJunkcode = 4927704013.77135;
	pJunkcode = 675087036.955471;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3143() {
	float pJunkcode = 1924247421.22513;
	pJunkcode = 1227640820.01484;
	if (pJunkcode = 8929493512.14993)
		pJunkcode = 3302487157.62152;
	pJunkcode = 4703842433.7492;
	pJunkcode = 5492313246.5335;
	if (pJunkcode = 7300059774.32569)
		pJunkcode = 9081889288.2784;
	pJunkcode = 7081235099.63279;
	if (pJunkcode = 3407907080.74408)
		pJunkcode = 8765962300.42182;
	pJunkcode = 6999971205.02816;
	pJunkcode = 8536843594.40835;
	if (pJunkcode = 9661150039.05517)
		pJunkcode = 4868954973.48588;
	pJunkcode = 6807263899.92491;
	if (pJunkcode = 7341452867.65848)
		pJunkcode = 7414122659.36666;
	pJunkcode = 2987953419.0604;
	pJunkcode = 6125504068.88275;
	if (pJunkcode = 1022771115.52217)
		pJunkcode = 4078284305.64707;
	pJunkcode = 386851133.211211;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3142() {
	float pJunkcode = 7600294618.01232;
	pJunkcode = 2053730879.48907;
	if (pJunkcode = 295614610.477322)
		pJunkcode = 8093616875.32494;
	pJunkcode = 3440931954.2271;
	pJunkcode = 1919149105.09942;
	if (pJunkcode = 9933129322.67695)
		pJunkcode = 7995189939.01173;
	pJunkcode = 9079816219.88301;
	if (pJunkcode = 7854423178.21338)
		pJunkcode = 8207141335.5736;
	pJunkcode = 6405089364.62671;
	pJunkcode = 7437457763.18415;
	if (pJunkcode = 1490533601.44923)
		pJunkcode = 1173666436.55739;
	pJunkcode = 4029546477.79927;
	if (pJunkcode = 922100987.820118)
		pJunkcode = 2779932758.17341;
	pJunkcode = 2852375293.14785;
	pJunkcode = 1654556856.24096;
	if (pJunkcode = 2121387493.4016)
		pJunkcode = 2923945364.54599;
	pJunkcode = 7452375220.02881;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3141() {
	float pJunkcode = 7431940952.13134;
	pJunkcode = 7335720443.61598;
	if (pJunkcode = 7863091513.76161)
		pJunkcode = 9057083047.10913;
	pJunkcode = 1444671322.03813;
	pJunkcode = 3882178079.66539;
	if (pJunkcode = 813648878.683448)
		pJunkcode = 244874633.738973;
	pJunkcode = 1078147903.08445;
	if (pJunkcode = 9282128142.35202)
		pJunkcode = 5017362274.84557;
	pJunkcode = 6286582068.76846;
	pJunkcode = 5135947114.17424;
	if (pJunkcode = 5204978728.29889)
		pJunkcode = 7999488224.45002;
	pJunkcode = 7596392163.9929;
	if (pJunkcode = 463735555.143996)
		pJunkcode = 4403438337.84702;
	pJunkcode = 9933168898.83399;
	pJunkcode = 9903414138.62051;
	if (pJunkcode = 2690774933.62358)
		pJunkcode = 9923077362.95582;
	pJunkcode = 610069438.865808;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3140() {
	float pJunkcode = 9692155406.29673;
	pJunkcode = 5111027844.17101;
	if (pJunkcode = 4221600210.53857)
		pJunkcode = 1446896243.38907;
	pJunkcode = 2853592817.60061;
	pJunkcode = 6880703058.83139;
	if (pJunkcode = 1264081310.49478)
		pJunkcode = 2430142214.31776;
	pJunkcode = 9934986801.3655;
	if (pJunkcode = 6357616639.69791)
		pJunkcode = 3927797528.45675;
	pJunkcode = 5638539980.33632;
	pJunkcode = 9385092679.01572;
	if (pJunkcode = 9782008726.27696)
		pJunkcode = 8490296387.64408;
	pJunkcode = 635050517.801914;
	if (pJunkcode = 3371202721.27781)
		pJunkcode = 8292941284.5487;
	pJunkcode = 319494926.032522;
	pJunkcode = 1429791758.00179;
	if (pJunkcode = 8328190564.83209)
		pJunkcode = 3787700367.92357;
	pJunkcode = 6844285397.16776;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3139() {
	float pJunkcode = 4668471547.23672;
	pJunkcode = 2672394057.80059;
	if (pJunkcode = 4354173619.52165)
		pJunkcode = 9778152509.63269;
	pJunkcode = 5694564112.10511;
	pJunkcode = 8455343786.51322;
	if (pJunkcode = 2577355246.11682)
		pJunkcode = 7048470297.70627;
	pJunkcode = 2829348561.12088;
	if (pJunkcode = 9674653697.17725)
		pJunkcode = 7296803649.38133;
	pJunkcode = 7868123713.63062;
	pJunkcode = 8298844900.26661;
	if (pJunkcode = 1087826464.39488)
		pJunkcode = 3247721828.43751;
	pJunkcode = 1869895285.46748;
	if (pJunkcode = 417397230.302815)
		pJunkcode = 361604567.13199;
	pJunkcode = 6964437783.39704;
	pJunkcode = 2388478986.58868;
	if (pJunkcode = 3026941821.97171)
		pJunkcode = 1788520024.42306;
	pJunkcode = 4890409548.61161;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3138() {
	float pJunkcode = 9758387034.08614;
	pJunkcode = 2314424057.32353;
	if (pJunkcode = 7487205313.885)
		pJunkcode = 106878616.039464;
	pJunkcode = 4289787975.68685;
	pJunkcode = 8960223259.56009;
	if (pJunkcode = 9836740204.5522)
		pJunkcode = 3934447533.5465;
	pJunkcode = 9683645754.88682;
	if (pJunkcode = 3537170288.07828)
		pJunkcode = 5415811857.44711;
	pJunkcode = 5560435009.51096;
	pJunkcode = 4714928183.78633;
	if (pJunkcode = 2317846051.48801)
		pJunkcode = 9894337492.05571;
	pJunkcode = 3974711702.91365;
	if (pJunkcode = 755099041.648006)
		pJunkcode = 2459704714.22158;
	pJunkcode = 1614803884.72294;
	pJunkcode = 8688188986.54679;
	if (pJunkcode = 5937304354.88843)
		pJunkcode = 4771517243.19457;
	pJunkcode = 8081171419.56772;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3137() {
	float pJunkcode = 4997282252.90158;
	pJunkcode = 1241142473.48513;
	if (pJunkcode = 4645787380.16665)
		pJunkcode = 1502684487.47847;
	pJunkcode = 9758040406.27514;
	pJunkcode = 8656859040.33569;
	if (pJunkcode = 9747933240.63215)
		pJunkcode = 5136013733.34011;
	pJunkcode = 5689244983.93342;
	if (pJunkcode = 9286563670.76907)
		pJunkcode = 4354871748.2403;
	pJunkcode = 4248734387.9469;
	pJunkcode = 610029306.249096;
	if (pJunkcode = 6477855305.11599)
		pJunkcode = 5012140389.74806;
	pJunkcode = 9827485230.12412;
	if (pJunkcode = 3316990575.87304)
		pJunkcode = 8274555409.90178;
	pJunkcode = 8185460188.22938;
	pJunkcode = 798834385.504677;
	if (pJunkcode = 3241052957.26936)
		pJunkcode = 5011554768.32794;
	pJunkcode = 8838834387.34006;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3136() {
	float pJunkcode = 9147093.66674401;
	pJunkcode = 2857866893.55068;
	if (pJunkcode = 989118203.956544)
		pJunkcode = 3773230977.53034;
	pJunkcode = 5419154927.46926;
	pJunkcode = 4934139346.49781;
	if (pJunkcode = 1879237551.88754)
		pJunkcode = 3740080870.35308;
	pJunkcode = 4563332969.19383;
	if (pJunkcode = 6666054886.39818)
		pJunkcode = 2971455372.27292;
	pJunkcode = 5518956759.22184;
	pJunkcode = 8378936646.32886;
	if (pJunkcode = 1648368044.46305)
		pJunkcode = 4528224664.1271;
	pJunkcode = 5138300233.12388;
	if (pJunkcode = 9112783733.78226)
		pJunkcode = 524335217.324118;
	pJunkcode = 6142926844.54677;
	pJunkcode = 6205945986.40296;
	if (pJunkcode = 7465255200.94263)
		pJunkcode = 9778377729.11867;
	pJunkcode = 7334168887.36407;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3135() {
	float pJunkcode = 4295460181.01388;
	pJunkcode = 318875882.107738;
	if (pJunkcode = 7129618439.38566)
		pJunkcode = 2013822425.22207;
	pJunkcode = 2515335158.92027;
	pJunkcode = 5152343724.64959;
	if (pJunkcode = 1667548790.32192)
		pJunkcode = 3677800502.05454;
	pJunkcode = 8124798009.31831;
	if (pJunkcode = 7718527236.99385)
		pJunkcode = 138354559.573978;
	pJunkcode = 4678542314.82917;
	pJunkcode = 9216770903.6291;
	if (pJunkcode = 4205721191.37789)
		pJunkcode = 9205985029.47725;
	pJunkcode = 5660067435.38131;
	if (pJunkcode = 9836493336.68946)
		pJunkcode = 8824753061.21311;
	pJunkcode = 1133081965.27685;
	pJunkcode = 7282764446.55319;
	if (pJunkcode = 5276402536.94933)
		pJunkcode = 4886007642.93158;
	pJunkcode = 9942806677.93672;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3134() {
	float pJunkcode = 9270251390.70953;
	pJunkcode = 2473679384.02556;
	if (pJunkcode = 6767414483.93383)
		pJunkcode = 7198652556.4147;
	pJunkcode = 6955411511.56653;
	pJunkcode = 2316839869.95364;
	if (pJunkcode = 5029481810.54214)
		pJunkcode = 2517903557.9151;
	pJunkcode = 5773826484.12736;
	if (pJunkcode = 8302923135.00753)
		pJunkcode = 2518701847.04672;
	pJunkcode = 1257140435.31897;
	pJunkcode = 7954195526.9164;
	if (pJunkcode = 6976570653.17123)
		pJunkcode = 2447153363.48999;
	pJunkcode = 4749215065.90004;
	if (pJunkcode = 7197517057.37743)
		pJunkcode = 3934538648.57942;
	pJunkcode = 5261533724.9246;
	pJunkcode = 4803585020.00866;
	if (pJunkcode = 2950866850.95999)
		pJunkcode = 4521062206.24359;
	pJunkcode = 2188940105.23397;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3133() {
	float pJunkcode = 8224238361.14165;
	pJunkcode = 6076034615.30657;
	if (pJunkcode = 8391495859.67266)
		pJunkcode = 3030932862.50854;
	pJunkcode = 3454384373.82183;
	pJunkcode = 6959207036.85385;
	if (pJunkcode = 3383943538.37928)
		pJunkcode = 2563230481.21108;
	pJunkcode = 2252218962.41934;
	if (pJunkcode = 8808368427.42631)
		pJunkcode = 2534050143.37575;
	pJunkcode = 5450469958.69848;
	pJunkcode = 3646429429.04981;
	if (pJunkcode = 3975412711.71856)
		pJunkcode = 8786741703.70079;
	pJunkcode = 7791689421.00813;
	if (pJunkcode = 5699485632.37786)
		pJunkcode = 9763039721.47289;
	pJunkcode = 6451295766.61615;
	pJunkcode = 3342216424.88876;
	if (pJunkcode = 5259337089.88569)
		pJunkcode = 5031058989.55015;
	pJunkcode = 8525267337.97447;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3132() {
	float pJunkcode = 4446702646.73896;
	pJunkcode = 649423519.600885;
	if (pJunkcode = 1269185105.07076)
		pJunkcode = 6755621792.62631;
	pJunkcode = 2722004221.16912;
	pJunkcode = 9632836148.51249;
	if (pJunkcode = 9451528206.20227)
		pJunkcode = 5575482626.69702;
	pJunkcode = 4366411611.28021;
	if (pJunkcode = 1197713622.49447)
		pJunkcode = 5574286138.28255;
	pJunkcode = 2556415654.62862;
	pJunkcode = 3256372378.23073;
	if (pJunkcode = 624910998.336236)
		pJunkcode = 8985563373.72813;
	pJunkcode = 4109923848.32563;
	if (pJunkcode = 1872860917.50766)
		pJunkcode = 1854373864.98803;
	pJunkcode = 6626810314.13293;
	pJunkcode = 868508766.431156;
	if (pJunkcode = 1616200016.43295)
		pJunkcode = 6048687060.06038;
	pJunkcode = 9902380078.22367;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3131() {
	float pJunkcode = 5174143255.87902;
	pJunkcode = 640830469.088421;
	if (pJunkcode = 9968552984.38367)
		pJunkcode = 7369850543.79871;
	pJunkcode = 2544651757.19885;
	pJunkcode = 607811034.474845;
	if (pJunkcode = 7326106955.4914)
		pJunkcode = 9604122168.12104;
	pJunkcode = 5181005612.03377;
	if (pJunkcode = 8305699789.61729)
		pJunkcode = 1698363138.22875;
	pJunkcode = 1675381442.18281;
	pJunkcode = 4449042534.05544;
	if (pJunkcode = 4440541092.17192)
		pJunkcode = 6582414678.48713;
	pJunkcode = 7745463095.89975;
	if (pJunkcode = 9183675387.70692)
		pJunkcode = 3292933145.49009;
	pJunkcode = 4972964643.86111;
	pJunkcode = 6037776229.27998;
	if (pJunkcode = 8573135810.50396)
		pJunkcode = 5528836952.18163;
	pJunkcode = 2462798387.86548;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3130() {
	float pJunkcode = 553922315.539451;
	pJunkcode = 6964024191.83426;
	if (pJunkcode = 8311543486.96825)
		pJunkcode = 9383683127.08161;
	pJunkcode = 188176403.46196;
	pJunkcode = 9262136492.7234;
	if (pJunkcode = 9108470542.67522)
		pJunkcode = 1655594488.22417;
	pJunkcode = 3430164669.08794;
	if (pJunkcode = 1314602770.87435)
		pJunkcode = 819052714.452813;
	pJunkcode = 9716421881.5826;
	pJunkcode = 9513283395.31135;
	if (pJunkcode = 7731089635.429)
		pJunkcode = 6611622308.45758;
	pJunkcode = 8410651979.28063;
	if (pJunkcode = 7151640293.71683)
		pJunkcode = 9871525071.50814;
	pJunkcode = 3865758626.40403;
	pJunkcode = 3617382548.40483;
	if (pJunkcode = 9673049558.97861)
		pJunkcode = 4979952848.37686;
	pJunkcode = 4101530569.16428;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3129() {
	float pJunkcode = 8402374713.81224;
	pJunkcode = 7114632875.6022;
	if (pJunkcode = 2198603022.8099)
		pJunkcode = 9781206281.15991;
	pJunkcode = 1444798043.50531;
	pJunkcode = 2835444097.21911;
	if (pJunkcode = 3778327228.4689)
		pJunkcode = 2208314433.86734;
	pJunkcode = 2873877137.07816;
	if (pJunkcode = 37636837.1077063)
		pJunkcode = 1922747427.13194;
	pJunkcode = 3237027527.99036;
	pJunkcode = 2583474167.49104;
	if (pJunkcode = 1294347678.33523)
		pJunkcode = 9899992305.10757;
	pJunkcode = 9931020962.16552;
	if (pJunkcode = 2180639955.07046)
		pJunkcode = 4657866941.56977;
	pJunkcode = 7038714216.20825;
	pJunkcode = 6798201118.28628;
	if (pJunkcode = 2363950665.59455)
		pJunkcode = 2436269813.20329;
	pJunkcode = 6274161774.76728;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3128() {
	float pJunkcode = 6937224719.09891;
	pJunkcode = 1188741276.03007;
	if (pJunkcode = 207828833.876569)
		pJunkcode = 1093409838.73476;
	pJunkcode = 9230198790.10424;
	pJunkcode = 8933887448.11672;
	if (pJunkcode = 1415171199.16827)
		pJunkcode = 505742660.981679;
	pJunkcode = 3583743855.66442;
	if (pJunkcode = 7448809889.76857)
		pJunkcode = 2346695696.28223;
	pJunkcode = 9132324974.70661;
	pJunkcode = 9776045690.4447;
	if (pJunkcode = 6010594380.27409)
		pJunkcode = 2352590578.61003;
	pJunkcode = 3382046611.58927;
	if (pJunkcode = 9525829544.65837)
		pJunkcode = 1046251315.61749;
	pJunkcode = 3183120320.15845;
	pJunkcode = 3697529451.49498;
	if (pJunkcode = 4402024064.15468)
		pJunkcode = 7626486597.43561;
	pJunkcode = 9778227550.19872;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3127() {
	float pJunkcode = 2530734400.41297;
	pJunkcode = 7823938575.73642;
	if (pJunkcode = 947564803.672084)
		pJunkcode = 3697205380.09116;
	pJunkcode = 2178536141.69603;
	pJunkcode = 6558251481.18538;
	if (pJunkcode = 2371925189.06109)
		pJunkcode = 9888772802.18813;
	pJunkcode = 2393118288.67558;
	if (pJunkcode = 4045208961.97013)
		pJunkcode = 6875296342.97576;
	pJunkcode = 6230299828.18587;
	pJunkcode = 5358980318.98987;
	if (pJunkcode = 7207772252.93501)
		pJunkcode = 4595435886.78555;
	pJunkcode = 9190434586.94828;
	if (pJunkcode = 7916854514.71986)
		pJunkcode = 678339187.520485;
	pJunkcode = 9269371139.02607;
	pJunkcode = 5934839473.69087;
	if (pJunkcode = 7079332249.69044)
		pJunkcode = 5210818230.08361;
	pJunkcode = 5980690377.11048;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3126() {
	float pJunkcode = 5570627844.33257;
	pJunkcode = 9652659968.50657;
	if (pJunkcode = 1890721056.50545)
		pJunkcode = 8429662737.18648;
	pJunkcode = 36626795.279866;
	pJunkcode = 507784756.22629;
	if (pJunkcode = 673153393.140138)
		pJunkcode = 6574924088.67399;
	pJunkcode = 7052305550.48903;
	if (pJunkcode = 8541756078.61419)
		pJunkcode = 4685293271.94817;
	pJunkcode = 93611069.1119537;
	pJunkcode = 2595589654.30321;
	if (pJunkcode = 9703782384.3791)
		pJunkcode = 4750031664.65038;
	pJunkcode = 6572298141.50446;
	if (pJunkcode = 5113919122.48868)
		pJunkcode = 6489377491.83021;
	pJunkcode = 2704616511.77767;
	pJunkcode = 9515165451.35686;
	if (pJunkcode = 1564147959.00642)
		pJunkcode = 2340921893.99153;
	pJunkcode = 3439309902.21788;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3125() {
	float pJunkcode = 9570021604.53839;
	pJunkcode = 7098145392.23539;
	if (pJunkcode = 3742747073.53833)
		pJunkcode = 733549493.524172;
	pJunkcode = 1276014793.08458;
	pJunkcode = 121580532.07121;
	if (pJunkcode = 5058741894.40375)
		pJunkcode = 9812933879.50733;
	pJunkcode = 7982690897.81774;
	if (pJunkcode = 9594378352.01867)
		pJunkcode = 5412047493.90151;
	pJunkcode = 8359264472.07311;
	pJunkcode = 8849656864.43136;
	if (pJunkcode = 7592086423.39195)
		pJunkcode = 2184329999.81781;
	pJunkcode = 428843712.8903;
	if (pJunkcode = 6402192503.08603)
		pJunkcode = 5723511176.92303;
	pJunkcode = 8612081023.76278;
	pJunkcode = 6975230817.01958;
	if (pJunkcode = 5200209352.01707)
		pJunkcode = 3066055833.43086;
	pJunkcode = 3201494031.85418;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3124() {
	float pJunkcode = 8061793910.49957;
	pJunkcode = 1996894162.9301;
	if (pJunkcode = 2595129632.11779)
		pJunkcode = 6213074481.62011;
	pJunkcode = 5421946594.84879;
	pJunkcode = 4957242795.45469;
	if (pJunkcode = 2309923150.71554)
		pJunkcode = 8686302173.8188;
	pJunkcode = 5301732062.08559;
	if (pJunkcode = 111349946.466558)
		pJunkcode = 1095657027.78577;
	pJunkcode = 4693138382.4522;
	pJunkcode = 2240786815.07123;
	if (pJunkcode = 2801792445.25467)
		pJunkcode = 7417685132.82347;
	pJunkcode = 7984619323.95053;
	if (pJunkcode = 9132106223.19123)
		pJunkcode = 7825454681.54904;
	pJunkcode = 2953787428.39286;
	pJunkcode = 4082623543.25774;
	if (pJunkcode = 869340734.178587)
		pJunkcode = 4653254307.6381;
	pJunkcode = 8114380395.05624;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3123() {
	float pJunkcode = 5693864062.55286;
	pJunkcode = 1983982924.81495;
	if (pJunkcode = 9339325353.97645)
		pJunkcode = 3097588773.33697;
	pJunkcode = 7776480914.79962;
	pJunkcode = 9628763403.86325;
	if (pJunkcode = 4598684358.66318)
		pJunkcode = 8169543774.51297;
	pJunkcode = 9482777430.83646;
	if (pJunkcode = 9571568723.0826)
		pJunkcode = 1911436746.31593;
	pJunkcode = 3679838732.34912;
	pJunkcode = 1998784987.28996;
	if (pJunkcode = 6217854316.54986)
		pJunkcode = 9992768700.76851;
	pJunkcode = 4680065479.9963;
	if (pJunkcode = 5197821646.42119)
		pJunkcode = 5076502026.65526;
	pJunkcode = 1410348493.56082;
	pJunkcode = 1043636392.86868;
	if (pJunkcode = 5448715841.81225)
		pJunkcode = 1640621178.19495;
	pJunkcode = 7641479669.93223;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3122() {
	float pJunkcode = 9462584533.7252;
	pJunkcode = 7232780721.79413;
	if (pJunkcode = 9539528881.00979)
		pJunkcode = 7207896918.52816;
	pJunkcode = 8634912263.82308;
	pJunkcode = 47243915.4687691;
	if (pJunkcode = 1018075892.73049)
		pJunkcode = 767037846.693094;
	pJunkcode = 8932882768.32499;
	if (pJunkcode = 6530744701.63919)
		pJunkcode = 789714111.570143;
	pJunkcode = 1992711604.80594;
	pJunkcode = 9780495416.73241;
	if (pJunkcode = 8944905815.31112)
		pJunkcode = 5043613185.44375;
	pJunkcode = 4167815379.59747;
	if (pJunkcode = 6339177642.94256)
		pJunkcode = 4991682978.71605;
	pJunkcode = 6796355092.42257;
	pJunkcode = 7775534477.01096;
	if (pJunkcode = 6296519302.33722)
		pJunkcode = 6167977627.62392;
	pJunkcode = 8825535583.21122;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3121() {
	float pJunkcode = 2210920648.83715;
	pJunkcode = 262106899.962264;
	if (pJunkcode = 7252956550.26159)
		pJunkcode = 266080832.274482;
	pJunkcode = 4360852416.16088;
	pJunkcode = 8030122037.28114;
	if (pJunkcode = 5807433633.28761)
		pJunkcode = 3844357043.00287;
	pJunkcode = 6257839124.53458;
	if (pJunkcode = 5067702306.0887)
		pJunkcode = 5956859532.91572;
	pJunkcode = 8011181246.61232;
	pJunkcode = 4837772817.37621;
	if (pJunkcode = 3245779122.94415)
		pJunkcode = 3136539427.79351;
	pJunkcode = 7225504550.56142;
	if (pJunkcode = 8193082204.96995)
		pJunkcode = 7602777701.72757;
	pJunkcode = 8400822670.94915;
	pJunkcode = 8890817390.21655;
	if (pJunkcode = 9286416960.99101)
		pJunkcode = 2136713694.32351;
	pJunkcode = 9115041979.84997;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3120() {
	float pJunkcode = 5152176181.39388;
	pJunkcode = 3566093823.26666;
	if (pJunkcode = 5774857624.85714)
		pJunkcode = 3789727008.02462;
	pJunkcode = 6068710984.35007;
	pJunkcode = 2780486142.18499;
	if (pJunkcode = 583889338.343163)
		pJunkcode = 5964322157.71991;
	pJunkcode = 7956817797.72683;
	if (pJunkcode = 5957217571.64246)
		pJunkcode = 8537320317.36883;
	pJunkcode = 1987265592.18725;
	pJunkcode = 442835977.530071;
	if (pJunkcode = 1889414372.77762)
		pJunkcode = 4904038859.51891;
	pJunkcode = 2021923807.5245;
	if (pJunkcode = 4380405563.92527)
		pJunkcode = 372433332.916089;
	pJunkcode = 2988960797.33232;
	pJunkcode = 4428742789.702;
	if (pJunkcode = 4301529677.59332)
		pJunkcode = 7839860870.69607;
	pJunkcode = 591109847.310526;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3119() {
	float pJunkcode = 8227400176.6486;
	pJunkcode = 454619232.305974;
	if (pJunkcode = 1266980784.36916)
		pJunkcode = 7141902251.75668;
	pJunkcode = 2464356402.77485;
	pJunkcode = 1907086514.5522;
	if (pJunkcode = 8847137007.81199)
		pJunkcode = 7805373115.95332;
	pJunkcode = 5694543110.99107;
	if (pJunkcode = 7892771505.81819)
		pJunkcode = 4295218637.93471;
	pJunkcode = 6606086016.65963;
	pJunkcode = 2913057669.91528;
	if (pJunkcode = 1677923536.23458)
		pJunkcode = 8768171436.01298;
	pJunkcode = 8048497534.77005;
	if (pJunkcode = 169834887.26567)
		pJunkcode = 9668902460.60687;
	pJunkcode = 8797658657.86589;
	pJunkcode = 2057796306.34436;
	if (pJunkcode = 2157827266.77033)
		pJunkcode = 8500406996.81784;
	pJunkcode = 480645521.78695;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3118() {
	float pJunkcode = 8028915267.16827;
	pJunkcode = 3366879129.72992;
	if (pJunkcode = 1389978325.62685)
		pJunkcode = 412217445.221286;
	pJunkcode = 8952872406.31696;
	pJunkcode = 7818183523.10996;
	if (pJunkcode = 9428464398.59437)
		pJunkcode = 7169523643.36937;
	pJunkcode = 1934480533.017;
	if (pJunkcode = 6194875481.96391)
		pJunkcode = 489860787.345853;
	pJunkcode = 2531533071.38303;
	pJunkcode = 8079173422.00604;
	if (pJunkcode = 3272375837.1411)
		pJunkcode = 975791786.389504;
	pJunkcode = 1602548221.29524;
	if (pJunkcode = 2586862257.19624)
		pJunkcode = 1167796709.00078;
	pJunkcode = 736833637.027995;
	pJunkcode = 6026362666.288;
	if (pJunkcode = 2602585595.70388)
		pJunkcode = 8276050116.46393;
	pJunkcode = 5345486786.57123;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3117() {
	float pJunkcode = 2768721048.01768;
	pJunkcode = 417781788.17467;
	if (pJunkcode = 743460750.43779)
		pJunkcode = 6582635457.93714;
	pJunkcode = 8364102483.81401;
	pJunkcode = 5786538698.27029;
	if (pJunkcode = 2300478602.81656)
		pJunkcode = 2674339613.11253;
	pJunkcode = 6396091176.5476;
	if (pJunkcode = 8647274034.09892)
		pJunkcode = 8518055641.55884;
	pJunkcode = 8381104440.92435;
	pJunkcode = 3387019507.75809;
	if (pJunkcode = 4847400447.49267)
		pJunkcode = 6181314983.46521;
	pJunkcode = 5174399176.10666;
	if (pJunkcode = 9865943205.89574)
		pJunkcode = 7159138273.50069;
	pJunkcode = 4596773497.0985;
	pJunkcode = 3212588301.07938;
	if (pJunkcode = 8445577837.75119)
		pJunkcode = 9037873416.56945;
	pJunkcode = 6992194285.28112;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3116() {
	float pJunkcode = 2237161484.95413;
	pJunkcode = 5716459789.78735;
	if (pJunkcode = 7382273417.53762)
		pJunkcode = 1222479104.73887;
	pJunkcode = 6331736708.42423;
	pJunkcode = 5159914032.54961;
	if (pJunkcode = 2448159675.40821)
		pJunkcode = 2865277979.47899;
	pJunkcode = 8723222025.20403;
	if (pJunkcode = 6505200711.35684)
		pJunkcode = 7943484628.13865;
	pJunkcode = 9664740416.98557;
	pJunkcode = 5461185048.38591;
	if (pJunkcode = 490998884.747121)
		pJunkcode = 7528392351.83045;
	pJunkcode = 9999915815.26603;
	if (pJunkcode = 3939357847.79244)
		pJunkcode = 4433568412.64824;
	pJunkcode = 3526653912.70142;
	pJunkcode = 2487817755.41804;
	if (pJunkcode = 510054157.098586)
		pJunkcode = 7682699937.13484;
	pJunkcode = 6135297242.25968;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3115() {
	float pJunkcode = 4849509841.76182;
	pJunkcode = 5854976213.69586;
	if (pJunkcode = 5506406928.85915)
		pJunkcode = 9754976758.17077;
	pJunkcode = 1972401578.75409;
	pJunkcode = 3418328073.345;
	if (pJunkcode = 3591081218.83496)
		pJunkcode = 9830107782.65682;
	pJunkcode = 7564352717.72822;
	if (pJunkcode = 2797875825.96055)
		pJunkcode = 5112988184.08097;
	pJunkcode = 9916034674.286;
	pJunkcode = 4427663509.38975;
	if (pJunkcode = 8112641421.32363)
		pJunkcode = 743376471.244209;
	pJunkcode = 6155272423.94156;
	if (pJunkcode = 8307652313.48071)
		pJunkcode = 3866748556.52199;
	pJunkcode = 813159529.443432;
	pJunkcode = 620169717.447222;
	if (pJunkcode = 7653488055.71936)
		pJunkcode = 1914691173.94187;
	pJunkcode = 1236034681.24698;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3114() {
	float pJunkcode = 4162443546.08005;
	pJunkcode = 1892701384.72629;
	if (pJunkcode = 4702815306.97958)
		pJunkcode = 4278907952.74914;
	pJunkcode = 8497665038.28363;
	pJunkcode = 4450690623.25516;
	if (pJunkcode = 7600603006.10081)
		pJunkcode = 3658101734.14861;
	pJunkcode = 7046656970.25116;
	if (pJunkcode = 5781050073.41334)
		pJunkcode = 9210860712.83956;
	pJunkcode = 9007539826.53781;
	pJunkcode = 3547712214.03447;
	if (pJunkcode = 6155148638.43944)
		pJunkcode = 2121645206.40727;
	pJunkcode = 2175609650.42394;
	if (pJunkcode = 361622252.770822)
		pJunkcode = 1184553283.69303;
	pJunkcode = 1285927761.28632;
	pJunkcode = 3780556.99349362;
	if (pJunkcode = 6225818234.68663)
		pJunkcode = 1596206071.11714;
	pJunkcode = 7362788169.49483;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3113() {
	float pJunkcode = 3982604932.04561;
	pJunkcode = 8702137716.94816;
	if (pJunkcode = 513203788.699797)
		pJunkcode = 9199635524.25266;
	pJunkcode = 9545332699.5269;
	pJunkcode = 5840876515.08655;
	if (pJunkcode = 234226342.495215)
		pJunkcode = 5736238757.73477;
	pJunkcode = 5756195618.15652;
	if (pJunkcode = 496249095.737469)
		pJunkcode = 9555727522.22518;
	pJunkcode = 8317489912.82849;
	pJunkcode = 1709514806.61852;
	if (pJunkcode = 1620002943.33241)
		pJunkcode = 8273755579.44671;
	pJunkcode = 4683056371.58041;
	if (pJunkcode = 4507049396.83328)
		pJunkcode = 1024994788.91334;
	pJunkcode = 6745285449.41599;
	pJunkcode = 4307908605.33211;
	if (pJunkcode = 6364290924.12439)
		pJunkcode = 3141122418.02407;
	pJunkcode = 8974624503.43371;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3112() {
	float pJunkcode = 8965153413.10678;
	pJunkcode = 5688887543.57546;
	if (pJunkcode = 1278774476.83844)
		pJunkcode = 4334967399.91679;
	pJunkcode = 9538788321.65895;
	pJunkcode = 6798914122.406;
	if (pJunkcode = 8692887294.31365)
		pJunkcode = 8351352237.16577;
	pJunkcode = 6666151349.4975;
	if (pJunkcode = 7358435802.69545)
		pJunkcode = 9062733611.04424;
	pJunkcode = 1399927710.50538;
	pJunkcode = 7278367518.74254;
	if (pJunkcode = 3455981236.13561)
		pJunkcode = 2461745456.73368;
	pJunkcode = 6331543074.42751;
	if (pJunkcode = 3979995487.74707)
		pJunkcode = 467390922.442055;
	pJunkcode = 8233044451.89092;
	pJunkcode = 6877072168.77412;
	if (pJunkcode = 6661890169.90782)
		pJunkcode = 3723273231.75471;
	pJunkcode = 1728871633.54743;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3111() {
	float pJunkcode = 9938509717.29153;
	pJunkcode = 3748663490.15701;
	if (pJunkcode = 4390824626.37946)
		pJunkcode = 2911106929.51675;
	pJunkcode = 1612175453.27528;
	pJunkcode = 7169359108.2555;
	if (pJunkcode = 128206106.48584)
		pJunkcode = 2725936658.81899;
	pJunkcode = 9301888397.88774;
	if (pJunkcode = 7124772672.13358)
		pJunkcode = 4451320256.21172;
	pJunkcode = 6736096211.73446;
	pJunkcode = 9965395653.15007;
	if (pJunkcode = 5811103747.93116)
		pJunkcode = 1539076146.87584;
	pJunkcode = 2482996721.5179;
	if (pJunkcode = 8397852740.89828)
		pJunkcode = 8989286133.18206;
	pJunkcode = 4189598517.38522;
	pJunkcode = 1857213180.68229;
	if (pJunkcode = 701384003.322711)
		pJunkcode = 2656220601.62406;
	pJunkcode = 8424673969.11141;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3110() {
	float pJunkcode = 8724598136.00721;
	pJunkcode = 6772305911.14683;
	if (pJunkcode = 4387988898.56953)
		pJunkcode = 6340686084.04703;
	pJunkcode = 9294923986.09753;
	pJunkcode = 1856919044.01497;
	if (pJunkcode = 311027900.830576)
		pJunkcode = 2979293944.95786;
	pJunkcode = 7850309942.39182;
	if (pJunkcode = 5269305270.99412)
		pJunkcode = 6898184472.30662;
	pJunkcode = 4896682015.60322;
	pJunkcode = 805239250.317796;
	if (pJunkcode = 1042215569.71583)
		pJunkcode = 6283197403.96778;
	pJunkcode = 4701757169.80664;
	if (pJunkcode = 7864023930.62912)
		pJunkcode = 5398632988.06965;
	pJunkcode = 8342994902.03974;
	pJunkcode = 3123719374.55977;
	if (pJunkcode = 8736009570.94839)
		pJunkcode = 3287983811.54678;
	pJunkcode = 8396354348.66389;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3109() {
	float pJunkcode = 2941983696.36663;
	pJunkcode = 7330904692.59031;
	if (pJunkcode = 4577594294.89136)
		pJunkcode = 9709262338.7096;
	pJunkcode = 1501188558.56766;
	pJunkcode = 6291342390.3571;
	if (pJunkcode = 801281904.694762)
		pJunkcode = 4186777788.37537;
	pJunkcode = 6166680287.29638;
	if (pJunkcode = 6933771437.46542)
		pJunkcode = 3657449849.78995;
	pJunkcode = 5166825126.65561;
	pJunkcode = 5087847273.16296;
	if (pJunkcode = 7528420389.01618)
		pJunkcode = 3956474579.9491;
	pJunkcode = 2558153229.06305;
	if (pJunkcode = 9741257230.94786)
		pJunkcode = 865329626.169261;
	pJunkcode = 5173524181.68655;
	pJunkcode = 5605314990.91628;
	if (pJunkcode = 9829323540.05746)
		pJunkcode = 9389027642.8528;
	pJunkcode = 738329342.423619;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3108() {
	float pJunkcode = 2328521395.05372;
	pJunkcode = 8594865723.73883;
	if (pJunkcode = 203718124.242231)
		pJunkcode = 8346759216.69139;
	pJunkcode = 8499230696.9466;
	pJunkcode = 7895634278.96522;
	if (pJunkcode = 7552316010.70483)
		pJunkcode = 1700496679.12248;
	pJunkcode = 253841720.611723;
	if (pJunkcode = 3462640437.94382)
		pJunkcode = 6107661615.52379;
	pJunkcode = 1992198413.94643;
	pJunkcode = 8195083809.04224;
	if (pJunkcode = 499506994.833589)
		pJunkcode = 7580926247.45553;
	pJunkcode = 9400255674.27343;
	if (pJunkcode = 9796927661.58142)
		pJunkcode = 7319633098.99188;
	pJunkcode = 6375274143.55631;
	pJunkcode = 1146391872.98805;
	if (pJunkcode = 2781776523.27418)
		pJunkcode = 1553750770.77246;
	pJunkcode = 605107918.68848;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3107() {
	float pJunkcode = 3508283455.94554;
	pJunkcode = 364808678.230703;
	if (pJunkcode = 9303031396.32504)
		pJunkcode = 8375060408.77775;
	pJunkcode = 1273517644.2108;
	pJunkcode = 5176915873.24579;
	if (pJunkcode = 3126181492.08523)
		pJunkcode = 9269876551.09156;
	pJunkcode = 1969964678.86347;
	if (pJunkcode = 7384744757.39265)
		pJunkcode = 3540684577.71524;
	pJunkcode = 1331944876.18368;
	pJunkcode = 9100734118.75706;
	if (pJunkcode = 7799943166.16011)
		pJunkcode = 5487755862.76107;
	pJunkcode = 2671650408.81801;
	if (pJunkcode = 4043927859.08931)
		pJunkcode = 5081250660.26248;
	pJunkcode = 8916669985.11772;
	pJunkcode = 1785937848.97681;
	if (pJunkcode = 6447662788.08889)
		pJunkcode = 8321292602.24307;
	pJunkcode = 2837193497.00368;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3106() {
	float pJunkcode = 4272947817.95768;
	pJunkcode = 211653986.567011;
	if (pJunkcode = 2813224902.26444)
		pJunkcode = 2588113602.29008;
	pJunkcode = 2039091156.55691;
	pJunkcode = 205104398.00137;
	if (pJunkcode = 7724004157.5714)
		pJunkcode = 3698442406.24361;
	pJunkcode = 1580116246.87148;
	if (pJunkcode = 507631897.45243)
		pJunkcode = 3620791560.06244;
	pJunkcode = 4930832870.10199;
	pJunkcode = 4109918276.74404;
	if (pJunkcode = 600768983.754612)
		pJunkcode = 4289987532.83592;
	pJunkcode = 8199163424.45507;
	if (pJunkcode = 6352210089.15039)
		pJunkcode = 4712126926.20849;
	pJunkcode = 2964081897.43067;
	pJunkcode = 8666593974.03782;
	if (pJunkcode = 5201828170.04098)
		pJunkcode = 8078947813.69677;
	pJunkcode = 1443111549.27498;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3105() {
	float pJunkcode = 4423510539.0879;
	pJunkcode = 462960059.904447;
	if (pJunkcode = 2947804890.86091)
		pJunkcode = 9211797050.5359;
	pJunkcode = 4333108088.86941;
	pJunkcode = 7944149318.05465;
	if (pJunkcode = 5315947676.59997)
		pJunkcode = 786715562.262746;
	pJunkcode = 4502363247.36105;
	if (pJunkcode = 4206197162.3029)
		pJunkcode = 9298066383.72299;
	pJunkcode = 537491556.135097;
	pJunkcode = 1959317430.69056;
	if (pJunkcode = 7351621902.76969)
		pJunkcode = 1340359906.16625;
	pJunkcode = 5271311694.61254;
	if (pJunkcode = 3856087110.47842)
		pJunkcode = 2467736734.64059;
	pJunkcode = 2086045959.55484;
	pJunkcode = 2693597744.11857;
	if (pJunkcode = 5491802851.94207)
		pJunkcode = 2127365476.81171;
	pJunkcode = 7817066543.23689;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3104() {
	float pJunkcode = 3974021632.38299;
	pJunkcode = 1534205285.38793;
	if (pJunkcode = 2084331030.00236)
		pJunkcode = 1787092614.55717;
	pJunkcode = 8102554802.77948;
	pJunkcode = 393204736.214145;
	if (pJunkcode = 4654033183.54479)
		pJunkcode = 8841396648.9805;
	pJunkcode = 9839666812.23845;
	if (pJunkcode = 2408761625.04579)
		pJunkcode = 9176297854.66714;
	pJunkcode = 7583778587.56576;
	pJunkcode = 6095798367.92359;
	if (pJunkcode = 8287276136.24792)
		pJunkcode = 4265385856.95276;
	pJunkcode = 829782327.970548;
	if (pJunkcode = 6979604482.95325)
		pJunkcode = 2187387231.1738;
	pJunkcode = 8194496439.08148;
	pJunkcode = 7612488831.06275;
	if (pJunkcode = 8148553261.14393)
		pJunkcode = 363859378.982048;
	pJunkcode = 734588266.366123;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3103() {
	float pJunkcode = 6709564568.39507;
	pJunkcode = 4107657311.24161;
	if (pJunkcode = 2517495253.88337)
		pJunkcode = 5283548962.27504;
	pJunkcode = 1651807177.11982;
	pJunkcode = 9984337088.75332;
	if (pJunkcode = 9000436236.96555)
		pJunkcode = 623532684.977198;
	pJunkcode = 2071306693.20475;
	if (pJunkcode = 1700217348.00774)
		pJunkcode = 865858144.116764;
	pJunkcode = 5871484584.09084;
	pJunkcode = 5596566061.84671;
	if (pJunkcode = 127341030.55197)
		pJunkcode = 6452975017.55759;
	pJunkcode = 5756119883.76211;
	if (pJunkcode = 8014529948.75142)
		pJunkcode = 4538287974.2286;
	pJunkcode = 3879555983.47619;
	pJunkcode = 3961445438.87016;
	if (pJunkcode = 3473770048.179)
		pJunkcode = 7465584706.55933;
	pJunkcode = 6842518260.52213;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3102() {
	float pJunkcode = 4147868344.53063;
	pJunkcode = 6174716966.60998;
	if (pJunkcode = 3131545539.82379)
		pJunkcode = 334740219.114854;
	pJunkcode = 8382647745.74076;
	pJunkcode = 7886917582.64762;
	if (pJunkcode = 5570569732.71334)
		pJunkcode = 1115967670.83265;
	pJunkcode = 9928025660.81312;
	if (pJunkcode = 4843608470.50888)
		pJunkcode = 8673506630.42182;
	pJunkcode = 313820632.23551;
	pJunkcode = 2198357372.63925;
	if (pJunkcode = 5358428138.70068)
		pJunkcode = 5244537008.77964;
	pJunkcode = 5870315643.92264;
	if (pJunkcode = 9145184739.13781)
		pJunkcode = 3174505765.62278;
	pJunkcode = 1469483498.00117;
	pJunkcode = 57456999.3689575;
	if (pJunkcode = 8869680527.95334)
		pJunkcode = 6875677175.59457;
	pJunkcode = 9470068038.57271;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3101() {
	float pJunkcode = 5686249242.53551;
	pJunkcode = 4680083521.86245;
	if (pJunkcode = 8815804754.27228)
		pJunkcode = 6834685666.86833;
	pJunkcode = 371506326.978626;
	pJunkcode = 3754759841.15435;
	if (pJunkcode = 4747607358.44333)
		pJunkcode = 6676236757.33441;
	pJunkcode = 3232715803.70171;
	if (pJunkcode = 3643346633.1448)
		pJunkcode = 5131822302.12885;
	pJunkcode = 1398928541.75987;
	pJunkcode = 8614472156.59146;
	if (pJunkcode = 5270892480.24595)
		pJunkcode = 5701278731.70894;
	pJunkcode = 4708368332.14855;
	if (pJunkcode = 45974675.190611)
		pJunkcode = 7068781832.4856;
	pJunkcode = 8960925227.61766;
	pJunkcode = 3162722129.00844;
	if (pJunkcode = 1489633966.80852)
		pJunkcode = 5695387458.22002;
	pJunkcode = 1310877545.76856;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs3100() {
	float pJunkcode = 674663182.320556;
	pJunkcode = 6257551625.80254;
	if (pJunkcode = 2915042829.32366)
		pJunkcode = 1648321842.01216;
	pJunkcode = 2635273541.00351;
	pJunkcode = 9044256674.91491;
	if (pJunkcode = 9998328941.28697)
		pJunkcode = 4196372959.89639;
	pJunkcode = 2865819051.52925;
	if (pJunkcode = 5513628527.46303)
		pJunkcode = 760996536.309504;
	pJunkcode = 8253310799.35081;
	pJunkcode = 7679783781.11168;
	if (pJunkcode = 7497592020.18821)
		pJunkcode = 5123580740.88261;
	pJunkcode = 2312866973.72742;
	if (pJunkcode = 1003948720.37998)
		pJunkcode = 8279017115.20632;
	pJunkcode = 1428043932.13664;
	pJunkcode = 3626619813.91125;
	if (pJunkcode = 5222988217.34783)
		pJunkcode = 7272394818.20691;
	pJunkcode = 9000714122.21646;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs399() {
	float pJunkcode = 6084295948.33335;
	pJunkcode = 7456295343.38907;
	if (pJunkcode = 3036944084.48894)
		pJunkcode = 8802926950.3032;
	pJunkcode = 5614278608.64713;
	pJunkcode = 5520928754.51727;
	if (pJunkcode = 4539126037.42133)
		pJunkcode = 6162176891.7537;
	pJunkcode = 930116437.847926;
	if (pJunkcode = 3417550640.97387)
		pJunkcode = 1344113832.01847;
	pJunkcode = 1196752483.92598;
	pJunkcode = 7080966918.66607;
	if (pJunkcode = 7102788866.6057)
		pJunkcode = 4937415308.41464;
	pJunkcode = 8608613441.81417;
	if (pJunkcode = 561602283.247708)
		pJunkcode = 3780737937.50233;
	pJunkcode = 9763033092.85075;
	pJunkcode = 7611015209.43283;
	if (pJunkcode = 4483750954.16232)
		pJunkcode = 5304998450.10208;
	pJunkcode = 5565980940.40117;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs398() {
	float pJunkcode = 9644573617.60018;
	pJunkcode = 3919716013.39446;
	if (pJunkcode = 1423755076.24511)
		pJunkcode = 1701934750.19693;
	pJunkcode = 5308781982.40288;
	pJunkcode = 9298293517.05772;
	if (pJunkcode = 2939839342.92593)
		pJunkcode = 8734902164.97565;
	pJunkcode = 2900236550.32785;
	if (pJunkcode = 5099885810.21692)
		pJunkcode = 2360684615.17911;
	pJunkcode = 537605793.348011;
	pJunkcode = 8177309257.73317;
	if (pJunkcode = 9460800733.55705)
		pJunkcode = 342163724.105499;
	pJunkcode = 9897258835.49374;
	if (pJunkcode = 6841820923.41976)
		pJunkcode = 3150747887.49376;
	pJunkcode = 5383258631.26729;
	pJunkcode = 6139350131.31019;
	if (pJunkcode = 4373895493.30958)
		pJunkcode = 9364115422.89095;
	pJunkcode = 7308335640.4959;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs397() {
	float pJunkcode = 1194234864.78671;
	pJunkcode = 389998301.488845;
	if (pJunkcode = 641602553.39315)
		pJunkcode = 45158774.9252206;
	pJunkcode = 8040378958.09654;
	pJunkcode = 5832698536.92495;
	if (pJunkcode = 6687214686.90701)
		pJunkcode = 2129722055.59011;
	pJunkcode = 9179186625.42386;
	if (pJunkcode = 8880288546.8834)
		pJunkcode = 171913857.858862;
	pJunkcode = 9987980912.42351;
	pJunkcode = 5361397267.82216;
	if (pJunkcode = 7844798912.46608)
		pJunkcode = 179132599.659312;
	pJunkcode = 2865086412.9977;
	if (pJunkcode = 2910712075.97679)
		pJunkcode = 1320833657.05713;
	pJunkcode = 1268535337.12148;
	pJunkcode = 6260505723.52797;
	if (pJunkcode = 9312842122.62867)
		pJunkcode = 3680056813.31895;
	pJunkcode = 2690434196.36895;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs396() {
	float pJunkcode = 5033405293.615;
	pJunkcode = 86613826.2010045;
	if (pJunkcode = 8127156872.14248)
		pJunkcode = 2314526024.48423;
	pJunkcode = 8122557497.78338;
	pJunkcode = 4804259050.99667;
	if (pJunkcode = 960893339.976794)
		pJunkcode = 2059535450.04164;
	pJunkcode = 9778966491.39276;
	if (pJunkcode = 2756149466.83377)
		pJunkcode = 5918305587.64081;
	pJunkcode = 7872915511.27921;
	pJunkcode = 9297577744.60933;
	if (pJunkcode = 9382978285.92263)
		pJunkcode = 6054982607.32289;
	pJunkcode = 8177259097.20091;
	if (pJunkcode = 31676506.740469)
		pJunkcode = 2563998071.92932;
	pJunkcode = 2220811938.39444;
	pJunkcode = 8952148645.52908;
	if (pJunkcode = 9351696896.05669)
		pJunkcode = 5655508889.79558;
	pJunkcode = 5870980198.79822;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs395() {
	float pJunkcode = 4263123894.94514;
	pJunkcode = 1355216611.11859;
	if (pJunkcode = 8643693223.08399)
		pJunkcode = 8319290667.55457;
	pJunkcode = 5443160493.01669;
	pJunkcode = 565383328.942095;
	if (pJunkcode = 8211927747.90325)
		pJunkcode = 3950746041.81119;
	pJunkcode = 6763963330.35115;
	if (pJunkcode = 3321664799.87895)
		pJunkcode = 5517608077.92249;
	pJunkcode = 813495551.924674;
	pJunkcode = 7487730777.125;
	if (pJunkcode = 1554453961.17868)
		pJunkcode = 4980573729.31705;
	pJunkcode = 6784381565.69082;
	if (pJunkcode = 4144587213.82521)
		pJunkcode = 7443420801.3447;
	pJunkcode = 6440149937.56052;
	pJunkcode = 1153661825.37646;
	if (pJunkcode = 1307453356.03307)
		pJunkcode = 3221710675.12532;
	pJunkcode = 6181508009.78003;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs394() {
	float pJunkcode = 2631834429.30129;
	pJunkcode = 3009371612.43358;
	if (pJunkcode = 3779942814.8358)
		pJunkcode = 8545265693.05857;
	pJunkcode = 930401040.195728;
	pJunkcode = 8982119700.91921;
	if (pJunkcode = 1330354072.06266)
		pJunkcode = 710420421.811197;
	pJunkcode = 3488646804.30586;
	if (pJunkcode = 2582097523.51102)
		pJunkcode = 5954539186.87975;
	pJunkcode = 6201989134.37596;
	pJunkcode = 2680926505.43085;
	if (pJunkcode = 1078602688.96105)
		pJunkcode = 4667406837.32044;
	pJunkcode = 4374877868.14871;
	if (pJunkcode = 885752587.923736)
		pJunkcode = 3769373575.78995;
	pJunkcode = 2126691685.63686;
	pJunkcode = 8104548642.02746;
	if (pJunkcode = 6715127707.62606)
		pJunkcode = 8460312976.05525;
	pJunkcode = 2587173001.50729;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs393() {
	float pJunkcode = 4839513823.70001;
	pJunkcode = 6175587011.1001;
	if (pJunkcode = 6018779052.24975)
		pJunkcode = 7617841449.25931;
	pJunkcode = 6097332105.82452;
	pJunkcode = 4756191489.30303;
	if (pJunkcode = 6457614443.0435)
		pJunkcode = 4269263891.93847;
	pJunkcode = 8373391667.70646;
	if (pJunkcode = 7441034280.84494)
		pJunkcode = 9058118232.06674;
	pJunkcode = 3960133518.83352;
	pJunkcode = 8778705615.77111;
	if (pJunkcode = 8565817222.60877)
		pJunkcode = 6152596171.88622;
	pJunkcode = 4290946771.52535;
	if (pJunkcode = 1691870052.43867)
		pJunkcode = 7935636858.73525;
	pJunkcode = 6489312181.15948;
	pJunkcode = 1797021966.88746;
	if (pJunkcode = 9980726512.96656)
		pJunkcode = 5759157967.31745;
	pJunkcode = 1785870927.13414;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs392() {
	float pJunkcode = 1473023768.94983;
	pJunkcode = 7221430313.48268;
	if (pJunkcode = 4489241031.86797)
		pJunkcode = 2082796132.61641;
	pJunkcode = 9753354454.70697;
	pJunkcode = 6169001236.35769;
	if (pJunkcode = 566914820.049785)
		pJunkcode = 433498207.024777;
	pJunkcode = 7377921979.72322;
	if (pJunkcode = 2621634940.24643)
		pJunkcode = 7684908136.75618;
	pJunkcode = 3901591888.99619;
	pJunkcode = 1213134943.09632;
	if (pJunkcode = 9277723070.2069)
		pJunkcode = 7548595791.40782;
	pJunkcode = 2044640021.08389;
	if (pJunkcode = 3306831466.2329)
		pJunkcode = 6771867092.63609;
	pJunkcode = 5440783914.286;
	pJunkcode = 3464197242.14595;
	if (pJunkcode = 475924119.779499)
		pJunkcode = 3158000240.38981;
	pJunkcode = 9781272828.149;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs391() {
	float pJunkcode = 6206708911.24177;
	pJunkcode = 1446530416.55518;
	if (pJunkcode = 8637987488.15128)
		pJunkcode = 2711919844.76165;
	pJunkcode = 573901576.373412;
	pJunkcode = 2982343292.74709;
	if (pJunkcode = 904458317.453688)
		pJunkcode = 5506906711.13597;
	pJunkcode = 2236797173.83257;
	if (pJunkcode = 6726639242.44485)
		pJunkcode = 5516501898.95497;
	pJunkcode = 5730049003.00218;
	pJunkcode = 8993702870.02577;
	if (pJunkcode = 323942971.2998)
		pJunkcode = 7019818454.53561;
	pJunkcode = 9727656676.66579;
	if (pJunkcode = 8426902990.56898)
		pJunkcode = 7626498278.43219;
	pJunkcode = 5040875047.6222;
	pJunkcode = 1852653450.00043;
	if (pJunkcode = 7787622098.93036)
		pJunkcode = 4060899902.41769;
	pJunkcode = 9253532774.01753;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs390() {
	float pJunkcode = 400904843.571678;
	pJunkcode = 4272246767.24368;
	if (pJunkcode = 1537254875.31861)
		pJunkcode = 8443679830.6378;
	pJunkcode = 6691957080.71537;
	pJunkcode = 5886285352.04043;
	if (pJunkcode = 557239460.128687)
		pJunkcode = 8933558713.16175;
	pJunkcode = 2557387640.66832;
	if (pJunkcode = 3572651474.67686)
		pJunkcode = 2184692129.33805;
	pJunkcode = 1496195988.4062;
	pJunkcode = 7497514970.31382;
	if (pJunkcode = 7497657477.54078)
		pJunkcode = 6379813180.4575;
	pJunkcode = 7621410318.50994;
	if (pJunkcode = 6375891003.5515)
		pJunkcode = 2362448122.0313;
	pJunkcode = 2131935684.76776;
	pJunkcode = 6624786505.2242;
	if (pJunkcode = 2536788406.9246)
		pJunkcode = 1039915707.91504;
	pJunkcode = 7054040512.58977;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs389() {
	float pJunkcode = 8453649012.90799;
	pJunkcode = 3169402031.58217;
	if (pJunkcode = 4332580141.35128)
		pJunkcode = 448740510.591541;
	pJunkcode = 1234527399.62752;
	pJunkcode = 2221131185.59557;
	if (pJunkcode = 278541037.705473)
		pJunkcode = 4728501867.35253;
	pJunkcode = 8216189514.22029;
	if (pJunkcode = 9529935753.72138)
		pJunkcode = 6892097364.59341;
	pJunkcode = 6163573102.62534;
	pJunkcode = 8094225211.4755;
	if (pJunkcode = 4919965813.37315)
		pJunkcode = 1500894165.7892;
	pJunkcode = 4330575335.88965;
	if (pJunkcode = 742660121.381602)
		pJunkcode = 1822690336.38413;
	pJunkcode = 7519800414.23733;
	pJunkcode = 1924252583.02888;
	if (pJunkcode = 5907981970.8262)
		pJunkcode = 4780491352.16931;
	pJunkcode = 2758072773.0719;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs388() {
	float pJunkcode = 8092111006.97335;
	pJunkcode = 4239030355.21877;
	if (pJunkcode = 9551401276.43449)
		pJunkcode = 5262772159.68836;
	pJunkcode = 2850770107.2218;
	pJunkcode = 8494407269.57137;
	if (pJunkcode = 2928040134.75386)
		pJunkcode = 4753160383.6123;
	pJunkcode = 2921489719.6477;
	if (pJunkcode = 3372666557.13364)
		pJunkcode = 9483311089.63316;
	pJunkcode = 4062120977.50226;
	pJunkcode = 8238744056.45409;
	if (pJunkcode = 1795554321.96039)
		pJunkcode = 7752609300.03857;
	pJunkcode = 9855151736.87948;
	if (pJunkcode = 5736370478.02589)
		pJunkcode = 192559848.184869;
	pJunkcode = 1338923765.23853;
	pJunkcode = 6625131764.00472;
	if (pJunkcode = 304905566.193253)
		pJunkcode = 9481804825.47344;
	pJunkcode = 1229036039.03786;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs387() {
	float pJunkcode = 1887589084.93196;
	pJunkcode = 7424692841.3672;
	if (pJunkcode = 4969552645.01621)
		pJunkcode = 308012552.960229;
	pJunkcode = 5989324282.00744;
	pJunkcode = 1243059577.17694;
	if (pJunkcode = 6179972018.10101)
		pJunkcode = 7197563266.81317;
	pJunkcode = 7772701519.85799;
	if (pJunkcode = 982196637.232925)
		pJunkcode = 3616834316.88387;
	pJunkcode = 829527678.985512;
	pJunkcode = 6520898343.29018;
	if (pJunkcode = 6153943817.09956)
		pJunkcode = 2448807680.84714;
	pJunkcode = 808664990.689866;
	if (pJunkcode = 2620725247.4225)
		pJunkcode = 494951657.622042;
	pJunkcode = 5758720468.94326;
	pJunkcode = 4844970730.35557;
	if (pJunkcode = 5331605741.6894)
		pJunkcode = 4028616112.14922;
	pJunkcode = 8481026959.23418;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs386() {
	float pJunkcode = 1234691517.97871;
	pJunkcode = 9981220549.04057;
	if (pJunkcode = 1094568422.69949)
		pJunkcode = 2343070501.20556;
	pJunkcode = 4331357473.72103;
	pJunkcode = 6658891185.21147;
	if (pJunkcode = 2030577623.89898)
		pJunkcode = 5163515302.1886;
	pJunkcode = 434411629.758324;
	if (pJunkcode = 1051123229.48938)
		pJunkcode = 3268198615.44193;
	pJunkcode = 7026870295.68651;
	pJunkcode = 216560324.105325;
	if (pJunkcode = 105712106.598876)
		pJunkcode = 8541819212.34017;
	pJunkcode = 4448665550.08868;
	if (pJunkcode = 4531690097.07036)
		pJunkcode = 7821132261.12757;
	pJunkcode = 4058975147.98744;
	pJunkcode = 1924041451.98426;
	if (pJunkcode = 5674468155.39122)
		pJunkcode = 8378167253.51946;
	pJunkcode = 260844060.134494;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs385() {
	float pJunkcode = 3924931681.97595;
	pJunkcode = 7126098000.7529;
	if (pJunkcode = 5032108200.08548)
		pJunkcode = 3873949344.93105;
	pJunkcode = 3336760056.06661;
	pJunkcode = 2456962052.54585;
	if (pJunkcode = 7306847695.26803)
		pJunkcode = 2218782951.38399;
	pJunkcode = 1374584201.70051;
	if (pJunkcode = 9903362315.69409)
		pJunkcode = 9067731850.57518;
	pJunkcode = 6737830624.36037;
	pJunkcode = 3045798349.74494;
	if (pJunkcode = 6209037881.23854)
		pJunkcode = 692443435.595077;
	pJunkcode = 300650996.599443;
	if (pJunkcode = 2904645603.83716)
		pJunkcode = 7533830650.84705;
	pJunkcode = 6738179645.64645;
	pJunkcode = 7109511318.55405;
	if (pJunkcode = 1068315502.79295)
		pJunkcode = 2334464002.5296;
	pJunkcode = 5056814739.51357;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs384() {
	float pJunkcode = 3250860836.0827;
	pJunkcode = 7187115671.24894;
	if (pJunkcode = 8951322589.29257)
		pJunkcode = 4192726153.69548;
	pJunkcode = 9658218202.04496;
	pJunkcode = 7767960476.45386;
	if (pJunkcode = 9271070517.97537)
		pJunkcode = 7889732293.04552;
	pJunkcode = 5305662166.79573;
	if (pJunkcode = 4527278286.8738)
		pJunkcode = 8246401365.3107;
	pJunkcode = 4334045851.47504;
	pJunkcode = 7251531450.05258;
	if (pJunkcode = 6925102238.12414)
		pJunkcode = 9761698432.47255;
	pJunkcode = 8987279353.33631;
	if (pJunkcode = 2315257005.27817)
		pJunkcode = 9153780539.35268;
	pJunkcode = 6126600024.35894;
	pJunkcode = 4058345265.90458;
	if (pJunkcode = 4668992901.05696)
		pJunkcode = 9270942744.12421;
	pJunkcode = 6958003593.8576;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs383() {
	float pJunkcode = 9202449100.77666;
	pJunkcode = 3070302375.10436;
	if (pJunkcode = 2326080285.62716)
		pJunkcode = 4194414426.73414;
	pJunkcode = 1492328940.5887;
	pJunkcode = 1013414818.86783;
	if (pJunkcode = 9237314209.6832)
		pJunkcode = 3020942.97333282;
	pJunkcode = 2018859880.02616;
	if (pJunkcode = 7330827889.51393)
		pJunkcode = 6281048603.50625;
	pJunkcode = 8109795190.6284;
	pJunkcode = 8171925875.9313;
	if (pJunkcode = 6394826087.59831)
		pJunkcode = 4905660301.51794;
	pJunkcode = 3717821112.04931;
	if (pJunkcode = 2698216885.60165)
		pJunkcode = 5029522671.91148;
	pJunkcode = 7444946227.27477;
	pJunkcode = 5036981415.46765;
	if (pJunkcode = 9663244129.51916)
		pJunkcode = 2336318813.00565;
	pJunkcode = 2427468828.37898;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs382() {
	float pJunkcode = 5676170679.85107;
	pJunkcode = 419880321.002916;
	if (pJunkcode = 1665947769.69443)
		pJunkcode = 4934089537.53632;
	pJunkcode = 9539152846.98256;
	pJunkcode = 2712734375.99654;
	if (pJunkcode = 2246711499.42339)
		pJunkcode = 5231955707.01356;
	pJunkcode = 3075874366.4679;
	if (pJunkcode = 465043507.786143)
		pJunkcode = 823133072.587966;
	pJunkcode = 4266998262.88488;
	pJunkcode = 2817326915.7612;
	if (pJunkcode = 1767159276.78012)
		pJunkcode = 6947255568.27669;
	pJunkcode = 6358428365.35161;
	if (pJunkcode = 1737286977.84822)
		pJunkcode = 240420278.824349;
	pJunkcode = 7262185727.31233;
	pJunkcode = 2429888494.3177;
	if (pJunkcode = 9480141553.53927)
		pJunkcode = 4987180125.66335;
	pJunkcode = 8954777537.9768;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs381() {
	float pJunkcode = 5058259986.3567;
	pJunkcode = 7031566540.59449;
	if (pJunkcode = 7233782428.36828)
		pJunkcode = 1649529992.79694;
	pJunkcode = 2435402395.01657;
	pJunkcode = 2972289505.32368;
	if (pJunkcode = 782993897.101899)
		pJunkcode = 1124682336.88734;
	pJunkcode = 9992416566.56784;
	if (pJunkcode = 9037765782.36756)
		pJunkcode = 9402541317.24901;
	pJunkcode = 5648674629.03364;
	pJunkcode = 4861473838.83063;
	if (pJunkcode = 1894302852.38415)
		pJunkcode = 9079954179.97297;
	pJunkcode = 3614000665.90252;
	if (pJunkcode = 7217650513.41978)
		pJunkcode = 5721358434.67813;
	pJunkcode = 9359282277.26184;
	pJunkcode = 5825292559.24988;
	if (pJunkcode = 3305727920.55363)
		pJunkcode = 285722673.657309;
	pJunkcode = 6073782923.35594;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs380() {
	float pJunkcode = 3285962789.59299;
	pJunkcode = 3930069362.38494;
	if (pJunkcode = 121755758.661322)
		pJunkcode = 8420083856.66269;
	pJunkcode = 1040111400.11407;
	pJunkcode = 228017100.032631;
	if (pJunkcode = 9735398935.26153)
		pJunkcode = 9564697659.14886;
	pJunkcode = 2360236958.50552;
	if (pJunkcode = 2282299526.2793)
		pJunkcode = 9744143524.4113;
	pJunkcode = 9839587988.21229;
	pJunkcode = 250273012.012628;
	if (pJunkcode = 9281073253.13408)
		pJunkcode = 2858189735.62099;
	pJunkcode = 8750302784.81198;
	if (pJunkcode = 6496510573.76517)
		pJunkcode = 1256216058.01319;
	pJunkcode = 4114178946.43971;
	pJunkcode = 542313602.838413;
	if (pJunkcode = 8354856086.78107)
		pJunkcode = 1607159579.32774;
	pJunkcode = 9006064060.55911;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs379() {
	float pJunkcode = 8259047293.23226;
	pJunkcode = 4995410646.19087;
	if (pJunkcode = 5251836237.00041)
		pJunkcode = 3499123717.61155;
	pJunkcode = 2391912584.91639;
	pJunkcode = 6135091352.18044;
	if (pJunkcode = 1805812262.56381)
		pJunkcode = 3436924094.17277;
	pJunkcode = 2956124212.1079;
	if (pJunkcode = 2696237379.26476)
		pJunkcode = 3645695896.45753;
	pJunkcode = 3405124260.84053;
	pJunkcode = 3434856711.44483;
	if (pJunkcode = 4680126485.67148)
		pJunkcode = 273723803.563434;
	pJunkcode = 5202369856.758;
	if (pJunkcode = 2569346697.12028)
		pJunkcode = 1296506408.9244;
	pJunkcode = 5655723760.06295;
	pJunkcode = 8058735973.67378;
	if (pJunkcode = 9710256035.9897)
		pJunkcode = 5756434865.5485;
	pJunkcode = 807795151.088665;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs378() {
	float pJunkcode = 6607240509.83086;
	pJunkcode = 8354802678.29382;
	if (pJunkcode = 1264973406.15465)
		pJunkcode = 8931559961.23632;
	pJunkcode = 521528446.371771;
	pJunkcode = 5936244101.3061;
	if (pJunkcode = 181503606.808415)
		pJunkcode = 4276256299.00531;
	pJunkcode = 9641525697.04852;
	if (pJunkcode = 5633262803.17049)
		pJunkcode = 2090637205.87746;
	pJunkcode = 1638544454.395;
	pJunkcode = 5187343147.55466;
	if (pJunkcode = 2758676683.15619)
		pJunkcode = 9702081838.95994;
	pJunkcode = 2755174968.57655;
	if (pJunkcode = 708960259.98619)
		pJunkcode = 4950019199.18386;
	pJunkcode = 9378785543.88796;
	pJunkcode = 4023266304.19042;
	if (pJunkcode = 8736343323.35356)
		pJunkcode = 8156873224.99707;
	pJunkcode = 9188224524.70043;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs377() {
	float pJunkcode = 5234848817.69008;
	pJunkcode = 296766160.69865;
	if (pJunkcode = 4660335268.62639)
		pJunkcode = 8016034953.30996;
	pJunkcode = 250521064.971491;
	pJunkcode = 4245792918.82929;
	if (pJunkcode = 5950028084.71983)
		pJunkcode = 7237754798.4025;
	pJunkcode = 5420374833.75568;
	if (pJunkcode = 1629809352.99907)
		pJunkcode = 4664613973.20337;
	pJunkcode = 6936124449.31403;
	pJunkcode = 7074833922.82019;
	if (pJunkcode = 2880651106.94785)
		pJunkcode = 7195680156.05539;
	pJunkcode = 3483400961.1747;
	if (pJunkcode = 2851200932.73647)
		pJunkcode = 502333223.248248;
	pJunkcode = 6091930898.68684;
	pJunkcode = 6791514633.65608;
	if (pJunkcode = 1336026833.84936)
		pJunkcode = 4384469342.35608;
	pJunkcode = 8250328277.94464;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs376() {
	float pJunkcode = 1054247757.54163;
	pJunkcode = 23624355.9853165;
	if (pJunkcode = 2200485781.99691)
		pJunkcode = 3462104786.13674;
	pJunkcode = 9168904817.58984;
	pJunkcode = 4079708341.83067;
	if (pJunkcode = 5847411626.38754)
		pJunkcode = 2391849807.41773;
	pJunkcode = 8304538029.87433;
	if (pJunkcode = 3471589592.37973)
		pJunkcode = 377964730.662854;
	pJunkcode = 3227535746.21105;
	pJunkcode = 1515974001.75753;
	if (pJunkcode = 7092928557.18404)
		pJunkcode = 7461300935.63821;
	pJunkcode = 6632692451.43875;
	if (pJunkcode = 3551926227.26403)
		pJunkcode = 9389354540.79236;
	pJunkcode = 8651552296.30209;
	pJunkcode = 9584904255.87818;
	if (pJunkcode = 850228749.506075)
		pJunkcode = 2422120536.13252;
	pJunkcode = 1006251716.04989;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs375() {
	float pJunkcode = 8812012432.70258;
	pJunkcode = 8644273480.13972;
	if (pJunkcode = 1280480246.925)
		pJunkcode = 8682282448.83353;
	pJunkcode = 7500726576.156;
	pJunkcode = 2884390650.99503;
	if (pJunkcode = 7065610904.07838)
		pJunkcode = 518577631.525582;
	pJunkcode = 1604251523.40166;
	if (pJunkcode = 3972342388.4264)
		pJunkcode = 9638441825.47669;
	pJunkcode = 9368168935.92135;
	pJunkcode = 2916510555.74063;
	if (pJunkcode = 5804773227.52741)
		pJunkcode = 3679402551.78319;
	pJunkcode = 7378158530.96331;
	if (pJunkcode = 4203357413.54797)
		pJunkcode = 1426067750.58809;
	pJunkcode = 4645922401.96327;
	pJunkcode = 8706696800.3673;
	if (pJunkcode = 8838710084.95907)
		pJunkcode = 4400039433.26658;
	pJunkcode = 1314348696.97525;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs374() {
	float pJunkcode = 5009117002.31599;
	pJunkcode = 8953017109.11493;
	if (pJunkcode = 8374233020.76329)
		pJunkcode = 5123401071.92824;
	pJunkcode = 9527833290.75117;
	pJunkcode = 6730751712.26125;
	if (pJunkcode = 5909813565.99093)
		pJunkcode = 3409817509.24809;
	pJunkcode = 7110264366.81572;
	if (pJunkcode = 5556716254.70406)
		pJunkcode = 9617127615.55742;
	pJunkcode = 9885005521.30375;
	pJunkcode = 7975531739.11146;
	if (pJunkcode = 4544794028.55585)
		pJunkcode = 7327395508.87831;
	pJunkcode = 849580816.243926;
	if (pJunkcode = 7986076149.34577)
		pJunkcode = 1488362465.76742;
	pJunkcode = 8576217903.26746;
	pJunkcode = 3602715519.36785;
	if (pJunkcode = 4480078930.32658)
		pJunkcode = 6890661649.74151;
	pJunkcode = 489920125.970296;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs373() {
	float pJunkcode = 7658757023.35879;
	pJunkcode = 3724247571.68867;
	if (pJunkcode = 470762763.059128)
		pJunkcode = 4151246844.03752;
	pJunkcode = 3946204339.02579;
	pJunkcode = 7967234147.73115;
	if (pJunkcode = 5628794583.32969)
		pJunkcode = 6757905775.536;
	pJunkcode = 3596696187.97723;
	if (pJunkcode = 5558327002.89857)
		pJunkcode = 951414827.08317;
	pJunkcode = 7215614804.65586;
	pJunkcode = 7602175908.05267;
	if (pJunkcode = 6979670194.08868)
		pJunkcode = 1906767013.56822;
	pJunkcode = 4916072707.4106;
	if (pJunkcode = 8852588298.64858)
		pJunkcode = 624330494.482488;
	pJunkcode = 9307888316.05343;
	pJunkcode = 1379824505.84604;
	if (pJunkcode = 2404766540.28112)
		pJunkcode = 8590215208.64026;
	pJunkcode = 7986861774.94532;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs372() {
	float pJunkcode = 454419426.030612;
	pJunkcode = 3468920110.97179;
	if (pJunkcode = 5244319433.04725)
		pJunkcode = 9446141350.60194;
	pJunkcode = 6582810090.81505;
	pJunkcode = 8323687516.2036;
	if (pJunkcode = 5114363321.66179)
		pJunkcode = 3977949089.83519;
	pJunkcode = 6109637161.45103;
	if (pJunkcode = 3439506302.21499)
		pJunkcode = 5201225861.81551;
	pJunkcode = 3735649325.6285;
	pJunkcode = 467805401.080712;
	if (pJunkcode = 7020440394.84464)
		pJunkcode = 1314084257.55956;
	pJunkcode = 4189534410.43688;
	if (pJunkcode = 547100271.279217)
		pJunkcode = 6525061648.84095;
	pJunkcode = 9928022531.60056;
	pJunkcode = 5645785497.39303;
	if (pJunkcode = 5119582498.96599)
		pJunkcode = 1773871956.92408;
	pJunkcode = 7690695443.08916;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs371() {
	float pJunkcode = 9968037104.35712;
	pJunkcode = 9686251811.53337;
	if (pJunkcode = 8779742581.86534)
		pJunkcode = 7726070974.52653;
	pJunkcode = 4102591199.92847;
	pJunkcode = 4634812707.86218;
	if (pJunkcode = 2471918342.82522)
		pJunkcode = 8401096078.91156;
	pJunkcode = 7075428931.87798;
	if (pJunkcode = 8978737830.05832)
		pJunkcode = 5612000935.38479;
	pJunkcode = 1663050025.73444;
	pJunkcode = 2215902376.18396;
	if (pJunkcode = 375527944.858105)
		pJunkcode = 938994318.879941;
	pJunkcode = 2979621986.08445;
	if (pJunkcode = 7653707051.70447)
		pJunkcode = 5598081185.12383;
	pJunkcode = 3048897033.85419;
	pJunkcode = 2471209962.67585;
	if (pJunkcode = 3907924448.97599)
		pJunkcode = 7501265590.00396;
	pJunkcode = 703092523.111674;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs370() {
	float pJunkcode = 1319246317.42144;
	pJunkcode = 4672520899.62512;
	if (pJunkcode = 6120261739.13087)
		pJunkcode = 4043485669.22456;
	pJunkcode = 8075619735.48985;
	pJunkcode = 2717494406.54979;
	if (pJunkcode = 5720713327.95173)
		pJunkcode = 9236123639.7645;
	pJunkcode = 1454109599.87058;
	if (pJunkcode = 5693603599.84479)
		pJunkcode = 8451801871.79665;
	pJunkcode = 9415694461.6555;
	pJunkcode = 2423827922.04758;
	if (pJunkcode = 4822449994.79316)
		pJunkcode = 5287337463.13209;
	pJunkcode = 1527548863.5498;
	if (pJunkcode = 7307165433.74167)
		pJunkcode = 9205175415.57637;
	pJunkcode = 1560572679.99227;
	pJunkcode = 4195661930.16078;
	if (pJunkcode = 1134756534.04087)
		pJunkcode = 1829294232.15433;
	pJunkcode = 4753617034.44371;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs369() {
	float pJunkcode = 1730777005.02043;
	pJunkcode = 4563449788.47276;
	if (pJunkcode = 8205320061.68809)
		pJunkcode = 4219106749.6653;
	pJunkcode = 4101378290.13292;
	pJunkcode = 2633208663.50824;
	if (pJunkcode = 4874898756.74475)
		pJunkcode = 9459138944.73925;
	pJunkcode = 8306397571.42175;
	if (pJunkcode = 2281216031.77454)
		pJunkcode = 1568311555.59259;
	pJunkcode = 6439428591.20227;
	pJunkcode = 4564935722.06399;
	if (pJunkcode = 8642879017.80873)
		pJunkcode = 2831870489.91226;
	pJunkcode = 9443224824.38175;
	if (pJunkcode = 4978334922.08539)
		pJunkcode = 3683940699.28021;
	pJunkcode = 7405276945.89469;
	pJunkcode = 1283367620.71183;
	if (pJunkcode = 4981086882.92402)
		pJunkcode = 9823564776.14765;
	pJunkcode = 6148428403.01664;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs368() {
	float pJunkcode = 929016526.618653;
	pJunkcode = 1080975733.35792;
	if (pJunkcode = 5453993161.00335)
		pJunkcode = 7301785427.95037;
	pJunkcode = 5219530940.38045;
	pJunkcode = 7400229297.86696;
	if (pJunkcode = 1045420195.36512)
		pJunkcode = 677583582.803914;
	pJunkcode = 7871153014.9019;
	if (pJunkcode = 8574934204.35092)
		pJunkcode = 1777451065.96403;
	pJunkcode = 5320349733.6184;
	pJunkcode = 1090207369.07567;
	if (pJunkcode = 2364980984.44037)
		pJunkcode = 4921871273.32091;
	pJunkcode = 1853662192.64627;
	if (pJunkcode = 329422881.392915)
		pJunkcode = 2661466598.4219;
	pJunkcode = 6251351261.08361;
	pJunkcode = 3181838681.7725;
	if (pJunkcode = 4077268780.7124)
		pJunkcode = 4703764829.74341;
	pJunkcode = 4693722616.21556;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs367() {
	float pJunkcode = 7932640491.02917;
	pJunkcode = 308221537.327781;
	if (pJunkcode = 7409429494.7773)
		pJunkcode = 2076293174.90577;
	pJunkcode = 3899387728.94613;
	pJunkcode = 9181627632.40596;
	if (pJunkcode = 3153472162.71742)
		pJunkcode = 8988334156.40778;
	pJunkcode = 5183388512.05322;
	if (pJunkcode = 6047358266.95573)
		pJunkcode = 9436238929.52289;
	pJunkcode = 5085436422.10426;
	pJunkcode = 4681243041.79702;
	if (pJunkcode = 271351082.79793)
		pJunkcode = 409983634.485271;
	pJunkcode = 9065037840.70422;
	if (pJunkcode = 3947908777.33438)
		pJunkcode = 2470629910.50903;
	pJunkcode = 4427325314.62032;
	pJunkcode = 6663597534.86834;
	if (pJunkcode = 2101596364.83127)
		pJunkcode = 7786999071.3131;
	pJunkcode = 6209887248.24597;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs366() {
	float pJunkcode = 1117985503.03852;
	pJunkcode = 9699598773.87096;
	if (pJunkcode = 1897273811.53618)
		pJunkcode = 4110549686.18065;
	pJunkcode = 4306505539.99487;
	pJunkcode = 8025506916.74307;
	if (pJunkcode = 5304120335.78256)
		pJunkcode = 3494751972.51149;
	pJunkcode = 9249038908.13779;
	if (pJunkcode = 6223863848.19669)
		pJunkcode = 2347132288.95428;
	pJunkcode = 7070888758.77461;
	pJunkcode = 749295181.221799;
	if (pJunkcode = 6314063872.82127)
		pJunkcode = 5220794960.64778;
	pJunkcode = 2096313050.31105;
	if (pJunkcode = 3882610874.90762)
		pJunkcode = 2260615552.39933;
	pJunkcode = 9907998621.28057;
	pJunkcode = 9256226960.838;
	if (pJunkcode = 6962884422.72569)
		pJunkcode = 7425478497.23443;
	pJunkcode = 1342493339.95896;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs365() {
	float pJunkcode = 3568697849.6503;
	pJunkcode = 8947530968.5361;
	if (pJunkcode = 8911191856.16122)
		pJunkcode = 3977923306.42755;
	pJunkcode = 1164367629.87067;
	pJunkcode = 4881736628.67386;
	if (pJunkcode = 7103942556.51925)
		pJunkcode = 6366876603.63679;
	pJunkcode = 3025900881.49815;
	if (pJunkcode = 3971140116.55658)
		pJunkcode = 7363899235.52869;
	pJunkcode = 4102107324.48215;
	pJunkcode = 4526489222.40003;
	if (pJunkcode = 55825503.8432697)
		pJunkcode = 648479917.661105;
	pJunkcode = 2184749672.85618;
	if (pJunkcode = 572498937.111989)
		pJunkcode = 4286521764.61988;
	pJunkcode = 7243718637.49245;
	pJunkcode = 6712672970.7582;
	if (pJunkcode = 9000825220.90368)
		pJunkcode = 8162725883.90988;
	pJunkcode = 8942597534.43845;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs364() {
	float pJunkcode = 1070704721.77287;
	pJunkcode = 8506076671.30108;
	if (pJunkcode = 4471377300.32927)
		pJunkcode = 400159739.412128;
	pJunkcode = 8598650215.29228;
	pJunkcode = 7585199822.82473;
	if (pJunkcode = 6240246046.59932)
		pJunkcode = 3306517879.08813;
	pJunkcode = 5555362677.02524;
	if (pJunkcode = 7640286685.58391)
		pJunkcode = 8593295736.79748;
	pJunkcode = 7562874732.89568;
	pJunkcode = 8566202919.01109;
	if (pJunkcode = 4847194392.10532)
		pJunkcode = 2279195063.37126;
	pJunkcode = 8587898469.11489;
	if (pJunkcode = 1714305491.34776)
		pJunkcode = 9049325645.35976;
	pJunkcode = 8151402532.57696;
	pJunkcode = 5315482257.4542;
	if (pJunkcode = 9815675286.23843)
		pJunkcode = 3711147307.24916;
	pJunkcode = 9385431935.48226;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs363() {
	float pJunkcode = 6930011383.57194;
	pJunkcode = 3613735171.77455;
	if (pJunkcode = 7918269580.02847)
		pJunkcode = 9980485568.25617;
	pJunkcode = 7618781828.8552;
	pJunkcode = 8535124918.64827;
	if (pJunkcode = 9796389856.11242)
		pJunkcode = 9633833133.3284;
	pJunkcode = 6173999.47906174;
	if (pJunkcode = 1817975712.00894)
		pJunkcode = 2455588314.26321;
	pJunkcode = 8130991603.38982;
	pJunkcode = 392004585.410277;
	if (pJunkcode = 4996734123.82508)
		pJunkcode = 9256065105.57804;
	pJunkcode = 4608976243.30669;
	if (pJunkcode = 2786528165.02322)
		pJunkcode = 4341694038.11825;
	pJunkcode = 3951114853.50408;
	pJunkcode = 1438499075.99485;
	if (pJunkcode = 8912904753.12251)
		pJunkcode = 502953252.036805;
	pJunkcode = 8724816272.85584;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs362() {
	float pJunkcode = 7319712889.25554;
	pJunkcode = 2720121881.07307;
	if (pJunkcode = 3678140957.5869)
		pJunkcode = 886202840.633043;
	pJunkcode = 7515473451.33543;
	pJunkcode = 565336455.945654;
	if (pJunkcode = 9147470372.80025)
		pJunkcode = 7304232393.00907;
	pJunkcode = 1678736477.25208;
	if (pJunkcode = 7673131943.66308)
		pJunkcode = 9733414034.29258;
	pJunkcode = 6717035621.58427;
	pJunkcode = 4565467015.32502;
	if (pJunkcode = 5454974266.92816)
		pJunkcode = 8227529573.31313;
	pJunkcode = 2224962519.87778;
	if (pJunkcode = 8137260671.08803)
		pJunkcode = 3452824463.68997;
	pJunkcode = 5641954549.60582;
	pJunkcode = 5930204634.92277;
	if (pJunkcode = 937577179.07104)
		pJunkcode = 474833069.995908;
	pJunkcode = 2351955488.15776;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs361() {
	float pJunkcode = 7551309600.9987;
	pJunkcode = 7137989523.02371;
	if (pJunkcode = 1134500110.90407)
		pJunkcode = 4156154545.94977;
	pJunkcode = 4333832569.09679;
	pJunkcode = 5067256609.82953;
	if (pJunkcode = 5635404681.6462)
		pJunkcode = 7174299049.90456;
	pJunkcode = 362728911.364672;
	if (pJunkcode = 4184807325.09025)
		pJunkcode = 8117689688.67468;
	pJunkcode = 5650847846.2006;
	pJunkcode = 9556208857.51458;
	if (pJunkcode = 3611488825.56204)
		pJunkcode = 8254959554.43951;
	pJunkcode = 4032020618.57893;
	if (pJunkcode = 9289257536.28646)
		pJunkcode = 7485627899.72039;
	pJunkcode = 8462583471.22063;
	pJunkcode = 6074960387.36877;
	if (pJunkcode = 2598918309.92436)
		pJunkcode = 3373873676.48898;
	pJunkcode = 6952760966.08549;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs360() {
	float pJunkcode = 3976796198.52277;
	pJunkcode = 8032300842.32766;
	if (pJunkcode = 457429984.622513)
		pJunkcode = 1827278218.3376;
	pJunkcode = 7122999887.07302;
	pJunkcode = 5229647414.49948;
	if (pJunkcode = 2177589072.94263)
		pJunkcode = 2737333553.63187;
	pJunkcode = 6094488110.28112;
	if (pJunkcode = 3989126278.53661)
		pJunkcode = 7923851624.91867;
	pJunkcode = 2904654628.26267;
	pJunkcode = 5140293180.63941;
	if (pJunkcode = 8635204614.69721)
		pJunkcode = 3275313459.93494;
	pJunkcode = 5167003198.40773;
	if (pJunkcode = 4057792289.34255)
		pJunkcode = 8456158615.76835;
	pJunkcode = 1717753799.76844;
	pJunkcode = 850862550.089484;
	if (pJunkcode = 4594853715.88106)
		pJunkcode = 1663836782.8796;
	pJunkcode = 9344086192.44395;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs359() {
	float pJunkcode = 3607787472.34342;
	pJunkcode = 3526044931.61643;
	if (pJunkcode = 7488730742.14336)
		pJunkcode = 2166360324.41079;
	pJunkcode = 5028717234.95058;
	pJunkcode = 4640768900.87026;
	if (pJunkcode = 1969245232.38427)
		pJunkcode = 1371804175.73674;
	pJunkcode = 5400640303.42052;
	if (pJunkcode = 9679937649.64705)
		pJunkcode = 7362020632.3271;
	pJunkcode = 2409072970.05389;
	pJunkcode = 1875364972.06927;
	if (pJunkcode = 8963654082.57429)
		pJunkcode = 4555443139.35528;
	pJunkcode = 7965922860.98752;
	if (pJunkcode = 1739622292.41658)
		pJunkcode = 3446117208.44576;
	pJunkcode = 6940572534.16212;
	pJunkcode = 8295509092.15962;
	if (pJunkcode = 1313279579.61897)
		pJunkcode = 163663078.939624;
	pJunkcode = 743891932.179788;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs358() {
	float pJunkcode = 8879881986.73574;
	pJunkcode = 628070259.716568;
	if (pJunkcode = 8250057398.49774)
		pJunkcode = 2494652474.85176;
	pJunkcode = 6523385180.19788;
	pJunkcode = 9499422350.38181;
	if (pJunkcode = 5616416808.43546)
		pJunkcode = 4449002962.55497;
	pJunkcode = 2213930669.59564;
	if (pJunkcode = 3526654918.51974)
		pJunkcode = 7106423907.99183;
	pJunkcode = 7346697120.57224;
	pJunkcode = 9004955213.37909;
	if (pJunkcode = 8296224040.39577)
		pJunkcode = 3780975480.59104;
	pJunkcode = 2578959833.6741;
	if (pJunkcode = 3134698321.59211)
		pJunkcode = 1096634236.16459;
	pJunkcode = 551722690.984974;
	pJunkcode = 6091071520.06184;
	if (pJunkcode = 5287252632.1354)
		pJunkcode = 1707308430.0429;
	pJunkcode = 6924472772.80397;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs357() {
	float pJunkcode = 4338125438.94847;
	pJunkcode = 2469181208.49371;
	if (pJunkcode = 9888228680.08694)
		pJunkcode = 2399055648.20123;
	pJunkcode = 9254097133.55589;
	pJunkcode = 2737725380.40373;
	if (pJunkcode = 9390518081.67193)
		pJunkcode = 1595281053.53349;
	pJunkcode = 2157646459.2855;
	if (pJunkcode = 3460541669.95898)
		pJunkcode = 4349738167.93794;
	pJunkcode = 986284757.86411;
	pJunkcode = 9045838685.11045;
	if (pJunkcode = 5736745764.67422)
		pJunkcode = 1289379401.48019;
	pJunkcode = 554670174.057971;
	if (pJunkcode = 6093702498.65127)
		pJunkcode = 5914940508.35801;
	pJunkcode = 2736171549.41919;
	pJunkcode = 2819676407.24568;
	if (pJunkcode = 5809405870.72449)
		pJunkcode = 2192721549.06477;
	pJunkcode = 7257600171.31374;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs356() {
	float pJunkcode = 2206932374.30071;
	pJunkcode = 2229969066.72301;
	if (pJunkcode = 1545564150.02379)
		pJunkcode = 6452503789.34536;
	pJunkcode = 3491309694.34418;
	pJunkcode = 6673550060.60084;
	if (pJunkcode = 263567131.635766)
		pJunkcode = 1770938573.93386;
	pJunkcode = 6539679406.74984;
	if (pJunkcode = 7150861825.25969)
		pJunkcode = 8288662370.42624;
	pJunkcode = 554232189.072599;
	pJunkcode = 3582622954.96252;
	if (pJunkcode = 4051233164.12911)
		pJunkcode = 2935179384.31089;
	pJunkcode = 5476288728.30201;
	if (pJunkcode = 7719085324.1374)
		pJunkcode = 2466858257.10889;
	pJunkcode = 9469048387.92029;
	pJunkcode = 1147607546.4557;
	if (pJunkcode = 7673726314.77129)
		pJunkcode = 4355509981.87483;
	pJunkcode = 2385487710.83785;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs355() {
	float pJunkcode = 3734708205.63857;
	pJunkcode = 7679624230.85203;
	if (pJunkcode = 5651639923.47276)
		pJunkcode = 5862139365.40774;
	pJunkcode = 3211397023.36261;
	pJunkcode = 5759930575.48589;
	if (pJunkcode = 9789557108.73453)
		pJunkcode = 9382219805.77637;
	pJunkcode = 465191407.316887;
	if (pJunkcode = 7821129532.37972)
		pJunkcode = 2619171355.90259;
	pJunkcode = 3260158851.15315;
	pJunkcode = 8059999828.63301;
	if (pJunkcode = 1092277224.44032)
		pJunkcode = 6914398964.47372;
	pJunkcode = 9291571686.76097;
	if (pJunkcode = 2735086329.65661)
		pJunkcode = 3457977180.15156;
	pJunkcode = 7283291087.90447;
	pJunkcode = 3481215124.71306;
	if (pJunkcode = 7301622068.61949)
		pJunkcode = 1501495626.51961;
	pJunkcode = 1337462429.95882;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs354() {
	float pJunkcode = 7834794042.27172;
	pJunkcode = 8227190177.1496;
	if (pJunkcode = 9211402869.86978)
		pJunkcode = 1487488342.12522;
	pJunkcode = 8145423769.192;
	pJunkcode = 6070689484.86864;
	if (pJunkcode = 2248365914.56844)
		pJunkcode = 8556800636.43618;
	pJunkcode = 1393106213.39169;
	if (pJunkcode = 9516334172.66644)
		pJunkcode = 6688314274.50492;
	pJunkcode = 3214511167.272;
	pJunkcode = 3599669340.37199;
	if (pJunkcode = 9492983546.18517)
		pJunkcode = 1626918957.10071;
	pJunkcode = 1331404016.00718;
	if (pJunkcode = 4623859909.77795)
		pJunkcode = 6069826243.62189;
	pJunkcode = 5700251041.49479;
	pJunkcode = 1519687548.19502;
	if (pJunkcode = 7603633480.82681)
		pJunkcode = 5278015799.68965;
	pJunkcode = 2597368001.63222;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs353() {
	float pJunkcode = 573618671.344028;
	pJunkcode = 2152147508.49276;
	if (pJunkcode = 3003897266.9372)
		pJunkcode = 662011643.236598;
	pJunkcode = 3238904845.97965;
	pJunkcode = 7888260939.16339;
	if (pJunkcode = 600534632.742105)
		pJunkcode = 1523904224.98051;
	pJunkcode = 6366297359.38423;
	if (pJunkcode = 1850430125.85174)
		pJunkcode = 1480811636.28426;
	pJunkcode = 857089508.458581;
	pJunkcode = 5670976261.50992;
	if (pJunkcode = 3771428008.60699)
		pJunkcode = 3309051970.8307;
	pJunkcode = 9624851471.40312;
	if (pJunkcode = 4505268863.24639)
		pJunkcode = 1163389627.5473;
	pJunkcode = 6985934030.13464;
	pJunkcode = 2783874534.567;
	if (pJunkcode = 41366909.6990199)
		pJunkcode = 3033372176.8288;
	pJunkcode = 486687274.544339;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs352() {
	float pJunkcode = 5333384370.74068;
	pJunkcode = 9802255832.80487;
	if (pJunkcode = 6152613985.58062)
		pJunkcode = 2979761853.03832;
	pJunkcode = 4928623913.78863;
	pJunkcode = 9031441320.15657;
	if (pJunkcode = 5156581528.27641)
		pJunkcode = 678489575.985919;
	pJunkcode = 6638089372.20715;
	if (pJunkcode = 8148851602.69062)
		pJunkcode = 4676512510.59132;
	pJunkcode = 7322426133.07362;
	pJunkcode = 5772238930.28962;
	if (pJunkcode = 8539213579.44115)
		pJunkcode = 497512641.035034;
	pJunkcode = 1823956407.86892;
	if (pJunkcode = 5175418654.46612)
		pJunkcode = 1637783664.59185;
	pJunkcode = 6692590811.45028;
	pJunkcode = 5354298439.58895;
	if (pJunkcode = 5582173304.56518)
		pJunkcode = 6110905365.64319;
	pJunkcode = 2695489101.65528;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs351() {
	float pJunkcode = 6301511062.25614;
	pJunkcode = 592700909.870974;
	if (pJunkcode = 1849208086.85795)
		pJunkcode = 2501107274.35367;
	pJunkcode = 1543896270.41727;
	pJunkcode = 3951998890.97595;
	if (pJunkcode = 8356008761.56316)
		pJunkcode = 4873158565.32006;
	pJunkcode = 6042717893.85858;
	if (pJunkcode = 5878121234.14843)
		pJunkcode = 161276393.131766;
	pJunkcode = 9196039325.61437;
	pJunkcode = 2929429912.57914;
	if (pJunkcode = 5821679603.95774)
		pJunkcode = 9919224068.3069;
	pJunkcode = 3751930823.45647;
	if (pJunkcode = 1830056570.26578)
		pJunkcode = 8987443741.76835;
	pJunkcode = 2550165855.80179;
	pJunkcode = 4151627727.87856;
	if (pJunkcode = 1769699901.68522)
		pJunkcode = 8137532763.57045;
	pJunkcode = 4030747694.95682;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs350() {
	float pJunkcode = 6064552703.23435;
	pJunkcode = 3303023368.21656;
	if (pJunkcode = 1965193891.64205)
		pJunkcode = 4005357110.59487;
	pJunkcode = 6213244460.25999;
	pJunkcode = 9863650855.13641;
	if (pJunkcode = 8390512949.32841)
		pJunkcode = 2025963450.97215;
	pJunkcode = 3787496349.78399;
	if (pJunkcode = 8628556132.30464)
		pJunkcode = 7290754939.71388;
	pJunkcode = 5344481445.54707;
	pJunkcode = 1460857797.97405;
	if (pJunkcode = 8345371215.34615)
		pJunkcode = 1781513037.64712;
	pJunkcode = 9178123098.27542;
	if (pJunkcode = 7681919929.68254)
		pJunkcode = 4894326498.52194;
	pJunkcode = 5044682584.5223;
	pJunkcode = 7253561950.44709;
	if (pJunkcode = 4596873047.50129)
		pJunkcode = 643877893.71577;
	pJunkcode = 2488610091.12964;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs349() {
	float pJunkcode = 8290451039.03447;
	pJunkcode = 3514709163.54944;
	if (pJunkcode = 4950786806.20009)
		pJunkcode = 1917439819.3053;
	pJunkcode = 945181149.288892;
	pJunkcode = 7769612423.61876;
	if (pJunkcode = 7629766603.58027)
		pJunkcode = 5764666838.92471;
	pJunkcode = 3579290541.49236;
	if (pJunkcode = 4765398643.12343)
		pJunkcode = 3187772348.64107;
	pJunkcode = 7187860766.09535;
	pJunkcode = 3453940628.66393;
	if (pJunkcode = 359101057.388636)
		pJunkcode = 653827567.301417;
	pJunkcode = 8529977133.4904;
	if (pJunkcode = 9243262796.19021)
		pJunkcode = 4140514664.75175;
	pJunkcode = 5827396675.30113;
	pJunkcode = 8481213487.31072;
	if (pJunkcode = 7457100119.11756)
		pJunkcode = 6538935790.36323;
	pJunkcode = 7186473409.68455;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs348() {
	float pJunkcode = 453836628.817958;
	pJunkcode = 9869725173.59026;
	if (pJunkcode = 8985472645.19021)
		pJunkcode = 3567587123.54579;
	pJunkcode = 6176546938.96135;
	pJunkcode = 3253703404.38852;
	if (pJunkcode = 644013714.113515)
		pJunkcode = 1031985270.49001;
	pJunkcode = 6518859284.1268;
	if (pJunkcode = 4706797525.65154)
		pJunkcode = 6527490499.85671;
	pJunkcode = 9828501304.53361;
	pJunkcode = 6477823617.18228;
	if (pJunkcode = 9086776489.65748)
		pJunkcode = 8860402054.16339;
	pJunkcode = 6870922104.28444;
	if (pJunkcode = 2263453706.2803)
		pJunkcode = 599567878.35465;
	pJunkcode = 4861923546.72168;
	pJunkcode = 7828817479.11434;
	if (pJunkcode = 9136686391.5426)
		pJunkcode = 2930450108.04981;
	pJunkcode = 7681649534.51854;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs347() {
	float pJunkcode = 3334194907.35505;
	pJunkcode = 875844874.657077;
	if (pJunkcode = 684130701.061131)
		pJunkcode = 8349954356.91511;
	pJunkcode = 597244631.277869;
	pJunkcode = 6561745663.76902;
	if (pJunkcode = 8781113474.98597)
		pJunkcode = 6965009633.02231;
	pJunkcode = 4450127364.99473;
	if (pJunkcode = 9352650025.33113)
		pJunkcode = 4430471600.47864;
	pJunkcode = 1986017550.9272;
	pJunkcode = 8280568014.88237;
	if (pJunkcode = 7632427586.75469)
		pJunkcode = 8031030128.24627;
	pJunkcode = 8578297600.285;
	if (pJunkcode = 9349404582.54988)
		pJunkcode = 625884958.76005;
	pJunkcode = 6618447800.90517;
	pJunkcode = 2455819033.154;
	if (pJunkcode = 7426713171.82089)
		pJunkcode = 5793877949.67847;
	pJunkcode = 9662414371.31244;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs346() {
	float pJunkcode = 574667683.975287;
	pJunkcode = 2586999421.01336;
	if (pJunkcode = 1705369594.15613)
		pJunkcode = 2553489668.59398;
	pJunkcode = 6633445464.77617;
	pJunkcode = 5868206165.83529;
	if (pJunkcode = 8971481386.36876)
		pJunkcode = 5057179486.02407;
	pJunkcode = 5670665954.91611;
	if (pJunkcode = 9655890060.91699)
		pJunkcode = 3556075465.32799;
	pJunkcode = 1597161749.86164;
	pJunkcode = 2636457475.31522;
	if (pJunkcode = 1655256917.43463)
		pJunkcode = 2099394984.7513;
	pJunkcode = 7270161650.07704;
	if (pJunkcode = 5858599071.17625)
		pJunkcode = 7157323297.62112;
	pJunkcode = 3839741591.84859;
	pJunkcode = 6336450140.71845;
	if (pJunkcode = 87797222.6793026)
		pJunkcode = 6725004675.91368;
	pJunkcode = 9311719592.12857;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs345() {
	float pJunkcode = 8029620406.34749;
	pJunkcode = 1302742165.87553;
	if (pJunkcode = 7104045652.89727)
		pJunkcode = 7912074004.588;
	pJunkcode = 6897550110.60697;
	pJunkcode = 8213643653.00638;
	if (pJunkcode = 1244546956.63201)
		pJunkcode = 3058503504.53335;
	pJunkcode = 6651906265.90325;
	if (pJunkcode = 3582683478.68309)
		pJunkcode = 8695484182.18114;
	pJunkcode = 645817728.436008;
	pJunkcode = 8500282664.16918;
	if (pJunkcode = 3869151693.97861)
		pJunkcode = 4614171597.61767;
	pJunkcode = 7414087981.91738;
	if (pJunkcode = 6268931209.83018)
		pJunkcode = 3293001650.5653;
	pJunkcode = 3434831039.79479;
	pJunkcode = 358658385.569276;
	if (pJunkcode = 3001267473.4439)
		pJunkcode = 4265226427.76385;
	pJunkcode = 3417990723.74236;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs344() {
	float pJunkcode = 3332344711.53621;
	pJunkcode = 6740201880.63577;
	if (pJunkcode = 5275582585.47126)
		pJunkcode = 6361240309.74912;
	pJunkcode = 9584367351.45476;
	pJunkcode = 9232726448.80046;
	if (pJunkcode = 7730136713.37176)
		pJunkcode = 6341068197.22158;
	pJunkcode = 2071750463.97376;
	if (pJunkcode = 8857406552.57864)
		pJunkcode = 5234408278.57809;
	pJunkcode = 9028812169.26942;
	pJunkcode = 8999885763.30791;
	if (pJunkcode = 7075860078.69573)
		pJunkcode = 9010616641.0834;
	pJunkcode = 2267621028.11951;
	if (pJunkcode = 7798214548.63097)
		pJunkcode = 9568759512.08951;
	pJunkcode = 3506373292.8658;
	pJunkcode = 5401833290.09713;
	if (pJunkcode = 9551208124.39238)
		pJunkcode = 5669273280.70658;
	pJunkcode = 4691754045.82464;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs343() {
	float pJunkcode = 6267505676.56051;
	pJunkcode = 5252276538.62752;
	if (pJunkcode = 4952476641.51675)
		pJunkcode = 5635172509.90673;
	pJunkcode = 1224799909.44662;
	pJunkcode = 9030079267.52707;
	if (pJunkcode = 7148243158.63158)
		pJunkcode = 5936160306.39542;
	pJunkcode = 1103456059.23792;
	if (pJunkcode = 8758071928.09329)
		pJunkcode = 659482499.095726;
	pJunkcode = 5530418561.91247;
	pJunkcode = 8821497604.48602;
	if (pJunkcode = 4951067073.28466)
		pJunkcode = 5585656362.61192;
	pJunkcode = 5543935544.79904;
	if (pJunkcode = 9363313636.67804)
		pJunkcode = 7413761710.28598;
	pJunkcode = 4799837084.82899;
	pJunkcode = 2536755098.50581;
	if (pJunkcode = 2695443790.93683)
		pJunkcode = 4848896509.11562;
	pJunkcode = 8359096090.63547;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs342() {
	float pJunkcode = 4615764771.93094;
	pJunkcode = 8220563080.03778;
	if (pJunkcode = 9395953542.51137)
		pJunkcode = 6097200301.71522;
	pJunkcode = 5938930541.92852;
	pJunkcode = 3423795790.21924;
	if (pJunkcode = 8127669511.19888)
		pJunkcode = 8118514743.74943;
	pJunkcode = 2788044773.745;
	if (pJunkcode = 61743275.522749)
		pJunkcode = 1567877940.79469;
	pJunkcode = 8523353845.44983;
	pJunkcode = 6832172022.10841;
	if (pJunkcode = 8046346063.76775)
		pJunkcode = 7374696543.87911;
	pJunkcode = 716357108.463013;
	if (pJunkcode = 1278274445.43532)
		pJunkcode = 8371196280.62733;
	pJunkcode = 57779841.4392691;
	pJunkcode = 6074502162.61794;
	if (pJunkcode = 5742101023.14388)
		pJunkcode = 1068388541.03546;
	pJunkcode = 9901698645.62351;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs341() {
	float pJunkcode = 9554388386.88676;
	pJunkcode = 3028014990.54069;
	if (pJunkcode = 9530204086.0265)
		pJunkcode = 8793939991.53374;
	pJunkcode = 4625079518.04413;
	pJunkcode = 3442680953.54696;
	if (pJunkcode = 6436699997.01336)
		pJunkcode = 6585254932.47951;
	pJunkcode = 6578852558.92936;
	if (pJunkcode = 6172193999.40432)
		pJunkcode = 2039875517.89871;
	pJunkcode = 517647573.208905;
	pJunkcode = 3429096937.06013;
	if (pJunkcode = 323609561.152256)
		pJunkcode = 360203527.401093;
	pJunkcode = 5179804755.7601;
	if (pJunkcode = 3083834948.17492)
		pJunkcode = 3417249430.98233;
	pJunkcode = 8857624195.83124;
	pJunkcode = 7119051796.37552;
	if (pJunkcode = 7554517742.35556)
		pJunkcode = 2214578123.23146;
	pJunkcode = 6220817347.89775;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs340() {
	float pJunkcode = 1013198984.69124;
	pJunkcode = 9029994622.79304;
	if (pJunkcode = 7970032893.09147)
		pJunkcode = 178817410.655725;
	pJunkcode = 8146646534.61013;
	pJunkcode = 7809223036.91995;
	if (pJunkcode = 5290217842.75633)
		pJunkcode = 9772281068.45967;
	pJunkcode = 9709463893.52561;
	if (pJunkcode = 6138293940.58197)
		pJunkcode = 7365555175.69921;
	pJunkcode = 4528805738.41022;
	pJunkcode = 7028215656.85408;
	if (pJunkcode = 8785270561.69244)
		pJunkcode = 7057761020.59389;
	pJunkcode = 8985182336.2204;
	if (pJunkcode = 5128313025.28552)
		pJunkcode = 5618173707.62006;
	pJunkcode = 9731118810.43183;
	pJunkcode = 5193541116.47022;
	if (pJunkcode = 7931862339.21854)
		pJunkcode = 7967257225.67376;
	pJunkcode = 154909109.137023;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs339() {
	float pJunkcode = 6804383927.75023;
	pJunkcode = 8587252142.36812;
	if (pJunkcode = 2185926796.93966)
		pJunkcode = 3960245669.90473;
	pJunkcode = 2748338093.15263;
	pJunkcode = 7237145873.1963;
	if (pJunkcode = 9435391010.70758)
		pJunkcode = 5518995015.24233;
	pJunkcode = 2428346183.6782;
	if (pJunkcode = 354304109.577711)
		pJunkcode = 9049963178.18106;
	pJunkcode = 4568175227.7232;
	pJunkcode = 1854225725.64412;
	if (pJunkcode = 5945563729.82907)
		pJunkcode = 674295918.935964;
	pJunkcode = 4550777713.93139;
	if (pJunkcode = 2428869786.61996)
		pJunkcode = 6967407748.37216;
	pJunkcode = 4981483736.45921;
	pJunkcode = 9610724690.20417;
	if (pJunkcode = 4107597609.40499)
		pJunkcode = 1081164720.10146;
	pJunkcode = 1365260659.33357;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs338() {
	float pJunkcode = 1717737201.43784;
	pJunkcode = 1583107157.08185;
	if (pJunkcode = 6021766779.69705)
		pJunkcode = 8248409926.29306;
	pJunkcode = 7171542062.3026;
	pJunkcode = 1501554948.84607;
	if (pJunkcode = 1946743472.32955)
		pJunkcode = 4681753587.72686;
	pJunkcode = 2039452811.10739;
	if (pJunkcode = 6122381268.4166)
		pJunkcode = 7596240916.39098;
	pJunkcode = 3659967766.94946;
	pJunkcode = 9165733707.57654;
	if (pJunkcode = 3376894754.26417)
		pJunkcode = 7782197338.88029;
	pJunkcode = 9401915109.19696;
	if (pJunkcode = 1838773818.38648)
		pJunkcode = 635314039.874048;
	pJunkcode = 8461497278.23569;
	pJunkcode = 4964356561.95261;
	if (pJunkcode = 725441696.801657)
		pJunkcode = 5412666402.78588;
	pJunkcode = 2339663969.1695;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs337() {
	float pJunkcode = 7913267473.21922;
	pJunkcode = 8720514066.58871;
	if (pJunkcode = 6603251988.49641)
		pJunkcode = 4688656707.46305;
	pJunkcode = 8132299784.20371;
	pJunkcode = 5140930948.61731;
	if (pJunkcode = 3940298883.96419)
		pJunkcode = 8783017994.30734;
	pJunkcode = 7966569974.694;
	if (pJunkcode = 9120558390.43808)
		pJunkcode = 2686875413.24094;
	pJunkcode = 7623213338.65553;
	pJunkcode = 3884785207.3222;
	if (pJunkcode = 7692190817.86858)
		pJunkcode = 3827597311.15866;
	pJunkcode = 368127406.052484;
	if (pJunkcode = 6604554334.63272)
		pJunkcode = 410226099.564566;
	pJunkcode = 5832616807.20142;
	pJunkcode = 5918969609.33954;
	if (pJunkcode = 772264944.81658)
		pJunkcode = 1123443301.42653;
	pJunkcode = 4014394020.35111;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs336() {
	float pJunkcode = 2647046025.14155;
	pJunkcode = 8400766144.89076;
	if (pJunkcode = 72657559.1470665)
		pJunkcode = 1302636222.00721;
	pJunkcode = 1414103788.44353;
	pJunkcode = 4019196721.3497;
	if (pJunkcode = 9686463864.57812)
		pJunkcode = 9288253636.46958;
	pJunkcode = 8010572640.38091;
	if (pJunkcode = 924339646.041658)
		pJunkcode = 6787711245.75655;
	pJunkcode = 6136672009.64348;
	pJunkcode = 2672324318.7353;
	if (pJunkcode = 8732743778.97238)
		pJunkcode = 1596553120.3474;
	pJunkcode = 9821469370.20368;
	if (pJunkcode = 5562987420.18538)
		pJunkcode = 8347870771.66471;
	pJunkcode = 1374078950.28656;
	pJunkcode = 5914382279.18919;
	if (pJunkcode = 7037102192.11457)
		pJunkcode = 7249846841.7316;
	pJunkcode = 2608062041.31166;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs335() {
	float pJunkcode = 9776751164.8967;
	pJunkcode = 181647257.222404;
	if (pJunkcode = 9389121807.93665)
		pJunkcode = 1898962063.62029;
	pJunkcode = 1702377717.70825;
	pJunkcode = 8106911106.70567;
	if (pJunkcode = 6183103964.06338)
		pJunkcode = 447951446.117774;
	pJunkcode = 6554687554.87988;
	if (pJunkcode = 96785438.7216674)
		pJunkcode = 6305107724.22277;
	pJunkcode = 5477601155.90438;
	pJunkcode = 4372967106.70021;
	if (pJunkcode = 9013321982.70846)
		pJunkcode = 9162655454.15195;
	pJunkcode = 8670671682.58675;
	if (pJunkcode = 3856135920.60644)
		pJunkcode = 1021133403.48046;
	pJunkcode = 2421622297.50742;
	pJunkcode = 6234671368.39701;
	if (pJunkcode = 916158010.147221)
		pJunkcode = 1298034088.63257;
	pJunkcode = 1628286360.12489;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs334() {
	float pJunkcode = 5623756928.29325;
	pJunkcode = 6017003922.60806;
	if (pJunkcode = 4042163616.49964)
		pJunkcode = 3294544447.81598;
	pJunkcode = 7908921490.57224;
	pJunkcode = 8487780300.51513;
	if (pJunkcode = 3030473727.39901)
		pJunkcode = 9744807634.5848;
	pJunkcode = 478646467.184709;
	if (pJunkcode = 4787210463.03134)
		pJunkcode = 6188637259.36092;
	pJunkcode = 9865964935.76241;
	pJunkcode = 1427192278.75514;
	if (pJunkcode = 850576913.983961)
		pJunkcode = 465120415.635261;
	pJunkcode = 8631897160.42246;
	if (pJunkcode = 9466074241.34863)
		pJunkcode = 8423650202.05439;
	pJunkcode = 668826989.0262;
	pJunkcode = 7972387544.05142;
	if (pJunkcode = 4858761873.24)
		pJunkcode = 5651513812.01577;
	pJunkcode = 7315166883.80287;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs333() {
	float pJunkcode = 5715979052.86672;
	pJunkcode = 7087135462.73913;
	if (pJunkcode = 1876819802.12586)
		pJunkcode = 665630826.686172;
	pJunkcode = 4943707095.49985;
	pJunkcode = 9817597917.27445;
	if (pJunkcode = 9990671448.49599)
		pJunkcode = 1530310826.69286;
	pJunkcode = 5513832494.38038;
	if (pJunkcode = 6833808437.2964)
		pJunkcode = 2575906579.02574;
	pJunkcode = 5604263750.11804;
	pJunkcode = 8634699870.84899;
	if (pJunkcode = 1945765029.96067)
		pJunkcode = 2243814393.41298;
	pJunkcode = 87939809.0678902;
	if (pJunkcode = 7413752627.6534)
		pJunkcode = 9504484589.76007;
	pJunkcode = 8018445580.85348;
	pJunkcode = 6601597792.20998;
	if (pJunkcode = 5038613069.31659)
		pJunkcode = 5227750878.86104;
	pJunkcode = 6694404923.53681;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs332() {
	float pJunkcode = 7218039909.46402;
	pJunkcode = 7917482801.92876;
	if (pJunkcode = 7190132469.63969)
		pJunkcode = 7837994847.20836;
	pJunkcode = 6350943903.57973;
	pJunkcode = 5682520429.54538;
	if (pJunkcode = 1795421188.40141)
		pJunkcode = 6454878915.22736;
	pJunkcode = 8127937452.35244;
	if (pJunkcode = 7523229463.09976)
		pJunkcode = 3715180543.26157;
	pJunkcode = 8507805528.64339;
	pJunkcode = 8405217046.96147;
	if (pJunkcode = 1300710785.38999)
		pJunkcode = 4011660369.13119;
	pJunkcode = 3706159314.49238;
	if (pJunkcode = 3020977892.07872)
		pJunkcode = 2920969355.3478;
	pJunkcode = 7053848988.01758;
	pJunkcode = 2733134448.39934;
	if (pJunkcode = 3053229642.84273)
		pJunkcode = 1891524253.65796;
	pJunkcode = 3574937797.51111;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs331() {
	float pJunkcode = 1414670422.71454;
	pJunkcode = 1841328929.86936;
	if (pJunkcode = 7947449689.69205)
		pJunkcode = 4355682053.64127;
	pJunkcode = 2039042569.94407;
	pJunkcode = 3007072132.47123;
	if (pJunkcode = 9814099930.27613)
		pJunkcode = 2876389454.71668;
	pJunkcode = 156332372.330618;
	if (pJunkcode = 1739072545.18329)
		pJunkcode = 7824328639.99793;
	pJunkcode = 9019044304.33036;
	pJunkcode = 2233411421.72366;
	if (pJunkcode = 9551644693.7816)
		pJunkcode = 8288508715.37955;
	pJunkcode = 2706425371.25741;
	if (pJunkcode = 1702515538.10123)
		pJunkcode = 1695683410.04666;
	pJunkcode = 1762324955.36802;
	pJunkcode = 6845023979.8062;
	if (pJunkcode = 6889753400.45313)
		pJunkcode = 450396685.918653;
	pJunkcode = 7691359585.85862;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs330() {
	float pJunkcode = 4617397736.60329;
	pJunkcode = 3614903976.90233;
	if (pJunkcode = 3310827694.1846)
		pJunkcode = 760739577.667579;
	pJunkcode = 6192909735.78046;
	pJunkcode = 9702167957.49721;
	if (pJunkcode = 9342652158.61249)
		pJunkcode = 4312939121.8524;
	pJunkcode = 7355930348.83567;
	if (pJunkcode = 684625472.87261)
		pJunkcode = 1423933571.74477;
	pJunkcode = 8462002152.46778;
	pJunkcode = 3834186396.71971;
	if (pJunkcode = 4670594783.14922)
		pJunkcode = 6053350462.2062;
	pJunkcode = 3475866581.01202;
	if (pJunkcode = 6579421828.8169)
		pJunkcode = 8156477447.1287;
	pJunkcode = 5073243415.1045;
	pJunkcode = 9177123603.57026;
	if (pJunkcode = 6624142574.2808)
		pJunkcode = 9594503774.30385;
	pJunkcode = 3064946171.35174;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs329() {
	float pJunkcode = 6386767704.02915;
	pJunkcode = 793348196.031898;
	if (pJunkcode = 9659422285.31908)
		pJunkcode = 9220358112.76612;
	pJunkcode = 3164965953.6903;
	pJunkcode = 62102757.7850432;
	if (pJunkcode = 7505754443.09006)
		pJunkcode = 3402511626.79406;
	pJunkcode = 9359494402.48337;
	if (pJunkcode = 6101428498.84575)
		pJunkcode = 9566114151.69504;
	pJunkcode = 2264550860.82232;
	pJunkcode = 9481265692.88304;
	if (pJunkcode = 1720368897.13848)
		pJunkcode = 4867428287.95462;
	pJunkcode = 9296472699.51571;
	if (pJunkcode = 1497494355.50661)
		pJunkcode = 4855026052.11758;
	pJunkcode = 720281252.768282;
	pJunkcode = 7840079144.91341;
	if (pJunkcode = 3046791560.41386)
		pJunkcode = 6620863173.48976;
	pJunkcode = 1977437629.60344;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs328() {
	float pJunkcode = 6017505281.13258;
	pJunkcode = 1874048095.1178;
	if (pJunkcode = 8290828546.86494)
		pJunkcode = 4243721100.72194;
	pJunkcode = 2863492291.07319;
	pJunkcode = 6072818487.93851;
	if (pJunkcode = 7342643062.72358)
		pJunkcode = 8147088533.55566;
	pJunkcode = 1731555927.49278;
	if (pJunkcode = 5125307588.91656)
		pJunkcode = 6631663105.81528;
	pJunkcode = 4049253078.04972;
	pJunkcode = 3441100768.72541;
	if (pJunkcode = 6138662507.80513)
		pJunkcode = 1803847578.07748;
	pJunkcode = 226613415.372072;
	if (pJunkcode = 4663327133.43285)
		pJunkcode = 5324802956.98741;
	pJunkcode = 8644875942.03189;
	pJunkcode = 5784014317.79675;
	if (pJunkcode = 2538396247.09345)
		pJunkcode = 1894092599.10228;
	pJunkcode = 6963624702.68255;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs327() {
	float pJunkcode = 8943307381.40621;
	pJunkcode = 6745062164.73331;
	if (pJunkcode = 4161367152.83891)
		pJunkcode = 9391719303.48605;
	pJunkcode = 5428287687.98634;
	pJunkcode = 8155353743.17388;
	if (pJunkcode = 6896309335.91865)
		pJunkcode = 747083780.211133;
	pJunkcode = 7945935807.38985;
	if (pJunkcode = 8141440330.49966)
		pJunkcode = 6461617313.58703;
	pJunkcode = 910682868.378005;
	pJunkcode = 1024603697.87895;
	if (pJunkcode = 8388149984.06931)
		pJunkcode = 1144064330.22885;
	pJunkcode = 731697728.442489;
	if (pJunkcode = 8252382456.97882)
		pJunkcode = 9441323778.85885;
	pJunkcode = 5855991170.52944;
	pJunkcode = 1322272538.37411;
	if (pJunkcode = 2221537403.4856)
		pJunkcode = 2420128505.96055;
	pJunkcode = 7774907873.15078;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs326() {
	float pJunkcode = 3562271599.55642;
	pJunkcode = 4602711399.11413;
	if (pJunkcode = 4179009550.79621)
		pJunkcode = 6681007802.99526;
	pJunkcode = 8875026631.61364;
	pJunkcode = 1519846476.80306;
	if (pJunkcode = 1433438254.54104)
		pJunkcode = 288440462.284106;
	pJunkcode = 9496741206.60356;
	if (pJunkcode = 2923813378.83059)
		pJunkcode = 4359106695.06369;
	pJunkcode = 4635628878.20842;
	pJunkcode = 5038306611.37483;
	if (pJunkcode = 3769910449.94568)
		pJunkcode = 9404312095.32949;
	pJunkcode = 6195435487.61278;
	if (pJunkcode = 2243576331.11713)
		pJunkcode = 3956806160.06609;
	pJunkcode = 4608849782.60722;
	pJunkcode = 9079689680.02191;
	if (pJunkcode = 2357104789.02796)
		pJunkcode = 2259482116.22094;
	pJunkcode = 2402596382.47823;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs325() {
	float pJunkcode = 8508929595.81037;
	pJunkcode = 7679491576.91938;
	if (pJunkcode = 9914925624.54535)
		pJunkcode = 480598537.032917;
	pJunkcode = 9910079328.773;
	pJunkcode = 3968051951.63061;
	if (pJunkcode = 6476378763.819)
		pJunkcode = 9853373375.58076;
	pJunkcode = 1767372775.68871;
	if (pJunkcode = 1464566191.92092)
		pJunkcode = 8465491119.69716;
	pJunkcode = 9330913727.94386;
	pJunkcode = 4846616821.88835;
	if (pJunkcode = 5469767735.71006)
		pJunkcode = 5048922050.5433;
	pJunkcode = 2263283138.58478;
	if (pJunkcode = 8241139510.5763)
		pJunkcode = 1739317080.10631;
	pJunkcode = 8981946150.69297;
	pJunkcode = 1289746967.54159;
	if (pJunkcode = 6360878492.21882)
		pJunkcode = 5885675977.47558;
	pJunkcode = 3750899352.11049;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs324() {
	float pJunkcode = 5060954915.89949;
	pJunkcode = 17095291.2356706;
	if (pJunkcode = 1875036497.88201)
		pJunkcode = 4220967941.96547;
	pJunkcode = 9145141314.32872;
	pJunkcode = 2309181208.36451;
	if (pJunkcode = 8977425784.29438)
		pJunkcode = 9098901997.40034;
	pJunkcode = 8396006449.61029;
	if (pJunkcode = 3758063702.88819)
		pJunkcode = 9320419061.47911;
	pJunkcode = 3376367509.88755;
	pJunkcode = 8145767922.03801;
	if (pJunkcode = 6987107226.40445)
		pJunkcode = 7315459139.2174;
	pJunkcode = 4933831580.06092;
	if (pJunkcode = 1010544667.39148)
		pJunkcode = 6593486365.69403;
	pJunkcode = 7138926815.31614;
	pJunkcode = 2402465195.36397;
	if (pJunkcode = 6619948349.12975)
		pJunkcode = 3977201431.54902;
	pJunkcode = 9941673879.7079;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs323() {
	float pJunkcode = 7285034455.10739;
	pJunkcode = 9943727567.47433;
	if (pJunkcode = 530024013.996892)
		pJunkcode = 5433081839.5292;
	pJunkcode = 8155740254.4753;
	pJunkcode = 2758919842.06386;
	if (pJunkcode = 8695824460.76399)
		pJunkcode = 7120537243.35557;
	pJunkcode = 4207700502.13899;
	if (pJunkcode = 7231443819.27925)
		pJunkcode = 5025640834.83939;
	pJunkcode = 6834208855.4331;
	pJunkcode = 2991623489.46657;
	if (pJunkcode = 3119161529.78727)
		pJunkcode = 6550973219.1544;
	pJunkcode = 289778824.632518;
	if (pJunkcode = 8503825514.85557)
		pJunkcode = 103262529.206484;
	pJunkcode = 7150278962.85511;
	pJunkcode = 1062356588.02187;
	if (pJunkcode = 7494390942.84244)
		pJunkcode = 8431107231.99415;
	pJunkcode = 5326473062.43263;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs322() {
	float pJunkcode = 487815765.449318;
	pJunkcode = 4597473700.31744;
	if (pJunkcode = 1286126610.6373)
		pJunkcode = 2181919411.8551;
	pJunkcode = 2171100646.04714;
	pJunkcode = 1643639597.07078;
	if (pJunkcode = 1010183974.42195)
		pJunkcode = 6023271507.1899;
	pJunkcode = 6029317094.62964;
	if (pJunkcode = 1771247490.54264)
		pJunkcode = 9123367524.59783;
	pJunkcode = 228412323.76961;
	pJunkcode = 4055414674.52586;
	if (pJunkcode = 3421974392.93473)
		pJunkcode = 5178916946.46953;
	pJunkcode = 1751961008.03264;
	if (pJunkcode = 8604892628.80495)
		pJunkcode = 4659319454.98461;
	pJunkcode = 456826970.927239;
	pJunkcode = 7740348088.31212;
	if (pJunkcode = 4233540452.13436)
		pJunkcode = 5735884139.25599;
	pJunkcode = 1526721796.83841;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs321() {
	float pJunkcode = 7955705188.04124;
	pJunkcode = 7808314578.08608;
	if (pJunkcode = 8035751125.13245)
		pJunkcode = 2324571097.14379;
	pJunkcode = 5955071374.2015;
	pJunkcode = 3739700764.15861;
	if (pJunkcode = 7866959911.98334)
		pJunkcode = 2582728197.21016;
	pJunkcode = 256140667.397233;
	if (pJunkcode = 387723138.115745)
		pJunkcode = 7881355470.1895;
	pJunkcode = 8535337213.83447;
	pJunkcode = 2363469928.67392;
	if (pJunkcode = 2304305811.38102)
		pJunkcode = 8332975114.53522;
	pJunkcode = 3415768642.89687;
	if (pJunkcode = 1719172478.5725)
		pJunkcode = 207957881.29862;
	pJunkcode = 2086406354.50413;
	pJunkcode = 6604927353.53327;
	if (pJunkcode = 4348771744.1667)
		pJunkcode = 7895716222.8908;
	pJunkcode = 2151540753.24646;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs320() {
	float pJunkcode = 1973591547.97573;
	pJunkcode = 2865961353.86729;
	if (pJunkcode = 4543243592.20814)
		pJunkcode = 5422744241.37426;
	pJunkcode = 3706929086.81194;
	pJunkcode = 6950924756.26355;
	if (pJunkcode = 9105528067.73929)
		pJunkcode = 1072081733.62163;
	pJunkcode = 4558932239.29679;
	if (pJunkcode = 4738146042.24907)
		pJunkcode = 2739734141.61844;
	pJunkcode = 5719062791.72641;
	pJunkcode = 7551351705.67125;
	if (pJunkcode = 4491814504.19179)
		pJunkcode = 95202033.8845098;
	pJunkcode = 5547920895.01184;
	if (pJunkcode = 977121478.211453)
		pJunkcode = 6908564321.81595;
	pJunkcode = 6977331175.22182;
	pJunkcode = 5972761471.69576;
	if (pJunkcode = 7544392469.65449)
		pJunkcode = 5468304786.93012;
	pJunkcode = 1753018374.54385;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs319() {
	float pJunkcode = 561407405.947782;
	pJunkcode = 5294305358.03389;
	if (pJunkcode = 245796903.948503)
		pJunkcode = 2086004585.96321;
	pJunkcode = 9472858262.41668;
	pJunkcode = 3166687884.39023;
	if (pJunkcode = 6256226294.85493)
		pJunkcode = 3624938050.75228;
	pJunkcode = 5823757517.51038;
	if (pJunkcode = 2014463981.31666)
		pJunkcode = 37137294.6440457;
	pJunkcode = 3365637682.16777;
	pJunkcode = 7491001931.13711;
	if (pJunkcode = 5975448892.52539)
		pJunkcode = 7310010085.86092;
	pJunkcode = 4030005461.5704;
	if (pJunkcode = 2728437726.19331)
		pJunkcode = 1552424231.4459;
	pJunkcode = 5354430374.08211;
	pJunkcode = 8920949574.4423;
	if (pJunkcode = 1513273135.09269)
		pJunkcode = 475023993.87104;
	pJunkcode = 6682182228.59857;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs318() {
	float pJunkcode = 4120341823.47158;
	pJunkcode = 894859015.558357;
	if (pJunkcode = 8777554226.20132)
		pJunkcode = 6570333675.87674;
	pJunkcode = 6562777214.27663;
	pJunkcode = 4949027052.54034;
	if (pJunkcode = 8072235713.29531)
		pJunkcode = 2096352668.37719;
	pJunkcode = 9739940372.62265;
	if (pJunkcode = 8383745805.98493)
		pJunkcode = 6549438971.35946;
	pJunkcode = 3501368294.32887;
	pJunkcode = 3920991172.1684;
	if (pJunkcode = 7475195151.61783)
		pJunkcode = 9002666862.61021;
	pJunkcode = 4837842256.09297;
	if (pJunkcode = 4766879831.67359)
		pJunkcode = 5185426309.28713;
	pJunkcode = 2659454782.4997;
	pJunkcode = 9945832680.03077;
	if (pJunkcode = 1404582702.89245)
		pJunkcode = 705736086.071545;
	pJunkcode = 2849940893.83334;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs317() {
	float pJunkcode = 9362642322.79646;
	pJunkcode = 7153313560.97057;
	if (pJunkcode = 266595159.336166)
		pJunkcode = 1825243922.84155;
	pJunkcode = 1246351238.70707;
	pJunkcode = 1241624197.59782;
	if (pJunkcode = 5274318297.45137)
		pJunkcode = 5960486532.41184;
	pJunkcode = 8249280445.75294;
	if (pJunkcode = 545237896.211204)
		pJunkcode = 3373938135.00808;
	pJunkcode = 4740396039.20957;
	pJunkcode = 5177092401.34619;
	if (pJunkcode = 5169024904.87677)
		pJunkcode = 698855445.909194;
	pJunkcode = 974367326.458369;
	if (pJunkcode = 2624728062.08723)
		pJunkcode = 8302568842.48838;
	pJunkcode = 6058645292.4149;
	pJunkcode = 2689500268.44568;
	if (pJunkcode = 1483656919.36824)
		pJunkcode = 4347267466.0325;
	pJunkcode = 2322029247.56176;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs316() {
	float pJunkcode = 9730231497.06556;
	pJunkcode = 6048409514.73882;
	if (pJunkcode = 6751200080.2415)
		pJunkcode = 8115153995.0733;
	pJunkcode = 3642309552.9647;
	pJunkcode = 5730105417.30299;
	if (pJunkcode = 9720683082.12683)
		pJunkcode = 5793263906.9059;
	pJunkcode = 2511902734.04726;
	if (pJunkcode = 8087067375.23406)
		pJunkcode = 5829813691.65362;
	pJunkcode = 9330465791.87629;
	pJunkcode = 2508486050.69666;
	if (pJunkcode = 5181921784.46972)
		pJunkcode = 7106442594.79242;
	pJunkcode = 1902044975.93947;
	if (pJunkcode = 1284265094.70578)
		pJunkcode = 6270215998.38323;
	pJunkcode = 5326600554.56154;
	pJunkcode = 2313148723.92836;
	if (pJunkcode = 1677980425.50514)
		pJunkcode = 8623135858.85621;
	pJunkcode = 4020066786.79437;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs315() {
	float pJunkcode = 8124599306.64916;
	pJunkcode = 5565109880.7796;
	if (pJunkcode = 701278264.343306)
		pJunkcode = 9721022122.06303;
	pJunkcode = 8443419102.50545;
	pJunkcode = 18708875.2790604;
	if (pJunkcode = 3976594212.97437)
		pJunkcode = 3577699986.49764;
	pJunkcode = 2143172323.85121;
	if (pJunkcode = 5728191792.0662)
		pJunkcode = 5895911426.86374;
	pJunkcode = 5152202714.50869;
	pJunkcode = 4876903671.63569;
	if (pJunkcode = 9042740757.69323)
		pJunkcode = 5020696758.15945;
	pJunkcode = 5837308102.17941;
	if (pJunkcode = 5948938.3163368)
		pJunkcode = 4299726599.24065;
	pJunkcode = 151080526.885496;
	pJunkcode = 1018280029.61252;
	if (pJunkcode = 5664359972.2885)
		pJunkcode = 1664782061.18325;
	pJunkcode = 7860356339.84451;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs314() {
	float pJunkcode = 9560348284.19333;
	pJunkcode = 4662836959.9806;
	if (pJunkcode = 3495028130.1763)
		pJunkcode = 1735505401.19047;
	pJunkcode = 4889327683.90506;
	pJunkcode = 3941417565.81224;
	if (pJunkcode = 4390746673.1312)
		pJunkcode = 7359092935.00434;
	pJunkcode = 2070096765.93992;
	if (pJunkcode = 7896758069.06651)
		pJunkcode = 8367437388.54714;
	pJunkcode = 7421478106.31246;
	pJunkcode = 5466183392.68959;
	if (pJunkcode = 497223405.403757)
		pJunkcode = 476289555.461816;
	pJunkcode = 1275010106.308;
	if (pJunkcode = 3764578059.87316)
		pJunkcode = 3297899638.88023;
	pJunkcode = 7368378156.15887;
	pJunkcode = 2209368310.2821;
	if (pJunkcode = 3863494091.64353)
		pJunkcode = 7619953346.43285;
	pJunkcode = 1526308826.92723;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs313() {
	float pJunkcode = 6817697323.22959;
	pJunkcode = 107327848.960749;
	if (pJunkcode = 9440153073.85202)
		pJunkcode = 1502502040.8823;
	pJunkcode = 3792277155.60781;
	pJunkcode = 180983121.437791;
	if (pJunkcode = 5918856431.49547)
		pJunkcode = 9451703933.37326;
	pJunkcode = 8121246876.92874;
	if (pJunkcode = 7051138167.94226)
		pJunkcode = 1851836683.61371;
	pJunkcode = 2909008811.12291;
	pJunkcode = 8532520438.24941;
	if (pJunkcode = 5071022603.17333)
		pJunkcode = 4358046823.31981;
	pJunkcode = 8890276162.17131;
	if (pJunkcode = 4577575959.66153)
		pJunkcode = 8347883444.50992;
	pJunkcode = 1602715552.67719;
	pJunkcode = 5439518890.52863;
	if (pJunkcode = 6736334134.33332)
		pJunkcode = 3528726946.17257;
	pJunkcode = 6840206494.20958;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs312() {
	float pJunkcode = 7503582217.77173;
	pJunkcode = 3582086653.56444;
	if (pJunkcode = 2187293445.6001)
		pJunkcode = 2079133321.79712;
	pJunkcode = 4192218336.51349;
	pJunkcode = 24599101.0405537;
	if (pJunkcode = 7781321257.15196)
		pJunkcode = 3900817210.98403;
	pJunkcode = 3117873199.91554;
	if (pJunkcode = 2876155126.98701)
		pJunkcode = 3670901643.07651;
	pJunkcode = 6041170002.32436;
	pJunkcode = 6265396889.97326;
	if (pJunkcode = 5071743639.86992)
		pJunkcode = 6372470650.82664;
	pJunkcode = 3698102092.73652;
	if (pJunkcode = 2490489444.03841)
		pJunkcode = 9396220454.56377;
	pJunkcode = 6278207916.45321;
	pJunkcode = 3648905716.50654;
	if (pJunkcode = 6999063346.89135)
		pJunkcode = 743178613.741847;
	pJunkcode = 5436225839.01346;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs311() {
	float pJunkcode = 6941409028.092;
	pJunkcode = 773951893.062128;
	if (pJunkcode = 4120426922.22083)
		pJunkcode = 2483385144.33979;
	pJunkcode = 3516727847.90695;
	pJunkcode = 576655141.399142;
	if (pJunkcode = 6382646465.8984)
		pJunkcode = 4205888101.34058;
	pJunkcode = 3771744669.08553;
	if (pJunkcode = 5430995180.94503)
		pJunkcode = 2233476974.53584;
	pJunkcode = 6858413845.41745;
	pJunkcode = 6038729505.23626;
	if (pJunkcode = 2414099931.28581)
		pJunkcode = 1396372647.97385;
	pJunkcode = 3719896287.54096;
	if (pJunkcode = 2537508514.63621)
		pJunkcode = 2141862729.76944;
	pJunkcode = 1784585917.39418;
	pJunkcode = 1705511684.62041;
	if (pJunkcode = 2527577437.04811)
		pJunkcode = 2120114127.4057;
	pJunkcode = 3296525327.83993;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs310() {
	float pJunkcode = 9110848578.91116;
	pJunkcode = 7023484047.65328;
	if (pJunkcode = 4821451491.93663)
		pJunkcode = 793812257.788586;
	pJunkcode = 9185263803.00942;
	pJunkcode = 7083757896.87379;
	if (pJunkcode = 3930285343.24336)
		pJunkcode = 9219422166.22135;
	pJunkcode = 7465533265.47134;
	if (pJunkcode = 1684086655.40789)
		pJunkcode = 4046571089.10469;
	pJunkcode = 3693915515.99563;
	pJunkcode = 1589302264.53741;
	if (pJunkcode = 9992398003.16626)
		pJunkcode = 5249858788.86583;
	pJunkcode = 2814378936.76389;
	if (pJunkcode = 1225632328.88098)
		pJunkcode = 5617041025.80541;
	pJunkcode = 5892124236.83011;
	pJunkcode = 2992657500.96948;
	if (pJunkcode = 1970045961.55663)
		pJunkcode = 4787201357.11593;
	pJunkcode = 8920235375.91332;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs39() {
	float pJunkcode = 5344756932.66633;
	pJunkcode = 1605715322.00495;
	if (pJunkcode = 846584138.876184)
		pJunkcode = 444756566.661764;
	pJunkcode = 5692058816.56858;
	pJunkcode = 1276485208.726;
	if (pJunkcode = 8405838665.96742)
		pJunkcode = 985350236.228629;
	pJunkcode = 380086730.257002;
	if (pJunkcode = 1078785838.69874)
		pJunkcode = 5762276376.64157;
	pJunkcode = 1887227209.19458;
	pJunkcode = 3119072843.15368;
	if (pJunkcode = 3506780330.3115)
		pJunkcode = 2116917939.45456;
	pJunkcode = 2046166881.23639;
	if (pJunkcode = 2131745976.25643)
		pJunkcode = 7179864356.76717;
	pJunkcode = 2246204823.10015;
	pJunkcode = 5810614687.39826;
	if (pJunkcode = 9937503153.91872)
		pJunkcode = 2206150553.11583;
	pJunkcode = 469770018.046833;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs38() {
	float pJunkcode = 9709200590.31209;
	pJunkcode = 9823539761.07345;
	if (pJunkcode = 9430123006.97737)
		pJunkcode = 6365940566.28897;
	pJunkcode = 5768835601.97119;
	pJunkcode = 1543870312.38839;
	if (pJunkcode = 4916320425.09668)
		pJunkcode = 1526556870.57296;
	pJunkcode = 7876396245.69936;
	if (pJunkcode = 9005154339.79577)
		pJunkcode = 3289832151.72395;
	pJunkcode = 6932895043.74714;
	pJunkcode = 4523681957.85166;
	if (pJunkcode = 8871330591.53717)
		pJunkcode = 3303229260.62408;
	pJunkcode = 589718276.707213;
	if (pJunkcode = 6301444778.36305)
		pJunkcode = 4032177651.96448;
	pJunkcode = 1763038946.6798;
	pJunkcode = 9445181085.13964;
	if (pJunkcode = 7405358482.37072)
		pJunkcode = 1848286009.89592;
	pJunkcode = 8301624958.82634;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs37() {
	float pJunkcode = 7568387159.79131;
	pJunkcode = 4185859196.85322;
	if (pJunkcode = 1237793368.55305)
		pJunkcode = 3377939366.44027;
	pJunkcode = 9838312333.51405;
	pJunkcode = 7491788380.9489;
	if (pJunkcode = 3770931045.88106)
		pJunkcode = 3370171706.27533;
	pJunkcode = 2657208026.50935;
	if (pJunkcode = 4351439607.41784)
		pJunkcode = 9749074397.39749;
	pJunkcode = 6479804841.79213;
	pJunkcode = 6906259053.74167;
	if (pJunkcode = 8244567469.80222)
		pJunkcode = 7798308092.06321;
	pJunkcode = 8738633045.48121;
	if (pJunkcode = 3987697734.79682)
		pJunkcode = 4783325643.42853;
	pJunkcode = 5175483387.72263;
	pJunkcode = 9931707484.39265;
	if (pJunkcode = 1330246361.02743)
		pJunkcode = 2338155879.63578;
	pJunkcode = 564731779.856209;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs36() {
	float pJunkcode = 6827206241.17291;
	pJunkcode = 579562114.673555;
	if (pJunkcode = 2655530098.03274)
		pJunkcode = 7743812801.81589;
	pJunkcode = 7887729908.99837;
	pJunkcode = 3617983510.88264;
	if (pJunkcode = 1094417787.43604)
		pJunkcode = 7960806428.49059;
	pJunkcode = 5311521321.61854;
	if (pJunkcode = 8616964159.95947)
		pJunkcode = 235202240.050613;
	pJunkcode = 2798381666.43014;
	pJunkcode = 3958578819.39936;
	if (pJunkcode = 4932086697.14567)
		pJunkcode = 369851278.54056;
	pJunkcode = 3819856905.87631;
	if (pJunkcode = 9662029541.03146)
		pJunkcode = 5614574826.79958;
	pJunkcode = 5154017986.08023;
	pJunkcode = 7225862051.45475;
	if (pJunkcode = 1181420124.69513)
		pJunkcode = 1451311667.08109;
	pJunkcode = 685978503.404887;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs35() {
	float pJunkcode = 8090371565.25583;
	pJunkcode = 7877616249.77367;
	if (pJunkcode = 4894449606.49367)
		pJunkcode = 6313612241.76833;
	pJunkcode = 1387660941.16705;
	pJunkcode = 980699523.225993;
	if (pJunkcode = 1542841139.05788)
		pJunkcode = 8596430828.2704;
	pJunkcode = 5994141246.06863;
	if (pJunkcode = 1047094538.28568)
		pJunkcode = 9104790304.65652;
	pJunkcode = 4453923963.62058;
	pJunkcode = 23508745.0853423;
	if (pJunkcode = 2829340183.95809)
		pJunkcode = 467695198.782503;
	pJunkcode = 495472056.84619;
	if (pJunkcode = 6538038521.25819)
		pJunkcode = 1932698798.036;
	pJunkcode = 7733872178.26686;
	pJunkcode = 1184884386.51197;
	if (pJunkcode = 2110431708.13008)
		pJunkcode = 3303164811.41812;
	pJunkcode = 993327372.028867;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs34() {
	float pJunkcode = 4350644512.69071;
	pJunkcode = 7046162703.67713;
	if (pJunkcode = 7991736433.86305)
		pJunkcode = 2223605703.5571;
	pJunkcode = 5042898714.50565;
	pJunkcode = 6554522484.26088;
	if (pJunkcode = 442565896.684295)
		pJunkcode = 3694771695.55957;
	pJunkcode = 8376316699.14501;
	if (pJunkcode = 7155709818.35047)
		pJunkcode = 941225.001248227;
	pJunkcode = 924636271.643664;
	pJunkcode = 9302693669.54054;
	if (pJunkcode = 4244135835.47175)
		pJunkcode = 8230090819.82211;
	pJunkcode = 7473501577.07843;
	if (pJunkcode = 6906712340.20393)
		pJunkcode = 6865045352.29756;
	pJunkcode = 5356165669.12734;
	pJunkcode = 2530455576.86396;
	if (pJunkcode = 8976514699.15712)
		pJunkcode = 7643004658.14508;
	pJunkcode = 6620101789.9744;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs33() {
	float pJunkcode = 5019631307.84913;
	pJunkcode = 9678604218.60236;
	if (pJunkcode = 2059315285.25838)
		pJunkcode = 7423967283.14472;
	pJunkcode = 3290734144.91549;
	pJunkcode = 1827310830.59972;
	if (pJunkcode = 3201616955.89154)
		pJunkcode = 583009836.366875;
	pJunkcode = 6275869328.31873;
	if (pJunkcode = 1382213238.21479)
		pJunkcode = 1921556591.78897;
	pJunkcode = 7186871054.83601;
	pJunkcode = 3300537895.68279;
	if (pJunkcode = 8002905867.05329)
		pJunkcode = 3560308812.62086;
	pJunkcode = 9633045512.39019;
	if (pJunkcode = 7188556953.02585)
		pJunkcode = 7851377318.19231;
	pJunkcode = 3124171638.90572;
	pJunkcode = 4842532417.19708;
	if (pJunkcode = 7044525226.79202)
		pJunkcode = 9806904226.83749;
	pJunkcode = 4865908673.4193;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs32() {
	float pJunkcode = 2578835761.79315;
	pJunkcode = 7839959506.08358;
	if (pJunkcode = 2040315747.34904)
		pJunkcode = 5912121669.91408;
	pJunkcode = 723072710.602331;
	pJunkcode = 835926897.711985;
	if (pJunkcode = 8413760896.19434)
		pJunkcode = 2918960549.89571;
	pJunkcode = 8569386753.09182;
	if (pJunkcode = 2078470205.80052)
		pJunkcode = 4558598612.61892;
	pJunkcode = 6302804591.18415;
	pJunkcode = 5987064990.54059;
	if (pJunkcode = 6207440273.87415)
		pJunkcode = 2928577.65236057;
	pJunkcode = 2954394367.57353;
	if (pJunkcode = 7285548733.27513)
		pJunkcode = 9532857565.14433;
	pJunkcode = 250102134.655445;
	pJunkcode = 7465004989.66529;
	if (pJunkcode = 1216621598.34052)
		pJunkcode = 7343839804.92092;
	pJunkcode = 7781915255.73486;
}




void dAD4AS4da5S4D5As4d5aS4D35As4d35aS4D35As4d35aS4DA3s54da3S4DAs354dA35S4Das35d4aS3D4As35d4AS35D4As86d4as8d4AS65D4As3d4AS32D4A32d4aS23D4as32d4aS23D4a3d4AS34DAs32d4AS32D4As32d4aS324DAs31() {
	float pJunkcode = 3857439540.31364;
	pJunkcode = 9055913874.11261;
	if (pJunkcode = 3978324304.30675)
		pJunkcode = 7638711073.51047;
	pJunkcode = 314160026.070758;
	pJunkcode = 8973145424.30808;
	if (pJunkcode = 8832564667.72257)
		pJunkcode = 3189819112.0934;
	pJunkcode = 2310934023.74047;
	if (pJunkcode = 1337656203.65253)
		pJunkcode = 4832296076.07647;
	pJunkcode = 9725245569.49594;
	pJunkcode = 6324122063.02812;
	if (pJunkcode = 4076011545.06218)
		pJunkcode = 2082233717.19448;
	pJunkcode = 7709251380.19417;
	if (pJunkcode = 5781033333.05746)
		pJunkcode = 646408143.79114;
	pJunkcode = 2998334330.26684;
	pJunkcode = 5485656107.58417;
	if (pJunkcode = 3764830506.30233)
		pJunkcode = 6395922823.04936;
	pJunkcode = 3848398916.94578;
}




