#pragma once 
#include "FBGui.h"
#include "Ogre.h"
#include "OgreHardwarePixelBuffer.h"

using namespace Ogre;
#define PUSH_VERTEX2( x, y, u, v, colour )	{								\
												*mpPosition++	= x;		\
												*mpPosition++	= y;		\
												*mpPosition++	= -1.0f;	\
												*mpColour++		= colour;	\
												*mpTexcoor++	= u;		\
												*mpTexcoor++	= v;		\
											}								\

struct RealRect
{
	Real mLeft, mTop, mRight, mBottom;
public:
	RealRect( Real vLeft, Real vTop, Real vRight, Real vBottom )
	{
		mLeft	= vLeft;
		mTop	= vTop;
		mRight	= vRight;
		mBottom	= vBottom;
	}
};

// ************************************************************ //
// CGUITexture
// Ogre 纹理格式
// ************************************************************ //

class COgreTexture : public CGUITexture
{
	friend class COgreGUIRender;

public:
	TexturePtr		mpTexture;

public:
	COgreTexture( const char* pTexName );

public:
	COgreTexture( const char* pTexName, int vTexWidth, int vTexHeight );

public:
	virtual ~COgreTexture( );

public:
	virtual const std::string& GetTexName( );
	virtual int GetTexWidth( );
	virtual int GetTexHeight( );
	virtual unsigned int* Lock( );
	virtual int Unlock( );
};

// ************************************************************ //
// COgreTextureFactory
// Ogre 纹理工厂
// ************************************************************ //

class COgreTextureFactory : public CGUITextureFactory
{
public:
	COgreTextureFactory( );
public:
	virtual ~COgreTextureFactory( );

public:
	virtual CGUITexture* LoadTexture( const char* pTexName );
	virtual CGUITexture* CreateTexture( const char* pTexName, int vWidth, int vHeight );
};

// ************************************************************ //
// CRenderUnit
// ************************************************************ //

class CRenderUnit
{
public:
	LayerBlendModeEx mBlendColour;		// 颜色操作
	LayerBlendModeEx mBlendAlpha;		// Alpha操作
	SceneBlendFactor mSrcFactor;		// 原混合操作
	SceneBlendFactor mDestFactor;		// 目标混合操作
	CGUITexture*	 mpTexture;			// 纹理
	int				 mOperationType;	// 操作类型
	int				 mVertexCount;		// Vertex 数目

public:
	CRenderUnit( );
public:
	~CRenderUnit( );
};

// ************************************************************ //
// COgreGUIRender
// Ogre 渲染功能
// ************************************************************ //

class COgreGUIRender : public CGUIRender
{
public:
	int								mUnitIndex;
	CRenderUnit						mRenderUnits[ 5000 ];
	RenderSystem*					mpRenderSystem;
	CGUIViewport*					mpViewport;
	VertexData						mVertex;
	HardwareVertexBufferSharedPtr	mpPosBuffer;
	HardwareVertexBufferSharedPtr	mpClrBuffer;
	HardwareVertexBufferSharedPtr	mpTexcoorBuffer;
	unsigned int*					mpColour;
	float*							mpPosition;
	float*							mpTexcoor;

public:
	COgreGUIRender( RenderSystem* pSystem, CGUIViewport* pCamera );
public:
	virtual ~COgreGUIRender( );

public:
	void SetRenderSystem( RenderSystem* pRenderSystem );

protected:
	RealRect LPToUV( const CGUIRect& rRect, float vCx, float vCy );
	RealRect LPToDP( const CGUIRect& tRect );
	Vector2 LPToDP( const CGUIPoint& rPoint );
    
	void Rotate2D( const CGUIPoint& in, const CGUIPoint& center, float rAngle, CGUIPoint& out ) ;
protected:
	// 准备渲染状态
	virtual int PrepareRender( );
	// 渲染后处理
	virtual int AfterRender( );
	// 填充三角形
	virtual int FillTriangle( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, const CGUIPoint& rPoint3, unsigned int vColour );
	// 画直线
	virtual int DrawLine( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, unsigned int vColour );
	// 画矩形
	virtual int DrawRect( const CGUIRect& rDestRect, unsigned int vTopLeftColoru, unsigned int vBottomRightColour );
	// 填充矩形
	virtual int FillRect( const CGUIRect& rDestRect, unsigned int vColor );
	// 贴图
	virtual int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColour );
	
	virtual int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, float fRotAngle, const CGUIPoint& Point,  unsigned int vColour );

	// 高亮贴图
	virtual int MaskBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColour );
	
};