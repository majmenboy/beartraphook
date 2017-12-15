#pragma once

#include "Hacks.h"


class CVisuals : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	void DrawCrosshair();
	void NoScopeCrosshair();
	void DrawRecoilCrosshair();
	void bighak();
	void DefaultCrosshair();
//	void SpreadCrosshair();
	void DrawDamageIndicator();
	void DrawFOVCrosshair();
	void SpreadCrosshair();
};

extern float lineRealAngle;
extern float lineFakeAngle;