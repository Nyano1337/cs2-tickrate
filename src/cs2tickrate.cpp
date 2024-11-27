#include "cs2tickrate.h"

#include <core/interfaces.h>
#include <core/logger.h>

CCS2Tickrate g_TickratePlugin;

PLUGIN_EXPOSE(CCS2Tickrate, g_TickratePlugin);

CCS2Tickrate* TickratePlugin() {
	return &g_TickratePlugin;
}

bool CCS2Tickrate::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) {
	PLUGIN_SAVEVARS();

	IFACE::Setup(ismm, error, maxlen);

	LOG::Setup(0x00FFFF);

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
