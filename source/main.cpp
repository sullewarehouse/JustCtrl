
#include "JustCtrl.h"

class MAIN_WINDOW
{
public:
	UINT DPI;
	static bool Init(WNDPROC WndProc, HINSTANCE hInstance);
	static MAIN_WINDOW* New(HINSTANCE hInstance);
	HWND hWnd;
};

static MAIN_WINDOW* mainWindow;

bool MAIN_WINDOW::Init(WNDPROC WndProc, HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	// Register the window class.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
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
	MAIN_WINDOW* nwc;

	nwc = (MAIN_WINDOW*)malloc(sizeof(MAIN_WINDOW));
	if (nwc == nullptr)
		return nullptr;

	// Create the Frame window.

	nwc->hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", L"JustCtrl",
		WS_BORDER | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, 1000, 600, NULL, NULL, hInstance, NULL);

	if (!nwc->hWnd)
		return nullptr;

	nwc->DPI = JustCtrl_GetDpiForWindow(nwc->hWnd);
	JustCtrl_CenterWindow(nwc->hWnd, JUSTCTRL_CENTER_CLIENT);

	ShowWindow(nwc->hWnd, SW_SHOWNORMAL);

	return nwc;
}

LRESULT CALLBACK MainWindow_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC WinDC;
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
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

		RECT Temp;
		GetClientRect(hWnd, &Temp);
		HBRUSH hBrush = (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND);
		FillRect(bufferDC, &Temp, hBrush);

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
