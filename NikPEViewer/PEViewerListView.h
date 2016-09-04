
#pragma once

extern TCHAR DOS_HEADER_E_LFANEW[]             ;
extern TCHAR DOS_HEADER_E_OEMINFO[]            ;
extern TCHAR DOS_HEADER_E_OEMID[]              ;
extern TCHAR DOS_HEADER_E_OVNO[]               ;
extern TCHAR DOS_HEADER_E_LFARLC[]             ;
extern TCHAR DOS_HEADER_E_CS[]                 ;
extern TCHAR DOS_HEADER_E_IP[]                 ;
extern TCHAR DOS_HEADER_E_CSUM[]               ;
extern TCHAR DOS_HEADER_E_SP[]                 ;
extern TCHAR DOS_HEADER_E_SS[]                 ;
extern TCHAR DOS_HEADER_E_MAXALLOC[]           ;
extern TCHAR DOS_HEADER_E_MINALLOC[]           ;
extern TCHAR DOS_HEADER_E_CPARHDR[]            ;
extern TCHAR DOS_HEADER_E_CRLC[]               ;
extern TCHAR DOS_HEADER_E_CP[]                 ;
extern TCHAR DOS_HEADER_E_CBLP[]               ;
extern TCHAR DOS_HEADER_E_MAGIC[]              ;
extern TCHAR DOS_HEADER_1[]                    ;
extern TCHAR STR_PRINTF_X[]                    ;
extern TCHAR STR_PRINTF_s[]                    ;
extern TCHAR STR_HEX_VALUES[]                  ;
extern TCHAR STR_DESCRIPTION[]                 ;
extern TCHAR NT_HEADERS_1[]                    ;
extern TCHAR NT_HEADERS_SIGNATURE[]            ;
extern TCHAR NT_HEADERS_FH_1[]                 ;
extern TCHAR NT_HEADERS_FH_2[]                 ;
extern TCHAR NT_HEADERS_FH_MACHINE[]           ;
extern TCHAR NT_HEADERS_FH_NUOFSECTIONS[]      ;
extern TCHAR NT_HEADERS_FH_TIMEDATESTAMP[]     ;
extern TCHAR NT_HEADERS_FH_PTSYMBOLTABLE[]     ;
extern TCHAR NT_HEADERS_FH_NUMBEROFSYMBOL[]    ;
extern TCHAR NT_HEADERS_FH_SIZEOFOPHEADER[]    ;
extern TCHAR NT_HEADERS_FH_CHARACTERISTIC[]    ;
extern TCHAR NT_HEADERS_OP_NUOFRVAANDSIZES[]   ;
extern TCHAR NT_HEADERS_OP_LOADERFLAGS[]       ;
extern TCHAR NT_HEADERS_OP_SIZEOFHEAPCOMMIT[]  ;
extern TCHAR NT_HEADERS_OP_SIZEOFHEAPRESERVE[] ;
extern TCHAR NT_HEADERS_OP_SIZEOFSTACKCOMMIT[] ;
extern TCHAR NT_HEADERS_OP_SIZEOFSTACKRESERVE[];
extern TCHAR NT_HEADERS_OP_SUBSYSTEM[]         ;
extern TCHAR NT_HEADERS_OP_CHECKSUM[]          ;
extern TCHAR NT_HEADERS_OP_DLLCHARACTERISTIC[] ;
extern TCHAR NT_HEADERS_OP_WDMDRIVER[]         ;
extern TCHAR NT_HEADERS_OP_SIZEOFHEADERS[]     ;
extern TCHAR NT_HEADERS_OP_SIZEOFIMAGE[]       ;
extern TCHAR NT_HEADERS_OP_WIN32VERSIONVALUE[] ;
extern TCHAR NT_HEADERS_OP_MINORSUBSYSVERSION[];
extern TCHAR NT_HEADERS_OP_MAJORSUBSYSVERSION[];
extern TCHAR NT_HEADERS_OP_MINORIMAGEVERSION[] ;
extern TCHAR NT_HEADERS_OP_MAJORLINKERVERSION[];
extern TCHAR NT_HEADERS_OP_MINOROSVERSION[]    ;
extern TCHAR NT_HEADERS_OP_MAJOROSVERSION[]    ;
extern TCHAR NT_HEADERS_OP_FILEALIGNMENT[]     ;
extern TCHAR NT_HEADERS_OP_SECTIONALIGNMENT[]  ;
extern TCHAR NT_HEADERS_OP_IMAGEBASE[]         ;
extern TCHAR NT_HEADERS_OP_BASEOFDATA[]        ;
extern TCHAR NT_HEADERS_OP_BASEOFCODE[]        ;
extern TCHAR NT_HEADERS_OP_ADDOFENTRYPOINT[]   ;
extern TCHAR NT_HEADERS_OP_SIZEOFUNINITDATA[]  ;
extern TCHAR NT_HEADERS_OP_SIZEOFINITDATA[]    ;
extern TCHAR NT_HEADERS_OP_SIZEOFCODE[]        ;
extern TCHAR NT_HEADERS_OP_MINORLINKERVERSION[];
extern TCHAR NT_HEADERS_OP_MAGIC[]             ;
extern TCHAR NT_HEADERS_OP_1[]                 ;
extern TCHAR NT_HEADERS_OP_2[]                 ;
extern TCHAR STR_ORDIANL_NO[]                  ;
extern TCHAR STR_ENTRY_POINT[]                 ;
extern TCHAR STR_FUNCTION_NAME[]               ;
extern TCHAR STR_EXPORT_FUNCTIONS[]            ;
extern TCHAR STR_VIRTUAL_ADDRESS[]             ;
extern TCHAR STR_DATADIRECTORY_SIZE[]          ;
extern TCHAR STR_DATA_DIRECTORY[]              ;
extern TCHAR SH_NUMBEROFLINENUMBERS[]          ;
extern TCHAR SH_NUMBEROFRELOCATIONS[]          ;
extern TCHAR SH_POINTERTOLINENUMBERS[]         ;
extern TCHAR SH_POINTERTORELOCATIONS[]         ;
extern TCHAR SH_POINTERTORAWDATA[]             ;
extern TCHAR SH_SIZEOFRAWDATA[]                ;
extern TCHAR SH_VIRTUAL_ADDRESS[]              ;
extern TCHAR SH_PHYSICAL_ADDRESS[]             ;
extern TCHAR SH_SECTIONS_NAME[]                ;
extern TCHAR SH_SECTIONS_LIST[]                ;
extern TCHAR STR_RECENT_FILES[]                ;
extern TCHAR STR_HTML_START[]                  ;
extern TCHAR STR_HTML_END[]                    ;
extern TCHAR STR_HTML_HEAD[]                   ;
extern TCHAR STR_HTML_BODY_S[]                 ;
extern TCHAR STR_HTML_BODY_S_1[]               ;
extern TCHAR STR_HTML_BODY_E[]                 ;
extern TCHAR STR_HTML_TABLE_S[]                ;
extern TCHAR STR_HTML_TABLE_E[]                ;
extern TCHAR STR_HTML_TB_TR_S[]                ;
extern TCHAR STR_HTML_TB_TR_E[]                ;
extern TCHAR STR_HTML_TB_TD_CENTER[]           ;
extern TCHAR STR_HTML_TB_TD_S[]                ;
extern TCHAR STR_HTML_TB_TD_E[]                ;

class CNikPEVListViewCtrl
{

protected:

    HWND            m_hWnd;
    HINSTANCE       m_hInstance;

    void *          m_pNikPEViewer;

public:
    CNikPEVListViewCtrl();
    virtual ~CNikPEVListViewCtrl();

    HWND GetWindowHandle()
    {
        return m_hWnd;
    }

    LRESULT ListView_Notify(HWND , LPARAM );

    virtual int FillPEVListViewInfo(void * );

    virtual HWND CreatePEListViewCtrl(HINSTANCE , HWND , RECT , _tstring );

    virtual BOOL InsertColumnListViewCtl( HWND , TCHAR * , int , int );
    virtual BOOL InsertColumnListViewValues( HWND , TCHAR * , int , int nFlag = 0);
    static BOOL InsertColumnListViewValues_1( HWND , TCHAR * , int , int nFlag = 0);
};

class CNikPEVHeaderList : public CNikPEVListViewCtrl
{
public:

    CNikPEVHeaderList();
    virtual ~CNikPEVHeaderList();

    virtual int FillPEVListViewInfo(void * );

    int List_IMAGE_DOS_HEADER(const IMAGE_DOS_HEADER * );
    int List_IMAGE_NT_HEADERS(const IMAGE_NT_HEADERS * );
};

class CNikDataSectionList : public CNikPEVListViewCtrl
{
public:

    CNikDataSectionList();
    virtual ~CNikDataSectionList();

    int List_IMAGE_NT_DATA_DIRECTORY(void * );
    int List_IMAGE_NT_SECTION_DETAILS(void * );
    virtual int FillPEVListViewInfo(void * );
};

class CNikImportFunList : public CNikPEVListViewCtrl
{
public:

    CNikImportFunList();
    virtual ~CNikImportFunList();

    virtual int FillPEVListViewInfo(void * );

    int List_IMAGE_NT_IMPORT_TABLE(void * );
};

class CNikExportFunList : public CNikPEVListViewCtrl
{
public:

    CNikExportFunList();
    virtual ~CNikExportFunList();

    virtual int FillPEVListViewInfo(void * );
    int List_IMAGE_NT_EXPORT_TABLE(void * );
};

class CNikResourceItemsList : public CNikPEVListViewCtrl
{
public:

    CNikResourceItemsList();
    virtual ~CNikResourceItemsList();

    virtual int FillPEVListViewInfo(const void * );
    int List_IMAGE_RESOURCE_ITEMS_TABLE(const void *, int );
};

class CNikDisAssemblyList : public CNikPEVListViewCtrl
{
    HANDLE          m_hThread;
    DWORD           m_dwThreadId;

    BOOL            m_bIsThreadRunning;

public:

    CNikDisAssemblyList();
    virtual ~CNikDisAssemblyList();

    int StopDisAssemblyThread();
    virtual int FillPEVListViewInfo(const void * );

    static DWORD WINAPI DisAssemblyThreadProc( LPVOID lpParam );
};
