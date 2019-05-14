#pragma once

#define  ID_STATIC_QQ      1001
#define  ID_STATIC_MSG     1002
#define  ID_EDIT_QQ        2001   
#define  ID_EDIT_MSG       2002
#define  ID_BUTTON_QQ    3001
#define  ID_BUTTON_Group   3002

#define  DEFAULT_DPIX      96
#define  DEFAULT_DPIY      96


LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);

int ShowMainWindow(int32_t AuthCode);