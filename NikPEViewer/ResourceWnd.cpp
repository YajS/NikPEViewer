
#include "stdafx.h"
#include "ResourceWnd.h"
#include "MenuWnd.h"
#include "PEViewer.h"

// VKey Lookup Support 
 
#define MAXKEYS 25 
 
typedef struct _VKEYS { 
    TCHAR *pKeyName; 
    TCHAR *pKeyString; 
} VKEYS; 
 
VKEYS vkeys[MAXKEYS] = { 
    _T("BkSp"),     _T("Back Space"), 
    _T("PgUp"),     _T("Page Up"), 
    _T("PgDn"),     _T("Page Down"), 
    _T("End"),      _T("End"), 
    _T("Home"),     _T("Home"), 
    _T("Lft"),      _T("Left"), 
    _T("Up"),       _T("Up"), 
    _T("Rgt"),      _T("Right"), 
    _T("Dn"),       _T("Down"), 
    _T("Ins"),      _T("Insert"), 
    _T("Del"),      _T("Delete"), 
    _T("Mult"),     _T("Multiply"), 
    _T("Add"),      _T("Add"), 
    _T("Sub"),      _T("Subtract"), 
    _T("DecPt"),    _T("Decimal Point"), 
    _T("Div"),      _T("Divide"), 
    _T("F2"),       _T("F2"), 
    _T("F3"),       _T("F3"), 
    _T("F5"),       _T("F5"), 
    _T("F6"),       _T("F6"), 
    _T("F7"),       _T("F7"), 
    _T("F8"),       _T("F8"), 
    _T("F9"),       _T("F9"), 
    _T("F11"),      _T("F11"), 
    _T("F12"),      _T("F12") 
};

TCHAR * g_szTitleString[] = {_T("Resource ID")
							,_T("String value")
							,_T("Key value")
							,_T("Key flags")
							,_T("Key type")
							,_T("Version")
							,_T("Value")
							,_T("Message value")
							};

int CNikResource::RegisterResourceclass(HINSTANCE hInstance)
{
	WNDCLASS    wc;
	memset(&wc,0,sizeof(wc));

	wc.lpfnWndProc		= (WNDPROC)CNikResource::ResourceWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hIcon			= (HICON)LoadIcon(hInstance,MAKEINTRESOURCE(IDC_NIKPEVIEWER));
    wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName	= CLASS_RESOURCE;
	wc.lpszMenuName		= NULL;
    if (!RegisterClass(&wc)) 
		return FALSE;

	CMenuWnd::RegisterMenuclass(hInstance);

	return TRUE;
}

void CNikResource::DrawMenuItems(HDC hdc)
{
	if(hdc)
	{
		if(m_MenuItem.m_vMenuList.size())
		{
			int nBKMode = SetBkMode(hdc,TRANSPARENT);

			RECT rc;

			HFONT hOldFont = (HFONT)SelectObject(hdc,m_hFont);

			GetClientRect(m_hWnd,&rc);

			for(int i=0;i<(int)m_MenuItem.m_vMenuList.size();i++)
			{
				MENU_INFO stMenu = m_MenuItem.m_vMenuList.at(i);

                rc = m_MenuItem.m_vMenuList.at(i).m_rRect;

				DrawRectArea(hdc,rc,RGB(216,216,216));

				DrawText(hdc, stMenu.m_strText.data()
					, stMenu.m_strText.length(), &rc, DT_CENTER);

				OffsetRect(&rc,(rc.right-rc.left)+5,0);
			}

			SelectObject(hdc,hOldFont);

			SetBkMode(hdc,nBKMode);
		}
	}
}

void CNikResource::OnLbuttonDown(WPARAM //wParam
									, LPARAM lParam)
{
    if(m_hWnd)
        SetWindowPos( m_hWnd, HWND_TOP, 0 , 0, 0, 0
                        , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	RECT rc;
	for(int i=0;i<(int)m_MenuItem.m_vMenuList.size();i++)
	{
        rc = m_MenuItem.m_vMenuList.at(i).m_rRect;

		if(PtInRect(&rc,pt))
		{
			CreateMenuItemsWindow(i);
			break;
		}
	}
}

void CNikResource::OnLbuttonUP(WPARAM //wParam
								  , LPARAM //lParam
                                  )
{
}

void CNikResource::OnMouseMove(WPARAM //wParam
								  , LPARAM //lParam
                                  )
{
}

void CNikResource::DrawRectArea(HDC hdc
								 , RECT rc
								 ,COLORREF col)
{
	if(hdc)
	{
		HBRUSH hBrush = CreateSolidBrush(col);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);

		FillRect(hdc,&rc,hBrush);

		SelectObject(hdc,hOldBrush);

		DeleteObject(hBrush);
	}
}

LRESULT CALLBACK CNikResource::ResourceWndProc(HWND hwnd, UINT uMsg,
							WPARAM wParam, LPARAM lParam)
{
    CNikResource * pResourceEditor = (CNikResource *)GetWindowLong( hwnd, GWL_USERDATA );
    switch(uMsg)
    {
		case WM_SIZE:
		if(pResourceEditor)
			pResourceEditor->OnSize(wParam,lParam);
		break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd,&ps);
				if(pResourceEditor)
					pResourceEditor->DrawMenuItems(hdc);
				EndPaint(hwnd,&ps);
			}
			break;

	case WM_MOUSEMOVE:
		if(pResourceEditor)
			pResourceEditor->OnMouseMove(wParam,lParam);
		break;

	case WM_LBUTTONDOWN:
		if(pResourceEditor)
		{
			SetCapture(hwnd);
			pResourceEditor->OnLbuttonDown(wParam,lParam);
		}
		break;

	case WM_LBUTTONUP:
		if(pResourceEditor)
		{
			ReleaseCapture();
			pResourceEditor->OnLbuttonUP(wParam,lParam);
		}
		break;

    case WM_CLOSE:
        ShowWindow( hwnd, SW_HIDE );
        break;

	case WM_DESTROY:
        return DefWindowProc( hwnd, uMsg, wParam, lParam );

	default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
	}

	return 0;
}

void CNikResource::OnSize(WPARAM //wParam
							, LPARAM lParam
                            )
{
    RECT rc;
	rc.left = 0;
	rc.right = LOWORD(lParam);
	rc.top = 0;
	rc.bottom = HIWORD(lParam);

    if(m_hListView)
    {
        int nWidth = rc.right-rc.left;
        SetWindowPos( m_hListView, NULL, rc.left, rc.top
            , nWidth, rc.bottom-rc.top, NULL );

        LVCOLUMN lv;
        memset( &lv, 0, sizeof(LVCOLUMN));
        lv.mask = LVCF_WIDTH;
        int nCount = 0, nSuccess = 0;
        do
        {
            nSuccess = ListView_GetColumn( m_hListView, nCount, &lv);
            if(FALSE == nSuccess)
                break;

            nCount++;
        }while(nSuccess);

        nWidth = nWidth/nCount;

        for(int i=0;i<nCount;i++)
            ListView_SetColumnWidth( m_hListView, i, nWidth );
    }
}

HWND CNikResource::CreateResourceWindow(HWND hParent)
{
    m_hParentWnd = hParent;

    DWORD dwStyle = WS_TABSTOP |
                    WS_VISIBLE |
                    WS_SIZEBOX |
                    WS_CAPTION |
                    WS_SYSMENU |
                    //WS_VSCROLL | 
                    //WS_HSCROLL |
                    WS_CHILDWINDOW |
                    WS_CLIPSIBLINGS |
                    0;

    m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW
                            , CLASS_RESOURCE
						    , RESOURCE_CAPTION
						    , dwStyle
						    , 200, 100, 200, 400
						    , hParent
                            , NULL
						    , GetWindowInstance(hParent)
						    , (LPARAM)NULL );

    return m_hWnd;
}

BOOL CNikResource::InitMessageView(HWND hWndListView)
{
	RECT rc;
	GetClientRect(m_hWnd,&rc);

	ListView_DeleteAllItems(hWndListView);

    LVCOLUMN lvc;
    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text, and subitem members
    // of the structure are valid. 

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 	  
    // Add the columns.
    lvc.iSubItem = 0;
    lvc.pszText = g_szTitleString[0];	
	lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 0, &lvc) == -1) 
        return FALSE;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 1;
    lvc.pszText = g_szTitleString[7];	
    lvc.cx = (rc.right >> 2) * 3;     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 1, &lvc) == -1) 
        return FALSE;

	LVITEM lvI;
	lvI.mask = LVIF_TEXT; 
	lvI.state = 0; 
	lvI.stateMask = 0;

	int nEnteries = 0;

	int nNode = 0;

	if(m_pResource 
		&& ((nEnteries = m_pResource->m_vDirList.size()) > 0))
	{
		for(int j=0;j<nEnteries;j++)
		{
			RESOURCELIST * pRes = m_pResource->m_vDirList.at(j);
			if(NULL == pRes)
				continue;

			DWORD * pdwOffset = (DWORD *)&m_pBuffer[pRes->m_nOffsetToData];
			int nMessageEnteries = pdwOffset[0];
			MESSAGE_RESOURCE_BLOCK * pMessage = (MESSAGE_RESOURCE_BLOCK *)(pdwOffset + 1);
			TCHAR chBuf[4096];
            chBuf[0] = 0;
			LVITEM lvI;
			for(int i=0;i<nMessageEnteries;i++)
			{
				MESSAGE_RESOURCE_ENTRY * pMessageValue 
					= (MESSAGE_RESOURCE_ENTRY *)&m_pBuffer[pRes->m_nOffsetToData
																+ pMessage->OffsetToEntries];
				lvI.mask = LVIF_TEXT;
				lvI.iItem = nNode;
				lvI.iSubItem = 0;

				chBuf[0] = 0;
				_stprintf_s(chBuf,4095,_T("0x%X"),pMessage->LowId);

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_InsertItem(hWndListView, &lvI) == -1)
					return NULL;

				lvI.iSubItem = 1;

				chBuf[0] = 0;

				BYTE * pByte = (BYTE *)&chBuf[0];

				int k;
				for(k=0;k<pMessageValue->Length;k++)
					pByte[k] = pMessageValue->Text[k];

				pByte[k] = 0;
				pByte[k+1] = 0;

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
					return NULL;

				pMessage++;
			}
		}
	}

	return 0;
}

BOOL CNikResource::InitVersionView(HWND hWndListView)
{
	RECT rc;
	GetClientRect(m_hWnd,&rc);

	ListView_DeleteAllItems(hWndListView);

	LVCOLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
    lvc.pszText = g_szTitleString[5];	
    lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 0, &lvc) == -1) 
        return FALSE;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 1;
    lvc.pszText = g_szTitleString[6];	
    lvc.cx = (rc.right >> 2) * 2;     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 1, &lvc) == -1) 
        return FALSE;

	//int nEnteries = 0;

	if(m_pResource 
		/*&& ((nEnteries = m_pResource->m_vDirList.size()) > 0)*/)
	{
		//for(int j=0;j<nEnteries;j++)
		{
			//RESOURCELIST * pRes = m_pResource->m_vDirList.at(j);
            RESOURCELIST * pRes = m_pResource;
			if(NULL == pRes)
				//continue;
                return 0;
			WORD * pdwLen = (WORD *)&m_pBuffer[pRes->m_nOffsetToData+2];
			DWORD * pdwOffset = (DWORD *)&m_pBuffer[pRes->m_nOffsetToData];
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

				int nNode = 0;

				LVITEM lvI;
				lvI.mask = LVIF_TEXT;
				lvI.iItem = nNode;
				lvI.iSubItem = 0;

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("File Version"));

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_InsertItem(hWndListView, &lvI) == -1)
					return NULL;

				lvI.iSubItem = 1;

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("%d.%d.%d.%d"),pstVerFixInfo->dwFileVersionMS>>16
														,pstVerFixInfo->dwFileVersionMS&0xFFFF
														,pstVerFixInfo->dwFileVersionLS>>16
														,pstVerFixInfo->dwFileVersionLS&0xFFFF);

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
					return NULL;

				nNode++;

				lvI.mask = LVIF_TEXT;
				lvI.iItem = nNode;
				lvI.iSubItem = 0;

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("Product Version"));

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_InsertItem(hWndListView, &lvI) == -1)
					return NULL;

				lvI.iSubItem = 1;

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("%d.%d.%d.%d"),pstVerFixInfo->dwProductVersionMS>>16
														,pstVerFixInfo->dwProductVersionMS&0xFFFF
														,pstVerFixInfo->dwProductVersionLS>>16
														,pstVerFixInfo->dwProductVersionLS&0xFFFF);

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
					return NULL;

				nNode++;

				lvI.mask = LVIF_TEXT;
				lvI.iItem = nNode;
				lvI.iSubItem = 0;

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("File OS"));

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_InsertItem(hWndListView, &lvI) == -1)
					return NULL;

				lvI.iSubItem = 1;

				chBuf[0] = 0;
                _stprintf_s(chBuf,1023,CNikPEViewer::GetVersionOSName(pstVerFixInfo->dwFileOS).data());

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
					return NULL;

				nNode++;

				lvI.mask = LVIF_TEXT;
				lvI.iItem = nNode;
				lvI.iSubItem = 0;

				chBuf[0] = 0;
				_stprintf_s(chBuf,1023,_T("File type"));

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_InsertItem(hWndListView, &lvI) == -1)
					return NULL;

				lvI.iSubItem = 1;

				chBuf[0] = 0;
                _stprintf_s(chBuf,1023,CNikPEViewer::GetVerResourceType(pstVerFixInfo->dwFileType).data());

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
					return NULL;

				int nSize = (DWORD)pstVerFixInfo - (DWORD)(&m_pBuffer[pRes->m_nOffsetToData]);

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
								nNode++;
								lvI.mask = LVIF_TEXT;
								lvI.iItem = nNode;
								lvI.iSubItem = 0;

								lvI.pszText = chBuf; // sends an LVN_GETDISP message.

								if(ListView_InsertItem(hWndListView, &lvI) == -1)
									return NULL;

								lvI.iSubItem = 1;

								_stprintf_s(chBuf,1023,_T("%s"),(TCHAR *)(pdwOffset+((nLen+3)/4)));

								lvI.pszText = chBuf; // sends an LVN_GETDISP message.

								if(ListView_SetItem(hWndListView, &lvI) == -1)
									return NULL;
							}

							pdwOffset += ((pResString->wLength+3)/4);
							nSize -= ((pResString->wLength+3)/4);
						}
						else
						{
							pdwOffset += ((nLen+3)/4);
							nSize -= ((nLen+3)/4);

                            int nTemp = CNikPEViewer::GetHexValues(chBuf);
							if(nTemp 
								&& (0 == pResString->wValueLength))
							{
								chBuf[0] = 0;
                                _stprintf_s(chBuf,1023,CNikPEViewer::GetVerResLanguage(nTemp).data());

								if(chBuf[0])
								{
									/////////////////////////// blank item //////////////
									nNode++;
									lvI.mask = LVIF_TEXT;
									lvI.iItem = nNode;
									lvI.iSubItem = 0;

									lvI.pszText = _T(""); // sends an LVN_GETDISP message.

									if(ListView_InsertItem(hWndListView, &lvI) == -1)
										return NULL;
									/////////////////////////// blank item //////////////

									nNode++;
									lvI.mask = LVIF_TEXT;
									lvI.iItem = nNode;
									lvI.iSubItem = 0;

									lvI.pszText = _T("Language block"); // sends an LVN_GETDISP message.

									if(ListView_InsertItem(hWndListView, &lvI) == -1)
										return NULL;

									lvI.iSubItem = 1;

									lvI.pszText = chBuf; // sends an LVN_GETDISP message.

									if(ListView_SetItem(hWndListView, &lvI) == -1)
										return NULL;
								}
							}
						}
					}
					pResString = (RESOURCESTRING *)(pdwOffset);
				}
			}
		}
	}

	return 0;
}

BOOL CNikResource::InitAccelaratorListView(HWND hWndListView)
{
	RECT rc;
	GetClientRect(m_hWnd,&rc);

	ListView_DeleteAllItems(hWndListView);

    LVCOLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
    lvc.pszText = g_szTitleString[0];	
    lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 0, &lvc) == -1) 
        return FALSE;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 1;
    lvc.pszText = g_szTitleString[2];	
    lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 1, &lvc) == -1) 
        return FALSE;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 2;
    lvc.pszText = g_szTitleString[3];	
    lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 2, &lvc) == -1) 
        return FALSE;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 3;
    lvc.pszText = g_szTitleString[4];	
    lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 3, &lvc) == -1) 
        return FALSE;

	if(m_pResource 
		/*&& ((nEnteries = m_pResource->m_vDirList.size()) > 0)*/)
	{
		LVITEM lvI;
		lvI.mask = LVIF_TEXT; 
		lvI.state = 0; 
		lvI.stateMask = 0;

		//for(int j=0;j<nEnteries;j++)
		{
			//RESOURCELIST * pRes = m_pResource->m_vDirList.at(j);
			RESOURCELIST * pRes = m_pResource;
			if(NULL == pRes)
				//continue;
				return 0;

			int nSize = pRes->m_nSize / sizeof(ACCELTABLEENTRY);

			TCHAR chBuf[1024];
            chBuf[0] = 0;

			ACCELTABLEENTRY * pAccel = (ACCELTABLEENTRY *)&m_pBuffer[pRes->m_nOffsetToData];
			for(int i=0;i<nSize;i++)
			{
				lvI.mask = LVIF_TEXT;
				lvI.iItem = i;
				lvI.iSubItem = 0;

				_stprintf_s(chBuf,1023,_T("0x%X"),pAccel->wId);

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_InsertItem(hWndListView, &lvI) == -1)
					return NULL;

				lvI.iSubItem = 1;

				chBuf[0] = 0;
				if(pAccel->fFlags & FVIRTKEY)
					_stprintf_s(chBuf,1023,_T("0x%X"),pAccel->wAnsi);
				else
					_stprintf_s(chBuf,1023,_T("\'%c\'"),pAccel->wAnsi);

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
						return NULL;

				lvI.iSubItem = 2;

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

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
						return NULL;

				lvI.iSubItem = 3;

				chBuf[0] = 0;
				if(pAccel->fFlags & FVIRTKEY)
					_stprintf_s(chBuf,1023,_T("VIRTKEY"));
				else
					_stprintf_s(chBuf,1023,_T("ASCII"));

				lvI.pszText = chBuf; // sends an LVN_GETDISP message.

				if(ListView_SetItem(hWndListView, &lvI) == -1)
						return NULL;

				if(pAccel->fFlags & 0x80)
					break;

				pAccel++;
			}
		}
	}

	return 0;
}

BOOL CNikResource::InitStringListView(HWND hWndListView) 
{
	RECT rc;
	GetClientRect(m_hWnd,&rc);

	ListView_DeleteAllItems(hWndListView);

    LVCOLUMN lvc;
    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text, and subitem members
    // of the structure are valid. 

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 	  
    // Add the columns.
    lvc.iSubItem = 0;
    lvc.pszText = g_szTitleString[0];	
	lvc.cx = (rc.right >> 2);     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 0, &lvc) == -1) 
        return FALSE;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 1;
    lvc.pszText = g_szTitleString[1];	
    lvc.cx = (rc.right >> 2) * 3;     // width of column in pixels
        
	lvc.fmt = LVCFMT_LEFT;  // center-aligned column

    if (ListView_InsertColumn(hWndListView, 1, &lvc) == -1) 
        return FALSE;

	LVITEM lvI;
	lvI.mask = LVIF_TEXT; 
	lvI.state = 0; 
	lvI.stateMask = 0;

	int nNode = 0;

	if(m_pResource 
		/*&& ((nEnteries = m_pResource->m_vDirList.size()) > 0)*/)
	{
		//for(int j=0;j<nEnteries;j++)
		{
			//RESOURCELIST * pRes = m_pResource->m_vDirList.at(j);
			RESOURCELIST * pRes = m_pResource;
			//if(NULL == pRes)
			//	continue;

			BYTE * pBuf = &m_pBuffer[pRes->m_nOffsetToData];
			DWORD dwID = 0;
			TCHAR * pchName = pRes->m_chName;

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
            chBuf[0] = 0;
            chBuffer[0] = 0;

			int nOffset = 0;
			WORD * pdwOffset;

			for (int iCol = 0; iCol < 16; iCol++)
			{
				lvI.mask = LVIF_TEXT;
				lvI.iItem = nNode;
				lvI.iSubItem = 0;

				pdwOffset = (WORD *)&pBuf[nOffset];
				nOffset += 2;

                if(pdwOffset[0] > 4095)
                    return FALSE;

				if(pdwOffset[0])
				{
					chBuf[0] = 0;
					_stprintf_s(chBuf,_T("%d"),dwID+iCol-0x10);

					lvI.pszText = chBuf; // sends an LVN_GETDISP message.

					if(ListView_InsertItem(hWndListView, &lvI) == -1)
						return NULL;

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
					

					lvI.mask = LVIF_TEXT;
					lvI.iItem = nNode;
					lvI.iSubItem = 1;
					lvI.pszText = chBuf; // sends an LVN_GETDISP message.			

					if(ListView_SetItem(hWndListView, &lvI) == -1)
						return NULL;

					nOffset += (pdwOffset[0]*2);

					nNode++;
				}
			}
		}
	}

    return TRUE; 
}

int CNikResource::CreateMenuList(MENU_INFO * pMenu
								, int nOffset
								, int nSize
                                , BYTE * pBuffer)
{
	MENU_INFO * pMenInfo = (MENU_INFO *)&pBuffer[nOffset];

	pMenu->m_vMenuList.clear();

	while(nSize > nOffset)
	{
		MENU_INFO stMenuInfo;

		stMenuInfo.m_wFlag = pMenInfo->m_wFlag;
		if(pMenInfo->m_wFlag & MF_POPUP)
		{
			nOffset += 2;
			stMenuInfo.m_strText = (WCHAR *)&pBuffer[nOffset];
			nOffset += ((stMenuInfo.m_strText.length()+1) * sizeof(WCHAR));

			nOffset = CreateMenuList(&stMenuInfo,nOffset,nSize,pBuffer);
			pMenu->m_vMenuList.push_back(stMenuInfo);

			if(pMenInfo->m_wFlag & MF_END)
				return nOffset;
		}
		else
		{
			nOffset += 4;
			stMenuInfo.m_wMenuID = pMenInfo->m_wMenuID;
			stMenuInfo.m_strText = (WCHAR *)&pBuffer[nOffset];

			nOffset += ((stMenuInfo.m_strText.length()+1) * sizeof(WCHAR));
			pMenu->m_vMenuList.push_back(stMenuInfo);
		}

		if(pMenInfo->m_wFlag & MF_END)
			return nOffset;

		pMenInfo = (MENU_INFO *)&pBuffer[nOffset];
	}
	return nOffset;
}

int CNikResource::CreateMenuWindows(RESOURCELIST * pRes)
{
    if(NULL == m_hWnd)
        return -1;

	m_pResource = pRes;
	BYTE * pBuffer = &m_pBuffer[pRes->m_nOffsetToData];
	if(!pBuffer[0] && !pBuffer[1])
	{
		int nSize = pRes->m_nSize - 2;
		int nOffset = 0;
		nOffset += 4;
		MENU_INFO stMenuInfo;
		MENU_INFO * pMenu = &m_MenuItem;

		if(NULL == m_hFont)
		{
			LOGFONT lf;
			memset(&lf,0,sizeof(LOGFONT));

			_tcscpy_s(lf.lfFaceName, sizeof(lf.lfFaceName)/sizeof(TCHAR), MENU_FONT);
			lf.lfClipPrecision		= 2;
			lf.lfHeight				= -14;
			lf.lfQuality			= 1;
			lf.lfWeight				= 400;
			lf.lfOutPrecision		= 3;
			lf.lfPitchAndFamily		= 34;

			m_hFont = CreateFontIndirect(&lf);
		}

		CreateMenuList(pMenu, nOffset, nSize, pBuffer);

		CreateMenuRectSize();

		CreateMenuItemsWindow(0);

		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
    else
        return -2;

    return 0;
}

void CNikResource::CreateMenuRectSize()
{
	RECT rc;

	HDC hdc = GetDC(m_hWnd);
	if(NULL == hdc)
		return;

	HFONT hOldFont = (HFONT)SelectObject(hdc,m_hFont);

	GetClientRect(m_hWnd,&rc);

	for(int i=0;i<(int)m_MenuItem.m_vMenuList.size();i++)
	{
		MENU_INFO stMenu = m_MenuItem.m_vMenuList.at(i);

		DWORD dwSize = GetTabbedTextExtent(hdc,stMenu.m_strText.data()
								, stMenu.m_strText.length(),0,0);

		rc.right = rc.left + (dwSize & 0xFFFF) + 5;
		rc.bottom = rc.top + (dwSize>>16) + 5;

        m_MenuItem.m_vMenuList.at(i).m_rRect = rc;

		OffsetRect(&rc,(rc.right-rc.left)+5,0);
	}

	SelectObject(hdc,hOldFont);

	ReleaseDC(m_hWnd,hdc);
}

void CNikResource::CreateMenuItemsWindow(int nNo)
{
	if(m_pcMenuWnd)
	{
        for(int i=0;i<(int)m_vMenuWndList.size();i++)
        {
            if(m_pcMenuWnd == m_vMenuWndList.at(i))
            {
                m_vMenuWndList.erase(m_vMenuWndList.begin() + i);
                break;
            }
        }

		DestroyWindow(m_pcMenuWnd->GethWnd());
		delete m_pcMenuWnd;
		m_pcMenuWnd = NULL;
	}

	m_pcMenuWnd = new CMenuWnd;
	if(m_pcMenuWnd)
	{
		m_vMenuWndList.push_back(m_pcMenuWnd);
		m_pcMenuWnd->CreateMenuWindow(m_hWnd, NULL
            , m_MenuItem.m_vMenuList.at(0xFF & nNo));
	}
}

int CNikResource::CreateMessageWindows(RESOURCELIST * pRes)
{
    if(NULL == m_hWnd)
        return -1;

	m_pResource = pRes;

	CreateListViewWindow();

	InitMessageView(m_hListView);

    return 0;
}

int CNikResource::CreateVersionWindows(RESOURCELIST * pRes)
{
    if(NULL == m_hWnd)
        return -1;

	m_pResource = pRes;

	CreateListViewWindow();

	InitVersionView( m_hListView );

    return 0;
}

int CNikResource::CreateAccelaratorWindows(RESOURCELIST * pRes)
{
    if(NULL == m_hWnd)
        return -1;

	m_pResource = pRes;

	CreateListViewWindow();

	InitAccelaratorListView(m_hListView);

    return 0;
}

void CNikResource::CreateListViewWindow()
{
	HWND hWndListView;
	RECT rcl; 
	INITCOMMONCONTROLSEX icex;
    
	// Ensure that the common control DLL is loaded. 
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex); 
    
	// Create the list-view window in report view with label editing enabled.
	GetClientRect (m_hWnd, &rcl); 

    if(m_hListView)
	{
		DestroyWindow(m_hListView);
		m_hListView = NULL;
	}
    
	hWndListView = CreateWindow(WC_LISTVIEW, 
								_T(""), 
								WS_CHILD | LVS_REPORT 
								| WS_VISIBLE | LVS_NOSORTHEADER, 
								0, 
								0, 
								rcl.right - rcl.left, 
								rcl.bottom - rcl.top, 
								m_hWnd, 
								(HMENU) ID_LISTVIEW, 
								GetWindowInstance(m_hParentWnd), 
								NULL); 
    
	if (hWndListView == NULL) 
		return;

	SendMessage(hWndListView,LVM_SETEXTENDEDLISTVIEWSTYLE,0,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT);

	m_hListView = hWndListView;
}

int CNikResource::CreateStringWindows(RESOURCELIST * pRes)
{
    if(NULL == m_hWnd)
        return -1;

	m_pResource = pRes;

	CreateListViewWindow();

	if(FALSE == InitStringListView(m_hListView))
        return -1;

    return 0;
}

HWND CNikResource::SetResourceData(HWND hParent
                                   , RESOURCELIST * pRes
                                   , BYTE * pBuffer
                                   , int nFlag)
{
    if ((!pRes) 
        || (!pBuffer))
        return NULL;

    int nCount = 0xFFFF & (nFlag>>16);
    if(nCount > 0)
        nCount--;

    nFlag = 0xFFFF & nFlag;

    if(hParent 
        || (!m_hWnd))
    {
        CreateResourceWindow(hParent);
        SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)this );
        m_pBuffer = pBuffer;
    }

    int nRet = 0;
	SetFlag(nFlag);

    if(m_hWnd)
    {
        _tstring strCaption;

	    if(0x06 == nFlag)    		//	string
	    {
		    nRet = CreateStringWindows(pRes);
            strCaption = _T("Resource string Viewer");
	    }
	    else if(0x09 == nFlag)		//	Accelarator keys
	    {
		    nRet = CreateAccelaratorWindows(pRes);
            strCaption = _T("Resource accelarator Viewer");
	    }
	    else if(0x10 == nFlag)		//	Version
	    {
		    nRet = CreateVersionWindows(pRes);
            strCaption = _T("Resource version Viewer");
	    }
	    else if(0x0B == nFlag)		//	Message table
	    {
		    nRet = CreateMessageWindows(pRes);
            strCaption = _T("Resource message Viewer");
	    }
	    else if(0x04 == nFlag)		//	Menu
	    {
		    nRet = CreateMenuWindows(pRes);
            strCaption = _T("Resource menu Viewer");
	    }
        else
        {
            if(m_hWnd)
                ShowWindow( m_hWnd, SW_HIDE );
            return NULL;
        }

        if(pRes->m_chName[0])
        {
            strCaption += _T(" - ");
            strCaption += pRes->m_chName;
        }

        SetWindowText( m_hWnd, strCaption.c_str());

        if(nRet < 0)
            return NULL;
    }

    if(m_hWnd)
    {
        POINT pt;
        pt.x = 20 * nCount;
        pt.y = 20 * nCount;
	    ShowWindowPosition((NULL != hParent), pt);
    }

    return m_hWnd;
}
