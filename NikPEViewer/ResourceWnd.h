#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WindowsX.h>
#include <commctrl.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "RESOURCE.H"

#include "MenuWnd.h"

using namespace std;

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"GDI32.lib")

#ifdef  _UNICODE
	#define		_tstring	wstring
#else
	#define		_tstring	string
#endif

#define CLASS_RESOURCE				_T("NIKHIL_RESOURCE")
#define RESOURCE_CAPTION			_T("Nik's Resource")
#define	MENU_FONT					_T("Tahoma")

typedef struct _ACCELTABLEENTRY 
{ 
  WORD fFlags; 
  WORD wAnsi; 
  WORD wId; 
  WORD padding; 
}ACCELTABLEENTRY;

typedef struct _ResourceString { 
  WORD   wLength; 
  WORD   wValueLength; 
  WORD   wType;
}RESOURCESTRING;

class CNikResource
{
	HWND							m_hWnd;
	HWND							m_hListView;
	HWND							m_hParentWnd;

    BYTE *                          m_pBuffer;
	RESOURCELIST *					m_pResource;

	int								m_nSize;
	int								m_nFlag;

	HFONT							m_hFont;

    MENU_INFO					    m_MenuItem;

	CMenuWnd *						m_pcMenuWnd;

    vector<CMenuWnd *>		        m_vMenuWndList;

public:

	void OnSize(WPARAM wParam, LPARAM lParam);

	static void DrawRectArea(HDC hdc, RECT rc,COLORREF col);

	static int CreateMenuList(MENU_INFO * pMenu, int nOffset, int nSize, BYTE *);

	void CreateListViewWindow();

	int CreateStringWindows(RESOURCELIST * pRes);

	int CreateVersionWindows(RESOURCELIST * pRes);

	int CreateAccelaratorWindows(RESOURCELIST * pRes);

	int CreateMessageWindows(RESOURCELIST * pRes);

	int CreateMenuWindows(RESOURCELIST * pRes);

	void CreateMenuItemsWindow(int nNo);

	void CreateMenuRectSize();

	void DrawMenuItems(HDC hdc);

	BOOL InitStringListView(HWND hWndListView);

	BOOL InitVersionView(HWND hWndListView);

	BOOL InitMessageView(HWND hWndListView);

	BOOL InitAccelaratorListView(HWND hWndListView);

	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void OnLbuttonUP(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	CNikResource()
	{
		m_hWnd				= NULL;
		m_hListView			= NULL;
		m_hParentWnd		= NULL;
		m_hFont				= NULL;
		m_nFlag				= 0;
		m_pResource			= NULL;
		m_pcMenuWnd			= NULL;
        m_pBuffer           = NULL;
	}

	~CNikResource()
	{
		if(m_hFont)
			DeleteObject(m_hFont);

		//UnregisterClass(CLASS_RESOURCE
        //  , GetWindowInstance(m_hParentWnd));

		CMenuWnd * pTemp;
		for(int i=0;i<(int)m_vMenuWndList.size();i++)
		{
			pTemp = m_vMenuWndList.at(i);
            if(pTemp->GethWnd())
                DestroyWindow(pTemp->GethWnd());
            if(pTemp == m_pcMenuWnd)
                m_pcMenuWnd = NULL;
			delete pTemp;
		}
        m_vMenuWndList.clear();

		if(m_pcMenuWnd)
			delete m_pcMenuWnd;

		m_MenuItem.m_vMenuList.clear();
	}

	void SetParentHwnd(HWND hwnd)
	{
		m_hParentWnd = hwnd;
	}
	HWND GetParenthWnd()
	{
		return m_hParentWnd;
	}

	void SetFlag(int nFlag)
	{
		m_nFlag = nFlag;
	}

	void SethWnd(HWND hwnd)
	{
		m_hWnd = hwnd;
	}
	HWND GethWnd()
	{
		return m_hWnd;
	}

    void ShowWindowPosition( int nFlag
                            , POINT pt )
    {
        if(m_hWnd)
        {
            if(nFlag)
            {
                RECT rc;
                GetClientRect( GetParent( m_hWnd ), &rc );
                int nWidth = rc.right - rc.left;
                int nHeight = rc.bottom - rc.top;
                SetWindowPos( m_hWnd, HWND_TOP
                    , pt.x+(nWidth/4) , pt.y, nWidth/2, nHeight/2, SWP_SHOWWINDOW );
            }
            else
                SetWindowPos( m_hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        }
    }

	static int RegisterResourceclass(HINSTANCE hInstance);

	static LRESULT CALLBACK ResourceWndProc(HWND hwnd, UINT uMsg,
									WPARAM wParam, LPARAM lParam);

	HWND CreateResourceWindow(HWND hParent);

	HWND SetResourceData(HWND hParent, RESOURCELIST * pTemp
        , BYTE * pBuffer, int nFlag);
};