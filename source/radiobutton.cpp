
//
// radiobutton.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The source file for the JustCtrl_RadioButton control.
//

#include "radiobutton.h"

struct JUSTCTRL_RADIOBUTTON
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

int WINAPI GetRadioButtonStyle(HWND hWnd)
{
	JUSTCTRL_RADIOBUTTON* pRadioButton;
	int rbStyle = 1;

	pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
	if (pRadioButton != 0)
	{
		if (!IsWindowEnabled(hWnd))
			rbStyle += 3;
		else
		{
			if (pRadioButton->state == RADIOBUTTON_CHECKED)
				rbStyle += 4;
			if (pRadioButton->lbuttonDown)
				rbStyle += 2;
			else
			{
				if (pRadioButton->hot)
					rbStyle += 1;
			}
		}
	}

	return rbStyle;
}

void WINAPI DrawRadioButtonCtrl(HWND hWnd, HDC hDC)
{
	HWND hWndParent;
	RECT rect;
	RECT ClientArea;
	HFONT hFont;
	HFONT hOldFont;
	SIZE size;
	size_t strLength;
	WCHAR* pStrText;
	int rbStyle;
	DWORD dwStyle;
	HTHEME hTheme;
	UINT dtFormat;
	SIZE cbSize;
	double averageWidth;
	double radioButtonSpacing;
	int min_y;

	GetClientRect(hWnd, &ClientArea);
	hWndParent = GetParent(hWnd);

	JUSTCTRL_CTLCOLOR ctlColor;
	memset(&ctlColor, 0, sizeof(JUSTCTRL_CTLCOLOR));
	ctlColor.nmh.code = RADIOBUTTON_CTLCOLOR;
	ctlColor.nmh.idFrom = GetDlgCtrlID(hWnd);
	ctlColor.nmh.hwndFrom = hWnd;
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

		if (!IsWindowEnabled(hWnd))
			SetTextColor(hDC, GetSysColor(COLOR_GRAYTEXT));
		else
			SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));

		DrawThemeParentBackground(hWnd, hDC, NULL);
	}

	hTheme = OpenThemeData(hWnd, L"BUTTON");
	if (!hTheme) return;

	hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	rbStyle = GetRadioButtonStyle(hWnd);
	GetThemePartSize(hTheme, hDC, BP_RADIOBUTTON, rbStyle, NULL, TS_TRUE, &cbSize);

	GetTextExtentPoint32(hDC, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &size);
	averageWidth = (size.cx / 26 + 1) / 2;
	radioButtonSpacing = ceil(MulDiv(12, (int)averageWidth, 4));

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
	if ((dwStyle & 0x000C) == RADIOBUTTON_TOP)
	{
		rect.top = (cbSize.cy < min_y) ? ((min_y - cbSize.cy) / 2) : 0;
		rect.bottom = rect.top + cbSize.cy;
	}
	else if ((dwStyle & 0x000C) == RADIOBUTTON_VCENTER)
	{
		rect.top = ((ClientArea.bottom - ClientArea.top)) / 2 - (cbSize.cy / 2);
		rect.bottom = rect.top + cbSize.cy;
	}
	else if ((dwStyle & 0x000C) == RADIOBUTTON_BOTTOM)
	{
		rect.bottom = (ClientArea.bottom - ClientArea.top);
		rect.bottom -= (cbSize.cy < min_y) ? ((min_y - cbSize.cy) / 2) : 0;
		rect.top = rect.bottom - cbSize.cy;
	}

	DrawThemeBackground(hTheme, hDC, BP_RADIOBUTTON, rbStyle, &rect, NULL);

	if (pStrText != NULL)
	{
		rect.left = (LONG)radioButtonSpacing;
		rect.right = ClientArea.right;

		dtFormat = 0;
		if ((dwStyle & 0x0003) == RADIOBUTTON_LEFT)
			dtFormat |= DT_LEFT;
		else if ((dwStyle & 0x0003) == RADIOBUTTON_CENTER)
			dtFormat |= DT_CENTER;
		else if ((dwStyle & 0x0003) == RADIOBUTTON_RIGHT)
			dtFormat |= DT_RIGHT;

		if ((dwStyle & 0x000C) == RADIOBUTTON_TOP)
		{
			rect.top = 0;
			rect.bottom = rect.top + min_y;
		}
		else if ((dwStyle & 0x000C) == RADIOBUTTON_VCENTER)
		{
			rect.top = ((ClientArea.bottom - ClientArea.top)) / 2 - (min_y / 2);
			rect.bottom = rect.top + min_y;
		}
		else if ((dwStyle & 0x000C) == RADIOBUTTON_BOTTOM)
		{
			rect.bottom = (ClientArea.bottom - ClientArea.top);
			rect.top = rect.bottom - min_y;
		}

		if ((dwStyle & 0x0030) == RADIOBUTTON_NOPREFIX)
			dtFormat |= DT_NOPREFIX;
		else if ((dwStyle & 0x0030) == RADIOBUTTON_HIDEPREFIX)
			dtFormat |= DT_HIDEPREFIX;
		else if ((dwStyle & 0x0030) == RADIOBUTTON_PREFIXONLY)
			dtFormat |= DT_PREFIXONLY;

		if (dwStyle & RADIOBUTTON_RTLREADING)
			dtFormat |= DT_RTLREADING;

		DrawText(hDC, pStrText, -1, &rect, DT_SINGLELINE | DT_VCENTER | dtFormat);
		free(pStrText);
	}

	CloseThemeData(hTheme);
	SelectObject(hDC, hOldFont);
}

LRESULT CALLBACK RadioButton_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC WinDC;
	PAINTSTRUCT ps;
	HDC bufferDC;
	HBITMAP hMemoryBmp;
	HBITMAP hOldBmp;

	JUSTCTRL_RADIOBUTTON* pRadioButton;

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)malloc(sizeof(JUSTCTRL_RADIOBUTTON));
		if (pRadioButton != 0)
		{
			memset(pRadioButton, 0, sizeof(JUSTCTRL_RADIOBUTTON));

			pRadioButton->hWnd = hWnd;
			pRadioButton->hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			pRadioButton->DPI = JustCtrl_GetDpiForWindow(hWnd);

			SetWindowLongPtr(hWnd, 0, (LONG_PTR)pRadioButton);
		}
		break;
	}
	case WM_DESTROY:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton != 0)
		{
			free(pRadioButton);
		}
		break;
	}
	case WM_SETFONT:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton != 0)
		{
			pRadioButton->hFont = (HFONT)wParam;
			pRadioButton->DPI = JustCtrl_GetDpiForWindow(hWnd);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	case WM_GETFONT:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton != 0)
		{
			return (LRESULT)pRadioButton->hFont;
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

		DrawRadioButtonCtrl(hWnd, bufferDC);
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
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton)
		{
			pRadioButton->hot = true;

			if (!pRadioButton->trackingMouse)
			{
				TRACKMOUSEEVENT tme;

				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				tme.dwHoverTime = 0;

				TrackMouseEvent(&tme);
				pRadioButton->trackingMouse = true;
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_LBUTTONUP:
	{
		bool lbuttonDown;

		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton)
		{
			lbuttonDown = pRadioButton->lbuttonDown;
			pRadioButton->lbuttonDown = false;

			if (lbuttonDown)
			{
				DWORD dwStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
				if (dwStyle & RADIOBUTTON_AUTO)
				{
					HWND hWndParent = GetParent(hWnd);
					HWND hWndSibling = hWnd;

					WCHAR* pClassName = (WCHAR*)malloc(sizeof(WCHAR) * 256);
					if (pClassName == NULL) return 0;

					do
					{
						hWndSibling = GetNextDlgGroupItem(hWndParent, hWndSibling, TRUE);
						if (hWndSibling != hWnd)
						{
							GetClassName(hWndSibling, pClassName, 256);
							if (wcscmp(pClassName, L"JustCtrl_RadioButton") == 0)
							{
								RadioButton_SetCheck(hWndSibling, RADIOBUTTON_UNCHECKED, true);
							}
						}

					} while (hWndSibling != hWnd);

					free(pClassName);

					pRadioButton->state = 1;
				}

				HWND hWndParent = GetParent(hWnd);
				SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), RADIOBUTTON_CLICKED), (LPARAM)hWnd);
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton)
		{
			pRadioButton->lbuttonDown = true;
			SetFocus(hWnd);

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_MOUSELEAVE:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton)
		{
			pRadioButton->lbuttonDown = false;
			pRadioButton->trackingMouse = false;
			pRadioButton->hot = false;

			InvalidateRect(hWnd, NULL, FALSE);
		}

		return 0;
	}
	case WM_SETFOCUS:
	{
		HWND hWndParent = GetParent(hWnd);
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), RADIOBUTTON_SETFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case WM_KILLFOCUS:
	{
		HWND hWndParent = GetParent(hWnd);
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), RADIOBUTTON_KILLFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case RADIOBUTTON_GETCHECK:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton)
		{
			return pRadioButton->state;
		}

		return 0;
	}
	case RADIOBUTTON_SETCHECK:
	{
		pRadioButton = (JUSTCTRL_RADIOBUTTON*)GetWindowLongPtr(hWnd, 0);
		if (pRadioButton)
		{
			pRadioButton->state = (int)LOWORD(wParam);

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

SIZE WINAPI JustCtrl_GetRadioButtonMinimumSize(HWND hWnd)
{
	HDC dc;
	HFONT hFont;
	HFONT oldFont;
	RECT txtRect;
	SIZE size;
	size_t strLength;
	WCHAR* pStrText;
	HTHEME hTheme;
	int rbStyle;
	DWORD dwStyle;
	DWORD dwStyleEx;
	SIZE cbSize;
	double averageWidth;
	double radioButtonSpacing;
	int borderX;
	int borderY;

	dc = GetDC(hWnd);

	// Note that GetDC returns an uninitialized DC, which has "System" (a bitmap font) as the default font; thus the need to select a font into the DC.

	hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	oldFont = (HFONT)SelectObject(dc, hFont);

	hTheme = OpenThemeData(hWnd, L"BUTTON");
	rbStyle = GetRadioButtonStyle(hWnd);
	GetThemePartSize(hTheme, dc, BP_RADIOBUTTON, rbStyle, NULL, TS_TRUE, &cbSize);

	// Get space from label.

	GetTextExtentPoint32(dc, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &size);
	averageWidth = (size.cx / 26 + 1) / 2;
	radioButtonSpacing = ceil(MulDiv(12, (int)averageWidth, 4));

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

	size.cx = (txtRect.right == 0) ? cbSize.cx : (txtRect.right + (LONG)radioButtonSpacing);
	size.cy = ((LONG)cbSize.cy < txtRect.bottom) ? txtRect.bottom : (LONG)cbSize.cy;

	return size;
}

bool WINAPI JustCtrl_InitRadioButton(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	// Register the JustCtrl_RadioButton class.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = RadioButton_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"JustCtrl_RadioButton";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"Failed to register the JustCtrl_RadioButton class.", L"Error!", MB_OK);
		return false;
	}

	return true;
}
