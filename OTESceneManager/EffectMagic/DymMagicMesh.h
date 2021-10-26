/***************************************************************************************************
文件名称:	DymMagicMesh.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	网格
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <msxml.h>
#include <vector>
#include "DymRenderSystem.h"

//----------------------------------------------------------------------------------
//
// 网格
//
//----------------------------------------------------------------------------------
class CDymMagicMesh
{
protected:
	// 顶点FVF
	struct MAGIC_MESH_VERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		DWORD dwColor;
		D3DXVECTOR2 vUV;
	};
	// 网格文件头
	struct MESH_FILE_HEADER
	{
		char header[3];//文件头
		char version;//版本
	};
	struct MESH_FILE_HEADER2
	{
		int vertexnum;//顶点数
		int indexnum;//索引数
		BOOL bUser32bitindexes;//是否使用32位索引 
	};
	BOOL bUse32bitindexes;//是否使用32位索引 
	std::vector< int > m_vecIndex;// 索引缓冲
	std::vector< MAGIC_MESH_VERTEX > m_vecVertex;//顶点缓冲
	CDymD3DVertexBuffer *m_pVB;//顶点设备缓冲
	CDymD3DIndexBuffer *m_pIB;// 索引设备缓冲
	CDymRenderSystem *m_pDevice;// D3D设备
	int m_nNumVertices;// 顶点数
	int m_nPrimitiveCount;// 三角形数
	
	// 载入用添加一三角形
	void Loading_AddFaces( IXMLDOMNodeList *pList , int facenum );
	// 载入用添加一物体
	void Loading_AddGeometry( IXMLDOMNodeList *pList , int vertexnum );
	// 载入用添加一顶点
	void Loading_AddVertex( IXMLDOMNodeList *pList , int vertexnum );
	// 载入用添加一网格
	void Loading_AddSubMesh( IXMLDOMNodeList *pList );
	// 添加索引
	void Mesh_AddIndex( int i1 , int i2 , int i3 );
	// 添加顶点
	void Mesh_AddVertex( const MAGIC_MESH_VERTEX *pVertex );

	// 内部用初始化
	HRESULT _init();
public:
	CDymMagicMesh(CDymRenderSystem *pDevice);
	~CDymMagicMesh(void);
	// 从文件中载入文件数据
	HRESULT LoadFromFile( LPCTSTR fileName );
	// 从XML中载入网格
	HRESULT LoadFromXML( void *xmldata , int datalen );
	// 从文件中载入网格
	HRESULT LoadFromMeshFile( LPCTSTR meshFileName );
	// 从内存中载入网格
	HRESULT LoadFromMeshInMemory( char *pMem , int datalen );
	// 保存到文件
	HRESULT SaveToMeshFile( LPCTSTR meshFileName );
	// 渲染
	HRESULT Render();
	// 释放 
	void Release();
	// 创建贴图回调
	virtual void OnCreateTexture( LPCTSTR strTextureName );
	// 获得顶点数据
	D3DXVECTOR3* GetVertexStreamZeroData(int *pfacenum,int *pStride);
};
