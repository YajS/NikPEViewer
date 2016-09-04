#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WindowsX.h>
#include <shellapi.h>
#include <Commdlg.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <wininet.h>
#include <shlobj.h>
#include <gdiplus.h>
#include "RESOURCE.H"

using namespace std;
using namespace Gdiplus;

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"GDI32.lib")
#pragma comment (lib,"shell32.lib")
#pragma comment (lib,"Comdlg32.lib")
#pragma comment (lib,"Gdiplus.lib")

#ifdef  _UNICODE
	#define		_tstring	wstring
#else
	#define		_tstring	string
#endif

#define CLASS_NAME					_T("BITMAP_EDITOR")
#define WINDOWS_CAPTION				_T("PE Bitmap viewer")
#define	NEW_LINE_CHAR				_T('\n')
#define	NEW_LINE_STR				_T("\n")
#define	NULL_CHAR					_T('\0')
#define	BITMAP_FONT					_T("Courier New")

#define MAX_FILESIZE				1024
#define	LEFT_MARGIN					20

#define LINE_NUMBER_COLOR			RGB(0,208,0)
#define SIDE_BAR_COLOR				RGB(232,232,232)
#define WINDOW_BACK_COLOR			RGB(255,255,255)

#define BITMAP_DATA					1
#define ICON_DATA					2
#define CURSOR_DATA					3

typedef struct _ICONDIR{
	WORD idReverved;
	WORD idType;
	WORD idCount;
}ICONDIR;

typedef struct _ICON{
	BYTE bWidth;
	BYTE bHeight;
	BYTE bColorCount;
	BYTE bReserved;
	WORD wPlanes;
	WORD wBitCount;
	DWORD dwBytesInRes;
	DWORD dwImageOffset;
}ICON;

class CNikBitmapEditor
{
	HWND							m_hWnd;
	HWND							m_hParentWnd;

	RECT							m_rDrawArea;
	RECT							m_rRectSelection;

	HFONT							m_hFont;

	int								m_nSelectionType;
	int								m_nLeftBoundary;
	int								m_nOffsetX;
	int								m_nOffsetY;

	BOOL							m_bIsLButtonDown;
	BOOL							m_bKeyDown[256];

	POINT							m_ptStartSelection;
	POINT							m_ptEndSelection;

	BITMAPINFOHEADER				m_stBitmapHeader;

	HBITMAP							m_hBitmap;
	HBITMAP							m_hImageBitmap;

	LOGFONT							m_lf;

public:

	CNikBitmapEditor()
	{
		m_hWnd						= NULL;
		m_hFont						= NULL;

		m_nOffsetX					= 0;
		m_nOffsetY					= 0;
		m_nSelectionType			= 0;
		m_nLeftBoundary				= LEFT_MARGIN;

		m_hImageBitmap				= NULL;

		m_bIsLButtonDown			= false;

		m_ptStartSelection.x		= -1;
		m_ptStartSelection.y		= -1;
		m_ptEndSelection.x			= -1;
		m_ptEndSelection.y			= -1;

		memset(&m_lf,0,sizeof(LOGFONT));

		_tcscpy_s(m_lf.lfFaceName,sizeof(m_lf.lfFaceName)/sizeof(TCHAR),BITMAP_FONT);
		m_lf.lfClipPrecision		= 2;
		m_lf.lfHeight				= -20;
		m_lf.lfQuality				= 1;
		m_lf.lfWeight				= 400;
		m_lf.lfOutPrecision			= 3;
		m_lf.lfPitchAndFamily		= 34;

		m_hBitmap					= NULL;

		memset(&m_rDrawArea,0,sizeof(RECT));
		memset(m_bKeyDown,0,sizeof(m_bKeyDown));
	}

	~CNikBitmapEditor()
	{
		if(m_hFont)
			DeleteObject(m_hFont);

		if(m_hBitmap)
			DeleteObject(m_hBitmap);

		if(m_hImageBitmap)
			DeleteObject(m_hImageBitmap);
	}

	void ProcessKeyMessages(DWORD dwKeycode,int nFlag);

	void DrawBitmapEditor(HDC hdc, RECT rPaint);

	void DrawRectArea(HDC hdc, RECT rc, COLORREF col);
	void DrawLineRect(HDC hdc, RECT rc);

	BYTE * ReadBinaryFile(_tstring str, DWORD &dwFileSize);

	void SetScrollBar();
	void OnScrollBar(int nFlag, WPARAM wParam);

	int CreateBitmap(IStream * pIStream, DWORD dwSize);

	void DrawSideBar(HDC hdc);

	void OnSize(WPARAM wParam, LPARAM lParam);

	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void OnLbuttonUP(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnRbuttonDown(WPARAM wParam, LPARAM lParam);

	int CreateBitmapfromBuffer(BYTE * pBuffer, int nSize, int nFlag);

	void SetWindowHWnd(HWND hwnd)
	{
		m_hWnd = hwnd;
	}

	void SetLogFont(LOGFONT lf)
	{
		m_lf = lf;
	}

	void GetEditorWindowSize(int &nWidth,int &nHeight)
	{
		nWidth = m_rDrawArea.right - m_rDrawArea.left;
		nHeight = m_rDrawArea.bottom - m_rDrawArea.top;
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

	LOGFONT GetEditorFont()
	{
		return m_lf;
	}

	void SetDrawRect(RECT rc)
	{
		m_rDrawArea = rc;
	}


	HBITMAP GetEditorBitmap()
	{
		return m_hBitmap;
	}
	void SetEditorBitmap(HBITMAP hBitmap)
	{
		m_hBitmap = hBitmap;
	}

	HWND GethWnd()
	{
		return m_hWnd;
	}

	HWND GetEditorhWnd()
	{
		return m_hParentWnd;
	}

	HWND CreateBitmapWindow(HWND hParent);

	HWND CreateBitmapWindowBuf(HWND hParent, BYTE * pBuffer
                                        , _tstring strID
                                        , int nSize, int nFlag);

	void PaintWindow(HWND hwnd, HDC hdc, RECT rPaint);

	static void MenuProcess(HWND hwnd,DWORD dwValue);

	static int RegisterEditorclass(HINSTANCE hInstance);

	static LRESULT CALLBACK EditorWndProc(HWND hwnd, UINT uMsg,
									WPARAM wParam, LPARAM lParam);

	static CNikBitmapEditor * GetBitmapEditorPtr(HWND hwnd);
};
