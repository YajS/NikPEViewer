#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WindowsX.h>
#include <commctrl.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "RESOURCE.H"

using namespace std;

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"GDI32.lib")

#ifdef  _UNICODE
	#define		_tstring	wstring
#else
	#define		_tstring	string
#endif

#define CLASS_DIALOG				_T("NIKHIL_DIALOG")
#define DIALOG_CAPTION				_T("Resource dialog viewer")

typedef struct _DIALOGCONTROL
{
	DWORD				m_lStyle;
	DWORD				m_lExStyle;
	WORD				m_NoOfItems;
	WORD				m_wX;
	WORD				m_wY;
	WORD				m_wCX;
	WORD				m_wCY;
	WORD				m_wControlID;
	WORD				m_wPointSize;
	DWORD				m_dwHelpID;
	WORD				m_wWeight;
    BYTE				m_bItalic;
    BYTE				m_bCharset;
	DWORD				m_dwIsExtend;
	int					m_nFlag;

	RECT				m_rRect;

	LOGFONT				m_lf;

	HWND				m_hWnd;

	WCHAR				m_strMenuName[1024];
	WCHAR				m_strClassName[1024];
	WCHAR				m_strCaption[1024];
	WCHAR				m_strFontName[1024];

	_DIALOGCONTROL()
	{
		m_lStyle				= 0;
		m_lExStyle				= 0;
		m_NoOfItems				= 0;
		m_wX					= 0;
		m_wX					= 0;
		m_wY					= 0;
		m_wCX					= 0;
		m_wCY					= 0;
		m_wControlID			= 0;
		m_wPointSize			= 0;
		m_dwHelpID				= 0;
		m_wWeight				= 0;
		m_bItalic				= 0;
		m_bCharset				= 0;
		m_dwIsExtend			= 0;
		m_nFlag					= 0;
		m_hWnd					= NULL;

		m_strMenuName[0]		= NULL;
		m_strClassName[0]		= NULL;
		m_strCaption[0]			= NULL;
		m_strFontName[0]		= NULL;
		m_strMenuName[1023]		= NULL;
		m_strClassName[1023]	= NULL;
		m_strCaption[1023]		= NULL;
		m_strFontName[1023]		= NULL;

		memset(&m_rRect,0,sizeof(RECT));

		memset(&m_lf,0,sizeof(LOGFONT));
	}

	~_DIALOGCONTROL()
	{
	}
}DIALOGCONTROL;

class CNikDialog
{
	HWND							m_hWnd;
	HWND							m_hParentWnd;
	HWND							m_hDlgWnd;

    BYTE *                          m_pBuffer;
	int								m_nPos;

	RECT							m_rRect;

	HFONT							m_hFont;

	POINT							m_ptStartPos;
	POINT							m_ptEndPos;

	vector<DIALOGCONTROL>			m_vDialogList;

public:

	void OnSize(WPARAM wParam, LPARAM lParam);

	int MakeDialogList(RESOURCELIST * pRes);

	void DrawDialogBox(HDC hdc);

	void DrawDialogBorder(HDC hdc);

	int SetCursorShape(POINT pt);

	int DrawRectMarkers(HDC hdc, RECT rc, POINT *pt, int nFlag);

	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void OnLbuttonUP(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	CNikDialog()
	{
		m_hWnd				= NULL;
		m_hParentWnd		= NULL;
		m_hFont				= NULL;
		m_hDlgWnd			= NULL;

		m_nPos				= 0;
		m_ptEndPos.x		= 0;
		m_ptEndPos.y		= 0;
		m_ptStartPos.x		= 0;
		m_ptStartPos.y		= 0;

		memset(&m_rRect,0,sizeof(RECT));
	}

	~CNikDialog()
	{
		if(m_hFont)
			DeleteObject(m_hFont);

		UnregisterClass(CLASS_DIALOG, GetWindowInstance(m_hParentWnd));

		m_vDialogList.clear();
	}

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

	void DestroyDialogBox()
	{
		if(m_hDlgWnd 
			&& IsWindow(m_hDlgWnd))
		{
			DestroyWindow(m_hDlgWnd);
			m_hDlgWnd = NULL;
		}
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

    HWND CreateDialogWindow(HWND hParent);

    HWND SetDialogData( HWND hParent
					    , RESOURCELIST * pRes
                        , BYTE * pBuffer
                        , int nFlag);

    static void DrawLineRect(HDC hdc, RECT rc
        , COLORREF cLineCol, int nLineWidth);
	static void DrawRectArea(HDC hdc, RECT rc,COLORREF col);

	static int RegisterDialogclass(HINSTANCE hInstance);

	static LRESULT CALLBACK ResourceWndProc(HWND hwnd, UINT uMsg,
									WPARAM wParam, LPARAM lParam);

	static INT_PTR CALLBACK DialogWndProc(HWND hwnd, UINT uMsg,
									WPARAM wParam, LPARAM lParam);
};