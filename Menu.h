/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "GUI.h"
#include "Controls.h"

class CRageBotTab : public CTab
{
public:
	void Setup();

	CLabel ActiveLabel;
	CCheckBox Active;

	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CSlider  AimbotFov;
	CComboBox AimbotSilentAim;
	CCheckBox PVSFix;
	CComboBox AimbotResolver;
	CCheckBox AdvancedResolver;
	CCheckBox AutoRevolver;
	CSlider AddFakeYaw;
	CSlider CustomPitch;
	CCheckBox LBY;
	CComboBox FakeYaw;
	CCheckBox EnginePrediction;
	CCheckBox LowerbodyFix;
	CCheckBox AWPAtBody;
	CCheckBox AimbotAimStep;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CKeyBind  AimbotStopKey;
	CKeyBind OtherSlowMotion;
	CKeyBind FakeWalk;

	CGroupBox TargetGroup;
	CComboBox TargetSelection;
	CCheckBox TargetFriendlyFire;
	CComboBox TargetHitbox;
	CComboBox TargetHitscan;
	CCheckBox TargetMultipoint;
	CSlider   TargetPointscale;
	CSlider AccuracySmart;

	CGroupBox AccuracyGroup;
	CCheckBox AccuracyRecoil;
	CCheckBox AccuracyAutoWall;
	CSlider AccuracyMinimumDamage;
	CSlider AccuracyMinimumDamageAWall;
	CCheckBox AccuracyAutoStop;
	CCheckBox AccuracyAutoCrouch;
	CCheckBox AccuracyAutoScope;
	CSlider   AccuracyHitchance;
	CCheckBox ResolverDebug;
	CComboBox AccuracyResolver;
	CCheckBox AccuracyPositionAdjustment;
	CCheckBox Accuracydisableinterp;
	CCheckBox Accuracylagcomp;
	CCheckBox PreferBodyAim;
	CCheckBox AutoHitChance;
	CCheckBox AutoMinimumDamage;
	CCheckBox AccuracyBacktracking;
	CSlider BaimIfUnderXHealth;
	CSlider XShots;

	CGroupBox AntiAimGroup;
	CCheckBox AntiAimEnable;
	CComboBox AntiAimPitch;
	CComboBox AntiAimYaw;
	CCheckBox FlipAA;
	CKeyBind  FlipKey;
	CCheckBox AntiAimEdge;
	CSlider AntiAimOffset;
	CCheckBox AntiAimKnife;
	CSlider	  SpinSpeed;
	CCheckBox AccuracyResolver2;
	CKeyBind AccuracyAntifakewalk;
	CCheckBox AntiAimTarget;
};

class CLegitBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	CGroupBox AimbotGroup;

	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CCheckBox AimbotFriendlyFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CCheckBox AimbotAutoPistol;
	CCheckBox AimbotSmokeCheck;
	CSlider   TickModulation;
	CCheckBox AimbotBacktrack;

	CGroupBox TriggerGroup;
	CCheckBox TriggerEnable;
	CCheckBox TriggerKeyPress;
	CKeyBind  TriggerKeyBind;
	CCheckBox TriggerHitChance;
	CSlider   TriggerHitChanceAmmount;
	CSlider   TriggerDelay;
	CSlider   TriggerBurst;
	CSlider   TriggerBreak;
	CCheckBox TriggerRecoil;

	CGroupBox TriggerFilterGroup;
	CCheckBox TriggerHead;
	CCheckBox TriggerChest;
	CCheckBox TriggerStomach;
	CCheckBox TriggerArms;
	CCheckBox TriggerLegs;
	CCheckBox TriggerTeammates;
	CCheckBox TriggerFlashCheck;
	CCheckBox TriggerSmokeCheck;

	CGroupBox WeaponMainGroup;
	CSlider   WeaponMainSpeed;
	CSlider   WeaponMainFoV;
	CCheckBox WeaponMainRecoil;
	CComboBox WeaponMainHitbox;
	CSlider WeaponMainAimtime;
	CSlider WeaoponMainStartAimtime;
	CCheckBox WeaponMainPsilent;
	CSlider   WeaponMainInacc;

	CGroupBox WeaponPistGroup;
	CSlider   WeaponPistSpeed;
	CSlider   WeaponPistFoV;
	CCheckBox WeaponPistRecoil;
	CComboBox WeaponPistHitbox;
	CSlider WeaponPistAimtime;
	CSlider WeaoponPistStartAimtime;
	CCheckBox WeaponPistPSilent;
	CSlider   WeaponPistInacc;

	CGroupBox WeaponSnipGroup;
	CSlider   WeaponSnipSpeed;
	CSlider   WeaponSnipFoV;
	CCheckBox WeaponSnipRecoil;
	CComboBox WeaponSnipHitbox;
	CSlider WeaponSnipAimtime;
	CSlider WeaoponSnipStartAimtime;
	CCheckBox WeaponSnipPSilent;
	CSlider   WeaponSnipInacc;

	CGroupBox WeaponMpGroup;
	CSlider   WeaponMpSpeed;
	CSlider   WeaponMpFoV;
	CCheckBox WeaponMpRecoil;
	CComboBox WeaponMpHitbox;
	CSlider WeaponMpAimtime;
	CSlider WeaoponMpStartAimtime;
	CCheckBox WeaponMpPsilent;
	CSlider   WeaponMpInacc;

	CGroupBox WeaponShotgunGroup;
	CSlider   WeaponShotgunSpeed;
	CSlider   WeaponShotgunFoV;
	CCheckBox WeaponShotgunRecoil;
	CComboBox WeaponShotgunHitbox;
	CSlider WeaponShotgunAimtime;
	CSlider WeaoponShotgunStartAimtime;
	CCheckBox WeaponShotgunPsilent;
	CSlider   WeaponShotgunInacc;

	CGroupBox WeaponMGGroup;
	CSlider   WeaponMGSpeed;
	CSlider   WeaponMGFoV;
	CCheckBox WeaponMGRecoil;
	CComboBox WeaponMGHitbox;
	CSlider WeaponMGAimtime;
	CSlider WeaoponMGStartAimtime;
	CCheckBox WeaponMGPsilent;
	CSlider   WeaponMGInacc;

	CGroupBox Other;
	CCheckBox LegitAA;
};
class CVisualTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Options Settings
	CGroupBox OptionsGroup;
	CCheckBox OtherPenetration;
	CComboBox OptionsBox;
	CCheckBox OptionsName;
	CCheckBox OptionsHealth;
	CCheckBox OptionsWeapon;
	CCheckBox OptionsArmor;
	CCheckBox OptionsInfo;
	CCheckBox OptionsMoney;
	CCheckBox Seethrough;
	//	CCheckBox ChamsVisibleOnly; avoz <--- lol
	CCheckBox HitmarkerSound;
	CCheckBox OptionsHelmet;
	CCheckBox WeaponIcon;
	CCheckBox OptionsBarrels;
	CCheckBox OptionsKit;
	CCheckBox OtherSpreadCrosshair;
	CCheckBox IsScoped;
	CCheckBox ResolverInfo;
	CCheckBox OptionsDefuse;
	CCheckBox Logs;
	CCheckBox OptionsGlow;
	CComboBox OptionsChams;
	CComboBox OptionsSkeleton;
	CCheckBox OptionsAimSpot;
	CCheckBox OptionsCompRank;
	CCheckBox lbyidicador;
	CCheckBox BacktrackingLol;
	CCheckBox GrenadeTrace;
	CCheckBox OtherPlayerLine;
	CCheckBox ShowImpacts;

	// Filters Settings
	CGroupBox FiltersGroup;
	CCheckBox FiltersAll;
	CCheckBox FiltersPlayers;
	CCheckBox FiltersEnemiesOnly;
	CCheckBox FiltersWeapons;
	CCheckBox Filtersselfesp;
	CCheckBox FiltersChickens;
	CCheckBox FiltersC4;
	CCheckBox FiltersNightMod;

	// Other Settings
	CCheckBox SniperCrosshair;
	CGroupBox OtherGroup;
	CCheckBox Clock;
	CCheckBox Watermark;
	CComboBox OtherCrosshair;
	CCheckBox OtherHitmarker;
	CCheckBox OtherRadar;
	CCheckBox NightSky;
	CCheckBox OtherNoVisualRecoil;
	CCheckBox Grenades;
	CCheckBox OtherNoSky;
	CCheckBox OtherNoFlash;
	CCheckBox DisablePostProcess;
	CSlider AmbientExposure;
	CSlider AmbientRed;
	CSlider AmbientGreen;
	CSlider AmbientBlue;
	CSlider NightMod;
	CCheckBox OtherNightMode;
	CComboBox SkyboxChanger;
	CCheckBox OtherNoScope;
	CCheckBox OtherGrenadeTrajectory;
	CCheckBox OtherNoSmoke;
	CCheckBox OtherAsusWalls;
	CComboBox OtherNoHands;
	CSlider OtherViewmodelFOV;
	CSlider OtherFOV;
};

class CMiscTab : public CTab
{
public:
	void Setup();

	CLabel ActiveLabel;
	CCheckBox Active;

	//settings
	CComboBox ConfigBox;
	CGroupBox ConfigGroup;
	CGroupBox Settings;
	CButton SettingsSave;
	CButton SettingsLoad;

	// Other Settings
	CGroupBox OtherGroup;
	CCheckBox TrashTalk;
	CCheckBox OtherAutoJump;
	CCheckBox OtherEdgeJump;
	CComboBox OtherAutoStrafe;
	CCheckBox OtherSafeMode;
	CComboBox OtherChatSpam;
	CCheckBox OtherTeamChat;
	CSlider	  OtherChatDelay;
	CCheckBox EnableBuyBot;
	CComboBox BuyBot;
	CGroupBox BuyBotGroup;
	CComboBox BuyBotGrenades;
	CCheckBox BuyBotKevlar;
	CCheckBox BuyBotDefuser;
	CKeyBind  OtherAirStuck;
	CKeyBind  OtherLagSwitch;
	CCheckBox OtherSpectators;
	CCheckBox OtherThirdperson;
	CComboBox OtherThirdpersonType;
	CKeyBind OtherThirdpersonKey;
	CCheckBox OtherAutoAccept;
	//CCheckBox OtherAutoAccept;
	CCheckBox OtherWalkbot;
	CComboBox OtherClantag;

	// Fake Lag Settings
	CGroupBox FakeLagGroup;
	CComboBox FakeLagTyp;
	CCheckBox FakeLagEnable;
	CSlider   FakeLagChoke;
	//CCheckBox FakeLagWhileShooting;
};


class CSkinchangerTab : public CTab
{
public:
	void Setup();

	CLabel SkinActive;
	CCheckBox SkinEnable;
	CButton   SkinApply;

	CGroupBox KnifeGroup;
	CComboBox KnifeModel;
	CComboBox KnifeSkin;
	CCheckBox GG;

	CGroupBox PistolGroup;
	CComboBox GLOCKSkin;
	CComboBox USPSSkin;
	CComboBox DEAGLESkin;
	CComboBox MAGNUMSkin;
	CComboBox DUALSSkin;
	CComboBox RevolverSkin;
	CComboBox FIVESEVENSkin;
	CComboBox TECNINESkin;
	CComboBox P2000Skin;
	CComboBox P250Skin;

	CGroupBox MPGroup;
	CComboBox MAC10Skin;
	CComboBox P90Skin;
	CComboBox UMP45Skin;
	CComboBox BIZONSkin;
	CComboBox MP7Skin;
	CComboBox MP9Skin;

	CGroupBox Riflegroup;
	CComboBox M41SSkin;
	CComboBox M4A4Skin;
	CComboBox AK47Skin;
	CComboBox AUGSkin;
	CComboBox FAMASSkin;
	CComboBox GALILSkin;
	CComboBox SG553Skin;

	CGroupBox MachinegunsGroup;
	CComboBox NEGEVSkin;
	CComboBox M249Skin;

	CGroupBox Snipergroup;
	CComboBox SCAR20Skin;
	CComboBox G3SG1Skin;
	CComboBox SSG08Skin;
	CComboBox AWPSkin;

	CGroupBox Shotgungroup;
	CComboBox MAG7Skin;
	CComboBox XM1014Skin;
	CComboBox SAWEDOFFSkin;
	CComboBox NOVASkin;

	CGroupBox GloveGroup;
	CComboBox GloveModel;
	CComboBox GloveSkin;
	CCheckBox GloveActive;
	CButton   GloveApply;
	CCheckBox StatTrakEnable;
	CComboBox StatTrackAmount;
	CComboBox SkinName;
	CComboBox KnifeName;
};

class CColorTab : public CTab
{
public:
	void Setup();

	CGroupBox RGBGroup;
	CSlider2   MenuR;
	CSlider2   MenuG;
	CSlider2   MenuB;


	CGroupBox ColorsGroup;
	CSlider2 CTColorVisR;
	CSlider2 CTColorVisG;
	CSlider2 CTColorVisB;

	CSlider2 TChams;
	CSlider2 CTChams;
	CSlider2 CTChamsR;
	CSlider2 CTChamsG;
	CSlider2 CTChamsB;
	CSlider2 TChamsR;
	CSlider2 TChamsB;
	CSlider2 TChamsG;

	CGroupBox Gloves;
	CCheckBox EnableGloves;
	CComboBox GloveSkin;
	CComboBox GloveModel;
	CButton GlovesApply;

	CSlider2 CTColorNoVisR;
	CSlider2 CTColorNoVisG;
	CSlider2 CTColorNoVisB;

	CLabel ColorSpacer;

	CSlider2 TColorVisR;
	CSlider2 TColorVisG;
	CSlider2 TColorVisB;

	CSlider2 TColorNoVisR;
	CSlider2 TColorNoVisG;
	CSlider2 TColorNoVisB;

	CSlider2 GlowR;
	CSlider2 GlowG;
	CSlider2 GlowB;

	CComboBox ConfigBox;
	CGroupBox ConfigGroup;
	CGroupBox Settings;
	CButton SettingsSave;
	CButton SettingsLoad;
	CComboBox ControlColor;
};


class AyyWareWindow : public CWindow
{
public:
	void Setup();

	CRageBotTab RageBotTab;
	CLegitBotTab LegitBotTab;
	CVisualTab VisualsTab;
	CSkinchangerTab SkinchangerTab;
	CMiscTab MiscTab;
	CColorTab ColorsTab;

	CButton SaveButton;
	CButton LoadButton;
	CComboBox ConfigBox;
	CButton UnloadButton;
};

namespace Menu
{
	void SetupMenu();
	void DoUIFrame();

	extern AyyWareWindow Window;
};