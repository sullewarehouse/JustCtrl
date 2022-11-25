
#include <Windows.h>
#include <CommCtrl.h>

// Version # 1.0

#define JUSTCTRL_VERSION                   10

// DPI for the application.

#define JUSTCTRL_APPLICATION_DPI           96

// Center flag options.

#define JUSTCTRL_CENTER_WINDOW              0x10000
#define JUSTCTRL_CENTER_CLIENT              0x30000
#define JUSTCTRL_CENTER_OVER_WORKAREA       0x40000
#define JUSTCTRL_CENTER_WITH_OWNER          0x80000

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

// JustCtrl Functions

bool WINAPI JustCtrl_Init(HINSTANCE hInstance, bool report);
UINT WINAPI JustCtrl_GetDpiForWindow(HWND hWnd);
bool WINAPI JustCtrl_CenterWindow(HWND hWnd, ULONG centerFlags);
