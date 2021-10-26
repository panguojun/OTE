#include "stdafx.h"
#include "ProjectList.h"
#include "OTECommon.h"

CProjectList::CProjectList()
{
}

CProjectList::~CProjectList()
{
}

int CProjectList::Refresh( LPCTSTR pszPath )
{
    CString strPath = pszPath;
    if( strPath.Right( 1 ) != _T ( "\\" ) )
        strPath += _T ( "\\" );
	strPath += _T ( "*.fx" );

    HANDLE hFind;
    WIN32_FIND_DATA fd;
    int nCount = 0;

	//搜索
    if( ( hFind = ::FindFirstFile( strPath, &fd ) ) != INVALID_HANDLE_VALUE )
	{
		DeleteAllItems ();
		TCHAR szFullPath[ MAX_PATH ];
		::GetFullPathName( pszPath, sizeof( szFullPath ) / sizeof( TCHAR ), szFullPath, NULL );

		CString strTitle = szFullPath;
		//AfxGetMainWnd()->SetWindowText( strTitle );

        if( !( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
            AddItem( nCount++, &fd );

        while( ::FindNextFile( hFind, &fd ) )
		{
            if( !( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
                if( !AddItem( nCount++, &fd ) )
                    break;
        }

        ::FindClose( hFind );
    }

    return nCount;
}

BOOL CProjectList::AddItem( int nIndex, WIN32_FIND_DATA *pfd )
{
    ITEMINFO *pItem;
    try 
	{
        pItem = new ITEMINFO;
    }
    catch( CMemoryException *e )
	{
        e->Delete();
        return FALSE;
    }

    pItem->strFileName = pfd->cFileName;
    pItem->nFileSizeLow = pfd->nFileSizeLow;
    pItem->ftLastWriteTime = pfd->ftLastWriteTime;

    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
    lvi.iItem = nIndex; 
    lvi.iSubItem = 0; 
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.iImage = nIndex;
    lvi.lParam = ( LPARAM )pItem;

    if( InsertItem( &lvi ) == -1 )
        return FALSE;

	POINT pt;
	GetItemPosition( nIndex, &pt );
	pt.y += 20;
	SetItemPosition( nIndex, pt );

	delete pItem;
	pItem = NULL;

    return TRUE;
}

void CProjectList::FreeItemMemory()
{
    int nCount = GetItemCount();
    if( nCount )
	{
        for( int i = 0; i < nCount; ++i )
            delete ( ITEMINFO* )GetItemData( i );
    }
}

void CProjectList::LoadFileName( char *szFileName )
{
	FILE *fp;
	int line = 0, iFileNum = 0;
	char szFile[ MAX_PATH ] = "";

	if( NULL == ( fp = fopen( OTE::OTEHelper::GetResPath("ResConfig", "WaterCFGFile"), "r" ) ) )
	{
		::MessageBox( NULL, "打开水类型列表错误！", NULL, 0 );
		return;
	}

	DeleteAllItems();

	while( !feof( fp ) )
	{
		::fgets( szFile, sizeof( szFile ), fp );
		if( szFile[ 0 ] == '*' ) return;
		if( szFile[ 0 ] == '<' ) continue;
		if( szFile[ 0 ] == '/' ) continue;

		char szName[ MAX_PATH ] = "", szMiniature[MAX_PATH ] = "", szTexturePath[ MAX_PATH ] = "";
		int index = 0;
		::sscanf( szFile, "%d %s %s %s %d", &index, szName, szMiniature, szTexturePath, &iFileNum );

		AddItem( line, szName );
		++line;
	}
	
	::fclose( fp );	
}

BOOL CProjectList::AddItem( int nIndex, char *szName )
{
    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
    lvi.iItem = nIndex; 
    lvi.iSubItem = 0; 

	lvi.pszText = szName;
	lvi.iImage = 0;

    if( InsertItem( &lvi ) == -1 )
        return FALSE;

	POINT pt;
	GetItemPosition( nIndex, &pt );
	pt.y += 20;
	SetItemPosition( nIndex, pt );

    return TRUE;
}

BEGIN_MESSAGE_MAP( CProjectList, CListCtrl )
END_MESSAGE_MAP()

// TextureDialog 消息处理程序