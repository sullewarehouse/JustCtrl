
//
// tabwindow.h
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The header file for the JustCtrl_TabWindow control.
//

#ifndef JUSTCTRL_TABWINDOW_H
#define JUSTCTRL_TABWINDOW_H

#include "JustCtrl.h"

//
// Tab Window Control Styles
//

#define TABWINDOW_NOPREFIX          0x0001
#define TABWINDOW_HIDEPREFIX        0x0002
#define TABWINDOW_PREFIXONLY        0x0003
#define TABWINDOW_WORD_ELLIPSIS     0x0004
#define TABWINDOW_END_ELLIPSIS      0x0008
#define TABWINDOW_PATH_ELLIPSIS     0x000C
#define TABWINDOW_BORDER            0x0010
#define TABWINDOW_DEVBAR            0x0020
#define TABWINDOW_RTLREADING        0x0200

//
// Tab Window control notification codes
//

#define TABWINDOW_SELCHANGE                  0
#define TABWINDOW_SELCHANGING                1

//
// Tab Window control hit test codes
//

#define TABWINDOW_HT_NOWHERE                 0
#define TABWINDOW_HT_ITEM                    1
#define TABWINDOW_HT_LABEL                   2
#define TABWINDOW_HT_BUTTON                  3
#define TABWINDOW_HT_DEVBAR                  4

//
// Tab Window control GETITEMRECT portion codes
//

#define TABWINDOW_IR_ITEM_BOUNDS             0
#define TABWINDOW_IR_LABEL_BOUNDS            1
#define TABWINDOW_IR_DEVBAR_BOUNDS           2
#define TABWINDOW_IR_BUTTON_BOUNDS           9

//
// Tab Window button event codes
//

#define TABWINDOW_BT_PAINT                   0
#define TABWINDOW_BT_LCLICK                  1
#define TABWINDOW_BT_RCLICK                  2

//
// Tab Window control messages
//

#define TABWINDOW_INSERTITEM                 (WM_USER + 0x0001)
#define TABWINDOW_DELETEITEM                 (WM_USER + 0x0002)
#define TABWINDOW_GETITEMINDEX               (WM_USER + 0x0003)
#define TABWINDOW_FINDITEM                   (WM_USER + 0x0004)
#define TABWINDOW_SETTEXT                    (WM_USER + 0x0005)
#define TABWINDOW_GETTABHEIGHT               (WM_USER + 0x0006)
#define TABWINDOW_SETTABDATA                 (WM_USER + 0x0007)
#define TABWINDOW_GETTABDATA                 (WM_USER + 0x0008)
#define TABWINDOW_GETHOTITEM                 (WM_USER + 0x0009)
#define TABWINDOW_GETCURSEL                  (WM_USER + 0x000A)
#define TABWINDOW_SETCURSEL                  (WM_USER + 0x000B)
#define TABWINDOW_GETITEMRECT                (WM_USER + 0x000C)
#define TABWINDOW_HITTEST                    (WM_USER + 0x000D)
#define TABWINDOW_CREATEBUTTON               (WM_USER + 0x000E)
#define TABWINDOW_DELETEBUTTON               (WM_USER + 0x000F)

//
// Tab Window Macros
//

#define TabWindow_InsertTab(hWnd, Index, pText) \
SendMessage(hWnd, TABWINDOW_INSERTITEM, (WPARAM)Index, (LPARAM)pText)

#define TabWindow_DeleteItem(hWnd, Index) \
SendMessage(hWnd, TABWINDOW_DELETEITEM, (WPARAM)Index)

#define TabWindow_GetItemIndex(hWnd, pTabItem) \
SendMessage(hWnd, TABWINDOW_GETITEMINDEX, 0, (LPARAM)pTabItem)

#define TabWindow_FindItem(hWnd, pText) \
SendMessage(hWnd, TABWINDOW_FINDITEM, 0, (LPARAM)pText)

#define TabWindow_SetText(hWnd, Index, pText) \
SendMessage(hWnd, TABWINDOW_SETTEXT, (WPARAM)Index, (LPARAM)pText)

#define TabWindow_GetTabHeight(hWnd) \
SendMessage(hWnd, TABWINDOW_GETTABHEIGHT, 0, 0)

#define TabWindow_SetTabData(hWnd, Index, pData) \
SendMessage(hWnd, TABWINDOW_SETTABDATA, (WPARAM)Index, (LPARAM)pData)

#define TabWindow_GetTabData(hWnd, Index, ppData) \
SendMessage(hWnd, TABWINDOW_GETTABDATA, (WPARAM)Index, (LPARAM)ppData)

#define TabWindow_GetHotItem(hWnd, ppHotCode) \
SendMessage(hWnd, TABWINDOW_GETHOTITEM, 0, (LPARAM)ppHotCode)

#define TabWindow_GetCurSel(hWnd) \
SendMessage(hWnd, TABWINDOW_GETCURSEL, 0, 0)

#define TabWindow_SetCurSel(hWnd, Index) \
SendMessage(hWnd, TABWINDOW_SETCURSEL, (WPARAM)Index, 0)

#define TabWindow_GetItemRect(hWnd, Index, Code, pRect) \
SendMessage(hWnd, TABWINDOW_GETITEMRECT, MAKEWPARAM(Index, Code), (LPARAM)pRect)

#define TabWindow_HitTest(hWnd, pHitTest) \
SendMessage(hWnd, TABWINDOW_HITTEST, 0, (LPARAM)pHitTest)

#define TabWindow_CreateButton(hWnd, Index, pTabButton) \
SendMessage(hWnd, TABWINDOW_CREATEBUTTON, (WPARAM)Index, (LPARAM)pTabButton)

#define TabWindow_DeleteButton(hWnd, Index) \
SendMessage(hWnd, TABWINDOW_DELETEBUTTON, (WPARAM)Index, 0)

//
// Tab Window item struct
//

struct TABWINDOW_ITEM
{
	WCHAR* pText;
	void* userData;
};

//
// Tab Window button struct
//

struct TABWINDOW_BUTTON
{
	void (*buttonEventHandler)(HWND hWnd, ULONG tabIndex, ULONG btnIndex, ULONG code, WPARAM wParam);
	int leftSpace;
	int rightSpace;
};

//
// Tab Window hit test struct.
//

struct TABWINDOW_HITTESTINFO
{
	POINT pt;
	ULONG code;
	ULONG b;
};

//
// Tab Window Control Functions
//

bool WINAPI JustCtrl_InitTabWindow(HINSTANCE hInstance);

#endif // !JUSTCTRL_TABWINDOW_H
