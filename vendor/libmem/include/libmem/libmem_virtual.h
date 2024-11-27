#pragma once

#include <libmem/libmem.hpp>

#define GET_VIRTUAL(pInstance, idx)        vmt::GetVMethod(idx, pInstance)
#define SET_VIRTUAL(pInstance, idx, pFunc) vmt::SetVMethod(idx, pInstance, pFunc)
#define CALL_VIRTUAL(retType, idx, ...)    vmt::CallVirtual<retType>(idx, __VA_ARGS__)

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

namespace vmt {
	template<typename T = void*>
	inline T GetVMethod(uint32_t uIndex, void* pInstance) {
		if (!pInstance) {
			printf("vmt::GetVMethod failed: invalid instance pointer\n");
			return T();
		}

		void** vtable = *static_cast<void***>(pInstance);
		if (!vtable) {
			printf("vmt::GetVMethod failed: invalid vtable pointer\n");
			return T();
		}

		return reinterpret_cast<T>(vtable[uIndex]);
	}

	// PS: The template function can't discerm should passed the var reference so we should use the `std::ref`
	template<typename T, typename... Args>
	inline T CallVirtual(uint32_t uIndex, void* pClass, Args... args) {
		auto func_ptr = GetVMethod<T(__thiscall*)(void*, Args...)>(uIndex, pClass);
		if (!func_ptr) {
			printf("vmt::CallVirtual failed: invalid function pointer\n");
			return T();
		}
		return func_ptr(pClass, args...);
	}

	template<typename T>
	inline bool SetVMethod(uint32_t uIndex, void* pInstance, T pFunc) {
		if (!pInstance) {
			printf("vmt::SetVMethod failed: invalid instance pointer\n");
			return false;
		}

		void** vtable = *static_cast<void***>(pInstance);
		if (!vtable) {
			printf("vmt::SetVMethod failed: invalid vtable pointer\n");
			return false;
		}

		libmem::Vmt vmt((libmem::Address*)vtable);
		vmt.Hook(uIndex, (libmem::Address)pFunc);

		return true;
	}

	template<typename T>
	inline bool SetVMethodEx(uint32_t uIndex, void* pInstance, T pFunc, void*& pOriginFunc) {
		if (!pInstance) {
			printf("vmt::SetVMethodEx failed: invalid instance pointer\n");
			return false;
		}

		void** vtable = *static_cast<void***>(pInstance);
		if (!vtable) {
			printf("vmt::SetVMethodEx failed: invalid vtable pointer\n");
			return false;
		}

		libmem::Vmt vmt((libmem::Address*)vtable);
		vmt.Hook(uIndex, (libmem::Address)pFunc);

		pOriginFunc = (void*)vmt.GetOriginal(uIndex);

		return true;
	}

	template<typename T>
	inline void Override(T& pOrigin, T pFunc) {
		if (!pFunc) {
			return;
		}

		pOrigin = pFunc;
	}

} // namespace vmt
