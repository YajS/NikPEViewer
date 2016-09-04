// NikPEViewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PEViewer.h"
#include "NikPEViewer.h"

CNikDisassembly g_cNikDisassembly;

#define RECENT_FILE_ID_START    2000


#define MAX_FILESIZE				1024
#define MENU_INDEX_NO               3           // change it anytime insert menu (recent file menu)

HINSTANCE	g_hinstance;
HWND        g_hWndMain, g_hMDIClientWnd;
HWND        g_hwndStatus, g_hWndToolbar;
HIMAGELIST  g_hToolBarImgList;

extern TCHAR STR_LICENSE[];
extern TCHAR STR_RECENT_FILES[];
extern CNikPEViewer * g_pNikPEViewer;

TCHAR       g_szAppName[]       = _T("NIK_PEVIEWER_FRAME");
TCHAR       g_szTitle[]         = _T("Portable Executable Viewer");

vector<CNikPEViewer *>        g_vPEViewerWindList;
vector<_tstring>              g_vstrRecentFileNames;

WNDPROC wpOrigMDIProc;
LRESULT APIENTRY SubclassMDIProc(HWND hwnd
                                 , UINT uMsg
                                 , WPARAM wParam
                                 , LPARAM lParam);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    LicenseDlg(HWND , UINT , WPARAM , LPARAM );

#define MAX_VALUE_NAME 16383
#define REG_RECENT_KEY_VALUE    _T("Software\\CodeDebug\\PEViewer\\v0.0.2.1")
#define REG_RECENT_KEY_NAME     _T("Software\\CodeDebug\\PEViewer\\v0.0.2.1\\Recent Files")

int UpdateRegisterValues()
{
    HKEY hKey = NULL;
    DWORD dwBufLen, dwValue;
    LONG lRet = RegOpenKeyEx( HKEY_CURRENT_USER, REG_RECENT_KEY_NAME,
                0, KEY_QUERY_VALUE, &hKey );
    if( lRet == ERROR_SUCCESS )
    {
        dwBufLen = 0;
        lRet = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL
                    , NULL, &dwBufLen, NULL, NULL, NULL, NULL );

        if(dwBufLen)
        {
            g_vstrRecentFileNames.clear();

            DWORD dwInfo = 0;
            TCHAR chValueName[256];
            TCHAR chFileName[MAX_VALUE_NAME + 1];
            for(int i=0; i<(int)dwBufLen; i++)
            {
                dwInfo = 255;
                dwValue = MAX_VALUE_NAME;
                chValueName[0] = chValueName[255] = 0;
                chFileName[0] = chFileName[MAX_VALUE_NAME] = 0;
                lRet = RegEnumValue(hKey, i, chValueName, &dwInfo
                            , NULL, NULL, (LPBYTE)chFileName, &dwValue ); 
                if( (lRet == ERROR_SUCCESS) 
                    || (chFileName[0]) )
                {
                    g_vstrRecentFileNames.push_back( chFileName );
                }
            }
        }

        RegCloseKey( hKey );
    }
    else
        return -1;

    return 0;
}

int UpdateMenuItems( HWND hWnd )
{
    if( hWnd )
    {
        HMENU hMenu = GetMenu( hWnd );
        if( hMenu )
        {
            HMENU hMenu1;
            DWORD dwValue = GetMenuItemCount( hMenu );
            if(dwValue > 4)
                hMenu1 = GetSubMenu( hMenu, 1);
            else
                hMenu1 = GetSubMenu( hMenu, 0);

            if( hMenu1 )
            {
                HMENU hMenu2;
                dwValue = GetMenuItemID( hMenu1, MENU_INDEX_NO);
                if(dwValue > 0)
                {
                    dwValue = DeleteMenu( hMenu1, MENU_INDEX_NO, MF_BYPOSITION);
                    dwValue = DrawMenuBar( hWnd );
                }

                hMenu2 = CreatePopupMenu();
                if( hMenu2 )
                {
                    if( g_vstrRecentFileNames.size() )
                    {
                        for(int i=(int)g_vstrRecentFileNames.size()-1; i>=0; i--)
                        {
                            InsertMenu( hMenu2, 0, MF_BYPOSITION
                                , RECENT_FILE_ID_START+i, g_vstrRecentFileNames.at(i).c_str());
                        }
                    }
                    else
                        InsertMenu( hMenu2, 0, MF_BYPOSITION
                                , 0, _T("Doesn't contain any recent file record."));
                }

                MENUITEMINFO stMenuItem;
                memset( &stMenuItem, 0, sizeof(MENUITEMINFO));
                stMenuItem.cbSize = sizeof(MENUITEMINFO);
                stMenuItem.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_SUBMENU | MIIM_ID;
                stMenuItem.cch = _tcslen( _T("Recent Files") );
                stMenuItem.dwTypeData = _T("Recent Files");
                stMenuItem.fType = MF_STRING;
                stMenuItem.hSubMenu = hMenu2;
                stMenuItem.wID = 1977;

                dwValue = InsertMenuItem( hMenu1, MENU_INDEX_NO, TRUE, &stMenuItem);
                dwValue = DrawMenuBar( hWnd );

                dwValue = DestroyMenu( hMenu2 );
            }
        }
    }

    return 0;
}

int AppendRecentFileMenu( HWND hWnd )
{
    UpdateRegisterValues();
    UpdateMenuItems( hWnd );

    return 0;
}

int ADDRecentFileMenu( HWND hWnd, _tstring strFileName )
{
    if( hWnd )
    {
        HMENU hMenu = GetMenu( hWnd );
        if(hMenu)
        {
            HKEY hKey = NULL;
            DWORD dwBufLen;
            LONG lRet = RegOpenKeyEx( HKEY_CURRENT_USER,
                        REG_RECENT_KEY_VALUE,
                        0, KEY_QUERY_VALUE, &hKey );
            if( lRet != ERROR_SUCCESS )
            {
                if (ERROR_SUCCESS != (lRet = RegCreateKeyEx(HKEY_CURRENT_USER,
                                        REG_RECENT_KEY_VALUE, 
                                        0, NULL, REG_OPTION_NON_VOLATILE,
                                        KEY_WRITE, NULL, &hKey, &dwBufLen)) )
                {
                    return -1;
                }
            }
            else
                RegDeleteKey(hKey, _T("Recent Files"));

            RegCloseKey( hKey );
            hKey = NULL;

            if( !strFileName.empty() )
            {
                g_vstrRecentFileNames.insert( g_vstrRecentFileNames.begin(), strFileName );

                for(int j=1; j<(int)g_vstrRecentFileNames.size(); j++)
                {
                    if(!strFileName.compare(g_vstrRecentFileNames.at(j)))
                        g_vstrRecentFileNames.erase(g_vstrRecentFileNames.begin() + j );
                }
            }

            if(g_vstrRecentFileNames.size() > 6)
                g_vstrRecentFileNames.pop_back();

            if (ERROR_SUCCESS != (lRet = RegCreateKeyEx(HKEY_CURRENT_USER,
                                    REG_RECENT_KEY_NAME, 
                                    0, NULL, REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE, NULL, &hKey, &dwBufLen)) )
            {
                return -1;
            }

            if( lRet == ERROR_SUCCESS )
            {
                TCHAR chFileValues[1024];
                for(int i=0; i < (int)g_vstrRecentFileNames.size(); i++)
                {
                    _stprintf_s(chFileValues, 1023, _T("File %d"), i);
                    if (RegSetValueEx(hKey, chFileValues, 0, REG_SZ
                        , (LPBYTE) g_vstrRecentFileNames.at(i).c_str()
                        , (g_vstrRecentFileNames.at(i).length() + 1) * sizeof(TCHAR)))
                    {
                        //return;
                    }
                }

                RegCloseKey( hKey );

                AppendRecentFileMenu( hWnd );

                return 0;
            }
        }
    }

    return -1;
}

RECT RearrangeWindow(HWND hWnd)
{
	RECT rectClient, rectStatus, rc, rToolBar;
	UINT uStatusHeight, uClientAlreaHeight;

	SendMessage(g_hwndStatus, WM_SIZE, 0, 0);
	GetClientRect(hWnd, &rectClient);
	GetWindowRect(g_hwndStatus, &rectStatus);
	uStatusHeight = rectStatus.bottom - rectStatus.top;
	uClientAlreaHeight = rectClient.bottom;

	rc.left = rectClient.left;
	rc.right = rectClient.right;
	rc.top = rectClient.top;
	rc.bottom = (uClientAlreaHeight - uStatusHeight);

    GetClientRect(g_hWndToolbar, &rToolBar);
    MoveWindow(g_hWndToolbar, rc.left, rToolBar.top, rc.right-rc.left
        , rToolBar.bottom-rToolBar.left, TRUE);

	GetWindowRect(g_hWndToolbar, &rectClient);
    int nHeight = rectClient.bottom - rectClient.top;
    MoveWindow(g_hMDIClientWnd, rc.left, rc.top+nHeight, rc.right-rc.left
				, rc.bottom-rc.top-nHeight, TRUE);

	return rc;
}

HWND CreateToolBarWindow(HWND hwndParent) 
{ 
    HWND hwndTB;  
    TBBUTTON tbb[12];
    INITCOMMONCONTROLSEX icex;

    memset( tbb, 0, sizeof(tbb));

    // Ensure that the common control DLL is loaded. 
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    g_hToolBarImgList = (HIMAGELIST)ImageList_LoadImage( g_hinstance
                        , MAKEINTRESOURCE(IDB_TOOLBARBITMAP)
						, 15, 12, CLR_NONE, IMAGE_BITMAP
                        , LR_LOADTRANSPARENT);

    // Create a toolbar. 
    hwndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPWSTR) NULL, 
        WS_CHILD | CCS_ADJUSTABLE | TBSTYLE_LIST | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS
        , 0, 0, 0, 0, hwndParent, (HMENU) IDB_TOOLBARCTRL, g_hinstance, NULL); 

    // Send the TB_BUTTONSTRUCTSIZE message, which is required for 
    // backward compatibility. 
    SendMessage(hwndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

    // Fill the TBBUTTON array with button information, and add the 
    // buttons to the toolbar. The buttons on this toolbar have text 
    // but do not have bitmap images. 
    tbb[0].iBitmap = MAKELONG(0, 0); 
    tbb[0].idCommand = ID_FILE_OPEN; 
    tbb[0].fsState = TBSTATE_ENABLED; 
    tbb[0].fsStyle = BTNS_BUTTON; 
    tbb[0].dwData = 0;

    tbb[1].iBitmap = MAKELONG(1, 0); 
    tbb[1].idCommand = ID_FILE_CLOSE; 
    tbb[1].fsState = TBSTATE_ENABLED; 
    tbb[1].fsStyle = BTNS_BUTTON; 
    tbb[1].dwData = 0;

    tbb[2].iBitmap = MAKELONG(3, 0); 
    tbb[2].idCommand = IDM_PE_SAMMARY; 
    tbb[2].fsState = TBSTATE_ENABLED; 
    tbb[2].fsStyle = BTNS_BUTTON; 
    tbb[2].dwData = 0;

    tbb[3].iBitmap = MAKELONG(2, 0); 
    tbb[3].idCommand = IDM_PE_HEADERS; 
    tbb[3].fsState = TBSTATE_ENABLED; 
    tbb[3].fsStyle = BTNS_BUTTON; 
    tbb[3].dwData = 0;

    tbb[4].iBitmap = MAKELONG(4, 0); 
    tbb[4].idCommand = IDM_PE_DATA_SECTIONS; 
    tbb[4].fsState = TBSTATE_ENABLED; 
    tbb[4].fsStyle = BTNS_BUTTON; 
    tbb[4].dwData = 0;

    tbb[5].iBitmap = MAKELONG(11, 0); 
    tbb[5].idCommand = IDM_PE_IMPORT_TABLE; 
    tbb[5].fsState = TBSTATE_ENABLED;
    tbb[5].fsStyle = BTNS_BUTTON; 
    tbb[5].dwData = 0;

    tbb[6].iBitmap = MAKELONG(12, 0); 
    tbb[6].idCommand = IDM_PE_EXPORT_TABLE; 
    tbb[6].fsState = TBSTATE_ENABLED; 
    tbb[6].fsStyle = BTNS_BUTTON; 
    tbb[6].dwData = 0;

    tbb[7].iBitmap = MAKELONG(7, 0); 
    tbb[7].idCommand = IDM_MODULES_BINARY; 
    tbb[7].fsState = TBSTATE_ENABLED; 
    tbb[7].fsStyle = BTNS_BUTTON; 
    tbb[7].dwData = 0;

    tbb[8].iBitmap = MAKELONG(10, 0); 
    tbb[8].idCommand = IDM_PE_DISASSEMBLY; 
    tbb[8].fsState = TBSTATE_ENABLED; 
    tbb[8].fsStyle = BTNS_BUTTON; 
    tbb[8].dwData = 0;

    tbb[9].iBitmap = MAKELONG(13, 0); 
    tbb[9].idCommand = IDM_MODULES_HTML; 
    tbb[9].fsState = TBSTATE_ENABLED; 
    tbb[9].fsStyle = BTNS_BUTTON; 
    tbb[9].dwData = 0;

	tbb[10].iBitmap = MAKELONG(8, 0); 
    tbb[10].idCommand = IDM_LICENSE;
    tbb[10].fsState = TBSTATE_ENABLED; 
    tbb[10].fsStyle = BTNS_BUTTON; 
    tbb[10].dwData = 0;

    tbb[11].iBitmap = MAKELONG(9, 0); 
    tbb[11].idCommand = IDM_ABOUT; 
    tbb[11].fsState = TBSTATE_ENABLED; 
    tbb[11].fsStyle = BTNS_BUTTON; 
    tbb[11].dwData = 0;

    SendMessage(hwndTB, TB_SETIMAGELIST, 0, (LPARAM)g_hToolBarImgList);
    SendMessage(hwndTB, TB_ADDBUTTONS, (WPARAM) 12, (LPARAM) (LPTBBUTTON) &tbb); 
    SendMessage(hwndTB, TB_SETBUTTONSIZE, 0, (LPARAM)MAKELONG (16, 16));

    SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);

    ShowWindow(hwndTB, SW_SHOW); 
    return hwndTB; 
}

void ReadLicenseAggreement(int nFlag)
{
    HKEY hKey;
    DWORD dwBufLen, dwValue;
    LONG lRet;

    if(1 == nFlag)
    {
        nFlag = 0;
        lRet = RegOpenKeyEx( HKEY_CURRENT_USER,
            REG_RECENT_KEY_VALUE,
            0, KEY_QUERY_VALUE, &hKey );
        if( lRet == ERROR_SUCCESS )
        {
            DWORD dwType = REG_DWORD;
            lRet = RegQueryValueEx( hKey, _T("Aggreement"), NULL, &dwType,
                (LPBYTE) &dwValue, &dwBufLen);

            RegCloseKey( hKey );

            if(1 == dwValue)
                nFlag = 1;
        }
    }

    if(0 == nFlag)
    {
        if(IDYES != DialogBox(g_hinstance
            , MAKEINTRESOURCE(IDD_LICENSEBOX)
            , g_hWndMain, LicenseDlg))
        {
            lRet = RegOpenKeyEx( HKEY_CURRENT_USER,
                _T("Software\\CodeDebug\\PEViewer"),
                0, KEY_QUERY_VALUE, &hKey );
            if( lRet != ERROR_SUCCESS )
            {
                PostQuitMessage(0);
                return;
            }

            RegDeleteKey(hKey, _T("v0.0.0.1"));

            PostQuitMessage(0);
        }
        else
        {
            hKey = NULL;
            if (!RegCreateKeyEx(HKEY_CURRENT_USER,
                REG_RECENT_KEY_VALUE, 
                0, NULL, REG_OPTION_NON_VOLATILE,
                KEY_WRITE, NULL, &hKey, &dwBufLen)) 
            {
                dwValue = 1;
                if (RegSetValueEx(hKey, _T("Aggreement"), 0, REG_DWORD,
                    (LPBYTE) &dwValue, (DWORD) sizeof(DWORD)))
                {
                    //return;
                }
            }

            RegCloseKey(hKey);

        }
    }
}

CNikPEViewer * GetActivePEWindow(HWND hWnd
                                 , int &nIndex)
{
    nIndex = -1;
    CNikPEViewer * pNikPEViewer = NULL;
    for(int i=0;i<(int)g_vPEViewerWindList.size();i++)
    {
        pNikPEViewer = g_vPEViewerWindList.at(i);
        if(pNikPEViewer && (hWnd == pNikPEViewer->GetWindowHandle()))
        {
            nIndex = i;
            return pNikPEViewer;
        }
    }

    return NULL;
}

int OpenPEFileForView( _tstring strFileName
                      , HWND hWnd
                      , CNikPEViewer * pNikPEViewer)
{
    int nErrorCode = 0;
    if((nErrorCode = pNikPEViewer->DecodePEFile( strFileName )) >= 0)
    {
        pNikPEViewer->CreatePEFrameWindow(g_hinstance, g_hMDIClientWnd);
        g_vPEViewerWindList.push_back(pNikPEViewer);

        pNikPEViewer->InitPEFrameWindow();

        ADDRecentFileMenu( hWnd, strFileName );
    }
    else
    {
        if(-1 == nErrorCode)
            ::MessageBox(hWnd, _T("Fail to open the PE file [Invalid file handle]"), _T("Error"), MB_OK);
        else if(-2 == nErrorCode)
            ::MessageBox(hWnd, _T("Fail to open the PE file [Error reading the file]"), _T("Error"), MB_OK);
        else if(-3 == nErrorCode)
            ::MessageBox(hWnd, _T("Not a valid PE file [DOS signature is missing]"), _T("Error"), MB_OK);
        else if(-4 == nErrorCode)
            ::MessageBox(hWnd, _T("Not a valid PE 32-bit file [NT signature is missing - older version Executable file]"), _T("Error"), MB_OK);
		else if(-5 == nErrorCode)
            ::MessageBox(hWnd, _T("PE machine code not supported"), _T("Error"), MB_OK);
		else
            ::MessageBox(hWnd, _T("File is not supported"), _T("Error"), MB_OK);
    }

    return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
    switch( Message )
    {
    case WM_NOTIFY: 
        switch (((LPNMHDR) lParam)->code) 
        { 
        case TTN_GETDISPINFO: 
            { 
                TCHAR chResourceString[1024];
                chResourceString[1023] = 0;

                LPTOOLTIPTEXT lpttt; 

                lpttt = (LPTOOLTIPTEXT) lParam; 
                lpttt->hinst = g_hinstance; 

                // Specify the resource identifier of the descriptive 
                // text for the given button. 
                int idButton = lpttt->hdr.idFrom; 
                switch (idButton) 
                { 
                case ID_FILE_OPEN:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_FILE_OPEN );
                    //LoadString( g_hinstance, IDS_FILE_OPEN, chResourceString, 255);
                    //SendMessage( g_hwndStatus, SB_SETTEXT, 0, (LPARAM)chResourceString);
                    break;

                case ID_FILE_CLOSE:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_FILE_CLOSE );
                    break;

                case IDM_PE_SAMMARY:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_PE_SAMMARY );
                    break;

                case IDM_PE_HEADERS:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_PE_HEADERS );
                    break;

                case IDM_PE_DATA_SECTIONS:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_PE_DATA_SECTIONS );
                    break;

                case IDM_PE_IMPORT_TABLE:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_PE_IMPORT_TABLE );
                    break;

                case IDM_PE_EXPORT_TABLE:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_PE_EXPORT_TABLE );
                    break;

                case IDM_MODULES_BINARY:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_MODULES_BINARY );
                    break;

                case IDM_LICENSE:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_LICENSE );
                    break;

                case IDM_MODULES_HTML:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_MODULES_HTML );
                    break;

                case IDM_ABOUT:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_ABOUT );
                    break; 

                case IDM_PE_DISASSEMBLY:
                    lpttt->lpszText = MAKEINTRESOURCE( IDS_PE_DISASSEMBLY );
                    break;
                } 
                break; 
            }
        }
        return DefFrameProc( hWnd, g_hMDIClientWnd, Message, wParam, lParam );

        case WM_CREATE:
        {
            CLIENTCREATESTRUCT ccs;
            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 2 );
            ccs.idFirstChild = 0;
            g_hMDIClientWnd = CreateWindowEx(WS_EX_ACCEPTFILES, TEXT("MDICLIENT"), (LPCTSTR) NULL,
                                            WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                                            0, 0, 0, 0, hWnd, (HMENU) 0xCAC, g_hinstance, (LPSTR) &ccs );

			wpOrigMDIProc = (WNDPROC) SetWindowLong(g_hMDIClientWnd, GWL_WNDPROC, (LONG) SubclassMDIProc);

            ShowWindow(g_hMDIClientWnd, SW_SHOW);

			g_hwndStatus = CreateWindowEx(0,STATUSCLASSNAME,(LPCTSTR) NULL,WS_VISIBLE|SBARS_SIZEGRIP |SBT_TOOLTIPS|
										WS_CHILD,0, 0, 0, 0,hWnd,(HMENU) 0,
										g_hinstance, NULL);

            g_hWndToolbar = CreateToolBarWindow( hWnd );

            AppendRecentFileMenu( hWnd );
        }
        break;

		case WM_SIZE:
		{
			RearrangeWindow(hWnd);
            return 0;
		}
		break;

		case WM_COMMAND:
            switch( LOWORD( wParam ) )
			{
            case IDM_LICENSE:
                ReadLicenseAggreement(0);
                break;
            case IDM_ABOUT:
                DialogBox(g_hinstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

            case ID_FILE_CLOSE:
                if(g_pNikPEViewer)
                    SendMessage( g_pNikPEViewer->GetWindowHandle() , WM_CLOSE, (WPARAM)0, (LPARAM)0);
                break;

            case IDM_EXIT:
			    DestroyWindow(hWnd);
			break;

            case IDM_CASCADE_ICON:
				SendMessage( g_hMDIClientWnd , WM_MDICASCADE
                            , (WPARAM)MDITILE_SKIPDISABLED, (LPARAM)0);
				break;
			case IDM_ARRANGE_ICON:
				SendMessage( g_hMDIClientWnd 
                            , WM_MDIICONARRANGE, 0, 0);
				break;
			case IDM_TILE_HORIZONTAL:
				SendMessage( g_hMDIClientWnd , WM_MDITILE
                            , (WPARAM)MDITILE_HORIZONTAL, 0);
			break;
			case IDM_TILE_VERTICAL:
				SendMessage( g_hMDIClientWnd , WM_MDITILE
                            , (WPARAM)MDITILE_VERTICAL, 0);
			break;

            case IDM_GROUP_ALL_VIEW:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->SetAllViewsDefaultPos();
                break;

            case IDM_SHOW_ALL_VIEW:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEAllSectionView();
                break;

            case IDM_PE_SAMMARY:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPESammaryView();
                break;

            case IDM_PE_HEADERS:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEHeadersView();
                break;

            case IDM_PE_DATA_SECTIONS:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEDataSectionsView();
                break;

            case IDM_PE_IMPORT_TABLE:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEImportListView();
                break;

            case IDM_PE_EXPORT_TABLE:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEExportListView();
                break;

            case IDM_MODULES_BINARY:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEBinaryView();
                break;

            case IDM_MODULES_HTML:
                    if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowPEHtmlView();
                break;

            case IDM_PE_DISASSEMBLY:
                if(g_pNikPEViewer)
                        g_pNikPEViewer->ShowCodeDisassembly();
                break;

            case ID_FILE_OPEN:
				{
                    CNikPEViewer * pNikPEViewer = new CNikPEViewer;
                    if(pNikPEViewer)
                    {
                        _tstring strFileName = pNikPEViewer->OpenFileDialogBox( hWnd );
                        if(!strFileName.empty())
                        {
                            g_pNikPEViewer = pNikPEViewer;
                            OpenPEFileForView( strFileName, hWnd, pNikPEViewer );
                            SendMessage( g_hwndStatus, SB_SETTEXT, 0
                                , (LPARAM)g_pNikPEViewer->GetModuleFileName().c_str());
                        }
                    }
				}
				break;

            case RECENT_FILE_ID_START:
            case RECENT_FILE_ID_START + 1:
            case RECENT_FILE_ID_START + 2:
            case RECENT_FILE_ID_START + 3:
            case RECENT_FILE_ID_START + 4:
            case RECENT_FILE_ID_START + 5:
                {
                    DWORD dwIndex = LOWORD(wParam) - RECENT_FILE_ID_START;
                    if(dwIndex < g_vstrRecentFileNames.size())
                    {
                        _tstring strFileName = g_vstrRecentFileNames.at( dwIndex );
                        if(!strFileName.empty())
                        {
                            dwIndex = GetFileAttributes( strFileName.c_str() );
                            if(dwIndex != (DWORD)INVALID_HANDLE_VALUE)
                            {
                                CNikPEViewer * pNikPEViewer = new CNikPEViewer;
                                if(pNikPEViewer)
                                {
                                    g_pNikPEViewer = pNikPEViewer;
                                    OpenPEFileForView( strFileName, hWnd, pNikPEViewer );
                                    SendMessage( g_hwndStatus, SB_SETTEXT, 0
                                        , (LPARAM)g_pNikPEViewer->GetModuleFileName().c_str());
                                }
                            }
                            else
                            {
                                if(IDYES == MessageBox( hWnd
                                    , _T("Fail to locate the file.\nDo you want to remove it from the menu list?")
                                    , _T("Error"), MB_YESNO ))
                                {
                                    _tstring str;
                                    dwIndex = LOWORD(wParam) - RECENT_FILE_ID_START;
                                    g_vstrRecentFileNames.erase( g_vstrRecentFileNames.begin() + dwIndex );
                                    ADDRecentFileMenu( hWnd, str );
                                }
                            }
                        }
                    }
                }
                break;
			}
			break;

		case WM_DESTROY:
			{
                CNikPEViewer * pNikPEViewer = NULL;
                for(int i=0;i<(int)g_vPEViewerWindList.size();i++)
                {
                    pNikPEViewer = g_vPEViewerWindList.at(i);
                    if(pNikPEViewer)
                        delete pNikPEViewer;
                }
                g_vPEViewerWindList.clear();

				PostQuitMessage (0);
			}
	    break;

        default:
            return DefFrameProc( hWnd, g_hMDIClientWnd, Message, wParam, lParam );
        break;
    }
    return DefFrameProc( hWnd, g_hMDIClientWnd, Message, wParam, lParam );
}

// Subclass procedure 
LRESULT APIENTRY SubclassMDIProc(HWND hWnd, 
									UINT uMsg, 
									WPARAM wParam, 
									LPARAM lParam) 
{
	switch(uMsg)
	{
	case WM_DROPFILES:
			{
				HDROP hdrop = (HDROP) wParam;
				TCHAR strFile[MAX_FILESIZE];
				DragQueryFile (hdrop, 0, strFile, sizeof(strFile)) ;
				DragFinish (hdrop) ;

                CNikPEViewer * pNikPEViewer = new CNikPEViewer;
                if(pNikPEViewer)
                    OpenPEFileForView( strFile, hWnd, pNikPEViewer );
			}
			break;
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		if(MK_LBUTTON & wParam)
		{
		}
		break;
    case WM_PE_VIEWER_DISTORY:
        {
            int i = 0;
            CNikPEViewer * pNikPEViewer = GetActivePEWindow((HWND)lParam, i);
            if(pNikPEViewer 
                && (i >= 0))
            {
                g_vPEViewerWindList.erase(g_vPEViewerWindList.begin() + i);
                delete pNikPEViewer;
            }

            if(0 == g_vPEViewerWindList.size())
                SendMessage( g_hwndStatus, SB_SETTEXT, 0, (LPARAM)_T(""));
        }
        break;
	}
 
    return CallWindowProc(wpOrigMDIProc, hWnd, uMsg, wParam, lParam); 
}

int PASCAL WinMain(HINSTANCE hInstance
                   , HINSTANCE hPrevInstance
                   , LPSTR lpszCmdLine
                   , int // nCmdShow
)
{
    MSG         msg;
    WNDCLASS    wc;
    BOOL        bResult;

    GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);   

    if( ! hPrevInstance )
    {
        wc.style         = 0;
		wc.lpfnWndProc   = (WNDPROC)WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
        wc.hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_NIKPEVIEWER) );
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = 0;
		wc.lpszMenuName  = MAKEINTRESOURCE(IDC_NIKPEVIEWER);
		wc.lpszClassName = g_szAppName;
		bResult = RegisterClass( &wc );
		if(!bResult)
			return 0;
    }

    g_hinstance = hInstance;

    g_hWndMain = CreateWindow(
						g_szAppName,
						g_szTitle,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, 0,
						CW_USEDEFAULT, 0,
						NULL,
						NULL,
						hInstance,
						NULL );

    if( g_hWndMain == NULL )
    {
        return FALSE;
    }

    //g_cNikDisassembly.ParserIntructionSetFile( _T("c:\\asm.txt") );
    //g_cNikDisassembly.DumpSortedOpcodeFile( _T("c:\\Dumpfile.dat") );

    g_cNikDisassembly.LoadOpcodeResource( hInstance, IDB_OPCODE_FILE );


	//ShowWindow( g_hWndMain, nCmdShow );
    ShowWindow( g_hWndMain, SW_MAXIMIZE );
    UpdateWindow( g_hWndMain );
	//DragAcceptFiles (g_hWndMain, TRUE);

    ReadLicenseAggreement(1);

    OleInitialize(NULL);

    if(lpszCmdLine 
        && lpszCmdLine[0])
    {
        TCHAR strComandLine[1024];
        strComandLine[0] = strComandLine[1023] = 0;
        if('\"' == lpszCmdLine[0])
        {
            lpszCmdLine++;
            int npos = strlen(lpszCmdLine);
            if(npos > 2)
                lpszCmdLine[npos - 1] = 0;
        }
        _stprintf_s(strComandLine, 1023, _T("%S"), lpszCmdLine);
        CNikPEViewer * pNikPEViewer = new CNikPEViewer;
        if(pNikPEViewer)
            OpenPEFileForView( strComandLine, g_hWndMain, pNikPEViewer );
    }

	int stat;
    // Standard message loop
    while ((stat = GetMessage(&msg, (HWND) NULL, 0, 0))!=0)
    {
		if(stat == -1)
			return 0;
        if( !TranslateMDISysAccel(g_hMDIClientWnd, &msg) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    g_cNikDisassembly.CleanUp();


	OleUninitialize();

    GdiplusShutdown(gdiplusToken);

    UnregisterClass(g_szAppName,hInstance);
    return msg.wParam;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK LicenseDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        {
            ///// certer the dialog box
            RECT rc;
            GetWindowRect(GetDesktopWindow(), &rc);
            int nWidth = (rc.right - rc.left)/2;
            int nHeight = (rc.bottom - rc.top)/2;

            GetWindowRect(hDlg, &rc);

            SetWindowPos(hDlg, NULL, nWidth-((rc.right - rc.left)/2)
                        , nHeight - ((rc.bottom - rc.top)/2)
                        , 0, 0, SWP_NOSIZE);

            ///// set the edit text
            HWND hEdit = GetDlgItem( hDlg, IDD_EDITLICENSE);
            if(hEdit)
            {
                SetWindowText( hEdit, STR_LICENSE );
            }
		    return (INT_PTR)TRUE;
        }

	case WM_COMMAND:
        if ((LOWORD(wParam) == IDYES) 
            || (LOWORD(wParam) == IDNO) 
            || (LOWORD(wParam) == IDCANCEL))
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
