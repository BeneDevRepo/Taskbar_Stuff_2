#pragma once

// #define WIN32_LEAN_AND_MEAN
// #ifdef WINVER
// 	#undef WINVER
// #endif
// #define WINVER 0x0602 // Windows 8
// #define WINVER 0x0A00 // Windows 10 ?

#define UNICODE
#define _UNICODE 

#include <windows.h>
#include <windowsx.h>
// #pragma comment(lib, "user32.lib");

#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#define MOD_NOREPEAT 0x4000