#include "utils.h"
#include <fstream>
#include <steam/isteamgameserver.h>
#include <core/interfaces.h>
#include <public/iserver.h>

std::string UTIL::GetWorkingDirectory() {
	return PATH::Join(std::filesystem::current_path().string(), "..", "..", GAME_NAME, "addons", "cs2surf");
}

std::string UTIL::GetPublicIP() {
	static std::string ipv4;
	if (!ipv4.empty()) {
		return ipv4;
	}

	auto sAddr = SteamGameServer()->GetPublicIP();
	if (!sAddr.IsSet()) {
		return std::string();
	}

	uint32_t ip = sAddr.m_unIPv4;
	std::ostringstream oss;

	// clang-format off
	oss << ((ip >> 24) & 0xFF) << "." 
		<< ((ip >> 16) & 0xFF) << "." 
		<< ((ip >>  8) & 0xFF) << "." 
		<< ((ip >>  0) & 0xFF);
	// clang-format on

	ipv4 = oss.str();
	return ipv4;
}

json UTIL::LoadJsonc(std::string sFilePath) {
	std::ifstream file(sFilePath);
	if (!file.good()) {
		return json();
	}

	return json::parse(file, nullptr, true, true);
}

std::wstring UTIL::ToWideString(const char* pszCharStr) {
	if (!pszCharStr || !pszCharStr[0]) {
		return std::wstring();
	}

	auto size_needed = WIN_LINUX(MultiByteToWideChar(CP_UTF8, 0, pszCharStr, -1, NULL, 0), mbstowcs(NULL, pszCharStr, 0));
	if (size_needed > 0) {
		std::wstring wSayContent;
		wSayContent.resize(size_needed, L'\0');
#ifdef _WIN32
		MultiByteToWideChar(CP_UTF8, 0, pszCharStr, -1, &wSayContent[0], size_needed);
#else
		mbstowcs(&wSayContent[0], pszCharStr, size_needed);
#endif
		return wSayContent;
	}

	return std::wstring();
}

CGlobalVars* UTIL::GetGlobals() {
	INetworkGameServer* server = g_pNetworkServerService->GetIGameServer();

	if (!server) {
		return nullptr;
	}

	return server->GetGlobals();
}
