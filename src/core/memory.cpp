#include "memory.h"
#include <core/gamedata.h>
#include <core/interfaces.h>

PLUGIN_GLOBALVARS();

template<typename T>
struct ReturnType;

template<typename Ret, typename... Args>
struct ReturnType<Ret (*)(Args...)> {
	using type = Ret;
};

#define CALL_SIG(sig, fnCurrent, ...) \
	static auto fnSig = GAMEDATA::GetMemSig(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnSig, __VA_ARGS__);

#define CALL_ADDRESS(sig, fnCurrent, ...) \
	static auto fnSig = GAMEDATA::GetAddress(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnSig, __VA_ARGS__);

#pragma region calls

#pragma endregion

#pragma region hooks

// clang-format off
SH_DECL_HOOK0(CNetworkGameServerBase, ActivateServer, SH_NOATTRIB, false, bool);
static bool Hook_ActivateServer() {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnActivateServer(META_IFACEPTR(CNetworkGameServerBase));
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

// clang-format on

#pragma endregion

#pragma region setup

static bool SetupDetours() {
	// clang-format off
	// clang-format on

	return true;
}

static bool SetupVMTHooks() {
	// clang-format off

	// clang-format on
	return true;
}

static bool SetupSourceHooks() {
	SH_ADD_HOOK(ISource2Server, GameFrame, IFACE::pServer, SH_STATIC(Hook_OnGameFrame), false);

	SH_ADD_HOOK(ISource2GameClients, ClientConnect, g_pSource2GameClients, SH_STATIC(Hook_ClientConnect), false);
	SH_ADD_HOOK(ISource2GameClients, OnClientConnected, g_pSource2GameClients, SH_STATIC(Hook_OnClientConnected), false);
	SH_ADD_HOOK(ISource2GameClients, ClientFullyConnect, g_pSource2GameClients, SH_STATIC(Hook_ClientFullyConnect), false);
	SH_ADD_HOOK(ISource2GameClients, ClientPutInServer, g_pSource2GameClients, SH_STATIC(Hook_ClientPutInServer), false);
	SH_ADD_HOOK(ISource2GameClients, ClientActive, g_pSource2GameClients, SH_STATIC(Hook_ClientActive), false);
	SH_ADD_HOOK(ISource2GameClients, ClientDisconnect, g_pSource2GameClients, SH_STATIC(Hook_ClientDisconnect), true);
	SH_ADD_HOOK(ISource2GameClients, ClientVoice, g_pSource2GameClients, SH_STATIC(Hook_ClientVoice), false);
	SH_ADD_HOOK(ISource2GameClients, ClientCommand, g_pSource2GameClients, SH_STATIC(Hook_ClientCommand), false);

	SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_STATIC(Hook_StartupServer), true);

	SH_ADD_HOOK(ICvar, DispatchConCommand, g_pCVar, SH_STATIC(Hook_DispatchConCommand), false);

	SH_ADD_HOOK(IGameEventSystem, PostEventAbstract, IFACE::pGameEventSystem, SH_STATIC(Hook_PostEvent), false);

	// clang-format off
	SH_ADD_DVPHOOK(
		CNetworkGameServerBase, 
		ActivateServer,
		MEM::MODULE::engine->GetVirtualTableByName("CNetworkGameServer").RCast<CNetworkGameServerBase*>(),
		SH_STATIC(Hook_ActivateServer), 
		true
	);
	// clang-format on
	return true;
}

void MEM::SetupHooks() {
	SURF_ASSERT(SetupDetours());
	SURF_ASSERT(SetupVMTHooks());
	SURF_ASSERT(SetupSourceHooks());
}

void MEM::MODULE::Setup() {
	engine = std::make_shared<libmodule::CModule>();
	engine->InitFromName(LIB::engine2, true);

	tier0 = std::make_shared<libmodule::CModule>();
	tier0->InitFromName(LIB::tier0, true);

	server = std::make_shared<libmodule::CModule>();
	server->InitFromMemory(libmem::GetModule(LIB::server).base);

	schemasystem = std::make_shared<libmodule::CModule>();
	schemasystem->InitFromName(LIB::schemasystem, true);

	steamnetworkingsockets = std::make_shared<libmodule::CModule>();
	steamnetworkingsockets->InitFromName(LIB::steamnetworkingsockets, true);
}

#pragma endregion
