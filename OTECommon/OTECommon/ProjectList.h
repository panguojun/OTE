#ifndef _PROJECTLIST_H
#define _PROJECTLIST_H

//������¼�б����ѡ
struct ObjectInfo
{
	int iSelectedItem;//��ѡ
	int iType;//��ѡ����
	CString strFileName;//��ѡ����
	bool bChange;//ֻ��һ��

	ObjectInfo(): iSelectedItem( -1 ), bChange( false ), strFileName( "��" ), iType( 0 ) {}
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