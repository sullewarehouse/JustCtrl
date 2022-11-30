
//
// radiobutton.h
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The header file for the JustCtrl_RadioButton control.
//

#ifndef JUSTCTRL_RADIOBUTTON_H
#define JUSTCTRL_RADIOBUTTON_H

#include "JustCtrl.h"

//
// Radio Button Control Styles
//

#define RADIOBUTTON_LEFT              0x0000
#define RADIOBUTTON_CENTER            0x0001
#define RADIOBUTTON_RIGHT             0x0002
#define RADIOBUTTON_TOP               0x0000
#define RADIOBUTTON_VCENTER           0x0004
#define RADIOBUTTON_BOTTOM            0x0008
#define RADIOBUTTON_NOPREFIX          0x0010
#define RADIOBUTTON_HIDEPREFIX        0x0020
#define RADIOBUTTON_PREFIXONLY        0x0030
#define RADIOBUTTON_AUTO              0x0040
#define RADIOBUTTON_RTLREADING        0x0200

//
// Radio Button Control States
//

#define RADIOBUTTON_UNCHECKED         0x0000
#define RADIOBUTTON_CHECKED           0x0001

//
// Radio Button Control Notification Codes
//

#define RADIOBUTTON_CLICKED           0x0000
#define RADIOBUTTON_KILLFOCUS         0x0001
#define RADIOBUTTON_SETFOCUS          0x0002

//
// Radio Button Control Messages
//

#define RADIOBUTTON_GETCHECK          (WM_USER + 0x0001)
#define RADIOBUTTON_SETCHECK          (WM_USER + 0x0002)

//
// Radio Button Macros
//

#define RadioButton_GetCheck(hWnd) SendMessage(hWnd, RADIOBUTTON_GETCHECK, 0, 0)
#define RadioButton_SetCheck(hWnd, state, redraw) SendMessage(hWnd, RADIOBUTTON_SETCHECK, MAKEWPARAM(state, redraw), 0)

//
// Radio Button Control Functions
//

bool WINAPI JustCtrl_InitRadioButton(HINSTANCE hInstance);
SIZE WINAPI JustCtrl_GetRadioButtonMinimumSize(HWND hWnd);

#endif // !JUSTCTRL_RADIOBUTTON_H
