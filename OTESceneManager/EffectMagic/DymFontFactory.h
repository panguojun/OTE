/***************************************************************************************************
文件名称:	DymFontFactory.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	文字渲染
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
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
	/// 渲染系统设备指针
	CDymRenderSystem *m_pDevice;
	/// 贴图列表
	std::list< LPDIRECT3DTEXTURE9 > m_listTexture;
	/// CGUIFont指针
	D3DFont::CGUIFont *m_pFont;

	/// 考贝贴图
	HRESULT CopyTexture(LPDIRECT3DTEXTURE9 pTextureTo,LPDIRECT3DTEXTURE9 pTextureFrom,RECT rcFrom,RECT rcTo);
public:
	CDymFontFactory(CDymRenderSystem *pDevice);
	~CDymFontFactory(void);
	//virtual CGUITexture* LoadTexture( const char* pTexName ) ;
	//virtual CGUITexture* CreateTexture( const char* pTexName, int vWidth, int vHeight ) ;

	/// 创建文子贴图
	CDymD3DTexture * NewTexture( LPCTSTR str , int *pWidthOut , int *pHeightOut , void *pUnknow );
};
