/************************************************************
 ************************************************************

 模块名称： 地表的光照图与碰撞图技术
 相关模块： OTETile, OTETilePage, 碰撞模块
						
 描述    ： 光照图是在地表之上的一层贴图, 是非适时渲染的.

 *************************************************************
 *************************************************************/

#include "oteterrainlightmapmgr.h"
#include "OTETerrainSerializer.h"
#include "OTETile.h"
#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"

// ----------------------------------------------
// TEMP! unselName == "_border_" 与悬崖上侧的边缘相关
// 使用 Ent好一些

// ----------------------------------------------
using namespace Ogre;
using namespace OTE;

// ----------------------------------------------
COTETerrainLightmapMgr COTETerrainLightmapMgr::s_Inst;
COTETerrainLightmapMgr* COTETerrainLightmapMgr::GetInstance()
{	
	return &s_Inst;
}


//------------------------------------------------------
// 碰撞

bool COTETerrainLightmapMgr::_IntersectObjects(const Ray& r, float raylen, const std::string& unselName)
{	
	COTEEntity* ent = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(ent)
	{
		if(!r.intersects(ent->getWorldBoundingBox()).first)
			return false;

		IntersectInfo_t ii = COTEEntityPhysicsMgr::GetTriangleIntersection(ent, r, ALPHATEST);  
		if(ii.isHit && (ii.hitPos - r.getOrigin()).length() < raylen) 
		{		
			return 1;
		}
		return 0;
	}

	return COTEObjectIntersection::TriangleIntersection(r, "Ent", unselName, raylen, ALPHATEST).first != 0;

}

//------------------------------------------------------
bool COTETerrainLightmapMgr::_IntersectObjects(const Ray& r, Vector3& hitPos, float raylen, const std::string& unselName)
{		
	// 选中的物体

	COTEEntity* ent = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(ent && ent->getName().find("Lgt_") == std::string::npos)
	{
		if(!r.intersects(ent->getWorldBoundingBox()).first)
			return false;

		IntersectInfo_t ii = COTEEntityPhysicsMgr::GetTriangleIntersection(ent, r, ALPHATEST);  
		if(ii.isHit && (ii.hitPos - r.getOrigin()).length() < raylen) 
		{
			hitPos = ii.hitPos;
			return 1;
		}
		return 0;
	}

	// 全部

	std::pair<MovableObject*,Vector3> result = COTEObjectIntersection::TriangleIntersection(r, "Ent", unselName, raylen, ALPHATEST);
	if(result.first)
	{
		hitPos = result.second;
		return 1;
	}
	return 0;

}

// ----------------------------------------------
void COTETerrainLightmapMgr::SetLightMapMaterial(COTETile* tile, const std::string& rLightmapName)
{
	tile->SetLightMap(rLightmapName, 1); // 如果使用shader 可能会有错误！
}

// ----------------------------------------------
//动态大小光照图

void COTETerrainLightmapMgr::GenLightmapA(COTETilePage* page,
							const CRectA<int>& rect,
							const Vector3& fSunDir,
							const ColourValue& diffuse, 						
							const ColourValue& ambient,	
							eCoverMode CoverMode, unsigned char* pBase0,
							bool bRenderShadow,
							GetLightInforHndler_t	 pLightInforHndler)
{
	OTE_LOGMSG("计算地表光照图 ...")

	OTE_ASSERT(rect.sx >= 0);
	OTE_ASSERT(rect.sz >= 0);	
	OTE_ASSERT(rect.ex <= page->m_LightmapBmih.biWidth);
	OTE_ASSERT(rect.ez <= page->m_LightmapBmih.biHeight);

	int imgHeight = page->m_LightmapBmih.biHeight; // 暂时

	int tileSize = (page->m_TileSize) * COTETilePage::c_LightmapScale;

	// 光照图一个像素到3d空间的比例

	float scale_x = 1.0f / COTETilePage::c_LightmapScale * page->m_Scale.x;	
	float scale_z = 1.0f / COTETilePage::c_LightmapScale * page->m_Scale.z;	

	for(int i = rect.sx; i < rect.ex; i ++)
	for(int j = rect.sz; j < rect.ez; j ++)
	{
		COTETile* tile = page->GetTileAt(i / tileSize, j / tileSize);
		if(!tile)
			continue;

		Vector3 fVertex;
		fVertex.x = i * scale_x;
		fVertex.z = j * scale_z;
		fVertex.y = tile->GetHeightAt(fVertex.x, fVertex.z) + 0.125f; // 小偏移		
					
		Ogre::ColourValue tLightCor		=	Ogre::ColourValue::White;
		float			   blend_alpha	=	1.0f;

		if(pLightInforHndler)
		{
			Ogre::Vector3		_position;
			Ogre::Vector3		_direction = fSunDir;
			Ogre::ColourValue	_diffuse   = diffuse;
			float				reflectDis;			
			float				reflectDis0;	

			(*pLightInforHndler)(fVertex, _position, _direction, _diffuse, reflectDis, reflectDis0, blend_alpha);
		
			Ogre::Ray ray(fVertex, - _direction);

			int   hits = 0;				
			Ogre::Vector3 hitPos;
			float hitDis = 0.0f;
			bool  isHit = false;

			if(bRenderShadow)
			{
				isHit = COTECollisionManager::IntersectMap(hitPos, tile->m_pParentPage, fVertex, ray, 0.25f, hits, reflectDis);
				if(!isHit)
				{			
					hits  =  5;			
					bool tHit = _IntersectObjects(ray, hitPos, reflectDis, "_border_");
					if(tHit)
					{							
						//hitDis = (hitPos - fVertex).length();							
						isHit = true;				
					}	
				}
				else
				{
					//hitDis = (hitPos - fVertex).length();		
				}
			}

			// 光照图

			if (isHit)
			{
				// in the shadow

				//Ogre::Vector3 normal;			
				//tile->_GetNormalAt( fVertex.x, fVertex.z, &normal );
				//float factor = -fSunDir.dotProduct( normal );

				//factor *= ( (((hitDis + 8.0f) / 200.0f) + (1.0f / hits)) / 2.0f ); // 用于渐变
				//
				//Ogre::ColourValue delta = diffuse * (factor > 0.0f ? factor : 0.0f);										
				
				tLightCor = ambient;
			}
			else
			{
				Ogre::Vector3 normal;			
				tile->_GetNormalAt(fVertex.x, fVertex.z, &normal);
				
				float factor = -_direction.dotProduct( normal );
				
				tLightCor = ambient + _diffuse * (factor > 0.0f ? factor : 0.0f); // color			
			}		

		}
		else
		{
			Ogre::Ray ray(fVertex, -fSunDir);					
			
			int   hits = 0;				
			Ogre::Vector3 hitPos;
			float hitDis = 0.0f;

			bool  isHit = COTECollisionManager::IntersectMap(hitPos, tile->m_pParentPage, fVertex, ray, 0.25f, hits);
			if(!isHit && bRenderShadow)
			{			
				hits  =  5;			
				bool tHit = _IntersectObjects(ray, hitPos, MAX_FLOAT, "_border_");
				if(tHit)
				{							
					hitDis = (hitPos - fVertex).length();							
					isHit = true;				
				}	
			}
			else
			{
				hitDis = (hitPos - fVertex).length();		
			}

			// 光照图

			if ( isHit )
			{
				// in the shadow

				//Ogre::Vector3 normal;			
				//tile->_GetNormalAt( fVertex.x, fVertex.z, &normal );
				//float factor = -fSunDir.dotProduct( normal );

				//factor *= ( (((hitDis + 8.0f) / 200.0f) + (1.0f / hits)) / 2.0f ); // 用于渐变
				//
				//Ogre::ColourValue delta = diffuse * (factor > 0.0f ? factor : 0.0f);										
				
				tLightCor = ambient;
			}
			else
			{
				Ogre::Vector3 normal;			
				tile->_GetNormalAt(fVertex.x, fVertex.z, &normal);
				float factor = -fSunDir.dotProduct( normal );
				
				tLightCor = ambient + diffuse * (factor > 0.0f ? factor : 0.0f); // color			
			}
		}

		Ogre::ColourValue vCor = tLightCor * (CoverMode != E_COVER ? 0.5f : 1.0f);	

		vCor.r < 0.0f ? vCor.r = 0.0f : NULL;
		vCor.g < 0.0f ? vCor.g = 0.0f : NULL;
		vCor.b < 0.0f ? vCor.b = 0.0f : NULL;
		
		vCor.r > 1.0f ? vCor.r = 1.0f : NULL;
		vCor.g > 1.0f ? vCor.g = 1.0f : NULL;
		vCor.b > 1.0f ? vCor.b = 1.0f : NULL;		

		setLightmapColorAt(
			page, 
			i, 
			j, 
			vCor.r * 255, 
			vCor.g * 255, 
			vCor.b * 255, 
			blend_alpha, 
			CoverMode,
			pBase0);
	}

	OTE_LOGMSG("计算地表光照图完成！")

}

//------------------------------------------------------------------------------------
void COTETerrainLightmapMgr::GenLightmap( COTETilePage* page, 
											    const Vector3& lightDir,
												const ColourValue& diffuse, const ColourValue& ambient,
												eCoverMode coverMode,
												bool bRenderShadow,
												GetLightInforHndler_t	 pLightInforHndler)
{	
	// 把上一次的计算数据保存起来

	m_Ambient	= ambient;

	m_Diffuse	= diffuse;

	m_lightDir	= lightDir;

	// 计算光照图 

	int s_xTileIndex = 0;
	int s_zTileIndex = 0;	
			
	if(page->m_LightmapPtr.isNull())
	{
		//OTE_MSG("光照图无效！", "失败")	
		page->m_LightmapPtr = Ogre::TextureManager::getSingleton().getByName(page->m_LightmapName);
		if(page->m_LightmapPtr.isNull())
		{
			page->m_LightmapPtr = Ogre::TextureManager::getSingleton().createManual(
				page->m_LightmapName, 
				RES_GROUP,
				Ogre::TEX_TYPE_2D, 
				COTETilePage::c_LightmapScale * (page->m_PageSizeX), 
				COTETilePage::c_LightmapScale * (page->m_PageSizeZ), 
				24, -1, 
				Ogre::PF_R8G8B8);
		}
	}

	// 特别光源 可能有范围限制

	CRectA<int> rect(0, 0, (int)page->m_LightmapBmih.biWidth, (int)page->m_LightmapBmih.biHeight);

	if(pLightInforHndler)
	{	
		// 计算影响范围
			
		Ogre::Vector3		_position; 
		Ogre::Vector3		_direction = Ogre::Vector3::NEGATIVE_UNIT_X;
		Ogre::ColourValue	_diffuse   = diffuse;		
		float				blend_alpha;
		float				reflectDis;	
		float				reflectDis0;

		(*pLightInforHndler)(Ogre::Vector3::ZERO, _position, _direction, _diffuse, reflectDis, reflectDis0, blend_alpha);
		float lm_x = _position.x / page->m_Scale.x * page->c_LightmapScale;
		float lm_z = _position.z / page->m_Scale.z * page->c_LightmapScale;
		reflectDis0 = reflectDis0 * page->c_LightmapScale;
		
		rect.setRect(
			lm_x > reflectDis0 ? lm_x - reflectDis0 : 0,
			lm_z > reflectDis0 ? lm_z - reflectDis0 : 0, 
			lm_x + reflectDis0 < page->m_LightmapBmih.biWidth  ? lm_x + reflectDis0 : page->m_LightmapBmih.biWidth , 
			lm_z + reflectDis0 < page->m_LightmapBmih.biHeight ? lm_z + reflectDis0 : page->m_LightmapBmih.biHeight
			);	

	}

	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_LightmapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 
	GenLightmapA(	page,
					rect,
					lightDir, diffuse, ambient,
					coverMode, pBase0, bRenderShadow, pLightInforHndler);

	page->m_LightmapPtr->getBuffer(0, 0)->unlock();

	//SaveLightMapFile(page);	
	
}

//------------------------------------------------------------------------------------
void COTETerrainLightmapMgr::SaveLightMapFile( COTETilePage* page, 
												 const std::string& lightmapPath,
												 const std::string& lightmapName )
{
	std::string str = lightmapPath + lightmapName;
	
	FILE * p;
	p = fopen( str.c_str() , "wb" );
	if(!p)			
	{
		::MessageBox(NULL, "光照图写打开失败！保存失败。", "失败", MB_OK);
		return;
	}

	fwrite(&page->m_LightmapBmfh,  sizeof(BITMAPFILEHEADER), 1, p);
	fwrite(&page->m_LightmapBmih,  sizeof(BITMAPINFOHEADER), 1, p);

	fwrite( page->m_LightmapData,  sizeof(unsigned char) * page->m_LightmapBmih.biWidth * page->m_LightmapBmih.biHeight * 3, 1, p);

	fclose(p);

	// 把光信息记录在文件里
	
	std::string lightmapInfo = page->m_PageName + "_LM.txt";
	FILE* file = fopen((page->m_LightmapPath + lightmapInfo).c_str(), "w");
	if(!file)
	{
		OTE_MSG_ERR("打开文件失败！" << page->m_LightmapPath + lightmapInfo)
		return;
	}
	fprintf(file, "场景： %s\n", page->m_PageName.c_str());
	fprintf(file, "环境光： %.2f %.2f %.2f\n",		m_Ambient.r, m_Ambient.g, m_Ambient.b);
	fprintf(file, "漫反射： %.2f %.2f %.2f\n",		m_Diffuse.r, m_Diffuse.g, m_Diffuse.b);
	fprintf(file, "光的方位： %.2f %.2f %.2f\n",	m_lightDir.x, m_lightDir.y, m_lightDir.z);

	fclose(file);

	OTE_TRACE("光照图被顺利保存!")
}

//------------------------------------------------------------------------------------
void COTETerrainLightmapMgr::SaveLightMapFile(COTETilePage* page)
{
	OTE_TRACE("尝试保存本场景光照图...")

	if( !page || 
		!page->m_LightmapData ||
		 page->m_LightmapBmih.biWidth == 0
		)
	{
		::MessageBox(NULL, "光照图数据为空！保存失败。", "失败", MB_OK);
		return;
	}

	// 文件

	SaveLightMapFile(page, page->m_LightmapPath, page->m_LightmapName);
}

//------------------------------------------------------------------------------------
void COTETerrainLightmapMgr::SetLightMapMaterial(COTETilePage* page)
{			
	page->m_LightmapPtr = TextureManager::getSingleton().getByName(page->m_LightmapName);

	// 更新光照图

	if(page->m_LightmapPtr.isNull())
	{
		page->m_LightmapPtr = TextureManager::getSingleton().load(page->m_LightmapName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}	

	int tilesPerLineX = (page->m_PageSizeX - 1) / (page->m_TileSize - 1);
	int tilesPerLineZ = (page->m_PageSizeZ - 1) / (page->m_TileSize - 1);

	for ( int j = 0 ; j < tilesPerLineZ; j++ )	
	{
		for ( int i = 0; i < tilesPerLineX; i++ )
		{			
			SetLightMapMaterial(page->GetTileAt(i, j), page->m_LightmapName);
		}
	}
}

//------------------------------------------------------------------------------------
void COTETerrainLightmapMgr::SetLightMapMaterial(COTETilePage* page, 
												   const std::string& lightMapName,
												   const std::string& lightmapPath
												   )
{
	Ogre::TextureManager::getSingleton().remove(page->m_LightmapName);	
	Ogre::TextureManager::getSingleton().unload(page->m_LightmapName);

	page->m_LightmapPath = lightmapPath;

	page->m_LightmapName = lightMapName;

	SetLightMapMaterial(page);
	
}

//------------------------------------------------------------------------------------
bool COTETerrainLightmapMgr::LoadLightMapData(COTETilePage* page, std::string strFileName)
{
	return page->LoadLightmapData(strFileName);
}

//------------------------------------------------------------------------------------
// 加载光照图数据
bool COTETerrainLightmapMgr::LoadLightMapData(COTETilePage* page)
{
	return page->LoadLightmapData();
}

//
////////////////////////////////////////////////////////////////////
//////////////////THE SHADOW AND LIGHTMAP///////////////////////////
////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------
// 更新光照图缓存
// x, z 为像素单位
void COTETerrainLightmapMgr::setLightmapColorAt(COTETilePage* page,  int x, int z,
												unsigned char nRed, unsigned char nGreen,unsigned char nBlue, float bindFactor,
												eCoverMode CoverMode, unsigned char* pBase0)
{	
	int offset = (page->m_LightmapBmih.biHeight - z - 1) * (page->m_LightmapBmih.biWidth) + x;
	if(offset < 0 || offset >= page->m_LightmapBmih.biHeight * page->m_LightmapBmih.biWidth)
	{
		return;
	}

	unsigned char* a = &page->m_LightmapData[offset * 3];

	int tBlue	= (*(a	  ));
	int tGreen	= (*(a + 1));
	int tRed	= (*(a + 2));

	switch(CoverMode)
	{
		case E_SUB:		//	sub
		{
			tRed-=nRed,tGreen-=nGreen,tBlue-=nBlue;
		}break;
		case E_COVER:	//	replace		
		{
			tRed = nRed,tGreen = nGreen,tBlue = nBlue;
		}break;
		case E_ADD:		//	add
		{
			tRed += nRed,tGreen += nGreen,tBlue += nBlue;
		}break;
		case E_ADDGRAY:	//	add the gray
		{
			int tgray=(tRed+tGreen+tBlue) / 3;
			tRed = tgray+nRed,tGreen = tgray+nGreen,tBlue = tgray+nBlue;
		}break;
		case E_BLIND:	//bind
		{
			tRed   = tRed * (1 - bindFactor) + nRed * bindFactor;
			tGreen = tGreen * (1 - bindFactor) + nGreen * bindFactor;
			tBlue  = tBlue * (1 - bindFactor) + nBlue * bindFactor;
		}break;

		default:
			break;
	}

	tRed  = tRed<255 ? tRed:255; 
	tRed  = tRed>0 ? tRed:0;
	tGreen= tGreen<255 ? tGreen:255; 
	tGreen= tGreen>0 ? tGreen:0;
	tBlue = tBlue<255 ? tBlue:255; 
	tBlue = tBlue>0 ? tBlue:0;

	//更新lightmap数据颜色	
	
	*a       = (unsigned char)tBlue;	//蓝	
	*(a + 1) = (unsigned char)tGreen;	//绿	
	*(a + 2) = (unsigned char)tRed;		//红

	// 更新显存中数据

	int offset_in_buffer = z * (page->m_LightmapBmih.biWidth) + x;

	if(offset_in_buffer < 0 || offset_in_buffer >= page->m_LightmapBmih.biHeight * page->m_LightmapBmih.biWidth)
	{
		return;
	}	

	updateLightmapBuffer(offset_in_buffer, pBase0, (unsigned char)tRed, (unsigned char)tGreen, (unsigned char)tBlue);	

}

// ----------------------------------------------------------------------
void COTETerrainLightmapMgr::updateLightmapBuffer(int offset_in_buffer, unsigned char* pBase0, 
												  unsigned char red, unsigned char green, unsigned char blue)
{		
	unsigned char* pBase = pBase0 + offset_in_buffer * 4;   
	
	*(pBase    )	=	blue;
	*(pBase + 1)	=	green;
	*(pBase + 2)	=	red;
}

// ----------------------------------------------------------------------
// 设置光照

void COTETerrainLightmapMgr::setLightmapColorAt(COTETilePage* page, float x, float z, 
												const Ogre::ColourValue& rCor, eCoverMode CoverMode, int brushSize)
{
	if(!page->m_LightmapData || page->m_LightmapPtr.isNull())
		return;
		
	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_LightmapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 		
	x = x / page->m_Scale.x * COTETilePage::c_LightmapScale; 
	z = z / page->m_Scale.z * COTETilePage::c_LightmapScale;

	for( int i = 0; i < brushSize; i ++)
	for( int j = 0; j < brushSize; j ++)
	{
		setLightmapColorAt(page, 
			x + i,	
			z + j, 
			rCor.r * 255, rCor.g * 255, rCor.b * 255, 1.0f, CoverMode, pBase0);
	}

	page->m_LightmapPtr->getBuffer(0, 0)->unlock();
	
}

// ----------------------------------------------------------------------
// 设置光照

void COTETerrainLightmapMgr::setLightmapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, eCoverMode CoverMode, 
												const unsigned char* pCorRawData, int brushSize, float brushStrength)
{
	if(!page->m_LightmapData || page->m_LightmapPtr.isNull())
		return;
	
	x = x / page->m_Scale.x * COTETilePage::c_LightmapScale; 
	z = z / page->m_Scale.z * COTETilePage::c_LightmapScale;

	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_LightmapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 
	for( int j = 0; j < brushSize; j ++)
	for( int i = 0; i < brushSize; i ++)		
	{
		float f = pCorRawData[i + j * brushSize] / 255.0f;

		setLightmapColorAt(page, x + i,	z + j, 
			rCor.r * 255, rCor.g * 255, rCor.b * 255, 
			f * brushStrength, CoverMode, pBase0);
	}

	page->m_LightmapPtr->getBuffer(0, 0)->unlock();
	
}


// ----------------------------------------------
// 刷新光照图 

void COTETerrainLightmapMgr::RefreshLightmapMaterial(COTETilePage* page)
{
	Ogre::TextureManager::getSingleton().unload(page->m_LightmapName);
	
	int tilesPerLineX = (page->m_PageSizeX) / (page->m_TileSize);
	int tilesPerLineZ = (page->m_PageSizeZ) / (page->m_TileSize);
	
	for( int j = 0 ; j < tilesPerLineZ; j++ )
	{
		for( int i = 0; i < tilesPerLineX; i++ )
		{			
			SetLightMapMaterial(page->GetTileAt( i, j), page->m_LightmapName);
		}
	}
}

// ----------------------------------------------

Ogre::ColourValue COTETerrainLightmapMgr::PickLightmapColor(COTETilePage* page, float x, float z)
{
	return page->GetLightMapColorAt(x, z);
}

// ----------------------------------------------
// 光照图模糊算法

void COTETerrainLightmapMgr::Blur(COTETilePage* page, float cx, float cz, int brushSize)
{	
	if(!page || page->m_LightmapPtr.isNull())
	{
		return;
	}
	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_LightmapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 	
	int xstart = 1;
	int xend = page->m_LightmapBmih.biWidth - 1;
	int zstart = 1;
	int zend = page->m_LightmapBmih.biHeight - 1;

	float scale = 1.0f / page->m_Scale.x * COTETilePage::c_LightmapScale;

	if(brushSize != -1)
	{
		xstart = cx * scale - brushSize / 2;
		zstart = cz * scale - brushSize / 2;
		xend   = xstart + brushSize;
		zend   = zstart + brushSize;
		
		xstart < 1 ? xstart = 1 : NULL;
		zstart < 1 ? zstart = 1 : NULL;
		xend > page->m_LightmapBmih.biWidth - 1  ? xend = page->m_LightmapBmih.biWidth - 1 : NULL;
		zend > page->m_LightmapBmih.biHeight - 1 ? zend = page->m_LightmapBmih.biHeight - 1 : NULL;
	}

	for(int x = xstart; x < xend; x ++)
	for(int z = zstart; z < zend; z ++)
	{
		int offset = (page->m_LightmapBmih.biHeight - z - 1) * (page->m_LightmapBmih.biWidth) + x;
		if(offset < 0 || offset >= page->m_LightmapBmih.biHeight * page->m_LightmapBmih.biWidth)
		{
			return;
		}

		unsigned char* a = &page->m_LightmapData[offset * 3];		
		unsigned int Blue = (*(a	));
		unsigned int Green= (*(a + 1));
		unsigned int Red  = (*(a + 2));		
		
		int offset1 = (page->m_LightmapBmih.biHeight - z - 1) * (page->m_LightmapBmih.biWidth) + x + 1;
		unsigned char* right = &page->m_LightmapData[offset1 * 3];
		unsigned int rBlue = (*(right	));
		unsigned int rGreen= (*(right + 1));
		unsigned int rRed  = (*(right + 2));	

		offset1 = (page->m_LightmapBmih.biHeight - z - 1) * (page->m_LightmapBmih.biWidth) + x - 1;
		unsigned char* left = &page->m_LightmapData[offset1 * 3];
		unsigned int lBlue = (*(left	));
		unsigned int lGreen= (*(left + 1));
		unsigned int lRed  = (*(left + 2));	

		offset1 = (page->m_LightmapBmih.biHeight - z - 1 - 1) * (page->m_LightmapBmih.biWidth) + x;
		unsigned char* top = &page->m_LightmapData[offset1 * 3];
		unsigned int tBlue = (*(top	));
		unsigned int tGreen= (*(top + 1));
		unsigned int tRed  = (*(top + 2));	

		offset1 = (page->m_LightmapBmih.biHeight - z - 1 + 1) * (page->m_LightmapBmih.biWidth) + x;
		unsigned char* bottom = &page->m_LightmapData[offset1 * 3];
		unsigned int bBlue = (*(bottom	));
		unsigned int bGreen= (*(bottom + 1));
		unsigned int bRed  = (*(bottom + 2));

		// 平均

		Red = (lRed + rRed + tRed + bRed + 3 * Red) / 7;
		Green = (lGreen + rGreen + tGreen + bGreen + 3 * Green) / 7;
		Blue = (lBlue + rBlue + tBlue + bBlue + 3 * Blue) / 7;

		Red  = Red<255 ? Red:255; 
		Red  = Red>0 ? Red:0;
		Green= Green<255 ? Green:255; 
		Green= Green>0 ? Green:0;
		Blue = Blue<255 ? Blue:255; 
		Blue = Blue>0 ? Blue:0;

		//更新lightmap数据颜色	
		
		*a       = (unsigned char)Blue; //蓝	
		*(a + 1) = (unsigned char)Green;//绿	
		*(a + 2) = (unsigned char)Red;  //红

		// 更新显存中数据

		int offset_in_buffer = z * (page->m_LightmapBmih.biWidth) + x;

		if(offset_in_buffer < 0 || offset_in_buffer >= page->m_LightmapBmih.biHeight * page->m_LightmapBmih.biWidth)
		{
			return;
		}	

		updateLightmapBuffer(offset_in_buffer, pBase0, (unsigned char)Red, (unsigned char)Green, (unsigned char)Blue);	
	}

	page->m_LightmapPtr->getBuffer(0, 0)->unlock();

	if(brushSize == -1)
		SaveLightMapFile(page);

}

//------------------------------------------------------------------------------------
// 逐个tile 计算光照

void COTETerrainLightmapMgr::GenLightmapOnTiles( 
				COTETilePage* page, float x, float z, int brushSize,
				const Vector3& lightDir,
				const ColourValue& diffuse, const Ogre::ColourValue& ambient,			  
				eCoverMode CoverMode)
{	
				
	if(page->m_LightmapPtr.isNull())
	{	
		page->m_LightmapPtr = Ogre::TextureManager::getSingleton().createManual(
			page->m_LightmapName, 
			RES_GROUP,
			Ogre::TEX_TYPE_2D, 
			COTETilePage::c_LightmapScale * (page->m_PageSizeX), 
			COTETilePage::c_LightmapScale * (page->m_PageSizeZ), 
			24, -1, 
			Ogre::PF_R8G8B8);		
	}

	if(!page->m_LightmapData)
	{
		page->LoadLightmapData();
	}

	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_LightmapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 		
	//	3d到光照图的变换

	float scale = 1.0f / page->m_Scale.x * COTETilePage::c_LightmapScale;	

	int centerX = x * scale;
	int centerZ = z * scale;

	CRectA<int> rect(
		(centerX - brushSize / 2), 
		(centerZ - brushSize / 2), 
		(centerX + brushSize / 2), 
		(centerZ + brushSize / 2)
		);
	rect.sx < 0 ? rect.sx = 0 : NULL;
	rect.sz < 0 ? rect.sz = 0 : NULL;
	rect.ex > page->m_LightmapBmih.biWidth ? rect.ex = page->m_LightmapBmih.biWidth : NULL;
	rect.ez > page->m_LightmapBmih.biWidth ? rect.ez = page->m_LightmapBmih.biHeight : NULL;	

	GenLightmapA(page, 
			rect,
			lightDir,
			diffuse, ambient, 
			CoverMode,
			pBase0
			);
	
	page->m_LightmapPtr->getBuffer(0, 0)->unlock();	

}

//----------------------------------------------------------------------------
// 碰撞图

void COTETerrainLightmapMgr::GenCollisionMap(COTETilePage* page, unsigned char* pCollisionMap)
{	
	int imgWidth	= page->m_LightmapBmih.biWidth; // 暂时
	int imgHeight	= page->m_LightmapBmih.biHeight; // 暂时

	int tileSize = (page->m_TileSize) * 2;

	float scale = 0.5f * page->m_Scale.x; // 碰撞图尺寸 / 高度图尺寸 * 缩放比	

	for(int j = 0; j < imgHeight; j ++)
	for(int i = 0; i < imgWidth; i ++)
	{
		COTETile* tile = page->GetTileAt(i / tileSize, j / tileSize);
		float upperdis = 10.0f; // 高于地面
		float belowdis = 2.0f; // 地面以下

		Vector3 fVertex;
		fVertex.x = i * scale;
		fVertex.z = j * scale;
		fVertex.y = tile->GetHeightAt(fVertex.x, fVertex.z);			

		Ray ray(Vector3(fVertex.x, fVertex.y - belowdis, fVertex.z), Vector3(0, 1, 0));
		
		pCollisionMap[((imgHeight - j) * imgWidth + i) * 3	  ] = 0xFF;
		pCollisionMap[((imgHeight - j) * imgWidth + i) * 3 + 1] = 0xFF;
		pCollisionMap[((imgHeight - j) * imgWidth + i) * 3 + 2] = 0xFF;
		pCollisionMap[((imgHeight - j) * imgWidth + i) * 3 + 3] = 0xFF;
		//setLightmapbuffer(tile, i, j, 0xFF, 0xFF, 0xFF, 0);

		if(_IntersectObjects(ray, upperdis + belowdis))
		{
			pCollisionMap[((imgHeight - j) * imgWidth + i) * 3	  ] = 0;
			pCollisionMap[((imgHeight - j) * imgWidth + i) * 3 + 1] = 0;
			pCollisionMap[((imgHeight - j) * imgWidth + i) * 3 + 2] = 0;
			pCollisionMap[((imgHeight - j) * imgWidth + i) * 3 + 3] = 0;
			//setLightmapbuffer(tile, i, j, 0, 0, 0, 0);
		}
	}

	
}

//----------------------------------------------------------------------------
// 碰撞图

void COTETerrainLightmapMgr::GenCollisionMap(COTETilePage* page)
{	
	if(page->m_LightmapData)
		GenCollisionMap(page, page->m_LightmapData);
	
}