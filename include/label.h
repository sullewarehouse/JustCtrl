
//
// label.h
// 
// Author:
//     Brian Sullender
//     SULLE WAREHOUSE LLC
// 
// Description:
//     The header file for the JustCtrl_Label control.
//

#ifndef JUSTCTRL_LABEL_H
#define JUSTCTRL_LABEL_H

#include "JustCtrl.h"

//
// Label Control Styles
//

#define LABEL_LEFT              0x0000
#define LABEL_CENTER            0x0001
#define LABEL_RIGHT             0x0002
#define LABEL_TOP               0x0000
#define LABEL_VCENTER           0x0004
#define LABEL_BOTTOM            0x0008
#define LABEL_NOPREFIX          0x0010
#define LABEL_HIDEPREFIX        0x0020
#define LABEL_PREFIXONLY        0x0030
#define LABEL_WORD_ELLIPSIS     0x0040
#define LABEL_END_ELLIPSIS      0x0080
#define LABEL_PATH_ELLIPSIS     0x00C0
#define LABEL_WORDBREAK         0x0100
#define LABEL_RTLREADING        0x0200

//
// Label Control Functions
//

bool WINAPI JustCtrl_InitLabel(HINSTANCE hInstance);
SIZE WINAPI JustCtrl_GetLabelMinimumSize(HWND hWnd);

#endif // !JUSTCTRL_LABEL_H
