
#pragma once

#include "DialogWnd.h"
#include "NikDisAssembly.h"
#include "ResourceWnd.h"
#include "NikHTMLView.h"
#include "EditorBitmap.h"
#include "PEViewerTree.h"
#include "PEBinaryFile.h"
#include "PEViewerListView.h"
#include "NikDisassemblyWin.h"

extern CNikDisassembly g_cNikDisassembly;

class CNikPEViewer
{
    HWND                            m_hWnd;
    HWND                            m_hTreeCtrl;

    int                             m_nChildCount;
    int                             m_nDiffOffset;

    DWORD                           m_dwPEFileSize;
    DWORD                           m_dwDiffAddress;
    DWORD                           m_dwResourceBaseAdd;

    BYTE *                          m_pPEBuffer;

    HINSTANCE                       m_hInstance;

    _tstring                        m_strFileName;
    _tstring                        m_strShortName;
	_tstring                        m_strSaveFileName;

    CNikPEVTreeViewCtrl             m_cPEVTreeCtrl;
    CNikPEVHeaderList               m_cPEVHeaderList;
    CNikImportFunList               m_cPEVImportList;
    CNikExportFunList               m_cPEVExportList;
    CNikPEBinaryViewer              m_cPEBinaryViewer;
    CNikDataSectionList             m_cDataSectionList;
    //CNikDisAssemblyList           m_cDisAssemblyList;
    CNikDisassemblyWin              m_cDisAssemblyList;
    CNikResourceItemsList           m_cResourceItemsList;
	CNikHTMLWindow                  m_cNikHTMLWindow;

    RESOURCELIST *                  m_pstResourceList;

    EXECUTABLE_INFO                 m_stExecutableInfo;
    IMAGE_DOS_HEADER                m_stImageDosHeader;
    IMAGE_NT_HEADERS32              m_stImageNtHeaders32;
	IMAGE_NT_HEADERS64              m_stImageNtHeaders64;

    vector<IMPORTDLLLIST>		    m_vImportDllList;
    vector<EXPORTFUNCTIONLIST>	    m_vExportFunctionList;
    vector<IMAGE_SECTION_HEADER>    m_vImageSectionHeaderList;

    vector<CNikDialog *>         m_vNikDialogList;

    vector<CNikResource *>       m_vMenuViewerList;
    vector<CNikResource *>       m_vStringViewerList;
    vector<CNikResource *>       m_vVersionViewerList;
    vector<CNikResource *>       m_vMessageViewerList;
    vector<CNikResource *>       m_vResourceViewerList;
    vector<CNikResource *>       m_vAccelaratorViewerList;

    vector<CNikBitmapEditor *>   m_vIconEditorList;
    vector<CNikBitmapEditor *>   m_vCursorEditorList;
    vector<CNikBitmapEditor *>   m_vBitmapEditorList;

public:
    CNikPEViewer();
    virtual ~CNikPEViewer();

    HWND GetWindowHandle()
    {
        return m_hWnd;
    }
    void SetWindowHandle( HWND hWnd )
    {
        m_hWnd = hWnd;
    }

    BYTE * GetPEBuffer()
    {
        return m_pPEBuffer;
    }

    HINSTANCE GetPEViewerInstance()
    {
        return m_hInstance;
    }

    const IMAGE_DOS_HEADER &GetImageDosHeader()
    {
        return m_stImageDosHeader;
    }
    const IMAGE_NT_HEADERS &GetImageNtHeaders()
    {
		if(m_stImageNtHeaders32.OptionalHeader.Magic == 0x020b)
			return (const IMAGE_NT_HEADERS32 &)m_stImageNtHeaders64;
		else
			return m_stImageNtHeaders32;
    }
    const vector<IMPORTDLLLIST> &GetImportFunctionList()
    {
        return m_vImportDllList;
    }
    const vector<EXPORTFUNCTIONLIST> &GetExportFunctionList()
    {
        return m_vExportFunctionList;
    }
    const vector<IMAGE_SECTION_HEADER> &GetImageSectionHeaderList()
    {
        return m_vImageSectionHeaderList;
    }
    const RESOURCELIST * GetResourceItemsList()
    {
        return m_pstResourceList;
    }
    const EXECUTABLE_INFO &GetExecutableInfo()
    {
        return m_stExecutableInfo;
    }

    _tstring GetModuleFileName()
    {
        return m_strFileName;
    }
    _tstring GetModuleShortName()
    {
        return m_strShortName;
    }
    DWORD GetModuleFileSize()
    {
        return m_dwPEFileSize;
    }

    CNikPEBinaryViewer &GetBinaryViewer()
    {
        return m_cPEBinaryViewer;
    }

    int GetDiffAddOffset()
    {
        return m_nDiffOffset;
    }

    //CNikDisAssemblyList &GetDisAssemblyList()
    CNikDisassemblyWin &GetDisAssemblyList()
    {
        return m_cDisAssemblyList;
    }

    int ShowPESammaryView(int nShow = FALSE)
    {
        if(nShow)
            return SetWindowPos( m_cPEVTreeCtrl.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        else
        {
            if(TRUE == IsWindowVisible( m_cPEVTreeCtrl.GetWindowHandle() ))
                return ShowWindow( m_cPEVTreeCtrl.GetWindowHandle(), SW_HIDE );
            else
                return SetWindowPos( m_cPEVTreeCtrl.GetWindowHandle(), HWND_TOP
                    , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        }
    }

    int ShowPEImportListView(int nShow = FALSE)
    {
        if(nShow)
            return SetWindowPos( m_cPEVImportList.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        else
        {
            if(TRUE == IsWindowVisible( m_cPEVImportList.GetWindowHandle() ))
                return ShowWindow( m_cPEVImportList.GetWindowHandle(), SW_HIDE );
            else
                return SetWindowPos( m_cPEVImportList.GetWindowHandle(), HWND_TOP
                    , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        }
    }

    int ShowPEExportListView(int nShow = FALSE)
    {
        if(nShow)
            return SetWindowPos( m_cPEVExportList.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        else
        {
            if(TRUE == IsWindowVisible( m_cPEVExportList.GetWindowHandle() ))
                return ShowWindow( m_cPEVExportList.GetWindowHandle(), SW_HIDE );
            else
                return SetWindowPos( m_cPEVExportList.GetWindowHandle(), HWND_TOP
                    , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        }
    }

    int ShowPEHeadersView(int nShow = FALSE)
    {
        if(nShow)
            return SetWindowPos( m_cPEVHeaderList.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        else
        {
            if(TRUE == IsWindowVisible( m_cPEVHeaderList.GetWindowHandle() ))
                return ShowWindow( m_cPEVHeaderList.GetWindowHandle(), SW_HIDE );
            else
                return SetWindowPos( m_cPEVHeaderList.GetWindowHandle(), HWND_TOP
                    , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        }
    }

    int ShowPEBinaryView(int nShow = FALSE)
    {
        if(nShow)
            return SetWindowPos( m_cPEBinaryViewer.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        else
        {
            if(TRUE == IsWindowVisible( m_cPEBinaryViewer.GetWindowHandle() ))
                return ShowWindow( m_cPEBinaryViewer.GetWindowHandle(), SW_HIDE );
            else
                return SetWindowPos( m_cPEBinaryViewer.GetWindowHandle(), HWND_TOP
                    , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        }
    }

    int ShowPEDataSectionsView(int nShow = FALSE)
    {
        if(nShow)
            return SetWindowPos( m_cDataSectionList.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        else
        {
            if(TRUE == IsWindowVisible( m_cDataSectionList.GetWindowHandle() ))
                return ShowWindow( m_cDataSectionList.GetWindowHandle(), SW_HIDE );
            else
                return SetWindowPos( m_cDataSectionList.GetWindowHandle(), HWND_TOP
                    , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
        }
    }

    int ShowPEAllSectionView()
    {
        ShowPEImportListView( TRUE );
        ShowPEExportListView( TRUE );
        ShowPEDataSectionsView( TRUE );
        ShowPEHeadersView( TRUE );
        ShowPESammaryView( TRUE );
        ShowPEDataSectionsView( TRUE );
        ShowPEBinaryView( TRUE );
        ShowCodeDisassembly( TRUE );
		ShowPEHtmlView();

        return 0;
    }
    int ShowPEHtmlView();

    virtual int InitPEFrameWindow();
    virtual int SetAllViewsDefaultPos();
    virtual int DecodePEFile(_tstring );
    virtual int SavePEFileRecords(_tstring );
    virtual int RegisterNikWindows( HINSTANCE );
    virtual HWND CreatePEFrameWindow(HINSTANCE , HWND );

    void CreateExecutableInfo( BYTE * pBuf
                              , DWORD dwSize
                              , PIMAGE_NT_HEADERS pNTHeader
                              , PIMAGE_SECTION_HEADER pNTSection);

    void CreateImportFunctionList( BYTE * , DWORD , DWORD , int 
        , PIMAGE_NT_HEADERS , PIMAGE_SECTION_HEADER );

    void CreateExportFunctionList(BYTE * , DWORD , DWORD , int 
        , PIMAGE_NT_HEADERS , PIMAGE_SECTION_HEADER );

    void CreateResourceList(BYTE * ,int );
    void GetResourceName(DWORD ,TCHAR * );
    RESOURCELIST * DecodeResourceItems(BYTE * ,int ,int );
    int SavePEHtmlFilePart(_tstring , _tstring , _tstring );
    void DecodeDirResourceEntry(BYTE * , int , int , int , RESOURCELIST * );

    static _tstring OpenFileDialogBox(HWND hWnd);
    static _tstring SaveFileDialogBox(HWND hWnd);
    static LRESULT CALLBACK PEViewerWndProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam );

    int ShowCodeDisassembly(int nShow = FALSE);
	static DWORD GetHexValues(_tstring str);
    static _tstring GetVersionOSName( DWORD dwOScode);
    static _tstring GetVerResourceType( DWORD dwType );
    static _tstring GetVerResLanguage( DWORD dwResLang );
    static int GetMachineName(int nCode, int nStrSize, TCHAR *chMachine);
    static int GetNTOptionalSubsystem(int nCode, int nStrSize, TCHAR *chBuf);
    static int GetNTCharacteristics(WORD wCharacteristics, _tstring &strChar);
    static int GetStrDateTimeStamp(DWORD dwDateTime, int nStrSize, TCHAR *chBuf);
    static int GetDirectoryEnteries(int i, int nStrSize, TCHAR *chBuf);
    static int GetNTSectionCharacteristics(DWORD dwCharacteristics, _tstring &strChar);
};