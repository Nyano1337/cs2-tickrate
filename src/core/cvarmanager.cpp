#include "cvarmanager.h"
#include <pch.h>
#include <libmem/libmem_virtual.h>

void CVAR::Register(const char* name, const char* defaultValue, const char* helpString, ConVarS2<const char*>::FnChangeCallback_t callback) {
	ConVarS2<const char*>* pCvarCreated = new ConVarS2<const char*>(name, 0, helpString, defaultValue, callback);
}

void CVAR::Register(const char* name, bool defaultValue, const char* helpString, ConVarS2<bool>::FnChangeCallback_t callback) {
	ConVarS2<bool>* pCvarCreated = new ConVarS2<bool>(name, 0, helpString, defaultValue, callback);
}

void CVAR::Register(const char* name, int defaultValue, const char* helpString, ConVarS2<int>::FnChangeCallback_t callback) {
	ConVarS2<int>* pCvarCreated = new ConVarS2<int>(name, 0, helpString, defaultValue, callback);
}

void CVAR::Register(const char* name, float defaultValue, const char* helpString, ConVarS2<float>::FnChangeCallback_t callback) {
	ConVarS2<float>* pCvarCreated = new ConVarS2<float>(name, 0, helpString, defaultValue, callback);
}

void CVAR::Register(const char* name, double defaultValue, const char* helpString, ConVarS2<double>::FnChangeCallback_t callback) {
	ConVarS2<double>* pCvarCreated = new ConVarS2<double>(name, 0, helpString, defaultValue, callback);
}
