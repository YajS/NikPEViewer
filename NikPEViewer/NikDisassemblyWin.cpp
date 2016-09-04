
#include "stdafx.h"
#include "Resource.h"
#include "NikDisassemblyWin.h"


TCHAR szDisAssemblyTitle1[]         = _T("DisAssembly Viewer");
TCHAR szDisAssemblyWindowClass1[]   = _T("DISASSEMBLY_WIN_CLASS");

CNikDisassemblyWin::CNikDisassemblyWin()
{
    m_hBitmap               =   NULL;
    m_hFont                 =   NULL;
    m_nLineHeight           =   0;
    m_nAveCharWidth         =   0;
    m_nMaxCharWidth         =   0;
    m_nOffsetLineNo         =   0;
    m_pchDisCodeBuffer      =   NULL;

    m_nListRectNo           =   -1;
    m_nListRectStart        =   -1;
    memset( &m_rListViewRect, 0, sizeof(m_rListViewRect));

    m_hWnd                  =   NULL;
    m_hThread               =   NULL;
    m_bIsThreadRunning      =   FALSE;

    m_pNikDisassembly       =   NULL;

    m_dwActiveLine          =   (DWORD)-1;
    m_dwFocusLine           =   0;

    m_bIsDisplayBinary      =   TRUE;

    memset(&m_lf, 0, sizeof(LOGFONT));
}

CNikDisassemblyWin::~CNikDisassemblyWin()
{
    StopDisAssemblyThread();

    if(m_hBitmap)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = NULL;
    }

    if(m_pchDisCodeBuffer)
    {
        delete [] m_pchDisCodeBuffer;
        m_pchDisCodeBuffer = NULL;
        m_vDisNewLineOffset.clear();
    }

    if( m_hWnd )
        DestroyWindow( m_hWnd );

    CloseHandle(m_hThread);
}

int CNikDisassemblyWin::StopDisAssemblyThread()
{
    DWORD dwExitCode;
    if(FALSE == m_bIsThreadRunning)
    {
        dwExitCode = 0;
        GetExitCodeThread( m_hThread, &dwExitCode);
        if(STILL_ACTIVE != dwExitCode)
        {
            return 0;
        }
    }

    m_bIsThreadRunning  = FALSE;
    Sleep( 100 );

    for(int i=0;i<12;i++)
    {
        dwExitCode = 0;
        GetExitCodeThread( m_hThread, &dwExitCode);
        if(STILL_ACTIVE == dwExitCode)
        {
            if(i >= 10)
            {
                dwExitCode = 0;
                if(TerminateThread( m_hThread, dwExitCode ))
                    break;
            }
        }
        else
            break;

        Sleep( 50 );
    }

    return 0;
}

void CNikDisassemblyWin::SetLogFont(LOGFONT  &lf)
{
    memset(&lf, 0, sizeof(LOGFONT));

	_tcscpy_s(lf.lfFaceName
        , sizeof(lf.lfFaceName)/sizeof(TCHAR)
        , EDITOR_FONT);
	lf.lfClipPrecision		= 2;
	lf.lfHeight				= -12;
	lf.lfQuality			= 1;
	lf.lfWeight				= 400;
	lf.lfOutPrecision		= 3;
	lf.lfPitchAndFamily		= 34;
}

void CNikDisassemblyWin::CalculateLineHeight(HDC hdc)
{
	if(hdc)
	{
        SetLogFont(m_lf);

		HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);
		SIZE sSize;
		int nChar = sizeof(TES_STRING)/sizeof(TCHAR) - 1;
		GetTextExtentPoint32(hdc
							,TES_STRING
							,nChar
							,&sSize);

		m_nLineHeight = sSize.cy;

		m_nAveCharWidth = (sSize.cx+nChar-1)/nChar;

        TCHAR chTemp[3], * pTemp = TES_STRING;
        chTemp[1] = chTemp[2] = 0;

        m_nMaxCharWidth = 0;

        for(int i=0; i<nChar; i++)
        {
            chTemp[0] = pTemp[i];
            GetTextExtentPoint32(hdc
							,chTemp
							,1
							,&sSize);

            if(m_nMaxCharWidth < sSize.cx)
                m_nMaxCharWidth = sSize.cx;
        }

		SelectObject(hdc,hOldFont);

        RECT rc;
        GetClientRect( m_hWnd, &rc);
        rc.bottom = m_nLineHeight;
        m_rListViewRect[0] = m_rListViewRect[1] = m_rListViewRect[2] = m_rListViewRect[3] = rc;

        m_rListViewRect[0].left = 20;
        m_rListViewRect[0].right = m_rListViewRect[0].left + (12 * m_nMaxCharWidth);

        m_rListViewRect[1].left = m_rListViewRect[0].right;
        m_rListViewRect[1].right = m_rListViewRect[1].left + ((7*5) * m_nMaxCharWidth);

        m_rListViewRect[2].left = m_rListViewRect[1].right;
        m_rListViewRect[2].right = m_rListViewRect[2].left + 10 * m_nMaxCharWidth;

        m_rListViewRect[3].left = m_rListViewRect[2].right;
        m_rListViewRect[3].right = rc.right;
	}
}

void CNikDisassemblyWin::CreateEditorFont( HDC hdc )
{
	if(m_hFont)
		DeleteObject(m_hFont);

    SetLogFont(m_lf);

	m_hFont = CreateFontIndirect(&m_lf);

	CalculateLineHeight( hdc );
}

void CNikDisassemblyWin::SetScrollBar( HWND hWnd )
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    m_rDrawRect = rc;
    m_rDrawRect.top += m_nLineHeight;
    int nHeight = m_rDrawRect.bottom - m_rDrawRect.top;

    // Set the vertical scrolling range and page size
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;

    si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | /*SIF_PAGE | */SIF_POS;
    si.nMin   = 0;
    if(m_vDisNewLineOffset.size() > 0)
        //si.nMax   = m_vDisNewLineOffset.size() - 2;
        si.nMax   = m_vDisNewLineOffset.size() - (nHeight/m_nLineHeight);
    else
        si.nMax   = 0;

    //if(m_nLineHeight)
	//    si.nPage  = nHeight/m_nLineHeight;
    //else
    //    si.nPage  = 0;

    si.nPos   = m_nOffsetLineNo;

    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

DWORD WINAPI CNikDisassemblyWin::DisAssemblyThreadProc( LPVOID lpParam )
{
    CNikDisassemblyWin * pNikDisassemblyWin = (CNikDisassemblyWin *)lpParam;
    if(pNikDisassemblyWin)
    {
        pNikDisassemblyWin->ShowPEDisassemblyCode();
    }

    return 0;
}

DWORD CNikDisassemblyWin::GetLineAddress( POINT pt )
{
    DWORD dwAddress = (DWORD)-1;

    int nLineNo = pt.y / m_nLineHeight;
    nLineNo += m_nOffsetLineNo;
    nLineNo -= m_rDrawRect.top / m_nLineHeight;

    if(nLineNo < (int)m_vDisOpcodeOffset.size())
        dwAddress = m_vDisOpcodeOffset.at( nLineNo );

    return dwAddress;
}

int CNikDisassemblyWin::DumpAssemblyCode( _tstring strFileName )
{
    if( !strFileName.empty() 
        && (FALSE == m_bIsThreadRunning)
        && (NULL != m_pchDisCodeBuffer)
        && (NULL != m_pchDisCodeBuffer[0]))
    {
        HANDLE hFile = CreateFile(strFileName.c_str(),
                                   GENERIC_WRITE,
                                   FILE_SHARE_READ|FILE_SHARE_WRITE,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
         
        if (hFile == INVALID_HANDLE_VALUE) 
            return 0;

        DWORD dwUnicode = 0xFEFF;
        DWORD dwWritten = 0;
        WriteFile( hFile, &dwUnicode, 2, &dwWritten, NULL );

        int nSize = _tcslen( m_pchDisCodeBuffer );

        for(int i=0; i<nSize ; i++)
        {
            if(_T('|') == m_pchDisCodeBuffer[i])
                m_pchDisCodeBuffer[i] = _T(' ');
        }

        WriteFile( hFile, m_pchDisCodeBuffer
            , nSize * sizeof(TCHAR), &dwWritten, NULL );

        CloseHandle( hFile );
    }

    return 0;
}

void CNikDisassemblyWin::OnMouseMove( WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = LOWORD( lParam );
    pt.y = HIWORD( lParam );

    RECT rc;
    for(int i=0;i<4;i++)
    {
        rc = m_rListViewRect[i];
        rc.left = m_rListViewRect[i].right - 4;
        rc.right = rc.left + 8;
        if(PtInRect( &rc, pt))
        {
            HCURSOR hCursor = LoadCursor( NULL, IDC_SIZEWE );
            SetCursor( hCursor );
        }
    }

    if( wParam & MK_LBUTTON )
    {
        if(m_nListRectNo >= 0)
        {
            int nDiff = pt.x - m_nListRectStart;
            if( nDiff 
                && (m_nListRectNo >= 0))
            {
                int i = m_nListRectNo;
                if((m_rListViewRect[i].right + nDiff) > m_rListViewRect[i].left)
                {
                    m_rListViewRect[i].right += nDiff;
                    if(m_rListViewRect[i].right > 0)
                    {
                        i++;
                        for( ; i<4; i++)
                        {
                            OffsetRect( &m_rListViewRect[i], nDiff, 0 );
                        }
                    }

                    m_nListRectStart = pt.x;
                    InvalidateRect( m_hWnd, NULL, TRUE );
                    UpdateWindow( m_hWnd );
                }
            }
            return;
        }
    }
}

void CNikDisassemblyWin::OnLButtonDown( WPARAM /*wParam*/, LPARAM lParam)
{
    SetFocus( m_hWnd );

    POINT pt;
    pt.x = LOWORD( lParam );
    pt.y = HIWORD( lParam );

    RECT rc;
    for(int i=0;i<4;i++)
    {
        rc = m_rListViewRect[i];
        rc.left = m_rListViewRect[i].right - 4;
        rc.right = rc.left + 8;
        if(PtInRect( &rc, pt))
        {
            m_nListRectNo = i;
            SetCapture( m_hWnd );
            HCURSOR hCursor = LoadCursor( NULL, IDC_SIZEWE );
            SetCursor( hCursor );
            m_nListRectStart = pt.x;
            return;
        }
    }

    m_nListRectNo           =   -1;
    m_nListRectStart        =   -1;
}

void CNikDisassemblyWin::OnLButtonUp( WPARAM /*wParam*/, LPARAM lParam )
{
    POINT pt;
    RECT rc,rc1;
    rc1 = GetDrawRectangle();
    rc = GetDrawRectangle();
    rc.right = rc.left + 20;
    pt.x = LOWORD( lParam );
    pt.y = HIWORD( lParam );

    if(TRUE == PtInRect(&rc, pt))
    {
        SendMessage( GetParent( m_hWnd ), UM_SETBREAKPOINT
            , GetLineAddress( pt ), lParam );
        InvalidateRect( m_hWnd, &rc, TRUE );
        UpdateWindow( m_hWnd );
    }
    else if(TRUE == PtInRect(&rc1, pt))
    {
        int nLineNo = 0;
        if(m_nLineHeight)
            nLineNo = pt.y / m_nLineHeight;
        nLineNo += m_nOffsetLineNo;
        nLineNo -= rc1.top / m_nLineHeight;
        m_dwFocusLine = nLineNo;

        InvalidateRect( m_hWnd, &rc1, TRUE );
        UpdateWindow( m_hWnd );
    }

    m_nListRectNo           =   -1;
    m_nListRectStart        =   -1;

    ReleaseCapture();
}

_tstring CNikDisassemblyWin::GetImportFunctionName( DWORD dwAddress )
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;

    DWORD dwBaseAddress = m_pstExecutableInfo->m_stNTHeader.OptionalHeader.ImageBase;
    DWORD dwTemp, dwTemp1 = dwAddress - dwBaseAddress;

    const vector<IMPORTDLLLIST> &vImportFunctionList = m_pstExecutableInfo->m_vImportDllList;
    for(int j=0;j<(int)vImportFunctionList.size();j++)
    {
        dwTemp = vImportFunctionList.at(j).m_stImageDescriptor.FirstThunk;
        if((dwTemp <= dwTemp1)
            && (dwTemp+(4*vImportFunctionList.at(j).m_vImportFunctionList.size()) > dwTemp1))
        {
            dwTemp1 -= dwTemp;
            dwTemp1 >>= 2;
            if(dwTemp1 < vImportFunctionList.at(j).m_vImportFunctionList.size())
            {
                _stprintf_s( chBuf, 1023, _T("%s [%s]"), vImportFunctionList.at(j).m_vImportFunctionList.at(dwTemp1).m_strFunctionName.c_str()
                                                        , vImportFunctionList.at(j).m_strDllName.c_str());
                return chBuf;
            }
            break;
        }
    }

    return _T("");
}

int CNikDisassemblyWin::ShowPEDisassemblyCode()
{
    TCHAR chBuf[1024];
    chBuf[1023] = 0;

    if(m_pchDisCodeBuffer)
    {
        delete [] m_pchDisCodeBuffer;
        m_pchDisCodeBuffer = NULL;
        m_vDisNewLineOffset.clear();
        m_nOffsetLineNo = 0;
    }

    m_vDisOpcodeOffset.clear();

    _tstring strTemp, strDisBuffer;
    BYTE * pOffsetBytes = NULL;
    int nOffset = 0, nEntryPoint = 0;

    int nBaseCode = m_pstExecutableInfo->m_stNTHeader.OptionalHeader.BaseOfCode 
                        + m_pstExecutableInfo->m_stNTHeader.OptionalHeader.ImageBase;
    nBaseCode = m_pstExecutableInfo->m_dwStartOffset;
    int nEndStartDiff = (m_pstExecutableInfo->m_dwEndOffset - m_pstExecutableInfo->m_dwStartOffset);
    int nProgressStep = nEndStartDiff / 100;

    for(DWORD i=m_pstExecutableInfo->m_dwStartOffset; i<m_pstExecutableInfo->m_dwEndOffset; i++)
    {
        //_stprintf_s(chBuf, 64, _T("0x%08X"), i);
        _stprintf_s(chBuf, 1023, _T("0x%08X"), nBaseCode);
        strDisBuffer += chBuf;

        m_vDisOpcodeOffset.push_back( i );

        pOffsetBytes = (BYTE *)&m_pstExecutableInfo->m_pBuffer[i];
        _tstring str = m_pNikDisassembly->DisOpcodeReturns(&m_pstExecutableInfo->m_pBuffer[i], nOffset);
        if(nOffset)
        {
	        i += nOffset - 1;
            nBaseCode += nOffset;
        }
        else
        {
            _stprintf_s(chBuf, 1023, _T("??? 0x%02X db"), pOffsetBytes[0]);
            str = chBuf;
            nOffset = 1;
            nBaseCode += nOffset;
        }

        if(i < m_pstExecutableInfo->m_dwEntryPoint)
            nEntryPoint++;

        if(nOffset)
        {
            strTemp = _T("");

            switch(nOffset)
            {
            case 1:
                _stprintf_s(chBuf, 1023, _T("0x%02X"), pOffsetBytes[0]);
                strTemp += chBuf;
                break;
            case 2:
                _stprintf_s(chBuf, 1023, _T("0x%02X 0x%02X"), pOffsetBytes[0]
                                                          , pOffsetBytes[1]);
                strTemp += chBuf;
                break;
            case 3:
                _stprintf_s(chBuf, 1023, _T("0x%02X 0x%02X 0x%02X"), pOffsetBytes[0]
                                                                 , pOffsetBytes[1]
                                                                 , pOffsetBytes[2]);
                strTemp += chBuf;
                break;
            case 4:
                _stprintf_s(chBuf, 1023, _T("0x%02X 0x%02X 0x%02X 0x%02X"), pOffsetBytes[0]
                                                                        , pOffsetBytes[1]
                                                                        , pOffsetBytes[2]
                                                                        , pOffsetBytes[3]);
                strTemp += chBuf;
                break;
            case 5:
                _stprintf_s(chBuf, 1023, _T("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X"), pOffsetBytes[0]
                                                                               , pOffsetBytes[1]
                                                                               , pOffsetBytes[2]
                                                                               , pOffsetBytes[3]
                                                                               , pOffsetBytes[4]);
                strTemp += chBuf;
                break;
            case 6:
                _stprintf_s(chBuf, 1023, _T("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X"), pOffsetBytes[0]
                                                                                      , pOffsetBytes[1]
                                                                                      , pOffsetBytes[2]
                                                                                      , pOffsetBytes[3]
                                                                                      , pOffsetBytes[4]
                                                                                      , pOffsetBytes[5]);
                strTemp += chBuf;
                break;

            default:
                for(int j=0; j<nOffset; j++)
                {
                    _stprintf_s(chBuf, 1023, _T("0x%02X"), pOffsetBytes[j]);
                    strTemp += chBuf;
                    if(nOffset != (j+1))
                        strTemp += _T(" ");
                }
                break;
            }
        }

        strDisBuffer += _T("|");
        strDisBuffer += strTemp;

        strTemp = m_pNikDisassembly->GetNoString(str, 1);
        strDisBuffer += _T("|");
        strDisBuffer += strTemp;

        strTemp = m_pNikDisassembly->GetNoString(str, 2);
        if(!strTemp.empty())
        {
            int nPos = str.find(strTemp);
            strTemp = str.substr(nPos, (DWORD)-1);
            strDisBuffer += _T("|");
            strDisBuffer += strTemp;
        }

        ///----------- CALL ds:0x00000000 ---------------
        WORD * pwOpcode = (WORD *)pOffsetBytes;
        if( (0x15FF == pwOpcode[0])
            //|| (0x25FF == pwOpcode[0])
            )
        {
            DWORD * pdwAddress = (DWORD *)&pwOpcode[1];
            _tstring strDisBuf = GetImportFunctionName( pdwAddress[0] );
            if( !strDisBuf.empty() )
            {
                strDisBuffer += _T("  -->  ");
                strDisBuffer += strDisBuf;
            }
        }
        ///-------------- MOV ds:0x00000000 ---------------
        else if(0x8b == pOffsetBytes[0])
        {
            THREEPART * pth = (THREEPART *)&pOffsetBytes[1];
            if(5 == ((pth->mod<<3)|(pth->rm)))
            {
                DWORD * pdwAddress = (DWORD *)&pwOpcode[1];
                _tstring strDisBuf = GetImportFunctionName( pdwAddress[0] );
                if( !strDisBuf.empty() )
                {
                    strDisBuffer += _T("  <--  ");
                    strDisBuffer += strDisBuf;
                }
            }
        }

        strDisBuffer += _T("\n");
        m_vDisNewLineOffset.push_back(strDisBuffer.length());

        if(FALSE == m_bIsThreadRunning)
            return -1;

        if(0 == (i%nProgressStep))
        {
            int nProcess = (((i-m_pstExecutableInfo->m_dwStartOffset)*100)/nEndStartDiff);
            if(nProcess > 1)
            {
                if( !m_pchDisCodeBuffer )
                {
                    SetDisasmBuffer( strDisBuffer );
                    SetScrollBar( m_hWnd );
                }
            }

            if(nEndStartDiff)
            {
                _stprintf_s(chBuf, 1023, _T("Code dis-assembly progress - [%d%%]"), nProcess );
                SetWindowText( m_hWnd, chBuf);
            }
        }
    }

    m_vDisOpcodeOffset.push_back( m_pstExecutableInfo->m_dwEndOffset );

    strDisBuffer += _T("\n");
    m_vDisNewLineOffset.push_back(strDisBuffer.length());
    SetWindowText( m_hWnd, _T("Code dis-assembly viewer"));

    nOffset = strDisBuffer.length();

    SetDisasmBuffer( strDisBuffer );
    strDisBuffer.clear();

    m_bIsThreadRunning = FALSE;

    SetScrollBar( m_hWnd );

    ///--------------------------------------------
    SetActiveAddress( m_pstExecutableInfo->m_dwEntryPoint );
    int nLineNo = CovertAddressToLineNo( m_pstExecutableInfo->m_dwEntryPoint );
    if(FALSE == IsLineVisible(nLineNo))
    {
        SetOffsetLineNo( nLineNo );
        InvalidateRect( m_hWnd, NULL, TRUE );
        UpdateWindow( m_hWnd );
    }
    ///--------------------------------------------

    return 0;
}

void CNikDisassemblyWin::SetDisasmBuffer( _tstring strBuffer )
{
    TCHAR * pByteTemp =  m_pchDisCodeBuffer;

    int nOffset = strBuffer.length();
    m_pchDisCodeBuffer = new TCHAR[nOffset + 2];
    if(m_pchDisCodeBuffer)
    {
        m_pchDisCodeBuffer[0] = 0;
        m_pchDisCodeBuffer[nOffset] = m_pchDisCodeBuffer[nOffset+1] = 0;

        _tcscpy_s(m_pchDisCodeBuffer
                  , nOffset + 1         //  include the null character
                  , (TCHAR *)strBuffer.c_str());

        if( pByteTemp )
            delete [] pByteTemp;
    }
}

int CNikDisassemblyWin::DrawSelectionRect( HDC hdc, RECT rc, COLORREF cRect, int nItemNo )
{
    if( hdc 
        && !IsRectEmpty(&rc))
    {
        int nHeight = rc.bottom - rc.top;
        int nLineInPage = nHeight / m_nLineHeight;
        //int nWidth = rc.right - rc.left;
        int nSize = m_vDisNewLineOffset.size();
        if((nSize > 0)
            && ((nItemNo < (m_nOffsetLineNo+nLineInPage)) 
            && (nItemNo >= m_nOffsetLineNo)))
        {
            RECT rItemRect;
            rItemRect.left      = rc.left;
            rItemRect.right     = rc.right;
            rItemRect.top       = ((nItemNo-m_nOffsetLineNo)*m_nLineHeight) + rc.top;
            rItemRect.bottom    = rItemRect.top + m_nLineHeight;

            HBRUSH hBrush;
            hBrush = CreateSolidBrush( cRect );
            if(hBrush)
            {
                FillRect(hdc, &rItemRect, hBrush);
                DeleteObject(hBrush);
            }
        }
    }

    return 0;
}

int CNikDisassemblyWin::DrawActivePoint( HDC hdc, RECT rc )
{
    if(hdc 
        && m_pchDisCodeBuffer
        && m_vDisNewLineOffset.size())
    {
        //HPEN hPen = CreatePen( PS_SOLID, 1, RGB(255,255,255));
        //HPEN hPenOld = (HPEN)SelectObject( hdc, hPen );

        //HBRUSH hBrush = CreateSolidBrush( RGB(0,0,255) );
        //HBRUSH hBrushOld = (HBRUSH)SelectObject( hdc, hBrush );

        int nMode = SetBkMode(hdc, TRANSPARENT);
        COLORREF cColor = SetTextColor( hdc, RGB(0,0,255) );

        int i = m_dwActiveLine;
        if(i >= 0)
        {
            int nHeight = rc.bottom - rc.top;
            int nLineInPage = nHeight / m_nLineHeight;

            if((i >= m_nOffsetLineNo) 
                &&  (i <= (m_nOffsetLineNo+nLineInPage)))
            {
                RECT rc1    = rc;
                rc1.left    = rc.left;
                rc1.right   = rc1.left + m_nLineHeight - 1;
                rc1.top     = ((i - m_nOffsetLineNo) * m_nLineHeight) + rc.top;
                rc1.bottom  = rc1.top + m_nLineHeight;

                /*POINT pt[6];
                pt[0].x = rc1.left + 10;
                pt[0].y = rc1.top + 3;
                pt[1].x = pt[0].x + 10;
                pt[1].y = pt[0].y;
                pt[2].x = pt[1].x + 5;
                pt[2].y = pt[1].y + 5;
                pt[3].x = pt[1].x;
                pt[3].y = pt[2].y + 5;
                pt[4].x = pt[0].x;
                pt[4].y = pt[3].y;
                pt[5].x = pt[0].x;
                pt[5].y = pt[0].y;

                Polygon( hdc, pt, 6 );*/

                TextOut( hdc, rc1.left + 3, rc1.top, _T("-->"), 3 );
            }
        }

        SetBkMode(hdc, nMode);

        SetTextColor( hdc, cColor );

        //SelectObject( hdc, hBrushOld );
        //DeleteObject( hBrush );

        //SelectObject( hdc, hPenOld );
        //DeleteObject( hPen );

        return TRUE;
    }

    return FALSE;
}

int CNikDisassemblyWin::DrawBreakPoints( HDC hdc, RECT rc )
{
    if(hdc 
        && m_pchDisCodeBuffer
        && m_vDisNewLineOffset.size())
    {
        RECT rItemRect;
        int nBreakPoint;
        rItemRect.left  = rc.left + 5;
        rItemRect.right = rItemRect.left + m_nLineHeight - 1;

        int nHeight = rc.bottom - rc.top;
        int nLineInPage = nHeight / m_nLineHeight;

        HPEN hPen = CreatePen( PS_SOLID, 1, RGB(255,255,255));
        HPEN hPenOld = (HPEN)SelectObject( hdc, hPen );

        HBRUSH hBrush = CreateSolidBrush( RGB(255,0,0) );
        HBRUSH hBrushOld = (HBRUSH)SelectObject( hdc, hBrush );

        for(int i=0; i<(int)m_vBreakPoints.size(); i++)
        {
            nBreakPoint = m_vBreakPoints.at(i);
            if((nBreakPoint >= m_nOffsetLineNo) 
                &&  (nBreakPoint <= (m_nOffsetLineNo+nLineInPage)))
            {
                rItemRect.top   = (nBreakPoint-m_nOffsetLineNo)*m_nLineHeight + 1 + rc.top;
                rItemRect.bottom=rItemRect.top + m_nLineHeight - 1;

                Ellipse( hdc, rItemRect.left, rItemRect.top
                    , rItemRect.right, rItemRect.bottom );
                
                
            }
        }

        SelectObject( hdc, hBrushOld );
        DeleteObject( hBrush );

        SelectObject( hdc, hPenOld );
        DeleteObject( hPen );

        return TRUE;
    }

    return FALSE;
}

int CNikDisassemblyWin::DrawDisassemblyData( HDC hdc, RECT rc, TCHAR * pBuffer, int nOffset)
{
    if(hdc 
        && pBuffer
        && !IsRectEmpty(&rc))
    {
        HRGN hrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom); 
		SelectClipRgn(hdc, hrgn);

		int nMode = SetBkMode(hdc, TRANSPARENT);

		HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);

        _tstring strBuffer;
        TCHAR * pTempBuf, * pTempBuf1, * pDisBuffer;
        int nLineHeight = 0, nTemp;
        //int nWidth = rc.right - rc.left;
        //int nHeight = rc.bottom - rc.top;

        pDisBuffer = &pBuffer[nOffset];

        int nBufferSize = _tcslen(pBuffer);

        int nLineNo = 0;
        COLORREF cTextColor = RGB(0, 0, 0);

        int nIsFocus = 0;

        if( m_hWnd == GetFocus() )
            nIsFocus = 1;

        while(nOffset < nBufferSize)
        {
            if(((int)m_dwFocusLine == (nLineNo + m_nOffsetLineNo)) 
                && ( 1 == nIsFocus ))
                cTextColor = SetTextColor( hdc, RGB(255, 255, 255));

            pTempBuf = NULL;
            pTempBuf1 = _tcschr(pDisBuffer, _T('\n'));
            for(int i=0;i<4;i++)
            {
                switch(i)
                {
                case 0:
                    pTempBuf = _tcschr(pDisBuffer, _T('|'));
                    break;

                case 1:
                    pTempBuf = _tcschr(pDisBuffer, _T('|'));
                    break;

                case 2:
                    pTempBuf = _tcschr(pDisBuffer, _T('|'));
                    break;

                case 3:
                    pTempBuf = _tcschr(pDisBuffer, _T('\n'));
                    break;
                }

                RECT rTemp = m_rListViewRect[i];
                rTemp.left += 3;
                rTemp.top = rc.top + nLineHeight;
                rTemp.bottom = rTemp.top + m_nLineHeight;

                nTemp = (int)(pTempBuf1-pTempBuf);
                if(nTemp < 0)
                {
                    nTemp = (int)(pTempBuf1 - pDisBuffer);
                    //TextOut(hdc, m_rListViewRect[i].left+3, rc.top+nLineHeight, pDisBuffer, nTemp);
                    DrawText( hdc, pDisBuffer, nTemp, &rTemp, DT_END_ELLIPSIS | DT_LEFT );
                    pDisBuffer = (pTempBuf1+1);
                    nOffset += (nTemp + 1);
                    break;
                }

                nTemp = (int)(pTempBuf - pDisBuffer);
                if(nTemp == 0)
                {
                    pDisBuffer = (pTempBuf+1);
                    nOffset += (nTemp + 1);
                    continue;
                }
                else if(nTemp < 0)
                    break;

                if(1 == i)
                {
                    if(TRUE == m_bIsDisplayBinary)
                        DrawText( hdc, pDisBuffer, nTemp, &rTemp, DT_END_ELLIPSIS | DT_LEFT );
                }
                else
                    DrawText( hdc, pDisBuffer, nTemp, &rTemp, DT_END_ELLIPSIS | DT_LEFT );

                pDisBuffer = (pTempBuf+1);
                nOffset += (nTemp + 1);
            }

            if(((int)m_dwFocusLine == (nLineNo + m_nOffsetLineNo)) 
                && ( 1 == nIsFocus ))
                SetTextColor( hdc, cTextColor);

            nLineHeight += m_nLineHeight;
            if(nLineHeight > rc.bottom)
                break;

            nLineNo++;
        }

		SelectObject(hdc, hOldFont);

		SetBkMode(hdc, nMode);
		SelectClipRgn(hdc, NULL);
		DeleteObject(hrgn);
    }

    return 0;
}

ATOM CNikDisassemblyWin::NikDissembeRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= NikDissembeWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NIKPEVIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szDisAssemblyWindowClass1;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

HWND CNikDisassemblyWin::NikDissembeInitInstance( HWND hWndParent
                                                 , RECT rc
                                                 , _tstring strFileName
                                                 , EXECUTABLE_INFO * pstExecutableInfo
                                                 , CNikDisassembly * pNikDisassembly )
{
    if(NULL == pNikDisassembly)
        return NULL;

    DWORD dwStyle = WS_TABSTOP |
                    WS_VISIBLE |
                    WS_SIZEBOX |
                    WS_CAPTION |
                    WS_SYSMENU |
                    WS_VSCROLL | 
                    //WS_HSCROLL |
                    WS_CHILDWINDOW |
                    WS_CLIPSIBLINGS |
                    0;

    m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW
                            , szDisAssemblyWindowClass1
                            , szDisAssemblyTitle1
						    , dwStyle
						    , 0, 0, 0, 0
						    , hWndParent
                            , NULL
						    , GetWindowInstance(hWndParent)
						    , (LPARAM)NULL );

    if (!m_hWnd)
    {
        return FALSE;
    }

    m_pNikDisassembly = pNikDisassembly;
    m_pstExecutableInfo = pstExecutableInfo;

    SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)this);

    SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 0);

    HDC hdc = GetDC( m_hWnd );
    if(hdc)
    {
        CreateEditorFont( hdc );
        ReleaseDC(m_hWnd, hdc);
    }

    SetFocus( m_hWnd );

    //OpenDisassemblyFile(strFileName);

    DWORD dwThreadId = 0;
    m_bIsThreadRunning  = TRUE;
    m_hThread = CreateThread( NULL, 0, DisAssemblyThreadProc, this, 0, &dwThreadId);

    return m_hWnd;
}

void CNikDisassemblyWin::OnKeyDown( WPARAM wParam, LPARAM /*lParam*/)
{
    int nHeight = m_rDrawRect.bottom - m_rDrawRect.top;
    int nLineInPage = (nHeight / m_nLineHeight) - 1;

    switch(wParam)
    {
    case VK_UP:
        m_dwFocusLine--;
        break;
    case VK_DOWN:
        m_dwFocusLine++;
        break;
    case VK_NEXT:
        m_dwFocusLine += nLineInPage;
        break;
    case VK_PRIOR:
        m_dwFocusLine -= nLineInPage;
        break;
    }

    if((int)m_dwFocusLine > (int)(m_vDisNewLineOffset.size()-2))
        m_dwFocusLine = m_vDisNewLineOffset.size() - 2;

    if((int)m_dwFocusLine < 0)
        m_dwFocusLine = 0;

    if(((int)m_dwFocusLine >= m_nOffsetLineNo)
        && ((int)m_dwFocusLine < (m_nOffsetLineNo+nLineInPage)))
    {
        // Just do nothing
    }
    else if((int)m_dwFocusLine < m_nOffsetLineNo)
    {
        m_nOffsetLineNo = m_dwFocusLine;
        SetScrollBar( m_hWnd );
    }
    else if((int)m_dwFocusLine > (m_nOffsetLineNo+nLineInPage))
    {
        m_nOffsetLineNo = m_dwFocusLine - nLineInPage;
        SetScrollBar( m_hWnd );
    }

    InvalidateRect( m_hWnd, NULL, TRUE );
    UpdateWindow( m_hWnd );
}

void CNikDisassemblyWin::OnScrollBar( HWND hWnd, int nFlag, WPARAM wParam )
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;

    int nHeight = m_rDrawRect.bottom - m_rDrawRect.top;
    int nLineInPage = (nHeight / m_nLineHeight) - 1;

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
			si.nPos -= nLineInPage;//si.nPage;
			break;

			// user clicked the scroll bar shaft below the scroll box
		case SB_PAGEDOWN:
			si.nPos += nLineInPage;//si.nPage;
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
            m_nOffsetLineNo = si.nPos;
			ScrollWindowEx(hWnd, 0, m_nLineHeight * (nCurPos - si.nPos)
							, NULL, &m_rDrawRect, NULL, NULL, SW_INVALIDATE);
		}
	}
}

void CNikDisassemblyWin::DrawListViewBoders( HDC hdc , RECT rc )
{
    if(hdc && (!IsRectEmpty(&rc)))
	{
        int nHeight = rc.bottom - rc.top;
        int nWeight = rc.right - rc.left;
        HPEN hPen = CreatePen( PS_SOLID, 1, RGB(165, 165, 165));
        HPEN hPenOld = (HPEN)SelectObject( hdc, hPen );
        MoveToEx( hdc, 0, nHeight-1, NULL );
        LineTo( hdc, rc.left + nWeight-1, rc.top + nHeight-1 );
        LineTo( hdc, rc.left + nWeight-1, rc.top );
        SelectObject( hdc, hPenOld );
        DeleteObject( hPen );

        hPen = CreatePen( PS_SOLID, 1, RGB(255, 255, 255));
        hPenOld = (HPEN)SelectObject( hdc, hPen );
        MoveToEx( hdc, rc.left + nWeight, rc.top + nHeight-1, NULL );
        LineTo( hdc, rc.left + nWeight, rc.top );
        SelectObject( hdc, hPenOld );
        DeleteObject( hPen );
    }
}

void CNikDisassemblyWin::DrawListViewTop( HDC hdc )
{
    if( hdc )
    {
        RECT rc2, rc1, rc = m_rDrawRect;
        GetClientRect( m_hWnd, &rc1 );

        SubtractRect( &rc2, &rc1, &rc );

        HBRUSH hBrush = CreateSolidBrush( RGB(235, 235, 235) );
        if(hBrush)
        {
            FillRect(hdc, &rc2, hBrush);

            RECT rc3 = rc1;
            rc3.right = 20;
            FillRect(hdc, &rc3, hBrush);

            DeleteObject(hBrush);
        }

        int nWid = 20;
        RECT rc3 = rc2;
        rc3.right = nWid;
        DrawListViewBoders( hdc, rc3 );

        DrawListViewBoders( hdc, m_rListViewRect[0] );

        RECT rTemp;
        int nMode = SetBkMode( hdc, TRANSPARENT );
        HFONT hOldFont = (HFONT)SelectObject( hdc, m_hFont );

        rTemp = m_rListViewRect[0];
        rTemp.left += 5;
        DrawText( hdc, _T("Offset"), _tcslen(_T("Offset"))
            , &rTemp, DT_END_ELLIPSIS | DT_LEFT );

        if(TRUE == m_bIsDisplayBinary)
        {
            DrawListViewBoders( hdc, m_rListViewRect[1] );

            rTemp = m_rListViewRect[1];
            rTemp.left += 5;
            DrawText( hdc, _T("Hex code"), _tcslen(_T("Hex code"))
            , &rTemp, DT_END_ELLIPSIS | DT_LEFT );
        }

        DrawListViewBoders( hdc, m_rListViewRect[2] );

        rTemp = m_rListViewRect[2];
        rTemp.left += 5;
        DrawText( hdc, _T("Command"), _tcslen(_T("Command"))
            , &rTemp, DT_END_ELLIPSIS | DT_LEFT );

        DrawListViewBoders( hdc, m_rListViewRect[3] );

        rTemp = m_rListViewRect[3];
        rTemp.left += 5;
        DrawText( hdc, _T("Opcode"), _tcslen(_T("Opcode"))
            , &rTemp, DT_END_ELLIPSIS | DT_LEFT );

        SelectObject( hdc, hOldFont );
        SetBkMode( hdc, nMode );
    }
}

void CNikDisassemblyWin::PaintWindow( HDC hdc, RECT rc1 )
{
	if(hdc)
	{
		HDC memDC = CreateCompatibleDC ( hdc );

        int nWidth = rc1.right - rc1.left;
        int nHeight = rc1.bottom - rc1.top;

        if(NULL == m_hBitmap)
		{
			m_hBitmap = CreateCompatibleBitmap ( hdc, nWidth, nHeight );
		}

        HBITMAP OldBM = (HBITMAP)SelectObject ( memDC, m_hBitmap );

        RECT rc = m_rDrawRect;

        FillRect(memDC, &rc, GetStockBrush(WHITE_BRUSH));

        DrawListViewTop( memDC );

        RECT rDrawRect = rc;
        rDrawRect.left += 20;

        if( m_hWnd == GetFocus() )
            DrawSelectionRect( memDC, rDrawRect, RGB(50, 95, 170), m_dwFocusLine );
        else
            DrawSelectionRect( memDC, rDrawRect, RGB(235, 235, 235), m_dwFocusLine );

        if(m_nOffsetLineNo <= 0)
            DrawDisassemblyData(memDC, rDrawRect, m_pchDisCodeBuffer, 0);
        else
		    DrawDisassemblyData(memDC, rDrawRect, m_pchDisCodeBuffer
                        , m_vDisNewLineOffset.at(m_nOffsetLineNo-1));

        //DrawBreakPoints( memDC, rc );
        //DrawActivePoint( memDC, rc );

		BitBlt(hdc, 0, 0, nWidth, nHeight, memDC, 0, 0, SRCCOPY);

		SelectObject ( hdc, OldBM);

		DeleteDC(memDC);
	}
}

LRESULT CALLBACK CNikDisassemblyWin::NikDissembeWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

    CNikDisassemblyWin * pNikDisassemblyWin = (CNikDisassemblyWin *)GetWindowLong( hWnd, GWL_USERDATA );

	switch (message)
	{
    case WM_VSCROLL:
        if(pNikDisassemblyWin)
            pNikDisassemblyWin->OnScrollBar(hWnd, WM_VSCROLL, wParam);
		break;

    case WM_SIZE:
        if(pNikDisassemblyWin)
        {
            if(pNikDisassemblyWin->m_hBitmap)
	        {
		        DeleteObject(pNikDisassemblyWin->m_hBitmap);
		        pNikDisassemblyWin->m_hBitmap = NULL;
	        }

            pNikDisassemblyWin->SetScrollBar(hWnd);
        }
        return DefWindowProc( hWnd, message, wParam, lParam );

    case WM_KILLFOCUS:
        if(pNikDisassemblyWin)
        {
            InvalidateRect( hWnd, NULL, TRUE );
            UpdateWindow( hWnd );
        }
        break;

    case WM_CREATE:
        break;

    case WM_KEYDOWN:
        if(pNikDisassemblyWin)
            pNikDisassemblyWin->OnKeyDown( wParam, lParam );
        break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
        if(pNikDisassemblyWin)
        {
            if(pNikDisassemblyWin->m_pchDisCodeBuffer 
                && (pNikDisassemblyWin->m_vDisNewLineOffset.size() > 0))
            {
                RECT rc;
                GetClientRect(hWnd, &rc);
                pNikDisassemblyWin->PaintWindow( hdc, rc );
            }
        }
		EndPaint(hWnd, &ps);
		break;

    case WM_COMMAND:
        PostMessage( GetParent( hWnd ), message, wParam, lParam );
        break;

    case WM_LBUTTONUP:
        if(pNikDisassemblyWin)
        {
            pNikDisassemblyWin->OnLButtonUp( wParam, lParam );
            SetFocus(hWnd);
        }
        //break;
        return 0;

    case WM_LBUTTONDOWN:
        if(pNikDisassemblyWin)
        {
            pNikDisassemblyWin->OnLButtonDown( wParam, lParam );
            SetWindowPos( hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        }
        //break;
        return 0;

    case WM_MOUSEMOVE:
        if(pNikDisassemblyWin)
            pNikDisassemblyWin->OnMouseMove( wParam, lParam );
        //break;
        return 0;

    case WM_MOUSEWHEEL:
		if(pNikDisassemblyWin)
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if(zDelta > 0)
				pNikDisassemblyWin->OnScrollBar(hWnd, WM_VSCROLL, UM_WHEELUP);
			else
				pNikDisassemblyWin->OnScrollBar(hWnd, WM_VSCROLL, UM_WHEELDOWN);
		}
		break;

    case WM_ERASEBKGND:
        return TRUE;

    case WM_CLOSE:
        ShowWindow ( hWnd, SW_HIDE );
        return 0;

	case WM_DESTROY:
        if(pNikDisassemblyWin)
        {
            pNikDisassemblyWin->m_hWnd = NULL;
        }
		return DefWindowProc( hWnd, message, wParam, lParam );

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	//return 0;
    return DefWindowProc( hWnd, message, wParam, lParam );
}