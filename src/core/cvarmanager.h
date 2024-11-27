#pragma once

#include <sdk/convars2.h>

namespace CVAR {
	BaseConVar* Register(const char* name, const char* defaultValue = "", const char* helpString = "",
						 ConVarS2<const char*>::FnChangeCallback_t callback = 0);
	BaseConVar* Register(const char* name, bool defaultValue = false, const char* helpString = "", ConVarS2<bool>::FnChangeCallback_t callback = 0);
	BaseConVar* Register(const char* name, int defaultValue = 0, const char* helpString = "", ConVarS2<int>::FnChangeCallback_t callback = 0);
	BaseConVar* Register(const char* name, float defaultValue = 0.0, const char* helpString = "", ConVarS2<float>::FnChangeCallback_t callback = 0);
	BaseConVar* Register(const char* name, double defaultValue = 0.0, const char* helpString = "", ConVarS2<double>::FnChangeCallback_t callback = 0);
} // namespace CVAR
