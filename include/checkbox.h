
//
// checkbox.h
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The header file for the JustCtrl_Checkbox control.
//

#ifndef JUSTCTRL_CHECKBOX_H
#define JUSTCTRL_CHECKBOX_H

#include "JustCtrl.h"

//
// Checkbox Control Styles
//

#define CHECKBOX_LEFT              0x0000
#define CHECKBOX_CENTER            0x0001
#define CHECKBOX_RIGHT             0x0002
#define CHECKBOX_TOP               0x0000
#define CHECKBOX_VCENTER           0x0004
#define CHECKBOX_BOTTOM            0x0008
#define CHECKBOX_NOPREFIX          0x0010
#define CHECKBOX_HIDEPREFIX        0x0020
#define CHECKBOX_PREFIXONLY        0x0030
#define CHECKBOX_AUTO              0x0040
#define CHECKBOX_3STATE            0x0080
#define CHECKBOX_RTLREADING        0x0200

//
// Checkbox Control States
//

#define CHECKBOX_UNCHECKED         0x0000
#define CHECKBOX_CHECKED           0x0001
#define CHECKBOX_INDETERMINATE     0x0002

//
// Checkbox Control Notification Codes
//

#define CHECKBOX_CLICKED           0x0000
#define CHECKBOX_KILLFOCUS         0x0001
#define CHECKBOX_SETFOCUS          0x0002

//
// Checkbox Control Messages
//

#define CHECKBOX_GETCHECK          (WM_USER + 0x0001)
#define CHECKBOX_SETCHECK          (WM_USER + 0x0002)

//
// Checkbox Macros
//

#define Checkbox_GetCheck(hWnd) SendMessage(hWnd, CHECKBOX_GETCHECK, 0, 0)
#define Checkbox_SetCheck(hWnd, state, redraw) SendMessage(hWnd, CHECKBOX_SETCHECK, MAKEWPARAM(state, redraw), 0)

//
// Checkbox Control Functions
//

bool WINAPI JustCtrl_InitCheckbox(HINSTANCE hInstance);
SIZE WINAPI JustCtrl_GetCheckboxMinimumSize(HWND hWnd);

#endif // !JUSTCTRL_CHECKBOX_H
