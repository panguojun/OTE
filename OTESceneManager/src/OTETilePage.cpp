// ========================================================================================
//  COTETilePage class
//  By Romeo Jan 10/06
// ========================================================================================
#include "OTETilepage.h"
#include "Ogre.h"
#include "OTEQTSceneManager.h"
#include "OTETerrainSerializer.h"
#include "OgrePanelOverlayElement.h"
#include "OTEVegManager.h"
#include "OTEWater.h"
#include "OTECommon.h"

#include "OTED3DManager.h"
#include "OTEPrjTexture.h"
#include "OTESky.h"

using namespace Ogre;
using namespace std; 

#define	 T_DEBUG

namespace OTE
{

// =============================================
// COTETilePage
// =============================================

HashMap<std::string, COTETilePage*>	COTETilePage::s_TerrainPageMap;	
COTETilePage* 						COTETilePage::s_CurrentPage = NULL;	
Ogre::SceneManager* 				COTETilePage::s_pCreator = NULL;	
const int							COTETilePage::c_LightmapScale = 2;
const int							COTETilePage::c_AlphamapScale = 2;
const int							COTETilePage::c_PatchTileSize = 4; // 注意这个尺寸 必须满足: c_PatchTileSize * tilesize <= pageSize

COTETile*							COTETilePage::s_LastTile = NULL;

//-------------------------------------------------------------------------
COTETilePage::COTETilePage(Ogre::SceneManager* sm) :
m_pPage(NULL),
m_TerrainName("TilePage"),
m_pSceneNode( NULL),

// 默认配置

m_Scale(Ogre::Vector3(2.0f, 30.0f, 2.0f)), // 山体缩放
m_TileSize(DEFAULT_TILESIZE),
m_PageSizeX(DEFAULT_PAGESIZE),
m_PageSizeZ(DEFAULT_PAGESIZE),
m_UnitSize(DEFAULT_TILESIZE),
m_ShowTerrain(true),
m_LightmapData(NULL),
m_AlphaBmpdata(NULL),
m_BaseTerrainHeight(m_Scale.y / 2.0f),
m_ShowAll(false)
{
	m_LightmapPath = OTEHelper::GetResPath("ResConfig", "LightmapPath");	
	m_AlphamapPath = OTEHelper::GetResPath("ResConfig", "AlphamapPath");
	m_MaxDistance = SCENE_CAM->getFarClipDistance();
	s_pCreator = sm;
}

COTETilePage::~COTETilePage(void)
{
	//OTE_TRACE("COTETilePage::~COTETilePage")
}

//-------------------------------------------------------------------------
// 摧毁所有页
void COTETilePage::DestroyAllPages()
{
	HashMap<std::string, COTETilePage*>::iterator it = s_TerrainPageMap.begin();
	
	while(it != s_TerrainPageMap.end())
	{
		/*it->second->m_pSceneNode->attachObject(it->second);*/
		it->second->m_pSceneNode->getParentSceneNode()->removeChild(it->second->m_pSceneNode);
		it->second->s_pCreator->destroySceneNode(it->second->m_pSceneNode->getName());
		
		it->second->Shutdown();

		delete it->second;

		++ it;
	}
	s_TerrainPageMap.clear();
}

//-------------------------------------------------------------------------
// 删除
void COTETilePage::Shutdown(void)
{ 	
	if (m_pPage)
	{	
		RemoveTileMaterials();	

		delete m_pPage;	m_pPage = NULL;

		delete m_pRenderPatchPage; m_pRenderPatchPage = NULL;

		// 草

		COTELiquidManager::GetSingleton()->Clear();

		// 水效果

		COTELiquidManager::GetSingleton()->Clear();

		// 贴花效果

		COTEPrjTextureRQListener::Destroy();

		// 摧毁tile poor buffer

		if(m_pTileVertexDataPool)
		{
			delete [] m_pTileVertexDataPool;
			m_pTileVertexDataPool = NULL;
		}

		if(m_pTileIndexDataPool)
		{
			delete [] m_pTileIndexDataPool;
			m_pTileIndexDataPool = NULL;
		}

		if(m_pTilePositionBufferPool)
		{
			delete [] m_pTilePositionBufferPool;
			m_pTilePositionBufferPool = NULL;
		}

		if(m_pTileNormalBufferPool)
		{
			delete [] m_pTileNormalBufferPool;
			m_pTileNormalBufferPool = NULL;
		}


		if(m_pTileMapBufferPool)
		{
			delete [] m_pTileMapBufferPool;
			m_pTileMapBufferPool = NULL;
		}

		if(m_pTerrainUnitPool)
		{
			delete [] m_pTerrainUnitPool;	
			m_pTerrainUnitPool = NULL;
		}
		
		// 清理

		if(s_CurrentPage == this)
			s_CurrentPage = NULL;

		s_LastTile = NULL;

		if(m_LightmapData)
			delete [] m_LightmapData;

		if(m_AlphaBmpdata)
			delete [] m_AlphaBmpdata;
	} 	

}
//-------------------------------------------------------------------------

void COTETilePage::RemoveTileMaterials()
{
	// 清理材质 需要做成材质跟贴图邦定！
 
	for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
	{
		TileCreateData_t::TileTextureMap_t::iterator itr;	
		itr = m_TileTexes[i].begin();
		while( itr != m_TileTexes[i].end() )    
		{				
			MaterialManager::getSingleton().remove(itr->first);

			++ itr;
		}
	}

}

//-------------------------------------------------------------------------
// 注意 m_PageSizeX m_PageSizeZ 必须是2的n次方
void COTETilePage::BuildPage(TileCreateData_t* pData)
{	
	int tilesX = (m_PageSizeX) / (m_TileSize);
	int tilesZ = (m_PageSizeZ) / (m_TileSize);

	/// build buffer poors

	int tPageTileNum = tilesX * tilesZ;

	int tUnitSize = (m_TileSize) / m_UnitSize;

	int tSize = (m_TileSize + 1 + (m_TileSize) / m_UnitSize - 1);
	int tTileVertexCount = tSize * tSize;

	m_pTileVertexDataPool		= new Ogre::VertexData[tPageTileNum];

	m_pTileIndexDataPool		= new Ogre::IndexData[tPageTileNum];

	m_pTilePositionBufferPool	= new float[tTileVertexCount * 3 * tPageTileNum];

	m_pTileNormalBufferPool		= new Ogre::Vector3[tTileVertexCount * tPageTileNum];

	m_pTileMapBufferPool		= new CTileMap[tPageTileNum];	

	m_pTerrainUnitPool			= new CTerrainUnit[tUnitSize * tUnitSize * tPageTileNum];
	
	// create 

	m_pPage = new COTEPage<COTETile>(tilesX, tilesZ);

	m_pRenderPatchPage = new COTEPage<RenderPatch_t>(tilesX / c_PatchTileSize, tilesZ / c_PatchTileSize); 
	
	if(!pData->name.empty())
		m_TerrainName = pData->name;

	if(!m_pSceneNode)
		m_pSceneNode = (Ogre::SceneNode*)s_pCreator->getRootSceneNode()->createChild(m_TerrainName);
		
	{// 光照图
		if(!Ogre::ResourceGroupManager::getSingleton().resourceExists(
			Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
			m_LightmapName)
			)
		{
			OTE_MSG_ERR("没有找到LightMap: " << m_LightmapName)
			return;
		}
		m_LightmapPtr = TextureManager::getSingleton().getByName(m_LightmapName);
		if(m_LightmapPtr.isNull())
		{
			m_LightmapPtr = TextureManager::getSingleton().load(m_LightmapName, 
									ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		}	
	}

	{// alpha图
		if(!Ogre::ResourceGroupManager::getSingleton().resourceExists(
			Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
			m_AlphamapName)
			)
		{
			OTE_MSG_ERR("没有找到AlphaMap: " << m_AlphamapName)
			return;
		}
		m_AlphamapPtr = TextureManager::getSingleton().getByName(m_AlphamapName);
		if(m_AlphamapPtr.isNull())
			m_AlphamapPtr = TextureManager::getSingleton().load(m_AlphamapName, 
								ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}

#ifdef GAME_DEBUG1

	int tileIndex = 0;	//tile的索引号
	int q = 0;
    for ( int j = 0; j < m_PageSizeZ; j += ( m_TileSize ) )
    {
        int p = 0;

        for ( int i = 0; i < m_PageSizeX; i += ( m_TileSize ) )
        {
			Ogre::StringUtil::StrStreamType new_name_str;
			
            // Create scene node for the tile and the TerrainRenderable
            new_name_str << "tile[" << (int)p << "," << (int)q << "]";
			Ogre::String tileName = new_name_str.str();

			//SceneNode *c = m_pSceneNode->createChildSceneNode( tileName );
            COTETile* tile = new COTETile(tileName, this);
		
			// set queue
			tile->setRenderQueueGroup(s_pCreator->getWorldGeometryRenderQueue());

            // Initialise the tile

            tile->Initialise(i, j, pData, tileIndex++, NULL);		
            
			// Attach it to the page
			m_pPage->m_Tiles[ p ][ q ] = tile;

            // Attach it to the node
            //c ->attachObject( tile );		

            p++;
        }
        q++;
    }

#else

	int tileIndex = 0;	//tile的索引号
	int q = 0;
    for ( int j = 0; j < (m_PageSizeZ); j += ( m_TileSize ) * c_PatchTileSize )
    {
        int p = 0;
        for ( int i = 0; i < (m_PageSizeX); i += ( m_TileSize ) * c_PatchTileSize )
        {
			RenderPatch_t* renderPatch = new RenderPatch_t();
			m_pRenderPatchPage->m_Tiles[ p ][ q ] = renderPatch;

			renderPatch->m_PatchMapX = p;
			renderPatch->m_PatchMapZ = q;

			for(int jj = 0; jj < c_PatchTileSize; jj ++)
			for(int ii = 0; ii < c_PatchTileSize; ii ++)
			{
				Ogre::StringUtil::StrStreamType new_name_str;
				
				// Create scene node for the tile and the TerrainRenderable
				new_name_str << "tile[" << (int)(p * c_PatchTileSize + ii) << "," << (int)(q * c_PatchTileSize + jj) << "]";
				Ogre::String tileName = new_name_str.str();

				//SceneNode *c = m_pSceneNode->createChildSceneNode( tileName );
				COTETile* tile = new COTETile(tileName, this);
			
				// set queue
				tile->setRenderQueueGroup(s_pCreator->getWorldGeometryRenderQueue());	
	            
				// Attach it to the page

				int pagex = p * c_PatchTileSize + ii;
				int pagez = q * c_PatchTileSize + jj;				

				m_pPage->m_Tiles[ pagex ][ pagez ] = tile;

				// Initialise the tile

				tile->Initialise(i + ii * ( m_TileSize ), j + jj * ( m_TileSize ), pData, 
					pagez * (m_PageSizeX) / ( m_TileSize ) + pagex, renderPatch );					
			}

			p++;
        }
        q++;
    }
	
#endif

	LogManager::getSingleton().logMessage("加载地形顶点信息完毕！");

	BuildNeighbers();

	// 加入渲染队列 

	setRenderQueueGroup(RENDER_QUEUE_MAIN);

	m_pSceneNode->attachObject(this);

	// 尺寸调整

	float maxX_lim = (m_PageSizeX + 1) * m_Scale.x;
	float maxZ_lim = (m_PageSizeZ + 1) * m_Scale.z;

	m_BoundingBox = Ogre::AxisAlignedBox(0, 0, 0, maxX_lim, m_Scale.y, maxZ_lim);	

}

//-------------------------------------------------------------------------
//平均地形全部法线

void COTETilePage::AverageVertexNormal()
{	
#ifndef GAME_DEBUG1
	return;
#endif 

	int q = 0;
	for ( int j = 0; j < m_PageSizeZ; j += ( m_TileSize ) )	//此循环仿创建地形顶点数据时的循环
	{
		int p = 0;
		for ( int i = 0; i < m_PageSizeX; i += ( m_TileSize ) )
		{
			COTETile* tile = m_pPage->m_Tiles[ p ][ q ];
			if(tile)
				tile->AverageVertexNormal();	

			p++;
		}
		q++;
	}
}

//-------------------------------------------------------------------------
void COTETilePage::AverageVertexNormal(float x, float z, float halfSize)
{	
#ifndef GAME_DEBUG1
	return;
#endif 

	int startX = int((x - halfSize) / m_Scale.x);
	int startZ = int((z - halfSize) / m_Scale.z);
	int endX   = int((x + halfSize) / m_Scale.x);
	int endZ   = int((z + halfSize) / m_Scale.z);

	for ( int i = startZ; i <= endZ; i += ( m_TileSize ) )
	for ( int j = startX; j <= endX; j += ( m_TileSize ) )	
	{
		COTETile* tile = this->GetTileAt(j / (m_TileSize), i / (m_TileSize));
		if(tile)
			tile->AverageVertexNormal();			
	}		

}

//-------------------------------------------------------------------------
void COTETilePage::heightMapDeleteDirt(int Sensitivity)
{
	COTETerrainXmlSerializer::GetSingleton()->heightMapDeleteDirt(m_PageSizeX + 1, m_PageSizeZ + 1, Sensitivity, this);
}

//-------------------------------------------------------------------------
void COTETilePage::ExportData(TileCreateData_t* pData)
{
	int tiles = (m_PageSizeX) / (m_TileSize);

	int q = 0;
    for ( int j = 0; j < m_PageSizeZ; j += ( m_TileSize ) )
    {
        int p = 0;

        for ( int i = 0; i < m_PageSizeX; i += ( m_TileSize ) )
        {	            
			// Attach it to the page
			COTETile* tile = m_pPage->m_Tiles[ p ][ q ];
			if(tile)
				tile->ExportData(i, j, pData);        

            p++;
        }
        q++;

    }

}

//-------------------------------------------------------------------------
void COTETilePage::BuildNeighbers()
{
	int tilesPerLine  = (m_PageSizeX) / (m_TileSize);
	int tilesPerColum = (m_PageSizeZ) / (m_TileSize);

	 //setup the neighbor links.

    for ( int j = 0; j < tilesPerColum; j++ )
    {
        for ( int i = 0; i < tilesPerLine; i++ )
        {
            if ( j != tilesPerColum - 1 )
            {
				m_pPage->m_Tiles[ i ][ j ] -> SetNeighbor( CTile::SOUTH, m_pPage->m_Tiles[ i ][ j + 1 ] );//最下面的一行，不设置关联
                m_pPage->m_Tiles[ i ][ j + 1 ] -> SetNeighbor( CTile::NORTH, m_pPage->m_Tiles[ i ][ j ] );	
            }

            if ( i != tilesPerLine - 1 )
            {
                m_pPage->m_Tiles[ i ][ j ] -> SetNeighbor( CTile::EAST, m_pPage->m_Tiles[ i + 1 ][ j ] );//最右面的一行，不设置关联
                m_pPage->m_Tiles[ i + 1 ][ j ] -> SetNeighbor( CTile::WEST, m_pPage->m_Tiles[ i ][ j ] );
            }

        }
    }
}

// -----------------------------------------------
float COTETilePage::GetHeightAt(float x, float y)
{	
	COTETile* t = GetTileAt(x, y);
	if(t)
	{
		return t->GetHeightAt(x, y);
	}
	return 0.0f;
}

// -----------------------------------------------
// 添加到渲染队列

void COTETilePage::_updateRenderQueue(Ogre::RenderQueue* queue)
{
	TIMECOST_START

	if(!m_ShowTerrain)
		return;	
	
	if(m_ShowAll)
	{
		for(int j = 0;  j <= (m_PageSizeZ + 1) / (m_TileSize) - 1; ++ j)			
		for(int i = 0;  i <= (m_PageSizeX + 1) / (m_TileSize) - 1; ++ i)
		{
			OTE::COTETile* t = m_pPage->m_Tiles[ i ][ j ];
			if(t)
			{
				t->m_RenderState = eRenderPrepared;
				t->UpdateRenderQueue( queue );		
			}
		}	

		return;
	}

	Ogre::Camera *cam = SCENE_CAM;

	Vector3 camPos = cam->getPosition();
	
	Vector3 groundCorners[4];	

	const Vector3* pCorners = cam->getWorldSpaceCorners();
	
	float maxX_lim = (m_PageSizeX + 1) / (m_TileSize) - 1;
	float maxZ_lim = (m_PageSizeZ + 1) / (m_TileSize) - 1;

	float minX = maxX_lim;
	float maxX = -1;
	float minZ = maxZ_lim;
	float maxZ = -1;		
	
	float topX    = minX;
	float bottomX = maxX;

	Ogre::Vector2	CorsInTiles[4];

	// 计算可见矩形

	Ogre::Plane basePlane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, m_BaseTerrainHeight, 0));

	for( int i = 0; i < 4; ++ i)
	{	
		Ogre::Ray ray(pCorners[i], (pCorners[i + 4] - pCorners[i]).normalisedCopy());

		Ogre::Vector3 hitPos;
		std::pair<bool, float> hitResult = ray.intersects(basePlane);

		hitPos = ray.getPoint(hitResult.first ? min((hitResult.second > 0 ? hitResult.second : 0), m_MaxDistance) : m_MaxDistance);		

		CorsInTiles[i].x = (hitPos.x / m_Scale.x) / (m_TileSize);
		CorsInTiles[i].y = (hitPos.z / m_Scale.z) / (m_TileSize);
	
		float x = CorsInTiles[i].x;
		float z = CorsInTiles[i].y;

		x < minX ? minX = x: NULL;		

		z < minZ ? minZ = z, topX = x : NULL;

		x > maxX ? maxX = x: NULL;

		z > maxZ ? maxZ = z, bottomX = x : NULL;
	}

	// 摄像机点的投影 
	{		
		float x = (camPos.x / m_Scale.x) / (m_TileSize);
		float z = (camPos.z / m_Scale.z) / (m_TileSize);

		x < minX ? minX = x: NULL;		

		z < minZ ? minZ = z, topX = x : NULL;

		x > maxX ? maxX = x: NULL;

		z > maxZ ? maxZ = z, bottomX = x : NULL;
	}

	// fixed

	minX < 0 ? minX = 0 : NULL;
	maxX > maxX_lim ? maxX = maxX_lim : NULL;
	minZ < 0 ? minZ = 0 : NULL;
	maxZ > maxZ_lim ? maxZ = maxZ_lim : NULL;

	// 直线方程系数 A, B, C, R = SQRT(A^2 + B^2)

	static struct Line_Dita_t	{float A;float B;float C;float R;} Line_Dita[4];

	for( int i = 0; i < 4; ++ i)
	{	
		Line_Dita[i].A = CorsInTiles[(i + 1) % 4 ].y - CorsInTiles[i].y;
		Line_Dita[i].B = CorsInTiles[i].x - CorsInTiles[(i + 1) % 4 ].x;
		Line_Dita[i].C = CorsInTiles[(i + 1) % 4 ].x * CorsInTiles[i].y - CorsInTiles[i].x * CorsInTiles[(i + 1) % 4 ].y;
		
		Line_Dita[i].R = Ogre::Math::Sqrt(Line_Dita[i].A * Line_Dita[i].A +
													Line_Dita[i].B * Line_Dita[i].B);
	}

	// 根据索引筛选可见tile	
	
	topX < minX ? topX = minX : NULL;
	topX > maxX_lim ? topX = maxX_lim : NULL;

	for(int j = (int)minZ ; j <= (int)maxZ; ++ j)
	{
		// 向右
		
		for(int i = (int)topX; i <= (int)maxX; ++ i)
		{
			OTE::COTETile* t = m_pPage->m_Tiles[ i ][ j ];				

			// 是否在视图内部

			Ogre::Vector2 Point(
				(t->m_Center.x / m_Scale.x) / (m_TileSize), 
				(t->m_Center.z / m_Scale.z) / (m_TileSize) ); 

			bool inside = true;
			for( int ii = 0; ii < 4; ++ ii)
			{
				if(ii == 2)
					continue;

				float d = (Line_Dita[ii].A * Point.x + Line_Dita[ii].B * Point.y + Line_Dita[ii].C) / Line_Dita[ii].R;
				
				if(d < - (m_TileSize)) // 在内部
				{
					inside = false;

					// 是否在"中线"以右
					
					if((bottomX - topX) * (Point.y - minZ) - (maxZ - minZ) * (Point.x - topX) < 0)
					{
						i = maxX;
					}
					break;
				}				
			}
			if(inside)
			{
				// t->UpdateRenderQueue( queue );
				t->m_RenderState = eRenderPrepared;
				//CTerrainRQListener::AddToRenderQueue(queue, t->getMaterial(), t);
			}
		}

		// 向左

		for(int i = (int)(topX - 1); i >= (int)minX; -- i)
		{
			OTE::COTETile* t = m_pPage->m_Tiles[ i ][ j ];			

			// 是否在视图内部

			Ogre::Vector2 Point(
				(t->m_Center.x / m_Scale.x) / (m_TileSize), 
				(t->m_Center.z / m_Scale.z) / (m_TileSize) ); 

			bool inside = true;
			for( int ii = 0; ii < 4; ++ ii)
			{
				if(ii == 2)
					continue;

				float d = (Line_Dita[ii].A * Point.x + Line_Dita[ii].B * Point.y + Line_Dita[ii].C) / Line_Dita[ii].R;
				
				if(d < - (m_TileSize)) // 在内部
				{
					inside = false;	

					// 是否在"中线"以右
					
					if((bottomX - topX) * (Point.y - minZ) - (maxZ - minZ) * (Point.x - topX) > 0)
					{
						i = minX;
					}
					break;
				}				
			}
			if(inside)
			{
				//t->UpdateRenderQueue( queue );
				t->m_RenderState = eRenderPrepared;
				// CTerrainRQListener::AddToRenderQueue(queue, t->getMaterial(), t);
			}
		}
	}

#   ifdef GAME_DEBUG

	int cnt = 0;
	int rcnt = 0;

	for(int j = (int)minZ ; j <= (int)maxZ; ++ j)			
	for(int i = (int)minX;  i <= (int)maxX; ++ i)
	{
		OTE::COTETile* t = m_pPage->m_Tiles[ i ][ j ];
		if(t && t->m_RenderState == eRenderPrepared)
		{
			if(t->UpdateRenderQueue( queue ))
				rcnt ++;
			cnt ++;
		}
	}	

//#ifdef T_DEBUG
//	char buff[32];
//	sprintf(buff, "tile数： %d, tile渲染提交: %d", cnt, rcnt);
//	SCENE_MGR->m_RenderInfoList["TilePage"] = buff;
//#endif

#	else

	for(int j = (int)minZ ; j <= (int)maxZ; ++ j)			
	for(int i = (int)minX;  i <= (int)maxX; ++ i)
	{
		OTE::COTETile* t = m_pPage->m_Tiles[ i ][ j ];
		if(t && t->m_RenderState == eRenderPrepared)
		{
			t->UpdateRenderQueue( queue );
		}
	}

#   endif

	TIMECOST_END

}

// -----------------------------------------------
bool COTETilePage::LoadLightmapData(std::string strFileName)
{
	OTE_TRACE("加载LoadLightmapData file: " << strFileName << " ... ")

	if(strFileName.empty())
	{	
		OTEHelper::GetFullPath(m_LightmapName, m_LightmapPath, strFileName);
		m_LightmapPath += "\\";
	}	
	else
	{
		std::string path;
		OTEHelper::GetFullPath(strFileName, path, strFileName);
	}

	std::string failDesc;

	FILE *p = fopen(strFileName.c_str() , "rb");
	if(!p)
	{
		failDesc = "文件打开失败";
		goto _exit_;
	}
	
	fread(&m_LightmapBmfh, sizeof(BITMAPFILEHEADER), 1, p);
	if(m_LightmapBmfh.bfType != 0x4d42)
	{
		failDesc = "文件格式错误";
		goto _exit_;
	}

	fread(&m_LightmapBmih, sizeof(BITMAPINFOHEADER), 1, p);
	if(m_LightmapBmih.biCompression || m_LightmapBmih.biBitCount != 24)
	{
		failDesc = "像素格式错误";
		goto _exit_;
	}

	fseek(p, m_LightmapBmfh.bfOffBits, 0);
	
	// 清理

	if(m_LightmapData)
		delete [] m_LightmapData;
	
	m_LightmapData = new unsigned char[m_LightmapBmih.biWidth * m_LightmapBmih.biHeight * 3];

	fread(m_LightmapData, m_LightmapBmih.biWidth * m_LightmapBmih.biHeight * 3, 1, p);		

	fclose(p);

	OTE_TRACE("加载LoadLightmapData 完成! lightmap = " << (strFileName.empty() ? m_LightmapName : strFileName) << " width: " << m_LightmapBmih.biWidth << " height: " << m_LightmapBmih.biHeight)

	return true;

_exit_:

	if(m_LightmapData)
		delete [] m_LightmapData;

	m_LightmapData = NULL;

	OTE_MSG_ERR("读取光照图数据失败! 描述:" << failDesc)

	if(p)
		fclose(p);

	return false;
}

// -----------------------------------------------
Ogre::ColourValue COTETilePage::GetLightMapColorAt(float fx, float fz)
{	
	if(!m_LightmapData && !LoadLightmapData())				
		return Ogre::ColourValue::Black;	

	// check bounder

	int x = (int)(fx / m_Scale.x);
	int y = (int)(fz / m_Scale.z);

	if( x < 0 || x >= COTETilePage::m_PageSizeX ||
		y < 0 || y >= COTETilePage::m_PageSizeZ )
		return Ogre::ColourValue::Black;
	
	int offset = (m_LightmapBmih.biHeight - 
				int(fz / m_Scale.z * COTETilePage::c_LightmapScale)) * (m_LightmapBmih.biWidth) + 
				int(fx / m_Scale.x * COTETilePage::c_LightmapScale);

	if(offset < 0 || offset >= m_LightmapBmih.biHeight * m_LightmapBmih.biWidth)
	{
		return Ogre::ColourValue::Black;
	}

	unsigned char* a = &m_LightmapData[offset * 3];

	unsigned char tBlue	 =	(*(a  ));
	unsigned char tGreen =	(*(a+1));
	unsigned char tRed	 =	(*(a+2));

	return Ogre::ColourValue(tRed / 255.0f, tGreen / 255.0f, tBlue / 255.0f);
}

// -----------------------------------------------
bool COTETilePage::LoadAlphamapData(std::string strFileName)
{
	OTE_TRACE("加载LoadAlphamapData ... ")

	if(strFileName.empty())
	{	
		OTEHelper::GetFullPath(m_AlphamapName, m_AlphamapPath, strFileName);	
		m_AlphamapPath += "\\";
	}	

	FILE *p;
	if(!(p = fopen( strFileName.c_str() , "rb" )))
	{
		char filename[256];
		::GetCurrentDirectory(256, filename);

		OTE_MSG_ERR("打开AlphaMap 失败！ File: " << filename <<  strFileName)

		goto _exit_;
	}
	
	fread(&m_Alphabmfh, sizeof(BITMAPFILEHEADER), 1, p);
	if(m_Alphabmfh.bfType != 0x4d42)
	{
		OTE_MSG_ERR("打开AlphaMap 文件格式错误! File: " << strFileName)
		goto _exit_;
	}	

	fread(&m_Alphabmih, sizeof(BITMAPINFOHEADER), 1, p);
	if(m_Alphabmih.biCompression || m_Alphabmih.biBitCount != 24)
	{
		OTE_MSG_ERR("打开AlphaMap 像素格式错误! File: " << strFileName)
		goto _exit_;
	}
	
	// 清理

	if(m_AlphaBmpdata)
		delete [] m_AlphaBmpdata;
	
	m_AlphaBmpdata = new unsigned char[m_Alphabmih.biWidth * m_Alphabmih.biHeight * 3];

	fread(m_AlphaBmpdata, m_Alphabmih.biWidth * m_Alphabmih.biHeight * 3, 1, p);		

	fclose(p);

	OTE_TRACE("加载LoadAlphamapData 完成! ")

	return true;

_exit_:

	if(m_AlphaBmpdata)
		delete [] m_AlphaBmpdata;

	m_AlphaBmpdata = NULL;

	if(p)
		fclose(p);

	return false;
}

// -----------------------------------------------
Ogre::ColourValue COTETilePage::GetAlphaMapColorAt(int x, int y)
{	
	if(!m_AlphaBmpdata && !LoadAlphamapData())				
		return Ogre::ColourValue::Black;	

	if( x < 0 || x >= COTETilePage::m_PageSizeX ||
		y < 0 || y >= COTETilePage::m_PageSizeZ )
		return Ogre::ColourValue::Black;
	
	int offset = (this->m_Alphabmih.biHeight - 
				int(y * COTETilePage::c_AlphamapScale)) * (m_Alphabmih.biWidth) + 
				int(x * COTETilePage::c_AlphamapScale);

	if(offset < 0 || offset >= m_Alphabmih.biHeight * m_Alphabmih.biWidth)
	{
		return Ogre::ColourValue::Black;
	}

	unsigned char* a = &m_AlphaBmpdata[offset * 3];

	int tBlue	=	(*(a))	;
	int tGreen	=	(*(a+1));
	int tRed	=	(*(a+2));
	
	return Ogre::ColourValue(tRed / 255.0f, tGreen / 255.0f, tBlue / 255.0f);

}

//------------------------------------------------------------------------------------
// 从alpha图中分离出各个通道的颜色

void COTETilePage::GetAlphamapChannels(unsigned char* pData[3])
{
	if(!m_AlphaBmpdata && !LoadAlphamapData())
	{
		return;
	}	

	int pixelSize = 3;
	int size = m_Alphabmih.biWidth * m_Alphabmih.biHeight;
	unsigned char* pSrc = m_AlphaBmpdata + (size - m_Alphabmih.biWidth) * pixelSize;

	unsigned char* pDest1 = pData[0];
	unsigned char* pDest2 = pData[1];
	unsigned char* pDest3 = pData[2];
	
	for(int i = 0; i < m_Alphabmih.biHeight; i ++)
	{
		for(int j = 0;	j < m_Alphabmih.biWidth; j ++)
		{		
			*pDest3 ++ = *pSrc ++;
			*pDest2 ++ = *pSrc ++;
			*pDest1 ++ = *pSrc ++;

		}	
		pSrc -= 2 * m_Alphabmih.biWidth * pixelSize;
	}
}

// -----------------------------------------------
COTETile* COTETilePage::TerrainHitTest(const Ogre::Ray& r, Vector3& result)
{
	std::pair<bool, Real> intersection = r.intersects(GetBoundingBox());

	if(intersection.first)
	{
		Vector3 pos = r.getPoint(intersection.second * 1.0001f); // 往前一点点
		
		COTETile* t = GetTileAt(pos.x, pos.z);
		if(t)
		{		
			t->intersectSegment(pos, r.getPoint(MAX_FLOAT), &result);
			return t;
		}		
	}
	return NULL;
}

//-----------------------------------------------------------------------
COTETilePage* COTETilePage::CreatePage()
{
	return new COTETilePage(SCENE_MGR);	
}

//-----------------------------------------------------------------------
// 加载创建山体

void COTETilePage::CreateTerrain(const std::string& szPageName, const std::string& szFileName)
{
	// 加载地形

	HashMap<std::string, COTETilePage*>::iterator it = s_TerrainPageMap.find(szPageName);

	COTETilePage* page;

	if(it == s_TerrainPageMap.end())
	{
		page = CreatePage();	

		// create a root terrain node.		
	
		COTETerrainXmlSerializer::GetSingleton()->LoadTerrain(szFileName, page);
		s_TerrainPageMap[szPageName] = page;

		page->m_PageName = szPageName;

	}
	else
	{	
		it->second->Shutdown();
		
		COTETerrainXmlSerializer::GetSingleton()->LoadTerrain(szFileName,  it->second);		

		page = it->second;
	}

	s_CurrentPage = page;

	std::stringstream ss;

	// 草

	ss << /*OTEHelper::GetResPath("ResConfig", "GrassMapPath") << */page->m_TerrainName << "_VegMap.oraw";

	COTEVegManager::GetInstance()->CreateFromFile(ss.str());

	// 水	

	ss.str("");
	ss << /*OTEHelper::GetResPath("ResConfig", "WaterMapPath") << */page->m_TerrainName << "_WaterMap.raw";
	
	COTELiquidManager::GetSingleton()->LoadLiquidFromFile(ss.str());

	// 天空盒
#ifndef __ZG
	COTESky::GetInstance()->Create();
#endif
}

//-----------------------------------------------------------------------
void COTETilePage::AddTerrainPage(const std::string& name, COTETilePage* page)
{
	assert(page);
	HashMap<std::string, COTETilePage*>::iterator it = s_TerrainPageMap.find(name);

	if(it == s_TerrainPageMap.end())
	{	
		s_TerrainPageMap[name] = page;
		page->m_PageName = name;
	}
	else
	{
		OTE_MSG_ERR("这个名字(" << name << ")的Page已经存在!")
	}
}

//-----------------------------------------------------------------------

void COTETilePage::ShowTerrainBoundingBox(bool bVisible)
{
 //   for (int j = 0; j < (m_PageSizeZ) / (m_TileSize); j++)
	//{
	//	for (int i = 0; i < (m_PageSizeX) / (m_TileSize); i++)
	//	{  
	//		m_pPage->m_Tiles[i][j]->getParentSceneNode()->showBoundingBox(bVisible);
	//	}
	//}
}

//-----------------------------------------------------------------------
COTETilePage* COTETilePage::SwitchTerrainPage(const char* szPageName)
{
	HashMap<std::string, COTETilePage*>::iterator it = s_TerrainPageMap.begin();
	while(it != s_TerrainPageMap.end())
	{
		it->second->ShowTerrain(false);

		++ it;
	}

	s_CurrentPage = s_TerrainPageMap[szPageName];

	if(s_CurrentPage)
	{
		s_CurrentPage->ShowTerrain(true);
	}
	else
	{
		OTE_MSG("切换场景时没有找到场景！ 场景名称：" << szPageName, "失败")		
	}
	
	
	return s_CurrentPage;
}

//-----------------------------------------------------------------------
COTETilePage* COTETilePage::GetCurrentPage()
{
	return s_CurrentPage;
}

//-----------------------------------------------------------------------
void COTETilePage::SetCurrentPage(COTETilePage* page)
{
	s_CurrentPage = page;
}

//-----------------------------------------------------------------------
int COTETilePage::GetLightmapScale()
{
	return c_LightmapScale;
}

//-----------------------------------------------------------------------
// 导入山体

void COTETilePage::ImportTerrain(const std::string& szPageName, const std::string& szFileName)
{
	// 加载地形

	HashMap<std::string, COTETilePage*>::iterator it = s_TerrainPageMap.find(szPageName);

	COTETilePage* page;

	if(it == s_TerrainPageMap.end())
	{
		page = new COTETilePage(SCENE_MGR);	

		// create a root terrain node.
		
		COTETerrainXmlSerializer::GetSingleton()->LoadTerrain(szFileName, page);	

		s_TerrainPageMap[szPageName] = page;

		page->m_PageName = szPageName;

	} 
	else
	{		

		page = it->second;
	}

}

}