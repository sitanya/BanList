#include "stdafx.h"
#include "winmain.h"
#include "CQAPI.h"
#include <Windows.h>

int auth = -1;

int g_dpix = DEFAULT_DPIX;
int g_dpiy = DEFAULT_DPIY;

#define DPIX(value) (MulDiv(value, g_dpix, DEFAULT_DPIX))
#define DPIY(value) (MulDiv(value, g_dpiy, DEFAULT_DPIY))

HINSTANCE g_hInstance = GetModuleHandle(NULL);

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
		TEXT("消息发送测试"),          //窗口标题文字  
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,        //窗口外观样式  
		DPIX(38),             //窗口相对于父级的X坐标  
		DPIY(20),             //窗口相对于父级的Y坐标  
		DPIX(480),                //窗口的宽度  
		DPIY(400),                //窗口的高度  
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
	static HWND hStaticQQ, hStaticMsg;
	static HWND hEditQQ, hEditMsg;
	static HWND hButtonSend, hButtonClear;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HFONT hFontRegular, hFontBold;
		HDC hDC;
		hDC = GetDC(hwnd);
		hFontRegular = CreateFont(DPIY(20), 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("微软雅黑"));
		hFontBold = CreateFont(DPIY(20), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("微软雅黑"));
		SelectObject(hDC, hFontRegular);

		// ID_STATIC_QQ
		// 目标 QQ 号码 标签提示
		hStaticQQ = CreateWindow(TEXT("Static"), TEXT("目标 QQ 号码："),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(30), DPIY(30), DPIX(135), DPIY(20),
			hwnd, (HMENU)ID_STATIC_QQ, g_hInstance, NULL);
		SendMessage(hStaticQQ, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_QQ
		// 目标 QQ 号码 文本框（只能输入数字）
		hEditQQ = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER,
			DPIX(175), DPIY(30), DPIX(180), DPIY(20),
			hwnd, (HMENU)ID_EDIT_QQ, g_hInstance, NULL);
		SendMessage(hEditQQ, WM_SETFONT, (WPARAM)hFontRegular, 1);
		SetFocus(hEditQQ);

		// ID_STATIC_MSG
		// 发送的消息内容 标签提示
		hStaticMsg = CreateWindow(TEXT("Static"), TEXT("要发送的消息："),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(30), DPIY(70), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hStaticMsg, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// 发送的消息内容 文本框（多行）
		hEditMsg = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE,
			DPIX(30), DPIY(100), DPIX(400), DPIY(200),
			hwnd, (HMENU)ID_EDIT_MSG, g_hInstance, NULL);
		SendMessage(hEditMsg, WM_SETFONT, (WPARAM)hFontRegular, 1);

		// ID_BUTTON_SEND
		// 发送 按钮
		hButtonSend = CreateWindow(TEXT("Button"), TEXT("发送"),
			WS_VISIBLE | WS_CHILD,
			DPIX(50), DPIY(320), DPIX(150), DPIY(30),
			hwnd, (HMENU)ID_BUTTON_SEND, g_hInstance, NULL);
		SendMessage(hButtonSend, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_BUTTON_CLEAR
		// 清空 按钮
		hButtonClear = CreateWindow(TEXT("Button"), TEXT("清空"),
			WS_VISIBLE | WS_CHILD,
			DPIX(250), DPIY(320), DPIX(150), DPIY(30),
			hwnd, (HMENU)ID_BUTTON_CLEAR, g_hInstance, NULL);
		SendMessage(hButtonClear, WM_SETFONT, (WPARAM)hFontBold, 1);

		ReleaseDC(hwnd, hDC);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEND:
		{
			TCHAR szQQ[20];
			TCHAR *szMsg = NULL;
			int64_t qqNum;
			size_t nCount = (GetWindowTextLength(hEditMsg) + 1) * sizeof(TCHAR);
			szMsg = (TCHAR *)malloc(nCount);
			if (szMsg)
			{
				GetWindowText(hEditQQ, szQQ, 20);
				GetWindowText(hEditMsg, szMsg, nCount);
				qqNum = _atoi64(szQQ);
				CQ_sendPrivateMsg(auth, qqNum, szMsg);
				MessageBox(hwnd, TEXT("已经尝试发送此消息。"), TEXT("提示"), MB_ICONASTERISK);
				free(szMsg);
				szMsg = NULL;
			}
			else
			{
				MessageBox(hwnd, TEXT("分配内存失败"), TEXT("错误"), MB_ICONHAND);
			}
			break;
		}
		case ID_BUTTON_CLEAR:
			SendMessage(hEditQQ, WM_SETTEXT, 0, NULL);
			SendMessage(hEditMsg, WM_SETTEXT, 0, NULL);
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
