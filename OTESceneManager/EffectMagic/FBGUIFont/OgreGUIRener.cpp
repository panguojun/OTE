#include "OgreGUIRender.h"

// ************************************************************ //
// COgreTexture
// Ogre纹理实现
// ************************************************************ //

COgreTexture::COgreTexture( const char* pTexName, int vTexWidth, int vTexHeight )
{
	mpTexture = TextureManager::getSingleton( ).createManual( pTexName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		vTexWidth, vTexHeight, 5, PF_A8R8G8B8 );
	mpTexture->createInternalResources( );
}

COgreTexture::COgreTexture( const char* pTexName )
{
	mpTexture = TextureManager::getSingleton( ).load( pTexName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
}

COgreTexture::~COgreTexture( )
{
}

const std::string& COgreTexture::GetTexName( )
{
	return mpTexture->getName( );
}

int COgreTexture::GetTexWidth( )
{
	return mpTexture->getWidth( );
}

int COgreTexture::GetTexHeight( )
{
	return mpTexture->getHeight( );
}

unsigned int* COgreTexture::Lock( )
{
	HardwarePixelBufferSharedPtr tpPixelBuffer = mpTexture->getBuffer( );
	return (unsigned int*) tpPixelBuffer->lock( 0, tpPixelBuffer->getSizeInBytes( ), HardwareBuffer::HBL_NORMAL );
}

int COgreTexture::Unlock( )
{
	mpTexture->getBuffer( )->unlock( );
	return 0;
}

// ************************************************************ //
// COgreTextureFactory
// Ogre纹理工厂
// ************************************************************ //

COgreTextureFactory::COgreTextureFactory( )
{
}

COgreTextureFactory::~COgreTextureFactory( )
{
}

CGUITexture* COgreTextureFactory::LoadTexture( const char* pTexName )
{
	return new COgreTexture( pTexName );
}

CGUITexture* COgreTextureFactory::CreateTexture( const char* pTexName, int vWidth, int vHeight )
{
	return new COgreTexture( pTexName, vWidth, vHeight );
}

// ************************************************************ //
// CRenderUnit
// ************************************************************ //

CRenderUnit::CRenderUnit( )
{
}

CRenderUnit::~CRenderUnit( )
{
}

// ************************************************************ //
// COgreGUIRender
// Ogre渲染功能
// ************************************************************ //

COgreGUIRender::COgreGUIRender( RenderSystem* pRenderSystem, CGUIViewport* pViewport ) : mpRenderSystem( pRenderSystem ), mpViewport( pViewport )
{
	// 预先分配32个顶点
	mVertex.vertexCount = 0;
	mVertex.vertexStart = 0;
	VertexDeclaration*		tpDecl = mVertex.vertexDeclaration;
	VertexBufferBinding*	tpBind = mVertex.vertexBufferBinding;
	tpDecl->addElement( 0, 0, VET_FLOAT3, VES_POSITION );
	tpDecl->addElement( 1, 0, VET_COLOUR, VES_DIFFUSE );
	tpDecl->addElement( 2, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES );	

	mpPosBuffer		= HardwareBufferManager::getSingleton( ).createVertexBuffer( sizeof( float ) * 3, 8000, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true );
	mpClrBuffer		= HardwareBufferManager::getSingleton( ).createVertexBuffer( sizeof( int ), 8000, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true );
	mpTexcoorBuffer	= HardwareBufferManager::getSingleton( ).createVertexBuffer( sizeof( float ) * 2, 8000, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true );
 	
/*	mpPosBuffer		= HardwareBufferManager::getSingleton( ).createVertexBuffer( sizeof( float ) * 3, 8000, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true, true );
	mpClrBuffer		= HardwareBufferManager::getSingleton( ).createVertexBuffer( sizeof( int ), 8000, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true, true );
	mpTexcoorBuffer	= HardwareBufferManager::getSingleton( ).createVertexBuffer( sizeof( float ) * 2, 8000, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true, true );
 */  
	tpBind->setBinding( 0, mpPosBuffer );
	tpBind->setBinding( 1, mpClrBuffer );
	tpBind->setBinding( 2, mpTexcoorBuffer );
}

COgreGUIRender::~COgreGUIRender( )
{
}

void COgreGUIRender::SetRenderSystem( RenderSystem* pRenderSystem )
{
	mpRenderSystem = pRenderSystem;
}

RealRect COgreGUIRender::LPToUV( const CGUIRect& rRect, float vCx, float vCy )
{
	Real tLeft	 = ( rRect.mTopLeft.mX + 0.5f ) / vCx;
	Real tTop	 = ( rRect.mTopLeft.mY + 0.5f ) / vCy;
	Real tRight	 = ( rRect.mBottomRight.mX + 0.5f ) / vCx;
	Real tBottom = ( rRect.mBottomRight.mY + 0.5f ) / vCy;
	
	return RealRect( tLeft, tTop, tRight, tBottom );
}

// ************************************************************ //
// Rotation 
// ************************************************************ //

void COgreGUIRender::Rotate2D( const CGUIPoint& in, const CGUIPoint& center, float rAngle, CGUIPoint& out )
{
	double tRad = rAngle * Math::PI / 180.0 ;
	out.mX =  ( in.mX - center.mX ) * cos( tRad ) + ( in.mY - center.mY ) * sin( tRad ) + center.mX ;
	out.mY = -( in.mX - center.mX ) * sin( tRad ) + ( in.mY - center.mY ) * cos( tRad ) + center.mY ;
}

Vector2 COgreGUIRender::LPToDP( const CGUIPoint& rPoint )
{
	Real tPixelScaleX = 1.0 / mpViewport->mDestSize.mCX;
    Real tPixelScaleY = 1.0 / mpViewport->mDestSize.mCY;
	
	Real tX = Real( rPoint.mX ) * tPixelScaleX * 2;
	Real tY = Real( rPoint.mY ) * tPixelScaleY * 2;
	return Vector2( tX - 1, -( tY - 1 ) );
}

RealRect COgreGUIRender::LPToDP( const CGUIRect& tRect )
{
	Vector2 tTopLeft		= LPToDP( tRect.TopLeft( ) );
	Vector2 tBottomRight	= LPToDP( tRect.BottomRight( ) );

	return RealRect( tTopLeft.x, tTopLeft.y, tBottomRight.x, tBottomRight.y );
}

int COgreGUIRender::PrepareRender( )
{
	//mpRenderSystem->unbindGpuProgram(GPT_VERTEX_PROGRAM);
	//mpRenderSystem->unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
	//mpRenderSystem->_setDepthBias( 0 );
	//mpRenderSystem->_setAlphaRejectSettings( CMPF_ALWAYS_PASS, 0 );
	//mpRenderSystem->_setColourBufferWriteEnabled( true, true, true, true );
	//mpRenderSystem->_setCullingMode( CULL_NONE );
	//mpRenderSystem->setShadingType( SO_GOURAUD );
	//mpRenderSystem->_disableTextureUnitsFrom( 0 );
	//mpRenderSystem->_setTextureCoordSet( 0, 0 );
	//mpRenderSystem->_setTextureLayerAnisotropy( 0, 1 );
	//mpRenderSystem->_setTextureCoordCalculation( 0, TEXCALC_NONE );

	mpRenderSystem->setLightingEnabled( false );
	mpRenderSystem->_setDepthBufferFunction( );
	mpRenderSystem->_setDepthBufferCheckEnabled( false );
	mpRenderSystem->_setDepthBufferWriteEnabled( true );
	mpRenderSystem->_setTextureAddressingMode( 0, TextureUnitState::TextureAddressingMode(0) );
	mpRenderSystem->_setTextureMatrix( 0, Matrix4::IDENTITY );
	mpRenderSystem->_setTextureUnitFiltering( 0, FO_LINEAR, FO_LINEAR, FO_NONE );
	mpRenderSystem->_setWorldMatrix( Matrix4::IDENTITY );
	mpRenderSystem->_setViewMatrix( Matrix4::IDENTITY );
	mpRenderSystem->_setProjectionMatrix( Matrix4::IDENTITY );

	mpPosition	= (float*) mpPosBuffer->lock( HardwareBuffer::HBL_DISCARD );
	mpColour	= (unsigned int*) mpClrBuffer->lock( HardwareBuffer::HBL_DISCARD );
	mpTexcoor	= (float*) mpTexcoorBuffer->lock( HardwareBuffer::HBL_DISCARD );

	mUnitIndex	= 0;
	return 0;
}

int COgreGUIRender::AfterRender( )
{
	mpPosBuffer->unlock( );
	mpClrBuffer->unlock( );
	mpTexcoorBuffer->unlock( );

	RenderOperation ro;
	ro.vertexData	 = &mVertex;
	ro.useIndexes	 = false;
	CGUITexture* tpLastTexture = NULL;
	LayerBlendModeEx tLastColour;
	LayerBlendModeEx tLastAlpha;
	int tLastSrcFactor	 = 0;
	int tLastDestFactor	 = 0;
	int tStartVertex	 = 0;
	for ( int i = 0; i < mUnitIndex; i ++ )
	{
		if ( tpLastTexture != mRenderUnits[ i ].mpTexture )
		{
			if ( mRenderUnits[ i ].mpTexture != NULL )
                mpRenderSystem->_setTexture( 0, true, mRenderUnits[ i ].mpTexture->GetTexName( ) );
			tpLastTexture = mRenderUnits[ i ].mpTexture;
		}

		if ( tLastColour != mRenderUnits[ i ].mBlendColour )
		{
			mpRenderSystem->_setTextureBlendMode( 0, mRenderUnits[ i ].mBlendColour );
			tLastColour = mRenderUnits[ i ].mBlendColour;
		}

		if ( tLastAlpha != mRenderUnits[ i ].mBlendAlpha )
		{
            mpRenderSystem->_setTextureBlendMode( 0, mRenderUnits[ i ].mBlendAlpha );
			tLastAlpha = mRenderUnits[ i ].mBlendAlpha;
		}

		if ( tLastSrcFactor != mRenderUnits[ i ].mSrcFactor || tLastDestFactor != mRenderUnits[ i ].mDestFactor )
		{
			mpRenderSystem->_setSceneBlending( mRenderUnits[ i ].mSrcFactor, mRenderUnits[ i ].mDestFactor );
			tLastSrcFactor = mRenderUnits[ i ].mSrcFactor;
			tLastDestFactor = mRenderUnits[ i ].mDestFactor;
		}

		mVertex.vertexCount = mRenderUnits[ i ].mVertexCount;
		mVertex.vertexStart = tStartVertex;
		ro.operationType = Ogre::RenderOperation::OperationType( mRenderUnits[ i ].mOperationType );
		mpRenderSystem->_render( ro );

		tStartVertex += int( mVertex.vertexCount );
	}

	return 0;
}

int COgreGUIRender::FillTriangle( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, const CGUIPoint& rPoint3, unsigned int vColour )
{
	Vector2 tPoint1 = LPToDP( rPoint1 );
	Vector2 tPoint2 = LPToDP( rPoint2 );
	Vector2 tPoint3 = LPToDP( rPoint3 );

	PUSH_VERTEX2( tPoint1.x, tPoint1.y, 0, 0, vColour );
	PUSH_VERTEX2( tPoint2.x, tPoint2.y, 0, 0, vColour );
	PUSH_VERTEX2( tPoint3.x, tPoint3.y, 0, 0, vColour );
	
	// 颜色操作，使用顶点的颜色作为原色
	mRenderUnits[ mUnitIndex ].mBlendColour.blendType	= LBT_COLOUR;	
	mRenderUnits[ mUnitIndex ].mBlendColour.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendColour.operation	= LBX_SOURCE1;

	// Alpha操作，使用顶点的Alpha作为Alpha
	mRenderUnits[ mUnitIndex ].mBlendAlpha.blendType	= LBT_ALPHA;	
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.operation	= LBX_SOURCE1;

	// 混合操作
	mRenderUnits[ mUnitIndex ].mSrcFactor				= SBF_SOURCE_ALPHA; 
	mRenderUnits[ mUnitIndex ].mDestFactor				= SBF_ONE_MINUS_SOURCE_ALPHA;

	// 纹理
	mRenderUnits[ mUnitIndex ].mpTexture				= NULL;

	// 渲染类型
	mRenderUnits[ mUnitIndex ].mOperationType			= RenderOperation::OT_TRIANGLE_STRIP;
	mRenderUnits[ mUnitIndex ].mVertexCount				= 3;
	mUnitIndex ++;
	return 0;
}

int COgreGUIRender::DrawLine( const CGUIPoint& rPoint1, const CGUIPoint& rPoint2, unsigned int vColour )
{
	Vector2 tStart	= LPToDP( rPoint1 );
	Vector2 tEnd	= LPToDP( rPoint2 );

	PUSH_VERTEX2( tStart.x, tStart.y, 0, 0, vColour )
	PUSH_VERTEX2( tEnd.x, tEnd.y, 0, 0, vColour )
	
	// 颜色操作，使用顶点的颜色作为原色
	mRenderUnits[ mUnitIndex ].mBlendColour.blendType	= LBT_COLOUR;	
	mRenderUnits[ mUnitIndex ].mBlendColour.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendColour.operation	= LBX_SOURCE1;

	// Alpha操作，使用顶点的Alpha作为Alpha
	mRenderUnits[ mUnitIndex ].mBlendAlpha.blendType	= LBT_ALPHA;	
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.operation	= LBX_SOURCE1;

	// 混合操作
	mRenderUnits[ mUnitIndex ].mSrcFactor				= SBF_SOURCE_ALPHA; 
	mRenderUnits[ mUnitIndex ].mDestFactor				= SBF_ONE_MINUS_SOURCE_ALPHA;

	// 纹理
	mRenderUnits[ mUnitIndex ].mpTexture				= NULL;

	// 渲染类型
	mRenderUnits[ mUnitIndex ].mOperationType			= RenderOperation::OT_LINE_LIST;
	mRenderUnits[ mUnitIndex ].mVertexCount				= 2;
	mUnitIndex ++;

	return 0;
}

int COgreGUIRender::MaskBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColour )
{
	RealRect tDestRect = LPToDP( rDestRect );
	RealRect tSrcRect = LPToUV( rSrcRect, pTexture->GetTexWidth( ), pTexture->GetTexHeight( ) );

	PUSH_VERTEX2( tDestRect.mLeft, tDestRect.mTop, tSrcRect.mLeft, tSrcRect.mTop, vColour );
	PUSH_VERTEX2( tDestRect.mLeft, tDestRect.mBottom, tSrcRect.mLeft, tSrcRect.mBottom, vColour );
	PUSH_VERTEX2( tDestRect.mRight, tDestRect.mTop, tSrcRect.mRight, tSrcRect.mTop, vColour );
	PUSH_VERTEX2( tDestRect.mRight, tDestRect.mBottom, tSrcRect.mRight, tSrcRect.mBottom, vColour );
	
	// 颜色操作，使用顶点的颜色作为原色
	mRenderUnits[ mUnitIndex ].mBlendColour.blendType	= LBT_COLOUR;	
	mRenderUnits[ mUnitIndex ].mBlendColour.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendColour.source2		= LBS_TEXTURE;
	mRenderUnits[ mUnitIndex ].mBlendColour.operation	= LBX_MODULATE;

	// Alpha操作，使用顶点的Alpha作为Alpha
	mRenderUnits[ mUnitIndex ].mBlendAlpha.blendType	= LBT_ALPHA;	
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source2		= LBS_TEXTURE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.operation	= LBX_MODULATE;

	// 混合操作
	mRenderUnits[ mUnitIndex ].mSrcFactor				= SBF_SOURCE_ALPHA; 
	mRenderUnits[ mUnitIndex ].mDestFactor				= SBF_ONE_MINUS_SOURCE_ALPHA;

	// 纹理
	mRenderUnits[ mUnitIndex ].mpTexture				= pTexture;

	// 渲染类型
	mRenderUnits[ mUnitIndex ].mOperationType			= RenderOperation::OT_TRIANGLE_STRIP;
	mRenderUnits[ mUnitIndex ].mVertexCount				= 4;
	mUnitIndex ++;

	return 0;
}

int COgreGUIRender::BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, unsigned int vColour )
{
	RealRect tDestRect = LPToDP( rDestRect );
	RealRect tSrcRect = LPToUV( rSrcRect, pTexture->GetTexWidth( ), pTexture->GetTexHeight( ) );

	PUSH_VERTEX2( tDestRect.mLeft, tDestRect.mTop, tSrcRect.mLeft, tSrcRect.mTop, vColour );
	PUSH_VERTEX2( tDestRect.mLeft, tDestRect.mBottom, tSrcRect.mLeft, tSrcRect.mBottom, vColour );
	PUSH_VERTEX2( tDestRect.mRight, tDestRect.mTop, tSrcRect.mRight, tSrcRect.mTop, vColour );
	PUSH_VERTEX2( tDestRect.mRight, tDestRect.mBottom, tSrcRect.mRight, tSrcRect.mBottom, vColour );
	
	// 颜色操作，使用顶点的颜色作为原色
	mRenderUnits[ mUnitIndex ].mBlendColour.blendType	= LBT_COLOUR;	
	mRenderUnits[ mUnitIndex ].mBlendColour.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendColour.source2		= LBS_TEXTURE;
	mRenderUnits[ mUnitIndex ].mBlendColour.operation	= LBX_MODULATE;

	// Alpha操作，使用顶点的Alpha作为Alpha
	mRenderUnits[ mUnitIndex ].mBlendAlpha.blendType	= LBT_ALPHA;	
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source2		= LBS_TEXTURE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.operation	= LBX_MODULATE;

	// 混合操作
	mRenderUnits[ mUnitIndex ].mSrcFactor				= SBF_SOURCE_ALPHA; 
	mRenderUnits[ mUnitIndex ].mDestFactor				= SBF_ONE_MINUS_SOURCE_ALPHA;

	// 纹理
	mRenderUnits[ mUnitIndex ].mpTexture				= pTexture;

	// 渲染类型
	mRenderUnits[ mUnitIndex ].mOperationType			= RenderOperation::OT_TRIANGLE_STRIP;
	mRenderUnits[ mUnitIndex ].mVertexCount				= 4;
	mUnitIndex ++;
	return 0;
}

int COgreGUIRender::FillRect( const CGUIRect& rDestRect, unsigned int vColour )
{
	RealRect tDestRect	= LPToDP( rDestRect );

	PUSH_VERTEX2( tDestRect.mLeft, tDestRect.mTop, 0, 0, vColour );
	PUSH_VERTEX2( tDestRect.mLeft, tDestRect.mBottom, 0, 0, vColour );
	PUSH_VERTEX2( tDestRect.mRight, tDestRect.mTop, 0, 0, vColour );
	PUSH_VERTEX2( tDestRect.mRight, tDestRect.mBottom, 0, 0, vColour );

	// 颜色操作，使用顶点的颜色作为原色
	mRenderUnits[ mUnitIndex ].mBlendColour.blendType	= LBT_COLOUR;	
	mRenderUnits[ mUnitIndex ].mBlendColour.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendColour.operation	= LBX_SOURCE1;

	// Alpha操作，使用顶点的Alpha作为Alpha
	mRenderUnits[ mUnitIndex ].mBlendAlpha.blendType	= LBT_ALPHA;	
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.operation	= LBX_SOURCE1;

	// 混合操作
	mRenderUnits[ mUnitIndex ].mSrcFactor				= SBF_SOURCE_ALPHA; 
	mRenderUnits[ mUnitIndex ].mDestFactor				= SBF_ONE_MINUS_SOURCE_ALPHA;

	// 纹理
	mRenderUnits[ mUnitIndex ].mpTexture				= NULL;

	// 渲染类型
	mRenderUnits[ mUnitIndex ].mOperationType			= RenderOperation::OT_TRIANGLE_STRIP;
	mRenderUnits[ mUnitIndex ].mVertexCount				= 4;
	mUnitIndex ++;
	return 0;
}

int COgreGUIRender::DrawRect( const CGUIRect& rDestRect, unsigned int vTopLeftColour, unsigned int vBottomRightColour )
{
	const CGUIPoint& tTopLeft	= rDestRect.TopLeft( );
	CGUIPoint tTopRight			= rDestRect.TopRight( );
	CGUIPoint tBottomLeft		= rDestRect.BottomLeft( );
	CGUIPoint tBottomRight		= rDestRect.BottomRight( );

	tTopRight		-= CGUISize( 1, 0 );
	tBottomRight	-= CGUISize( 1, 1 ); 
	tBottomLeft		-= CGUISize( 0, 1 ); 

	DrawLine( tTopLeft, tTopRight, vTopLeftColour );
	DrawLine( tTopRight, tBottomRight, vBottomRightColour );
	DrawLine( tBottomRight, tBottomLeft, vBottomRightColour );
	DrawLine( tBottomLeft, tTopLeft, vTopLeftColour );
	return 0;
}

int COgreGUIRender::BitBlt( CGUITexture* pTexture, const CGUIRect& rSrcRect, const CGUIRect& rDestRect, float fRotAngle, const CGUIPoint& Point,  unsigned int vColour )
{
	// 先旋转

    CGUIPoint  tDecPoint11, tDecPoint21, tDecPoint22, tDecPoint12 ;    
	RealRect tDestRect( rDestRect.mTopLeft.mX, rDestRect.mTopLeft.mY, rDestRect.mBottomRight.mX, rDestRect.mBottomRight.mY );

	Rotate2D( CGUIPoint( tDestRect.mLeft ,tDestRect.mTop ), Point, fRotAngle, tDecPoint11 ); 
	Rotate2D( CGUIPoint( tDestRect.mLeft ,tDestRect.mBottom ), Point, fRotAngle, tDecPoint12 );
	Rotate2D( CGUIPoint( tDestRect.mRight,tDestRect.mTop ), Point, fRotAngle, tDecPoint21 );
	Rotate2D( CGUIPoint( tDestRect.mRight,tDestRect.mBottom ), Point, fRotAngle, tDecPoint22 ); 
	Vector2 tDecVec11 = LPToDP( tDecPoint11 );
	Vector2 tDecVec12 = LPToDP( tDecPoint12 );
	Vector2 tDecVec21 = LPToDP( tDecPoint21 );
	Vector2 tDecVec22 = LPToDP( tDecPoint22 ); 

	RealRect tSrcRect = LPToUV( rSrcRect, pTexture->GetTexWidth( ), pTexture->GetTexHeight( ) );

	PUSH_VERTEX2( tDecVec11.x, tDecVec11.y, tSrcRect.mLeft, tSrcRect.mTop, vColour );
	PUSH_VERTEX2( tDecVec12.x, tDecVec12.y, tSrcRect.mLeft, tSrcRect.mBottom, vColour );
	PUSH_VERTEX2( tDecVec21.x, tDecVec21.y, tSrcRect.mRight, tSrcRect.mTop, vColour );
	PUSH_VERTEX2( tDecVec22.x, tDecVec22.y, tSrcRect.mRight, tSrcRect.mBottom, vColour );
	
	// 颜色操作，使用顶点的颜色作为原色
	mRenderUnits[ mUnitIndex ].mBlendColour.blendType	= LBT_COLOUR;	
	mRenderUnits[ mUnitIndex ].mBlendColour.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendColour.source2		= LBS_TEXTURE;
	mRenderUnits[ mUnitIndex ].mBlendColour.operation	= LBX_MODULATE;

	// Alpha操作，使用顶点的Alpha作为Alpha
	mRenderUnits[ mUnitIndex ].mBlendAlpha.blendType	= LBT_ALPHA;	
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source1		= LBS_DIFFUSE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.source2		= LBS_TEXTURE;
	mRenderUnits[ mUnitIndex ].mBlendAlpha.operation	= LBX_MODULATE;

	// 混合操作
	mRenderUnits[ mUnitIndex ].mSrcFactor				= SBF_SOURCE_ALPHA; 
	mRenderUnits[ mUnitIndex ].mDestFactor				= SBF_ONE_MINUS_SOURCE_ALPHA;

	// 纹理
	mRenderUnits[ mUnitIndex ].mpTexture				= pTexture;

	// 渲染类型
	mRenderUnits[ mUnitIndex ].mOperationType			= RenderOperation::OT_TRIANGLE_STRIP;
	mRenderUnits[ mUnitIndex ].mVertexCount				= 4;
	mUnitIndex ++;
	return 0;
}

