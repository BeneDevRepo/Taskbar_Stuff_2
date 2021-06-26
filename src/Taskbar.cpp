#include "Taskbar.h"

#include <iostream>

// An application-defined message identifier
#define APPBAR_CALLBACK (WM_USER + 0x01)

Taskbar::Taskbar(int width, int height):
		graphics(width, height),
		width(width), height(height),
		mouseX(0), mouseY(0),
		device_context(nullptr),
		wnd(nullptr),
		appbarData{0} {

    WNDCLASSW win_class = {0};
    win_class.hInstance = GetModuleHandle(NULL);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.lpszClassName = L"win32app";
    win_class.lpfnWndProc = StaticWndProc;

    if (!RegisterClassW(&win_class)) {
        printf("Err: %i\n", (int)GetLastError());
        exit(-1);
    }

	// MessageBox(NULL,
	//     	L"Call to RegisterClassEx failed!",
	//     	L"Win32 Guided Tour",
	//     	NULL);

	// DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_TOPMOST; // WS_EX_TOOLWINDOW, // WS_EX_APPWINDOW
	DWORD extendedWindowStyle = WS_EX_LAYERED*0 | (WS_EX_APPWINDOW) |
            					(WS_EX_TRANSPARENT*0) | // Don't hittest this window
            					(WS_EX_TOOLWINDOW*0) | WS_EX_TOPMOST;

	DWORD windowStyle = WS_POPUP;

	CreateWindowExW(
		extendedWindowStyle,
		win_class.lpszClassName, // lpClassName
		L"Title", // lpWindowName
		windowStyle,
		100, // x
		100, // y
		width, // width
		height, // height
		NULL, // hWndParent
		NULL, // hMenu
		NULL, // hInstance
		this  // lpParam
		);

    ShowWindow(wnd, SW_SHOW);
    UpdateWindow(wnd);
	
	SetWindowLong(this->wnd, GWL_EXSTYLE, GetWindowLong(this->wnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// SetLayeredWindowAttributes(this->wnd, RGB(0xB0, 0x0B, 0x69), 200, LWA_COLORKEY | LWA_ALPHA);
	// SetLayeredWindowAttributes(this->wnd, RGB(0xB0, 0x0B, 0x69), 0, LWA_COLORKEY);
	
	{ // register appbar
		this->appbarData.cbSize = sizeof(APPBARDATA);
		this->appbarData.hWnd = this->wnd;
		this->appbarData.uCallbackMessage = APPBAR_CALLBACK;// Provide an identifier for notification messages.
		this->appbarData.uEdge = ABE_TOP; // default edge

		// Register the appbar.
		if (!SHAppBarMessage(ABM_NEW, &appbarData))
			std::cout << "ERROR: Registering Appbar failed\n";
	}

	{
		RECT cRect{0};
		// cRect.right = 400;
		// cRect.bottom = 30;
		this->setPos(&cRect);

		this->appbarData.uEdge = ABE_BOTTOM;
		// this->appBarPosChanged();
	}

	// this->appbarData.uEdge = ABE_BOTTOM;
	// this->appBarPosChanged();

	// RegisterHotKey(
	// 	wnd,
	// 	88,
	// 	MOD_ALT | MOD_CONTROL | MOD_NOREPEAT | MOD_SHIFT | MOD_WIN,
	// 	VK_TAB
	// 	);
}

Taskbar::~Taskbar() {
	SHAppBarMessage(ABM_REMOVE, &appbarData); // unregister Appbar
    ReleaseDC(this->wnd, this->device_context); // release DC
    DestroyWindow(this->wnd); // Destroy window
}


/* ----------------------
 *     Message Loop
 * --------------------- */

bool Taskbar::pollMsg() {
    MSG msg;
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT) 
            return false;
    }
	return true;
}

LRESULT WINAPI Taskbar::StaticWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Taskbar* targetWindow;
	if(msg == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)(lParam);
		targetWindow = (Taskbar*)(lpcs->lpCreateParams);
		targetWindow->wnd = wnd;
		targetWindow->device_context = GetDC(wnd);
		SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)(targetWindow));
	} else {
 		targetWindow = (Taskbar*)GetWindowLongPtr(wnd, GWLP_USERDATA);
	}
	if (targetWindow)
		return targetWindow->WndProc(msg, wParam, lParam);
	return DefWindowProc(wnd, msg, wParam, lParam);
}

LRESULT Taskbar::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
		case APPBAR_CALLBACK:
			taskbarCallback(wnd, wParam, lParam);
			break;

		case WM_CREATE:
			break;

		case WM_NCHITTEST:
			{
				// uint32_t xPos = GET_X_LPARAM(lParam);// - this->posX;
				// uint32_t yPos = GET_Y_LPARAM(lParam);// - this->posY;
				// std::cout << "Hittest (" << xPos << " | " << yPos << ")\n";
				// if(xPos >= this->width)
				// 	return HTNOWHERE;
				// if(yPos >= this->height)
				// 	return HTNOWHERE;

				// if(yPos > 100 && yPos < 200 && xPos > 100 && xPos < 200)
				// 	return HTNOWHERE;
				
				// if(xPos < 5)
				// 	return HTLEFT;
				// if(xPos >= this->width-5)
				// 	return HTRIGHT;
				// if(yPos < CAPTION_HEIGHT)
				// 	return HTCAPTION;
			}
			break;

		case WM_NCLBUTTONDOWN: //WM_NCLBUTTONUP
			// switch(wParam) {
			// 	case HTMINBUTTON:
			// 		ShowWindow(this->wnd, SW_MINIMIZE);
			// 		UpdateWindow(this->wnd);
			// 		break;

			// 	case HTCLOSE:
			// 		PostQuitMessage(0);
        	// 		return 0;
			// }
			break;

		case WM_HOTKEY:
			std::cout << "Hotkey Pressed: " << wParam << "\n";
			// SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLong(wnd, GWL_EXSTYLE) ^ WS_EX_TRANSPARENT); // hit test
			// SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLong(wnd, GWL_EXSTYLE) ^ WS_EX_TOOLWINDOW); // taskbar entry
			// SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) ^ WS_SYSMENU); // Close Button
			// SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) ^ WS_CAPTION); // Title
			// SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) ^ WS_THICKFRAME);
			// SetWindowLong(wnd, GWL_STYLE, GetWindowLong(wnd, GWL_STYLE) ^ WS_POPUP);
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			SetCapture(wnd);
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			ReleaseCapture();
			break;

		// case WM_MOUSE:
		case WM_MOUSEMOVE:
			this->mouseX = GET_X_LPARAM(lParam);
			this->mouseY = GET_Y_LPARAM(lParam);
			break;

		case WM_SIZING:
			// {
			// 	RECT& sizeRect = *(RECT*)lParam;
			// }
			// return false;
			break;

		case WM_SIZE:
			{
				RECT winRect;
				GetClientRect(wnd, &winRect);
				this->resize(winRect.right-winRect.left, winRect.bottom-winRect.top);
			}
			return 0;

		// case WM_WINDOWPOSCHANGING:
		// 	{
		// 		RECT winRect;
		// 		GetWindowRect(wnd, &winRect);
		// 		this->posX = winRect.left;
		// 		this->posY = winRect.top;
		// 	}
		// 	break;

        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(wnd, msg, wParam, lParam);
}

void Taskbar::taskbarCallback(HWND hwndAccessBar, UINT uNotifyMsg, LPARAM lParam) {
    switch (uNotifyMsg) { 
        case ABN_STATECHANGE:
			std::cout << "ABN_STATECHANGE\n";
            // Check to see if the taskbar's always-on-top state has changed and, if it has, change the appbar's state accordingly.
            // uState = SHAppBarMessage(ABM_GETSTATE, &appbarData); 

            // SetWindowPos(hwndAccessBar, 
            //              (ABS_ALWAYSONTOP & uState) ? HWND_TOPMOST : HWND_BOTTOM, 
            //              0, 0, 0, 0, 
            //              SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            break;

        case ABN_FULLSCREENAPP:
				std::cout << "ABN_FULLSCREENAPP\n";
				// A full-screen application has started, or the last full-screen application has closed. Set the appbar's z-order appropriately.
				if (lParam) // application entered Fullscreen
					SetWindowPos(hwndAccessBar, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				else // application exited Fullscreen
					SetWindowPos(hwndAccessBar, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				appBarPosChanged();
			break;

        case ABN_POSCHANGED:
			std::cout << "ABN_POSCHANGED\n";// The taskbar or another appbar has changed its size or position.
            appBarPosChanged();
            break;

		default:
			std::cout << "UNHANDLED APPBAR MESSAGE: " << uNotifyMsg << "\n";
            break;
    } 
}

/* ----------------------
 *     Message Loop End
 * ---------------------   */

// setPos - sets the size and position of an appbar.

// uEdge - screen edge to which the appbar is to be anchored
// lprc - current bounding rectangle of the appbar
// pabd - address of the APPBARDATA structure with the hWnd and cbSize members filled

void Taskbar::setPos(LPRECT lprc) {
    // int iHeight = 0;
    // int iWidth = 0;

    this->appbarData.rc = *lprc;

    // Copy the screen coordinates of the appbar's bounding rectangle into the APPBARDATA structure. 
    // if ((this->appbarData.uEdge == ABE_TOP) || (this->appbarData.uEdge == ABE_BOTTOM)) {
    //     iHeight = this->appbarData.rc.bottom - this->appbarData.rc.top;
    //     this->appbarData.rc.left = 0;
    //     this->appbarData.rc.right = GetSystemMetrics(SM_CXSCREEN);
    // } else {
    //     iWidth = this->appbarData.rc.right - this->appbarData.rc.left;
    //     this->appbarData.rc.top = 0;
    //     this->appbarData.rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    // }
	this->appbarData.rc.left = 0;
	this->appbarData.rc.right = GetSystemMetrics(SM_CXSCREEN);

    // Query the system for an approved size and position.
    SHAppBarMessage(ABM_QUERYPOS, &this->appbarData);

    // Adjust the rectangle, depending on the edge to which the appbar is anchored.
    switch (appbarData.uEdge) {
        case ABE_TOP:
            this->appbarData.rc.top = 0; // my code
            this->appbarData.rc.bottom = this->appbarData.rc.top + this->height;
            break;

        case ABE_BOTTOM:
            this->appbarData.rc.bottom = GetSystemMetrics(SM_CYSCREEN); // my code
            this->appbarData.rc.top = this->appbarData.rc.bottom - this->height;
            break;
    }
    
    SHAppBarMessage(ABM_SETPOS, &this->appbarData); // Pass the final bounding rectangle to the system.

    // Move and size the appbar so that it conforms to the 
    // bounding rectangle passed to the system.
    MoveWindow(this->appbarData.hWnd,
               GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2,
               GetSystemMetrics(SM_CYSCREEN) - this->height,
               this->width,
               this->height,
               true);

	// MoveWindow(this->appbarData.hWnd,
    //            this->appbarData.rc.left,
    //            this->appbarData.rc.top,
    //            this->appbarData.rc.right - this->appbarData.rc.left,
    //            this->appbarData.rc.bottom - this->appbarData.rc.top,
    //            true);
}

// AppBarPosChanged - adjusts the appbar's size and position.

// pabd - address of an APPBARDATA structure that contains information
//        used to adjust the size and position.

void Taskbar::appBarPosChanged() {
    RECT rc;
    RECT rcWindow; 
    int iHeight; 
    int iWidth; 

    rc.top = 0; 
    rc.left = 0; 
    rc.right = GetSystemMetrics(SM_CXSCREEN); 
    rc.bottom = GetSystemMetrics(SM_CYSCREEN); 

    GetWindowRect(this->wnd, &rcWindow);

    iHeight = rcWindow.bottom - rcWindow.top; 
    iWidth = rcWindow.right - rcWindow.left; 

    switch (this->appbarData.uEdge) {
        case ABE_TOP:
            rc.bottom = rc.top + iHeight; 
            break;

        case ABE_BOTTOM: 
            rc.top = rc.bottom - iHeight; 
            break;

        case ABE_LEFT: 
            rc.right = rc.left + iWidth; 
            break;

        case ABE_RIGHT: 
            rc.left = rc.right - iWidth; 
            break;
    }

    this->setPos(&rc); 
}



void Taskbar::blitTexture(const Texture& tex) {
	HBITMAP hbmp = CreateBitmap(tex.width, tex.height, 1, 32, tex.buffer);
	HDC hdcMem = CreateCompatibleDC(this->device_context);
	HBITMAP prevBMP = SelectBitmap(hdcMem, hbmp);
	SIZE size{(long)width, (long)height};
	POINT relPos{0, 0};
	BLENDFUNCTION pBlend = {0};
	pBlend.BlendOp = AC_SRC_OVER;
	pBlend.BlendFlags = 0;
	pBlend.SourceConstantAlpha = 255;
  	pBlend.AlphaFormat = AC_SRC_ALPHA;
	UpdateLayeredWindow(
					this->wnd, // dest window
					this->device_context, // dest hdc
					NULL, //NULL, // POINT* new screen pos
					&size, // SIZE* new screen size
					hdcMem, // source hdc
					&relPos, // POINT* location of layer in device context
					0, // COLORREF color key
					&pBlend, // BLENDFUNCTION* blendfunc
					ULW_ALPHA // dwFlags
	);
	// BitBlt(
	// 		this->device_context, // destination device context
	// 		0, 0, // xDest, yDest (upper left corner of dest rect)
	// 		width, height, // width, height of dest rect
	// 		hdcMem, // source device context
	// 		0, 0, // xSrc, ySrc (upper left corner of source rect)
	// 		SRCCOPY // raster operation code
	// 		);
	SelectObject(hdcMem, prevBMP);
	DeleteObject(hbmp);
	DeleteDC(hdcMem);

    // SetDIBitsToDevice(
    //     this->device_context, // destination device context
    //     0, 0, // xDest, yDest (upper left corner of dest rect)
    //     this->width, this->height, // width, height
    //     0, 0, // xSrc, ySrc (lower left corner of source rect)
    //     0, // startScan
    //     this->height, // cLines
    //     tex.buffer, // buffer
    //     &tex.bit_map_info,
    //     DIB_RGB_COLORS
    //     );
}

void Taskbar::updateScreen() {
	// this->graphics.fillRect(0, 0, this->width, CAPTION_HEIGHT, 0x66AABBEE);
	// this->graphics.fillRect(100, 100, 200, 200, 0x01000000);
	// this->graphics.fillRect(10, 10 + CAPTION_HEIGHT, this->width - 10, this->height - 10, 0x00000000);
    this->blitTexture(this->graphics);
}

void Taskbar::resize(uint32_t width, uint32_t height) {
	this->width = width;
	this->height = height;
	this->graphics.resize(width, height);
}