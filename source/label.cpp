
//
// label.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The source file for the JustCtrl_Label control.
//

#include "label.h"

struct JUSTCTRL_LABEL
{
	HWND hWnd;
	HINSTANCE hInstance;
	UINT DPI;
	HBITMAP bitmapBuffer;
	HFONT hFont;
	bool lbuttonDown;
	bool trackingMouse;
};

void WINAPI Label_OnPaint(JUSTCTRL_LABEL* pLabel, HDC hDC)
{
	HWND hWndParent;
	RECT rect;
	RECT ClientArea;
	HFONT hFont;
	HFONT hOldFont;
	size_t strLength;
	WCHAR* pStrText;
	DWORD dwStyle;
	UINT dtFormat;
	int min_y;

	GetClientRect(pLabel->hWnd, &ClientArea);
	hWndParent = GetParent(pLabel->hWnd);

	JUSTCTRL_CTLCOLOR ctlColor;
	memset(&ctlColor, 0, sizeof(JUSTCTRL_CTLCOLOR));
	ctlColor.nmh.code = LABEL_CTLCOLOR;
	ctlColor.nmh.idFrom = GetDlgCtrlID(pLabel->hWnd);
	ctlColor.nmh.hwndFrom = pLabel->hWnd;
	ctlColor.hDC = hDC;
	SendMessage(hWndParent, WM_NOTIFY, ctlColor.nmh.idFrom, (LPARAM)&ctlColor);

	if (ctlColor.bSet)
	{
		if (ctlColor.hBrush)
			FillRect(hDC, &ClientArea, ctlColor.hBrush);
	}
	else
	{
		SetBkMode(hDC, TRANSPARENT);

		if (!IsWindowEnabled(pLabel->hWnd))
			SetTextColor(hDC, GetSysColor(COLOR_GRAYTEXT));
		else
			SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));

		DrawThemeParentBackground(pLabel->hWnd, hDC, NULL);
	}

	hFont = (HFONT)SendMessage(pLabel->hWnd, WM_GETFONT, 0, 0);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	rect.left = 0;
	rect.right = 0;
	rect.top = 0;
	rect.bottom = 0;

	strLength = (size_t)GetWindowTextLength(pLabel->hWnd) + 1;
	pStrText = (WCHAR*)malloc(strLength * sizeof(WCHAR));
	if (pStrText != NULL)
	{
		if (GetWindowText(pLabel->hWnd, pStrText, (int)strLength) != 0)
		{
			DrawText(hDC, pStrText, -1, &rect, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
		}
	}

	min_y = rect.bottom;

	dwStyle = (DWORD)GetWindowLongPtr(pLabel->hWnd, GWL_STYLE);

	if (pStrText != NULL)
	{
		rect.left = 0;
		rect.right = ClientArea.right;

		dtFormat = 0;
		if ((dwStyle & 0x0003) == LABEL_LEFT)
			dtFormat |= DT_LEFT;
		else if ((dwStyle & 0x0003) == LABEL_CENTER)
			dtFormat |= DT_CENTER;
		else if ((dwStyle & 0x0003) == LABEL_RIGHT)
			dtFormat |= DT_RIGHT;

		if ((dwStyle & 0x000C) == LABEL_TOP)
		{
			rect.top = 0;
			rect.bottom = rect.top + min_y;
		}
		else if ((dwStyle & 0x000C) == LABEL_VCENTER)
		{
			rect.top = ((ClientArea.bottom - ClientArea.top)) / 2 - (min_y / 2);
			rect.bottom = rect.top + min_y;
		}
		else if ((dwStyle & 0x000C) == LABEL_BOTTOM)
		{
			rect.bottom = (ClientArea.bottom - ClientArea.top);
			rect.top = rect.bottom - min_y;
		}

		if ((dwStyle & 0x0030) == LABEL_NOPREFIX)
			dtFormat |= DT_NOPREFIX;
		else if ((dwStyle & 0x0030) == LABEL_HIDEPREFIX)
			dtFormat |= DT_HIDEPREFIX;
		else if ((dwStyle & 0x0030) == LABEL_PREFIXONLY)
			dtFormat |= DT_PREFIXONLY;

		if ((dwStyle & 0x00C0) == LABEL_WORD_ELLIPSIS)
			dtFormat |= DT_WORD_ELLIPSIS;
		else if ((dwStyle & 0x00C0) == LABEL_END_ELLIPSIS)
			dtFormat |= DT_END_ELLIPSIS;
		else if ((dwStyle & 0x00C0) == LABEL_PATH_ELLIPSIS)
			dtFormat |= DT_PATH_ELLIPSIS;

		if (dwStyle & LABEL_WORDBREAK)
			dtFormat |= DT_WORDBREAK;

		if (dwStyle & LABEL_RTLREADING)
			dtFormat |= DT_RTLREADING;

		DrawText(hDC, pStrText, -1, &rect, DT_VCENTER | dtFormat);
		free(pStrText);
	}

	SelectObject(hDC, hOldFont);
}

LRESULT CALLBACK Label_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
			if (pLabel->bitmapBuffer) {
				DeleteObject(pLabel->bitmapBuffer);
			}
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

		return 0;
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
		PAINTSTRUCT ps;
		HDC winDC = BeginPaint(hWnd, &ps);

		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel != NULL)
		{
			HDC bufferDC = CreateCompatibleDC(winDC);
			HBITMAP hOldBmp = pLabel->bitmapBuffer;
			pLabel->bitmapBuffer = CreateCompatibleBitmap(winDC, ps.rcPaint.right, ps.rcPaint.bottom);

			HBITMAP hDefaultBmp = NULL;
			if (pLabel->bitmapBuffer) {
				hDefaultBmp = (HBITMAP)SelectObject(bufferDC, pLabel->bitmapBuffer);
				Label_OnPaint(pLabel, bufferDC);
				BitBlt(winDC, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, bufferDC, 0, 0, SRCCOPY);
			}

			if (hOldBmp) {
				DeleteObject(hOldBmp);
			}
			if (hDefaultBmp) {
				SelectObject(bufferDC, hDefaultBmp);
			}
			DeleteDC(bufferDC);
		}

		EndPaint(hWnd, &ps);

		return 0;
	}
	case WM_PRINT:
	case WM_PRINTCLIENT:
	{
		HDC winDC = (HDC)wParam;
		BOOL visible = true;

		pLabel = (JUSTCTRL_LABEL*)GetWindowLongPtr(hWnd, 0);
		if (pLabel != NULL)
		{
			if ((lParam & PRF_CHECKVISIBLE) == PRF_CHECKVISIBLE) {
				visible = IsWindowVisible(hWnd);
			}

			if (visible)
			{
				if ((lParam & PRF_CLIENT) == PRF_CLIENT)
				{
					RECT clientRt;
					GetClientRect(hWnd, &clientRt);

					HDC bufferDC = CreateCompatibleDC(winDC);
					HBITMAP hOldBmp = (HBITMAP)SelectObject(bufferDC, pLabel->bitmapBuffer);

					BitBlt(winDC, 0, 0, clientRt.right, clientRt.bottom, bufferDC, 0, 0, SRCCOPY);

					SelectObject(bufferDC, hOldBmp);
					DeleteDC(bufferDC);

					return 0;
				}
			}
		}

		break;
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
				DWORD dwStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
				if (dwStyle & LABEL_NOTIFY)
				{
					HWND hWndParent = GetParent(hWnd);
					SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), LABEL_CLICKED), (LPARAM)hWnd);
				}
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
	case WM_SETFOCUS:
	{
		HWND hWndParent = GetParent(hWnd);
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), LABEL_SETFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case WM_KILLFOCUS:
	{
		HWND hWndParent = GetParent(hWnd);
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), LABEL_KILLFOCUS), (LPARAM)hWnd);

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
