/***************************************************************************************************
文件名称:	DymResourceBuffer.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	资源管理
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include <tchar.h>
#include <windows.h>
#include <list>

class __declspec( dllexport ) CDymResourceBuffer
{
public:
	struct BUFFER_LIST
	{
		int addref;
		void *pData ;
		int datasize;
		int datatype;
		TCHAR *pName ;
		BUFFER_LIST()
		{
			addref = 0;
			datasize = 0;
			datatype = 0;
			pData = NULL;
			pName = NULL;
		}
		~BUFFER_LIST()
		{
			if( pName )delete[] pName;
		}
	};
	std::list< BUFFER_LIST* > m_listBuffer;
	BUFFER_LIST * _FindBuffer( void *pBuffer , int datatype , LPCTSTR strName );
public:
	CDymResourceBuffer(void);
	virtual ~CDymResourceBuffer(void);
	void AddBuffer( void *pBuffer , int size , int datatype , LPCTSTR strName );
	void *LoadBuffer( LPCTSTR strName ,  int datatype , int *size );
	bool UnLoadBuffer( void *pBuffer , int datatype , LPCTSTR strName );
	void ClearBuffer();
	virtual void DelObj( CDymResourceBuffer::BUFFER_LIST *pDel );
};