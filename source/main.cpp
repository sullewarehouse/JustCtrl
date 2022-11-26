
//
// main.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     This example program demonstrates creating a DPI aware application that uses JustCtrl controls alongside native controls.
//

#include "JustCtrl.h"

class MAIN_WINDOW
{
public:
	UINT DPI;
	static bool Init(WNDPROC WndProc, HINSTANCE hInstance);
	static MAIN_WINDOW* New(HINSTANCE hInstance);
	HINSTANCE hInstance;
	HWND hWnd;
	FORM_CTRL MyCheckbox;
	FORM_CTRL OkBtn;
	FORM_CTRL* control_linked_list;
	UINT control_count;
	bool handleResize;
};

static MAIN_WINDOW* mainWindow;

bool MAIN_WINDOW::Init(WNDPROC WndProc, HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	// Register the Main window class.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindow";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"Failed to register the MAIN_WINDOW class.", L"Error!", MB_OK);
		return false;
	}

	return true;
}

MAIN_WINDOW* MAIN_WINDOW::New(HINSTANCE hInstance)
{
	HWND hWnd;
	MAIN_WINDOW* nwc;
	int x, y;
	int width, height;

	// Create the Main window.

	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", L"JustCtrl Example",
		WS_BORDER | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, 500, 300, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return nullptr;

	nwc = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
	if (!nwc)
		return nullptr;

	// Create a JustCtrl Checkbox.

	nwc->MyCheckbox.x = 8;
	nwc->MyCheckbox.y = 8;
	nwc->MyCheckbox.width = 100;
	nwc->MyCheckbox.height = 15;

	x = MulDiv(nwc->MyCheckbox.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->MyCheckbox.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->MyCheckbox.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->MyCheckbox.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->MyCheckbox.hWnd = CreateWindowEx(0, L"JustCtrl_Checkbox", L"My Checkbox",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_VCENTER,
		x, y, width, height, nwc->hWnd, NULL, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->MyCheckbox, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->MyCheckbox.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->MyCheckbox.hWnd, nwc->DPI, &nwc->MyCheckbox.lpFont);

	// Create a normal native windows button.

	nwc->OkBtn.x = 418;
	nwc->OkBtn.y = 270;
	nwc->OkBtn.width = 74;
	nwc->OkBtn.height = 22;

	x = MulDiv(nwc->OkBtn.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->OkBtn.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->OkBtn.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->OkBtn.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->OkBtn.hWnd = CreateWindowEx(0, WC_BUTTON, L"OK",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,
		x, y, width, height, nwc->hWnd, NULL, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->OkBtn, JUSTCTRL_ANCHOR_BOTTOM | JUSTCTRL_ANCHOR_RIGHT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->OkBtn.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->OkBtn.hWnd, nwc->DPI, &nwc->MyCheckbox.lpFont);

	// Setup the control linked list and count var.

	nwc->control_linked_list = &nwc->MyCheckbox;
	nwc->MyCheckbox.next = &nwc->OkBtn;

	nwc->control_count = 2;

	// Center and show the Main window.

	JustCtrl_CenterWindow(nwc->hWnd, JUSTCTRL_CENTER_CLIENT);
	ShowWindow(nwc->hWnd, SW_SHOWNORMAL);

	return nwc;
}

LRESULT CALLBACK MainWindow_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC WinDC;
	PAINTSTRUCT ps;

	MAIN_WINDOW* pThisWindow = NULL;

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		pThisWindow = (MAIN_WINDOW*)malloc(sizeof(MAIN_WINDOW));
		if (pThisWindow != 0)
		{
			memset(pThisWindow, 0, sizeof(MAIN_WINDOW));

			pThisWindow->hWnd = hWnd;
			pThisWindow->hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			pThisWindow->DPI = JustCtrl_GetDpiForWindow(hWnd);

			SetWindowLongPtr(hWnd, 0, (LONG_PTR)pThisWindow);
		}
		break;
	}
	case WM_DESTROY:
	{
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			free(pThisWindow);
		}

		PostQuitMessage(0);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		if (hWnd == WindowFromDC((HDC)wParam))
		{
			return 1;
		}
		break;
	}
	case WM_PAINT:
	{
		WinDC = BeginPaint(hWnd, &ps);

		HDC bufferDC = CreateCompatibleDC(WinDC);
		HBITMAP hMemoryBmp = CreateCompatibleBitmap(WinDC, ps.rcPaint.right, ps.rcPaint.bottom);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(bufferDC, hMemoryBmp);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		HBRUSH hBrush = (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND);
		FillRect(bufferDC, &clientRect, hBrush);

		BitBlt(WinDC, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, bufferDC, 0, 0, SRCCOPY);

		SelectObject(bufferDC, hOldBmp);
		DeleteObject(hMemoryBmp);
		DeleteDC(bufferDC);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_PRINTCLIENT:
	{
		WinDC = (HDC)wParam;

		// Code goes here ...

		return 0;
	}
	case WM_DPICHANGED:
	{
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			pThisWindow->handleResize = false;
			JustCtrl_WindowResizeHandler(hWnd, pThisWindow->control_linked_list, pThisWindow->control_count, HIWORD(wParam), true, (RECT*)lParam);
			pThisWindow->handleResize = true;
		}

		break;
	}
	case WM_SIZE:
	{
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			if (pThisWindow->handleResize)
			{
				UINT monitorDpi = JustCtrl_GetDpiForWindow(hWnd);
				RECT winArea;
				GetWindowRect(hWnd, &winArea);
				JustCtrl_WindowResizeHandler(hWnd, pThisWindow->control_linked_list, pThisWindow->control_count, monitorDpi, false, &winArea);
			}
		}

		break;
	}
	case WM_CREATE:
	{
		CREATESTRUCT* lpCreateParams = (CREATESTRUCT*)lParam;

		// Set width & height as windows client size.

		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			RECT winArea;
			RECT clientArea;

			GetWindowRect(hWnd, &winArea);
			GetClientRect(hWnd, &clientArea);

			int xExtra = winArea.right - winArea.left - clientArea.right;
			int yExtra = winArea.bottom - winArea.top - clientArea.bottom;

			clientArea.right = clientArea.left + MulDiv(lpCreateParams->cx, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			clientArea.bottom = clientArea.top + MulDiv(lpCreateParams->cy, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI);

			int width = clientArea.right - clientArea.left;
			int height = clientArea.bottom - clientArea.top;

			SetWindowPos(hWnd, NULL, winArea.right, winArea.top,
				width + xExtra, height + yExtra, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

			// ------------------- MONITOR FROM MOUSE ------------------- //

			POINT xy;
			HMONITOR hMonitor;

			GetCursorPos(&xy);
			hMonitor = MonitorFromPoint(xy, MONITOR_DEFAULTTONEAREST);
			JustCtrl_MoveWindowToMonitor(hWnd, hMonitor);

			// ---------------------------------------------------------- //

			pThisWindow->handleResize = true;
		}

		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ WCHAR* pCmdLine, _In_ int nCmdShow)
{
	MSG msg;

	// Init JustCtrl

	if (!JustCtrl_Init(hInstance, true))
	{
		MessageBox(0, L"JustCtrl failed to initialize, exiting the app!", L"Error!", MB_OK);
		return 0;
	}

	// Init window classes.

	if (!MAIN_WINDOW::Init(MainWindow_WndProc, hInstance))
	{
		MessageBox(0, L"The MAIN_WINDOW form failed to initialize, exiting the app!", L"Error!", MB_OK);
		return 0;
	}

	// Create the main window.

	mainWindow = MAIN_WINDOW::New(hInstance);
	if (!mainWindow)
	{
		MessageBox(0, L"Failed to create the MAIN_WINDOW form, exiting the app!", L"Error!", MB_OK);
		return 0;
	}

	// Start main msg loop.

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(mainWindow->hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
