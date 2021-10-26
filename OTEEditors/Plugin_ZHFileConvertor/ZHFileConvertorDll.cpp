// --------------------------------------------------------

#include "stdafx.h"
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"
#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"
#include "OTETerrainSerializer.h"
#include "OTESceneXmlLoader.h"
#include "OTETerrainMaterialMgr.h"
#include "OTETerrainLightmapMgr.h"
#include "SectorDlg.h"
#include "OTEEntityVertexColorPainter.h"
#include "OTEEntityRender.h"
#include "OTEEntityLightmapPainter.h"
#include "OTEShadowRender.h"

using namespace Ogre; 
using namespace OTE;  

// ========================================================
// 全局变量
// ========================================================

int gZHFileConvertorOpenToolID = 0; 
int gZHFileConvertorSaveToolID = 0;

// ========================================================
// 回调函数
// ========================================================
void EntityWalKCB(Ogre::MovableObject* e)
{
	COTEShadowMap::AddShadowObj((COTEActorEntity*)e);

}


// ========================================================
// CZHFileConvertorPluginInterface
// ========================================================

class  _OTEExport CZHFileConvertorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID;
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
// --------------------------------------------------------
	virtual int CreateOTEToolGroup(int startToolID, HashMap<int, OTETool_t>& rToolGroup)
	{
		OTETool_t tTool;
		tTool.pPI = this;
		
		// button1
		tTool.ToolID		= startToolID;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "打开";
		tTool.IconFileName	= "gui/open.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;
		rToolGroup[tTool.ToolID]	= tTool;

		gZHFileConvertorOpenToolID = tTool.ToolID;

		// button2
		tTool.ToolID		= startToolID + 1;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "保存";
		tTool.IconFileName	= "gui/save.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;
		rToolGroup[tTool.ToolID]	= tTool;

		gZHFileConvertorSaveToolID = tTool.ToolID;		

		// return last id
		return tTool.ToolID;
	}

	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShifDwon)
	{		
		if(keyCode == 'O')
		{
			

		}
		return false;
	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		// 打开所有

		if(gZHFileConvertorOpenToolID == toolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			CSectorDlg dlg;
			if(dlg.DoModal() == IDOK)
			{				
				// 摄像机

				char str[128];

				::GetPrivateProfileString("ZHFileConvertor", "CameraPosition", "\0", str, sizeof(str), ".\\Editor.ini");
				
				float x = 0, y = 0, z = 0;

				if(sscanf(str, "%f %f %f", &x, &y, &z) != 3)
				{
					COTETilePage* page =  COTETilePage::GetCurrentPage();
					if(page)
					{
						x = page->m_PageSizeX / 2.0f;
						z = page->m_PageSizeZ / 2.0f;
						y = page->GetHeightAt(x, z) + 5.0f;						
					}

				}

				Ogre::Camera* cam = SCENE_CAM;
				cam->setPosition(Ogre::Vector3(x, y, z));

				::GetPrivateProfileString("ZHFileConvertor", "CameraDirection", "\0", str, sizeof(str), ".\\Editor.ini");

				if(sscanf(str, "%f %f %f", &x, &y, &z) == 3)
				{
					cam->setDirection(Ogre::Vector3(x, y, z));
				}
			}
		}		

		// 保存所有

		if(gZHFileConvertorSaveToolID == toolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());			
				
			Ogre::Vector3 camPos = SCENE_CAM->getPosition();

			std::stringstream ss;
			ss << camPos.x << " " << camPos.y << " " << camPos.z;
			WritePrivateProfileString("ZHFileConvertor", "CameraPosition", ss.str().c_str(), ".\\Editor.ini"); 
			ss.str("");
			Ogre::Vector3 camDir = SCENE_CAM->getDirection();
			ss << camDir.x << " " << camDir.y << " " << camDir.z;
			WritePrivateProfileString("ZHFileConvertor", "CameraDirection", ss.str().c_str(), ".\\Editor.ini"); 
				
			//COTEActorVertexColorPainter::GenLightDiffmap(
			//	(COTEActorEntity*)SCENE_MGR->GetCurObjSection(),
			//	SCENE_MAINLIGHT->getDirection(), SCENE_MAINLIGHT->getDiffuseColour(), SCENE_MGR->getAmbientLight(),  0);
		
		//	COTEActorEntity* ae = (COTEActorEntity*)SCENE_MGR->GetCurObjSection();	

			//{
			//	LightInfo_t* li = COTERenderLight::GetInstance()->GetLightKey();

			//	COTEEntityLightmapPainter::GenSceneLightmaps(
			//		//(COTEActorEntity*)SCENE_MGR->GetCurObjSection(),
			//		SCENE_MAINLIGHT->getDirection(),
			//		SCENE_MAINLIGHT->getDiffuseColour(), SCENE_MGR->getAmbientLight(),
			//		0.0f
			//		);
			//}

			CSectorDlg dlg;
			dlg.SaveScene();
			
		}	
	}

};


// ========================================================
// Plugin 接口处理
// ========================================================

CZHFileConvertorPluginInterface gPI;
// --------------------------------------------------------
extern "C" void __declspec(dllexport) dllStartPlugin( void )
{		
	COTEPluginManager::GetInstance()->AddPluginInterface(&gPI);			

}

// --------------------------------------------------------
extern "C" void __declspec(dllexport) dllStopPlugin( void )
{	
	// the plugin
	COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);
}
