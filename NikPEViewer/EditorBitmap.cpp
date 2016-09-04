
#include "Stdafx.h"
#include "EditorBitmap.h"

void CNikBitmapEditor::SetScrollBar()
{
	// Set the vertical scrolling range and page size
	SCROLLINFO si;

	GetScrollInfo(m_hWnd,SB_VERT,&si);

	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin   = 0;
	si.nMax   = m_stBitmapHeader.biHeight;

	si.nPage  = m_rDrawArea.bottom;

	SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

	GetScrollInfo(m_hWnd,SB_HORZ,&si);

	si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE;
    si.nMin   = 0;
	si.nMax   = m_stBitmapHeader.biWidth;

	si.nPage  = m_rDrawArea.right;

	SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
}

void CNikBitmapEditor::OnScrollBar(int nFlag
								, WPARAM wParam)
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;

	int nDiffX;
	int nDiffY;

	if(WM_VSCROLL == nFlag)
	{
		GetScrollInfo (m_hWnd, SB_VERT, &si);

		int nCurPos = si.nPos;

		switch (LOWORD (wParam))
		{
			// user clicked the HOME keyboard key
		case UM_WHEELUP:
			si.nPos -= 60;
			break;

		case UM_WHEELDOWN:
			si.nPos += 60;
			break;

		case SB_TOP:
			si.nPos = si.nMin;
			break;

			// user clicked the END keyboard key
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

			// user clicked the top arrow
		case SB_LINEUP:
			si.nPos -= 20;
			break;

			// user clicked the bottom arrow
		case SB_LINEDOWN:
			si.nPos += 20;
			break;

			// user clicked the scroll bar shaft above the scroll box
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

			// user clicked the scroll bar shaft below the scroll box
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

			// user dragged the scroll box
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break; 
		}
		// Set the position and then retrieve it.  Due to adjustments
		//   by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo (m_hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo (m_hWnd, SB_VERT, &si);
		// If the position has changed, scroll window and update it
		if (si.nPos != nCurPos)
		{
			nDiffY = m_nOffsetY - si.nPos;
			m_nOffsetY = si.nPos;
			ScrollWindowEx(m_hWnd, 0,nDiffY
							, NULL, NULL, NULL, NULL, SW_INVALIDATE);
			UpdateWindow (m_hWnd);
		}
	}
	else if(WM_HSCROLL == nFlag)
	{
		GetScrollInfo (m_hWnd, SB_HORZ, &si);

		int nCurPos = si.nPos;

		switch (LOWORD (wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;

			// user clicked the END keyboard key
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

			// user clicked the top arrow
		case SB_LINEUP:
			si.nPos -= 20;
			break;

			// user clicked the bottom arrow
		case SB_LINEDOWN:
			si.nPos += 20;
			break;

			// user clicked the scroll bar shaft above the scroll box
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

			// user clicked the scroll bar shaft below the scroll box
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

			// user dragged the scroll box
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break; 
		}
		// Set the position and then retrieve it.  Due to adjustments
		//   by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo (m_hWnd, SB_HORZ, &si, TRUE);
		GetScrollInfo (m_hWnd, SB_HORZ, &si);
		// If the position has changed, scroll window and update it
		if (si.nPos != nCurPos)
		{
			nDiffX = m_nOffsetX - si.nPos;
			m_nOffsetX = si.nPos;
			ScrollWindowEx(m_hWnd, nDiffX,0
							, NULL, NULL, NULL, NULL, SW_INVALIDATE);
			UpdateWindow (m_hWnd);
		}
	}
}

void CNikBitmapEditor::DrawRectArea(HDC hdc
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

void CNikBitmapEditor::DrawLineRect(HDC hdc, RECT rc)
{
	if(hdc)
	{
		HPEN hOldPen,hPen = CreatePen(PS_SOLID,1,LINE_NUMBER_COLOR);
		hOldPen = (HPEN)SelectObject(hdc,hPen);

		MoveToEx(hdc,rc.left,rc.top,NULL);
		LineTo(hdc,rc.right,rc.top);
		LineTo(hdc,rc.right,rc.bottom);
		LineTo(hdc,rc.left,rc.bottom);
		LineTo(hdc,rc.left,rc.top);

		SelectObject(hdc,hOldPen);
		DeleteObject(hOldPen);
	}
}

void CNikBitmapEditor::DrawSideBar(HDC hdc)
{
	if(hdc)
	{
		RECT rc = m_rDrawArea;

		rc.right = m_nLeftBoundary;

		DrawRectArea(hdc,m_rDrawArea,WINDOW_BACK_COLOR);
	}
}

BYTE * CNikBitmapEditor::ReadBinaryFile(_tstring strFile
										   , DWORD &dwFileSize)
{
	BYTE * pBuffer = NULL;
	HANDLE hFile = CreateFile(strFile.data(), GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);					
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	dwFileSize = GetFileSize(hFile,NULL);
	if(dwFileSize)
	{
		pBuffer = new BYTE [dwFileSize + 2];
		if(pBuffer)
		{
			pBuffer[dwFileSize] = 0;
			pBuffer[dwFileSize+1] = 0;
			DWORD dwRead = 0;
			ReadFile(hFile,pBuffer,dwFileSize,&dwRead,NULL);
			if(dwRead == dwFileSize)
			{
				return pBuffer;
			}
		}
		delete [] pBuffer;
		pBuffer = NULL;
	}
	return NULL;
}

void CNikBitmapEditor::DrawBitmapEditor(HDC hdc
							   , RECT rPaint
)
{
	if(hdc && m_hImageBitmap)
	{
		HDC memDC = CreateCompatibleDC ( hdc );
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC,m_hImageBitmap);

		GetClientRect(m_hWnd,&rPaint);

		if(rPaint.right > m_stBitmapHeader.biWidth)
			rPaint.right = m_stBitmapHeader.biWidth;

		if((rPaint.bottom) > m_stBitmapHeader.biHeight)
			rPaint.bottom = m_stBitmapHeader.biHeight;

		BitBlt(hdc,-m_nOffsetX,-m_nOffsetY
			, rPaint.right+m_nOffsetX
			, rPaint.bottom+m_nOffsetY
			, memDC,0,0,SRCCOPY);

		SelectObject(memDC,hOldBitmap);
		DeleteDC(memDC);
	}
}

int CNikBitmapEditor::CreateBitmap(IStream * pIStream
									  , DWORD dwSize)
{
	if(pIStream 
		&& (dwSize > 0))
	{
		Color cColor;
		{
			if(m_hImageBitmap)
            {
				DeleteObject(m_hImageBitmap);
                m_hImageBitmap = NULL;
            }

			{
				Bitmap hBitmap(pIStream,FALSE);
				hBitmap.GetHBITMAP(cColor,&m_hImageBitmap);
			}

			{
				memset(&m_stBitmapHeader,0,sizeof(BITMAPINFO));
				m_stBitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
				HDC hdc = GetDC(m_hWnd);
				GetDIBits(hdc,m_hImageBitmap,0,0,NULL,(BITMAPINFO *)&m_stBitmapHeader,DIB_RGB_COLORS);
				ReleaseDC(m_hWnd,hdc);
			}
		}
	}
	return 0;
}

HWND CNikBitmapEditor::CreateBitmapWindowBuf(HWND hParent
                                             , BYTE * pBuffer
                                             , _tstring strID
                                             , int nSize
                                             , int nFlag)
{
    if((!pBuffer) 
        || (nSize<=0))
        return NULL;

    int nCount = 0xFFFF & (nFlag>>16);
    if(nCount > 0)
        nCount--;

    nFlag = 0xFFFF & nFlag;

    if(hParent 
        || (!m_hWnd))
    {
        m_hParentWnd = hParent;
        m_hWnd = CreateBitmapWindow(hParent);
        SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)this );
    }

    if(m_hWnd)
    {
        _tstring strCaption;
        if(CURSOR_DATA == nFlag)
            strCaption = _T("Resource cursor Viewer");
        else if(BITMAP_DATA == nFlag)
            strCaption = _T("Resource bitmap Viewer");
        else if(ICON_DATA == nFlag)
            strCaption = _T("Resource icon Viewer");

        if(!strCaption.empty())
        {
            strCaption += _T(" - ");
            strCaption += strID;
        }

        SetWindowText( m_hWnd, strCaption.c_str());

	    if(CreateBitmapfromBuffer(pBuffer, nSize, nFlag) < 0)
            return 0;

        POINT pt;
        pt.x = 20 * nCount;
        pt.y = 20 * nCount;
        ShowWindowPosition((NULL != hParent), pt);
    }

    return m_hWnd;
}

int CNikBitmapEditor::CreateBitmapfromBuffer(BYTE * pBuffer
                                              , int nSize
                                              , int nFlag)
{
	if(pBuffer 
		&& (nSize > 0))
	{
        if(m_hBitmap)
        {
			DeleteObject(m_hBitmap);
            m_hBitmap = NULL;
        }

		if(m_hImageBitmap)
        {
			DeleteObject(m_hImageBitmap);
            m_hImageBitmap = NULL;
        }

		if(BITMAP_DATA == nFlag)
		{
			nSize += 0xE;
		}

		HGLOBAL hGobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_NODISCARD
									,nSize+1);
		if(hGobal)
		{
			BYTE * pbuf = (BYTE *)GlobalLock(hGobal);
			if(pbuf)
			{
				IStream * pIStream;
				if(BITMAP_DATA == nFlag)
				{
					for(int i=0;i<0xE;i++)
						pbuf[i] = 0;

					pbuf[0] = 'B';
					pbuf[1] = 'M';
					pbuf[2] = nSize & 0XFF;
					pbuf[3] = (nSize>>8) & 0XFF;
					pbuf[4] = (nSize>>16) & 0XFF;
					pbuf[5] = (nSize>>24) & 0XFF;
					BITMAPINFOHEADER * pBitInfoHeader = (BITMAPINFOHEADER *)pBuffer;
					int nOffset = 0;
					if(pBitInfoHeader->biSizeImage)
					{
						nOffset = nSize - pBitInfoHeader->biSizeImage;
					}
					else
					{
						nOffset = 0xE + 0x28;
						if(pBitInfoHeader->biBitCount <= 8)
                        {
                            if(pBitInfoHeader->biClrUsed)
							    nOffset += (pBitInfoHeader->biClrUsed << 2);
                            else
                                nOffset += ((1 << pBitInfoHeader->biBitCount) << 2);
                        }
					}
					pbuf[10] = nOffset & 0XFF;
					pbuf[11] = (nOffset>>8) & 0XFF;
					pbuf[12] = (nOffset>>16) & 0XFF;
					pbuf[13] = (nOffset>>24) & 0XFF;
					memcpy(&pbuf[0xE],pBuffer,nSize-0xE);
				}
				else if((ICON_DATA == nFlag) 
					|| (CURSOR_DATA == nFlag))
				{
					for(int i=0;i<0xE;i++)
						pbuf[i] = 0;

					if(CURSOR_DATA == nFlag)
					{
						pBuffer = pBuffer + 4;
						nSize -= 4;
					}

					BITMAPINFOHEADER * pBitInfoHeader = (BITMAPINFOHEADER *)pBuffer;

					pBitInfoHeader->biHeight = pBitInfoHeader->biHeight>>1;

					int nBitSize = ((pBitInfoHeader->biWidth * pBitInfoHeader->biHeight) + 7)>>3;

					nBitSize = 0xE + pBitInfoHeader->biSizeImage;

					pbuf[0] = 'B';
					pbuf[1] = 'M';
					pbuf[2] = nBitSize & 0XFF;
					pbuf[3] = (nBitSize>>8) & 0XFF;
					pbuf[4] = (nBitSize>>16) & 0XFF;
					pbuf[5] = (nBitSize>>24) & 0XFF;

					int nOffset = 0;
					{
						nOffset = 0xE + 0x28;
						if(pBitInfoHeader->biBitCount <= 8)
							nOffset += ((1 << pBitInfoHeader->biBitCount) << 2);
					}
					pbuf[10] = nOffset & 0XFF;
					pbuf[11] = (nOffset>>8) & 0XFF;
					pbuf[12] = (nOffset>>16) & 0XFF;
					pbuf[13] = (nOffset>>24) & 0XFF;
					memcpy(&pbuf[0xE],pBuffer,nSize-0xE);

                    pBitInfoHeader->biHeight = pBitInfoHeader->biHeight<<1;
				}
				else
				{
					memcpy(pbuf,pBuffer,nSize);
				}

				CreateStreamOnHGlobal(hGobal,TRUE,&pIStream);

				if(pIStream)
				{
					CreateBitmap(pIStream,nSize);

					SetScrollBar();

					if(pIStream)
						pIStream->Release();

					InvalidateRect(m_hWnd,NULL,0);
					UpdateWindow(m_hWnd);
				}

				GlobalUnlock(hGobal);
			}
			GlobalFree(hGobal);
		}

        if((m_hBitmap) 
            && (m_hImageBitmap))
            return 0;

        return -2;
	}

    return -1;
}

void CNikBitmapEditor::ProcessKeyMessages(DWORD dwKeycode
									   ,int nFlag)
{
	m_bKeyDown[(BYTE)(dwKeycode)] = nFlag;

	if(TRUE == nFlag)
	{
		m_bKeyDown[(BYTE)(dwKeycode)] = TRUE;

		switch(dwKeycode)
		{
		case VK_INSERT:
			break;
		case VK_DOWN:
			break;

		case VK_UP:
			break;

		case VK_RIGHT:
			break;

		case VK_LEFT:
			break;

		case VK_DELETE:
			break;
		}
	}
}

void CNikBitmapEditor::OnMouseMove(WPARAM wParam
								, LPARAM lParam)
{
	if(MK_LBUTTON & wParam)
	{
		POINT ptDiff;

		ptDiff.x = GET_X_LPARAM(lParam);
		ptDiff.y = GET_Y_LPARAM(lParam);

		InvalidateRect(m_hWnd,NULL,FALSE);
	}
}

void CNikBitmapEditor::OnSize(WPARAM //wParam
							, LPARAM lParam)
{
	RECT rc;
	rc.left = 0;
	rc.right = LOWORD(lParam);
	rc.top = 0;
	rc.bottom = HIWORD(lParam);

	if(m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	SetDrawRect(rc);

	if(m_hImageBitmap)
	{
		if((rc.right+m_nOffsetX) > m_stBitmapHeader.biWidth)
		{
			m_nOffsetX = m_stBitmapHeader.biWidth - rc.right;
			if(m_nOffsetX < 0)
				m_nOffsetX = 0;
			InvalidateRect(m_hWnd,NULL,0);
		}

		if((rc.bottom+m_nOffsetY) > m_stBitmapHeader.biHeight)
		{
			m_nOffsetY = m_stBitmapHeader.biHeight - rc.bottom;
			if(m_nOffsetY < 0)
				m_nOffsetY = 0;
			InvalidateRect(m_hWnd,NULL,0);
		}
	}

	SetScrollBar();
}


void CNikBitmapEditor::OnLbuttonDown(WPARAM //wParam
								, LPARAM //lParam
                                )
{
    if(m_hWnd)
        SetWindowPos( m_hWnd, HWND_TOP, 0 , 0, 0, 0
            , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

void CNikBitmapEditor::OnLbuttonUP(WPARAM //wParam
								, LPARAM //lParam
                                )
{
}

void CNikBitmapEditor::OnRbuttonDown(WPARAM //wParam
								, LPARAM //lParam
								)
{
}

HWND CNikBitmapEditor::CreateBitmapWindow(HWND hParent)
{
    DWORD dwStyle = WS_TABSTOP |
	                WS_VISIBLE |
                    WS_SIZEBOX |
                    WS_CAPTION |
                    WS_SYSMENU |
                    WS_VSCROLL | 
                    WS_HSCROLL |
                    WS_CHILDWINDOW |
                    WS_CLIPSIBLINGS |
	                0;

	HWND hwnd = CreateWindowEx( WS_EX_TOOLWINDOW
                                , CLASS_NAME
								, WINDOWS_CAPTION
								, dwStyle
								, 0, 0, 0, 0
								, hParent
                                , NULL
								, GetWindowInstance(hParent)
								, (LPARAM)NULL );
    return hwnd;
}

void CNikBitmapEditor::PaintWindow(HWND hwnd
								 , HDC hdc
								 , RECT rPaint
)
{
	RECT rc;
	GetClientRect(hwnd,&rc);

	HDC memDC = CreateCompatibleDC ( hdc );

	int nWidth,nHeight;
	GetEditorWindowSize(nWidth,nHeight);

	if(NULL == GetEditorBitmap())
	{
		HBITMAP hBitmap = CreateCompatibleBitmap ( hdc
												, nWidth
												, nHeight );
		SetEditorBitmap(hBitmap);
	}

	HBITMAP OldBM = (HBITMAP)SelectObject ( memDC, GetEditorBitmap() );

	DrawSideBar(memDC);

	DrawBitmapEditor(memDC,rPaint);

	BitBlt(hdc,0,0,nWidth,nHeight,memDC,0,0,SRCCOPY);

	SelectObject ( hdc, OldBM);

	DeleteDC(memDC);
}

void CNikBitmapEditor::MenuProcess( HWND //hwnd
                                      , DWORD //dwValue
                                      )
{
}

int CNikBitmapEditor::RegisterEditorclass( HINSTANCE hInstance )
{
	WNDCLASS    wc;
	memset(&wc,0,sizeof(wc));

	wc.lpfnWndProc		= (WNDPROC)CNikBitmapEditor::EditorWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor(NULL, IDC_IBEAM);
    wc.hIcon			= (HICON)LoadIcon(hInstance,MAKEINTRESOURCE(IDC_NIKPEVIEWER));
    wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName	= CLASS_NAME;
	wc.lpszMenuName		= NULL;
    if (!RegisterClass(&wc)) 
		return FALSE;
	return TRUE;
}

LRESULT CALLBACK CNikBitmapEditor::EditorWndProc(HWND hwnd
                                                    , UINT uMsg
                                                    , WPARAM wParam
                                                    , LPARAM lParam)
{
    CNikBitmapEditor * pBitmapEditor = (CNikBitmapEditor *)GetWindowLong( hwnd, GWL_USERDATA );

    switch(uMsg)
    {
	case WM_MOUSEMOVE:
		if(pBitmapEditor)
			pBitmapEditor->OnMouseMove(wParam,lParam);
		break;

	case WM_LBUTTONDOWN:
		if(pBitmapEditor)
		{
			SetCapture(hwnd);
			pBitmapEditor->OnLbuttonDown(wParam,lParam);
		}
		break;

	case WM_LBUTTONUP:
		if(pBitmapEditor)
		{
			ReleaseCapture();
			pBitmapEditor->OnLbuttonUP(wParam,lParam);
			SetFocus(hwnd);
		}
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_MOUSEWHEEL:
		if(pBitmapEditor)
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if(zDelta > 0)
				pBitmapEditor->OnScrollBar(WM_VSCROLL,UM_WHEELUP);
			else
				pBitmapEditor->OnScrollBar(WM_VSCROLL,UM_WHEELDOWN);
		}
		break;

	case WM_VSCROLL:
		if(pBitmapEditor)
			pBitmapEditor->OnScrollBar(WM_VSCROLL,wParam);
		break;

	case WM_HSCROLL:
		if(pBitmapEditor)
			pBitmapEditor->OnScrollBar(WM_HSCROLL,wParam);
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		if(pBitmapEditor)
			pBitmapEditor->OnSize(wParam,lParam);
		break;

	case WM_KEYDOWN:
		if(pBitmapEditor)
			pBitmapEditor->ProcessKeyMessages(wParam, TRUE);
		break;

	case WM_KEYUP:
		if(pBitmapEditor)
			pBitmapEditor->ProcessKeyMessages(wParam, FALSE);
		break;

	case WM_COMMAND:
		MenuProcess(hwnd,LOWORD(wParam));
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			/////////////
            if(pBitmapEditor)
			    pBitmapEditor->PaintWindow(hwnd,hdc,ps.rcPaint);
			/////////////

			EndPaint(hwnd, &ps); 
		}
        break;

    case WM_CLOSE:
        ShowWindow( hwnd, SW_HIDE );
        break;

	case WM_DESTROY:
        return DefWindowProc( hwnd, uMsg, wParam, lParam );

	default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
    }

    return 0;
}

//	end: