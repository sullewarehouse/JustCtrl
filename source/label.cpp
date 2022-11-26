
//
// label.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     blank.
//

#include "label.h"

struct JUSTCTRL_LABEL
{
	HWND hWnd;
	HINSTANCE hInstance;
	UINT DPI;
	HFONT hFont;
	bool lbuttonDown;
	bool trackingMouse;
};

void WINAPI DrawLabelCtrl(HWND hWnd, HDC hDC)
{
}

LRESULT CALLBACK Label_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC WinDC;
	PAINTSTRUCT ps;
	HDC bufferDC;
	HBITMAP hMemoryBmp;
	HBITMAP hOldBmp;

	JUSTCTRL_LABEL* pLabel;

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		pLabel = (JUSTCTRL_LABEL*)malloc(sizeof(JUSTCTRL_LABEL));
		if (pLabel != 0)
		{
			memset(pLabel, 0, sizeof(JUSTCTRL_LABEL));

			pLabel->hWnd = hWnd;
			pLabel->hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			pLabel->DPI = JustCtrl_GetDpiForWindow(hWnd);

			SetWindowLongPtr(hWnd, 0, (LONG_PTR)pLabel);
		}
		break;
	}
	case WM_DESTROY:
	{
		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel != 0)
		{
			free(pLabel);
		}
		break;
	}
	case WM_SETFONT:
	{
		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel != 0)
		{
			pLabel->hFont = (HFONT)wParam;
			pLabel->DPI = JustCtrl_GetDpiForWindow(hWnd);
		}
		break;
	}
	case WM_GETFONT:
	{
		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel != 0)
		{
			return (LRESULT)pLabel->hFont;
		}
		return NULL;
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

		bufferDC = CreateCompatibleDC(WinDC);
		hMemoryBmp = CreateCompatibleBitmap(WinDC, ps.rcPaint.right, ps.rcPaint.bottom);
		hOldBmp = (HBITMAP)SelectObject(bufferDC, hMemoryBmp);

		DrawLabelCtrl(hWnd, bufferDC);
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
	case WM_MOUSEMOVE:
	{
		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel)
		{
			if (!pLabel->trackingMouse)
			{
				TRACKMOUSEEVENT tme;

				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				tme.dwHoverTime = 0;

				TrackMouseEvent(&tme);
				pLabel->trackingMouse = true;
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_LBUTTONUP:
	{
		bool lbuttonDown;

		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel)
		{
			lbuttonDown = pLabel->lbuttonDown;
			pLabel->lbuttonDown = false;

			if (lbuttonDown)
			{
				// Notify parent!
			}
		}

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel)
		{
			pLabel->lbuttonDown = true;
			SetFocus(hWnd);

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_MOUSELEAVE:
	{
		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel)
		{
			pLabel->lbuttonDown = false;
			pLabel->trackingMouse = false;

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

SIZE WINAPI JustCtrl_GetLabelMinimumSize(HWND hWnd)
{
	HDC dc;
	HFONT hFont;
	HFONT oldFont;
	RECT txtRect;
	SIZE size;
	size_t strLength;
	WCHAR* pStrText;
	DWORD dwStyle;
	DWORD dwStyleEx;
	int borderX;
	int borderY;

	dc = GetDC(hWnd);

	// Note that GetDC returns an uninitialized DC, which has "System" (a bitmap font) as the default font; thus the need to select a font into the DC.

	hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	oldFont = (HFONT)SelectObject(dc, hFont);

	// Get the Text width.

	txtRect.left = 0;
	txtRect.right = 0;
	txtRect.top = 0;
	txtRect.bottom = 0;

	strLength = GetWindowTextLength(hWnd) + 1;
	pStrText = (WCHAR*)malloc(strLength * sizeof(WCHAR));
	if (pStrText != NULL)
	{
		if (GetWindowText(hWnd, pStrText, (int)strLength) != 0)
		{
			DrawTextW(dc, pStrText, -1, &txtRect, DT_CALCRECT | DT_LEFT | DT_SINGLELINE);
		}
		free(pStrText);
	}

	dwStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
	if (dwStyle & WS_BORDER)
	{
		borderX = GetSystemMetrics(SM_CXBORDER) * 2;
		borderY = GetSystemMetrics(SM_CYBORDER) * 2;
		txtRect.right += borderX;
		txtRect.bottom += borderY;
	}
	else
	{
		dwStyleEx = (DWORD)GetWindowLongPtr(hWnd, GWL_EXSTYLE);
		if (dwStyleEx & WS_EX_STATICEDGE)
		{
			borderX = GetSystemMetrics(SM_CXEDGE) * 2;
			borderY = GetSystemMetrics(SM_CYEDGE) * 2;
			txtRect.right += borderX;
			txtRect.bottom += borderY;
		}
	}

	// Cleanup.

	SelectObject(dc, oldFont);
	ReleaseDC(hWnd, dc);

	// Done.

	size.cx = txtRect.right;
	size.cy = txtRect.bottom;

	return size;
}

bool WINAPI JustCtrl_InitLabel(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	// Register the JustCtrl_Label class.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Label_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"JustCtrl_Label";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"Failed to register the JustCtrl_Label class.", L"Error!", MB_OK);
		return false;
	}

	return true;
}
