#include "stdafx.h"
#include "OTESceneFile.h"
#include "OTEStaticInclude.h"
#include "Plugin_ZHFileConvertor.h"
#include "SectorDlg.h"
#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"
#include "OTETerrainSerializer.h"
#include "OTESceneXmlLoader.h"
#include "OTETerrainMaterialMgr.h"
#include "OTETerrainLightmapMgr.h"
#include "OTESceneFile.h"
#include "OTESky.h"
#include "OTEWater.h"
#include "OTEVegManager.h"
#include "OTEEntityVertexColorPainter.h"
#include "MagicEntity.h"

using namespace Ogre; 
using namespace OTE; 

// ------------------------------------------------
// 偏移值

std::string g_SceneName;
float g_StartPosX = 0;					
float g_StartPosZ = 0;

// ------------------------------------------------
// CSectorDlg 对话框
IMPLEMENT_DYNAMIC(CSectorDlg, CDialog)
CSectorDlg::CSectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSectorDlg::IDD, pParent)
{
}

CSectorDlg::~CSectorDlg()
{
}

void CSectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_SectorNameList);
}


BEGIN_MESSAGE_MAP(CSectorDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

// CSectorDlg 消息处理程序

void CSectorDlg::OnLbnSelchangeList1()
{
}

// -----------------------------------------
BOOL CSectorDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	m_SectorNameList.ResetContent();

	FILE* file = fopen("SecotrInfo.cfg", "r");

	if(!file)
	{
		::MessageBox(NULL, "没找到文件：　SecotrInfo.cfg", "", MB_OK);			
	}
	else
	{
		char szLine[256];

		while (fgets (szLine, 256, file))
		{
			if (!strncmp (szLine, "//", 2))
				continue;		

			m_SectorNameList.AddString(szLine);				
		}
	}

	fclose(file);
		
	return ret;
}

// -----------------------------------------
// 加载纵横场景
#ifdef OGRE_THREAD_SUPPORT
	Ogre::CriSection OGRE_AUTO_MUTEX_NAME;
#endif
// -----------------------------------------
void LoadScene(const std::string& sceneName, int sx, int sz)
{	
	g_SceneName = sceneName;
	g_StartPosX = sx * 2.0f;
	g_StartPosZ = sz * 2.0f;

	// 设置场景名

	SCENE_MGR->SetUserData("SectorName", sceneName);

	SCENE_MGR->clearScene();

	FILE* sceneFile=fopen(OTEHelper::GetOTESetting("ZHSettings", "SceneFile"), "rb");
	if(!sceneFile)
	{
		MessageBox(NULL,"场景文件没有找到!","错误",0);
		return;
	}		

	// 地皮

	COTETilePage* tp = COTETilePage::CreatePage();	
	COTETilePage::SetCurrentPage(tp);
	tp->SetVal("StartPosX", g_StartPosX);
	tp->SetVal("StartPosZ", g_StartPosZ);

	tp->m_PageSizeX = tp->m_PageSizeZ = 512;
	tp->m_PageName = "zhtest";	
	tp->m_Scale.y = 500.0f;	

	// 创建高度

	int totalPageSize = (tp->m_PageSizeX + 1) * (tp->m_PageSizeZ + 1); 

	TileCreateData_t createData;
	createData.fHeightData = new float[totalPageSize]; 
	::ZeroMemory(createData.fHeightData, totalPageSize);

	createData.normal = new Ogre::Vector3[totalPageSize];		
	::ZeroMemory(createData.normal, totalPageSize);

	int pageSize = ( COTETilePage::GetCurrentPage()->m_PageSizeX - 0 );
	unsigned char* pAlphaBmpdata = new unsigned char[pageSize * pageSize * 2 * 2 * 3];

	unsigned char* pLightBmpdata = new unsigned char[pageSize * pageSize * 4 * 4 * 3];

	int tileSize = ( COTETilePage::GetCurrentPage()->m_TileSize );
	int size = ( COTETilePage::GetCurrentPage()->m_PageSizeX - 0 ) / tileSize;
	float waterMaxHgt = COTETilePage::GetCurrentPage()->m_Scale.y;

	unsigned int* pLiquidMap = new unsigned int [size * size];
	::ZeroMemory(pLiquidMap, size * size * sizeof(unsigned int));

	// 地皮相关尺寸
	
	// 按tile尺寸分别加载

	tp->m_PageName += Ogre::StringConverter::toString(sx);
	tp->m_PageName += ",";
	tp->m_PageName += Ogre::StringConverter::toString(sz);

	tp->m_PageName = sceneName; // 场景名

	for(int j = sz / TILE_SIZE; j < sz / TILE_SIZE + 64; j ++)
	for(int i = sx / TILE_SIZE; i < sx / TILE_SIZE + 64; i ++)
	{
		//read scene file

		TileRaw raw ;
		fseek(sceneFile,sizeof(TileRaw) * (j * ROW_TILES + i),0);
		fread(&raw, sizeof(TileRaw),1,sceneFile); 

		// 物件

		for(int ii=0; ii < 4; ii++)
		{					
			char* nodeName = raw.entities[ii].sNodeName;	

			if(strcmp(nodeName, "") != 0)
			{
				Ogre::Vector3 sOffset = Ogre::Vector3(sx * 2.0f, 0, sz * 2.0f);

				std::string meshName = OTEHelper::ScanfStringAt(nodeName, 4, '.', '.') + ".mesh";
				Ogre::Vector3 vec(
					(raw.entities[ii].fMax[0] - raw.entities[ii].fMin[0]),
					(raw.entities[ii].fMax[1] - raw.entities[ii].fMin[1]),
					(raw.entities[ii].fMax[2] - raw.entities[ii].fMin[2]) 
					);

#ifndef GAME_DEBUG1	

				std::string entName = std::string("Ent_") + nodeName;

				std::string lightmapName = entName + "_LM.bmp";

				if(CHECK_RES(lightmapName))
				{
					COTEActorEntity* ae = SCENE_MGR->CreateEntity(
															meshName, entName,
															Ogre::Vector3::ZERO,
															Ogre::Quaternion::IDENTITY,
															Ogre::Vector3::UNIT_SCALE,
															"default",
															true,
															false
															);

					SCENE_MGR->AttachObjToSceneNode(ae);
					ae->getParentSceneNode()->setPosition(Ogre::Vector3(raw.entities[ii].fPos[0], raw.entities[ii].fPos[1], raw.entities[ii].fPos[2]) - sOffset);
					ae->getParentSceneNode()->setOrientation(raw.entities[ii].fRot[3], raw.entities[ii].fRot[0], raw.entities[ii].fRot[1], raw.entities[ii].fRot[2]);		
					ae->getParentSceneNode()->setScale(raw.entities[ii].fScale + 1, raw.entities[ii].fScale + 1, raw.entities[ii].fScale + 1);
				}
				else
				{		
					
					COTEAutoEntityMgr::GetInstance()->AppendMesh(meshName, 
						Ogre::Vector3(raw.entities[ii].fPos[0], raw.entities[ii].fPos[1], raw.entities[ii].fPos[2]) - sOffset,
						Ogre::Vector3(raw.entities[ii].fScale + 1, raw.entities[ii].fScale + 1, raw.entities[ii].fScale + 1),
						Ogre::Quaternion(raw.entities[ii].fRot[3], raw.entities[ii].fRot[0], raw.entities[ii].fRot[1], raw.entities[ii].fRot[2])
						);
				}

#else
							
				std::string entName = std::string("Ent_") + nodeName;

				COTEActorEntity* ae = SCENE_MGR->CreateEntity(
											meshName, entName,
											Ogre::Vector3::ZERO,
											Ogre::Quaternion::IDENTITY,
											Ogre::Vector3::UNIT_SCALE,
											"default",
											false,
											false
											);

				//WAIT_LOADING_RES(ae)
				SCENE_MGR->AttachObjToSceneNode(ae);//, nodeName, "default");  
				ae->getParentSceneNode()->setPosition(Ogre::Vector3(raw.entities[ii].fPos[0], raw.entities[ii].fPos[1], raw.entities[ii].fPos[2]) - sOffset);
				ae->getParentSceneNode()->setOrientation(raw.entities[ii].fRot[3], raw.entities[ii].fRot[0], raw.entities[ii].fRot[1], raw.entities[ii].fRot[2]);		
				ae->getParentSceneNode()->setScale(raw.entities[ii].fScale + 1, raw.entities[ii].fScale + 1, raw.entities[ii].fScale + 1);
				
				// 保卫和无限大

				//Ogre::AxisAlignedBox bounds(- MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT, MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
				//ae->getMesh()->_setBounds(bounds);

				const Ogre::AxisAlignedBox& bounds = ae->getMesh()->getBounds();

				if((bounds.getMaximum() - bounds.getMinimum()).length() > 40)
				{
					const Ogre::Vector3& center = ae->getParentSceneNode()->getPosition();
					Ogre::AxisAlignedBox bounds(Ogre::Vector3(-20, -20, -20), Ogre::Vector3(20, 20, 20));
					ae->getMesh()->_setBounds(bounds);

					ae->getParentSceneNode()->_update(true, true);
				}
				
				// 顶点色

				//COTEEntityVertexColorPainter::SetVertexColor(ae, Ogre::ColourValue::White);
				
				// 光
				//COTEShadowMap::AddShadowObj((COTEActorEntity*)ae);	
#endif
			}
		}
		
		// 特效
		
		if(raw.magic.magTemName[0] != '\0')
		{
			Ogre::Vector3 sOffset = Ogre::Vector3(sx * 2.0f, 0, sz * 2.0f);

			static int sMagicCnt = 0;
			std::stringstream  ss;
			ss << "Mag_" << ++ sMagicCnt; 
			OTE::COTEMagicEntity* magicEnt = (OTE::COTEMagicEntity*)OTE::COTEMagicEntity::CreateMagicEntity(ss.str().c_str(), raw.magic.magTemName);
			magicEnt->getParentSceneNode()->setPosition(Ogre::Vector3(raw.magic.fPos[0], raw.magic.fPos[1], raw.magic.fPos[2]) - sOffset);
			magicEnt->getParentSceneNode()->setOrientation(raw.magic.fRot[0], raw.magic.fRot[1], raw.magic.fRot[2], raw.magic.fRot[3]);
			magicEnt->getParentSceneNode()->setScale(raw.magic.fScale, raw.magic.fScale, raw.magic.fScale);
		}

		// 地皮

		for(int jj=0;jj<TILE_SIZE;jj++)
		for(int ii=0;ii<TILE_SIZE;ii++)
		{	
			int px=(i - sx / TILE_SIZE ) * TILE_SIZE+ii;
			int py=(j - sz / TILE_SIZE ) * TILE_SIZE+jj; 
			if(px<0) px=0;
			if(py<0) py=0;
			if(px>=tp->m_PageSizeX + 1) px=tp->m_PageSizeX;
			if(py>=tp->m_PageSizeZ + 1) py=tp->m_PageSizeZ;

			createData.fHeightData[py * (tp->m_PageSizeX + 1) + px] = raw.height[jj][ii];

			// 水

			if(raw.waterheight > 0)
			{
				float waterHeight = raw.waterheight - ((int)raw.waterheight / 500) * 500;					
				int val = int((waterHeight / waterMaxHgt) * MAX_WATER_HGT);
				
				pLiquidMap[int(py / tileSize ) * size + int(px / tileSize)] = unsigned int(val) + 0 ;
			}
		}	

		// AlphaMap

		for(int jj=0;jj<TILE_SIZE*2;jj++)
		for(int ii=0;ii<TILE_SIZE*2;ii++)
		{	
			int px=(i - sx / TILE_SIZE ) * TILE_SIZE * 2 + ii;
			int py=(tp->m_PageSizeX - 0) * 2 - ((j - sz / TILE_SIZE ) * TILE_SIZE * 2 + jj) - 1; // 偏一下 
			if(px<0) px=0;
			if(py<0) py=0;
			if(px>=(tp->m_PageSizeX - 0) * 2) px=(tp->m_PageSizeX - 0) * 2;
			if(py>=(tp->m_PageSizeX - 0) * 2) py=(tp->m_PageSizeX - 0) * 2;

			pAlphaBmpdata[(py*(tp->m_PageSizeX - 0) * 2 + px) * 3 + 2] = raw.alpha[0][jj][ii];
			pAlphaBmpdata[(py*(tp->m_PageSizeX - 0) * 2 + px) * 3 + 1] = raw.alpha[1][jj][ii];
			
		}

		// LightMap

		for(int jj=0;jj<TILE_SIZE*2;jj++)
		for(int ii=0;ii<TILE_SIZE*2;ii++)
		{	
			int px=(i - sx / TILE_SIZE ) * TILE_SIZE * 2 + ii;
			int py=(tp->m_PageSizeX - 0) * 2 - ((j - sz / TILE_SIZE ) * TILE_SIZE * 2 + jj); 
			if(px<0) px=0;
			if(py<0) py=0;
			if(px>=(tp->m_PageSizeX - 0) * 2) px=(tp->m_PageSizeX - 0) * 2;
			if(py>=(tp->m_PageSizeX - 0) * 2) py=(tp->m_PageSizeX - 0) * 2;

			unsigned int color = raw.lightmap[jj / 2][ii / 2];
			pLightBmpdata[(py*(tp->m_PageSizeX - 0) * 2 + px) * 3 + 2] = (color & 0xFF000000) >> 24;
			pLightBmpdata[(py*(tp->m_PageSizeX - 0) * 2 + px) * 3 + 1] = (color & 0x00FF0000) >> 16;
			pLightBmpdata[(py*(tp->m_PageSizeX - 0) * 2 + px) * 3 + 0] = (color & 0x0000FF00) >> 8;
		}

		for(int p = 0; p < 3; p ++)
		{
			if(strcmp(raw.texture[p], "基础材质.bmp") != 0)
			{
				if(DEFAULT_TILESIZE == 8)
				{
					char tileName[32];
					sprintf(tileName, "tile[%d,%d]", (i - sx / TILE_SIZE), (j - sz / TILE_SIZE));
					COTETerrainMaterialMgr::GetInstance()->UpdateTileTexture(tp, tileName, raw.texture[p], p);
				}
				else if(DEFAULT_TILESIZE == 16)
				{
					char tileName[32];
					sprintf(tileName, "tile[%d,%d]", (i - sx / TILE_SIZE) / 2, (j - sz / TILE_SIZE) / 2);
					COTETerrainMaterialMgr::GetInstance()->UpdateTileTexture(tp, tileName, raw.texture[p], p);
				}
				else
				{
					OTE_MSG_ERR("不支持其他尺寸的tile")
					throw;
				}
			}
		}		
	}

	// 包围盒

	tp->m_BoundingBox = Ogre::AxisAlignedBox(Ogre::Vector3::ZERO, 
		Ogre::Vector3(
		tp->m_Scale.x * (tp->m_PageSizeX - 0), 
		tp->m_Scale.y, 
		tp->m_Scale.z * (tp->m_PageSizeZ - 0) )
		);

	// 地表贴图

	for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)
	{
		tp->m_strDefaultTexs[i] = createData.layerTexes[i] = "基础材质.bmp";
		createData.tileTexes[i] = &tp->m_TileTexes[i];
	}

	// 光照图

	tp->m_LightmapName = tp->m_PageName + "_lightmap.bmp";

	// Alpha

	tp->m_AlphamapName = tp->m_PageName + "_alphamap.bmp";

	// 建立页	

	Ogre::String spath, sfullpath;
	OTEHelper::GetFullPath("lightMapTemplate1024.bmp", spath, sfullpath);

	Ogre::String dpath, dfullpath;
	OTEHelper::GetFullPath("lightmapdirhelper.ote", dpath, dfullpath);
	
	bool lightmapExist = !::CopyFile(sfullpath.c_str(), (dpath + "\\" + tp->m_LightmapName).c_str(), true);

	if(COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(tp))
	{	
		//SAFE_DELETE_ARRAY(tp->m_LightmapData)	
		//tp->m_LightmapData = pLightBmpdata;

		if(!lightmapExist)
			COTETerrainLightmapMgr::GetInstance()->SaveLightMapFile(tp);
	}

	SAFE_DELETE_ARRAY(pLightBmpdata)

	Ogre::TextureManager::getSingleton().remove(tp->m_LightmapName);
	Ogre::TextureManager::getSingleton().load(tp->m_LightmapName, 
						Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);	
	
	// alpha map

	OTEHelper::GetFullPath("alphaMapTemplate1024.bmp", spath, sfullpath);
	OTEHelper::GetFullPath("lightmapdirhelper.ote", dpath, dfullpath);
	::CopyFile(sfullpath.c_str(), (dpath + "\\" + tp->m_AlphamapName).c_str(), true);

	if(COTETerrainMaterialMgr::GetInstance()->LoadAlphamapData(tp))
	{
		if(tp->m_AlphaBmpdata)
			delete [] tp->m_AlphaBmpdata;

		tp->m_AlphaBmpdata = pAlphaBmpdata;

		OTE_TRACE("SaveAlphamap ... ")
		COTETerrainMaterialMgr::GetInstance()->SaveAlphamap(tp);
		OTE_TRACE("SaveAlphamap OK! ")
		//Ogre::TextureManager::getSingleton().remove(tp->m_AlphamapName);
	}

	tp->BuildPage(&createData);

	COTEAutoEntityMgr::GetInstance()->SetupMeshes(NULL);

	// 保存水数据

	std::string fileName = OTE::COTETilePage::GetCurrentPage()->m_LightmapPath + "\\zhtest_water.raw";

	if(!OTEHelper::SaveRawFile(fileName, (unsigned char*)pLiquidMap, size * size * sizeof(unsigned int)))
	{
		OTE_MSG("水数据文件保存失败！", "失败")		
	}

	// 草

	OTE::COTEVegManager::GetInstance()->SetGrassTexture("grassTexture.tga");

	// 保存高度数据

	fileName = OTE::COTETilePage::GetCurrentPage()->m_LightmapPath + "\\heightmap.tga";

	unsigned char* pHeight256 = new unsigned char[512 * 512 * 3];

	// 数据交换

	for(int i = 0; i < 512; i ++)
	for(int j = 0; j < 512; j ++)
	{
		pHeight256[(i * 512 + j) * 3	] = createData.fHeightData[i * 513 + j] * 256;
		pHeight256[(i * 512 + j) * 3 + 1] = createData.fHeightData[i * 513 + j] * 256;
		pHeight256[(i * 512 + j) * 3 + 2] = createData.fHeightData[i * 513 + j] * 256;
	}

	// 由于数据按照正序 (内存地址从小到大) 要转换为图片数据	

	if(!OTEHelper::SaveTgaFile(fileName, pHeight256, 512, 512, 24))
	{
		OTE_MSG("高度信息保存失败！", "失败")		
	}
	
	delete [] createData.fHeightData;
	delete [] createData.normal;
//	delete [] pAlphaBmpdata;
//	delete [] pLightBmpdata;
	delete [] pLiquidMap;
	delete [] pHeight256;	

	fclose(sceneFile);

//	SCENE_MGR->setSkyDome(true, "Examples/CloudySky", 5, 8);

	COTETilePage::AddTerrainPage(tp->m_PageName, tp);

	// water

	OTE::COTELiquidManager::GetSingleton()->LoadLiquidFromFile("zhtest_water.raw");

	COTETilePage::GetCurrentPage()->AverageVertexNormal();

	// 天空

	std::string path, fullpath;
	OTEHelper::GetFullPath("skydirhelper.ote", path, fullpath);

	std::stringstream ss;
	ss << path << "\\" << tp->m_PageName << "_sky.oraw";
	
	COTESky::GetInstance()->Create();

	if(CHECK_RES(ss.str()))
		COTESkyMgr::GetInstance()->CreateFromFile(ss.str());

	// 草

	OTEHelper::GetFullPath("grassdirhelper.ote", path, fullpath);

	ss.str("");
	ss << path << "\\" << tp->m_PageName << "_veg.oraw";
	COTEVegManager::GetInstance()->CreateFromFile(ss.str());

	OTE_TRACE("AddTerrainPage OK")
}

// -----------------------------------------
void CSectorDlg::AutoLoadScene()
{
	if(m_SectorNameList.GetCurSel() == -1)
		return;

	CString str;
	m_SectorNameList.GetText(m_SectorNameList.GetCurSel(), str);
	char sectorName[32];
	int sx, sz;	
	if (sscanf((char*)LPCTSTR(str), "%s %d %d", sectorName, &sx, &sz) == 3)
	{
		LoadScene(sectorName, sx, sz);
		OnOK();
	}
}
// -----------------------------------------
void fixedRect(CRect& rect)
{
   int sx=(rect.left>0)?rect.left:0;
   int sy=(rect.top>0)?rect.top:0;
   int ex=(rect.right<ROW_TILES*TILE_SIZE)?rect.right:ROW_TILES*TILE_SIZE-1;
   int ey=(rect.bottom<COL_TILES*TILE_SIZE)?rect.bottom:COL_TILES*TILE_SIZE-1;
   rect.SetRect(sx,sy,ex,ey);
}

// -----------------------------------------
// 保存高度图信息
// -----------------------------------------
void SaveHgtMapInfo()
{
	COTETilePage* tp = COTETilePage::GetCurrentPage();
	TileCreateData_t createData;	

	int totalPageSize		= (tp->m_PageSizeX + 1) * (tp->m_PageSizeZ + 1); 
	createData.fHeightData  = new float[totalPageSize];					// 高度
	int pageUnitSize		= (tp->m_PageSizeX - 0) / tp->m_UnitSize;	// 页的材质unit数量

	tp->ExportData(&createData);	

	// sceneFile

	FILE* sceneFile=fopen(OTEHelper::GetOTESetting("ZHSettings", "SceneFile"), "r+b");
	if(!sceneFile)
	{
		MessageBox(NULL,"场景文件没有找到!","错误",0);
		return;
	}		

	// ttttttttttttttttttttttttttttt
	int    imgRowSize = 1024;		
	int    imgColSize = 1024;	

	CRect pageRect(
		int(g_StartPosX / 2),
		int(g_StartPosZ / 2), 
		int(g_StartPosX / 2) + 512,
		int(g_StartPosZ / 2) + 512);

	fixedRect(pageRect);
	for(int j=pageRect.top/TILE_SIZE;j<(pageRect.top+pageRect.Height())/TILE_SIZE;j++)
	for(int i=pageRect.left/TILE_SIZE;i<(pageRect.left+pageRect.Width())/TILE_SIZE;i++)
	{		
		//read scene file
		TileRaw raw ;
		fseek(sceneFile,sizeof(TileRaw) * (j *ROW_TILES + i),0);
		fread(&raw, sizeof(TileRaw),1,sceneFile);	    
		
		int size = TILE_SIZE + 1;
		for(int jj=0;jj < size; jj++)
		for(int ii=0;ii < size; ii++)
		{	
			int px = i * TILE_SIZE + ii - pageRect.left;
			int py = j * TILE_SIZE + jj - pageRect.top; 
			if(px < 0) px = 0;
			if(py < 0) py = 0;
			if(px > tp->m_PageSizeX)  px =  tp->m_PageSizeX;
			if(py > tp->m_PageSizeZ)  py =  tp->m_PageSizeZ;
			 
			float hgt = createData.fHeightData[py * tp->m_PageSizeX + px];

			raw.height[jj][ii] = hgt; // 换算 
		}		
	   	fseek(sceneFile,sizeof(TileRaw) * (j *ROW_TILES + i),0);
		fwrite(&raw, sizeof(TileRaw),1,sceneFile);		
	}
	fclose(sceneFile);

	delete [] createData.fHeightData;	
}
// -----------------------------------------
// 保存Apha图数据
// -----------------------------------------
void ImportAlphaInfo(const char* filename, int layerIndex)
{
	FILE* sceneFile=fopen(OTEHelper::GetOTESetting("ZHSettings", "SceneFile"), "r+b");
	if(!sceneFile)
	{
		MessageBox(NULL,"场景文件没有找到!","错误",0);
		return;
	}	
	
	FILE* file =fopen(filename,"rb"); 
	if(!file)
	{	
		MessageBox(NULL,"alphamap文件没有找到!","错误",0);
		return;		
	}

	BITMAPFILEHEADER FileHeader;
	fread( &FileHeader, sizeof(BITMAPFILEHEADER), 1, file );

	BITMAPINFOHEADER tHeader;
	fread(&tHeader,sizeof(BITMAPINFOHEADER),1,file);

	int m_BitMapHeight = tHeader.biHeight;
	int m_BitMapWidth  = tHeader.biWidth;	

	// ttttttttttttttttttttttttttttt
	int    imgRowSize = 1024;		
	int    imgColSize = 1024;	
	unsigned int* m_pAlphatmap = NULL;

	CRect pageRect(
		int(g_StartPosX / 2),
		int(g_StartPosZ / 2), 
		int(g_StartPosX / 2) + 512,
		int(g_StartPosZ / 2) + 512);

	// 32位
	if( tHeader.biBitCount == 32 )
	{
		//if( tHeader.biSizeImage != imgRowSize * imgColSize*4 )
		if( tHeader.biWidth != imgRowSize || tHeader.biHeight != imgColSize )
		{
			MessageBox(NULL,"图片尺寸不对!","错误",0);
			fclose(file);
			return;
		}
		m_pAlphatmap = new unsigned int[m_BitMapHeight * m_BitMapWidth];

		for(int j=0;j<m_BitMapHeight;j++)
		for(int i=0;i<m_BitMapWidth;i++)
		{
			unsigned int color;
			fread(&color,4,1,file);	   
			unsigned int tcolor=((color&0x000000ff)<<16)|((color&0x00ff0000)>>16)|(color&0x0000ff00);
			tcolor=tcolor<<8;
			m_pAlphatmap[(m_BitMapHeight-j-1)*m_BitMapWidth+i]=tcolor;
		}		
	}
	// 24位
	else if( tHeader.biBitCount == 24 )
	{
		//if( tHeader.biSizeImage != imgRowSize * imgColSize*3 )
		if( tHeader.biWidth != imgRowSize || tHeader.biHeight != imgColSize )
		{
			MessageBox(NULL,"图片尺寸不对!","错误",0);
			fclose(file);
			return;
		}
		m_pAlphatmap = new unsigned int[m_BitMapHeight * m_BitMapWidth];

		for(int j=0;j<m_BitMapHeight;j++)
		for(int i=0;i<m_BitMapWidth;i++)
		{
			unsigned int color;
			fread(&color,3,1,file);	   
			unsigned int tcolor=((color&0x000000ff)<<16)|((color&0x00ff0000)>>16)|(color&0x0000ff00);
			tcolor=tcolor<<8;
			m_pAlphatmap[(m_BitMapHeight-j-1)*m_BitMapWidth+i]=tcolor;
		}		
	}
	fixedRect(pageRect);
	OTE_TRACE("pageRect: " << pageRect.top << " " << pageRect.left << "  "  << pageRect.bottom << "  " << pageRect.right)
	for(int j=pageRect.top/TILE_SIZE;j<(pageRect.top+pageRect.Height())/TILE_SIZE;j++)
	for(int i=pageRect.left/TILE_SIZE;i<(pageRect.left+pageRect.Width())/TILE_SIZE;i++)
	{		
		//read scene file
		TileRaw raw ;
		fseek(sceneFile,sizeof(TileRaw) * (j *ROW_TILES + i),0);
		fread(&raw, sizeof(TileRaw),1,sceneFile);	    
		
		int size = 2 * TILE_SIZE;
		for(int jj=0;jj< size; jj++)
		for(int ii=0;ii< size; ii++)
		{	
			int px = i * size + ii - pageRect.left* 2;
			int py = j * size + jj - pageRect.top * 2; 

			if(px<0) px=0;
			if(py<0) py=0;
			if(px>=m_BitMapWidth) px=m_BitMapWidth-1;
			if(py>=m_BitMapHeight) py=m_BitMapHeight-1;

			unsigned int color=m_pAlphatmap[py*m_BitMapWidth+px];

			//if((color & 0x000000ff) > 0)	OTE_TRACE("(color & 0x000000ff)" << (color & 0x000000ff))
			//if((color & 0x0000ff00) > 0)	OTE_TRACE("(color & 0x0000ff00)" << (color & 0x0000ff00))
			//if((color & 0x00ff0000) > 0)	OTE_TRACE("(color & 0x00ff0000)" << (color & 0x00ff0000))
			 
			raw.alpha[0][jj][ii] = (color & 0x0000ff00) >> 8; 
			raw.alpha[1][jj][ii] = (color & 0x00ff0000) >> 16; 
		}		
	   	fseek(sceneFile,sizeof(TileRaw) * (j *ROW_TILES + i),0);
		fwrite(&raw, sizeof(TileRaw),1,sceneFile);		
	}
	fclose(sceneFile);
	fclose(file);
	if(m_pAlphatmap)
		delete []m_pAlphatmap;
}

// -----------------------------------------
// 保存实体
// -----------------------------------------
void SaveEntities()
{
	FILE* sceneFile = fopen(OTEHelper::GetOTESetting("ZHSettings", "SceneFile"), "r+b");
	if(!sceneFile)
	{
		MessageBox(NULL,"场景文件没有找到!","错误",0);
		return;
	}	

	// 逐个保存物件

	std::vector<Ogre::MovableObject*> eList; SCENE_MGR->GetEntityList(&eList);
	OTE_TRACE("----------------------------------------------")
	OTE_TRACE("正在保存实体列表： size = " << eList.size() << " ... ")
	OTE_TRACE("----------------------------------------------")
	std::vector<Ogre::MovableObject*>::iterator lit = eList.begin();
	while(lit != eList.end())
	{
		Ogre::MovableObject* m = (*lit);
		if(m && m->isVisible() && m->getName().find("Ent_") != std::string::npos)
		{
			// 找到对应的tile块
			const Ogre::Vector3 rPos = m->getParentSceneNode()->getPosition();
			COTETile* t = OTE::COTETilePage::GetCurrentPage()->GetTileAt(rPos.x, rPos.z);
			if(t)
			{
				int emptyPos = -1; // 实体索引
				std::string entName = m->getName();
//OTE_TRACE("entNDDDame : " << entName)
				// 消除前缀
				if(entName.find("Ent_") != std::string::npos)
					entName.erase(0, 4);

				int tileZ = int((rPos.z + g_StartPosZ) / WORLD_TILE_SIZE);
				int tileX = int((rPos.x + g_StartPosX) / WORLD_TILE_SIZE);
				unsigned int fileOffset = (tileZ * ROW_TILES + tileX) * sizeof(TileRaw);

				// 已经存储过的实体 按照名称找到对应的tile索引 从而找到文件偏移
				if(entName.find("entity.tile[") != std::string::npos)
				{	
					OTE_ASSERT(entName.length() < 64)
					char buff[64]; // 一定要足够
					OTE_ASSERT(sscanf(entName.c_str(), "entity.tile[%d,%d].%d.%s", &tileX, &tileZ, &emptyPos, buff) == 4)
					fileOffset =  (tileZ * ROW_TILES + tileX) * sizeof(TileRaw);					
				}	
				
				// 读文件
				static TileRaw raw;					
				fseek(sceneFile, fileOffset, 0);
				fread(&raw, sizeof(TileRaw),1,sceneFile);
				
				// 名字相同就直接保存		
				OTE_ASSERT(emptyPos == -1 || entName == std::string(raw.entities[emptyPos].sNodeName)) 			

				// 找到一个空位
				if(emptyPos == -1)
				{	
					OTE_ASSERT(entName.find("entity.tile[") == std::string::npos) // 这种情况一定是新添加的entity, 名称不含tile

					// 在对应的tile上找
					for( int ii = 0; ii < 4; ii ++ )
					{						
						if(strcmp(raw.entities[ii].sNodeName, "" ) == 0) 
						{
							emptyPos = ii;
							std::stringstream ss;
							ss << "entity.tile[" << tileX << "," << tileZ << "]." << emptyPos << "." << ((COTEEntity*)m)->getMesh()->getName();
							entName = ss.str();
//OTE_TRACE("entNDDDsdfsdfame : " << entName)
							OTE_ASSERT(entName.length() < 64)
							OTE_TRACE("*** 保存新实体： " << entName << " ... ")
							break;
						}						
					}
					// 在周边的tile上找
					// TODO
				}

				// 如果过于密集
				if(emptyPos == -1)
				{					
					OTE_MSG("物件过分密集 实体:" << entName << "保存失败！", "Error")
				}
				else
				{
					OTE_ASSERT(!entName.empty() && entName.length() < 64)					
					
					strcpy(raw.entities[emptyPos].sNodeName, entName.c_str());// 名字
					
					Ogre::Vector3 sOffset(g_StartPosX, 0.0f, g_StartPosZ);
					const Ogre::Vector3& min = m->getBoundingBox().getMinimum() + sOffset;
					raw.entities[emptyPos].fMin[0] = min.x; 
					raw.entities[emptyPos].fMin[1] = min.y; 
					raw.entities[emptyPos].fMin[2] = min.z; 
					const Ogre::Vector3& max = m->getBoundingBox().getMaximum() + sOffset;
					raw.entities[emptyPos].fMax[0] = max.x; 
					raw.entities[emptyPos].fMax[1] = max.y; 
					raw.entities[emptyPos].fMax[2] = max.z; 
					
					const Ogre::Vector3& pos = m->getParentSceneNode()->getPosition() + sOffset;
					raw.entities[emptyPos].fPos[0] = pos.x; 
					raw.entities[emptyPos].fPos[1] = pos.y; 
					raw.entities[emptyPos].fPos[2] = pos.z; 

					const Ogre::Quaternion& rot = m->getParentSceneNode()->getOrientation();				
					raw.entities[emptyPos].fRot[0] = rot.x; 
					raw.entities[emptyPos].fRot[1] = rot.y; 
					raw.entities[emptyPos].fRot[2] = rot.z; 
					raw.entities[emptyPos].fRot[3] = rot.w; 

					raw.entities[emptyPos].fScale = m->getParentSceneNode()->getScale().x - 1.0f; // 0 - 对应1:1比例
					raw.entities[emptyPos].id = 0; // TODO

					// 写文件
					fseek(sceneFile, fileOffset, 0);
					fwrite(&raw, sizeof(TileRaw),1,sceneFile);

					OTE_TRACE("实体： " << entName << "已经被保存")
				}
			}	
		}
		++ lit;
	}	
	fclose(sceneFile);
}
// -----------------------------------------
// 保存特效
// -----------------------------------------
void SaveMagics()
{
	FILE* sceneFile = fopen(OTEHelper::GetOTESetting("ZHSettings", "SceneFile"), "r+b");
	if(!sceneFile)
	{
		MessageBox(NULL,"场景文件没有找到!","错误",0);
		return;
	}	

	/*int magNum = COTEMagicEntity::GetDymMagicInterface()->GetMagicNum();
	for(int i = 0; i < magNum; i ++)
	{
		std::string magName = g_dymMagicToOTE.GetMagicID(i);

	}*/

	fclose(sceneFile);	
}

// -----------------------------------------
// 保存数据
// -----------------------------------------
void CSectorDlg::SaveScene()
{
	// 保存alpha信息
	COTETerrainMaterialMgr::GetInstance()->SaveAlphamap(COTETilePage::GetCurrentPage());
	std::stringstream ss;	ss << OTEHelper::GetOTESetting("ResConfig", "LightmapPath") << g_SceneName << "_alphamap.bmp";
	ImportAlphaInfo(ss.str().c_str(), 0);

	// 保存高度
	SaveHgtMapInfo();

	// 保存场景物件
	SaveEntities();

	// 保存场景特效
	SaveMagics();
}
// -----------------------------------------
void CSectorDlg::OnLbnDblclkList1()
{
	AutoLoadScene();
}

void CSectorDlg::OnBnClickedButton1()
{
	AutoLoadScene();
}

void CSectorDlg::OnBnClickedButton2()
{
	OnCancel();
}
