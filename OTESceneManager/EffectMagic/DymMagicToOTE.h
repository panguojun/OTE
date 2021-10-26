/***************************************************************************************************
文件名称:	DymMagicToOTE.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效插件转换到OTE
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
#include "OGRE.h"
using namespace Ogre;
#include <list>

#include "OgreNoMemoryMacros.h"
#include "DymMagic.h"
#include "OgreMemoryMacros.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#   ifdef PLUGIN_OTE_EXPORTS 
#       define _OgreMagicExport __declspec(dllexport) 
#   else 
#       define _OgreMagicExport __declspec(dllimport) 
#   endif 
#else 
#   define _OgreMagicExport 
#endif 
//---------------------------------------------------------------------------------------------
//
// CDymMagicRenderSystemForOTE
//
//---------------------------------------------------------------------------------------------
class _OgreMagicExport CDymMagicRenderSystemForOTE :public CDymRenderSystem
{
public:
	virtual HRESULT CreateTextureFromFile( LPCTSTR pSrcFile,CDymD3DTexture ** ppTexture);
	virtual void *LoadFromFile( LPCTSTR fullpathfilename , int *pSizeOut );
};
//---------------------------------------------------------------------------------------------
//
// CDymMagicSoundSystemForOGRE
//
//---------------------------------------------------------------------------------------------
class _OgreMagicExport CDymMagicSoundSystemForOTE :public CDymSoundManager
{
public:
	virtual HRESULT CreateSoundFromFile( LPTSTR filename , CDymSoundObj *pObj );
};
//---------------------------------------------------------------------------------------------
//
// CDymMagicToOTE_Obj
//
//---------------------------------------------------------------------------------------------

class _OgreMagicExport CDymMagicToOTE_Obj
{
	TCHAR *m_pStrID;
	D3DXVECTOR3 m_vPosOTE , m_vScaOTE , m_vAxisOTE;
	float m_fAngleOTE;
	
public:

	CDymMagic *m_pMagic;
	DWORD m_dwBoundingBoxColor;
	BOOL m_bRenderBoundingBox;

	CDymMagicToOTE_Obj( LPCTSTR strID , CDymMagicRenderSystemForOTE *pDevice , LPCTSTR strMagicFileName , CDymSoundManager *pSound );
	~CDymMagicToOTE_Obj();
	TCHAR *GetStrID();
	void SetPosition( float x , float y , float z );
	void SetScaling( float x , float y , float z );
	void SetRotation( float angle , float axisX , float axisY , float axisZ );
	void GetPosition( float *px , float *py , float *pz );
	void GetScaling( float *px , float *py , float *pz );
	void GetRotation( float *pAngle , float *pAxisX , float *pAxisY , float *pAxisZ );
	void GetBoundingBox( float *pX1 , float *pY1 , float *pZ1 , float *pX2 , float *pY2 , float *pZ2 );
	void Render( DWORD flag );
	void FrameMove( float fElapsedTime );
	void SetRenderBoundingBox( BOOL bRender , DWORD color = 0xFFFFFFFF );
	float GetSoundArea();

public:
	// by romeo

	bool m_bVisible; // 可见性
};

//---------------------------------------------------------------------------------------------
//
// CDymMagicToOTE
//
//---------------------------------------------------------------------------------------------
class _OgreMagicExport CDymMagicToOTE
{
	std::list< CDymMagicToOTE_Obj* > m_listObj;
	CDymMagicToOTE_Obj *_FindObj( LPCTSTR strID );
public:
	CDymMagicToOTE(void);
	~CDymMagicToOTE(void);
	CDymMagicRenderSystemForOTE m_renderSystem;
	CDymMagicSoundSystemForOTE m_soundSystem;

	void init( LPDIRECT3DDEVICE9 pDevice );
	void CreateMagic( LPCTSTR strMagicFileName , LPCTSTR strID );
	void DeleteMagic( LPCTSTR strID );
	bool FindMagic( LPCTSTR strName);
	void SetPosition( LPCTSTR strID , float x , float y , float z );
	void SetScaling( LPCTSTR strID , float x , float y , float z );
	void SetRotation( LPCTSTR strID , float angle , float axisX , float axisY , float axisZ );
	void GetPosition( LPCTSTR strID ,float *px , float *py , float *pz );
	void GetScaling( LPCTSTR strID ,float *px , float *py , float *pz );
	void GetRotation( LPCTSTR strID ,float *pAngle , float *pAxisX , float *pAxisY , float *pAxisZ );
	void SetRenderBoundingBox( LPCTSTR strID ,BOOL bRender , DWORD color = 0xFFFFFFFF );
	void GetBoundingBox( LPCTSTR strID , float *pX1 , float *pY1 , float *pZ1 , float *pX2 , float *pY2 , float *pZ2 );
	void FrameMove( float fElapsedTime );
	void Render( Camera *pCamera );
	int GetMagicNum();
	LPCTSTR GetMagicID( int num );
	float GetSoundArea( LPCTSTR strID );

	// by Romeo
	CDymMagicToOTE_Obj* GetMagicObj(LPCTSTR strID);
};

extern _OgreMagicExport CDymMagicToOTE g_dymMagicToOTE;
