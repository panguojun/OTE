/***************************************************************************************************
文件名称:	DymFontFactory.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	文字渲染
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/

#include ".\dymfontfactory.h"
#include <atlpath.h>
/*
//-----------------------------------------------------------------------------------------
//
// CDymTextTexture
//
//-----------------------------------------------------------------------------------------
CDymTextTexture::CDymTextTexture( CDymRenderSystem *pDevice , LPCTSTR filename , int width , int height )
{
	m_pTextureBase = NULL;
	if( filename )
		pDevice->CreateTextureFromFile ( filename , &m_pTextureBase );
	else
		pDevice->CreateTexture( &m_pTextureBase , width , height );
}
CDymTextTexture::~CDymTextTexture( )
{
	if( m_pTextureBase )
	{
		if( FAILED( m_pTextureBase->Release () ))
			delete m_pTextureBase;
	}
}
int CDymTextTexture::GetTexWidth( )
{
	if( m_pTextureBase == NULL )
		return 0;
	return m_pTextureBase->GetDesc ().Width ;
}
int CDymTextTexture::GetTexHeight( )
{
	if( m_pTextureBase == NULL )
		return 0;
	return m_pTextureBase->GetDesc ().Height ;
}
const std::string& CDymTextTexture::GetTexName( )
{
	std::string str;
	return str;
}
unsigned int* CDymTextTexture::Lock( )
{
	if( m_pTextureBase == NULL )
		return NULL;
	D3DLOCKED_RECT  d3dlr;
	if( FAILED( m_pTextureBase->m_pTex->LockRect( 0, &d3dlr, 0, 0 ) ) )
		return NULL;
	return (unsigned int*)d3dlr.pBits ;
}
int CDymTextTexture::Unlock( )
{
	if( m_pTextureBase )
		m_pTextureBase->m_pTex->UnlockRect ( 0 );
	return 1;
}
*/
//-----------------------------------------------------------------------------------------
//
// CDymFontFactory
//
//-----------------------------------------------------------------------------------------
CDymFontFactory::CDymFontFactory(CDymRenderSystem *pDevice)
{
	m_pDevice = pDevice;
	TCHAR strpath[MAX_PATH];
	GetModuleFileName( NULL , strpath , MAX_PATH );
	TCHAR strfilename[MAX_PATH];

	TCHAR chardrive[_MAX_DRIVE];
	TCHAR chardir[_MAX_DIR];
	TCHAR charext[_MAX_EXT];
	TCHAR charname[_MAX_PATH];
	_splitpath(strpath,chardrive,chardir,charname,charext); 
	TCHAR path[MAX_PATH];
	_stprintf(path , "%s%s",chardrive,chardir );

	_stprintf( strfilename , "%s%s" , path , "FZLBZW.ttf" );
	if( PathFileExists ( strfilename ) )
		m_pFont = new D3DFont::CGUIFont( m_pDevice->m_pd3dDevice , 64 , strfilename , 0 , 0 , 0 , NULL );
	else if( PathFileExists( "C:\\WINDOWS\\Fonts\\SIMSUN.TTC" ) )
		m_pFont = new D3DFont::CGUIFont( m_pDevice->m_pd3dDevice , 64 , "C:\\WINDOWS\\Fonts\\SIMSUN.TTC" , 0 , 0 , 0 , NULL );
	else
		m_pFont = new D3DFont::CGUIFont( m_pDevice->m_pd3dDevice , 64 , "C:\\WINNT\\Fonts\\SIMSUN.TTC" , 0 , 0 , 0 , NULL );
}

CDymFontFactory::~CDymFontFactory(void)
{
	m_listTexture.clear ();
	delete m_pFont;
}

/*CGUITexture* CDymFontFactory::LoadTexture( const char* pTexName ) 
{
	CDymTextTexture *pTex = new CDymTextTexture( m_pDevice , pTexName , 0 , 0 );
	m_listTexture.push_back ( pTex );
	return pTex;
	return NULL;
}
CGUITexture* CDymFontFactory::CreateTexture( const char* pTexName, int vWidth, int vHeight ) 
{
	CDymTextTexture *pTex = new CDymTextTexture( m_pDevice , NULL , vWidth , vHeight );
	m_listTexture.push_back ( pTex );
	return pTex;
	return NULL;
}*/

HRESULT CDymFontFactory::CopyTexture(LPDIRECT3DTEXTURE9 pTextureTo,LPDIRECT3DTEXTURE9 pTextureFrom,RECT rcFrom,RECT rcTo)
{
	HRESULT hr;
	D3DSURFACE_DESC d3dsd1,d3dsd2;
	pTextureFrom->GetLevelDesc( 0, &d3dsd1 );
	pTextureTo->GetLevelDesc( 0, &d3dsd2 );
	// Lock the texture
	D3DLOCKED_RECT  d3dlr1,d3dlr2;
	if( FAILED( hr=pTextureFrom->LockRect( 0, &d3dlr1, 0, 0 ) ) )return hr;
	if(pTextureTo==pTextureFrom)d3dlr2=d3dlr1;
	else if( FAILED( hr=pTextureTo->LockRect( 0, &d3dlr2, 0, 0 ) ) )return hr;
	// Scan through each pixel
	for( long y=rcFrom.top ; y<rcFrom.bottom ; y++ )
	{
		for( long x=rcFrom.left ; x<rcFrom.right ; x++ )
		{
			// Handle 32-bit formats
			((DWORD*)d3dlr2.pBits)[d3dsd2.Width*(y-rcFrom.top+rcTo.top)+(x-rcFrom.left+rcTo.left)]
				=((DWORD*)d3dlr1.pBits)[d3dsd1.Width*y+x];

		}
	}
	// Unlock the texture and return OK.
	if(pTextureTo!=pTextureFrom)pTextureTo->UnlockRect(0);
	pTextureFrom->UnlockRect(0);
	return S_OK;
}
CDymD3DTexture * CDymFontFactory::NewTexture( LPCTSTR str , int *pWidthOut , int *pHeightOut , void *pUnknow )
{
	//if( pFont == NULL )
	D3DFont::CGUIFont *pFont = m_pFont;
	std::list< _FONT_TEXTURE > list_font;

	int len = _tcslen( str );
	TCHAR strtmp[3];
	int width = 0;
	int height = 0;
	for( int i=0 ; i<len ; i++)
	{
		if( str[i] < 0 )
		{
			strtmp[0] = str[i];
			strtmp[1] = str[i+1];
			strtmp[2] = 0;
			i++;
		}
		else
		{
			strtmp[0] = str[i];
			strtmp[1] = 0;
		}

		D3DFont::CGUIFontGlyph* tpFontGlyph = pFont->GetFontGlyph( strtmp );
		LPDIRECT3DTEXTURE9 pTextTexture = tpFontGlyph->GetGlyphTexture();
		D3DFont::CGUIRect tCRect = tpFontGlyph->GetGlyphRect( );
		_FONT_TEXTURE font_texture;
		font_texture.pTex = pTextTexture;
		font_texture.tRect = tCRect;
		width += tCRect.mBottomRight .mX - tCRect.mTopLeft .mX ;
		if( height < tCRect.mBottomRight .mY - tCRect.mTopLeft .mY )
			height = tCRect.mBottomRight .mY - tCRect.mTopLeft .mY;
		list_font.push_back ( font_texture );
	}
	CDymD3DTexture *pD3DTexture = NULL;
	m_pDevice->CreateTexture ( &pD3DTexture , width , height );
	if( pD3DTexture == NULL )return NULL;
	int writewidth = 0;
	// clear texture
	D3DLOCKED_RECT  d3dlr;
	int texturewidth = pD3DTexture->GetDesc ().Width ;
	int textureheight = pD3DTexture->GetDesc().Height ;
	pD3DTexture->m_pTex->LockRect ( 0 , &d3dlr , NULL , 0 );
	for( int i=0;i< texturewidth*textureheight ; i++)
		((DWORD*)d3dlr.pBits )[i] = 0xFF0000FF;
	pD3DTexture->m_pTex->UnlockRect ( 0 );

	for( std::list< _FONT_TEXTURE >::iterator it = list_font.begin () ; it != list_font.end () ; it++ )
	{
		_FONT_TEXTURE font_texture = *it;
		RECT rc;
		int textwidth = font_texture.tRect.mBottomRight .mX - font_texture.tRect.mTopLeft .mX ;
		int textheight = font_texture.tRect.mBottomRight .mY - font_texture.tRect.mTopLeft .mY ;
		SetRect(&rc , writewidth , 0 , writewidth+textwidth , textheight );
		RECT rcFrom;
		SetRect(&rcFrom , font_texture.tRect.mTopLeft .mX , font_texture.tRect.mTopLeft .mY , 
			font_texture.tRect.mBottomRight .mX , font_texture.tRect.mBottomRight .mY );
		CopyTexture( pD3DTexture->m_pTex , font_texture.pTex , rcFrom , rc );
		writewidth += textwidth;
	}
	*pWidthOut = writewidth ;
	*pHeightOut = height;
	list_font.clear ();
	return pD3DTexture;
}

