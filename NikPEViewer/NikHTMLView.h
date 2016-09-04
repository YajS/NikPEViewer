
#define NOTIMPLEMENTED  return E_NOTIMPL

class CNikHTMLWindow;

class CNullStorage: public IStorage
{
public:

    CNikHTMLWindow * m_pNikHTMLWin;

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID //riid
                                ,void ** //ppvObject
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP_(ULONG) AddRef(void)
    {  return (ULONG)E_NOTIMPL; }
    STDMETHODIMP_(ULONG) Release(void)
    {  return (ULONG)E_NOTIMPL; }

    // IStorage
    STDMETHODIMP CreateStream(const WCHAR * //pwcsName
                                ,DWORD //grfMode
                                ,DWORD //reserved1
                                ,DWORD //reserved2
                                ,IStream ** //ppstm
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP OpenStream(const WCHAR * //pwcsName
                            ,void * //reserved1
                            ,DWORD //grfMode
                            ,DWORD //reserved2
                            ,IStream ** //ppstm
                            )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP CreateStorage(const WCHAR * //pwcsName
                                ,DWORD //grfMode
                                ,DWORD //reserved1
                                ,DWORD //reserved2
                                ,IStorage ** //ppstg
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP OpenStorage(const WCHAR * //pwcsName
                                ,IStorage * //pstgPriority
                                ,DWORD //grfMode
                                ,SNB //snbExclude
                                ,DWORD //reserved
                                ,IStorage ** //ppstg
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP CopyTo(DWORD //ciidExclude
                        ,IID const * //rgiidExclude
                        ,SNB //snbExclude
                        ,IStorage * //pstgDest
                        )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP MoveElementTo(const OLECHAR * //pwcsName
                                ,IStorage * //pstgDest
                                ,const OLECHAR* //pwcsNewName
                                ,DWORD //grfFlags
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP Commit(DWORD /*grfCommitFlags*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP Revert(void)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP EnumElements(DWORD //reserved1
                            ,void * //reserved2
                            ,DWORD //reserved3
                            ,IEnumSTATSTG ** //ppenum
                            )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP DestroyElement(const OLECHAR * /*pwcsName*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP RenameElement(const WCHAR * //pwcsOldName
                                ,const WCHAR * //pwcsNewName
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetElementTimes(const WCHAR * //pwcsName
                                    ,FILETIME const * //pctime
                                    ,FILETIME const * //patime
                                    ,FILETIME const * //pmtime
                                    )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetStateBits(DWORD //grfStateBits
                                ,DWORD //grfMask
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP Stat(STATSTG * //pstatstg
                        ,DWORD //grfStatFlag
                        )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetClass(REFCLSID /*clsid*/)
    {  return S_OK; }
};

class CHtmlFrame: public IOleInPlaceFrame
{
public:

    CNikHTMLWindow * m_pNikHTMLWin;

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID //riid
                                ,void ** //ppvObject
                                )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP_(ULONG) AddRef(void)
    {  return 1;}
    STDMETHODIMP_(ULONG) Release(void)
    {  return 1;}

    // IOleWindow
    STDMETHODIMP GetWindow(HWND FAR* /*lphwnd*/);
    STDMETHODIMP ContextSensitiveHelp(BOOL /*fEnterMode*/)
    {  NOTIMPLEMENTED; }

    // IOleInPlaceUIWindow
    STDMETHODIMP GetBorder(LPRECT /*lprectBorder*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS /*pborderwidths*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS /*pborderwidths*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject * //pActiveObject
                                    ,LPCOLESTR //pszObjName
                                    )
    {  return S_OK; }

    // IOleInPlaceFrame
    STDMETHODIMP InsertMenus(HMENU //hmenuShared
                            ,LPOLEMENUGROUPWIDTHS //lpMenuWidths
                            )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetMenu(HMENU //hmenuShared
                        ,HOLEMENU //holemenu
                        ,HWND //hwndActiveObject
                        )
    {  return S_OK; }
    STDMETHODIMP RemoveMenus(HMENU /*hmenuShared*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP SetStatusText(LPCOLESTR /*pszStatusText*/)
    {  return S_OK; }
    STDMETHODIMP EnableModeless(BOOL /*fEnable*/)
    {  return S_OK; }
    STDMETHODIMP TranslateAccelerator(  LPMSG //lpmsg
                                        ,WORD //wID
                                        )
    {  NOTIMPLEMENTED; }
};

class CHtmlSite: public IOleClientSite, public IOleInPlaceSite
{
public:

    CNikHTMLWindow * m_pNikHTMLWin;

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID //riid
                                , void ** //ppvObject
                                );
    STDMETHODIMP_(ULONG) AddRef(void)
    {  return 1;}
    STDMETHODIMP_(ULONG) Release(void)
    {  return 1;}

    // IOleClientSite
    STDMETHODIMP SaveObject()
    {  NOTIMPLEMENTED; }
    STDMETHODIMP GetMoniker(DWORD //dwAssign
                            ,DWORD //dwWhichMoniker
                            ,IMoniker ** //ppmk
                            )
    {  NOTIMPLEMENTED; }
    STDMETHODIMP GetContainer(LPOLECONTAINER FAR* /*ppContainer*/);
    STDMETHODIMP ShowObject()
    {  return S_OK; }
    STDMETHODIMP OnShowWindow(BOOL /*fShow*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP RequestNewObjectLayout()
    {  NOTIMPLEMENTED; }

    // IOleWindow
    STDMETHODIMP GetWindow(HWND FAR* /*lphwnd*/);
    STDMETHODIMP ContextSensitiveHelp(BOOL /*fEnterMode*/)
    {  NOTIMPLEMENTED; }

    // IOleInPlaceSite methods
    STDMETHODIMP CanInPlaceActivate()
    {  return S_OK; }
    STDMETHODIMP OnInPlaceActivate()
    {  return S_OK; }
    STDMETHODIMP OnUIActivate()
    {  return S_OK; }
    STDMETHODIMP GetWindowContext(LPOLEINPLACEFRAME FAR* //lplpFrame
                                ,LPOLEINPLACEUIWINDOW FAR* //lplpDoc
                                ,LPRECT //lprcPosRect
                                ,LPRECT //lprcClipRect
                                ,LPOLEINPLACEFRAMEINFO //lpFrameInfo
                                );
    STDMETHODIMP Scroll(SIZE /*scrollExtent*/)
    {  NOTIMPLEMENTED; }
    STDMETHODIMP OnUIDeactivate(BOOL /*fUndoable*/)
    {  return S_OK; }
    STDMETHODIMP OnInPlaceDeactivate()
    {  return S_OK; }
    STDMETHODIMP DiscardUndoState()
    {  NOTIMPLEMENTED; }
    STDMETHODIMP DeactivateAndUndo()
    {  NOTIMPLEMENTED; }
    STDMETHODIMP OnPosRectChange(LPCRECT /*lprcPosRect*/);
};

/*class CHtmlUIHandler : public IDocHostUIHandler
{
public:

    CNikHTMLWindow * m_pNikHTMLWin;

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv)
    { NOTIMPLEMENTED; }
    STDMETHODIMP_(ULONG) AddRef(void)
    {  return 1;}
    STDMETHODIMP_(ULONG) Release(void)
    {  return 1;}

    // IDocHostUIHandler
    HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO *pInfo) 
    {
        //pInfo->dwFlags=0;
        return S_OK;
        //NOTIMPLEMENTED;
    }
    HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject,
        IOleCommandTarget *pCommandTarget,IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc) 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE HideUI() 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE UpdateUI() 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate) 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate) 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fRameWindow) 
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID) 
    {return S_FALSE;}
    HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR *pchKey,DWORD dw) 
    {return S_FALSE;}
    HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget *pDropTarget,IDropTarget **ppDropTarget) 
    {return S_FALSE;}
    HRESULT STDMETHODCALLTYPE GetExternal(IDispatch **ppDispatch) 
    {*ppDispatch=0; return S_FALSE;}
    HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut) 
    {*ppchURLOut=0; return S_FALSE;}
    HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject *pDO,IDataObject **ppDORet) 
    {*ppDORet=0; return S_FALSE;}
};*/

class CNikHTMLWindow
{
//protected:
public:

    HWND            m_hWnd;
    HINSTANCE       m_hInstance;

    CHtmlSite       m_cSite;
    CHtmlFrame      m_cFrame;
    CNullStorage    m_cStorage;
    IOleObject*     m_pWebObject;
    //CHtmlUIHandler  m_cUIHandler;

    void *          m_pNikPEViewer;

public:

    CNikHTMLWindow();
    virtual ~CNikHTMLWindow();

    HWND GetWindowHandle()
    {
        return m_hWnd;
    }

    void SetTable1ColValue(_tstring &strHtmlText, TCHAR* strRow1);
    void SetTable2ColValue(_tstring &strHtmlText, TCHAR* strRow1, DWORD nValue);
    void SetTable2ColStr(_tstring &strHtmlText, TCHAR* strRow1, TCHAR* strValue);

    _tstring CreatePEDosHrHtmlPageStr();
    _tstring CreatePENTHtmlPageString();
    _tstring CreateDataHtmlPageString();
    _tstring CreateImportHtmlPageString();
    _tstring CreateExportHtmlPageString();
    _tstring CreateSectionHtmlPageString();

    void Fill_IMAGE_DOS_HEADER(_tstring &);
    void Fill_IMAGE_NT_HEADERS(_tstring &);

    int Fill_IMAGE_NT_SECTION_DETAILS(_tstring &);
    int Fill_IMAGE_NT_DATA_DIRECTORY(_tstring &);

    int Fill_IMAGE_NT_IMPORT_TABLE(_tstring &);
    int Fill_IMAGE_NT_EXPORT_TABLE(_tstring &);

    _tstring SetTreeItemStr(void * );
    _tstring GetHtmlStrVersion(void * );
    _tstring GetHtmlResMenuItems(void * );
    _tstring GetHtmlStrAccelarator(void * );
    _tstring GetHtmlResStringTable(void * );
    _tstring InitHtmlTableResMenu(MENU_INFO * );

    _tstring GetHtmlResBitmap(void * , TCHAR *, TCHAR *);

    void SetTableColumnVal(TCHAR * , _tstring &, TCHAR *);

    static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    int SaveBitmapResBuffer(BYTE * pBuffer, int nSize, int nFlag,_tstring );

    HRESULT CreateEmbeddedWebControl(void * pNikPEViewer);
    void UnCreateEmbeddedWebControl(void);
    virtual HWND CreateHTMLWindow(HINSTANCE , HWND , RECT , _tstring );
    HRESULT LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream);
};
