
#include "stdafx.h"
#include "Resource.h"
#include "PEBinaryFile.h"

void CBinaryEditor::DrawRectArea( HDC hdc
								 , RECT rc
								 , COLORREF col)
{
	if (hdc)
	{
		HBRUSH hBrush = CreateSolidBrush(col);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);

		FillRect(hdc,&rc,hBrush);

		SelectObject(hdc,hOldBrush);

		DeleteObject(hBrush);
	}
}

CBinaryEditor::CBinaryEditor( HWND hWnd ):
     m_hFont( NULL )
   , m_hBitmap( NULL )
   , m_pbBinBuffer( NULL )
   , m_nMaxCharLen( 0 )
   , m_nLineHeight( 0 )
   , m_nBinarySize( 0 )
   , m_nLineNumbers( 0 )
   , m_nTotalLineNo( 0 )
   , m_nASCIIOffset( 0 )
   , m_nLeftBoundary( 0 )
   , m_nAveCharWidth( 0 )
   , m_nBufferOffset( 0 )
   , m_nSelectionEnd( -1 )
   , m_nSelectionStart( -1 )
   , m_nLineNumberWidth( 0 )
   , m_bBinHeaderEnable( TRUE )
   , m_bLineNumberEnable( TRUE )
   , m_nBinRowNumbers( BINARY_CHAR_ROW )
{
    memset(&m_rDrawArea,0,sizeof(RECT));

    memset(&m_lf,0,sizeof(LOGFONT));

    m_nTabsSizes[0] = EDITOR_TAB_SIZE;
    m_nTabsSizes[1] = EDITOR_TAB_SIZE;

	_tcscpy_s(m_lf.lfFaceName,sizeof(m_lf.lfFaceName)/sizeof(TCHAR),EDITOR_FONT);
	m_lf.lfClipPrecision		= 2;
	m_lf.lfHeight				= -15;
	m_lf.lfQuality				= 1;
	m_lf.lfWeight				= 400;
	m_lf.lfOutPrecision			= 3;
	m_lf.lfPitchAndFamily		= 34;

    HDC hdc = GetDC( hWnd );
    if(hdc)
    {
        CreateEditorFont( hdc );
        ReleaseDC(hWnd, hdc);
    }
}

CBinaryEditor::~CBinaryEditor()
{
    if(m_hBitmap)
	    DeleteObject(m_hBitmap);

    if(m_hFont)
		DeleteObject(m_hFont);
}

int CBinaryEditor::GetBinOffsetfromPt( POINT pt )
{
    int nOffsetY, nOffsetX;
    int nBinOffset;

    nOffsetY = pt.y - m_nLineHeight;
    nOffsetY = nOffsetY / m_nLineHeight;

    int nDiffOffset = pt.x - m_nASCIIOffset + m_nMaxCharLen;

    if(nDiffOffset < 0)
    {
        int nCharsLen = m_nMaxCharLen * 3;
        nOffsetX = pt.x - (m_nLeftBoundary + m_nMaxCharLen);
        if( nOffsetX < 0 )
            nOffsetX = 0;

        nDiffOffset = nOffsetX + ((m_nBinRowNumbers>>1) * nCharsLen);
        if(pt.x > nDiffOffset)
        {
            nDiffOffset += m_nMaxCharLen;
            nOffsetX -= nDiffOffset;
        }
        else
            nDiffOffset = 0;

        nOffsetX = (nOffsetX + m_nMaxCharLen) / nCharsLen;
    }
    else
    {
        nOffsetX = pt.x - m_nASCIIOffset;
        nOffsetX = nOffsetX / m_nMaxCharLen;
    }

    nBinOffset = (nOffsetY*m_nBinRowNumbers) + nOffsetX;

    nBinOffset += m_nBufferOffset;

    return nBinOffset;
}

POINT CBinaryEditor::GetBinPtfromOffset( int nOffset
                                        , int nASCIIOffset )
{
    POINT pt;

    int nHeight = m_rDrawArea.bottom - m_rDrawArea.top;
    int nPageLines = (nHeight / m_nLineHeight) + 1;
    int nEndPage = m_nBufferOffset + (m_nBinRowNumbers * nPageLines);
    nEndPage -= 1;

    /// page bound check
    if(nOffset < m_nBufferOffset)
        nOffset = m_nBufferOffset;
    if(nOffset > nEndPage)
        nOffset = nEndPage;

    /// Binary buffer bound check
    if(nOffset < 0)
        nOffset = 0;
    if(nOffset > m_nBinarySize)
        nOffset = m_nBinarySize;

    int nPageBufOffset = nOffset - m_nBufferOffset;

    pt.y = (nPageBufOffset / m_nBinRowNumbers) * m_nLineHeight;
    pt.y += m_nLineHeight;

    int nXLineOffset = nPageBufOffset % m_nBinRowNumbers;
    int nBreak = (m_nBinRowNumbers>>1) - 1;

    if(0 == nASCIIOffset)
    {
        int nXOffset = m_nLeftBoundary + m_nMaxCharLen;
        pt.x = ((m_nMaxCharLen * 3) * nXLineOffset) + nXOffset;
        if(nXLineOffset > nBreak)
            pt.x += m_nMaxCharLen;
    }
    else
    {
        pt.x = (m_nMaxCharLen * nXLineOffset) + m_nASCIIOffset;
    }

    return pt;
}

void CBinaryEditor::SetScrollBar( HWND hWnd )
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    // Set the vertical scrolling range and page size
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;

    si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin   = 0;
    if(m_nTotalLineNo > 0)
        si.nMax   = m_nTotalLineNo;
    else
        si.nMax   = 0;

    if(m_nLineHeight)
	    si.nPage  = rc.bottom/m_nLineHeight;
    else
        si.nPage  = 0;

    si.nPos   = m_nBufferOffset/m_nBinRowNumbers;
    if((int)(si.nPos+si.nPage + 1) > si.nMax)
    {
        si.nPos = si.nMax - si.nPage + 1;
        m_nBufferOffset = si.nPos * m_nBinRowNumbers;

        InvalidateRect( hWnd, NULL, TRUE );
        UpdateWindow( hWnd );
    }

    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

void CBinaryEditor::OnScrollBar(HWND hWnd
                                , int nFlag
                                , WPARAM wParam)
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;	

	if(WM_VSCROLL == nFlag)
	{
		GetScrollInfo (hWnd, SB_VERT, &si);

		int nCurPos = si.nPos;

		switch (LOWORD (wParam))
		{
			// user clicked the HOME keyboard key
		case UM_WHEELUP:
			si.nPos -= 3;
			break;

		case UM_WHEELDOWN:
			si.nPos += 3;
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
			si.nPos -= 1;
			break;

			// user clicked the bottom arrow
		case SB_LINEDOWN:
			si.nPos += 1;
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
		SetScrollInfo (hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo (hWnd, SB_VERT, &si);
		// If the position has changed, scroll window and update it
		if (si.nPos != nCurPos)
		{
            RECT rc;
            GetClientRect( hWnd, &rc );
            rc.top += m_nLineHeight + 2;
            m_nBufferOffset = si.nPos * m_nBinRowNumbers;
			ScrollWindowEx( hWnd, 0, m_nLineHeight * (nCurPos - si.nPos)
							, &rc, &rc, NULL, &rc, SW_INVALIDATE );
		}
	}
}

void CBinaryEditor::CalculateBinRowNumber()
{
    if(!IsRectEmpty( &m_rDrawArea ))
    {
        int nWidth = (m_rDrawArea.right - m_rDrawArea.left);
        nWidth -= m_nLeftBoundary;

        if(nWidth < m_nMaxCharLen)
            m_nBinRowNumbers = 2;
        else
        {
            int nCharLength = m_nMaxCharLen + (m_nMaxCharLen>>1);
            int nRows = (nWidth / nCharLength) - 1;
            nRows -= EDITOR_TAB_SIZE;
            m_nBinRowNumbers = nRows/3;
            if(m_nBinRowNumbers < 2)
                m_nBinRowNumbers = 2;
        }

        m_nTotalLineNo = m_nBinarySize / m_nBinRowNumbers;
    }
}

void CBinaryEditor::CreateEditorFont( HDC hdc )
{
	if(m_hFont)
		DeleteObject(m_hFont);

	m_hFont = CreateFontIndirect(&m_lf);

	CalculateLineHeight( hdc );
}

void CBinaryEditor::CalculateCharLen( HDC hdc )
{
	if(hdc)
	{
		HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);

	    DWORD dwSize;
	    TCHAR chBuf[2];
	    chBuf[1] = 0;

	    m_nMaxCharLen = 0;

	    for(int i=0; i<256; i++)
	    {
		    chBuf[0] = (TCHAR)i;
		    dwSize = GetTabbedTextExtent( hdc
									    , chBuf
									    , 1
									    , 1
                                        , m_nTabsSizes);

		    if(m_nMaxCharLen < LOWORD(dwSize))
		    {
			    if((i >= (char)'A') 
				    && (i <= (char)'Z'))
				    m_nMaxCharLen = LOWORD(dwSize);
		    }
	    }

		SelectObject(hdc,hOldFont);
	}
}

void CBinaryEditor::CalculateLineHeight( HDC hdc )
{
	if(hdc)
	{
		CalculateCharLen( hdc );

		HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);

		SIZE sSize;
		int nChar = sizeof(TES_STRING)/sizeof(TCHAR) - 1;
		GetTextExtentPoint32(hdc
							,TES_STRING
							,nChar
							,&sSize);

		m_nLineHeight = sSize.cy;

		m_nAveCharWidth = (sSize.cx+nChar-1)/nChar;

		m_nTabsSizes[0] = m_nAveCharWidth * EDITOR_TAB_SIZE;
		m_nTabsSizes[1] = m_nAveCharWidth * EDITOR_TAB_SIZE;

        int nBinLineWidth   = m_nMaxCharLen * (3*(m_nBinRowNumbers+1));
		m_nLineNumberWidth  = m_nMaxCharLen * 11;

        m_nASCIIOffset      = m_nLineNumberWidth + LEFT_WINDOW_BAR + nBinLineWidth + m_nMaxCharLen;

		SelectObject(hdc,hOldFont);

		m_nLeftBoundary = LEFT_WINDOW_BAR;
		if(m_bLineNumberEnable)
			m_nLeftBoundary += m_nLineNumberWidth;
	}
}

void CBinaryEditor::SetSelection( int nStart
                                 , int nEnd)
{
    m_nSelectionEnd = nEnd;
    m_nSelectionStart = nStart;

    m_stSelectRect[0].ResetSelection();
    m_stSelectRect[1].ResetSelection();
    m_stSelectRect[2].ResetSelection();

    memset( &m_rACSIIRect[0], 0, sizeof(RECT));
    memset( &m_rACSIIRect[1], 0, sizeof(RECT));
    memset( &m_rACSIIRect[2], 0, sizeof(RECT));

    if(m_nSelectionEnd != m_nSelectionStart)
    {
        if(m_nSelectionEnd < m_nSelectionStart)
        {
            int nTemp = m_nSelectionEnd;
            m_nSelectionEnd = m_nSelectionStart;
            m_nSelectionStart = nTemp;
        }

        POINT ptStart, ptEnd;
        int nStart = m_nBinRowNumbers - (m_nSelectionStart % m_nBinRowNumbers);
        nStart = m_nSelectionStart + nStart - 1;

        int nEnd  = m_nSelectionEnd % m_nBinRowNumbers;
        nEnd      = m_nSelectionEnd - nEnd;

        if(nEnd > nStart)
        {
            if( nStart >= m_nBufferOffset )
            {
                ptEnd     = GetBinPtfromOffset( nStart, 0 );
                ptStart   = GetBinPtfromOffset( m_nSelectionStart, 0 );

                m_stSelectRect[0].m_nEnd = nStart;
                m_stSelectRect[0].m_nStart = m_nSelectionStart;
                m_stSelectRect[0].m_rSelect.left = ptStart.x;
                m_stSelectRect[0].m_rSelect.right = ptEnd.x + (m_nMaxCharLen<<1);
                m_stSelectRect[0].m_rSelect.top = ptStart.y;
                m_stSelectRect[0].m_rSelect.bottom = ptEnd.y + m_nLineHeight;

                ptEnd     = GetBinPtfromOffset( nStart, 1 );
                ptStart   = GetBinPtfromOffset( m_nSelectionStart, 1 );
                m_rACSIIRect[0].left = ptStart.x;
                m_rACSIIRect[0].top = ptStart.y;
                m_rACSIIRect[0].right = ptEnd.x + m_nMaxCharLen;
                m_rACSIIRect[0].bottom = ptEnd.y + m_nLineHeight;
            }

            if( m_nSelectionEnd >= m_nBufferOffset )
            {
                ptEnd     = GetBinPtfromOffset( m_nSelectionEnd, 0 );
                ptStart   = GetBinPtfromOffset( nEnd, 0 );
                m_stSelectRect[1].m_nEnd = m_nSelectionEnd;
                m_stSelectRect[1].m_nStart = nEnd;
                m_stSelectRect[1].m_rSelect.left = ptStart.x;
                m_stSelectRect[1].m_rSelect.right = ptEnd.x - m_nMaxCharLen;
                m_stSelectRect[1].m_rSelect.top = ptStart.y;
                m_stSelectRect[1].m_rSelect.bottom = ptEnd.y + m_nLineHeight;

                ptEnd     = GetBinPtfromOffset( m_nSelectionEnd, 1 );
                ptStart   = GetBinPtfromOffset( nEnd, 1 );
                m_rACSIIRect[1].left = ptStart.x;
                m_rACSIIRect[1].top = ptStart.y;
                m_rACSIIRect[1].right = ptEnd.x;
                m_rACSIIRect[1].bottom = ptEnd.y + m_nLineHeight;
            }

            nEnd--;
            nStart++;
            if(nEnd > nStart)
            {
                if( nEnd >= m_nBufferOffset )
                {
                    ptEnd     = GetBinPtfromOffset( nEnd, 0 );
                    ptStart   = GetBinPtfromOffset( nStart, 0 );
                    m_stSelectRect[2].m_nEnd = nEnd;
                    m_stSelectRect[2].m_nStart = nStart;
                    m_stSelectRect[2].m_rSelect.left = ptStart.x;
                    m_stSelectRect[2].m_rSelect.right = ptEnd.x + (m_nMaxCharLen<<1);
                    m_stSelectRect[2].m_rSelect.top = ptStart.y;
                    m_stSelectRect[2].m_rSelect.bottom = ptEnd.y + m_nLineHeight;

                    ptEnd     = GetBinPtfromOffset( nEnd, 1 );
                    ptStart   = GetBinPtfromOffset( nStart, 1 );
                    m_rACSIIRect[2].left = ptStart.x;
                    m_rACSIIRect[2].top = ptStart.y;
                    m_rACSIIRect[2].right = ptEnd.x + m_nMaxCharLen;
                    m_rACSIIRect[2].bottom = ptEnd.y + m_nLineHeight;
                }
            }
        }
        else
        {
            ptEnd     = GetBinPtfromOffset( m_nSelectionEnd, 0 );
            ptStart   = GetBinPtfromOffset( m_nSelectionStart, 0 );
            m_stSelectRect[0].m_nEnd = m_nSelectionEnd;
            m_stSelectRect[0].m_nStart = m_nSelectionStart;
            m_stSelectRect[0].m_rSelect.left = ptStart.x;
            m_stSelectRect[0].m_rSelect.right = ptEnd.x - m_nMaxCharLen;
            m_stSelectRect[0].m_rSelect.top = ptStart.y;
            m_stSelectRect[0].m_rSelect.bottom = ptEnd.y + m_nLineHeight;

            ptEnd     = GetBinPtfromOffset( m_nSelectionEnd, 1 );
            ptStart   = GetBinPtfromOffset( m_nSelectionStart, 1 );
            m_rACSIIRect[0].left = ptStart.x;
            m_rACSIIRect[0].top = ptStart.y;
            m_rACSIIRect[0].right = ptEnd.x;
            m_rACSIIRect[0].bottom = ptEnd.y + m_nLineHeight;
        }
    }
    else
    {
        m_nSelectionEnd     = -1;
        m_nSelectionStart   = -1;
    }
}

void CBinaryEditor::DrawTextSelection( HDC hdc )
{
    if( hdc && IsBinSelection())
    {
        if(!IsRectEmpty(&m_stSelectRect[0].m_rSelect))
        {
            DrawRectArea(hdc, m_stSelectRect[0].m_rSelect, BIN_SELECTION_COLOR);
            DrawRectArea(hdc, m_rACSIIRect[0], TEXT_SELECTION_COLOR);
        }

        if(!IsRectEmpty(&m_stSelectRect[1].m_rSelect))
        {
            DrawRectArea(hdc, m_stSelectRect[1].m_rSelect, BIN_SELECTION_COLOR);
            DrawRectArea(hdc, m_rACSIIRect[1], TEXT_SELECTION_COLOR);
        }

        if(!IsRectEmpty(&m_stSelectRect[2].m_rSelect))
        {
            DrawRectArea(hdc, m_stSelectRect[2].m_rSelect, BIN_SELECTION_COLOR);
            DrawRectArea(hdc, m_rACSIIRect[2], TEXT_SELECTION_COLOR);
        }
    }
}

void CBinaryEditor::DrawBinBackground( HDC hdc )
{
	if (hdc)
	{
		RECT rc = m_rDrawArea;
        rc.left = LEFT_WINDOW_BAR;
		DrawRectArea(hdc, rc, WINDOW_BACKGROUND_COLOR);

        rc.left = 0;
        rc.right = LEFT_WINDOW_BAR;
		DrawRectArea(hdc, rc, SIDE_BAR_COLOR);
        SetSelection( m_nSelectionStart, m_nSelectionEnd );
        DrawTextSelection( hdc );
	}
}

void CBinaryEditor::DrawBinaryLineNumber(HDC hdc)
{
	if(hdc 
        && (TRUE == IsLinenumberEnable()))
	{
		RECT rc = m_rDrawArea;

		TCHAR chLineNo[64];
		COLORREF col = SetTextColor(hdc, LINE_NUMBER_COLOR);

		HPEN hOldPen,hPen = CreatePen(PS_DOT, 1, LINE_NUMBER_COLOR);
		hOldPen = (HPEN)SelectObject(hdc,hPen);

		HFONT hFontOld = (HFONT)SelectObject(hdc,m_hFont);

		rc.left = LEFT_WINDOW_BAR;
		rc.right = rc.left + m_nLineNumberWidth;
		rc.top = m_rDrawArea.top;
		rc.bottom = rc.top + m_nLineHeight;

        if(IsBinHeaderEnable())
            OffsetRect(&rc, 0, m_nLineHeight);

		for(int i=0; i<m_nLineNumbers; i++)
		{
            _stprintf_s(chLineNo, sizeof(chLineNo)/sizeof(TCHAR), _T("0x%08X")
                        , (i*m_nBinRowNumbers)+m_nBufferOffset);

            DrawText(hdc, chLineNo, _tcslen(chLineNo), &rc, DT_CENTER|DT_TOP);
			OffsetRect(&rc, 0, m_nLineHeight);
		}

        rc.bottom -= m_nLineHeight;

        if(rc.bottom > 0)
        {
            int nYOffset = m_rDrawArea.top + m_nLineHeight;
            int nOffset = m_nLineNumberWidth + LEFT_WINDOW_BAR;
		    MoveToEx(hdc, nOffset, nYOffset, NULL);
		    LineTo(hdc, nOffset, rc.bottom+1);

            nOffset = m_nASCIIOffset - (m_nMaxCharLen<<1);
            MoveToEx(hdc, nOffset, nYOffset, NULL);
		    LineTo(hdc, nOffset, rc.bottom+1);
        }

		SetTextColor(hdc,col);
		SelectObject(hdc,hFontOld);
		SelectObject(hdc,hOldPen);

		DeleteObject(hPen);
	}
}

void CBinaryEditor::DrawLineBinary( HDC hdc
                                   , BYTE * pBuffer
                                   , int nOffsetX
                                   , int nOffsetY
                                   , int nLineOffset)
{
	if(hdc)
	{
		BYTE * pBuff = pBuffer;
		int nSize = m_nBinarySize - nLineOffset;

		TCHAR chBuf[0x10];
        chBuf[0] = 0;

        int i = 0;
		int nBreak = (m_nBinRowNumbers>>1) - 1;

        _tstring strBin, strASCII;
		while(i < nSize)
		{
			_stprintf_s(chBuf, sizeof(chBuf)/sizeof(TCHAR), _T("%02X "), pBuff[i]);
            strBin += chBuf;

			if((pBuff[i] >= 0x20) 
				&& (pBuff[i] < 0x80))
				_stprintf_s(chBuf, sizeof(chBuf)/sizeof(TCHAR), _T("%c"),pBuff[i]);
			else
            {
				chBuf[0] = _T('.');
                chBuf[1] = 0;
            }

            strASCII += chBuf;

			if(i == nBreak)
                strBin += _T(" ");

			i++;
			if(i >= m_nBinRowNumbers)
				break;
		}

        TextOut(hdc, nOffsetX+m_nMaxCharLen, nOffsetY, strBin.c_str(), strBin.length());
        TextOut(hdc, m_nASCIIOffset, nOffsetY, strASCII.c_str(), strASCII.length());
	}
}

void CBinaryEditor::DrawBinaryHeader( HDC hdc )
{
    if(hdc && (IsBinHeaderEnable()))
	{
        TCHAR chBuf[0x10];
        chBuf[0] = 0;

        COLORREF col = SetTextColor(hdc, LINE_NUMBER_COLOR);
        HPEN hOldPen,hPen = CreatePen(PS_DOT, 1, LINE_NUMBER_COLOR);
		hOldPen = (HPEN)SelectObject(hdc,hPen);
        HFONT hFontOld = (HFONT)SelectObject(hdc, m_hFont);

        RECT rc = m_rDrawArea;
        MoveToEx(hdc, rc.left+LEFT_WINDOW_BAR, rc.top+m_nLineHeight, NULL);
        LineTo(hdc, rc.right, rc.top+m_nLineHeight);

        rc.left = LEFT_WINDOW_BAR;
		rc.right = rc.left + m_nLineNumberWidth;
		rc.top = m_rDrawArea.top;
		rc.bottom = rc.top + m_nLineHeight;

        DrawText(hdc, HEADER_ADD, _tcslen(HEADER_ADD), &rc, DT_TOP|DT_CENTER|DT_NOCLIP);

        int nOffsetY = m_rDrawArea.top;
        int nOffset = m_rDrawArea.left + m_nLeftBoundary + m_nMaxCharLen;
        int nBreak = (m_nBinRowNumbers>>1) - 1;

        _tstring strTemp = _T("");

        for(int i=0; i<m_nBinRowNumbers; i++)
        {
            //_stprintf_s(chBuf, sizeof(chBuf)/sizeof(TCHAR), _T("%2d"), i);
            _stprintf_s(chBuf, sizeof(chBuf)/sizeof(TCHAR), _T("%02X "), i);

            strTemp += chBuf;

            if(i == nBreak)
				strTemp += _T(" ");
        }

        TextOut(hdc, nOffset, nOffsetY, strTemp.c_str(), strTemp.length());

        SetTextColor(hdc, col);
        SelectObject(hdc,hFontOld);
		SelectObject(hdc,hOldPen);
		DeleteObject(hPen);
    }
}

void CBinaryEditor::DrawBinaryData( HDC hdc
                                   , BYTE * pbBinBuffer
                                   , RECT rc)
{
    if((NULL == pbBinBuffer) 
		|| IsRectEmpty(&rc))
		return;

	if(hdc)
	{
        int nSize = 0;
        int nHeight = 0;

        if(IsBinHeaderEnable())
            nHeight = m_nLineHeight;

		m_nLineNumbers = 0;

		for(nSize=m_nBufferOffset; nSize<m_nBinarySize; nSize+=m_nBinRowNumbers)
		{
			DrawLineBinary(hdc, &pbBinBuffer[nSize], rc.left, nHeight, nSize);

			m_nLineNumbers++;

			nHeight += m_nLineHeight;
			if(nHeight > rc.bottom)
				break;
		}
	}
}

void CBinaryEditor::DrawBinForground( HDC hdc )
{
    if (hdc && m_pbBinBuffer)
	{
        RECT rc = m_rDrawArea;

		rc.left += m_nLeftBoundary;

		HRGN hrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom); 
		SelectClipRgn(hdc, hrgn);

		int nMode;

		nMode = SetBkMode(hdc,TRANSPARENT);

		HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);

		DrawBinaryData(hdc, m_pbBinBuffer, rc);

		SelectObject(hdc, hOldFont);

		SetBkMode(hdc, nMode);
		SelectClipRgn(hdc, NULL);
		DeleteObject(hrgn);

        DrawBinaryLineNumber(hdc);
        DrawBinaryHeader(hdc);
    }
}

void CBinaryEditor::DrawBinEditor( HDC hdc )
{
    if (hdc)
	{
        DrawBinBackground( hdc );
        DrawBinForground( hdc );
    }
}

void CBinaryEditor::PaintWindow( HDC hdc )
{
	if(hdc)
	{
		HDC memDC = CreateCompatibleDC ( hdc );

		int nWidth, nHeight;
		GetEditorWindowSize(nWidth, nHeight);

		if(NULL == GetEditorBitmap())
		{
			HBITMAP hBitmap = CreateCompatibleBitmap ( hdc
													, nWidth
													, nHeight );
			SetEditorBitmap(hBitmap);
		}

		HBITMAP OldBM = (HBITMAP)SelectObject ( memDC, GetEditorBitmap() );

		DrawBinEditor(memDC);

		BitBlt(hdc,0,0,nWidth,nHeight,memDC,0,0,SRCCOPY);

		SelectObject ( hdc, OldBM);

		DeleteDC(memDC);
	}
}

///------------------------------------

TCHAR       szToolClassName[]   = _T("TOOL_CHILD_WINDOW");

CNikPEBinaryViewer::CNikPEBinaryViewer()
{
    m_hWnd          =   NULL;
    m_hInstance     =   NULL;
    m_pBinaryEditor =   NULL;
}

CNikPEBinaryViewer::~CNikPEBinaryViewer()
{
    if(m_pBinaryEditor)
        delete m_pBinaryEditor;
}

HWND CNikPEBinaryViewer::CreateBinaryViewer(HINSTANCE hInstance
                                            , HWND hParent
                                            , RECT rcWidRect
                                            , _tstring strCaption)
{
    if(hParent && hInstance)
    {
        WNDCLASS    wc;
        BOOL        bResult;

        memset(&wc, 0, sizeof(WNDCLASS));
        wc.lpfnWndProc   = (WNDPROC)ToolChildWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 8;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_NIKPEVIEWER) );
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = szToolClassName;
		bResult = RegisterClass( &wc );

        DWORD dwStyle = WS_TABSTOP |
		                WS_VISIBLE |
                        WS_SIZEBOX |
                        WS_CAPTION |
                        WS_SYSMENU |
                        LVS_REPORT |
                        WS_CHILDWINDOW |
                        WS_CLIPSIBLINGS |
                        LVS_NOSORTHEADER |
		                0;

        m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW
                                , szToolClassName
                                , strCaption.c_str()
                                , dwStyle
                                , rcWidRect.left, rcWidRect.top
                                , rcWidRect.right - rcWidRect.left
                                , rcWidRect.bottom - rcWidRect.top
                                , hParent, NULL, hInstance, NULL );

        if(m_hWnd)
        {
            m_hInstance = hInstance;
            m_pBinaryEditor = new CBinaryEditor( m_hWnd );
            SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)m_pBinaryEditor);
        }
    }

    return m_hWnd;
}

int CNikPEBinaryViewer::SetBinaryData(BYTE * pBuffer
                                      , int nSize)
{
    if(pBuffer 
        && m_pBinaryEditor)
    {
        m_pBinaryEditor->SetBinBuffer( pBuffer, nSize, 0 );
        m_pBinaryEditor->SetScrollBar( m_hWnd );
    }

    return 0;
}

LRESULT CALLBACK CNikPEBinaryViewer::ToolChildWndProc( HWND hWnd
                                                     , UINT uMsg
                                                     , WPARAM wParam
                                                     , LPARAM lParam )
{
    //static int nStartValue, nCapture = 0;
    CBinaryEditor * pBinaryEditor = (CBinaryEditor *)GetWindowLong(hWnd, GWL_USERDATA);

    switch( uMsg )
    {

    case WM_LBUTTONDOWN:
        if(pBinaryEditor)
        {
            /*POINT pt;
            pt.x = lParam & 0xFFFF;
            pt.y = (lParam >> 16) & 0xFFFF;
            nStartValue = pBinaryEditor->GetBinOffsetfromPt( pt );
            SetCapture( hWnd );
            nCapture = 1;*/
            SetWindowPos( hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        }
        break;

    case WM_LBUTTONUP:
        if(pBinaryEditor 
            /*&& (1 == nCapture)*/)
        {
            /*POINT pt;
            pt.x = lParam & 0xFFFF;
            pt.y = (lParam >> 16) & 0xFFFF;
            int nEndValue = pBinaryEditor->GetBinOffsetfromPt( pt );
            pBinaryEditor->SetSelection( nStartValue, nEndValue );

            ReleaseCapture();
            nCapture = 0;

            InvalidateRect( hWnd, NULL, TRUE );
            UpdateWindow( hWnd );*/
            SetFocus(hWnd);
        }
        break;

    case WM_MOUSEMOVE:
        if(pBinaryEditor 
            && (MK_LBUTTON & wParam)
            /*&& (1 == nCapture)*/)
        {
            /*POINT pt;
            pt.x = lParam & 0xFFFF;
            pt.y = (lParam >> 16) & 0xFFFF;
            int nEndValue = pBinaryEditor->GetBinOffsetfromPt( pt );
            pBinaryEditor->SetSelection( nStartValue, nEndValue );

            InvalidateRect( hWnd, NULL, TRUE );
            UpdateWindow( hWnd );*/
        }
        break;

    case WM_MOUSEWHEEL:
        if(pBinaryEditor)
        {
            int fwKeys = GET_WHEEL_DELTA_WPARAM(wParam);
            if(fwKeys >= 0)
                fwKeys = UM_WHEELUP;
            else
                fwKeys = UM_WHEELDOWN;
            pBinaryEditor->OnScrollBar(hWnd, WM_VSCROLL, fwKeys);
        }
        break;
    case WM_KEYDOWN:
        if(pBinaryEditor)
        {
            WPARAM nCode = 0;
            switch(wParam)
            {
            case VK_UP:
                nCode = SB_LINEUP;
                break;
            case VK_DOWN:
                nCode = SB_LINEDOWN;
                break;
            case VK_NEXT:
               nCode = SB_PAGEDOWN;
                break;
            case VK_PRIOR:
                nCode = SB_PAGEUP;
                break;
            }
            pBinaryEditor->OnScrollBar(hWnd, WM_VSCROLL, nCode);
        }
        break;
    case WM_VSCROLL:
        if(pBinaryEditor)
            pBinaryEditor->OnScrollBar(hWnd, WM_VSCROLL, wParam);
		break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
		    HDC hdc = BeginPaint(hWnd, &ps);
		    // TODO: Add any drawing code here...
            if(pBinaryEditor)
            {
                RECT rc = pBinaryEditor->GetDrawRect();
                if(IsRectEmpty( &rc))
                {
                    GetClientRect( hWnd, &rc);
                    pBinaryEditor->SetDrawRect( rc );
                    pBinaryEditor->SetEditorBitmap( NULL );
                    //pBinaryEditor->CalculateBinRowNumber();
                }

                pBinaryEditor->PaintWindow( hdc );
            }
		    EndPaint(hWnd, &ps);
        }
		break;
    case WM_SIZE:

        if(pBinaryEditor)
        {
            RECT rcBin;
            rcBin.left = 0;
	        rcBin.right = LOWORD(lParam);
	        rcBin.top = 0;
	        rcBin.bottom = HIWORD(lParam);

            pBinaryEditor->SetDrawRect( rcBin );
            pBinaryEditor->SetEditorBitmap( NULL );
            //pBinaryEditor->CalculateBinRowNumber();

            pBinaryEditor->SetScrollBar( hWnd );
        }

        break;
    case WM_ERASEBKGND:
        return TRUE;
	case WM_CREATE:
		break;

    case WM_CLOSE:
        ShowWindow( hWnd, SW_HIDE );
        break;

	case WM_DESTROY:
        return DefWindowProc( hWnd, uMsg, wParam, lParam);

	default:
        return DefWindowProc( hWnd, uMsg, wParam, lParam);
    }
    return 0;
}
