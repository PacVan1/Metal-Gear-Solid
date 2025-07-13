#pragma once

enum debugLogTypes : u8
{
	INFO,
	WARNING,
	ERROR,
	ASSET_SAVED,
	ASSET_LOADED,
	ASSET_FAILED,
	ASSET_GENERATED,
};

void _DebugLog(u8 const type, char const* message, char const* file, int const line);
#define DebugLog(type, message) _DebugLog(type, message, __FILE__, __LINE__)
#define DebugLogLoc(type, message, file, line) _DebugLog(type, message, file, line)