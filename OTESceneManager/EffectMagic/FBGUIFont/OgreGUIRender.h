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
// Ogre �����ʽ
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
// Ogre ������
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
	LayerBlendModeEx mBlendColour;		// ��ɫ����
	LayerBlendModeEx mBlendAlpha;		// Alpha����
	SceneBlendFactor mSrcFactor;		// ԭ��ϲ���
	SceneBlendFactor mDestFactor;		// Ŀ���ϲ���
	CGUITexture*	 mpTexture;			// ����
	int				 mOperationType;	// ��������
	int				 mVertexCount;		// Vertex ��Ŀ

public:
	CRenderUnit( );
public:
	~CRenderUnit( );
};

// ************************************************************ //
// COgreGUIRender
// Ogre ��Ⱦ����
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
	// ׼����Ⱦ״̬
	virtual int PrepareRender( );
	// ��Ⱦ����
	virtual int AfterRender( );
	// ���������
	virtual int FillTriangle( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, const CGUIPoint& rPoint3, unsigned int vColour );
	// ��ֱ��
	virtual int DrawLine( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, unsigned int vColour );
	// ������
	virtual int DrawRect( const CGUIRect& rDestRect, unsigned int vTopLeftColoru, unsigned int vBottomRightColour );
	// ������
	virtual int FillRect( const CGUIRect& rDestRect, unsigned int vColor );
	// ��ͼ
	virtual int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColour );
	
	virtual int BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, float fRotAngle, const CGUIPoint& Point,  unsigned int vColour );

	// ������ͼ
	virtual int MaskBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColour );
	
};