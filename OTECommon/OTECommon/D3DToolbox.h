#pragma once

#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreMemoryMacros.h"

// -----------------------------------------
// YiMing的D3D绘制函数(稍改过)
// -----------------------------------------

inline void DrawLine(LPDIRECT3DDEVICE9 d3dDevice, D3DXVECTOR3 v1,D3DXVECTOR3 v2,DWORD color)
{
	d3dDevice->SetTexture ( 0 , NULL );
	struct VERTEX_LINE
	{
		D3DXVECTOR3 pos;
		DWORD color;
	};
	// Draw the quad
    VERTEX_LINE svQuad[2];

    svQuad[0].pos = v1;
    svQuad[0].color = color;

    svQuad[1].pos = v2;
    svQuad[1].color = color;

    d3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
    d3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, svQuad, sizeof(VERTEX_LINE));
}

// -----------------------------------------
inline BOOL DrawCircle(LPDIRECT3DDEVICE9 d3dDevice, const D3DXVECTOR3& origal, float radio, DWORD color)
{
	D3DXMATRIX idt;
	D3DXMatrixIdentity( &idt );
	d3dDevice->SetTransform ( D3DTS_WORLD , &idt );
	
	const int c_Line_Num = 16;

	D3DXMATRIX mat;	
	D3DXMatrixRotationZ( &mat, D3DX_PI * 2.0f / (float)c_Line_Num );

	D3DXVECTOR3 v = D3DXVECTOR3(radio, 0 ,0);
	for(int i = 0; i < c_Line_Num; i++)
	{
		D3DXVECTOR3 v2;
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4, &v, &mat );
		v2 = D3DXVECTOR3(v4.x , v4.y , v4.z) / v4.w;
		DrawLine( d3dDevice, v + origal, v2 + origal, color );
		v = v2;
	}
	return TRUE;
}


// -----------------------------------------
// 效率不高
inline BOOL DrawWireBall(LPDIRECT3DDEVICE9 d3dDevice, const D3DXVECTOR3& origal, float radio, DWORD color)
{
	D3DXMATRIX idt;
	D3DXMatrixIdentity( &idt );
	d3dDevice->SetTransform ( D3DTS_WORLD , &idt );
	
	const int c_Line_Num = 4;

	D3DXMATRIX mat;	
	D3DXVECTOR3 v;

	float rad = D3DX_PI * 2.0f / (float)c_Line_Num;

	// X
	D3DXMatrixRotationX( &mat, rad );
	v = D3DXVECTOR3(0 ,0, radio);
	for(int i = 0; i < c_Line_Num; i++)
	{
		D3DXVECTOR3 v2;
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4, &v, &mat );
		v2 = D3DXVECTOR3(v4.x , v4.y , v4.z) / v4.w;
		DrawLine( d3dDevice, v + origal, v2 + origal, color );
		v = v2;
	}

	// Y

	D3DXMatrixIdentity( &mat );
	D3DXMatrixRotationY( &mat, rad );
	v = D3DXVECTOR3(radio, 0 ,0);
	for(int i = 0; i < c_Line_Num; i++)
	{
		D3DXVECTOR3 v2;
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4, &v, &mat );
		v2 = D3DXVECTOR3(v4.x , v4.y , v4.z) / v4.w;
		DrawLine( d3dDevice, v + origal, v2 + origal, color );
		v = v2;
	}

	// Z

	D3DXMatrixIdentity( &mat );	
	D3DXMatrixRotationZ( &mat, rad );
	v = D3DXVECTOR3(0, radio, 0);
	for(int i = 0; i < c_Line_Num; i++)
	{
		D3DXVECTOR3 v2;
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4, &v, &mat );
		v2 = D3DXVECTOR3(v4.x , v4.y , v4.z) / v4.w;
		DrawLine( d3dDevice, v + origal, v2 + origal, color );
		v = v2;
	}


	return TRUE;
}


// -----------------------------------------
// 点选测试
// -----------------------------------------
inline bool D3DLineHitTest(const Ogre::Matrix4& camMat,
							const Ogre::Vector3& p1, 
							const Ogre::Vector3& p2, 
							float prjX, float prjY)
{
	Ogre::Vector2 pp1 = Ogre::Vector2(Ogre::Vector3(camMat * p1).x, Ogre::Vector3(camMat * p1).y);
	Ogre::Vector2 pp2 = Ogre::Vector2(Ogre::Vector3(camMat * p2).x, Ogre::Vector3(camMat * p2).y);
	Ogre::Vector2 pp = Ogre::Vector2(prjX, prjY);
	
	Ogre::Vector2 p1topp = pp - pp1;
	Ogre::Vector2 p1top2 = pp2 - pp1;

	float dot = p1topp.dotProduct(p1top2.normalisedCopy()); // 点在 向量p1top2 上投影长度
			
	float a = Ogre::Math::Abs(p1topp.x * p1top2.y - p1top2.x * p1topp.y);
	float d = a / (pp2 - pp1).length(); // 点到向量p1top2的距离

	if(dot < p1top2.length() * 1.0f && dot > p1top2.length() * 0.05 && d < 0.015)
	{		
		return true;
	}	
	return false;
}