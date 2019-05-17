#include "stdafx.h"
#include "winmain.h"
#include "CQAPI.h"
#include <Windows.h>
#include "BanList.h"
#include <map>
#include "CQLogger.h"

using namespace std;
using namespace CQ;

int auth = -1;

int g_dpix = DEFAULT_DPIX;
int g_dpiy = DEFAULT_DPIY;

#define DPIX(value) (MulDiv(value, g_dpix, DEFAULT_DPIX))
#define DPIY(value) (MulDiv(value, g_dpiy, DEFAULT_DPIY))

HINSTANCE g_hInstance = GetModuleHandle(NULL);

void TcharToChar(const TCHAR *tchar, char *_char)
{
	int iLength;
	//��ȡ�ֽڳ���
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CharToTchar(const char *_char, TCHAR *tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

int ShowMainWindow(int32_t AuthCode)
{
	auth = AuthCode;
	// ��ȡDPI
	HDC hdcDpi = GetDC(NULL);
	if (hdcDpi != NULL)
	{
		g_dpix = GetDeviceCaps(hdcDpi, LOGPIXELSX);
		g_dpiy = GetDeviceCaps(hdcDpi, LOGPIXELSY);
		ReleaseDC(NULL, hdcDpi);
	}
	// ����
	//wchar *cls_Name = "";
	const wchar_t *cls_Name = TEXT("MainWindow");
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
		cls_Name,												  //������Ҫ�͸ղ�ע���һ��
		TEXT("������"),											  //���ڱ�������
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //���������ʽ
		DPIX(38),												  //��������ڸ�����X����
		DPIY(20),												  //��������ڸ�����Y����
		DPIX(2000),												  //���ڵĿ��
		DPIY(3800),												  //���ڵĸ߶�
		NULL,													  //û�и����ڣ�ΪNULL
		NULL,													  //û�в˵���ΪNULL
		g_hInstance,											  //��ǰӦ�ó����ʵ�����
		NULL);													  //û�и������ݣ�ΪNULL
	if (hwnd == NULL)											  //��鴰���Ƿ񴴽��ɹ�
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
	_In_ HWND hwnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	// �����ؼ�
	static HWND hEditQQText;
	static HWND hEditGroupText;
	static HWND hGroupMsgText, hGroupMsgStatic;
	static HWND hQQMsgText, hQQMsgStatic;
	static HWND hQQBanMsgStatic, hQQBanMsgText;
	static HWND hGroupBanMsgText, hGroupBanMsg;
	static HWND hSaveButton;

	static HWND LeaveBanGroup;
	static HWND refuseGroup;
	static HWND BanDetachOperator;
	static HWND deleteDetachOperator;
	static HWND agreeGroup;
	static HWND LeaveForbiddenGroup;
	static HWND BanForbiddenGroup;
	static HWND DontRunningInBanGroup;
	static HWND BanDetachGroup;

	static HWND LeaveGroupByUser;
	static HWND DontRunningInBanGroupForUser;
	static HWND refuseBanUser;
	static HWND agreeUser;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		TCHAR Msg[500];
		map<string, string> Messages = getMSG();
		map<string, bool> Switch = getSwitch();

		HFONT hFontRegular, hFontBold;
		HDC hDC;
		hDC = GetDC(hwnd);
		hFontRegular = CreateFont(DPIY(15), 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("΢���ź�"));
		hFontBold = CreateFont(DPIY(15), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, TEXT("΢���ź�"));
		SelectObject(hDC, hFontRegular);

		// ID_EDIT_QQ
		// Ŀ�� QQ ���� �ı���ֻ���������֣�
		hEditQQText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER | WS_BORDER,
			DPIX(30), DPIY(20), DPIX(150), DPIY(25),
			hwnd, (HMENU)ID_EDIT_QQ, g_hInstance, NULL);
		SendMessage(hEditQQText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end())
		{
			CharToTchar(Messages["MASTER"].c_str(), Msg);
			SendMessage(hEditQQText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_EDIT_QQ
		// Ŀ�� QQ ���� �ı���ֻ���������֣�
		hEditGroupText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NUMBER | WS_BORDER,
			DPIX(30), DPIY(50), DPIX(150), DPIY(20),
			hwnd, (HMENU)ID_EDIT_GROUP, g_hInstance, NULL);
		SendMessage(hEditGroupText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end())
		{
			CharToTchar(Messages["MASTERGroup"].c_str(), Msg);
			SendMessage(hEditGroupText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_STATIC_MSG
		// ���͵���Ϣ���� ��ǩ��ʾ
		hGroupMsgStatic = CreateWindow(TEXT("Static"), TEXT("��Ⱥ��Ϣ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(300), DPIY(20), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hGroupMsgStatic, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// ���͵���Ϣ���� �ı��򣨶��У�
		hGroupMsgText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(300), DPIY(40), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_GROUP_MSG, g_hInstance, NULL);
		SendMessage(hGroupMsgText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end())
		{
			CharToTchar(Messages["GroupMsg"].c_str(), Msg);
			SendMessage(hGroupMsgText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_STATIC_MSG
		// ���͵���Ϣ���� ��ǩ��ʾ
		hQQMsgStatic = CreateWindow(TEXT("Static"), TEXT("˽����Ϣ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(300), DPIY(270), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hQQMsgStatic, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// ���͵���Ϣ���� �ı��򣨶��У�
		hQQMsgText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(300), DPIY(300), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_QQ_MSG, g_hInstance, NULL);
		SendMessage(hQQMsgText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end())
		{
			CharToTchar(Messages["QQMsg"].c_str(), Msg);
			SendMessage(hQQMsgText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

	
		// ID_STATIC_MSG
		// ���͵���Ϣ���� ��ǩ��ʾ
		hQQBanMsgStatic = CreateWindow(TEXT("Static"), TEXT("����QQ��������Ϣ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(550), DPIY(20), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hQQBanMsgStatic, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// ���͵���Ϣ���� �ı��򣨶��У�
		hQQBanMsgText = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(550), DPIY(40), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_QQ_BAN_MSG, g_hInstance, NULL);
		SendMessage(hQQBanMsgText, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end())
		{
			CharToTchar(Messages["BanQQMsg"].c_str(), Msg);
			SendMessage(hQQBanMsgText, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		// ID_STATIC_MSG
		// ���͵���Ϣ���� ��ǩ��ʾ
		hGroupBanMsgText = CreateWindow(TEXT("Static"), TEXT("����Ⱥ��������Ϣ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(550), DPIY(270), DPIX(330), DPIY(20),
			hwnd, (HMENU)ID_STATIC_MSG, g_hInstance, NULL);
		SendMessage(hGroupBanMsgText, WM_SETFONT, (WPARAM)hFontBold, 1);

		// ID_EDIT_MSG
		// ���͵���Ϣ���� �ı��򣨶��У�
		hGroupBanMsg = CreateWindow(TEXT("Edit"), NULL,
			WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
			DPIX(550), DPIY(300), DPIX(240), DPIY(200),
			hwnd, (HMENU)ID_EDIT_GROUP_BAN_MSG, g_hInstance, NULL);
		SendMessage(hGroupBanMsg, WM_SETFONT, (WPARAM)hFontRegular, 1);
		if (Messages.find("QQ") != Messages.end())
		{
			CharToTchar(Messages["BanGroupMsg"].c_str(), Msg);
			SendMessage(hGroupBanMsg, WM_SETTEXT, (WPARAM)hFontRegular, (LPARAM)Msg);
		}

		hSaveButton = CreateWindow(TEXT("Button"), TEXT("���浽�ƶ�"),
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			DPIX(300), DPIY(600), DPIX(90), DPIY(50),
			hwnd, (HMENU)ID_BUTTON_SAVE, g_hInstance, NULL);
		SendMessage(hSaveButton, WM_SETFONT, (WPARAM)hFontBold, 1);


		agreeGroup = CreateWindow(TEXT("Button"), TEXT("�Զ�ͬ��Ǻ�����Ⱥ����"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(70), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_agreeGroup, g_hInstance, NULL);
		SendMessage(agreeGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["agreeGroup"])
		{
			SendMessage(agreeGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		refuseGroup = CreateWindow(TEXT("Button"), TEXT("�����ܺ�����Ⱥ����"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(100), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_refuseGroup, g_hInstance, NULL);
		SendMessage(refuseGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["refuseGroup"])
		{
			SendMessage(refuseGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		LeaveBanGroup = CreateWindow(TEXT("Button"), TEXT("�Զ��˳�������Ⱥ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(130), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_LeaveBanGroup, g_hInstance, NULL);
		SendMessage(LeaveBanGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["LeaveBanGroup"])
		{
			SendMessage(LeaveBanGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		BanDetachGroup = CreateWindow(TEXT("Button"), TEXT("���ڱ��߳���Ⱥ"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(160), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_BanDetachGroup, g_hInstance, NULL);
		SendMessage(BanDetachGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["BanDetachGroup"])
		{
			SendMessage(BanDetachGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		BanDetachOperator = CreateWindow(TEXT("Button"), TEXT("�����߳���"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(190), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_BanDetachOperator, g_hInstance, NULL);
		SendMessage(BanDetachOperator, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["BanDetachOperator"])
		{
			SendMessage(BanDetachOperator, BM_SETCHECK, BST_CHECKED, 0);
		}

		deleteDetachOperator = CreateWindow(TEXT("Button"), TEXT("ɾ���߳��ߺ��Ѳ�����"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(220), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_deleteDetachOperator, g_hInstance, NULL);
		SendMessage(deleteDetachOperator, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["deleteDetachOperator"])
		{
			SendMessage(deleteDetachOperator, BM_SETCHECK, BST_CHECKED, 0);
		}

		LeaveForbiddenGroup = CreateWindow(TEXT("Button"), TEXT("�����Ժ���Ⱥ"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(250), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_LeaveForbiddenGroup, g_hInstance, NULL);
		SendMessage(LeaveForbiddenGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["LeaveForbiddenGroup"])
		{
			SendMessage(LeaveForbiddenGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		BanForbiddenGroup = CreateWindow(TEXT("Button"), TEXT("�����Ժ�����Ⱥ��"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(280), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_BanForbiddenGroup, g_hInstance, NULL);
		SendMessage(BanForbiddenGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["BanForbiddenGroup"])
		{
			SendMessage(BanForbiddenGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		DontRunningInBanGroup = CreateWindow(TEXT("Button"), TEXT("���غ�����Ⱥ�Ļ����˹���"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(310), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_DontRunningInBanGroup, g_hInstance, NULL);
		SendMessage(DontRunningInBanGroup, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["DontRunningInBanGroup"])
		{
			SendMessage(DontRunningInBanGroup, BM_SETCHECK, BST_CHECKED, 0);
		}

		agreeUser = CreateWindow(TEXT("Button"), TEXT("�Զ�ͬ��Ǻ��������ѵ�����"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(340), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_agreeUser, g_hInstance, NULL);
		SendMessage(agreeUser, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["agreeUser"])
		{
			SendMessage(agreeUser, BM_SETCHECK, BST_CHECKED, 0);
		}

		refuseBanUser = CreateWindow(TEXT("Button"), TEXT("�������ɺ������û�������Ⱥ����(�蹴ѡ<ɾ���߳��ߺ��Ѳ�����>�ſɶ�20������Ⱥ��Ч)"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(370), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_refuseBanUser, g_hInstance, NULL);
		SendMessage(refuseBanUser, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["refuseBanUser"])
		{
			SendMessage(refuseBanUser, BM_SETCHECK, BST_CHECKED, 0);
		}

		DontRunningInBanGroupForUser = CreateWindow(TEXT("Button"), TEXT("����Ⱥ�к�������Ա�Ļ����˹���"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(400), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_DontRunningInBanGroupForUser, g_hInstance, NULL);
		SendMessage(DontRunningInBanGroupForUser, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["DontRunningInBanGroupForUser"])
		{
			SendMessage(DontRunningInBanGroupForUser, BM_SETCHECK, BST_CHECKED, 0);
		}

		LeaveGroupByUser = CreateWindow(TEXT("Button"), TEXT("Ⱥ�м�⵽�������û��Զ���Ⱥ"),
			WS_VISIBLE | WS_CHILD | SS_LEFT | BS_AUTOCHECKBOX,
			DPIX(30), DPIY(430), DPIX(200), DPIY(20),
			hwnd, (HMENU)ID_LeaveGroupByUser, g_hInstance, NULL);
		SendMessage(LeaveGroupByUser, WM_SETFONT, (WPARAM)hFontBold, 1);

		if (Switch["LeaveGroupByUser"])
		{
			SendMessage(LeaveGroupByUser, BM_SETCHECK, BST_CHECKED, 0);
		}

		ReleaseDC(hwnd, hDC);
		return 0;
	}
	case WM_COMMAND:
	{
		map<string, bool> Switch_tmp;
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

			Switch_tmp["LeaveBanGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_LeaveBanGroup);
			Switch_tmp["agreeGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_agreeGroup);
			Switch_tmp["refuseGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_refuseGroup);
			Switch_tmp["BanDetachOperator"] = (bool)IsDlgButtonChecked(hwnd, ID_BanDetachOperator);
			Switch_tmp["deleteDetachOperator"] = (bool)IsDlgButtonChecked(hwnd, ID_deleteDetachOperator);
			//Switch_tmp["agreeGroup"] = false;
			Switch_tmp["LeaveForbiddenGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_LeaveForbiddenGroup);
			Switch_tmp["BanForbiddenGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_BanForbiddenGroup);
			Switch_tmp["DontRunningInBanGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_DontRunningInBanGroup);
			Switch_tmp["BanDetachGroup"] = (bool)IsDlgButtonChecked(hwnd, ID_BanDetachGroup);

			Switch_tmp["LeaveGroupByUser"] = (bool)IsDlgButtonChecked(hwnd, ID_LeaveGroupByUser);
			Switch_tmp["DontRunningInBanGroupForUser"] = (bool)IsDlgButtonChecked(hwnd, ID_DontRunningInBanGroupForUser);
			Switch_tmp["refuseBanUser"] = (bool)IsDlgButtonChecked(hwnd, ID_refuseBanUser);
			Switch_tmp["agreeUser"] = (bool)IsDlgButtonChecked(hwnd, ID_agreeUser);

			saveSwitch(Switch_tmp);

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
