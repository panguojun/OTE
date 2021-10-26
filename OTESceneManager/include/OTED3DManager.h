#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 

namespace OTE
{
// ***********************************************
// COTED3DManager
// 处理直接用D3D渲染情况
// ***********************************************

class _OTEExport COTED3DManager
{
public:

	static float				s_ElapsedTime;
	static float				s_FrameTime;
	static D3DXMATRIX			s_ViewMat;
	static D3DXMATRIX			s_PrjMat;

private:

	static D3DXMATRIX			s_ViewPrjMatCach;
	static bool					s_NeedUpdate;  

public:

	static void _notifyUpdater();	

	static D3DXMATRIX* GetD3DViewPrjMatrix(const Ogre::Camera* cam, bool manualUpdate = false);

	static D3DXMATRIX* GetD3DViewMatrix(const Ogre::Camera* cam, bool manualUpdate = false);

	static D3DXMATRIX* GetD3DProjectMatrix(const Ogre::Camera* cam, bool manualUpdate = false);

	static D3DXMATRIX* GetD3DWorldViewPrjMatrix(const Ogre::Camera* cam, const Ogre::Matrix4* worldMatrix, bool manualUpdate = false);

	static void MAKED3DXMatrix(const Ogre::Matrix4& mat, D3DXMATRIX& d3dMat);

	static LPDIRECT3DDEVICE9 GetD3DDevice();

	static bool CheckDevCap(int major, int minor);

	static bool IsDeviceLost();

private:

	static void _UpdateMatrices(const Ogre::Camera* cam, bool updateComplete = true);

};

}

// d3d device
extern LPDIRECT3DDEVICE9    g_pDevice;

// ***********************************************
#define RENDERSYSTEM	GetD3D9RenderSystem()

inline Ogre::D3D9RenderSystem*	GetD3D9RenderSystem()
{
	static Ogre::D3D9RenderSystem* __renderSystem__ = ( (Ogre::D3D9RenderSystem*)(Ogre::Root::getSingleton().getRenderSystem( ) ) );
	//assert(__renderSystem__);
	return __renderSystem__;
}

#define IS_DEVICELOST	COTED3DManager::IsDeviceLost()