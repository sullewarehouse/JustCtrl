
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

#define IDC_TABWINDOW                   0x0001
#define IDC_OKBTN                       0x0009

class MAIN_WINDOW
{
public:
	UINT DPI;
	static bool Init(WNDPROC WndProc, HINSTANCE hInstance);
	static MAIN_WINDOW* New(HINSTANCE hInstance);
	HINSTANCE hInstance;
	HWND hWnd;
	HBITMAP bitmapBuffer;
	FORM_CTRL tabWindow;
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
	wcex.lpszClassName = L"MAIN_WINDOW";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"Failed to register the MAIN_WINDOW class.", L"Error!", MB_OK);
		return false;
	}

	return true;
}

void GetRoundRectPath(GraphicsPath* pPath, Rect r, int dia)
{
	// diameter can't exceed width or height
	if (dia > r.Width)    dia = r.Width;
	if (dia > r.Height)    dia = r.Height;

	// define a corner 
	Rect Corner(r.X, r.Y, dia, dia);

	// begin path
	pPath->Reset();

	// top left
	pPath->AddArc(Corner, 180, 90);

	// tweak needed for radius of 10 (dia of 20)
	if (dia == 20)
	{
		Corner.Width += 1;
		Corner.Height += 1;
		r.Width -= 1; r.Height -= 1;
	}

	// top right
	Corner.X += (r.Width - dia - 1);
	pPath->AddArc(Corner, 270, 90);

	// bottom right
	Corner.Y += (r.Height - dia - 1);
	pPath->AddArc(Corner, 0, 90);

	// bottom left
	Corner.X -= (r.Width - dia - 1);
	pPath->AddArc(Corner, 90, 90);

	// end path
	pPath->CloseFigure();
}

void FillRoundRect(Graphics* pGraphics, Rect r, Brush* pBrush, Color border, int radius, int width)
{
	int dia = 2 * radius;

	// set to pixel mode
	int oldPageUnit = pGraphics->SetPageUnit(UnitPixel);

	// define the pen
	Pen pen(border, 1);
	pen.SetAlignment(PenAlignmentCenter);

	// get the corner path
	GraphicsPath path;

	// get path
	GetRoundRectPath(&path, r, dia);

	// fill
	pGraphics->FillPath(pBrush, &path);

	// draw the border last so it will be on top
	pGraphics->DrawPath(&pen, &path);

	// if width > 1
	for (int i = 1; i < width; i++)
	{
		// left stroke
		r.Inflate(-1, 0);
		// get the path
		GetRoundRectPath(&path, r, dia);

		// draw the round rect
		pGraphics->DrawPath(&pen, &path);

		// up stroke
		r.Inflate(0, -1);

		// get the path
		GetRoundRectPath(&path, r, dia);

		// draw the round rect
		pGraphics->DrawPath(&pen, &path);
	}

	// restore page unit
	pGraphics->SetPageUnit((Unit)oldPageUnit);
}

void DrawCloseBox(HDC WinDC, RECT* pContainer, bool bDisabled, bool bHighLight, bool bPressed, UINT DPI)
{
	int borderThickness;
	Gdiplus::Point pt[3];
	Gdiplus::REAL lineThickness;
	Gdiplus::Color lineColor;
	Gdiplus::Color gdiFillColor;
	Gdiplus::Brush* gdiBrush;
	Gdiplus::Rect btnRt;

	borderThickness = (int)JustCtrl_DipsToPixels(1, DPI);
	lineThickness = (Gdiplus::REAL)JustCtrl_DipsToPixels(1.5F, DPI);

	int innerOffset = (int)JustCtrl_DipsToPixels(6, DPI);
	int innerOffset2 = (int)JustCtrl_DipsToPixels(1, DPI);

	lineColor = Gdiplus::Color(255, 35, 35, 35);

	Graphics graphics(WinDC);
	Pen GdiPen(lineColor, lineThickness);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	if (bHighLight)
	{
		btnRt.X = pContainer->left + innerOffset2;
		btnRt.Y = pContainer->top + innerOffset2;
		btnRt.Width = (pContainer->right - pContainer->left) - (innerOffset2 * 2);
		btnRt.Height = (pContainer->bottom - pContainer->top) - (innerOffset2 * 2);

		gdiFillColor = Gdiplus::Color(255, 204, 204, 204);
		gdiBrush = new Gdiplus::SolidBrush(gdiFillColor);
		FillRoundRect(&graphics, btnRt, gdiBrush, Gdiplus::Color(255, 204, 204, 204), 3, borderThickness);
		delete gdiBrush;
	}

	pt[0].X = pContainer->left + innerOffset;
	pt[0].Y = pContainer->top + innerOffset;
	pt[1].X = pContainer->right - innerOffset;
	pt[1].Y = pContainer->bottom - innerOffset;
	graphics.DrawLines(&GdiPen, pt, 2);

	pt[0].X = pContainer->right - innerOffset;
	pt[0].Y = pContainer->top + innerOffset;
	pt[1].X = pContainer->left + innerOffset;
	pt[1].Y = pContainer->bottom - innerOffset;
	graphics.DrawLines(&GdiPen, pt, 2);
}

void TabButtonEventHandler(HWND hWnd, ULONG tabIndex, ULONG btnIndex, ULONG code, WPARAM wParam)
{
	switch (code)
	{
	case TABWINDOW_BT_PAINT:
	{
		RECT drawRt;

		SendMessage(hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(tabIndex, TABWINDOW_IR_BUTTON_BOUNDS + btnIndex), (LPARAM)&drawRt);

		UINT DPI = JustCtrl_GetDpiForWindow(hWnd);

		DrawCloseBox((HDC)wParam, &drawRt, false, false, false, DPI);

		return;
	}
	case TABWINDOW_BT_LCLICK:
	{
		if (btnIndex == 0)
		{
			MessageBox(NULL, L"button 0 pressed", L"OK", MB_OK);
		}
		else if (btnIndex == 1)
		{
			MessageBox(NULL, L"button 1 pressed", L"OK", MB_OK);
		}

		return;
	}
	default:
		break;
	}
}

MAIN_WINDOW* MAIN_WINDOW::New(HINSTANCE hInstance)
{
	HWND hWnd;
	MAIN_WINDOW* nwc;
	int x, y;
	int width, height;

	// Create the Main window.

	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MAIN_WINDOW", L"JustCtrl Example",
		WS_BORDER | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
		0, 0, 500, 300, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return nullptr;

	nwc = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
	if (!nwc)
		return nullptr;

	// Create a JustCtrl TabWindow.

	nwc->tabWindow.x = 8;
	nwc->tabWindow.y = 8;
	nwc->tabWindow.width = 350;
	nwc->tabWindow.height = 284;

	x = MulDiv(nwc->tabWindow.x, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	y = MulDiv(nwc->tabWindow.y, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	width = MulDiv(nwc->tabWindow.width, nwc->DPI, JUSTCTRL_APPLICATION_DPI);
	height = MulDiv(nwc->tabWindow.height, nwc->DPI, JUSTCTRL_APPLICATION_DPI);

	nwc->tabWindow.hWnd = CreateWindowEx(0, L"JustCtrl_TabWindow", NULL,
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | TABWINDOW_BORDER,
		x, y, width, height, nwc->hWnd, (HMENU)IDC_TABWINDOW, hInstance, NULL);

	JustCtrl_SetAnchors(&nwc->tabWindow, JUSTCTRL_ANCHOR_TOP | JUSTCTRL_ANCHOR_LEFT | JUSTCTRL_ANCHOR_RIGHT | JUSTCTRL_ANCHOR_BOTTOM, nwc->DPI);
	JustCtrl_GetdefaultFont(&nwc->tabWindow.lpFont, nwc->DPI);
	JustCtrl_ResizeFont(nwc->tabWindow.hWnd, nwc->DPI, &nwc->tabWindow.lpFont);

	TABWINDOW_BUTTON tabButton = { TabButtonEventHandler, 0, 0 };
	TabWindow_CreateButton(nwc->tabWindow.hWnd, 0, &tabButton);

	TabWindow_InsertTab(nwc->tabWindow.hWnd, -1, L"Drawing");
	TabWindow_InsertTab(nwc->tabWindow.hWnd, -1, L"Math");

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

	nwc->control_linked_list = &nwc->tabWindow;
	nwc->tabWindow.next = &nwc->OkBtn;

	nwc->control_count = 2;

	// Center and show the Main window.

	JustCtrl_CenterWindow(nwc->hWnd, JUSTCTRL_CENTER_CLIENT);
	ShowWindow(nwc->hWnd, SW_SHOWNORMAL);

	return nwc;
}

void WINAPI MAIN_WINDOW_OnPaint(MAIN_WINDOW* pThisWindow, HDC hDC)
{
	RECT clientRect;
	GetClientRect(pThisWindow->hWnd, &clientRect);
	HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
	FillRect(hDC, &clientRect, hBrush);
	DeleteObject(hBrush);
}

LRESULT CALLBACK MAIN_WINDOW_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MAIN_WINDOW* pThisWindow = NULL;

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		// Enable Non-Client DPI scaling
		JustCtrl_EnableNonClientDpiScaling(hWnd);

		// Create the window class pointer
		pThisWindow = (MAIN_WINDOW*)malloc(sizeof(MAIN_WINDOW));
		if (pThisWindow != 0)
		{
			// Initialize the class members to zero
			memset(pThisWindow, 0, sizeof(MAIN_WINDOW));

			// Set the hWnd, hInstance and DPI for the window
			pThisWindow->hWnd = hWnd;
			pThisWindow->hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			pThisWindow->DPI = JustCtrl_GetDpiForWindow(hWnd);

			// Attach the class pointer to this instance
			SetWindowLongPtr(hWnd, 0, (LONG_PTR)pThisWindow);
		}

		break;
	}
	//
	// Sent when a window is being destroyed
	//
	case WM_DESTROY:
	{
		// Get the window class pointer
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			// Free resources
			if (pThisWindow->bitmapBuffer) {
				DeleteObject(pThisWindow->bitmapBuffer);
			}
			free(pThisWindow);
		}

		// Tell the thread to exit the message loop (closes the application)
		PostQuitMessage(0);

		break;
	}
	//
	// Request to erase the background
	// An application should return nonzero if it erases the background
	// In our case, we erase during the WM_PAINT message
	// 
	// Note: Some native child controls send a WM_ERASEBKGND through their parent, so we check who the HDC is from
	//
	case WM_ERASEBKGND:
	{
		// Check who the HDC is from
		if (hWnd == WindowFromDC((HDC)wParam))
		{
			return 1;
		}
		break;
	}
	//
	// Request to draw the client area
	//
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		// Start paint process
		HDC winDC = BeginPaint(hWnd, &ps);

		// Get the window class pointer
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != NULL)
		{
			// Create a device context for buffering our window
			HDC bufferDC = CreateCompatibleDC(winDC);

			// Get the old bitmap buffer and create a new one
			HBITMAP hOldBmp = pThisWindow->bitmapBuffer;
			pThisWindow->bitmapBuffer = CreateCompatibleBitmap(winDC, ps.rcPaint.right, ps.rcPaint.bottom);

			HBITMAP hDefaultBmp = NULL;
			if (pThisWindow->bitmapBuffer) {

				// Select our bitmap buffer into our window device context
				hDefaultBmp = (HBITMAP)SelectObject(bufferDC, pThisWindow->bitmapBuffer);

				// Paint to the device context
				MAIN_WINDOW_OnPaint(pThisWindow, bufferDC);

				// BitBlt the buffered DC to the actual DC
				BitBlt(winDC, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, bufferDC, 0, 0, SRCCOPY);
			}

			// Free resources
			if (hOldBmp) {
				DeleteObject(hOldBmp);
			}
			if (hDefaultBmp) {
				SelectObject(bufferDC, hDefaultBmp);
			}
			DeleteDC(bufferDC);
		}

		// End paint process
		EndPaint(hWnd, &ps);

		return 0;
	}
	//
	// Request to draw in the specified device context
	// The AnimateWindow function requires that the window being animated implements these messages
	// The DrawThemeParentBackground and DrawThemeParentBackgroundEx functions require the parent of transparent child windows implements these messages
	// 
	// Note: This function should be optimized as much as possible, so we use a bitmap buffer created during a WM_PAINT message
	//
	case WM_PRINT:
	case WM_PRINTCLIENT:
	{
		HDC winDC = (HDC)wParam;
		BOOL visible = true;

		// Get the window class pointer
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != NULL)
		{
			// Check if we should actually draw to the DC
			if ((lParam & PRF_CHECKVISIBLE) == PRF_CHECKVISIBLE) {
				visible = IsWindowVisible(hWnd);
			}

			if (visible)
			{
				// Check if its the client area that needs to be drawn
				if ((lParam & PRF_CLIENT) == PRF_CLIENT)
				{
					RECT clientRt;

					// Get width and height of our client area
					GetClientRect(hWnd, &clientRt);

					// Create a device context for buffering our window
					HDC bufferDC = CreateCompatibleDC(winDC);

					// Select our bitmap buffer into our window device context
					HBITMAP hOldBmp = (HBITMAP)SelectObject(bufferDC, pThisWindow->bitmapBuffer);

					// BitBlt the buffered DC to the actual DC
					BitBlt(winDC, 0, 0, clientRt.right, clientRt.bottom, bufferDC, 0, 0, SRCCOPY);

					// Free resources
					SelectObject(bufferDC, hOldBmp);
					DeleteDC(bufferDC);

					return 0;
				}
			}
		}

		break;
	}
	//
	// Sent when the effective dots per inch (dpi) for a window has changed
	//
	case WM_DPICHANGED:
	{
		// Get the window class pointer
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			// Disable WM_SIZE handling
			pThisWindow->handleResize = false;

			// Resize and reposition controls
			JustCtrl_WindowResizeHandler(hWnd, pThisWindow->control_linked_list, pThisWindow->control_count, HIWORD(wParam), true, (RECT*)lParam);

			// Set the new DPI
			pThisWindow->DPI = (UINT)HIWORD(wParam);

			// Enable WM_SIZE handling
			pThisWindow->handleResize = true;
		}

		return 0;
	}
	//
	// Sent to a window after its size has changed
	//
	case WM_SIZE:
	{
		// Get the window class pointer
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			// Check if its OK to handle the event
			if (pThisWindow->handleResize)
			{
				RECT winRt;

				// Get the window size
				GetWindowRect(hWnd, &winRt);

				// Resize and reposition controls
				JustCtrl_WindowResizeHandler(hWnd, pThisWindow->control_linked_list, pThisWindow->control_count, pThisWindow->DPI, false, &winRt);
			}
		}

		return 0;
	}
	//
	// Sent when an application requests that a window be created
	// We receive this message after the window is created, but before the window becomes visible
	//
	case WM_CREATE:
	{
		CREATESTRUCT* lpCreateParams = (CREATESTRUCT*)lParam;

		// Get the window class pointer
		pThisWindow = (MAIN_WINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pThisWindow != 0)
		{
			RECT winArea;
			RECT clientArea;

			//
			// Here we will convert the width and height from window size, to client size
			//

			// Get both window and client size
			GetWindowRect(hWnd, &winArea);
			GetClientRect(hWnd, &clientArea);

			// Calculate the difference for both width and height
			int xExtra = winArea.right - winArea.left - clientArea.right;
			int yExtra = winArea.bottom - winArea.top - clientArea.bottom;

			// Scale the requested width and height using the monitor DPI
			clientArea.right = clientArea.left + MulDiv(lpCreateParams->cx, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			clientArea.bottom = clientArea.top + MulDiv(lpCreateParams->cy, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI);

			// Calculate width and height from the client rect
			int width = clientArea.right - clientArea.left;
			int height = clientArea.bottom - clientArea.top;

			// Set new width and height
			SetWindowPos(hWnd, NULL, winArea.right, winArea.top,
				width + xExtra, height + yExtra, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

			//
			// Here we move the window to the same monitor as the mouse
			//

			POINT xy;
			// Get mouse(x, y)
			GetCursorPos(&xy);

			// Get monitor from mouse(x, y) and move the window to the same monitor
			HMONITOR hMonitor = MonitorFromPoint(xy, MONITOR_DEFAULTTONEAREST);
			JustCtrl_MoveWindowToMonitor(hWnd, hMonitor);

			// Enable WM_SIZE handling
			pThisWindow->handleResize = true;
		}

		break;
	}
	//
	// Handle events from menu's, controls and accelerator keystrokes
	//
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		// HWND ctrl_hWnd = (HWND)lParam; // Not needed for this example.

		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Application entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ WCHAR* pCmdLine, _In_ int nCmdShow)
{
	MSG msg;

	// Init JustCtrl
	if (!JustCtrl_Init(hInstance, true))
	{
		MessageBox(0, L"JustCtrl failed to initialize, exiting the app!", L"Error!", MB_OK);
		return 0;
	}

	// Init MAIN_WINDOW class
	if (!MAIN_WINDOW::Init(MAIN_WINDOW_WndProc, hInstance))
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
		// Notify user of error
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
