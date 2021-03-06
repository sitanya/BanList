#pragma once

#define  ID_STATIC_QQ      1001
#define  ID_STATIC_MSG     1002

#define  ID_EDIT_QQ        2001   
#define  ID_EDIT_GROUP        2002

#define  ID_EDIT_QQ_MSG	2003
#define  ID_EDIT_GROUP_MSG 2004
#define  ID_EDIT_QQ_BAN_MSG	2005
#define  ID_EDIT_GROUP_BAN_MSG 2006

#define  ID_BUTTON_SAVE 3001

#define  ID_LeaveBanGroup  4001
#define  ID_refuseGroup  4002
#define  ID_BanDetachOperator  4003
#define  ID_deleteDetachOperator  4004
#define  ID_agreeGroup  4005
#define  ID_LeaveForbiddenGroup  4006
#define  ID_BanForbiddenGroup  4007
#define  ID_DontRunningInBanGroup  4008
#define  ID_BanDetachGroup 4009

#define  ID_LeaveGroupByUser  5001
#define  ID_DontRunningInBanGroupForUser  5002
#define  ID_refuseBanUser  5003
#define  ID_agreeUser  5004

#define  DEFAULT_DPIX      96
#define  DEFAULT_DPIY      96


LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);

int ShowMainWindow(int32_t AuthCode);