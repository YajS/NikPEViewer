
#include "Stdafx.h"
#include "PEViewer.h"
#include "DialogWnd.h"

int CNikDialog::RegisterDialogclass(HINSTANCE hInstance)
{
	WNDCLASS    wc;
	memset(&wc,0,sizeof(wc));

	wc.lpfnWndProc		= (WNDPROC)CNikDialog::ResourceWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hIcon			= (HICON)LoadIcon(hInstance,MAKEINTRESOURCE(IDC_NIKPEVIEWER));
    wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName	= CLASS_DIALOG;
	wc.lpszMenuName		= NULL;
    if (!RegisterClass(&wc)) 
		return FALSE;
	return TRUE;
}

int CNikDialog::SetCursorShape(POINT pt)
{
	int nPos = 0;
	if(m_hDlgWnd)
	{
		HCURSOR hCurs;
		switch(DrawRectMarkers(NULL,m_rRect,&pt,1))
		{
		case 4:
			hCurs = LoadCursor(NULL, IDC_SIZENWSE);
			SetCursor(hCurs);
			nPos = 4;
			break;
		case 6:
			hCurs = LoadCursor(NULL, IDC_SIZEWE);
			SetCursor(hCurs);
			nPos = 6;
			break;
		case 8:
			hCurs = LoadCursor(NULL, IDC_SIZENS);
			SetCursor(hCurs);
			nPos = 8;
			break;
		}
	}
	return nPos;
}

void CNikDialog::OnLbuttonDown(WPARAM //wParam
									, LPARAM lParam)
{
	if(m_hDlgWnd)
	{
        SetWindowPos( m_hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		SetCapture(m_hWnd);

		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		m_nPos = SetCursorShape(pt);

		if(m_nPos)
		{
			m_ptStartPos	= pt;
			m_ptEndPos		= pt;
		}
		else
		{
			m_ptEndPos.x	= 0;
			m_ptEndPos.y	= 0;
			m_ptStartPos.x	= 0;
			m_ptStartPos.y	= 0;
		}
	}
}

void CNikDialog::OnLbuttonUP(WPARAM //wParam
								  , LPARAM lParam)
{
	if(m_hDlgWnd)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		SetCursorShape(pt);

		m_ptEndPos.x	= 0;
		m_ptEndPos.y	= 0;
		m_ptStartPos.x	= 0;
		m_ptStartPos.y	= 0;

		m_nPos			= 0;
	}

	ReleaseCapture();
}

void CNikDialog::OnMouseMove(WPARAM wParam
								  , LPARAM lParam)
{
	if(m_hDlgWnd)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		SetCursorShape(pt);

		if(m_nPos 
			&& (wParam & MK_LBUTTON))
		{
			int nDifX = pt.x - m_ptEndPos.x;
			int nDifY = pt.y - m_ptEndPos.y;
			m_ptEndPos = pt;

			RECT rc;

			switch(m_nPos)
			{
			case 4:
				m_rRect.right += nDifX;
				m_rRect.bottom += nDifY;
				GetWindowRect(m_hDlgWnd,&rc);
				rc.right += nDifX;
				rc.bottom += nDifY;
				SetWindowPos(m_hDlgWnd, NULL, 0, 0
					, rc.right-rc.left, rc.bottom-rc.top
					, SWP_NOMOVE|SWP_SHOWWINDOW);

				rc = m_rRect;
				InflateRect(&rc,4+abs(nDifX),4+abs(nDifY));
				InvalidateRect(m_hWnd,&rc,TRUE);
				UpdateWindow(m_hWnd);
				break;
			case 6:
				m_rRect.right += nDifX;
				GetWindowRect(m_hDlgWnd,&rc);
				rc.right += nDifX;
				SetWindowPos(m_hDlgWnd, NULL, 0, 0
					, rc.right-rc.left, rc.bottom-rc.top
					, SWP_NOMOVE|SWP_SHOWWINDOW);

				rc = m_rRect;
				InflateRect(&rc,4+abs(nDifX),8);
				InvalidateRect(m_hWnd,&rc,TRUE);
				UpdateWindow(m_hWnd);
				break;
			case 8:
				m_rRect.bottom += nDifY;

				GetWindowRect(m_hDlgWnd,&rc);
				rc.bottom += nDifY;
				SetWindowPos(m_hDlgWnd, NULL, 0, 0
					, rc.right-rc.left, rc.bottom-rc.top
					, SWP_NOMOVE|SWP_SHOWWINDOW);

				rc = m_rRect;
				InflateRect(&rc,8,4+abs(nDifY));
				InvalidateRect(m_hWnd,&rc,TRUE);

				UpdateWindow(m_hWnd);
				break;
			}
		}
	}
}

void CNikDialog::DrawLineRect( HDC hdc
								 , RECT rc
								 , COLORREF cLineCol
								 , int nLineWidth)
{
	if(hdc)
	{
		HPEN hOldPen,hPen = CreatePen(PS_SOLID,nLineWidth,cLineCol);
		hOldPen = (HPEN)SelectObject(hdc,hPen);

		MoveToEx(hdc,rc.left,rc.top,NULL);
		LineTo(hdc,rc.right,rc.top);
		LineTo(hdc,rc.right,rc.bottom);
		LineTo(hdc,rc.left,rc.bottom);
		LineTo(hdc,rc.left,rc.top-1);

		SelectObject(hdc,hOldPen);
		DeleteObject(hPen);
	}
}

int CNikDialog::DrawRectMarkers(HDC hdc
									, RECT rc
									, POINT *pt
									, int nFlag)
{
	int nRect = 0;

	if(nFlag 
		&& (NULL == pt))
		return 0;
	else if((0 == nFlag) 
		&& (NULL == hdc))
		return 0;

	RECT rc1;
	int nWid = (rc.right - rc.left)>>1;
	int nHei = (rc.bottom - rc.top)>>1;

	rc1.left = rc1.right = rc.left;
	rc1.bottom = rc1.top = rc.top;
	InflateRect(&rc1,2,2);
	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	rc1.left = rc1.right = rc.right;
	rc1.bottom = rc1.top = rc.top;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	rc1.left = rc1.right = rc.left;
	rc1.bottom = rc1.top = rc.bottom;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	rc1.left = rc1.right = rc.right;
	rc1.bottom = rc1.top = rc.bottom;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	///////////////////////////////////////

	rc1.left = rc1.right = nWid;
	rc1.bottom = rc1.top = rc.top;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	rc1.left = rc1.right = rc.right;
	rc1.bottom = rc1.top = nHei;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	rc1.left = rc1.right = rc.left;
	rc1.bottom = rc1.top = nHei;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	rc1.left = rc1.right = nWid;
	rc1.bottom = rc1.top = rc.bottom;
	InflateRect(&rc1,2,2);

	if(nFlag)
	{
		nRect++;
		if(PtInRect(&rc1,pt[0]))
			return nRect;
	}
	else
		DrawLineRect(hdc,rc1,RGB(0,128,0),1);

	return 0;
}

void CNikDialog::DrawDialogBorder(HDC hdc)
{
	if(hdc && m_hDlgWnd)
	{
		DrawLineRect(hdc,m_rRect,RGB(0,128,0),2);
		DrawRectMarkers(hdc,m_rRect,NULL,0);
	}
}

void CNikDialog::DrawDialogBox(HDC hdc)
{
	if(m_hDlgWnd)
	{
		DrawDialogBorder(hdc);
		return;
	}

	if(hdc 
		&& m_vDialogList.size())
	{
		HRGN hrn = NULL;
		DIALOGCONTROL * pDlgTemp = NULL;
		HFONT hOldFont = NULL,hFont = NULL;
		for(int i=0;i<(int)m_vDialogList.size();i++)
		{
			pDlgTemp = &m_vDialogList.at(i);
			if(pDlgTemp)
			{
				if(0 == i)
				{
					hFont = CreateFontIndirect(&pDlgTemp->m_lf);

					hOldFont = (HFONT)SelectObject(hdc,hFont);

					if(1 == pDlgTemp->m_nFlag)
					{
						RECT rc;
						rc.left		= pDlgTemp->m_rRect.left;
						rc.top		= pDlgTemp->m_rRect.top - GetSystemMetrics(SM_CYCAPTION);
						rc.right	= pDlgTemp->m_rRect.right;
						rc.bottom	= pDlgTemp->m_rRect.top;

						InflateRect(&rc,1,0);

						DrawRectArea(hdc,rc,GetSysColor(COLOR_ACTIVECAPTION));

						COLORREF col = SetTextColor(hdc,GetSysColor(COLOR_CAPTIONTEXT));

						int nMode = SetBkMode(hdc,TRANSPARENT);

						rc.left += 5;

						DrawText(hdc, pDlgTemp->m_strCaption
							, wcslen(pDlgTemp->m_strCaption)
							, &rc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

						rc.left -= 5;

						SetTextColor(hdc,col);

						SetBkMode(hdc,nMode);
					}

					HRGN hrn = CreateRectRgn(pDlgTemp->m_rRect.left-2
										, pDlgTemp->m_rRect.top-2
										, pDlgTemp->m_rRect.right+2
										, pDlgTemp->m_rRect.bottom+2);
					SelectClipRgn(hdc,hrn);
				}

				if(0 != i)
				{
					if(0 == _tcscmp(pDlgTemp->m_strClassName,L"0x82"))
					{
						
						if(pDlgTemp->m_lStyle & (ES_CENTER<<16))
						{
							DrawText(hdc,pDlgTemp->m_strCaption
								, _tcslen(pDlgTemp->m_strCaption)
								, &pDlgTemp->m_rRect
								, DT_CENTER);
						}
						else if(pDlgTemp->m_lStyle & (ES_RIGHT<<16))
						{
							DrawText(hdc,pDlgTemp->m_strCaption
								, _tcslen(pDlgTemp->m_strCaption)
								, &pDlgTemp->m_rRect
								, DT_RIGHT);
						}
						else
						{
							DrawText(hdc,pDlgTemp->m_strCaption
								, _tcslen(pDlgTemp->m_strCaption)
								, &pDlgTemp->m_rRect
								, DT_LEFT);
						}
					}
					else if(0 == _tcscmp(pDlgTemp->m_strClassName,L"0x80"))
					{
						DrawText(hdc,pDlgTemp->m_strCaption
							, _tcslen(pDlgTemp->m_strCaption)
							, &pDlgTemp->m_rRect
							, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
					}
				}

				DrawLineRect(hdc,pDlgTemp->m_rRect,RGB(0,0,0),1);
			}
		}

		SelectClipRgn(hdc,NULL);

		SelectObject(hdc,hOldFont);

		if(hFont)
			DeleteObject(hFont);

		if(hrn)
			DeleteObject(hrn);
	}
}

void CNikDialog::DrawRectArea(HDC hdc
								 , RECT rc
								 ,COLORREF col)
{
	if(hdc)
	{
		HBRUSH hBrush = CreateSolidBrush(col);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);

		FillRect(hdc,&rc,hBrush);

		SelectObject(hdc,hOldBrush);

		DeleteObject(hBrush);
	}
}

LRESULT CALLBACK CNikDialog::ResourceWndProc(HWND hwnd, UINT uMsg,
							WPARAM wParam, LPARAM lParam)
{
    CNikDialog * pDialogEditor = (CNikDialog *)GetWindowLong( hwnd, GWL_USERDATA );

    switch(uMsg)
    {
		case WM_SIZE:
		if(pDialogEditor)
			pDialogEditor->OnSize(wParam,lParam);
		break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd,&ps);

                hdc;
				//if(pDialogEditor)
				//	pDialogEditor->DrawDialogBox(hdc);

				EndPaint(hwnd,&ps);
			}
			break;

		case WM_CHAR:
			{
				if(0x1B == wParam)
				{
					if(pDialogEditor)
						pDialogEditor->DestroyDialogBox();
				}
			}
			break;

		case WM_MOUSEMOVE:
		//if(pDialogEditor)
		//	pDialogEditor->OnMouseMove(wParam,lParam);
		break;

	case WM_LBUTTONDOWN:
		//if(pDialogEditor)
		//{
		//	SetCapture(hwnd);
		//	pDialogEditor->OnLbuttonDown(wParam,lParam);
		//}
		break;

	case WM_LBUTTONUP:
		//if(pDialogEditor)
		//{
		//	ReleaseCapture();
		//	pDialogEditor->OnLbuttonUP(wParam,lParam);
		//}
		break;

        case WM_CLOSE:
            ShowWindow( hwnd, SW_HIDE );
        break;

		case WM_DESTROY:
		{
			if(pDialogEditor)
				pDialogEditor->DestroyDialogBox();
		}
        return DefWindowProc( hwnd, uMsg, wParam, lParam );

	default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
    }

    return 0;
}

void CNikDialog::OnSize(WPARAM //wParam
							, LPARAM //lParam
                            )
{
}

INT_PTR CALLBACK CNikDialog::DialogWndProc( HWND //hwnd
                                              , UINT uMsg
                                              , WPARAM wParam
                                              , LPARAM //lParam
                                              )
{
    switch(uMsg)
    {
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		{
			switch(wParam & 0xFFFF)
			{
			case IDCANCEL:
			case IDOK:
				//if(pDialogEditor)
				//	pDialogEditor->DestroyDialogBox();
				//else
				//	EndDialog(hwnd,(wParam & 0xFFFF));
				break;
			}
		}
		break;

	case WM_CLOSE:
		//if(pDialogEditor)
		//	pDialogEditor->DestroyDialogBox();
		//else
		//	EndDialog(hwnd,IDCANCEL);
		break;

	default: 
        return FALSE;
	}
	return TRUE;
}

HWND CNikDialog::CreateDialogWindow(HWND hParent)
{
    m_hParentWnd = hParent;

    DWORD dwStyle = WS_TABSTOP |
                    WS_VISIBLE |
                    WS_SIZEBOX |
                    WS_CAPTION |
                    WS_SYSMENU |
                    //WS_VSCROLL | 
                    //WS_HSCROLL |
                    WS_CHILDWINDOW |
                    WS_CLIPSIBLINGS |
                    0;

    m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW
                            , CLASS_DIALOG
						    , DIALOG_CAPTION
						    , dwStyle
						    , 200, 100, 200, 400
						    , hParent
                            , NULL
						    , GetWindowInstance(hParent)
						    , (LPARAM)NULL );

    return m_hWnd;
}

int CNikDialog::MakeDialogList(RESOURCELIST * pRes)
{
	m_vDialogList.clear();

	if(pRes)
	{
		BYTE * pBufferTemp = new BYTE [pRes->m_nSize+8];
		if(NULL == pBufferTemp)
			return -2;

		memcpy(pBufferTemp,&m_pBuffer[pRes->m_nOffsetToData],pRes->m_nSize+8);

		BYTE * pBuf = &m_pBuffer[pRes->m_nOffsetToData];

		int bHasMenu = FALSE;

		int nBufSize = pRes->m_nSize;

		int nStructLen = 0;

		DestroyDialogBox();

		HINSTANCE hInst = (HINSTANCE)GetWindowInstance(m_hParentWnd);

		BYTE * pBackupBuf = pBuf;

		DIALOGCONTROL * pDlgTemp = (DIALOGCONTROL *)pBuf;
		DIALOGCONTROL stDialogCtrl;

		if(0xFFFF0001 == pDlgTemp->m_lStyle)
		{
			pBuf += 8;
			stDialogCtrl.m_dwHelpID = pDlgTemp->m_lExStyle;
			pDlgTemp = (DIALOGCONTROL *)pBuf;
			stDialogCtrl.m_dwIsExtend = 1;
		}

		if(FALSE == stDialogCtrl.m_dwIsExtend)
		{
			stDialogCtrl.m_lStyle = pDlgTemp->m_lStyle;
			stDialogCtrl.m_lExStyle = pDlgTemp->m_lExStyle;
			stDialogCtrl.m_NoOfItems = pDlgTemp->m_NoOfItems;
			stDialogCtrl.m_wX = pDlgTemp->m_wX;
			stDialogCtrl.m_wY = pDlgTemp->m_wY;
			stDialogCtrl.m_wCX = pDlgTemp->m_wCX;
			stDialogCtrl.m_wCY = pDlgTemp->m_wCY;
		}
		else
		{
			stDialogCtrl.m_lStyle = pDlgTemp->m_lExStyle;
			stDialogCtrl.m_lExStyle = pDlgTemp->m_lStyle;
			stDialogCtrl.m_NoOfItems = pDlgTemp->m_NoOfItems;
			stDialogCtrl.m_wX = pDlgTemp->m_wX;
			stDialogCtrl.m_wY = pDlgTemp->m_wY;
			stDialogCtrl.m_wCX = pDlgTemp->m_wCX;
			stDialogCtrl.m_wCY = pDlgTemp->m_wCY;
		}

		DWORD dwBaseUnit = GetDialogBaseUnits();

		DWORD * pdwTemp = (DWORD *)&pDlgTemp->m_wControlID;

		if(0xFFFF == (pdwTemp[0]&0xFFFF))
		{
			bHasMenu = TRUE;

			pdwTemp[0] = 0;
			BYTE * pTemp_1 = (BYTE *)&pdwTemp[0] + 2;
			BYTE * pTemp_2 = (BYTE *)&pdwTemp[0] + 4;
			int nSize = nBufSize - ((DWORD)pTemp_2-(DWORD)pBuf);
			memcpy(pTemp_1,pTemp_2,nSize);
			pdwTemp = (DWORD *)pTemp_1;
		}
		else
		{
			BYTE * pTemp = (BYTE *)pdwTemp;
			int nLen = 2;
			if(((WCHAR *)pdwTemp)[0])
			{
                if(1023 > _tcslen((WCHAR *)pdwTemp))
				    _tcscpy_s(stDialogCtrl.m_strMenuName,1023,(WCHAR *)pdwTemp);
                else
                {
                    delete [] pBufferTemp;
                    return -1;
                }

				nLen += (_tcslen(stDialogCtrl.m_strMenuName) * 2);
			}
			pdwTemp = (DWORD *)(pTemp + nLen);
		}

		if(0xFFFF == (pdwTemp[0]&0xFFFF))
		{
			_stprintf_s(stDialogCtrl.m_strClassName,1023,_T("0x%X"),(WORD)(pdwTemp[0]>>16));
			pdwTemp++;
		}
		else
		{
			int nLen = 2;
			BYTE * pTemp = (BYTE *)pdwTemp;
			if(((WCHAR *)pdwTemp)[0])
			{
                if(1023 > _tcslen((WCHAR *)pdwTemp))
				    _tcscpy_s(stDialogCtrl.m_strClassName,1023,(WCHAR *)pdwTemp);
                else
                {
                    delete [] pBufferTemp;
                    return -1;
                }

				nLen += (_tcslen(stDialogCtrl.m_strClassName) * 2);
			}
			pdwTemp = (DWORD *)(pTemp + nLen);
		}

		{
			int nLen = 2;
			BYTE * pTemp = (BYTE *)pdwTemp;
			if(((WCHAR *)pdwTemp)[0])
			{
                if(1023 > _tcslen((WCHAR *)pdwTemp))
				    _tcscpy_s(stDialogCtrl.m_strCaption,1023,(WCHAR *)pdwTemp);
                else
                {
                    delete [] pBufferTemp;
                    return -1;
                }

				nLen += (_tcslen(stDialogCtrl.m_strCaption) * 2);
			}
			pdwTemp = (DWORD *)(pTemp + nLen);
		}

		if(stDialogCtrl.m_lStyle & DS_SETFONT)
		{
			stDialogCtrl.m_wPointSize = (WORD)(pdwTemp[0] & 0xFFFF);
			pdwTemp = (DWORD *)(((BYTE *)pdwTemp) + 2);

			if(stDialogCtrl.m_dwIsExtend)
			{
				stDialogCtrl.m_wWeight = (WORD)(pdwTemp[0] & 0xFFFF);
				stDialogCtrl.m_bItalic = (BYTE)((pdwTemp[0] >> 16) & 0xFF);
				stDialogCtrl.m_bCharset = (BYTE)(pdwTemp[0] >> 24);
				pdwTemp++;
			}

			int nLen = 0;
			BYTE * pTemp = (BYTE *)pdwTemp;
			if(((WCHAR *)pdwTemp)[0])
			{
                if(1023 > _tcslen((WCHAR *)pdwTemp))
				    _tcscpy_s(stDialogCtrl.m_strFontName,1023,(WCHAR *)pdwTemp);
                else
                {
                    delete [] pBufferTemp;
                    return -1;
                }

				nLen += (_tcslen(stDialogCtrl.m_strFontName) * 2);
			}
			pdwTemp = (DWORD *)(pTemp + nLen);

			if(stDialogCtrl.m_wPointSize)
			{
				int nPoints = stDialogCtrl.m_wPointSize * 2;

				memset(&stDialogCtrl.m_lf,0,sizeof(stDialogCtrl.m_lf));
				stDialogCtrl.m_lf.lfItalic = stDialogCtrl.m_bItalic;
				stDialogCtrl.m_lf.lfWeight = stDialogCtrl.m_wWeight;
				stDialogCtrl.m_lf.lfCharSet = stDialogCtrl.m_bCharset;
				stDialogCtrl.m_lf.lfHeight = nPoints;//MulDiv(nPoints,nPoints,dwBaseUnit>>16);
                if(32 > _tcslen(stDialogCtrl.m_strFontName))
				    wcscpy_s(stDialogCtrl.m_lf.lfFaceName,32,stDialogCtrl.m_strFontName);
                else
                {
                    delete [] pBufferTemp;
                    return -1;
                }

				HDC hdc = GetDC(m_hWnd);
				if(hdc)
				{
					HFONT hOldFont, hFont = CreateFontIndirect(&stDialogCtrl.m_lf);

					hOldFont = (HFONT)SelectObject(hdc,hFont);

					SIZE sSize;
					int nX = wcslen(stDialogCtrl.m_strFontName);
					GetTextExtentPoint32(hdc,stDialogCtrl.m_strFontName
						, nX, &sSize);

					nX = MulDiv(sSize.cx,1,nX) + 2;

					dwBaseUnit = (nPoints<<16) | nX;

					SelectObject(hdc,hOldFont);
					ReleaseDC(m_hWnd,hdc);
				}
			}
		}

		stDialogCtrl.m_rRect.left = MulDiv(pDlgTemp->m_wX, dwBaseUnit&0xFFFF, 4);
		stDialogCtrl.m_rRect.top = MulDiv(pDlgTemp->m_wY, dwBaseUnit>>16, 8);
		stDialogCtrl.m_rRect.right = stDialogCtrl.m_rRect.left 
										+ MulDiv(pDlgTemp->m_wCX, dwBaseUnit&0xFFFF, 4);
		stDialogCtrl.m_rRect.bottom = stDialogCtrl.m_rRect.top 
										+ MulDiv(pDlgTemp->m_wCY, dwBaseUnit>>16, 8);

		OffsetRect(&stDialogCtrl.m_rRect
			, -stDialogCtrl.m_rRect.left
			, -stDialogCtrl.m_rRect.top);

		if(bHasMenu)
		{
			int nSize = ((DWORD)pdwTemp-(DWORD)pBuf);
			nSize = (nSize % 4)%4;
			if(nSize)
			{
				BYTE * pTemp_1 = (BYTE *)&pdwTemp[0] + (4-nSize);
				nSize = nBufSize - ((DWORD)pTemp_1-(DWORD)pBuf);
				memcpy(pdwTemp,pTemp_1,nSize);
			}
		}

		m_hDlgWnd = CreateDialogIndirectParam(hInst
						, (LPCDLGTEMPLATE)pBackupBuf, m_hWnd
						, (DLGPROC)DialogWndProc, NULL);

		if(m_hDlgWnd)
		{
			stDialogCtrl.m_hWnd = m_hDlgWnd;
			DWORD dwStyle = stDialogCtrl.m_lStyle;

			dwStyle &= (~WS_POPUP);
			dwStyle = dwStyle | WS_CHILD | WS_BORDER | WS_SYSMENU;
			SetWindowLong(m_hDlgWnd,GWL_STYLE,dwStyle);

			SetParent(m_hDlgWnd,m_hWnd);
			SetWindowPos(m_hDlgWnd,NULL,4,4,0,0,SWP_NOSIZE);
			EnableWindow(m_hDlgWnd,FALSE);
			ShowWindow(m_hDlgWnd,SW_SHOW);

			{
				GetWindowRect(m_hDlgWnd,&m_rRect);
				int nWid = m_rRect.right - m_rRect.left;
				int nHei = m_rRect.bottom - m_rRect.top;

				POINT pt;
				pt.x = m_rRect.left;
				pt.y = m_rRect.top;
				ScreenToClient(m_hWnd,&pt);

				m_rRect.left = pt.x;
				m_rRect.right = m_rRect.left + nWid;
				m_rRect.top = pt.y;
				m_rRect.bottom = m_rRect.top + nHei;

				InflateRect(&m_rRect,2,2);
			}
		}

		int nCaptionWidth = 0;

		if((WS_CAPTION & stDialogCtrl.m_lStyle) 
			|| (WS_OVERLAPPED & stDialogCtrl.m_lStyle)
			|| (WS_SYSMENU & stDialogCtrl.m_lStyle)
			|| (WS_TILED & stDialogCtrl.m_lStyle))
		{
			nCaptionWidth = GetSystemMetrics(SM_CYCAPTION);
			stDialogCtrl.m_nFlag = TRUE;
			OffsetRect(&stDialogCtrl.m_rRect,0,nCaptionWidth);
		}

		m_vDialogList.push_back(stDialogCtrl);

		nStructLen = (BYTE *)pdwTemp - (BYTE *)&m_pBuffer[pRes->m_nOffsetToData] + 2;

		if(nStructLen%4)
		{
			pdwTemp = (DWORD *)(((BYTE *)pdwTemp) + 2);
			nStructLen += 2;
		}

		for(int i=0;i<stDialogCtrl.m_NoOfItems;i++)
		{
			nStructLen = 0;

			DIALOGCONTROL stDlgCtrl;
			pDlgTemp = (DIALOGCONTROL *)pdwTemp;

			if(stDialogCtrl.m_dwIsExtend)
			{
				stDlgCtrl.m_dwHelpID	= pdwTemp[0];
				stDlgCtrl.m_dwIsExtend	= TRUE;

				pdwTemp++;
				pDlgTemp				= (DIALOGCONTROL *)pdwTemp;

				stDlgCtrl.m_lStyle		= pDlgTemp->m_lExStyle;
				stDlgCtrl.m_lExStyle	= pDlgTemp->m_NoOfItems;
				stDlgCtrl.m_wX			= pDlgTemp->m_wX;
				stDlgCtrl.m_wY			= pDlgTemp->m_wY;
				stDlgCtrl.m_wCX			= pDlgTemp->m_wCX;
				stDlgCtrl.m_wCY			= pDlgTemp->m_wCY;
				stDlgCtrl.m_wControlID	= pDlgTemp->m_wControlID;
			}
			else
			{
				stDlgCtrl.m_lStyle		= pDlgTemp->m_lStyle;
				stDlgCtrl.m_lExStyle	= pDlgTemp->m_lExStyle;
				stDlgCtrl.m_wX			= pDlgTemp->m_wX;
				stDlgCtrl.m_wY			= pDlgTemp->m_wY;
				stDlgCtrl.m_wCX			= pDlgTemp->m_wCX;
				stDlgCtrl.m_wCY			= pDlgTemp->m_wCY;
				stDlgCtrl.m_wControlID	= pDlgTemp->m_wControlID;
			}

			if(m_hDlgWnd)
			{
				stDlgCtrl.m_hWnd = GetDlgItem(m_hDlgWnd
									, stDlgCtrl.m_wControlID);
			}

			stDlgCtrl.m_rRect.left	= MulDiv(pDlgTemp->m_wX, dwBaseUnit&0xFFFF, 4);
			stDlgCtrl.m_rRect.top	= MulDiv(pDlgTemp->m_wY, dwBaseUnit>>16, 8);
			stDlgCtrl.m_rRect.right = stDlgCtrl.m_rRect.left 
											+ MulDiv(pDlgTemp->m_wCX, dwBaseUnit&0xFFFF, 4);
			stDlgCtrl.m_rRect.bottom = stDlgCtrl.m_rRect.top 
											+ MulDiv(pDlgTemp->m_wCY, dwBaseUnit>>16, 8);

			OffsetRect(&stDlgCtrl.m_rRect
				, stDialogCtrl.m_rRect.left
				, stDialogCtrl.m_rRect.top);

			nStructLen = ((DWORD)((BYTE *)&pDlgTemp->m_wPointSize) - (DWORD)((BYTE *)pdwTemp));

			pdwTemp = (DWORD *)&pDlgTemp->m_wPointSize;

			if(stDialogCtrl.m_dwIsExtend)
			{
				pdwTemp = (DWORD *)((BYTE *)pdwTemp + 2);
				nStructLen += 2;
			}

			if(0xFFFF == (pdwTemp[0]&0xFFFF))
			{
				_stprintf_s(stDlgCtrl.m_strClassName,1023,_T("0x%X"),(WORD)(pdwTemp[0]>>16));
				pdwTemp++;
				nStructLen += sizeof(DWORD);
			}
			else
			{
				int nLen = 2;
				BYTE * pTemp = (BYTE *)pdwTemp;
				if(((WCHAR *)pdwTemp)[0])
				{
                    if(1023 > _tcslen((WCHAR *)pdwTemp))
					    _tcscpy_s(stDlgCtrl.m_strClassName,1023,(WCHAR *)pdwTemp);
                    else
                    {
                        delete [] pBufferTemp;
                        return -1;
                    }

					nLen += (_tcslen(stDlgCtrl.m_strClassName) * 2);
				}
				pdwTemp = (DWORD *)(pTemp + nLen);
				nStructLen += nLen;

				if(nStructLen%4)
				{
					pdwTemp = (DWORD *)(((BYTE *)pdwTemp) + 2);
					nStructLen += 2;
				}
			}

			if(0xFFFF == (pdwTemp[0]&0xFFFF))
			{
				_stprintf_s(stDlgCtrl.m_strCaption,1023,_T("0x%X"),(WORD)(pdwTemp[0]>>16));
				pdwTemp++;
				nStructLen += sizeof(DWORD);
			}
			else
			{
				int nLen = 2;
				BYTE * pTemp = (BYTE *)pdwTemp;
				if(((WCHAR *)pdwTemp)[0])
				{
                    if(1023 > _tcslen((WCHAR *)pdwTemp))
					    _tcscpy_s(stDlgCtrl.m_strCaption,1023,(WCHAR *)pdwTemp);
                    else
                    {
                        delete [] pBufferTemp;
                        return -1;
                    }

					nLen += (_tcslen(stDlgCtrl.m_strCaption) * 2);
				}
				pdwTemp = (DWORD *)(pTemp + nLen);
				nStructLen += nLen;

				if(nStructLen%4)
				{
					pdwTemp = (DWORD *)(((BYTE *)pdwTemp) + 2);
					nStructLen += 2;
				}
			}

			if(nStructLen%4)
			{
				pdwTemp = (DWORD *)((BYTE *)pdwTemp + 2);
				nStructLen += 2;
			}

			m_vDialogList.push_back(stDlgCtrl);
		}

		memcpy(&m_pBuffer[pRes->m_nOffsetToData],pBufferTemp,pRes->m_nSize+8);

		delete [] pBufferTemp;

		InvalidateRect(m_hWnd,NULL,TRUE);
		UpdateWindow(m_hWnd);
	}

    return 0;
}

HWND CNikDialog::SetDialogData( HWND hParent
                               , RESOURCELIST * pRes
                               , BYTE * pBuffer
                               , int nFlag)
{
    if ((!pRes) 
        || (!pBuffer))
        return NULL;

    int nCount = 0xFFFF & (nFlag>>16);
    if(nCount > 0)
        nCount--;

    nFlag = 0xFFFF & nFlag;

    if(hParent 
        || (!m_hWnd))
    {
        CreateDialogWindow( hParent );
        SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)this );
        m_pBuffer = pBuffer;
    }

    if(m_hWnd)
    {
        _tstring strCaption = _T("Resource dialog Viewer");

        if(pRes->m_chName[0])
        {
            strCaption += _T(" - ");
            strCaption += pRes->m_chName;
        }

        SetWindowText( m_hWnd, strCaption.c_str());

	    if(MakeDialogList( pRes ) < 0)
            return 0;

        POINT pt;
        pt.x = 20 * nCount;
        pt.y = 20 * nCount;
        ShowWindowPosition( (NULL != hParent), pt );
    }

    return m_hWnd;
}