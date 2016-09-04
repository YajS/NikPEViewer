#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "RESOURCE.H"

using namespace std;

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"GDI32.lib")

#ifdef  _UNICODE
	#define		_tstring	wstring
#else
	#define		_tstring	string
#endif

#define CLASS_MENU					_T("NIKHIL_MENU")

class CMenuWnd
{

	HWND						m_hWnd;
    HWND						m_hToolTip;
	HWND						m_hParentWnd;
	HFONT						m_hFont;
	MENU_INFO					m_stMenuInfo;

	CMenuWnd *					m_pcMenuWnd;

public:

	CMenuWnd();
	~CMenuWnd();

	void DrawMenuItems(HDC hdc);
	void DrawMenuText(HDC hdc, RECT &rc, _tstring &strText, int nHighlightItem);
	void InitMenuItems();
	RECT SetMenuSize();

	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void OnLbuttonUP(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	void SetParentHwnd(HWND hwnd)
	{
		m_hParentWnd = hwnd;
	}
	HWND GetParenthWnd()
	{
		return m_hParentWnd;
	}

	void SethWnd(HWND hwnd)
	{
		m_hWnd = hwnd;
	}
	HWND GethWnd()
	{
		return m_hWnd;
	}

    HWND CreateMyTooltip();

	HWND CreateMenuWindow(HWND hParent, HWND hSubling, MENU_INFO &stMenuInfo);

	static int RegisterMenuclass(HINSTANCE hInstance);
	static INT_PTR CALLBACK MenuWndProc(HWND hwnd, UINT uMsg,
									WPARAM wParam, LPARAM lParam);
};