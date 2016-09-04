
#pragma once

#define TES_STRING					_T("ABCDEFGHIJKLMNOPQRSTWXYZ")
#define	EDITOR_FONT					_T("Courier New")
#define	HEADER_ADD					_T("Offset")
#define LEFT_WINDOW_BAR				20
#define EDITOR_TAB_SIZE				4
#define BINARY_CHAR_ROW				0x10

#define	BINARY_HEX					1
#define BINARY_TEXT					2
#define BINARY_OCTAL				3
#define BINARY_DECIMAL				4
#define BINARY_BINARY				5

#define SIDE_BAR_COLOR				RGB(232,232,232)
#define LINE_NUMBER_COLOR			RGB(0,208,0)
#define WINDOW_BACKGROUND_COLOR		RGB(255,255,255)
#define BIN_SELECTION_COLOR         RGB(50, 95, 170)
#define TEXT_SELECTION_COLOR        RGB(200, 200, 200)

typedef struct _TEXT_SELECT_RECT
{
    int         m_nEnd;
    int         m_nStart;
    RECT        m_rSelect;

    _TEXT_SELECT_RECT()
    {
        ResetSelection();
    }

    void ResetSelection()
    {
        m_nEnd      = -1;
        m_nStart    = -1;
        memset( &m_rSelect, 0, sizeof(RECT));
    }

}TEXT_SELECT_RECT;

class CBinaryEditor
{
    BYTE *                          m_pbBinBuffer;

    RECT							m_rDrawArea;
    RECT                            m_rACSIIRect[3];

    LOGFONT							m_lf;
    HFONT							m_hFont;

    HBITMAP							m_hBitmap;

    int 							m_nLineHeight;
    int                             m_nMaxCharLen;
    int								m_nBinarySize;
    int                             m_nLineNoType;
    int                             m_nASCIIOffset;
    int                             m_nLineNumbers;
    int                             m_nTotalLineNo;
    int                             m_nBufferOffset;
    int                             m_nTabsSizes[2];
    int								m_nLeftBoundary;
    int 							m_nAveCharWidth;
    int 							m_nSelectionEnd;
    int                             m_nBinRowNumbers;
    int 							m_nSelectionStart;
    int								m_nLineNumberWidth;

    BOOL							m_bLineNumberEnable;
    BOOL							m_bBinHeaderEnable;

    TEXT_SELECT_RECT                m_stSelectRect[3];

public:

    void SetBinBuffer( BYTE * pbBuffer
                     , int nBufferSize
                     , int nBufferOffset )
    {
        m_pbBinBuffer = pbBuffer;
        m_nBinarySize = nBufferSize;
        m_nTotalLineNo = m_nBinarySize / m_nBinRowNumbers;

        SetBufferOffset( nBufferOffset );
    }

    void SetBufferOffset( int nBufferOffset )
    {
        m_nBufferOffset = (nBufferOffset/m_nBinRowNumbers) * m_nBinRowNumbers;
    }

    BYTE * GetBinBuffer()
    {
        return m_pbBinBuffer;
    }

    RECT GetDrawRect()
	{
		return m_rDrawArea;
	}
    void SetDrawRect(RECT rc)
	{
		m_rDrawArea = rc;
	}

    BOOL IsLinenumberEnable()
	{
		return m_bLineNumberEnable;
	}

    BOOL IsBinHeaderEnable()
	{
		return m_bBinHeaderEnable;
	}

    int GetBinRowNumbers()
    {
        return m_nBinRowNumbers;
    }

    void SetBinRowNumbers( int nBinRowNumbers )
    {
        m_nBinRowNumbers = nBinRowNumbers;
    }

    void SetSelection(int nStart=-1, int nEnd=-1);

    BOOL IsBinSelection()
    {
        if((m_nSelectionEnd == m_nSelectionStart)
            || (m_nSelectionEnd < 0)
            || (m_nSelectionStart < 0))
        {
            //m_nSelectionEnd = -1;
            //m_nSelectionStart = -1;
            return FALSE;
        }

        return TRUE;
    }

    void GetEditorWindowSize(int &nWidth,int &nHeight)
	{
		nWidth = m_rDrawArea.right - m_rDrawArea.left;
		nHeight = m_rDrawArea.bottom - m_rDrawArea.top;
	}

    HBITMAP GetEditorBitmap()
	{
		return m_hBitmap;
	}
	void SetEditorBitmap(HBITMAP hBitmap = NULL)
	{
        if(m_hBitmap)
	        DeleteObject(m_hBitmap);

		m_hBitmap = hBitmap;
	}

    CBinaryEditor( HWND hWnd );
    virtual ~CBinaryEditor();

    virtual void CalculateBinRowNumber();
    virtual void CreateEditorFont( HDC hdc );
    virtual void CalculateCharLen( HDC hdc );
    virtual void CalculateLineHeight( HDC hdc );

    virtual void PaintWindow( HDC hdc );
    virtual void DrawBinEditor( HDC hdc );
    virtual void DrawBinaryHeader( HDC hdc);
    virtual void DrawBinForground( HDC hdc );
    virtual void DrawTextSelection( HDC hdc );
    virtual void DrawBinBackground( HDC hdc );
    virtual void DrawBinaryLineNumber(HDC hdc);
    virtual void DrawBinaryData( HDC hdc, BYTE * pbBinBuffer, RECT rc);
    virtual void DrawLineBinary(HDC hdc,BYTE * pBuffer
			    , int nOffsetX, int nOffsetY, int nLineOffset);

    virtual int GetBinOffsetfromPt(POINT pt);

    void SetScrollBar( HWND hWnd );
    void OnScrollBar(HWND hWnd, int nFlag, WPARAM wParam);

    virtual POINT GetBinPtfromOffset( int nOffset , int nASCIIOffset );

    static void DrawRectArea( HDC hdc, RECT rc, COLORREF col );
};

///----------------------------

class CNikPEBinaryViewer
{
    HWND            m_hWnd;
    HINSTANCE       m_hInstance;
    CBinaryEditor   * m_pBinaryEditor;

public:
    CNikPEBinaryViewer();
    virtual ~CNikPEBinaryViewer();

    HWND GetWindowHandle()
    {
        return m_hWnd;
    }

    CBinaryEditor * GetBinaryEditor()
    {
        return m_pBinaryEditor;
    }

    int SetBinaryData(BYTE *, int );
    HWND CreateBinaryViewer(HINSTANCE , HWND , RECT , _tstring );
    static LRESULT CALLBACK ToolChildWndProc( HWND hWnd
                                              , UINT uMsg
                                              , WPARAM wParam
                                              , LPARAM lParam );
};