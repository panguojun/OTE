#include "stdafx.h"
#include "OTESceneFile.h"
#include "OTEStaticInclude.h"
#include "OTESceneXmlLoader.h"
#include "Plugin_EntEditor.h"
#include "EntEditorDlg.h"
#include "EntPorpDlg.h"
#include "EntCrtDlg.h"
#include "EntEditDlg.h"
#include "TransformManager.h"
#include "SaveDlg.h"
#include "OTETilePage.h"
#include "OTECollisionManager.h"
#include "OTEPluginManager.h"
#include "3DPathView.h"
#include "OTEEntityAutoLink.h"
#include "D3DToolbox.h"
#include "OTETerrainLightmapMgr.h"
#include "OTEMathLib.h"

// ----------------------------------------------------------------
using namespace OTE;
using namespace Ogre;
using namespace std;
// ----------------------------------------------------------------
// 鼠标操作的标示

#define		MOUSE_FLAG_EMPTY		0x00000000
#define		MOUSE_FLAG_TRANS		0x0F000000
#define		MOUSE_FLAG_COPY			0xF0000000
#define		MOUSE_FLAG_FREEMOV		0x00F00000
#define		MOUSE_FLAG_GROUPSEL		0x000F0000
#define		MOUSE_FLAG_SNAPSTART	0x0000F000
#define		MOUSE_FLAG_SNAPEND		0x00000F00

// ----------------------------------------------------------------
std::string				g_LightmapName;

// =================================================================
// NPC 相关功能
// =================================================================
inline float FixedSector(float x){return x - (int(x) / 1024) * 1024;}
// ----------------------------------------------------------------
// 查询NPC
NpcStartPos_t& GetNpcStartPos(int secIndex, int npcIndex)
{
	OTE_ASSERT(secIndex < CLogicEditorDlg::s_Inst.m_vLogicSectors.size())
	OTE_ASSERT(npcIndex < CLogicEditorDlg::s_Inst.m_vLogicSectors[secIndex].vNpcStartPos.size())

	return CLogicEditorDlg::s_Inst.m_vLogicSectors[secIndex].vNpcStartPos[npcIndex];
}
// ----------------------------------------------------------------
// 编辑事件响应
// ----------------------------------------------------------------
void OnEditMoved(const OTE::CEventObjBase::EventArg_t& arg)
{
	OTE::COTEActorEntity* ae = SCENE_MGR->GetEntity(arg.strObjName);
	if(!ae)
		return;
	
	if("Npc" == ae->GetStringVal("ObjType"))
	{
		int secIndex = ae->GetIntVal("SecIndex");
		int npcIndex = ae->GetIntVal("NpcIndex");
		NpcStartPos_t& npc = GetNpcStartPos(secIndex, npcIndex);
		npc.Pos = ae->GetParentSceneNode()->getPosition();
	}
	else if("WayPoint" == ae->GetStringVal("ObjType"))
	{
		int secIndex = ae->GetIntVal("SecIndex");
		int npcIndex = ae->GetIntVal("NpcIndex");
		int wpIndex  = ae->GetIntVal("WpIndex");

		NpcStartPos_t& npc = GetNpcStartPos(secIndex, npcIndex);
		Ogre::Vector3& pos = npc.WayPoints[wpIndex];
		pos = ae->GetParentSceneNode()->getPosition();	

		// 更新显示

		char npcEntName[128];
		sprintf(npcEntName, "npc_%s_%d_%d", npc.npcClassName, secIndex, npcIndex);
		OTE::COTEActorEntity* npcEnt = SCENE_MGR->GetEntity(npcEntName);
		std::stringstream ss;
		ss << "wp_pos_" << wpIndex;
		npcEnt->SetVal(ss.str(), pos);	
	}	
}
// ----------------------------------------------------------------
void OnEditRoted(const OTE::CEventObjBase::EventArg_t& arg)
{
	OTE::COTEActorEntity* ae = SCENE_MGR->GetEntity(arg.strObjName);
	if(!ae)
		return;
	
	if("Npc" == ae->GetStringVal("ObjType"))
	{
		int secIndex = ae->GetIntVal("SecIndex");
		int npcIndex = ae->GetIntVal("NpcIndex");
		NpcStartPos_t& npc = GetNpcStartPos(secIndex, npcIndex);
		npc.Rot = MathLib::FixRadian(MathLib::GetRadian(ae->getParentSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z ) ).valueDegrees();
	}	
}

// ----------------------------------------------------------------
// 删除npc
void OnDeleteNpc(const OTE::CEventObjBase::EventArg_t& arg)
{
	OTE::COTEActorEntity* ae = SCENE_MGR->GetEntity(SCENE_MGR->GetCurObjSection()->getName());
	if(!ae || ae->GetName().find("npc") == std::string::npos)
		return;
	
	int secIndex = ae->GetIntVal("SecIndex");
	int npcIndex = ae->GetIntVal("NpcIndex");

	// 先移除Npc
	NpcStartPos_t& npc = GetNpcStartPos(secIndex, npcIndex);
	for(int i = 0; i < npc.numWP; i ++)
	{		
		char wpEntName[128];
		sprintf(wpEntName, "wp_%d_%d_%d", secIndex, npcIndex, i);
		SCENE_MGR->RemoveEntity(wpEntName);
	}
	npc.alive = false;
	SCENE_MGR->RemoveEntity(ae);
}
// ----------------------------------------------------------------
// 添加NPC路点
void AddSectorNpcWayPoint(const Ogre::Vector3& pos, int secIndex, int npcIndex, int wpIndex)
{
	// 不超过 NPCMAXWP 个
	if(wpIndex >= NPCMAXWP)
		return;

	// NPC 是否存在
	char npcEntName[128];
	sprintf(npcEntName, "npc_%s_%d_%d", GetNpcStartPos(secIndex, npcIndex).npcClassName, secIndex, npcIndex);
	OTE::COTEActorEntity* npcEnt = SCENE_MGR->GetEntity(npcEntName);
	if(!npcEnt)
		return;

	// 创建路点
	char wpEntName[128];
	sprintf(wpEntName, "wp_%d_%d_%d", secIndex, npcIndex, wpIndex);
	OTE::COTEActorEntity* ae = SCENE_MGR->CreateEntity("waypointhelper.mesh", wpEntName);

	// position
	Ogre::Vector3 position(FixedSector(pos.x), 0, FixedSector(pos.z));
	position.y = OTE::COTETilePage::GetCurrentPage() ? OTE::COTETilePage::GetCurrentPage()->GetHeightAt(position.x, position.z) : 0.0f;
	ae->GetParentSceneNode()->setPosition(position);

	NpcStartPos_t& npc = GetNpcStartPos(secIndex, npcIndex);
	npc.WayPoints[wpIndex] = position;
	npc.numWP = wpIndex + 1;

	// 与数据绑定
	ae->SetVal("ObjType", "WayPoint");
	ae->SetVal("SecIndex", secIndex);
	ae->SetVal("NpcIndex", npcIndex);
	ae->SetVal("WpIndex", wpIndex);

	ae->AddEventHandler("OnEditMoved", OnEditMoved);		
	
	// 记录在实体对象上
	npcEnt->SetVal("WayPoints", wpIndex + 1);
	std::stringstream ss;
	ss << "wp_pos_" << wpIndex;
	npcEnt->SetVal(ss.str(), position);	
}
// ----------------------------------------------------------------
// 添加NPC
void AddSectorNpc(const NpcStartPos_t& npcStartPos, int secIndex, int npcIndex)
{
	// npc实体
	char npcEntName[128];
	sprintf(npcEntName, "npc_%s_%d_%d", npcStartPos.npcClassName, secIndex, npcIndex);
	OTE::COTEActorEntity* ae = SCENE_MGR->CreateEntity("heighthelper.mesh", npcEntName);

	// position
	Ogre::Vector3 position(FixedSector(npcStartPos.Pos[0]),0, FixedSector(npcStartPos.Pos[2]));
	position.y = OTE::COTETilePage::GetCurrentPage() ? OTE::COTETilePage::GetCurrentPage()->GetHeightAt(position.x, position.z) : 0.0f;
	ae->GetParentSceneNode()->setPosition(position);
	
	// rotation
	Ogre::Quaternion q(- Ogre::Radian(Ogre::Degree(npcStartPos.Rot)), Ogre::Vector3::UNIT_Y);	
	ae->GetParentSceneNode()->setOrientation(q);

	// 与数据绑定
	ae->SetVal("ObjType", "Npc");
	ae->SetVal("SecIndex", secIndex);
	ae->SetVal("NpcIndex", npcIndex);

	ae->AddEventHandler("OnEditMoved", OnEditMoved);
	ae->AddEventHandler("OnEditRoted", OnEditRoted);
}
// ----------------------------------------------------------------
void OnCreateNpc(const OTE::CEventObjBase::EventArg_t& arg)
{	
	std::string name = COTETilePage::GetCurrentPage()->GetStringVal("CrtNpcName");
	Ogre::Vector3 crtPos = COTETilePage::GetCurrentPage()->GetVector3Val("CrtPos");
	int secIndex = CLogicEditorDlg::s_Inst.GetLogicSectorAt(crtPos);
	if(secIndex < 0)
		return;

	NpcStartPos_t npcStartPos;
	strcpy(npcStartPos.npcClassName, name.c_str());
	npcStartPos.name[0] = 0;
	npcStartPos.Pos = crtPos;
	npcStartPos.secIndex = secIndex;	
	npcStartPos.numWP = 0;
	npcStartPos.npcID = 0;
	npcStartPos.alive = true;
	npcStartPos.npcspeech[0][0] = npcStartPos.npcspeech[1][0] = npcStartPos.npcspeech[2][0] = npcStartPos.npcspeech[3][0] = npcStartPos.npcspeech[4][0] = 0;

	AddSectorNpc(npcStartPos, secIndex, CLogicEditorDlg::s_Inst.m_vLogicSectors[secIndex].vNpcStartPos.size());
	CLogicEditorDlg::s_Inst.m_vLogicSectors[secIndex].vNpcStartPos.push_back(npcStartPos);
}

// ----------------------------------------------------------------
void OnCreateWp(const OTE::CEventObjBase::EventArg_t& arg)
{	
	OTE::COTEActorEntity* ae = (OTE::COTEActorEntity*)SCENE_MGR->GetCurObjSection();
	if(ae->GetName().find("npc") == std::string::npos)
		return;

	Ogre::Vector3 crtPos = COTETilePage::GetCurrentPage()->GetVector3Val("CrtPos");
	int secIndex = CLogicEditorDlg::s_Inst.GetLogicSectorAt(crtPos);
	int npcIndex = ae->GetIntVal("NpcIndex");
	int wpIndex = GetNpcStartPos(secIndex, npcIndex).numWP;
	AddSectorNpcWayPoint(crtPos, secIndex, npcIndex, wpIndex);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// 得到逻辑区域索引
int CLogicEditorDlg::GetLogicSector(unsigned int id)
{
   for(unsigned int i=0;i<m_vLogicSectors.size();i++)
   {     
     if(m_vLogicSectors.at(i).LogicSectorID==id)
		 return i;
   } 
   //OTE_MSG("没有找到ID = " << id << " 的逻辑区域.", "失败")
   return -1;
}
// ----------------------------------------------------------------
// 得到区域索引
int CLogicEditorDlg::GetLogicSectorAt(const Vector3& pos)
{
	return GetLogicSector(GetLogicSectorIDAt(pos));
}
// ----------------------------------------------------------------
// 显示逻辑区域图
bool CLogicEditorDlg::ShowSectorMap(const std::string& logicSectorMap)
{
	COTETilePage* page = COTETilePage::GetCurrentPage();
	if(!page)
		return false;

	// 把逻辑区域图的本场景部分保存起来
	
	if(!page->m_LightmapData)
	{
		page->LoadLightmapData();
	}
	
	int secOffsetX = int(page->GetfloatVal("StartPosX") / 16.0f);
	if(secOffsetX >= ROW_TILES || secOffsetX < 0) return false;
	int secOffsetZ = int(page->GetfloatVal("StartPosZ") / 16.0f);	
	if(secOffsetZ >= COL_TILES || secOffsetZ < 0) return false;	

	// 数据

	unsigned char* lightmapData = page->m_LightmapData;
	
	for(int j = 0; j < 64; j ++)
	for(int i = 0; i < 64; i ++)	
	{
		int x = secOffsetX + i;
		int z = secOffsetZ + j;
		unsigned int color = m_SectorMapData[((COL_TILES - z - 1) * ROW_TILES + x)];

		// 存入光照图

		for(int jj = 0; jj < 16; jj ++)
		for(int ii = 0; ii < 16; ii ++)	
		{
			lightmapData[((1024 - (j * 16 + jj) - 1) * 1024 + i * 16 + ii) * 3    ] = (color&0x000000ff)<<16;
			lightmapData[((1024 - (j * 16 + jj) - 1) * 1024 + i * 16 + ii) * 3 + 1] = (color&0x00ff0000)>>16;
			lightmapData[((1024 - (j * 16 + jj) - 1) * 1024 + i * 16 + ii) * 3 + 2] = (color&0x0000ff00);
		}
	}

	// 按照光照图格式保存文件

	std::string str = page->m_LightmapPath + logicSectorMap;
	
	FILE * p;
	p = fopen( str.c_str() , "wb" );
	if(!p)	
	{			
		::MessageBox(NULL, "写光照图数据时文件打开失败！", "失败", MB_OK);
		return false;
	}

	fwrite(&page->m_LightmapBmfh, sizeof(BITMAPFILEHEADER), 1, p);
	fwrite(&page->m_LightmapBmih,  sizeof(BITMAPINFOHEADER), 1, p);
	fwrite(page->m_LightmapData, sizeof(unsigned char) * page->m_LightmapBmih.biWidth * page->m_LightmapBmih.biHeight * 3, 1, p);
	
	fclose(p);

	page->m_LightmapName != logicSectorMap ? g_LightmapName = page->m_LightmapName : NULL;

	// 打开并且作为光照图显示

	COTETerrainLightmapMgr::GetInstance()->SetLightMapMaterial(page, page->m_LightmapPath, logicSectorMap);
		
	return true;
}
// ----------------------------------------------------------------
// 加载逻辑区域图
bool CLogicEditorDlg::LoadSectorMap(const std::string& logicSectorMap)
{
	std::string path, fullPath;

	OTEHelper::GetFullPath(logicSectorMap, path, fullPath);
	path += "\\";

	std::string failDesc;
	FILE *p = fopen(fullPath.c_str() , "rb");
	if(!p)
	{
		failDesc = "文件打开失败";
		goto _exit_;
	}
	BITMAPFILEHEADER bmfh;
	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, p);

	BITMAPINFOHEADER bmih;
	fread(&bmih, sizeof(BITMAPINFOHEADER), 1, p);
	if(bmih.biCompression || bmih.biBitCount != 32)
	{
		failDesc = "像素格式错误 必须是32位位图";
		goto _exit_;
	}

	fseek(p, bmfh.bfOffBits, 0);
	
	// 清理

	if(m_SectorMapData)
		delete [] m_SectorMapData;

	m_SectorMapData = new unsigned int[bmih.biWidth * bmih.biHeight];

	fread(m_SectorMapData, bmih.biWidth * bmih.biHeight * 4, 1, p);		

	fclose(p);

	OTE_TRACE("加载逻辑区域图 完成! ")

	return true;

_exit_:

	if(m_SectorMapData)
	{
		delete [] m_SectorMapData;
		m_SectorMapData = NULL;
	}

	if(p)
		fclose(p);

	return false;
}

// ----------------------------------------------------------------
// 得到逻辑区域ID
unsigned int CLogicEditorDlg::GetLogicSectorIDAt(const Vector3& pos)
{
	if(!OTE::COTETilePage::GetCurrentPage())
		return 0;

	if(!m_SectorMapData && !LoadSectorMap("LogicSectorMap.bmp"))
		return 0;	

	// 读入

	int x = int((pos.x + OTE::COTETilePage::GetCurrentPage()->GetfloatVal("StartPosX")) / 16.0f);
	if(x >= ROW_TILES || x < 0) return 0;
	int z = int((pos.z + OTE::COTETilePage::GetCurrentPage()->GetfloatVal("StartPosZ")) / 16.0f);	
	if(z >= COL_TILES || z < 0) return 0;	
	
	unsigned int color = m_SectorMapData[((COL_TILES - z - 1) * ROW_TILES + x)];
		
	color=((color&0x000000ff)<<16)|((color&0x00ff0000)>>16)|(color&0x0000ff00);

	OTE_TRACE(" x = " << x << " z = " << z << " color = " << color)

	return color;
}
// ----------------------------------------------------------------
// 区域列表
void CLogicEditorDlg::ReadSectorList()
{
	std::string path, fullPath;
	OTEHelper::GetFullPath("logicsectorInfor.dat", path, fullPath);

	FILE* file=fopen(fullPath.c_str(), "r");
	if(!file)
	{
		OTE_MSG("错误","逻辑区域信息文件没有找到！");
	    return;
	}
	int success=1;
	int sectorNum;
	success|=fscanf(file,"logicsectors: %d\n",&sectorNum);
	OTE_LOGMSG("--------------------读取区域列表--------------------")
	OTE_LOGMSG("logicsectors: " << sectorNum)
	for(int i=0;i<sectorNum;i++)
	{
	   LogicSector_t logicSector;
	   success|=fscanf(file,"name: %s color: %d\n",logicSector.Name,&logicSector.LogicSectorID);   
	   strcpy(logicSector.NpcFileName,logicSector.Name);//using logic sector's name
	   logicSector.isLoaded=0;
	   m_vLogicSectors.push_back(logicSector);
	   	   
	   OTE_TRACE("LogicSectorID: " << logicSector.LogicSectorID << " Sector Name: " <<logicSector.Name << " NpcFileName: " <<logicSector.NpcFileName )

	   OTE_LOGMSG("LogicSectorID: " << logicSector.LogicSectorID << " Sector Name: " <<logicSector.Name << " NpcFileName: " <<logicSector.NpcFileName )
	}
    fclose(file);
}
// ----------------------------------------------------------------
// 读取区域之NPC
bool CLogicEditorDlg::ReadLogicSectorNpcInfor(unsigned int secID)
{
	int secIndex=GetLogicSector(secID);
	OTE_TRACE("secIndex = " << secIndex)
	if(secIndex==-1 || m_vLogicSectors[secIndex].isLoaded)
		return false;

	std::string path, fullPath;
	OTEHelper::GetFullPath(m_vLogicSectors[secIndex].NpcFileName, path, fullPath);

    FILE* file=fopen(fullPath.c_str(),"r");
	if(!file)
	{
		OTE_MSG("该逻辑区域NPC信息文件没有找到！ " << fullPath,  "错误" );
	    return false;
	}
	int success=1;
	int Num;
	success|=fscanf(file,"npcs %d\n",&Num);
	for(int i=0;i<Num;i++)
	{
	   char szLine[256];
	   NpcStartPos_t npcStartPos;
	   ZeroMemory( &npcStartPos, sizeof(npcStartPos));
	   strcpy(npcStartPos.name,"");
       fgets(szLine,256,file);
	   success|=sscanf(szLine,"npc name: %s\n",npcStartPos.name);
	   fgets(szLine,256,file);
       success|=sscanf(szLine,"npc id: %d\n",&npcStartPos.npcID);
	   fgets(szLine,256,file);
	   success|=sscanf(szLine,"npc pos: %f %f %f\n",&npcStartPos.Pos.x,&npcStartPos.Pos.y,&npcStartPos.Pos.z);
   	   fgets(szLine,256,file);
	   success|=sscanf(szLine,"npc rot: %f\n",&npcStartPos.Rot);
	   fgets(szLine,256,file);
   	   success|=sscanf(szLine,"npc territoryRadious: %d\n",&npcStartPos.territoryRadious);
	   
	   fgets(szLine,256,file);
	   success|=sscanf(szLine,"npc waypoints: %d\n",&npcStartPos.numWP);		
	   npcStartPos.alive = true;
	   AddSectorNpc(npcStartPos, secIndex, m_vLogicSectors[secIndex].vNpcStartPos.size());// add to scene
	  
	   m_vLogicSectors[secIndex].vNpcStartPos.push_back(npcStartPos);

	   for( int ii = 0; ii < npcStartPos.numWP; ii ++ )
	   {
		 fgets(szLine,256,file);
		 success|=sscanf(szLine,"npc wp: %f %f %f\n",&npcStartPos.WayPoints[ii].x,&npcStartPos.WayPoints[ii].y,&npcStartPos.WayPoints[ii].z);
		
		 if(npcStartPos.WayPoints[ii].x > 0)
			AddSectorNpcWayPoint(npcStartPos.WayPoints[ii], secIndex, i, ii);
	   }
	   // npc对话	
		for(int iii = 0; iii < 5; iii ++)
		{	
			if(fgets(szLine,256,file))
			{
				if(std::string(szLine).find("npc speech:", 0) == std::string::npos)			
				{
					fclose(file);
					return false;
				}
				success |= sscanf(szLine,"npc speech: %s\n", npcStartPos.npcspeech[iii]);				
			}
		}
	}
    fclose(file);
	m_vLogicSectors[secIndex].isLoaded=1;

	return true;
}
// ----------------------------------------------------------------
// 保存区域npc
void CLogicEditorDlg::SaveLogicSectorNpcInfor()
{   
    for(unsigned int i=0;i<m_vLogicSectors.size();i++)
	{	
		if(!m_vLogicSectors[i].isLoaded && m_vLogicSectors[i].vNpcStartPos.size() == 0) continue;
		char resPath[256];
		GetPrivateProfileString("ResConfig","ResPathLogicWorks","\0",
										resPath,sizeof(resPath),".\\OTESettings.cfg");		
		char str[256];
		sprintf(str,"%s%s",resPath,m_vLogicSectors[i].NpcFileName);

		FILE* file=fopen(str,"w");
		int count=0;
		for(unsigned int ii=0;ii<m_vLogicSectors[i].vNpcStartPos.size();ii++)
		{
				if(!m_vLogicSectors[i].vNpcStartPos[ii].alive) continue;
				count++;
		}
		fprintf(file,"npcs %d\n",count);

		for(unsigned int ii=0;ii<m_vLogicSectors[i].vNpcStartPos.size();ii++)
		{	
			const NpcStartPos_t& npcStartPos = m_vLogicSectors[i].vNpcStartPos[ii];
			if(!m_vLogicSectors[i].vNpcStartPos[ii].alive) 
				continue;

			fprintf(file,"npc name: %s\n",npcStartPos.name);
			fprintf(file,"npc id: %d\n",npcStartPos.npcID);
			fprintf(file,"npc pos: %f %f %f\n",npcStartPos.Pos.x,npcStartPos.Pos.y,npcStartPos.Pos.z);
   			fprintf(file,"npc rot: %f\n",npcStartPos.Rot);  
   			fprintf(file,"npc territoryRadious: %d\n",npcStartPos.territoryRadious);		
			fprintf(file,"npc waypoints: %d\n",npcStartPos.numWP);	
			
			for( int jj = 0; jj < npcStartPos.numWP; jj ++ )
			{
				fprintf(file,"npc wp: %f %f %f\n",npcStartPos.WayPoints[jj].x, npcStartPos.WayPoints[jj].y, npcStartPos.WayPoints[jj].z);
			}
			// npc对话
			for(int iii = 0; iii < 5; iii ++)
			{				
				fprintf(file,"npc speech: %s\n", npcStartPos.npcspeech[iii]);
			}
		}
		fclose(file);
	}

	// save as binary file

	for(unsigned int i=0;i<m_vLogicSectors.size();i++)
	{	
		if(!m_vLogicSectors[i].isLoaded && m_vLogicSectors[i].vNpcStartPos.size() == 0) continue;

		char resPath[256];
		GetPrivateProfileString("ResConfig","ResPathLogicWorks","\0",
											resPath,sizeof(resPath),".\\OTESettings.cfg");		
		char str[64];
		sprintf(str,"%s%s.dat",resPath,m_vLogicSectors[i].NpcFileName);	

		FILE* file=fopen(str,"wb");
		for(unsigned int ii=0;ii<m_vLogicSectors[i].vNpcStartPos.size();ii++)
		{	
		   	if(!m_vLogicSectors[i].vNpcStartPos[ii].alive) continue;
			npc_t npc;
			strcpy(npc.name,m_vLogicSectors[i].vNpcStartPos[ii].name);
			npc.npcID=m_vLogicSectors[i].vNpcStartPos[ii].npcID;//temp!
			npc.Pos[0]=m_vLogicSectors[i].vNpcStartPos[ii].Pos.x;
			npc.Pos[1]=m_vLogicSectors[i].vNpcStartPos[ii].Pos.y;
			npc.Pos[2]=m_vLogicSectors[i].vNpcStartPos[ii].Pos.z;
			npc.Rot=m_vLogicSectors[i].vNpcStartPos[ii].Rot ;
			npc.territoryRadious=m_vLogicSectors[i].vNpcStartPos[ii].territoryRadious ;
			npc.numWP	= m_vLogicSectors[i].vNpcStartPos[ii].numWP;
			for( int iii = 0; iii < npc.numWP; iii ++ )
			{
				npc.WayPoints[iii][0]=m_vLogicSectors[i].vNpcStartPos[ii].WayPoints[iii].x;
				npc.WayPoints[iii][1]=m_vLogicSectors[i].vNpcStartPos[ii].WayPoints[iii].y;
				npc.WayPoints[iii][2]=m_vLogicSectors[i].vNpcStartPos[ii].WayPoints[iii].z;
			}
			// npc对话
			for(int iii = 0; iii < 5; iii ++)
			{
				strcpy(npc.npcspeech[iii], m_vLogicSectors[i].vNpcStartPos[ii].npcspeech[iii]);
			}

			fwrite(&npc,sizeof(npc_t),1,file);
		}
		fclose(file);
	}
}

// ========================================================
// CLogicEditorDlg 对话框
// ========================================================

CLogicEditorDlg				CLogicEditorDlg::s_Inst;
CTransformManager			CLogicEditorDlg::s_TransMgr;
CString						CLogicEditorDlg::s_EditState = "选择";

std::list<Ogre::MovableObject*>		CLogicEditorDlg::s_pSelGroup;
std::list<EntityEditData_t>			CLogicEditorDlg::s_pCopyGroup;

DWORD						g_MouseFlag = MOUSE_FLAG_EMPTY;	// 鼠标状态

// 群选效果

OTE::CRectA<float>			g_Rect;						
C3DPathView*				g_pPathView = NULL;

// --------------------------------------------------------
IMPLEMENT_DYNAMIC(CLogicEditorDlg, CDialog)
CLogicEditorDlg::CLogicEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogicEditorDlg::IDD, pParent),
	m_Snap3DMark(NULL),
	m_SectorMapData(0)
{
	ReadSectorList();

}

CLogicEditorDlg::~CLogicEditorDlg()
{
	ClearTrackBall();
}

void CLogicEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CLogicEditorDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, OnTcnSelchangeTab2)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)	
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()



// --------------------------------------------------------
// Clear scene
void CLogicEditorDlg::ClearScene()
{
	s_pSelGroup.clear();

	SCENE_MGR->RemoveAllEntities();

	CEntEditDlg::sInst.UpdateSceneTree();

	SAFE_DELETE(g_pPathView)
}

void CLogicEditorDlg::OnBnClickedButton1()
{
	ClearScene();	
}

// --------------------------------------------------------
// Load From File
void CLogicEditorDlg::OnBnClickedButton2()
{	
	int secID = GetLogicSectorIDAt(SCENE_CAM->getPosition());
	ReadLogicSectorNpcInfor(secID);

	// 一些额外的初始化操作

	if(COTETilePage::GetCurrentPage())
	{
		COTETilePage::GetCurrentPage()->AddEventHandler("OnCreateNpc", OnCreateNpc);
		COTETilePage::GetCurrentPage()->AddEventHandler("OnCreateWp",  OnCreateWp );
		COTETilePage::GetCurrentPage()->AddEventHandler("OnDeleteNpc", OnDeleteNpc);
	}

	//// 摄像机

	//char str[128];

	//::GetPrivateProfileString("CommonEditor", "CameraPosition", "\0", str, sizeof(str), ".\\Editor.ini");
	//
	//float x = 0, y = 0, z = 0;

	//if(sscanf(str, "%f %f %f", &x, &y, &z) != 3)
	//{
	//	COTETilePage* page =  COTETilePage::GetCurrentPage();
	//	if(page)
	//	{
	//		x = page->m_PageSize / 2.0f;
	//		z = page->m_PageSize / 2.0f;
	//		y = page->GetHeightAt(x, z) + 5.0f;						
	//	}

	//}

	//Ogre::Camera* cam = SCENE_CAM;
	//cam->setPosition(Ogre::Vector3(x, y, z));
}
// --------------------------------------------------------
// Save To File
void CLogicEditorDlg::OnBnClickedButton3()
{	
	//先清除鼠标实体
	CEntCrtDlg::sInst.ClearMouseEntity();
	// 保存Npc数据文件

	SaveLogicSectorNpcInfor();

	Ogre::Vector3 camPos = SCENE_CAM->getPosition();

	std::stringstream ss;
	ss << camPos.x << " " << camPos.y << " " << camPos.z;
	WritePrivateProfileString("CommonEditor", "CameraPosition", ss.str().c_str(), ".\\Editor.ini"); 

}
// --------------------------------------------------------
void CLogicEditorDlg::OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult)
{	
	CEntCrtDlg::sInst.ShowWindow(SW_HIDE);	
	CEntPorpDlg::sInst.ShowWindow(SW_HIDE);	
	CEntEditDlg::sInst.ShowWindow(SW_HIDE);	

	if(m_TabCtrl.GetCurSel() == 0)
	{
		CEntCrtDlg::sInst.ShowWindow(SW_SHOW);
	}
	else if(m_TabCtrl.GetCurSel() == 1)
	{
		CEntEditDlg::sInst.ShowWindow(SW_SHOW);		
	}
	else if(m_TabCtrl.GetCurSel() == 2)
	{
		CEntPorpDlg::sInst.ShowWindow(SW_SHOW);		
	}
}
// --------------------------------------------------------
BOOL CLogicEditorDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	m_TabCtrl.InsertItem(0, "添加");
	m_TabCtrl.InsertItem(1, "编辑");
	m_TabCtrl.InsertItem(2, "属性");

	CEntCrtDlg::sInst.Create( IDD_CRT_DLG, this);
	CEntCrtDlg::sInst.ShowWindow(SW_SHOW);

	CEntPorpDlg::sInst.Create( IDD_PP_DLG, this);
	CEntPorpDlg::sInst.ShowWindow(SW_HIDE);	
	CEntEditDlg::sInst.Create( IDD_EDIT_DLG, this);
	CEntEditDlg::sInst.ShowWindow(SW_HIDE);


	CRect tRect;
	GetClientRect(tRect);	
	CEntPorpDlg::sInst.MoveWindow(tRect.left + 2, tRect.top + 70, tRect.right - 5, tRect.bottom - 100);
	CEntEditDlg::sInst.MoveWindow(tRect.left + 2, tRect.top + 70, tRect.right - 5, tRect.bottom - 100);
	CEntCrtDlg::sInst.MoveWindow(tRect.left + 2, tRect.top + 70, tRect.right - 5, tRect.bottom - 100);

	// 图片
 
	((CButton*)GetDlgItem(IDC_BUTTON2))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP2)));  
	((CButton*)GetDlgItem(IDC_BUTTON3))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP3))); 

	((CButton*)GetDlgItem(IDC_BUTTON4))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP4))); 
	((CButton*)GetDlgItem(IDC_BUTTON5))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP5))); 
	((CButton*)GetDlgItem(IDC_BUTTON6))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP6))); 

	((CButton*)GetDlgItem(IDC_BUTTON7))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP7)));

	return tRet;
}


void CLogicEditorDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}

void CLogicEditorDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{

}

// ----------------------------------------------------------------
// 移动
void CLogicEditorDlg::OnBnClickedButton4()
{
	if(!s_pSelGroup.empty())
	{
		CLogicEditorDlg::s_EditState = "变换";
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(true);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(false);
		s_TransMgr.ShowDummy(CTransformManager::ePOSITION);
		s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getWorldPosition());
	}
}

// ----------------------------------------------------------------
// 旋转
void CLogicEditorDlg::OnBnClickedButton5()
{
	if(!s_pSelGroup.empty())
	{
		CLogicEditorDlg::s_EditState = "变换";
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(true);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(false);
		s_TransMgr.ShowDummy(CTransformManager::eROTATION);
		s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getPosition());
	}
}

// ----------------------------------------------------------------
// 缩放
void CLogicEditorDlg::OnBnClickedButton6()
{
	if(!s_pSelGroup.empty())
	{
		CLogicEditorDlg::s_EditState = "变换";
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(true);
		s_TransMgr.ShowDummy(CTransformManager::eSCALING);
		s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getPosition());
	}
}

// ----------------------------------------------------------------
void CLogicEditorDlg::SelectSceneEntity(Ogre::MovableObject* e, bool autoSel)
{		
	if(e == NULL || e == CEntCrtDlg::sInst.m_MouseObj)
	{
		return;
	}
	
	if(!s_pSelGroup.empty() && !(::GetKeyState(VK_CONTROL) & 0x80) && !autoSel)
	{
		CancelSelection();
	}

	// 添加到组

	//if(::GetKeyState(VK_CONTROL) & 0x80)
	{
		std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
		while(it != s_pSelGroup.end())
		{
			if((*it) == e)
			{
				e->getParentSceneNode()->showBoundingBox(false);
				s_pSelGroup.erase(it);

				return;
			}

			++ it;
		}

		e->getParentSceneNode()->showBoundingBox(true);
		SCENE_MGR->SetCurFocusObj(e);

		s_pSelGroup.push_back(e);

		SCENE_MGR->m_SelMovableObjects.push_back(e);
	}

	
	if(!s_pSelGroup.empty())
	{	
		CEntPorpDlg::sInst.UpdateEntityPropertyList((*s_pSelGroup.begin()));
	}

	
}
// ----------------------------------------------------------------
void CLogicEditorDlg::CancelSelection()
{
	if(!s_pSelGroup.empty())
	{
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(false);
		
		s_TransMgr.HideAllDummies();		

		// 清理组
		
		std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
		while(it != s_pSelGroup.end())
		{				
			(*it)->getParentSceneNode()->showBoundingBox(false);						

			++ it;
		}
		s_pSelGroup.clear();

		SCENE_MGR->ClearCurFocusObj();

		SCENE_MGR->m_SelMovableObjects.clear();
	}
}


// ---------------------------------------------------------------
// 左键ｕｐ
void CLogicEditorDlg::OnLButtonUp(float x, float y)
{
	if(CEntCrtDlg::sInst.m_MouseObj)
		return;


	if(!s_pSelGroup.empty())
	{	
		CEntPorpDlg::sInst.UpdateEntityPropertyList((*s_pSelGroup.begin()));
		g_MouseFlag = MOUSE_FLAG_FREEMOV;
	}	
}

// ---------------------------------------------------------------
// 左键双击
void CLogicEditorDlg::OnLButtonDClick(float x, float y)
{	
	this->CancelSelection();
	
	SelectEntityByMouse(x, y);	
}

// ---------------------------------------------------------------
// 移动 / 旋转 / 缩放
void CLogicEditorDlg::OnSceneMouseMove(const Ogre::Vector2& rScreenMove, float x, float y)
{
	// 清理选择框

	if(!(::GetKeyState(VK_LBUTTON) & 0x80))
	{
		if(g_pPathView && !g_pPathView->m_NodeList.empty())
		{
			g_pPathView->ClearNodes();
			g_pPathView->Update();
		}
	}

	if(s_pSelGroup.empty() || 
		!(g_MouseFlag & MOUSE_FLAG_TRANS) && !(g_MouseFlag & MOUSE_FLAG_FREEMOV))
	{

		// 框选效果
		// 与屏幕交点, 线显示
		if(g_MouseFlag & MOUSE_FLAG_GROUPSEL && g_pPathView)
		{
			g_Rect.ex = x;
			g_Rect.ez = y;
			
			Ray tCamRay1 = SCENE_CAM->getCameraToViewportRay( g_Rect.sx, g_Rect.sz);
			Ray tCamRay2 = SCENE_CAM->getCameraToViewportRay( g_Rect.sx, g_Rect.ez);
			Ray tCamRay3 = SCENE_CAM->getCameraToViewportRay( g_Rect.ex, g_Rect.ez);
			Ray tCamRay4 = SCENE_CAM->getCameraToViewportRay( g_Rect.ex, g_Rect.sz);
			
			if(g_pPathView->m_NodeList.size() == 0)
			{				
				g_pPathView->AddPathNode(tCamRay1.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f));
				g_pPathView->AddPathNode(tCamRay2.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f));
				g_pPathView->AddPathNode(tCamRay3.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f));
				g_pPathView->AddPathNode(tCamRay4.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f));
				g_pPathView->AddPathNode(tCamRay1.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f));
			}
			else
			{
				g_pPathView->m_NodeList[0] = tCamRay1.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f);
				g_pPathView->m_NodeList[1] = tCamRay2.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f);
				g_pPathView->m_NodeList[2] = tCamRay3.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f);
				g_pPathView->m_NodeList[3] = tCamRay4.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f);
				g_pPathView->m_NodeList[4] = tCamRay1.getPoint(SCENE_CAM->getNearClipDistance() * 2.0f);
			}
			g_pPathView->Update();

		}

		return;	
	}

	if((::GetKeyState(VK_LBUTTON) & 0x80) && !(::GetKeyState(VK_CONTROL) & 0x80))
	{
		// 移动
		if(s_TransMgr.GetEditType() == CTransformManager::ePOSITION)
		{				
			if(s_TransMgr.IsVisible()) // 锁定方向轴
			{
				Ogre::Vector3 pos = (*s_pSelGroup.begin())->getParentNode()->getPosition();
				Ogre::Vector3 newPos;
				s_TransMgr.GetViewportMove(pos, pos, rScreenMove, newPos);	
				s_TransMgr.SetDummyPos(newPos);

				// 组
		
				std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
				while(it != s_pSelGroup.end())
				{	
					(*it)->getParentNode()->setPosition(
						(*it)->getParentNode()->getPosition() + newPos - pos);				
					if((*it)->getMovableType() == "OTEEntity")
					{							
						((COTEActorEntity*)(*it))->TriggerEvent("OnEditMoved");
					}					
					++ it;
				}
			}
			else
			{
				Ogre::Ray r = SCENE_CAM->getCameraToViewportRay(x, y);
				Ogre::Vector3 p(0, 0, 0);		
				
				if(OTE::COTETilePage::GetCurrentPage() && OTE::COTETilePage::GetCurrentPage()->TerrainHitTest(r, p))
				{
					// nothing here
				}
				else
				{
					std::pair<bool,Ogre::Real> result = r.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0, 0)));
					p = r.getPoint(result.second);						
				}		
				
				// 组

				Ogre::Vector3 pos = (*s_pSelGroup.begin())->getParentNode()->getPosition();
				std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
				while(it != s_pSelGroup.end())
				{	
					if((p - pos).length() < 5.0f) // 防止过远距离移动
					{
						(*it)->getParentNode()->setPosition(
							(*it)->getParentNode()->getPosition() + p - pos);				
						if((*it)->getMovableType() == "OTEEntity")
						{							
							((COTEActorEntity*)(*it))->TriggerEvent("OnEditMoved");
						}				
					}
					++ it;
				}

				s_TransMgr.SetDummyPos(p);
			}
		}

		// 旋转

		else if(s_TransMgr.GetEditType() == CTransformManager::eROTATION)
		{
			Ogre::Vector3 pos = (*s_pSelGroup.begin())->getParentNode()->getPosition();
			Ogre::Quaternion q;		

			s_TransMgr.GetViewportRotate(pos, rScreenMove / 10.0f, q);		

			// 组
		
			std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
			while(it != s_pSelGroup.end())
			{	
				(*it)->getParentNode()->setOrientation(
					q * (*it)->getParentNode()->getOrientation());	
				Ogre::Vector3 v = (*it)->getParentNode()->getPosition() - pos;
				(*it)->getParentNode()->setPosition(pos + q * v);

				if((*it)->getMovableType() == "OTEEntity")
				{							
					((COTEActorEntity*)(*it))->TriggerEvent("OnEditRoted");
				}	

				++ it;
			}
		}

		// 缩放

		else if(s_TransMgr.GetEditType() == CTransformManager::eSCALING)
		{
			Ogre::Vector3 pos = (*s_pSelGroup.begin())->getParentNode()->getPosition();
			Ogre::Vector3 os, ns;
			os = (*s_pSelGroup.begin())->getParentNode()->getScale();

			s_TransMgr.GetViewportScaling(pos, rScreenMove / 10.0f, os, ns);

			//如果按下Shift则为等比缩放
			if(::GetKeyState( VK_SHIFT ) & 0x80)
			{
				float x = (ns.x - os.x) / os.x;
				float y = (ns.y - os.y) / os.y;
				float z = (ns.z - os.z) / os.z;
				
				float f = abs(x)>abs(y) ? x:y;
				f = abs(f)>abs(z) ? f:z;

				ns.x = os.x * (1.0f + f);
				ns.y = os.y * (1.0f + f);
				ns.z = os.z * (1.0f + f);
			}
		
			// 组
		
			std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
			while(it != s_pSelGroup.end())
			{	
				(*it)->getParentNode()->setScale(
					(*it)->getParentNode()->getScale() + ns - os);

				++ it;
			}

		}

		// shift拖动复制

		if( s_TransMgr.GetEditType() == CTransformManager::ePOSITION &&
			::GetKeyState( VK_SHIFT) & 0x80 && !(g_MouseFlag & MOUSE_FLAG_COPY))
		{
			ClearTrackBall();
			//CancelSelection();
			std::list<MovableObject*> tSelGroup;
			std::list<MovableObject*>::iterator ita = s_pSelGroup.begin();
			while(ita != s_pSelGroup.end())
			{
				tSelGroup.push_back(*ita);
				++ ita;
			}

			CancelSelection();

			std::list<MovableObject*>::iterator it = tSelGroup.begin();
			while(it != tSelGroup.end())
			{
				Ogre::MovableObject* m = EntityEditData_t(*it).CloneEntity();
				SelectSceneEntity( m );
				TrackBallTrack(new CreateTrack_t(m)); 

				++ it;
			}	
			s_TransMgr.ShowDummyA();

			g_MouseFlag |= MOUSE_FLAG_COPY;
		}
	}	

}

// ---------------------------------------------------------------
// 选择状态
void CLogicEditorDlg::OnBnClickedButton7()
{
	CLogicEditorDlg::s_EditState = "选择";
}

// ---------------------------------------------------------------
// 移除选择	
void CLogicEditorDlg::RemoveSelEntities()
{
	// 组
		
	std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
	while(it != s_pSelGroup.end())
	{	
		if((*it)->getName().find("Mag_") != std::string::npos)
		{
			TrackBallTrack(new DeleteTrack_t(*it));
			MAGIC_MGR->DeleteMagic((*it)->getName().c_str());
		}
		else
		{	
			if(OTE::COTETilePage::GetCurrentPage())
				OTE::COTETilePage::GetCurrentPage()->TriggerEvent("OnDeleteNpc");
		}
		++ it;
	}

	s_pSelGroup.clear();
	s_TransMgr.HideAllDummies();
	CEntEditDlg::sInst.UpdateSceneTree();
}

// ----------------------------------------------------------------
// 群选
void CLogicEditorDlg::RectSelEntities(const OTE::CRectA<float>& rect)
{
	if(!g_pPathView)
		return;

	float minX = std::min(rect.sx, rect.ex);
	float maxX = std::max(rect.sx, rect.ex);
	float minZ = std::min(rect.sz, rect.ez);
	float maxZ = std::max(rect.sz, rect.ez);

	// 前视图					
	
	if((DWORD)SCENE_CAM->getUserObject() & 0xF0000000)
	{
		// todo
	}

	// 俯视图

	else if((DWORD)SCENE_CAM->getUserObject() & 0x0F000000)
	{
		// todo						
	}

	// 右视图

	else if((DWORD)SCENE_CAM->getUserObject() & 0x00F00000)
	{
		// todo				
	}	
	else
	{
		Ray tCamRay1 = SCENE_CAM->getCameraToViewportRay( minX, minZ);
		Ray tCamRay2 = SCENE_CAM->getCameraToViewportRay( minX, maxZ);
		Ray tCamRay3 = SCENE_CAM->getCameraToViewportRay( maxX, maxZ);
		Ray tCamRay4 = SCENE_CAM->getCameraToViewportRay( maxX, minZ);
		Ray tCamRay5 = SCENE_CAM->getCameraToViewportRay( (rect.sx + rect.ex) / 2.0f , (rect.sz + rect.ez) / 2.0f);

		Vector3 camPos=SCENE_CAM->getPosition();
		Vector3 camDir=SCENE_CAM->getDirection();

		Plane plane1(tCamRay1.getOrigin()+tCamRay1.getDirection()*1.0f,camPos,tCamRay2.getOrigin()+tCamRay2.getDirection()*1.0f);
		Plane plane2(tCamRay1.getOrigin()+tCamRay1.getDirection()*1.0f,camPos,tCamRay4.getOrigin()+tCamRay4.getDirection()*1.0f);
		Plane plane3(tCamRay3.getOrigin()+tCamRay3.getDirection()*1.0f,camPos,tCamRay4.getOrigin()+tCamRay4.getDirection()*1.0f);
		Plane plane4(tCamRay2.getOrigin()+tCamRay2.getDirection()*1.0f,camPos,tCamRay3.getOrigin()+tCamRay3.getDirection()*1.0f);

		//the entities
		CancelSelection();

		std::vector<Ogre::MovableObject*> eList;
		SCENE_MGR->GetEntityList(&eList);
		std::vector<Ogre::MovableObject*>::iterator it =  eList.begin();
		
		while( it != eList.end() )
		{
			Ogre::MovableObject* e = *it;

			{		
				Vector3 tPos=e->getParentNode()->getPosition();
		
				Vector3 v=tPos-camPos;	
				v.normalise();
				Ray tRay(camPos+tCamRay5.getDirection(),v);

				pair<bool,Real> res1=tRay.intersects(plane1);
				pair<bool,Real> res2=tRay.intersects(plane2);
				pair<bool,Real> res3=tRay.intersects(plane3);
				pair<bool,Real> res4=tRay.intersects(plane4); 

				if( (!res1.first || (tPos-tRay.getPoint(res1.second)).dotProduct(v)<0) &&
					(!res2.first || (tPos-tRay.getPoint(res2.second)).dotProduct(v)<0) &&
					(!res3.first || (tPos-tRay.getPoint(res3.second)).dotProduct(v)<0) &&
					(!res4.first || (tPos-tRay.getPoint(res4.second)).dotProduct(v)<0)	  
				)
				{						
					SelectSceneEntity(e, true);
				}
			}

			++ it;
	        
		}	
	}
	
	g_pPathView->Update();

}

	
// ---------------------------------------------------------------
// 左键ｄｏｗｎ

void CLogicEditorDlg::OnLButtonDown(float x, float y)
{
	if(CEntCrtDlg::sInst.m_MouseObj)
		return;	

	// 是否在进行编辑操作

	if(s_TransMgr.IsVisible() && 
		!s_TransMgr.PickDummies(x, y).empty() &&
		!s_TransMgr.GetFocusedCoordString().empty()
		)
	{
		g_MouseFlag |= MOUSE_FLAG_TRANS; 
	}
	else if(s_pSelGroup.size() == 0)
		SelectEntityByMouse(x, y);
}

// ---------------------------------------------------------------
// 选择
bool CLogicEditorDlg::SelectEntityByMouse(float x, float y)
{
	bool bFind = false;

	if(!(::GetKeyState(VK_SHIFT) & 0x80))
	{
		// 点选
		Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay((float)x, (float)y);		

		// 点选物件
		
		Ogre::MovableObject* m = COTEActCollisionManager::PickTriangleIntersection(tCameraRay, "default");
		if(m)
		{
			bFind = true;
			if(m->getName().find("npc") != std::string::npos ||
			   m->getName().find("wp") != std::string::npos)
				SelectSceneEntity(m);	
		}		
		if(!bFind)
			SelectSceneEntity(NULL);					
	}

	return bFind;
}


// ---------------------------------------------------------------
void CLogicEditorDlg::OnSceneKeyDown()
{
	if(::GetKeyState( VK_ESCAPE ) & 0x80)
	{
		CancelSelection();
		CLogicEditorDlg::s_EditState = "选择";
	}
	else if(::GetKeyState( VK_DELETE ) & 0x80)
	{		
		ClearTrackBall();
		RemoveSelEntities();
	}
	
	// 拷贝

	if(::GetKeyState( VK_CONTROL) & 0x80 && ::GetKeyState( 'C' ) & 0x80)
	{
		s_pCopyGroup.clear();
		
		std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
		while(it != s_pSelGroup.end())
		{
			s_pCopyGroup.push_back(EntityEditData_t(*it));

			++ it;
		}
	}

	// 复制

	if(::GetKeyState( VK_CONTROL) & 0x80 && ::GetKeyState( 'V' ) & 0x80)
	{	
		ClearTrackBall();
		CancelSelection();

		std::list<EntityEditData_t>::iterator it = s_pCopyGroup.begin();
		while(it != s_pCopyGroup.end())
		{
			Ogre::MovableObject* m = (*it).CloneEntity();
			WAIT_LOADING_RES(m);
			SelectSceneEntity( m );
			TrackBallTrack(new CreateTrack_t(m)); 

			++ it;
		}	

		s_TransMgr.ShowDummyA();
	}		


	// 后退

	if(::GetKeyState( VK_CONTROL) & 0x80 && ::GetKeyState( 'Z' ) & 0x80)
	{	
		std::list<EntityEditData_t*>::iterator it = g_pTrackBallGroup.begin();
		while(it != g_pTrackBallGroup.end())
		{	
			if((*it)->EditDataType == "create" || (*it)->EditDataType == "delete")
			{
				// 如果在选择列表中则删除之

				std::list<Ogre::MovableObject*>::iterator ita = s_pSelGroup.begin();
				while(ita != s_pSelGroup.end())
				{								
					if((*ita)->getName() == (*it)->Name)
					{
						s_pSelGroup.erase(ita);
						break;
					}
					++ ita;
				}
			}

			(*it)->UpdateEntity();				
			++ it;
		}	
		g_pTrackBallGroup.clear();	
				
		if(!s_pSelGroup.empty())
			s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getWorldPosition());
		
	}

	if( ::GetKeyState('Q') & 0x80 )
	{
		if( !s_pSelGroup.empty() )
		{
			CLogicEditorDlg::s_EditState = "变换";
			s_TransMgr.ShowDummy( CTransformManager::ePOSITION );
			s_TransMgr.SetDummyPos( ( *s_pSelGroup.begin() )->getParentNode()->getPosition() );
		}
	}

	if( ::GetKeyState('W') & 0x80 )
	{
		if( !s_pSelGroup.empty() )
		{
			CLogicEditorDlg::s_EditState = "变换";
			s_TransMgr.ShowDummy( CTransformManager::eROTATION );
			s_TransMgr.SetDummyPos( ( *s_pSelGroup.begin() )->getParentNode()->getPosition() );
		}
	}

	if( ::GetKeyState('E') & 0x80 )
	{
		if( !s_pSelGroup.empty() )
		{
			CLogicEditorDlg::s_EditState = "变换";
			s_TransMgr.ShowDummy( CTransformManager::eSCALING );
			s_TransMgr.SetDummyPos( ( *s_pSelGroup.begin() )->getParentNode()->getPosition() );
		}
	}

	// 取消当前操作

	if( ::GetKeyState(VK_ESCAPE) & 0x80 )
	{
		s_TransMgr.HideAllDummies();

		if(m_Snap3DMark) // 磁性对齐
		{	
			SAFE_DELETE(m_Snap3DMark);					
			
			SCENE_MGR->getRootSceneNode()->removeChild("SnapHelper");
			SCENE_MGR->destroySceneNode("SnapHelper");

			CLogicEditorDlg::s_EditState	= "选择";
		}	
	}

	// 隐藏/显示

	if( (::GetKeyState(VK_CONTROL) & 0x80) && (::GetKeyState('H') & 0x80) )
	{
		std::vector<Ogre::MovableObject*> eList;
		SCENE_MGR->GetEntityList(&eList);

		std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
		while( it != eList.end() )
		{
			(*it)->setVisible(false);
			++ it;
		}

		std::list<Ogre::MovableObject*>::iterator ita = s_pSelGroup.begin();
		while( ita != s_pSelGroup.end() )
		{
			(*ita)->setVisible(true);
			++ ita;
		}

	}

	if( (::GetKeyState(VK_CONTROL) & 0x80) && (::GetKeyState('S') & 0x80) )
	{
		std::vector<Ogre::MovableObject*> eList;
		SCENE_MGR->GetEntityList(&eList);

		std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
		while( it != eList.end() )
		{
			(*it)->setVisible(true);
			++ it;
		}
	}

	// 传递编辑消息到相应的插件

	if( ::GetKeyState(VK_F1) & 0x80 || ::GetKeyState(VK_F2) & 0x80 || ::GetKeyState(VK_F3) & 0x80)
	{		
		if(!s_pSelGroup.empty())
		{
			ShowWindow(SW_HIDE); 			
		}
	}
	
	if( ::GetKeyState(VK_RETURN) & 0x80 )
	{
		// 磁性对齐

		if(CLogicEditorDlg::s_EditState ==  "Snap_started")
		{
			Ogre::Vector3 pos = (*s_pSelGroup.begin())->getParentNode()->getPosition();
			Ogre::Vector3 newPos = m_SnapEndPos - m_SnapStartPos + pos;		

			s_TransMgr.SetDummyPos(newPos);
			
			// 组

			std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
			while(it != s_pSelGroup.end())
			{	
				(*it)->getParentNode()->setPosition(
					(*it)->getParentNode()->getPosition() + newPos - pos);				

				++ it;
			}
			this->CancelSelection();
			CLogicEditorDlg::s_EditState =  "选择";
			
			if(m_Snap3DMark)
			{	
				SAFE_DELETE(m_Snap3DMark)					
				
				SCENE_MGR->getRootSceneNode()->removeChild("SnapHelper");
				SCENE_MGR->destroySceneNode("SnapHelper");				
			}	
		}
	}

	if(::GetKeyState( VK_LEFT ) & 0x80 || ::GetKeyState( VK_RIGHT ) & 0x80)
	{	
		if(s_TransMgr.GetEditType() == CTransformManager::ePOSITION)
		{
			float step = 0.1f;
			if(::GetKeyState( VK_CONTROL) & 0x80)
			{
				step = 0.01f;
			}
			if(::GetKeyState( VK_LEFT )   & 0x80)
			{
				step = - step;
			}

			std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
			while(it != s_pSelGroup.end())
			{
				Ogre::Vector3 oldPos = (*it)->getParentSceneNode()->getPosition();	
				if     (s_TransMgr.GetCoordAxis() == "X")
				{							
					(*it)->getParentSceneNode()->setPosition(oldPos + Ogre::Vector3(step, 0, 0));
				}
				else if(s_TransMgr.GetCoordAxis() == "Y")
				{
					(*it)->getParentSceneNode()->setPosition(oldPos + Ogre::Vector3(0, step, 0));
				}
				else if(s_TransMgr.GetCoordAxis() == "Z")
				{
					(*it)->getParentSceneNode()->setPosition(oldPos + Ogre::Vector3(0, 0, step));
				}

				++ it;
			}
		}
	}	
}
// --------------------------------------------------
void CLogicEditorDlg::OnBnClickedCheck1()
{
	if(CLogicEditorDlg::s_Inst.m_vLogicSectors.size() == 0)
		return;

	if(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED)
	{
		ShowSectorMap("LogicSectorMap1.bmp");
	}
	else
	{
		if(!g_LightmapName.empty())
			COTETerrainLightmapMgr::GetInstance()->SetLightMapMaterial(COTETilePage::GetCurrentPage(), COTETilePage::GetCurrentPage()->m_LightmapPath, g_LightmapName);

	}
}
