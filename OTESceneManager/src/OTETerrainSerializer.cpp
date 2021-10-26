#include "tinyxml.h"
#include "oteterrainserializer.h"
#include "OTETilePage.h"
#include "OTEVegManager.h"
#include "OTEWater.h"
#include "OTEQTSceneManager.h"
#include "OTESceneXmlLoader.h"
#include "OTECommon.h"
#include "OTETerrainMaterialMgr.h"
#include "OTETerrainLightmapMgr.h"

using namespace OTE;
using namespace Ogre;


// ------------------------------------------------------
std::string		COTETerrainXmlSerializer::s_strHeightMapFileName = "";
std::string		COTETerrainXmlSerializer::s_strLightMapFileName = "";
std::string		COTETerrainXmlSerializer::s_strAlphaMapFileName = "";
std::string		COTETerrainXmlSerializer::s_strUVInfoFileName = "";
std::string		COTETerrainXmlSerializer::s_strNormalFileName = "";

// --------------------------------------------------------
COTETerrainXmlSerializer* COTETerrainXmlSerializer::s_pSingleton = NULL;
COTETerrainXmlSerializer* COTETerrainXmlSerializer::GetSingleton()
{
	if(!s_pSingleton) 
	{
		s_pSingleton = new COTETerrainXmlSerializer();		
	}
	return s_pSingleton; 
}

// --------------------------------------------------------
void COTETerrainXmlSerializer::DestroySingleton()
{	
	SAFE_DELETE(s_pSingleton)	
}


// ------------------------------------------------------
COTETerrainXmlSerializer::COTETerrainXmlSerializer(void):
m_NormalMapRawDataPtr(NULL), 
m_HeightMapRawDataPtr(NULL),
m_UVMapRawDataPtr(NULL)
{
}

COTETerrainXmlSerializer::~COTETerrainXmlSerializer(void)
{
}

// ------------------------------------------------------
// 从二进制中加载地形
void COTETerrainXmlSerializer::LoadTerrainByBinary(const std::string& szFileName, COTETilePage* tp)
{
	//// 目标山体对象

	//tp == NULL ? tp = COTETilePage::GetCurrentPage() : NULL;

	//Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(szFileName);

	//tp->m_TerrainName = readString(stream);
	//tp->m_PageSizeZ = tp->m_PageSizeX = atoi(readString(stream).c_str()); // 长宽相等

	//// 高度图
	//s_strHeightMapFileName = readString(stream);
	//
	//// 法线
	//s_strNormalFileName = readString(stream);

	//// UV信息
	//s_strUVInfoFileName = readString(stream);

	//// 贴图信息
	//for(int i = 0; i < TEXTURE_LAYER_COUNT; ++ i)
	//{
	//	tp->m_strDefaultTexs[i] = readString(stream);
	//}

	//for(int p = 0; p < TEXTURE_LAYER_COUNT; ++ p)
	//{
	//	unsigned int size;
	//	readInts(stream, &size, 1);
	//	for (unsigned int i = 0; i < size; ++i)
	//	{
	//		tp->m_TileTexes[p].insert( TileTextureMap_t::value_type(readString(stream), readString(stream)) );
	//	}
	//}	

	//LoadTerrainHeight(s_strHeightMapFileName.c_str());
	//LoadTerrainNormal(s_strNormalFileName.c_str());
	//LoadTerrainTextureInfo(s_strUVInfoFileName.c_str());
	
	//BuildTerrainPage(tp);
}

// ------------------------------------------------------
// 从文件加载
bool COTETerrainXmlSerializer::LoadTerrain(const std::string& szFileName, COTETilePage* tp)
{	
	// 目标山体对象

	tp == NULL ? tp = COTETilePage::GetCurrentPage() : NULL;

	Ogre::DataStreamPtr stream = RESMGR_LOAD(szFileName);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("LoadTerrain 没有找到文件: " << szFileName)
		return false;
	}

	return ParseTerrainXml(stream->getAsString(), tp);

}

/* ------------------------------------------------------
// 从字符串加载
// 格式：

	<terrain name="xxx" pagesizeX="xxx" pagesizeZ="xxx">
		<node heightmap="xxx" />
		<node terrainNormal="xxx" />		
		<node terrainLayer0="xxx" />
		<node terrainLayer1="xxx" />
		<node terrainLayer2="xxx" />
		<node terrainLayer3="xxx" />
		<node TileName0="tile[xx,xx]" pic="xxx" />	
	</terrain>

  ------------------------------------------------------ */
bool COTETerrainXmlSerializer::ParseTerrainXml(const std::string& szXmlString, COTETilePage* tp)
{	
	TileCreateData_t createData;

	// 目标山体对象

	tp == NULL ? tp = COTETilePage::GetCurrentPage() : NULL;

	// 从xml读入
	
	TiXmlDocument doc;
	
	if(!doc.Parse(szXmlString.c_str()))
	{	
		return false;		
	}

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("terrain").Element();
	if(!rootElem)
	{
		return false;		
	}

	const char* szData = rootElem->Attribute("name");
	if(szData)
	{
		createData.name = szData;		
	}
	szData = rootElem->Attribute("pagesizeX");
	if(szData)
	{	
		createData.pageSizeX = atoi(szData);
	}
	szData = rootElem->Attribute("pagesizeZ");
	if(szData)
	{
		createData.pageSizeZ = atoi(szData);
	}
	::TiXmlNode* child = NULL;
	do{			
		child = rootElem->IterateChildren("node",child);	

		// 高度图

		TiXmlElement* elm = child->ToElement();
		if(elm)
		{
			// 高度图

			const char* szData = elm->Attribute("heightmap");
			if(szData)
			{	
				createData.hgtmapName = s_strHeightMapFileName = szData;
				continue;
			}

			// 光照图

			elm = child->ToElement();
			szData = elm->Attribute("terrainLightmap");
			if(szData)
			{	
				createData.lgtmapName = s_strLightMapFileName = szData;
				continue;
			}

			// AlphaMap

			elm = child->ToElement();
			szData = elm->Attribute("terrainAlphamap");
			if(szData)
			{	
				createData.alphamapName = s_strAlphaMapFileName = szData;
				continue;
			}

			//法线信息

			elm = child->ToElement();
			szData = elm->Attribute("terrainNormal");
			if(szData)
			{	
				createData.normalName = s_strNormalFileName = szData;
				continue;
			}		

			// 贴图信息

			for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
			{
				elm = child->ToElement();
				std::stringstream ss;
				ss << "terrainLayer" << i;
				szData = elm->Attribute(ss.str().c_str());
				if(szData)
				{	
					tp->m_strDefaultTexs[i] = szData;
					continue;
				}
			}


			//第一层t单独纹理	

			for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
			{
				elm = child->ToElement();				
				std::stringstream ss;
				ss << "TileName" << i;
				szData = elm->Attribute(ss.str().c_str());

				if(szData)
				{
					std::string tileName = szData;
					std::string tilePic  = elm->Attribute("pic");
					tp->m_TileTexes[i].insert( TileTextureMap_t::value_type(tileName, tilePic) );
					continue;
				}
			}			

		}

	}while(child);


	LoadTerrainHeight(s_strHeightMapFileName.c_str());
	LoadTerrainNormal(s_strNormalFileName.c_str());
//	LoadTerrainTextureInfo(s_strUVInfoFileName.c_str());

	BuildTerrainPage(tp, createData);

	return true;
}

// ------------------------------------------------------
// 加载高度图
void COTETerrainXmlSerializer::LoadTerrainHeight(const std::string& szFileName)
{	
	m_HeightMapRawDataPtr.setNull();
	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		szFileName, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	m_HeightMapRawDataPtr = Ogre::MemoryDataStreamPtr(new Ogre::MemoryDataStream(szFileName, stream));	

}

// ------------------------------------------------------
// 加载法线
void COTETerrainXmlSerializer::LoadTerrainNormal(const std::string& szFileName)
{	
	m_NormalMapRawDataPtr.setNull();
	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		szFileName, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	m_NormalMapRawDataPtr = Ogre::MemoryDataStreamPtr(new Ogre::MemoryDataStream(szFileName, stream));	
}

// ------------------------------------------------------
// 加载UV图
void COTETerrainXmlSerializer::LoadTerrainTextureInfo(const std::string& szFileName)
{
	m_UVMapRawDataPtr.setNull();
	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		szFileName, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	m_UVMapRawDataPtr = Ogre::MemoryDataStreamPtr(new Ogre::MemoryDataStream(szFileName, stream));
	
}


//-------------------------------------------------------------------------
// 创建
void COTETerrainXmlSerializer::BuildTerrainPage(COTETilePage* tp, TileCreateData_t& createData)
{
	// 一些属性

	tp->m_TerrainName = createData.name;
	tp->m_PageSizeX = createData.pageSizeX;
	tp->m_PageSizeZ = createData.pageSizeZ;

	// 尺寸

	int totalPageSize = (tp->m_PageSizeX + 1) * (tp->m_PageSizeZ + 1); 

	// 高度

	if(!createData.heightData)
	{
		createData.heightData = new unsigned char[totalPageSize];
		const unsigned char* pSrc		= m_HeightMapRawDataPtr->getPtr();		
		unsigned char* pDest = createData.heightData;
		for (int j = 0; j < tp->m_PageSizeZ + 1; ++ j)
		{           
			for (int i = 0; i < tp->m_PageSizeX + 1; ++ i)
			{
				*pDest++ = *pSrc++;				//设置高度数据信息
			}
		}	
	}

	// 法线

	if(!createData.normal)
	{
		createData.normal = new Ogre::Vector3[totalPageSize];
		const Ogre::Vector3* pSrcNormal =(Ogre::Vector3*) m_NormalMapRawDataPtr->getPtr();
		Ogre::Vector3* pDestNormal = createData.normal;
		for (int j = 0; j < tp->m_PageSizeZ + 1; ++ j)
		{           
			for (int i = 0; i < tp->m_PageSizeX + 1; ++ i)
			{
				*pDestNormal++ = *pSrcNormal++;	//设置法线信息 Ogre::Vector3(0.0, 1.0, 0.0);//
			}
		}
	}

	// 包围盒

	tp->m_BoundingBox = Ogre::AxisAlignedBox(Ogre::Vector3::ZERO, 
		Ogre::Vector3(
		tp->m_Scale.x * (tp->m_PageSizeX), 
		tp->m_Scale.y, 
		tp->m_Scale.z * (tp->m_PageSizeZ) )
		);
	

	// 创建贴图	

	int pageUnitSizeX = (tp->m_PageSizeX) / tp->m_UnitSize; // 页的材质unit数量
	int pageUnitSizeZ = (tp->m_PageSizeZ) / tp->m_UnitSize; // 页的材质unit数量

	// 地表贴图	

	for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
	{
		createData.layerTexes[i] = tp->m_strDefaultTexs[i];
		createData.tileTexes[i] = &tp->m_TileTexes[i];
	}

	// 光照图

	tp->m_LightmapName = COTETerrainXmlSerializer::s_strLightMapFileName;

	// Alpha

	tp->m_AlphamapName = COTETerrainXmlSerializer::s_strAlphaMapFileName;

	// 建立页

	tp->BuildPage(&createData);

	//if(createData.uvData)		delete [] createData.uvData;
	if(createData.heightData)	delete [] createData.heightData;
	if(createData.normal)		delete [] createData.normal;
}

//-------------------------------------------------------------------------
// 新建场景
void COTETerrainXmlSerializer::CreateNewTerrainPage(TileCreateData_t& createData)
{
	// 创建高度

	int totalPageSize		= (createData.pageSizeX + 1) * (createData.pageSizeZ + 1); 
    createData.heightData	= new unsigned char[totalPageSize]; 
	createData.normal		= new Ogre::Vector3[totalPageSize];

	unsigned char* pDest = createData.heightData;
	Ogre::Vector3* pDestNormal = createData.normal;

	for (int j = 0; j < createData.pageSizeZ + 1; ++ j)
    {           
        for (int i = 0; i < createData.pageSizeX + 1; ++ i)
        {
			*pDest++ = 128;		//设置高度数据信息
			*pDestNormal++ = Ogre::Vector3(0.0f, 1.0f, 0.0f);	//设置法线信息 Ogre::Vector3(0.0, 1.0, 0.0)
        }
    }	

	std::stringstream ss;

	// 创建贴图	

	// int pageUnitSize = (createData.pageSizeX) / 1 /* unitsize temp*/; // 页的材质unit数量

	/*createData.uvData = new CTerrainUnitData[pageUnitSize * pageUnitSize];   
	
	CTerrainUnitData* pUVDest = createData.uvData;


	for (int j = 0; j < pageUnitSize; ++ j)
    {           
        for (int i = 0; i < pageUnitSize; ++ i)
        {		

			for (int ii = 0; ii < 3; ++ ii)
			{
				pUVDest->m_LayerUIndex[ii] = 3;
				pUVDest->m_LayerVIndex[ii] = 2;
			}

			pUVDest ++;		
        }
    }*/

	// 这两个根据高度图命名

	createData.lgtmapName	= createData.hgtmapName + "_lm.bmp";
	createData.alphamapName = createData.hgtmapName + "_am.bmp";

	// 自动生成地形配置文件	
	{
		TiXmlDocument doc("root");
		
		ss.str("");
		ss << "<terrain " << "name = " << "\"" << createData.name << "\"" << " pagesizeX = " << "\"" << createData.pageSizeX << "\"" << " pagesizeZ = " << "\"" << createData.pageSizeZ << "\"" << ">";	
		ss << "\t\n<node heightmap =\""		<< createData.hgtmapName << "\"/>";
		ss << "\t\n<node terrainNormal =\"" << createData.normalName << "\"/>";	
	//	ss << "\t\n<node terrainUVInfo =\"" << createData.uvInfoName << "\"/>";	
		ss << "\t\n<node terrainLightmap =\"" << createData.lgtmapName << "\"/>";	
		ss << "\t\n<node terrainAlphamap =\"" << createData.alphamapName << "\"/>";	
		for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)	
			ss << "\t\n<node terrainLayer" << i << " =\"" << createData.layerTexes[i] << "\"/>";
		
		ss << "</terrain>";	

		doc.Parse(ss.str().c_str());
		doc.SaveFile( (std::string(	OTEHelper::GetResPath("ResConfig", "TerrainXmlPath")) + createData.name + ".ter.xml").c_str() );
	}

	// 自动生成场景配置文件	
	{
		TiXmlDocument doc("root");
			
		ss.str("");
		ss << "<scene>\n\t";	

		ss << "<node type=\"terrain\">";
		ss << "<attribute>\n\t\t";

		std::string terName = createData.name;
		
		ss << "<string name=\"terrainFile\" val=\"" << terName << ".ter.xml" << "\"/>";
					
		ss << "</attribute>\n\t\t";
		ss << "</node>";

		doc.Parse(ss.str().c_str());
		doc.SaveFile( (OTEHelper::GetResPath("ResConfig", "SceneXmlPath") + createData.name + ".xml").c_str() );	
	}
		
	//临时生成高度数据	

	FILE * p;

	p = fopen( (OTEHelper::GetResPath("ResConfig", "HeightMapPath") + createData.hgtmapName).c_str() , "wb" );
	fwrite(createData.heightData, sizeof(unsigned char) * (createData.pageSizeX + 1) * (createData.pageSizeZ + 1), 1, p);
	fclose(p);	

	//保存UV单位数据

	//p = fopen( (OTEHelper::GetResPath("UVInfoPath") + createData.uvInfoName).c_str() , "wb" );
	//fwrite(createData.uvData, sizeof(unsigned char) * pageUnitSize * pageUnitSize * 3 * 2, 1, p);
	//fclose(p);

	//临时生成法线数据	

	p = fopen( (OTEHelper::GetResPath("ResConfig", "NormalPath") + createData.normalName).c_str() , "wb" );
	fwrite(createData.normal, sizeof(Ogre::Vector3) * (createData.pageSizeX + 1) * (createData.pageSizeZ + 1), 1, p);
	fclose(p);	

	std::string lightmapPath = OTEHelper::GetResPath("ResConfig", "LightmapPath");	
	std::string alphamapPath = OTEHelper::GetResPath("ResConfig", "AlphamapPath");

	// 光照图
	{
		int bmpSizeX = (createData.pageSizeX) * COTETilePage::c_LightmapScale;
		int bmpSizeZ = (createData.pageSizeZ) * COTETilePage::c_LightmapScale;
		unsigned char* bmpData = new unsigned char[bmpSizeX * bmpSizeZ * 3]; // 24 位
		::ZeroMemory(bmpData, bmpSizeX * bmpSizeZ * 3);

		ss.str("");
		ss << lightmapPath << "\\" << createData.lgtmapName;

		OTEHelper::SaveToBmp(ss.str().c_str(), bmpData, bmpSizeX, bmpSizeZ, 24);	

		delete []bmpData;
	}

	// Alpha图
	{
		int bmpSizeX = (createData.pageSizeX) * COTETilePage::c_AlphamapScale;
		int bmpSizeZ = (createData.pageSizeZ) * COTETilePage::c_AlphamapScale;
		unsigned char* bmpData = new unsigned char[bmpSizeX * bmpSizeZ * 3]; // 24 位
		::ZeroMemory(bmpData, bmpSizeX * bmpSizeZ * 3);

		ss.str("");
		ss << alphamapPath << "\\" << createData.alphamapName;

		OTEHelper::SaveToBmp(ss.str().c_str(), bmpData, bmpSizeX, bmpSizeZ, 24);

		delete []bmpData;
	}

	//水文件			
	
	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "WaterMapPath") << createData.name << "_WaterMap.raw";
	COTELiquidManager::GetSingleton()->CreateEmptyFile(ss.str(), createData.pageSizeX, createData.pageSizeZ);

	//草文件	
	
	unsigned char* pVegMap = new unsigned char [( createData.pageSizeX ) * ( createData.pageSizeZ)];
	memset(pVegMap, 0, sizeof(unsigned char) * ( createData.pageSizeX) * ( createData.pageSizeZ) );

	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "GrassMapPath") << createData.name << "_VegMap.raw";
	p = fopen( ss.str().c_str(), "wb" );
	fwrite(pVegMap, ( createData.pageSizeX) * ( createData.pageSizeZ) * sizeof(unsigned char), 1, p);
	fclose(p);	
	
	delete []pVegMap;

	// 清理

	//delete [] createData.uvData;
	delete [] createData.heightData;
	delete [] createData.normal;
}

// -------------------------------------------------------------------------
// 保存山体

void COTETerrainXmlSerializer::SaveTerrain(const std::string& szFileName, COTETilePage* tp)
{	
	// 先保存数据

	ExportTerrainPage(tp);

	// 保存场景配置

	TiXmlDocument doc("terrain");

	std::stringstream ss;
	ss << "<terrain " << "name = " << "\""	<< tp->m_TerrainName		<< "\"" << " pagesizeX = " << "\"" << tp->m_PageSizeX << "\"" << " pagesizeZ = " << "\"" << tp->m_PageSizeZ << "\"" << ">";	
	ss << "\t\n<node heightmap =\""			<< s_strHeightMapFileName	<< "\"/>";
	ss << "\t\n<node terrainLightmap =\""	<< s_strLightMapFileName	<< "\"/>";
	ss << "\t\n<node terrainAlphamap=\""	<< s_strAlphaMapFileName	<< "\"/>";	 
	ss << "\t\n<node terrainNormal =\""		<< s_strNormalFileName		<< "\"/>";	
//	ss << "\t\n<node terrainUVInfo =\""		<< s_strUVInfoFileName		<< "\"/>";
	for( int i = 0; i < TEXTURE_LAYER_COUNT; i ++)	
		ss << "\t\n<node terrainLayer" << i << " =\"" << tp->m_strDefaultTexs[i] << "\"/>";
	
	//第一层tile纹理

	TileCreateData_t::TileTextureMap_t::iterator itr;
	for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
	{
		for(itr = tp->m_TileTexes[i].begin(); itr != tp->m_TileTexes[i].end(); itr++)
		{
			ss << "\t\n<node TileName" << i << "=\"" + itr->first + "\" pic=\"" + itr->second + "\" />";
		}
	}
	
	ss << "</terrain>";	

	doc.Parse(ss.str().c_str());
	doc.SaveFile(szFileName);	

	// 保存草数据

	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "GrassMapPath") << tp->m_TerrainName << "_VegMap.raw";
	COTEVegManager::GetInstance()->SaveToFile(ss.str());

	// 保存水贴图

	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "WaterMapPath") << tp->m_TerrainName << "_WaterMap.raw";
	COTELiquidManager::GetSingleton()->SaveLiquidToFile(ss.str());

}

// -------------------------------------------------------------------------
// 保存数据
void COTETerrainXmlSerializer::ExportTerrainPage(COTETilePage* tp)
{
	TileCreateData_t createData;	

	int totalPageSize		= (tp->m_PageSizeX + 1) * (tp->m_PageSizeZ + 1); 
	createData.heightData	= new unsigned char[totalPageSize];			// 高度
	createData.normal		= new Ogre::Vector3[totalPageSize];
	//int pageUnitSizeX		= (tp->m_PageSizeX) / tp->m_UnitSize;	// 页的材质unit数量
	//int pageUnitSizeZ		= (tp->m_PageSizeZ) / tp->m_UnitSize;	// 页的材质unit数量
	//createData.uvData		= new CTerrainUnitData[pageUnitSizeX * pageUnitSizeZ];   

	tp->ExportData(&createData);
	
	if(s_strHeightMapFileName.empty())
	{
		s_strHeightMapFileName	= tp->m_TerrainName + "_hm.raw";
		s_strNormalFileName		= tp->m_TerrainName + "_nm.raw";
		//s_strUVInfoFileName		= tp->m_TerrainName + "_uv.raw";
		s_strLightMapFileName = s_strHeightMapFileName + "_lm.bmp";
		s_strAlphaMapFileName = s_strHeightMapFileName + "_am.bmp";
	}

	SaveTerrainHeight(tp->m_PageSizeX + 1, tp->m_PageSizeZ + 1, createData.heightData, s_strHeightMapFileName.c_str());
	SaveTerrainNormal(tp->m_PageSizeX + 1, tp->m_PageSizeZ + 1, createData.normal, s_strNormalFileName.c_str());
	//SaveTerrainTextureInfo(tp, (unsigned char*)createData.uvData, s_strUVInfoFileName.c_str());

	//delete [] createData.uvData;
	delete [] createData.heightData;
	delete [] createData.normal;

	// Alphamap

	COTETerrainMaterialMgr::GetInstance()->SaveAlphamap(tp);
	COTETerrainLightmapMgr::GetInstance()->SaveLightMapFile(tp);
}

// ------------------------------------------------------
// 保存高度图
void COTETerrainXmlSerializer::SaveTerrainHeight(int width, int height, unsigned char* pData, const std::string& szFileName)//lyhraw	
{	
	std::string workPath = OTEHelper::GetResPath("ResConfig", "HeightMapPath");
	workPath += szFileName;
	FILE* file = fopen(workPath.c_str(), "wb");
	int totalPageSize = width * height;
	fwrite(pData, totalPageSize, 1, file);
	fclose(file);
}



// ------------------------------------------------------
// 保存法线信息
void COTETerrainXmlSerializer::SaveTerrainNormal(int width, int height, Ogre::Vector3* pData, const std::string& szFileName)
{	
	std::string workPath = OTEHelper::GetResPath("ResConfig", "NormalPath");	
	workPath += szFileName;
	FILE* file = fopen(workPath.c_str(), "wb");
	int totalPageSize = width * height;
	fwrite(pData, sizeof(Ogre::Vector3) * totalPageSize, 1, file);
	fclose(file);
}

// ------------------------------------------------------
// 保存UV图
void COTETerrainXmlSerializer::SaveTerrainTextureInfo(COTETilePage* tp, unsigned char* pData, const std::string& szFileName)
{
	std::string workPath = OTEHelper::GetResPath("ResConfig", "UVInfoPath");	
	workPath += szFileName;
	FILE* file = fopen(workPath.c_str(), "wb");
	int pageUnitSizeX = (tp->m_PageSizeX) / tp->m_UnitSize; // 页的材质unit数量
	int pageUnitSizeZ = (tp->m_PageSizeZ) / tp->m_UnitSize; // 页的材质unit数量

	fwrite(pData, pageUnitSizeX * pageUnitSizeZ * sizeof(CTerrainUnitData), 1, file);
	fclose(file);	
}

// ------------------------------------------------------
void COTETerrainXmlSerializer::heightMapDeleteDirt(int iHeight, int iWidth, int Sensitivity, COTETilePage* tp)
{
	if(s_strHeightMapFileName == "")
		return;

	LoadTerrainHeight(s_strHeightMapFileName.c_str());
	LoadTerrainNormal(s_strNormalFileName.c_str());

	TileCreateData_t createData;

	// 创建高度
	int totalPageSize = iHeight * iWidth; 
    createData.heightData = new unsigned char[totalPageSize];	

	const unsigned char* pSrc = m_HeightMapRawDataPtr->getPtr();	
	
	unsigned char* pDest = createData.heightData;
	

	// unsigned char maxHgt = 0;
	for (int j = 0; j < iHeight; ++ j)
    {           
        for (int i = 0; i < iWidth; ++ i)
        {		
			*pDest++ = *pSrc++;		//设置高度数据信息
        }
    }

	pDest = createData.heightData;//指针归为初始位，后面再次调用


	//去尘
	int TemplateSize = 1;//2	//参与比较的范围，1代表当前象素周围的1个象素，总共是3X3，如果参数是2，则是5X5
	int SortNum = 5;//12		//取最接近当前象素的个数，一般来说取范围内的象素总数的一半左右，太大和太小都不是很好
	//int Sensitivity = 4;	//置了一个敏感度，当前象素和最接近的象素的差值大于该值才进行运算
	long X;				long Y;
	long X1;			long Y1;
	long TemplNum;
	unsigned char* RGB;			unsigned char* DRGB;
	unsigned char L;			unsigned char M;		int P;
	long XBegin;		long YBegin;
	long XEnd;			long YEnd;
	long XBegin1;		long YBegin1;
	long XEnd1;			long YEnd1;


	TemplNum = (TemplateSize * 2 + 1) * (TemplateSize * 2 + 1);	//总范围点的数量
	RGB = new unsigned char[TemplNum];
	DRGB = new unsigned char[TemplNum];

	XBegin = TemplateSize;
	YBegin = TemplateSize;
	XEnd = iWidth - TemplateSize - 1;
	YEnd = iHeight - TemplateSize - 1;

	for( X = XBegin; X <= XEnd; X++)
	{
		XBegin1 = X - TemplateSize;
		XEnd1 = X + TemplateSize;
		for( Y = YBegin; Y <= YEnd; Y++)
		{
			YBegin1 = Y - TemplateSize;
			YEnd1 = Y + TemplateSize;
			//for( I = 0; I <= 2; I++)	//原图片去尘用的三个RGB值
			{
				
				L = 0;
				
				M = pDest[Y * iWidth + X];
				for( X1 = XBegin1; X1 <= XEnd1; X1++)
				{
					for( Y1 = YBegin1; Y1 <= YEnd1; Y1++)
					{						
						RGB[L] = pDest[Y1 * iWidth + X1];//获取颜色
						
						DRGB[L] = abs(RGB[L] - M);	//作为比较，提取较接近的点
						
						L = L + 1;
					}
				}
				
				if( DRGB[TemplateSize] <= Sensitivity || DRGB[TemplateSize + 2] <= Sensitivity || DRGB[TemplateSize + 4] <= Sensitivity || DRGB[TemplateSize + 6] <= Sensitivity)	//如果是附合要求的点则更新
				{
					P = 0;
					//for( L = TemplateSize + 1; L <= SortNum + TemplateSize - 1; L++)
					for( L = 0; L < 9; L++)
					{
						P = P + RGB[L];
					}
					//P = P + pDest[Y * iWidth + X];
					P = int(P / (9));//SortNum9
					pDest[Y * iWidth + X] = P;
				}
			}
		}
	}

	delete [] RGB;
	delete [] DRGB;	


	//重新构建地形


	tp->Shutdown();
	pDest = m_HeightMapRawDataPtr->getPtr(); 
	pSrc  = createData.heightData;
	for (int j = 0; j < iHeight; ++ j)
    {           
        for (int i = 0; i < iWidth; ++ i)
        {		
			*pDest++ = *pSrc++;		//设置高度数据信息
        }
    }
	//LoadTerrainHeight(s_strHeightMapFileName.c_str());
	LoadTerrainTextureInfo(s_strUVInfoFileName.c_str());	

	createData.name			= tp->m_TerrainName;
	createData.pageSizeX	= tp->m_PageSizeX;	
	createData.pageSizeZ	= tp->m_PageSizeZ;

	BuildTerrainPage(tp, createData);

	//去尘end
	delete [] createData.heightData;
}

