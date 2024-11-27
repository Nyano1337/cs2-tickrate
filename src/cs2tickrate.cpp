#include "cs2tickrate.h"

#include <core/interfaces.h>
#include <core/logger.h>
#include <core/gamedata.h>
#include <core/cvarmanager.h>
#include <core/memory.h>
#include <sdk/serversideclient.h>
#include <libmem/libmem_helper.h>
#include <memaddr.h>
#include <sdk/protobuf/netmessages.pb.h>
#include <networksystem/inetworkmessages.h>

CCS2Tickrate g_TickratePlugin;

PLUGIN_EXPOSE(CCS2Tickrate, g_TickratePlugin);

CCS2Tickrate* TickratePlugin() {
	return &g_TickratePlugin;
}

bool CCS2Tickrate::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) {
	PLUGIN_SAVEVARS();

	GAMEDATA::Append("cs2tickrate.games.jsonc");

	IFACE::Setup(ismm, error, maxlen);

	RegisterTickrate();

	m_iCurrentTickrate = TICKRATE_DEFAULT;
	m_iOldTickrate = TICKRATE_DEFAULT;

	LOG::Setup(0x00FFFF);
	MEM::SetupHooks();

	auto* pCommandLine = CommandLine();

	if (pCommandLine) {
		int iTickrate = pCommandLine->ParmValue("-tickrate", -1);

		if (iTickrate != -1) {
			TICKRATE_ASSERT(DoChangeTickrate(iTickrate));
		}
	}

	sv_tickrate = CVAR::Register("sv_tickrate", TICKRATE_DEFAULT, "Server tickrate", OnTickrateChanged);

	return true;
}

const char* CCS2Tickrate::GetAuthor() {
	return "Wend4r, Nyano1337";
}

const char* CCS2Tickrate::GetName() {
	return "CS2-Tickrate";
}

const char* CCS2Tickrate::GetDescription() {
	return "yep";
}

const char* CCS2Tickrate::GetURL() {
	return "";
}

const char* CCS2Tickrate::GetLicense() {
	return "MIT License";
}

const char* CCS2Tickrate::GetVersion() {
	return "dev";
}

const char* CCS2Tickrate::GetDate() {
	return "";
}

const char* CCS2Tickrate::GetLogTag() {
	return "";
}

void CCS2Tickrate::RegisterTickrate() {
	m_pInterval = (float*)GAMEDATA::GetAddress("&tick_interval");
	m_pInterval2 = (double*)GAMEDATA::GetAddress("&(double)tick_interval");
	m_pInterval3Default = (float*)GAMEDATA::GetAddress("&ticks_per_second");

	m_pInterval3 = (float*)GAMEDATA::GetPatchAddress("&tick_interval3_default");
	m_pPerSecond = (float*)GAMEDATA::GetPatchAddress("&tick_interval3");

	m_pHostFrame = GetHostFramePointer();

	libmem::ProtMemory((libmem::Address)m_pInterval, sizeof(m_pInterval), libmem::Prot::XRW);
	libmem::ProtMemory((libmem::Address)m_pInterval2, sizeof(m_pInterval2), libmem::Prot::XRW);
	libmem::ProtMemory((libmem::Address)m_pInterval3Default, sizeof(m_pInterval3Default), libmem::Prot::XRW);
	libmem::ProtMemory((libmem::Address)m_pInterval3, sizeof(m_pInterval3), libmem::Prot::XRW);
	libmem::ProtMemory((libmem::Address)m_pPerSecond, sizeof(m_pPerSecond), libmem::Prot::XRW);
}

bool CCS2Tickrate::DoChangeTickrate(int newTickrate) {
	m_iCurrentTickrate = newTickrate;

	float flOldInterval = (1.0f / m_iOldTickrate);
	float flNewInterval = (1.0f / newTickrate);
	float dblNewInterval = (1.02l / newTickrate);
	float flMultiple = (m_iOldTickrate / (newTickrate * 1.0));

	*m_pInterval = flNewInterval;
	*m_pInterval3 = flNewInterval;
	*m_pInterval2 = dblNewInterval;
	*m_pPerSecond = newTickrate * 1.0;

	CFrame* pHostFrame = GetHostFramePointer();
	if (!pHostFrame) {
		return false;
	}

	pHostFrame->time_unbounded = flNewInterval;
	pHostFrame->time_computationduration = flNewInterval;

	INetworkGameServer* pServer = g_pNetworkServerService->GetIGameServer();
	if (!pServer) {
		return false;
	}

	pServer->SetServerTick((int)(pServer->GetServerTick() * flMultiple));

	auto pGlobals = GetGlobals();
	pGlobals->absoluteframetime = flNewInterval;
	pGlobals->absoluteframestarttimestddev = flNewInterval;

	pGlobals->frametime *= flMultiple;
	pGlobals->curtime *= flMultiple;
	pGlobals->rendertime *= flMultiple;

	auto pClientList = GetClientList();
	if (pClientList) {
		for (int i = 0; i < pClientList->Count(); i++) {
			auto pClient = pClientList->Element(i);
			if (pClient->IsConnected() && !pClient->IsFakeClient()) {
				auto aPlayerSlot = pClient->GetPlayerSlot();
				int slot = aPlayerSlot.Get();
				if (slot >= 0 && slot < pGlobals->maxClients) {
					pClient->SetUpdateRate(GetTickrate() * 1.0);
				}
			}
		}
	}

	return true;
}

int CCS2Tickrate::GetTickrate() const {
	if (m_pInterval) {
		return (int)(1.0f / *m_pInterval);
	}

	return TICKRATE_DEFAULT;
}

void CCS2Tickrate::OnFillServerInfo(CNetworkGameServerBase* pNetServer, CSVCMsg_ServerInfo_t* pServerInfo) {
	auto* pMessage = pServerInfo->ToPB<CSVCMsg_ServerInfo>();

	pMessage->set_tick_interval(1.0f / (float)this->GetTickrate());
}

void CCS2Tickrate::OnConnectClient(CNetworkGameServerBase* pNetServer, CServerSideClientBase* pClient, const char* pszName, ns_address* pAddr,
								   int socket, CCLCMsg_SplitPlayerConnect_t* pSplitPlayer, const char* pszChallenge, const byte* pAuthTicket,
								   int nAuthTicketLength, bool bIsLowViolence) {
	static INetworkMessageInternal* pMessage = g_pNetworkMessages->FindNetworkMessagePartial("CNETMsg_SetConVar");

	auto* pSetConVarMessage = pMessage->AllocateMessage()->ToPB<CNETMsg_SetConVar>();
	auto* pConVar = pSetConVarMessage->mutable_convars()->add_cvars();
	pConVar->set_name("cl_clock_correction");
	pConVar->set_value("0");

	auto aPlayerSlot = pClient->GetPlayerSlot();
	uint64 clients = {1llu << aPlayerSlot.Get()};
	IFACE::pGameEventSystem->PostEventAbstract(-1, false, aPlayerSlot.Get(), &clients, pMessage, pSetConVarMessage, 0, BUF_RELIABLE);

	delete pSetConVarMessage;

	pClient->SetUpdateRate((float)this->GetTickrate());
}

void CCS2Tickrate::OnTickrateChanged(BaseConVar* ref, const CSplitScreenSlot nSlot, const int* pNewValue, const int* pOldValue) {
	// check server startup
	if (*pOldValue == 0) {
		return;
	}

	TickratePlugin()->m_iOldTickrate = *pOldValue;
	TICKRATE_ASSERT(TickratePlugin()->DoChangeTickrate(*pNewValue));
}

CUtlVector<CServerSideClient*>* CCS2Tickrate::GetClientList() {
	if (!g_pNetworkServerService) {
		return nullptr;
	}

	static int offset = GAMEDATA::GetOffset("ClientOffset");
	return reinterpret_cast<CUtlVector<CServerSideClient*>*>(((uint8_t*)g_pNetworkServerService->GetIGameServer() + offset));
}

CGlobalVars* CCS2Tickrate::GetGlobals() {
	INetworkGameServer* server = g_pNetworkServerService->GetIGameServer();

	if (!server) {
		return nullptr;
	}

	return server->GetGlobals();
}

CFrame* CCS2Tickrate::GetHostFramePointer() {
	using namespace libmodule;
	CMemory addr = GAMEDATA::GetAddress("GetHostFrame");

#ifdef _WIN32
	constexpr auto offset_to_hostframe = offsetof(CFrame, time_unbounded);
	addr.OffsetSelf(-offset_to_hostframe);
#else
	addr = MEM::SDKCall<CFrame*>(addr.RCast<void*>());
#endif

	return addr.RCast<CFrame*>();
}
