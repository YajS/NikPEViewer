
#include "stdafx.h"
#include "MenuWnd.h"
#include "DialogWnd.h"
#include "ResourceWnd.h"

HWND CMenuWnd::CreateMyTooltip()
{
    /////-------------------------------------
    TOOLINFOA ti;
    char strTT[2] = "";
    RECT rect;

    HWND hToolTip = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_VISIBLE | WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        m_hWnd,
        NULL,
        GetWindowInstance(m_hParentWnd),
        NULL
        );

    SetWindowPos( hToolTip, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    GetClientRect (m_hWnd, &rect);

    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = m_hWnd;
    ti.hinst = GetWindowInstance(m_hParentWnd);
    ti.uId = 0;
    ti.lpszText = strTT;
    ti.rect.left = rect.left;
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;

    SendMessage(hToolTip, TTM_ADDTOOLA, 0, (LPARAM) (LPTOOLINFO) &ti);
    /////-------------------------------------

    return hToolTip;
}

CMenuWnd::CMenuWnd()
{
	m_hWnd			= NULL;
	m_hParentWnd	= NULL;

	m_hFont			= NULL;

    m_hToolTip      = NULL;
	m_pcMenuWnd	    = NULL;
}

CMenuWnd::~CMenuWnd()
{
	if(IsWindow(m_hWnd))
		DestroyWindow(m_hWnd);

	if(m_pcMenuWnd)
    {
		DestroyWindow(m_pcMenuWnd->GethWnd());
        delete m_pcMenuWnd;
    }
}

int CMenuWnd::RegisterMenuclass(HINSTANCE hInstance)
{
	WNDCLASS    wc;
	memset(&wc,0,sizeof(wc));

	wc.lpfnWndProc		= (WNDPROC)CMenuWnd::MenuWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= (HICON)LoadIcon(hInstance,MAKEINTRESOURCE(IDC_NIKPEVIEWER));
    wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName	= CLASS_MENU;
	wc.lpszMenuName		= NULL;

    if (!RegisterClass(&wc)) 
		return FALSE;

	return TRUE;
}

void CMenuWnd::OnLbuttonDown(WPARAM //wParam
							, LPARAM lParam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	RECT rc;
	for(int i=0;i<(int)m_stMenuInfo.m_vMenuList.size();i++)
	{
        rc = m_stMenuInfo.m_vMenuList.at(i).m_rRect;

		if(PtInRect(&rc,pt))
		{
			MENU_INFO stMenu = m_stMenuInfo.m_vMenuList.at(i);
			if(stMenu.m_wFlag & MF_POPUP)
			{
				if(m_pcMenuWnd)
				{
					DestroyWindow(m_pcMenuWnd->GethWnd());
					delete m_pcMenuWnd;
					m_pcMenuWnd = NULL;
				}

				m_pcMenuWnd = new CMenuWnd;
				if(m_pcMenuWnd)
				{
					m_pcMenuWnd->CreateMenuWindow(m_hParentWnd
                        , m_hWnd, m_stMenuInfo.m_vMenuList.at(i));
				}
			}
			break;
		}
	}
}

void CMenuWnd::OnLbuttonUP(WPARAM //wParam
							, LPARAM //lParam
                            )
{
}

void CMenuWnd::OnMouseMove(WPARAM //wParam
							, LPARAM lParam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	HDC hdc = GetDC(m_hWnd);

	RECT rcWnd;
	GetClientRect(m_hWnd,&rcWnd);
	HDC hdcComp = CreateCompatibleDC(hdc);
	HBITMAP hbmComp = CreateCompatibleBitmap(hdc, rcWnd.right, rcWnd.bottom);
	SelectObject(hdcComp,hbmComp);

	HFONT hOldFont = (HFONT)SelectObject(hdcComp,m_hFont);

	int nBKMode = SetBkMode(hdcComp,TRANSPARENT);

	RECT rc;
	for(int i=0;i<(int)m_stMenuInfo.m_vMenuList.size();i++)
	{
		MENU_INFO stMenu = m_stMenuInfo.m_vMenuList.at(i);

        rc = m_stMenuInfo.m_vMenuList.at(i).m_rRect;

		if(!stMenu.m_strText.empty())
		{
			_tstring strText = _T(" ");
			strText += stMenu.m_strText;

			if(PtInRect(&rc,pt))
            {
                ////----------------------
                char chToolStr[1024];
                chToolStr[0] = chToolStr[255] = 0;
                TOOLINFOA ti;
                memset(&ti, 0, sizeof(TOOLINFO));
                ti.cbSize = sizeof(TOOLINFO);
                ti.hwnd = m_hWnd;
                ti.hinst = GetWindowInstance(m_hParentWnd);
                if(stMenu.m_wMenuID)
                    sprintf_s(chToolStr, 1023, "Menu ID = 0x%04X", stMenu.m_wMenuID);
                else
                    sprintf_s(chToolStr, 1023, "Click to POP-UP the Menu");
                ti.lpszText = (char *)chToolStr;
                SendMessage(m_hToolTip, TTM_UPDATETIPTEXTA, 0, (LPARAM) (LPTOOLINFO)&ti);
                ////----------------------

				DrawMenuText(hdcComp,rc,strText,1);
            }
			else
				DrawMenuText(hdcComp,rc,strText,0);

			if(stMenu.m_wFlag & MF_POPUP)
			{
				RECT rcTemp = rc;
				rcTemp.left = rc.right - 20;
				DrawFrameControl(hdcComp,&rcTemp,DFC_MENU,DFCS_MENUARROW);
			}
		}
		else
		{
			CNikDialog::DrawRectArea(hdcComp
										, rc
										, RGB(255,255,255));
			MoveToEx(hdcComp,rc.left,rc.top+4,NULL);
			LineTo(hdcComp,rc.right,rc.top+4);
			MoveToEx(hdcComp,rc.left,rc.top+6,NULL);
			LineTo(hdcComp,rc.right,rc.top+6);
		}
	}

	BitBlt(hdc,0,0,rcWnd.right,rcWnd.bottom,hdcComp,0,0,SRCCOPY);
	SetBkMode(hdcComp,nBKMode);

	SelectObject(hdcComp,hOldFont);

	DeleteObject(hbmComp);
	DeleteDC(hdcComp);

	ReleaseDC(m_hWnd,hdc);
}

RECT CMenuWnd::SetMenuSize()
{
	RECT rcWnd,rc;
	rc.left = rc.right = rc.top = rc.bottom = 0;
	rcWnd.left = rcWnd.right = rcWnd.bottom = rcWnd.top = 0;

    HDC hdc = GetDC(m_hWnd);
	if(NULL == hdc)
		return rcWnd;

	for(int i=0;i<(int)m_stMenuInfo.m_vMenuList.size();i++)
	{
		MENU_INFO stMenu = m_stMenuInfo.m_vMenuList.at(i);

		if(stMenu.m_strText.length())
		{
			_tstring strText = stMenu.m_strText;

			int nPos = strText.find(_T('\t'),0);
			while(nPos >= 0)
			{
				strText.replace(strText.begin()+nPos
					, strText.begin() + nPos + 1
					, _T("        "));
				nPos = strText.find(_T('\t'),nPos);
			}

			int nTabSize = 4;
			DWORD dwSize = GetTabbedTextExtent(hdc,strText.data()
									, strText.length(),0,&nTabSize);

			rc.right = rc.left + (dwSize & 0xFFFF) + 5;
			rc.bottom = rc.top + (dwSize>>16) + 10;

            m_stMenuInfo.m_vMenuList.at(i).m_rRect = rc;

			rcWnd.bottom = rc.bottom;

			OffsetRect(&rc,0,(rc.bottom-rc.top));
		}
		else
		{
			rc.bottom = rc.top + 10;
			m_stMenuInfo.m_vMenuList.at(i).m_rRect = rc;
			OffsetRect(&rc,0,10);
		}

		if(rcWnd.right < rc.right)
			rcWnd.right = rc.right;
	}

	ReleaseDC(m_hWnd,hdc);

	RECT * pRect;
	for(int i=0;i<(int)m_stMenuInfo.m_vMenuList.size();i++)
	{
        pRect = &m_stMenuInfo.m_vMenuList.at(i).m_rRect;
		pRect->left = 0;
		pRect->right = rcWnd.right;
	}

    return rcWnd;
}

void CMenuWnd::DrawMenuText(HDC hdc
							, RECT &rc
							, _tstring &strText
							, int nHighlightItem)
{
	DRAWTEXTPARAMS stExtParams;
	stExtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	stExtParams.iLeftMargin = 0;
	stExtParams.iRightMargin = 0;
	stExtParams.iTabLength = 8;
	stExtParams.uiLengthDrawn = 0;

	if(nHighlightItem)
		CNikDialog::DrawRectArea(hdc,rc,RGB(216,216,216));
	else
		CNikDialog::DrawRectArea(hdc,rc,RGB(255,255,255));

	DrawTextEx(hdc
		, (LPWSTR)strText.data()
		, strText.length()
		, &rc
		, DT_LEFT | DT_EXPANDTABS | DT_VCENTER | DT_SINGLELINE
		, &stExtParams);
}

void CMenuWnd::DrawMenuItems(HDC hdc)
{
	if(hdc)
	{
		int nBKMode = SetBkMode(hdc,TRANSPARENT);

		RECT rc;
		rc.left = rc.right = rc.top = rc.bottom = 0;

		RECT rcWnd;
		rcWnd.left = rcWnd.right = rcWnd.bottom = rcWnd.top = 0;

		GetClientRect(m_hWnd,&rcWnd);

		HDC hdcComp = CreateCompatibleDC(hdc);
		HBITMAP hbmComp = CreateCompatibleBitmap(hdc, rcWnd.right, rcWnd.bottom);
		SelectObject(hdcComp,hbmComp);

		HFONT hOldFont = (HFONT)SelectObject(hdcComp,m_hFont);

		CNikDialog::DrawLineRect(hdcComp, rcWnd, RGB(0,0,0), 0);

		int nMode = SetBkMode(hdcComp,TRANSPARENT);

		for(int i=0;i<(int)m_stMenuInfo.m_vMenuList.size();i++)
		{
			MENU_INFO stMenu = m_stMenuInfo.m_vMenuList.at(i);

            RECT rec = m_stMenuInfo.m_vMenuList.at(i).m_rRect;

			if(stMenu.m_strText.length())
			{
				_tstring strText = _T(" ");
				strText += stMenu.m_strText;

				DrawMenuText(hdcComp,rec,strText,0);

				if(stMenu.m_wFlag & MF_POPUP)
				{
					RECT rcTemp = rec;
					rcTemp.left = rec.right - 20;
					DrawFrameControl(hdcComp,&rcTemp,DFC_MENU,DFCS_MENUARROW);
				}

				OffsetRect(&rc,0,(rc.bottom-rc.top)+5);
			}
			else
			{
				CNikDialog::DrawRectArea(hdcComp,rec,RGB(255,255,255));
				MoveToEx(hdcComp,rec.left,rec.top+4,NULL);
				LineTo(hdcComp,rec.right,rec.top+4);
				MoveToEx(hdcComp,rec.left,rec.top+6,NULL);
				LineTo(hdcComp,rec.right,rec.top+6);
			}
		}

		BitBlt(hdc,0,0,rcWnd.right,rcWnd.bottom,hdcComp,0,0,SRCCOPY);

		SetBkMode(hdcComp,nMode);

		SelectObject(hdcComp,hOldFont);

		SetBkMode(hdcComp,nBKMode);

		DeleteObject(hbmComp);
		DeleteDC(hdcComp);
	}
}

void CMenuWnd::InitMenuItems()
{
	if(NULL == m_hFont)
	{
		LOGFONT lf;
		memset(&lf,0,sizeof(LOGFONT));

		_tcscpy_s(lf.lfFaceName,sizeof(lf.lfFaceName)/sizeof(TCHAR),MENU_FONT);
		lf.lfClipPrecision		= 2;
		lf.lfHeight				= -14;
		lf.lfQuality			= 1;
		lf.lfWeight				= 400;
		lf.lfOutPrecision		= 3;
		lf.lfPitchAndFamily		= 34;

		m_hFont = CreateFontIndirect(&lf);
	}
}

INT_PTR CALLBACK CMenuWnd::MenuWndProc(HWND hwnd
									   , UINT uMsg
									   , WPARAM wParam
									   , LPARAM lParam)
{
    CMenuWnd * pMenuWnd = (CMenuWnd *)GetWindowLong( hwnd, GWL_USERDATA );

	switch(uMsg)
    {
	case WM_CREATE:
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			if(pMenuWnd)
				pMenuWnd->DrawMenuItems(hdc);
			EndPaint(hwnd,&ps);
		}
		break;

	case WM_MOUSEMOVE:
		if(pMenuWnd)
			pMenuWnd->OnMouseMove(wParam,lParam);
		break;

	case WM_LBUTTONDOWN:
		if(pMenuWnd)
		{
			SetCapture(hwnd);
			pMenuWnd->OnLbuttonDown(wParam,lParam);
		}
		break;

	case WM_LBUTTONUP:
		if(pMenuWnd)
		{
			ReleaseCapture();
			pMenuWnd->OnLbuttonUP(wParam,lParam);
		}
		break;

	//case WM_ERASEBKGND:
	//	break;

	case WM_CLOSE:
		break;

	case WM_DESTROY:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	default: 
        return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	}

	return FALSE;
}

HWND CMenuWnd::CreateMenuWindow( HWND hParent
								, HWND hSubling
								, MENU_INFO &stMenuInfo)
{
    int nWidth = stMenuInfo.m_rRect.right - stMenuInfo.m_rRect.left;

	m_hWnd = CreateWindowEx(0 , CLASS_MENU
									, _T("")
									, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN 
                                        | WS_CLIPSIBLINGS | WS_BORDER
									, nWidth, 30, 0, 0
									, hParent
									, (HMENU)NULL
									, GetWindowInstance(hParent)
									, (LPARAM)NULL );

	ShowWindow( m_hWnd, SW_SHOW );

    SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)this );

	m_stMenuInfo = stMenuInfo;

    SetParentHwnd(hParent);
    InitMenuItems();
    RECT rRect = SetMenuSize();

    if(hSubling)
    {
        RECT rPRect, rSRect;
        GetWindowRect( hParent, &rPRect );
        GetWindowRect( hSubling, &rSRect );
        int nDiffX = rSRect.right - rPRect.left;
        if(nDiffX > 2)
            nDiffX -= 2;
        else
            nDiffX = 2;
        SetWindowPos(m_hWnd, HWND_TOP, nDiffX, 30
            , rRect.right, rRect.bottom, SWP_SHOWWINDOW);
    }
    else
    {
        SetWindowPos(m_hWnd, HWND_TOP, m_stMenuInfo.m_rRect.left, 30
            , rRect.right, rRect.bottom, SWP_SHOWWINDOW);
    }

    m_hToolTip = CreateMyTooltip();

    return m_hWnd;
}
