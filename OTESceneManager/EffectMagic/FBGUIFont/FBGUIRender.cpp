#include "FBGUIRender.h"

namespace FBGUISystemLayer
{

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
// CGUIViewport
// 2D 视口
// ************************************************************ //

CGUIViewport::CGUIViewport( int vSrcWidth, int vSrcHeight, int vDestWidth, int vDestHeight )
{
	mSrcSize.mCX	= vSrcWidth;
	mSrcSize.mCY	= vSrcHeight;
	mDestSize.mCX	= vDestWidth;
	mDestSize.mCY	= vDestHeight;
}

CGUIViewport::CGUIViewport( const CGUISize& rSrcSize, const CGUISize& rDestSize )
{
	mSrcSize	= rSrcSize;
	mDestSize	= rDestSize;
}

CGUIViewport::~CGUIViewport( )
{
}

void CGUIViewport::SetWindowExt( int vSrcWidth, int vSrcHeight )
{
	mSrcSize.mCX = vSrcWidth;
	mSrcSize.mCY = vSrcHeight;
}

void CGUIViewport::SetWindowExt( const CGUISize& rSrcSize )
{
	mSrcSize = rSrcSize;
}

void CGUIViewport::SetViewportExt( int vDestWidth, int vDestHeight )
{
	mDestSize.mCX = vDestWidth;
	mDestSize.mCY = vDestHeight;
}

void CGUIViewport::SetViewportExt( const CGUISize& rDestSize )
{
	mDestSize = rDestSize;
}

CGUIPoint CGUIViewport::LP2DP( const CGUIPoint& rPoint )
{
	int tX = int( rPoint.mX * mDestSize.mCX / (float) mSrcSize.mCX );
	int tY = int( rPoint.mY * mDestSize.mCY / (float) mSrcSize.mCY );
	return CGUIPoint( tX, tY );
}

CGUISize CGUIViewport::LP2DP( const CGUISize& rSize )
{
	int tCX = int( rSize.mCX * mDestSize.mCX / (float) mSrcSize.mCX );
	int tCY = int( rSize.mCY * mDestSize.mCY / (float) mSrcSize.mCY );
	return CGUISize( tCX, tCY );
}

CGUIRect CGUIViewport::LP2DP( const CGUIRect& rRect )
{
	return CGUIRect( LP2DP( rRect.mTopLeft ), LP2DP( rRect.mBottomRight ) );
}

CGUIPoint CGUIViewport::DP2LP( const CGUIPoint& rPoint )
{
	int tX = int( rPoint.mX * mSrcSize.mCX / (float) mDestSize.mCX );
	int tY = int( rPoint.mY * mSrcSize.mCY / (float) mDestSize.mCY );
	return CGUIPoint( tX, tY );
}

CGUISize CGUIViewport::DP2LP( const CGUISize& rSize )
{
	int tCX = int( rSize.mCX * mSrcSize.mCX / (float) mDestSize.mCX );
	int tCY = int( rSize.mCY * mSrcSize.mCY / (float) mDestSize.mCY );
	return CGUISize( tCX, tCY );
}

CGUIRect CGUIViewport::DP2LP( const CGUIRect& rRect )
{
	return CGUIRect( DP2LP( rRect.mTopLeft ), DP2LP( rRect.mBottomRight ) );
}

// ************************************************************ //
// CGUITexture
// GUI同一纹理接口
// ************************************************************ //

CGUITexture::CGUITexture( )
{
}
CGUITexture::~CGUITexture( )
{
}

// ************************************************************ //
// CGUITextureFactory
// GUI纹理工厂
// ************************************************************ //

CGUITextureFactory::CGUITextureFactory( )
{
}

CGUITextureFactory::~CGUITextureFactory( )
{
}

// ************************************************************ //
// CGlyph
// ************************************************************ //

CGUIFontGlyph::CGUIFontGlyph( ) : mpTextureFactory( NULL ), mpTexture( NULL ), mAdvanceWidth( 0 ), mIsCreated( false ), mUnicode( 0 ), mpFont( NULL )
{
}

CGUIFontGlyph::~CGUIFontGlyph( )
{
}

// 得到字形纹理
CGUITexture* CGUIFontGlyph::GetGlyphTexture( )
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

int CGUIFontGlyph::FillFont( CGUITexture* pTexture, const CGUIRect& rRect )
{
	FT_Size_Metrics* tpMetrics	= &mpFont->mpFTFace->size->metrics;
	FT_GlyphSlot tpSlot			= mpFont->mpFTFace->glyph;
	FT_Load_Char( mpFont->mpFTFace, mUnicode, FT_LOAD_RENDER );

	int tBoxWidth	= tpSlot->bitmap.width;
	int tBoxHeight	= tpSlot->bitmap.rows;
	int tOriginX	= tpSlot->bitmap_left;
	int tOriginY	= tpSlot->bitmap_top;
	int tPitch		= pTexture->GetTexWidth( );
	unsigned int* tpColour	= pTexture->Lock( );
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

	pTexture->Unlock( );
	mGlyphRect.SetRect( rRect.TopLeft( ), CGUISize( tMaxWidth + 1, mpFont->GetFontHeight( ) ) );
	mAdvanceWidth = int( tpSlot->metrics.horiAdvance / 64.0f );
	return GUI_ERR_SUCCESS;
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

	CGUITexture* tpTexture = NULL;
	if ( tTextureIndex < int( mpFont->mFontTex.size( ) ) )
	{
		// 如果纹理已经建立
		tpTexture = mpFont->mFontTex[ tTextureIndex ];
		FillFont( tpTexture, tFontRect );
	}
	else
	{
		// 如果纹理没有建立
		tpTexture = mpTextureFactory->CreateTexture( CGUIText( "Font%s%d", mpFont->mFaceName.c_str( ), tTextureIndex ), 512, 512 );
		mpFont->mFontTex.push_back( tpTexture );
		FillFont( tpTexture, tFontRect );
	}

	mpFont->mFontUsed ++;
	mIsCreated	= true;
	mpTexture	= tpTexture;
	return GUI_ERR_SUCCESS;
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
	return GUI_ERR_OUTOFCODEPAGE;
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

CGUIFont::CGUIFont( int vFontHeight, const char* pFace, const int vEdge, const int vHOffset, const int vVOffset, CGUICodePage* pCharMap, CGUITextureFactory* pTextureFactory ) : 
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
		mpGlyph[ i ].SetContext( this, pTextureFactory, mpCharMap->GetUnicode( i ) );
	sFTRef ++;
}

CGUIFont::~CGUIFont( )
{
	for ( int i = 0; i < int( mFontTex.size( ) ); i ++ )
        delete mFontTex[ i ];

	// 释放CodePage
	if ( mpCharMap != NULL )
        delete mpCharMap;

	// 释放字体表面
	delete [] mpGlyph;

	// 释放FreeType字体
	FT_Done_Face( mpFTFace );

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
	if ( tGlyphIndex == GUI_ERR_OUTOFCODEPAGE )
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
	if ( tGlyphIndex == GUI_ERR_OUTOFCODEPAGE )
		return GUI_ERR_OUTOFCODEPAGE;

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

int CGUIFont::PrintText( CRenderDevice* pRenderDevice, const char* pText, const CGUIPoint& rPoint, int vColor )
{
	int tGlyphIndex = mpCharMap->GetGlyphIndex( pText );
	if ( tGlyphIndex == GUI_ERR_OUTOFCODEPAGE )
		return GUI_ERR_OUTOFCODEPAGE;
	
	const CGUIRect& rSrcRect = mpGlyph[ tGlyphIndex ].GetGlyphRect( );
	pRenderDevice->MaskBlt( mpGlyph[ tGlyphIndex ].GetGlyphTexture( ), rSrcRect, CGUIRect( rPoint, rSrcRect.Size( ) ), vColor );
	return mpGlyph[ tGlyphIndex ].GetGlyphWidth( );
}

// ************************************************************ //
// CGUIRender
// 渲染功能
// ************************************************************ //

CGUIRender::CGUIRender( )
{
}

CGUIRender::~CGUIRender( )
{
}

// ************************************************************ //
// CRenderDevice
// 渲染屏幕，代表一个显示屏幕，有裁减功能
// ************************************************************ //

CRenderDevice::CRenderDevice( )
{
	mpActiveRender	= NULL;
	mpFont			= NULL;
	mpViewport		= NULL;
}

CRenderDevice::~CRenderDevice( )
{
}

void CRenderDevice::SetViewport( CGUIViewport* pViewport )
{
	mpViewport = pViewport; 
	mClipRect.SetRect( CGUIPoint( 0, 0 ), mpViewport->mSrcSize );
}

CGUIRect CRenderDevice::SetClipRect( const CGUIRect& rClipRect )
{
	CGUIRect tOldRect = mClipRect;
	mClipRect = rClipRect;
	return tOldRect;
}

CGUIFont* CRenderDevice::SetFont( CGUIFont* pFont )
{
	CGUIFont* pOldFont = mpFont;
	mpFont = pFont;
	return pOldFont;
}

int CRenderDevice::PrepareRender( )
{
	return mpActiveRender->PrepareRender( );
}

int CRenderDevice::AfterRender( )
{
	return mpActiveRender->AfterRender( );
}

int CRenderDevice::GetFormatText( char* pBuffer, const char* pText, int vWidth )
{
	if ( GetTextWidth( pText, int( ::strlen( pText ) ) ) < vWidth )
	{
		::memcpy( pBuffer, pText, ::strlen( pText ) );
		return GUI_ERR_SUCCESS;
	}

	int tPreWidth = GetTextWidth( "...", 3 );
	int tWidth = 0, tIndex = 0;
	while( pText[ tIndex ] != 0 )
	{
		int tTextLen = 0; pText[ tIndex ] & 0x80 ? tTextLen = 2 : tTextLen = 1;
		tWidth += GetTextWidth( pText + tIndex, tTextLen );
		if ( tWidth + tPreWidth > vWidth )
			break;

		::memcpy( pBuffer + tIndex, pText + tIndex, tTextLen );
		tIndex += tTextLen;
	}

	::strcat( pBuffer, "..." );
	return GUI_ERR_SUCCESS;
}

int CRenderDevice::DrawCombine( CGUITexture* pTexture, const CGUIRect& rDestRect, const CGUIPoint& rPoint, const CGUISize& rSize )
{
	CGUIRect tRectTL( rPoint, rSize );
	CGUIRect tRectTC( rPoint + CGUISize( rSize.mCX, 0 ), rSize );
	CGUIRect tRectTR( rPoint + CGUISize( rSize.mCX * 2, 0 ), rSize );
	CGUIRect tRectCL( rPoint + CGUISize( 0, rSize.mCY ), rSize );
	CGUIRect tRectCC( rPoint + CGUISize( rSize.mCX, rSize.mCY ), rSize );
	CGUIRect tRectCR( rPoint + CGUISize( rSize.mCX * 2, rSize.mCY ), rSize );
	CGUIRect tRectBL( rPoint + CGUISize( 0, rSize.mCY * 2 ), rSize );
	CGUIRect tRectBC( rPoint + CGUISize( rSize.mCX, rSize.mCY * 2 ), rSize );
	CGUIRect tRectBR( rPoint + CGUISize( rSize.mCX * 2, rSize.mCY * 2 ), rSize );
	
	// 上面一行
	BitBlt( pTexture, tRectTL, CGUIRect( rDestRect.mTopLeft, rSize ), 0xFFFFFFFF );
	BitBlt( pTexture, tRectTR, CGUIRect( rDestRect.TopRight( ) - CGUISize( tRectTR.Width( ), 0 ), rSize ), 0xFFFFFFFF );
	for ( int x = rDestRect.mTopLeft.mX + tRectTL.Width( ); x < rDestRect.mBottomRight.mX - tRectTR.Width( ); x += tRectTC.Width( ) )
	{
		int tWidth = min( tRectTC.Width( ), rDestRect.mBottomRight.mX - tRectTR.Width( ) - x );
		BitBlt( pTexture, CGUIRect( tRectTC.mTopLeft, CGUISize( tWidth, rSize.mCY ) ), CGUIRect( CGUIPoint( x, rDestRect.mTopLeft.mY ), CGUISize( tWidth, rSize.mCY ) ), 0xFFFFFFFF );
	}

	// 中间多行
	for ( int y = rDestRect.mTopLeft.mY + tRectTL.Height( ); y < rDestRect.mBottomRight.mY - tRectBL.Height( ); y += tRectCL.Height( ) )
	{
		int tHeight = min( tRectCC.Height( ), rDestRect.mBottomRight.mY - tRectBL.Height( ) - y );

		BitBlt( pTexture, CGUIRect( tRectCL.mTopLeft, CGUISize( rSize.mCX, tHeight ) ), CGUIRect( CGUIPoint( rDestRect.mTopLeft.mX, y ), CGUISize( rSize.mCX, tHeight ) ), 0xFFFFFFFF );
		BitBlt( pTexture, CGUIRect( tRectCR.mTopLeft, CGUISize( rSize.mCX, tHeight ) ), CGUIRect( CGUIPoint( rDestRect.mBottomRight.mX - tRectCR.Width( ), y ), CGUISize( rSize.mCX, tHeight ) ), 0xFFFFFFFF );
		for ( int x = rDestRect.mTopLeft.mX + tRectCL.Width( ); x < rDestRect.mBottomRight.mX - tRectCR.Width( ); x += tRectCC.Width( ) )
		{
			int tWidth = min( tRectCC.Width( ), rDestRect.mBottomRight.mX - tRectTR.Width( ) - x );
			BitBlt( pTexture, CGUIRect( tRectCC.mTopLeft, CGUISize( tWidth, tHeight ) ), CGUIRect( CGUIPoint( x, y ), CGUISize( tWidth, tHeight ) ), 0xFFFFFFFF );
		}
	}

	// 下面一行

	BitBlt( pTexture, tRectBL, CGUIRect( rDestRect.BottomLeft( ) - CGUISize( 0, rSize.mCY ), rSize ), 0xFFFFFFFF );
	BitBlt( pTexture, tRectBR, CGUIRect( rDestRect.mBottomRight - CGUISize( rSize.mCX, rSize.mCY ), rSize ), 0xFFFFFFFF );
	for ( int x = rDestRect.mTopLeft.mX + tRectBL.Width( ); x < rDestRect.mBottomRight.mX - tRectBR.Width( ); x += tRectBC.Width( ) )
	{
		int tWidth = min( tRectTC.Width( ), rDestRect.mBottomRight.mX - tRectTR.Width( ) - x );
		BitBlt( pTexture, CGUIRect( tRectBC.mTopLeft, CGUISize( tWidth, rSize.mCY ) ), CGUIRect( CGUIPoint( x, rDestRect.mBottomRight.mY - tRectBL.Height( ) ), CGUISize( tWidth, rSize.mCY ) ), 0xFFFFFFFF );
	}

	return GUI_ERR_SUCCESS;
}

int CRenderDevice::DrawLine( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, unsigned int vColor )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	return mpActiveRender->DrawLine( mpViewport->LP2DP( rPoint1 ), mpViewport->LP2DP( rPoint2 ), vColor );
}

int CRenderDevice::DrawRect( const CGUIRect& rDestRect, unsigned int vTopLeftColor, unsigned int vBottomRightColor )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	return mpActiveRender->DrawRect( mpViewport->LP2DP( rDestRect & mClipRect ), vTopLeftColor, vBottomRightColor );
}

int CRenderDevice::FillRect( const CGUIRect& rDestRect, unsigned int vColor )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	return mpActiveRender->FillRect( mpViewport->LP2DP( rDestRect & mClipRect ), vColor );
}

int CRenderDevice::FillTriangle( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, const CGUIPoint& rPoint3, unsigned int vColor )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	return mpActiveRender->FillTriangle( mpViewport->LP2DP( rPoint1 ), mpViewport->LP2DP( rPoint2 ), mpViewport->LP2DP( rPoint3 ), vColor );
}

int CRenderDevice::BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColor )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	// 得到裁减后的矩形
	CGUIRect tClipedRect = rDestRect & mClipRect;
	if ( tClipedRect.IsRectEmpty( ) )
		return GUI_ERR_SUCCESS;

	// 按照裁减后的区域在裁减前的区域的比例计算源矩形
	float tLeft		= ( tClipedRect.mTopLeft.mX - rDestRect.mTopLeft.mX ) / (float) rDestRect.Width( );
	float tTop		= ( tClipedRect.mTopLeft.mY - rDestRect.mTopLeft.mY ) / (float) rDestRect.Height( );
	float tRight	= ( tClipedRect.mBottomRight.mX - rDestRect.mTopLeft.mX ) / (float) rDestRect.Width( );
	float tBottom	= ( tClipedRect.mBottomRight.mY - rDestRect.mTopLeft.mY ) / (float) rDestRect.Height( );

	int tSrcWidth	= rSrcRect.Width( );
	int tSrcHeight	= rSrcRect.Height( );
	
	// 重新计算源矩形
	CGUIRect tSrcRect( rSrcRect.mTopLeft.mX + int( tLeft * tSrcWidth ), rSrcRect.mTopLeft.mY + int( tTop * tSrcHeight ), 
		rSrcRect.mTopLeft.mX + int( tRight * tSrcWidth ), rSrcRect.mTopLeft.mY + int( tBottom * tSrcHeight ) );
	return mpActiveRender->BitBlt( pTexture, tSrcRect, mpViewport->LP2DP( tClipedRect ), vColor );
}

int CRenderDevice::BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, float fRotAngle, const CGUIPoint& Point,  unsigned int vColour )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	// 得到裁减后的矩形
	CGUIRect tClipedRect = rDestRect & mClipRect;
	if ( tClipedRect.IsRectEmpty( ) )
		return GUI_ERR_SUCCESS;

	// 按照裁减后的区域在裁减前的区域的比例计算源矩形
	float tLeft		= ( tClipedRect.mTopLeft.mX - rDestRect.mTopLeft.mX ) / (float) rDestRect.Width( );
	float tTop		= ( tClipedRect.mTopLeft.mY - rDestRect.mTopLeft.mY ) / (float) rDestRect.Height( );
	float tRight	= ( tClipedRect.mBottomRight.mX - rDestRect.mTopLeft.mX ) / (float) rDestRect.Width( );
	float tBottom	= ( tClipedRect.mBottomRight.mY - rDestRect.mTopLeft.mY ) / (float) rDestRect.Height( );

	int tSrcWidth	= rSrcRect.Width( );
	int tSrcHeight	= rSrcRect.Height( );
	
	// 重新计算源矩形
	CGUIRect tSrcRect( rSrcRect.mTopLeft.mX + int( tLeft * tSrcWidth ), rSrcRect.mTopLeft.mY + int( tTop * tSrcHeight ), 
		rSrcRect.mTopLeft.mX + int( tRight * tSrcWidth ), rSrcRect.mTopLeft.mY + int( tBottom * tSrcHeight ) );
	return mpActiveRender->BitBlt( pTexture, tSrcRect, mpViewport->LP2DP( tClipedRect ), fRotAngle, Point, vColour );
}
int CRenderDevice::MaskBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColor )
{
	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	// 得到裁减后的矩形
	CGUIRect tClipedRect = rDestRect & mClipRect;
	if ( tClipedRect.IsRectEmpty( ) )
		return GUI_ERR_SUCCESS;

	// 按照裁减后的区域在裁减前的区域的比例计算源矩形
	float tLeft		= ( tClipedRect.mTopLeft.mX - rDestRect.mTopLeft.mX ) / (float) rDestRect.Width( );
	float tTop		= ( tClipedRect.mTopLeft.mY - rDestRect.mTopLeft.mY ) / (float) rDestRect.Height( );
	float tRight	= ( tClipedRect.mBottomRight.mX - rDestRect.mTopLeft.mX ) / (float) rDestRect.Width( );
	float tBottom	= ( tClipedRect.mBottomRight.mY - rDestRect.mTopLeft.mY ) / (float) rDestRect.Height( );

	int tSrcWidth	= rSrcRect.Width( );
	int tSrcHeight	= rSrcRect.Height( );
	
	// 重新计算源矩形,
	CGUIRect tSrcRect( rSrcRect.mTopLeft.mX + int( tLeft * tSrcWidth ), rSrcRect.mTopLeft.mY + int( tTop * tSrcHeight ), 
		rSrcRect.mTopLeft.mX + int( tRight * tSrcWidth ), rSrcRect.mTopLeft.mY + int( tBottom * tSrcHeight ) );
	return mpActiveRender->MaskBlt( pTexture, tSrcRect, mpViewport->LP2DP( tClipedRect ), vColor );
}

int CRenderDevice::FillClock( float vPer, const CGUIRect& rRect, unsigned int vColor )
{
	int tHalfWidth	= rRect.Width( ) / 2;
	int tHalfHeight	= rRect.Height( ) / 2;
	CGUIPoint tPoint1, tPoint2, tPoint3;
	if ( vPer < 0.125f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( int( rRect.mTopLeft.mX + tHalfWidth + ( vPer / 0.125f ) * tHalfWidth ), rRect.mTopLeft.mY );

		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 0.25f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, int( rRect.mTopLeft.mY + ( ( vPer - 0.125f ) / 0.125f ) * tHalfHeight ) );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 0.375f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, int( rRect.mTopLeft.mY + tHalfHeight + ( ( vPer - 0.25f ) / 0.125f ) * tHalfHeight ) );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 0.5f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( int( rRect.mBottomRight.mX - ( ( vPer - 0.375f ) / 0.125f ) * tHalfWidth ), rRect.mBottomRight.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 0.625f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( int( rRect.mBottomRight.mX - tHalfWidth - ( ( vPer - 0.5f ) / 0.125f ) * tHalfWidth ), rRect.mBottomRight.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 0.75f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomLeft( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomLeft( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mTopLeft.mX, int( rRect.mBottomRight.mY - ( ( vPer - 0.625f ) / 0.125f ) * tHalfWidth ) );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 0.875f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomLeft( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomLeft( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mTopLeft.mX, rRect.mBottomRight.mY - tHalfWidth );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mTopLeft.mX, rRect.mBottomRight.mY - tHalfWidth );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mTopLeft.mX, int( rRect.mBottomRight.mY - tHalfWidth - ( ( vPer - 0.75f ) / 0.125f ) * tHalfWidth ) );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}
	else if ( vPer < 1.0f && vPer > 0.0f )
	{
		tPoint1 = CGUIPoint( rRect.mTopLeft.mX + tHalfWidth, rRect.mTopLeft.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX, rRect.mTopLeft.mY + tHalfHeight );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomRight( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomRight( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mBottomRight.mX - tHalfWidth, rRect.mBottomRight.mY );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.BottomLeft( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.BottomLeft( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( rRect.mTopLeft.mX, rRect.mBottomRight.mY - tHalfWidth );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = CGUIPoint( rRect.mTopLeft.mX, rRect.mBottomRight.mY - tHalfWidth );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = rRect.TopLeft( );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );

		tPoint1 = rRect.TopLeft( );
		tPoint2 = rRect.CenterPoint( );
		tPoint3 = CGUIPoint( int( rRect.mTopLeft.mX + ( ( vPer - 0.875f ) / 0.125f ) * tHalfWidth ), rRect.mTopLeft.mY );
		FillTriangle( tPoint1, tPoint2, tPoint3, vColor );
	}

	return GUI_ERR_SUCCESS;
}

int CRenderDevice::PrintText( const char* pText, const CGUIRect& rDestRect, unsigned int vFormat, unsigned int vColor )
{
	if ( mpFont == NULL )
		return GUI_ERR_INVALIDFONT;

	if ( mpViewport == NULL )
		return GUI_ERR_INVALIDVP;

	// 目前只支持单行显示
	vFormat |= GUI_DT_SINGLELINE;

	int tFontHeight = GetFontHeight( );
	int tTextWidth	= GetTextWidth( pText, int( ::strlen( pText ) ) );
	CGUIPoint tPoint;
	if ( vFormat & GUI_DT_SINGLELINE )
	{
		if ( vFormat & GUI_DT_LEFT )
			tPoint.mX = rDestRect.mTopLeft.mX;
		else if ( vFormat & GUI_DT_RIGHT )
			tPoint.mX = rDestRect.mBottomRight.mX - tTextWidth;
		else if ( vFormat & GUI_DT_HCENTER )
			tPoint.mX = rDestRect.mTopLeft.mX + ( ( rDestRect.Width( ) - tTextWidth ) >> 1 );
		else
			tPoint.mX = rDestRect.mTopLeft.mX;

		if ( vFormat & GUI_DT_TOP )
			tPoint.mY = rDestRect.mTopLeft.mY;
		else if ( vFormat & GUI_DT_BOTTOM )
			tPoint.mY = rDestRect.mBottomRight.mY - tFontHeight;
		else if ( vFormat & GUI_DT_VCENTER )
			tPoint.mY = rDestRect.mTopLeft.mY + ( ( rDestRect.Height( ) - tFontHeight ) >> 1 );
		else
			tPoint.mY = rDestRect.mTopLeft.mY;
	}

	int tIndex = 0;
	while( pText[ tIndex ] != 0 )
	{
		tPoint.mX += mpFont->PrintText( this, pText + tIndex, tPoint, vColor );
		tIndex += pText[ tIndex ] & 0x80 ? 2 : 1;
	}
	return GUI_ERR_SUCCESS;
}

int CRenderDevice::GetMaxTextWidth( )
{
	if ( mpFont == NULL )
		return GUI_ERR_INVALIDFONT;

	return mpFont->GetMaxTextWidth( );
}

int CRenderDevice::GetTextWidth( const char* pText, int vTextCount )
{
	if ( mpFont == NULL )
		return GUI_ERR_INVALIDFONT;

	int tTextWidth = 0;
	for ( int tIndex = 0; tIndex < vTextCount; )
	{
		tTextWidth += mpFont->GetTextWidth( pText + tIndex );
		pText[ tIndex ] & 0x80 ? tIndex += 2 : tIndex ++;
	}
	return tTextWidth;
}

int CRenderDevice::GetFontHeight( )
{
	if ( mpFont == NULL )
		return GUI_ERR_INVALIDFONT;

	return mpFont->GetFontHeight( );
}

}