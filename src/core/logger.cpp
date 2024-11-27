#include "logger.h"

void LOG::Setup(Color color) {
	g_iChannelID = LoggingSystem_RegisterLoggingChannel("cs2tickrate", nullptr, 0, LV_DEFAULT, color);
}

void LOG::Setup(uint32_t color32) {
	// clang-format off
	char a = (color32 >> 24) & 0xFF;
	char r = (color32 >> 16) & 0xFF;
	char g = (color32 >>  8) & 0xFF;
	char b = (color32 >>  0) & 0xFF;
	// clang-format on
	Setup(Color(r, g, b, a));
}
