/***************************************************************************************************
�ļ�����:	DymMagicMesh.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	����
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <msxml.h>
#include <vector>
#include "DymRenderSystem.h"

//----------------------------------------------------------------------------------
//
// ����
//
//----------------------------------------------------------------------------------
class CDymMagicMesh
{
protected:
	// ����FVF
	struct MAGIC_MESH_VERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		DWORD dwColor;
		D3DXVECTOR2 vUV;
	};
	// �����ļ�ͷ
	struct MESH_FILE_HEADER
	{
		char header[3];//�ļ�ͷ
		char version;//�汾
	};
	struct MESH_FILE_HEADER2
	{
		int vertexnum;//������
		int indexnum;//������
		BOOL bUser32bitindexes;//�Ƿ�ʹ��32λ���� 
	};
	BOOL bUse32bitindexes;//�Ƿ�ʹ��32λ���� 
	std::vector< int > m_vecIndex;// ��������
	std::vector< MAGIC_MESH_VERTEX > m_vecVertex;//���㻺��
	CDymD3DVertexBuffer *m_pVB;//�����豸����
	CDymD3DIndexBuffer *m_pIB;// �����豸����
	CDymRenderSystem *m_pDevice;// D3D�豸
	int m_nNumVertices;// ������
	int m_nPrimitiveCount;// ��������
	
	// ���������һ������
	void Loading_AddFaces( IXMLDOMNodeList *pList , int facenum );
	// ���������һ����
	void Loading_AddGeometry( IXMLDOMNodeList *pList , int vertexnum );
	// ���������һ����
	void Loading_AddVertex( IXMLDOMNodeList *pList , int vertexnum );
	// ���������һ����
	void Loading_AddSubMesh( IXMLDOMNodeList *pList );
	// �������
	void Mesh_AddIndex( int i1 , int i2 , int i3 );
	// ��Ӷ���
	void Mesh_AddVertex( const MAGIC_MESH_VERTEX *pVertex );

	// �ڲ��ó�ʼ��
	HRESULT _init();
public:
	CDymMagicMesh(CDymRenderSystem *pDevice);
	~CDymMagicMesh(void);
	// ���ļ��������ļ�����
	HRESULT LoadFromFile( LPCTSTR fileName );
	// ��XML����������
	HRESULT LoadFromXML( void *xmldata , int datalen );
	// ���ļ�����������
	HRESULT LoadFromMeshFile( LPCTSTR meshFileName );
	// ���ڴ�����������
	HRESULT LoadFromMeshInMemory( char *pMem , int datalen );
	// ���浽�ļ�
	HRESULT SaveToMeshFile( LPCTSTR meshFileName );
	// ��Ⱦ
	HRESULT Render();
	// �ͷ� 
	void Release();
	// ������ͼ�ص�
	virtual void OnCreateTexture( LPCTSTR strTextureName );
	// ��ö�������
	D3DXVECTOR3* GetVertexStreamZeroData(int *pfacenum,int *pStride);
};
