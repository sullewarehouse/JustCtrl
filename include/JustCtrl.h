
//
// JustCtrl.h
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The primary header file for JustCtrl. Include this in your project source files.
//

#ifndef JUSTCTRL_H
#define JUSTCTRL_H

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <ShlObj.h>
#include <math.h>
#include <Uxtheme.h>
#include <vssym32.h>
#include <wingdi.h>
#include <objidl.h>
#include <gdiplus.h>

#include "checkbox.h"
#include "label.h"
#include "radiobutton.h"

#pragma comment (lib, "Shell32.lib")
#pragma comment (lib, "UxTheme.lib")
#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "Msimg32.lib")

// Version # 1.0

#define JUSTCTRL_VERSION                    10

// DPI for the application.

#define JUSTCTRL_APPLICATION_DPI            96

// Center flag options.

#define JUSTCTRL_CENTER_WINDOW              0x10000
#define JUSTCTRL_CENTER_CLIENT              0x30000
#define JUSTCTRL_CENTER_OVER_WORKAREA       0x40000
#define JUSTCTRL_CENTER_WITH_OWNER          0x80000

// Anchor flag options.

#define JUSTCTRL_ANCHOR_TOP                 1
#define JUSTCTRL_ANCHOR_BOTTOM              2
#define JUSTCTRL_ANCHOR_LEFT                4
#define JUSTCTRL_ANCHOR_RIGHT               8

// define WM_DPICHANGED when it doesn't exist.

#ifndef WM_DPICHANGED
#define WM_DPICHANGED  0x02E0
#endif

// define GUI_PROCESS_DPI_AWARENESS for when PROCESS_DPI_AWARENESS doesn't exist.

typedef enum GUI_PROCESS_DPI_AWARENESS {
	GUI_PROCESS_DPI_UNAWARE = 0,
	GUI_PROCESS_SYSTEM_DPI_AWARE = 1,
	GUI_PROCESS_PER_MONITOR_DPI_AWARE = 2
} GUI_PROCESS_DPI_AWARENESS;

// define GUI_MONITOR_DPI_TYPE for when MONITOR_DPI_TYPE doesn't exist.

typedef enum GUI_MONITOR_DPI_TYPE {
	GUI_MDT_EFFECTIVE_DPI = 0,
	GUI_MDT_ANGULAR_DPI = 1,
	GUI_MDT_RAW_DPI = 2,
	GUI_MDT_DEFAULT
} GUI_MONITOR_DPI_TYPE;

// define GUI_DPI_AWARENESS_CONTEXT for when DPI_AWARENESS_CONTEXT doesn't exist.

DECLARE_HANDLE(GUI_DPI_AWARENESS_CONTEXT);

#define GUI_DPI_AWARENESS_CONTEXT_SYSTEM_AWARE          ((GUI_DPI_AWARENESS_CONTEXT)-2)
#define GUI_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE     ((GUI_DPI_AWARENESS_CONTEXT)-3)
#define GUI_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2  ((GUI_DPI_AWARENESS_CONTEXT)-4)

// DPI function pointer types.

typedef HRESULT(WINAPI* GetDpiForMonitor_proc)(HMONITOR hmonitor, GUI_MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
typedef UINT(WINAPI* GetDpiForSystem_proc)();
typedef UINT(WINAPI* GetDpiForWindow_proc)(HWND hWnd);
typedef BOOL(WINAPI* SetProcessDPIAware_proc)();
typedef HRESULT(WINAPI* SetProcessDpiAwareness_proc)(GUI_PROCESS_DPI_AWARENESS value);
typedef BOOL(WINAPI* SetProcessDpiAwarenessContext_proc)(GUI_DPI_AWARENESS_CONTEXT value);
typedef BOOL(WINAPI* EnableNonClientDpiScaling_proc)(HWND hWnd);
typedef BOOL(WINAPI* AdjustWindowRectExForDpi_proc)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);

// CTLCOLOR message struct.

struct JUSTCTRL_CTLCOLOR
{
	NMHDR nmh;
	HDC hDC;
	HBRUSH hBrush;
	bool bSet;
};

// Common control class.

typedef class _FORM_CTRL FORM_CTRL;
class _FORM_CTRL
{
public:
	HWND hWnd;
	int x;
	int y;
	int width;
	int height;
	int anchors;
	int xOffset;
	int yOffset;
	int wOffset;
	int hOffset;
	LOGFONT lpFont;
	FORM_CTRL* next;
};

// JustCtrl Functions.

bool WINAPI JustCtrl_Init(HINSTANCE hInstance, bool report);
UINT WINAPI JustCtrl_GetDpiForWindow(HWND hWnd);
UINT WINAPI JustCtrl_GetDpiForMonitor(HMONITOR hMonitor);
double WINAPI JustCtrl_DipsToPixels(double dips, double monitorDpi);
double WINAPI JustCtrl_PixelsToDips(double pixels, double monitorDpi);
double WINAPI JustCtrl_AlignToDipsReturnPixels(double x, int* pDips, int monitorDpi, int round_mode);
void WINAPI JustCtrl_GetdefaultFont(LOGFONT* lpFont, UINT DPI);
void WINAPI JustCtrl_ResizeFont(HWND hWnd, UINT monitorDpi, LOGFONT* lpFont);
bool WINAPI JustCtrl_SetAnchors(FORM_CTRL* pFormCtrl, int anchors, UINT monitorDpi);
bool WINAPI JustCtrl_CenterWindow(HWND hWnd, ULONG centerFlags);
bool WINAPI JustCtrl_MoveWindowToMonitor(HWND hWnd, HMONITOR hMonitor);
void WINAPI JustCtrl_WindowResizeHandler(HWND hWnd, FORM_CTRL* pControl_Linked_List, UINT ctrl_cnt, int monitorDpi, bool dpiChanged, RECT* pNewScale);

#endif // !JUSTCTRL_H
