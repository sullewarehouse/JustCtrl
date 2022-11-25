
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
