
//
// tabwindow.cpp
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The source file for the JustCtrl_TabWindow control.
//

#include "tabwindow.h"

struct TABWINDOW_DIMS
{
	int txtHeight;
	int devBar;
	int tabBorder;
	int tabInnerSpace;
	int tabMinWidth;
	int btnWidth;
};

struct TABWINDOW_COLORS
{
	COLORREF text;
	COLORREF background;
	COLORREF headbar;
	COLORREF tabBorder;
	COLORREF itemNormal;
	COLORREF itemHot;
	COLORREF itemPressed;
	COLORREF itemSelect;
	COLORREF itemDisbaled;
};

typedef struct _TABWINDOW_TAB_INTERNAL TABWINDOW_TAB_INTERNAL;
struct _TABWINDOW_TAB_INTERNAL
{
	TABWINDOW_ITEM userItem;
	SIZE txtMinSize;
	ULONG itemWidth;
	TABWINDOW_TAB_INTERNAL* next;
};

typedef struct _TABWINDOW_BTN_INTERNAL TABWINDOW_BTN_INTERNAL;
struct _TABWINDOW_BTN_INTERNAL
{
	TABWINDOW_BUTTON userButton;
	TABWINDOW_BTN_INTERNAL* next;
};

struct JUSTCTRL_TABWINDOW
{
	HWND hWnd;
	HINSTANCE hInstance;
	UINT DPI;
	HBITMAP bitmapBuffer;
	HBITMAP ncBitmapBuffer;
	HFONT hFont;
	TABWINDOW_DIMS dims;
	TABWINDOW_COLORS colors;
	TABWINDOW_TAB_INTERNAL* items;
	int nItems;
	TABWINDOW_BTN_INTERNAL* buttons;
	int nButtons;
	ULONG btnTotalSpace;
	int selectedItem;
	int prevSelected;
	bool lbuttonDown;
	bool trackingMouse;
	ULONG hot_htCode;
	LONG hot_itemI;
	ULONG mbtn_htCode;
	LONG mbtn_ItemI;
	ULONG mbtn_ItemB;
};

TABWINDOW_TAB_INTERNAL* WINAPI ctrl_TabWindow_InsertItem(JUSTCTRL_TABWINDOW* pTabWindow, ULONG i)
{
	ULONG cnt;
	TABWINDOW_TAB_INTERNAL* thisItem, * prevItem, * newItem;

	newItem = (TABWINDOW_TAB_INTERNAL*)malloc(sizeof(TABWINDOW_TAB_INTERNAL));
	if (newItem == 0)
		return 0;

	memset(newItem, 0, sizeof(TABWINDOW_TAB_INTERNAL));

	prevItem = 0;

	if ((i == 0) || (pTabWindow->items == NULL))
	{
		newItem->next = pTabWindow->items;
		pTabWindow->items = newItem;
	}
	else
	{
		cnt = 0;
		thisItem = pTabWindow->items;
		while ((thisItem != 0) && (cnt < i))
		{
			prevItem = thisItem;
			thisItem = thisItem->next;
			cnt++;
		}

		newItem->next = thisItem;

		if (prevItem)
			prevItem->next = newItem;
	}

	return newItem;
}

TABWINDOW_BTN_INTERNAL* WINAPI ctrl_TabWindow_InsertButton(JUSTCTRL_TABWINDOW* pTabWindow, ULONG i)
{
	ULONG cnt;
	TABWINDOW_BTN_INTERNAL* thisButton, * prevButton, * newButton;

	newButton = (TABWINDOW_BTN_INTERNAL*)malloc(sizeof(TABWINDOW_BTN_INTERNAL));
	if (newButton == 0)
		return 0;

	memset(newButton, 0, sizeof(TABWINDOW_BTN_INTERNAL));

	prevButton = 0;

	if ((i == 0) || (pTabWindow->buttons == NULL))
	{
		newButton->next = pTabWindow->buttons;
		pTabWindow->buttons = newButton;
	}
	else
	{
		cnt = 0;
		thisButton = pTabWindow->buttons;
		while ((thisButton != 0) && (cnt < i))
		{
			prevButton = thisButton;
			thisButton = thisButton->next;
			cnt++;
		}

		newButton->next = thisButton;

		if (prevButton)
			prevButton->next = newButton;
	}

	return newButton;
}

bool WINAPI ctrl_TabWindow_DeleteItem(JUSTCTRL_TABWINDOW* pTabWindow, ULONG i)
{
	ULONG cnt;
	TABWINDOW_TAB_INTERNAL* thisItem, * prevItem;
	bool success;

	success = false;

	cnt = 0;
	thisItem = pTabWindow->items;
	while ((thisItem != 0) && (cnt < i))
	{
		prevItem = thisItem;
		thisItem = thisItem->next;
		cnt++;
	}

	if (thisItem != NULL)
	{
		if (thisItem == pTabWindow->items)
			pTabWindow->items = thisItem->next;
		else
			prevItem->next = thisItem->next;

		if (thisItem->userItem.pText != NULL)
			free(thisItem->userItem.pText);

		free(thisItem);

		success = true;
	}

	return success;
}

bool WINAPI ctrl_TabWindow_DeleteButton(JUSTCTRL_TABWINDOW* pTabWindow, ULONG i)
{
	ULONG cnt;
	TABWINDOW_BTN_INTERNAL* thisButton, * prevButton;
	bool success;

	success = false;

	cnt = 0;
	thisButton = pTabWindow->buttons;
	while ((thisButton != 0) && (cnt < i))
	{
		prevButton = thisButton;
		thisButton = thisButton->next;
		cnt++;
	}

	if (thisButton != NULL)
	{
		if (thisButton == pTabWindow->buttons)
			pTabWindow->buttons = thisButton->next;
		else
			prevButton->next = thisButton->next;

		free(thisButton);

		success = true;
	}

	return success;
}

TABWINDOW_TAB_INTERNAL* WINAPI ctrl_TabWindow_GetItem(JUSTCTRL_TABWINDOW* pTabWindow, ULONG i)
{
	ULONG cnt;
	TABWINDOW_TAB_INTERNAL* thisItem, * prevItem;

	cnt = 0;
	thisItem = prevItem = pTabWindow->items;
	while ((thisItem != 0) && (cnt < i))
	{
		prevItem = thisItem;
		thisItem = thisItem->next;
		cnt++;
	}

	if ((LONG)i == -1)
		thisItem = prevItem;

	return thisItem;
}

ULONG WINAPI ctrl_TabWindow_GetItemIndex(JUSTCTRL_TABWINDOW* pTabWindow, TABWINDOW_TAB_INTERNAL* tabItem)
{
	ULONG cnt;
	TABWINDOW_TAB_INTERNAL* thisItem;

	cnt = 0;
	thisItem = pTabWindow->items;
	while (thisItem != 0)
	{
		if (thisItem == tabItem)
			break;

		thisItem = thisItem->next;
		cnt++;
	}

	return cnt;
}

TABWINDOW_TAB_INTERNAL* WINAPI ctrl_TabWindow_FindItem(JUSTCTRL_TABWINDOW* pTabWindow, WCHAR* pText)
{
	TABWINDOW_TAB_INTERNAL* thisItem;

	thisItem = pTabWindow->items;
	while (thisItem != 0)
	{
		if (wcscmp(thisItem->userItem.pText, pText) == 0)
			break;

		thisItem = thisItem->next;
	}

	return thisItem;
}

SIZE WINAPI ctrl_TabWindow_GetTextMinimumSize(HWND hWnd, int monitorDpi, WCHAR* strText)
{
	HDC dc;
	HFONT hFont;
	HFONT oldFont;
	RECT txtRect;
	SIZE size;

	dc = GetDC(hWnd);

	// Note that GetDC returns an uninitialized DC, which has "System" (a bitmap font) as the default font; thus the need to select a font into the DC.

	hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	oldFont = (HFONT)SelectObject(dc, hFont);

	// Get the Text width.

	txtRect.left = 0;
	txtRect.right = 0;
	txtRect.top = 0;
	txtRect.bottom = 0;
	DrawTextW(dc, strText, -1, &txtRect, DT_CALCRECT | DT_LEFT | DT_SINGLELINE);

	// Cleanup.

	SelectObject(dc, oldFont);
	ReleaseDC(hWnd, dc);

	// Done.

	size.cx = txtRect.right;
	size.cy = txtRect.bottom;

	return size;
}

LONG WINAPI ctrl_TabWindow_GetTabHeight(JUSTCTRL_TABWINDOW* pTabWindow, bool bNewFont)
{
	LONG height;
	DWORD dwStyle;

	if ((bNewFont == true) || (pTabWindow->dims.txtHeight == 0))
	{
		HDC hDC = GetDC(pTabWindow->hWnd);

		// Note that GetDC returns an uninitialized DC, which has "System" (a bitmap font) as the default font; thus the need to select a font into the DC.

		HFONT hFont = (HFONT)SendMessage(pTabWindow->hWnd, WM_GETFONT, 0, 0);
		HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);

		// Cleanup.

		SelectObject(hDC, hOldFont);
		ReleaseDC(pTabWindow->hWnd, hDC);

		pTabWindow->dims.txtHeight = tm.tmHeight;
	}

	height = pTabWindow->dims.txtHeight + (pTabWindow->dims.tabInnerSpace * 2);

	dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

	if (dwStyle & TABWINDOW_DEVBAR) {
		height += pTabWindow->dims.devBar;
	}

	if (dwStyle & TABWINDOW_BORDER) {
		height += (pTabWindow->dims.tabBorder * 2);
	}

	return height;
}

void WINAPI TabWindow_OnPaint(JUSTCTRL_TABWINDOW* pTabWindow, HDC hDC)
{
	RECT clientRt;
	GetClientRect(pTabWindow->hWnd, &clientRt);

	HBRUSH hBrush = CreateSolidBrush(pTabWindow->colors.background);
	FillRect(hDC, &clientRt, hBrush);
	DeleteBrush(hBrush);
}

void WINAPI TabWindow_NonClient_OnPaint(JUSTCTRL_TABWINDOW* pTabWindow, HDC hDC)
{
	RECT winRt;
	RECT drawRt;
	HBRUSH hbBorder;
	HBRUSH hbNormal;
	HBRUSH hbHot;
	HBRUSH hbSelected;
	HFONT hOldFont;

	TABWINDOW_TAB_INTERNAL* tabItem;

	GetWindowRect(pTabWindow->hWnd, &winRt);
	LONG tabHeight = ctrl_TabWindow_GetTabHeight(pTabWindow, false);

	drawRt.left = 0;
	drawRt.right = winRt.right - winRt.left;
	drawRt.top = 0;
	drawRt.bottom = tabHeight;

	DrawThemeParentBackgroundEx(pTabWindow->hWnd, hDC, DTPB_WINDOWDC, &drawRt);

	MapWindowPoints(NULL, pTabWindow->hWnd, (POINT*)&winRt, 2);

	LONG winWidth = winRt.right + -winRt.left;
	LONG winHeight = winRt.bottom + -winRt.top;

	winRt.right -= winRt.left;
	winRt.left = 0;
	winRt.bottom = -winRt.top;
	winRt.top = 0;

	DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

	if (dwStyle & TABWINDOW_DEVBAR)
	{
		SendMessage(pTabWindow->hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(0, TABWINDOW_IR_DEVBAR_BOUNDS), (LPARAM)&drawRt);
		HBRUSH hBrush = CreateSolidBrush(pTabWindow->colors.headbar);
		FillRect(hDC, &drawRt, hBrush);
		DeleteBrush(hBrush);
	}

	hbBorder = CreateSolidBrush(pTabWindow->colors.tabBorder);

	if (dwStyle & TABWINDOW_BORDER)
	{
		// Draw left border
		drawRt.top = tabHeight;
		drawRt.bottom = winHeight;
		drawRt.left = 0;
		drawRt.right = pTabWindow->dims.tabBorder;
		FillRect(hDC, &drawRt, hbBorder);

		// Draw right border
		drawRt.top = tabHeight;
		drawRt.right = winWidth;
		drawRt.left = winWidth - pTabWindow->dims.tabBorder;
		FillRect(hDC, &drawRt, hbBorder);

		// Draw bottom border
		drawRt.top = winHeight - pTabWindow->dims.tabBorder;
		drawRt.bottom = winHeight;
		drawRt.left = 0;
		drawRt.right = winWidth;
		FillRect(hDC, &drawRt, hbBorder);
	}

	hbNormal = CreateSolidBrush(pTabWindow->colors.itemNormal);
	hbSelected = CreateSolidBrush(pTabWindow->colors.itemSelect);
	hbHot = CreateSolidBrush(pTabWindow->colors.itemHot);

	// Draw tabs //
	for (int i = 0; i < pTabWindow->nItems; i++)
	{
		tabItem = ctrl_TabWindow_GetItem(pTabWindow, i);
		if (tabItem)
		{
			SendMessage(pTabWindow->hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(i, TABWINDOW_IR_ITEM_BOUNDS), (LPARAM)&drawRt);

			if (i == pTabWindow->selectedItem)
				FillRect(hDC, &drawRt, hbSelected);
			else
			{
				if (i == pTabWindow->hot_itemI)
					FillRect(hDC, &drawRt, hbHot);
				else
					FillRect(hDC, &drawRt, hbNormal);
			}

			if (dwStyle & TABWINDOW_BORDER)
			{
				RECT borderRt;

				if (i == 0)
				{
					// Draw left tab item border
					borderRt.top = drawRt.top;
					borderRt.bottom = drawRt.bottom;
					borderRt.left = drawRt.left;
					borderRt.right = borderRt.left + pTabWindow->dims.tabBorder;
					FillRect(hDC, &borderRt, hbBorder);
				}

				// Draw right tab item border
				borderRt.top = drawRt.top;
				borderRt.bottom = drawRt.bottom;
				borderRt.right = drawRt.right;
				borderRt.left = borderRt.right - pTabWindow->dims.tabBorder;
				FillRect(hDC, &borderRt, hbBorder);

				// Draw top tab item border
				borderRt.top = drawRt.top;
				borderRt.bottom = drawRt.top + pTabWindow->dims.tabBorder;
				borderRt.left = drawRt.left;
				borderRt.right = drawRt.right;
				FillRect(hDC, &borderRt, hbBorder);

				if (i != pTabWindow->selectedItem)
				{
					// Draw bottom item border
					borderRt.bottom = drawRt.bottom;
					borderRt.top = borderRt.bottom - pTabWindow->dims.tabBorder;
					FillRect(hDC, &borderRt, hbBorder);
				}

				if ((i + 1) == pTabWindow->nItems)
				{
					// Draw border from last item to edge of control
					borderRt.left = drawRt.right;
					borderRt.right = winWidth;
					borderRt.bottom = drawRt.bottom;
					borderRt.top = borderRt.bottom - pTabWindow->dims.tabBorder;
					FillRect(hDC, &borderRt, hbBorder);
				}
			}

			SendMessage(pTabWindow->hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(i, TABWINDOW_IR_LABEL_BOUNDS), (LPARAM)&drawRt);

			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, pTabWindow->colors.text);

			UINT dtFormat = 0;

			if ((dwStyle & 0x0003) == TABWINDOW_NOPREFIX) {
				dtFormat |= DT_NOPREFIX;
			}
			else if ((dwStyle & 0x0003) == TABWINDOW_HIDEPREFIX) {
				dtFormat |= DT_HIDEPREFIX;
			}
			else if ((dwStyle & 0x0003) == TABWINDOW_PREFIXONLY) {
				dtFormat |= DT_PREFIXONLY;
			}

			if ((dwStyle & 0x000C) == TABWINDOW_WORD_ELLIPSIS) {
				dtFormat |= DT_WORD_ELLIPSIS;
			}
			else if ((dwStyle & 0x000C) == TABWINDOW_END_ELLIPSIS) {
				dtFormat |= DT_END_ELLIPSIS;
			}
			else if ((dwStyle & 0x000C) == TABWINDOW_PATH_ELLIPSIS) {
				dtFormat |= DT_PATH_ELLIPSIS;
			}

			if (dwStyle & TABWINDOW_RTLREADING) {
				dtFormat |= DT_RTLREADING;
			}

			hOldFont = (HFONT)SelectObject(hDC, pTabWindow->hFont);
			DrawTextEx(hDC, tabItem->userItem.pText, -1, &drawRt, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS | dtFormat, 0);
			if (hOldFont) {
				SelectObject(hDC, hOldFont);
			}

			TABWINDOW_BTN_INTERNAL* tabButton = pTabWindow->buttons;

			int b = 0;
			while (tabButton)
			{
				tabButton->userButton.buttonEventHandler(pTabWindow->hWnd, i, b, TABWINDOW_BT_PAINT, (WPARAM)hDC);
				tabButton = tabButton->next;
				b++;
			}
		}
	}

	DeleteBrush(hbHot);
	DeleteBrush(hbSelected);
	DeleteBrush(hbNormal);
	DeleteBrush(hbBorder);
}

LRESULT CALLBACK TabWindow_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	JUSTCTRL_TABWINDOW* pTabWindow;

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		CREATESTRUCT* ct = (CREATESTRUCT*)lParam;

		pTabWindow = (JUSTCTRL_TABWINDOW*)malloc(sizeof(JUSTCTRL_TABWINDOW));
		if (pTabWindow != 0)
		{
			memset(pTabWindow, 0, sizeof(JUSTCTRL_TABWINDOW));

			pTabWindow->hWnd = hWnd;
			pTabWindow->hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			pTabWindow->DPI = JustCtrl_GetDpiForWindow(hWnd);

			pTabWindow->dims.devBar = (int)MulDiv(2, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			pTabWindow->dims.tabInnerSpace = (int)MulDiv(4, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			pTabWindow->dims.tabMinWidth = (int)MulDiv(54, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);

			if (ct->style & TABWINDOW_BORDER) {
				pTabWindow->dims.tabBorder = (int)MulDiv(1, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			}

			ctrl_TabWindow_GetTabHeight(pTabWindow, false);
			pTabWindow->dims.btnWidth = pTabWindow->dims.txtHeight + (pTabWindow->dims.tabInnerSpace * 2);

			pTabWindow->colors.text = RGB(0, 0, 0);
			pTabWindow->colors.background = RGB(250, 250, 250);
			pTabWindow->colors.headbar = RGB(0, 162, 232);
			pTabWindow->colors.tabBorder = RGB(165, 167, 174);
			pTabWindow->colors.itemNormal = RGB(240, 240, 240);
			pTabWindow->colors.itemHot = RGB(216, 234, 249);
			pTabWindow->colors.itemPressed = RGB(0, 0, 0);
			pTabWindow->colors.itemSelect = RGB(250, 250, 250);
			pTabWindow->colors.itemDisbaled = RGB(236, 236, 236);

			pTabWindow->hot_itemI = -1;

			SetWindowLongPtr(hWnd, 0, (LONG_PTR)pTabWindow);
		}

		break;
	}
	case WM_DESTROY:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow != 0)
		{
			if (pTabWindow->bitmapBuffer) {
				DeleteObject(pTabWindow->bitmapBuffer);
			}
			if (pTabWindow->ncBitmapBuffer) {
				DeleteObject(pTabWindow->ncBitmapBuffer);
			}
			free(pTabWindow);
		}

		break;
	}
	case WM_SETFONT:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow != 0)
		{
			pTabWindow->hFont = (HFONT)wParam;
			pTabWindow->DPI = JustCtrl_GetDpiForWindow(hWnd);

			DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

			pTabWindow->dims.devBar = (int)MulDiv(2, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			pTabWindow->dims.tabInnerSpace = (int)MulDiv(4, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			pTabWindow->dims.tabMinWidth = (int)MulDiv(54, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);

			if (dwStyle & TABWINDOW_BORDER) {
				pTabWindow->dims.tabBorder = (int)MulDiv(1, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
			}

			ctrl_TabWindow_GetTabHeight(pTabWindow, true);
			pTabWindow->dims.btnWidth = pTabWindow->dims.txtHeight + (pTabWindow->dims.tabInnerSpace * 2);

			pTabWindow->btnTotalSpace = 0;
			TABWINDOW_BTN_INTERNAL* tabButton = pTabWindow->buttons;
			while (tabButton)
			{
				pTabWindow->btnTotalSpace +=
					(LONG)MulDiv(tabButton->userButton.leftSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
					(LONG)MulDiv(tabButton->userButton.rightSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
					pTabWindow->dims.btnWidth;
				tabButton = tabButton->next;
			}

			TABWINDOW_TAB_INTERNAL* thisTab = pTabWindow->items;
			while (thisTab)
			{
				thisTab->itemWidth = pTabWindow->dims.tabMinWidth;

				thisTab->txtMinSize = ctrl_TabWindow_GetTextMinimumSize(hWnd, pTabWindow->DPI, thisTab->userItem.pText);
				SIZE szNeed = thisTab->txtMinSize;
				szNeed.cx += (pTabWindow->dims.tabInnerSpace * 2);
				szNeed.cx += pTabWindow->btnTotalSpace;

				if (dwStyle & TABWINDOW_BORDER) {
					szNeed.cx += (pTabWindow->dims.tabBorder * 2);
				}

				if ((ULONG)szNeed.cx > thisTab->itemWidth)
					thisTab->itemWidth = szNeed.cx;

				thisTab = thisTab->next;
			}

			//SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
			RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
		}

		return 0;
	}
	case WM_GETFONT:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow != 0)
		{
			return (LRESULT)pTabWindow->hFont;
		}

		return NULL;
	}
	case WM_NCCALCSIZE:
	{
		LPNCCALCSIZE_PARAMS pncc = (LPNCCALCSIZE_PARAMS)lParam;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			pncc->rgrc[0].top += ctrl_TabWindow_GetTabHeight(pTabWindow, false);

			DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);
			if (dwStyle & TABWINDOW_BORDER) {
				pncc->rgrc[0].bottom -= pTabWindow->dims.tabBorder;
				pncc->rgrc[0].left += pTabWindow->dims.tabBorder;
				pncc->rgrc[0].right -= pTabWindow->dims.tabBorder;
			}
		}

		return 0;
	}
	case WM_NCPAINT:
	{
		HDC winDC = GetWindowDC(hWnd);

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow != NULL)
		{
			RECT winRt;
			GetWindowRect(hWnd, &winRt);

			int width = winRt.right - winRt.left;
			int height = winRt.bottom - winRt.top;

			HDC bufferDC = CreateCompatibleDC(winDC);
			HBITMAP hOldBmp = pTabWindow->ncBitmapBuffer;

			pTabWindow->ncBitmapBuffer = CreateCompatibleBitmap(winDC, width, height);

			HBITMAP hDefaultBmp = NULL;
			if (pTabWindow->ncBitmapBuffer) {
				hDefaultBmp = (HBITMAP)SelectObject(bufferDC, pTabWindow->ncBitmapBuffer);
				TabWindow_NonClient_OnPaint(pTabWindow, bufferDC);

				// We must be careful not to touch the client area

				// First draw BitBlt the Tab area
				int tabHeight = ctrl_TabWindow_GetTabHeight(pTabWindow, false);
				BitBlt(winDC, 0, 0, width, tabHeight, bufferDC, 0, 0, SRCCOPY);

				// Now the left, right and bottom borders
				if (pTabWindow->dims.tabBorder) {
					// Left
					BitBlt(winDC, 0, tabHeight, pTabWindow->dims.tabBorder, height, bufferDC, 0, tabHeight, SRCCOPY);
					// Right
					int x = width - pTabWindow->dims.tabBorder;
					BitBlt(winDC, x, tabHeight, pTabWindow->dims.tabBorder, height, bufferDC, x, tabHeight, SRCCOPY);
					// Bottom
					int y = height - pTabWindow->dims.tabBorder;
					BitBlt(winDC, 0, y, width, pTabWindow->dims.tabBorder, bufferDC, 0, y, SRCCOPY);
				}
			}

			if (hOldBmp) {
				DeleteObject(hOldBmp);
			}
			if (hDefaultBmp) {
				SelectObject(bufferDC, hDefaultBmp);
			}
			DeleteDC(bufferDC);
		}

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
		PAINTSTRUCT ps;
		HDC winDC = BeginPaint(hWnd, &ps);

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow != NULL)
		{
			HDC bufferDC = CreateCompatibleDC(winDC);
			HBITMAP hOldBmp = pTabWindow->bitmapBuffer;
			pTabWindow->bitmapBuffer = CreateCompatibleBitmap(winDC, ps.rcPaint.right, ps.rcPaint.bottom);

			HBITMAP hDefaultBmp = NULL;
			if (pTabWindow->bitmapBuffer) {
				hDefaultBmp = (HBITMAP)SelectObject(bufferDC, pTabWindow->bitmapBuffer);
				TabWindow_OnPaint(pTabWindow, bufferDC);
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

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow != NULL)
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
					HBITMAP hOldBmp = (HBITMAP)SelectObject(bufferDC, pTabWindow->bitmapBuffer);

					BitBlt(winDC, 0, 0, clientRt.right, clientRt.bottom, bufferDC, 0, 0, SRCCOPY);

					SelectObject(bufferDC, hOldBmp);
					DeleteDC(bufferDC);

					return 0;
				}
				else if ((lParam & PRF_NONCLIENT) == PRF_NONCLIENT)
				{
					RECT winRt;
					GetWindowRect(hWnd, &winRt);

					int width = winRt.right - winRt.left;
					int height = winRt.bottom - winRt.top;

					HDC bufferDC = CreateCompatibleDC(winDC);
					HBITMAP hOldBmp = (HBITMAP)SelectObject(bufferDC, pTabWindow->ncBitmapBuffer);

					// We must be careful not to touch the client area

					// First draw BitBlt the Tab area
					int tabHeight = ctrl_TabWindow_GetTabHeight(pTabWindow, false);
					BitBlt(winDC, 0, 0, width, tabHeight, bufferDC, 0, 0, SRCCOPY);

					// Now the left, right and bottom borders
					if (pTabWindow->dims.tabBorder) {
						// Left
						BitBlt(winDC, 0, tabHeight, pTabWindow->dims.tabBorder, height, bufferDC, 0, tabHeight, SRCCOPY);
						// Right
						int x = width - pTabWindow->dims.tabBorder;
						BitBlt(winDC, x, tabHeight, pTabWindow->dims.tabBorder, height, bufferDC, x, tabHeight, SRCCOPY);
						// Bottom
						int y = height - pTabWindow->dims.tabBorder;
						BitBlt(winDC, 0, y, width, pTabWindow->dims.tabBorder, bufferDC, 0, y, SRCCOPY);
					}

					SelectObject(bufferDC, hOldBmp);
					DeleteDC(bufferDC);

					return 0;
				}
			}
		}

		break;
	}
	case WM_NCHITTEST:
	{
		return HTCLIENT;
	}
	case WM_MOUSEMOVE:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			TABWINDOW_HITTESTINFO hitTestInfo;

			hitTestInfo.code = 0;
			hitTestInfo.pt.x = GET_X_LPARAM(lParam);
			hitTestInfo.pt.y = GET_Y_LPARAM(lParam);

			if (hitTestInfo.pt.y >= 0)
			{
				if (pTabWindow->hot_htCode != TABWINDOW_HT_NOWHERE)
				{
					pTabWindow->hot_itemI = -1;
					pTabWindow->hot_htCode = TABWINDOW_HT_NOWHERE;
					RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
				}
				return 0;
			}

			hitTestInfo.pt.x += pTabWindow->dims.tabBorder;
			hitTestInfo.pt.y = ctrl_TabWindow_GetTabHeight(pTabWindow, false) - -hitTestInfo.pt.y;

			if (!pTabWindow->trackingMouse)
			{
				TRACKMOUSEEVENT tme;

				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				tme.dwHoverTime = 0;

				TrackMouseEvent(&tme);
				pTabWindow->trackingMouse = true;
			}

			LONG i = (LONG)SendMessage(hWnd, TABWINDOW_HITTEST, 0, (LPARAM)&hitTestInfo);

			if ((pTabWindow->hot_htCode != hitTestInfo.code) || (i != pTabWindow->hot_itemI))
			{
				pTabWindow->hot_htCode = hitTestInfo.code;
				pTabWindow->hot_itemI = i;
				RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
			}
		}

		return 0;
	}
	case WM_LBUTTONUP:
	{
		bool lbuttonDown;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			lbuttonDown = pTabWindow->lbuttonDown;
			pTabWindow->lbuttonDown = false;

			TABWINDOW_HITTESTINFO hitTestInfo;

			hitTestInfo.code = 0;
			hitTestInfo.pt.x = GET_X_LPARAM(lParam);
			hitTestInfo.pt.y = GET_Y_LPARAM(lParam);

			if (hitTestInfo.pt.y >= 0)
				return 0;

			hitTestInfo.pt.x += pTabWindow->dims.tabBorder;
			hitTestInfo.pt.y = ctrl_TabWindow_GetTabHeight(pTabWindow, false) - -hitTestInfo.pt.y;

			LONG i = (LONG)SendMessage(hWnd, TABWINDOW_HITTEST, 0, (LPARAM)&hitTestInfo);

			if ((i != -1) && (lbuttonDown == true) && (pTabWindow->mbtn_ItemI == i) && (hitTestInfo.code == pTabWindow->mbtn_htCode))
			{
				if ((hitTestInfo.code == TABWINDOW_HT_BUTTON) && (hitTestInfo.b == pTabWindow->mbtn_ItemB))
				{
					TABWINDOW_BTN_INTERNAL* tabButton = pTabWindow->buttons;

					ULONG b = 0;
					while (tabButton)
					{
						if (b == hitTestInfo.b)
						{
							tabButton->userButton.buttonEventHandler(hWnd, i, b, TABWINDOW_BT_LCLICK, 0);
							break;
						}
						tabButton = tabButton->next;
						b++;
					}
				}
				else
				{
					if (pTabWindow->selectedItem != i)
					{
						NMHDR nmh;
						bool bStopChange;

						HWND hWndParent = GetParent(hWnd);
						int ctrlId = GetDlgCtrlID(hWnd);

						nmh.code = TABWINDOW_SELCHANGING;
						nmh.idFrom = ctrlId;
						nmh.hwndFrom = hWnd;
						bStopChange = (bool)SendMessage(hWndParent, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
						if (bStopChange)
							return 0;

						pTabWindow->prevSelected = pTabWindow->selectedItem;
						pTabWindow->selectedItem = i;
						RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

						nmh.code = TABWINDOW_SELCHANGE;
						nmh.idFrom = ctrlId;
						nmh.hwndFrom = hWnd;
						SendMessage(hWndParent, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
					}
				}
			}
		}

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			pTabWindow->lbuttonDown = true;
			SetFocus(hWnd);

			TABWINDOW_HITTESTINFO hitTestInfo;

			hitTestInfo.code = 0;
			hitTestInfo.pt.x = GET_X_LPARAM(lParam);
			hitTestInfo.pt.y = GET_Y_LPARAM(lParam);

			if (hitTestInfo.pt.y >= 0)
				return 0;

			hitTestInfo.pt.x += pTabWindow->dims.tabBorder;
			hitTestInfo.pt.y = ctrl_TabWindow_GetTabHeight(pTabWindow, false) - -hitTestInfo.pt.y;

			LONG i = (LONG)SendMessage(hWnd, TABWINDOW_HITTEST, 0, (LPARAM)&hitTestInfo);

			pTabWindow->mbtn_ItemI = i;
			pTabWindow->mbtn_ItemB = hitTestInfo.b;
			pTabWindow->mbtn_htCode = hitTestInfo.code;

			RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
		}

		return 0;
	}
	case WM_MOUSELEAVE:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			pTabWindow->lbuttonDown = false;
			pTabWindow->hot_htCode = TABWINDOW_HT_NOWHERE;
			pTabWindow->hot_itemI = -1;
			pTabWindow->trackingMouse = false;

			RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
		}

		return 0;
	}
	case WM_SETFOCUS:
	{
		//HWND hWndParent = GetParent(hWnd);
		//SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), TABWINDOW_SETFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case WM_KILLFOCUS:
	{
		//HWND hWndParent = GetParent(hWnd);
		//SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), TABWINDOW_KILLFOCUS), (LPARAM)hWnd);

		return 0;
	}
	case TABWINDOW_INSERTITEM:
	{
		WCHAR* pText;
		size_t strLength, strSize;
		TABWINDOW_TAB_INTERNAL* tabItem;
		SIZE szNeed;
		HWND parent_hWnd;
		UINT ctrlId;

		pText = (WCHAR*)lParam;
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			tabItem = ctrl_TabWindow_InsertItem(pTabWindow, (ULONG)wParam);
			if (tabItem)
			{
				strLength = wcslen(pText) + 1;
				strSize = strLength * sizeof(WCHAR);
				tabItem->userItem.pText = (WCHAR*)malloc(strSize);
				if (tabItem->userItem.pText != NULL)
					wcscpy_s(tabItem->userItem.pText, strLength, pText);

				tabItem->itemWidth = pTabWindow->dims.tabMinWidth;

				DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

				tabItem->txtMinSize = ctrl_TabWindow_GetTextMinimumSize(hWnd, pTabWindow->DPI, pText);
				szNeed = tabItem->txtMinSize;
				szNeed.cx += (pTabWindow->dims.tabInnerSpace * 2);
				szNeed.cx += pTabWindow->btnTotalSpace;

				if (dwStyle & TABWINDOW_BORDER) {
					szNeed.cx += (pTabWindow->dims.tabBorder * 2);
				}

				if ((ULONG)szNeed.cx > tabItem->itemWidth)
					tabItem->itemWidth = szNeed.cx;

				pTabWindow->nItems++;

				NMHDR nmh;
				bool bStopChange;

				parent_hWnd = GetParent(hWnd);
				ctrlId = GetDlgCtrlID(hWnd);

				nmh.code = TABWINDOW_SELCHANGING;
				nmh.idFrom = ctrlId;
				nmh.hwndFrom = hWnd;
				bStopChange = (bool)SendMessage(parent_hWnd, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
				if (bStopChange)
				{
					RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
					return (LRESULT)tabItem;
				}

				pTabWindow->prevSelected = pTabWindow->selectedItem;
				if ((int)wParam == -1)
					pTabWindow->selectedItem = pTabWindow->nItems - 1;
				else
					pTabWindow->selectedItem = (int)wParam;
				RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

				nmh.code = TABWINDOW_SELCHANGE;
				nmh.idFrom = ctrlId;
				nmh.hwndFrom = hWnd;
				SendMessage(parent_hWnd, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);

				RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

				return (LRESULT)tabItem;
			}
		}

		return NULL;
	}
	case TABWINDOW_DELETEITEM:
	{
		TABWINDOW_TAB_INTERNAL* tabItem;

		HWND parent_hWnd;
		UINT ctrlId;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			tabItem = ctrl_TabWindow_GetItem(pTabWindow, (ULONG)wParam);
			if (tabItem)
			{
				if (pTabWindow->selectedItem != (int)wParam)
				{
					ctrl_TabWindow_DeleteItem(pTabWindow, (ULONG)wParam);
					pTabWindow->nItems--;

					if (pTabWindow->selectedItem > (int)wParam)
						pTabWindow->selectedItem--;

					RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
				}
				else
				{
					NMHDR nmh;
					bool bStopChange;

					parent_hWnd = GetParent(hWnd);
					ctrlId = GetDlgCtrlID(hWnd);

					nmh.code = TABWINDOW_SELCHANGING;
					nmh.idFrom = ctrlId;
					nmh.hwndFrom = hWnd;
					bStopChange = (bool)SendMessage(parent_hWnd, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
					if (bStopChange)
						return 0;

					ctrl_TabWindow_DeleteItem(pTabWindow, (ULONG)wParam);
					pTabWindow->nItems--;

					if (pTabWindow->selectedItem != pTabWindow->prevSelected)
						pTabWindow->selectedItem = pTabWindow->prevSelected;
					else
					{
						if (((pTabWindow->selectedItem - 1) == -1) && (pTabWindow->nItems != 0))
						{
							pTabWindow->prevSelected = 0;
							pTabWindow->selectedItem = 0;
						}
						else
						{
							pTabWindow->prevSelected = pTabWindow->selectedItem - 1;
							pTabWindow->selectedItem = pTabWindow->selectedItem - 1;
						}
					}

					RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

					nmh.code = TABWINDOW_SELCHANGE;
					nmh.idFrom = ctrlId;
					nmh.hwndFrom = hWnd;
					SendMessage(parent_hWnd, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
				}

				return true;
			}
		}

		return false;
	}
	case TABWINDOW_GETITEMINDEX:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			return (LRESULT)ctrl_TabWindow_GetItemIndex(pTabWindow, (TABWINDOW_TAB_INTERNAL*)lParam);
		}

		return -1;
	}
	case TABWINDOW_FINDITEM:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			return (LRESULT)ctrl_TabWindow_FindItem(pTabWindow, (WCHAR*)lParam);
		}

		return NULL;
	}
	case TABWINDOW_SETTEXT:
	{
		WCHAR* pText;
		size_t strLength, strSize;
		TABWINDOW_TAB_INTERNAL* tabItem;
		SIZE szNeed;

		pText = (WCHAR*)lParam;
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			tabItem = ctrl_TabWindow_GetItem(pTabWindow, (ULONG)wParam);
			if (tabItem)
			{
				if (tabItem->userItem.pText != NULL)
					free(tabItem->userItem.pText);

				strLength = wcslen(pText) + 1;
				strSize = strLength * sizeof(WCHAR);
				tabItem->userItem.pText = (WCHAR*)malloc(strSize);
				if (tabItem->userItem.pText != NULL)
					wcscpy_s(tabItem->userItem.pText, strLength, pText);

				tabItem->itemWidth = pTabWindow->dims.tabMinWidth;

				DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

				tabItem->txtMinSize = ctrl_TabWindow_GetTextMinimumSize(hWnd, pTabWindow->DPI, pText);
				szNeed = tabItem->txtMinSize;
				szNeed.cx += (pTabWindow->dims.tabInnerSpace * 2);
				szNeed.cx += (pTabWindow->nButtons * pTabWindow->dims.btnWidth);

				TABWINDOW_BTN_INTERNAL* tabButton = pTabWindow->buttons;
				while (tabButton) {
					szNeed.cx +=
						(LONG)MulDiv(tabButton->userButton.leftSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
						(LONG)MulDiv(tabButton->userButton.rightSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
					tabButton = tabButton->next;
				}

				if ((ULONG)szNeed.cx > tabItem->itemWidth)
					tabItem->itemWidth = szNeed.cx;

				RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

				return true;
			}
		}

		return false;
	}
	case TABWINDOW_GETTABHEIGHT:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			return ctrl_TabWindow_GetTabHeight(pTabWindow, false);
		}

		return 0;
	}
	case TABWINDOW_SETTABDATA:
	{
		TABWINDOW_TAB_INTERNAL* tabItem;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			tabItem = ctrl_TabWindow_GetItem(pTabWindow, (ULONG)wParam);
			if (tabItem)
			{
				tabItem->userItem.userData = (void*)lParam;
				return true;
			}
		}

		return false;
	}
	case TABWINDOW_GETTABDATA:
	{
		TABWINDOW_TAB_INTERNAL* tabItem;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			tabItem = ctrl_TabWindow_GetItem(pTabWindow, (ULONG)wParam);
			if (tabItem)
			{
				if (lParam != NULL)
					*((void**)lParam) = tabItem->userItem.userData;

				return true;
			}
		}

		return false;
	}
	case TABWINDOW_GETHOTITEM:
	{
		LONG* pHotCode;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			pHotCode = (LONG*)lParam;
			if (pHotCode)
				*pHotCode = pTabWindow->hot_htCode;

			return pTabWindow->hot_itemI;
		}

		return -1;
	}
	case TABWINDOW_GETCURSEL:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			return pTabWindow->selectedItem;
		}

		return -1;
	}
	case TABWINDOW_SETCURSEL:
	{
		HWND parent_hWnd;
		UINT ctrlId;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			NMHDR nmh;
			bool bStopChange;

			parent_hWnd = GetParent(hWnd);
			ctrlId = GetDlgCtrlID(hWnd);

			nmh.code = TABWINDOW_SELCHANGING;
			nmh.idFrom = ctrlId;
			nmh.hwndFrom = hWnd;
			bStopChange = (bool)SendMessage(parent_hWnd, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
			if (bStopChange)
				return false;

			pTabWindow->prevSelected = pTabWindow->selectedItem;
			pTabWindow->selectedItem = (int)wParam;
			RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

			nmh.code = TABWINDOW_SELCHANGE;
			nmh.idFrom = ctrlId;
			nmh.hwndFrom = hWnd;
			SendMessage(parent_hWnd, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);

			return true;
		}

		return false;
	}
	case TABWINDOW_GETITEMRECT:
	{
		RECT irt;
		RECT winRt;
		RECT* pResultRt;
		ULONG i, b, code;
		ULONG cnt;

		TABWINDOW_TAB_INTERNAL* thisItem, * nextItem;

		pResultRt = (RECT*)lParam;
		i = LOWORD(wParam);
		code = HIWORD(wParam);

		b = 0;
		if (code >= TABWINDOW_IR_BUTTON_BOUNDS) {
			b = code - TABWINDOW_IR_BUTTON_BOUNDS;
			code -= b;
		}

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			irt.left = 0;
			irt.right = 0;
			irt.top = 0;
			irt.bottom = 0;

			GetWindowRect(hWnd, &winRt);
			MapWindowPoints(NULL, hWnd, (POINT*)&winRt, 2);

			DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

			switch (code)
			{
			case TABWINDOW_IR_ITEM_BOUNDS:
			{
				thisItem = pTabWindow->items;
				irt.bottom = -winRt.top;

				if (dwStyle & TABWINDOW_DEVBAR) {
					irt.bottom -= pTabWindow->dims.devBar;
				}

				cnt = 0;
				while (thisItem)
				{
					nextItem = thisItem->next;
					irt.right = irt.left + thisItem->itemWidth;
					if (cnt == i) break;
					irt.left += thisItem->itemWidth;
					thisItem = nextItem;
					cnt++;
				}

				break;
			}
			case TABWINDOW_IR_LABEL_BOUNDS:
			{
				thisItem = pTabWindow->items;
				irt.bottom = -winRt.top;

				if (dwStyle & TABWINDOW_DEVBAR) {
					irt.bottom -= pTabWindow->dims.devBar;
				}

				cnt = 0;
				while (thisItem)
				{
					nextItem = thisItem->next;
					irt.right = irt.left + thisItem->itemWidth;
					if (cnt == i)
					{
						irt.right -= pTabWindow->btnTotalSpace;
						irt.top += pTabWindow->dims.tabInnerSpace;
						irt.bottom -= pTabWindow->dims.tabInnerSpace;
						irt.left += pTabWindow->dims.tabInnerSpace;
						irt.right -= pTabWindow->dims.tabInnerSpace;
						if (dwStyle & TABWINDOW_BORDER) {
							irt.top += pTabWindow->dims.tabBorder;
							irt.bottom -= pTabWindow->dims.tabBorder;
							irt.left += pTabWindow->dims.tabBorder;
						}
						break;
					}
					irt.left += thisItem->itemWidth;
					thisItem = nextItem;
					cnt++;
				}

				break;
			}
			case TABWINDOW_IR_DEVBAR_BOUNDS:
			{
				if (!(dwStyle & TABWINDOW_DEVBAR)) {
					memset(&irt, 0, sizeof(RECT));
				}
				else {
					irt.left = 0;
					irt.right = winRt.right + -winRt.left;
					irt.bottom = ctrl_TabWindow_GetTabHeight(pTabWindow, false);
					irt.top = irt.bottom - pTabWindow->dims.devBar;
				}

				break;
			}
			case TABWINDOW_IR_BUTTON_BOUNDS:
			{
				thisItem = pTabWindow->items;
				irt.bottom = -winRt.top;

				if (dwStyle & TABWINDOW_DEVBAR) {
					irt.bottom -= pTabWindow->dims.devBar;
				}

				if (dwStyle & TABWINDOW_BORDER) {
					irt.top += pTabWindow->dims.tabBorder;
					irt.bottom -= pTabWindow->dims.tabBorder;
				}

				cnt = 0;
				while (thisItem)
				{
					nextItem = thisItem->next;
					irt.right = irt.left + thisItem->itemWidth;
					if (cnt == i)
					{
						TABWINDOW_BTN_INTERNAL* tabButton = pTabWindow->buttons;

						ULONG btn = 0;
						irt.left += (pTabWindow->dims.tabInnerSpace * 2);
						while (tabButton)
						{
							if (btn == b)
							{
								irt.left += thisItem->txtMinSize.cx + (LONG)MulDiv(tabButton->userButton.leftSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI);
								if (dwStyle & TABWINDOW_BORDER) {
									irt.left += pTabWindow->dims.tabBorder;
								}
								irt.right = irt.left + pTabWindow->dims.btnWidth;
								break;
							}
							irt.left +=
								(LONG)MulDiv(tabButton->userButton.leftSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
								(LONG)MulDiv(tabButton->userButton.rightSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) + pTabWindow->dims.btnWidth;
							tabButton = tabButton->next;
							btn++;
						}
						break;
					}
					irt.left += thisItem->itemWidth;

					thisItem = nextItem;
					cnt++;
				}

				break;
			}
			default:
				break;
			}

			*pResultRt = irt;

			return true;
		}

		return false;
	}
	case TABWINDOW_HITTEST:
	{
		bool success;
		TABWINDOW_HITTESTINFO* pHitTestInfo;
		RECT itemRt;

		pHitTestInfo = (TABWINDOW_HITTESTINFO*)lParam;
		pHitTestInfo->code = TABWINDOW_HT_NOWHERE;

		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

			for (LONG i = 0; i < pTabWindow->nItems; i++)
			{
				for (LONG b = 0; b < pTabWindow->nButtons; b++)
				{
					success = SendMessage(hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(i, TABWINDOW_IR_BUTTON_BOUNDS + b), (LPARAM)&itemRt);
					if (success)
					{
						if ((pHitTestInfo->pt.x >= itemRt.left) && (pHitTestInfo->pt.x <= itemRt.right) &&
							(pHitTestInfo->pt.y >= itemRt.top) && (pHitTestInfo->pt.y <= itemRt.bottom))
						{
							pHitTestInfo->b = b;
							pHitTestInfo->code = TABWINDOW_HT_BUTTON;
							return i;
						}
					}
				}

				success = SendMessage(hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(i, TABWINDOW_IR_LABEL_BOUNDS), (LPARAM)&itemRt);
				if (success)
				{
					if ((pHitTestInfo->pt.x >= itemRt.left) && (pHitTestInfo->pt.x <= itemRt.right) &&
						(pHitTestInfo->pt.y >= itemRt.top) && (pHitTestInfo->pt.y <= itemRt.bottom))
					{
						pHitTestInfo->code = TABWINDOW_HT_LABEL;
						return i;
					}
				}

				success = SendMessage(hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(i, TABWINDOW_IR_ITEM_BOUNDS), (LPARAM)&itemRt);
				if (success)
				{
					if ((pHitTestInfo->pt.x >= itemRt.left) && (pHitTestInfo->pt.x <= itemRt.right) &&
						(pHitTestInfo->pt.y >= itemRt.top) && (pHitTestInfo->pt.y <= itemRt.bottom))
					{
						pHitTestInfo->code = TABWINDOW_HT_ITEM;
						return i;
					}
				}
			}

			success = SendMessage(hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(0, TABWINDOW_IR_DEVBAR_BOUNDS), (LPARAM)&itemRt);
			if (success)
			{
				if ((pHitTestInfo->pt.x >= itemRt.left) && (pHitTestInfo->pt.x <= itemRt.right) &&
					(pHitTestInfo->pt.y >= itemRt.top) && (pHitTestInfo->pt.y <= itemRt.bottom))
				{
					pHitTestInfo->code = TABWINDOW_HT_DEVBAR;
					return -1;
				}
			}
		}

		return -1;
	}
	case TABWINDOW_CREATEBUTTON:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			TABWINDOW_BTN_INTERNAL* tabButton = ctrl_TabWindow_InsertButton(pTabWindow, (ULONG)wParam);
			if (tabButton)
			{
				TABWINDOW_BUTTON* pUserButton = (TABWINDOW_BUTTON*)lParam;

				memcpy(&tabButton->userButton, pUserButton, sizeof(TABWINDOW_BUTTON));
				pTabWindow->nButtons++;

				DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

				pTabWindow->btnTotalSpace = 0;
				TABWINDOW_BTN_INTERNAL* thisButton = pTabWindow->buttons;
				while (thisButton)
				{
					pTabWindow->btnTotalSpace +=
						(LONG)MulDiv(thisButton->userButton.leftSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
						(LONG)MulDiv(thisButton->userButton.rightSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
						pTabWindow->dims.btnWidth;
					thisButton = thisButton->next;
				}

				TABWINDOW_TAB_INTERNAL* thisTab = pTabWindow->items;
				while (thisTab)
				{
					thisTab->itemWidth = pTabWindow->dims.tabMinWidth;

					thisTab->txtMinSize = ctrl_TabWindow_GetTextMinimumSize(hWnd, pTabWindow->DPI, thisTab->userItem.pText);
					SIZE szNeed = thisTab->txtMinSize;
					szNeed.cx += (pTabWindow->dims.tabInnerSpace * 2);
					szNeed.cx += pTabWindow->btnTotalSpace;

					if (dwStyle & TABWINDOW_BORDER) {
						szNeed.cx += (pTabWindow->dims.tabBorder * 2);
					}

					if ((ULONG)szNeed.cx > thisTab->itemWidth)
						thisTab->itemWidth = szNeed.cx;

					thisTab = thisTab->next;
				}

				RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

				return (LRESULT)tabButton;
			}
		}

		return NULL;
	}
	case TABWINDOW_DELETEBUTTON:
	{
		pTabWindow = (JUSTCTRL_TABWINDOW*)GetWindowLongPtr(hWnd, 0);
		if (pTabWindow)
		{
			if (ctrl_TabWindow_DeleteButton(pTabWindow, (ULONG)wParam))
			{
				pTabWindow->nButtons--;

				DWORD dwStyle = (DWORD)GetWindowLongPtr(pTabWindow->hWnd, GWL_STYLE);

				pTabWindow->btnTotalSpace = 0;
				TABWINDOW_BTN_INTERNAL* thisButton = pTabWindow->buttons;
				while (thisButton)
				{
					pTabWindow->btnTotalSpace +=
						(LONG)MulDiv(thisButton->userButton.leftSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
						(LONG)MulDiv(thisButton->userButton.rightSpace, pTabWindow->DPI, JUSTCTRL_APPLICATION_DPI) +
						pTabWindow->dims.btnWidth;
					thisButton = thisButton->next;
				}

				TABWINDOW_TAB_INTERNAL* thisTab = pTabWindow->items;
				while (thisTab)
				{
					thisTab->itemWidth = pTabWindow->dims.tabMinWidth;

					thisTab->txtMinSize = ctrl_TabWindow_GetTextMinimumSize(hWnd, pTabWindow->DPI, thisTab->userItem.pText);
					SIZE szNeed = thisTab->txtMinSize;
					szNeed.cx += (pTabWindow->dims.tabInnerSpace * 2);
					szNeed.cx += pTabWindow->btnTotalSpace;

					if (dwStyle & TABWINDOW_BORDER) {
						szNeed.cx += (pTabWindow->dims.tabBorder * 2);
					}

					if ((ULONG)szNeed.cx > thisTab->itemWidth)
						thisTab->itemWidth = szNeed.cx;

					thisTab = thisTab->next;
				}

				RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);

				return true;
			}
		}

		return false;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool WINAPI JustCtrl_InitTabWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	// Register the JustCtrl_TabWindow class.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = TabWindow_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"JustCtrl_TabWindow";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"Failed to register the JustCtrl_TabWindow class.", L"Error!", MB_OK);
		return false;
	}

	return true;
}
