
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
#define IDC_TABWINDOW                   0x0008
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
	FORM_CTRL cbHide;
	FORM_CTRL lbColor;
	FORM_CTRL rbRed;
	FORM_CTRL rbBlue;
	FORM_CTRL lbShape;
	FORM_CTRL rbTriangle;
	FORM_CTRL rbCircle;
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
	nwc->rbCircle.next = &nwc->tabWindow;
	nwc->tabWindow.next = &nwc->OkBtn;

	nwc->control_count = 9;

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

	// *** GDI Plus drawing *** //

	Graphics graphics(hDC);
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
			const int x = 124;
			const int y = 20;
			const int width = 250;
			const int height = 250;

			Point drawingPoints[] = {
				Point(MulDiv(x, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI), MulDiv(y + height, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI)),
				Point(MulDiv(x + (width / 2), pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI), MulDiv(y, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI)),
				Point(MulDiv(x + width, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI), MulDiv(y + height, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI)) };

			graphics.DrawPolygon(&GdiPen, drawingPoints, 3);
		}
		else if (RadioButton_GetCheck(pThisWindow->rbCircle.hWnd) == CHECKBOX_CHECKED)
		{
			const int x = 124;
			const int y = 20;
			const int width = 250;
			const int height = 250;

			Rect DrawingRect = Rect(
				MulDiv(x, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI),
				MulDiv(y, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI),
				MulDiv(width, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI),
				MulDiv(height, pThisWindow->DPI, JUSTCTRL_APPLICATION_DPI));

			graphics.DrawEllipse(&GdiPen, DrawingRect);
		}
	}
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
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
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
