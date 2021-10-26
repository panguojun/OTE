#pragma once
#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace D3DFont
{

class CGUIPoint;
class CGUISize;
class CGUIRect;
//! CGUIText ��
/*!
	��ʽ���ַ����Ĺ�����
	CGUIText ͨ��sprintf��ʽ���ַ���
	����: MessageBox( CGUIText( "%d-%d-%d", GetMonth( ), GetDay( ), GetYear( ) ), NULL, NULL, MB_OK );
*/
class CGUIText
{
protected:
	char mBuffer[ 1024 ];	

public:
    //! CGUIText ���캯��������ͨ��������������죬�ڲ�ʹ��sprintf,���mBuffer
	/*! \param pFormat ��ʽ�������ַ���.
        \param ... ��ѡ���� */
	CGUIText( const char* pFormat, ... )
	{
		va_list tArgs;

		va_start( tArgs, pFormat );
		::vsprintf( mBuffer, pFormat, tArgs );
		va_end( tArgs );
	}

public:
	//! CGUIText ���캯��
	~CGUIText( ) {	}

public:
	//! char* ������
    /*!	CGUIText���Ե���char* ʹ�� */
	operator char* ( )	{ return mBuffer; }
};

//! CGUISize ��
/*!
	CGUISize �Ƚ����� MFC CSize, ʵ�����������  mCX �� mCY ��Ա�ǹ����ģ�����
	CGUISize �ṩ��ά�����������ݵĳ�Ա����
*/
class CGUISize
{
public:
	//! ������
	int	mCX;
	//! ������
	int	mCY;

public:
	//! CGUISize ���캯��, Ĭ�Ϲ���һ�� CGUISize( 0, 0 ) ����
	CGUISize( ) : mCX( 0 ), mCY( 0 )						{	}

	//! CGUISize ���캯��, ͨ��������������һ�� CGUISize ����
	/*! \param int vCX ������
        \param int vCY ������ */
	CGUISize( int vCX, int vCY ) : mCX( vCX ), mCY( vCY )	{	}

public:
	//! CGUISize ���캯��
	~CGUISize( )											{	}

public:
	//! != ������, �ж�����CGUISize�Ƿ�һ��
    /*! \param const CGUISize& rSize  ��������
		\return ����ö����rSize��һ������true, ���򷵻�false */
	bool operator != ( const CGUISize& rSize ) const			
		{ return mCX != rSize.mCX || mCY != rSize.mCY; }

	//! == ������, �ж�����CGUISize�Ƿ�һ��
    /*! \param rSize CGUISize ��������
		\return ����ö����rSizeһ������true, ���򷵻�false */
	bool operator == ( const CGUISize& rSize ) const			
		{ return mCX == rSize.mCX && mCY == rSize.mCY; }

	//! -= ������, ��CGUISize�����ȥһ��CGUISize����
    /*! \param rSize CGUISize ��������
		\return û�з���ֵ */
	void operator -= ( const CGUISize& rSize )
		{ mCX -= rSize.mCX; mCY -= rSize.mCY; }

	//! += ������, ��CGUISize�������һ��rSize����
    /*! \param rSize CGUISize ��������
		\return û�з���ֵ */
	void operator += ( const CGUISize& rSize )
		{ mCX += rSize.mCX; mCY += rSize.mCY; }

	//! - ������, �����������rPoint��ֵ��ȥ��CGUISize�����ֵ
    /*! \param rSize CGUIPoint ��������
		\return ����rPoint��ֵ��ȥ��CGUISize�����ֵ */
	CGUIPoint operator - ( const CGUIPoint& rPoint ) const;

	//! - ������, �����������rRect��ֵ��ȥ��CGUISize�����ֵ
    /*! \param rRect CGUIRect ��������
		\return ����rRect��ֵ��ȥ��CGUISize����󣬹����CGUIRect���� */
	CGUIRect operator - ( const CGUIRect& rRect ) const;
		
	//! - ������, ����������ظ�CGUISize�����ֵ��ȥCGUISize����󣬹����CGUISize���� */
    /*! \param rSize CGUISize ��������
		\return ���ظ�CGUISize�����ֵ��ȥ rSize��ֵ */
	CGUISize operator - ( const CGUISize& rSize ) const
		{ return CGUISize( mCX - rSize.mCX, mCY - rSize.mCY ); }

	//! - ������, ����������ظ�CGUISize�����ȡ���󣬹����CGUISize����
    /*! \return ���ظ�CGUISize����ȡ�����ֵ */
	CGUISize operator - ( ) const
		{ return CGUISize( -mCX, -mCY ); }

	//! + ������, �����������rPoint��ֵ���ϸ�CGUISize�����ֵ
    /*! \param rPoint CGUIPoint ��������
		\return ����rPoint��ֵ���ϸ�CGUISize����󣬹����CGUIPoint���� */
	CGUIPoint operator + ( const CGUIPoint& rPoint ) const;

	//! + ������, �����������rRect��ֵ���ϸ�CGUISize�����ֵ
    /*! \param rRect CGUIRect ��������
		\return ����rRect��ֵ���ϸ�CGUISize����󣬹����CGUIRect���� */
	CGUIRect operator + ( const CGUIRect& rRect ) const;

	//! + ������, ����������ظ�CGUISize�����ֵ����rSize�󣬹����CGUISize����
    /*! \param rSize CGUISize ��������
		\return ���ظ�CGUISize�������һ��rSize����󣬹����CGUISize���� */
	CGUISize operator + ( const CGUISize& rSize ) const
		{ return CGUISize( mCX + rSize.mCX, mCY + rSize.mCY ); }
};

//! CGUIPoint ��
/*!
	CGUIPoint �Ƚ����� MFC CPoint, ʵ�����������  mX �� mY ��Ա�ǹ����ģ�����
	CGUIPoint �ṩ��ά�����������ݵĳ�Ա����
*/
class CGUIPoint
{
public:
	int		mX;
	int		mY;

public:
	//! CGUIPoint ���캯��, Ĭ�Ϲ���һ�� CGUIPoint( 0, 0 ) ����
	CGUIPoint( ) : mX( 0 ), mY( 0 )						{	}
	//! CGUIPoint ���캯��, ͨ��������������һ�� CGUIPoint ����
	/*! \param vX int ��, ������
        \param vY int ��, ������ */
	CGUIPoint( int vX, int vY ) : mX( vX ), mY( vY )	{	}
public:
	//! CGUIPoint ���캯��
	~CGUIPoint( )										{	}

public:
	//! ��Ա���� Offset, �������������ֱ����vXOffset, vYOffset
	/*! \param vX int ��, ������ƫ����
		\param vY int ��, ������ƫ���� */
	void Offset( int vXOffset, int vYOffset )
		{ mX += vXOffset; mY += vYOffset; }

	//! ��Ա���� Offset, �������������ֱ����rPoint.mX, rPoint.mY
    /*! \param rPoint const CGUIPoint������, �洢�˺���������ƫ���� */
	void Offset( const CGUIPoint& rPoint )
		{ mX += rPoint.mX; mY += rPoint.mY; }

	//! ��Ա���� Offset, �������������ֱ�ƫ��rSize.mX, rSize.mY */
    /*! \param rSize const CGUISize������, �洢�˺���������ƫ���� */
	void Offset( const CGUISize& rSize )
		{ mX += rSize.mCX; mY += rSize.mCY; }

public:
	//! != ������, �ж�����CGUIPoint�Ƿ�һ�� */
    /*! \param rPoint const CGUIPoint������
		\return ����ö����rPoint��һ������true, ���򷵻�false */
	bool operator != ( const CGUIPoint& rPoint ) const
		{ return mX != rPoint.mX || mY != rPoint.mY; }

	//! == ������, �ж�����CGUIPoint�Ƿ�һ�� */
    /*! \param rPoint const CGUIPoint������
		\return ����ö����rPointһ������true, ���򷵻�false */
	bool operator == ( const CGUIPoint& rPoint ) const
		{ return mX == rPoint.mX && mY == rPoint.mY; }

	//! += ������, �ò�������һ��CGUISize���� */
    /*! \param rSize const CGUISize������
		\return û�з���ֵ */
	void operator += ( const CGUISize& rSize )
		{ mX += rSize.mCX; mY += rSize.mCY; }

	//! += ������, �ò�������һ��CGUIPoint���� */
    /*! \param rPoint const CGUIPoint������
		\return û�з���ֵ */
	void operator += ( const CGUIPoint& rPoint )
		{ mX += rPoint.mX; mY += rPoint.mY; }

	//! -= ������, �ò�����ȥһ��CGUISize���� */
    /*! \param rSize const CGUISize������
		\return û�з���ֵ */
	void operator -= ( const CGUISize& rSize )
		{ mX -= rSize.mCX; mY -= rSize.mCY; }

	//! -= ������, �ò�����ȥһ��CGUIPoint���� */
    /*! \param rPoint const CGUIPoint������
		\return û�з���ֵ */
	void operator -= ( const CGUIPoint& rPoint )
		{ mX -= rPoint.mX; mY -= rPoint.mY; }

	//! - ������, �ò������ظ�CGUIPoint�����ȥһ��CGUIPoint����󣬹����CGUIPoint���� */
    /*! \param rPoint const CGUIPoint������
		\return ���ظ�CGUIPoint�����ȥһ��CGUIPoint����󣬹����CGUIPoint���� */
	CGUIPoint operator - ( const CGUIPoint& rPoint ) const
		{ return CGUIPoint( mX - rPoint.mX, mY - rPoint.mY ); }

	//! - ������, �ò������ظ�CGUIPoint�����ȥһ��CGUISize����󣬹����CGUIPoint����
    /*! \param rSize const CGUISize������
		\return ���ظ�CGUIPoint�����ȥһ��CGUISize����󣬹����CGUIPoint���� */
	CGUIPoint operator - ( const CGUISize& rSize ) const
		{ return CGUIPoint( mX - rSize.mCX, mY - rSize.mCY ); }

	//! - ������, �ò�������CGUIRect�����ȥ��CGUIPoint����󣬹����CGUIRect����
    /*! \param rRect const CGUIRect������
		\return ����CGUIRect�����ȥ��CGUIPoint����󣬹����CGUIRect���� */
	CGUIRect operator - ( const CGUIRect& rRect ) const;

	//! - ������, �ò������ظ�CGUIPoint����ȡ���󣬹����CGUIPoint����
    /*! \return ���ظ�CGUIPoint����ȡ���󣬹����CGUIPoint���� */
	CGUIPoint operator - ( ) const
		{ return CGUIPoint( -mX, -mY ); }

	//! + ������, �ò������ظ�CGUIPoint�������һ��CGUIPoint����󣬹����CGUIPoint����
    /*! \param rPoint const CGUIPoint������
		\return ���ظ�CGUIPoint�������һ��CGUIPoint����󣬹����CGUIPoint���� */
	CGUIPoint operator + ( const CGUIPoint& rPoint ) const
		{ return CGUIPoint( mX + rPoint.mX, mY + rPoint.mY ); }

	//! + ������, �ò������ظ�CGUIPoint�������һ��CGUISize����󣬹����CGUIPoint����
    /*! \param rSize const CGUISize������
		\return ���ظ�CGUIPoint�������һ��CGUISize����󣬹����CGUIPoint���� */
	CGUIPoint operator + ( const CGUISize& rSize ) const
		{ return CGUIPoint( mX + rSize.mCX, mY + rSize.mCY ); }

	//! + ������, �ò�������CGUIRect������ϸ�CGUIPoint����󣬹����CGUIRect����
    /*! \param rRect const CGUIRect������
		\return ����CGUIRect������ϸ�CGUIPoint����󣬹����CGUIRect���� */
	CGUIRect operator + ( const CGUIRect& rRect ) const;
};

//! CGUIRect ��
/*!
	CGUIRect �Ƚ����� MFC CGUIRect, ʵ������Ծ�������  mTopLeft �� mBottomRight ��Ա�ǹ����ģ�����
	CGUIRect �ṩ��ά�����������ݵĳ�Ա����, CGUIRect�Ĺ��캯��Ĭ�Ͻ�����NormalizeRect, �ú�����ʹ
	��һ��CGUIRect����������, ����:mTopLeft( 10, 10 ) �� mBottomRight( 20, 20 )��һ�������ľ���, ��
	һ��mTopLeft( 20, 20 ) �� mBottomRight( 10, 10 )��һ���������ľ���, NormalizeRect��ʹ�������ľ�
	����������һ���������ľ�����ʹ��ĳЩCGUIRect�ĳ�Ա����ʱ���ܻᷢ������
*/
class CGUIRect
{
public:
	//! CGUIPoint ��,���ε����Ͻ�
	CGUIPoint	mTopLeft;
	//! CGUIPoint ��,���ε����½�
	CGUIPoint	mBottomRight;

public:
	//! CGUIRect ���캯��, Ĭ�Ϲ���һ�� CGUIRect( 0, 0, 0, 0 ) ����
	CGUIRect( )	{ NormalizeRect( ); }

	//! CGUIRect ���캯��, ͨ��4���������� CGUIRect ����, 
	/*! \param vLeft int �� ���ϽǺ�����
		\param vTop int �� ���Ͻ�������
		\param vRight int �� ���½Ǻ�����
		\param vBottom int �� ���½�������
		\param vNormalize bool �� �ǲ���Ҫ�������� CGUIRect ���� 
		\sa CGUIRect */
	CGUIRect( int vLeft, int vTop, int vRight, int vBottom, bool vNormalize = true ) : mTopLeft( vLeft, vTop ), mBottomRight( vRight, vBottom )
		{ if ( vNormalize ) NormalizeRect( ); }

	//! CGUIRect ���캯��, ͨ��1�� CGUIPoint ��һ�� CGUISize ���� CGUIRect ����, 
	/*! \param rPoint const CGUIPoint ������ ���Ͻ�����
		\param rSize const CGUISize ������ CGUIRect�ߴ�
		\param vNormalize bool �� �ǲ���Ҫ�������� CGUIRect ���� 
		\sa CGUIRect */
	CGUIRect( const CGUIPoint& rPoint, const CGUISize& rSize, bool vNormalize = true ) : mTopLeft( rPoint ), mBottomRight( rPoint + rSize )
		{ if ( vNormalize ) NormalizeRect( ); }

	//! CGUIRect ���캯��, ͨ��2�� CGUIPoint ���� CGUIRect ����, 
	/*! \param rTopLeft const CGUIPoint ������ ���Ͻ�����
		\param rBottomRight const CGUIPoint ������ ���½�����
		\param vNormalize bool �� �ǲ���Ҫ�������� CGUIRect ���� 
		\sa CGUIRect */
	CGUIRect( const CGUIPoint& rTopLeft, const CGUIPoint& rBottomRight, bool vNormalize = true ) : mTopLeft( rTopLeft ), mBottomRight( rBottomRight )
		{ if ( vNormalize ) NormalizeRect( ); }
public:
	//! CGUIRect ���캯��
	~CGUIRect( ) {	}

public:
	//! ��Ա���� BottomLeft, 
	/*! \return ����һ��CGUIPoint �Ŀ��� */
	CGUIPoint BottomLeft( ) const
		{ return CGUIPoint( mTopLeft.mX, mBottomRight.mY );	}

	//! ��Ա���� TopRight, 
	/*! \return ����һ��CGUIPoint �Ŀ��� */
	CGUIPoint TopRight( ) const
		{ return CGUIPoint( mBottomRight.mX, mTopLeft.mY );	}

	//! ��Ա���� BottomRight, 
	/*! \return ����һ��CGUIPoint �Ŀ��� */
	CGUIPoint BottomRight( ) const
		{ return mBottomRight; }

	//! ��Ա���� TopLeft, 
	/*! \return ����һ��CGUIPoint �Ŀ��� */
	CGUIPoint TopLeft( ) const
		{ return mTopLeft; }

	//! ��Ա���� CenterPoint, 
	/*! \return ���ظ� CGUIRect ��������ĵ�, һ��CGUIPoint �Ŀ��� */
	CGUIPoint CenterPoint( ) const
		{ return CGUIPoint( ( mTopLeft.mX + mBottomRight.mX ) >> 1, ( mTopLeft.mY + mBottomRight.mY ) >> 1 ); }

	//! ��Ա���� EqualRect, �ж����������Ƿ����
	/*! \param rRect const CGUIRect ������
		\return ���ظ� CGUIRect �����rRect�Ƿ����, �����ȷ���true, ���򷵻�false */
	bool EqualRect( const CGUIRect& rRect )	const
		{ return mTopLeft == rRect.mTopLeft && mBottomRight == rRect.mBottomRight; }

	//! ��Ա���� DeflateRect, ��С�� CGUIRect ����, ��Сָ���������Ŀ���
	/*! \param vX int �� mTopLeft�ĺ��������Ӻ�mBottomRight�ĺ����������
		\param vY int �� mTopLeft�����������Ӻ�mBottomRight�������������
		\return û�з���ֵ */
	void DeflateRect( int vX, int vY )
		{ mTopLeft.mX += vX; mTopLeft.mY += vY; mBottomRight.mX -= vX; mBottomRight.mY -= vY; }

	//! ��Ա���� DeflateRect, ��С�� CGUIRect ����, ��Сָ���������Ŀ���
	/*! \param rSize const CGUISize �� rSize��mCXָ���� mTopLeft�ĺ��������Ӻ�mBottomRight�ĺ����������
									   rSize��mCYָ���� mTopLeft�����������Ӻ�mBottomRight�������������
		\return û�з���ֵ */
	void DeflateRect( const CGUISize& rSize )
	{ 
		mTopLeft.mX		+= rSize.mCX; mTopLeft.mY		+= rSize.mCY; 
		mBottomRight.mX	-= rSize.mCX; mBottomRight.mY	-= rSize.mCY; 
	}

	//! ��Ա���� DeflateRect, ��С�� CGUIRect ����, ��Сָ���������Ŀ���
	/*! \param rRect const CGUIRect �� rRect ָ����ÿһ���ߵ���С��
		\return û�з���ֵ */
	void DeflateRect( const CGUIRect& rRect )
	{
		mTopLeft.mX		+= rRect.mTopLeft.mX;		mTopLeft.mY		+= rRect.mTopLeft.mY;		
		mBottomRight.mX -= rRect.mBottomRight.mX;	mBottomRight.mY -= rRect.mBottomRight.mY; 
	}

	//! ��Ա���� DeflateRect, ��С�� CGUIRect ����, ��Сָ���������Ŀ���
	/*! \param vLeft int �� vLeft ָ���� ��� ��С��
		\param vTop int �� vTop ָ���� �ϱ� ��С��
		\param vRight int �� vRight ָ���� �ұ� ��С��
		\param vBottom int �� vBottom ָ���� �±� ��С��
		\return û�з���ֵ */
	void DeflateRect( int vLeft, int vTop, int vRight, int vBottom )
		{ mTopLeft.mX += vLeft; mTopLeft.mY += vTop; mBottomRight.mX -= vRight; mBottomRight.mY -= vBottom; }

	//! ��Ա���� InflateRect, ����� CGUIRect ����, ����ָ���������ķ�ɢ
	/*! \param vX int �� mTopLeft�ĺ�������ٺ�mBottomRight�ĺ�����������
		\param vY int �� mTopLeft����������ٺ�mBottomRight��������������
		\return û�з���ֵ */
	void InflateRect( int vX, int vY )
		{ mTopLeft.mX -= vX; mTopLeft.mY -= vY; mBottomRight.mX += vX; mBottomRight.mY += vY; }

	//! ��Ա���� InflateRect, ����� CGUIRect ����, ����ָ���������ķ�ɢ
	/*! \param rSize const CGUISize �� rSize��mCXָ���� mTopLeft�ĺ�������ٺ�mBottomRight�ĺ�����������
									   rSize��mCYָ���� mTopLeft����������ٺ�mBottomRight��������������
		\return û�з���ֵ */
	void InflateRect( const CGUISize& rSize )
	{ 
		mTopLeft.mX		-= rSize.mCX;	mTopLeft.mY		-= rSize.mCY;			
		mBottomRight.mX += rSize.mCX;	mBottomRight.mY	+= rSize.mCY; 
	}

	//! ��Ա���� InflateRect, ����� CGUIRect ����, ����ָ���������ķ�ɢ
	/*! \param rRect const CGUIRect �� rRect ָ����ÿһ���ߵ�������
		\return û�з���ֵ */
	void InflateRect( const CGUIRect& rRect )
	{
		mTopLeft.mX		-= rRect.mTopLeft.mX;		mTopLeft.mY		-= rRect.mTopLeft.mY;	
		mBottomRight.mX += rRect.mBottomRight.mX;	mBottomRight.mY += rRect.mBottomRight.mY; 
	}

	//! ��Ա���� InflateRect, ����� CGUIRect ����, ����ָ���������ķ�ɢ
	/*! \param vLeft int �� vLeft ָ���� ��� ������
		\param vTop int �� vTop ָ���� �ϱ� ������
		\param vRight int �� vRight ָ���� �ұ� ������
		\param vBottom int �� vBottom ָ���� �±� ������
		\return û�з���ֵ */
	void InflateRect( int vLeft, int vTop, int vRight, int vBottom )
		{ mTopLeft.mX -= vLeft; mTopLeft.mY -= vTop; mBottomRight.mX += vRight; mBottomRight.mY += vBottom; }

	//! ��Ա���� Height, ���ظ� CGUIRect ����ĸ߶�
	/*! \return ���ظ� CGUIRect ����ĸ߶� */
	int Height( ) const
		{ return mBottomRight.mY - mTopLeft.mY; }

	//! ��Ա���� Width, ���ظ� CGUIRect ����Ŀ��
	/*! \return ���ظ� CGUIRect ����Ŀ�� */
	int	Width( ) const	
		{ return mBottomRight.mX - mTopLeft.mX; }

	//! ��Ա���� IsRectEmpty, ���ظ� CGUIRect ����Ŀ�͸��ǲ���ȫ����0
	/*! \return ���ظ� CGUIRect �����Ƿ����� */
	bool IsRectEmpty( ) const
		{ return Width( ) <= 0 || Height( ) <= 0; }

	//! ��Ա���� IsRectNull, ���ظ� CGUIRect ������ĸ����Ƿ�ȫΪ0
	/*! \return ���ظ� CGUIRect �����Ƿ�Ϊ0 */
	bool IsRectNull( ) const
		{ return mTopLeft.mX == 0 && mTopLeft.mY == 0 && mBottomRight.mX == 0 && mBottomRight.mY == 0; }

	//! ��Ա���� MoveToX, ����� CGUIRect ����vX,�����겻��
	/*! \param vX int �� ָ������ĺ�����
		\return û�з���ֵ */
	void MoveToX( int vX )	
		{ mBottomRight.mX = vX + Width( ); mTopLeft.mX = vX; }

	//! ��Ա���� MoveToY, ����� CGUIRect ����vY,�����겻��
	/*! \param vY int �� ָ�������������
		\return û�з���ֵ */
	void MoveToY( int vY )	
		{ mBottomRight.mY = vY + Height( ); mTopLeft.mY = vY; }

	//! ��Ա���� MoveToXY, ����� CGUIRect ����vX, vY
	/*! \param vX int �� ָ������ĺ�����
		\param vY int �� ָ�������������
		\return û�з���ֵ */
	void MoveToXY( int vX, int vY )
	{
		mBottomRight.mX = vX + Width( );	mTopLeft.mX	= vX; 
		mBottomRight.mY = vY + Height( ); 	mTopLeft.mY	= vY;  
	}

	//! ��Ա���� MoveToXY, ����� CGUIRect ����rPoint
	/*! \param rPoint const CGUIPoint ������ ָ�����������
		\return û�з���ֵ */
	void MoveToXY( const CGUIPoint& rPoint )
	{
		mBottomRight.mX = rPoint.mX + Width( ); 	mTopLeft.mX = rPoint.mX; 
		mBottomRight.mY = rPoint.mY + Height( );	mTopLeft.mY = rPoint.mY; 
	}

	//! ��Ա���� OffsetRect, �ƶ��� CGUIRect ����
	/*! \param vX int �� ָ���������ƫ����
		\param vY int �� ָ���������ƫ����
		\return û�з���ֵ */
	void OffsetRect( int vX, int vY )					 
		{ *this += CGUIPoint( vX, vY ); }

	//! ��Ա���� OffsetRect, �ƶ��� CGUIRect ����
	/*! \param rPoint const CGUIPoint ������ ָ������ƫ����
		\return û�з���ֵ */
	void OffsetRect( const CGUIPoint& rPoint )
		{ *this += rPoint; }

	//! ��Ա���� OffsetRect, �ƶ��� CGUIRect ����
	/*! \param rSize const CGUISize ������ ָ������ƫ����
		\return û�з���ֵ */
	void OffsetRect( const CGUISize& rSize )				 
		{ *this += rSize; }	

	//! ��Ա���� SetRect, ���� CGUIRect ����
	/*! \param vLeft int �� CGUIRect��������ϽǺ�����
		\param vTop int �� CGUIRect��������Ͻ�������
		\param vRight int �� CGUIRect��������½Ǻ�����
		\param vBottom int �� CGUIRect��������½�������
		\return û�з���ֵ */
	void SetRect( int vLeft, int vTop, int vRight, int vBottom )
	{
		mTopLeft.mX		= vLeft;	mTopLeft.mY		= vTop; 
		mBottomRight.mX = vRight;	mBottomRight.mY = vBottom; 
	}

	//! ��Ա���� SetRect, ͨ��1�� CGUIPoint ��1�� CGUISize ���� CGUIRect ����, 
	/*! \param rPoint const CGUIPoint ������ ���Ͻ�����
		\param rSize const CGUISize ������ CGUIRect�ߴ�	
		\return û�з���ֵ */
	void SetRect( const CGUIPoint& rPoint, const CGUISize& rSize )
		{ mTopLeft = rPoint; mBottomRight = rPoint + rSize; }

	//! ��Ա���� SetRect, ͨ��2�� CGUIPoint ���� CGUIRect ����, 
	/*! \param rTopLeft const CGUIPoint ������ ���Ͻ�����
		\param rBottomRight const CGUIPoint ������ ���½�����
		\return û�з���ֵ */
	void SetRect( const CGUIPoint& rTopLeft, const CGUIPoint& rBottomRight )
		{ mTopLeft = rTopLeft; mBottomRight	= rBottomRight; }

	//! ��Ա���� SetRectEmpty, ���� CGUIRect ������0, 
	/*! \return û�з���ֵ */
	void SetRectEmpty( )
		{ mTopLeft.mX = 0; mTopLeft.mY = 0; mBottomRight.mX = 0; mBottomRight.mY = 0; }

	//! ��Ա���� Size, ���ظ� CGUIRect ����Ŀ�Ⱥ͸߶�
	/*! \return ���ظ� CGUIRect ����Ŀ�Ⱥ͸߶� */
	CGUISize Size( ) const
		{ return CGUISize( Width( ), Height( ) ); }

	//! ��Ա���� NormalizeRect, �������� CGUIRect ����
	/*! \return û�з���ֵ */
	void NormalizeRect( );

	//! ��Ա���� PtInRect, �ж�ָ�������Ƿ��ڸ� CGUIRect ������
    /*! \param rPoint const CGUIPoint������ ���жϵ�����
		\return ����ָ�������ǲ����ڸ� CGUIRect ������ */
	bool PtInRect( const CGUIPoint&	rPoint ) const;

	//! ��Ա���� IntersectRect, �������� CGUIRect ����Ľ���
	/*! \brief �����������ľ��β�����, ��ô�þ��ν�������
		\brief eg. CGUIRect( ).( CGUIRect( 0, 0, 10, 10 ), CGUIRect( 5, 5, 15, 15 ) ) == CGUIRect( 5, 5, 10, 10 )
		\param const CGUIRect& rSrcRect1 Դ����
		\param const CGUIRect& rSrcRect2 Դ����
		\return ������������Դ���λ��Ǽ��������ľ��β�������������false, ���ⷵ��true */
   	bool IntersectRect( const CGUIRect& rSrcRect1, const CGUIRect& rSrcRect2 );

	//! ��Ա���� UnionRect, �������� CGUIRect ����Ĳ���
	/*! \brief ������������Դ������һ��������, ����������false
		\brief eg. CGUIRect( ).( CGUIRect( 0, 0, 10, 10 ), CGUIRect( 5, 5, 15, 15 ) ) == CGUIRect( 0, 0, 15, 15 )
		\param const CGUIRect& rSrcRect1 Դ����
		\param const CGUIRect& rSrcRect2 Դ����
		\return ������������Դ������һ��������, ����������false, ���ⷵ��true */
	bool UnionRect( const CGUIRect& rSrcRectl, const CGUIRect& rSrcRect2 );
public:
	//! != ������, �ж����� CGUIRect �����Ƿ����
	/*! \param const CGUIRect& rRect 
		\return ���ظ� CGUIRect �����rRect�Ƿ����, �������ȷ���true, ���򷵻�false */
	bool operator != ( const CGUIRect& rRect ) const
		{ return !EqualRect( rRect ); }

	//! == ������, �ж����� CGUIRect �����Ƿ����
	/*! \param const CGUIRect& rRect 
		\return ���ظ� CGUIRect �����rRect�Ƿ����, �����ȷ���true, ���򷵻�false */
	bool operator == ( const CGUIRect& rRect ) const
		{ return EqualRect( rRect ); }

	//! &= ������, ����� CGUIRect ����� ָ�� CGUIRect ����Ľ���
	/*! \param const CGUIRect& rRect 
		\return û�з���ֵ */
	void operator &= ( const CGUIRect& rRect )
		{ IntersectRect( *this, rRect ); }

	//! |= ������, ����� CGUIRect ����� ָ�� CGUIRect ����Ĳ���
	/*! \param const CGUIRect& rRect 
		\return û�з���ֵ */
	void operator |= ( const CGUIRect& rRect )
		{ UnionRect( *this, rRect ); }

	//! & ������, ����� CGUIRect ����� ָ�� CGUIRect ����Ľ���
	/*! \param const CGUIRect& rRect 
		\return ����� CGUIRect ����� ָ�� CGUIRect ����Ĳ���, ���� CGUIRect �Ŀ���  */
	CGUIRect operator & ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult;
		tRectResult.IntersectRect( *this, rRect );
		return tRectResult;
	}

	//! | ������, ����� CGUIRect ����� ָ�� CGUIRect ����Ĳ���
	/*! \param const CGUIRect& rRect 
		\return ����� CGUIRect ����� ָ�� CGUIRect ����Ĳ���, ���� CGUIRect �Ŀ���  */
	CGUIRect operator | ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult;
		tRectResult.UnionRect( *this, rRect );
		return tRectResult;
	}

	//! - ������, �Ѹ� CGUIRect ����ƫ�� rPoint ����λ
	/*! \param const CGUIPoint& rPoint
		\return �Ѹ� CGUIRect ����ƫ�� rPoint ����λ, ���� CGUIRect �Ŀ���  */
	CGUIRect operator - ( const CGUIPoint& rPoint ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( -rPoint );
		return tRectResult;
	}

	//! - ������, �Ѹ� CGUIRect ����ƫ�� rSize ����λ
	/*! \param const CGUISize& rSize
		\return �Ѹ� CGUIRect ����ƫ�� rSize ����λ, ���� CGUIRect �Ŀ���  */
	CGUIRect operator - ( const CGUISize& rSize ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( -rSize );
		return tRectResult;
	}

	//! - ������, ��С�� CGUIRect ������С�ĳߴ��� rRect ָ��
	/*! \param const CGUIRect& rRect
		\return ��С�� CGUIRect ������С�ĳߴ��� rRect ָ��, ���� CGUIRect �Ŀ���  */
	CGUIRect operator - ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.DeflateRect( rRect );
		return tRectResult;
	}

	//! + ������, �Ѹ� CGUIRect ����ƫ�� rPoint ����λ
	/*! \param const CGUIPoint& rPoint
		\return �Ѹ� CGUIRect ����ƫ�� rPoint ����λ, ���� CGUIRect �Ŀ���  */
	CGUIRect operator + ( const CGUIPoint& rPoint ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( rPoint );
		return tRectResult;
	}

	//! + ������, �Ѹ� CGUIRect ����ƫ�� rSize ����λ
	/*! \param const CGUISize& rSize
		\return �Ѹ� CGUIRect ����ƫ�� rSize ����λ, ���� CGUIRect �Ŀ���  */
	CGUIRect operator + ( const CGUISize& rSize ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.OffsetRect( rSize );
		return tRectResult;
	}

	//! + ������, ����� CGUIRect ��������ĳߴ��� rRect ָ��
	/*! \param const CGUIRect& rRect
		\return ����� CGUIRect ��������ĳߴ��� rRect ָ��, ���� CGUIRect �Ŀ���  */
	CGUIRect operator + ( const CGUIRect& rRect ) const
	{
		CGUIRect tRectResult = *this;
		tRectResult.InflateRect( rRect );
		return tRectResult;
	}

	//! += ������, �Ѹ� CGUIRect ����ƫ�� rPoint ����λ
	/*! \param const CGUIPoint& rPoint
		\return û�з���ֵ  */
	void operator += ( const CGUIPoint& rPoint )
		{ mTopLeft += rPoint; mBottomRight += rPoint; }

	//! += ������, �Ѹ� CGUIRect ����ƫ�� rSize ����λ
	/*! \param const CGUISize& rSize
		\return û�з���ֵ  */
	void operator += ( const CGUISize& rSize )
		{ mTopLeft += rSize; mBottomRight += rSize; }

	//! += ������, ����� CGUIRect ��������ĳߴ��� rRect ָ��
	/*! \param const CGUIRect& rRect
		\return û�з���ֵ  */
	void operator += ( const CGUIRect& rRect )
		{ InflateRect( rRect );	}

	//! += ������, �Ѹ� CGUIRect ����ƫ�� rPoint ����λ
	/*! \param const CGUIPoint& rPoint
		\return û�з���ֵ  */
	void operator -= ( const CGUIPoint& rPoint )
		{ mTopLeft -= rPoint; mBottomRight -= rPoint; }

	//! += ������, �Ѹ� CGUIRect ����ƫ�� rSize ����λ
	/*! \param const CGUISize& rSize
		\return û�з���ֵ  */
	void operator -= ( const CGUISize& rSize )
		{ mTopLeft -= rSize; mBottomRight -= rSize;	}

	//! += ������, ��С�� CGUIRect ������С�ĳߴ��� rRect ָ��
	/*! \param const CGUIRect& rRect
		\return û�з���ֵ  */
	void operator -= ( const CGUIRect& rRect )
		{ DeflateRect( rRect );	}
};

// ************************************************************ //
// CFontTexture
// ************************************************************ //

class CGUIFontGlyph
{
	friend class CGUIFont;
protected:
	IDirect3DDevice9*		mpDevice;			// D3D�豸
	IDirect3DTexture9*		mpTexture;			// ��������
	CGUIRect				mGlyphRect;			// �����������λ��
	int						mAdvanceWidth;		// �����߼����
	bool					mIsCreated;			// �Ƿ������Ѿ�����
	wchar_t					mUnicode;			// UNICODE����
	CGUIFont*				mpFont;				// ��������

public:
	CGUIFontGlyph( );
public:
	virtual ~CGUIFontGlyph( );

protected:
	int CreateGlyph( );
	int FillFont( IDirect3DTexture9* pTexture, const CGUIRect& rRect );
	void SetContext( CGUIFont* tpFont, IDirect3DDevice9* pDevice, wchar_t tUnicode ) 
		{ mpFont = tpFont; mpDevice = pDevice; mUnicode = tUnicode; }

public:
	IDirect3DTexture9* GetGlyphTexture( );							// �õ���������pText�Ƕ��ֽڣ�ָ��һ���ַ�
	int GetGlyphWidth( );										// �õ��߼���ȣ�pText�Ƕ��ֽڣ�ָ��һ���ַ�
	CGUIRect GetGlyphRect( );									// �õ������������е�λ�ã�pText�Ƕ��ֽڣ�ָ��һ���ַ�
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
	virtual int GetGlyphIndex( const char* pText ) const = 0;		// pText��һ�����ֽ��ַ����Ƕ��ֽ����ģ� ��������
	virtual int GetCodeSpace( ) const = 0;							// �õ��ַ�����ռ�
	virtual wchar_t GetUnicode( int vGlphyIndex ) const = 0;		// �õ�������Ӧ��Unicode����
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
	virtual int GetGlyphIndex( const char* pText ) const;		// pText��'\0'�������ַ�����ֻ����һ���ַ�����һ�����ģ� ��������
	virtual int GetCodeSpace( ) const;							// �õ��ַ�����ռ�
	virtual wchar_t GetUnicode( int vGlphyIndex ) const;		// �õ�������Ӧ��Unicode����
};

// ************************************************************ //
// CGUIFont
// ************************************************************ //

class CGUIFont
{
	friend class CGUIFontGlyph;

protected:
	static int			sFTRef;							// freetype��������ü���
	static FT_Library	spFTLibrary;					// freetype�����

protected:
	std::vector< IDirect3DTexture9* >	mFontTex;			// �ַ�����
	FT_Face								mpFTFace;			// FreeType�������
	CGUICodePage*						mpCharMap;			// ����ҳ
	CGUIFontGlyph*						mpGlyph;			// �ַ���
	std::string							mFaceName;			// ��������
	int									mDpi;				// ������
	int									mEdge;				// �߿�
	int									mHOffset;			// �ߺ���ƫ��
	int									mVOffset;			// ������ƫ��
	int									mUnitWidth;			// ���嵥Ԫ���
	int									mUnitHeight;		// ���嵥Ԫ�߶�
	int									mFontUsed;			// ����	�˶��ٸ�����

public:
	CGUIFont( IDirect3DDevice9* pDevice, int vFontHeight, const char* pFace, const int vEdge = 0, const int vHOffset = 0, const int vVOffset = 0, 
		CGUICodePage* pCharMap = NULL );
public:
	~CGUIFont( );

public:
	CGUIFontGlyph* GetFontGlyph( const char* pText );						// �õ�һ���ַ�����
	int GetMaxTextWidth( );													// �õ�����ַ����
	int GetTextWidth( const char* pText );									// �õ�һ���ַ��ı����
	int GetFontHeight( );													// �õ�һ���ַ��߶�
	int GetTextWidth( const char* pText, int vTextCount );					// �õ�ָ���ı����
};

};
