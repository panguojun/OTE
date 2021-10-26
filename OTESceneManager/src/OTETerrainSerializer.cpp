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
// �Ӷ������м��ص���
void COTETerrainXmlSerializer::LoadTerrainByBinary(const std::string& szFileName, COTETilePage* tp)
{
	//// Ŀ��ɽ�����

	//tp == NULL ? tp = COTETilePage::GetCurrentPage() : NULL;

	//Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(szFileName);

	//tp->m_TerrainName = readString(stream);
	//tp->m_PageSizeZ = tp->m_PageSizeX = atoi(readString(stream).c_str()); // �������

	//// �߶�ͼ
	//s_strHeightMapFileName = readString(stream);
	//
	//// ����
	//s_strNormalFileName = readString(stream);

	//// UV��Ϣ
	//s_strUVInfoFileName = readString(stream);

	//// ��ͼ��Ϣ
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
// ���ļ�����
bool COTETerrainXmlSerializer::LoadTerrain(const std::string& szFileName, COTETilePage* tp)
{	
	// Ŀ��ɽ�����

	tp == NULL ? tp = COTETilePage::GetCurrentPage() : NULL;

	Ogre::DataStreamPtr stream = RESMGR_LOAD(szFileName);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("LoadTerrain û���ҵ��ļ�: " << szFileName)
		return false;
	}

	return ParseTerrainXml(stream->getAsString(), tp);

}

/* ------------------------------------------------------
// ���ַ�������
// ��ʽ��

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

	// Ŀ��ɽ�����

	tp == NULL ? tp = COTETilePage::GetCurrentPage() : NULL;

	// ��xml����
	
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

		// �߶�ͼ

		TiXmlElement* elm = child->ToElement();
		if(elm)
		{
			// �߶�ͼ

			const char* szData = elm->Attribute("heightmap");
			if(szData)
			{	
				createData.hgtmapName = s_strHeightMapFileName = szData;
				continue;
			}

			// ����ͼ

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

			//������Ϣ

			elm = child->ToElement();
			szData = elm->Attribute("terrainNormal");
			if(szData)
			{	
				createData.normalName = s_strNormalFileName = szData;
				continue;
			}		

			// ��ͼ��Ϣ

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


			//��һ��t��������	

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
// ���ظ߶�ͼ
void COTETerrainXmlSerializer::LoadTerrainHeight(const std::string& szFileName)
{	
	m_HeightMapRawDataPtr.setNull();
	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		szFileName, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	m_HeightMapRawDataPtr = Ogre::MemoryDataStreamPtr(new Ogre::MemoryDataStream(szFileName, stream));	

}

// ------------------------------------------------------
// ���ط���
void COTETerrainXmlSerializer::LoadTerrainNormal(const std::string& szFileName)
{	
	m_NormalMapRawDataPtr.setNull();
	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		szFileName, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	m_NormalMapRawDataPtr = Ogre::MemoryDataStreamPtr(new Ogre::MemoryDataStream(szFileName, stream));	
}

// ------------------------------------------------------
// ����UVͼ
void COTETerrainXmlSerializer::LoadTerrainTextureInfo(const std::string& szFileName)
{
	m_UVMapRawDataPtr.setNull();
	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		szFileName, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	m_UVMapRawDataPtr = Ogre::MemoryDataStreamPtr(new Ogre::MemoryDataStream(szFileName, stream));
	
}


//-------------------------------------------------------------------------
// ����
void COTETerrainXmlSerializer::BuildTerrainPage(COTETilePage* tp, TileCreateData_t& createData)
{
	// һЩ����

	tp->m_TerrainName = createData.name;
	tp->m_PageSizeX = createData.pageSizeX;
	tp->m_PageSizeZ = createData.pageSizeZ;

	// �ߴ�

	int totalPageSize = (tp->m_PageSizeX + 1) * (tp->m_PageSizeZ + 1); 

	// �߶�

	if(!createData.heightData)
	{
		createData.heightData = new unsigned char[totalPageSize];
		const unsigned char* pSrc		= m_HeightMapRawDataPtr->getPtr();		
		unsigned char* pDest = createData.heightData;
		for (int j = 0; j < tp->m_PageSizeZ + 1; ++ j)
		{           
			for (int i = 0; i < tp->m_PageSizeX + 1; ++ i)
			{
				*pDest++ = *pSrc++;				//���ø߶�������Ϣ
			}
		}	
	}

	// ����

	if(!createData.normal)
	{
		createData.normal = new Ogre::Vector3[totalPageSize];
		const Ogre::Vector3* pSrcNormal =(Ogre::Vector3*) m_NormalMapRawDataPtr->getPtr();
		Ogre::Vector3* pDestNormal = createData.normal;
		for (int j = 0; j < tp->m_PageSizeZ + 1; ++ j)
		{           
			for (int i = 0; i < tp->m_PageSizeX + 1; ++ i)
			{
				*pDestNormal++ = *pSrcNormal++;	//���÷�����Ϣ Ogre::Vector3(0.0, 1.0, 0.0);//
			}
		}
	}

	// ��Χ��

	tp->m_BoundingBox = Ogre::AxisAlignedBox(Ogre::Vector3::ZERO, 
		Ogre::Vector3(
		tp->m_Scale.x * (tp->m_PageSizeX), 
		tp->m_Scale.y, 
		tp->m_Scale.z * (tp->m_PageSizeZ) )
		);
	

	// ������ͼ	

	int pageUnitSizeX = (tp->m_PageSizeX) / tp->m_UnitSize; // ҳ�Ĳ���unit����
	int pageUnitSizeZ = (tp->m_PageSizeZ) / tp->m_UnitSize; // ҳ�Ĳ���unit����

	// �ر���ͼ	

	for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
	{
		createData.layerTexes[i] = tp->m_strDefaultTexs[i];
		createData.tileTexes[i] = &tp->m_TileTexes[i];
	}

	// ����ͼ

	tp->m_LightmapName = COTETerrainXmlSerializer::s_strLightMapFileName;

	// Alpha

	tp->m_AlphamapName = COTETerrainXmlSerializer::s_strAlphaMapFileName;

	// ����ҳ

	tp->BuildPage(&createData);

	//if(createData.uvData)		delete [] createData.uvData;
	if(createData.heightData)	delete [] createData.heightData;
	if(createData.normal)		delete [] createData.normal;
}

//-------------------------------------------------------------------------
// �½�����
void COTETerrainXmlSerializer::CreateNewTerrainPage(TileCreateData_t& createData)
{
	// �����߶�

	int totalPageSize		= (createData.pageSizeX + 1) * (createData.pageSizeZ + 1); 
    createData.heightData	= new unsigned char[totalPageSize]; 
	createData.normal		= new Ogre::Vector3[totalPageSize];

	unsigned char* pDest = createData.heightData;
	Ogre::Vector3* pDestNormal = createData.normal;

	for (int j = 0; j < createData.pageSizeZ + 1; ++ j)
    {           
        for (int i = 0; i < createData.pageSizeX + 1; ++ i)
        {
			*pDest++ = 128;		//���ø߶�������Ϣ
			*pDestNormal++ = Ogre::Vector3(0.0f, 1.0f, 0.0f);	//���÷�����Ϣ Ogre::Vector3(0.0, 1.0, 0.0)
        }
    }	

	std::stringstream ss;

	// ������ͼ	

	// int pageUnitSize = (createData.pageSizeX) / 1 /* unitsize temp*/; // ҳ�Ĳ���unit����

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

	// ���������ݸ߶�ͼ����

	createData.lgtmapName	= createData.hgtmapName + "_lm.bmp";
	createData.alphamapName = createData.hgtmapName + "_am.bmp";

	// �Զ����ɵ��������ļ�	
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

	// �Զ����ɳ��������ļ�	
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
		
	//��ʱ���ɸ߶�����	

	FILE * p;

	p = fopen( (OTEHelper::GetResPath("ResConfig", "HeightMapPath") + createData.hgtmapName).c_str() , "wb" );
	fwrite(createData.heightData, sizeof(unsigned char) * (createData.pageSizeX + 1) * (createData.pageSizeZ + 1), 1, p);
	fclose(p);	

	//����UV��λ����

	//p = fopen( (OTEHelper::GetResPath("UVInfoPath") + createData.uvInfoName).c_str() , "wb" );
	//fwrite(createData.uvData, sizeof(unsigned char) * pageUnitSize * pageUnitSize * 3 * 2, 1, p);
	//fclose(p);

	//��ʱ���ɷ�������	

	p = fopen( (OTEHelper::GetResPath("ResConfig", "NormalPath") + createData.normalName).c_str() , "wb" );
	fwrite(createData.normal, sizeof(Ogre::Vector3) * (createData.pageSizeX + 1) * (createData.pageSizeZ + 1), 1, p);
	fclose(p);	

	std::string lightmapPath = OTEHelper::GetResPath("ResConfig", "LightmapPath");	
	std::string alphamapPath = OTEHelper::GetResPath("ResConfig", "AlphamapPath");

	// ����ͼ
	{
		int bmpSizeX = (createData.pageSizeX) * COTETilePage::c_LightmapScale;
		int bmpSizeZ = (createData.pageSizeZ) * COTETilePage::c_LightmapScale;
		unsigned char* bmpData = new unsigned char[bmpSizeX * bmpSizeZ * 3]; // 24 λ
		::ZeroMemory(bmpData, bmpSizeX * bmpSizeZ * 3);

		ss.str("");
		ss << lightmapPath << "\\" << createData.lgtmapName;

		OTEHelper::SaveToBmp(ss.str().c_str(), bmpData, bmpSizeX, bmpSizeZ, 24);	

		delete []bmpData;
	}

	// Alphaͼ
	{
		int bmpSizeX = (createData.pageSizeX) * COTETilePage::c_AlphamapScale;
		int bmpSizeZ = (createData.pageSizeZ) * COTETilePage::c_AlphamapScale;
		unsigned char* bmpData = new unsigned char[bmpSizeX * bmpSizeZ * 3]; // 24 λ
		::ZeroMemory(bmpData, bmpSizeX * bmpSizeZ * 3);

		ss.str("");
		ss << alphamapPath << "\\" << createData.alphamapName;

		OTEHelper::SaveToBmp(ss.str().c_str(), bmpData, bmpSizeX, bmpSizeZ, 24);

		delete []bmpData;
	}

	//ˮ�ļ�			
	
	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "WaterMapPath") << createData.name << "_WaterMap.raw";
	COTELiquidManager::GetSingleton()->CreateEmptyFile(ss.str(), createData.pageSizeX, createData.pageSizeZ);

	//���ļ�	
	
	unsigned char* pVegMap = new unsigned char [( createData.pageSizeX ) * ( createData.pageSizeZ)];
	memset(pVegMap, 0, sizeof(unsigned char) * ( createData.pageSizeX) * ( createData.pageSizeZ) );

	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "GrassMapPath") << createData.name << "_VegMap.raw";
	p = fopen( ss.str().c_str(), "wb" );
	fwrite(pVegMap, ( createData.pageSizeX) * ( createData.pageSizeZ) * sizeof(unsigned char), 1, p);
	fclose(p);	
	
	delete []pVegMap;

	// ����

	//delete [] createData.uvData;
	delete [] createData.heightData;
	delete [] createData.normal;
}

// -------------------------------------------------------------------------
// ����ɽ��

void COTETerrainXmlSerializer::SaveTerrain(const std::string& szFileName, COTETilePage* tp)
{	
	// �ȱ�������

	ExportTerrainPage(tp);

	// ���泡������

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
	
	//��һ��tile����

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

	// ���������

	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "GrassMapPath") << tp->m_TerrainName << "_VegMap.raw";
	COTEVegManager::GetInstance()->SaveToFile(ss.str());

	// ����ˮ��ͼ

	ss.str("");
	ss << OTEHelper::GetResPath("ResConfig", "WaterMapPath") << tp->m_TerrainName << "_WaterMap.raw";
	COTELiquidManager::GetSingleton()->SaveLiquidToFile(ss.str());

}

// -------------------------------------------------------------------------
// ��������
void COTETerrainXmlSerializer::ExportTerrainPage(COTETilePage* tp)
{
	TileCreateData_t createData;	

	int totalPageSize		= (tp->m_PageSizeX + 1) * (tp->m_PageSizeZ + 1); 
	createData.heightData	= new unsigned char[totalPageSize];			// �߶�
	createData.normal		= new Ogre::Vector3[totalPageSize];
	//int pageUnitSizeX		= (tp->m_PageSizeX) / tp->m_UnitSize;	// ҳ�Ĳ���unit����
	//int pageUnitSizeZ		= (tp->m_PageSizeZ) / tp->m_UnitSize;	// ҳ�Ĳ���unit����
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
// ����߶�ͼ
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
// ���淨����Ϣ
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
// ����UVͼ
void COTETerrainXmlSerializer::SaveTerrainTextureInfo(COTETilePage* tp, unsigned char* pData, const std::string& szFileName)
{
	std::string workPath = OTEHelper::GetResPath("ResConfig", "UVInfoPath");	
	workPath += szFileName;
	FILE* file = fopen(workPath.c_str(), "wb");
	int pageUnitSizeX = (tp->m_PageSizeX) / tp->m_UnitSize; // ҳ�Ĳ���unit����
	int pageUnitSizeZ = (tp->m_PageSizeZ) / tp->m_UnitSize; // ҳ�Ĳ���unit����

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

	// �����߶�
	int totalPageSize = iHeight * iWidth; 
    createData.heightData = new unsigned char[totalPageSize];	

	const unsigned char* pSrc = m_HeightMapRawDataPtr->getPtr();	
	
	unsigned char* pDest = createData.heightData;
	

	// unsigned char maxHgt = 0;
	for (int j = 0; j < iHeight; ++ j)
    {           
        for (int i = 0; i < iWidth; ++ i)
        {		
			*pDest++ = *pSrc++;		//���ø߶�������Ϣ
        }
    }

	pDest = createData.heightData;//ָ���Ϊ��ʼλ�������ٴε���


	//ȥ��
	int TemplateSize = 1;//2	//����Ƚϵķ�Χ��1����ǰ������Χ��1�����أ��ܹ���3X3�����������2������5X5
	int SortNum = 5;//12		//ȡ��ӽ���ǰ���صĸ�����һ����˵ȡ��Χ�ڵ�����������һ�����ң�̫���̫С�����Ǻܺ�
	//int Sensitivity = 4;	//����һ�����жȣ���ǰ���غ���ӽ������صĲ�ֵ���ڸ�ֵ�Ž�������
	long X;				long Y;
	long X1;			long Y1;
	long TemplNum;
	unsigned char* RGB;			unsigned char* DRGB;
	unsigned char L;			unsigned char M;		int P;
	long XBegin;		long YBegin;
	long XEnd;			long YEnd;
	long XBegin1;		long YBegin1;
	long XEnd1;			long YEnd1;


	TemplNum = (TemplateSize * 2 + 1) * (TemplateSize * 2 + 1);	//�ܷ�Χ�������
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
			//for( I = 0; I <= 2; I++)	//ԭͼƬȥ���õ�����RGBֵ
			{
				
				L = 0;
				
				M = pDest[Y * iWidth + X];
				for( X1 = XBegin1; X1 <= XEnd1; X1++)
				{
					for( Y1 = YBegin1; Y1 <= YEnd1; Y1++)
					{						
						RGB[L] = pDest[Y1 * iWidth + X1];//��ȡ��ɫ
						
						DRGB[L] = abs(RGB[L] - M);	//��Ϊ�Ƚϣ���ȡ�Ͻӽ��ĵ�
						
						L = L + 1;
					}
				}
				
				if( DRGB[TemplateSize] <= Sensitivity || DRGB[TemplateSize + 2] <= Sensitivity || DRGB[TemplateSize + 4] <= Sensitivity || DRGB[TemplateSize + 6] <= Sensitivity)	//����Ǹ���Ҫ��ĵ������
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


	//���¹�������


	tp->Shutdown();
	pDest = m_HeightMapRawDataPtr->getPtr(); 
	pSrc  = createData.heightData;
	for (int j = 0; j < iHeight; ++ j)
    {           
        for (int i = 0; i < iWidth; ++ i)
        {		
			*pDest++ = *pSrc++;		//���ø߶�������Ϣ
        }
    }
	//LoadTerrainHeight(s_strHeightMapFileName.c_str());
	LoadTerrainTextureInfo(s_strUVInfoFileName.c_str());	

	createData.name			= tp->m_TerrainName;
	createData.pageSizeX	= tp->m_PageSizeX;	
	createData.pageSizeZ	= tp->m_PageSizeZ;

	BuildTerrainPage(tp, createData);

	//ȥ��end
	delete [] createData.heightData;
}

