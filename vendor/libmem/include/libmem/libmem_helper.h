#pragma once

#include <sstream>
#include <cstring>
#include <unordered_map>
#include <libmem/libmem_virtual.h>

#ifdef _WIN32
#define MODULE_PREFIX ""
#define MODULE_EXT    ".dll"
#else
#define MODULE_PREFIX "lib"
#define MODULE_EXT    ".so"
#endif

#define LM_STORE_TO_ADDRESS(address, ...) libmem::StoreToAddress(address, __VA_ARGS__)

#define LM_STORE_TO_ADDRESS_WITH_OFFSET(address, offset, ...) libmem::StoreToAddress<offset>(address, __VA_ARGS__)

namespace libmem {
	template<typename T>
	inline bool HookFunc(void* from, T to, void*& trampoline) {
		if (!from) {
			return false;
		}

		auto res = HookCode((Address)from, (Address)to);
		if (!res.has_value()) {
			return false;
		}

		trampoline = res.value().callable<void*>();
		return true;
	}

	inline bool UnhookFunc(void* from, void* trampoline) {
		return UnhookCode((Address)from, *(Trampoline*)trampoline);
	}

	inline Module GetModule(const char* name) {
		static std::unordered_map<std::string, Module> mods;
		auto it = mods.find(name);
		if (it != mods.end()) {
			return it->second;
		}

		Module mod;
		auto modules = std::move(EnumModules().value());

		// dont get our own server.dll module.
		constexpr const char* libServer = MODULE_PREFIX "server" MODULE_EXT;
		bool is_find_server_hdl = !strcmp(name, libServer);

		for (auto& it : modules) {
			if (is_find_server_hdl && strstr(it.path.c_str(), "addons") != nullptr) {
				continue;
			}

			if (!strcmp(it.name.c_str(), name)) {
				mod = Module(it);
				break;
			}
		}

		mods[name] = mod;
		return mod;
	}

	inline Address FindSymbolAddress(const Module& module, const char* symbol_name) {
		auto res = FindSymbolAddress(&module, symbol_name);
		return res.has_value() ? res.value() : (Address) nullptr;
	}

	inline Address FindSymbolAddress(const char* module, const char* name) {
		auto pModule = GetModule(module);
		if (!pModule.base) {
			return (Address) nullptr;
		}

		return FindSymbolAddress(&pModule, name).value();
	}

	inline auto PatternScan(const unsigned char* pattern, const char* mask, const Module& module) {
		std::vector<uint8_t> vecPattern;
		std::istringstream iss(reinterpret_cast<const char*>(pattern));

		uint8_t val;
		while (iss >> std::hex >> val) {
			vecPattern.push_back(val);
		}

		PatternScan(vecPattern, mask, module.base, module.size);
	}

	inline auto PatternScan(const unsigned char* pattern, const char* mask, const char* moduleName) {
		return PatternScan(pattern, mask, GetModule(moduleName));
	}

	inline void* SignScan(const char* sig, const char* moduleName) {
		auto module = GetModule(moduleName);
		auto val = SigScan(sig, module.base, module.size);
		return val.has_value() ? (void*)val.value() : nullptr;
	}

	inline void* SignScan(const char* sig, const Module& module) {
		auto val = SigScan(sig, module.base, module.size);
		return val.has_value() ? (void*)val.value() : nullptr;
	}

	inline auto VmtGetOriginalFunc(void* instance, int fnindex) {
		Vmt vmt(*static_cast<Address**>(instance));
		return vmt.GetOriginal(fnindex);
	}

	inline auto VmtGet(void* instance) {
		return *static_cast<void***>(instance);
	}

	template<typename T>
	inline auto VmtHook(void* instance, int fnindex, T hook_func) {
		Vmt vmt(*static_cast<Address**>(instance));

		Address* vfunc_ptr = *(Address**)vmt.Convert() + fnindex;

		vmt.Hook(fnindex, (Address)hook_func);
	}

	template<typename T>
	inline auto VmtHookEx(void* instance, int fnindex, T hook_func, void*& org_func) {
		Vmt vmt(*static_cast<Address**>(instance));

		Address* vfunc_ptr = *(Address**)vmt.Convert() + fnindex;

		org_func = vmt.GetOriginal<void*>(fnindex);
		vmt.Hook(fnindex, (Address)hook_func);
	}

	template<size_t nOffset = 0, typename... Args>
	inline void StoreToAddress(void* pAddress, Args... args) {
		const uint8_t bytes[] = {static_cast<uint8_t>(args)...};
		uint8_t* adr_patch = static_cast<uint8_t*>(pAddress) + nOffset;
		for (size_t i = 0; i < sizeof...(args); ++i) {
			auto old_mem_prot = ProtMemory((Address)adr_patch, 1, Prot::XRW);
			if (old_mem_prot.has_value()) {
				adr_patch[i] = bytes[i];
				ProtMemory((Address)adr_patch, 1, old_mem_prot.value());
			}
		}
	}
} // namespace libmem
