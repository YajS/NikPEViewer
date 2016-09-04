// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsX.h>
#include <exdisp.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <mshtmdid.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <vector>

#include <wininet.h>
#include <commctrl.h>
#include <shellapi.h>
#include <Commdlg.h>
#include <Psapi.h>


// TODO: reference additional headers your program requires here
using namespace std;

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"GDI32.lib")
#pragma comment (lib,"shell32.lib")
#pragma comment (lib,"Comdlg32.lib")
#pragma comment (lib,"comctl32.lib")
#pragma comment (lib,"Psapi.lib")

#ifdef  _UNICODE
	#define		_tstring	wstring
#else
	#define		_tstring	string
#endif

#define WM_PE_VIEWER_DISTORY    WM_USER + 1977
#define OPEN_RESOURCE_ITEM		WM_USER + 1978
#define UM_WHEELDOWN			WM_USER + 1979
#define UM_WHEELUP				WM_USER + 1980

#define	MENU_FONT		  _T("Tahoma")
#define DOS_HEADER        _T("Dos header")
#define NT_HEADERS        _T("NT headers")
#define NT_SIGNATURE      _T("NT Signature")
#define FILE_HEADER       _T("File header")
#define OPTIONAL_HEADER   _T("Optional header")
#define DATA_DIRECTORIES  _T("Data Directories")
#define EXPORT_TABLE      _T("Export table")
#define IMPORT_TABLE      _T("Import table")
#define RESOURCE_TABLE    _T("Resource table")
#define EXCEPTION_TABLE   _T("Exception table")
#define SECURITY_TABLE    _T("Security table")
#define BASERELOC_TABLE   _T("Basereloc table")
#define DEBUG_TABLE       _T("Debug table")
#define COPYRIGHT_TABLE   _T("Copyright table")
#define GLOBALPTR_TABLE   _T("Globalptr table")
#define TLS_TABLE         _T("Tls table")
#define LOAD_CONFIG_TABLE _T("LOAD_CONFIG TABLE")
#define UNDEFINED_TABLE   _T("UNDEFINED TABLE")
#define IMAGE_SECTIONS    _T("Sections")
#define UNDEFINED_SECTION _T("<UNDEFINED SECTION>")

//---------------------------
typedef struct _MENU_INFO
{
	WORD				m_wFlag;
	WORD				m_wMenuID;
	wstring				m_strText;
    RECT                m_rRect;
	vector<_MENU_INFO>	m_vMenuList;

	_MENU_INFO()
	{
		m_wFlag		= 0;
		m_wMenuID	= 0;
        memset( &m_rRect, 0, sizeof(RECT));
	}

	~_MENU_INFO()
	{
		m_vMenuList.clear();
	}
}MENU_INFO;
//---------------------------

//---------------------------
typedef struct _IMPORTFUNCTIONLIST
{
	int			m_nOrdinalNo;
	_tstring	m_strFunctionName;
}IMPORTFUNCTIONLIST;
//---------------------------

//---------------------------
typedef struct _IMPORTDLLLIST
{
	_tstring					m_strDllName;
	IMAGE_IMPORT_DESCRIPTOR		m_stImageDescriptor;
	vector<IMPORTFUNCTIONLIST>	m_vImportFunctionList;

	_IMPORTDLLLIST()
	{
		memset(&m_stImageDescriptor,0,sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	~_IMPORTDLLLIST()
	{
		m_vImportFunctionList.clear();
	}
}IMPORTDLLLIST;
//---------------------------

//---------------------------
typedef struct _EXPORTFUNCTIONLIST
{
	_tstring		m_strFunctionName;
	DWORD			m_dwOrdianlNo;
	DWORD			m_dwHint;
	DWORD			m_dwEntryPoint;

	_EXPORTFUNCTIONLIST()
	{
		m_dwHint		= 0;
		m_dwOrdianlNo	= 0;
		m_dwEntryPoint	= 0;
	}
}EXPORTFUNCTIONLIST;
//---------------------------

//---------------------------
typedef struct _RESOURCELIST
{
	TCHAR						m_chName[128];
	BOOL						m_bIsDir;
	int							m_nType;
	int							m_nFileOffset;
	int							m_nSize;
	int							m_nOffsetToData;
	vector<_RESOURCELIST *>		m_vDirList;

	_RESOURCELIST()
	{
		memset(m_chName,0,sizeof(m_chName));
		m_nType = -1;
		m_nSize = 0;
		m_nFileOffset = -1;
		m_nOffsetToData = -1;
		m_bIsDir = FALSE;
	}
	~_RESOURCELIST()
	{
		_RESOURCELIST * pTemp;
		for(int i=0;i<(int)m_vDirList.size();i++)
		{
			pTemp = m_vDirList.at(i);
			if(pTemp)
				delete pTemp;
		}
		m_vDirList.clear();
	}
}RESOURCELIST;
//---------------------------

/*/---------------------------
typedef struct _EXECUTABLE_INFO
{
    HWND            m_hListWnd;
    BYTE *          m_pBuffer;
    DWORD           m_dwBufferSize;
	DWORD			m_dwEndOffset;
	DWORD			m_dwStartOffset;
    DWORD			m_dwEntryPoint;

	_EXECUTABLE_INFO()
	{
        m_hListWnd      = NULL;

        m_pBuffer       = NULL;

		m_dwEndOffset	= 0;
		m_dwEntryPoint	= 0;
        m_dwBufferSize  = 0;
        m_dwStartOffset	= 0;
	}
}EXECUTABLE_INFO;
*///---------------------------

//---------------------------
typedef struct _EXECUTABLE_INFO
{
    HWND                        m_hListWnd;

    BYTE *                      m_pBuffer;
    DWORD                       m_dwBufferSize;

    DWORD			            m_dwBaseCode;
	DWORD			            m_dwEndOffset;
	DWORD			            m_dwStartOffset;
    DWORD			            m_dwEntryPoint;

    IMAGE_NT_HEADERS            m_stNTHeader;

    vector<IMPORTDLLLIST>		m_vImportDllList;
    vector<EXPORTFUNCTIONLIST>	m_vExportFunctionList;

    void CleanUp()
    {
        m_hListWnd      = NULL;
        m_dwBaseCode    = 0;
		m_dwEndOffset	= 0;
		m_dwStartOffset	= 0;
		m_dwEntryPoint	= 0;

        m_pBuffer       = NULL;
        m_dwBufferSize  = 0;

        memset(&m_stNTHeader, 0, sizeof(IMAGE_NT_HEADERS));

        m_vImportDllList.clear();
        m_vExportFunctionList.clear();
    }

	_EXECUTABLE_INFO()
	{
        CleanUp();
	}

    ~_EXECUTABLE_INFO()
    {
        if(m_pBuffer)
        {
            //delete [] m_pBuffer;
            m_pBuffer = NULL;
        }

        m_dwBufferSize = 0;
    }

}EXECUTABLE_INFO;
//---------------------------
