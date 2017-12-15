/*
Syn's AyyWare Framework 2015
*/

// Credits to Valve and Shad0w
#include "Interfaces.h"
#include "Menu.h"
RecvVarProxyFn oSmokeEffectTickBegin = NULL;
RecvVarProxyFn oFlashMaxAlpha = NULL;
RecvVarProxyFn oDidSmokeEffect = NULL;

// Shad0ws Yaw fix
// (FIX ME UP LATER)
void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;
	static bool last[128];
	bool tmp = last[((IClientEntity*)(pStruct))->GetIndex()];
	float yaw = pData->m_Value.m_Float;
	yaw -= 0.087929;

	switch (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 5)
	{
	case 0:
		// No resolver needed
		break;
	case 1:
		// Normal Fix
		yaw -= 0.087929;

		last[((IClientEntity*)(pStruct))->GetIndex()] = (yaw >= 180 && yaw <= 360);

		if (tmp && (yaw >= 0 && yaw <= 180))
			yaw += 359;

		yaw -= (int)(yaw / 360) * 360;
		if (yaw < -180)
			yaw += 360;
		else if (yaw > 180)
			yaw -= 360;

		break;
	case 2:
		// Anglefix
		bHasAA = ((*flPitch == 90.0f) || (*flPitch == 270.0f));
		bSpinbot = false;

		if (!bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
			&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f) && !bHasAA)
		{
			flYaw = vLast[((IClientEntity*)(pStruct))->GetIndex()].y;
			bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
		}
		else
		{
			if (bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
				&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f))
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
				bSpinbot = true;
			}
			else
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = false;
			}
		}

		vLast[((IClientEntity*)(pStruct))->GetIndex()].y = flYaw;


		bool bTmp = bJitterFix[((IClientEntity*)(pStruct))->GetIndex()];

		bJitterFix[((IClientEntity*)(pStruct))->GetIndex()] = (flYaw >= 180.0f && flYaw <= 360.0f);

		if (bTmp && (flYaw >= 0.0f && flYaw <= 180.0f))
		{
			flYaw += 359.0f;
		}
		break;
	}

	*(float*)(pOut) = flYaw;
}

// Simple fix for some Fake-Downs
void FixX(const CRecvProxyData* pData, void* pStruct, void* pOut) // Clamp other player angles to fix fakedown or lisp
{
	float* ang = (float*)pOut;
	*ang = pData->m_Value.m_Float;
	DWORD hex = *(DWORD*)(&ang);

	switch (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 5)
	{
	case 0:
		// No resolver needed
		break;
	case 1:
		// Regular Pitch Resolver
		if (hex >= 0x43330000)
		{
			*ang -= 360.f;
		}
		else if (hex <= -0x43330000)
		{
			*ang += 360.f;
		}

		if (hex == 0x42b40000)
		{
			*ang = 90.f;
		}
		else if (hex == -0x42b40000)
		{
			*ang = -90.f;
		}

		*(float*)(pOut) = *ang;
		break;
	case 2:
		if (pData->m_Value.m_Float > 180.0f)
			*ang -= 360.0f;
		else if (pData->m_Value.m_Float < -180.0f)
			*ang += 360.0f; 
		break;
	}
}

RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	int default_t = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
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

	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Menu::Window.SkinchangerTab.SkinEnable.GetState() && pLocal)
	{

		if (pLocal->IsAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == iBowie))
		{

			if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 0)
				pData->m_Value.m_Int = iBayonet;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 1)
				pData->m_Value.m_Int = iBowie;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 2)
				pData->m_Value.m_Int = iButterfly;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 3)
				pData->m_Value.m_Int = iFalchion;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 4)
				pData->m_Value.m_Int = iFlip;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 5)
				pData->m_Value.m_Int = iGut;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 6)
				pData->m_Value.m_Int = iHuntsman;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 7)
				pData->m_Value.m_Int = iKarambit;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 8)
				pData->m_Value.m_Int = iM9Bayonet;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 9)
				pData->m_Value.m_Int = iDagger;
		}
	}


	oRecvnModelIndex(pData, pStruct, pOut);
}

void RecvProxy_DidSmokeEffect(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	int *value = (int*)&pData->m_Value.m_Int;

	if (Menu::Window.VisualsTab.OtherNoSmoke.GetState())
	{
		//IClientEntity *entity = (IClientEntity*)pStruct;
		//*value = 0;
	}

	oDidSmokeEffect(pData, pStruct, pOut);
}

void RecvProxy_SmokeEffectTickBegin(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	float *value = &pData->m_Value.m_Float;

	if (Menu::Window.VisualsTab.OtherNoSmoke.GetState())
	{
		IClientEntity *entity = (IClientEntity*)pStruct;

		if (entity)
		{
			//*value = Interfaces::Globals->tickcount - 60000;
			*entity->GetOriginPtr() = Vector(10000, 10000, 10000);
		}
	}

	oSmokeEffectTickBegin(pData, pStruct, pOut);
}

void RecvProxy_FlashMaxAlpha(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	float *value = &pData->m_Value.m_Float;

	if (Menu::Window.VisualsTab.OtherNoFlash.GetState())
	{
		*value = 0.0f;
	}
	else
	{
		*value = 255.0f;
	}

	oFlashMaxAlpha(pData, pStruct, pOut);
}

void ApplyNetVarsHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();

	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

		for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
		{
			RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
			const char *name = pProp->m_pVarName;

			if (!strcmp(pszName, "DT_BaseViewModel"))
			{
				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
			else if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
			{
				if (!strcmp(name, "m_nSmokeEffectTickBegin"))
				{
					oSmokeEffectTickBegin = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_SmokeEffectTickBegin;
				}
				else if (!strcmp(name, "m_bDidSmokeEffect"))
				{
					oDidSmokeEffect = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_DidSmokeEffect;
				}
			}
			else if (!strcmp(pszName, "DT_CSPlayer"))
			{
				if (!strcmp(name, "m_flFlashMaxAlpha"))
				{
					oFlashMaxAlpha = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_FlashMaxAlpha;
				}
			}
		}

		pClass = pClass->m_pNext;
	}
}

void RemoveNetVarsHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();

	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

		for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
		{
			RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
			const char *name = pProp->m_pVarName;

			if (!strcmp(pszName, "DT_BaseViewModel"))
			{
				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					pProp->m_ProxyFn = oRecvnModelIndex;
				}
			}
			else if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
			{
				if (!strcmp(name, "m_nSmokeEffectTickBegin"))
				{
					pProp->m_ProxyFn = oSmokeEffectTickBegin;
				}
				else if (!strcmp(name, "m_bDidSmokeEffect"))
				{
					pProp->m_ProxyFn = oDidSmokeEffect;
				}
			}
			else if (!strcmp(pszName, "DT_CSPlayer"))
			{
				if (!strcmp(name, "m_flFlashMaxAlpha"))
				{
					pProp->m_ProxyFn = oFlashMaxAlpha;
				}
			}
		}

		pClass = pClass->m_pNext;
	}
}