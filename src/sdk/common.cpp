#include "common.h"

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#include <shellapi.h>
#else
#include <dlfcn.h>
#include <limits.h>
#endif

void* Plat_LoadLibrary(const char* path, int mode) {
#ifdef _WIN32
	return (void*)LoadLibrary(path);
#else
	return mode != -1 ? dlopen(path, mode) : dlopen(path, RTLD_LAZY);
#endif
}

bool Plat_FreeLibrary(void* lib) {
#ifdef _WIN32
	return FreeLibrary(reinterpret_cast<HMODULE>(lib));
#else
	return dlclose(lib);
#endif
}

void* Plat_GetProcAddress(void* lib, const char* symbol) {
#ifdef _WIN32
	return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(lib), symbol));
#else
	return dlsym(lib, symbol);
#endif
}

void* Plat_GetModuleHandle(const char* path) {
#ifdef _WIN32
	return GetModuleHandle(path);
#else
	return dlopen(path, RTLD_LAZY);
#endif
}

bool Plat_GetCommandArgument(const char* argName, char* buffer, size_t maxlength) {
	if (buffer) {
		buffer[0] = '\0';
	}

#if defined _WIN32
	wchar_t wargName[256];
	mbstowcs(wargName, argName, sizeof(wargName) / sizeof(wchar_t));

	LPWSTR pCmdLine = GetCommandLineW();
	int argc;
	LPWSTR* wargv = CommandLineToArgvW(pCmdLine, &argc);

	bool found = false;
	for (int i = 0; i < argc; ++i) {
		if (wcscmp(wargv[i], wargName) == 0) {
			found = true;

			if ((++i) < argc && buffer && wargv[i][0] != '-') {
				wcstombs(buffer, wargv[i], maxlength);
				buffer[maxlength - 1] = '\0';
			}

			break;
		}
	}

	LocalFree(wargv);

	return found;
#else
	FILE* pFile = fopen("/proc/self/cmdline", "rb");
	if (pFile) {
		char* arg = nullptr;
		size_t argsize = 0;
		bool nextIsValue = false;
		bool found = false;

		while (getdelim(&arg, &argsize, 0, pFile) != -1) {
			if (nextIsValue) {
				if (buffer && arg[0] != '-') {
					strncpy(buffer, arg, maxlength);
					buffer[maxlength - 1] = '\0';
				}

				found = true;
				break;
			}

			if (strcmp(arg, argName) == 0) {
				nextIsValue = true;
			}
		}

		free(arg);
		fclose(pFile);

		return found || nextIsValue;
	}
#endif
	return false;
}
