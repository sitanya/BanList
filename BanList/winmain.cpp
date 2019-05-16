#include "stdafx.h"
#include "winmain.h"
#include "CQAPI.h"
#include <Windows.h>
#include "BanList.h"
#include <map>
#include <CString>

using namespace std;

int auth = -1;

int g_dpix = DEFAULT_DPIX;
int g_dpiy = DEFAULT_DPIY;

#define DPIX(value) (MulDiv(value, g_dpix, DEFAULT_DPIX))
#define DPIY(value) (MulDiv(value, g_dpiy, DEFAULT_DPIY))

HINSTANCE g_hInstance = GetModuleHandle(NULL);

void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

int ShowMainWindow(int32_t AuthCode)
{
	auth = AuthCode;
	// 获取DPI
	HDC hdcDpi = GetDC(NULL);
	if (hdcDpi != NULL) {
		g_dpix = GetDeviceCaps(hdcDpi, LOGPIXELSX);
		g_dpiy = GetDeviceCaps(hdcDpi, LOGPIXELSY);
		ReleaseDC(NULL, hdcDpi);
	}
	// 类名  
	//wchar *cls_Name = "";
	const wchar_t* cls_Name = TEXT("MainWindow");
	//TCHAR* test = _T("窗口标题");
	// 设计窗口类  
	WNDCLASS wc = {};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(g_hInstance, IDC_ARROW);
	wc.hIcon = LoadIcon(g_hInstance, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = cls_Name;
	wc.hInstance = g_hInstance;
	// 注册窗口类  
	RegisterClass(&wc);

	// 创建窗口  
	HWND hwnd = CreateWindow(
		cls_Name,           //类名，要和刚才注册的一致  
		TEXT("黑名单"),          //窗口标题文字  
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,        //窗口外观样式  
		DPIX(38),             //窗口相对于父级的X坐标  
		DPIY(20),             //窗口相对于父级的Y坐标  
		DPIX(2000),                //窗口的宽度  
		DPIY(3800),                //窗口的高度  
		NULL,               //没有父窗口，为NULL  
		NULL,               //没有菜单，为NULL  
		g_hInstance,          //当前应用程序的实例句柄  
		NULL);              //没有附加数据，为NULL  
	if (hwnd == NULL)                //检查窗口是否创建成功  
		return 0;
	// 显示窗口  
	ShowWindow(hwnd, SW_SHOW);
	// 更新窗口  
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
)
{
	// 声明控件
	static HWND hEditQQText;
	static HWND hEditGroupText;
	static HWND hGroupMsgText, hGroupMsgStatic;
	static HWND hQQMsgText, hQQMsgStatic;
	static HWND hQQBanMsgStatic, hQQBanMsgText;
	static HWND hGroupBanMsgText, hGroupBanMsg;
	static HWND hSaveButton;
	static HWND hLeaveGroupButton;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		TCHAR Msg[500];
		map<string, string> Messages = getMSG();
		HFONT hFontRegular, hFontBold;
		HDC hDC;
		hDC = GetDC(hwnd);
		hFontRegular = CreateFont(DPIY(15), 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("微软雅黑"));
		hFontBold = CreateFont(DPIY(15), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("微软雅黑"));
		SelectObject(hDC, hFontRegular);

		// ID_EDIT_QQ
		// 目标 QQ 号码 文本框（只能输入数字）
		hEditQQText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER | WS_BORDER,
			DPIX(30), DPIY(20), DPIX(150), DPIY(25),
			hwnd, (HMENU)ID_EDIT_QQ, g_hInstance, NULL);
		SendMessage(hEditQQText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end()) {
			CharToTchar(Messages["MASTER"].c_str(), Msg);
			SendMessage(hEditQQText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}


		// ID_EDIT_QQ
	// 目标 QQ 号码 文本框（只能输入数字）
		hEditGroupText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER | WS_BORDER,
			DPIX(30), DPIY(50), DPIX(150), DPIY(20),
			hwnd, (HMENU)ID_EDIT_GROUP, g_hInstance, NULL);
		SendMessage(hEditGroupText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end()) {
			CharToTchar(Messages["MASTERGroup"].c_str(), Msg);
			SendMessage(hEditGroupText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}


		// ID_STATIC_MSG
		// 发送的消息内容 标签提示
		hGroupMsgStatic = CreateWindow(TEXT("Static"), TEXT("入群信息："),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(300), DPIY(20), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hGroupMsgStatic, WM_SETFONT, (WPARAM)hFontBold, 1);


		// ID_EDIT_MSG
		// 发送的消息内容 文本框（多行）
		hGroupMsgText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(300), DPIY(40), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_GROUP_MSG, g_hInstance, NULL);
		SendMessage(hGroupMsgText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end()) {
			CharToTchar(Messages["GroupMsg"].c_str(), Msg);
			SendMessage(hGroupMsgText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_STATIC_MSG
	// 发送的消息内容 标签提示
		hQQMsgStatic = CreateWindow(TEXT("Static"), TEXT("私聊信息："),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(300), DPIY(270), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hQQMsgStatic, WM_SETFONT, (WPARAM)hFontBold, 1);


		// ID_EDIT_MSG
		// 发送的消息内容 文本框（多行）
		hQQMsgText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(300), DPIY(300), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_QQ_MSG, g_hInstance, NULL);
		SendMessage(hQQMsgText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end()) {
			CharToTchar(Messages["QQMsg"].c_str(), Msg);
			SendMessage(hQQMsgText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_STATIC_MSG
// 发送的消息内容 标签提示
		hQQBanMsgStatic = CreateWindow(TEXT("Static"), TEXT("拦截QQ黑名单信息："),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(550), DPIY(20), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hQQBanMsgStatic, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// 发送的消息内容 文本框（多行）
		hQQBanMsgText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(550), DPIY(40), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_QQ_BAN_MSG, g_hInstance, NULL);
		SendMessage(hQQBanMsgText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end()) {
			CharToTchar(Messages["BanQQMsg"].c_str(), Msg);
			SendMessage(hQQBanMsgText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_STATIC_MSG
// 发送的消息内容 标签提示
		hGroupBanMsgText = CreateWindow(TEXT("Static"), TEXT("拦截群黑名单信息："),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(550), DPIY(270), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hGroupBanMsgText, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// 发送的消息内容 文本框（多行）
		hGroupBanMsg = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(550), DPIY(300), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_GROUP_BAN_MSG, g_hInstance, NULL);
		SendMessage(hGroupBanMsg, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end()) {
			CharToTchar(Messages["BanGroupMsg"].c_str(), Msg);
			SendMessage(hGroupBanMsg, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}



		// ID_STATIC_QQ
// 目标 QQ 号码 标签提示
		hSaveButton = CreateWindow(TEXT("Button"), TEXT("保存到云端"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(300), DPIY(600), DPIX(90), DPIY(20),
			hwnd, (HMENU)ID_BUTTON_SAVE, g_hInstance, NULL);
		SendMessage(hSaveButton, WM_SETFONT, (WPARAM)hFontBold, 1);

		//自动退出黑名单群聊
		//不接受该群邀请
		//拉黑操作者（指踢出）
		//删除操作者好友
		//自动同意非黑名单群邀请
		//被禁言后退群
		//被禁言后拉黑群聊
		//拦截黑名单群的机器人功能
		
		
		//群中检测到黑名单用户自动退群
		//拦截群中黑名单成员的机器人功能
		//不接受由黑名单用户发出的群邀请(20人以下的群无效）
		//将黑名单用户列入屏蔽列表，因此也拒绝其群邀请（20人以下的群仍有效）
		//自动同意非黑名单好友的请求
		
		// ID_STATIC_QQ
// 目标 QQ 号码 标签提示
		hLeaveGroupButton = CreateWindow(TEXT("Button"), TEXT("群中检测到黑名单用户自动退群"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_CHECKBOX,
			DPIX(350), DPIY(600), DPIX(90), DPIY(20),
			hwnd, (HMENU)ID_BUTTON_SAVE, g_hInstance, NULL);
		SendMessage(hLeaveGroupButton, WM_SETFONT, (WPARAM)hFontBold, 1);

		ReleaseDC(hwnd, hDC);
		return 0;
	}
	case WM_COMMAND:
	{
		char Msg[500] = "";
		switch (LOWORD(wParam))
		{
		case ID_EDIT_QQ_MSG:
			TCHAR MSG_QQ[500];
			GetWindowText(hQQMsgText, MSG_QQ, 500);
			TcharToChar(MSG_QQ, Msg);
			setQQMSG(Msg);
			break;
		case ID_EDIT_GROUP_MSG:
			TCHAR MSG_GROUP[500];
			GetWindowText(hGroupMsgText, MSG_GROUP, 500);
			TcharToChar(MSG_GROUP, Msg);
			setGroupMSG(Msg);
			break;
		case ID_EDIT_QQ_BAN_MSG:
			TCHAR MSG_BAN_QQ[500];
			GetWindowText(hQQBanMsgText, MSG_BAN_QQ, 500);
			TcharToChar(MSG_BAN_QQ, Msg);
			setQQBanMSG(Msg);
			break;
		case ID_EDIT_GROUP_BAN_MSG:
			TCHAR MSG_BAN_GROUP[500];
			GetWindowText(hGroupBanMsg, MSG_BAN_GROUP, 500);
			TcharToChar(MSG_BAN_GROUP, Msg);
			setGroupBanMSG(Msg);
			break;
		case ID_BUTTON_SAVE:
			TCHAR ManagerQQ[20];
			GetWindowText(hEditQQText, ManagerQQ, 20);
			setMaster(_wtoi(ManagerQQ));
			TCHAR ManagerGroup[20];
			GetWindowText(hEditGroupText, ManagerGroup, 20);
			setMASTERGroup(_wtoi(ManagerGroup));
			saveMysql();
			break;
		default:
			break;
		}

		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
