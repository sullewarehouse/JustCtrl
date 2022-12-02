
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
using namespace Gdiplus;

//
// Control ID's
//

#define IDC_CBHIDE                      0x0001
#define IDC_LBCOLOR                     0x0002
#define IDC_RBRED                       0x0003
#define IDC_RBBLUE                      0x0004
#define IDC_LBSHAPE                     0x0005
#define IDC_RBTRIANGLE                  0x0006
#define IDC_RBCIRCLE                    0x0007
#define IDC_OKBTN                       0x0008

class MAIN_WINDOW
{
public:
	UINT DPI;
	static bool Init(WNDPROC WndProc, HINSTANCE hInstance);
	static MAIN_WINDOW* New(HINSTANCE hInstance);
	HINSTANCE hInstance;
	HWND hWnd;
	FORM_CTRL cbHide;
	FORM_CTRL lbColor;
	FORM_CTRL rbRed;
	FORM_CTRL rbBlue;
	FORM_CTRL lbShape;
	FORM_CTRL rbTriangle;
	FORM_CTRL rbCircle;
	FORM_CTRL OkBtn;
	FORM_CTRL* control_linked_list;
	UINT control_count;
	bool handleResize;
};

// Main window instance.
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
	wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
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

	nwc->cbHide.x = 8;
	nwc->cbHide.y = 8;
	nwc->cbHide.width = 100;
	nwc->cbHide.height = 15;

	x = MulDiv(nwc->cbHide.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->cbHide.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->cbHide.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->cbHide.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->cbHide.hWnd = CreateWindowEx(0, L"JustCtrl_Checkbox", L"Hide Shape",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | CHECKBOX_VCENTER | CHECKBOX_AUTO,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_CBHIDE, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->cbHide, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->cbHide.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->cbHide.hWnd, nwc->DPI, &nwc->cbHide.lpFont);

	// Create a JustCtrl Label 'Color:'.

	nwc->lbColor.x = 8;
	nwc->lbColor.y = 31;
	nwc->lbColor.width = 100;
	nwc->lbColor.height = 15;

	x = MulDiv(nwc->lbColor.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->lbColor.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->lbColor.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->lbColor.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->lbColor.hWnd = CreateWindowEx(0, L"JustCtrl_Label", L"Color:",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | LABEL_VCENTER,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_LBCOLOR, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->lbColor, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->lbColor.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->lbColor.hWnd, nwc->DPI, &nwc->lbColor.lpFont);

	// Create 2 JustCtrl RadioButtons (Red, Blue).

	nwc->rbRed.x = 8;
	nwc->rbRed.y = 54;
	nwc->rbRed.width = 100;
	nwc->rbRed.height = 15;

	x = MulDiv(nwc->rbRed.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->rbRed.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->rbRed.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->rbRed.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->rbRed.hWnd = CreateWindowEx(0, L"JustCtrl_RadioButton", L"Red",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP | RADIOBUTTON_VCENTER | RADIOBUTTON_AUTO,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_RBRED, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->rbRed, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->rbRed.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->rbRed.hWnd, nwc->DPI, &nwc->rbRed.lpFont);

	RadioButton_SetCheck(nwc->rbRed.hWnd, RADIOBUTTON_CHECKED, true);

	// ---- Blue ---- //

	nwc->rbBlue.x = 8;
	nwc->rbBlue.y = 77;
	nwc->rbBlue.width = 100;
	nwc->rbBlue.height = 15;

	x = MulDiv(nwc->rbBlue.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->rbBlue.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->rbBlue.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->rbBlue.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->rbBlue.hWnd = CreateWindowEx(0, L"JustCtrl_RadioButton", L"Blue",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | RADIOBUTTON_VCENTER | RADIOBUTTON_AUTO,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_RBBLUE, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->rbBlue, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->rbBlue.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->rbBlue.hWnd, nwc->DPI, &nwc->rbBlue.lpFont);

	// Create a JustCtrl Label 'Shape:'.

	nwc->lbShape.x = 8;
	nwc->lbShape.y = 100;
	nwc->lbShape.width = 100;
	nwc->lbShape.height = 15;

	x = MulDiv(nwc->lbShape.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->lbShape.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->lbShape.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->lbShape.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->lbShape.hWnd = CreateWindowEx(0, L"JustCtrl_Label", L"Shape:",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | LABEL_VCENTER,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_LBSHAPE, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->lbShape, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->lbShape.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->lbShape.hWnd, nwc->DPI, &nwc->lbShape.lpFont);

	// Create 2 JustCtrl RadioButtons (Triangle, Circle).

	nwc->rbTriangle.x = 8;
	nwc->rbTriangle.y = 123;
	nwc->rbTriangle.width = 100;
	nwc->rbTriangle.height = 15;

	x = MulDiv(nwc->rbTriangle.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->rbTriangle.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->rbTriangle.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->rbTriangle.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->rbTriangle.hWnd = CreateWindowEx(0, L"JustCtrl_RadioButton", L"Triangle",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP | RADIOBUTTON_VCENTER | RADIOBUTTON_AUTO,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_RBTRIANGLE, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->rbTriangle, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->rbTriangle.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->rbTriangle.hWnd, nwc->DPI, &nwc->rbTriangle.lpFont);

	// ---- Circle ---- //

	nwc->rbCircle.x = 8;
	nwc->rbCircle.y = 146;
	nwc->rbCircle.width = 100;
	nwc->rbCircle.height = 15;

	x = MulDiv(nwc->rbCircle.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->rbCircle.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->rbCircle.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->rbCircle.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->rbCircle.hWnd = CreateWindowEx(0, L"JustCtrl_RadioButton", L"Circle",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | RADIOBUTTON_VCENTER | RADIOBUTTON_AUTO,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_RBCIRCLE, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->rbCircle, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->rbCircle.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->rbCircle.hWnd, nwc->DPI, &nwc->rbCircle.lpFont);

	RadioButton_SetCheck(nwc->rbCircle.hWnd, RADIOBUTTON_CHECKED, true);

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
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_OKBTN, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->OkBtn, JUSTCTRL_ANCHOR_BOTTOM | JUSTCTRL_ANCHOR_RIGHT, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->OkBtn.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->OkBtn.hWnd, nwc->DPI, &nwc->OkBtn.lpFont);

	// Setup the control linked list and count var.

	nwc->control_linked_list = &nwc->cbHide;
	nwc->cbHide.next = &nwc->lbColor;
	nwc->lbColor.next = &nwc->rbRed;
	nwc->rbRed.next = &nwc->rbBlue;
	nwc->rbBlue.next = &nwc->lbShape;
	nwc->lbShape.next = &nwc->rbTriangle;
	nwc->rbTriangle.next = &nwc->rbCircle;
	nwc->rbCircle.next = &nwc->OkBtn;

	nwc->control_count = 8;

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

		// ** Get window class pointer ** //
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			// *** GDI Plus drawing *** //

			Graphics graphics(bufferDC);
			Color lineColor;

			if (Checkbox_GetCheck(pThisWindow->cbHide.hWnd) != CHECKBOX_CHECKED)
			{
				if (RadioButton_GetCheck(pThisWindow->rbRed.hWnd) == CHECKBOX_CHECKED)
					lineColor = Gdiplus::Color(255, 255, 0, 0);
				else if (RadioButton_GetCheck(pThisWindow->rbBlue.hWnd) == CHECKBOX_CHECKED)
					lineColor = Gdiplus::Color(255, 0, 0, 255);

				Pen GdiPen(lineColor, (Gdiplus::REAL)MulDiv(2, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI));
				GdiPen.SetAlignment(PenAlignmentCenter);
				graphics.SetSmoothingMode(SmoothingModeAntiAlias);

				if (RadioButton_GetCheck(pThisWindow->rbTriangle.hWnd) == CHECKBOX_CHECKED)
				{
					Point drawingPoints[] = {
						Point(MulDiv(124, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI), MulDiv(20 + 250, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI)),
						Point(MulDiv(124 + 125, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI), MulDiv(20, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI)),
						Point(MulDiv(124 + 250, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI), MulDiv(20 + 250, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI)) };

					graphics.DrawPolygon(&GdiPen, drawingPoints, 3);
				}
				else if (RadioButton_GetCheck(pThisWindow->rbCircle.hWnd) == CHECKBOX_CHECKED)
				{
					Rect DrawingRect = Rect(
						MulDiv(124, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI),
						MulDiv(20, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI),
						MulDiv(250, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI),
						MulDiv(250, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI));

					graphics.DrawEllipse(&GdiPen, DrawingRect);
				}
			}

			// *** GDI Plus Done! *** //
		}

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
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		// HWND ctrl_hWnd = (HWND)lParam; // Not needed for this example.

		switch (wmId)
		{
		case IDC_CBHIDE:
		case IDC_RBRED:
		case IDC_RBBLUE:
		case IDC_RBTRIANGLE:
		case IDC_RBCIRCLE:
		{
			if ((wmEvent == CHECKBOX_CLICKED) || (RADIOBUTTON_CLICKED))
			{
				// Tell the main window to redraw itself!
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		default:
			break;
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

	// Init custom window classes.

	if (!MAIN_WINDOW::Init(MainWindow_WndProc, hInstance))
	{
		MessageBox(0, L"The MAIN_WINDOW form failed to initialize, exiting the app!", L"Error!", MB_OK);
		return 0;
	}

	// Init GDI Plus.

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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
