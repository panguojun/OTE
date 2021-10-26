#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include "Windows.h"
#include "D3D9.h"
#include "D3dx9tex.h"
#include "D3DFont.h"

#define GUI_MAKECOLOUR( tAlpha, tRed, tGreen, tBlue ) ( tAlpha ) << 24 | ( tRed ) << 16 | ( tGreen ) << 8 | ( tBlue )
#define GUI_MIN( a, b )	( ( a ) > ( b ) ? ( b ) : ( a ) )
#define GUI_MAX( a, b )	( ( a ) > ( b ) ? ( a ) : ( b ) )

using namespace D3DFont;
// ************************************************************ //
// CGUISize
// ************************************************************ //

CGUIPoint CGUISize::operator - ( const CGUIPoint& rPoint ) const
{
	return rPoint - *this; 
}

CGUIRect CGUISize::operator - ( const CGUIRect& rRect ) const
{
	return rRect - *this;	
}

CGUIPoint CGUISize::operator + ( const CGUIPoint& rPoint ) const
{
	return rPoint + *this; 
}

CGUIRect CGUISize::operator + ( const CGUIRect& rRect ) const
{
	return rRect + *this; 
}

// ************************************************************ //
// CGUIPoint
// 2D 坐标
// ************************************************************ //

CGUIRect CGUIPoint::operator - ( const CGUIRect& rRect ) const	
{
	return rRect - *this;
}

CGUIRect CGUIPoint::operator + ( const CGUIRect& rRect ) const	
{
	return rRect + *this;
}

// ************************************************************ //
// CGUIRect
// 2D 矩形区域
// ************************************************************ //

bool CGUIRect::IntersectRect( const CGUIRect& rSrcRect1, const CGUIRect& rSrcRect2 )
{
	if ( rSrcRect1.IsRectEmpty( ) || rSrcRect2.IsRectEmpty( ) )
		return false;

	int tLeft	= GUI_MAX( rSrcRect1.mTopLeft.mX, rSrcRect2.mTopLeft.mX );
	int tTop	= GUI_MAX( rSrcRect1.mTopLeft.mY, rSrcRect2.mTopLeft.mY );
	int tRight	= GUI_MIN( rSrcRect1.mBottomRight.mX, rSrcRect2.mBottomRight.mX );
	int tBottom	= GUI_MIN( rSrcRect1.mBottomRight.mY, rSrcRect2.mBottomRight.mY );

	if ( CGUIRect( tLeft, tTop, tRight, tBottom, false ).IsRectEmpty( ) )
	{
		SetRectEmpty( );
		return false;
	}

	mTopLeft.mX		= tLeft;
	mTopLeft.mY		= tTop;
	mBottomRight.mX = tRight;
	mBottomRight.mY = tBottom;
	return true;
}

void CGUIRect::NormalizeRect( )
{
	mTopLeft.mX		= GUI_MIN( mTopLeft.mX, mBottomRight.mX );
	mTopLeft.mY		= GUI_MIN( mTopLeft.mY, mBottomRight.mY );
	mBottomRight.mX	= GUI_MAX( mTopLeft.mX, mBottomRight.mX );
	mBottomRight.mY	= GUI_MAX( mTopLeft.mY, mBottomRight.mY );
}

bool CGUIRect::PtInRect( const CGUIPoint& rPoint ) const
{
	if ( IsRectEmpty( ) )
		return false;

	if ( rPoint.mX < mTopLeft.mX || rPoint.mX > mBottomRight.mX )
		return false;

	if ( rPoint.mY < mTopLeft.mY || rPoint.mY > mBottomRight.mY )
		return false;

	return true;
}

bool CGUIRect::UnionRect( const CGUIRect& rSrcRectl, const CGUIRect& rSrcRect2 )
{
	if ( rSrcRectl.IsRectEmpty( ) || rSrcRect2.IsRectEmpty( ) )
		return false;

	int tLeft	= rSrcRectl.mTopLeft.mX < rSrcRect2.mTopLeft.mX ? rSrcRectl.mTopLeft.mX : rSrcRect2.mTopLeft.mX;
	int tTop	= rSrcRectl.mTopLeft.mY < rSrcRect2.mTopLeft.mY ? rSrcRectl.mTopLeft.mY : rSrcRect2.mTopLeft.mY;
	int tRight	= rSrcRectl.mBottomRight.mX > rSrcRect2.mBottomRight.mX ? rSrcRectl.mBottomRight.mX : rSrcRect2.mBottomRight.mX;
	int tBottom	= rSrcRectl.mBottomRight.mY > rSrcRect2.mBottomRight.mY ? rSrcRectl.mBottomRight.mY : rSrcRect2.mBottomRight.mY;

	mTopLeft.mX		= tLeft;
	mTopLeft.mY		= tTop;
	mBottomRight.mX = tRight;
	mBottomRight.mY = tBottom;
	return true;
}

// ************************************************************ //
// CGlyph
// ************************************************************ //

CGUIFontGlyph::CGUIFontGlyph( ) : mpTexture( NULL ), mAdvanceWidth( 0 ), mIsCreated( false ), mUnicode( 0 ), mpFont( NULL )
{
}

CGUIFontGlyph::~CGUIFontGlyph( )
{
}

// 得到字形纹理
IDirect3DTexture9* CGUIFontGlyph::GetGlyphTexture( )
{
	if ( mIsCreated == false )
		CreateGlyph( );

	return mpTexture;
}

// 得到逻辑宽度
int CGUIFontGlyph::GetGlyphWidth( )
{
	if ( mIsCreated == false )
		CreateGlyph( );

	return mAdvanceWidth;
}

// 得到字形在纹理中的位置
CGUIRect CGUIFontGlyph::GetGlyphRect( )
{
	if ( mIsCreated == false )
		CreateGlyph( );

	return mGlyphRect;
}

int CGUIFontGlyph::FillFont( IDirect3DTexture9* pTexture, const CGUIRect& rRect )
{
	FT_Size_Metrics* tpMetrics	= &mpFont->mpFTFace->size->metrics;
	FT_GlyphSlot tpSlot			= mpFont->mpFTFace->glyph;
	FT_Load_Char( mpFont->mpFTFace, mUnicode, FT_LOAD_RENDER );

	int tBoxWidth	= tpSlot->bitmap.width;
	int tBoxHeight	= tpSlot->bitmap.rows;
	int tOriginX	= tpSlot->bitmap_left;
	int tOriginY	= tpSlot->bitmap_top;
	D3DLOCKED_RECT tD3DRect;
	RECT tRect;
	tRect.left		= 0;
	tRect.top		= 0;
	tRect.right		= 512;
	tRect.bottom	= 512;
	pTexture->LockRect( 0, &tD3DRect, &tRect, D3DLOCK_DISCARD );
	unsigned int* tpColour	= (unsigned int*) tD3DRect.pBits;
	int tPitch				= 512;
	unsigned int tColour	= 0;
	int tMaxWidth			= int( tpSlot->metrics.horiAdvance / 64.0f );
	for ( int x = 0; x < tBoxWidth; x ++ )
	{
		for ( int y = 0; y < tBoxHeight; y ++ )
		{
			int tMask = tpSlot->bitmap.buffer[ y * tpSlot->bitmap.width + x ];
			// 提亮像素
			int( tMask * 1.5f ) > 255 ? tMask = 255 : tMask = int( tMask * 1.5f );

			// 忽略太暗的像素
			tMask < 32 ? tMask = 0 : NULL;
			tMask >= 1 ? tColour = GUI_MAKECOLOUR( tMask, 255, 255, 255 ) : tColour = GUI_MAKECOLOUR( 0, 0, 0, 0 );

			int tX = x + tOriginX;
			int tY = int( mpFont->GetFontHeight( ) - tOriginY + y + tpMetrics->descender / 64.0f + 1 );

			if ( tMask > 1 )
			{
				for ( int m = 0; m < mpFont->mEdge; m ++ )
				{
					for ( int n = 0; n < mpFont->mEdge; n ++ )
					{
						int tY1 = tY + n + mpFont->mVOffset;
						int tX1 = tX + m + mpFont->mHOffset;
						tMaxWidth = max( tX1, tMaxWidth );
						if ( tX1 > 0 && tY1 > 0 && ( tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] & 0x00FFFFFF ) == 0 )
							tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] = GUI_MAKECOLOUR( 255, 0, 0, 0 );

						tY1 = tY + n + mpFont->mVOffset;
						tX1 = tX - m + mpFont->mHOffset;
						tMaxWidth = max( tX1, tMaxWidth );
						if ( tX1 > 0 && tY1 > 0 && ( tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] & 0x00FFFFFF ) == 0 )
							tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] = GUI_MAKECOLOUR( 255, 0, 0, 0 );

						tY1 = tY - n + mpFont->mVOffset;
						tX1 = tX + m + mpFont->mHOffset;
						tMaxWidth = max( tX1, tMaxWidth );
						if ( tX1 > 0 && tY1 > 0 && ( tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] & 0x00FFFFFF ) == 0 )
							tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] = GUI_MAKECOLOUR( 255, 0, 0, 0 );

						tY1 = tY - n + mpFont->mVOffset;
						tX1 = tX - m + mpFont->mHOffset;
						tMaxWidth = max( tX1, tMaxWidth );
						if ( tX1 > 0 && tY1 > 0 && ( tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] & 0x00FFFFFF ) == 0 )
							tpColour[ ( tY1 + rRect.mTopLeft.mY ) * tPitch + tX1 + rRect.mTopLeft.mX ] = GUI_MAKECOLOUR( 192, 0, 0, 0 );
					}
				}

				tpColour[ ( tY + rRect.mTopLeft.mY ) * tPitch + tX + rRect.mTopLeft.mX ] = tColour;
			}
		}
	}	

	pTexture->UnlockRect( 0 );
	mGlyphRect.SetRect( rRect.TopLeft( ), CGUISize( tMaxWidth + 1, mpFont->GetFontHeight( ) ) );
	mAdvanceWidth = int( tpSlot->metrics.horiAdvance / 64.0f );
	return 0;
}

int CGUIFontGlyph::CreateGlyph( )
{
	// 计算一张纹理上有多少个字形
	int tFontCountInHorz	 = ( 512 / mpFont->mUnitWidth );
	int tFontCountInVert	 = ( 512 / mpFont->mUnitHeight );
	int tFontCountPerTexture = tFontCountInHorz * tFontCountInVert;
	
	// 计算一个可以用的“位置”，“位置”代表了纹理和纹理区域
	int tFontIndex		= mpFont->mFontUsed % tFontCountPerTexture;	// 纹理区域
	int tTextureIndex	= mpFont->mFontUsed / tFontCountPerTexture;	// 纹理
	int tHorzIndex		= tFontIndex % tFontCountInHorz;
	int tVertIndex		= tFontIndex / tFontCountInHorz;
	CGUIRect tFontRect( tHorzIndex * mpFont->mUnitWidth, tVertIndex * mpFont->mUnitHeight, 
		( tHorzIndex + 1 ) * mpFont->mUnitWidth, ( tVertIndex + 1 ) * mpFont->mUnitHeight );

	IDirect3DTexture9* tpTexture = NULL;
	if ( tTextureIndex < int( mpFont->mFontTex.size( ) ) )
	{
		// 如果纹理已经建立
		tpTexture = mpFont->mFontTex[ tTextureIndex ];
		FillFont( tpTexture, tFontRect );
	}
	else
	{
		// 如果纹理没有建立
		D3DXCreateTexture( mpDevice, 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tpTexture );
		mpFont->mFontTex.push_back( tpTexture );
		FillFont( tpTexture, tFontRect );
	}

	mpFont->mFontUsed ++;
	mIsCreated	= true;
	mpTexture	= tpTexture;
	return 0;
}

// ************************************************************ //
// CCodePage
// ************************************************************ //

CGUICodePage::CGUICodePage( )
{
}

CGUICodePage::~CGUICodePage( )
{
}

// ************************************************************ //
// CGUIGBKPage
// ************************************************************ //

CGUIGBKPage::CGUIGBKPage( )
{
}

CGUIGBKPage::~CGUIGBKPage( )
{
}

int CGUIGBKPage::GetGlyphIndex( const char* pText ) const
{
	int tGlphyIndex = 0;
	if ( pText[0] & 0x80 )
		tGlphyIndex = ( (unsigned char) pText[ 0 ] - GB_MIN_1ST ) * ( GB_MAX_2ND + 1 - GB_MIN_2ND ) + (unsigned char) pText[ 1 ] - GB_MIN_2ND + 256;
	else // 如果是英文
		tGlphyIndex = (unsigned char) pText[ 0 ];

	// 如果在内码空间里面
	if ( tGlphyIndex >= 0 && tGlphyIndex < GetCodeSpace( ) )
		return tGlphyIndex;

	// 否则返回错误码
	return -1;
}

int CGUIGBKPage::GetCodeSpace( ) const
{
	return ( GB_MAX_2ND + 1 - GB_MIN_2ND ) * ( GB_MAX_1ST + 1 - GB_MIN_1ST ) + 256;
}

wchar_t CGUIGBKPage::GetUnicode( int vGlphyIndex ) const
{
	char tText[ 3 ] = { 0 };
	if ( vGlphyIndex < 256 )
		tText[ 0 ] = char( vGlphyIndex );
	else
	{
		tText[ 0 ] = ( vGlphyIndex -  256 ) / ( GB_MAX_2ND + 1 - GB_MIN_2ND ) + GB_MIN_1ST;
		tText[ 1 ] = ( vGlphyIndex -  256 ) % ( GB_MAX_2ND + 1 - GB_MIN_2ND ) + GB_MIN_2ND;
	}

	// 得到Unicode编码
	wchar_t tUnicode;
	::MultiByteToWideChar( CP_ACP, 0, tText, int( ::strlen( tText ) ), &tUnicode, 1 );
	return tUnicode;
}

// ************************************************************ //
// CGUIFont
// ************************************************************ //

FT_Library CGUIFont::spFTLibrary = NULL;
int		   CGUIFont::sFTRef = 0;

CGUIFont::CGUIFont( IDirect3DDevice9* pDevice, int vFontHeight, const char* pFace, const int vEdge, const int vHOffset, const int vVOffset, CGUICodePage* pCharMap ) : 
	mDpi( 72 ), mEdge( vEdge ), mHOffset( vHOffset ), mVOffset( vVOffset ), mpCharMap( pCharMap ), mFontUsed( 0 ), mFaceName( pFace )
{
	// 如果没有CodePage, 使用GBK作为默认CodePage
	if ( mpCharMap == NULL )
		mpCharMap = new CGUIGBKPage;

	// 初始化FreeType
	if ( spFTLibrary == NULL )
        FT_Init_FreeType( &spFTLibrary );              

	// 创建字体
	FT_New_Face( spFTLibrary, mFaceName.c_str( ), 0, &mpFTFace );

	// 设置字体解析度，字体大小
	FT_Set_Char_Size( mpFTFace, vFontHeight * 64, 0, mDpi, mDpi );

	mUnitWidth	= mpFTFace->size->metrics.x_ppem + vEdge + vVOffset + 5;
	mUnitHeight = mpFTFace->size->metrics.y_ppem + vEdge + vHOffset + 5;
	// 创建表面
	mpGlyph = new CGUIFontGlyph[ mpCharMap->GetCodeSpace( ) ];
	for ( int i = 0; i < mpCharMap->GetCodeSpace( ); i ++ )
		mpGlyph[ i ].SetContext( this, pDevice, mpCharMap->GetUnicode( i ) );
	sFTRef ++;
}

CGUIFont::~CGUIFont( )
{
	for ( int i = 0; i < int( mFontTex.size( ) ); i ++ )
        mFontTex[ i ]->Release( );

	// 释放CodePage
	if ( mpCharMap != NULL )
	{
		delete mpCharMap;
		mpCharMap = NULL;
	}

	// 释放字体表面	
	delete [] mpGlyph;
	mpGlyph = NULL;

	// 释放FreeType字体
	FT_Done_Face( mpFTFace );
	mpFTFace = NULL;

	// 释放FreeType
	if ( -- sFTRef == 0 )
	{
		FT_Done_FreeType( spFTLibrary );
		spFTLibrary = NULL;
	}
}

CGUIFontGlyph* CGUIFont::GetFontGlyph( const char* pText )
{
	int tGlyphIndex = mpCharMap->GetGlyphIndex( pText );
	if ( tGlyphIndex == -1 )
		return NULL;

	return &mpGlyph[ tGlyphIndex ];
}

int CGUIFont::GetFontHeight( )
{
	return int( mpFTFace->size->metrics.height / 64.0f );
}

int CGUIFont::GetTextWidth( const char* pText )
{
	int tGlyphIndex = mpCharMap->GetGlyphIndex( pText );
	if ( tGlyphIndex == -1 )
		return -1;

	return mpGlyph[ tGlyphIndex ].GetGlyphWidth( );
}

int CGUIFont::GetTextWidth( const char* pText, int vTextCount )
{
	int tTextWidth = 0;
	for ( int tIndex = 0; tIndex < vTextCount; )
	{
		tTextWidth += GetTextWidth( pText + tIndex );
		pText[ tIndex ] & 0x80 ? tIndex += 2 : tIndex ++;
	}
	return tTextWidth;
}

int CGUIFont::GetMaxTextWidth( )
{
	return int( mpFTFace->size->metrics.max_advance / 64.0f );
}

//int CGUIFont::PrintText( CRenderDevice* pRenderDevice, const char* pText, const CGUIPoint& rPoint, int vColor )
//{
//	int tGlyphIndex = mpCharMap->GetGlyphIndex( pText );
//	if ( tGlyphIndex == GUI_ERR_OUTOFCODEPAGE )
//		return GUI_ERR_OUTOFCODEPAGE;
//	
//	const CGUIRect& rSrcRect = mpGlyph[ tGlyphIndex ].GetGlyphRect( );
//	pRenderDevice->MaskBlt( mpGlyph[ tGlyphIndex ].GetGlyphTexture( ), rSrcRect, CGUIRect( rPoint, rSrcRect.Size( ) ), vColor );
//	return mpGlyph[ tGlyphIndex ].GetGlyphWidth( );
//}
