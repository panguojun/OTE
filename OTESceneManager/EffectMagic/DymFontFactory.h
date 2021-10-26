/***************************************************************************************************
�ļ�����:	DymFontFactory.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	������Ⱦ
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include "DymRenderSystem.h"
#include <tchar.h>
//#include "FBGUIFont/FBGUIRender.h"
#include "FBGUIFont/D3DFont.h"
//using namespace FBGUISystemLayer;
/*
//-----------------------------------------------------------------------------------------
//
// CDymTextTexture
//
//-----------------------------------------------------------------------------------------
class CDymTextTexture : public CGUITexture
{
	CDymD3DTexture *m_pTextureBase;
public:
	CDymTextTexture( CDymRenderSystem *pDevice , LPCTSTR filename , int width , int height );
public:
	virtual ~CDymTextTexture( );

public:
	virtual int GetTexWidth( )					;
	virtual int GetTexHeight( )					;
	virtual const std::string& GetTexName( )	;
	virtual unsigned int* Lock( )				;
	virtual int Unlock( )						;
	CDymD3DTexture *GetTextureBase(){return m_pTextureBase;}
};*/
//-----------------------------------------------------------------------------------------
//
// CDymFontFactory
//
//-----------------------------------------------------------------------------------------
class CDymFontFactory //:public CGUITextureFactory
{
	struct _FONT_TEXTURE
	{
		//CDymTextTexture *pTex;
		LPDIRECT3DTEXTURE9 pTex;
		D3DFont::CGUIRect tRect;
	};
	/// ��Ⱦϵͳ�豸ָ��
	CDymRenderSystem *m_pDevice;
	/// ��ͼ�б�
	std::list< LPDIRECT3DTEXTURE9 > m_listTexture;
	/// CGUIFontָ��
	D3DFont::CGUIFont *m_pFont;

	/// ������ͼ
	HRESULT CopyTexture(LPDIRECT3DTEXTURE9 pTextureTo,LPDIRECT3DTEXTURE9 pTextureFrom,RECT rcFrom,RECT rcTo);
public:
	CDymFontFactory(CDymRenderSystem *pDevice);
	~CDymFontFactory(void);
	//virtual CGUITexture* LoadTexture( const char* pTexName ) ;
	//virtual CGUITexture* CreateTexture( const char* pTexName, int vWidth, int vHeight ) ;

	/// ����������ͼ
	CDymD3DTexture * NewTexture( LPCTSTR str , int *pWidthOut , int *pHeightOut , void *pUnknow );
};
