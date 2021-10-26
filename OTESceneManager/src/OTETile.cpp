// =================================================
//  COTETile class
//  By Romeo Jan 10/06
// =================================================
#include "OTEStaticInclude.h"
#include "OTETile.h"
#include "OTETilePage.h"
#include "OgreHardwareVertexBuffer.h"
#include "OgreDefaultHardwareBufferManager.h"
#include "OgreRenderQueue.h"
#include "OgreRenderOperation.h"
#include "OgrePass.h"

#include "OTETerrainSerializer.h"
#include "OgreNoMemoryMacros.h"
#include "OTEGrass.h"
#include "OTEWater.h"
#include "OgreMemoryMacros.h"
#include "OTEVegManager.h"
#include "OTERenderLight.h"
#include "OTEEntityRender.h"

using namespace Ogre;
using namespace OTE;
using namespace std;

// ----------------------------------------------------
#define TILE_CG_BASE_ID	100

// һ��Ĭ����ͼ ����һ���������� ������ͼӦ����Զ��������
//"��������.bmp"
//#define DEFAULT_TEXTURE		"l2default.tga"
#define DEFAULT_TEXTURE		"��������.bmp"

// ----------------------------------------------------
// �ر�����

const float c_TexUMap[] = {0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0}; // ��������ͼ
const float c_TexUMapA[] = {0, 0.25, 0.5, 0.75, 1.0}; // ��ͨ��ͼ
const float c_TexVMap[] = {0, 0.25, 0.5, 0.75, 1.0};

/// ��������ͼ

inline float GetULeft(short uIndex)
{
	if(uIndex / 10)
		return c_TexUMapA[uIndex % 10];		
	else
		return c_TexUMap[uIndex];
		
}	
inline float GetURight(short uIndex)
{	
	if(uIndex / 10)
		return c_TexUMapA[(uIndex + 1) % 10];
	else
		return c_TexUMap[uIndex + 1];
		
}
inline float GetVTop(short vIndex)
{
	return c_TexVMap[vIndex];
}
inline float GetVBottom(short vIndex)
{
	return c_TexVMap[vIndex + 1];
}

// ----------------------------------------------------
COTETile::COTETile(const std::string& tileName, COTETilePage* parent) : 
m_Name(tileName),
m_Terrain(NULL),
m_pPositionBuffer(NULL),
m_pTerrainUnitMap(NULL),
m_pNormalBuffer(NULL),
m_pGrassList(NULL),
m_IndexData(NULL),
m_RenderState(eRenderfinished),
m_GrassNeedUpdate(false),
m_isVisible(true),
m_CliffMark(0),
m_WaterValue(0),
m_AlphaMapping(Ogre::Vector3::ZERO),
m_NormalUpdated(false),
m_UseSharedMaterial(false),
m_LightmapUnit(0)
#ifndef GAME_DEBUG1
,m_ParentRenderUnit(0)
#endif
{	
	m_pParentPage	=   parent;		

	MovableObject::mCastShadows = false;   

    for ( int i = 0; i < 4; i++ )
    {
        m_Neighbors[ i ] = 0;
    } 
}

COTETile::~COTETile(void)
{	
	DeleteGeometry();
	
	if(m_pGrassList)
	{
		((std::vector<SingleGrassVertex>*)m_pGrassList)->clear();
		delete (std::vector<SingleGrassVertex>*)m_pGrassList;
		m_pGrassList = 0;
	}
}

// ----------------------------------------------------
void COTETile::DeleteGeometry()
{
   /* 
   if(m_Terrain)
	{
        delete m_Terrain;
		m_Terrain = NULL;
	}

   if (m_pPositionBuffer)
	{
		delete [] m_pPositionBuffer;
		m_pPositionBuffer = NULL;
	}
	if(m_pNormalBuffer)
	{
		delete[] m_pNormalBuffer;
		m_pNormalBuffer = NULL;
	}

	if(m_pTerrainUnitMap)
	{
		delete m_pTerrainUnitMap;
		m_pTerrainUnitMap = NULL;
	}

	if(m_IndexData)
	{
		delete m_IndexData;
		m_IndexData = NULL;
	}*/
	
}

//-----------------------------------------------------------------------
bool COTETile::UpdateRenderQueue( RenderQueue* queue )
{
#ifdef GAME_DEBUG1

	static int tileCnt = 0;
	if(!m_isVisible) 
	{
		m_pParentPage->s_LastTile = NULL;
		m_RenderState = eRenderfinished;
		return false;
	}

	if(!m_pParentPage->s_LastTile || 
		m_IndexInPage != m_pParentPage->s_LastTile->m_IndexInPage + tileCnt				||
		this->getMaterial() != m_pParentPage->s_LastTile->getMaterial()					||
		tileCnt == (m_pParentPage->m_PageSizeX) / (m_pParentPage->m_TileSize)	||
		m_pParentPage->s_LastTile->m_IndexData->indexBuffer != m_IndexData->indexBuffer
		)
	{
		// ��һ��

if(!(::GetKeyState('G') & 0x80) )
{		
		queue->addRenderable(this, mRenderQueueID);			
}

		m_pParentPage->s_LastTile = this;
		m_pParentPage->s_LastTile->m_Terrain  ->vertexCount = GetTileVertexCount();
		m_pParentPage->s_LastTile->m_IndexData->indexCount  = GetTileIndexCount();
		tileCnt = 0;	
	}
	else
	{
		m_pParentPage->s_LastTile->m_Terrain  ->vertexCount += GetTileVertexCount();
		m_pParentPage->s_LastTile->m_IndexData->indexCount  += GetTileIndexCount();

	}

	tileCnt ++;

	UpdateLodLevel(m_IndexData, SCENE_CAM);

#else	

	bool isAddToRQ = false;
	if(m_ParentRenderUnit->m_RenderState == eRenderfinished)
	{
if(!(::GetKeyState('G') & 0x80) )
{
	if(m_isVisible) 
	{
		UpdateLodLevel(m_ParentRenderUnit->m_IndexData, SCENE_CAM);

		queue->addRenderable(this, mRenderQueueID);	
		m_ParentRenderUnit->m_RenderState = eRenderPrepared;
	}
	else
	{
		m_ParentRenderUnit->m_RenderState = eRenderfinished;
	}
}
else
{
	m_ParentRenderUnit->m_RenderState = eRenderfinished;
}		
		isAddToRQ = true;
	}

#endif	

	// ��Ⱦɽ����Ч

	UpdateTileEffect();

	// ��̬С���

	if(!m_ObjectList.empty())
		COTEAutoEntityMgr::RenderTileEntities(&m_ObjectList, queue);


	m_RenderState = eRenderfinished;

#ifdef GAME_DEBUG1

	return tileCnt == 1;
#else
	return isAddToRQ;
#endif
}

// ----------------------------------------------------------------------
void COTETile::UpdateTileEffect()
{
	static const float c_grass_max_squared_dis = 32400.0f;
	
	// ���²�

if(!(::GetKeyState('G') & 0x80))	
{
	Ogre::Vector3 vec = m_Center - SCENE_CAM->getPosition();

	if ( vec.squaredLength() < c_grass_max_squared_dis )
	{
		if(m_GrassNeedUpdate)
		{
			if(!m_pGrassList)
				m_pGrassList = new std::vector<SingleGrassVertex>();

			COTEVegManager::GetInstance()->UpdateTileVeg(	_vertex(0, 0, 0), _vertex(0, 0, 2),
															(std::vector<SingleGrassVertex>*)m_pGrassList,
															NULL, NULL);
			m_GrassNeedUpdate = false;
		}

		if(m_pGrassList)
		{
			int size = ((std::vector<SingleGrassVertex>*)m_pGrassList)->size();
			if( size > 0 )
			{		
				COTEGrass::GetSingleton()->AddGrass(&( *( (std::vector<SingleGrassVertex>*)m_pGrassList )->begin() ),	size);			
			}
		}
		
	}
}

	// ֱ����Ⱦˮ

if(!(::GetKeyState('W') & 0x80))
{
		if(m_WaterValue > 0)	
		{
			COTELiquid* ld = COTELiquidManager::GetSingleton()->GetLiquid(m_WaterValue);
			if(ld)
				ld->Update(this);
		}
}

}

//-----------------------------------------------------------------------
void COTETile::UpdateLodLevel(Ogre::IndexData* indexData, const Ogre::Camera* cam)
{
if(DEFAULT_TILESIZE != 16)	return; // ����Lod �㷨����Թ̶��ߴ�tile������ ���Բ���֧��������tile�ߴ�

#ifdef GAME_DEBUG1

	// ���ھ��Ե�ƽ��

	if(indexData->indexBuffer == m_pParentPage->m_LodLevel[0].IndexBuffer)
	{
		return;
	}	

	// ���ݾ������ʹ�õ�Lod 

	float distance = (cam->getPosition() - GetCenter()).squaredLength();

	if( distance > 150.0f * 150.0f)
	{
		indexData->indexBuffer = m_pParentPage->m_LodLevel[1].IndexBuffer;
		indexData->indexCount  = m_pParentPage->m_LodLevel[1].IndexCount;	

	}
	else
	{
		indexData->indexBuffer = m_pParentPage->m_IndexBuffer;
		indexData->indexCount  = GetTileIndexCount();
	}

#else	

	// ���ھ��Ե�ƽ��

	if(indexData->indexBuffer != m_pParentPage->m_LodLevel[0].IndexBuffer)
	{
		// ���ݾ������ʹ�õ�Lod 
		
		float distance = (Ogre::Vector2(cam->getPosition().x, cam->getPosition().z)
			- m_ParentRenderUnit->m_Position).squaredLength();

		if( distance > 150.0f * 150.0f)
		{
			indexData->indexBuffer = m_pParentPage->m_LodLevel[1].IndexBuffer;	
		}
		else
		{
			indexData->indexBuffer = m_pParentPage->m_IndexBuffer;		
		}
	}	

	indexData->indexCount  = GetTileIndexCount() * m_ParentRenderUnit->m_TileCount;	

#endif

}

//-----------------------------------------------------------------------
void COTETile::getRenderOperation( RenderOperation& op )
{
    op.useIndexes = true;
    op.operationType = false ?
        RenderOperation::OT_TRIANGLE_STRIP : RenderOperation::OT_TRIANGLE_LIST;

	m_RenderState = eRenderfinished;

#ifdef GAME_DEBUG1

    op.vertexData = m_Terrain;
    op.indexData  = m_IndexData;

#else

	op.vertexData = m_ParentRenderUnit->m_Terrain;	
	op.indexData  = m_ParentRenderUnit->m_IndexData;

	m_ParentRenderUnit->m_RenderState = eRenderfinished;

#endif	

}

// ----------------------------------------------------------
void COTETile::getWorldTransforms( Ogre::Matrix4* xform ) const
{
	Ogre::Matrix4 mm = m_pParentPage->s_pCreator->getRootSceneNode()->_getFullTransform();
	// mm.setTrans(m_Center);

	//*xform = mParentNode->_getFullTransform();
	*xform = mm;
}

// ----------------------------------------------------------
const Ogre::Quaternion& COTETile::getWorldOrientation(void) const
{
	return m_pParentPage->s_pCreator->getRootSceneNode()->_getDerivedOrientation();
	//return mParentNode->_getDerivedOrientation();
}

// ------------------------------------------------------------
inline int COTETile::_index( int x, int z ) const
{
	int verUnitSize = m_pParentPage->m_UnitSize + 1;

	int uX = x / m_pParentPage->m_UnitSize;
	int uZ = z / m_pParentPage->m_UnitSize;

	int TileUnitSize = (m_pParentPage->m_TileSize) / m_pParentPage->m_UnitSize;

	int xRemaind = x % m_pParentPage->m_UnitSize;
	int zRemaind = z % m_pParentPage->m_UnitSize;

	if(uX > TileUnitSize - 1)
	{
		uX = TileUnitSize - 1;
		xRemaind = m_pParentPage->m_UnitSize;
	}
	
	if(uZ > TileUnitSize - 1)	
	{
		uZ = TileUnitSize - 1;		
		zRemaind = m_pParentPage->m_UnitSize;
	}	
	

	int unitOffset = (uX  + uZ * ((m_pParentPage->m_TileSize) / m_pParentPage->m_UnitSize)) * 
													(m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
	int index = xRemaind + zRemaind * verUnitSize;

	return unitOffset + index;
}

// ----------------------------------------------------------
inline float COTETile::_vertex( int x, int z, int n )
{	
	return m_pPositionBuffer[_index(x, z) * 3 + n];	
}

// ----------------------------------------------------
inline bool COTETile::isInRect(const CRectA<float>& rRect)
{
    Vector3 start	= m_Bounds.getMinimum();
    Vector3 end		= m_Bounds.getMaximum();

	if( // tile �� rect��
		isInRect(start.x,start.z,rRect.sx,rRect.sz,rRect.ex,rRect.ez)   ||
		isInRect(end.x,start.z,rRect.sx,rRect.sz,rRect.ex,rRect.ez  )   ||
		isInRect(end.x,end.z,rRect.sx,rRect.sz,rRect.ex,rRect.ez    )   ||
		isInRect(start.x,end.z,rRect.sx,rRect.sz,rRect.ex,rRect.ez  )   ||
		// rect �� tile�� 
		isInRect(rRect.sx,rRect.sz,start.x,start.z,end.x,end.z)			||
		isInRect(rRect.ex,rRect.sz,start.x,start.z,end.x,end.z)			||
		isInRect(rRect.ex,rRect.ez,start.x,start.z,end.x,end.z)			||
		isInRect(rRect.sx,rRect.ez,start.x,start.z,end.x,end.z) 		  
		)
		return 1;
	else
		return 0;
	
}

// -----------------------------------------------------------------
inline void COTETile::GetPctIndex(float x, float z, int& x_index, int& z_index)
{
    Vector3 start	= m_Bounds.getMinimum();
    Vector3 end		= m_Bounds.getMaximum();

	//get this title's x,z index...	
	//x
    if(x<start.x)		
		x_index=0;
	else if(x>end.x)
        x_index=m_pParentPage->m_TileSize;
	else
	{
        float x_pct = ( x - start.x ) / ( end.x - start.x );
		float x_pt = x_pct * ( float ) ( m_pParentPage->m_TileSize );
		x_index = ( int ) x_pt;
	}	
	//z  
    if(z<start.z)		
		z_index=0;
	else if(z>end.z)
        z_index=m_pParentPage->m_TileSize;
	else
	{
		float z_pct = ( z - start.z ) / ( end.z - start.z );
		float z_pt = z_pct * ( float ) ( m_pParentPage->m_TileSize );
		z_index = ( int ) z_pt;
	}	       
}

// ----------------------------------------------------------
void COTETile::SetPosBufHeightAt(int x, int z, float y, float* buff)
{
	int verUnitSize = m_pParentPage->m_UnitSize + 1;

	int uX = x / m_pParentPage->m_UnitSize;
	int uZ = z / m_pParentPage->m_UnitSize;

	int xRemaind = x % m_pParentPage->m_UnitSize;
	int zRemaind = z % m_pParentPage->m_UnitSize;
	
	int TileUnitSize = (m_pParentPage->m_TileSize) / m_pParentPage->m_UnitSize;
	if(uX > TileUnitSize - 1)
	{
		uX = TileUnitSize - 1;
		xRemaind = m_pParentPage->m_UnitSize;
	}
	
	if(uZ > TileUnitSize - 1)	
	{
		uZ = TileUnitSize - 1;		
		zRemaind = m_pParentPage->m_UnitSize;
	}	
	
	// ����λ��

	int unitOffset = (uX  + uZ * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
	int totalOffset = xRemaind + zRemaind * verUnitSize + unitOffset;

	buff[totalOffset * 3 + 1] = y;

	// ----------------- �����ӱ�Ե -------------------- //
	
	if(xRemaind == 0 && zRemaind == 0) // ����
	{
		if(uX > 0 && uZ > 0)
		{
		
			unitOffset = (uX - 1  + (uZ - 1) * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
			int _xRemaind = m_pParentPage->m_UnitSize; int _zRemaind = m_pParentPage->m_UnitSize;
			totalOffset = _xRemaind + _zRemaind * verUnitSize + unitOffset;
			buff[totalOffset * 3 + 1] = y;
		}
	}	 
	{	
		// ��
		if(xRemaind == 0 && uX > 0) 
		{
			unitOffset = (uX - 1  + uZ * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
			int _xRemaind = m_pParentPage->m_UnitSize;
			totalOffset = _xRemaind + zRemaind * verUnitSize + unitOffset;
			buff[totalOffset * 3 + 1] = y;
		}	

		// ��
		if(zRemaind == 0 && uZ > 0)
		{
			unitOffset = (uX + (uZ - 1) * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
			int _zRemaind = m_pParentPage->m_UnitSize;
			totalOffset = xRemaind + _zRemaind * verUnitSize + unitOffset;
			buff[totalOffset * 3 + 1] = y;
		}

		// ��
		if(xRemaind == m_pParentPage->m_UnitSize && uX < TileUnitSize - 1)
		{	
			unitOffset = (uX + 1 + uZ * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
			int _xRemaind = 0;
			totalOffset = _xRemaind + zRemaind * verUnitSize + unitOffset;
			buff[totalOffset * 3 + 1] = y;
		}

		// ��
		if(zRemaind == m_pParentPage->m_UnitSize && uZ < TileUnitSize - 1)
		{	
			unitOffset = (uX + (uZ + 1) * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
			int _zRemaind = 0;
			totalOffset = xRemaind + _zRemaind * verUnitSize + unitOffset;
			buff[totalOffset * 3 + 1] = y;
		}
	}
}

// ----------------------------------------------------
float COTETile::GetHeightAt( float x, float z )
{
    Vector3 start	= m_Bounds.getMinimum();
    Vector3 end		= m_Bounds.getMaximum();
  

    if ( x < start.x )
    {
        if ( m_Neighbors[ WEST ] != 0 )
            return m_Neighbors[ WEST ] ->GetHeightAt( x, z );
        else
            x = start.x;
    }

    if ( x > end.x )
    {
        if ( m_Neighbors[ EAST ] != 0 )
            return m_Neighbors[ EAST ] ->GetHeightAt( x, z );
        else
            x = end.x;
    }

    if ( z < start.z )
    {
        if ( m_Neighbors[ NORTH ] != 0 )
            return m_Neighbors[ NORTH ] ->GetHeightAt( x, z );
        else
            z = start.z;
    }

    if ( z > end.z )
    {
        if ( m_Neighbors[ SOUTH ] != 0 )
            return m_Neighbors[ SOUTH ] ->GetHeightAt( x, z );
        else
            z = end.z;
    }



    float x_pct = ( x - start.x ) / ( end.x - start.x );
    float z_pct = ( z - start.z ) / ( end.z - start.z );

    float x_pt = x_pct * ( float ) ( m_pParentPage->m_TileSize );
    float z_pt = z_pct * ( float ) ( m_pParentPage->m_TileSize );
	
	// ��������

    int x_index = ( int ) x_pt;
    int z_index = ( int ) z_pt;


    // If we got to the far right / bottom edge, move one back
    if (x_index == m_pParentPage->m_TileSize)
    {
        --x_index;
        x_pct = 1.0f;
    }
    else
    {
        // get remainder
        x_pct = x_pt - ( int ) x_pt;
    }
    if (z_index == m_pParentPage->m_TileSize)
    {
        --z_index;
        z_pct = 1.0f;
    }
    else
    {
        z_pct = z_pt - ( int ) z_pt;
    }

    //bilinear interpolate to find the height.

    float t1 = _vertex( x_index, z_index, 1 );
    float t2 = _vertex( x_index + 1, z_index, 1 );
    float b1 = _vertex( x_index, z_index + 1, 1 );
    float b2 = _vertex( x_index + 1, z_index + 1, 1 );

    float midpoint = (b1 + t2) / 2.0;

    if (x_pct + z_pct <= 1) {
        b2 = midpoint + (midpoint - t1);
    } else {
        t1 = midpoint + (midpoint - b2);
    }

    float t = ( t1 * ( 1 - x_pct ) ) + ( t2 * ( x_pct ) );
    float b = ( b1 * ( 1 - x_pct ) ) + ( b2 * ( x_pct ) );

    float h = ( t * ( 1 - z_pct ) ) + ( b * ( z_pct ) );

    return h;
}

// ----------------------------------------------------
void COTETile::GetNormalAt(float x, float z, Vector3* result)
{
	int ix, iz;

	GetPctIndex(x, z, ix, iz);	

	// ����Ǹ�ƽ��ֵ
	*result = m_pNormalBuffer[_index(ix, iz)];
	//*result = (m_pNormalBuffer[_index(ix, iz)] + m_pNormalBuffer[_index(ix + 1, iz)] + m_pNormalBuffer[_index(ix, iz + 1)]) / 3.0f;
}

// ----------------------------------------------------
void COTETile::_GetNormalAt(float x, float z, Vector3 * result)
{   
	//if(!m_NormalUpdated)
	{
		float unit = COTETilePage::s_CurrentPage->m_Scale.x;

		Vector3 here, left, down;
		here.x = x;
		here.y = GetHeightAt( x, z );
		here.z = z;

		left.x = x - unit;
		left.y = GetHeightAt( x - unit, z );
		left.z = z;

		down.x = x;
		down.y = GetHeightAt( x, z + unit );
		down.z = z + unit;

		left = left - here;

		down = down - here;

		left.normalise();
		down.normalise();

		*result = left.crossProduct( down );
		result -> normalise(); 	
		
		// ���·���

		int ix, iz;
		GetPctIndex(x, z, ix, iz);
		m_pNormalBuffer[_index(ix, iz)] = *result;

		m_NormalUpdated = true;
	}
	//else
	//{
	//	// ֱ�ӻ�ȡ

	//	GetNormalAt(x, z, result);
	//}
}

// --------------------------------------------------------------
const Vector3& COTETile::GetAveNormal()
{
	Vector3 start	= m_Bounds.getMinimum();
    Vector3 end		= m_Bounds.getMaximum();

	int tse = m_pParentPage->m_TileSize;

	Vector3 lt(start.x, _vertex(0, 0, 1), start.z);
		
	Vector3 rt(end.x, _vertex(tse, 0, 1), start.z);

	Vector3 lb(start.x, _vertex(0, tse, 1), end.z);

	Vector3 rb(end.x, _vertex(tse, tse, 1), end.z);
	
	Vector3 n1 = (lb - lt).crossProduct(rb - lt);
	Vector3 n2 = (rb - lt).crossProduct(rt - lt);

	static Vector3 sNormal;
	sNormal = ((n1 + n2) * 0.5f).normalisedCopy();

	return sNormal;
}

//-----------------------------------------------------------------------
bool COTETile::intersectSegment( const Vector3 & start, const Vector3 & end, Vector3 * result )
{
	float step = 0.1f;

    Vector3 dir = end - start;
    Vector3 ray = start;

    //special case...
    if ( dir.x == 0 && dir.z == 0 )
    {
        if ( ray.y <= GetHeightAt( ray.x, ray.z ) )
        {
            if ( result != 0 )
                * result = start;

            return true;
        }
    }

    dir.normalise();

    //dir.x *= mScale.x;
    //dir.y *= mScale.y;
    //dir.z *= mScale.z;

    const Vector3 * corners = getBoundingBox().getAllCorners();

    //start with the next one...
    ray += dir*step;


    while ( ! ( ( ray.x < corners[ 0 ].x ) ||
        ( ray.x > corners[ 4 ].x ) ||
        ( ray.z < corners[ 0 ].z ) ||
        ( ray.z > corners[ 4 ].z ) ) )
    {


        float h = GetHeightAt( ray.x, ray.z );

        if ( ray.y <= h )
        {
            if ( result != 0 )
                * result = ray;

            return true;
        }

        else
        {
            ray += dir*step;
        }

    }

    if ( ray.x < corners[ 0 ].x && m_Neighbors[ WEST ] != 0 )
        return m_Neighbors[ WEST ] ->intersectSegment( ray, end, result );
    else if ( ray.z < corners[ 0 ].z && m_Neighbors[ NORTH ] != 0 )
        return m_Neighbors[ NORTH ] ->intersectSegment( ray, end, result );
    else if ( ray.x > corners[ 4 ].x && m_Neighbors[ EAST ] != 0 )
        return m_Neighbors[ EAST ] ->intersectSegment( ray, end, result );
    else if ( ray.z > corners[ 4 ].z && m_Neighbors[ SOUTH ] != 0 )
        return m_Neighbors[ SOUTH ] ->intersectSegment( ray, end, result );
    else
    {
        if ( result != 0 )
            * result = Vector3( -1, -1, -1 );

        return false;
    }
}

//----------------------------------------------------------------------------------
inline void COTETile::SetVertexHgtBufferAt(const VertexElement* poselem, unsigned char* pBase0, int totalOffset)
{	
	int verOffset = totalOffset;
	float *pPos, *pSysPos;
	unsigned char* pBase = pBase0 + verOffset * m_pParentPage->m_MainBuffer->getVertexSize();
	poselem->baseVertexPointerToElement(pBase, &pPos);	
	pSysPos= m_pPositionBuffer + verOffset * 3 + 1; 

	*pSysPos=(*pSysPos<m_pParentPage->m_Scale.y)?(*pSysPos):m_pParentPage->m_Scale.y;	
	*pSysPos=(*pSysPos>0.0f)?(*pSysPos):0.0f; pPos++; 
	*pPos = *pSysPos; 

}

//----------------------------------------------------------------------------------
void COTETile::UpdateVertexHgtbuffer(int x, int z, const VertexElement* poselem, unsigned char* pBase0)		
{
	int verUnitSize = m_pParentPage->m_UnitSize + 1;	

	int uX = x / m_pParentPage->m_UnitSize;	
	int uZ = z / m_pParentPage->m_UnitSize;	

	int xRemaind = x % m_pParentPage->m_UnitSize;	
	int zRemaind = z % m_pParentPage->m_UnitSize;	
	
	int TileUnitSize = (m_pParentPage->m_TileSize) / m_pParentPage->m_UnitSize;		

	if(uX > TileUnitSize - 1)	
	{	
		uX = TileUnitSize - 1;	
		xRemaind = m_pParentPage->m_UnitSize;	
	}	
	
	if(uZ > TileUnitSize - 1)	
	{	
		uZ = TileUnitSize - 1;	
		zRemaind = m_pParentPage->m_UnitSize;	
	}	
	
	// ����λ��	

	int unitOffset = (uX  + uZ * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);	
	int totalOffset = xRemaind + zRemaind * verUnitSize + unitOffset;	

	SetVertexHgtBufferAt(poselem, pBase0, totalOffset);

	// ----------------- �����ӱ�Ե -------------------- //
	
	if(xRemaind == 0 && zRemaind == 0)	 // ����	
	{	
		if(uX > 0 && uZ > 0)	
		{	
			unitOffset = (uX - 1  + (uZ - 1) * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);	
			int _xRemaind = m_pParentPage->m_UnitSize; int _zRemaind = m_pParentPage->m_UnitSize;	
			totalOffset = _xRemaind + _zRemaind * verUnitSize + unitOffset;	
			SetVertexHgtBufferAt(poselem, pBase0, totalOffset);	
		}	
	}		
	{	
		// ��
		if(xRemaind == 0 && uX > 0) 
		{	
			unitOffset = (uX - 1  + uZ * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);	
			int _xRemaind = m_pParentPage->m_UnitSize;	
			totalOffset = _xRemaind + zRemaind * verUnitSize + unitOffset;	
			SetVertexHgtBufferAt(poselem, pBase0, totalOffset);
		}	

		// ��
		if(zRemaind == 0 && uZ > 0)	
		{	
			unitOffset = (uX + (uZ - 1) * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);	
			int _zRemaind = m_pParentPage->m_UnitSize;	
			totalOffset = xRemaind + _zRemaind * verUnitSize + unitOffset;	
			SetVertexHgtBufferAt(poselem, pBase0, totalOffset);
		}	

		// ��
		if(xRemaind == m_pParentPage->m_UnitSize && uX < TileUnitSize - 1)	
		{	
			unitOffset = (uX + 1 + uZ * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);	
			int _xRemaind = 0;	
			totalOffset = _xRemaind + zRemaind * verUnitSize + unitOffset;	
			SetVertexHgtBufferAt(poselem, pBase0, totalOffset);	
		}	

		// ��
		if(zRemaind == m_pParentPage->m_UnitSize && uZ < TileUnitSize - 1)	
		{	
			unitOffset = (uX + (uZ + 1) * TileUnitSize) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);	
			int _zRemaind = 0;	
			totalOffset = xRemaind + _zRemaind * verUnitSize + unitOffset;	
			SetVertexHgtBufferAt(poselem, pBase0, totalOffset);
		}	
	}	
}

// --------------------------------------------------------------------
void COTETile::ManualSetVertexbufferHgtAt(int x, int z, float y)
{
	SetPosBufHeightAt(x, z, y, m_pPositionBuffer);

	Real min = 255000.0f, max = 0.0f;
	VertexDeclaration* decl = m_Terrain->vertexDeclaration;
    VertexBufferBinding* bind = m_Terrain->vertexBufferBinding;        
    
    const VertexElement* poselem = decl->findElementBySemantic(VES_POSITION);  

	int bufSize = m_pParentPage->m_MainBuffer->getVertexSize() * GetTileVertexCount();
    unsigned char* pBase0 = static_cast<unsigned char*>(m_pParentPage->m_MainBuffer->lock(
									m_IndexInPage * bufSize, bufSize,HardwareBuffer::HBL_DISCARD));  

	UpdateVertexHgtbuffer(x, z, poselem, pBase0);	

    m_pParentPage->m_MainBuffer->unlock();    
}

// --------------------------------------------------------------------
void COTETile::UpdateVertexHgtbuffer(int x, int z)
{
	Real min = 255000, max = 0;
	VertexDeclaration* decl = m_Terrain->vertexDeclaration;
    VertexBufferBinding* bind = m_Terrain->vertexBufferBinding;        
    
    const VertexElement* poselem = decl->findElementBySemantic(VES_POSITION);  

	int bufSize = m_pParentPage->m_MainBuffer->getVertexSize() * GetTileVertexCount();
    unsigned char* pBase0 = static_cast<unsigned char*>(m_pParentPage->m_MainBuffer->lock(
									m_IndexInPage * bufSize, bufSize,HardwareBuffer::HBL_DISCARD));


	UpdateVertexHgtbuffer(x, z, poselem, pBase0);	

    m_pParentPage->m_MainBuffer->unlock();    
}

// --------------------------------------------------------------------
void COTETile::UpdateVertexHgtbuffer(const CRectA<int>& rect)
{		
	Real min = 255000, max = 0;
	VertexDeclaration* decl = m_Terrain->vertexDeclaration;
    VertexBufferBinding* bind = m_Terrain->vertexBufferBinding;        
    
    const VertexElement* poselem = decl->findElementBySemantic(VES_POSITION);  
  
	int bufSize = m_pParentPage->m_MainBuffer->getVertexSize() * GetTileVertexCount();
    unsigned char* pBase0 = static_cast<unsigned char*>(m_pParentPage->m_MainBuffer->lock(
									m_IndexInPage * bufSize, bufSize,HardwareBuffer::HBL_DISCARD));

	for ( int j = rect.sz; j < rect.ez; j++ )
	{
		for ( int i = rect.sx; i < rect.ex; i++ )
		{			
			UpdateVertexHgtbuffer(i, j, poselem, pBase0);
		}
	}	

    m_pParentPage->m_MainBuffer->unlock();       

}

// ----------------------------------------------------
// �������
void COTETile::ExportData(int startx, int startz, TileCreateData_t* pData)
{
	COTETilePage* page = m_pParentPage;
	float* pSysPos = m_pPositionBuffer;
	Ogre::Vector3* pSysNormal = m_pNormalBuffer;

	int endx = startx + page->m_TileSize + 1;
    int endz = startz + page->m_TileSize + 1;

	int startUX = startx / m_pParentPage->m_UnitSize;
	int startUZ = startz / m_pParentPage->m_UnitSize;
	
	int j = startz;
    for ( int uZ = 0; uZ < (endz - startz - 1) / m_pParentPage->m_UnitSize; uZ ++ )
    {
		int i = startx;
        for ( int uX = 0; uX < (endx - startx - 1) / m_pParentPage->m_UnitSize; uX ++ )
        {     
			//if(m_pTerrainUnitMap)
			//{
				//CTerrainUnit* tu = (CTerrainUnit*)m_pTerrainUnitMap->m_Tiles[uX][uZ];			
				
				//int uvInfOffset = uX + startUX + (uZ + startUZ) * ( (page->m_PageSize - 1) / m_pParentPage->m_UnitSize);
				//CTerrainUnitData* pUnitData = pData->uvData + uvInfOffset;
				
				//for(int ii = 0; ii < TEXTURE_LAYER_COUNT; ii ++)	//TEXTURE_LAYER_COUNT��,
				//{
				//	pUnitData->m_LayerUIndex[ii] = tu->m_LayerUIndex[ii]; // ��������	
				//	pUnitData->m_LayerVIndex[ii] = tu->m_LayerVIndex[ii]; // ��������	
				//}
			//}			

			// һ����Ԫ		
			
			for(int jj = 0; jj < m_pParentPage->m_UnitSize + 1; jj ++)
			for(int ii = 0; ii < m_pParentPage->m_UnitSize + 1; ii ++)
			{			
				*pSysPos++;			// x
				Real height = *pSysPos++;// y
				*pSysPos++;			// z	

				float scaleHeight = (height / page->m_Scale.y * 255.0);
				int iscaleHeight = int(scaleHeight + 0.5f);

				//pData->heightData[(j + jj) * page->m_PageSize + (i + ii)] = (height / page->m_Scale.y) * 255;
				if(pData->heightData)
				pData->heightData[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)] = iscaleHeight;
				if(pData->normal)
				pData->normal[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)] = *pSysNormal++;
				if(pData->fHeightData)
					pData->fHeightData[(j + jj) * page->m_PageSizeX + (i + ii)] = height / page->m_Scale.y;
			}
   
			i += m_pParentPage->m_UnitSize;
        }

		j += m_pParentPage->m_UnitSize;
    }

}

// ----------------------------------------------------
//���㷨��
void COTETile::ClearVertexNormal()
{
	Ogre::Vector3* pSysNormal = m_pNormalBuffer;
	const VertexElement* poselem = m_Terrain->vertexDeclaration->findElementBySemantic(VES_NORMAL);  
  
	int bufSize = m_pParentPage->m_MainBuffer->getVertexSize() * GetTileVertexCount();
    unsigned char* pBase0 = static_cast<unsigned char*>(m_pParentPage->m_MainBuffer->lock(
									m_IndexInPage * bufSize, bufSize,HardwareBuffer::HBL_DISCARD));
	
	int endx = m_pParentPage->m_TileSize + 1;
	int endz = m_pParentPage->m_TileSize + 1;

	int oldX = 0, oldZ = 0;

	for(int jj = 0; jj < (endz - oldZ - 1) / m_pParentPage->m_UnitSize; jj++)
	{	
		for(int ii = 0; ii < (endx - oldX - 1) / m_pParentPage->m_UnitSize; ii++)
		{
			for(int jjj = 0; jjj < (m_pParentPage->m_UnitSize + 1); jjj++)
			{
				for(int iii = 0; iii < (m_pParentPage->m_UnitSize + 1); iii++)
				{
					//������ƽ��������
					
					Ogre::Vector3 v(0, 0, 0);
					Ogre::Vector3 left, down, result;
					float *pNormal;
					poselem->baseVertexPointerToElement(pBase0, &pNormal);					
					
					(*pSysNormal).x = *pNormal++ = 0.0f;
					(*pSysNormal).y = *pNormal++ = 1.0f;
					(*pSysNormal).z = *pNormal++ = 0.0f;
					pSysNormal++;
	
					pBase0 += m_pParentPage->m_MainBuffer->getVertexSize();
				}
			}			
		}	
	}

	m_pParentPage->m_MainBuffer->unlock(); 
}

// ----------------------------------------------------
//ƽ�����㷨��
void COTETile::AverageVertexNormal()
{
#ifndef GAME_DEBUG1
	return;
#endif 

	int x = this->_vertex(0, 0, 0) / m_pParentPage->m_Scale.x;
	int z = this->_vertex(0, 0, 2) / m_pParentPage->m_Scale.z;

	Ogre::Vector3* pSysNormal = m_pNormalBuffer;
	const VertexElement* poselem = m_Terrain->vertexDeclaration->findElementBySemantic(VES_NORMAL);  
  
	int bufSize = m_pParentPage->m_MainBuffer->getVertexSize() * GetTileVertexCount();
    unsigned char* pBase0 = static_cast<unsigned char*>(m_pParentPage->m_MainBuffer->lock(
									m_IndexInPage * bufSize, bufSize,HardwareBuffer::HBL_DISCARD));
	
	int endx = x + m_pParentPage->m_TileSize + 1;
	int endz = z + m_pParentPage->m_TileSize + 1;

	int oldX = x, oldZ = z;

	for(int jj = 0; jj < (endz - oldZ - 1) / m_pParentPage->m_UnitSize; jj++)
	{
		x = oldX;
		for(int ii = 0; ii < (endx - oldX - 1) / m_pParentPage->m_UnitSize; ii++)
		{
			for(int jjj = 0; jjj < (m_pParentPage->m_UnitSize + 1); jjj++)
			{
				for(int iii = 0; iii < (m_pParentPage->m_UnitSize + 1); iii++)
				{
					//������ƽ��������
					
					Ogre::Vector3 v(0, 0, 0);
					Ogre::Vector3 left, down, result;
					float *pNormal;
					poselem->baseVertexPointerToElement(pBase0, &pNormal);	

					Ogre::Vector3 vNormal[6] = {Ogre::Vector3(0.0f, 0.0f, 0.0f)};	//���������ķ���
					Ogre::Vector3 vector[7] = {Ogre::Vector3(0.0f, 0.0f, 0.0f)};	//������ڼ��������η��ߵĶ���
					int ix, iz;
					ix = x + iii;
					iz = z + jjj;

					//���Ķ���
					vector[0].x = (float)ix * m_pParentPage->m_Scale.x;
					vector[0].z = (float)iz * m_pParentPage->m_Scale.z;
					vector[0].y = GetHeightAt(vector[0].x, vector[0].z);

					//���涥��
					vector[1].x = (float)(ix) * m_pParentPage->m_Scale.x;
					vector[1].z = (float)(iz - 1) * m_pParentPage->m_Scale.z;
					vector[1].y = GetHeightAt(vector[1].x, vector[1].z);

					//���涥��
					vector[2].x = (float)(ix - 1) * m_pParentPage->m_Scale.x;	
					vector[2].z = (float)(iz) * m_pParentPage->m_Scale.z;
					vector[2].y = GetHeightAt(vector[2].x, vector[2].z);

					//�����涥��
					vector[3].x = (float)(ix - 1) * m_pParentPage->m_Scale.x;
					vector[3].z = (float)(iz + 1) * m_pParentPage->m_Scale.z;
					vector[3].y = GetHeightAt(vector[3].x, vector[3].z);

					//���涥��
					vector[4].x = (float)(ix) * m_pParentPage->m_Scale.x;
					vector[4].z = (float)(iz + 1) * m_pParentPage->m_Scale.z;
					vector[4].y = GetHeightAt(vector[4].x, vector[4].z);

					//���涥��
					vector[5].x = (float)(ix + 1) * m_pParentPage->m_Scale.x;
					vector[5].z = (float)(iz) * m_pParentPage->m_Scale.z;
					vector[5].y = GetHeightAt(vector[5].x, vector[5].z);

					//�����涥��
					vector[6].x = (float)(ix + 1) * m_pParentPage->m_Scale.x;
					vector[6].z = (float)(iz - 1) * m_pParentPage->m_Scale.z;
					vector[6].y = GetHeightAt(vector[6].x, vector[6].z);
					

					//�����������εķ���
					float y = 0;
					for(int k = 0; k < 6; k ++)
					{
						left = vector[k + 1] - vector[0];									

						if(k == 5)
							down = vector[1] - vector[0];
						else
							down = vector[k + 2] - vector[0];

						left.normalise();
						down.normalise();

						y += (-left.y + 1.0f) / 2.0f;

						vNormal[k] = left.crossProduct( down );
						vNormal[k].normalise();

						v += vNormal[k];						
					}

					v = v / 6;
					v.normalise();

					// ���߷���		
					
					v *= y / 3.0f;

					//�����������εķ���end
					
					(*pSysNormal).x = *pNormal++ = v.x;
					(*pSysNormal).y = *pNormal++ = v.y;
					(*pSysNormal).z = *pNormal++ = v.z;
					pSysNormal++;
	
					pBase0 += m_pParentPage->m_MainBuffer->getVertexSize();
				}
			}
			x += m_pParentPage->m_UnitSize;
		}
		z += m_pParentPage->m_UnitSize;
	}

	m_pParentPage->m_MainBuffer->unlock(); 
}

//-----------------------------------------------------------------------
const Ogre::LightList& COTETile::getLights(void) const
{
	static Ogre::LightList s_LL;
	return s_LL;
	//return COTETilePage::s_pCreator->getRootSceneNode()->findLights(this->getBoundingRadius());
}

// ----------------------------------------------------
// ������ͼ

bool COTETile::SetMaterialTexture(const std::string& fileName, int layer)
{		
	Pass* m_pass = m_Material->getTechnique(0)->getPass(0);
	
	std::string texNames[TEXTURE_LAYER_COUNT];

	GetTextureNames(texNames);

	texNames[layer] = fileName;
	
	std::string matName;
	for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++) 
		matName += texNames[i];
		
	MaterialPtr mp = MaterialManager::getSingleton().getByName(matName);
	
	if(m_Material == mp) 
	{		
		return false;
	}

	m_Material = mp;

	if(m_Material.isNull())
	{
		m_Material = MaterialManager::getSingleton().create(matName, 
					ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);
		
		SetupMaterial(texNames);

		m_UseSharedMaterial = true;
	}

	return true;	
}

// -----------------------------------------------------------------
// ����ͼ
void COTETile::SetLightMap(const std::string& lightmapName, int layer)
{	
	if(m_UseSharedMaterial) // ����tile����Ĳ���
		return;

	Pass* pass = m_Material->getTechnique(0)->getPass(0);

	if(!m_LightmapUnit)
	{
		m_LightmapUnit = pass->createTextureUnitState(lightmapName, layer);		
	}
	else
	{		
		m_LightmapUnit->setTextureName(lightmapName);
		m_LightmapUnit->setTextureCoordSet(layer);
	}
	m_LightmapUnit->setColourOperationEx(LBX_MODULATE_X2, LBS_TEXTURE, LBS_CURRENT);	
}

// -----------------------------------------------------------------
// Alphaͼ
void COTETile::AddAlphaMap(const std::string& alphaName, int layer)
{	
	Pass* pass = m_Material->getTechnique(0)->getPass(0);
	Ogre::TextureUnitState* t = pass->createTextureUnitState(alphaName, layer);	
	t->setColourOperationEx(LBX_MODULATE, LBS_TEXTURE, LBS_CURRENT);		
}

// -----------------------------------------------------------------
bool COTETile::GetTextureNames(std::string texNames[TEXTURE_LAYER_COUNT])
{
	Pass* pass = m_Material->getTechnique(0)->getPass(0);

	for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
	{
		TextureUnitState* t = pass->getTextureUnitState(i);
		if(!t)
			return false;

		texNames[i] = t->getTextureName();			
	}

	return true;

}

/*******************************************************************
 *******************************************************************
	
	�������õر����� ����2���汾���༭���汾���ͻ��˰汾),ÿ���汾
	����������Ⱦ��ʽ��shader/�̶����ߣ�

 ********************************************************************
 ********************************************************************/
void COTETile::SetupMaterial(std::string texNames[])
{
bool isShaderEnabled = true;

#ifdef GAME_DEBUG1	 // �༭���汾

	isShaderEnabled = true;

	Pass* pass = m_Material->getTechnique(0)->getPass(0);
	
	if(isShaderEnabled) // shader�汾
	{
		for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
		{
			pass->createTextureUnitState(texNames[i], 0);
			m_TexNames[i] = texNames[i];
		}
		
		// Alpha

		AddAlphaMap(m_pParentPage->m_AlphamapName, 1);

		// ����

		SetLightMap(m_pParentPage->m_LightmapName, 1);	

		// Gpu program
	
		if(TEXTURE_LAYER_COUNT == 4)
		{
			GpuProgramPtr prog = GpuProgramManager::getSingleton().getByName("Terrain_PS_4.hlsl");
			if (prog.isNull()) 
			{		
				prog = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
					"Terrain_PS_4.hlsl", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					"hlsl", GPT_FRAGMENT_PROGRAM);
			}
			pass->setFragmentProgram(prog->getName());
		}

		if(TEXTURE_LAYER_COUNT == 3)
		{
			GpuProgramPtr prog = GpuProgramManager::getSingleton().getByName("Terrain_PS_3.hlsl");
			if (prog.isNull()) 
			{		
				prog = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
					"Terrain_PS_3.hlsl", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					"hlsl", GPT_FRAGMENT_PROGRAM);
			}

			pass->setFragmentProgram(prog->getName());	
		}
	}
	else
	{	
		// �̶����߰汾

		if(m_pParentPage->m_AlphaTextures[0].isNull())
		{
			// alpha���ݶ����ڴ�

			if(!m_pParentPage->m_AlphaBmpdata)
			{
				if(!m_pParentPage->LoadAlphamapData())
				{
					OTE_MSG("����alphaͼʧ�ܣ�", "ʧ��")
					throw;
				}
			}

			int alphamap_size = m_pParentPage->m_Alphabmih.biWidth;			

			unsigned char* pRawData[3];
			pRawData[0] = new unsigned char[alphamap_size * alphamap_size]; // alphamap �̶�Ϊ 512
			pRawData[1] = new unsigned char[alphamap_size * alphamap_size];
			pRawData[2] = new unsigned char[alphamap_size * alphamap_size];

			m_pParentPage->GetAlphamapChannels(pRawData);		
	
			for(int i = 0; i < ALPHA_LAYER_COUNT; i ++)
			{
				std::stringstream ss;
				ss << m_pParentPage->m_AlphamapName << i;
				
				m_pParentPage->m_AlphaTextures[i] = 
					Ogre::TextureManager::getSingleton().createManual(
						ss.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						TEX_TYPE_2D, alphamap_size, alphamap_size, 1, -1, PF_A8, TU_DEFAULT);

				Ogre::Image img;
				img.loadDynamicImage(pRawData[i], alphamap_size, alphamap_size, PF_A8);

				m_pParentPage->m_AlphaTextures[i]->loadImage(img);
			}

			delete [] pRawData[0];
			delete [] pRawData[1];
			delete [] pRawData[2];
		}				

		for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
		{				
			if(i == 0)
			{
				Ogre::TextureUnitState* tu = pass->createTextureUnitState(texNames[i], 0);
				tu->setColourOperationEx(LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);						
			}
			else
			{						
				// ����alpha��				

				Ogre::TextureUnitState* tu = pass->createTextureUnitState(
									m_pParentPage->m_AlphaTextures[i - 1]->getName(), 1);
				
				tu->setColourOperationEx(LBX_SOURCE2,LBS_TEXTURE,LBS_CURRENT);
				tu->setAlphaOperation(LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);										

				// ��ͼ

				tu = pass->createTextureUnitState(texNames[i], 0);										
				tu->setAlphaOperation(LBX_SOURCE1, LBS_CURRENT, LBS_CURRENT);	
				tu->setColourOperationEx(LBX_BLEND_CURRENT_ALPHA, LBS_TEXTURE, LBS_CURRENT);

			}					
		}

		
		// ����

		SetLightMap(m_pParentPage->m_LightmapName, 1);	
	}

#else		// �ͻ��˷����汾

	std::string matName;
	int layerCnt = 0;
	for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
	{	
		if(texNames[i] != DEFAULT_TEXTURE) // �����Ĭ����ͼ�򲻴���
		{
			i == 1 ? m_AlphaMapping.x = 1.0f : NULL;
			i == 2 ? m_AlphaMapping.y = 1.0f : NULL;
			i == 3 ? m_AlphaMapping.z = 1.0f : NULL;	

			matName += m_TexNames[layerCnt ++] = texNames[i];
		}		
	}

	m_Material = MaterialManager::getSingleton().getByName(matName);

	if(m_Material.isNull())
	{
		m_UseSharedMaterial = false;

		m_Material = MaterialManager::getSingleton().create(matName, 
					ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);
	
		Pass* pass = m_Material->getTechnique(0)->getPass(0);
		
		if(isShaderEnabled) // shader�汾
		{
			for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
			{	
				if(texNames[i] != DEFAULT_TEXTURE) // �����Ĭ����ͼ�򲻴���
				{
					Ogre::TextureUnitState* tu = pass->createTextureUnitState(texNames[i], 0);	
				}		
			}	

			// Alpha

			if(layerCnt > 1)
			{
				AddAlphaMap(m_pParentPage->m_AlphamapName, 1);
			}

			// Gpu program

			std::stringstream ss;
			
			ss << "Terrain_PS_" << layerCnt << ".hlsl";	

			GpuProgramPtr prog = GpuProgramManager::getSingleton().getByName(ss.str());
			if (prog.isNull()) 
			{		
				prog = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
					ss.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					"hlsl", GPT_FRAGMENT_PROGRAM);
			}
			
			pass->setFragmentProgram(prog->getName());	

			if(layerCnt >= 2 && layerCnt < 4)
			{
				Ogre::GpuProgramParametersSharedPtr gppPtr = pass->getFragmentProgramParameters(); 			

				gppPtr->setNamedAutoConstant("AlphaMapping", Ogre::GpuProgramParameters::AutoConstantType::ACT_CUSTOM, TILE_CG_BASE_ID);
		
			}
		}
		else
		{
			// �̶�����

			if(m_pParentPage->m_AlphaTextures[0].isNull())
			{
				if(!m_pParentPage->m_AlphaBmpdata)
				{
					if(!m_pParentPage->LoadAlphamapData())
					{
						OTE_MSG("����alphaͼʧ�ܣ�", "ʧ��")
						throw;
					}
				}

				int alphamap_size = m_pParentPage->m_Alphabmih.biWidth;			

				unsigned char* pRawData[3];
				pRawData[0] = new unsigned char[alphamap_size * alphamap_size]; // alphamap �̶�Ϊ 512
				pRawData[1] = new unsigned char[alphamap_size * alphamap_size];
				pRawData[2] = new unsigned char[alphamap_size * alphamap_size];

				m_pParentPage->GetAlphamapChannels(pRawData);		
		
				for(int i = 0; i < ALPHA_LAYER_COUNT; i ++)
				{
					std::stringstream ss;
					ss << m_pParentPage->m_AlphamapName << i;
					
					m_pParentPage->m_AlphaTextures[i] = Ogre::TextureManager::getSingleton().getByName(ss.str());
					
					if(m_pParentPage->m_AlphaTextures[i].isNull())
					{
						m_pParentPage->m_AlphaTextures[i] = Ogre::TextureManager::getSingleton().createManual(
							ss.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
							TEX_TYPE_2D, alphamap_size, alphamap_size, 1, -1, PF_A8, TU_DEFAULT);
					}

					Ogre::Image img;
					img.loadDynamicImage(pRawData[i], alphamap_size, alphamap_size, PF_A8);

					m_pParentPage->m_AlphaTextures[i]->loadImage(img);
				}

				delete []pRawData[0];
				delete []pRawData[1];
				delete []pRawData[2];
			}				

			for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
			{	
				if(texNames[i] != DEFAULT_TEXTURE) // �����Ĭ����ͼ�򲻴���
				{
					if(i == 0)
					{
						Ogre::TextureUnitState* tu = pass->createTextureUnitState(texNames[i], 0);
						tu->setColourOperationEx(LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);						
					}
					else
					{						
						// ����alpha��				

						Ogre::TextureUnitState* tu = pass->createTextureUnitState(
											m_pParentPage->m_AlphaTextures[i - 1]->getName(), 1);									
						
						tu->setColourOperationEx(LBX_SOURCE2,LBS_TEXTURE,LBS_CURRENT);	
						tu->setAlphaOperation(LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);							

						// ��ͼ

						tu = pass->createTextureUnitState(texNames[i], 0);							
						tu->setAlphaOperation(LBX_SOURCE1, LBS_CURRENT, LBS_CURRENT);
						
						tu->setColourOperationEx(LBX_BLEND_CURRENT_ALPHA, LBS_TEXTURE, LBS_CURRENT);					
					}					
				}			
			}	
		}

		// ����

		SetLightMap(m_pParentPage->m_LightmapName, 1);	
	}

#endif	
}

//-----------------------------------------------------------------------
// ����cg�������
void COTETile::_updateCustomGpuParameter( const GpuProgramParameters::AutoConstantEntry& constantEntry, GpuProgramParameters* params) const
{		
	if( TILE_CG_BASE_ID == constantEntry.data ) // ʵ��֮��ʱ����λ��
	{
		params->setNamedConstant("AlphaMapping", m_AlphaMapping);		
	}
	Renderable::_updateCustomGpuParameter(constantEntry, params);
}

// -----------------------------------------------------------------
inline int COTETile::GetTileVertexCount()
{
	int size = (m_pParentPage->m_TileSize + 1 + (m_pParentPage->m_TileSize) / m_pParentPage->m_UnitSize - 1);
	return size * size;
}	

// -----------------------------------------------------------------
inline int COTETile::GetTileIndexCount()
{
#ifdef GAME_DEBUG1	

	if		(m_IndexData && m_IndexData->indexBuffer == m_pParentPage->m_LodLevel[0].IndexBuffer)
		return m_pParentPage->m_LodLevel[0].IndexCount;
	
	else if	(m_IndexData && m_IndexData->indexBuffer == m_pParentPage->m_LodLevel[1].IndexBuffer)
		return m_pParentPage->m_LodLevel[1].IndexCount;
	
	else
		return (m_pParentPage->m_TileSize) * (m_pParentPage->m_TileSize) * 2 * 3;
#else

	if		(m_ParentRenderUnit && m_ParentRenderUnit->m_IndexData && m_ParentRenderUnit->m_IndexData->indexBuffer == m_pParentPage->m_LodLevel[0].IndexBuffer)
		return m_pParentPage->m_LodLevel[0].IndexCount;
	
	else if	(m_ParentRenderUnit && m_ParentRenderUnit->m_IndexData && m_ParentRenderUnit->m_IndexData->indexBuffer == m_pParentPage->m_LodLevel[1].IndexBuffer)
		return m_pParentPage->m_LodLevel[1].IndexCount;	

	else
	{
		return (m_pParentPage->m_TileSize) * COTETilePage::c_PatchTileSize * (m_pParentPage->m_TileSize) * COTETilePage::c_PatchTileSize * 2 * 3;
	}
	//else
	//	return (m_pParentPage->m_TileSize) * (m_pParentPage->m_TileSize) * 2 * 3;

#endif

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��ʼ��
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///��������

// ��Ե - ����
#define	CREATE_OUTERBORD1_INDEX(key, i, j)	if((key) % 2 > 0){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * (j + 1)	+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;}else{*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 1)			+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;}
#define	CREATE_OUTERBORD2_INDEX(key, i, j)	if((key) % 2 > 0){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * (j - 1)	+ unitOffset;}else{*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j - 1)			+ unitOffset;}
// ��Ե - ����
#define	CREATE_OUTERBORD3_INDEX(key, i, j)	if((key) % 2 > 0){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 1)			+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * (j + 1)	+ unitOffset;}else{*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 1)			+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;}
#define	CREATE_OUTERBORD4_INDEX(key, i, j)	if((key) % 2 > 0){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = (i - 1) + (m_pParentPage->m_TileSize + 1) * (j + 1)	+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 1)			+ unitOffset;}else{*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = (i - 1) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 1)			+ unitOffset;}
// �α�Ե - ����
#define	CREATE_INNERBORD1_INDEX(i, j){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j	+ unitOffset;*pIdx++  = (i + 2) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * (j - 1)	+ unitOffset;}
#define	CREATE_INNERBORD2_INDEX(i, j){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j	+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * (j + 1)	+ unitOffset;*pIdx++  = (i + 2) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;}
// �α�Ե - ����
#define	CREATE_INNERBORD3_INDEX(i, j){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j	+ unitOffset;*pIdx++  = (i - 1) + (m_pParentPage->m_TileSize + 1) * (j + 1)	+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 2)			+ unitOffset;}
#define	CREATE_INNERBORD4_INDEX(i, j){*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j	+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 2)	+ unitOffset;*pIdx++  = (i + 1) + (m_pParentPage->m_TileSize + 1) * (j + 1)			+ unitOffset;}


void COTETile::Initialise(int startx, int startz, TileCreateData_t* pData,
							int pageIndex,	RenderPatch_t* renderPatch)
{
	COTETilePage* page = m_pParentPage;
	
	m_IndexInPage = pageIndex;

	// ����

	DeleteGeometry();		

	/// ÿһ����Ԫunit����������������ֵ	
	
	int endx = startx + (m_pParentPage->m_TileSize + 1) ;	
    int endz = startz + (m_pParentPage->m_TileSize + 1) ;	

	int startUX = startx / m_pParentPage->m_UnitSize;
	int startUZ = startz / m_pParentPage->m_UnitSize;

	////////////////////////////////////////////////////////////////
	///  unit map	
	////////////////////////////////////////////////////////////////

	//m_pTerrainUnitMap = page->m_pTileMapBufferPool + m_IndexInPage;

	//m_pTerrainUnitMap->SetSize((page->m_TileSize) / page->m_UnitSize, (page->m_TileSize) / page->m_UnitSize);	//lyh��: ��Ϊһ��unit���������飬���� [ unit���� ] ����tile������һ��	//unitsize=2
	//
	//int tUnitSize = (page->m_TileSize) / page->m_UnitSize;

	//int tUnitPoolOffset = m_IndexInPage * tUnitSize * tUnitSize;

	//for(int i = 0; i < tUnitSize; i ++)
	//for(int j = 0; j < tUnitSize; j ++)
	//{
	//	m_pTerrainUnitMap->m_Tiles[i][j] = page->m_pTerrainUnitPool + tUnitPoolOffset + i * tUnitSize + j;
	//}	

	///// uv��Ϣ

	//bool texNoEdited[] = {true, true, true};

	//bool texCoverd[] = {true, true, true};

	//for ( int uZ = 0; uZ < (endz - startz - 1) / m_pParentPage->m_UnitSize; uZ ++ )
	//{		
	//	for ( int uX = 0; uX < (endx - startx - 1) / m_pParentPage->m_UnitSize; uX ++ )
	//	{   
	//		CTerrainUnit* tu = (CTerrainUnit*)m_pTerrainUnitMap->m_Tiles[uX][uZ];			 
	//		if(pData)
	//		{
	//			int uvInfOffset = uX + startUX + (uZ + startUZ) * ( (page->m_PageSizeX) / m_pParentPage->m_UnitSize);
	//			CTerrainUnitData* pUnitData = pData->uvData + uvInfOffset;

	//			for(int ii = 0; ii < TEXTURE_LAYER_COUNT; ii ++)
	//			{
	//				tu->m_LayerUIndex[ii] = pUnitData->m_LayerUIndex[ii]; 
	//				tu->m_LayerVIndex[ii] = pUnitData->m_LayerVIndex[ii]; 				
	//				
	//				texNoEdited[ii] &= (tu->m_LayerUIndex[ii] == 3 && tu->m_LayerVIndex[ii] == 2);	
	//				texCoverd[ii] &= (tu->m_LayerUIndex[ii] == 1 && tu->m_LayerVIndex[ii] == 1 || tu->m_LayerUIndex[ii] > 3);
	//			}		
	//		}
	//		else
	//		{
	//			for(int ii = 0; ii < TEXTURE_LAYER_COUNT; ii ++)
	//			{
	//				// (3,2)λ���ϵ�ͼԪ����ȫ͸���� 

	//				tu->m_LayerUIndex[ii] = 3;
	//				tu->m_LayerVIndex[ii] = 2;					
	//			}
	//		}
	//	}
	//}

	////////////////////////////////////////////////////////////////
	/// ����	
	////////////////////////////////////////////////////////////////

	// ��ѯ��ͼ����	

	std::string matName;

	std::string texNames[TEXTURE_LAYER_COUNT];		

	TileCreateData_t::TileTextureMap_t::iterator itr;

	for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
	{				
		itr = pData->tileTexes[i]->find(m_Name); 
		if( itr != pData->tileTexes[i]->end() )    
		{
			texNames[i] = itr->second;		
		}
		else
		{
			texNames[i]= pData->layerTexes[i];
		}		

		matName += texNames[i];
	}

#ifdef GAME_DEBUG1	

	m_Material = MaterialManager::getSingleton().getByName(matName);	
	
	if(m_Material.isNull())
	{
		m_Material = MaterialManager::getSingleton().create(matName, 
					ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);

		SetupMaterial(texNames);	
	}
	else
	{
		m_UseSharedMaterial = true;
	}

#else

	m_UseSharedMaterial = true;
	SetupMaterial(texNames);	

#endif

	////////////////////////////////////////////////////////////////
	/// �����ʽ 	
	////////////////////////////////////////////////////////////////

	m_IndexInPage = pageIndex;

	// Ϊ�������ύ ��tile��pool�е�λ�ø��ݲ��ʼ���

	Real min = 256000, max = 0;

#ifdef GAME_DEBUG1

	m_Terrain = page->m_pTileVertexDataPool + m_IndexInPage;	
	
	m_Terrain->vertexCount = GetTileVertexCount();	

	m_Terrain->vertexStart = m_IndexInPage * GetTileVertexCount();

	VertexDeclaration* decl   = m_Terrain->vertexDeclaration;
    VertexBufferBinding* bind = m_Terrain->vertexBufferBinding;

    // positions
    size_t offset = 0;
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);

    // ����
    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);

    // texture coord sets
    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    offset += VertexElement::getTypeSize(VET_FLOAT2);
    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);

#endif

	////////////////////////////////////////////////////////////////
	/// ���� 	
	////////////////////////////////////////////////////////////////	

	// ����buffer
	
	if(page->m_IndexBuffer.isNull())
	{	
#ifdef GAME_DEBUG1
		int indexCnt = (m_pParentPage->m_PageSizeX) / (m_pParentPage->m_TileSize); // һ��tile
#else
		int indexCnt = COTETilePage::c_PatchTileSize * COTETilePage::c_PatchTileSize;
#endif		

		page->m_IndexBuffer = 
			HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT,
			GetTileIndexCount() * indexCnt, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);	


		unsigned short* pIdx = static_cast<unsigned short*>(page->m_IndexBuffer->lock(HardwareBuffer::HBL_DISCARD));
		
		/*
		һ��unit
		1 - 2
		|   |
		3 - 4
		��Ӧ2�������Σ� 1 - 3 - 2 �� 2 - 3 - 4

		*/		
		
		for ( int indexInPage = 0; indexInPage < indexCnt; indexInPage ++ )
		{
			// һ�� tile
	
			for ( int uZ = 0; uZ < (endz - startz - 1) / m_pParentPage->m_UnitSize; uZ ++ )			
			for ( int uX = 0; uX < (endx - startx - 1) / m_pParentPage->m_UnitSize; uX ++ )
			{
				int unitOffset = indexInPage * GetTileVertexCount() + 
					(uX  + uZ * ((page->m_TileSize) / m_pParentPage->m_UnitSize)) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);
	
				const int c_stepSize = 1;

				for(int jj = 0; jj < m_pParentPage->m_UnitSize; jj += c_stepSize)
				for(int ii = 0; ii < m_pParentPage->m_UnitSize; ii += c_stepSize)
				{			
					*pIdx++  = (ii) + (jj * (m_pParentPage->m_UnitSize + 1)) + unitOffset;
					*pIdx++  = (ii) + ((jj + c_stepSize) * (m_pParentPage->m_UnitSize + 1) ) + unitOffset;
					*pIdx++  = (ii + c_stepSize) + (jj * (m_pParentPage->m_UnitSize + 1)) + unitOffset;					

					*pIdx++  = (ii + c_stepSize) + (jj * (m_pParentPage->m_UnitSize + 1)) + unitOffset;		
					*pIdx++  = (ii) + ((jj + c_stepSize) * (m_pParentPage->m_UnitSize + 1)) + unitOffset;
					*pIdx++  = (ii + c_stepSize) + ((jj + c_stepSize) * (m_pParentPage->m_UnitSize + 1)) + unitOffset;					
				}	
			}			
		}

		page->m_IndexBuffer->unlock();

		if(DEFAULT_TILESIZE == 16)
		{
			/// ���´���Ϊ��Ч���Ż������һЩ "Ӳ"!

			/// ���������Ӽ���lod

			///////////////////////////����ƽ��/////////////////////////////////
			
			const int c_indexCnt_Lvl0 = 1;

			page->m_LodLevel[0].IndexCount = c_indexCnt_Lvl0 * 6;
			
			page->m_LodLevel[0].IndexBuffer = 
				HardwareBufferManager::getSingleton().createIndexBuffer(
				HardwareIndexBuffer::IT_16BIT,
				page->m_LodLevel[0].IndexCount * indexCnt, 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);			
			
			pIdx = static_cast<unsigned short*>(page->m_LodLevel[0].IndexBuffer->lock(HardwareBuffer::HBL_DISCARD));
				
			
			for ( int indexInPage = 0; indexInPage < indexCnt; indexInPage ++ )
			{
				int unitOffset = indexInPage * GetTileVertexCount();

				const int c_stepSize = m_pParentPage->m_UnitSize;

				for(int jj = 0; jj < m_pParentPage->m_UnitSize; jj += c_stepSize)
				for(int ii = 0; ii < m_pParentPage->m_UnitSize; ii += c_stepSize)
				{
					*pIdx++  = (ii) + (jj * (m_pParentPage->m_UnitSize + 1)) + unitOffset;
					*pIdx++  = (ii) + ((jj + c_stepSize) * (m_pParentPage->m_UnitSize + 1) ) + unitOffset;
					*pIdx++  = (ii + c_stepSize) + (jj * (m_pParentPage->m_UnitSize + 1)) + unitOffset;					

					*pIdx++  = (ii + c_stepSize) + (jj * (m_pParentPage->m_UnitSize + 1)) + unitOffset;		
					*pIdx++  = (ii) + ((jj + c_stepSize) * (m_pParentPage->m_UnitSize + 1)) + unitOffset;
					*pIdx++  = (ii + c_stepSize) + ((jj + c_stepSize) * (m_pParentPage->m_UnitSize + 1)) + unitOffset;					
				}		
			}

			page->m_LodLevel[0].IndexBuffer->unlock();

			
			////////////////////////////////////////////////////////////////////

			const int c_indexCnt_Lvl1 = 95;

			page->m_LodLevel[1].IndexCount = c_indexCnt_Lvl1 * 6;		

			page->m_LodLevel[1].IndexBuffer = 
				HardwareBufferManager::getSingleton().createIndexBuffer(
				HardwareIndexBuffer::IT_16BIT,
				page->m_LodLevel[1].IndexCount * indexCnt, 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);	
			

			pIdx = static_cast<unsigned short*>(page->m_LodLevel[1].IndexBuffer->lock(HardwareBuffer::HBL_DISCARD));

			for ( int indexInPage = 0; indexInPage < indexCnt; indexInPage ++ )
			{
				for ( int uZ = 0; uZ < (endz - startz - 1) / m_pParentPage->m_UnitSize; uZ ++ )			
				for ( int uX = 0; uX < (endx - startx - 1) / m_pParentPage->m_UnitSize; uX ++ )
				{
					int unitOffset = indexInPage * GetTileVertexCount() + 
						(uX  + uZ * ((page->m_TileSize) / m_pParentPage->m_UnitSize)) * (m_pParentPage->m_UnitSize + 1) * (m_pParentPage->m_UnitSize + 1);

					// ��������

					int i, j;
					for(i = 0, j = 0; i < 16; i ++		)	CREATE_OUTERBORD1_INDEX(i + 1, i, j)
					for(i = 1, j = 1; i < 15; i += 2	)	CREATE_INNERBORD1_INDEX(i, j)
					for(i = 0, j = 16; i < 16; i ++		)	CREATE_OUTERBORD2_INDEX(i + 1, i, j)
					for(i = 1, j = 15; i < 15; i += 2	)	CREATE_INNERBORD2_INDEX(i, j)

					// ��������

					for(i = 0, j = 0; j < 16; j ++		)	CREATE_OUTERBORD3_INDEX(j + 1, i, j)
					for(i = 1, j = 1; j < 15; j += 2	)	CREATE_INNERBORD3_INDEX(i, j)
					for(i = 16, j = 0; j < 16; j ++		)	CREATE_OUTERBORD4_INDEX(j + 1, i, j)
					for(i = 15, j = 1; j < 15; j += 2	)	CREATE_INNERBORD4_INDEX(i, j)

					// ʣ�µĲ���

					for(i = 1; i < 15; i += 2)
					for(j = 1; j < 15; j += 2)
					{
						*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * (j + 2)			+ unitOffset;*pIdx++  = (i + 2) + (m_pParentPage->m_TileSize + 1) * (j + 2)	+ unitOffset;*pIdx++  = i + (m_pParentPage->m_TileSize + 1) * j				+ unitOffset;*pIdx++  = (i + 2) + (m_pParentPage->m_TileSize + 1) * (j + 2)	+ unitOffset;*pIdx++  = (i + 2) + (m_pParentPage->m_TileSize + 1) * j			+ unitOffset;
					}

				}
			}

			page->m_LodLevel[1].IndexBuffer->unlock();
		}
	}
	
	// ��������

#ifdef GAME_DEBUG1

	m_IndexData = page->m_pTileIndexDataPool + m_IndexInPage;
	m_IndexData->indexStart  = 0;
	m_IndexData->indexCount  = GetTileIndexCount();	
	m_IndexData->indexBuffer = page->m_IndexBuffer;
	//m_IndexData->indexCount  = page->m_LodLevel[0].IndexCount;	
	//m_IndexData->indexBuffer = page->m_LodLevel[0].IndexBuffer;

	Ogre::HardwareVertexBufferSharedPtr& mainBuffer = page->m_MainBuffer;
    // Create shared vertex buffer
	if(mainBuffer.isNull())
	{		
		mainBuffer = 
			HardwareBufferManager::getSingleton().createVertexBuffer(
				decl->getVertexSize(0), 
				GetTileVertexCount() * m_pParentPage->GetPageTileNum(), 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);	

	}

#else

	RenderUnit_t* renderUnit;
	Ogre::HardwareVertexBufferSharedPtr mainBuffer;

	VertexDeclaration* decl;
	VertexBufferBinding* bind;

	std::map<Ogre::Material*, RenderUnit_t >::iterator rmIt = 
									renderPatch->m_RenderMap.find(m_Material.getPointer());
	if(rmIt == renderPatch->m_RenderMap.end())	
	{
		RenderUnit_t ru;		

		/// ����

		ru.m_Terrain = page->m_pTileVertexDataPool + m_IndexInPage;		

		decl	= ru.m_Terrain->vertexDeclaration;
		bind	= ru.m_Terrain->vertexBufferBinding;

		// positions
		size_t offset = 0;
		decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
		offset += VertexElement::getTypeSize(VET_FLOAT3);

		// ����
		decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
		offset += VertexElement::getTypeSize(VET_FLOAT3);

		// texture coord sets
		decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
		offset += VertexElement::getTypeSize(VET_FLOAT2);
		decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
		offset += VertexElement::getTypeSize(VET_FLOAT2);

		ru.m_Terrain->vertexStart = 0;
		ru.m_Terrain->vertexCount = 0;

		ru.m_VertBuffer = 
			HardwareBufferManager::getSingleton().createVertexBuffer(
				decl->getVertexSize(0), 
				GetTileVertexCount() * COTETilePage::c_PatchTileSize * COTETilePage::c_PatchTileSize, 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);		
		
		/// ����

		ru.m_IndexData = page->m_pTileIndexDataPool + m_IndexInPage; // ����tile��indexData
		ru.m_IndexData->indexBuffer = page->m_IndexBuffer;
		ru.m_IndexData->indexStart  = 0;	
		ru.m_IndexData->indexCount  = GetTileIndexCount();

		// λ��

		ru.m_Position = Ogre::Vector2(
			renderPatch->m_PatchMapX * (COTETilePage::c_PatchTileSize + COTETilePage::c_PatchTileSize / 2) * (page->m_TileSize) * page->m_Scale.x,	
			renderPatch->m_PatchMapZ * (COTETilePage::c_PatchTileSize + COTETilePage::c_PatchTileSize / 2) * (page->m_TileSize) * page->m_Scale.z
			);

		//OTE_TRACE("renderPatch->m_PatchMapn: " << renderPatch->m_PatchMapX << " " << renderPatch->m_PatchMapZ)

		renderPatch->m_RenderMap[m_Material.getPointer()] = ru;		
	}
	
	renderUnit = &renderPatch->m_RenderMap[m_Material.getPointer()];	

	mainBuffer = renderUnit->m_VertBuffer;

	decl	= renderUnit->m_Terrain->vertexDeclaration;
	bind	= renderUnit->m_Terrain->vertexBufferBinding;

	m_ParentRenderUnit = renderUnit;
	m_ParentRenderUnit->m_TileCount ++;

#endif	

	////////////////////////////////////////////////////////////////
	/// ���� 	
	////////////////////////////////////////////////////////////////

	// ���Խ�һ��pool�Ż�

    m_pPositionBuffer = page->m_pTilePositionBufferPool + m_IndexInPage * GetTileVertexCount() * 3;

	m_pNormalBuffer   = page->m_pTileNormalBufferPool + m_IndexInPage * GetTileVertexCount();  

	bind->setBinding(0, mainBuffer);

	////////////////////////////////////////////////////////////////
	///  ���ݳ�ʼ��	
	////////////////////////////////////////////////////////////////

    Vector3 left, down, here;

    const VertexElement* poselem = decl->findElementBySemantic(VES_POSITION);
	const VertexElement* normailelem = decl->findElementBySemantic(VES_NORMAL);	
    const VertexElement* texelem0 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);  
	const VertexElement* texelem1 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 1); 


    float* pSysPos = m_pPositionBuffer;
	Ogre::Vector3* pSysNormal = m_pNormalBuffer;

	int bufSize = mainBuffer->getVertexSize() * GetTileVertexCount();

#ifdef GAME_DEBUG1
	static unsigned char* pBase0;
	if(m_IndexInPage == 0)
		pBase0 = static_cast<unsigned char*>(m_pParentPage->m_MainBuffer->lock(HardwareBuffer::HBL_DISCARD));

	unsigned char* pBase = pBase0 + m_IndexInPage * bufSize;	
#else
	
	m_VertexStart = renderUnit->m_Terrain->vertexCount;
	unsigned char* pBase = static_cast<unsigned char*>(mainBuffer->lock(renderUnit->m_Terrain->vertexCount * mainBuffer->getVertexSize(),
								bufSize, HardwareBuffer::HBL_DISCARD));
	
#endif

	int j = startz;

	/// �������е�Ԫ

	float x = 0; float y = 0;

    for ( int uZ = 0; uZ < (endz - startz - 1) / m_pParentPage->m_UnitSize; uZ ++ )
    {
		int i = startx;

        for ( int uX = 0; uX < (endx - startx - 1) / m_pParentPage->m_UnitSize; uX ++ )
        {   

			/// һ����Ԫ
			
			Real height = 0;
			 
			for(int jj = 0; jj < m_pParentPage->m_UnitSize + 1; jj ++)
			for(int ii = 0; ii < m_pParentPage->m_UnitSize + 1; ii ++)
			{
				float *pPos, *pNormal;			

				poselem->baseVertexPointerToElement(pBase, &pPos);
				normailelem->baseVertexPointerToElement(pBase, &pNormal);	

				float *pTex0, *pTex1;

				texelem0->baseVertexPointerToElement(pBase, &pTex0);   
				texelem1->baseVertexPointerToElement(pBase, &pTex1); 				
				
				// �߶�

				if(pData)
				{
					if(pData->fHeightData)
					{
						height = pData->fHeightData[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)] * page->m_Scale.y;
					}
					else
					{
						height = (float)pData->heightData[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)] / 255.0f * page->m_Scale.y;
					}
				}
				else
					height = 0.0f;

				*pSysPos++ = *pPos++ = (( float )(i + ii) )  * page->m_Scale.x;	// x
				*pSysPos++ = *pPos++ = height;									// y
				*pSysPos++ = *pPos++ = (( float )(j + jj) )  * page->m_Scale.z;	// z
			
				// ����
				
				if(pData->normal)
				{
					Ogre::Vector3 tempNor = (Ogre::Vector3)pData->normal[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)];
					(*pSysNormal).x = *pNormal++ = tempNor.x;
					(*pSysNormal).y = *pNormal++ = tempNor.y;
					(*pSysNormal).z = *pNormal++ = tempNor.z;					
				}
				else
				{
					(*pSysNormal).x = 0.0f;
					(*pSysNormal).y = 0.0f;
					(*pSysNormal).z = 0.0f;
				}					
				pSysNormal++;

				// UV

				*pTex0++ = ( (float)(ii) / (m_pParentPage->m_TileSize) ) * 4.0f;
				*pTex0++ = ( (float)(jj) / (m_pParentPage->m_TileSize) ) * 4.0f;

				*pTex1++ = ( (float)(i + ii) / (m_pParentPage->m_PageSizeX) );
				*pTex1++ = ( (float)(j + jj) / (m_pParentPage->m_PageSizeZ) );

				pBase += mainBuffer->getVertexSize();				
			}		

			height < min ? min = height : NULL;
			height > max ? max = height : NULL;          

			i += m_pParentPage->m_UnitSize;
        }

		j += m_pParentPage->m_UnitSize;
    }
#ifdef GAME_DEBUG1

	if(m_IndexInPage == m_pParentPage->GetPageTileNum() - 1)
	{
		mainBuffer->unlock();
	}

#else

	mainBuffer->unlock(); 

	renderUnit->m_Terrain->vertexCount  += GetTileVertexCount();		

#endif

	m_Bounds.setExtents( 
            ( Real ) (startx ) * page->m_Scale.x, 
            min, 
            ( Real ) (startz ) * page->m_Scale.z,
            (( Real )( endx - 1 )) * page->m_Scale.x, 
            max, 
            (( Real )( endz - 1 )) * page->m_Scale.z );


    m_Center = Vector3( 
		((float)(startx + endx - 1) * page->m_Scale.x ) / 2,
        ( min + max ) / 2,
        ((float)(startz + endz - 1) * page->m_Scale.z) / 2 );	
	
}

// -------------------------------------------------
void COTETile::SetWater(int waterValue)
{	
	// ��ɾ��

	if(m_WaterValue >= 0)
	{
		COTELiquid* ld = COTELiquidManager::GetSingleton()->GetLiquid(waterValue);
		if(ld)
			ld->EraseLiquidPlane(this);

		m_WaterValue = 0;
	}

	if(waterValue > 0)
	{
		// ��ʼ��ˮƬ		

		if(waterValue != m_WaterValue)
		{				
			COTELiquid* ld = COTELiquidManager::GetSingleton()->GetLiquid(waterValue);
			if(ld)
			{
				ld->AddLiquidPlane(this, waterValue);
			
				m_WaterValue = waterValue;			
			}
		}	
	}
}