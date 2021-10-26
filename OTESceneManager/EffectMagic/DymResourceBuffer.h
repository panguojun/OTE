/***************************************************************************************************
�ļ�����:	DymResourceBuffer.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	��Դ����
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
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