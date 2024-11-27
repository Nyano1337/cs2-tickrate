#pragma once

#include <ISmmPlugin.h>

class BaseConVar;
class CServerSideClient;

struct CFrame {
	float starttime_stddeviation;
	float time_stddeviation;
	float time_computationduration;
	float time_unbounded;
};

class CCS2Tickrate : public ISmmPlugin, public IMetamodListener {
private:
	bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
	const char* GetAuthor() override;
	const char* GetName() override;
	const char* GetDescription() override;
	const char* GetURL() override;
	const char* GetLicense() override;
	const char* GetVersion() override;
	const char* GetDate() override;
	const char* GetLogTag() override;

public:
	void RegisterTickrate();
	bool DoChangeTickrate(int newTickrate);
	int GetTickrate() const;

public:
	void OnFillServerInfo(CNetworkGameServerBase* pNetServer, CSVCMsg_ServerInfo_t* pServerInfo);
	void OnConnectClient(CNetworkGameServerBase* pNetServer, CServerSideClientBase* pClient, const char* pszName, ns_address* pAddr, int socket,
					CCLCMsg_SplitPlayerConnect_t* pSplitPlayer, const char* pszChallenge, const byte* pAuthTicket, int nAuthTicketLength,
					bool bIsLowViolence);

public:
	static void OnTickrateChanged(BaseConVar* ref, const CSplitScreenSlot nSlot, const int* pNewValue, const int* pOldValue);
	static CUtlVector<CServerSideClient*>* GetClientList();
	static CGlobalVars* GetGlobals();
	static CFrame* GetHostFramePointer();

public:
	int m_iCurrentTickrate;
	int m_iOldTickrate;
	BaseConVar* sv_tickrate;

public:
	float* m_pInterval = nullptr;
	double* m_pInterval2 = nullptr;
	float* m_pInterval3Default = nullptr;
	float* m_pInterval3 = nullptr;
	float* m_pPerSecond = nullptr;
	CFrame* m_pHostFrame = nullptr;
};

extern CCS2Tickrate* TickratePlugin();
