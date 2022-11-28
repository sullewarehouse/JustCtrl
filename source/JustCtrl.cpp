
//
// JustCtrl.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The primary source file for JustCtrl. Contains initialize, window and DPI helper functions.
//

#include "JustCtrl.h"

// Common controls dependencies.

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// hInstance

static HINSTANCE JustCtrl_hInstance;

// DPI function pointers.

static GetDpiForMonitor_proc pGetDpiForMonitor;
static GetDpiForSystem_proc pGetDpiForSystem;
static GetDpiForWindow_proc pGetDpiForWindow;
static SetProcessDPIAware_proc pSetProcessDPIAware;
static SetProcessDpiAwareness_proc pSetProcessDpiAwareness;
static SetProcessDpiAwarenessContext_proc pSetProcessDpiAwarenessContext;
static EnableNonClientDpiScaling_proc pEnableNonClientDpiScaling;
static AdjustWindowRectExForDpi_proc pAdjustWindowRectExForDpi;

bool WINAPI JustCtrl_Init(HINSTANCE hInstance, bool report)
{
	INITCOMMONCONTROLSEX ICC;
	HMODULE hMod_user32, hMod_shcore;

	JustCtrl_hInstance = hInstance;

	// Init DPI Awareness

	pGetDpiForSystem = NULL;
	pGetDpiForWindow = NULL;
	pSetProcessDPIAware = NULL;
	pSetProcessDpiAwareness = NULL;
	pSetProcessDpiAwarenessContext = NULL;
	pEnableNonClientDpiScaling = NULL;
	pAdjustWindowRectExForDpi = NULL;
	pGetDpiForMonitor = NULL;

	// Try to use SetProcessDpiAwarenessContext (Windows 10)

	hMod_user32 = GetModuleHandle(L"user32.dll");
	if (hMod_user32 == NULL)
	{
		hMod_user32 = LoadLibrary(L"user32.dll");
	}

	hMod_shcore = GetModuleHandle(L"SHCore.dll");
	if (hMod_shcore == NULL)
	{
		hMod_shcore = LoadLibrary(L"SHCore.dll");
	}

	if (hMod_user32 != NULL)
	{
		pSetProcessDpiAwarenessContext = (SetProcessDpiAwarenessContext_proc)GetProcAddress(hMod_user32, "SetProcessDpiAwarenessContext");
		if (pSetProcessDpiAwarenessContext != NULL)
		{
			if (pSetProcessDpiAwarenessContext(GUI_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) goto DpiAwarenessSet;
			if (pSetProcessDpiAwarenessContext(GUI_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) goto DpiAwarenessSet;
			if (pSetProcessDpiAwarenessContext(GUI_DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)) goto DpiAwarenessSet;
		}
	}

	// Fallback, and try to use SetProcessDpiAwareness (Windows 8.1)

	if (hMod_shcore != NULL)
	{
		pSetProcessDpiAwareness = (SetProcessDpiAwareness_proc)GetProcAddress(hMod_shcore, "SetProcessDpiAwareness");
		if (pSetProcessDpiAwareness != NULL)
		{
			if (pSetProcessDpiAwareness(GUI_PROCESS_PER_MONITOR_DPI_AWARE) != E_INVALIDARG) goto DpiAwarenessSet;
			if (pSetProcessDpiAwareness(GUI_PROCESS_SYSTEM_DPI_AWARE) != E_INVALIDARG) goto DpiAwarenessSet;
		}
	}

	// Fallback, and try to use SetProcessDPIAware (Windows Vista)

	if (hMod_user32 != NULL)
	{
		pSetProcessDPIAware = (SetProcessDPIAware_proc)GetProcAddress(hMod_user32, "SetProcessDPIAware");
		if (pSetProcessDPIAware != NULL)
		{
			pSetProcessDPIAware();
		}
	}

DpiAwarenessSet:

	if (hMod_shcore != NULL)
	{
		pGetDpiForMonitor = (GetDpiForMonitor_proc)GetProcAddress(hMod_shcore, "GetDpiForMonitor");
	}

	if (hMod_user32 != NULL)
	{
		pGetDpiForSystem = (GetDpiForSystem_proc)GetProcAddress(hMod_user32, "GetDpiForSystem");
		pGetDpiForWindow = (GetDpiForWindow_proc)GetProcAddress(hMod_user32, "GetDpiForWindow");
		pEnableNonClientDpiScaling = (EnableNonClientDpiScaling_proc)GetProcAddress(hMod_user32, "EnableNonClientDpiScaling");
		pAdjustWindowRectExForDpi = (AdjustWindowRectExForDpi_proc)GetProcAddress(hMod_user32, "AdjustWindowRectExForDpi");
	}

	// Init common controls.

	ICC.dwSize = sizeof(INITCOMMONCONTROLSEX);
	ICC.dwICC = ICC_STANDARD_CLASSES | ICC_INTERNET_CLASSES;

	if (!InitCommonControlsEx(&ICC))
	{
		if (report)
			MessageBox(NULL, L"Failed to register the common control classes.", L"Error!", MB_OK);
	}

	if (!JustCtrl_InitCheckbox(hInstance))
	{

	}

	if (!JustCtrl_InitLabel(hInstance))
	{
	}

	if (!JustCtrl_InitRadioButton(hInstance))
	{

	}

	return true;
}

UINT WINAPI JustCtrl_GetDpiForWindow(HWND hWnd)
{
	UINT monitorDpi;

	monitorDpi = 0;

	if (pGetDpiForWindow)
		monitorDpi = pGetDpiForWindow(hWnd);
	else
	{
		monitorDpi = JUSTCTRL_APPLICATION_DPI;
	}

	return monitorDpi;
}

UINT WINAPI JustCtrl_GetDpiForMonitor(HMONITOR hMonitor)
{
	MONITORINFO mInfo;
	UINT x_dpi, y_dpi;
	double dxDpi, dyDpi;
	UINT monitorDpi;

	x_dpi = y_dpi = 0;
	monitorDpi = 0;

	if (pGetDpiForMonitor)
	{
		if (pGetDpiForMonitor(hMonitor, GUI_MDT_EFFECTIVE_DPI, &x_dpi, &y_dpi) == S_OK)
		{
			monitorDpi = y_dpi;
		}
	}

	if (monitorDpi == 0)
	{
		memset(&mInfo, 0, sizeof(MONITORINFO));
		mInfo.cbSize = sizeof(MONITORINFO);

		if (GetMonitorInfo(hMonitor, &mInfo))
		{
			dxDpi = (((double)mInfo.rcMonitor.right - mInfo.rcMonitor.left) * (double)25.4) / (double)508;
			dyDpi = (((double)mInfo.rcMonitor.bottom - mInfo.rcMonitor.top) * (double)25.4) / (double)285.75;

			monitorDpi = (dxDpi > dyDpi) ? (UINT)dyDpi : (UINT)dxDpi;
		}
	}

	return monitorDpi;
}

double WINAPI JustCtrl_DipsToPixels(double dips, double monitorDpi)
{
	return ((dips * monitorDpi) / JUSTCTRL_APPLICATION_DPI);
}

double WINAPI JustCtrl_PixelsToDips(double pixels, double monitorDpi)
{
	return pixels / (monitorDpi / JUSTCTRL_APPLICATION_DPI);
}

double WINAPI JustCtrl_AlignToDipsReturnPixels(double x, int* pDips, int monitorDpi, int round_mode)
{
	x = JustCtrl_PixelsToDips(x, monitorDpi);

	if (round_mode > 0) x = ceil(x);
	else if (round_mode < 0) x = floor(x);
	else x = round(x);

	if (pDips) *pDips = (int)x;
	x = JustCtrl_DipsToPixels(x, monitorDpi);

	return x;
}

void WINAPI JustCtrl_GetdefaultFont(LOGFONT* lpFont, UINT DPI)
{
	memset(lpFont, 0, sizeof(LOGFONT));
	lpFont->lfWeight = 400;
	lpFont->lfCharSet = 1;
	lpFont->lfQuality = 5;
	wcscpy_s(lpFont->lfFaceName, 32, L"Segoe UI");
	//lpFont->lfHeight = MulDiv(12, DPI, 96);
	lpFont->lfHeight = 11;
}

void WINAPI JustCtrl_ResizeFont(HWND hWnd, UINT monitorDpi, LOGFONT* lpFont)
{
	HFONT hFontOld;
	HFONT hFontNew;
	LOGFONT font;

	hFontOld = GetWindowFont(hWnd);

	if (lpFont != 0)
	{
		memcpy(&font, lpFont, sizeof(LOGFONT));
		font.lfHeight = -MulDiv(lpFont->lfHeight, monitorDpi, JUSTCTRL_APPLICATION_DPI);
		hFontNew = CreateFontIndirect(&font);
		if (hFontNew)
		{
			DeleteObject(hFontOld);
			SendMessage(hWnd, WM_SETFONT, (WPARAM)hFontNew, MAKELPARAM(TRUE, 0));
		}
	}
}

bool WINAPI JustCtrl_SetAnchors(FORM_CTRL* pFormCtrl, int anchors, UINT monitorDpi)
{
	HWND hWndParent;
	RECT clientArea;
	int width, height;

	if (!pFormCtrl)
		return false;

	hWndParent = GetParent(pFormCtrl->hWnd);
	if (!hWndParent)
		return false;

	GetClientRect(hWndParent, &clientArea);
	width = (int)JustCtrl_PixelsToDips((clientArea.right - clientArea.left), monitorDpi);
	height = (int)JustCtrl_PixelsToDips((clientArea.bottom - clientArea.top), monitorDpi);

	pFormCtrl->xOffset = width - pFormCtrl->x;
	pFormCtrl->yOffset = height - pFormCtrl->y;
	pFormCtrl->wOffset = width - (pFormCtrl->x + pFormCtrl->width);
	pFormCtrl->hOffset = height - (pFormCtrl->y + pFormCtrl->height);

	pFormCtrl->anchors = anchors;

	return true;
}

bool WINAPI JustCtrl_CenterWindow(HWND hWnd, ULONG centerFlags)
{
	HMONITOR hMonitor;
	MONITORINFO mInfo;
	RECT WinArea, ClientArea, parentRt, monitorRt;
	DWORD dwExStyle, style;
	LONG monitorWidth, monitorHeight;
	int xLoc, yLoc, thisWidth, thisHeight, parentWidth, parentHeight;
	int xExtra, yExtra, borderWidth, sysMenuBarHeight;
	HWND hWndParent;
	POINT xy;

	dwExStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	style = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);

	if ((centerFlags & JUSTCTRL_CENTER_WINDOW) &&
		(!(centerFlags & JUSTCTRL_CENTER_WITH_OWNER)) &&
		(!(style & WS_CHILD)) &&
		(!(dwExStyle & WS_EX_MDICHILD)))
	{
		hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		if (hMonitor == 0)
			return false;

		memset(&mInfo, 0, sizeof(MONITORINFO));
		mInfo.cbSize = sizeof(MONITORINFO);

		if (!GetMonitorInfo(hMonitor, &mInfo))
			return false;

		if (centerFlags & JUSTCTRL_CENTER_OVER_WORKAREA)
			monitorRt = mInfo.rcWork;
		else
			monitorRt = mInfo.rcMonitor;

		monitorWidth = monitorRt.right - monitorRt.left;
		monitorHeight = monitorRt.bottom - monitorRt.top;

		if ((centerFlags & JUSTCTRL_CENTER_CLIENT) == JUSTCTRL_CENTER_CLIENT)
		{
			GetWindowRect(hWnd, &WinArea);
			GetClientRect(hWnd, &ClientArea);

			xExtra = WinArea.right - WinArea.left - ClientArea.right;
			yExtra = WinArea.bottom - WinArea.top - ClientArea.bottom;
			borderWidth = xExtra / 2;
			sysMenuBarHeight = yExtra - borderWidth;

			xLoc = monitorRt.left + ((monitorWidth - (ClientArea.right - ClientArea.left)) / 2) - borderWidth;
			yLoc = monitorRt.top + ((monitorHeight - (ClientArea.bottom - ClientArea.top)) / 2) - sysMenuBarHeight;
		}
		else
		{
			GetWindowRect(hWnd, &WinArea);

			xLoc = monitorRt.left + ((monitorWidth - (WinArea.right - WinArea.left)) / 2);
			yLoc = monitorRt.top + ((monitorHeight - (WinArea.bottom - WinArea.top)) / 2);
		}

		if (SetWindowPos(hWnd, NULL, xLoc, yLoc, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE))
			return true;
	}
	else if (centerFlags & JUSTCTRL_CENTER_WINDOW) // Center using the parent/owner
	{
		hWndParent = GetParent(hWnd);
		if (hWndParent)
		{
			GetWindowRect(hWndParent, &parentRt);

			if (style & WS_CHILD)
			{
				xy.x = parentRt.left;
				xy.y = parentRt.top;
				ScreenToClient(hWndParent, &xy);
				parentRt.left = xy.x;
				parentRt.top = xy.y;

				xy.x = parentRt.right;
				xy.y = parentRt.bottom;
				ScreenToClient(hWndParent, &xy);
				parentRt.right = xy.x;
				parentRt.bottom = xy.y;
			}
			else
			{
				// If the owner is on a different monitor, move it first for DPI changes to take effect.

				HMONITOR src_hMonitor;
				MONITORINFO src_mInfo;

				hMonitor = MonitorFromWindow(hWndParent, MONITOR_DEFAULTTONEAREST);
				if (hMonitor == 0)
					return false;

				src_hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
				if (src_hMonitor == 0)
					return false;

				memset(&mInfo, 0, sizeof(MONITORINFO));
				mInfo.cbSize = sizeof(MONITORINFO);

				if (!GetMonitorInfo(hMonitor, &mInfo))
					return false;

				memset(&src_mInfo, 0, sizeof(MONITORINFO));
				src_mInfo.cbSize = sizeof(MONITORINFO);

				if (!GetMonitorInfo(src_hMonitor, &src_mInfo))
					return false;

				if ((mInfo.rcMonitor.left != src_mInfo.rcMonitor.left) ||
					(mInfo.rcMonitor.right != src_mInfo.rcMonitor.right) ||
					(mInfo.rcMonitor.top != src_mInfo.rcMonitor.top) ||
					(mInfo.rcMonitor.bottom != src_mInfo.rcMonitor.bottom))
				{
					if (!SetWindowPos(hWnd, NULL, mInfo.rcMonitor.left, mInfo.rcMonitor.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE))
						return false;
				}
			}

			if ((centerFlags & JUSTCTRL_CENTER_CLIENT) == JUSTCTRL_CENTER_CLIENT)
			{
				GetWindowRect(hWnd, &WinArea);
				GetClientRect(hWnd, &ClientArea);

				xExtra = WinArea.right - WinArea.left - ClientArea.right;
				yExtra = WinArea.bottom - WinArea.top - ClientArea.bottom;
				borderWidth = xExtra / 2;
				sysMenuBarHeight = yExtra - borderWidth;
			}
			else
			{
				GetWindowRect(hWnd, &WinArea);

				borderWidth = 0;
				sysMenuBarHeight = 0;
			}

			thisWidth = WinArea.right - WinArea.left;
			thisHeight = WinArea.bottom - WinArea.top;

			parentWidth = parentRt.right - parentRt.left;
			parentHeight = parentRt.bottom - parentRt.top;

			xLoc = parentRt.left + ((parentWidth - thisWidth) / 2) - borderWidth;
			yLoc = parentRt.top + ((parentHeight - thisHeight) / 2) - sysMenuBarHeight;

			if (SetWindowPos(hWnd, NULL, xLoc, yLoc, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE))
				return true;
		}
	}

	return false;
}

bool WINAPI JustCtrl_MoveWindowToMonitor(HWND hWnd, HMONITOR hMonitor)
{
	HMONITOR hOldMonitor;
	MONITORINFOEX desInfoEx, srcInfoEx;
	int xLoc, yLoc;
	UINT old_monitorDpi, monitorDpi;
	RECT winArea;

	memset(&desInfoEx, 0, sizeof(MONITORINFOEX));
	memset(&srcInfoEx, 0, sizeof(MONITORINFOEX));

	desInfoEx.cbSize = sizeof(MONITORINFOEX);
	srcInfoEx.cbSize = sizeof(MONITORINFOEX);

	if (!GetMonitorInfo(hMonitor, &desInfoEx))
		return false;

	hOldMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	if (!GetMonitorInfo(hOldMonitor, &srcInfoEx))
		return false;

	if (!GetWindowRect(hWnd, &winArea))
		return false;

	if ((desInfoEx.rcMonitor.left == srcInfoEx.rcMonitor.left) &&
		(desInfoEx.rcMonitor.right == srcInfoEx.rcMonitor.right) &&
		(desInfoEx.rcMonitor.top == srcInfoEx.rcMonitor.top) &&
		(desInfoEx.rcMonitor.bottom == srcInfoEx.rcMonitor.bottom))
	{
		return true; // Its the same monitor.
	}

	old_monitorDpi = JustCtrl_GetDpiForMonitor(hOldMonitor);
	monitorDpi = JustCtrl_GetDpiForMonitor(hMonitor);

	// Get offsets for the current monitor.

	xLoc = winArea.left - srcInfoEx.rcMonitor.left;
	yLoc = winArea.top - srcInfoEx.rcMonitor.top;

	// Adjust for the DPI of the new monitor.

	xLoc = MulDiv(xLoc, monitorDpi, old_monitorDpi);
	yLoc = MulDiv(yLoc, monitorDpi, old_monitorDpi);

	// Make relative to the new monitor & move the window.

	xLoc = desInfoEx.rcMonitor.left + xLoc;
	yLoc = desInfoEx.rcMonitor.top + yLoc;

	SetWindowPos(hWnd, nullptr, xLoc, yLoc, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	return false;
}

void WINAPI JustCtrl_ResizeControls(HWND hWnd, FORM_CTRL* pControl_Linked_List, UINT ctrl_cnt, int monitorDpi, bool dpiChanged)
{
	FORM_CTRL* pFormCtrl;
	int x, y, width, height;
	RECT parentClientArea;

	HDWP hdwp = BeginDeferWindowPos(ctrl_cnt);

	pFormCtrl = pControl_Linked_List;
	while (pFormCtrl)
	{
		if (dpiChanged)
		{
			JustCtrl_ResizeFont(pFormCtrl->hWnd, monitorDpi, &pFormCtrl->lpFont);
		}

		GetClientRect(hWnd, &parentClientArea);

		if (pFormCtrl->anchors & JUSTCTRL_ANCHOR_LEFT)
			x = MulDiv(pFormCtrl->x, monitorDpi, JUSTCTRL_APPLICATION_DPI);
		else
			x = (parentClientArea.right - parentClientArea.left) - MulDiv(pFormCtrl->xOffset, monitorDpi, JUSTCTRL_APPLICATION_DPI);

		if (pFormCtrl->anchors & JUSTCTRL_ANCHOR_TOP)
			y = MulDiv(pFormCtrl->y, monitorDpi, JUSTCTRL_APPLICATION_DPI);
		else
			y = (parentClientArea.bottom - parentClientArea.top) - MulDiv(pFormCtrl->yOffset, monitorDpi, JUSTCTRL_APPLICATION_DPI);

		if (!(pFormCtrl->anchors & JUSTCTRL_ANCHOR_RIGHT))
			width = MulDiv(pFormCtrl->width, monitorDpi, JUSTCTRL_APPLICATION_DPI);
		else
			width = ((parentClientArea.right - parentClientArea.left) - x) - MulDiv(pFormCtrl->wOffset, monitorDpi, JUSTCTRL_APPLICATION_DPI);

		if (!(pFormCtrl->anchors & JUSTCTRL_ANCHOR_BOTTOM))
			height = MulDiv(pFormCtrl->height, monitorDpi, JUSTCTRL_APPLICATION_DPI);
		else
			height = ((parentClientArea.bottom - parentClientArea.top) - y) - MulDiv(pFormCtrl->hOffset, monitorDpi, JUSTCTRL_APPLICATION_DPI);

		DeferWindowPos(hdwp, pFormCtrl->hWnd, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);

		pFormCtrl = pFormCtrl->next;
	}

	EndDeferWindowPos(hdwp);
}

void WINAPI JustCtrl_WindowResizeHandler(HWND hWnd, FORM_CTRL* pControl_Linked_List, UINT ctrl_cnt, int monitorDpi, bool dpiChanged, RECT* pNewScale)
{
	RECT localScale;

	if (pNewScale == 0)
	{
		GetWindowRect(hWnd, &localScale);
		pNewScale = &localScale;

		SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		if (dpiChanged)
		{
			SetWindowPos(hWnd, nullptr, pNewScale->left, pNewScale->top,
				pNewScale->right - pNewScale->left, pNewScale->bottom - pNewScale->top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	JustCtrl_ResizeControls(hWnd, pControl_Linked_List, ctrl_cnt, monitorDpi, dpiChanged);
}
