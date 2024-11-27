#include "memory.h"
#include <core/gamedata.h>
#include <core/interfaces.h>
#include <cs2tickrate.h>

PLUGIN_GLOBALVARS();

template<typename T>
struct ReturnType;

template<typename Ret, typename... Args>
struct ReturnType<Ret (*)(Args...)> {
	using type = Ret;
};

#define CALL_SIG(sig, fnCurrent, ...) \
	static auto fnSig = GAMEDATA::GetMemSig(sig); \
	TICKRATE_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnSig, __VA_ARGS__);

#define CALL_ADDRESS(sig, fnCurrent, ...) \
	static auto fnSig = GAMEDATA::GetAddress(sig); \
	TICKRATE_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnSig, __VA_ARGS__);

#pragma region calls

#pragma endregion

#pragma region hooks

// clang-format off
SH_DECL_HOOK1_void(CNetworkGameServerBase, FillServerInfo, SH_NOATTRIB, false, CSVCMsg_ServerInfo_t*)
static void Hook_OnFillServerInfo(CSVCMsg_ServerInfo_t* pServerInfo) {
	auto* pNetServer = META_IFACEPTR(CNetworkGameServerBase);

	TickratePlugin()->OnFillServerInfo(pNetServer, pServerInfo);

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK8(CNetworkGameServerBase, ConnectClient, SH_NOATTRIB, false, CServerSideClientBase*, const char*, ns_address*, int, CCLCMsg_SplitPlayerConnect_t*, const char*, const byte*, int, bool);
static CServerSideClientBase* Hook_OnConnectClient(const char* pszName, ns_address* pAddr, int socket, CCLCMsg_SplitPlayerConnect_t* pSplitPlayer,
												   const char* pszChallenge, const byte* pAuthTicket, int nAuthTicketLength, bool bIsLowViolence) {
	auto *pNetServer = META_IFACEPTR(CNetworkGameServerBase);

	auto *pClient = META_RESULT_ORIG_RET(CServerSideClientBase *);

	TickratePlugin()->OnConnectClient(pNetServer, pClient, pszName, pAddr, socket, pSplitPlayer, pszChallenge, pAuthTicket, nAuthTicketLength, bIsLowViolence);

	RETURN_META_VALUE(MRES_IGNORED, NULL);
}

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, false, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);
static void Hook_OnStartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession* pWorldSession, const char*) {
	auto* pNetServer = reinterpret_cast<CNetworkGameServerBase*>(g_pNetworkServerService->GetIGameServer());

	SH_ADD_HOOK(CNetworkGameServerBase, FillServerInfo, pNetServer, SH_STATIC(Hook_OnFillServerInfo), true);
	SH_ADD_HOOK(CNetworkGameServerBase, ConnectClient, pNetServer, SH_STATIC(Hook_OnConnectClient), true);

	RETURN_META(MRES_IGNORED);
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
	// clang-format off
	SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_STATIC(Hook_OnStartupServer), true);
	// clang-format on
	return true;
}

void MEM::SetupHooks() {
	TICKRATE_ASSERT(SetupDetours());
	TICKRATE_ASSERT(SetupVMTHooks());
	TICKRATE_ASSERT(SetupSourceHooks());
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
