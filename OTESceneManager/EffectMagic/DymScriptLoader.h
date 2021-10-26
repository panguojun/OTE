/***************************************************************************************************
文件名称:	DymScriptLoader.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效脚本读写
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <list>
#include <shlwapi.h>
#include <d3dx9.h>

class CDymScriptLoader
{
	struct KEY_NAME
	{
		char *str;
		int nSize;
		BOOL bLineEnd;
	};
	char *m_pFileBuffer;
	int m_nFileSize;
	std::list<KEY_NAME*> m_listKey;
	std::list<KEY_NAME*>::iterator m_it;

	void BeginLoad();
	void test();
	FILE *m_pSaveFile;
	char *m_pSaveFileBuffer;
	int m_nSaveFileBufferSize;
	int m_nSaveFileNowSize;
	int m_pSaveBracketStep;
public:
	CDymScriptLoader(void);
	~CDymScriptLoader(void);
	BOOL LoadFromFile(LPCTSTR strFileName);
	BOOL LoadFromMemory(void *pData,int datalen);
	char* GetKeyString(BOOL *pLineEnd);
	int GetKeyInt(BOOL *pLineEnd);
	float GetKeyFloat(BOOL *pLineEnd);
	bool GetKeyBool(BOOL *pLineEnd);
	void clear(void);
	D3DXVECTOR2 GetKeyVector2(BOOL * pLineEnd);
	D3DXVECTOR3 GetKeyVector3(BOOL * pLineEnd);
	D3DXVECTOR4 GetKeyVector4(BOOL * pLineEnd);
	BOOL SaveBegin(LPCTSTR fullpathfilename);
	BOOL SaveToMemoryBegin();
	void SaveEnd();
	void *SaveToMemoryEnd( int *pSizeOut );
	void SaveBracket_L();
	void SaveBracket_R();
	void SaveKeyString( LPCTSTR strKeyName , LPCTSTR string );
	void SaveKeyFloat( LPCTSTR strKeyName , float f );
	void SaveKeyBool( LPCTSTR strKeyName , bool b );
	void SaveKeyInt( LPCTSTR strKeyName , int t );
	void SaveKeyVector2( LPCTSTR strKeyName , D3DXVECTOR2 v );
	void SaveKeyVector3( LPCTSTR strKeyName , D3DXVECTOR3 v );
	void SaveKeyVector4( LPCTSTR strKeyName , D3DXVECTOR4 v );
};
