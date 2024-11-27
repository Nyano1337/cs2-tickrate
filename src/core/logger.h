#pragma once

#include <tier0/logging.h>
#include <cstdint>
#include <type_traits>

namespace LOG {
	inline LoggingChannelID_t g_iChannelID = INVALID_LOGGING_CHANNEL_ID;

	void Setup(Color color);
	void Setup(uint32_t color32);

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, LoggingResponse_t> Print(const char* fmt) {
		return LoggingSystem_Log(g_iChannelID, LS_MESSAGE, "%s", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, LoggingResponse_t> Print(const char* fmt, Args... args) {
		return LoggingSystem_Log(g_iChannelID, LS_MESSAGE, fmt, args...);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, LoggingResponse_t> PrintAssert(const char* fmt) {
		return LoggingSystem_LogAssert("%s", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, LoggingResponse_t> PrintAssert(const char* fmt, Args... args) {
		return LoggingSystem_LogAssert(fmt, args...);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, LoggingResponse_t> PrintWarning(const char* fmt) {
		return LoggingSystem_Log(g_iChannelID, LS_WARNING, "%s", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, LoggingResponse_t> PrintWarning(const char* fmt, Args... args) {
		return LoggingSystem_Log(g_iChannelID, LS_WARNING, fmt, args...);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, LoggingResponse_t> PrintError(const char* fmt) {
		return LoggingSystem_Log(g_iChannelID, LS_ERROR, "%s", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, LoggingResponse_t> PrintError(const char* fmt, Args... args) {
		return LoggingSystem_Log(g_iChannelID, LS_ERROR, fmt, args...);
	}
} // namespace LOG
