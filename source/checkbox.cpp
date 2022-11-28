
//
// checkbox.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     blank.
//

#include "checkbox.h"

struct JUSTCTRL_CHECKBOX
{
	HWND hWnd;
	HINSTANCE hInstance;
	UINT DPI;
	HFONT hFont;
	bool lbuttonDown;
	bool trackingMouse;
	bool hot;
	int state;
};

int WINAPI GetCheckboxStyle(HWND hWnd)
{
	JUSTCTRL_CHECKBOX* pCheckbox;
	int cbStyle = 1;

	pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
	if (pCheckbox != 0)
	{
		if (!IsWindowEnabled(hWnd))
			cbStyle += 3;
		else
		{
			if (pCheckbox->state == CHECKBOX_CHECKED)
				cbStyle += 4;
			else if (pCheckbox->state == CHECKBOX_INDETERMINATE)
				cbStyle += 8;
			if (pCheckbox->lbuttonDown)
				cbStyle += 2;
			else
			{
				if (pCheckbox->hot)
					cbStyle += 1;
			}
		}
	}

	return cbStyle;
}

void WINAPI DrawCheckboxCtrl(HWND hWnd, HDC hDC)
{
	HWND hWndParent;
	RECT rect;
	RECT ClientArea;
	HBRUSH hBrush;
	HFONT hFont;
	HFONT hOldFont;
	SIZE size;
	size_t strLength;
	WCHAR* pStrText;
	int cbStyle;
	DWORD dwStyle;
	HTHEME hTheme;
	UINT dtFormat;
	SIZE cbSize;
	double averageWidth;
	double checkboxSpacing;
	int min_y;

	GetClientRect(hWnd, &ClientArea);

	hWndParent = GetParent(hWnd);
	hBrush = (HBRUSH)SendMessage(hWndParent, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hWnd);
	if (!hBrush)
		DrawThemeParentBackground(hWnd, hDC, NULL);
	else
		FillRect(hDC, &ClientArea, hBrush);

	hTheme = OpenThemeData(hWnd, L"BUTTON");
	if (!hTheme) return;

	hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	cbStyle = GetCheckboxStyle(hWnd);
	GetThemePartSize(hTheme, hDC, BP_CHECKBOX, cbStyle, NULL, TS_TRUE, &cbSize);

	GetTextExtentPoint32(hDC, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &size);
	averageWidth = (size.cx / 26 + 1) / 2;
	checkboxSpacing = ceil(MulDiv(12, (int)averageWidth, 4));

	// Get the Text width.

	rect.left = 0;
	rect.right = 0;
	rect.top = 0;
	rect.bottom = 0;

	strLength = GetWindowTextLength(hWnd) + 1;
	pStrText = (WCHAR*)malloc(strLength * sizeof(WCHAR));
	if (pStrText != NULL)
	{
		if (GetWindowText(hWnd, pStrText, (int)strLength) != 0)
		{
			DrawText(hDC, pStrText, -1, &rect, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
		}
	}

	min_y = (cbSize.cy < rect.bottom) ? rect.bottom : cbSize.cy;

	rect.left = 0;
	rect.right = rect.left + cbSize.cx;

	dwStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
	if ((dwStyle & 0x000C) == CHECKBOX_TOP)
	{
		rect.top = (cbSize.cy < min_y) ? ((min_y - cbSize.cy) / 2) : 0;
		rect.bottom = rect.top + cbSize.cy;
	}
	else if ((dwStyle & 0x000C) == CHECKBOX_VCENTER)
	{
		rect.top = ((ClientArea.bottom - ClientArea.top)) / 2 - (cbSize.cy / 2);
		rect.bottom = rect.top + cbSize.cy;
	}
	else if ((dwStyle & 0x000C) == CHECKBOX_BOTTOM)
	{
		rect.bottom = (ClientArea.bottom - ClientArea.top);
		rect.bottom -= (cbSize.cy < min_y) ? ((min_y - cbSize.cy) / 2) : 0;
		rect.top = rect.bottom - cbSize.cy;
	}

	DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, cbStyle, &rect, NULL);

	if (pStrText != NULL)
	{
		rect.left = (LONG)checkboxSpacing;
		rect.right = ClientArea.right;

		dtFormat = 0;
		if ((dwStyle & 0x0003) == CHECKBOX_LEFT)
			dtFormat |= DT_LEFT;
		else if ((dwStyle & 0x0003) == CHECKBOX_CENTER)
			dtFormat |= DT_CENTER;
		else if ((dwStyle & 0x0003) == CHECKBOX_RIGHT)
			dtFormat |= DT_RIGHT;

		if ((dwStyle & 0x000C) == CHECKBOX_TOP)
		{
			rect.top = 0;
			rect.bottom = rect.top + min_y;
		}
		else if ((dwStyle & 0x000C) == CHECKBOX_VCENTER)
		{
			rect.top = ((ClientArea.bottom - ClientArea.top)) / 2 - (min_y / 2);
			rect.bottom = rect.top + min_y;
		}
		else if ((dwStyle & 0x000C) == CHECKBOX_BOTTOM)
		{
			rect.bottom = (ClientArea.bottom - ClientArea.top);
			rect.top = rect.bottom - min_y;
		}

		if ((dwStyle & 0x0030) == CHECKBOX_NOPREFIX)
			dtFormat |= DT_NOPREFIX;
		else if ((dwStyle & 0x0030) == CHECKBOX_HIDEPREFIX)
			dtFormat |= DT_HIDEPREFIX;
		else if ((dwStyle & 0x0030) == CHECKBOX_PREFIXONLY)
			dtFormat |= DT_PREFIXONLY;

		if (dwStyle & CHECKBOX_RTLREADING)
			dtFormat |= DT_RTLREADING;

		DrawText(hDC, pStrText, -1, &rect, DT_SINGLELINE | DT_VCENTER | dtFormat);
		free(pStrText);
	}

	CloseThemeData(hTheme);
	SelectObject(hDC, hOldFont);
}

LRESULT CALLBACK Checkbox_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC WinDC;
	PAINTSTRUCT ps;
	HDC bufferDC;
	HBITMAP hMemoryBmp;
	HBITMAP hOldBmp;

	JUSTCTRL_CHECKBOX* pCheckbox;

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)malloc(sizeof(JUSTCTRL_CHECKBOX));
		if (pCheckbox != 0)
		{
			memset(pCheckbox, 0, sizeof(JUSTCTRL_CHECKBOX));

			pCheckbox->hWnd = hWnd;
			pCheckbox->hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			pCheckbox->DPI = JustCtrl_GetDpiForWindow(hWnd);

			SetWindowLongPtr(hWnd, 0, (LONG_PTR)pCheckbox);
		}
		break;
	}
	case WM_DESTROY:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox != 0)
		{
			free(pCheckbox);
		}
		break;
	}
	case WM_SETFONT:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox != 0)
		{
			pCheckbox->hFont = (HFONT)wParam;
			pCheckbox->DPI = JustCtrl_GetDpiForWindow(hWnd);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	case WM_GETFONT:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox != 0)
		{
			return (LRESULT)pCheckbox->hFont;
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

		DrawCheckboxCtrl(hWnd, bufferDC);
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
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox)
		{
			pCheckbox->hot = true;

			if (!pCheckbox->trackingMouse)
			{
				TRACKMOUSEEVENT tme;

				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				tme.dwHoverTime = 0;

				TrackMouseEvent(&tme);
				pCheckbox->trackingMouse = true;
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_LBUTTONUP:
	{
		bool lbuttonDown;

		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox)
		{
			lbuttonDown = pCheckbox->lbuttonDown;
			pCheckbox->lbuttonDown = false;

			if (lbuttonDown)
			{
				DWORD dwStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
				if (dwStyle & CHECKBOX_AUTO)
				{
					if (dwStyle & CHECKBOX_3STATE)
					{
						if (pCheckbox->state == 2)
							pCheckbox->state = 0;
						else
							pCheckbox->state++;
					}
					else
					{
						if (pCheckbox->state == 1)
							pCheckbox->state = 0;
						else
							pCheckbox->state++;
					}
				}

				HWND hWndParent = GetParent(hWnd);
				SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), CHECKBOX_CLICKED), (LPARAM)hWnd);
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox)
		{
			pCheckbox->lbuttonDown = true;
			SetFocus(hWnd);

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_MOUSELEAVE:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox)
		{
			pCheckbox->lbuttonDown = false;
			pCheckbox->trackingMouse = false;
			pCheckbox->hot = false;

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_SETFOCUS:
	{
		HWND hWndParent = GetParent(hWnd);
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), CHECKBOX_SETFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case WM_KILLFOCUS:
	{
		HWND hWndParent = GetParent(hWnd);
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), CHECKBOX_KILLFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case CHECKBOX_GETCHECK:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox)
		{
			return pCheckbox->state;
		}

		return 0;
	}
	case CHECKBOX_SETCHECK:
	{
		pCheckbox = (JUSTCTRL_CHECKBOX*)GetWindowLongPtr(hWnd, 0);
		if (pCheckbox)
		{
			pCheckbox->state = (int)LOWORD(wParam);

			if ((bool)HIWORD(wParam) == true)
			{
				InvalidateRect(hWnd, NULL, FALSE);
			}
		}

		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

SIZE WINAPI JustCtrl_GetCheckboxMinimumSize(HWND hWnd)
{
	HDC dc;
	HFONT hFont;
	HFONT oldFont;
	RECT txtRect;
	SIZE size;
	size_t strLength;
	WCHAR* pStrText;
	HTHEME hTheme;
	int cbStyle;
	DWORD dwStyle;
	DWORD dwStyleEx;
	SIZE cbSize;
	double averageWidth;
	double checkboxSpacing;
	int borderX;
	int borderY;

	dc = GetDC(hWnd);

	// Note that GetDC returns an uninitialized DC, which has "System" (a bitmap font) as the default font; thus the need to select a font into the DC.

	hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	oldFont = (HFONT)SelectObject(dc, hFont);

	hTheme = OpenThemeData(hWnd, L"BUTTON");
	cbStyle = GetCheckboxStyle(hWnd);
	GetThemePartSize(hTheme, dc, BP_CHECKBOX, cbStyle, NULL, TS_TRUE, &cbSize);

	// Get space from label.

	GetTextExtentPoint32(dc, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &size);
	averageWidth = (size.cx / 26 + 1) / 2;
	checkboxSpacing = ceil(MulDiv(12, (int)averageWidth, 4));

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
			DrawText(dc, pStrText, -1, &txtRect, DT_CALCRECT | DT_LEFT | DT_SINGLELINE);
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

	CloseThemeData(hTheme);
	SelectObject(dc, oldFont);
	ReleaseDC(hWnd, dc);

	// Done.

	size.cx = (txtRect.right == 0) ? cbSize.cx : (txtRect.right + (LONG)checkboxSpacing);
	size.cy = ((LONG)cbSize.cy < txtRect.bottom) ? txtRect.bottom : (LONG)cbSize.cy;

	return size;
}

bool WINAPI JustCtrl_InitCheckbox(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	// Register the JustCtrl_Checkbox class.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Checkbox_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"JustCtrl_Checkbox";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"Failed to register the JustCtrl_Checkbox class.", L"Error!", MB_OK);
		return false;
	}

	return true;
}
