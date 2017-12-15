#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" // for the unload meme
#include "Interfaces.h"
#include "CRC32.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 700

AyyWareWindow Menu::Window;

void SaveCallbk()
{
	switch (Menu::Window.MiscTab.ConfigBox.GetIndex())
	{
	case 0:
		GUI.SaveWindowState(&Menu::Window, "beartraplegit.cfg");
		break;
	case 1:
		GUI.SaveWindowState(&Menu::Window, "beartraprage.cfg");
		break;
	case 2:
		GUI.SaveWindowState(&Menu::Window, "beartrapmmhvh.cfg");
		break;
	case 3:
		GUI.SaveWindowState(&Menu::Window, "beartrapcasual.cfg");
		break;
	}
}

void LoadCallbk()
{
	switch (Menu::Window.MiscTab.ConfigBox.GetIndex())
	{
	case 0:
		GUI.LoadWindowState(&Menu::Window, "beartraplegit.cfg");
		break;
	case 1:
		GUI.LoadWindowState(&Menu::Window, "beartraprage.cfg");
		break;
	case 2:
		GUI.LoadWindowState(&Menu::Window, "beartrapmmhvh.cfg");
		break;
	case 3:
		GUI.LoadWindowState(&Menu::Window, "beartrapcasual.cfg");
		break;
	}
}

void UnLoadCallbk()
{
	DoUnload = true;
}

void KnifeApplyCallbk()
{
	Interfaces::Engine->ClientCmd_Unrestricted("record meme;stop");
}

/*
void GlovesApplyCallbk()
{
static ConVar* Meme = Interfaces::CVar->FindVar("cl_fullupdate");
Meme->nFlags &= ~FCVAR_CHEAT;
Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate");
bGlovesNeedUpdate = true;
}*/


void AyyWareWindow::Setup()
{
	SetPosition(350, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("beartraphook.xyz");

	RegisterTab(&RageBotTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&SkinchangerTab);
	RegisterTab(&MiscTab);
	RegisterTab(&ColorsTab);
	//RegisterTab(&Playerlist);

	RECT Client = GetClientArea();
	Client.bottom -= 29;

	RageBotTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	SkinchangerTab.Setup();
	MiscTab.Setup();
	ColorsTab.Setup();
	//Playerlist.Setup();

#pragma region Bottom Buttons
	ConfigBox.SetFileId("cfg_box");
	ConfigBox.AddItem("Legit");
	ConfigBox.AddItem("Rage");
	ConfigBox.AddItem("HVH");
	ConfigBox.AddItem("Casual");
	ConfigBox.SetSize(112, 350);
	ConfigBox.SetPosition(600, 440);

	SaveButton.SetText("Save");
	SaveButton.SetCallback(SaveCallbk);
	SaveButton.SetSize(112, 350);
	SaveButton.SetPosition(600, 410);

	LoadButton.SetText("Load");
	LoadButton.SetCallback(LoadCallbk);
	LoadButton.SetSize(112, 350);
	LoadButton.SetPosition(600, 380);

	MiscTab.RegisterControl(&LoadButton);

	MiscTab.RegisterControl(&SaveButton);

	MiscTab.RegisterControl(&ConfigBox);
#pragma endregion Setting up the settings buttons
}

void CRageBotTab::Setup()
{
	SetTitle("A");

	ActiveLabel.SetPosition(8, 7);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(48, 8.5);
	RegisterControl(&Active);

#pragma region Aimbot

	AimbotGroup.SetPosition(16, 50);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(376, 270);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(39.f);
	AimbotGroup.PlaceLabledControl("FOV Range", this, &AimbotFov);

	AimbotSilentAim.SetFileId("aim_silentmethod");
	AimbotSilentAim.AddItem("Aim Lock");
	AimbotSilentAim.AddItem("Client-Sided");
	AimbotSilentAim.AddItem("Server-Sided");
	AimbotGroup.PlaceLabledControl("Aimbot Type", this, &AimbotSilentAim);

	AWPAtBody.SetFileId("aim_awpatbody");
	AimbotGroup.PlaceLabledControl("AWP Baim", this, &AWPAtBody);

	PreferBodyAim.SetFileId("aim_preferbaim");
	AimbotGroup.PlaceLabledControl("Prefer Body Aim", this, &PreferBodyAim);

	AutoRevolver.SetFileId("aim_autorevolver");
	AimbotGroup.PlaceLabledControl("Auto Revolver", this, &AutoRevolver);

	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceLabledControl("Aim Step (casual antikick)", this, &AimbotAimStep);

	FakeWalk.SetFileId("otr_fakewalk");
	AimbotGroup.PlaceLabledControl("FakeWalk", this, &FakeWalk);
#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(16, 336);
	TargetGroup.SetText("Target");
	TargetGroup.SetSize(376, 300);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("Closest To Crosshair");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Lowest Health");
	TargetSelection.AddItem("Threat");
	TargetSelection.AddItem("Next Shot");
	TargetGroup.PlaceLabledControl("Selection", this, &TargetSelection);

	TargetFriendlyFire.SetFileId("tgt_friendlyfire");
	TargetGroup.PlaceLabledControl("Friendly Fire", this, &TargetFriendlyFire);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	TargetHitbox.AddItem("Legs");
	TargetGroup.PlaceLabledControl("Hitbox", this, &TargetHitbox);

	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("Off");
	TargetHitscan.AddItem("Low");
	TargetHitscan.AddItem("Medium");
	TargetHitscan.AddItem("High");
	TargetHitscan.AddItem("Baim");
	TargetGroup.PlaceLabledControl("Hitscan", this, &TargetHitscan);

	PVSFix.SetFileId("tgt_pvsfix");
	TargetGroup.PlaceLabledControl("PVS Fix", this, &PVSFix);

	EnginePrediction.SetFileId("tgt_enginepred");
	TargetGroup.PlaceLabledControl("Velocity Prediction", this, &EnginePrediction);

	TargetMultipoint.SetFileId("tgt_multipoint");
	TargetGroup.PlaceLabledControl("Multipoint", this, &TargetMultipoint);

	TargetPointscale.SetFileId("tgt_pointscale");
	TargetPointscale.SetBoundaries(0.f, 10.f);
	TargetPointscale.SetValue(5.f);
	TargetGroup.PlaceLabledControl("Point Scale", this, &TargetPointscale);

	AccuracySmart.SetFileId("acc_smartaim");
	AccuracySmart.SetBoundaries(0.f, 30.f);
	AccuracySmart.SetValue(0.f);
	AimbotGroup.PlaceLabledControl("Baim target after X shots", this, &AccuracySmart);
#pragma endregion Targetting controls 

#pragma region Accuracy
	AccuracyGroup.SetPosition(408, 50);
	AccuracyGroup.SetText("Accuracy");
	AccuracyGroup.SetSize(360, 270);
	RegisterControl(&AccuracyGroup);

	AccuracyRecoil.SetFileId("acc_norecoil");
	AccuracyGroup.PlaceLabledControl("No Recoil", this, &AccuracyRecoil);

	AccuracyBacktracking.SetFileId("acc_bcktrk");
	AccuracyGroup.PlaceLabledControl("Backtrack", this, &AccuracyBacktracking);

	Accuracylagcomp.SetFileId("acc_lagcomp");
	AccuracyGroup.PlaceLabledControl("Position Adjustment", this, &Accuracylagcomp);

	//Accuracydisableinterp.SetFileId("acc_disableinterp");
	//AccuracyGroup.PlaceLabledControl("Disable Interpolation", this, &Accuracydisableinterp);     JE NAD TIMTO IIIIIIIIIIIIII

	AccuracyAutoWall.SetFileId("acc_awall");
	AccuracyGroup.PlaceLabledControl("Auto Wall", this, &AccuracyAutoWall);

	AccuracyAutoScope.SetFileId("acc_scope");
	AccuracyGroup.PlaceLabledControl("Auto Scope", this, &AccuracyAutoScope);

	//AccuracyPositionAdjustment.SetFileId("acc_posadj");
	//AccuracyGroup.PlaceLabledControl("Backtrack", this, &AccuracyPositionAdjustment); taky nahoøe

	LowerbodyFix.SetFileId("acc_lowerbodyfix");
	AccuracyGroup.PlaceLabledControl("LBY Correction", this, &LowerbodyFix);

	AdvancedResolver.SetFileId("acc_advancedresolver");
	AccuracyGroup.PlaceLabledControl("Pitch Resolver", this, &AdvancedResolver);

	AimbotResolver.SetFileId("acc_aaa");
	AimbotResolver.AddItem("Off");
	AimbotResolver.AddItem("Delta");
	AimbotResolver.AddItem("Delta 2");
	AimbotResolver.AddItem("Bruteforce");
	AimbotResolver.AddItem("Testing (!)");
	AimbotResolver.AddItem("Lowerbody Yaw");
	AimbotResolver.AddItem("LBY + New Delta");
	AccuracyGroup.PlaceLabledControl("Resolver", this, &AimbotResolver);

	AutoHitChance.SetFileId("acc_AutoHitChance");
	AccuracyGroup.PlaceLabledControl("Dynamic Hitchance", this, &AutoHitChance);

	AutoMinimumDamage.SetFileId("acc_AutoMinimumDamage");
	AccuracyGroup.PlaceLabledControl("Dynamic Damage", this, &AutoMinimumDamage);

	AccuracyHitchance.SetFileId("acc_chance");
	AccuracyHitchance.SetBoundaries(0, 100);
	AccuracyHitchance.SetValue(0);
	AccuracyGroup.PlaceLabledControl("Hit Chance", this, &AccuracyHitchance);

	//	AccuracyMinimumDamageAWall.SetFileId("acc_mindmg");
	//	AccuracyMinimumDamageAWall.SetBoundaries(1.f, 99.f);
	//	AccuracyMinimumDamageAWall.SetValue(1.f);
	//	AccuracyGroup.PlaceLabledControl("Minimum Damage", this, &AccuracyMinimumDamageAWall);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(1.f, 99.f);
	AccuracyMinimumDamage.SetValue(1.f);
	AccuracyGroup.PlaceLabledControl("Minimum Damage", this, &AccuracyMinimumDamage);


#pragma endregion  Accuracy controls get Setup in here

#pragma region AntiAim
	AntiAimGroup.SetPosition(408, 336);
	AntiAimGroup.SetText("Anti Aim");
	AntiAimGroup.SetSize(360, 300);
	RegisterControl(&AntiAimGroup);

	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl("Enable", this, &AntiAimEnable);
	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("Off");
	AntiAimPitch.AddItem("Half Down");
	AntiAimPitch.AddItem("Half Up");
	AntiAimPitch.AddItem("Jitter");
	AntiAimPitch.AddItem("Emotion");
	AntiAimPitch.AddItem("Safe Up");
	AntiAimPitch.AddItem("Zero");
	AntiAimPitch.AddItem("Fake Up (!)");
	AntiAimPitch.AddItem("Fake Down (!)");
	AntiAimPitch.AddItem("Fake Up-Down Jitter (!)");
	AntiAimPitch.AddItem("Angel Down (!)");
	AntiAimPitch.AddItem("Angel Up (!)");
	AntiAimPitch.AddItem("True Down (!)");
	AntiAimPitch.AddItem("True Up (!)");
	AntiAimPitch.AddItem("Custom");
	AntiAimGroup.PlaceLabledControl("Pitch", this, &AntiAimPitch);

	CustomPitch.SetFileId("aa_offset");
	CustomPitch.SetBoundaries(0, 89);
	CustomPitch.SetValue(0);
	AntiAimGroup.PlaceLabledControl("Pitch Offset", this, &CustomPitch);

	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("Off");
	AntiAimYaw.AddItem("Fast Spin");
	AntiAimYaw.AddItem("Slow Spin");
	AntiAimYaw.AddItem("Jitter");
	AntiAimYaw.AddItem("180 Jitter");
	AntiAimYaw.AddItem("Backwards");
	AntiAimYaw.AddItem("Backwards Jitter");
	AntiAimYaw.AddItem("Left-Right Switch");
	AntiAimYaw.AddItem("Static Right");
	AntiAimYaw.AddItem("Static Left");
	AntiAimYaw.AddItem("LBY");
	AntiAimYaw.AddItem("LBY Jitter");
	AntiAimYaw.AddItem("LBY Sideways");
	AntiAimYaw.AddItem("LBY Backwards");
	AntiAimYaw.AddItem("LBY Breaker");
	AntiAimYaw.AddItem("Jitter Tank");
	AntiAimYaw.AddItem("Static Tank");
	AntiAimYaw.AddItem("Static Zero Tank");
	AntiAimYaw.AddItem("Sideways [Manual Bind]");
	AntiAimYaw.AddItem("Automatic AA / Adaptive");//2
	AntiAimYaw.AddItem("Sideways Jitter [Manual]");//3
	AntiAimYaw.AddItem("Backwards Jitter New");//4
	AntiAimYaw.AddItem("Lowerbody");//5
	AntiAimYaw.AddItem("Random");//6
	AntiAimYaw.AddItem("Static Forward");//7
	AntiAimYaw.AddItem("LBY new");//7
	AntiAimYaw.AddItem("LBY test");
	AntiAimYaw.AddItem("LBY test2");
	AntiAimGroup.PlaceLabledControl("Real Yaw", this, &AntiAimYaw);

	AntiAimOffset.SetFileId("aa_offset");
	AntiAimOffset.SetBoundaries(0, 360);
	AntiAimOffset.SetValue(0);
	AntiAimGroup.PlaceLabledControl("Real Yaw Offset", this, &AntiAimOffset);

	FakeYaw.SetFileId("fakeaa");
	FakeYaw.AddItem("Off");
	FakeYaw.AddItem("Fast Spin");
	FakeYaw.AddItem("Slow Spin");
	FakeYaw.AddItem("Jitter");
	FakeYaw.AddItem("180 Jitter");
	FakeYaw.AddItem("Backwards");
	FakeYaw.AddItem("Backwards Jitter");
	FakeYaw.AddItem("Left-Right Switch");
	FakeYaw.AddItem("Static Right");
	FakeYaw.AddItem("Static Left");
	FakeYaw.AddItem("LBY");
	FakeYaw.AddItem("LBY Jitter");
	FakeYaw.AddItem("LBY Sideways");
	FakeYaw.AddItem("LBY Backwards");
	FakeYaw.AddItem("LBY Breaker");
	FakeYaw.AddItem("Jitter Tank");
	FakeYaw.AddItem("Static Tank");
	FakeYaw.AddItem("Static Zero Tank");
	FakeYaw.AddItem("Sideways [Manual Bind]");
	FakeYaw.AddItem("Automatic AA / Adaptive");//2
	FakeYaw.AddItem("Sideways Jitter [Manual]");//3
	FakeYaw.AddItem("Backwards Jitter New");//4
	FakeYaw.AddItem("Lowerbody");//5
	FakeYaw.AddItem("Random");//6
	FakeYaw.AddItem("Static Forward");//7
	FakeYaw.AddItem("LBY new");//7
	FakeYaw.AddItem("LBY test");//7
	FakeYaw.AddItem("fLBY test");//7
	AntiAimGroup.PlaceLabledControl("Fake Yaw", this, &FakeYaw);

	AddFakeYaw.SetFileId("aa_offset");
	AddFakeYaw.SetBoundaries(0, 360);
	AddFakeYaw.SetValue(0);
	AntiAimGroup.PlaceLabledControl("Fake Yaw Offset", this, &AddFakeYaw);

	FlipKey.SetFileId("aim_key");
	AntiAimGroup.PlaceLabledControl("AA Side Switch Key", this, &FlipKey);

	LBY.SetFileId("aa_lby");
	AntiAimGroup.PlaceLabledControl("Anti-Ayyware (!)", this, &LBY);

	FlipAA.SetFileId("aa_flip");
	AntiAimGroup.PlaceLabledControl("Flip Yaw between shots", this, &FlipAA);

	AccuracyResolver2.SetFileId("acc_fag");
	AntiAimGroup.PlaceLabledControl("Fakewalk Fix", this, &AccuracyResolver2);

	AntiAimKnife.SetFileId("aa_knife");
	AntiAimGroup.PlaceLabledControl("With Knife/Nade", this, &AntiAimKnife);

	AntiAimTarget.SetFileId("aa_target");
	AntiAimGroup.PlaceLabledControl("At Closest Target", this, &AntiAimTarget);
#pragma endregion  AntiAim controls get setup in here
}

void CLegitBotTab::Setup()
{
	SetTitle("I");

	ActiveLabel.SetPosition(8, 7);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(48, 8.5);
	RegisterControl(&Active);

#pragma region Aimbot
	AimbotGroup.SetPosition(16, 31);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(240, 170);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFriendlyFire.SetFileId("aim_friendfire");
	AimbotGroup.PlaceLabledControl("Friendly Fire", this, &AimbotFriendlyFire);

	AimbotSmokeCheck.SetFileId("otr_smokecheck");
	AimbotGroup.PlaceLabledControl("Smoke Check", this, &AimbotSmokeCheck);

	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key Bind", this, &AimbotKeyBind);


#pragma endregion Aimbot shit

#pragma region Triggerbot
	TriggerGroup.SetPosition(272, 31);
	TriggerGroup.SetText("Triggerbot");
	TriggerGroup.SetSize(240, 170);
	RegisterControl(&TriggerGroup);

	TriggerEnable.SetFileId("trig_enable");
	TriggerGroup.PlaceLabledControl("Enable", this, &TriggerEnable);

	TriggerRecoil.SetFileId("trig_recoil");
	TriggerGroup.PlaceLabledControl("Recoil Control", this, &TriggerRecoil);

	TriggerSmokeCheck.SetFileId("trig_smokecheck");
	TriggerGroup.PlaceLabledControl("Smoke Check", this, &TriggerSmokeCheck);

	TriggerKeyPress.SetFileId("trig_onkey");
	TriggerGroup.PlaceLabledControl("On Key Press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	TriggerGroup.PlaceLabledControl("Key Bind", this, &TriggerKeyBind);

	TriggerDelay.SetFileId("trig_time");
	TriggerDelay.SetBoundaries(0, 100);
	TriggerDelay.SetValue(1);
	TriggerGroup.PlaceLabledControl("Delay", this, &TriggerDelay);

#pragma endregion Triggerbot stuff

#pragma region TriggerbotFilter
	TriggerFilterGroup.SetPosition(528, 31);
	TriggerFilterGroup.SetText("Triggerbot Filter");
	TriggerFilterGroup.SetSize(240, 170);
	RegisterControl(&TriggerFilterGroup);

	TriggerHead.SetFileId("trig_head");
	TriggerFilterGroup.PlaceLabledControl("Head", this, &TriggerHead);

	TriggerChest.SetFileId("trig_chest");
	TriggerFilterGroup.PlaceLabledControl("Chest", this, &TriggerChest);

	TriggerStomach.SetFileId("trig_stomach");
	TriggerFilterGroup.PlaceLabledControl("Stomach", this, &TriggerStomach);

	TriggerArms.SetFileId("trig_arms");
	TriggerFilterGroup.PlaceLabledControl("Arms", this, &TriggerArms);

	TriggerLegs.SetFileId("trig_legs");
	TriggerFilterGroup.PlaceLabledControl("Legs", this, &TriggerLegs);

	TriggerTeammates.SetFileId("trig_teammates");
	TriggerFilterGroup.PlaceLabledControl("Friendly Fire", this, &TriggerTeammates);

#pragma endregion TriggerbotFilter stuff

#pragma region Main Weapon
	WeaponMainGroup.SetPosition(16, 220);
	WeaponMainGroup.SetText("Rifles");
	WeaponMainGroup.SetSize(240, 155);
	RegisterControl(&WeaponMainGroup);

	WeaponMainHitbox.SetFileId("main_hitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainHitbox.AddItem("Multihitbox");
	WeaponMainGroup.PlaceLabledControl("Hitbox", this, &WeaponMainHitbox);

	WeaponMainSpeed.SetFileId("main_speed");
	WeaponMainSpeed.SetBoundaries(0.f, 100.f);
	WeaponMainSpeed.SetValue(1.f);
	WeaponMainGroup.PlaceLabledControl("Max Speed", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fov");
	WeaponMainFoV.SetBoundaries(0.f, 30.f);
	WeaponMainFoV.SetValue(3.5f);
	WeaponMainGroup.PlaceLabledControl("Field of View", this, &WeaponMainFoV);

	WeaponMainInacc.SetFileId("main_inacc");
	WeaponMainInacc.SetBoundaries(0.f, 10.f);
	WeaponMainGroup.PlaceLabledControl("Humanize", this, &WeaponMainInacc);

	WeaponMainRecoil.SetFileId("main_recoil");
	WeaponMainGroup.PlaceLabledControl("Dynamic RCS", this, &WeaponMainRecoil);

	WeaponMainPsilent.SetFileId("main_psi");
	WeaponMainGroup.PlaceLabledControl("pSilent", this, &WeaponMainPsilent);

	WeaponMainAimtime.SetValue(0);
	WeaoponMainStartAimtime.SetValue(0);

#pragma endregion

#pragma region Pistols
	WeaponPistGroup.SetPosition(272, 220);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetSize(240, 155);
	RegisterControl(&WeaponPistGroup);

	WeaponPistHitbox.SetFileId("pist_hitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistHitbox.AddItem("Multihitbox");
	WeaponPistGroup.PlaceLabledControl("Hitbox", this, &WeaponPistHitbox);

	WeaponPistSpeed.SetFileId("pist_speed");
	WeaponPistSpeed.SetBoundaries(0.f, 100.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl("Max Speed", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov");
	WeaponPistFoV.SetBoundaries(0.f, 30.f);
	WeaponPistFoV.SetValue(3.f);
	WeaponPistGroup.PlaceLabledControl("Field of View", this, &WeaponPistFoV);

	WeaponPistInacc.SetFileId("pist_inacc");
	WeaponPistInacc.SetBoundaries(0.f, 10.f);
	WeaponPistGroup.PlaceLabledControl("Humanize", this, &WeaponPistInacc);

	WeaponPistRecoil.SetFileId("pist_recoil");
	WeaponPistGroup.PlaceLabledControl("Dynamic RCS", this, &WeaponPistRecoil);

	WeaponPistPSilent.SetFileId("pist_psi");
	WeaponPistGroup.PlaceLabledControl("pSilent", this, &WeaponPistPSilent);

	WeaponPistAimtime.SetValue(0);
	WeaoponPistStartAimtime.SetValue(0);

#pragma endregion

#pragma region Snipers
	WeaponSnipGroup.SetPosition(528, 220);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetSize(240, 155);
	RegisterControl(&WeaponSnipGroup);

	WeaponSnipHitbox.SetFileId("snip_hitbox");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipHitbox.AddItem("Multihitbox");
	WeaponSnipGroup.PlaceLabledControl("Hitbox", this, &WeaponSnipHitbox);

	WeaponSnipSpeed.SetFileId("snip_speed");
	WeaponSnipSpeed.SetBoundaries(0.f, 100.f);
	WeaponSnipSpeed.SetValue(1.5f);
	WeaponSnipGroup.PlaceLabledControl("Max Speed", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("snip_fov");
	WeaponSnipFoV.SetBoundaries(0.f, 30.f);
	WeaponSnipFoV.SetValue(2.f);
	WeaponSnipGroup.PlaceLabledControl("Field of View", this, &WeaponSnipFoV);

	WeaponSnipInacc.SetFileId("snip_inacc");
	WeaponSnipInacc.SetBoundaries(0.f, 10.f);
	WeaponSnipGroup.PlaceLabledControl("Humanize", this, &WeaponSnipInacc);

	WeaponSnipRecoil.SetFileId("snip_recoil");
	WeaponSnipGroup.PlaceLabledControl("Dynamic RCS", this, &WeaponSnipRecoil);

	WeaponSnipPSilent.SetFileId("snip_psi");
	WeaponSnipGroup.PlaceLabledControl("pSilent", this, &WeaponSnipPSilent);

	WeaponSnipAimtime.SetValue(0);
	WeaoponSnipStartAimtime.SetValue(0);

#pragma region MPs
	WeaponMpGroup.SetPosition(16, 385);
	WeaponMpGroup.SetText("MPs");
	WeaponMpGroup.SetSize(240, 155);
	RegisterControl(&WeaponMpGroup);

	WeaponMpHitbox.SetFileId("mps_hitbox");
	WeaponMpHitbox.AddItem("Head");
	WeaponMpHitbox.AddItem("Neck");
	WeaponMpHitbox.AddItem("Chest");
	WeaponMpHitbox.AddItem("Stomach");
	WeaponMpHitbox.AddItem("Multihitbox");
	WeaponMpGroup.PlaceLabledControl("Hitbox", this, &WeaponMpHitbox);

	WeaponMpSpeed.SetFileId("mps_speed");
	WeaponMpSpeed.SetBoundaries(0.f, 100.f);
	WeaponMpSpeed.SetValue(1.0f);
	WeaponMpGroup.PlaceLabledControl("Max Speed", this, &WeaponMpSpeed);

	WeaponMpFoV.SetFileId("mps_fov");
	WeaponMpFoV.SetBoundaries(0.f, 30.f);
	WeaponMpFoV.SetValue(4.f);
	WeaponMpGroup.PlaceLabledControl("Field of View", this, &WeaponMpFoV);

	WeaponMpInacc.SetFileId("mps_inacc");
	WeaponMpInacc.SetBoundaries(0.f, 10.f);
	WeaponMpGroup.PlaceLabledControl("Humanize", this, &WeaponMpInacc);

	WeaponMpRecoil.SetFileId("mps_recoil");
	WeaponMpGroup.PlaceLabledControl("Dynamic RCS", this, &WeaponMpRecoil);

	WeaponMpPsilent.SetFileId("mps_psi");
	WeaponMpGroup.PlaceLabledControl("pSilent", this, &WeaponMpPsilent);

	WeaponMpAimtime.SetValue(0);
	WeaoponMpStartAimtime.SetValue(0);
#pragma endregion

#pragma region Shotguns
	WeaponShotgunGroup.SetPosition(272, 385);
	WeaponShotgunGroup.SetText("Shotguns");
	WeaponShotgunGroup.SetSize(240, 155);
	RegisterControl(&WeaponShotgunGroup);

	WeaponShotgunHitbox.SetFileId("shotgun_hitbox");
	WeaponShotgunHitbox.AddItem("Head");
	WeaponShotgunHitbox.AddItem("Neck");
	WeaponShotgunHitbox.AddItem("Chest");
	WeaponShotgunHitbox.AddItem("Stomach");
	WeaponShotgunHitbox.AddItem("Nearest");
	WeaponShotgunGroup.PlaceLabledControl("Hitbox", this, &WeaponShotgunHitbox);

	WeaponShotgunSpeed.SetFileId("shotgun_speed");
	WeaponShotgunSpeed.SetBoundaries(0.f, 100.f);
	WeaponShotgunSpeed.SetValue(1.0f);
	WeaponShotgunGroup.PlaceLabledControl("Max Speed", this, &WeaponShotgunSpeed);

	WeaponShotgunFoV.SetFileId("shotgun_fov");
	WeaponShotgunFoV.SetBoundaries(0.f, 30.f);
	WeaponShotgunFoV.SetValue(3.f);
	WeaponShotgunGroup.PlaceLabledControl("Field of View", this, &WeaponShotgunFoV);

	WeaponShotgunInacc.SetFileId("mps_inacc");
	WeaponShotgunInacc.SetBoundaries(0.f, 10.f);
	WeaponShotgunGroup.PlaceLabledControl("Humanize", this, &WeaponShotgunInacc);

	WeaponShotgunRecoil.SetFileId("snip_recoil");
	WeaponShotgunGroup.PlaceLabledControl("Dynamic RCS", this, &WeaponShotgunRecoil);

	WeaponShotgunPsilent.SetFileId("shotgun_psi");
	WeaponShotgunGroup.PlaceLabledControl("pSilent", this, &WeaponShotgunPsilent);

	WeaponShotgunAimtime.SetValue(0);
	WeaoponShotgunStartAimtime.SetValue(0);

#pragma endregion

#pragma region Machineguns
	WeaponMGGroup.SetPosition(528, 385);
	WeaponMGGroup.SetText("Heavy");
	WeaponMGGroup.SetSize(240, 155);
	RegisterControl(&WeaponMGGroup);

	WeaponMGHitbox.SetFileId("mg_hitbox");
	WeaponMGHitbox.AddItem("Head");
	WeaponMGHitbox.AddItem("Neck");
	WeaponMGHitbox.AddItem("Chest");
	WeaponMGHitbox.AddItem("Stomach");
	WeaponMGHitbox.AddItem("Nearest");
	WeaponMGGroup.PlaceLabledControl("Hitbox", this, &WeaponMGHitbox);

	WeaponMGSpeed.SetFileId("mg_speed");
	WeaponMGSpeed.SetBoundaries(0.f, 100.f);
	WeaponMGSpeed.SetValue(1.0f);
	WeaponMGGroup.PlaceLabledControl("Max Speed", this, &WeaponMGSpeed);

	WeaponMGFoV.SetFileId("mg_fov");
	WeaponMGFoV.SetBoundaries(0.f, 30.f);
	WeaponMGFoV.SetValue(4.f);
	WeaponMGGroup.PlaceLabledControl("Field of View", this, &WeaponMGFoV);

	WeaponMGInacc.SetFileId("mg_inacc");
	WeaponMGInacc.SetBoundaries(0.f, 10.f);
	WeaponMGGroup.PlaceLabledControl("Humanize", this, &WeaponMGInacc);

	WeaponMGRecoil.SetFileId("mg_recoil");
	WeaponMGGroup.PlaceLabledControl("Dynamic RCS", this, &WeaponMGRecoil);

	WeaponMGPsilent.SetFileId("mg_psi");
	WeaponMGGroup.PlaceLabledControl("pSilent", this, &WeaponMGPsilent);

	WeaponMGAimtime.SetValue(0);
	WeaoponMGStartAimtime.SetValue(0);

#pragma endregion

#pragma region Other setts in lbot
	Other.SetPosition(16, 560);
	Other.SetText("Extra Settings // more soon");
	Other.SetSize(240, 85);
	RegisterControl(&Other);

	LegitAA.SetFileId("other_legitaa");
	Other.PlaceLabledControl("Legit AA (temporary off)", this, &LegitAA);

	AimbotBacktrack.SetFileId("legit_backtrack");
	AimbotBacktrack.SetState(true);
	Other.PlaceLabledControl("Backtrack", this, &AimbotBacktrack);

	TickModulation.SetFileId("tick_modulate");
	TickModulation.SetBoundaries(0.f, 12.f);
	TickModulation.SetValue(12.f);
	Other.PlaceLabledControl("Ticks", this, &TickModulation);

#pragma endregion Other setts in lbot 

}
void CVisualTab::Setup()
{
	SetTitle("D");

	ActiveLabel.SetPosition(8, 7);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(48, 8.5);
	RegisterControl(&Active);

#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.SetPosition(16, 48);
	OptionsGroup.SetSize(193, 430);
	RegisterControl(&OptionsGroup);

	OptionsBox.SetFileId("opt_box");
	OptionsBox.AddItem("Off");
	OptionsBox.AddItem("Corners");
	OptionsBox.AddItem("Full");
	OptionsBox.AddItem("Outline");
	OptionsGroup.PlaceLabledControl("Box", this, &OptionsBox);

	OptionsChams.SetFileId("opt_chams");
	OptionsChams.AddItem("Off");
	OptionsChams.AddItem("Normal");
	OptionsChams.AddItem("Flat");
	OptionsGroup.PlaceLabledControl("Chams", this, &OptionsChams);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsSkeleton.AddItem("Off");
	OptionsSkeleton.AddItem("Normal");
	OptionsSkeleton.AddItem("HP Based");
	OptionsGroup.PlaceLabledControl(" Skeleton", this, &OptionsSkeleton);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceLabledControl("Name", this, &OptionsName);

	OptionsHealth.SetFileId("opt_hp");
	OptionsGroup.PlaceLabledControl("Health", this, &OptionsHealth);

	OptionsArmor.SetFileId("opt_armor");
	OptionsGroup.PlaceLabledControl("Armor", this, &OptionsArmor);

	OptionsMoney.SetFileId("opt_money");
	OptionsGroup.PlaceLabledControl("Money", this, &OptionsMoney);

	OptionsWeapon.SetFileId("opt_weapon");
	OptionsGroup.PlaceLabledControl("Weapon", this, &OptionsWeapon);

	ResolverInfo.SetFileId("opt_resinfo");
	OptionsGroup.PlaceLabledControl("Resolver Info", this, &ResolverInfo);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl("Weapon Info", this, &OptionsInfo);

	Seethrough.SetFileId("opt_inv");
	OptionsGroup.PlaceLabledControl("crying when scoped", this, &Seethrough);

	IsScoped.SetFileId("opt_infoscoped");
	OptionsGroup.PlaceLabledControl("Is Scoped", this, &IsScoped);

	OtherPenetration.SetFileId("otr_penetration");
	OptionsGroup.PlaceLabledControl("Draw penetration", this, &OtherPenetration);

	OptionsBarrels.SetFileId("opt_barrels");
	OptionsGroup.PlaceLabledControl("Trace Lines", this, &OptionsBarrels);

	OtherSpreadCrosshair.SetFileId("otr_spreadhair");
	OtherGroup.PlaceLabledControl("Draw innacuracy", this, &OtherSpreadCrosshair);

	//	ChamsVisibleOnly.SetFileId("opt_chamsvisonly");
	//	OptionsGroup.PlaceLabledControl("Legit Chams", this, &ChamsVisibleOnly);

	OptionsGlow.SetFileId("opt_glow");
	OptionsGroup.PlaceLabledControl("Glow", this, &OptionsGlow);

	OptionsCompRank.SetFileId("opt_comprank");
	OptionsGroup.PlaceLabledControl("Comp Rank", this, &OptionsCompRank);

	lbyidicador.SetFileId("opt_lbyidicador");
	OptionsGroup.PlaceLabledControl("LBY Indicador", this, &lbyidicador);

	BacktrackingLol.SetFileId("opt_backdot");
	OptionsGroup.PlaceLabledControl("Backtrack Visualization", this, &BacktrackingLol);

	OtherHitmarker.SetFileId("otr_hitmarker");
	OptionsGroup.PlaceLabledControl("Hitmarker", this, &OtherHitmarker);

	HitmarkerSound.SetFileId("otr_hitmarkersound");
	OptionsGroup.PlaceLabledControl("Hitmarker Sound", this, &HitmarkerSound);

	OtherPlayerLine.SetFileId("otr_angllins");
	OptionsGroup.PlaceLabledControl("AA Angles in TP", this, &OtherPlayerLine);

	Logs.SetFileId("otr_logs");
	OptionsGroup.PlaceCheckBox("Logs", this, &Logs);



	//	ShowImpacts.SetFileId("opt_impacts");
	//	OptionsGroup.PlaceLabledControl("Show Impcats Spoof", this, &ShowImpacts);

	//	GrenadeTracer.SetFileId("opt_grentrail");
	//	OptionsGroup.PlaceLabledControl("Grenade Path Spoof", this, &GrenadeTracer);

#pragma endregion Setting up the Options controls

#pragma region Filters
	FiltersGroup.SetText("Filters");
	FiltersGroup.SetPosition(225, 48);
	FiltersGroup.SetSize(193, 430);
	RegisterControl(&FiltersGroup);

	FiltersAll.SetFileId("ftr_all");
	FiltersGroup.PlaceLabledControl("All", this, &FiltersAll);

	FiltersPlayers.SetFileId("ftr_players");
	FiltersGroup.PlaceLabledControl("Players", this, &FiltersPlayers);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	FiltersGroup.PlaceLabledControl("Enemies Only", this, &FiltersEnemiesOnly);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersGroup.PlaceLabledControl("Weapons", this, &FiltersWeapons);

	FiltersChickens.SetFileId("ftr_chickens");
	FiltersGroup.PlaceLabledControl("Chickens", this, &FiltersChickens);

	FiltersC4.SetFileId("ftr_c4");
	FiltersGroup.PlaceLabledControl("C4", this, &FiltersC4);

	FiltersNightMod.SetFileId("ftr_m,");
	FiltersGroup.PlaceLabledControl("Night Mode (!)", this, &FiltersNightMod);

#pragma endregion Setting up the Filters controls

#pragma region Other
	OtherGroup.SetText("Other");
	OtherGroup.SetPosition(430, 48);
	OtherGroup.SetSize(334, 430);
	RegisterControl(&OtherGroup);

	OtherCrosshair.SetFileId("otr_crosshair");
	OtherCrosshair.AddItem("Default");
	OtherCrosshair.AddItem("Red Recoil");
	OtherCrosshair.AddItem("Red Static");
	OtherCrosshair.AddItem("Spread Crosshair");
	OtherGroup.PlaceLabledControl("Crosshairs", this, &OtherCrosshair);


	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("Off");
	OtherNoHands.AddItem("None");
	OtherNoHands.AddItem("Transparent");
	OtherNoHands.AddItem("Chams");
	OtherNoHands.AddItem("Rainbow");
	OtherGroup.PlaceLabledControl("Hands", this, &OtherNoHands);

	SniperCrosshair.SetFileId("otr_snipercrosshair");
	OtherGroup.PlaceLabledControl("Sniper Crosshair", this, &SniperCrosshair);

	NightSky.SetFileId("otr_nightsky");
	OtherGroup.PlaceLabledControl("Night Sky", this, &NightSky);

	DisablePostProcess.SetFileId("otr_disablepostprocess");
	OtherGroup.PlaceLabledControl("Disable Post Process", this, &DisablePostProcess);

	OtherNoFlash.SetFileId("otr_noflash");
	OtherGroup.PlaceLabledControl("Remove Flash", this, &OtherNoFlash);

	OtherNoSmoke.SetFileId("otr_nosmoke");
	OtherGroup.PlaceLabledControl("Remove Smoke", this, &OtherNoSmoke);

	OtherNoScope.SetFileId("otr_noscope");
	OtherGroup.PlaceLabledControl("Remove Scope", this, &OtherNoScope);

	//GrenadeTrace.SetFileId("opt_grenadetrace"); 
	//OptionsGroup.PlaceLabledControl("Grenade Trace", this, &GrenadeTrace);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceLabledControl("No Visual Recoil", this, &OtherNoVisualRecoil);

	Clock.SetFileId("otr_clock");
	OtherGroup.PlaceLabledControl("Hours", this, &Clock);

	Watermark.SetFileId("otr_watermarker");
	Watermark.SetState(true);
	OtherGroup.PlaceLabledControl("Watermark", this, &Watermark);

	OtherRadar.SetFileId("otr_radar");
	OtherGroup.PlaceLabledControl("Radar", this, &OtherRadar);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(0.f, 180.f);
	OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Viewmodel FOV", this, &OtherViewmodelFOV);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 180.f);
	OtherFOV.SetValue(90.f);
	OtherGroup.PlaceLabledControl("Field of View", this, &OtherFOV);

	AmbientRed.SetFileId("otr_ambientred");
	AmbientRed.SetBoundaries(0.f, 10.f);
	AmbientRed.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Night Vision Red", this, &AmbientRed);

	AmbientGreen.SetFileId("otr_ambientgreen");
	AmbientGreen.SetBoundaries(0.f, 10.f);
	AmbientGreen.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Night Vision Green", this, &AmbientGreen);

	AmbientBlue.SetFileId("otr_ambientblue");
	AmbientBlue.SetBoundaries(0.f, 10.f);
	AmbientBlue.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Night Vision Blue", this, &AmbientBlue);

	NightMod.SetFileId("night_amm");
	NightMod.SetBoundaries(0.f, 1.f);
	NightMod.SetValue(.1f);
	OtherGroup.PlaceLabledControl("  NightMode Value", this, &NightMod);

	OtherNightMode.SetFileId("otr_nightsky");
	OtherGroup.PlaceLabledControl("Force Refresh", this, &OtherNightMode);

#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	SetTitle("G");

#pragma region Other
	OtherGroup.SetPosition(408, 48);
	OtherGroup.SetSize(360, 430);
	OtherGroup.SetText("Other");
	RegisterControl(&OtherGroup);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherGroup.PlaceLabledControl("BunnyHop", this, &OtherAutoJump);

	OtherEdgeJump.SetFileId("otr_edgejump");
	OtherGroup.PlaceLabledControl("Edge Jump", this, &OtherEdgeJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherAutoStrafe.AddItem("Off");
	OtherAutoStrafe.AddItem("Legit");
	OtherAutoStrafe.AddItem("Rage");
	OtherGroup.PlaceLabledControl("Auto Strafer", this, &OtherAutoStrafe);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceLabledControl("Anti Untrusted", this, &OtherSafeMode);

	OtherChatSpam.SetFileId("otr_spam");
	OtherChatSpam.AddItem("Off");
	OtherChatSpam.AddItem("Namestealer");
	OtherChatSpam.AddItem("beartraphook Chat");
	OtherChatSpam.AddItem("beartraphook Name");
	OtherChatSpam.AddItem("White Name");
	OtherGroup.PlaceLabledControl("Chat Spam", this, &OtherChatSpam);

	TrashTalk.SetFileId("otr_trashtalk");
	OtherGroup.PlaceLabledControl("ChatSpam Kill", this, &TrashTalk);

	OtherClantag.SetFileId("otr_spam");
	OtherClantag.AddItem("Off");
	OtherClantag.AddItem("beartraphook");
	OtherClantag.AddItem("Blank");
	OtherClantag.AddItem("Valve");
	OtherClantag.AddItem("Hours");
	OtherGroup.PlaceLabledControl("Custom Clantag", this, &OtherClantag);

	//	OtherTeamChat.SetFileId("otr_teamchat");
	//	OtherGroup.PlaceLabledControl("Team Chat Only", this, &OtherTeamChat);

	//	OtherChatDelay.SetFileId("otr_chatdelay");
	//	OtherChatDelay.SetBoundaries(0.1, 3.0);
	//	OtherChatDelay.SetValue(1.0);
	//	OtherGroup.PlaceLabledControl("Spam Delay", this, &OtherChatDelay);

	OtherAirStuck.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Air Stuck", this, &OtherAirStuck);

	OtherSpectators.SetFileId("otr_speclist");
	OtherGroup.PlaceLabledControl("Spectators List", this, &OtherSpectators);

	OtherThirdpersonKey.SetFileId("otr_tpkeylul");
	OtherGroup.PlaceLabledControl("Thirdperson", this, &OtherThirdpersonKey);

	OtherThirdpersonType.SetFileId("otr_tptypelul");
	OtherThirdpersonType.AddItem("Real Angle");
	OtherThirdpersonType.AddItem("Fake Angle");
	OtherGroup.PlaceLabledControl("Thirdperson Type", this, &OtherThirdpersonType);


	OtherAutoAccept.SetFileId("otr_autoaccept");
	OtherGroup.PlaceLabledControl("Auto Accept", this, &OtherAutoAccept);

#pragma endregion other random options

#pragma region FakeLag

	FakeLagGroup.SetPosition(16, 48);
	FakeLagGroup.SetSize(360, 137);
	FakeLagGroup.SetText("Fakelag");
	RegisterControl(&FakeLagGroup);

	FakeLagEnable.SetFileId("fakelag_enable");
	FakeLagGroup.PlaceLabledControl("Enable", this, &FakeLagEnable);

	FakeLagChoke.SetFileId("fakelag_choke");
	FakeLagChoke.SetBoundaries(0, 18);
	FakeLagChoke.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Choke Factor", this, &FakeLagChoke);

	FakeLagTyp.SetFileId("fakelag_typ");
	FakeLagTyp.AddItem("Off");
	FakeLagTyp.AddItem("Normal");
	FakeLagTyp.AddItem("Normal - Mooving");
	FakeLagTyp.AddItem("Normal - In Air");
	FakeLagTyp.AddItem("Adaptive - In Air");
	FakeLagTyp.AddItem("Lagcomp");
	FakeLagGroup.PlaceOtherControl("Mode", this, &FakeLagTyp);

#pragma region Buybot
	BuyBotGroup.SetPosition(16, 188);
	BuyBotGroup.SetSize(370, 150);
	BuyBotGroup.SetText("Buybot");
	RegisterControl(&BuyBotGroup);

	EnableBuyBot.SetFileId("bb_enable");
	BuyBotGroup.PlaceLabledControl("Enable", this, &EnableBuyBot);

	BuyBot.SetFileId("buybot");
	BuyBot.AddItem("Off");
	BuyBot.AddItem("Ak/M4");
	BuyBot.AddItem("AWP");
	BuyBot.AddItem("Auto + Duals");
	BuyBotGroup.PlaceLabledControl("Buy Bot", this, &BuyBot);

	BuyBotGrenades.SetFileId("buybot_grenades");
	BuyBotGrenades.AddItem("Off");
	BuyBotGrenades.AddItem("Flash + Smoke + HE");
	BuyBotGrenades.AddItem("Flash + Smoke + HE + Molo");
	BuyBotGroup.PlaceLabledControl("Buy Grenades", this, &BuyBotGrenades);

	BuyBotKevlar.SetFileId("buybot_kevlar");
	BuyBotGroup.PlaceLabledControl("Buy Armor", this, &BuyBotKevlar);

	BuyBotDefuser.SetFileId("buybot_defuser");
	BuyBotGroup.PlaceLabledControl("Buy Defuse Kit", this, &BuyBotDefuser);

#pragma endregion Buybot
#pragma endregion fakelag shit
}

void CSkinchangerTab::Setup()
{
	SetTitle("B");

	SkinActive.SetPosition(10, 8);
	SkinActive.SetText("Active");
	RegisterControl(&SkinActive);

	SkinEnable.SetFileId("skin_enable");
	SkinEnable.SetPosition(55, 8);
	RegisterControl(&SkinEnable);

#pragma region Knife
	KnifeGroup.SetPosition(16, 11);
	KnifeGroup.SetText("Knife");
	KnifeGroup.SetSize(376, 80);
	RegisterControl(&KnifeGroup);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("Bayonet");
	KnifeModel.AddItem("Bowie Knife");
	KnifeModel.AddItem("Butterfly Knife");
	KnifeModel.AddItem("Falchion Knife");
	KnifeModel.AddItem("Flip Knife");
	KnifeModel.AddItem("Gut Knife");
	KnifeModel.AddItem("Huntsman Knife");
	KnifeModel.AddItem("Karambit");
	KnifeModel.AddItem("M9 Bayonet");
	KnifeModel.AddItem("Shadow Daggers");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("Vanilla");
	KnifeSkin.AddItem("Crimson Web");
	KnifeSkin.AddItem("Bone Mask");
	KnifeSkin.AddItem("Fade");
	KnifeSkin.AddItem("Night");
	KnifeSkin.AddItem("Blue Steel");
	KnifeSkin.AddItem("Stained");
	KnifeSkin.AddItem("Case Hardened");
	KnifeSkin.AddItem("Slaughter");
	KnifeSkin.AddItem("Safari Mesh");
	KnifeSkin.AddItem("Boreal Forest");
	KnifeSkin.AddItem("Ultraviolet");
	KnifeSkin.AddItem("Urban Masked");
	KnifeSkin.AddItem("Scorched");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Tiger Tooth");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Marble Fade");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Doppler Ruby");
	KnifeSkin.AddItem("Doppler Sapphire");
	KnifeSkin.AddItem("Doppler Blackpearl");
	KnifeSkin.AddItem("Doppler Phase 1");
	KnifeSkin.AddItem("Doppler Phase 2");
	KnifeSkin.AddItem("Doppler Phase 3");
	KnifeSkin.AddItem("Doppler Phase 4");
	KnifeSkin.AddItem("Gamma Doppler Phase 1");
	KnifeSkin.AddItem("Gamma Doppler Phase 2");
	KnifeSkin.AddItem("Gamma Doppler Phase 3");
	KnifeSkin.AddItem("Gamma Doppler Phase 4");
	KnifeSkin.AddItem("Gamma Doppler Emerald");
	KnifeSkin.AddItem("Lore");
	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);
#pragma endregion

#pragma region Machineguns
	MachinegunsGroup.SetPosition(408, 11);
	MachinegunsGroup.SetText("Heavy");
	MachinegunsGroup.SetSize(360, 80);
	RegisterControl(&MachinegunsGroup);

	NEGEVSkin.SetFileId("negev_skin");
	NEGEVSkin.AddItem("Anodized Navy");
	NEGEVSkin.AddItem("Man-o'-war");
	NEGEVSkin.AddItem("Palm");
	NEGEVSkin.AddItem("VariCamo");
	NEGEVSkin.AddItem("Palm");
	NEGEVSkin.AddItem("CaliCamo");
	NEGEVSkin.AddItem("Terrain");
	NEGEVSkin.AddItem("Army Sheen");
	NEGEVSkin.AddItem("Bratatat");
	NEGEVSkin.AddItem("Desert-Strike");
	NEGEVSkin.AddItem("Nuclear Waste");
	NEGEVSkin.AddItem("Loudmouth");
	NEGEVSkin.AddItem("Power Loader");
	MachinegunsGroup.PlaceLabledControl("Negev", this, &NEGEVSkin);

	M249Skin.SetFileId("m249_skin");
	M249Skin.AddItem("Contrast Spray");
	M249Skin.AddItem("Blizzard Marbleized");
	M249Skin.AddItem("Jungle DDPAT");
	M249Skin.AddItem("Gator Mesh");
	M249Skin.AddItem("Magma");
	M249Skin.AddItem("System Lock");
	M249Skin.AddItem("Shipping Forecast");
	M249Skin.AddItem("Impact Drill");
	M249Skin.AddItem("Nebula Crusader");
	M249Skin.AddItem("Spectre");
	MachinegunsGroup.PlaceLabledControl("M249", this, &M249Skin);

#pragma endregion

#pragma region Snipers
	Snipergroup.SetPosition(16, 98);
	Snipergroup.SetText("Snipers");
	Snipergroup.SetSize(376, 125);
	RegisterControl(&Snipergroup);

	// AWP
	AWPSkin.SetFileId("awp_skin");
	AWPSkin.AddItem("BOOM");
	AWPSkin.AddItem("Dragon Lore");
	AWPSkin.AddItem("Pink DDPAT");
	AWPSkin.AddItem("Snake Camo");
	AWPSkin.AddItem("Lightning Strike");
	AWPSkin.AddItem("Safari Mesh");
	AWPSkin.AddItem("Corticera");
	AWPSkin.AddItem("Redline");
	AWPSkin.AddItem("Man-o'-war");
	AWPSkin.AddItem("Graphite");
	AWPSkin.AddItem("Electric Hive");
	AWPSkin.AddItem("Pit Viper");
	AWPSkin.AddItem("Asiimov");
	AWPSkin.AddItem("Worm God");
	AWPSkin.AddItem("Medusa");
	AWPSkin.AddItem("Sun in Leo");
	AWPSkin.AddItem("Hyper Beast");
	AWPSkin.AddItem("Elite Build");
	AWPSkin.AddItem("Fever Dream");
	AWPSkin.AddItem("Oni Taiji");
	Snipergroup.PlaceLabledControl("AWP", this, &AWPSkin);

	// SSG08
	SSG08Skin.SetFileId("sgg08_skin");
	SSG08Skin.AddItem("Lichen Dashed");
	SSG08Skin.AddItem("Dark Water");
	SSG08Skin.AddItem("Blue Spruce");
	SSG08Skin.AddItem("Sand Dune");
	SSG08Skin.AddItem("Palm");
	SSG08Skin.AddItem("Mayan Dreams");
	SSG08Skin.AddItem("Blood in the Water");
	SSG08Skin.AddItem("Tropical Storm");
	SSG08Skin.AddItem("Acid Fade");
	SSG08Skin.AddItem("Slashed");
	SSG08Skin.AddItem("Detour");
	SSG08Skin.AddItem("Abyss");
	SSG08Skin.AddItem("Big Iron");
	SSG08Skin.AddItem("Necropos");
	SSG08Skin.AddItem("Ghost Crusader");
	SSG08Skin.AddItem("Dragonfire");
	Snipergroup.PlaceLabledControl("SGG 08", this, &SSG08Skin);

	// SCAR20
	SCAR20Skin.SetFileId("scar20_skin");
	SCAR20Skin.AddItem("Splash Jam");
	SCAR20Skin.AddItem("Storm");
	SCAR20Skin.AddItem("Contractor");
	SCAR20Skin.AddItem("Carbon Fiber");
	SCAR20Skin.AddItem("Sand Mesh");
	SCAR20Skin.AddItem("Palm");
	SCAR20Skin.AddItem("Emerald");
	SCAR20Skin.AddItem("Crimson Web");
	SCAR20Skin.AddItem("Cardiac");
	SCAR20Skin.AddItem("Army Sheen");
	SCAR20Skin.AddItem("Cyrex");
	SCAR20Skin.AddItem("Grotto");
	SCAR20Skin.AddItem("Emerald");
	SCAR20Skin.AddItem("Green Marine");
	SCAR20Skin.AddItem("Outbreak");
	SCAR20Skin.AddItem("Bloodsport");
	Snipergroup.PlaceLabledControl("SCAR-20", this, &SCAR20Skin);

	// G3SG1
	G3SG1Skin.SetFileId("g3sg1_skin");
	G3SG1Skin.AddItem("Desert Storm");
	G3SG1Skin.AddItem("Arctic Camo");
	G3SG1Skin.AddItem("Bone Mask");
	G3SG1Skin.AddItem("Contractor");
	G3SG1Skin.AddItem("Safari Mesh");
	G3SG1Skin.AddItem("Polar Camo");
	G3SG1Skin.AddItem("Jungle Dashed");
	G3SG1Skin.AddItem("VariCamo");
	G3SG1Skin.AddItem("Predator");
	G3SG1Skin.AddItem("Demeter");
	G3SG1Skin.AddItem("Azure Zebra");
	G3SG1Skin.AddItem("Green Apple");
	G3SG1Skin.AddItem("Orange Kimono");
	G3SG1Skin.AddItem("Neon Kimono");
	G3SG1Skin.AddItem("Murky");
	G3SG1Skin.AddItem("Chronos");
	G3SG1Skin.AddItem("Flux");
	G3SG1Skin.AddItem("The Executioner");
	G3SG1Skin.AddItem("Orange Crash");
	Snipergroup.PlaceLabledControl("G3SG1", this, &G3SG1Skin);
#pragma endregion

#pragma region Shotguns
	Shotgungroup.SetPosition(408, 98);
	Shotgungroup.SetText("Shotguns");
	Shotgungroup.SetSize(360, 125);
	RegisterControl(&Shotgungroup);

	MAG7Skin.SetFileId("mag7_skin");
	MAG7Skin.AddItem("Counter Terrace");
	MAG7Skin.AddItem("Metallic DDPAT");
	MAG7Skin.AddItem("Silver");
	MAG7Skin.AddItem("Storm");
	MAG7Skin.AddItem("Bulldozer");
	MAG7Skin.AddItem("Heat");
	MAG7Skin.AddItem("Sand Dune");
	MAG7Skin.AddItem("Irradiated Alert");
	MAG7Skin.AddItem("Memento");
	MAG7Skin.AddItem("Hazard");
	MAG7Skin.AddItem("Heaven Guard");
	MAG7Skin.AddItem("Firestarter");
	MAG7Skin.AddItem("Seabird");
	MAG7Skin.AddItem("Cobalt Core");
	MAG7Skin.AddItem("Praetorian");
	Shotgungroup.PlaceLabledControl("Mag-7", this, &MAG7Skin);

	XM1014Skin.SetFileId("xm1014_skin");
	XM1014Skin.AddItem("Blaze Orange");
	XM1014Skin.AddItem("VariCamo Blue");
	XM1014Skin.AddItem("Bone Mask");
	XM1014Skin.AddItem("Blue Steel");
	XM1014Skin.AddItem("Blue Spruce");
	XM1014Skin.AddItem("Grassland");
	XM1014Skin.AddItem("Urban Perforated");
	XM1014Skin.AddItem("Jungle");
	XM1014Skin.AddItem("VariCamo");
	XM1014Skin.AddItem("VariCamo");
	XM1014Skin.AddItem("Fallout Warning");
	XM1014Skin.AddItem("Jungle");
	XM1014Skin.AddItem("CaliCamo");
	XM1014Skin.AddItem("Pit Viper");
	XM1014Skin.AddItem("Tranquility");
	XM1014Skin.AddItem("Red Python");
	XM1014Skin.AddItem("Heaven Guard");
	XM1014Skin.AddItem("Red Leather");
	XM1014Skin.AddItem("Bone Machine");
	XM1014Skin.AddItem("Quicksilver");
	XM1014Skin.AddItem("Scumbria");
	XM1014Skin.AddItem("Teclu Burner");
	XM1014Skin.AddItem("Black Tie");
	Shotgungroup.PlaceLabledControl("XM1014", this, &XM1014Skin);

	SAWEDOFFSkin.SetFileId("sawedoff_skin");
	SAWEDOFFSkin.AddItem("First Class");
	SAWEDOFFSkin.AddItem("Forest DDPAT");
	SAWEDOFFSkin.AddItem("Contrast Spray");
	SAWEDOFFSkin.AddItem("Snake Camo");
	SAWEDOFFSkin.AddItem("Orange DDPAT");
	SAWEDOFFSkin.AddItem("Fade");
	SAWEDOFFSkin.AddItem("Copper");
	SAWEDOFFSkin.AddItem("Origami");
	SAWEDOFFSkin.AddItem("Sage Spray");
	SAWEDOFFSkin.AddItem("VariCamo");
	SAWEDOFFSkin.AddItem("Irradiated Alert");
	SAWEDOFFSkin.AddItem("Mosaico");
	SAWEDOFFSkin.AddItem("Serenity");
	SAWEDOFFSkin.AddItem("Amber Fade");
	SAWEDOFFSkin.AddItem("Full Stop");
	SAWEDOFFSkin.AddItem("Highwayman");
	SAWEDOFFSkin.AddItem("The Kraken");
	SAWEDOFFSkin.AddItem("Rust Coat");
	SAWEDOFFSkin.AddItem("Bamboo Shadow");
	SAWEDOFFSkin.AddItem("Bamboo Forest");
	SAWEDOFFSkin.AddItem("Yorick");
	SAWEDOFFSkin.AddItem("Fubar");
	SAWEDOFFSkin.AddItem("Wasteland Princess");
	Shotgungroup.PlaceLabledControl("Sawed-Off", this, &SAWEDOFFSkin);

	NOVASkin.SetFileId("nova_skin");
	NOVASkin.AddItem("Candy Apple");
	NOVASkin.AddItem("Blaze Orange");
	NOVASkin.AddItem("Modern Hunter");
	NOVASkin.AddItem("Forest Leaves");
	NOVASkin.AddItem("Bloomstick");
	NOVASkin.AddItem("Sand Dune");
	NOVASkin.AddItem("Polar Mesh");
	NOVASkin.AddItem("Walnut");
	NOVASkin.AddItem("Predator");
	NOVASkin.AddItem("Tempest");
	NOVASkin.AddItem("Graphite");
	NOVASkin.AddItem("Ghost Camo");
	NOVASkin.AddItem("Rising Skull");
	NOVASkin.AddItem("Antique");
	NOVASkin.AddItem("Green Apple");
	NOVASkin.AddItem("Caged Steel");
	NOVASkin.AddItem("Koi");
	NOVASkin.AddItem("Moon in Libra");
	NOVASkin.AddItem("Ranger");
	NOVASkin.AddItem("HyperBeast");
	Shotgungroup.PlaceLabledControl("Nova", this, &NOVASkin);
#pragma endregion

#pragma region Rifles
	Riflegroup.SetPosition(16, 233);
	Riflegroup.SetText("Rifles");
	Riflegroup.SetSize(376, 200);
	RegisterControl(&Riflegroup);

	AK47Skin.SetFileId("ak47_skin");
	AK47Skin.AddItem("First Class");
	AK47Skin.AddItem("Red Laminate");
	AK47Skin.AddItem("Case Hardened");
	AK47Skin.AddItem("Black Laminate");
	AK47Skin.AddItem("Fire Serpent");
	AK47Skin.AddItem("Cartel");
	AK47Skin.AddItem("Emerald Pinstripe");
	AK47Skin.AddItem("Blue Laminate");
	AK47Skin.AddItem("Redline");
	AK47Skin.AddItem("Vulcan");
	AK47Skin.AddItem("Jaguar");
	AK47Skin.AddItem("Jet Set");
	AK47Skin.AddItem("Wasteland Rebel");
	AK47Skin.AddItem("Elite Build");
	AK47Skin.AddItem("Hydroponic");
	AK47Skin.AddItem("Aquamarine Revenge");
	AK47Skin.AddItem("Frontside Misty");
	AK47Skin.AddItem("Point Disarray");
	AK47Skin.AddItem("Fuel Injector");
	AK47Skin.AddItem("Neon Revolution");
	AK47Skin.AddItem("The Empress");
	AK47Skin.AddItem("Bloodsport");
	Riflegroup.PlaceLabledControl("AK-47", this, &AK47Skin);

	M41SSkin.SetFileId("m4a1s_skin");
	M41SSkin.AddItem("Dark Water");
	M41SSkin.AddItem("Hyper Beast");
	M41SSkin.AddItem("Boreal Forest");
	M41SSkin.AddItem("VariCamo");
	M41SSkin.AddItem("Nitro");
	M41SSkin.AddItem("Bright Water");
	M41SSkin.AddItem("Atomic Alloy");
	M41SSkin.AddItem("Blood Tiger");
	M41SSkin.AddItem("Guardian");
	M41SSkin.AddItem("Master Piece");
	M41SSkin.AddItem("Knight");
	M41SSkin.AddItem("Cyrex");
	M41SSkin.AddItem("Basilisk");
	M41SSkin.AddItem("Icarus Fell");
	M41SSkin.AddItem("Hot Rod");
	M41SSkin.AddItem("Golden Coi");
	M41SSkin.AddItem("Chantico's Fire");
	M41SSkin.AddItem("Mecha Industries");
	M41SSkin.AddItem("Flashback");
	Riflegroup.PlaceLabledControl("M4A1-S", this, &M41SSkin);

	M4A4Skin.SetFileId("m4a4_skin");
	M4A4Skin.AddItem("Bullet Rain");
	M4A4Skin.AddItem("Zirka");
	M4A4Skin.AddItem("Asiimov");
	M4A4Skin.AddItem("Howl");
	M4A4Skin.AddItem("X-Ray");
	M4A4Skin.AddItem("Desert-Strike");
	M4A4Skin.AddItem("Griffin");
	M4A4Skin.AddItem("Dragon King");
	M4A4Skin.AddItem("Poseidon");
	M4A4Skin.AddItem("Daybreak");
	M4A4Skin.AddItem("Evil Daimyo");
	M4A4Skin.AddItem("Royal Paladin");
	M4A4Skin.AddItem("The BattleStar");
	M4A4Skin.AddItem("Desolate Space");
	M4A4Skin.AddItem("Buzz Kill");
	Riflegroup.PlaceLabledControl("M4A4", this, &M4A4Skin);

	AUGSkin.SetFileId("aug_skin");
	AUGSkin.AddItem("Bengal Tiger");
	AUGSkin.AddItem("Hot Rod");
	AUGSkin.AddItem("Chameleon");
	AUGSkin.AddItem("Torque");
	AUGSkin.AddItem("Radiation Hazard");
	AUGSkin.AddItem("Asterion");
	AUGSkin.AddItem("Daedalus");
	AUGSkin.AddItem("Akihabara Accept");
	AUGSkin.AddItem("Ricochet");
	AUGSkin.AddItem("Fleet Flock");
	AUGSkin.AddItem("Syd Mead");
	Riflegroup.PlaceLabledControl("AUG", this, &AUGSkin);

	FAMASSkin.SetFileId("famas_skin");
	FAMASSkin.AddItem("Contrast Spray");
	FAMASSkin.AddItem("Colony");
	FAMASSkin.AddItem("Cyanospatter");
	FAMASSkin.AddItem("Djinn");
	FAMASSkin.AddItem("Afterimage");
	FAMASSkin.AddItem("Doomkitty");
	FAMASSkin.AddItem("Spitfire");
	FAMASSkin.AddItem("Teardown");
	FAMASSkin.AddItem("Hexane");
	FAMASSkin.AddItem("Pulse");
	FAMASSkin.AddItem("Sergeant");
	FAMASSkin.AddItem("Styx");
	FAMASSkin.AddItem("Neural Net");
	FAMASSkin.AddItem("Survivor");
	FAMASSkin.AddItem("Valence");
	FAMASSkin.AddItem("Roll Cage");
	FAMASSkin.AddItem("Mecha Industries");
	Riflegroup.PlaceLabledControl("FAMAS", this, &FAMASSkin);

	GALILSkin.SetFileId("galil_skin");
	GALILSkin.AddItem("Forest DDPAT");
	GALILSkin.AddItem("Contrast Spray");
	GALILSkin.AddItem("Orange DDPAT");
	GALILSkin.AddItem("Eco");
	GALILSkin.AddItem("Winter Forest");
	GALILSkin.AddItem("Sage Spray");
	GALILSkin.AddItem("VariCamo");
	GALILSkin.AddItem("VariCamo");
	GALILSkin.AddItem("Chatterbox");
	GALILSkin.AddItem("Shattered");
	GALILSkin.AddItem("Kami");
	GALILSkin.AddItem("Blue Titanium");
	GALILSkin.AddItem("Urban Rubble");
	GALILSkin.AddItem("Hunting Blind");
	GALILSkin.AddItem("Sandstorm");
	GALILSkin.AddItem("Tuxedo");
	GALILSkin.AddItem("Cerberus");
	GALILSkin.AddItem("Aqua Terrace");
	GALILSkin.AddItem("Rocket Pop");
	GALILSkin.AddItem("Stone Cold");
	GALILSkin.AddItem("Firefight");
	Riflegroup.PlaceLabledControl("GALIL", this, &GALILSkin);

	SG553Skin.SetFileId("sg552_skin");
	SG553Skin.AddItem("Bulldozer");
	SG553Skin.AddItem("Ultraviolet");
	SG553Skin.AddItem("Damascus Steel");
	SG553Skin.AddItem("Fallout Warning");
	SG553Skin.AddItem("Damascus Steel");
	SG553Skin.AddItem("Pulse");
	SG553Skin.AddItem("Army Sheen");
	SG553Skin.AddItem("Traveler");
	SG553Skin.AddItem("Fallout Warning");
	SG553Skin.AddItem("Cyrex");
	SG553Skin.AddItem("Tiger Moth");
	SG553Skin.AddItem("Atlas");
	Riflegroup.PlaceLabledControl("SG552", this, &SG553Skin);
#pragma endregion

#pragma region MPs
	MPGroup.SetPosition(16, 443);
	MPGroup.SetText("MPs");
	MPGroup.SetSize(376, 173);
	RegisterControl(&MPGroup);

	MAC10Skin.SetFileId("mac10_skin");
	MAC10Skin.AddItem("Tornado");
	MAC10Skin.AddItem("Candy Apple");
	MAC10Skin.AddItem("Silver");
	MAC10Skin.AddItem("Forest DDPAT");
	MAC10Skin.AddItem("Urban DDPAT");
	MAC10Skin.AddItem("Fade");
	MAC10Skin.AddItem("Neon Rider");
	MAC10Skin.AddItem("Ultraviolet");
	MAC10Skin.AddItem("Palm");
	MAC10Skin.AddItem("Graven");
	MAC10Skin.AddItem("Tatter");
	MAC10Skin.AddItem("Amber Fade");
	MAC10Skin.AddItem("Heat");
	MAC10Skin.AddItem("Curse");
	MAC10Skin.AddItem("Indigo");
	MAC10Skin.AddItem("Commuter");
	MAC10Skin.AddItem("Nuclear Garden");
	MAC10Skin.AddItem("Malachite");
	MAC10Skin.AddItem("Rangeen");
	MAC10Skin.AddItem("Lapis Gator");
	MPGroup.PlaceLabledControl("MAC-10", this, &MAC10Skin);

	P90Skin.SetFileId("p90_skin");
	P90Skin.AddItem("Leather");
	P90Skin.AddItem("Virus");
	P90Skin.AddItem("Contrast Spray");
	P90Skin.AddItem("Storm");
	P90Skin.AddItem("Cold Blooded");
	P90Skin.AddItem("Glacier Mesh");
	P90Skin.AddItem("Sand Spray");
	P90Skin.AddItem("Death by Kitty");
	P90Skin.AddItem("Ash Wood");
	P90Skin.AddItem("Fallout Warning");
	P90Skin.AddItem("Scorched");
	P90Skin.AddItem("Emerald Dragon");
	P90Skin.AddItem("Teardown");
	P90Skin.AddItem("Blind Spot");
	P90Skin.AddItem("Trigon");
	P90Skin.AddItem("Desert Warfare");
	P90Skin.AddItem("Module");
	P90Skin.AddItem("Asiimov");
	P90Skin.AddItem("Elite Build");
	P90Skin.AddItem("Shapewood");
	P90Skin.AddItem("Shallow Grave");
	MPGroup.PlaceLabledControl("P90", this, &P90Skin);

	UMP45Skin.SetFileId("ump45_skin");
	UMP45Skin.AddItem("Blaze");
	UMP45Skin.AddItem("Forest DDPAT");
	UMP45Skin.AddItem("Gunsmoke");
	UMP45Skin.AddItem("Urban DDPAT");
	UMP45Skin.AddItem("Grand Prix");
	UMP45Skin.AddItem("Carbon Fiber");
	UMP45Skin.AddItem("Caramel");
	UMP45Skin.AddItem("Fallout Warning");
	UMP45Skin.AddItem("Scorched");
	UMP45Skin.AddItem("Bone Pile");
	UMP45Skin.AddItem("Delusion");
	UMP45Skin.AddItem("Corporal");
	UMP45Skin.AddItem("Indigo");
	UMP45Skin.AddItem("Labyrinth");
	UMP45Skin.AddItem("Minotaur's Labyrinth");
	UMP45Skin.AddItem("Riot");
	UMP45Skin.AddItem("Primal Saber");
	MPGroup.PlaceLabledControl("UMP-45", this, &UMP45Skin);

	BIZONSkin.SetFileId("bizon_skin");
	BIZONSkin.AddItem("Blue Streak");
	BIZONSkin.AddItem("Modern Hunter");
	BIZONSkin.AddItem("Forest Leaves");
	BIZONSkin.AddItem("Bone Mask");
	BIZONSkin.AddItem("Carbon Fiber");
	BIZONSkin.AddItem("Sand Dashed");
	BIZONSkin.AddItem("Urban Dashed");
	BIZONSkin.AddItem("Brass");
	BIZONSkin.AddItem("VariCamo");
	BIZONSkin.AddItem("Irradiated Alert");
	BIZONSkin.AddItem("Rust Coat");
	BIZONSkin.AddItem("Water Sigil");
	BIZONSkin.AddItem("Night Ops");
	BIZONSkin.AddItem("Cobalt Halftone");
	BIZONSkin.AddItem("Antique");
	BIZONSkin.AddItem("Rust Coat");
	BIZONSkin.AddItem("Osiris");
	BIZONSkin.AddItem("Chemical Green");
	BIZONSkin.AddItem("Bamboo Print");
	BIZONSkin.AddItem("Bamboo Forest");
	BIZONSkin.AddItem("Fuel Rod");
	BIZONSkin.AddItem("Photic Zone");
	BIZONSkin.AddItem("Judgement of Anubis");
	MPGroup.PlaceLabledControl("PP-Bizon", this, &BIZONSkin);

	MP7Skin.SetFileId("mp7_skin");
	MP7Skin.AddItem("Groundwater");
	MP7Skin.AddItem("Whiteout");
	MP7Skin.AddItem("Forest DDPAT");
	MP7Skin.AddItem("Anodized Navy");
	MP7Skin.AddItem("Skulls");
	MP7Skin.AddItem("Gunsmoke");
	MP7Skin.AddItem("Contrast Spray");
	MP7Skin.AddItem("Bone Mask");
	MP7Skin.AddItem("Ossified");
	MP7Skin.AddItem("Orange Peel");
	MP7Skin.AddItem("VariCamo");
	MP7Skin.AddItem("Army Recon");
	MP7Skin.AddItem("Groundwater");
	MP7Skin.AddItem("Ocean Foam");
	MP7Skin.AddItem("Full Stop");
	MP7Skin.AddItem("Urban Hazard");
	MP7Skin.AddItem("Olive Plaid");
	MP7Skin.AddItem("Armor Core");
	MP7Skin.AddItem("Asterion");
	MP7Skin.AddItem("Nemesis");
	MP7Skin.AddItem("Special Delivery");
	MP7Skin.AddItem("Impire");
	MPGroup.PlaceLabledControl("MP7", this, &MP7Skin);

	MP9Skin.SetFileId("mp9_skin");
	MP9Skin.AddItem("Ruby Poison Dart");
	MP9Skin.AddItem("Bone Mask");
	MP9Skin.AddItem("Hot Rod");
	MP9Skin.AddItem("Storm");
	MP9Skin.AddItem("Bulldozer");
	MP9Skin.AddItem("Hypnotic");
	MP9Skin.AddItem("Sand Dashed");
	MP9Skin.AddItem("Orange Peel");
	MP9Skin.AddItem("Dry Season");
	MP9Skin.AddItem("Dark Age");
	MP9Skin.AddItem("Rose Iron");
	MP9Skin.AddItem("Green Plaid");
	MP9Skin.AddItem("Setting Sun");
	MP9Skin.AddItem("Dart");
	MP9Skin.AddItem("Deadly Poison");
	MP9Skin.AddItem("Pandora's Box");
	MP9Skin.AddItem("Bioleak");
	MP9Skin.AddItem("Airlock");
	MPGroup.PlaceLabledControl("MP9", this, &MP9Skin);

#pragma endregion

#pragma region Pistols
	PistolGroup.SetPosition(408, 233);
	PistolGroup.SetText("Pistols");
	PistolGroup.SetSize(360, 250);
	RegisterControl(&PistolGroup);

	// Glock-18
	GLOCKSkin.SetFileId("glock_skin");
	GLOCKSkin.AddItem("Groundwater");
	GLOCKSkin.AddItem("Candy Apple");
	GLOCKSkin.AddItem("Fade");
	GLOCKSkin.AddItem("Night");
	GLOCKSkin.AddItem("Dragon Tattoo");
	GLOCKSkin.AddItem("Twilight Galaxy");
	GLOCKSkin.AddItem("Sand Dune");
	GLOCKSkin.AddItem("Brass");
	GLOCKSkin.AddItem("Catacombs");
	GLOCKSkin.AddItem("Sand Dune");
	GLOCKSkin.AddItem("Steel Disruption");
	GLOCKSkin.AddItem("Blue Fissure");
	GLOCKSkin.AddItem("Death Rattle");
	GLOCKSkin.AddItem("Water Elemental");
	GLOCKSkin.AddItem("Reactor");
	GLOCKSkin.AddItem("Grinder");
	GLOCKSkin.AddItem("Bunsen Burner");
	GLOCKSkin.AddItem("Wraith");
	GLOCKSkin.AddItem("Royal Legion");
	GLOCKSkin.AddItem("Weasel");
	GLOCKSkin.AddItem("Wasteland Rebel");
	PistolGroup.PlaceLabledControl("Glock", this, &GLOCKSkin);

	// USP-S
	USPSSkin.SetFileId("usps_skin");
	USPSSkin.AddItem("Forest Leaves");
	USPSSkin.AddItem("Dark Water");
	USPSSkin.AddItem("VariCamo");
	USPSSkin.AddItem("Overgrowth");
	USPSSkin.AddItem("Caiman");
	USPSSkin.AddItem("Blood Tiger");
	USPSSkin.AddItem("Serum");
	USPSSkin.AddItem("Night Ops");
	USPSSkin.AddItem("Stainless");
	USPSSkin.AddItem("Guardian");
	USPSSkin.AddItem("Orion");
	USPSSkin.AddItem("Road Rash");
	USPSSkin.AddItem("Royal Blue");
	USPSSkin.AddItem("Business Class");
	USPSSkin.AddItem("Para Green");
	USPSSkin.AddItem("Torque");
	USPSSkin.AddItem("Kill Confirmed");
	USPSSkin.AddItem("Lead Conduit");
	USPSSkin.AddItem("Cyrex");
	PistolGroup.PlaceLabledControl("USP-S", this, &USPSSkin);

	// Deagle
	DEAGLESkin.SetFileId("deagle_skin");
	DEAGLESkin.AddItem("Blaze");
	DEAGLESkin.AddItem("Pilot");
	DEAGLESkin.AddItem("Midnight Storm");
	DEAGLESkin.AddItem("Sunset Storm");
	DEAGLESkin.AddItem("Forest DDPAT");
	DEAGLESkin.AddItem("Crimson Web");
	DEAGLESkin.AddItem("Urban DDPAT");
	DEAGLESkin.AddItem("Night");
	DEAGLESkin.AddItem("Hypnotic");
	DEAGLESkin.AddItem("Mudder");
	DEAGLESkin.AddItem("VariCamo");
	DEAGLESkin.AddItem("Golden Koi");
	DEAGLESkin.AddItem("Cobalt Disruption");
	DEAGLESkin.AddItem("Urban Rubble");
	DEAGLESkin.AddItem("Naga");
	DEAGLESkin.AddItem("Hand Cannon");
	DEAGLESkin.AddItem("Heirloom");
	DEAGLESkin.AddItem("Meteorite");
	DEAGLESkin.AddItem("Conspiracy");
	DEAGLESkin.AddItem("Bronze Deco");
	DEAGLESkin.AddItem("Sunset Storm");
	DEAGLESkin.AddItem("Corinthian");
	DEAGLESkin.AddItem("Kumicho Dragon");
	PistolGroup.PlaceLabledControl("Deagle", this, &DEAGLESkin);

	// Dual Berettas
	DUALSSkin.SetFileId("duals_skin");
	DUALSSkin.AddItem("Anodized Navy");
	DUALSSkin.AddItem("Ossified");
	DUALSSkin.AddItem("Stained");
	DUALSSkin.AddItem("Contractor");
	DUALSSkin.AddItem("Colony");
	DUALSSkin.AddItem("Demolition");
	DUALSSkin.AddItem("Dualing Dragons");
	DUALSSkin.AddItem("Black Limba");
	DUALSSkin.AddItem("Red Quartz");
	DUALSSkin.AddItem("Cobalt Quartz");
	DUALSSkin.AddItem("Hemoglobin");
	DUALSSkin.AddItem("Urban Shock");
	DUALSSkin.AddItem("Marina");
	DUALSSkin.AddItem("Panther");
	DUALSSkin.AddItem("Retribution");
	DUALSSkin.AddItem("Briar");
	DUALSSkin.AddItem("Duelist");
	DUALSSkin.AddItem("Moon in Libra");
	DUALSSkin.AddItem("Cartel");
	DUALSSkin.AddItem("Ventilators");
	DUALSSkin.AddItem("Royal Consorts");
	DUALSSkin.AddItem("Cobra Strike");
	PistolGroup.PlaceLabledControl("Duals", this, &DUALSSkin);

	// Five Seven
	FIVESEVENSkin.SetFileId("fiveseven_skin");
	FIVESEVENSkin.AddItem("Candy Apple");
	FIVESEVENSkin.AddItem("Bone Mask");
	FIVESEVENSkin.AddItem("Case Hardened");
	FIVESEVENSkin.AddItem("Contractor");
	FIVESEVENSkin.AddItem("Forest Night");
	FIVESEVENSkin.AddItem("Orange Peel");
	FIVESEVENSkin.AddItem("Jungle");
	FIVESEVENSkin.AddItem("Nitro");
	FIVESEVENSkin.AddItem("Red Quartz");
	FIVESEVENSkin.AddItem("Anodized Gunmetal");
	FIVESEVENSkin.AddItem("Nightshade");
	FIVESEVENSkin.AddItem("Silver Quartz");
	FIVESEVENSkin.AddItem("Kami");
	FIVESEVENSkin.AddItem("Copper Galaxy");
	FIVESEVENSkin.AddItem("Neon Kimono");
	FIVESEVENSkin.AddItem("Fowl Play");
	FIVESEVENSkin.AddItem("Hot Shot");
	FIVESEVENSkin.AddItem("Urban Hazard");
	FIVESEVENSkin.AddItem("Monkey Business");
	FIVESEVENSkin.AddItem("Retrobution");
	FIVESEVENSkin.AddItem("Triumvirate");
	FIVESEVENSkin.AddItem("Capillary");
	FIVESEVENSkin.AddItem("Hyper Beast");
	PistolGroup.PlaceLabledControl("Five-Seven", this, &FIVESEVENSkin);

	TECNINESkin.SetFileId("tec9_skin");
	TECNINESkin.AddItem("Tornado");
	TECNINESkin.AddItem("Groundwater");
	TECNINESkin.AddItem("Forest DDPAT");
	TECNINESkin.AddItem("Terrace");
	TECNINESkin.AddItem("Urban DDPAT");
	TECNINESkin.AddItem("Ossified");
	TECNINESkin.AddItem("Hades");
	TECNINESkin.AddItem("Brass");
	TECNINESkin.AddItem("VariCamo");
	TECNINESkin.AddItem("Nuclear Threat");
	TECNINESkin.AddItem("Red Quartz");
	TECNINESkin.AddItem("Tornado");
	TECNINESkin.AddItem("Blue Titanium");
	TECNINESkin.AddItem("Army Mesh");
	TECNINESkin.AddItem("Titanium Bit");
	TECNINESkin.AddItem("Sandstorm");
	TECNINESkin.AddItem("Isaac");
	TECNINESkin.AddItem("Toxic");
	TECNINESkin.AddItem("Bamboo Forest");
	TECNINESkin.AddItem("Avalanche");
	TECNINESkin.AddItem("Jambiya");
	TECNINESkin.AddItem("Re-Entry");
	TECNINESkin.AddItem("Fuel Injector");
	PistolGroup.PlaceLabledControl("Tec-9", this, &TECNINESkin);

	P2000Skin.SetFileId("p2000_skin");
	P2000Skin.AddItem("Grassland Leaves");
	P2000Skin.AddItem("Silver");
	P2000Skin.AddItem("Granite Marbleized");
	P2000Skin.AddItem("Forest Leaves");
	P2000Skin.AddItem("Ossified");
	P2000Skin.AddItem("Handgun");
	P2000Skin.AddItem("Fade");
	P2000Skin.AddItem("Scorpion");
	P2000Skin.AddItem("Grassland");
	P2000Skin.AddItem("Corticera");
	P2000Skin.AddItem("Ocean Foam");
	P2000Skin.AddItem("Pulse");
	P2000Skin.AddItem("Amber Fade");
	P2000Skin.AddItem("Red FragCam");
	P2000Skin.AddItem("Chainmail");
	P2000Skin.AddItem("Coach Class");
	P2000Skin.AddItem("Ivory");
	P2000Skin.AddItem("Fire Elemental");
	P2000Skin.AddItem("Asterion");
	P2000Skin.AddItem("Pathfinder");
	P2000Skin.AddItem("Imperial");
	P2000Skin.AddItem("Oceanic");
	P2000Skin.AddItem("Imperial Dragon");
	PistolGroup.PlaceLabledControl("P2000", this, &P2000Skin);

	P250Skin.SetFileId("p250_skin");
	P250Skin.AddItem("Whiteout");
	P250Skin.AddItem("Metallic DDPAT");
	P250Skin.AddItem("Splash");
	P250Skin.AddItem("Gunsmoke");
	P250Skin.AddItem("Modern Hunter");
	P250Skin.AddItem("Bone Mask");
	P250Skin.AddItem("Boreal Forest");
	P250Skin.AddItem("Sand Dune");
	P250Skin.AddItem("Nuclear Threat");
	P250Skin.AddItem("Mehndi");
	P250Skin.AddItem("Facets");
	P250Skin.AddItem("Hive");
	P250Skin.AddItem("Muertos");
	P250Skin.AddItem("Steel Disruption");
	P250Skin.AddItem("Undertow");
	P250Skin.AddItem("Franklin");
	P250Skin.AddItem("Neon Kimono");
	P250Skin.AddItem("Supernova");
	P250Skin.AddItem("Contamination");
	P250Skin.AddItem("Cartel");
	P250Skin.AddItem("Valence");
	P250Skin.AddItem("Crimson Kimono");
	P250Skin.AddItem("Mint Kimono");
	P250Skin.AddItem("Wing Shot");
	P250Skin.AddItem("Asiimov");
	PistolGroup.PlaceLabledControl("P250", this, &P250Skin);

#pragma endregion

#pragma region GloveChanger

	GloveGroup.SetPosition(408, 496);
	GloveGroup.SetText("Gloves");
	GloveGroup.SetSize(360, 118);
	RegisterControl(&GloveGroup);

	GloveModel.SetFileId("glove_model");
	GloveModel.AddItem("Bloodhound");
	GloveModel.AddItem("Sporty");
	GloveModel.AddItem("Slick");
	GloveModel.AddItem("Specialist");
	GloveModel.AddItem("Moto");
	GloveModel.AddItem("Handwrap");
	GloveGroup.PlaceLabledControl("Glove Model", this, &GloveModel);

	GloveSkin.SetFileId("glove_skin");
	GloveSkin.AddItem("Bloodhound Black"); //0
	GloveSkin.AddItem("Bloodhound Snakeskin"); //1
	GloveSkin.AddItem("Bloodhound Metallic"); //2
	GloveSkin.AddItem("Sporty Purple"); //3
	GloveSkin.AddItem("Sporty Green"); //4
	GloveSkin.AddItem("Sporty Light Blue"); //5
	GloveSkin.AddItem("Sporty Military"); //6
	GloveSkin.AddItem("Slick Black"); //7
	GloveSkin.AddItem("Slick Military"); //8
	GloveSkin.AddItem("Slick Red"); //9
	GloveSkin.AddItem("Specialist DDPAT"); //10
	GloveSkin.AddItem("Specialist Kimono"); //11
	GloveSkin.AddItem("Specialist Emerald"); //12
	GloveSkin.AddItem("Specialist Orange"); //13
	GloveSkin.AddItem("Moto Mint"); //14
	GloveSkin.AddItem("Moto Boom"); //15
	GloveSkin.AddItem("Moto Blue"); //16
	GloveSkin.AddItem("Moto Black"); //17
	GloveSkin.AddItem("Moto Black"); //17
	GloveSkin.AddItem("Handwrap Grey Camo"); //18

	GloveSkin.AddItem("Handwrap Slaughter"); //19
	GloveSkin.AddItem("Handwrap Orange Camo"); //20
	GloveSkin.AddItem("Handwrap Leathery"); //21
	GloveGroup.PlaceLabledControl("Glove Skin", this, &GloveSkin);

	SkinApply.SetText("Apply Changes");
	SkinApply.SetCallback(KnifeApplyCallbk);
	SkinApply.SetPosition(16, 625);
	SkinApply.SetSize(755, 108);
	RegisterControl(&SkinApply);

#pragma endregion 
}
void Menu::SetupMenu()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
		Window.VisualsTab.FiltersEnemiesOnly.SetState(true);
	}

	GUI.Update();
	GUI.Draw();


}

#pragma region Visual Colors
void CColorTab::Setup()
{
	SetTitle("H");

	ColorsGroup.SetPosition(16, 15);
	ColorsGroup.SetText("Colors");
	ColorsGroup.SetSize(400, 600);
	RegisterControl(&ColorsGroup);

	CTColorVisR.SetFileId("ct_color_vis_r");
	CTColorVisR.SetBoundaries(0.f, 255.f);
	CTColorVisR.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("CT visible red", this, &CTColorVisR);

	CTColorVisG.SetFileId("ct_color_vis_g");
	CTColorVisG.SetBoundaries(0.f, 255.f);
	CTColorVisG.SetValue(230.f);
	ColorsGroup.PlaceLabledControl("CT visible green", this, &CTColorVisG);

	CTColorVisB.SetFileId("ct_color_vis_b");
	CTColorVisB.SetBoundaries(0.f, 255.f);
	CTColorVisB.SetValue(15.f);
	ColorsGroup.PlaceLabledControl("CT visible blue", this, &CTColorVisB);

	CTColorNoVisR.SetFileId("ct_color_no_vis_r");
	CTColorNoVisR.SetBoundaries(0.f, 255.f);
	CTColorNoVisR.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("CT not visible red", this, &CTColorNoVisR);

	CTColorNoVisG.SetFileId("ct_color_no_vis_g");
	CTColorNoVisG.SetBoundaries(0.f, 255.f);
	CTColorNoVisG.SetValue(235.f);
	ColorsGroup.PlaceLabledControl("CT Not Visible Green", this, &CTColorNoVisG);

	CTColorNoVisB.SetFileId("ct_color_no_vis_b");
	CTColorNoVisB.SetBoundaries(0.f, 255.f);
	CTColorNoVisB.SetValue(10.f);
	ColorsGroup.PlaceLabledControl("CT Not Visible Blue", this, &CTColorNoVisB);

	TColorVisR.SetFileId("t_color_vis_r");
	TColorVisR.SetBoundaries(0.f, 255.f);
	TColorVisR.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("T Visible Red", this, &TColorVisR);

	TColorVisG.SetFileId("t_color_vis_g");
	TColorVisG.SetBoundaries(0.f, 255.f);
	TColorVisG.SetValue(50.f);
	ColorsGroup.PlaceLabledControl("T Visible Green", this, &TColorVisG);

	TColorVisB.SetFileId("t_color_vis_b");
	TColorVisB.SetBoundaries(0.f, 255.f);
	TColorVisB.SetValue(220.f);
	ColorsGroup.PlaceLabledControl("T Visible Blue", this, &TColorVisB);

	TColorNoVisR.SetFileId("t_color_no_vis_r");
	TColorNoVisR.SetBoundaries(0.f, 255.f);
	TColorNoVisR.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("T Not Visible Red", this, &TColorNoVisR);

	TColorNoVisG.SetFileId("t_color_no_vis_g");
	TColorNoVisG.SetBoundaries(0.f, 255.f);
	TColorNoVisG.SetValue(50.f);
	ColorsGroup.PlaceLabledControl("T Not Visible Green", this, &TColorNoVisG);

	TColorNoVisB.SetFileId("t_color_no_vis_b");
	TColorNoVisB.SetBoundaries(0.f, 255.f);
	TColorNoVisB.SetValue(220.f);
	ColorsGroup.PlaceLabledControl("T Not Visible Blue", this, &TColorNoVisB);

	GlowR.SetFileId("GlowR");
	GlowR.SetBoundaries(0.f, 255.f);
	GlowR.SetValue(230.f);
	ColorsGroup.PlaceLabledControl("Glow Red", this, &GlowR);

	GlowG.SetFileId("GlowG");
	GlowG.SetBoundaries(0.f, 255.f);
	GlowG.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("Glow Green", this, &GlowG);

	GlowB.SetFileId("GlowB");
	GlowB.SetBoundaries(0.f, 255.f);
	GlowB.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("Glow Blue", this, &GlowB);

	CTChamsR.SetFileId("ctchamsr");
	CTChamsR.SetBoundaries(0.f, 255.f);
	CTChamsR.SetValue(40.f);
	ColorsGroup.PlaceLabledControl("CT Chams Red", this, &CTChamsR);

	CTChamsG.SetFileId("ctchamsg");
	CTChamsG.SetBoundaries(0.f, 255.f);
	CTChamsG.SetValue(120.f);
	ColorsGroup.PlaceLabledControl("CT Chams Green", this, &CTChamsG);

	CTChamsB.SetFileId("ctchamsb");
	CTChamsB.SetBoundaries(0.f, 255.f);
	CTChamsB.SetValue(170.f);
	ColorsGroup.PlaceLabledControl("CT Chams Blue", this, &CTChamsB);

	TChamsR.SetFileId("tchamsr");
	TChamsR.SetBoundaries(0.f, 255.f);
	TChamsR.SetValue(210.f);
	ColorsGroup.PlaceLabledControl("T Chams Red", this, &TChamsR);

	TChamsG.SetFileId("tchamsg");
	TChamsG.SetBoundaries(0.f, 255.f);
	TChamsG.SetValue(20.f);
	ColorsGroup.PlaceLabledControl("T Chams Green", this, &TChamsG);

	TChamsB.SetFileId("tchamsb");
	TChamsB.SetBoundaries(0.f, 255.f);
	TChamsB.SetValue(70.f);
	ColorsGroup.PlaceLabledControl("T Chams Blue", this, &TChamsB);

};
#pragma endregion
