#pragma once

#include <ISmmPlugin.h>
#include <cstdint>

class CGameResourceService;
class IVEngineServer2;
class ISource2Server;
class IGameEventManager2;
class IGameEventSystem;
class CGameTraceService;

namespace IFACE {
	inline CGameResourceService* pGameResourceServiceServer = nullptr;
	inline IVEngineServer2* pEngine = nullptr;
	inline ISource2Server* pServer = nullptr;
	inline IGameEventManager2* pGameEventManager = nullptr;
	inline IGameEventSystem* pGameEventSystem = nullptr;
	inline CGameTraceService* pGameTraceService = nullptr;

	bool Setup(ISmmAPI* ismm, char* error, size_t maxlen);
} // namespace IFACE
