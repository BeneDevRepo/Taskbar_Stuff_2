#pragma once

#include "winapi_include.h"

#include "Texture.h"

#include <cstdint>

class Taskbar {
public:
    Texture graphics;
    uint32_t width, height;
	int32_t mouseX, mouseY;

private:
    HDC device_context;
    HWND wnd;
	// uint32_t appbarSide;
	APPBARDATA appbarData;

private:
	static LRESULT WINAPI StaticWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

private:
	void taskbarCallback(HWND hwndAccessBar, UINT uNotifyMsg, LPARAM lParam);
	void setPos(LPRECT lprc);
	void appBarPosChanged();

public:
    Taskbar(int width, int height);
    ~Taskbar();
    bool pollMsg();
    void updateScreen();
    void blitTexture(const Texture& tex);
    void resize(uint32_t width, uint32_t height);
};