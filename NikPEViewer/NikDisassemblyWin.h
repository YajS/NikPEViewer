

#pragma once

#include "NikDisAssembly.h"

#define	EDITOR_FONT					_T("Courier New")
#define TES_STRING					_T("ABCDEFGHIJKLMNOPQRSTWXYZ")

class CNikDisassemblyWin
{
    HBITMAP             m_hBitmap;
    HFONT               m_hFont;
    LOGFONT             m_lf;
    RECT                m_rDrawRect;

    int                 m_nLineHeight;
    int                 m_nMaxCharWidth;
    int                 m_nAveCharWidth;
    int                 m_nOffsetLineNo;

    int                 m_nListRectNo;
    int                 m_nListRectStart;
    RECT                m_rListViewRect[5];

    TCHAR *             m_pchDisCodeBuffer;

    DWORD               m_dwFocusLine;
    DWORD               m_dwActiveLine;

    vector<int>         m_vBreakPoints;
    vector<int>         m_vDisNewLineOffset;

    vector<DWORD>       m_vDisOpcodeOffset;

    HWND                m_hWnd;
    HANDLE              m_hThread;

    BOOL                m_bIsDisplayBinary;
    BOOL                m_bIsThreadRunning;

    CNikDisassembly *   m_pNikDisassembly;
    EXECUTABLE_INFO *   m_pstExecutableInfo;

public:
    CNikDisassemblyWin();
    virtual ~CNikDisassemblyWin();

    RECT GetDrawRectangle()
    {
        return m_rDrawRect;
    }

    HWND GetWindowHandle()
    {
        return m_hWnd;
    }

    int GetOffsetLineNo()
    {
        return m_nOffsetLineNo;
    }

    void SetOffsetLineNo( int nDisOffsetLineNo )
    {
        m_nOffsetLineNo = nDisOffsetLineNo;
        SetScrollBar( m_hWnd );
    }

    void ToggleBinaryData()
    {
        if(FALSE == m_bIsDisplayBinary)
            m_bIsDisplayBinary = TRUE;
        else
            m_bIsDisplayBinary = FALSE;

        InvalidateRect( m_hWnd, NULL, TRUE );
        UpdateWindow( m_hWnd );
    }

    BOOL IsLineVisible( int nDisOffsetLineNo )
    {
        RECT rc;
        GetClientRect( m_hWnd, &rc );
        int nLineInPage = rc.bottom / m_nLineHeight;

        if((nDisOffsetLineNo >= m_nOffsetLineNo) 
            &&  (nDisOffsetLineNo < (m_nOffsetLineNo+nLineInPage)))
        {
            return TRUE;
        }

        return FALSE;
    }

    vector<int> &GetBreakPointList()
    {
        return m_vBreakPoints;
    }

    BOOL IsDisThreadRunning()
    {
        return m_bIsThreadRunning;
    }

    void SetActiveAddress( DWORD dwActiveAddress )
    {
        m_dwActiveLine = CovertAddressToLineNo( dwActiveAddress );
        m_dwFocusLine = m_dwActiveLine;
    }

    int CovertAddressToLineNo( DWORD dwAddress )
    {
        for(int i=0; i<(int)m_vDisOpcodeOffset.size()-1; i++)
        {
            if(dwAddress == (DWORD)m_vDisOpcodeOffset.at(i))
                return i;
            else if((dwAddress >= (DWORD)m_vDisOpcodeOffset.at(i))
                && (dwAddress < (DWORD)m_vDisOpcodeOffset.at(i+1)))
                return i;
        }

        return -1;
    }

    int SetBreakPoint( DWORD dwAddress )
    {
        int i = CovertAddressToLineNo( dwAddress );

        if(i >= 0)
        {
            m_vBreakPoints.push_back( i );
            return TRUE;
        }

        return FALSE;
    }

    int UnSetBreakPoint( DWORD dwAddress )
    {
        int ii = CovertAddressToLineNo( dwAddress );

        if(ii >= 0)
        {
            for(int i=0; i<(int)m_vBreakPoints.size(); i++)
            {
                if(ii == m_vBreakPoints.at(i))
                {
                    m_vBreakPoints.erase(m_vBreakPoints.begin() + i);
                    return TRUE;
                }
            }
        }

        return FALSE;
    }

    int StopDisAssemblyThread();
    int ShowPEDisassemblyCode();
    void SetScrollBar(HWND hWnd);
    void SetLogFont(LOGFONT  &lf);
    void DrawListViewTop( HDC hdc );
    DWORD GetLineAddress( POINT pt );
    void CreateEditorFont( HDC hdc );
    void CalculateLineHeight(HDC hdc);
    void PaintWindow( HDC hdc, RECT rc );
    int DrawBreakPoints(HDC hdc, RECT rc);
    int DrawActivePoint(HDC hdc, RECT rc);
    void SetDisasmBuffer( _tstring strBuffer );
    int DumpAssemblyCode( _tstring strFileName );
    void DrawListViewBoders( HDC hdc , RECT rc );
    void OnKeyDown( WPARAM wParam, LPARAM lParam);
    void OnMouseMove( WPARAM wParam, LPARAM lParam );
    void OnLButtonUp( WPARAM wParam, LPARAM lParam );
    _tstring GetImportFunctionName( DWORD dwAddress );
    void OnLButtonDown( WPARAM wParam, LPARAM lParam );
    void OnScrollBar(HWND hWnd, int nFlag, WPARAM wParam);
    static ATOM NikDissembeRegisterClass(HINSTANCE hInstance);
    static DWORD WINAPI DisAssemblyThreadProc( LPVOID lpParam );
    int DrawSelectionRect( HDC hdc, RECT rc, COLORREF cRect, int nItemNo );
    int DrawDisassemblyData( HDC hdc, RECT rc, TCHAR * pBuffer, int nOffset);

    static LRESULT CALLBACK NikDissembeWndProc(HWND hWnd, UINT message
        , WPARAM wParam, LPARAM lParam);
    HWND NikDissembeInitInstance( HWND hWndParent, RECT rc, _tstring strFileName
        , EXECUTABLE_INFO * pstExecutableInfo, CNikDisassembly * pNikDisassembly);
};

