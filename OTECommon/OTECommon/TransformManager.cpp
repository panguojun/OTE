#include "TransformManager.h"
#include "D3DToolbox.h"
#include "OTEQTSceneManager.h"
#include "OTED3DManager.h"

using namespace Ogre;
using namespace OTE;

// -----------------------------------------
CTransformManager::CTransformManager(void): m_EditType(ePOSITION)
{
	m_CoordLockString = "X";
	m_FocusedCoordString = "";	
}

CTransformManager::~CTransformManager(void)
{
}

// -----------------------------------------
// 帮助物体的绘制

void CTransformManager::DrawDummies()
{
	if(!IsVisible())
		return;

	Ogre::Camera* cam = SCENE_CAM;

	// The d3d device

	LPDIRECT3DDEVICE9 d3dDevice = COTED3DManager::GetD3DDevice();

	LPDIRECT3DVERTEXSHADER9 pVS;
	LPDIRECT3DPIXELSHADER9 pPS;
	d3dDevice->GetVertexShader(&pVS);	
	d3dDevice->GetPixelShader(&pPS);

	d3dDevice->SetVertexShader(NULL);
	d3dDevice->SetPixelShader(NULL);
	RENDERSYSTEM->_disableTextureUnitsFrom(0);
	// ----------------------------------------------------
	// 保存现场

	// 矩阵
	
	D3DXMATRIX matrix, oldWorldMat, oldViewMat, oldPrjMat;
	d3dDevice->GetTransform(D3DTS_WORLD,		&oldWorldMat); // SAVE TRANSFORM
	d3dDevice->GetTransform(D3DTS_VIEW,			&oldViewMat); 
	d3dDevice->GetTransform(D3DTS_PROJECTION ,	&oldPrjMat); 
	
	// 设置世界矩阵到默认

	D3DXMatrixIdentity(&matrix);
	d3dDevice->SetTransform(D3DTS_WORLD,		&matrix);

	// 灯光

	DWORD oldLighting;
	d3dDevice->GetRenderState(D3DRS_LIGHTING, &oldLighting);
	DWORD oldAmbient;
	d3dDevice->GetRenderState(D3DRS_AMBIENT, &oldAmbient);	
	DWORD oldColor;
	d3dDevice->GetRenderState(D3DRS_COLORVERTEX, &oldColor);
	DWORD oldZB;	
	d3dDevice->GetRenderState( D3DRS_ZENABLE, &oldZB );

	// ----------------------------------------------------
	// 分类别绘制

	DWORD CXColor = 0xFF800000, CYColor = 0xFF008000, CZColor = 0xFF000080, FoucsedColor = 0xFFFFFF00;

	if(m_EditType == ePOSITION && m_TransCoordDummy.visible)		// 位置
	{	

		// 缩放以保证随摄像机位置不同能保持大小不变

		float dis = (cam->getPosition() - m_TransCoordDummy.center).length();			// 观察点离摄像机的距离
		float nd =	cam->getNearClipDistance();					// 摄像机近视面的距离
		float factor = 1.0f;

		// 缩放比例	
		if(dis - nd > 0.001)	 
			factor = dis / nd * 0.02f;	

		m_TransCoordDummy.px = m_TransCoordDummy.center + Ogre::Vector3(1, 0, 0) * factor;
		m_TransCoordDummy.py = m_TransCoordDummy.center + Ogre::Vector3(0, 1, 0) * factor;
		m_TransCoordDummy.pz = m_TransCoordDummy.center + Ogre::Vector3(0, 0, 1) * factor;	

		// ----------------------------------------------------
		// 设置灯光

		d3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		d3dDevice->SetRenderState( D3DRS_AMBIENT, FALSE );
		d3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );

		// ZBuffer
		d3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		// ----------------------------------------------------
		// DRAW

		if(m_CoordLockString.find("X", 0) != std::string::npos)	
			CXColor = FoucsedColor;

		if(m_CoordLockString.find("Y", 0) != std::string::npos)	
			CYColor = FoucsedColor;

		if(m_CoordLockString.find("Z", 0) != std::string::npos)	
			CZColor = FoucsedColor;
		

		if(m_FocusedCoordString.find("X", 0) != std::string::npos)	
			CXColor |= 0xFF000000;

		if(m_FocusedCoordString.find("Y", 0) != std::string::npos)	
			CYColor |= 0xFF000000;

		if(m_FocusedCoordString.find("Z", 0) != std::string::npos)	
			CZColor |= 0xFF000000;


		DrawLine(d3dDevice, D3DXVECTOR3(m_TransCoordDummy.center.x, m_TransCoordDummy.center.y, m_TransCoordDummy.center.z), D3DXVECTOR3(m_TransCoordDummy.px.x, m_TransCoordDummy.px.y, m_TransCoordDummy.px.z), CXColor);
		DrawLine(d3dDevice, D3DXVECTOR3(m_TransCoordDummy.center.x, m_TransCoordDummy.center.y, m_TransCoordDummy.center.z), D3DXVECTOR3(m_TransCoordDummy.py.x, m_TransCoordDummy.py.y, m_TransCoordDummy.py.z), CYColor);
		DrawLine(d3dDevice, D3DXVECTOR3(m_TransCoordDummy.center.x, m_TransCoordDummy.center.y, m_TransCoordDummy.center.z), D3DXVECTOR3(m_TransCoordDummy.pz.x, m_TransCoordDummy.pz.y, m_TransCoordDummy.pz.z), CZColor);		
	
	}
	if(m_EditType == eROTATION && m_RotateCoordDummy.visible) // 旋转
	{
		// 缩放以保证随摄像机位置不同能保持大小不变

		float dis = (cam->getPosition() - m_RotateCoordDummy.center).length();			// 观察点离摄像机的距离
		float nd =	cam->getNearClipDistance();					// 摄像机近视面的距离
		float factor = 1.0f;

		// 缩放比例	
		if(dis - nd > 0.001)	 
			factor = dis / nd * 0.02f;	

		m_RotateCoordDummy.px = m_RotateCoordDummy.center + Ogre::Vector3(1, 0, 0) * factor;
		m_RotateCoordDummy.py = m_RotateCoordDummy.center + Ogre::Vector3(0, 1, 0) * factor;
		m_RotateCoordDummy.pz = m_RotateCoordDummy.center + Ogre::Vector3(0, 0, 1) * factor;	

		// ZBuffer
		d3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		// ----------------------------------------------------
		// 设置灯光

		d3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		d3dDevice->SetRenderState( D3DRS_AMBIENT, FALSE );
		d3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		// ----------------------------------------------------
		// DRAW

		if(m_CoordLockString.find("X", 0) != std::string::npos)	
			CXColor = FoucsedColor;

		if(m_CoordLockString.find("Y", 0) != std::string::npos)	
			CYColor = FoucsedColor;

		if(m_CoordLockString.find("Z", 0) != std::string::npos)	
			CZColor = FoucsedColor;

		if(m_FocusedCoordString.find("X", 0) != std::string::npos)	
			CXColor |= 0xFF000000;

		if(m_FocusedCoordString.find("Y", 0) != std::string::npos)	
			CYColor |= 0xFF000000;

		if(m_FocusedCoordString.find("Z", 0) != std::string::npos)	
			CZColor |= 0xFF000000;


		DrawLine(d3dDevice, D3DXVECTOR3(m_RotateCoordDummy.center.x, m_RotateCoordDummy.center.y, m_RotateCoordDummy.center.z), D3DXVECTOR3(m_RotateCoordDummy.px.x, m_RotateCoordDummy.px.y, m_RotateCoordDummy.px.z), CXColor);
		DrawLine(d3dDevice, D3DXVECTOR3(m_RotateCoordDummy.center.x, m_RotateCoordDummy.center.y, m_RotateCoordDummy.center.z), D3DXVECTOR3(m_RotateCoordDummy.py.x, m_RotateCoordDummy.py.y, m_RotateCoordDummy.py.z), CYColor);
		DrawLine(d3dDevice, D3DXVECTOR3(m_RotateCoordDummy.center.x, m_RotateCoordDummy.center.y, m_RotateCoordDummy.center.z), D3DXVECTOR3(m_RotateCoordDummy.pz.x, m_RotateCoordDummy.pz.y, m_RotateCoordDummy.pz.z), CZColor);		
		
	}
	if(m_EditType == eSCALING && m_ScaleCoordDummy.visible) // 缩放
	{
		// 缩放以保证随摄像机位置不同能保持大小不变

		float dis = (cam->getPosition() - m_ScaleCoordDummy.center).length();			// 观察点离摄像机的距离
		float nd =	cam->getNearClipDistance();											// 摄像机近视面的距离
		float factor = 1.0f;

		// 缩放比例	
		if(dis - nd > 0.001)	 
			factor = dis / nd * 0.02f;	

		m_ScaleCoordDummy.px = m_ScaleCoordDummy.center + Ogre::Vector3(1, 0, 0) * factor;
		m_ScaleCoordDummy.py = m_ScaleCoordDummy.center + Ogre::Vector3(0, 1, 0) * factor;
		m_ScaleCoordDummy.pz = m_ScaleCoordDummy.center + Ogre::Vector3(0, 0, 1) * factor;	

		// ----------------------------------------------------
		// 设置灯光

		d3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		d3dDevice->SetRenderState( D3DRS_AMBIENT, FALSE );
		d3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );

		// ZBuffer
		d3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		// ----------------------------------------------------
		// DRAW

		if(m_CoordLockString.find("X", 0) != std::string::npos)	
			CXColor = FoucsedColor;

		if(m_CoordLockString.find("Y", 0) != std::string::npos)	
			CYColor = FoucsedColor;

		if(m_CoordLockString.find("Z", 0) != std::string::npos)	
			CZColor = FoucsedColor;
	
		if(m_FocusedCoordString.find("X", 0) != std::string::npos)	
			CXColor |= 0xFF000000;

		if(m_FocusedCoordString.find("Y", 0) != std::string::npos)	
			CYColor |= 0xFF000000;

		if(m_FocusedCoordString.find("Z", 0) != std::string::npos)	
			CZColor |= 0xFF000000;

		DrawLine(d3dDevice, D3DXVECTOR3(m_ScaleCoordDummy.center.x, m_ScaleCoordDummy.center.y, m_ScaleCoordDummy.center.z), D3DXVECTOR3(m_ScaleCoordDummy.px.x, m_ScaleCoordDummy.px.y, m_ScaleCoordDummy.px.z), CXColor);
		DrawLine(d3dDevice, D3DXVECTOR3(m_ScaleCoordDummy.center.x, m_ScaleCoordDummy.center.y, m_ScaleCoordDummy.center.z), D3DXVECTOR3(m_ScaleCoordDummy.py.x, m_ScaleCoordDummy.py.y, m_ScaleCoordDummy.py.z), CYColor);
		DrawLine(d3dDevice, D3DXVECTOR3(m_ScaleCoordDummy.center.x, m_ScaleCoordDummy.center.y, m_ScaleCoordDummy.center.z), D3DXVECTOR3(m_ScaleCoordDummy.pz.x, m_ScaleCoordDummy.pz.y, m_ScaleCoordDummy.pz.z), CZColor);		

	}

	// ----------------------------------------------------
	// 还原现场
		
	d3dDevice->SetRenderState( D3DRS_ZENABLE,		oldZB );
	d3dDevice->SetRenderState( D3DRS_COLORVERTEX,	oldColor);	
	d3dDevice->SetRenderState( D3DRS_AMBIENT,		oldAmbient );
	d3dDevice->SetRenderState( D3DRS_LIGHTING,		oldLighting );	
	
	d3dDevice->SetTransform( D3DTS_WORLD ,			&oldWorldMat); // SET SAVED TRANSFORM
	d3dDevice->SetTransform( D3DTS_VIEW ,			&oldViewMat); 
	d3dDevice->SetTransform( D3DTS_PROJECTION ,		&oldPrjMat); 

	d3dDevice->SetVertexShader(pVS);	
	d3dDevice->SetPixelShader(pPS);
}

// -----------------------------------------
// 点选测试

const std::string& CTransformManager::PickDummies(float mouseX, float mouseY)
{	
	static std::string tCoordLockString;
	tCoordLockString = "";

	if(!IsVisible())
		return tCoordLockString;		

	float mousePrjX = (mouseX - 0.5) * 2.0;
	float mousePrjY = (0.5 - mouseY) * 2.0;	

	if(m_EditType == ePOSITION /*&& m_TransCoordDummy.visible*/) // 位置
	{
		if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_TransCoordDummy.center, m_TransCoordDummy.px, mousePrjX, mousePrjY))
			tCoordLockString= "X";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_TransCoordDummy.center, m_TransCoordDummy.py, mousePrjX, mousePrjY))
			tCoordLockString = "Y";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_TransCoordDummy.center, m_TransCoordDummy.pz, mousePrjX, mousePrjY))
			tCoordLockString = "Z";
	}
	else if(m_EditType == eROTATION /*&& m_RotateCoordDummy.visible*/) // 旋转
	{
		if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_RotateCoordDummy.center, m_RotateCoordDummy.px, mousePrjX, mousePrjY))
			tCoordLockString = "X";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_RotateCoordDummy.center, m_RotateCoordDummy.py, mousePrjX, mousePrjY))
			tCoordLockString = "Y";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_RotateCoordDummy.center, m_RotateCoordDummy.pz, mousePrjX, mousePrjY))
			tCoordLockString = "Z";
	}
	else if(m_EditType == eSCALING /*&& m_ScaleCoordDummy.visible*/) // 缩放
	{
		if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_ScaleCoordDummy.center, m_ScaleCoordDummy.px, mousePrjX, mousePrjY))
			tCoordLockString = "X";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_ScaleCoordDummy.center, m_ScaleCoordDummy.py, mousePrjX, mousePrjY))
			tCoordLockString = "Y";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_ScaleCoordDummy.center, m_ScaleCoordDummy.pz, mousePrjX, mousePrjY))
			tCoordLockString = "Z";
	}
	if(!tCoordLockString.empty())
		m_CoordLockString = tCoordLockString;

	return tCoordLockString;
}

// -----------------------------------------
// 悬浮测试

const std::string& CTransformManager::HoverDummies(float mouseX, float mouseY)
{	
	m_FocusedCoordString = "";

	if(!IsVisible())
		return m_FocusedCoordString;

	float mousePrjX = (mouseX - 0.5) * 2.0;
	float mousePrjY = (0.5 - mouseY) * 2.0;	
	
	if(m_EditType == ePOSITION /*&& m_TransCoordDummy.visible*/) // 位置
	{
		if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_TransCoordDummy.center, m_TransCoordDummy.px, mousePrjX, mousePrjY))
			m_FocusedCoordString = "X";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_TransCoordDummy.center, m_TransCoordDummy.py, mousePrjX, mousePrjY))
			m_FocusedCoordString = "Y";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_TransCoordDummy.center, m_TransCoordDummy.pz, mousePrjX, mousePrjY))
			m_FocusedCoordString = "Z";
	}
	else if(m_EditType == eROTATION /*&& m_RotateCoordDummy.visible*/) // 旋转
	{
		if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_RotateCoordDummy.center, m_RotateCoordDummy.px, mousePrjX, mousePrjY))
			m_FocusedCoordString = "X";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_RotateCoordDummy.center, m_RotateCoordDummy.py, mousePrjX, mousePrjY))
			m_FocusedCoordString = "Y";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_RotateCoordDummy.center, m_RotateCoordDummy.pz, mousePrjX, mousePrjY))
			m_FocusedCoordString = "Z";
	}
	else if(m_EditType == eSCALING /*&& m_ScaleCoordDummy.visible*/) // 缩放
	{
		if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_ScaleCoordDummy.center, m_ScaleCoordDummy.px, mousePrjX, mousePrjY))
			m_FocusedCoordString = "X";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_ScaleCoordDummy.center, m_ScaleCoordDummy.py, mousePrjX, mousePrjY))
			m_FocusedCoordString = "Y";
		else if(D3DLineHitTest(SCENE_CAM->getStandardProjectionMatrix() * SCENE_CAM->getViewMatrix(), m_ScaleCoordDummy.center, m_ScaleCoordDummy.pz, mousePrjX, mousePrjY))
			m_FocusedCoordString = "Z";
	}

	return m_FocusedCoordString;
}

// -----------------------------------------
// 这里没有使用碰撞测试的方法
void CTransformManager::GetViewportMove(const Ogre::Vector3& rPos,
									const Ogre::Vector3& oldPos, const Ogre::Vector2& rScreenMove, Ogre::Vector3& newPos)
{		
	Ogre::Vector3 move3D  = Ogre::Vector3::ZERO;

	float camDis = (SCENE_CAM -> getViewMatrix() * rPos).z;

	float clipNear = SCENE_CAM->getNearClipDistance();	

	Ogre::Vector2 screenMove;
	screenMove.x =    rScreenMove.x * Ogre::Math::Abs(camDis) / clipNear * 0.0001;	//screen coordnate
	screenMove.y =  - rScreenMove.y * Ogre::Math::Abs(camDis) / clipNear * 0.0001;	//screen coordnate
	
	// 摄像机坐标轴
	Ogre::Vector3 camX = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_X;
	Ogre::Vector3 camY = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_Y;

	// 世界坐标轴 屏幕投影的到2维向量	
	Ogre::Vector2 cX = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_X), camY.dotProduct(Ogre::Vector3::UNIT_X));
	Ogre::Vector2 cY = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Y), camY.dotProduct(Ogre::Vector3::UNIT_Y));
	Ogre::Vector2 cZ = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Z), camY.dotProduct(Ogre::Vector3::UNIT_Z));
	float lcx = cX.length(); lcx < 0.0001f ? lcx = 0.0001f : NULL; cX.normalise();
	float lcy = cY.length(); lcy < 0.0001f ? lcy = 0.0001f : NULL; cY.normalise();
	float lcz = cZ.length(); lcz < 0.0001f ? lcz = 0.0001f : NULL; cZ.normalise();
	move3D = Ogre::Vector3(screenMove.dotProduct(cX) / lcx, screenMove.dotProduct(cY) / lcy, screenMove.dotProduct(cZ) / lcz);
		
	Ogre::Vector3 needMove = Ogre::Vector3::ZERO;
	if(m_CoordLockString.find("X", 0) != std::string::npos)	
		needMove.x += move3D.x;
	if(m_CoordLockString.find("Y", 0) != std::string::npos)	
		needMove.y += move3D.y;	
	if(m_CoordLockString.find("Z", 0) != std::string::npos)	
		needMove.z += move3D.z;	
	if( m_CoordLockString.find("XZ", 0) != std::string::npos)	
	{
		needMove.x += move3D.x;
		needMove.z += move3D.z;	
	}
	
	// setposition
	if(needMove.length() < 100.0f) // 防止过大
		newPos = oldPos +  needMove;	
}

// ----------------------------------------------
// 通过屏幕移动得到3D旋转
void CTransformManager::GetViewportRotate(const Ogre::Vector3& rPos, 
										  const Ogre::Vector2& rScreenMove, 
										  Ogre::Quaternion& rOldRot,   
										  Ogre::Quaternion& rNewRot,
										  Ogre::Vector3* rAngles )
{
	float camDis = (SCENE_CAM -> getViewMatrix() * rPos).z;

	float clipNear = SCENE_CAM->getNearClipDistance();	

	Ogre::Vector2 screenMove;
	screenMove.x =    rScreenMove.x * Ogre::Math::Abs(camDis) / clipNear * 0.001;	//screen coordnate
	screenMove.y =  - rScreenMove.y * Ogre::Math::Abs(camDis) / clipNear * 0.001;	//screen coordnate

	// 摄像机坐标轴
	Ogre::Vector3 camX = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_X;
	Ogre::Vector3 camY = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_Y;

	
	// 世界坐标轴 屏幕投影的到2维向量	
	Ogre::Vector2 cX = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_X), camY.dotProduct(Ogre::Vector3::UNIT_X));
	Ogre::Vector2 cY = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Y), camY.dotProduct(Ogre::Vector3::UNIT_Y));
	Ogre::Vector2 cZ = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Z), camY.dotProduct(Ogre::Vector3::UNIT_Z));
	
	Ogre::Vector3 move3D = Ogre::Vector3(
		screenMove.x * cX.y - screenMove.y * cX.x, 
		screenMove.x * cY.y - screenMove.y * cY.x,
		screenMove.x * cZ.y - screenMove.y * cZ.x );	

	Ogre::Vector3 rotVec = move3D;
	rotVec = rotVec * 0.5;	
	if(rAngles) 
	{
		rAngles->x = 0;
		rAngles->y = 0;
		rAngles->z = 0;
	}

	if(m_CoordLockString.find("X", 0) != std::string::npos)	
	{
		rNewRot = Ogre::Quaternion(Ogre::Radian(rotVec.x), Ogre::Vector3::UNIT_X) * rOldRot;
		if(rAngles) rAngles->x = rotVec.x * 180 / Ogre::Math::PI; 
	}

	if(m_CoordLockString.find("Y", 0) != std::string::npos)			
	{
		rNewRot = Ogre::Quaternion(Ogre::Radian(rotVec.y), Ogre::Vector3::UNIT_Y) * rOldRot;
		if(rAngles) rAngles->y = rotVec.y * 180 / Ogre::Math::PI; 
	}

	if(m_CoordLockString.find("Z", 0) != std::string::npos)
	{
		rNewRot = Ogre::Quaternion(Ogre::Radian(rotVec.z), Ogre::Vector3::UNIT_Z) * rOldRot;
		if(rAngles) rAngles->z = rotVec.z * 180 / Ogre::Math::PI;
	}

}
// ----------------------------------------------
void CTransformManager::GetViewportRotate(
										const Ogre::Vector3& rPos, 
										const Ogre::Vector2& rScreenMove, 
										Ogre::Quaternion& rDRot
										)
{
	float camDis = (SCENE_CAM -> getViewMatrix() * rPos).z;

	float clipNear = SCENE_CAM->getNearClipDistance();	

	Ogre::Vector2 screenMove;
	screenMove.x =    rScreenMove.x * Ogre::Math::Abs(camDis) / clipNear * 0.001;	//screen coordnate
	screenMove.y =  - rScreenMove.y * Ogre::Math::Abs(camDis) / clipNear * 0.001;	//screen coordnate

	// 摄像机坐标轴
	Ogre::Vector3 camX = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_X;
	Ogre::Vector3 camY = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_Y;

	
	// 世界坐标轴 屏幕投影的到2维向量	
	Ogre::Vector2 cX = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_X), camY.dotProduct(Ogre::Vector3::UNIT_X));
	Ogre::Vector2 cY = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Y), camY.dotProduct(Ogre::Vector3::UNIT_Y));
	Ogre::Vector2 cZ = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Z), camY.dotProduct(Ogre::Vector3::UNIT_Z));
	
	Ogre::Vector3 move3D = Ogre::Vector3(
		screenMove.x * cX.y - screenMove.y * cX.x, 
		screenMove.x * cY.y - screenMove.y * cY.x,
		screenMove.x * cZ.y - screenMove.y * cZ.x );	

	Ogre::Vector3 rotVec = move3D;
	rotVec = rotVec * 0.5;	

	if(m_CoordLockString.find("X", 0) != std::string::npos)	
	{
		rDRot = Ogre::Quaternion(Ogre::Radian(rotVec.x), Ogre::Vector3::UNIT_X);
	}

	if(m_CoordLockString.find("Y", 0) != std::string::npos)			
	{
		rDRot = Ogre::Quaternion(Ogre::Radian(rotVec.y), Ogre::Vector3::UNIT_Y);
	}

	if(m_CoordLockString.find("Z", 0) != std::string::npos)
	{
		rDRot = Ogre::Quaternion(Ogre::Radian(rotVec.z), Ogre::Vector3::UNIT_Z);
	}

}

// ----------------------------------------------
// 通过屏幕移动得到3D缩放
void CTransformManager::GetViewportScaling(
		const Ogre::Vector3& rPos, const Ogre::Vector2& rScreenMove, Ogre::Vector3& rOldScale, Ogre::Vector3& rNewScale)
{
	float camDis = (SCENE_CAM -> getViewMatrix() * rPos).z;

	float clipNear = SCENE_CAM->getNearClipDistance();	

	Ogre::Vector2 screenMove;
	screenMove.x =    rScreenMove.x * Ogre::Math::Abs(camDis) / clipNear * 0.0001;	//screen coordnate
	screenMove.y =  - rScreenMove.y * Ogre::Math::Abs(camDis) / clipNear * 0.0001;	//screen coordnate

	// 摄像机坐标轴
	Ogre::Vector3 camX = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_X;
	Ogre::Vector3 camY = SCENE_CAM->getOrientation() * Ogre::Vector3::UNIT_Y;

	
	// 世界坐标轴 屏幕投影的到2维向量	
	Ogre::Vector2 cX = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_X), camY.dotProduct(Ogre::Vector3::UNIT_X));
	Ogre::Vector2 cY = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Y), camY.dotProduct(Ogre::Vector3::UNIT_Y));
	Ogre::Vector2 cZ = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Z), camY.dotProduct(Ogre::Vector3::UNIT_Z));

	
	Ogre::Vector3 move = Ogre::Vector3(screenMove.dotProduct(cX), screenMove.dotProduct(cY), screenMove.dotProduct(cZ));

	
	Ogre::Vector3 needMove = Ogre::Vector3::ZERO;
	
	if(m_CoordLockString.find("X", 0) != std::string::npos)
		needMove.x += move.x;

	else if(m_CoordLockString.find("Y", 0) != std::string::npos)	
		needMove.y += move.y;

	else if(m_CoordLockString.find("Z", 0) != std::string::npos)
		needMove.z += move.z;

	else if(m_CoordLockString.find("XY", 0) != std::string::npos)
	{
		float fm = Ogre::Vector2(move.x, move.y).dotProduct(Ogre::Vector2(0.707, 0.707));
		needMove.x += fm;
		needMove.y += fm;	
	}
	else if(m_CoordLockString.find("XZ", 0) != std::string::npos)
	{
		float fm = Ogre::Vector2(move.x, move.z).dotProduct(Ogre::Vector2(0.707, 0.707));
	
		needMove.x += fm;
		needMove.z += fm;	
	}	
	else if(m_CoordLockString.find("YZ", 0) != std::string::npos)
	{		
		float fm = Ogre::Vector2(move.y, move.z).dotProduct(Ogre::Vector2(0.707, 0.707));

		needMove.y += fm;
		needMove.z += fm;
	}
	else if(m_CoordLockString.find("XYZ", 0) != std::string::npos)
	{	
		float fm = move.dotProduct(Ogre::Vector3(0.577, 0.577, 0.577));

		needMove.x += fm;
		needMove.y += fm;
		needMove.z += fm;
	}
	
	rNewScale = rOldScale +  needMove;

	rNewScale.x = Ogre::Math::Abs(rNewScale.x);
	rNewScale.y = Ogre::Math::Abs(rNewScale.y);
	rNewScale.z = Ogre::Math::Abs(rNewScale.z);
}
