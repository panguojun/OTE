/***************************************************************************************************
文件名称:	DymScriptLoader.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效脚本读写
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymscriptloader.h"

CDymScriptLoader::CDymScriptLoader(void)
{
	m_pFileBuffer = NULL;
	m_nFileSize = 0;
	//m_it = NULL; // ??
	m_it = m_listKey.begin();
	m_pSaveFile = NULL;
	m_pSaveFileBuffer = NULL;
	m_nSaveFileBufferSize = 0;
}

CDymScriptLoader::~CDymScriptLoader(void)
{
	clear();
}

BOOL CDymScriptLoader::LoadFromFile(LPCTSTR strFileName)
{
	FILE *p = fopen( strFileName , "rb" );
	if(!p)return FALSE;
	fseek(p,0,SEEK_END);
	m_nFileSize = ftell(p);
	fseek(p,0,SEEK_SET);
	m_pFileBuffer = new char[m_nFileSize];
	fread(m_pFileBuffer,m_nFileSize,1,p);
	fclose(p);

	BeginLoad();
	m_it = m_listKey.begin ();
	return TRUE;
}

BOOL CDymScriptLoader::LoadFromMemory(void *pData,int datalen)
{
	m_pFileBuffer = new char[datalen];
	m_nFileSize = datalen;
	memcpy( m_pFileBuffer , pData , datalen );

	BeginLoad();
	m_it = m_listKey.begin ();
	return TRUE;
}

void CDymScriptLoader::BeginLoad()
{
	if( m_pFileBuffer == NULL || m_nFileSize == 0 )return;
	char nameBuffer[256];
	int nLen = 0;
	BOOL bReadingUnicode=FALSE;
	BOOL bNewLine = TRUE;
	KEY_NAME *pOldKey = NULL;
	for(int i=0;i<m_nFileSize;i++)
	{
		if( m_pFileBuffer[i]=='/' && m_pFileBuffer[i+1]=='/' )
		{
			while( i<m_nFileSize && m_pFileBuffer[i]!='\n' )
			{
				i++;
			}
		}
		else
		{
			if( m_pFileBuffer[i] > 0x20 || m_pFileBuffer[i] < 0 || bReadingUnicode)
			{
				nameBuffer[nLen] = m_pFileBuffer[i];
				nLen++;
				if(bReadingUnicode==TRUE)bReadingUnicode=FALSE;
				else if(m_pFileBuffer[i]<0)bReadingUnicode=TRUE;
			}
			else
			{
				if(m_pFileBuffer[i]=='\n')bNewLine = TRUE;
				if(nLen>0)
				{
					KEY_NAME *p = new KEY_NAME;
					ZeroMemory( p , sizeof(KEY_NAME) );
					p->nSize = nLen;
					p->str = new char[nLen+1];
					if(pOldKey && bNewLine)
					{
						pOldKey->bLineEnd = TRUE;
					}
					bNewLine = FALSE;
					nameBuffer[nLen] = '\0';
					strcpy( p->str , nameBuffer );
					nLen = 0;
					m_listKey.push_back ( p );
					pOldKey = p;
				}
			}
		}
	}
}

void CDymScriptLoader::test ()
{
	for(std::list<KEY_NAME*>::iterator it=m_listKey.begin() ; it!=m_listKey.end () ; it++)
	{
		KEY_NAME *p = *it;
		rand();
	}
}
char* CDymScriptLoader::GetKeyString(BOOL *pLineEnd)
{
	if( m_it == m_listKey.end () )
		return NULL;
	KEY_NAME *p=*m_it;
	m_it++;
	if(pLineEnd)*pLineEnd = p->bLineEnd ;
	return p->str ;
}

int CDymScriptLoader::GetKeyInt(BOOL *pLineEnd)
{
	int n = 0;
	char *s = GetKeyString( pLineEnd );
	if( s )
		n = atoi ( s );
	return n;
}

float CDymScriptLoader::GetKeyFloat(BOOL *pLineEnd)
{
	float n = 0;
	char *s = GetKeyString( pLineEnd );
	if( s )
		n = (float)atof ( s );
	return n;
}

bool CDymScriptLoader::GetKeyBool(BOOL *pLineEnd)
{
	char *s = GetKeyString( pLineEnd );
	if( s )
		if( _stricmp( s , "true" ) == 0 )
			return true;
	return false;
}

void CDymScriptLoader::clear(void)
{
	for(std::list<KEY_NAME*>::iterator it=m_listKey.begin() ; it!=m_listKey.end () ; it++)
	{
		KEY_NAME *p = *it;
		if( p->str )delete[] p->str ;
		delete p;
	}
	m_listKey.clear ();
	delete[] m_pFileBuffer;
	m_pFileBuffer = NULL;
	m_nFileSize = 0;
}

D3DXVECTOR2 CDymScriptLoader::GetKeyVector2(BOOL * pLineEnd)
{
	D3DXVECTOR2 v;
	v.x = GetKeyFloat( NULL );
	v.y = GetKeyFloat( pLineEnd );
	return v;
}

D3DXVECTOR3 CDymScriptLoader::GetKeyVector3(BOOL * pLineEnd)
{
	D3DXVECTOR3 v;
	v.x = GetKeyFloat( NULL );
	v.y = GetKeyFloat( NULL );
	v.z = GetKeyFloat( pLineEnd );
	return v;
}

D3DXVECTOR4 CDymScriptLoader::GetKeyVector4(BOOL * pLineEnd)
{
	D3DXVECTOR4 v;
	v.x = GetKeyFloat( NULL );
	v.y = GetKeyFloat( NULL );
	v.z = GetKeyFloat( NULL );
	v.w = GetKeyFloat( pLineEnd );
	return v;
}

BOOL CDymScriptLoader::SaveBegin(LPCTSTR fullpathfilename)
{
	m_pSaveFile = fopen( fullpathfilename , "wb" );
	if(!m_pSaveFile)return FALSE;
	m_pSaveBracketStep = 0;
	return TRUE;
}
BOOL CDymScriptLoader::SaveToMemoryBegin()
{
	m_pSaveFileBuffer = new char[ 1024*1024 ];
	m_nSaveFileBufferSize = 1024*1024;
	m_pSaveBracketStep = 0;
	m_nSaveFileNowSize = 0;
	return TRUE;
}

void CDymScriptLoader::SaveEnd()
{
	fclose( m_pSaveFile );
}
void *CDymScriptLoader::SaveToMemoryEnd( int *pSizeOut )
{
	*pSizeOut = m_nSaveFileNowSize;
	void *p = m_pSaveFileBuffer;
	m_pSaveFileBuffer = NULL;
	return p;
}

void CDymScriptLoader::SaveBracket_L()
{
	SaveKeyString( "{" , "" );
	m_pSaveBracketStep++;
}

void CDymScriptLoader::SaveBracket_R()
{
	m_pSaveBracketStep--;
	SaveKeyString( "}" , "" );
}

void CDymScriptLoader::SaveKeyString( LPCTSTR strKeyName , LPCTSTR string )
{
	if( m_pSaveFile || m_pSaveFileBuffer )
	{
		const char enter[] = { 0x0D , 0x0A , 0 };
		char savebuf[256];
		char stepbuf[16];
		for(int i=0;i<m_pSaveBracketStep;i++)
			stepbuf[i] = '\t' ;
		stepbuf[m_pSaveBracketStep] = '\0';
		_stprintf( savebuf , "%s%s %s%s" , stepbuf , strKeyName , string , enter );
		if( m_pSaveFile )
			fwrite( savebuf , _tcslen(savebuf) , 1 , m_pSaveFile );
		else
		{
			memcpy( m_pSaveFileBuffer + m_nSaveFileNowSize , savebuf , _tcslen(savebuf) );
			m_nSaveFileNowSize += _tcslen(savebuf) ;
		}
	}
}

void CDymScriptLoader::SaveKeyFloat( LPCTSTR strKeyName , float f )
{
	static TCHAR buf[256];
	_stprintf(buf,"%f",f);
	SaveKeyString( strKeyName , buf );
}

void CDymScriptLoader::SaveKeyBool( LPCTSTR strKeyName , bool b )
{
	static TCHAR buf[256];
	if( b ) _stprintf(buf,"true");
	else _stprintf(buf,"false");
	SaveKeyString( strKeyName , buf );
}

void CDymScriptLoader::SaveKeyInt( LPCTSTR strKeyName , int t )
{
	static TCHAR buf[256];
	_stprintf(buf,"%d",t);
	SaveKeyString( strKeyName , buf );
}
void CDymScriptLoader::SaveKeyVector2( LPCTSTR strKeyName , D3DXVECTOR2 v )
{
	static TCHAR buf[256];
	_stprintf(buf,"%f %f",v.x,v.y);
	SaveKeyString( strKeyName , buf );
}
void CDymScriptLoader::SaveKeyVector3( LPCTSTR strKeyName , D3DXVECTOR3 v )
{
	static TCHAR buf[256];
	_stprintf(buf,"%f %f %f",v.x,v.y,v.z);
	SaveKeyString( strKeyName , buf );
}
void CDymScriptLoader::SaveKeyVector4( LPCTSTR strKeyName , D3DXVECTOR4 v )
{
	static TCHAR buf[256];
	_stprintf(buf,"%f %f %f %f",v.x,v.y,v.z,v.w);
	SaveKeyString( strKeyName , buf );
}