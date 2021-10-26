/***************************************************************************************************
文件名称:	DymMagicMesh.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	网格
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagicmesh.h"
#include <msxml.h>
#include <stdio.h>

#import   "msxml.dll"

void CDymMagicMesh::OnCreateTexture( LPCTSTR strTextureName )
{
}
CDymMagicMesh::CDymMagicMesh(CDymRenderSystem *pDevice)
{
	bUse32bitindexes = FALSE;
	m_pDevice = pDevice;
	m_pVB = NULL;
	m_pIB = NULL;
	m_nNumVertices = 0;
	m_nPrimitiveCount = 0;
}

CDymMagicMesh::~CDymMagicMesh(void)
{
	if(m_pVB)m_pVB->Release ();
	if(m_pIB)m_pIB->Release ();
}

void CDymMagicMesh::Release()
{
	m_pDevice->ReleaseMesh( this );
}

D3DXVECTOR3* CDymMagicMesh::GetVertexStreamZeroData(int *pfacenum,int *pStride)
{
	if( m_pVB == NULL || m_pIB == NULL )return NULL;
	if( m_nPrimitiveCount <= 0 )return NULL;
	MAGIC_MESH_VERTEX *pVertex = new MAGIC_MESH_VERTEX[ m_nPrimitiveCount*3 ];

	MAGIC_MESH_VERTEX *pVertices = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_READONLY);
	WORD *pIndex;
	m_pIB->Lock (0,0,(void**)&pIndex,D3DLOCK_READONLY);

	for(int i=0;i<m_nPrimitiveCount*3;i++)
	{
		pVertex[i] = pVertices[ pIndex[i] ];
	}
	m_pVB->Unlock ();
	m_pIB->Unlock ();
	*pfacenum = m_nPrimitiveCount;
	*pStride = sizeof( MAGIC_MESH_VERTEX );
	return (D3DXVECTOR3*)pVertex;
}
HRESULT CDymMagicMesh::LoadFromMeshInMemory( char *pMem , int datalen )
{
	if( datalen < sizeof( MESH_FILE_HEADER ) + sizeof( MESH_FILE_HEADER2 ) )return E_FAIL;
	MESH_FILE_HEADER header;
	header.header[0] = 'D';
	header.header[1] = 'M';
	header.header[2] = 'D';
	header.version = 1;

	int offset = 0;
	MESH_FILE_HEADER header_load;
	memcpy( &header_load , pMem , sizeof( MESH_FILE_HEADER ) );
	if( memcmp( &header_load , &header , sizeof( MESH_FILE_HEADER ) ) != 0 )return E_FAIL;
	offset += sizeof( MESH_FILE_HEADER );
	MESH_FILE_HEADER2 header2;
	memcpy( &header2 , pMem + offset , sizeof( MESH_FILE_HEADER2 ) );

	// 文件大小监测
	int d1 = header2.vertexnum * sizeof( MAGIC_MESH_VERTEX ) + header2.indexnum * sizeof( WORD ) * ( header2.bUser32bitindexes ? 2 : 1 );
	int d2 = datalen - sizeof( MESH_FILE_HEADER ) - sizeof( MESH_FILE_HEADER2 );
	if( d1 != d2 )
		return E_FAIL;
	offset += sizeof( MESH_FILE_HEADER2 );
	for( int i=0;i<header2.vertexnum ;i++ )
	{
		MAGIC_MESH_VERTEX vertex;
		memcpy( &vertex , pMem + offset , sizeof( MAGIC_MESH_VERTEX ) );
		m_vecVertex.push_back ( vertex );
		offset += sizeof( MAGIC_MESH_VERTEX );
	}
	for( int i=0;i<header2.indexnum ;i++)
	{
		WORD idx;
		memcpy( &idx , pMem + offset , sizeof( WORD ) );
		m_vecIndex.push_back ( idx );
		offset += sizeof( WORD );
	}
	_init();
	return S_OK;
}
HRESULT CDymMagicMesh::LoadFromMeshFile( LPCTSTR meshFileName )
{
	/*FILE *p = fopen( meshFileName , "rb" );
	if(!p)return E_POINTER ;
	fseek(p,0,SEEK_END);
	int nFileSize = ftell(p);
	fseek(p,0,SEEK_SET);
	char *pFileBuffer = new char[nFileSize+1];
	fread(pFileBuffer,nFileSize,1,p);
	fclose(p);*/
	int nFileSize;
	char *pFileBuffer = (char*)m_pDevice->LoadFromFile ( meshFileName , &nFileSize );
	if( pFileBuffer == NULL )return E_POINTER;

	HRESULT hr = LoadFromMeshInMemory( pFileBuffer , nFileSize );
	delete[] pFileBuffer;
	return hr;
}
HRESULT CDymMagicMesh::SaveToMeshFile( LPCTSTR meshFileName )
{
	if( m_pVB == NULL || m_pIB == NULL )return NULL;
	if( m_nPrimitiveCount <= 0 )return NULL;
	MESH_FILE_HEADER header;
	header.header[0] = 'D';
	header.header[1] = 'M';
	header.header[2] = 'D';
	header.version = 1;
	MESH_FILE_HEADER2 header2;
	header2.bUser32bitindexes = bUse32bitindexes ;
	header2.vertexnum = m_nNumVertices;
	header2.indexnum = m_nPrimitiveCount*3;

	FILE *pFile = fopen( meshFileName , "wb" );
	if( !pFile )return E_FAIL;
	fwrite( &header , 1 , sizeof( MESH_FILE_HEADER ) , pFile );
	fwrite( &header2 , 1 , sizeof( MESH_FILE_HEADER2 ) , pFile );

	MAGIC_MESH_VERTEX *pVertices = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_READONLY);
	WORD *pIndex;
	m_pIB->Lock (0,0,(void**)&pIndex,D3DLOCK_READONLY);

	for(int i=0;i<m_nNumVertices;i++)
	{
		fwrite( &pVertices[i] , 1 , sizeof( MAGIC_MESH_VERTEX ) , pFile );
	}
	for(int i=0;i<m_nPrimitiveCount*3;i++)
	{
		fwrite( &pIndex[i] , 1 , sizeof( WORD ) , pFile );
	}
	m_pVB->Unlock ();
	m_pIB->Unlock ();
	fclose( pFile );
	return S_OK;
}

HRESULT CDymMagicMesh::_init()
{
	m_pDevice->CreateVertexBuffer(m_vecVertex.size () * sizeof(MAGIC_MESH_VERTEX),  D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			0, D3DPOOL_DEFAULT, &m_pVB,NULL);
	m_pDevice->CreateIndexBuffer(m_vecIndex.size () * sizeof(WORD),D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pIB,NULL);

	MAGIC_MESH_VERTEX *pVertices = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	for(int i=0;i<m_vecVertex.size ();i++)
	{
		pVertices[i] = m_vecVertex[i];
	}
	m_pVB->Unlock ();

	WORD *pIndex;
	m_pIB->Lock (0,0,(void**)&pIndex,0);
	for(int i=0;i<m_vecIndex.size ();i++)
	{
		pIndex[i] = (WORD)m_vecIndex[i];
	}
	m_pIB->Unlock ();

	m_nNumVertices = m_vecVertex.size ();
	m_nPrimitiveCount = m_vecIndex.size ()/3;

	m_vecVertex.clear ();
	m_vecIndex.clear ();
	return S_OK;
}

HRESULT CDymMagicMesh::Render()
{
	HRESULT hr;
	if(FAILED(hr=m_pDevice->SetFVF(D3DFVF_DIFFUSE|D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)))return hr;
	if(FAILED(hr=m_pDevice->SetStreamSource(0, m_pVB, 0,sizeof(MAGIC_MESH_VERTEX))))return hr;
	hr=m_pDevice->SetIndices(m_pIB);
	hr=m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST ,0,0,m_nNumVertices,0,m_nPrimitiveCount);
	return S_OK;
}

void CDymMagicMesh::Mesh_AddIndex( int i1 , int i2 , int i3 )
{
	int size = m_vecVertex.size ();
	m_vecIndex.push_back ( i1+size );
	m_vecIndex.push_back ( i2+size );
	m_vecIndex.push_back ( i3+size );
}

void CDymMagicMesh::Mesh_AddVertex( const MAGIC_MESH_VERTEX *pVertex )
{
	m_vecVertex.push_back ( *pVertex );
}

void CDymMagicMesh::Loading_AddVertex( IXMLDOMNodeList *pList , int vertexnum )
{
	if( !pList )return;
	long len;
	pList->get_length(&len);//得到所包含的NODE节点的个数
	pList->reset();
	MAGIC_MESH_VERTEX vertexdata;
	for(long i=0;i<len;i++)
	{
		IXMLDOMNode *pNode;
		pList->get_item( i , &pNode );
		if( pNode )
		{
			BSTR str;
			pNode->get_nodeName ( &str );
			if( _wcsicmp( str , L"position" ) == 0 )
			{
				IXMLDOMNamedNodeMap* attributes;
				pNode->get_attributes ( &attributes );
				if( attributes )
				{
					IXMLDOMNode *pAttributesNode;
					BSTR str2;

					attributes->getNamedItem(L"x", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vPos.x = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"y", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vPos.y = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"z", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vPos.z = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
					attributes->Release ();
				}
			}
			else if( _wcsicmp( str , L"normal" ) == 0 )
			{
				IXMLDOMNamedNodeMap* attributes;
				pNode->get_attributes ( &attributes );
				if( attributes )
				{
					IXMLDOMNode *pAttributesNode;
					BSTR str2;

					attributes->getNamedItem(L"x", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vNormal.x = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"y", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vNormal.y = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"z", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vNormal.z = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
				}
				attributes->Release ();
			}
			else if( _wcsicmp( str , L"texcoord" ) == 0 )
			{
				IXMLDOMNamedNodeMap* attributes;
				pNode->get_attributes ( &attributes );
				if( attributes )
				{
					IXMLDOMNode *pAttributesNode;
					BSTR str2;

					attributes->getNamedItem(L"u", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vUV.x = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"v", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						vertexdata.vUV.y = (float)_wtof( str2 );
						pAttributesNode->Release ();
					}
				}
				attributes->Release ();
			}
			else if( _wcsicmp( str , L"colour_diffuse" ) == 0 )
			{
				IXMLDOMNamedNodeMap* attributes;
				pNode->get_attributes ( &attributes );
				if( attributes )
				{
					IXMLDOMNode *pAttributesNode;
					BSTR str2;

					attributes->getNamedItem(L"value", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );

						int color[4];
						int icolor=0;
						BSTR p = (BSTR)wcstok( str2 , L" " );
						while(p!=NULL)
						{
							float l = (float)_wtof( p );
							color[icolor] = (int)( l*255.0f );
							icolor++;
							p=wcstok(NULL,L" ");
						}

						color[1] = 255;
						color[2] = 255;
						color[3] = 255;
						vertexdata.dwColor = (color[0]<<24) | (color[1]<<16) | (color[2]<<8) | color[3];

						pAttributesNode->Release ();
					}
				}
				attributes->Release ();
			}
			pNode->Release ();
		}
	}
	Mesh_AddVertex( &vertexdata );
}

void CDymMagicMesh::Loading_AddGeometry( IXMLDOMNodeList *pList , int vertexnum )
{
	if( !pList )return;
	long len;
	pList->get_length(&len);//得到所包含的NODE节点的个数
	pList->reset();
	for(long i=0;i<len;i++)
	{
		IXMLDOMNode *pNode;
		pList->get_item( i , &pNode );
		if( pNode )
		{
			BSTR str;
			pNode->get_nodeName ( &str );
			if( _wcsicmp( str , L"vertexbuffer" ) == 0 )
			{
				IXMLDOMNodeList* pListSubMesh=NULL ;
				pNode->get_childNodes ( &pListSubMesh );

				if( pListSubMesh )
				{
					long len2;
					pListSubMesh->get_length ( &len2 );
					pListSubMesh->reset ();
					for(long j=0;j<len2;j++)
					{
						IXMLDOMNode *pNode2;
						pListSubMesh->get_item( j , &pNode2 );
						if( pNode2 )
						{
							BSTR str2;
							pNode2->get_nodeName ( &str2 );
							if( _wcsicmp( str2 , L"vertex" ) == 0 )
							{
								IXMLDOMNodeList* pListSubMesh2=NULL ;
								pNode2->get_childNodes ( &pListSubMesh2 );

								if( pListSubMesh2 )
								{
									Loading_AddVertex( pListSubMesh2 , vertexnum );
									pListSubMesh2->Release ();
								}
							}
							pNode2->Release ();
						}
					}
					pListSubMesh->Release ();
				}
			}
			pNode->Release ();
		}
	}
}
void CDymMagicMesh::Loading_AddFaces( IXMLDOMNodeList *pList , int facenum )
{
	if( !pList )return;
	long len;
	pList->get_length(&len);//得到所包含的NODE节点的个数
	pList->reset();
	for(long i=0;i<len;i++)
	{
		IXMLDOMNode *pNode;
		pList->get_item( i , &pNode );
		if( pNode )
		{
			BSTR str;
			pNode->get_nodeName ( &str );
			if( _wcsicmp( str , L"face" ) == 0 )
			{
				int i1,i2,i3;
				IXMLDOMNamedNodeMap* attributes;
				pNode->get_attributes ( &attributes );
				if( attributes )
				{
					IXMLDOMNode *pAttributesNode;
					BSTR str2;

					attributes->getNamedItem(L"v1", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						i1 = _wtoi( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"v2", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						i2 = _wtoi( str2 );
						pAttributesNode->Release ();
					}
					attributes->getNamedItem(L"v3", &pAttributesNode);
					if( pAttributesNode )
					{
						pAttributesNode->get_text ( &str2 );
						i3 = _wtoi( str2 );
						pAttributesNode->Release ();
					}

					Mesh_AddIndex( i1 , i2 , i3 );
					attributes->Release ();
				}
			}
			pNode->Release ();
		}
	}
}

void CDymMagicMesh::Loading_AddSubMesh( IXMLDOMNodeList *pList )
{
	if( !pList )return;
	long len;
	pList->get_length(&len);//得到所包含的NODE节点的个数
	pList->reset();
	for(long i=0;i<len;i++)
	{
		IXMLDOMNode *pNode;
		pList->get_item( i , &pNode );
		if( pNode )
		{
			BSTR str;
			pNode->get_nodeName ( &str );
			if( _wcsicmp( str , L"faces" ) == 0 )
			{
				IXMLDOMNodeList* pListSubMesh=NULL ;
				pNode->get_childNodes ( &pListSubMesh );

				if( pListSubMesh )
				{
					IXMLDOMNamedNodeMap* attributes;
					pNode->get_attributes ( &attributes );
					if( attributes )
					{
						IXMLDOMNode *pAttributesNode;
						attributes->getNamedItem(L"count", &pAttributesNode);
						if( pAttributesNode )
						{
							BSTR str2;
							pAttributesNode->get_text ( &str2 );
							Loading_AddFaces( pListSubMesh , _wtoi(str2) );
							pAttributesNode->Release ();
						}
						attributes->Release ();
					}
					pListSubMesh->Release ();
				}
			}
			else if( _wcsicmp( str , L"geometry" ) == 0 )
			{
				IXMLDOMNodeList* pListSubMesh=NULL ;
				pNode->get_childNodes ( &pListSubMesh );

				if( pListSubMesh )
				{
					IXMLDOMNamedNodeMap* attributes;
					pNode->get_attributes ( &attributes );
					if( attributes )
					{
						IXMLDOMNode *pAttributesNode;
						attributes->getNamedItem(L"vertexcount", &pAttributesNode);
						if( pAttributesNode )
						{
							BSTR str2;
							pAttributesNode->get_text ( &str2 );
							Loading_AddGeometry( pListSubMesh , _wtoi(str2) );
							pAttributesNode->Release ();
						}
						attributes->Release ();
					}
					pListSubMesh->Release ();
				}
			}
			pNode->Release ();
		}
	}
}
HRESULT CDymMagicMesh::LoadFromXML( void *xmldata , int datalen )
{
	if(m_pVB)m_pVB->Release ();
	if(m_pIB)m_pIB->Release ();
	HRESULT hr;
	IXMLDOMDocument *pXMLDoc;
	//COM的初始化
	hr=CoInitialize(NULL);
	if( FAILED( hr ) )return hr;
	//得到关于IXMLDOMDocument接口的指针pXMLDOC
	hr=CoCreateInstance(CLSID_DOMDocument,NULL,CLSCTX_INPROC_SERVER,IID_IXMLDOMDocument,(void**)&pXMLDoc);
	if( FAILED( hr ) )
	{
		CoUninitialize();
		return hr;
	}

	VARIANT_BOOL bSucceed;
	/*_variant_t varXml;
	varXml.SetString ( fileName );
	pXMLDoc->load( varXml , &bSucceed);*/
	pXMLDoc->loadXML( (BSTR)xmldata , &bSucceed );
	if( !bSucceed )
	{
		CoUninitialize();
		return E_FAIL;
	}
	//IXMLDOMNodePtr p = pXMLDoc->selectSingleNode ( L"mesh/" );
	IXMLDOMNodeList* pList ;
	pXMLDoc->getElementsByTagName ( L"mesh/submeshes/submesh" , &pList );

	if( pList == NULL )
	{
		CoUninitialize();
		return E_FAIL;
	}
	long len;
	pList->get_length(&len);//得到所包含的NODE节点的个数
	pList->reset();
	for(long i=0;i<len;i++)
	{
		IXMLDOMNode *pNode;
		pList->get_item( i , &pNode );
		if( pNode )
		{
			BSTR str;
			pNode->get_nodeName ( &str );
			if( _wcsicmp( str , L"submesh" ) == 0 )
			{
				IXMLDOMNodeList* pListSubMesh=NULL ;
				pNode->get_childNodes ( &pListSubMesh );

				if( pListSubMesh )
				{
					IXMLDOMNamedNodeMap* attributes;
					pNode->get_attributes ( &attributes );
					if( attributes )
					{
						IXMLDOMNode *pAttributesNode;
						attributes->getNamedItem(L"use32bitindexes", &pAttributesNode);
						if( pAttributesNode )
						{
							BSTR str2;
							pAttributesNode->get_text ( &str2 );
							if( _wcsicmp( str2 , L"true" ) == 0 )
								bUse32bitindexes = TRUE;
							pAttributesNode->Release ();
						}
						attributes->getNamedItem(L"resource", &pAttributesNode);
						if( pAttributesNode )
						{
							BSTR str2;
							pAttributesNode->get_text ( &str2 );//贴图

							TCHAR strtexbuffer[256];
							int j = 0 ;
							for( int i = 0 ; ; i ++ )
							{
								if( str2[i] == L'%' )
								{
									int n = 0;
									int n2 = 0;
									int n3 = 0;
									if( str2[i+1] >= L'0' && str2[i+1] <=L'9' )n = str2[i+1]-L'0';
									else if( str2[i+1] >=L'A' && str2[i+1] <=L'F' )n = str2[i+1]-L'A'+10;
									if( str2[i+2] >= L'0' && str2[i+2] <=L'9' )n2 = str2[i+2]-L'0';
									else if( str2[i+2] >=L'A' && str2[i+2] <=L'F' )n2 = str2[i+2]-L'A'+10;
									n3 = n<<4|n2;
									strtexbuffer[j] = (TCHAR)n3;
									j++;
									i+=2;
								}
								else
								{
#ifdef _UNICODE
									strtexbuffer[j] = str2[i];
#else
									BOOL lpUsedDefaultChar;
									::WideCharToMultiByte ( CP_ACP, 0, &str2[i],2,&strtexbuffer[j],1,"",&lpUsedDefaultChar );
#endif
									j++;
								}
								if( str2[i] == 0 )
									break;
							}
							OnCreateTexture ( strtexbuffer );
							pAttributesNode->Release ();
						}
						attributes->Release ();
						Loading_AddSubMesh( pListSubMesh );
					}
					pListSubMesh->Release ();
				}
			}
			pNode->Release ();
		}
	}
	pList->Release ();
	pXMLDoc->Release ();

	CoUninitialize();

	if( FAILED( hr ) )return hr;
	return _init();
}

HRESULT CDymMagicMesh::LoadFromFile( LPCTSTR fileName )
{
	FILE *p = fopen( fileName , "rb" );
	if(!p)return FALSE;
	fseek(p,0,SEEK_END);
	int nFileSize = ftell(p);
	fseek(p,0,SEEK_SET);
	char *pFileBuffer = new char[nFileSize+1];
	char *pFileBuffer2 = new char[nFileSize*3+3];
	fread(pFileBuffer,nFileSize,1,p);
	fclose(p);

	pFileBuffer[nFileSize]=0;
	int ii = 0;
	for(int i=0;i<nFileSize+1;i++)
	{
		//if( pFileBuffer[i] <0 )pFileBuffer[i]='%';
		if( pFileBuffer[i] < 0 )
		{
			pFileBuffer2[ii] = '%';
			BYTE b1 = (((BYTE)pFileBuffer[i])&0xF0)>>4;
			BYTE b2 = ((BYTE)pFileBuffer[i])&0x0F;
			if( b1<=9 )
				pFileBuffer2[ii+1] = b1+'0';
			else
				pFileBuffer2[ii+1] = b1+'A'-10;
			if( b2<=9 )
				pFileBuffer2[ii+2] = b2+'0';
			else
				pFileBuffer2[ii+2] = b2+'A'-10;
			ii+=2;
		}
		else
			pFileBuffer2[ii] = pFileBuffer[i];
		ii++;
	}
	
	WCHAR* wstr = new WCHAR[ (nFileSize*3+3)*2 ];
	
	::MultiByteToWideChar ( CP_ACP, 0, pFileBuffer2,nFileSize*3+3,wstr,(nFileSize*3+3)*2 );
	HRESULT hr = LoadFromXML( wstr , nFileSize*3 ); 
	delete[]pFileBuffer;
	delete[]pFileBuffer2;
	delete[]wstr;
	return hr;
}