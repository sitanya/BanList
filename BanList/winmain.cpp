#include "stdafx.h"
#include "winmain.h"
#include "CQAPI.h"
#include <Windows.h>
#include "BanList.h"

int auth = -1;

int g_dpix = DEFAULT_DPIX;
int g_dpiy = DEFAULT_DPIY;

#define DPIX(value) (MulDiv(value, g_dpix, DEFAULT_DPIX))
#define DPIY(value) (MulDiv(value, g_dpiy, DEFAULT_DPIY))

HINSTANCE g_hInstance = GetModuleHandle(NULL);

void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

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
		TEXT("������"),          //���ڱ�������  
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,        //���������ʽ  
		DPIX(38),             //��������ڸ�����X����  
		DPIY(20),             //��������ڸ�����Y����  
		DPIX(300),                //���ڵĿ��  
		DPIY(380),                //���ڵĸ߶�  
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
	static HWND hEditManagerQQText, hStaticMsg;
	static HWND hEditManagerGroupText;
	static HWND hEditManagerQQ, hEditMsg;
	static HWND hButtonSend, hButtonClear;
	static HWND hEditManagerGroup;
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

		// ID_EDIT_QQ
		// Ŀ�� QQ ���� �ı���ֻ���������֣�
		hEditManagerQQ = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER | WS_BORDER,
			DPIX(30), DPIY(20), DPIX(150), DPIY(25),
			hwnd, (HMENU)ID_EDIT_QQ, g_hInstance, NULL);
		SendMessage(hEditManagerQQ, WM_SETFONT, (WPARAM)hFontRegular, 1);

		// ID_STATIC_QQ
		// Ŀ�� QQ ���� ��ǩ��ʾ
		hEditManagerQQText = CreateWindow(TEXT("Button"), TEXT("����Ա�˺�"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(190), DPIY(20), DPIX(90), DPIY(25),
			hwnd, (HMENU)ID_BUTTON_QQ, g_hInstance, NULL);
		SendMessage(hEditManagerQQText, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_QQ
	// Ŀ�� QQ ���� �ı���ֻ���������֣�
		hEditManagerGroup = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER | WS_BORDER,
			DPIX(30), DPIY(50), DPIX(150), DPIY(20),
			hwnd, (HMENU)ID_EDIT_QQ, g_hInstance, NULL);
		SendMessage(hEditManagerGroup, WM_SETFONT, (WPARAM)hFontRegular, 1);

		// ID_STATIC_QQ
		// Ŀ�� QQ ���� ��ǩ��ʾ
		hEditManagerGroupText = CreateWindow(TEXT("Button"), TEXT("���Ⱥ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(190), DPIY(50), DPIX(90), DPIY(20),
			hwnd, (HMENU)ID_BUTTON_Group, g_hInstance, NULL);
		SendMessage(hEditManagerGroupText, WM_SETFONT, (WPARAM)hFontBold, 1);


		// ID_STATIC_MSG
		// ���͵���Ϣ���� ��ǩ��ʾ
		hStaticMsg = CreateWindow(TEXT("Static"), TEXT("��Ⱥ��Ϣ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(30), DPIY(80), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hStaticMsg, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// ���͵���Ϣ���� �ı��򣨶��У�
		hEditMsg = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(30), DPIY(110), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_MSG, g_hInstance, NULL);
		SendMessage(hEditMsg, WM_SETFONT, (WPARAM)hFontRegular, 1);

		//// ID_BUTTON_SEND
		//// ���� ��ť
		//hButtonSend = CreateWindow(TEXT("Button"), TEXT("����"),
		//	WS_VISIBLE | WS_CHILD,
		//	DPIX(50), DPIY(320), DPIX(150), DPIY(30),
		//	hwnd, (HMENU)ID_BUTTON_SEND, g_hInstance, NULL);
		//SendMessage(hButtonSend, WM_SETFONT, (WPARAM)hFontBold, 1);

		//// ID_BUTTON_CLEAR
		//// ��� ��ť
		//hButtonClear = CreateWindow(TEXT("Button"), TEXT("���"),
		//	WS_VISIBLE | WS_CHILD,
		//	DPIX(250), DPIY(320), DPIX(150), DPIY(30),
		//	hwnd, (HMENU)ID_BUTTON_CLEAR, g_hInstance, NULL);
		//SendMessage(hButtonClear, WM_SETFONT, (WPARAM)hFontBold, 1);

		ReleaseDC(hwnd, hDC);
		return 0;
	}
	case WM_COMMAND:
	{
		char Msg[500] = "";
	switch (LOWORD(wParam))
	{
	case ID_BUTTON_QQ:
	{
		TCHAR ManagerQQ[20];
		GetWindowText(hEditManagerQQ, ManagerQQ, 20);
		setMaster(_wtoi(ManagerQQ));
		break;
	}
	case ID_BUTTON_Group:
		TCHAR ManagerGroup[20];
		GetWindowText(hEditManagerGroup, ManagerGroup, 200);
		setMASTERGroup(_wtoi(ManagerGroup));
		break;
	case ID_EDIT_MSG:
		TCHAR MSG[500];
		GetWindowText(hEditMsg, MSG, 500);
		TcharToChar(MSG, Msg);
		setMSG(Msg);
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
