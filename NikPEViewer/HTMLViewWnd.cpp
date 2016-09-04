
#include "Stdafx.h"
#include "Resource.h"
#include "PEViewer.h"
#include "CommonStr.h"
#include "PEViewerListView.h"

extern TCHAR STR_IMPORT_FUNCTIONS[];

//////----------------------------------------------------------------------
//CNikHTMLWindow * g_pNikHTMLWindow;

STDMETHODIMP CHtmlFrame::GetWindow(HWND FAR* lphwnd)
{
    *lphwnd = m_pNikHTMLWin->m_hWnd;

    return S_OK;
}

STDMETHODIMP CHtmlSite::QueryInterface(REFIID riid,void ** ppvObject)
{
    if(riid == IID_IUnknown || riid == IID_IOleClientSite)
        *ppvObject = (IOleClientSite*)this;
    else if(riid == IID_IOleInPlaceSite) // || riid == IID_IOleInPlaceSiteEx || riid == IID_IOleInPlaceSiteWindowless)
        *ppvObject = (IOleInPlaceSite*)this;
    //else if(riid == IID_IDocHostUIHandler)
    //    *ppvObject = (IDocHostUIHandler*)&this->m_pNikHTMLWin->m_cUIHandler;
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }

    return S_OK;
}

STDMETHODIMP CHtmlSite::GetContainer(LPOLECONTAINER FAR* ppContainer)
{
    // We are a simple object and don't support a container.
    *ppContainer = NULL;

    return E_NOINTERFACE;
}

STDMETHODIMP CHtmlSite::GetWindow(HWND FAR* lphwnd)
{
    *lphwnd = m_pNikHTMLWin->m_hWnd;

    return S_OK;
}

STDMETHODIMP CHtmlSite::OnPosRectChange(LPCRECT lprcPosRect)
{
    IWebBrowser2* iBrowser;
    if(!m_pNikHTMLWin->m_pWebObject->QueryInterface(IID_IWebBrowser2,(void**)&iBrowser))
    {
        IOleInPlaceObject	*inplace;
        if (!iBrowser->QueryInterface( IID_IOleInPlaceObject, (void**)&inplace))
	    {
		    // Give the browser the dimensions of where it can draw.
		    inplace->SetObjectRects( lprcPosRect, lprcPosRect);
		    inplace->Release();
	    }

        iBrowser->Release();
    }

    return S_OK;
}

STDMETHODIMP CHtmlSite::GetWindowContext(
    LPOLEINPLACEFRAME FAR* ppFrame,
    LPOLEINPLACEUIWINDOW FAR* ppDoc,
    LPRECT prcPosRect,
    LPRECT prcClipRect,
    LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    *ppFrame = &m_pNikHTMLWin->m_cFrame;
    *ppDoc = NULL;
    GetClientRect(m_pNikHTMLWin->m_hWnd,prcPosRect);
    GetClientRect(m_pNikHTMLWin->m_hWnd,prcClipRect);

    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = m_pNikHTMLWin->m_hWnd;
    lpFrameInfo->haccel = NULL;
    lpFrameInfo->cAccelEntries = 0;

    return S_OK;
}

///----------------------------------------------------------

WNDPROC         g_wpOrigListProc1;
// Subclass procedure 
LRESULT APIENTRY ListSubclassProc1( HWND hWnd
                                  , UINT uMsg
                                  , WPARAM wParam
                                  , LPARAM lParam) 
{
    switch( uMsg )
    {
    case WM_LBUTTONDOWN:
        SetWindowPos( hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        return CallWindowProc(g_wpOrigListProc1, hWnd, uMsg, wParam, lParam);
    case WM_LBUTTONUP:
        SetFocus(hWnd);
        return CallWindowProc(g_wpOrigListProc1, hWnd, uMsg, wParam, lParam);
    case WM_CLOSE:
        ShowWindow ( hWnd, SW_HIDE );
        break;

    case WM_SIZE:
        {
            CNikHTMLWindow * pNikHTMLWin = (CNikHTMLWindow *)GetWindowLong(hWnd, GWL_USERDATA);
            if(pNikHTMLWin)
            {
                IWebBrowser2* iBrowser;
                pNikHTMLWin->m_pWebObject->QueryInterface(IID_IWebBrowser2,(void**)&iBrowser);
                iBrowser->put_Width(lParam & 0xFFFF);
                iBrowser->put_Height((lParam>>16) & 0xFFFF);
                iBrowser->Release();
            }
        }
        return CallWindowProc(g_wpOrigListProc1, hWnd, uMsg, wParam, lParam);

    default:
        return CallWindowProc(g_wpOrigListProc1, hWnd, uMsg, wParam, lParam); 
    }
 
    return 0;
}

CNikHTMLWindow::CNikHTMLWindow()
{
    m_hWnd                      = NULL;
    m_hInstance                 = NULL;
    m_pWebObject                = NULL;
    m_cSite.m_pNikHTMLWin       = this;
    m_cFrame.m_pNikHTMLWin      = this;
    m_cStorage.m_pNikHTMLWin    = this;
    //m_cUIHandler.m_pNikHTMLWin  = this;
}

CNikHTMLWindow::~CNikHTMLWindow()
{
    UnCreateEmbeddedWebControl();
}

HWND CNikHTMLWindow::CreateHTMLWindow(HINSTANCE hInstance
                                      , HWND hParent
                                      , RECT rWindRect
                                      , _tstring strCaption)
{
    DWORD dwStyle = WS_TABSTOP |
		            WS_VISIBLE |
                    WS_SIZEBOX |
                    WS_CAPTION |
                    WS_SYSMENU |
                    WS_CHILDWINDOW |
                    WS_CLIPSIBLINGS |
		            0;

    m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, WC_LISTVIEW, strCaption.c_str()
                            , dwStyle, rWindRect.left, rWindRect.top
                            , rWindRect.right - rWindRect.left
                            , rWindRect.bottom - rWindRect.top
                            , hParent, (HMENU)ID_LISTVIEW
                            , hInstance, NULL);

	if(!m_hWnd)
		return NULL;

    g_wpOrigListProc1 = (WNDPROC) SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) ListSubclassProc1);
    SetWindowLong(m_hWnd, GWL_USERDATA, (LONG) this);

    SetWindowPos( m_hWnd, HWND_TOP, 0 , 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

    return m_hWnd;
}

HRESULT CNikHTMLWindow::LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream)
{
    HRESULT hr;
    IDispatch* pHtmlDoc = NULL;
    IPersistStreamInit* pPersistStreamInit = NULL;

    // Retrieve the document object.
    hr = pWebBrowser->get_Document( &pHtmlDoc );
    if ( SUCCEEDED(hr) )
    {
        //IHTMLDocument2 * pHtmlDoc1 = (IHTMLDocument2 *)pHtmlDoc;

        // Query for IPersistStreamInit.
        hr = pHtmlDoc->QueryInterface( IID_IPersistStreamInit,  (void**)&pPersistStreamInit );
        if ( SUCCEEDED(hr) )
        {
            // Initialize the document.
            hr = pPersistStreamInit->InitNew();
            if ( SUCCEEDED(hr) )
            {
                // Load the contents of the stream.
                hr = pPersistStreamInit->Load( pStream );
            }
            pPersistStreamInit->Release();
        }

        pHtmlDoc->Release();
    }

    return hr;
}

_tstring CNikHTMLWindow::CreatePEDosHrHtmlPageStr()
{
    _tstring strHtmlText;
        
    strHtmlText = STR_HTML_START;
    strHtmlText += STR_HTML_HEAD;
    strHtmlText += STR_HTML_BODY_S;

    Fill_IMAGE_DOS_HEADER( strHtmlText );
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_BODY_E;
    strHtmlText += STR_HTML_END;

    return strHtmlText;
}

_tstring CNikHTMLWindow::CreatePENTHtmlPageString()
{
    _tstring strHtmlText;
        
    strHtmlText = STR_HTML_START;
    strHtmlText += STR_HTML_HEAD;
    strHtmlText += STR_HTML_BODY_S;

    Fill_IMAGE_NT_HEADERS( strHtmlText );
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_BODY_E;
    strHtmlText += STR_HTML_END;

    return strHtmlText;
}

_tstring CNikHTMLWindow::CreateSectionHtmlPageString()
{
    _tstring strHtmlText;
        
    strHtmlText = STR_HTML_START;
    strHtmlText += STR_HTML_HEAD;
    strHtmlText += STR_HTML_BODY_S;

    Fill_IMAGE_NT_SECTION_DETAILS( strHtmlText );
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_BODY_E;
    strHtmlText += STR_HTML_END;

    return strHtmlText;
}

_tstring CNikHTMLWindow::CreateDataHtmlPageString()
{
    _tstring strHtmlText;
        
    strHtmlText = STR_HTML_START;
    strHtmlText += STR_HTML_HEAD;
    strHtmlText += STR_HTML_BODY_S;

    Fill_IMAGE_NT_DATA_DIRECTORY(strHtmlText );
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_BODY_E;
    strHtmlText += STR_HTML_END;

    return strHtmlText;
}

_tstring CNikHTMLWindow::CreateImportHtmlPageString()
{
    _tstring strHtmlText;
        
    strHtmlText = STR_HTML_START;
    strHtmlText += STR_HTML_HEAD;
    strHtmlText += STR_HTML_BODY_S;

    Fill_IMAGE_NT_IMPORT_TABLE(strHtmlText );
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_BODY_E;
    strHtmlText += STR_HTML_END;

    return strHtmlText;
}

_tstring CNikHTMLWindow::CreateExportHtmlPageString()
{
    _tstring strHtmlText;
        
    strHtmlText = STR_HTML_START;
    strHtmlText += STR_HTML_HEAD;
    strHtmlText += STR_HTML_BODY_S;

    Fill_IMAGE_NT_EXPORT_TABLE(strHtmlText );
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_BODY_E;
    strHtmlText += STR_HTML_END;

    return strHtmlText;
}


HRESULT CNikHTMLWindow::CreateEmbeddedWebControl(void * pNikPEViewer)
{
    HRESULT hr;

    m_pNikPEViewer = pNikPEViewer;

    TCHAR chPathBuf[1024];
    int ilen = GetTempPath( 1023, chPathBuf);
    if(ilen <= 0)
        return 0;

    _tstring strShortName, strTempPath = chPathBuf;
    if(_T('\\') != chPathBuf[ilen-1])
        strTempPath += _T("\\");

    strTempPath += _T("NikViewer\\");

    CreateDirectory(strTempPath.data(), NULL);

    strShortName = ((CNikPEViewer *)m_pNikPEViewer)->GetModuleShortName();
    int iPos = strShortName.find_last_of(_T('.'),strShortName.length());
    if(iPos > 0)
        strTempPath += strShortName.substr(0, iPos);
    else
    {
        strTempPath += strShortName;
        strTempPath += _T("_nik");
    }

    CreateDirectory(strTempPath.data(), NULL);

    strTempPath += _T("\\nik_pedata.html");
    ((CNikPEViewer *)m_pNikPEViewer)->SavePEFileRecords(strTempPath);

    hr = OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, &m_cSite, &m_cStorage, (void**)&m_pWebObject);

    hr = m_pWebObject->SetHostNames(_T("Nik PE HTML view"),_T("Nik PE HTML view"));

    hr = OleSetContainedObject(m_pWebObject, TRUE);

    RECT rect;
    GetClientRect(m_hWnd,&rect);
    hr = m_pWebObject->DoVerb(OLEIVERB_SHOW,NULL,&m_cSite,-1,m_hWnd,&rect);

    IWebBrowser2* iBrowser;
    hr = m_pWebObject->QueryInterface(IID_IWebBrowser2,(void**)&iBrowser);

    VARIANT vURL;
    vURL.vt = VT_BSTR;
    //vURL.bstrVal = SysAllocString(L"about:blank");
    vURL.bstrVal = SysAllocString(strTempPath.data());
    VARIANT ve1, ve2, ve3, ve4;
    ve1.vt = VT_EMPTY;
    ve2.vt = VT_EMPTY;
    ve3.vt = VT_EMPTY;
    ve4.vt = VT_EMPTY;

    iBrowser->put_Left(0);
    iBrowser->put_Top(0);
    iBrowser->put_Width(rect.right);
    iBrowser->put_Height(rect.bottom);
    iBrowser->put_Visible(0);

    hr = iBrowser->Navigate2(&vURL, &ve1, &ve2, &ve3, &ve4);

    hr = VariantClear(&vURL);

    /*if ( SUCCEEDED(hr) )
    {
        IStream* pStream = NULL;
        HGLOBAL hHTMLText;
        
        _tstring strHtmlText;

        //strHtmlText = CreatePEHtmlPageString();

        size_t cchLength = (strHtmlText.length()+2) * sizeof(TCHAR);
        hHTMLText = GlobalAlloc( GPTR, cchLength );
        if ( hHTMLText )
        {
            _tcscpy_s((TCHAR*)hHTMLText, strHtmlText.length() + 1, strHtmlText.data());
            
            hr = CreateStreamOnHGlobal( hHTMLText, 0, &pStream );
            if ( SUCCEEDED(hr) )
            {
               // Call the helper function to load the browser from the stream.
               hr = LoadWebBrowserFromStream( iBrowser, pStream  );
               pStream->Release();
            }
            GlobalFree( hHTMLText );
        }

    }*/

    iBrowser->Release();

    return hr;
}

void CNikHTMLWindow::UnCreateEmbeddedWebControl(void)
{
    if(m_pWebObject)
    {
        m_pWebObject->Close(OLECLOSE_NOSAVE);
        m_pWebObject->Release();
    }
}

void CNikHTMLWindow::Fill_IMAGE_NT_HEADERS(_tstring &strHtmlText)
{
    int nExtended = 0;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)m_pNikPEViewer;

    const IMAGE_NT_HEADERS *pNTHeaders = &pNikPEViewer->GetImageNtHeaders();
	PIMAGE_NT_HEADERS64 pNTHeaders64 = (const PIMAGE_NT_HEADERS64)pNTHeaders;

	if(pNTHeaders->OptionalHeader.Magic == 0x020b)
		nExtended = 1;

    TCHAR chBuf[1024];
    chBuf[0] = 0;

    strHtmlText += STR_HTML_TABLE_S;
    
    SetTable1ColValue( strHtmlText, NT_HEADERS_1);

    _stprintf_s(chBuf, 1023, _T("0x%08X \'%C%C\'"), pNTHeaders->Signature
        , (char)(pNTHeaders->Signature&0xFF), (char)((pNTHeaders->Signature>>8)&0xFF));
    SetTable2ColStr( strHtmlText, NT_HEADERS_SIGNATURE, chBuf);

    strHtmlText += STR_HTML_TABLE_E;
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_TABLE_S;

    SetTable1ColValue( strHtmlText, NT_HEADERS_FH_2);

    
    _tstring strChar;
    CNikPEViewer::GetMachineName(pNTHeaders->FileHeader.Machine, 1023, chBuf);
    SetTable2ColStr( strHtmlText, NT_HEADERS_FH_MACHINE, chBuf);
    
    CNikPEViewer::GetStrDateTimeStamp(pNTHeaders->FileHeader.TimeDateStamp, 1023, chBuf);
    SetTable2ColStr( strHtmlText, NT_HEADERS_FH_TIMEDATESTAMP, chBuf);
    SetTable2ColValue( strHtmlText, NT_HEADERS_FH_CHARACTERISTIC, pNTHeaders->FileHeader.Characteristics);
    SetTable2ColValue( strHtmlText, NT_HEADERS_FH_NUOFSECTIONS, pNTHeaders->FileHeader.NumberOfSections);
    SetTable2ColValue( strHtmlText, NT_HEADERS_FH_SIZEOFOPHEADER, pNTHeaders->FileHeader.SizeOfOptionalHeader);
    SetTable2ColValue( strHtmlText, NT_HEADERS_FH_PTSYMBOLTABLE, pNTHeaders->FileHeader.PointerToSymbolTable);
    SetTable2ColValue( strHtmlText, NT_HEADERS_FH_NUMBEROFSYMBOL, pNTHeaders->FileHeader.NumberOfSymbols);

    CNikPEViewer::GetNTCharacteristics(pNTHeaders->FileHeader.Characteristics, strChar);
    _stprintf_s(chBuf, 1023, _T("%s 0x%X"), strChar.data(), pNTHeaders->FileHeader.Characteristics);
    SetTable2ColStr( strHtmlText, NT_HEADERS_FH_CHARACTERISTIC, chBuf);

    strHtmlText += STR_HTML_TABLE_E;
    strHtmlText += _T("<br/>");

    strHtmlText += STR_HTML_TABLE_S;
    SetTable1ColValue( strHtmlText, NT_HEADERS_OP_2);

    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MAGIC, pNTHeaders->OptionalHeader.Magic);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MINORLINKERVERSION, pNTHeaders->OptionalHeader.MinorLinkerVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MINORIMAGEVERSION, pNTHeaders->OptionalHeader.MinorImageVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFCODE, pNTHeaders->OptionalHeader.SizeOfCode);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFINITDATA, pNTHeaders->OptionalHeader.SizeOfInitializedData);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFUNINITDATA, pNTHeaders->OptionalHeader.SizeOfUninitializedData);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_ADDOFENTRYPOINT, pNTHeaders->OptionalHeader.AddressOfEntryPoint);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_BASEOFCODE, pNTHeaders->OptionalHeader.BaseOfCode);
    if(!nExtended)
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_BASEOFDATA, pNTHeaders->OptionalHeader.BaseOfData);
    if(nExtended)
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_IMAGEBASE, pNTHeaders->OptionalHeader.ImageBase);
    else
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_IMAGEBASE, (DWORD)pNTHeaders64->OptionalHeader.ImageBase);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SECTIONALIGNMENT, pNTHeaders->OptionalHeader.SectionAlignment);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_FILEALIGNMENT, pNTHeaders->OptionalHeader.FileAlignment);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MAJOROSVERSION, pNTHeaders->OptionalHeader.MajorOperatingSystemVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MINOROSVERSION, pNTHeaders->OptionalHeader.MinorOperatingSystemVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MAJORLINKERVERSION, pNTHeaders->OptionalHeader.MajorLinkerVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MINORIMAGEVERSION, pNTHeaders->OptionalHeader.MinorImageVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MAJORSUBSYSVERSION, pNTHeaders->OptionalHeader.MajorSubsystemVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_MINORSUBSYSVERSION, pNTHeaders->OptionalHeader.MinorSubsystemVersion);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_WIN32VERSIONVALUE, pNTHeaders->OptionalHeader.Win32VersionValue);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFIMAGE, pNTHeaders->OptionalHeader.SizeOfImage);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFHEADERS, pNTHeaders->OptionalHeader.SizeOfHeaders);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_CHECKSUM, pNTHeaders->OptionalHeader.CheckSum);
    CNikPEViewer::GetNTOptionalSubsystem(pNTHeaders->OptionalHeader.Subsystem, 1023, chBuf);
    SetTable2ColStr( strHtmlText, NT_HEADERS_OP_SUBSYSTEM, chBuf);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_DLLCHARACTERISTIC, pNTHeaders->OptionalHeader.DllCharacteristics);
    if(nExtended)
    {
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFSTACKRESERVE, (DWORD)pNTHeaders64->OptionalHeader.SizeOfStackReserve);
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFSTACKCOMMIT, (DWORD)pNTHeaders64->OptionalHeader.SizeOfStackCommit);
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFHEAPRESERVE, (DWORD)pNTHeaders64->OptionalHeader.SizeOfHeapReserve);
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFHEAPCOMMIT, (DWORD)pNTHeaders64->OptionalHeader.SizeOfHeapCommit);
    }
    else
    {
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFSTACKRESERVE, pNTHeaders->OptionalHeader.SizeOfStackReserve);
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFSTACKCOMMIT, pNTHeaders->OptionalHeader.SizeOfStackCommit);
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFHEAPRESERVE, pNTHeaders->OptionalHeader.SizeOfHeapReserve);
        SetTable2ColValue( strHtmlText, NT_HEADERS_OP_SIZEOFHEAPCOMMIT, pNTHeaders->OptionalHeader.SizeOfHeapCommit);
    }
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_LOADERFLAGS, pNTHeaders->OptionalHeader.LoaderFlags);
    SetTable2ColValue( strHtmlText, NT_HEADERS_OP_NUOFRVAANDSIZES, pNTHeaders->OptionalHeader.NumberOfRvaAndSizes);

    strHtmlText += STR_HTML_TABLE_E;
}

void CNikHTMLWindow::Fill_IMAGE_DOS_HEADER(_tstring &strHtmlText)
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)m_pNikPEViewer;
    const IMAGE_DOS_HEADER * pDosHeader = &pNikPEViewer->GetImageDosHeader();

    TCHAR chBuf[1024];
    chBuf[0] = 0;

    strHtmlText += STR_HTML_TABLE_S;

    SetTable1ColValue( strHtmlText, DOS_HEADER_1);

    //SetTable2ColStr( strHtmlText, STR_DESCRIPTION, STR_HEX_VALUES);
    _stprintf_s(chBuf, 1023, _T("0x%X \'%C%C\'"), pDosHeader->e_magic
        , (char)(pDosHeader->e_magic&0xFF), (char)((pDosHeader->e_magic>>8)&0xFF));
    SetTable2ColStr( strHtmlText, DOS_HEADER_E_MAGIC, chBuf);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_CBLP, pDosHeader->e_cblp);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_CP, pDosHeader->e_cp);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_CRLC, pDosHeader->e_crlc);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_CPARHDR, pDosHeader->e_cparhdr);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_MINALLOC, pDosHeader->e_minalloc);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_MAXALLOC, pDosHeader->e_maxalloc);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_SS, pDosHeader->e_ss);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_SP, pDosHeader->e_sp);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_CSUM, pDosHeader->e_csum);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_IP, pDosHeader->e_ip);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_CS, pDosHeader->e_cs);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_LFARLC, pDosHeader->e_lfarlc);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_OVNO, pDosHeader->e_ovno);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_OEMID, pDosHeader->e_oemid);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_OEMINFO, pDosHeader->e_oeminfo);
    SetTable2ColValue( strHtmlText, DOS_HEADER_E_LFANEW, pDosHeader->e_lfanew);

    strHtmlText += STR_HTML_TABLE_E;
}

void CNikHTMLWindow::SetTable1ColValue(_tstring &strHtmlText, TCHAR* strRow1)
{
    strHtmlText += STR_HTML_TB_TR_S;
    strHtmlText += STR_HTML_TB_TD_CENTER;
        strHtmlText += _T("<b>");
        strHtmlText += strRow1;
        strHtmlText += _T("</b>");
    strHtmlText += STR_HTML_TB_TD_E;
    strHtmlText += STR_HTML_TB_TR_E;
}

void CNikHTMLWindow::SetTable2ColValue(_tstring &strHtmlText, TCHAR* strRow1, DWORD nValue)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;

    strHtmlText += STR_HTML_TB_TR_S;
    strHtmlText += STR_HTML_TB_TD_S;
        strHtmlText += strRow1;
    strHtmlText += STR_HTML_TB_TD_E;
    strHtmlText += STR_HTML_TB_TD_S;
    _stprintf_s(chBuf, 1023, STR_PRINTF_X, nValue);
        strHtmlText += chBuf;
    strHtmlText += STR_HTML_TB_TD_E;
    strHtmlText += STR_HTML_TB_TR_E;
}

void CNikHTMLWindow::SetTable2ColStr(_tstring &strHtmlText, TCHAR* strRow1, TCHAR* strValue)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;

    strHtmlText += STR_HTML_TB_TR_S;
    strHtmlText += STR_HTML_TB_TD_S;
        strHtmlText += strRow1;
    strHtmlText += STR_HTML_TB_TD_E;
    strHtmlText += STR_HTML_TB_TD_S;
    _stprintf_s(chBuf, 1023, STR_PRINTF_s, strValue);
        strHtmlText += chBuf;
    strHtmlText += STR_HTML_TB_TD_E;
    strHtmlText += STR_HTML_TB_TR_E;
}

int CNikHTMLWindow::Fill_IMAGE_NT_DATA_DIRECTORY(_tstring &strHtmlText)
{
    TCHAR chBuf[1024],chBuf1[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;
    chBuf1[0] = 0;
    chBuf1[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)m_pNikPEViewer;
    if(pNikPEViewer)
    {
        strHtmlText += STR_HTML_TABLE_S;

        SetTable1ColValue( strHtmlText, STR_DATA_DIRECTORY);

        const IMAGE_NT_HEADERS *pNTHeaders = &pNikPEViewer->GetImageNtHeaders();

	    PIMAGE_NT_HEADERS64 pNTHeaders64 = (const PIMAGE_NT_HEADERS64)pNTHeaders;

	    if(pNTHeaders->OptionalHeader.Magic == 0x020b)
	    {
            for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		    {
			    if(pNTHeaders64->OptionalHeader.DataDirectory[i].Size)
			    {
                    CNikPEViewer::GetDirectoryEnteries(i, 1023, chBuf);
                    SetTable2ColStr( strHtmlText, chBuf, _T("&nbsp;"));

				    _stprintf_s(chBuf, 1023, _T("0x%08X"), pNTHeaders64->OptionalHeader.DataDirectory[i].VirtualAddress);
				    SetTable2ColStr( strHtmlText, STR_VIRTUAL_ADDRESS, chBuf);

                    _stprintf_s(chBuf1, 1023, STR_DATADIRECTORY_SIZE, i);
				    _stprintf_s(chBuf, 1023, _T("%d Bytes"), pNTHeaders64->OptionalHeader.DataDirectory[i].Size);
				    SetTable2ColStr( strHtmlText, chBuf1, chBuf);

			    }
		    }
	    }
	    else
	    {
		    for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		    {
			    if(pNTHeaders->OptionalHeader.DataDirectory[i].Size)
			    {
                    CNikPEViewer::GetDirectoryEnteries(i, 1023, chBuf);
                    SetTable2ColStr( strHtmlText, chBuf, _T("&nbsp;"));

				    _stprintf_s(chBuf, 1023, _T("0x%08X"), pNTHeaders->OptionalHeader.DataDirectory[i].VirtualAddress);
				    SetTable2ColStr( strHtmlText, STR_VIRTUAL_ADDRESS, chBuf);

				    _stprintf_s(chBuf1, 1023, STR_DATADIRECTORY_SIZE, i);
				    _stprintf_s(chBuf, 1023, _T("%d Bytes"), pNTHeaders->OptionalHeader.DataDirectory[i].Size);
				    SetTable2ColStr( strHtmlText, chBuf1, chBuf);
			    }
		    }
	    }
        strHtmlText += STR_HTML_TABLE_E;
    }

    return TRUE;
}

int CNikHTMLWindow::Fill_IMAGE_NT_SECTION_DETAILS(_tstring &strHtmlText)
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;
    chBuf[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)m_pNikPEViewer;
    if(pNikPEViewer)
    {
        strHtmlText += STR_HTML_TABLE_S;

        SetTable1ColValue( strHtmlText, SH_SECTIONS_LIST);

        for(int i=(int)pNikPEViewer->GetImageSectionHeaderList().size()-1; i>=0; i--)
        {
            _stprintf_s(chBuf, 1023, _T("%S"), pNikPEViewer->GetImageSectionHeaderList().at(i).Name);
            //SetTable2ColStr( strHtmlText, SH_SECTIONS_NAME, chBuf);
            SetTable1ColValue( strHtmlText, chBuf);

            SetTable2ColValue( strHtmlText, SH_PHYSICAL_ADDRESS, pNikPEViewer->GetImageSectionHeaderList().at(i).Misc.PhysicalAddress);
            SetTable2ColValue( strHtmlText, SH_VIRTUAL_ADDRESS, pNikPEViewer->GetImageSectionHeaderList().at(i).VirtualAddress);
            SetTable2ColValue( strHtmlText, SH_SIZEOFRAWDATA, pNikPEViewer->GetImageSectionHeaderList().at(i).SizeOfRawData);
            SetTable2ColValue( strHtmlText, SH_POINTERTORAWDATA, pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToRawData);
            SetTable2ColValue( strHtmlText, SH_POINTERTORELOCATIONS, pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToRelocations);
            SetTable2ColValue( strHtmlText, SH_POINTERTOLINENUMBERS, pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToLinenumbers);
            SetTable2ColValue( strHtmlText, SH_NUMBEROFRELOCATIONS, pNikPEViewer->GetImageSectionHeaderList().at(i).NumberOfRelocations);
            SetTable2ColValue( strHtmlText, SH_NUMBEROFLINENUMBERS, pNikPEViewer->GetImageSectionHeaderList().at(i).NumberOfLinenumbers);
            SetTable2ColValue( strHtmlText, NT_HEADERS_FH_CHARACTERISTIC, pNikPEViewer->GetImageSectionHeaderList().at(i).Characteristics);

            _tstring strChar;
            CNikPEViewer::GetNTSectionCharacteristics(pNikPEViewer->GetImageSectionHeaderList().at(i).Characteristics, strChar);
        }
        strHtmlText += STR_HTML_TABLE_E;
    }

    return TRUE;
}

int CNikHTMLWindow::Fill_IMAGE_NT_IMPORT_TABLE(_tstring &strHtmlText)
{
    TCHAR chBuf[1024], chBuf1[1024];
    chBuf[0] = 0;
    chBuf1[0] = 0;
    chBuf[1023] = 0;
    chBuf1[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)m_pNikPEViewer;
    if(pNikPEViewer)
    {
        strHtmlText += STR_HTML_TABLE_S;

        SetTable1ColValue( strHtmlText, STR_IMPORT_FUNCTIONS);

        const vector<IMPORTDLLLIST> &vImportFunctionList = pNikPEViewer->GetImportFunctionList();
        for(int i=0;i<(int)vImportFunctionList.size();i++)
        {
            _stprintf_s(chBuf, 1023, STR_PRINTF_s, vImportFunctionList.at(i).m_strDllName.data());
            _stprintf_s(chBuf1, 1023, _T("0x%08X"), vImportFunctionList.at(i).m_stImageDescriptor.FirstThunk);
            SetTable2ColStr( strHtmlText, chBuf, chBuf1);

            for(int j=0; j<(int)vImportFunctionList.at(i).m_vImportFunctionList.size(); j++)
            {
                _stprintf_s(chBuf, 1023, _T("%s"), vImportFunctionList.at(i).m_vImportFunctionList.at(j).m_strFunctionName.data());
                _stprintf_s(chBuf1, 1023, _T("%d (Ordinal No)"), vImportFunctionList.at(i).m_vImportFunctionList.at(j).m_nOrdinalNo);
                if(chBuf[0])
                    SetTable2ColStr( strHtmlText, chBuf, chBuf1);
                else
                    SetTable2ColStr( strHtmlText, _T("&nbsp;"), chBuf1);
            }
        }

        strHtmlText += STR_HTML_TABLE_E;
    }

    return TRUE;
}

int CNikHTMLWindow::Fill_IMAGE_NT_EXPORT_TABLE(_tstring &strHtmlText)
{
    TCHAR chBuf[1024], chBuf1[1024];
    chBuf[0] = 0;
    chBuf1[0] = 0;
    chBuf[1023] = 0;
    chBuf1[1023] = 0;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)m_pNikPEViewer;
    if(pNikPEViewer)
    {
        strHtmlText += STR_HTML_TABLE_S;

        strHtmlText += STR_HTML_TB_TR_S;
        strHtmlText += _T("<td width=\"400\" ColSpan=\"3\" style=\"text-align: center\">");
        strHtmlText += _T("<b>");
        strHtmlText += STR_EXPORT_FUNCTIONS;
        strHtmlText += _T("</b>");
        strHtmlText += STR_HTML_TB_TD_E;
        strHtmlText += STR_HTML_TB_TR_E;

        strHtmlText += STR_HTML_TB_TR_S;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += STR_FUNCTION_NAME;
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += STR_ENTRY_POINT;
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += STR_ORDIANL_NO;
            strHtmlText += STR_HTML_TB_TD_E;
        strHtmlText += STR_HTML_TB_TR_E;

        const vector<EXPORTFUNCTIONLIST> &vExportFunctionList = pNikPEViewer->GetExportFunctionList();
        for(int i=0;i<(int)vExportFunctionList.size();i++)
        {
            strHtmlText += STR_HTML_TB_TR_S;
                strHtmlText += STR_HTML_TB_TD_S;
                    _stprintf_s(chBuf, 1023, STR_PRINTF_s, vExportFunctionList.at(i).m_strFunctionName.data());
                    strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;
                strHtmlText += STR_HTML_TB_TD_S;
                    _stprintf_s(chBuf, 1023, _T("0x%08X"), vExportFunctionList.at(i).m_dwEntryPoint);
                    strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;
                strHtmlText += STR_HTML_TB_TD_S;
                    _stprintf_s(chBuf, 1023, _T("%d"), vExportFunctionList.at(i).m_dwOrdianlNo);
                    strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TR_E;
        }

        strHtmlText += STR_HTML_TABLE_E;
    }

    return TRUE;
}

void CNikHTMLWindow::SetTableColumnVal(TCHAR * pstrVal
                                       , _tstring &strHtmlText
                                       , TCHAR * pstrHtmlFile)
{
    strHtmlText += STR_HTML_TB_TR_S;
    strHtmlText += STR_HTML_TB_TD_S;
    if(pstrHtmlFile[0])
    {
        strHtmlText += _T("<a href=\"");
        strHtmlText += pstrHtmlFile;
        strHtmlText += _T("\" target=\"main\">");
        strHtmlText += (TCHAR *)pstrVal;
        strHtmlText += _T("</a>");
    }
    else
        strHtmlText += (TCHAR *)pstrVal;
    strHtmlText += STR_HTML_TB_TD_E;
    strHtmlText += STR_HTML_TB_TR_E;
}

_tstring CNikHTMLWindow::SetTreeItemStr(void * pNikView)
{
    _tstring strHtmlText;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(pNikPEViewer)
    {
	    //text and images are done on a callback basis
        TCHAR chName[1024];
        chName[1024-1] = 0;

        strHtmlText = STR_HTML_START;
        strHtmlText += STR_HTML_HEAD;
        strHtmlText += STR_HTML_BODY_S_1;

        strHtmlText += STR_HTML_TABLE_S;

        //SetTableColumnVal( chName, strHtmlText, _T("") );
        SetTable2ColStr(strHtmlText, _T("File:")
            , (TCHAR *)pNikPEViewer->GetModuleShortName().c_str());
        DWORD dwFileSize = pNikPEViewer->GetModuleFileSize();

        if((dwFileSize/1000000000))
            _stprintf_s(chName,  1024-1, _T("%0.2f GB"), ((float)dwFileSize/1000000000.0));
        else if((dwFileSize/1000000))
            _stprintf_s(chName,  1024-1, _T("%0.2f MB"), ((float)dwFileSize/1000000.0));
        else if((dwFileSize/1000))
            _stprintf_s(chName,  1024-1, _T("%0.2f KB"), ((float)dwFileSize/1000.0));
        else
            _stprintf_s(chName,  1024-1, _T("%d bytes"), dwFileSize);

        SetTable2ColStr(strHtmlText, _T("Size:"), chName);
        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += _T("<br/>");

        strHtmlText += STR_HTML_TABLE_S;
        SetTableColumnVal( DOS_HEADER, strHtmlText, _T("pedoshr.html") );
        SetTableColumnVal( NT_HEADERS, strHtmlText, _T("penthr.html") );
        SetTableColumnVal( SH_SECTIONS_LIST, strHtmlText, _T("pesect.html") );

        /*TCHAR chTemp[10];
        for(int i=0; i<(int)pNikPEViewer->GetImageSectionHeaderList().size(); i++)
        {
            memset(chTemp, 0, sizeof(chTemp));
            for(int j=0; j<8; j++)
            {
                chTemp[j] = pNikPEViewer->GetImageSectionHeaderList().at(i).Name[j];
            }

            if(0 == chTemp[0])
            {
                //strHtmlText += _T("\r\n---->");
                SetTableColumnVal( UNDEFINED_SECTION, strHtmlText );
            }
            else
            {
                //strHtmlText += _T("\r\n---->");
                SetTableColumnVal( chTemp, strHtmlText );
            }
        }*/

        SetTableColumnVal( DATA_DIRECTORIES, strHtmlText, _T("pedata.html") );

		//PIMAGE_NT_HEADERS pNTHeaders64 = (PIMAGE_NT_HEADERS64)&pNikPEViewer->GetImageNtHeaders();
		//if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.Magic == 0x020b)
		{
			for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
			{
				//if(pNTHeaders64->OptionalHeader.DataDirectory[i].Size)
				{
					if(0 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        if(pNikPEViewer->GetExportFunctionList().size())
                            SetTableColumnVal( EXPORT_TABLE, strHtmlText, _T("peexport.html") );
                    }
					else if(1 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        if(pNikPEViewer->GetImportFunctionList().size())
                            SetTableColumnVal( IMPORT_TABLE, strHtmlText, _T("peimport.html") );
                    }
					else if(2 == i)
					{
						RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();
						if(pResourceList)
						{
                            //SetTableColumnVal( RESOURCE_TABLE, strHtmlText, _T("") );

							for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
                            {
                                if(!_tcscmp(_T("VERSION"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("Version"), strHtmlText, _T("peversion.html") );
                                else if(!_tcscmp(_T("ACCELERATORS"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("Accelerators"), strHtmlText, _T("peaccel.html") );
                                else if(!_tcscmp(_T("STRING"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("String"), strHtmlText, _T("pestring.html") );
                                else if(!_tcscmp(_T("MENU"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("Menu"), strHtmlText, _T("pemenu.html") );
                                else if(!_tcscmp(_T("BITMAP"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("Bitmap"), strHtmlText, _T("pebitmap.html") );
                                else if(!_tcscmp(_T("ICON"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("Icon"), strHtmlText, _T("peicon.html") );
                                else if(!_tcscmp(_T("CURSOR"), pResourceList->m_vDirList.at(i)->m_chName))
                                    SetTableColumnVal( _T("Cursor"), strHtmlText, _T("pecursor.html") );
                                //else
                                //    SetTableColumnVal( pResourceList->m_vDirList.at(i)->m_chName, strHtmlText, _T("") );
                            }
						}
					}
					else if(3 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( EXCEPTION_TABLE, strHtmlText );
                    }
					else if(4 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( SECURITY_TABLE, strHtmlText );
                    }
					else if(5 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( BASERELOC_TABLE, strHtmlText );
                    }
					else if(6 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( DEBUG_TABLE, strHtmlText );
                    }
					else if(7 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( COPYRIGHT_TABLE, strHtmlText, _T("") );
                    }
					else if(8 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( GLOBALPTR_TABLE, strHtmlText );
                    }
					else if(9 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( TLS_TABLE, strHtmlText );
                    }
					else if(10 == i)
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( LOAD_CONFIG_TABLE, strHtmlText );
                    }
					else
                    {
                        //strHtmlText += _T("\r\n---->");
                        //SetTableColumnVal( UNDEFINED_TABLE, strHtmlText );
                    }
				}
			}
		}

        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += STR_HTML_BODY_E;
        strHtmlText += STR_HTML_END;
    }

	return strHtmlText;
}

extern TCHAR * g_szTitleString[];

_tstring CNikHTMLWindow::GetHtmlStrVersion(void * pNikView)
{
    _tstring strHtmlText, strTemp;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(!pNikPEViewer)
        return strHtmlText;
    
    RESOURCELIST * pRes = NULL;
    RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();

    for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
    {
        if(!_tcscmp(_T("VERSION"), pResourceList->m_vDirList.at(i)->m_chName))
        {
            pRes = pResourceList->m_vDirList.at(i);
            break;
        }
    }

    if(pRes && pRes->m_vDirList.size())
	{
        pRes = pRes->m_vDirList.at(0);

        strHtmlText = STR_HTML_START;
        strHtmlText += STR_HTML_HEAD;
        strHtmlText += STR_HTML_BODY_S_1;

        strHtmlText += STR_HTML_TABLE_S;
        SetTable1ColValue( strHtmlText, _T("Version Info"));
        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += _T("<br/>");

        strHtmlText += STR_HTML_TABLE_S;

        //SetTable2ColStr(strHtmlText, g_szTitleString[5], g_szTitleString[6]);

		{
            BYTE * pResBuffer = pNikPEViewer->GetPEBuffer();
			WORD * pdwLen = (WORD *)&pResBuffer[pRes->m_nOffsetToData+2];
			DWORD * pdwOffset = (DWORD *)&pResBuffer[pRes->m_nOffsetToData];
			VS_FIXEDFILEINFO * pstVerFixInfo = NULL;
			for(int i=0;i<pdwLen[0]/4;i++)
			{
				if(0xFEEF04BD == pdwOffset[i])
				{
					pstVerFixInfo = (VS_FIXEDFILEINFO *)&pdwOffset[i];
					pdwOffset += i;
					break;
				}
			}

			if(pstVerFixInfo)
			{
				TCHAR chBuf[1024];

				chBuf[0] = 0;
                _stprintf_s(chBuf,1023,_T("%d.%d.%d.%d"),pstVerFixInfo->dwFileVersionMS>>16
														,pstVerFixInfo->dwFileVersionMS&0xFFFF
														,pstVerFixInfo->dwFileVersionLS>>16
														,pstVerFixInfo->dwFileVersionLS&0xFFFF);

                SetTable2ColStr(strHtmlText, _T("File Version"), chBuf);

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("%d.%d.%d.%d"),pstVerFixInfo->dwProductVersionMS>>16
														,pstVerFixInfo->dwProductVersionMS&0xFFFF
														,pstVerFixInfo->dwProductVersionLS>>16
														,pstVerFixInfo->dwProductVersionLS&0xFFFF);

                SetTable2ColStr(strHtmlText, _T("Product Version"), chBuf);

				chBuf[0] = 0;
                _stprintf_s(chBuf,1023,pNikPEViewer->GetVersionOSName(pstVerFixInfo->dwFileOS).data());

                SetTable2ColStr(strHtmlText, _T("File OS"), chBuf);

				chBuf[0] = 0;
                _stprintf_s(chBuf,1023,pNikPEViewer->GetVerResourceType(pstVerFixInfo->dwFileType).data());

                SetTable2ColStr(strHtmlText, _T("File type"), chBuf);

				int nSize = (DWORD)pstVerFixInfo - (DWORD)(&pResBuffer[pRes->m_nOffsetToData]);

				nSize = pRes->m_nSize - sizeof(VS_FIXEDFILEINFO) - nSize;
				pdwOffset += (sizeof(VS_FIXEDFILEINFO)/4);

				RESOURCESTRING * pResString = (RESOURCESTRING *)(pstVerFixInfo+1);
				pResString = pResString;
				while(pResString->wLength 
					&& (nSize > 0))
				{
					if(pResString->wType)
					{
						chBuf[0] = 0;
						_stprintf_s(chBuf,1023,_T("%s"),(TCHAR *)(pResString+1));
						int nLen = (_tcslen(chBuf) * 2) + sizeof(RESOURCESTRING);

						if(pResString->wValueLength)
						{
							if((TCHAR *)(pdwOffset+((nLen+3)/4))[0])
							{
								strTemp = chBuf; // sends an LVN_GETDISP message.

                                chBuf[0] = 0;
								_stprintf_s(chBuf,1023,_T("%s"),(TCHAR *)(pdwOffset+((nLen+3)/4)));
                                SetTable2ColStr(strHtmlText, (TCHAR *)strTemp.data(), chBuf);
							}

							pdwOffset += ((pResString->wLength+3)/4);
							nSize -= ((pResString->wLength+3)/4);
						}
						else
						{
							pdwOffset += ((nLen+3)/4);
							nSize -= ((nLen+3)/4);

							int nTemp = pNikPEViewer->GetHexValues(chBuf);
							if(nTemp 
								&& (0 == pResString->wValueLength))
							{
								chBuf[0] = 0;
                                _stprintf_s(chBuf,1023,pNikPEViewer->GetVerResLanguage(nTemp).data());

								if(chBuf[0])
								{
                                    SetTable2ColStr(strHtmlText, _T("Language block"), chBuf);
								}
							}
						}
					}
					pResString = (RESOURCESTRING *)(pdwOffset);
				}
			}
		}

        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += STR_HTML_BODY_E;
        strHtmlText += STR_HTML_END;
	}

	return strHtmlText;
}

_tstring CNikHTMLWindow::GetHtmlStrAccelarator(void * pNikView)
{
    _tstring strHtmlText, strTemp;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(!pNikPEViewer)
        return strHtmlText;
    
    RESOURCELIST * pRes = NULL;
    RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();

    for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
    {
        if(!_tcscmp(_T("ACCELERATORS"), pResourceList->m_vDirList.at(i)->m_chName))
        {
            pRes = pResourceList->m_vDirList.at(i);
            break;
        }
    }

    if(pRes && pRes->m_vDirList.size())
	{
        pRes = pRes->m_vDirList.at(0);

        strHtmlText = STR_HTML_START;
        strHtmlText += STR_HTML_HEAD;
        strHtmlText += STR_HTML_BODY_S_1;

        strHtmlText += STR_HTML_TABLE_S;
        SetTable1ColValue( strHtmlText, _T("Accelarator table"));
        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += _T("<br/>");

        strHtmlText += STR_HTML_TABLE_S;

        strHtmlText += STR_HTML_TB_TR_S;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += g_szTitleString[0];
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += g_szTitleString[2];
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += g_szTitleString[3];
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += g_szTitleString[4];
            strHtmlText += STR_HTML_TB_TD_E;
        strHtmlText += STR_HTML_TB_TR_E;

        BYTE * pResBuffer = pNikPEViewer->GetPEBuffer();

		//for(int j=0;j<nEnteries;j++)
		{
			int nSize = pRes->m_nSize / sizeof(ACCELTABLEENTRY);

			TCHAR chBuf[1024];
            chBuf[0] = 0;

			ACCELTABLEENTRY * pAccel = (ACCELTABLEENTRY *)&pResBuffer[pRes->m_nOffsetToData];
			for(int i=0;i<nSize;i++)
			{
                strHtmlText += STR_HTML_TB_TR_S;
                strHtmlText += STR_HTML_TB_TD_S;
				
                _stprintf_s(chBuf,1023,_T("0x%X"),pAccel->wId);

                strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;


				chBuf[0] = 0;
				if(pAccel->fFlags & FVIRTKEY)
					_stprintf_s(chBuf,1023,_T("0x%X"),pAccel->wAnsi);
				else
					_stprintf_s(chBuf,1023,_T("\'%c\'"),pAccel->wAnsi);

                strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;

				chBuf[0] = 0;

				{
					if(pAccel->fFlags & FSHIFT)
						_stprintf_s(chBuf,1023,_T("SHIFT"));

					if(pAccel->fFlags & FALT)
					{
						if(chBuf[0])
							_tcscat_s(chBuf,1023,_T(", "));
						_tcscat_s(chBuf,1024,_T("ALT"));
					}

					if(pAccel->fFlags & FCONTROL)
					{
						if(chBuf[0])
							_tcscat_s(chBuf,1023,_T(", "));
						_tcscat_s(chBuf,1024,_T("CONTROL"));
					}

					if(0 == chBuf[0])
						_tcscat_s(chBuf,1023,_T("NONE"));
				}

                strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;

				chBuf[0] = 0;
				if(pAccel->fFlags & FVIRTKEY)
					_stprintf_s(chBuf,1023,_T("VIRTKEY"));
				else
					_stprintf_s(chBuf,1023,_T("ASCII"));

                strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += chBuf;
                strHtmlText += STR_HTML_TB_TD_E;

                strHtmlText += STR_HTML_TB_TR_E;

				if(pAccel->fFlags & 0x80)
					break;

				pAccel++;
			}
		}

        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += STR_HTML_BODY_E;
        strHtmlText += STR_HTML_END;
	}

	return strHtmlText;
}

_tstring CNikHTMLWindow::GetHtmlResStringTable(void * pNikView)
{
	_tstring strHtmlText;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(!pNikPEViewer)
        return strHtmlText;

    RESOURCELIST * pRes = NULL;
    RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();

    for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
    {
        if(!_tcscmp(_T("STRING"), pResourceList->m_vDirList.at(i)->m_chName))
        {
            pRes = pResourceList->m_vDirList.at(i);
            break;
        }
    }

    if(pRes && pRes->m_vDirList.size())
	{
        strHtmlText = STR_HTML_START;
        strHtmlText += STR_HTML_HEAD;
        strHtmlText += STR_HTML_BODY_S_1;

        strHtmlText += STR_HTML_TABLE_S;
        SetTable1ColValue( strHtmlText, _T("String Table"));
        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += _T("<br/>");

        BYTE * pResBuffer = pNikPEViewer->GetPEBuffer();
        pResourceList = pRes;

		for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
		{
            RESOURCELIST * pRes = pResourceList->m_vDirList.at(i);
			BYTE * pBuf = &pResBuffer[pRes->m_nOffsetToData];
			DWORD dwID = 0;
			TCHAR * pchName = pRes->m_chName;

            strHtmlText += STR_HTML_TABLE_S;

            strHtmlText += STR_HTML_TB_TR_S;
                strHtmlText += STR_HTML_TB_TD_S;
                    strHtmlText += g_szTitleString[0];
                strHtmlText += STR_HTML_TB_TD_E;
                strHtmlText += STR_HTML_TB_TD_S;
                    strHtmlText += g_szTitleString[1];
                strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TR_S;

			_tstring str;
			if(_T('(') == pchName[0])
			{
				str = &pchName[1];
				str.erase(str.length()-1);
				dwID = CNikPEViewer::GetHexValues(str);
			}

			dwID <<= 4;

			TCHAR chBuf[4096];
			WCHAR chBuffer[4096];
			int nOffset = 0;
			WORD * pdwOffset;

			for (int iCol = 0; iCol < 16; iCol++)
			{
				pdwOffset = (WORD *)&pBuf[nOffset];
				nOffset += 2;

                if(pdwOffset[0] > 4095)
                    return FALSE;

				if(pdwOffset[0])
				{
					chBuf[0] = 0;
					_stprintf_s(chBuf,_T("%d"),dwID+iCol-0x10);

                    strHtmlText += STR_HTML_TB_TR_S;
                    strHtmlText += STR_HTML_TB_TD_S;
                        strHtmlText += chBuf;
                    strHtmlText += STR_HTML_TB_TD_E;

					WCHAR * pwBuf = (WCHAR *)&pBuf[nOffset];

					int i=0;
					for(;i<pdwOffset[0];i++)
					{
						chBuffer[i] = pwBuf[i];
					}

					chBuffer[i] = 0;

		#ifdef  _UNICODE
					chBuf[0] = 0;
					_stprintf_s(chBuf,_T("%s"),chBuffer);
		#else
					_stprintf_s(chBuf,_T("%S"),chBuffer);
		#endif

                    strHtmlText += STR_HTML_TB_TD_S;
                        strHtmlText += chBuf;
                    strHtmlText += STR_HTML_TB_TD_E;
                    strHtmlText += STR_HTML_TB_TR_E;

					nOffset += (pdwOffset[0]*2);
				}
			}

            strHtmlText += STR_HTML_TABLE_E;
            strHtmlText += _T("<br/>");
		}

        strHtmlText += STR_HTML_BODY_E;
        strHtmlText += STR_HTML_END;
	}

    return strHtmlText;
}

_tstring CNikHTMLWindow::GetHtmlResMenuItems(void * pNikView)
{
	_tstring strHtmlText;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(!pNikPEViewer)
        return strHtmlText;

    RESOURCELIST * pRes = NULL;
    RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();

    for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
    {
        if(!_tcscmp(_T("MENU"), pResourceList->m_vDirList.at(i)->m_chName))
        {
            pRes = pResourceList->m_vDirList.at(i);
            break;
        }
    }

    if(pRes && pRes->m_vDirList.size())
	{
        strHtmlText = STR_HTML_START;
        strHtmlText += STR_HTML_HEAD;
        strHtmlText += STR_HTML_BODY_S_1;

        strHtmlText += STR_HTML_TABLE_S;
        SetTable1ColValue( strHtmlText, _T("Menu Table"));
        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += _T("<br/>");

        BYTE * pResBuffer = pNikPEViewer->GetPEBuffer();
        pResourceList = pRes;

		for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
		{
            RESOURCELIST * pRes = pResourceList->m_vDirList.at(i);
			BYTE * pBuffer = &pResBuffer[pRes->m_nOffsetToData];

            int nSize = pRes->m_nSize - 2;
		    int nOffset = 4;
            MENU_INFO stMenuInfo;
            stMenuInfo.m_vMenuList.clear();

            CNikResource::CreateMenuList(&stMenuInfo, nOffset, nSize, pBuffer);

            strHtmlText += InitHtmlTableResMenu(&stMenuInfo);
		}

        strHtmlText += STR_HTML_BODY_E;
        strHtmlText += STR_HTML_END;
	}

    return strHtmlText;
}

_tstring CNikHTMLWindow::InitHtmlTableResMenu(MENU_INFO * pMenuInfo)
{
    _tstring strHtmlText;

    if(pMenuInfo && pMenuInfo->m_vMenuList.size())
    {
        strHtmlText += STR_HTML_TABLE_S;

        if(pMenuInfo->m_strText.data()[0] || pMenuInfo->m_wMenuID)
        {
            SetTable2ColValue(strHtmlText, (TCHAR *)pMenuInfo->m_strText.data(), pMenuInfo->m_wMenuID);
        }

        MENU_INFO * pMTemp;
        for(int i=0;i<(int)pMenuInfo->m_vMenuList.size(); i++)
        {
            pMTemp = &pMenuInfo->m_vMenuList.at(i);
            if(pMTemp && (pMTemp->m_strText.data()[0] || pMTemp->m_wMenuID))
            {
                if(pMTemp->m_wFlag & MF_POPUP)
                    SetTable2ColStr(strHtmlText, (TCHAR *)pMTemp->m_strText.data(), _T("---->>"));
                else
                    SetTable2ColValue(strHtmlText, (TCHAR *)pMTemp->m_strText.data(), pMTemp->m_wMenuID);
            }
        }

        strHtmlText += STR_HTML_TABLE_E;
        strHtmlText += _T("<br/>");

        for(int i=0;i<(int)pMenuInfo->m_vMenuList.size(); i++)
        {
            pMTemp = &pMenuInfo->m_vMenuList.at(i);
            if(pMTemp->m_vMenuList.size())
                strHtmlText +=InitHtmlTableResMenu(pMTemp);
        }
    }

    return strHtmlText;
}

_tstring CNikHTMLWindow::GetHtmlResBitmap(void * pNikView, TCHAR * pchBitStr, TCHAR * pchPath)
{
	_tstring strHtmlText, strFileName, strPathName;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(!pNikPEViewer)
        return strHtmlText;

    RESOURCELIST * pRes = NULL;
    RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();

    for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
    {
        if(!_tcscmp(pchBitStr, pResourceList->m_vDirList.at(i)->m_chName))
        {
            pRes = pResourceList->m_vDirList.at(i);
            break;
        }
    }

    strPathName = pchPath;

    if(pRes && pRes->m_vDirList.size())
	{
        strHtmlText = STR_HTML_START;
        strHtmlText += STR_HTML_HEAD;
        strHtmlText += STR_HTML_BODY_S_1;

        BYTE * pResBuffer = pNikPEViewer->GetPEBuffer();
        pResourceList = pRes;

        strPathName += _T("\\image");
        CreateDirectory(strPathName.data(), NULL);

        int nBitType = 0;
        if(1 == pRes->m_nType)
        {
            nBitType = CURSOR_DATA;
            strPathName += _T("\\pecursor_");
        }
        else if(2 == pRes->m_nType)
        {
            nBitType = BITMAP_DATA;
            strPathName += _T("\\pebitmap_");
        }
        else if(3 == pRes->m_nType)
        {
            nBitType = ICON_DATA;
            strPathName += _T("\\peicon_");
        }

		for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
		{
            RESOURCELIST * pRes = pResourceList->m_vDirList.at(i);
			BYTE * pBuffer = &pResBuffer[pRes->m_nOffsetToData];
			TCHAR * pchName = pRes->m_chName;

            strFileName = strPathName;
            strFileName += pRes->m_chName;
            strFileName += _T(".png");

            strHtmlText += STR_HTML_TABLE_S;

            strHtmlText += STR_HTML_TB_TR_S;
            strHtmlText += STR_HTML_TB_TD_S;
                strHtmlText += pchName;
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TD_S;
            if(CURSOR_DATA == nBitType)
                strHtmlText += _T("CURSOR");
            else if(BITMAP_DATA == nBitType)
                strHtmlText += _T("BITMAP");
            else if(ICON_DATA == nBitType)
                strHtmlText += _T("ICON");
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TR_E;

            strHtmlText += STR_HTML_TB_TR_S;
            strHtmlText += STR_HTML_TB_TD_CENTER;
                strHtmlText += _T("<img src=\"");
                strHtmlText += strFileName.data();
                strHtmlText += _T("\" />");
            strHtmlText += STR_HTML_TB_TD_E;
            strHtmlText += STR_HTML_TB_TR_E;

            strHtmlText += STR_HTML_TABLE_E;
            strHtmlText += _T("<br/>");

            SaveBitmapResBuffer(pBuffer, pRes->m_nSize, nBitType, strFileName);
		}

        strHtmlText += STR_HTML_BODY_E;
        strHtmlText += STR_HTML_END;
	}

    return strHtmlText;
}

int CNikHTMLWindow::SaveBitmapResBuffer(BYTE * pBuffer
                                        , int nSize
                                        , int nFlag
                                        , _tstring strBitFilename)
{
	if(pBuffer 
		&& (nSize > 0))
	{
		if(BITMAP_DATA == nFlag)
		{
			nSize += 0xE;
		}

		HGLOBAL hGobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_NODISCARD, nSize+1);
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
                    CLSID   encoderClsid;
                    GetEncoderClsid(_T("image/png"), &encoderClsid);

                    Bitmap hBitmap(pIStream,FALSE);
                    hBitmap.Save(strBitFilename.data(), &encoderClsid, NULL);

					if(pIStream)
						pIStream->Release();
				}

				GlobalUnlock(hGobal);
			}
			GlobalFree(hGobal);
		}
	}

    return 0;
}

int CNikHTMLWindow::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}
