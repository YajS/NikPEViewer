
#include "Stdafx.h"
#include "Resource.h"
#include "PEViewer.h"
#include "PEViewerListView.h"

extern CNikDisassembly g_cNikDisassembly;

CNikPEVListViewCtrl::CNikPEVListViewCtrl()
{
    m_hWnd      = NULL;
    m_hInstance = NULL;
}

CNikPEVListViewCtrl::~CNikPEVListViewCtrl()
{
}

int CNikPEVListViewCtrl::FillPEVListViewInfo(void * )
{
    return -1;
}

WNDPROC         g_wpOrigListProc;

// Subclass procedure 
LRESULT APIENTRY ListSubclassProc( HWND hWnd
                                  , UINT uMsg
                                  , WPARAM wParam
                                  , LPARAM lParam) 
{
    switch( uMsg )
    {
    case WM_LBUTTONDOWN:
        SetWindowPos( hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        return CallWindowProc(g_wpOrigListProc, hWnd, uMsg, wParam, lParam);
    case WM_CLOSE:
        ShowWindow ( hWnd, SW_HIDE );
        break;

    default:
        return CallWindowProc(g_wpOrigListProc, hWnd, uMsg, wParam, lParam); 
    }
 
    return 0;
}

HWND CNikPEVListViewCtrl::CreatePEListViewCtrl(HINSTANCE hInstance
                                               , HWND hParent
                                               , RECT rWindRect
                                               , _tstring strCaption)
{
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

    m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, WC_LISTVIEW, strCaption.c_str()
                            , dwStyle, rWindRect.left, rWindRect.top
                            , rWindRect.right - rWindRect.left
                            , rWindRect.bottom - rWindRect.top
                            , hParent, (HMENU)ID_LISTVIEW
                            , hInstance, NULL);

	if(!m_hWnd)
		return NULL;

    g_wpOrigListProc = (WNDPROC) SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) ListSubclassProc);

    SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, /*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT);

    return m_hWnd;
}

BOOL CNikPEVListViewCtrl::InsertColumnListViewCtl( HWND hWnd
                                                 , TCHAR * pchStr
                                                 , int nIndex
                                                 , int nOffsetX)
{
    LVCOLUMN lvc;

    memset(&lvc, 0, sizeof(LVCOLUMN));
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 	  
    // Add the columns.
    lvc.iSubItem = nIndex;
    lvc.pszText = pchStr;
    lvc.cx = nOffsetX;     // width of column in pixels
        
    lvc.fmt = LVCFMT_LEFT;

    if (ListView_InsertColumn(hWnd, 0, &lvc) == -1) 
        return FALSE;

    return TRUE;
}

BOOL CNikPEVListViewCtrl::InsertColumnListViewValues_1( HWND hWnd
                                                       , TCHAR * pchStr
                                                       , int nIndex
                                                       , int nFlag)
{
    LVITEM lvI;
    memset(&lvI, 0, sizeof(LVITEM));
    lvI.mask = LVIF_TEXT; 
    lvI.state = 0; 
    lvI.stateMask = 0;
    lvI.pszText = pchStr;

    int nSize = ListView_GetItemCount(hWnd);

    if(0 == nFlag)
    {
	    lvI.iItem = nSize;
	    lvI.iSubItem = 0;

	    if(ListView_InsertItem(hWnd, &lvI) == -1)
		    return NULL;
    }
    else
    {
        if((nSize - 1) >= 0)
            lvI.iItem = nSize - 1;
        else
            lvI.iItem = 0;
	    lvI.iSubItem = nIndex;

        if(ListView_SetItem(hWnd, &lvI) == -1)
			return NULL;
    }

    return TRUE;
}

BOOL CNikPEVListViewCtrl::InsertColumnListViewValues( HWND hWnd
                                                    , TCHAR * pchStr
                                                    , int nIndex
                                                    , int nFlag// = 0
)
{
    LVITEM lvI;
    memset(&lvI, 0, sizeof(LVITEM));
    lvI.mask = LVIF_TEXT; 
    lvI.state = 0; 
    lvI.stateMask = 0;
    lvI.pszText = pchStr;

    //int nSize = ListView_GetItemCount(hWnd);

    if(0 == nFlag)
    {
	    lvI.iItem = 0;//    nSize;
	    lvI.iSubItem = 0;

	    if(ListView_InsertItem(hWnd, &lvI) == -1)
		    return NULL;
    }
    else
    {
        //if((nSize - 1) >= 0)
        //    lvI.iItem = nSize - 1;
        //else
            lvI.iItem = 0;
	    lvI.iSubItem = nIndex;

        if(ListView_SetItem(hWnd, &lvI) == -1)
			return NULL;
    }

    return TRUE;
}

LRESULT ListView_Notify(HWND // hWnd
                        , LPARAM lParam)
{
    LPNMHDR  lpnmh = (LPNMHDR) lParam;

	switch( lpnmh->code )
	{
    case NM_RCLICK:
		break;
    }

    return 0;
}

///------------------------------

CNikPEVHeaderList::CNikPEVHeaderList()
{
    m_hWnd      = NULL;
    m_hInstance = NULL;
}

CNikPEVHeaderList::~CNikPEVHeaderList()
{
}

int CNikPEVHeaderList::List_IMAGE_DOS_HEADER(const IMAGE_DOS_HEADER * pDosHeader)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;

    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_LFANEW, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_lfanew);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_OEMINFO, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_oeminfo);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_OEMID, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_oemid);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_OVNO, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_ovno);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_LFARLC, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_lfarlc);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_CS, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_cs);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_IP, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_ip);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_CSUM, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_csum);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_SP, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_sp);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_SS, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_ss);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_MAXALLOC, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_maxalloc);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_MINALLOC, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_minalloc);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_CPARHDR, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_cparhdr);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_CRLC, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_crlc);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_CP, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_cp);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_CBLP, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pDosHeader->e_cblp);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, DOS_HEADER_E_MAGIC, 0, 0);
    _stprintf_s(chBuf, 1023, _T("0x%X \'%C%C\'"), pDosHeader->e_magic
        , (char)(pDosHeader->e_magic&0xFF), (char)((pDosHeader->e_magic>>8)&0xFF));
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
    InsertColumnListViewValues(m_hWnd, DOS_HEADER_1, 0, 0);

    return TRUE;
}

int CNikPEVHeaderList::List_IMAGE_NT_HEADERS(const IMAGE_NT_HEADERS * pNTHeaders)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;

	int nExtended = 0;

	PIMAGE_NT_HEADERS64 pNTHeaders64 = (const PIMAGE_NT_HEADERS64)pNTHeaders;

	if(pNTHeaders->OptionalHeader.Magic == 0x020b)
		nExtended = 1;

    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_NUOFRVAANDSIZES, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.NumberOfRvaAndSizes);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_LOADERFLAGS, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.LoaderFlags);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFHEAPCOMMIT, 0, 0);
	if(nExtended)
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders64->OptionalHeader.SizeOfHeapCommit);
	else
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfHeapCommit);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFHEAPRESERVE, 0, 0);
	if(nExtended)
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders64->OptionalHeader.SizeOfHeapReserve);
	else
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfHeapReserve);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFSTACKCOMMIT, 0, 0);
	if(nExtended)
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders64->OptionalHeader.SizeOfStackCommit);
	else
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfStackCommit);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFSTACKRESERVE, 0, 0);
	if(nExtended)
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders64->OptionalHeader.SizeOfStackReserve);
	else
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfStackReserve);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_DLLCHARACTERISTIC, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.DllCharacteristics);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SUBSYSTEM, 0, 0);
    CNikPEViewer::GetNTOptionalSubsystem(pNTHeaders->OptionalHeader.Subsystem, 1023, chBuf);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_CHECKSUM, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.CheckSum);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFHEADERS, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfHeaders);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFIMAGE, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfImage);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_WIN32VERSIONVALUE, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.Win32VersionValue);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MINORSUBSYSVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MinorSubsystemVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MAJORSUBSYSVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MajorSubsystemVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MINORIMAGEVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MinorImageVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MAJORLINKERVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MajorLinkerVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MINOROSVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MinorOperatingSystemVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MAJOROSVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MajorOperatingSystemVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_FILEALIGNMENT, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.FileAlignment);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SECTIONALIGNMENT, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SectionAlignment);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_IMAGEBASE, 0, 0);
	if(nExtended)
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders64->OptionalHeader.ImageBase);
	else
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.ImageBase);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

	/// ----------------------------------------------------------------

	if(!nExtended)
	{
        InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_BASEOFDATA, 0, 0);
		_stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.BaseOfData);
		InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);
	}

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_BASEOFCODE, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.BaseOfCode);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_ADDOFENTRYPOINT, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.AddressOfEntryPoint);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFUNINITDATA, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfUninitializedData);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFINITDATA, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfInitializedData);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_SIZEOFCODE, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.SizeOfCode);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MINORIMAGEVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MinorLinkerVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MINORLINKERVERSION, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.MinorLinkerVersion);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_MAGIC, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->OptionalHeader.Magic);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, _T("------------------"), 0, 0);
    InsertColumnListViewValues(m_hWnd, NT_HEADERS_OP_1, 0, 0);

    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

    _tstring strChar;
    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_CHARACTERISTIC, 0, 0);
    CNikPEViewer::GetNTCharacteristics(pNTHeaders->FileHeader.Characteristics, strChar);
    _stprintf_s(chBuf, 1023, _T("%s 0x%X"), strChar.data(), pNTHeaders->FileHeader.Characteristics);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_SIZEOFOPHEADER, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->FileHeader.SizeOfOptionalHeader);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_NUMBEROFSYMBOL, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->FileHeader.NumberOfSymbols);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_PTSYMBOLTABLE, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->FileHeader.PointerToSymbolTable);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_TIMEDATESTAMP, 0, 0);
    CNikPEViewer::GetStrDateTimeStamp(pNTHeaders->FileHeader.TimeDateStamp, 1023, chBuf);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_NUOFSECTIONS, 0, 0);
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, pNTHeaders->FileHeader.NumberOfSections);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_MACHINE, 0, 0);
    CNikPEViewer::GetMachineName(pNTHeaders->FileHeader.Machine, 1023, chBuf);
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, _T("--------------"), 0, 0);
    InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_1, 0, 0);

    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

    InsertColumnListViewValues(m_hWnd, NT_HEADERS_SIGNATURE, 0, 0);
    _stprintf_s(chBuf, 1023, _T("0x%08X \'%C%C\'"), pNTHeaders->Signature
        , (char)(pNTHeaders->Signature&0xFF), (char)((pNTHeaders->Signature>>8)&0xFF));
    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

    InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
    InsertColumnListViewValues(m_hWnd, NT_HEADERS_1, 0, 0);

    return TRUE;
}

int CNikPEVHeaderList::FillPEVListViewInfo(void * pNikPEV)
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        RECT rc;
	    GetClientRect(m_hWnd, &rc);

	    ListView_DeleteAllItems(m_hWnd);

        InsertColumnListViewCtl(m_hWnd, STR_HEX_VALUES, 0, (rc.right >> 1));
        InsertColumnListViewCtl(m_hWnd, STR_DESCRIPTION, 0, (rc.right >> 1));

        List_IMAGE_NT_HEADERS(&pNikPEViewer->GetImageNtHeaders());
        List_IMAGE_DOS_HEADER(&pNikPEViewer->GetImageDosHeader());

        return 0;
    }

    return -1;
}

///------------------------------

TCHAR STR_IMPORT_FUNCTIONS[]            = _T("Import functions");

CNikImportFunList::CNikImportFunList()
{
    m_hWnd      = NULL;
    m_hInstance = NULL;
}

CNikImportFunList::~CNikImportFunList()
{
}

int CNikImportFunList::FillPEVListViewInfo(void * pNikPEV)
{
    m_pNikPEViewer = pNikPEV;

    if(m_pNikPEViewer)
    {
        List_IMAGE_NT_IMPORT_TABLE( (CNikPEViewer *)m_pNikPEViewer );
        return 0;
    }

    return -1;
}

int CNikImportFunList::List_IMAGE_NT_IMPORT_TABLE(void * pNikPEV)
{
    TCHAR chBuf[1024];
    chBuf[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        RECT rc;
	    GetClientRect(m_hWnd, &rc);

	    ListView_DeleteAllItems(m_hWnd);

        InsertColumnListViewCtl(m_hWnd, STR_HEX_VALUES, 0, (rc.right >> 1));
        InsertColumnListViewCtl(m_hWnd, STR_DESCRIPTION, 0, (rc.right >> 1));

        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

        const vector<IMPORTDLLLIST> &vImportFunctionList = pNikPEViewer->GetImportFunctionList();
        for(int i=0;i<(int)vImportFunctionList.size();i++)
        {
            for(int j=0; j<(int)vImportFunctionList.at(i).m_vImportFunctionList.size(); j++)
            {
                _stprintf_s(chBuf, 1023, _T("     %s"), vImportFunctionList.at(i).m_vImportFunctionList.at(j).m_strFunctionName.data());
                InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);
                _stprintf_s(chBuf, 1023, _T("%d (Ordinal No)"), vImportFunctionList.at(i).m_vImportFunctionList.at(j).m_nOrdinalNo);
                InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);
            }

            _stprintf_s(chBuf, 1023, STR_PRINTF_s, vImportFunctionList.at(i).m_strDllName.data());
            InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), vImportFunctionList.at(i).m_stImageDescriptor.FirstThunk);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
        }
    }

    InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
    InsertColumnListViewValues(m_hWnd, STR_IMPORT_FUNCTIONS, 0, 0);

    return TRUE;
}

///------------------------------

CNikExportFunList::CNikExportFunList()
{
    m_hWnd      = NULL;
    m_hInstance = NULL;
}

CNikExportFunList::~CNikExportFunList()
{
}

int CNikExportFunList::List_IMAGE_NT_EXPORT_TABLE(void * pNikPEV)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        RECT rc;
	    GetClientRect(m_hWnd, &rc);

	    ListView_DeleteAllItems(m_hWnd);

        InsertColumnListViewCtl(m_hWnd, STR_ORDIANL_NO, 0, (rc.right/6));
        InsertColumnListViewCtl(m_hWnd, STR_ENTRY_POINT, 0, (rc.right/3));
        InsertColumnListViewCtl(m_hWnd, STR_FUNCTION_NAME, 0, (rc.right >> 1));

        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

        const vector<EXPORTFUNCTIONLIST> &vExportFunctionList = pNikPEViewer->GetExportFunctionList();
        for(int i=0;i<(int)vExportFunctionList.size();i++)
        {
            _stprintf_s(chBuf, 1023, STR_PRINTF_s, vExportFunctionList.at(i).m_strFunctionName.data());
            InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);

            _stprintf_s(chBuf, 1023, _T("0x%08X"), vExportFunctionList.at(i).m_dwEntryPoint);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            _stprintf_s(chBuf, 1023, _T("%d"), vExportFunctionList.at(i).m_dwOrdianlNo);
            InsertColumnListViewValues(m_hWnd, chBuf, 2, 1);
        }

        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
    }

    InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
    InsertColumnListViewValues(m_hWnd, STR_EXPORT_FUNCTIONS, 0, 0);

    return TRUE;
}

int CNikExportFunList::FillPEVListViewInfo(void * pNikPEV)
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        List_IMAGE_NT_EXPORT_TABLE( pNikPEViewer );

        return 0;
    }

    return -1;
}

///------------------------------

CNikDataSectionList::CNikDataSectionList()
{
    m_hWnd      = NULL;
    m_hInstance = NULL;
}

CNikDataSectionList::~CNikDataSectionList()
{
}

int CNikDataSectionList::FillPEVListViewInfo(void * pNikPEV)
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        RECT rc;
	    GetClientRect(m_hWnd, &rc);

	    ListView_DeleteAllItems(m_hWnd);

        InsertColumnListViewCtl(m_hWnd, STR_HEX_VALUES, 0, (rc.right >> 1));
        InsertColumnListViewCtl(m_hWnd, STR_DESCRIPTION, 0, (rc.right >> 1));

        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

        List_IMAGE_NT_SECTION_DETAILS( pNikPEViewer );
        List_IMAGE_NT_DATA_DIRECTORY( pNikPEViewer );

        return 0;
    }

    return -1;
}

int CNikDataSectionList::List_IMAGE_NT_DATA_DIRECTORY(void * pNikPEV)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        const IMAGE_NT_HEADERS *pNTHeaders = &pNikPEViewer->GetImageNtHeaders();

        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

	    PIMAGE_NT_HEADERS64 pNTHeaders64 = (const PIMAGE_NT_HEADERS64)pNTHeaders;

	    if(pNTHeaders->OptionalHeader.Magic == 0x020b)
	    {
		    for(int i=IMAGE_NUMBEROF_DIRECTORY_ENTRIES-1; i>=0; i--)
		    {
			    if(pNTHeaders64->OptionalHeader.DataDirectory[i].Size)
			    {
				    _stprintf_s(chBuf, 1023, STR_VIRTUAL_ADDRESS, i);
				    InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);
				    _stprintf_s(chBuf, 1023, _T("0x%08X"), pNTHeaders64->OptionalHeader.DataDirectory[i].VirtualAddress);
				    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);
				    _stprintf_s(chBuf, 1023, STR_DATADIRECTORY_SIZE, i);
				    InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);
				    _stprintf_s(chBuf, 1023, _T("%d Bytes"), pNTHeaders64->OptionalHeader.DataDirectory[i].Size);
				    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

				    CNikPEViewer::GetDirectoryEnteries(i, 1023, chBuf);
                    InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);

				    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
			    }
		    }
	    }
	    else
	    {
		    for(int i=IMAGE_NUMBEROF_DIRECTORY_ENTRIES-1; i>=0; i--)
		    {
			    if(pNTHeaders->OptionalHeader.DataDirectory[i].Size)
			    {
				    _stprintf_s(chBuf, 1023, STR_VIRTUAL_ADDRESS, i);
				    InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);
				    _stprintf_s(chBuf, 1023, _T("0x%08X"), pNTHeaders->OptionalHeader.DataDirectory[i].VirtualAddress);
				    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);
				    _stprintf_s(chBuf, 1023, STR_DATADIRECTORY_SIZE, i);
				    InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);
				    _stprintf_s(chBuf, 1023, _T("%d Bytes"), pNTHeaders->OptionalHeader.DataDirectory[i].Size);
				    InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

				    CNikPEViewer::GetDirectoryEnteries(i, 1023, chBuf);
                    InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);

				    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
			    }
		    }
	    }

        InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
        InsertColumnListViewValues(m_hWnd, STR_DATA_DIRECTORY, 0, 0);
    }

    return TRUE;
}

int CNikDataSectionList::List_IMAGE_NT_SECTION_DETAILS(void * pNikPEV)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

        for(int i=(int)pNikPEViewer->GetImageSectionHeaderList().size()-1; i>=0; i--)
        {
            InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

            _tstring strChar;
            CNikPEViewer::GetNTSectionCharacteristics(pNikPEViewer->GetImageSectionHeaderList().at(i).Characteristics, strChar);

            InsertColumnListViewValues(m_hWnd, NT_HEADERS_FH_CHARACTERISTIC, 0, 0);
            _stprintf_s(chBuf, 1023, _T("%s 0x%08X"), strChar.data(), pNikPEViewer->GetImageSectionHeaderList().at(i).Characteristics);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_NUMBEROFLINENUMBERS, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%04X"), pNikPEViewer->GetImageSectionHeaderList().at(i).NumberOfLinenumbers);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_NUMBEROFRELOCATIONS, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%04X"), pNikPEViewer->GetImageSectionHeaderList().at(i).NumberOfRelocations);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_POINTERTOLINENUMBERS, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToLinenumbers);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_POINTERTORELOCATIONS, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToRelocations);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_POINTERTORAWDATA, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToRawData);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_SIZEOFRAWDATA, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), pNikPEViewer->GetImageSectionHeaderList().at(i).SizeOfRawData);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_VIRTUAL_ADDRESS, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), pNikPEViewer->GetImageSectionHeaderList().at(i).VirtualAddress);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            InsertColumnListViewValues(m_hWnd, SH_PHYSICAL_ADDRESS, 0, 0);
            _stprintf_s(chBuf, 1023, _T("0x%08X"), pNikPEViewer->GetImageSectionHeaderList().at(i).Misc.PhysicalAddress);
            InsertColumnListViewValues(m_hWnd, chBuf, 1, 1);

            //InsertColumnListViewValues(m_hWnd, SH_SECTIONS_NAME, 0, 0);
            _stprintf_s(chBuf, 1023, _T("%S"), pNikPEViewer->GetImageSectionHeaderList().at(i).Name);
            InsertColumnListViewValues(m_hWnd, chBuf, 0, 0);

        }

        InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
        InsertColumnListViewValues(m_hWnd, SH_SECTIONS_LIST, 0, 0);
    }

    return TRUE;
}

///------------------------------

CNikResourceItemsList::CNikResourceItemsList()
{
}

CNikResourceItemsList::~CNikResourceItemsList()
{
}

int CNikResourceItemsList::FillPEVListViewInfo(const void * pResList)
{
    RESOURCELIST * pResourceList = (RESOURCELIST *)pResList;
    if(pResourceList)
    {
        RECT rc;
	    GetClientRect(m_hWnd, &rc);

	    ListView_DeleteAllItems(m_hWnd);

        //InsertColumnListViewCtl(m_hWnd, _T("Hex values"), 0, (rc.right >> 1));
        InsertColumnListViewCtl(m_hWnd, _T("Name"), 0, (rc.right >> 1));

        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
        InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column

        List_IMAGE_RESOURCE_ITEMS_TABLE( pResourceList, 0 );

        InsertColumnListViewValues(m_hWnd, _T("=========="), 0, 0);
        InsertColumnListViewValues(m_hWnd, _T("Resource items"), 0, 0);

        return 0;
    }

    return -1;
}

int CNikResourceItemsList::List_IMAGE_RESOURCE_ITEMS_TABLE(const void * pResList
                                                           , int nLevel)
{
    RESOURCELIST * pResourceList = (RESOURCELIST *)pResList;
    if(pResourceList)
    {
        _tstring strTemp;
        RESOURCELIST * pTempResource;
        for(int i=(int)pResourceList->m_vDirList.size()-1; i>=0; i--)
        {
            int nLevel_1 = 0;
            pTempResource = pResourceList->m_vDirList.at(i);
            if(pTempResource)
            {
                List_IMAGE_RESOURCE_ITEMS_TABLE( pTempResource, ++nLevel_1 );

                strTemp = _T("");
                for(int j=0; j<nLevel; j++)
                    strTemp += _T("   ");
                strTemp += pTempResource->m_chName;

                InsertColumnListViewValues(m_hWnd, (TCHAR *)strTemp.c_str(), 0, 0);

                if(0 == nLevel)
                    InsertColumnListViewValues(m_hWnd, _T(""), 0, 0);  //  Blank column
            }
        }

        return 0;
    }

    return -1;
}

///------------------------------

CNikDisAssemblyList::CNikDisAssemblyList()
{
    m_hThread           = NULL;
    m_dwThreadId        = 0;
    m_bIsThreadRunning  = FALSE;
}

CNikDisAssemblyList::~CNikDisAssemblyList()
{
    StopDisAssemblyThread();
    CloseHandle(m_hThread);
}

int CNikDisAssemblyList::StopDisAssemblyThread()
{
    m_bIsThreadRunning  = FALSE;
    Sleep( 100 );

    DWORD dwExitCode;
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

/*TCHAR Regester_1[8][8] = {
				{_T("eax")},
				{_T("ecx")},
				{_T("edx")},
				{_T("ebx")},
				{_T("esp")},
				{_T("ebp")},
				{_T("esi")},
				{_T("edi")}};*/

DWORD WINAPI CNikDisAssemblyList::DisAssemblyThreadProc( LPVOID lpParam )
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)lpParam;
    if(pNikPEViewer)
    {
        EXECUTABLE_INFO * pstExeInfo = (EXECUTABLE_INFO *)&pNikPEViewer->GetExecutableInfo();
        if(pstExeInfo->m_dwBufferSize 
            && pstExeInfo->m_pBuffer)
        {
            TCHAR chBuf[1024];
            chBuf[0] = 0;
            chBuf[1023] = 0;

            BYTE * pbOpcode;
            WORD * pwOpcode;
            DWORD * pdwAddress;
            DWORD dwBaseAddress = pNikPEViewer->GetImageNtHeaders().OptionalHeader.ImageBase;
            _tstring strTemp;
            BYTE * pOffsetBytes = NULL;
            int nOffset = 0;
            int nEndStartDiff = (pstExeInfo->m_dwEndOffset - pstExeInfo->m_dwStartOffset);
            int nProgressStep = nEndStartDiff/100;
            int nCodeEnteries = 0;

            for(DWORD i=pstExeInfo->m_dwStartOffset; i<pstExeInfo->m_dwEndOffset; i++)
		    {
                ///-------------- Address ----------------
                _stprintf_s(chBuf, 1023, _T("0x%08X"), i);
                InsertColumnListViewValues_1( pstExeInfo->m_hListWnd, chBuf, 0, 0);

                pbOpcode = &pstExeInfo->m_pBuffer[i];
                pwOpcode = (WORD *)&pstExeInfo->m_pBuffer[i];

                ///-------- Get the disassembly opcode --------
                pOffsetBytes = (BYTE *)&pstExeInfo->m_pBuffer[i];
                _tstring str = g_cNikDisassembly.DisOpcodeReturns(&pstExeInfo->m_pBuffer[i], nOffset);
			    if(nOffset)
				    i += nOffset - 1;
                else
                {
                    str = _T("???");
                    nOffset = 1;
                }

                ///-------------- Hex bytes ----------------
                strTemp = _T("");
                for(int j=0; j<nOffset; j++)
                {
                    _stprintf_s(chBuf, 1023, _T("0x%02X"), pOffsetBytes[j]);
                    strTemp += chBuf;
                    if(nOffset != (j+1))
                        strTemp += _T(" ");
                }
                InsertColumnListViewValues_1(pstExeInfo->m_hListWnd
                    , (TCHAR *)strTemp.data(), 1, 1);

                ///-------------- Command ----------------
                strTemp = g_cNikDisassembly.GetNoString(str, 1);
                InsertColumnListViewValues_1(pstExeInfo->m_hListWnd
                    , (TCHAR *)strTemp.data(), 2, 1);

                ///----------- CALL ds:0x00000000 ---------------
                if( (0x15FF == pwOpcode[0])
                    || (0x25FF == pwOpcode[0]))
                {
                    DWORD dwTemp, dwTemp1;
                    pdwAddress = (DWORD *)&pwOpcode[1];
                    dwTemp1 = pdwAddress[0] - dwBaseAddress;

                    const vector<IMPORTDLLLIST> &vImportFunctionList = pNikPEViewer->GetImportFunctionList();
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
                                _stprintf_s(chBuf, 1023, _T("%s [%s]"), vImportFunctionList.at(j).m_vImportFunctionList.at(dwTemp1).m_strFunctionName.c_str()
                                                                        , vImportFunctionList.at(j).m_strDllName.c_str());
                                InsertColumnListViewValues_1( pstExeInfo->m_hListWnd, (TCHAR *)chBuf, 4, 1);
                            }
                            break;
                        }
                    }
                }
                /*//-------------- MOV ds:0x00000000 ---------------
                else if(0x8b == pbOpcode[0])
                {
                    THREEPART * pth = (THREEPART *)&pbOpcode[1];
                    if(5 == ((pth->mod<<3)|(pth->rm)))
                    {
                        DWORD dwTemp, dwTemp1;
                        pdwAddress = (DWORD *)&pwOpcode[1];
                        dwTemp1 = pdwAddress[0] - dwBaseAddress;

                        const vector<IMPORTDLLLIST> &vImportFunctionList = pNikPEViewer->GetImportFunctionList();
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
                                    _stprintf_s(chBuf, 1023, _T("%s <- %s [%s]")
                                                , Regester_1[pth->reg]
                                                , vImportFunctionList.at(j).m_vImportFunctionList.at(dwTemp1).m_strFunctionName.c_str()
                                                , vImportFunctionList.at(j).m_strDllName.c_str());
                                    InsertColumnListViewValues_1( pstExeInfo->m_hListWnd, (TCHAR *)chBuf, 4, 1);
                                }
                                break;
                            }
                        }
                    }
                }*/

                ///-------------- Opcode ----------------
                strTemp = g_cNikDisassembly.GetNoString(str, 2);
                if(!strTemp.empty())
                {
                    int nPos = str.find(strTemp);
                    strTemp = str.substr(nPos, str.length());
                    InsertColumnListViewValues_1( pstExeInfo->m_hListWnd
                        , (TCHAR *)strTemp.data(), 3, 1);
                }

                ///-------------- Disassembly progress ----------------
                if(0 == (i%nProgressStep))
                {
                    if(nEndStartDiff)
                    {
                        _stprintf_s(chBuf, 1023, _T("Code dis-assembly progress - [%d%%]")
                            , (((i-pstExeInfo->m_dwStartOffset)*100)/nEndStartDiff));
                        SetWindowText( pstExeInfo->m_hListWnd, chBuf);
                    }
                }

                if(i < pstExeInfo->m_dwEntryPoint)
                    nCodeEnteries++;

                if(FALSE == pNikPEViewer->GetDisAssemblyList().IsDisThreadRunning())
                    return (DWORD)-1;
		    }

            RECT rc;
            ListView_GetItemRect( pstExeInfo->m_hListWnd, 0, &rc, LVIR_BOUNDS);
            if(nCodeEnteries > 0)
                ListView_Scroll( pstExeInfo->m_hListWnd
                    , 0, nCodeEnteries * (rc.bottom-rc.top) );

            ListView_EnsureVisible( pstExeInfo->m_hListWnd, nCodeEnteries, FALSE );
            ListView_SetSelectionMark( pstExeInfo->m_hListWnd, nCodeEnteries );

            _stprintf_s(chBuf, 1023, _T("Code section dis-assembly view - [Entry point = 0x%08X]")
                , pstExeInfo->m_dwEntryPoint);
            SetWindowText( pstExeInfo->m_hListWnd, chBuf);

            SetFocus( pstExeInfo->m_hListWnd );
            ListView_SetItemState( pstExeInfo->m_hListWnd
                , nCodeEnteries, LVIS_SELECTED, LVIS_SELECTED);
        }
    }

    return 0;
}

int CNikDisAssemblyList::FillPEVListViewInfo(const void * pNikPEV)
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPEV;
    if(pNikPEViewer)
    {
        EXECUTABLE_INFO * pstExeInfo = (EXECUTABLE_INFO *)&pNikPEViewer->GetExecutableInfo();
        if(pstExeInfo->m_dwBufferSize 
            && pstExeInfo->m_pBuffer)
        {
            pstExeInfo->m_hListWnd = m_hWnd;

            RECT rc;
	        GetClientRect(m_hWnd, &rc);

	        ListView_DeleteAllItems( m_hWnd );

            InsertColumnListViewCtl( m_hWnd, _T("Import functions"), 0, (rc.right >> 2));
            InsertColumnListViewCtl( m_hWnd, _T("Opcode"), 0, (rc.right >> 2));
            InsertColumnListViewCtl( m_hWnd, _T("Command"), 0, (rc.right/10));
            InsertColumnListViewCtl( m_hWnd, _T("Hex code"), 0, (rc.right/3));
            InsertColumnListViewCtl( m_hWnd, _T("Address"), 0, (rc.right/10));

            m_bIsThreadRunning  = TRUE;

            m_hThread = CreateThread( NULL, 0, DisAssemblyThreadProc
                , pNikPEViewer, 0, &m_dwThreadId);

            return 0;
        }
    }

    return -1;
}
