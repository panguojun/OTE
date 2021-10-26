/***************************************************************************************************
文件名称:	DymMagicToOGRE.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效插件转换到OGRE
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"freetype.lib")
#include "OGRE.h"
#include "iSceneObj.h"
#include "ObjectFactory.h"
#include "clientScene.h"
#include <list>
#include "DymMagicInterface.h"

#include "OgreNoMemoryMacros.h"
#include "DymMagic.h"
#include "OgreMemoryMacros.h"

//---------------------------------------------------------------------------------------------
//
// 全局
//
//---------------------------------------------------------------------------------------------
void DymCreateMagicRenderSystem( void *pd3dDevice , SceneManager *pdymSceneManager );
void DymDestroyMagicRenderSystem();

//---------------------------------------------------------------------------------------------
//
// CDymMagicRenderSystemForOGRE
//
//---------------------------------------------------------------------------------------------
class CDymMagicRenderSystemForOGRE :public CDymRenderSystem
{
	D3DXMATRIX m_worldSaved;
public:
	virtual HRESULT CreateTextureFromFile( LPCTSTR pSrcFile,CDymD3DTexture ** ppTexture);
	virtual void *LoadFromFile( LPCTSTR fullpathfilename , int *pSizeOut );
	virtual HRESULT SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX * pMatrix );
	virtual HRESULT GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX * pMatrix);
};
//---------------------------------------------------------------------------------------------
//
// CDymMagicSoundSystemForOGRE
//
//---------------------------------------------------------------------------------------------
class CDymMagicSoundSystemForOGRE :public CDymSoundManager
{
public:
	virtual HRESULT CreateSoundFromFile( LPTSTR filename , CDymSoundObj *pObj );
};
//---------------------------------------------------------------------------------------------
//
// CDymMagicRenderQueueListener
//
//---------------------------------------------------------------------------------------------
class CDymMagicRenderQueueListener : public RenderQueueListener
{
public:
	virtual void renderQueueStarted(RenderQueueGroupID id, bool& skipThisQueue);
	virtual void renderQueueEnded(RenderQueueGroupID id, bool& repeatThisQueue);
};

//---------------------------------------------------------------------------------------------
//
// CDymMagicFrameListener
//
//---------------------------------------------------------------------------------------------
class CDymMagicFrameListener : public FrameListener
{
public:
	virtual bool frameStarted(const FrameEvent& evt);
};

//---------------------------------------------------------------------------------------------
//
// CDymMagicOGRE
//
//---------------------------------------------------------------------------------------------
class CDymMagicOGRE:public ISObject,public IDymMagicInterface
{
	CDymMagic *m_pMagic;
	float m_fCreateTime;
	float m_fCreateTimeFirst;
	float m_fAlpha;
public:
	int m_nRender;

	CDymMagicOGRE(const std::string& objName,const std::string& fileName);
	virtual ~CDymMagicOGRE();
	virtual void addRenderQueue ( RenderQueue *queue );
	virtual void notifyCameraLookYou(Camera* camera);
	virtual const sAABox& getLocalAABB();
	void Render( DWORD flag );
	void FrameMove( float fElapsedTime );
	bool isValid ();
	virtual void queryInterface( void** pPtr,const IGUID& rGuid );

	virtual void SetDefaultShadowPos( float directionX , float directionY , float directionXZ ,float centerX , float centerY , float centerZ );
	virtual void SetDefaultBillboardString( LPCTSTR str , DWORD color , void *pUnknow );
	virtual int GetMagicExData();
	virtual void SetFastRender( BOOL bFast , BOOL bAll );
	virtual void setAlpha(float alpha);
	virtual float getAlpha();
	virtual void SetAllKeyTexture( LPCTSTR texFileName );
	virtual void SetLoop( BOOL bLoop ) ;
	virtual BOOL GetLoop() ;
	virtual void CreateShadow( float followspeed , int facenum );
	virtual void SetMaxTime( float time , BOOL bReplay );
	virtual void SetPlayTime( float time );
};

//---------------------------------------------------------------------------------------------
//
// CDymMagicOGREManager
//
//---------------------------------------------------------------------------------------------
class CDymMagicOGREManager
{
	std::list< CDymMagicOGRE* > m_listMagic;
	Camera* m_pMagicCamera;
	float m_fElapsedTime;
	float m_fElapsedTime2;
	BOOL m_bFastRender;

	D3DXMATRIX makeD3DXMatrix( const Matrix4& mat );;
public:
	CDymMagicOGREManager();
	void SetCamera( Camera *pCamera );
	void AddMagic( CDymMagicOGRE *pMagic );
	void DelMagic( CDymMagicOGRE *pMagic );
	void FrameMove( float fElapsedTime );
	void BeginRender();
	float GetElapsedTime();
	void SetFastRender( BOOL bFast );
};

//---------------------------------------------------------------------------------------------
//
// CDymMagicFactoryOGRE
//
//---------------------------------------------------------------------------------------------
class CDymMagicFactoryOGRE:public IObjFactory
{
public:
	virtual ISObject*  createObject(const std::string& objName,const std::string& fileName);
	virtual ISObject*  createObject(const std::string& objName);
	virtual void destroyObject(ISObject* obj);
};


extern CDymMagicRenderSystemForOGRE *g_pMagicRenderSystem ;
extern CDymMagicOGREManager g_dymMagicOGREManager;