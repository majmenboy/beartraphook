#pragma once

#include "Hacks.h"

class CLegitBot : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool& bSendPacket);
private:
	// Targetting
	int GetTargetCrosshair();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	bool TargetMeetsTriggerRequirements(IClientEntity* pEntity);
	float FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int HitBox);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket);

	void SyncWeaponSettings();

	// Functionality
	void DoAimbot(CUserCmd *pCmd, bool &bSendpacket);
	void DoTrigger(CUserCmd *pCmd);
	void LegitAA(CUserCmd *pCmd, bool& bSendPacket);

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
	bool Multihitbox;
	float StartAim;
	float Aimtime;
	int besthitbox;

	float Speed;
	float FoV;
	float RecoilControl;
	bool PSilent;
	float Inacc;

	bool shoot;

};