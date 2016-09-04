
#pragma once

class CNikPEVTreeViewCtrl
{
    HWND            m_hWnd;
    HINSTANCE       m_hInstance;
    HIMAGELIST      m_hImgList;

public:
    CNikPEVTreeViewCtrl();
    virtual ~CNikPEVTreeViewCtrl();

    HWND GetWindowHandle()
    {
        return m_hWnd;
    }

    int DispalyBinaryInfo();
    int SetRootItems(void *);
    _tstring SetTreeItemStr(void * );
    int GetTreeItemIndex( HTREEITEM );
    int SetBinarySelection(_tstring );
    LRESULT Tree_Notify(HWND , LPARAM );
    LPARAM GetTreeItemlParam( HTREEITEM );
    _tstring GetTreeItemName( HTREEITEM );
    int GetSectionOffsetDiff(void *, int );
    int AddResourceItems( void *, HTREEITEM );
    int AddResourceString( void * , _tstring & );
    HWND CreatePETreeCtrl(HINSTANCE , HWND , RECT );
    HTREEITEM AddChildItems(HTREEITEM , _tstring , void * );
};