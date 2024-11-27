#include "convars2.h"
#include <libmem/libmem_virtual.h>

void RegisterConVarS2(ConVarCreation_t& cvar) {
	CALL_VIRTUAL(void, 35, g_pCVar, cvar, 0, cvar.m_pHandle, cvar.m_pConVarData);
	if (!cvar.m_pHandle->IsValid()) {
		TICKRATE_ASSERT(false);
	}
}

void UnRegisterConVar(ConVarHandleS2& cvar) {
	if (cvar.IsValid()) {
		CALL_VIRTUAL(void, 36, g_pCVar, cvar);

		cvar.Invalidate();
	}
}
