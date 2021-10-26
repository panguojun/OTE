#include "OTED3DManager.h"
#include "Ogre.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9RenderWindow.h"
#include "OgreD3D9Texture.h"


using namespace Ogre;
using namespace OTE;

//	----------------------------------------------
float				COTED3DManager::s_ElapsedTime = 0;
float				COTED3DManager::s_FrameTime = 0;
D3DXMATRIX			COTED3DManager::s_ViewMat;
D3DXMATRIX			COTED3DManager::s_PrjMat;
D3DXMATRIX			COTED3DManager::s_ViewPrjMatCach;
bool				COTED3DManager::s_NeedUpdate = true; 

//	----------------------------------------------
#ifdef __OTE
LPDIRECT3DDEVICE9	g_pDevice = NULL;	// ｄ３ｄ全局对象
#endif

//	----------------------------------------------
LPDIRECT3DDEVICE9 COTED3DManager::GetD3DDevice()
{
	return g_pDevice;
}
//	----------------------------------------------
void COTED3DManager::MAKED3DXMatrix(const Matrix4& mat, D3DXMATRIX& d3dMat)
{
	d3dMat.m[0][0] = mat[0][0];
	d3dMat.m[0][1] = mat[1][0];
	d3dMat.m[0][2] = mat[2][0];
	d3dMat.m[0][3] = mat[3][0];

	d3dMat.m[1][0] = mat[0][1];
	d3dMat.m[1][1] = mat[1][1];
	d3dMat.m[1][2] = mat[2][1];
	d3dMat.m[1][3] = mat[3][1];

	d3dMat.m[2][0] = mat[0][2];
	d3dMat.m[2][1] = mat[1][2];
	d3dMat.m[2][2] = mat[2][2];
	d3dMat.m[2][3] = mat[3][2];

	d3dMat.m[3][0] = mat[0][3];
	d3dMat.m[3][1] = mat[1][3];
	d3dMat.m[3][2] = mat[2][3];
	d3dMat.m[3][3] = mat[3][3];
}

//	----------------------------------------------
void COTED3DManager::_UpdateMatrices(const Camera* cam, bool updateComplete)
{
	if(s_NeedUpdate)
	{
		Matrix4 matrix4 = cam->getViewMatrix();
		matrix4[2][0] = -matrix4[2][0];	matrix4[2][1] = -matrix4[2][1];	matrix4[2][2] = -matrix4[2][2];	matrix4[2][3] = -matrix4[2][3];
		
		MAKED3DXMatrix(matrix4, s_ViewMat);	

		// 透视矩阵

		MAKED3DXMatrix(cam->getProjectionMatrix(), s_PrjMat);
		s_ViewPrjMatCach = s_ViewMat * s_PrjMat;
		
		if(updateComplete)
			s_NeedUpdate = false;
	}
}

//	----------------------------------------------
D3DXMATRIX* COTED3DManager::GetD3DViewPrjMatrix(const Camera* cam, bool manualUpdate)
{		
	manualUpdate ? s_NeedUpdate = true : NULL;
	_UpdateMatrices(cam);

	return &s_ViewPrjMatCach;
}

//	----------------------------------------------
D3DXMATRIX* COTED3DManager::GetD3DViewMatrix(const Camera* cam, bool manualUpdate)
{		
	manualUpdate ? s_NeedUpdate = true : NULL;
	_UpdateMatrices(cam);

	return &s_ViewMat;
}

//	----------------------------------------------
D3DXMATRIX* COTED3DManager::GetD3DProjectMatrix(const Camera* cam, bool manualUpdate)
{		
	manualUpdate ? s_NeedUpdate = true : NULL;
	_UpdateMatrices(cam);

	return &s_PrjMat;
}

//	----------------------------------------------
D3DXMATRIX* COTED3DManager::GetD3DWorldViewPrjMatrix(const Camera* cam, const Matrix4* worldMatrix, bool manualUpdate)
{		
	manualUpdate ? s_NeedUpdate = true : NULL;
	_UpdateMatrices(cam, false);

	static D3DXMATRIX s_WorldViewPrjMatCach;
	if(s_NeedUpdate)
	{
		MAKED3DXMatrix(*worldMatrix, s_WorldViewPrjMatCach);	
		s_WorldViewPrjMatCach = s_ViewPrjMatCach * s_WorldViewPrjMatCach;
		s_NeedUpdate = false;
	}

	return &s_WorldViewPrjMatCach;
}

//	----------------------------------------------
void COTED3DManager::_notifyUpdater()
{
	s_NeedUpdate = true;
}

//	----------------------------------------------
bool COTED3DManager::CheckDevCap(int major, int minor)
{
	D3DCAPS9 capDev;g_pDevice->GetDeviceCaps( &capDev );	
	return	( ( capDev.VertexShaderVersion & 0x0000FFFF ) >= ( D3DPS_VERSION( 1, 1 ) & 0x0000FFFF ) ||
			  ( capDev.PixelShaderVersion  & 0x0000FFFF ) >= ( D3DPS_VERSION( 1, 1 ) & 0x0000FFFF ) );
}

//	----------------------------------------------
bool COTED3DManager::IsDeviceLost()
{
#ifdef __ZH
	return RENDERSYSTEM->IsDeviceLost();
#endif
#ifdef __OTE
	return RENDERSYSTEM->isDeviceLost();
#endif

}