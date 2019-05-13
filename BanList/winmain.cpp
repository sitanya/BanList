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
	// ��ȡDPI
	HDC hdcDpi = GetDC(NULL);
	if (hdcDpi != NULL) {
		g_dpix = GetDeviceCaps(hdcDpi, LOGPIXELSX);
		g_dpiy = GetDeviceCaps(hdcDpi, LOGPIXELSY);
		ReleaseDC(NULL, hdcDpi);
	}
	// ����  
	//wchar *cls_Name = "";
	const wchar_t* cls_Name = TEXT("MainWindow");
	//TCHAR* test = _T("���ڱ���");
	// ��ƴ�����  
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
	// ע�ᴰ����  
	RegisterClass(&wc);

	// ��������  
	HWND hwnd = CreateWindow(
		cls_Name,           //������Ҫ�͸ղ�ע���һ��  
		TEXT("��Ϣ���Ͳ���"),          //���ڱ�������  
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,        //���������ʽ  
		DPIX(38),             //��������ڸ�����X����  
		DPIY(20),             //��������ڸ�����Y����  
		DPIX(480),                //���ڵĿ��  
		DPIY(400),                //���ڵĸ߶�  
		NULL,               //û�и����ڣ�ΪNULL  
		NULL,               //û�в˵���ΪNULL  
		g_hInstance,          //��ǰӦ�ó����ʵ�����  
		NULL);              //û�и������ݣ�ΪNULL  
	if (hwnd == NULL)                //��鴰���Ƿ񴴽��ɹ�  
		return 0;
	// ��ʾ����  
	ShowWindow(hwnd, SW_SHOW);
	// ���´���  
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
	// �����ؼ�
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
			FF_MODERN, TEXT("΢���ź�"));
		hFontBold = CreateFont(DPIY(20), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("΢���ź�"));
		SelectObject(hDC, hFontRegular);

		// ID_STATIC_QQ
		// Ŀ�� QQ ���� ��ǩ��ʾ
		hStaticQQ = CreateWindow(TEXT("Static"), TEXT("Ŀ�� QQ ���룺"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(30), DPIY(30), DPIX(135), DPIY(20),
			hwnd, (HMENU)ID_STATIC_QQ, g_hInstance, NULL);
		SendMessage(hStaticQQ, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_QQ
		// Ŀ�� QQ ���� �ı���ֻ���������֣�
		hEditQQ = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER,
			DPIX(175), DPIY(30), DPIX(180), DPIY(20),
			hwnd, (HMENU)ID_EDIT_QQ, g_hInstance, NULL);
		SendMessage(hEditQQ, WM_SETFONT, (WPARAM)hFontRegular, 1);
		SetFocus(hEditQQ);

		// ID_STATIC_MSG
		// ���͵���Ϣ���� ��ǩ��ʾ
		hStaticMsg = CreateWindow(TEXT("Static"), TEXT("Ҫ���͵���Ϣ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(30), DPIY(70), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hStaticMsg, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// ���͵���Ϣ���� �ı��򣨶��У�
		hEditMsg = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE,
			DPIX(30), DPIY(100), DPIX(400), DPIY(200),
			hwnd, (HMENU)ID_EDIT_MSG, g_hInstance, NULL);
		SendMessage(hEditMsg, WM_SETFONT, (WPARAM)hFontRegular, 1);

		// ID_BUTTON_SEND
		// ���� ��ť
		hButtonSend = CreateWindow(TEXT("Button"), TEXT("����"),
			WS_VISIBLE | WS_CHILD,
			DPIX(50), DPIY(320), DPIX(150), DPIY(30),
			hwnd, (HMENU)ID_BUTTON_SEND, g_hInstance, NULL);
		SendMessage(hButtonSend, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_BUTTON_CLEAR
		// ��� ��ť
		hButtonClear = CreateWindow(TEXT("Button"), TEXT("���"),
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
				MessageBox(hwnd, TEXT("�Ѿ����Է��ʹ���Ϣ��"), TEXT("��ʾ"), MB_ICONASTERISK);
				free(szMsg);
				szMsg = NULL;
			}
			else
			{
				MessageBox(hwnd, TEXT("�����ڴ�ʧ��"), TEXT("����"), MB_ICONHAND);
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
