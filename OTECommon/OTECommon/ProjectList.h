#ifndef _PROJECTLIST_H
#define _PROJECTLIST_H

//用来记录列表框所选
struct ObjectInfo
{
	int iSelectedItem;//所选
	int iType;//所选类型
	CString strFileName;//所选名字
	bool bChange;//只传一次

	ObjectInfo(): iSelectedItem( -1 ), bChange( false ), strFileName( "无" ), iType( 0 ) {}
};

typedef struct tagITEMINFO
{
    CString     strFileName; 
    DWORD       nFileSizeLow; 
    FILETIME    ftLastWriteTime; 
}ITEMINFO;

class CProjectList: public CListCtrl
{
public:
	CProjectList();
	virtual ~CProjectList();
	
	void LoadFileName( char *szFileName );

	int Refresh(LPCTSTR pszPath );
	void FreeItemMemory();

protected:
	BOOL AddItem( int nIndex, WIN32_FIND_DATA *pfd );
	BOOL AddItem( int nIndex, char *szName );

	DECLARE_MESSAGE_MAP()
};

#endif