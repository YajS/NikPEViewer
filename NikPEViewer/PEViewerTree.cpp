
#include "Stdafx.h"
#include "Resource.h"
#include "PEViewer.h"
#include "PEViewerTree.h"

CNikPEVTreeViewCtrl::CNikPEVTreeViewCtrl()
{
    m_hWnd          =   NULL;
    m_hImgList      =   NULL;
    m_hInstance     =   NULL;
}

CNikPEVTreeViewCtrl::~CNikPEVTreeViewCtrl()
{
}

WNDPROC         g_wpOrigTreeProc;

// Subclass procedure 
LRESULT APIENTRY TreeSubclassProc( HWND hWnd
                                  , UINT uMsg
                                  , WPARAM wParam
                                  , LPARAM lParam) 
{
    switch( uMsg )
    {
    case WM_LBUTTONDOWN:
        SetWindowPos( hWnd, HWND_TOP, 0 , 0, 0, 0
                    , SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        return CallWindowProc(g_wpOrigTreeProc, hWnd, uMsg, wParam, lParam);

    case WM_CLOSE:
        ShowWindow ( hWnd, SW_HIDE );
        break;

    default:
        return CallWindowProc(g_wpOrigTreeProc, hWnd, uMsg, wParam, lParam);
    }
 
    return 0;
}

extern HIMAGELIST g_hToolBarImgList;

HWND CNikPEVTreeViewCtrl::CreatePETreeCtrl( HINSTANCE hInstance
                                           , HWND hParent
                                           , RECT rWindRect)
{
    COLORREF rgb = RGB(255,255,255);
	COLORREF rgb1 = RGB(0,0,255);
    m_hImgList = (HIMAGELIST)ImageList_LoadImage(hInstance
						, MAKEINTRESOURCE(IDB_TREEBITMAP)
						, 16, 5, CLR_NONE,IMAGE_BITMAP
						, LR_LOADTRANSPARENT);

	DWORD dwStyle = WS_TABSTOP | 
		            WS_CHILDWINDOW | 
		            WS_VISIBLE | 
		            TVS_HASLINES | 
		            TVS_HASBUTTONS |
                    WS_SIZEBOX |
                    WS_CAPTION |
                    WS_SYSMENU |
                    WS_CLIPSIBLINGS |
		            0;

    m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, WC_TREEVIEW, _T("PE Sammary")
                            , dwStyle, rWindRect.left, rWindRect.top
                            , rWindRect.right - rWindRect.left
                            , rWindRect.bottom - rWindRect.top
                            , hParent, (HMENU)IDC_TREEVIEW
                            , hInstance, NULL);

	if(!m_hWnd)
		return NULL;

    g_wpOrigTreeProc = (WNDPROC) SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) TreeSubclassProc);

	// attach ImageList to TreeView
    if (m_hImgList)
		TreeView_SetImageList(m_hWnd, g_hToolBarImgList, 0);

	TreeView_SetScrollTime(m_hWnd, 100);
	TreeView_SetBkColor(m_hWnd, rgb);
	TreeView_SetLineColor(m_hWnd, ~rgb);
	TreeView_SetTextColor(m_hWnd, rgb1);

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)GetWindowLong( hParent, GWL_USERDATA );
    SetRootItems( pNikPEViewer );

    return m_hWnd;
}

int CNikPEVTreeViewCtrl::GetSectionOffsetDiff(void * pNikPE
                                              , int nIndex)
{
    DWORD dwDiffOffset = 0;
    IMAGE_SECTION_HEADER stSectionHeader;
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)pNikPE;
    if(pNikPEViewer)
    {
        DWORD dwDataVirtualAdd = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[nIndex].VirtualAddress;
        DWORD dwSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[nIndex].Size;

		PIMAGE_NT_HEADERS64 pNTHeaders64 = (PIMAGE_NT_HEADERS64)&pNikPEViewer->GetImageNtHeaders();
		if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.Magic == 0x020b)
		{
			dwSize = pNTHeaders64->OptionalHeader.DataDirectory[nIndex].Size;
			dwDataVirtualAdd = pNTHeaders64->OptionalHeader.DataDirectory[nIndex].VirtualAddress;
		}

        for(int i=0; i<(int)pNikPEViewer->GetImageSectionHeaderList().size(); i++)
        {
            stSectionHeader = pNikPEViewer->GetImageSectionHeaderList().at(i);
            if( (stSectionHeader.VirtualAddress <= dwDataVirtualAdd)
                && ((stSectionHeader.VirtualAddress + stSectionHeader.SizeOfRawData) >= (dwDataVirtualAdd + dwSize)))
            {
                dwDiffOffset = stSectionHeader.VirtualAddress - stSectionHeader.PointerToRawData;
                return dwDiffOffset;
            }
        }
    }

    return 0;
}

int CNikPEVTreeViewCtrl::GetTreeItemIndex( HTREEITEM hSeltree )
{
    int nIndex = -1;

    if(hSeltree)
    {
        HTREEITEM hParentTree = TreeView_GetParent( m_hWnd, hSeltree);
        nIndex = 0;
        HTREEITEM hItem = TreeView_GetChild( m_hWnd, hParentTree );
        while(hItem)
        {
            if( hItem == hSeltree )
                break;
            nIndex++;
            hItem = TreeView_GetNextSibling( m_hWnd, hItem );
        }
    }

    return nIndex;
}

LPARAM CNikPEVTreeViewCtrl::GetTreeItemlParam( HTREEITEM hItem )
{
    TVITEM tv;
    memset(&tv,0,sizeof(TVITEM));
    tv.hItem = hItem;
    tv.mask = TVIF_PARAM;
    tv.pszText  = NULL;

    TreeView_GetItem( m_hWnd, &tv );

    return tv.lParam;
}

_tstring CNikPEVTreeViewCtrl::GetTreeItemName( HTREEITEM hItem )
{
    TCHAR szName[1024];
    szName[0] = szName[1023] = 0;

    TVITEM tv;
    memset(&tv,0,sizeof(TVITEM));
    tv.hItem = hItem;
    tv.mask = TVIF_TEXT;
    tv.pszText  = (LPWSTR)szName;
    tv.cchTextMax  = 1023;
    TreeView_GetItem( m_hWnd, &tv );

    return szName;
}

int CNikPEVTreeViewCtrl::SetBinarySelection( _tstring strItem )
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *)GetWindowLong( GetParent(m_hWnd), GWL_USERDATA );
    if(pNikPEViewer 
        && (!strItem.empty()))
    {
        CBinaryEditor * pBinaryEditor = pNikPEViewer->GetBinaryViewer().GetBinaryEditor();
        if(pBinaryEditor)
        {
            int nOffset = -1, nSize = 0;
            PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pBinaryEditor->GetBinBuffer();

			int nExtended = 0;
			if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.Magic == 0x020b)
				nExtended = 1;

			PIMAGE_NT_HEADERS64 pNTHeaders64 = (PIMAGE_NT_HEADERS64)&pNikPEViewer->GetImageNtHeaders();

            if(!strItem.compare( DOS_HEADER ))
            {
                nOffset = 0;
                nSize = sizeof(IMAGE_DOS_HEADER);
            }
            else if(!strItem.compare( NT_HEADERS ))
            {
                nOffset = dosHeader->e_lfanew;
                nSize = sizeof(IMAGE_NT_HEADERS);
            }
            else if(!strItem.compare( NT_SIGNATURE ))
            {
                nOffset = dosHeader->e_lfanew;
                nSize = sizeof(DWORD);
            }
            else if(!strItem.compare( FILE_HEADER ))
            {
                nOffset = sizeof(DWORD) + dosHeader->e_lfanew;
                nSize = sizeof(IMAGE_FILE_HEADER);
            }
            else if(!strItem.compare( OPTIONAL_HEADER ))
            {
                nOffset = sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) 
                            + dosHeader->e_lfanew;
				if(1 == nExtended)
					nSize = sizeof(IMAGE_OPTIONAL_HEADER64);
				else
					nSize = sizeof(IMAGE_OPTIONAL_HEADER32);
            }
            else if(!strItem.compare( DATA_DIRECTORIES ))
            {
                int nDataDirectorySize = sizeof(IMAGE_DATA_DIRECTORY) 
                                            * IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
                nOffset = dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) 
                                        - nDataDirectorySize;
                nSize = nDataDirectorySize;
            }
            else if(!strItem.compare( IMAGE_SECTIONS ))
            {
                nOffset = dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS);
				nSize = pNikPEViewer->GetImageNtHeaders().FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
            }
            else if(!strItem.compare( EXPORT_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 0);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[0].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[0].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[0].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[0].Size;
				}
            }
            else if(!strItem.compare( IMPORT_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 1);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[1].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[1].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[1].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[1].Size;
				}
            }
            else if(!strItem.compare( RESOURCE_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 2);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[2].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( EXCEPTION_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 3);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[3].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[3].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( SECURITY_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 4);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[4].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[4].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( BASERELOC_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 5);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[5].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[5].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( DEBUG_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 6);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[6].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[6].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( COPYRIGHT_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 7);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[7].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[7].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( GLOBALPTR_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 8);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[8].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[8].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( TLS_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 9);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[9].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[9].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( LOAD_CONFIG_TABLE ))
            {
                DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, 10);
				if(1 == nExtended)
				{
					nOffset = pNTHeaders64->OptionalHeader.DataDirectory[10].VirtualAddress - dwDiffOffset;
					nSize = pNTHeaders64->OptionalHeader.DataDirectory[10].Size;
				}
				else
				{
					nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].VirtualAddress - dwDiffOffset;
					nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[2].Size;
				}
            }
            else if(!strItem.compare( UNDEFINED_TABLE ))
            {
                HTREEITEM hSeltree = TreeView_GetSelection( m_hWnd );
                int j, nIndex = GetTreeItemIndex( hSeltree );
                j = 0;
				if(1 == nExtended)
				{
					for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
					{
						if(pNTHeaders64->OptionalHeader.DataDirectory[i].Size)
						{
							if(nIndex == j)
							{
								DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, i);
								nOffset = pNTHeaders64->OptionalHeader.DataDirectory[i].VirtualAddress - dwDiffOffset;
								nSize = pNTHeaders64->OptionalHeader.DataDirectory[i].Size;
								break;
							}
							j++;
						}
					}
				}
				else
				{
					for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
					{
						if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[i].Size)
						{
							if(nIndex == j)
							{
								DWORD dwDiffOffset = GetSectionOffsetDiff(pNikPEViewer, i);
								nOffset = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[i].VirtualAddress - dwDiffOffset;
								nSize = pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[i].Size;
								break;
							}
							j++;
						}
					}
				}
            }
            else
            {
	            HTREEITEM hSeltree = TreeView_GetSelection( m_hWnd );
                HTREEITEM hParentTree = TreeView_GetParent( m_hWnd, hSeltree);

                if(hParentTree)
                {
                    _tstring strName = GetTreeItemName( hParentTree );

                    if(!strName.compare( IMAGE_SECTIONS ))
                    {
                        int i = GetTreeItemIndex( hSeltree );

                        if(i < (int)pNikPEViewer->GetImageSectionHeaderList().size())
                        {
                            nOffset = pNikPEViewer->GetImageSectionHeaderList().at(i).PointerToRawData;
                            nSize = pNikPEViewer->GetImageSectionHeaderList().at(i).SizeOfRawData;
                        }
                    }
                    else
                    {
                        HTREEITEM hTempItem = hParentTree;
                        while(strName.compare( RESOURCE_TABLE ) 
                            && (hTempItem))
                        {
                            hTempItem = TreeView_GetParent( m_hWnd, hTempItem );
                            if(hTempItem)
                                strName = GetTreeItemName( hTempItem );
                            else
                                break;
                        }

                        if(!strName.empty())
                        {
                            RESOURCELIST * pResourceList = (RESOURCELIST *)GetTreeItemlParam( hParentTree );
                            if(pResourceList)
                            {
                                int nIndex = GetTreeItemIndex( hSeltree );
                                if(nIndex < (int)pResourceList->m_vDirList.size())
                                {
                                    pResourceList = pResourceList->m_vDirList.at( nIndex );
                                    if(pResourceList)
                                    {
                                        nSize = pResourceList->m_nSize;
                                        nOffset = pResourceList->m_nOffsetToData;

                                        if((nSize>0) && (nOffset >=0))
                                        {
                                            if(0 == SendMessage( GetParent( m_hWnd ), OPEN_RESOURCE_ITEM
                                                            , 0, (LPARAM)pResourceList))
                                            {
                                                nSize = 0;
                                                nOffset = -1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if((nSize > 0) 
                && (nOffset>=0)
                && ((nOffset+nSize) <= (int)pNikPEViewer->GetModuleFileSize()))
            {
                HWND hEditorWnd = pNikPEViewer->GetBinaryViewer().GetWindowHandle();

                SetWindowPos( hEditorWnd, HWND_TOP, 0, 0, 0, 0
                        , SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
                pBinaryEditor->SetBufferOffset( nOffset );
                pBinaryEditor->SetSelection( nOffset, nOffset+nSize);
                pBinaryEditor->SetScrollBar( hEditorWnd );

                InvalidateRect( hEditorWnd, NULL, TRUE);
                UpdateWindow( hEditorWnd );
            }
        }
    }

    return 0;
}

int CNikPEVTreeViewCtrl::DispalyBinaryInfo()
{
    HTREEITEM htree = TreeView_GetSelection( m_hWnd );

    _tstring strName = GetTreeItemName( htree );

    SetBinarySelection( strName );

    return 0;
}

LRESULT CNikPEVTreeViewCtrl::Tree_Notify(HWND // hWnd
                                         , LPARAM lParam)
{
	LPNMHDR  lpnmh = (LPNMHDR) lParam;

	switch(lpnmh->code)
	{
	case NM_RETURN:
	case NM_DBLCLK:
		{
			DispalyBinaryInfo();

			//SendMessage(GetParent(hWnd), ACTIVATE_CONTEXT_MENU, tv.lParam, ParentTv.lParam);
		}
		break;

	case TVN_GETDISPINFO:
		{
			//  LPNMTVDISPINFO lpdi = (LPNMTVDISPINFO)lParam;
		}
		break;

	case TVN_ITEMEXPANDING:
		{
			LPNMTREEVIEW   pnmtv = (LPNMTREEVIEW)lParam;

			switch(pnmtv->action)
			{
			case TVE_EXPAND:
				break;
			case TVE_COLLAPSE:
				break;
			}
		}
		break;

	case TVN_DELETEITEM:
		{
			// LPNMTREEVIEW   pnmtv = (LPNMTREEVIEW)lParam;
		}
		break;

	case TVN_SELCHANGED:
		break;
	}

	return 0;
}

HTREEITEM CNikPEVTreeViewCtrl::AddChildItems(HTREEITEM hParentItem
                                             , _tstring strItem
                                             , void * pItemPtr)
{
    TVITEM         tvItem;
    TVINSERTSTRUCT tvInsert;

    ZeroMemory(&tvItem, sizeof(TVITEM));
    tvItem.mask     = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
    tvItem.pszText  = (LPWSTR)strItem.c_str();
    tvItem.iImage = tvItem.iSelectedImage = 5;
    tvItem.lParam   = (LPARAM)pItemPtr;
    tvItem.cChildren= (0 != pItemPtr);

    tvInsert.hParent = hParentItem;
    tvInsert.hInsertAfter = TVI_LAST;
    tvInsert.item = tvItem;

    return TreeView_InsertItem( m_hWnd, &tvInsert);
}

int CNikPEVTreeViewCtrl::AddResourceItems( void * pResItems
                                          , HTREEITEM hTreeItem)
{
    HTREEITEM hItems;
    RESOURCELIST * pResItemsList = (RESOURCELIST *) pResItems;
    if( pResItemsList )
    {
        if(pResItemsList->m_vDirList.size())
        {
            hItems = AddChildItems( hTreeItem
                                    , pResItemsList->m_chName
                                    , pResItemsList );

            for(int i=0; i<(int)pResItemsList->m_vDirList.size(); i++)
            {
                AddResourceItems( pResItemsList->m_vDirList.at(i), hItems);
            }
        }
        else
            AddChildItems( hTreeItem, pResItemsList->m_chName, NULL );
    }

    return 0;
}

BOOL CNikPEVTreeViewCtrl::SetRootItems(void * pNikView)
{
    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(pNikPEViewer)
    {

	    TV_ITEM           tvItem;
	    TV_INSERTSTRUCT   tvInsert;
	    HTREEITEM         hParentItem;

	    //fill in the TVITEM structure for this item
	    tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

	    //text and images are done on a callback basis
        TCHAR chName[_MAX_PATH];
        chName[_MAX_PATH-1] = 0;
        _stprintf_s(chName,  _MAX_PATH-1, _T("%s [%d bytes]")
            , pNikPEViewer->GetModuleShortName().c_str()
            , pNikPEViewer->GetModuleFileSize());
        tvItem.pszText = (TCHAR *)chName;
	    tvItem.iImage = tvItem.iSelectedImage = 5;
	    tvItem.lParam = (LPARAM)NULL;

	    //assume the desktop has children
	    tvItem.cChildren = TRUE;

	    //fill in the TV_INSERTSTRUCT structure for this item
	    tvInsert.item = tvItem;
	    tvInsert.hInsertAfter = TVI_LAST;
	    tvInsert.hParent = TVI_ROOT;

	    //add the item
        hParentItem = TreeView_InsertItem(m_hWnd, &tvInsert);
    
        AddChildItems(hParentItem, DOS_HEADER, NULL);
        HTREEITEM hItem_1 = AddChildItems(hParentItem, NT_HEADERS, (void *)1);
        AddChildItems(hItem_1, NT_SIGNATURE, 0);
        AddChildItems(hItem_1, FILE_HEADER, 0);
        AddChildItems(hItem_1, OPTIONAL_HEADER, 0);
    
        HTREEITEM hItem = AddChildItems(hItem_1, DATA_DIRECTORIES, (void *)2);

		PIMAGE_NT_HEADERS64 pNTHeaders64 = (PIMAGE_NT_HEADERS64)&pNikPEViewer->GetImageNtHeaders();
		if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.Magic == 0x020b)
		{
			for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
			{
				if(pNTHeaders64->OptionalHeader.DataDirectory[i].Size)
				{
					if(0 == i)
						AddChildItems(hItem, EXPORT_TABLE, 0);
					else if(1 == i)
						AddChildItems(hItem, IMPORT_TABLE, 0);
					else if(2 == i)
					{
						RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();
						if(pResourceList)
						{
							HTREEITEM hResItem = AddChildItems(hItem, RESOURCE_TABLE, pResourceList);

							for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
								AddResourceItems( pResourceList->m_vDirList.at(i), hResItem );

							TreeView_Expand(m_hWnd, hResItem, TVE_EXPAND);
						}
					}
					else if(3 == i)
						AddChildItems(hItem, EXCEPTION_TABLE, 0);
					else if(4 == i)
						AddChildItems(hItem, SECURITY_TABLE, 0);
					else if(5 == i)
						AddChildItems(hItem, BASERELOC_TABLE, 0);
					else if(6 == i)
						AddChildItems(hItem, DEBUG_TABLE, 0);
					else if(7 == i)
						AddChildItems(hItem, COPYRIGHT_TABLE, 0);
					else if(8 == i)
						AddChildItems(hItem, GLOBALPTR_TABLE, 0);
					else if(9 == i)
						AddChildItems(hItem, TLS_TABLE, 0);
					else if(10 == i)
						AddChildItems(hItem, LOAD_CONFIG_TABLE, 0);
					else
						AddChildItems(hItem, UNDEFINED_TABLE, 0);
				}
			}
		}
		else
		{
			for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
			{
				if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.DataDirectory[i].Size)
				{
					if(0 == i)
						AddChildItems(hItem, EXPORT_TABLE, 0);
					else if(1 == i)
						AddChildItems(hItem, IMPORT_TABLE, 0);
					else if(2 == i)
					{
						RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();
						if(pResourceList)
						{
							HTREEITEM hResItem = AddChildItems(hItem, RESOURCE_TABLE, pResourceList);

							for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
								AddResourceItems( pResourceList->m_vDirList.at(i), hResItem );

							TreeView_Expand(m_hWnd, hResItem, TVE_EXPAND);
						}
					}
					else if(3 == i)
						AddChildItems(hItem, EXCEPTION_TABLE, 0);
					else if(4 == i)
						AddChildItems(hItem, SECURITY_TABLE, 0);
					else if(5 == i)
						AddChildItems(hItem, BASERELOC_TABLE, 0);
					else if(6 == i)
						AddChildItems(hItem, DEBUG_TABLE, 0);
					else if(7 == i)
						AddChildItems(hItem, COPYRIGHT_TABLE, 0);
					else if(8 == i)
						AddChildItems(hItem, GLOBALPTR_TABLE, 0);
					else if(9 == i)
						AddChildItems(hItem, TLS_TABLE, 0);
					else if(10 == i)
						AddChildItems(hItem, LOAD_CONFIG_TABLE, 0);
					else
						AddChildItems(hItem, UNDEFINED_TABLE, 0);
				}
			}
		}

        TreeView_Expand(m_hWnd, hItem, TVE_EXPAND);

        TCHAR chTemp[10];
        hItem = AddChildItems(hParentItem, IMAGE_SECTIONS, (void *)3);
        for(int i=0; i<(int)pNikPEViewer->GetImageSectionHeaderList().size(); i++)
        {
            memset(chTemp, 0, sizeof(chTemp));
            for(int j=0; j<8; j++)
            {
                chTemp[j] = pNikPEViewer->GetImageSectionHeaderList().at(i).Name[j];
            }
            if(0 == chTemp[0])
                AddChildItems(hItem, UNDEFINED_SECTION, 0);
            else
                AddChildItems(hItem, chTemp, 0);
        }

        //go ahead and expand this item
	    TreeView_Expand(m_hWnd, hParentItem, TVE_EXPAND);
        TreeView_Expand(m_hWnd, hItem_1, TVE_EXPAND);
        TreeView_Expand(m_hWnd, hItem, TVE_EXPAND);
    }

	return TRUE;
}

int CNikPEVTreeViewCtrl::AddResourceString( void * pResItems
                                          , _tstring & strHtmlText)
{
    RESOURCELIST * pResItemsList = (RESOURCELIST *) pResItems;
    if( pResItemsList )
    {
        if(pResItemsList->m_vDirList.size())
        {
            strHtmlText += _T("\r\n               ");
            strHtmlText += pResItemsList->m_chName;

            for(int i=0; i<(int)pResItemsList->m_vDirList.size(); i++)
                AddResourceString( pResItemsList->m_vDirList.at(i), strHtmlText);
        }
        else
        {
            strHtmlText += _T("\r\n               ");
            strHtmlText += pResItemsList->m_chName;
        }
    }

    return 0;
}

_tstring CNikPEVTreeViewCtrl::SetTreeItemStr(void * pNikView)
{
    _tstring strHtmlText;

    CNikPEViewer * pNikPEViewer = (CNikPEViewer *) pNikView;
    if(pNikPEViewer)
    {
	    //text and images are done on a callback basis
        TCHAR chName[_MAX_PATH];
        chName[_MAX_PATH-1] = 0;
        _stprintf_s(chName,  _MAX_PATH-1, _T("%s [%d bytes]")
            , pNikPEViewer->GetModuleShortName().c_str()
            , pNikPEViewer->GetModuleFileSize());
        strHtmlText = (TCHAR *)chName;
    
        strHtmlText += _T("\r\n");
        strHtmlText += DOS_HEADER;
        strHtmlText += _T("\r\n");
        strHtmlText += NT_HEADERS;
        strHtmlText += _T("\r\n     ");
        strHtmlText += NT_SIGNATURE;
        strHtmlText += _T("\r\n     ");
        strHtmlText += FILE_HEADER;
        strHtmlText += _T("\r\n     ");
        strHtmlText += OPTIONAL_HEADER;
    
        strHtmlText += _T("\r\n     ");
        strHtmlText += DATA_DIRECTORIES;

		//PIMAGE_NT_HEADERS pNTHeaders64 = (PIMAGE_NT_HEADERS64)&pNikPEViewer->GetImageNtHeaders();
		//if(pNikPEViewer->GetImageNtHeaders().OptionalHeader.Magic == 0x020b)
		{
			for(int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
			{
				//if(pNTHeaders64->OptionalHeader.DataDirectory[i].Size)
				{
					if(0 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += EXPORT_TABLE;
                    }
					else if(1 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += IMPORT_TABLE;
                    }
					else if(2 == i)
					{
						RESOURCELIST * pResourceList = (RESOURCELIST *)pNikPEViewer->GetResourceItemsList();
						if(pResourceList)
						{
                            strHtmlText += _T("\r\n          ");
                            strHtmlText += RESOURCE_TABLE;
							for(int i=0; i<(int)pResourceList->m_vDirList.size(); i++)
                            {
                                strHtmlText += _T("\r\n               ");
                                //AddResourceString(pResourceList->m_vDirList.at(i), strHtmlText);
                                strHtmlText += pResourceList->m_vDirList.at(i)->m_chName;
                            }
						}
					}
					else if(3 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += EXCEPTION_TABLE;
                    }
					else if(4 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += SECURITY_TABLE;
                    }
					else if(5 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += BASERELOC_TABLE;
                    }
					else if(6 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += DEBUG_TABLE;
                    }
					else if(7 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += COPYRIGHT_TABLE;
                    }
					else if(8 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += GLOBALPTR_TABLE;
                    }
					else if(9 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += TLS_TABLE;
                    }
					else if(10 == i)
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += LOAD_CONFIG_TABLE;
                    }
					else
                    {
                        strHtmlText += _T("\r\n          ");
                        strHtmlText += UNDEFINED_TABLE;
                    }
				}
			}
		}

        TCHAR chTemp[10];
        strHtmlText += _T("\r\n     ");
        strHtmlText += IMAGE_SECTIONS;

        for(int i=0; i<(int)pNikPEViewer->GetImageSectionHeaderList().size(); i++)
        {
            memset(chTemp, 0, sizeof(chTemp));
            for(int j=0; j<8; j++)
            {
                chTemp[j] = pNikPEViewer->GetImageSectionHeaderList().at(i).Name[j];
            }

            if(0 == chTemp[0])
            {
                strHtmlText += _T("\r\n          ");
                strHtmlText += UNDEFINED_SECTION;
            }
            else
            {
                strHtmlText += _T("\r\n          ");
                strHtmlText += chTemp;
            }
        }
    }

	return strHtmlText;
}

