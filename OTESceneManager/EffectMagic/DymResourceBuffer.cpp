/***************************************************************************************************
文件名称:	DymResourceBuffer.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	资源管理
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymresourcebuffer.h"

CDymResourceBuffer::CDymResourceBuffer(void)
{
}

CDymResourceBuffer::~CDymResourceBuffer(void)
{
	ClearBuffer();
}

CDymResourceBuffer::BUFFER_LIST * CDymResourceBuffer::_FindBuffer( void *pBuffer ,  int datatype , LPCTSTR strName )
{
	for( std::list<BUFFER_LIST*>::iterator it = m_listBuffer.begin() ; it!=m_listBuffer.end() ; it++)
	{
		BUFFER_LIST *pList = *it;
		if( pBuffer == pList->pData && pBuffer!=NULL )return pList;
		if( strName )
		{
			if( _tcsicmp( strName , pList->pName ) == 0 && pList->datatype == datatype )
			{
				return pList;
			}
		}
	}
	return NULL;
}

void CDymResourceBuffer::AddBuffer( void *pBuffer , int size , int datatype , LPCTSTR strName )
{
	if(!pBuffer || !strName)return;
	BUFFER_LIST *pList = new BUFFER_LIST;
	pList->pName = new TCHAR[ _tcslen( strName ) +1 ];
	_tcscpy( pList->pName , strName );
	pList->datatype = datatype;
	pList->pData = pBuffer;
	pList->datasize = size;
	pList->addref = 1;
	m_listBuffer.push_front( pList );
	if( m_listBuffer.size () >50 )
	{
		std::list<BUFFER_LIST*>::iterator it = m_listBuffer.end ();
		it--;
		BUFFER_LIST *pDel = *it;
		if( pDel->addref <=0 )
		{
			m_listBuffer.remove ( pDel );
			DelObj( pDel );
		}
	}
}
void CDymResourceBuffer::DelObj( CDymResourceBuffer::BUFFER_LIST *pDel )
{
	delete pDel;
}
void *CDymResourceBuffer::LoadBuffer( LPCTSTR strName , int datatype , int *size )
{
	BUFFER_LIST *pList = _FindBuffer( NULL , datatype , strName );
	if(pList)
	{
		pList->addref ++;
		m_listBuffer.remove ( pList );
		m_listBuffer.push_front ( pList );
		if(size)*size = pList->datasize ;
		return pList->pData ;
	}
	return NULL;
}

bool CDymResourceBuffer::UnLoadBuffer( void *pBuffer , int datatype , LPCTSTR strName )
{
	BUFFER_LIST *pList = _FindBuffer( pBuffer , datatype , strName );
	if(pList)
	{
		pList->addref --;
		if( pList->addref <=0 )
		{
			m_listBuffer.remove ( pList );
			m_listBuffer.push_back ( pList );
		}
		return true;
	}
	return false;
}

void CDymResourceBuffer::ClearBuffer()
{
	for( std::list<BUFFER_LIST*>::iterator it = m_listBuffer.begin() ; it!=m_listBuffer.end() ; it++)
	{
		BUFFER_LIST *p = *it;
		DelObj( p );
	}
	m_listBuffer.clear ();
}
