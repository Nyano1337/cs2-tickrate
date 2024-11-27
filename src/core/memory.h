#pragma once

#include <pch.h>

#include <sdk/common.h>
#include <libmem/libmem_helper.h>
#include <vendor/libmodule/module.h>

class GameSessionConfiguration_t {};

#define HOOK_SIG(sig, fnHook, fnTrampoline) \
	static auto fn##fnHook = GAMEDATA::GetMemSig(sig); \
	SURF_ASSERT(fn##fnHook); \
	if (fn##fnHook) { \
		libmem::HookFunc(fn##fnHook, fnHook, fnTrampoline); \
	}

#define HOOK_VMT(gdOffsetKey, pModule, fnHook, fnTrampoline) \
	SURF_ASSERT(MEM::VmtHookEx(GAMEDATA::GetOffset(gdOffsetKey), pModule.get(), gdOffsetKey, fnHook, fnTrampoline));

namespace MEM {
	namespace CALL {} // namespace CALL

	namespace MODULE {
		inline std::shared_ptr<libmodule::CModule> engine;
		inline std::shared_ptr<libmodule::CModule> tier0;
		inline std::shared_ptr<libmodule::CModule> server;
		inline std::shared_ptr<libmodule::CModule> schemasystem;
		inline std::shared_ptr<libmodule::CModule> steamnetworkingsockets;

		void Setup();
	} // namespace MODULE

	namespace TRAMPOLINE {} // namespace TRAMPOLINE

	void SetupHooks();

	template<typename T, typename... Args>
	T SDKCall(void* pAddress, Args... args) {
		auto pFn = reinterpret_cast<T (*)(Args...)>(pAddress);
		TICKRATE_ASSERT((uintptr_t)pFn);
		return pFn(args...);
	}

	template<typename T, typename... Args>
	auto GetFn(void* pAddress, Args... args) {
		return reinterpret_cast<T (*)(Args...)>(pAddress);
	}

	template<typename T>
	inline bool VmtHookEx(uint32_t uIndex, libmodule::CModule* pModule, const char* className, T pFunc, void*& pOriginFunc,
						  bool allInstance = false) {
		if (!pModule) {
			return false;
		}

		auto extractClassName = [](std::string input) {
			size_t pos = input.find("::");
			if (pos != std::string::npos) {
				return input.substr(0, pos);
			}
			return input;
		};

		auto sClassName = extractClassName(className);

		void* vtable = pModule->GetVirtualTableByName(sClassName);
		if (!vtable) {
			return false;
		}

		libmem::Vmt vmt((libmem::Address*)vtable);
		auto from = vmt.GetOriginal(uIndex);

		if (allInstance) {
			libmem::HookFunc((void*)from, pFunc, pOriginFunc);
		} else {
			vmt.Hook(uIndex, (libmem::Address)pFunc);
			pOriginFunc = (void*)from;
		}

		return true;
	}
} // namespace MEM
