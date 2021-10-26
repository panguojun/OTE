#pragma once
#include "FBGUIBase.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include "Windows.h"
#include <string>

namespace FBGUISystemLayer
{
//! CGUIText 类
/*!
	格式化字符串的工具类
	CGUIText 通过sprintf格式化字符串
	例子: MessageBox( CGUIText( "%d-%d-%d", GetMonth( ), GetDay( ), GetYear( ) ), NULL, NULL, MB_OK );
*/
class CGUIText
{
protected:
	char mBuffer[ 1024 ];	

public:
    //! CGUIText 构造函数，可以通过任意个参数构造，内部使用sprintf,填充mBuffer
	/*! \param pFormat 格式化控制字符串.
        \param ... 可选参数 */
	CGUIText( const char* pFormat, ... )
	{
		va_list tArgs;

		va_start( tArgs, pFormat );
		::vsprintf( mBuffer, pFormat, tArgs );
		va_end( tArgs );
	}

public:
	//! CGUIText 析造函数
	~CGUIText( ) {	}

public:
	//! char* 操作符
    /*!	CGUIText可以当成char* 使用 */
	operator char* ( )	{ return mBuffer; }
};

//! CGUISize 类
/*!
	CGUISize 比较类似 MFC CSize, 实现了相对坐标  mCX 和 mCY 成员是公开的，另外
	CGUISize 提供了维护这两个数据的成员函数
*/
class CGUISize
{
public:
	//! 横坐标
	int	mCX;
	//! 纵坐标
	int	mCY;

public:
	//! CGUISize 构造函数, 默认构造一个 CGUISize( 0, 0 ) 对象
	CGUISize( ) : mCX( 0 ), mCY( 0 )						{	}

	//! CGUISize 构造函数, 通过两个整数构造一个 CGUISize 对象
	/*! \param int vCX 横坐标
        \param int vCY 纵坐标 */
	CGUISize( int vCX, int vCY ) : mCX( vCX ), mCY( vCY )	{	}

public:
	//! CGUISize 析造函数
	~CGUISize( )											{	}

public:
	//! != 操作符, 判断两个CGUISize是否不一样
    /*! \param const CGUISize& rSize  对象引用
		\return 如果该对象和rSize不一样返回true, 否则返回false */
	bool operator != ( const CGUISize& rSize ) const			
		{ return mCX != rSize.mCX || mCY != rSize.mCY; }

	//! == 操作符, 判断两个CGUISize是否一样
    /*! \param rSize CGUISize 对象引用
		\return 如果该对象和rSize一样返回true, 否则返回false */
	bool operator == ( const CGUISize& rSize ) const			
		{ return mCX == rSize.mCX && mCY == rSize.mCY; }

	//! -= 操作符, 该CGUISize对象减去一个CGUISize对象
    /*! \param rSize CGUISize 对象引用
		\return 没有返回值 */
	void operator -= ( const CGUISize& rSize )
		{ mCX -= rSize.mCX; mCY -= rSize.mCY; }

	//! += 操作符, 该CGUISize对象加上一个rSize对象
    /*! \param rSize CGUISize 对象引用
		\return 没有返回值 */
	void operator += ( const CGUISize& rSize )
		{ mCX += rSize.mCX; mCY += rSize.mCY; }

	//! - 操作符, 这个操作返回rPoint的值减去该CGUISize对象的值
    /*! \param rSize CGUIPoint 对象引用
		\return 返回rPoint的值减去该CGUISize对象的值 */
	CGUIPoint operator - ( const CGUIPoint& rPoint ) const;

	//! - 操作符, 这个操作返回rRect的值减去该CGUISize对象的值
    /*! \param rRect CGUIRect 对象引用
		\return 返回rRect的值减去该CGUISize对象后，构造的CGUIRect拷贝 */
	CGUIRect operator - ( const CGUIRect& rRect ) const;
		
	//! - 操作符, 这个操作返回该CGUISize对象的值减去CGUISize对象后，构造的CGUISize拷贝 */
    /*! \param rSize CGUISize 对象引用
		\return 返回该CGUISize对象的值减去 rSize的值 */
	CGUISize operator - ( const CGUISize& rSize ) const
		{ return CGUISize( mCX - rSize.mCX, mCY - rSize.mCY ); }

	//! - 操作符, 这个操作返回该CGUISize对象的取反后，构造的CGUISize拷贝
    /*! \return 返回该CGUISize对象取反后的值 */
	CGUISize operator - ( ) const
		{ return CGUISize( -mCX, -mCY ); }

	//! + 操作符, 这个操作返回rPoint的值加上该CGUISize对象的值
    /*! \param rPoint CGUIPoint 对象引用
		\return 返回rPoint的值加上该CGUISize对象后，构造的CGUIPoint拷贝 */
	CGUIPoint operator + ( const CGUIPoint& rPoint ) const;

	//! + 操作符, 这个操作返回rRect的值加上该CGUISize对象的值
    /*! \param rRect CGUIRect 对象引用
		\return 返回rRect的值加上该CGUISize对象后，构造的CGUIRect拷贝 */
	CGUIRect operator + ( const CGUIRect& rRect ) const;

	//! + 操作符, 这个操作返回该CGUISize对象的值加上rSize后，构造的CGUISize拷贝
    /*! \param rSize CGUISize 对象引用
		\return 返回该CGUISize对象加上一个rSize对象后，构造的CGUISize拷贝 */
	CGUISize operator + ( const CGUISize& rSize ) const
		{ return CGUISize( mCX + rSize.mCX, mCY + rSize.mCY ); }
};

//! CGUIPoint 类
/*!
	CGUIPoint 比较类似 MFC CPoint, 实现了相对坐标  mX 和 mY 成员是公开的，另外
	CGUIPoint 提供了维护这两个数据的成员函数
*/
class CGUIPoint
{
public:
	int		mX;
	int		mY;

public:
	//! CGUIPoint 构造函数, 默认构造一个 CGUIPoint( 0, 0 ) 对象
	CGUIPoint( ) : mX( 0 ), mY( 0 )						{	}
	//! CGUIPoint 构造函数, 通过两个整数构造一个 CGUIPoint 对象
	/*! \param vX int 型, 横坐标
        \param vY int 型, 纵坐标 */
	CGUIPoint( int vX, int vY ) : mX( vX ), mY( vY )	{	}
public:
	//! CGUIPoint 析造函数
	~CGUIPoint( )										{	}

public:
	//! 成员函数 Offset, 横坐标和纵坐标分别加上vXOffset, vYOffset
	/*! \param vX int 型, 横坐标偏移量
		\param vY int 型, 纵坐标偏移量 */
	void Offset( int vXOffset, int vYOffset )
		{ mX += vXOffset; mY += vYOffset; }

	//! 成员函数 Offset, 横坐标和纵坐标分别加上rPoint.mX, rPoint.mY
    /*! \param rPoint const CGUIPoint型引用, 存储了横向和纵向的偏移量 */
	void Offset( const CGUIPoint& rPoint )
		{ mX += rPoint.mX; mY += rPoint.mY; }

	//! 成员函数 Offset, 横坐标和纵坐标分别偏移rSize.mX, rSize.mY */
    /*! \param rSize const CGUISize型引用, 存储了横向和纵向的偏移量 */
	void Offset( const CGUISize& rSize )
		{ mX += rSize.mCX; mY += rSize.mCY; }

public:
	//! != 操作符, 判断两个CGUIPoint是否不一样 */
    /*! \param rPoint const CGUIPoint型引用
		\return 如果该对象和rPoint不一样返回true, 否则返回false */
	bool operator != ( const CGUIPoint& rPoint ) const
		{ return mX != rPoint.mX || mY != rPoint.mY; }

	//! == 操作符, 判断两个CGUIPoint是否一样 */
    /*! \param rPoint const CGUIPoint型引用
		\return 如果该对象和rPoint一样返回true, 否则返回false */
	bool operator == ( const CGUIPoint& rPoint ) const
		{ return mX == rPoint.mX && mY == rPoint.mY; }

	//! += 操作符, 该操作加上一个CGUISize对象 */
    /*! \param rSize const CGUISize型引用
		\return 没有返回值 */
	void operator += ( const CGUISize& rSize )
		{ mX += rSize.mCX; mY += rSize.mCY; }

	//! += 操作符, 该操作加上一个CGUIPoint对象 */
    /*! \param rPoint const CGUIPoint型引用
		\return 没有返回值 */
	void operator += ( const CGUIPoint& rPoint )
		{ mX += rPoint.mX; mY += rPoint.mY; }

	//! -= 操作符, 该操作减去一个CGUISize对象 */
    /*! \param rSize const CGUISize型引用
		\return 没有返回值 */
	void operator -= ( const CGUISize& rSize )
		{ mX -= rSize.mCX; mY -= rSize.mCY; }

	//! -= 操作符, 该操作减去一个CGUIPoint对象 */
    /*! \param rPoint const CGUIPoint型引用
		\return 没有返回值 */
	void operator -= ( const CGUIPoint& rPoint )
		{ mX -= rPoint.mX; mY -= rPoint.mY; }

	//! - 操作符, 该操作返回该CGUIPoint对象减去一个CGUIPoint对象后，构造的CGUIPoint拷贝 */
    /*! \param rPoint const CGUIPoint型引用
		\return 返回该CGUIPoint对象减去一个CGUIPoint对象后，构造的CGUIPoint拷贝 */
	CGUIPoint operator - ( const CGUIPoint& rPoint ) const
		{ return CGUIPoint( mX - rPoint.mX, mY - rPoint.mY ); }

	//! - 操作符, 该操作返回该CGUIPoint对象减去一个CGUISize对象后，构造的CGUIPoint拷贝
    /*! \param rSize const CGUISize型引用
		\return 返回该CGUIPoint对象减去一个CGUISize对象后，构造的CGUIPoint拷贝 */
	CGUIPoint operator - ( const CGUISize& rSize ) const
		{ return CGUIPoint( mX - rSize.mCX, mY - rSize.mCY ); }

	//! - 操作符, 该操作返回CGUIRect对象减去该CGUIPoint对象后，构造的CGUIRect拷贝
    /*! \param rRect const CGUIRect型引用
		\return 返回CGUIRect对象减去该CGUIPoint对象后，构造的CGUIRect拷贝 */
	CGUIRect operator - ( const CGUIRect& rRect ) const;

	//! - 操作符, 该操作返回该CGUIPoint对象取反后，构造的CGUIPoint拷贝
    /*! \return 返回该CGUIPoint对象取反后，构造的CGUIPoint拷贝 */
	CGUIPoint operator - ( ) const
		{ return CGUIPoint( -mX, -mY ); }

	//! + 操作符, 该操作返回该CGUIPoint对象加上一个CGUIPoint对象后，构造的CGUIPoint拷贝
    /*! \param rPoint const CGUIPoint型引用
		\return 返回该CGUIPoint对象加上一个CGUIPoint对象后，构造的CGUIPoint拷贝 */
	CGUIPoint operator + ( const CGUIPoint& rPoint ) const
		{ return CGUIPoint( mX + rPoint.mX, mY + rPoint.mY ); }

	//! + 操作符, 该操作返回该CGUIPoint对象加上一个CGUISize对象后，构造的CGUIPoint拷贝
    /*! \param rSize const CGUISize型引用
		\return 返回该CGUIPoint对象加上一个CGUISize对象后，构造的CGUIPoint拷贝 */
	CGUIPoint operator + ( const CGUISize& rSize ) const
		{ return CGUIPoint( mX + rSize.mCX, mY + rSize.mCY ); }

	//! + 操作符, 该操作返回CGUIRect对象加上该CGUIPoint对象后，构造的CGUIRect拷贝
    /*! \param rRect const CGUIRect型引用
		\return 返回CGUIRect对象加上该CGUIPoint对象后，构造的CGUIRect拷贝 */
	CGUIRect operator + ( const CGUIRect& rRect ) const;
};

//! CGUIRect 类
/*!
	CGUIRect 比较类似 MFC CGUIRect, 实现了相对矩形坐标  mTopLeft 和 mBottomRight 成员是公开的，另外
	CGUIRect 提供了维护这两个数据的成员函数, CGUIRect的构造函数默认将调用NormalizeRect, 该函数将使
	得一个CGUIRect对象正常化, 比如:mTopLeft( 10, 10 ) 和 mBottomRight( 20, 20 )是一个正常的矩形, 但
	一个mTopLeft( 20, 20 ) 和 mBottomRight( 10, 10 )是一个不正常的矩形, NormalizeRect将使得这样的矩
	形正常化，一个不正常的矩形在使用某些CGUIRect的成员函数时可能会发生错误！
*/
class CGUIRect
{
public:
	//! CGUIPoint 型,矩形的左上角
	CGUIPoint	mTopLeft;
	//! CGUIPoint 型,矩形的右下角
	CGUIPoint	mBottomRight;

public:
	//! CGUIRect 构造函数, 默认构造一个 CGUIRect( 0, 0, 0, 0 ) 对象
	CGUIRect( )	{ NormalizeRect( ); }

	//! CGUIRect 构造函数, 通过4个整数构造 CGUIRect 对象, 
	/*! \param vLeft int 型 左上角横坐标
		\param vTop int 型 左上角纵坐标
		\param vRight int 型 右下角横坐标
		\param vBottom int 型 右下角纵坐标
		\param vNormalize bool 型 是不是要正常化该 CGUIRect 对象 
		\sa CGUIRect */
	CGUIRect( int vLeft, int vTop, int vRight, int vBottom, bool vNormalize = true ) : mTopLeft( vLeft, vTop ), mBottomRight( vRight, vBottom )
		{ if ( vNormalize ) NormalizeRect( ); }

	//! CGUIRect 构造函数, 通过1个 CGUIPoint 和一个 CGUISize 构造 CGUIRect 对象, 
	/*! \param rPoint const CGUIPoint 型引用 左上角坐标
		\param rSize const CGUISize 型引用 CGUIRect尺寸
		\param vNormalize bool 型 是不是要正常化该 CGUIRect 对象 
		\sa CGUIRect */
	CGUIRect( const CGUIPoint& rPoint, const CGUISize& rSize, bool vNormalize = true ) : mTopLeft( rPoint ), mBottomRight( rPoint + rSize )
		{ if ( vNormalize ) NormalizeRect( ); }

	//! CGUIRect 构造函数, 通过2个 CGUIPoint 构造 CGUIRect 对象, 
	/*! \param rTopLeft const CGUIPoint 型引用 左上角坐标
		\param rBottomRight const CGUIPoint 型引用 右下角坐标
		\param vNormalize bool 型 是不是要正常化该 CGUIRect 对象 
		\sa CGUIRect */
	CGUIRect( const CGUIPoint& rTopLeft, const CGUIPoint& rBottomRight, bool vNormalize = true ) : mTopLeft( rTopLeft ), mBottomRight( rBottomRight )
		{ if ( vNormalize ) NormalizeRect( ); }
public:
	//! CGUIRect 析造函数
	~CGUIRect( ) {	}

public:
	//! 成员函数 BottomLeft, 
	/*! \return 返回一个CGUIPoint 的拷贝 */
	CGUIPoint BottomLeft( ) const
		{ return CGUIPoint( mTopLeft.mX, mBottomRight.mY );	}

	//! 成员函数 TopRight, 
	/*! \return 返回一个CGUIPoint 的拷贝 */
	CGUIPoint TopRight( ) const
		{ return CGUIPoint( mBottomRight.mX, mTopLeft.mY );	}

	//! 成员函数 BottomRight, 
	/*! \return 返回一个CGUIPoint 的拷贝 */
	CGUIPoint BottomRight( ) const
		{ return mBottomRight; }

	//! 成员函数 TopLeft, 
	/*! \return 返回一个CGUIPoint 的拷贝 */
	CGUIPoint TopLeft( ) const
		{ return mTopLeft; }

	//! 成员函数 CenterPoint, 
	/*! \return 返回该 CGUIRect 对象的中心点, 一个CGUIPoint 的拷贝 */
	CGUIPoint CenterPoint( ) const
		{ return CGUIPoint( ( mTopLeft.mX + mBottomRight.mX ) >> 1, ( mTopLeft.mY + mBottomRight.mY ) >> 1 ); }

	//! 成员函数 EqualRect, 判断两个对象是否相等
	/*! \param rRect const CGUIRect 型引用
		\return 返回该 CGUIRect 对象和rRect是否相等, 如果相等返回true, 否则返回false */
	bool EqualRect( const CGUIRect& rRect )	const
		{ return mTopLeft == rRect.mTopLeft && mBottomRight == rRect.mBottomRight; }

	//! 成员函数 DeflateRect, 缩小该 CGUIRect 对象, 缩小指的是像中心靠近
	/*! \param vX int 型 mTopLeft的横坐标增加和mBottomRight的横坐标减少量
		\param vY int 型 mTopLeft的纵坐标增加和mBottomRight的纵坐标减少量
		\return 没有返回值 */
	void DeflateRect( int vX, int vY )
		{ mTopLeft.mX += vX; mTopLeft.mY += vY; mBottomRight.mX -= vX; mBottomRight.mY -= vY; }

	//! 成员函数 DeflateRect, 缩小该 CGUIRect 对象, 缩小指的是像中心靠近
	/*! \param rSize const CGUISize 型 rSize的mCX指明了 mTopLeft的横坐标增加和mBottomRight的横坐标减少量
									   rSize的mCY指明了 mTopLeft的纵坐标增加和mBottomRight的纵坐标减少量
		\return 没有返回值 */
	void DeflateRect( const CGUISize& rSize )
	{ 
		mTopLeft.mX		+= rSize.mCX; mTopLeft.mY		+= rSize.mCY; 
		mBottomRight.mX	-= rSize.mCX; mBottomRight.mY	-= rSize.mCY; 
	}

	//! 成员函数 DeflateRect, 缩小该 CGUIRect 对象, 缩小指的是像中心靠近
	/*! \param rRect const CGUIRect 型 rRect 指明了每一个边的缩小量
		\return 没有返回值 */
	void DeflateRect( const CGUIRect& rRect )
	{
		mTopLeft.mX		+= rRect.mTopLeft.mX;		mTopLeft.mY		+= rRect.mTopLeft.mY;		
		mBottomRight.mX -= rRect.mBottomRight.mX;	mBottomRight.mY -= rRect.mBottomRight.mY; 
	}

	//! 成员函数 DeflateRect, 缩小该 CGUIRect 对象, 缩小指的是像中心靠近
	/*! \param vLeft int 型 vLeft 指明了 左边 缩小量
		\param vTop int 型 vTop 指明了 上边 缩小量
		\param vRight int 型 vRight 指明了 右边 缩小量
		\param vBottom int 型 vBottom 指明了 下边 缩小量
		\return 没有返回值 */
	void DeflateRect( int vLeft, int vTop, int vRight, int vBottom )
		{ mTopLeft.mX += vLeft; mTopLeft.mY += vTop; mBottomRight.mX -= vRight; mBottomRight.mY -= vBottom; }

	//! 成员函数 InflateRect, 扩大该 CGUIRect 对象, 扩大指的是像中心发散
	/*! \param vX int 型 mTopLeft的横坐标减少和mBottomRight的横坐标增加量
		\param vY int 型 mTopLeft的纵坐标减少和mBottomRight的纵坐标增加量
		\return 没有返回值 */
	void InflateRect( int vX, int vY )
		{ mTopLeft.mX -= vX; mTopLeft.mY -= vY; mBottomRight.mX += vX; mBottomRight.mY += vY; }

	//! 成员函数 InflateRect, 扩大该 CGUIRect 对象, 扩大指的是像中心发散
	/*! \param rSize const CGUISize 型 rSize的mCX指明了 mTopLeft的横坐标减少和mBottomRight的横坐标增加量
									   rSize的mCY指明了 mTopLeft的纵坐标减少和mBottomRight的纵坐标增加量
		\return 没有返回值 */
	void InflateRect( const CGUISize& rSize )
	{ 
		mTopLeft.mX		-= rSize.mCX;	mTopLeft.mY		-= rSize.mCY;			
		mBottomRight.mX += rSize.mCX;	mBottomRight.mY	+= rSize.mCY; 
	}

	//! 成员函数 InflateRect, 扩大该 CGUIRect 对象, 扩大指的是像中心发散
	/*! \param rRect const CGUIRect 型 rRect 指明了每一个边的扩大量
		\return 没有返回值 */
	void InflateRect( const CGUIRect& rRect )
	{
		mTopLeft.mX		-= rRect.mTopLeft.mX;		mTopLeft.mY		-= rRect.mTopLeft.mY;	
		mBottomRight.mX += rRect.mBottomRight.mX;	mBottomRight.mY += rRect.mBottomRight.mY; 
	}

	//! 成员函数 InflateRect, 扩大该 CGUIRect 对象, 扩大指的是像中心发散
	/*! \param vLeft int 型 vLeft 指明了 左边 扩大量
		\param vTop int 型 vTop 指明了 上边 扩大量
		\param vRight int 型 vRight 指明了 右边 扩大量
		\param vBottom int 型 vBottom 指明了 下边 扩大量
		\return 没有返回值 */
	void InflateRect( int vLeft, int vTop, int vRight, int vBottom )
		{ mTopLeft.mX -= vLeft; mTopLeft.mY -= vTop; mBottomRight.mX += vRight; mBottomRight.mY += vBottom; }

	//! 成员函数 Height, 返回该 CGUIRect 对象的高度
	/*! \return 返回该 CGUIRect 对象的高度 */
	int Height( ) const
		{ return mBottomRight.mY - mTopLeft.mY; }

	//! 成员函数 Width, 返回该 CGUIRect 对象的宽度
	/*! \return 返回该 CGUIRect 对象的宽度 */
	int	Width( ) const	
		{ return mBottomRight.mX - mTopLeft.mX; }

	//! 成员函数 IsRectEmpty, 返回该 CGUIRect 对象的宽和高是不是全大于0
	/*! \return 返回该 CGUIRect 对象是否正常 */
	bool IsRectEmpty( ) const
		{ return Width( ) <= 0 || Height( ) <= 0; }

	//! 成员函数 IsRectNull, 返回该 CGUIRect 对象的四个边是否全为0
	/*! \return 返回该 CGUIRect 对象是否为0 */
	bool IsRectNull( ) const
		{ return mTopLeft.mX == 0 && mTopLeft.mY == 0 && mBottomRight.mX == 0 && mBottomRight.mY == 0; }

	//! 成员函数 MoveToX, 对其该 CGUIRect 对象到vX,纵坐标不变
	/*! \param vX int 型 指明对齐的横坐标
		\return 没有返回值 */
	void MoveToX( int vX )	
		{ mBottomRight.mX = vX + Width( ); mTopLeft.mX = vX; }

	//! 成员函数 MoveToY, 对其该 CGUIRect 对象到vY,横坐标不变
	/*! \param vY int 型 指明对齐的纵坐标
		\return 没有返回值 */
	void MoveToY( int vY )	
		{ mBottomRight.mY = vY + Height( ); mTopLeft.mY = vY; }

	//! 成员函数 MoveToXY, 对其该 CGUIRect 对象到vX, vY
	/*! \param vX int 型 指明对齐的横坐标
		\param vY int 型 指明对齐的纵坐标
		\return 没有返回值 */
	void MoveToXY( int vX, int vY )
	{
		mBottomRight.mX = vX + Width( );	mTopLeft.mX	= vX; 
		mBottomRight.mY = vY + Height( ); 	mTopLeft.mY	= vY;  
	}

	//! 成员函数 MoveToXY, 对其该 CGUIRect 对象到rPoint
	/*! \param rPoint const CGUIPoint 型引用 指明对齐的坐标
		\return 没有返回值 */
	void MoveToXY( const CGUIPoint& rPoint )
	{
		mBottomRight.mX = rPoint.mX + Width( ); 	mTopLeft.mX = rPoint.mX; 
		mBottomRight.mY = rPoint.mY + Height( );	mTopLeft.mY = rPoint.mY; 
	}

	//! 成员函数 OffsetRect, 移动该 CGUIRect 对象
	/*! \param vX int 型 指明横坐标的偏移量
		\param vY int 型 指明纵坐标的偏移量
		\return 没有返回值 */
	void OffsetRect( int vX, int vY )					 
		{ *this += CGUIPoint( vX, vY ); }

	//! 成员函数 OffsetRect, 移动该 CGUIRect 对象
	/*! \param rPoint const CGUIPoint 型引用 指明坐标偏移量
		\return 没有返回值 */
	void OffsetRect( const CGUIPoint& rPoint )
		{ *this += rPoint; }

	//! 成员函数 OffsetRect, 移动该 CGUIRect 对象
	/*! \param rSize const CGUISize 型引用 指明坐标偏移量
		\return 没有返回值 */
	void OffsetRect( const CGUISize& rSize )				 
		{ *this += rSize; }	

	//! 成员函数 SetRect, 设置 CGUIRect 对象
	/*! \param vLeft int 型 CGUIRect对象的左上角横坐标
		\param vTop int 型 CGUIRect对象的左上角纵坐标
		\param vRight int 型 CGUIRect对象的右下角横坐标
		\param vBottom int 型 CGUIRect对象的右下角纵坐标
		\return 没有返回值 */
	void SetRect( int vLeft, int vTop, int vRight, int vBottom )
	{
		mTopLeft.mX		= vLeft;	mTopLeft.mY		= vTop; 
		mBottomRight.mX = vRight;	mBottomRight.mY = vBottom; 
	}

	//! 成员函数 SetRect, 通过1个 CGUIPoint 和1个 CGUISize 设置 CGUIRect 对象, 
	/*! \param rPoint const CGUIPoint 型引用 左上角坐标
		\param rSize const CGUISize 型引用 CGUIRect尺寸	
		\return 没有返回值 */
	void SetRect( const CGUIPoint& rPoint, const CGUISize& rSize )
		{ mTopLeft = rPoint; mBottomRight = rPoint + rSize; }

	//! 成员函数 SetRect, 通过2个 CGUIPoint 设置 CGUIRect 对象, 
	/*! \param rTopLeft const CGUIPoint 型引用 左上角坐标
		\param rBottomRight const CGUIPoint 型引用 右下角坐标
		\return 没有返回值 */
	void SetRect( const CGUIPoint& rTopLeft, const CGUIPoint& rBottomRight )
		{ mTopLeft = rTopLeft; mBottomRight	= rBottomRight; }

	//! 成员函数 SetRectEmpty, 将该 CGUIRect 对象置0, 
	/*! \return 没有返回值 */
	void SetRectEmpty( )
		{ mTopLeft.mX = 0; mTopLeft.mY = 0; mBottomRight.mX = 0; mBottomRight.mY = 0; }

	//! 成员函数 Size, 返回该 CGUIRect 对象的宽度和高度
	/*! \return 返回该 CGUIRect 对象的宽度和高度 */
	CGUISize Size( ) const
		{ return CGUISize( Width( ), Height( ) ); }

	//! 成员函数 NormalizeRect, 正常化该 CGUIRect 对象
	/*! \return 没有返回值 */
	void NormalizeRect( );

	//! 成员函数 PtInRect, 判断指定坐标是否在该 CGUIRect 对象中
    /*! \param rPoint const CGUIPoint型引用 待判断的坐标
		\return 返回指定坐标是不是在该 CGUIRect 对象中 */
	bool PtInRect( const CGUIPoint&	rPoint ) const;

	//! 成员函数 IntersectRect, 计算两个 CGUIRect 对象的交集
	/*! \brief 如果计算产生的矩形不正常, 那么该矩形将被清零
		\brief eg. CGUIRect( ).( CGUIRect( 0, 0, 10, 10 ), CGUIRect( 5, 5, 15, 15 ) ) == CGUIRect( 5, 5, 10, 10 )
		\param const CGUIRect& rSrcRect1 源矩形
		\param const CGUIRect& rSrcRect2 源矩形
		\return 如果传入的两个源矩形或是计算后产生的矩形不正常，将返回false, 否这返回true */
   	bool IntersectRect( const CGUIRect& rSrcRect1, const CGUIRect& rSrcRect2 );

	//! 成员函数 UnionRect, 计算两个 CGUIRect 对象的并集
	/*! \brief 如果传入的两个源矩形有一个不正常, 函数将返回false
		\brief eg. CGUIRect( ).( CGUIRect( 0, 0, 10, 10 ), CGUIRect( 5, 5, 15, 15 ) ) == CGUIRect( 0, 0, 15, 15 )
		\param const CGUIRect& rSrcRect1 源矩形
		\param const CGUIRect& rSrcRect2 源矩形
		\return 如果传入的两个源矩形有一个不正常, 函数将返回false, 否这返回true */
	bool UnionRect( const CGUIRect& rSrcRectl, const CGUIRect& rSrcRect2 );
public:
	//! != 操作符, 判断两个 CGUIRect 对象是否不相等
	/*! \param const CGUIRect& rRect 
		\return 返回该 CGUIRect 对象和rRect是否相等, 如果不相等返回true, 否则返回false */
	bool operator != ( const CGUIRect& rRect ) const
		{ return !EqualRect( rRect ); }

	//! == 操作符, 判断两个 CGUIRect 对象是否相等
	/*! \param const CGUIRect& rRect 
		\return 返回该 CGUIRect 对象和rRect是否相等, 如果相等返回true, 否则返回false */
	bool operator == ( const CGUIRect& rRect ) const
		{ return EqualRect( rRect ); }

	//! &= 操作符, 计算该 CGUIRect 对象和 指定 CGUIRect 对象的交集
	/*! \param const CGUIRect& rRect 
		\return 没有返回值 */
	void operator &= ( const CGUIRect& rRect )
		{ IntersectRect( *this, rRect ); }

	//! |= 操作符, 计算该 CGUIRect 对象和 指定 CGUIRect 对象的并集
	/*! \param const CGUIRect& rRect 
		\return 没有返回值 */
	void operator |= ( const CGUIRect& rRect )
		{ UnionRect( *this, rRect ); }

	//! & 操作符, 计算该 CGUIRect 对象和 指定 CGUIRect 对象的交集
	/*! \param const CGUIRect& rRect 
		\return 计算该 CGUIRect 对象和 指定 CGUIRect 对象的并集, 返回 CGUIRect 的拷贝  */
	CGUIRect operator & ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult;
		tRectResult.IntersectRect( *this, rRect );
		return tRectResult;
	}

	//! | 操作符, 计算该 CGUIRect 对象和 指定 CGUIRect 对象的并集
	/*! \param const CGUIRect& rRect 
		\return 计算该 CGUIRect 对象和 指定 CGUIRect 对象的并集, 返回 CGUIRect 的拷贝  */
	CGUIRect operator | ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult;
		tRectResult.UnionRect( *this, rRect );
		return tRectResult;
	}

	//! - 操作符, 把该 CGUIRect 反向偏移 rPoint 个单位
	/*! \param const CGUIPoint& rPoint
		\return 把该 CGUIRect 反向偏移 rPoint 个单位, 返回 CGUIRect 的拷贝  */
	CGUIRect operator - ( const CGUIPoint& rPoint ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( -rPoint );
		return tRectResult;
	}

	//! - 操作符, 把该 CGUIRect 反向偏移 rSize 个单位
	/*! \param const CGUISize& rSize
		\return 把该 CGUIRect 反向偏移 rSize 个单位, 返回 CGUIRect 的拷贝  */
	CGUIRect operator - ( const CGUISize& rSize ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( -rSize );
		return tRectResult;
	}

	//! - 操作符, 缩小该 CGUIRect 对象，缩小的尺寸由 rRect 指定
	/*! \param const CGUIRect& rRect
		\return 缩小该 CGUIRect 对象，缩小的尺寸由 rRect 指定, 返回 CGUIRect 的拷贝  */
	CGUIRect operator - ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.DeflateRect( rRect );
		return tRectResult;
	}

	//! + 操作符, 把该 CGUIRect 正向偏移 rPoint 个单位
	/*! \param const CGUIPoint& rPoint
		\return 把该 CGUIRect 正向偏移 rPoint 个单位, 返回 CGUIRect 的拷贝  */
	CGUIRect operator + ( const CGUIPoint& rPoint ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( rPoint );
		return tRectResult;
	}

	//! + 操作符, 把该 CGUIRect 正向偏移 rSize 个单位
	/*! \param const CGUISize& rSize
		\return 把该 CGUIRect 正向偏移 rSize 个单位, 返回 CGUIRect 的拷贝  */
	CGUIRect operator + ( const CGUISize& rSize ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( rSize );
		return tRectResult;
	}

	//! + 操作符, 扩大该 CGUIRect 对象，扩大的尺寸由 rRect 指定
	/*! \param const CGUIRect& rRect
		\return 扩大该 CGUIRect 对象，扩大的尺寸由 rRect 指定, 返回 CGUIRect 的拷贝  */
	CGUIRect operator + ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.InflateRect( rRect );
		return tRectResult;
	}

	//! += 操作符, 把该 CGUIRect 正向偏移 rPoint 个单位
	/*! \param const CGUIPoint& rPoint
		\return 没有返回值  */
	void operator += ( const CGUIPoint& rPoint )
		{ mTopLeft += rPoint; mBottomRight += rPoint; }

	//! += 操作符, 把该 CGUIRect 正向偏移 rSize 个单位
	/*! \param const CGUISize& rSize
		\return 没有返回值  */
	void operator += ( const CGUISize& rSize )
		{ mTopLeft += rSize; mBottomRight += rSize; }

	//! += 操作符, 扩大该 CGUIRect 对象，扩大的尺寸由 rRect 指定
	/*! \param const CGUIRect& rRect
		\return 没有返回值  */
	void operator += ( const CGUIRect& rRect )
		{ InflateRect( rRect );	}

	//! += 操作符, 把该 CGUIRect 反向偏移 rPoint 个单位
	/*! \param const CGUIPoint& rPoint
		\return 没有返回值  */
	void operator -= ( const CGUIPoint& rPoint )
		{ mTopLeft -= rPoint; mBottomRight -= rPoint; }

	//! += 操作符, 把该 CGUIRect 反向偏移 rSize 个单位
	/*! \param const CGUISize& rSize
		\return 没有返回值  */
	void operator -= ( const CGUISize& rSize )
		{ mTopLeft -= rSize; mBottomRight -= rSize;	}

	//! += 操作符, 缩小该 CGUIRect 对象，缩小的尺寸由 rRect 指定
	/*! \param const CGUIRect& rRect
		\return 没有返回值  */
	void operator -= ( const CGUIRect& rRect )
		{ DeflateRect( rRect );	}
};

// ************************************************************ //
// CGUIViewport
// @remarks
//    该类定义了一个逻辑屏幕，完成从逻辑坐标到屏幕坐标的转换
// @par
//    必须通过四个整数,或是两个CGUISize构造CGUIViewport
// ************************************************************ //

class CGUIViewport
{
public:
	CGUISize mSrcSize;
	CGUISize mDestSize;

public:
	CGUIViewport( int vSrcWidth = 0, int vSrcHeight = 0, int vDestWidth = 0, int vDestHeight = 0 );
	CGUIViewport( const CGUISize& rSrcSize, const CGUISize& rDestSize );
public:
	~CGUIViewport( );

public:
	void SetWindowExt( int vSrcWidth, int vSrcHeight );
	void SetWindowExt( const CGUISize& rSrcSize );
	void SetViewportExt( int vDestWidth, int vDestHeight );
	void SetViewportExt( const CGUISize& rDestSize );

public:
	CGUIPoint	LP2DP( const CGUIPoint& rPoint );
	CGUISize	LP2DP( const CGUISize& rSize );
	CGUIRect	LP2DP( const CGUIRect& rRect );
	CGUIPoint	DP2LP( const CGUIPoint& rPoint );
	CGUISize	DP2LP( const CGUISize& rSize );
	CGUIRect	DP2LP( const CGUIRect& rRect );
};

// ************************************************************ //
// CGUITexture
// @remarks
//    该类定义了纹理格式的抽象概念。
// @par
// ************************************************************ //

class CGUITexture
{
public:
	CGUITexture( );
public:
	virtual ~CGUITexture( );

public:
	virtual int GetTexWidth( )					= 0;
	virtual int GetTexHeight( )					= 0;
	virtual const std::string& GetTexName( )	= 0;
	virtual unsigned int* Lock( )				= 0;
	virtual int Unlock( )						= 0;
};

// ************************************************************ //
// CGUITextureFactory
// @remarks
//    该类定义了纹理工厂的抽象概念。
// @par
// ************************************************************ //

class CGUITextureFactory
{
public:
	CGUITextureFactory( );
public:
	virtual ~CGUITextureFactory( );

public:
	virtual CGUITexture* LoadTexture( const char* pTexName ) = 0;
	virtual CGUITexture* CreateTexture( const char* pTexName, int vWidth, int vHeight ) = 0;
};

// ************************************************************ //
// CGUIRender
// @remarks
//    该类定义了渲染器的抽象概念。
// @par
// ************************************************************ //

class CGUIRender
{
public:
	CGUIRender( );
public:
	virtual ~CGUIRender( );

public:
	// 准备渲染状态
	virtual int PrepareRender( ) = 0;
	// 渲染后处理
	virtual int AfterRender( ) = 0;
	// 填充三角形
	virtual int FillTriangle( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, const CGUIPoint& rPoint3, unsigned int vColor ) = 0;
	// 画直线
	virtual int DrawLine( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, unsigned int vColor ) = 0;
	// 画矩形
	virtual int DrawRect( const CGUIRect& rDestRect, unsigned int vTopLeftColor, unsigned int vBottomRightColor ) = 0;
	// 填充矩形
	virtual int FillRect( const CGUIRect& rDestRect, unsigned int vColor ) = 0;
	// 贴图
	virtual int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColor ) = 0;

	virtual int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, float fRotAngle, const CGUIPoint& Point,  unsigned int vColour ) = 0;
	// 高亮贴图
	virtual int MaskBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColor ) = 0;
};

// ************************************************************ //
// CFontTexture
// ************************************************************ //

class CGUIFontGlyph
{
	friend class CGUIFont;
protected:
	CGUITextureFactory*		mpTextureFactory;	// 纹理工厂
	CGUITexture*			mpTexture;			// 字形纹理
	CGUIRect				mGlyphRect;			// 字形在纹理的位置
	int						mAdvanceWidth;		// 字形逻辑宽度
	bool					mIsCreated;			// 是否字体已经建立
	wchar_t					mUnicode;			// UNICODE编码
	CGUIFont*				mpFont;				// 字体描述

public:
	CGUIFontGlyph( );
public:
	virtual ~CGUIFontGlyph( );

protected:
	int CreateGlyph( );
	int FillFont( CGUITexture* pTexture, const CGUIRect& rRect );
	void SetContext( CGUIFont* tpFont, CGUITextureFactory* pTextureFactory, wchar_t tUnicode ) 
		{ mpFont = tpFont; mpTextureFactory = pTextureFactory; mUnicode = tUnicode; }

public:
	CGUITexture* GetGlyphTexture( );							// 得到字形纹理，pText是多字节，指向一个字符
	int GetGlyphWidth( );										// 得到逻辑宽度，pText是多字节，指向一个字符
	CGUIRect GetGlyphRect( );									// 得到字形在纹理中的位置，pText是多字节，指向一个字符
};

// ************************************************************ //
// CCodePage
// ************************************************************ //

class CGUICodePage
{
public:
	CGUICodePage( );
public:
	virtual ~CGUICodePage( );

public:
	virtual int GetGlyphIndex( const char* pText ) const = 0;		// pText是一个多字节字符或是多字节中文， 返回索引
	virtual int GetCodeSpace( ) const = 0;							// 得到字符内码空间
	virtual wchar_t GetUnicode( int vGlphyIndex ) const = 0;		// 得到索引对应的Unicode编码
};

// ************************************************************ //
// CGBKPage
// ************************************************************ //

class CGUIGBKPage : public CGUICodePage
{
protected:
	enum CODE_SPACE
	{
		GB_MIN_1ST		= 0x00A1,
		GB_MAX_1ST		= 0x00FE,
		GB_MIN_2ND		= 0x00A1,
		GB_MAX_2ND		= 0x00FE,
	};

public:
	CGUIGBKPage( );
public:
	virtual ~CGUIGBKPage( );

public:
	virtual int GetGlyphIndex( const char* pText ) const;		// pText是'\0'结束的字符串，只能是一个字符或是一个中文， 返回索引
	virtual int GetCodeSpace( ) const;							// 得到字符内码空间
	virtual wchar_t GetUnicode( int vGlphyIndex ) const;		// 得到索引对应的Unicode编码
};

// ************************************************************ //
// CGUIFont
// ************************************************************ //

class CGUIFont
{
	friend class CGUIFontGlyph;

protected:
	static int			sFTRef;							// freetype字体库引用计数
	static FT_Library	spFTLibrary;					// freetype字体库

protected:
	std::vector< CGUITexture* >		mFontTex;			// 字符纹理
	FT_Face							mpFTFace;			// FreeType字体表面
	CGUICodePage*					mpCharMap;			// 代码页
	CGUIFontGlyph*					mpGlyph;			// 字符集
	std::string						mFaceName;			// 字体名称
	int								mDpi;				// 解析度
	int								mEdge;				// 边宽
	int								mHOffset;			// 边横向偏移
	int								mVOffset;			// 边纵向偏移
	int								mUnitWidth;			// 字体单元宽度
	int								mUnitHeight;		// 字体单元高度
	int								mFontUsed;			// 创建	了多少个字体

public:
	CGUIFont( int vFontHeight, const char* pFace, const int vEdge = 0, const int vHOffset = 0, const int vVOffset = 0, 
		CGUICodePage* pCharMap = NULL, CGUITextureFactory* pTextureFactory = NULL );
public:
	~CGUIFont( );

public:
	CGUIFontGlyph* GetFontGlyph( const char* pText );						// 得到一个字符字形
	int GetMaxTextWidth( );													// 得到最大字符宽度
	int GetTextWidth( const char* pText );									// 得到一个字符文本宽度
	int GetFontHeight( );													// 得到一个字符高度
	int GetTextWidth( const char* pText, int vTextCount );					// 得到指定文本宽度
	int PrintText( CRenderDevice* pRenderDevice, const char* pText, const CGUIPoint& rPoint, int vColor ); // 输出一个字符
};

// ************************************************************ //
// CRenderDevice
// @remarks
//    该类定义了一个抽象渲染设备，代表一张可以渲染的画布，
//	  画布的大小由mpViewport决定,并有裁减功能
// @par
// ************************************************************ //

class CRenderDevice
{
public:
	CGUIRender*		mpActiveRender;		// 当前渲染器
	CGUIFont*		mpFont;				// 当前字体
	CGUIViewport*	mpViewport;			// 视口
	CGUIRect		mClipRect;			// 裁减区域

public:
	CRenderDevice( );

public:
	~CRenderDevice( );

public:
	void SetRender( CGUIRender* pRender )				{ mpActiveRender = pRender; }
	void SetViewport( CGUIViewport* pViewport );
	CGUIRect GetClipRect( ) const						{ return mClipRect;			}
	CGUIRect SetClipRect( const CGUIRect& rClipRect );
	CGUIFont* SetFont( CGUIFont* pFont );

public:
	int PrepareRender( );
	int AfterRender( );
	int GetFormatText( char* pBuffer, const char* pText, int vWidth );
	int DrawCombine( CGUITexture* pTexture, const CGUIRect& rDestRect, const CGUIPoint& rPoint, const CGUISize& rSize );
	int DrawLine( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, unsigned int vColor );
	int DrawRect( const CGUIRect& rDestRect, unsigned int vTopLeftColor, unsigned int vBottomRightColor );
	int FillClock( float vPer, const CGUIRect& rRect, unsigned int vColor ); 
	int FillTriangle( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, const CGUIPoint& rPoint3, unsigned int vColor );
	int FillRect( const CGUIRect& rDestRect, unsigned int vColor );
	int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColor );
	int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, float fRotAngle, const CGUIPoint& Point,  unsigned int vColour );
	int MaskBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColor );
	int PrintText( const char* pText, const CGUIRect& rDestRect, unsigned int vFormat = GUI_DT_LEFT | GUI_DT_VCENTER, unsigned int vColor = 0xFFFFC700 );
	int GetTextWidth( const char* pText, int vTextCount );
	int GetFontHeight( );
	int GetMaxTextWidth( );
};

};