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
