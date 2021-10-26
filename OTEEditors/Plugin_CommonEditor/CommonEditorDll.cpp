// --------------------------------------------------------

#include "stdafx.h"
#include "OTEInterfaceInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"
#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"
#include "otelineobj.h"
#include "OTETerrainLightmapMgr.h"
#include "OTETerrainSerializer.h"
#include "OTESceneXmlLoader.h"
#include "OTEMagicManager.h"
#include "OTETrackManager.h"
#include "OTED3DManager.h"
#include "OTEActorMoveAIReactor.h"
#include "OTEReactorManager.h"
#include "OTEActorActionMgr.h"
#include "OTETieReactors.h"
#include "OTEBillBoardChain.h"
#include "OTETerrainReactor.h"
#include "OTEActorEquipmentMgr.h"
#include "OTEShadowRender.h"
#include "OTEActTrackUsage.h"

using namespace Ogre; 
using namespace OTE;  

// ========================================================
// 全局变量
// ========================================================

int gCommonEditorMenuID = 0;
int gCommonEditorOpenToolID = 0; 
int gCommonEditorSaveToolID = 0;
int gCommonEditorGridToolID = 0;

HWND							gMainWnd = NULL;	// 渲染窗口
Ogre::MovableObject*			gIHelperObj = NULL;

COTEActorMoveAIReactor*			g_ActorAI =	NULL;
COTEActorEntity*				g_TestPlayer = NULL;
COTEFootPrintReactor*			g_TestReactor = NULL;
COTEBillBoardChain*				g_TestBoardChain = NULL;
Ogre::MovableObject*			g_Magic = NULL;
COTEParabolaTrackCtrller*		g_ParabolaTrackCtrller = NULL;


// ========================================================
// 绘制网格的帮助物体
// ========================================================

class CDummyGrid
{
public:
	std::string			mEntName;
	bool				mIsVisible;

public:
	// ---------------------------------------------------
	CDummyGrid()
	{	
		mIsVisible = true;
	}
	// ---------------------------------------------------
	~CDummyGrid()
	{
		Destroy();
	}

public:
	// ---------------------------------------------------
	bool IsVisible()
	{
		return mIsVisible;
	}
	// ---------------------------------------------------
	void SetVisible(bool bVisible)
	{
		mIsVisible = bVisible;
	}
	// ---------------------------------------------------
	void Destroy()
	{
		ClearRes();
		mIsVisible = false;
	}
	// ---------------------------------------------------
	void ClearRes()		
	{	
		CLineObj* ent = NULL;
		if(!mEntName.empty())
		{
			SceneNode* sn = SCENE_MGR->GetSceneNode(mEntName);
			if(sn)
			{
				ent= (CLineObj*)(sn->getAttachedObject(0)); 				
				if(ent)								
					delete ent;					
				
				SCENE_MGR->getRootSceneNode()->removeChild(mEntName.c_str());
				SCENE_MGR->destroySceneNode(mEntName.c_str());

			}
			mEntName = "";
		}


		
	}
		
	// ---------------------------------------------------
	void DrawGridDummy()
	{	
		// clear first

		if(!mEntName.empty())
		{
			ClearRes();			
		}

		if(!mIsVisible)
			return;

		// create 

		Camera* cam = SCENE_CAM;

		mEntName = "Dummy.GridEnt";

		CLineObj* ent = new CLineObj(mEntName); 	
		
		SceneNode* node = SCENE_MGR->getRootSceneNode()->createChildSceneNode(mEntName, Vector3( 0, 0, 0 ));
		node->attachObject(ent); 	

		// 网格的默认尺寸
		int sceneUnit = 1;
		//默认网格数量
		int sceneN = 30;
		
		// 根据摄像机的位置决定方格尺寸大小
		Ray r = Ray(cam->getPosition(), cam->getDirection());			
		
		std::pair<bool,Real> result = r.intersects(Plane(Vector3::UNIT_Y, Vector3::ZERO));		

		if(result.second < 20)
		{
			sceneUnit = 1;
			sceneN = 30;
		}
		else if(result.second < 30)
		{
			sceneUnit = 2;
			sceneN = 30;
		}
		else if(result.second < 50)
		{
			sceneUnit = 2;
			sceneN = 50;
		}
		else if(result.second < 100)
		{
			sceneUnit = 5;
			sceneN = 30;
		}
		else if(result.second < 200)
		{
			sceneUnit = 5;
			sceneN = 80;
		}	
		else if(result.second < 500)
		{
			sceneUnit = 10;
			sceneN = 100;
		}	
		else
		{
			sceneUnit = 50;
			sceneN = 100;
		}

		// 根据网格尺寸决定画多大的网格

		int sceneLeft	= SCENE_MGR->m_Box.getMinimum().x;
		int sceneTop	= SCENE_MGR->m_Box.getMinimum().z;
		int sceneRight	= SCENE_MGR->m_Box.getMaximum().x;
		int sceneBottom = SCENE_MGR->m_Box.getMaximum().z;

		int centerX = (sceneLeft + sceneRight) / 2;
		int centerY = (sceneTop + sceneBottom) / 2;
		
		int maxLength = sceneUnit * sceneN /*20*20个格*/;

		sceneLeft   = centerX - maxLength / 2;
		sceneRight  = centerX + maxLength / 2;
		sceneTop    = centerY - maxLength / 2;
		sceneBottom = centerY + maxLength / 2;

		/// 一笔绘制蛇形网格

		// 横向的
		int cur = sceneTop;
		ent->addPoint(Vector3(sceneLeft, 0, cur));
		while(cur <= sceneBottom) 
		{
			ent->addPoint(Vector3(sceneRight, 0, cur));
			if(cur + sceneUnit <= sceneBottom) 
			{
				ent->addPoint(Vector3(sceneRight, 0, cur + sceneUnit));
				ent->addPoint(Vector3(sceneLeft, 0, cur + sceneUnit));
			}
			else
				ent->addPoint(Vector3(sceneLeft,		0, cur)); // 回到左边

			if(cur + 2 * sceneUnit <= sceneBottom) 		
				ent->addPoint(Vector3(sceneLeft, 0, cur + 2 * sceneUnit));
		

			cur += 2 * sceneUnit; 
		}
		
		// 绘制粗线

		ent->addPoint(Vector3(sceneLeft,  0, (sceneTop + sceneBottom) / 2 - sceneUnit * 0.01 ));
		ent->addPoint(Vector3(sceneRight, 0, (sceneTop + sceneBottom) / 2 - sceneUnit * 0.01 ));
		ent->addPoint(Vector3(sceneRight, 0, (sceneTop + sceneBottom) / 2 + sceneUnit * 0.01 ));
		ent->addPoint(Vector3(sceneLeft,  0, (sceneTop + sceneBottom) / 2 + sceneUnit * 0.01 ));

		// 纵向的
		cur = sceneLeft;
		ent->addPoint(Vector3(cur, 0, sceneTop));
		while(cur <= sceneRight)
		{
			ent->addPoint(Vector3(cur, 0, sceneBottom));
			if(cur + sceneUnit <= sceneRight) 
			{
				ent->addPoint(Vector3(cur + sceneUnit,		0, sceneBottom));
				ent->addPoint(Vector3(cur + sceneUnit,		0, sceneTop));
			}

			if(cur + 2 * sceneUnit <= sceneRight) 
				ent->addPoint(Vector3(cur + 2 * sceneUnit,	0, sceneTop));
			

			cur += 2 * sceneUnit; 
		}

		// 绘制粗线

		ent->addPoint(Vector3((sceneLeft + sceneRight) / 2 - sceneUnit * 0.01,  0, sceneBottom));
		ent->addPoint(Vector3((sceneLeft + sceneRight) / 2 - sceneUnit * 0.01,  0, sceneTop));
		ent->addPoint(Vector3((sceneLeft + sceneRight) / 2 + sceneUnit * 0.01,  0, sceneTop));
		ent->addPoint(Vector3((sceneLeft + sceneRight) / 2 + sceneUnit * 0.01,  0, sceneBottom));

		
		ent->changeColor(ColourValue(.2,.2,.4)); // 片蓝色

		ent->drawLines(); 
	}
	

};

CDummyGrid				gGridDummy;			// 帮助物体

// ========================================================
// 左下角XYZ帮助物体
// ========================================================

// --------------------------------------------------------
// 创建XYZ帮助物体

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
struct CUSTOMVERTEX{ FLOAT x, y, z;   DWORD color; };

Ogre::HardwareVertexBufferSharedPtr	bf;
const D3D9HardwareVertexBuffer*		d3dHB;

HRESULT InitXYZHelper()
{  
    CUSTOMVERTEX vertices[] =
    {
        {  0.0f, 0.0f, 0.0f, 0xffff0000		},
		{  0.15f, 0.0f, 0.0f, 0xffff0000	},		

		{  0.0f, 0.0f, 0.0f, 0xff00ff00		},
		{  0.0f, 0.15f, 0.0f, 0xff00ff00	},		

		{  0.0f, 0.0f, 0.0f, 0xff0000ff		},
		{  0.0f, 0.0f, 0.15f, 0xff0000ff	}	
    };

	bf = 
		Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				sizeof(CUSTOMVERTEX), 
				6, 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);	
 

    // Fill the vertex buffer.
	d3dHB = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(bf.getPointer());

	void* pVertices = bf->lock(Ogre::HardwareBuffer::HBL_DISCARD);     
    memcpy( pVertices, vertices, sizeof(vertices) );
	bf->unlock();

    return S_OK;
}



// ========================================================
// 渲染回调
// ========================================================

// 第一祯渲染
void RenderOnce()
{
	// 初始化坐标帮助物体	
	InitXYZHelper();

}

// ----------------------------------------------------
void OnSceneRender(int ud)
{
	WINDOWPLACEMENT wndpl; // 最小化窗口后程序挂起

	if(::GetWindowPlacement(gMainWnd, &wndpl))
	{
		if(wndpl.showCmd == SW_SHOWMINIMIZED) 
			Sleep(500);
	}

	if(!gGridDummy.IsVisible())
		return;

	Camera* cam = SCENE_CAM;	

	// The d3d device
	
	LPDIRECT3DDEVICE9 d3dDevice = COTED3DManager::GetD3DDevice();
	// 第一祯渲染	
	static bool bInit = false;	if(!bInit){	RenderOnce();	bInit = true;}

	RENDERSYSTEM->_disableTextureUnitsFrom(0);
	d3dDevice->SetPixelShader(0);
	d3dDevice->SetVertexShader(0);
	

	/*********************** 渲染XYZ坐标轴帮助物体 ***********************/	
	{
		
		// ----------------------------------------------------
		// 保存现场

		// 矩阵
		
		D3DXMATRIX matrix, oldWorldMat, oldViewMat, oldPrjMat;
		d3dDevice->GetTransform(D3DTS_WORLD,	   &oldWorldMat); // SAVE TRANSFORM
		d3dDevice->GetTransform(D3DTS_VIEW,		   &oldViewMat); 
		d3dDevice->GetTransform(D3DTS_PROJECTION , &oldPrjMat); 
			
		// 灯光

		DWORD oldLighting;
		d3dDevice->GetRenderState(D3DRS_LIGHTING, &oldLighting);
		DWORD oldAmbient;
		d3dDevice->GetRenderState(D3DRS_AMBIENT, &oldAmbient);	
		DWORD oldColor;
		d3dDevice->GetRenderState(D3DRS_COLORVERTEX, &oldColor);	
		
		// 雾
		DWORD fog;
		d3dDevice->GetRenderState(D3DRS_FOGVERTEXMODE, &fog);

		// ----------------------------------------------------
		// 渲染

		// 设置矩阵

		D3DXMatrixIdentity(&matrix);	
		d3dDevice->SetTransform(D3DTS_WORLD, &matrix);
		matrix = oldViewMat;	
				
		matrix(3, 0) = 2.5;
		matrix(3, 1) = -1.8;
		matrix(3, 2) = 5.0;		

 		d3dDevice->SetTransform(D3DTS_VIEW, &matrix);

		// 设置灯光

		d3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	
		d3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		d3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, FALSE );
		d3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		// 贴图
		d3dDevice->SetTexture( 0, NULL);

		// DRAW
		d3dDevice->SetStreamSource( 0, d3dHB->getD3D9VertexBuffer(), 0, sizeof(CUSTOMVERTEX) );
		d3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		d3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
		

		// ----------------------------------------------------
		// 还原现场

		d3dDevice->SetRenderState( D3DRS_COLORVERTEX,	oldColor);	
		d3dDevice->SetRenderState( D3DRS_AMBIENT,		oldAmbient );
		d3dDevice->SetRenderState( D3DRS_LIGHTING,		oldLighting );	
		d3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,  fog);

		d3dDevice->SetTransform(D3DTS_WORLD ,			&oldWorldMat); // SET SAVED TRANSFORM
		d3dDevice->SetTransform(D3DTS_VIEW ,			&oldViewMat); 
		d3dDevice->SetTransform(D3DTS_PROJECTION ,		&oldPrjMat); 
	}

	/*********************** 更新网格帮助物体 ***********************/	
	static Ogre::Vector3 oldCamPos;
	if((oldCamPos - cam->getPosition()).length() > 1) // 在摄像机移动时更新
	{		
		gGridDummy.DrawGridDummy(); 
		oldCamPos = cam->getPosition();
	}
	
	
}

// --------------------------------------------------------
// 场景清理
void OnClearRender(int ud)
{	
	gIHelperObj = NULL;
			
	// 清理

	gGridDummy.Destroy();	
}

// ------------------------------------------------------------------
// 接受拖拽事件

WNDPROC			gOldWndProc			= NULL;
LRESULT CALLBACK D_ZikeWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	long tResult = 0;

	if ( uMsg == WM_DROPFILES)
	{
		char tPathName[256] = {0}, tEntityName[256] = {0};
		::DragQueryFile( (HDROP) wParam, 0, tPathName, 256 );

		if ( ::strlen( tPathName ) >= 6 )
		{
			char* tpFileName = tPathName + ::strlen( tPathName ) - 5;

			if ( ::stricmp( tpFileName, ".mesh" ) == 0 )
			{
				for ( int i = ::strlen( tPathName ); i >= 0; i -- )
				{
					if ( tPathName[i] == '\\' )
					{
						tpFileName = tPathName + i + 1;
						break;
					}
				}

				static int tIndex = 0;			

 
				//create entity
				
				{
					::sprintf( tEntityName, "ENT%.4d", ++ tIndex );
					Entity* tpEntity = SCENE_MGR->createEntity( tEntityName, tPathName/*tpFileName*/ );
					SceneNode* sn=SCENE_MGR->getRootSceneNode( )->createChildSceneNode( );
					
					sn->setPosition(Ogre::Vector3::ZERO );
					sn->attachObject( tpEntity );

				}		   				
               
			}
		}
		

		::DragFinish( (HDROP) wParam );
	}
	else
	{
		tResult = (*gOldWndProc)( hWnd, uMsg, wParam, lParam );
	}

	return tResult;
}


// --------------------------------------------------------
// 事件响应
// --------------------------------------------------------
void OnBehitActionEnd(const CEventObjBase::EventArg_t& rArg)
{
	if(rArg.strUserData == OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Behit1"))
	{
		COTEActorEntity* ae = SCENE_MGR->GetEntity(rArg.strObjName);
		if(ae)
		{
			COTEActorActionMgr::SetAction(ae, 
				OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Idle"), 
				COTEAction::eCONTINUES);
		}
	}
}
// --------------------------------------------------------
void OnBehit(const CEventObjBase::EventArg_t& rArg)
{
	COTEActorEntity* ae = SCENE_MGR->GetEntity(rArg.strObjName);
	if(ae)
	{
		if(ae->GetCurAction())
			ae->GetCurAction()->Stop(false);
		COTEActorActionMgr::SetAction(ae, OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Behit1"), COTEAction::ePLAY_ONCE);
		ae->AddEventHandler("OnActionEnd", OnBehitActionEnd);		
	}
}
// --------------------------------------------------------
void OnActionMiss(const CEventObjBase::EventArg_t& rArg)
{
	COTEActorEntity* ae = SCENE_MGR->GetEntity(rArg.strObjName);
	if(ae)
	{
		if(ae->GetCurAction())
			ae->GetCurAction()->Stop(false);			
	}
}

// ========================================================
// CCommonEditorPluginInterface
// ========================================================

class  _OTEExport CCommonEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID;
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CCommonEditorPluginInterface()
	{			
	}
	// --------------------------------------------------------
	virtual void Init(HWND mainWnd)
	{			
		gMainWnd = mainWnd;
		
		/*gOldWndProc = (WNDPROC)::SetWindowLongPtr( gMainWnd, GWLP_WNDPROC, (INT_PTR) D_ZikeWndProc );
		::DragAcceptFiles( gMainWnd, true );*/

	}
	// --------------------------------------------------------
	virtual void OnMenuEven(int menuID)
	{
		if(gCommonEditorMenuID == menuID)
		{
		}
	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		// 打开所有

		if(gCommonEditorOpenToolID == toolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			// 场景

			CWinApp* pApp = AfxGetApp(); 

			static CString path;
			path = pApp->GetProfileString("OTE文件路径配置", "打开场景");

			CHAR curDir[256];
			::GetCurrentDirectory(256, curDir);			
			
			CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
				NULL,
				NULL,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"场景信息 (*.xml)|*.xml|All Files (*.*)|*.*||",
				NULL 
				); 

			tpFileDialog->m_ofn.lpstrInitialDir = path;

			int nResponse =tpFileDialog->DoModal();
			::SetCurrentDirectory(curDir);

			if (nResponse == IDOK)
			{ 		
				std::string file = tpFileDialog->GetFileName().GetString();
				if(file.find(".xml") != -1)
					file.erase(file.length() - 4);

				CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
						tpFileDialog->GetFileName().GetLength());  
				pApp->WriteProfileString("OTE文件路径配置", "打开场景", strPath);	

				gGridDummy.Destroy();

				// 山体 
				
				COTESceneXmlLoader::GetInstance()->LoadFromFile((/*OTEHelper::GetResPath("ResConfig", "SceneXmlPath") + */file + ".xml").c_str());
		
				// 摄像机

				char str[128];

				::GetPrivateProfileString("CommonEditor", "CameraPosition", "\0", str, sizeof(str), ".\\Editor.ini");
				
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

				::GetPrivateProfileString("CommonEditor", "CameraDirection", "\0", str, sizeof(str), ".\\Editor.ini");

				if(sscanf(str, "%f %f %f", &x, &y, &z) == 3)
				{
					cam->setDirection(Ogre::Vector3(x, y, z));
				}					
				
			}		
			delete tpFileDialog; 
		}		

		// 保存所有

		if(gCommonEditorSaveToolID == toolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			// 场景

			CHAR curDir[256];
			::GetCurrentDirectory(256, curDir);			
			
			CFileDialog* tpFileDialog=new CFileDialog(false, // TRUE for FileOpen, FALSE for FileSaveAs
				NULL,
				NULL,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"场景信息 (*.xml)|*.xml|All Files (*.*)|*.*||",
				NULL 
				); 

			tpFileDialog->m_ofn.lpstrInitialDir  = OTEHelper::GetResPath("ResConfig", "SceneXmlPath");
			int nResponse =tpFileDialog->DoModal();
			::SetCurrentDirectory(curDir);

			if (nResponse == IDOK)
			{ 		
				if(COTETilePage::GetCurrentPage())
				{
					std::string sceneName = COTETilePage::GetCurrentPage()->m_TerrainName;				

					// 山体 
					
					COTESceneXmlLoader::GetInstance()->SaveToFile((OTEHelper::GetResPath("ResConfig", "SceneXmlPath") + sceneName + ".xml").c_str());

					//COTESceneXmlLoader::GetInstance()->m_Light.writeConfigFile();
					COTETerrainXmlSerializer::GetSingleton()->SaveTerrain((OTEHelper::GetResPath("ResConfig", "TerrainXmlPath") + sceneName + ".ter").c_str(), 
						COTETilePage::GetCurrentPage()); 
				}
				else
				{
					std::string file = tpFileDialog->GetFileName().GetString();
					if(file.find(".xml") != -1)
						file.erase(file.length() - 4);

					COTESceneXmlLoader::GetInstance()->SaveToFile((OTEHelper::GetResPath("ResConfig", "SceneXmlPath") + file + ".xml").c_str());
				}
				
				Ogre::Vector3 camPos = SCENE_CAM->getPosition();

				std::stringstream ss;
				ss << camPos.x << " " << camPos.y << " " << camPos.z;
				WritePrivateProfileString("CommonEditor", "CameraPosition", ss.str().c_str(), ".\\Editor.ini"); 
				ss.str("");
				Ogre::Vector3 camDir = SCENE_CAM->getDirection();
				ss << camDir.x << " " << camDir.y << " " << camDir.z;
				WritePrivateProfileString("CommonEditor", "CameraDirection", ss.str().c_str(), ".\\Editor.ini"); 
			
			}	

			delete tpFileDialog; 
		}

		if(gCommonEditorGridToolID == toolID)
		{		
			if(gGridDummy.IsVisible())
				gGridDummy.SetVisible(false);
			else
				gGridDummy.SetVisible(true);

			gGridDummy.DrawGridDummy();				
		}
	}
	// --------------------------------------------------------
	virtual bool OnMouseMove(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		static CPoint oldPos = CPoint(x, y);

		if((::GetKeyState(VK_MBUTTON) & 0x80) || 
			(::GetKeyState(VK_MENU) & 0x80) && 
			(::GetKeyState(VK_LBUTTON) & 0x80) )
		{			
			// ------------------------------------------------------------
			// 平移摄像机
			// ------------------------------------------------------------			
			Ogre::Camera* cam = SCENE_CAM;

			if(cam->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
			{
				Ogre::Vector3 camPos = cam->getPosition();
								
				Ogre::Vector2 screenMove(x - oldPos.x, y - oldPos.y);
				
				// 前视图

				if((DWORD)cam->getUserObject() & 0xF0000000)
				{
						cam->setPosition(
							Ogre::Vector3(
							camPos.x - screenMove.x * 0.01f,
							camPos.y + screenMove.y * 0.01f, 
							camPos.z)
							);
				}

				// 俯视图

				if((DWORD)cam->getUserObject() & 0x0F000000)
				{
						cam->setPosition(
							Ogre::Vector3(
							camPos.x - screenMove.x * 0.1f,
							camPos.y, 
							camPos.z - screenMove.y * 0.1f)
							);
				}

				// 右视图

				if((DWORD)cam->getUserObject() & 0x00F00000)
				{
						cam->setPosition(
							Ogre::Vector3(
							camPos.x,
							camPos.y + screenMove.y * 0.01f, 
							camPos.z + screenMove.x * 0.01f)
							);
				}

			}
		}

		// --------------------------------------------
		// 角色控制
		// --------------------------------------------

		if(gIHelperObj && (::GetKeyState(VK_LBUTTON) & 0x80) &&
			COTEPluginManager::GetInstance()->GetUserString("EditState") != "true")
		{
			if(!g_ActorAI)
				return false;

			CRect r;	
			::GetWindowRect(hwnd, r);	

			g_ActorAI->SetCursorPos((float)x / r.Width(), (float)y / r.Height());

			return true;		
		}


		oldPos.x = x;
		oldPos.y = y;

		// --------------------------------------------
		// 摄像机控制
		// --------------------------------------------
		if(gIHelperObj && (::GetKeyState(VK_RBUTTON) & 0x80))
		{			
			//Ogre::Vector3 pos = rCenter;
			//Ogre::Vector3 campos = rCenter + CGameCamera::getSingleton().m_VecFromPlyer;
		
			//// 距离(pos 为相对原点)
			//Ogre::Vector2 vec(CGameCamera::getSingleton().m_VecFromPlyer.x, CGameCamera::getSingleton().m_VecFromPlyer.z);
			//float dis = vec.length();

			///// 一阶近似			
			//float sita = + (cursorPos.x - oldCursorPos.x) / 5.0f * 3.1416 / 180;  // 角度

			//oldCursorPos = CGameCursor::getSingleton().GetPos();

			//RECT rect;
			//::GetWindowRect(hWnd, &rect);

			//if (cursorPos.y < rect.top + 30.0f || cursorPos.x < rect.left + 20.0f || cursorPos.x > rect.right - 20.0f || cursorPos.y > rect.bottom - 30.0f)
			//{
			//	CGameCursor::getSingleton().SetPos((rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2);
			//	oldCursorPos.x = (rect.right - rect.left) / 2;
			//	oldCursorPos.y = (rect.bottom - rect.top) / 2;
			//}

			//float x = campos.x - pos.x;
			//float y = campos.z - pos.z;
			//
			//x = x - y * sita; 
			//y = y + x * sita;				
			//
			//// 得到摄像机新位置
			//campos.x = x + pos.x; 
			//campos.z = y + pos.z;
			//campos.y = pos.y = 0;

			//// 纠正位置
			//campos = (campos - pos).normalisedCopy() * dis + pos;

			//float old_y = CGameCamera::getSingleton().m_VecFromPlyer.y;
			//CGameCamera::getSingleton().m_VecFromPlyer = campos - rCenter;
			//CGameCamera::getSingleton().m_VecFromPlyer.y = old_y;

			//campos = CGameCamera::getSingleton().m_VecFromPlyer + rCenter;


			//CGameCamera::getSingleton().m_pOgreCamera->setPosition(campos);
			//CGameCamera::getSingleton().m_pOgreCamera->lookAt(rCenter);

			//return true;
		}

		return false;
	}
	// --------------------------------------------------------
	virtual bool OnWheel(HWND hwnd, int x, int y, int delta, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(SCENE_CAM->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
		{
			Ogre::Vector3 camOldPos = SCENE_CAM->getPosition();

			// 摄像机远近移动		
			
			if(delta < 0) 
			{			
				float nearClip = SCENE_CAM->getNearClipDistance();
				SCENE_CAM->setNearClipDistance(nearClip * 1.1f);
				
			}
			if(delta > 0)
			{				
				float nearClip = SCENE_CAM->getNearClipDistance();
				SCENE_CAM->setNearClipDistance(nearClip * 0.9f);				
			}
		}
		return false;
	}
	// --------------------------------------------------------
	virtual bool CreateLButtonUpListener(){return true;}
	// --------------------------------------------------------
	virtual bool CreateKeyEventListener(){return true;}
	
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

		gCommonEditorOpenToolID = tTool.ToolID;

		// button2
		tTool.ToolID		= startToolID + 1;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "保存";
		tTool.IconFileName	= "gui/save.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;
		rToolGroup[tTool.ToolID]	= tTool;

		gCommonEditorSaveToolID = tTool.ToolID;

		// button3
		tTool.ToolID		= startToolID + 2;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "网格";
		tTool.IconFileName	= "gui/grid.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;
		rToolGroup[tTool.ToolID]	= tTool;

		gCommonEditorGridToolID = tTool.ToolID;

		// return last id
		return tTool.ToolID;
	}

	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShifDwon)
	{
		// 摄像机位置
		
		if(keyCode == 'C' && !isControlDown)
		{
			// 摄像机
			Ogre::Camera* cam = SCENE_CAM;

			if(COTETilePage::GetCurrentPage())
			{				
				float x = COTETilePage::GetCurrentPage()->m_PageSizeX / 2.0f;
				float z = COTETilePage::GetCurrentPage()->m_PageSizeZ / 2.0f;
				float y = COTETilePage::GetCurrentPage()->GetHeightAt(x, z) + 5.0f;

				cam->setPosition(Ogre::Vector3(x, y, z));
			}
			else
			{
				cam->setPosition(Ogre::Vector3(0, 10, 10));
				cam->lookAt(Ogre::Vector3(0, 0, 0));
			}

		}

		// 帮助物体
		
		if(keyCode == 'I')
		{			
			if(!gIHelperObj)
			{
				COTEPluginManager::GetInstance()->SetUserString("EditState", "false");

				// 创建一个角色实体
				//gIHelperObj = SCENE_MGR->CreateEntity("红头发女NPC.mesh", "PlayerHelper");				
				
				gIHelperObj = SCENE_MGR->CreateEntity(OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperFileName"), "PlayerHelper");	
				WAIT_LOADING_RES(gIHelperObj)								
				
				//OTE::COTEActorEquipmentMgr::SetEquipment((COTEActorEntity*)gIHelperObj, "上身", "fz30000t_2x.submesh");
				//OTE::COTEActorEquipmentMgr::SetEquipment((COTEActorEntity*)gIHelperObj, "上身1", "fz30000t_3s.submesh");
				//OTE::COTEActorEquipmentMgr::SetEquipment((COTEActorEntity*)gIHelperObj, "三大法", "fz30000t_4j.submesh");
				//OTE::COTEActorEquipmentMgr::SetEquipment((COTEActorEntity*)gIHelperObj, "头", "toufa_6toufa.submesh");
				//OTE::COTEActorEquipmentMgr::UpdateEquipments((COTEActorEntity*)gIHelperObj);
				//OTE::COTEActorEquipmentMgr::UpdateEquipments((COTEActorEntity*)gIHelperObj);
				//OTE::COTEActorEquipmentMgr::UpdateEquipments((COTEActorEntity*)gIHelperObj);

				CRect r;	
				::GetWindowRect(::GetActiveWindow(), r);

				CPoint tP;		GetCursorPos(&tP);			

				Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay((float)tP.x / r.Width(), (float)tP.y / r.Height());
				Vector3 pos;
				if(COTETilePage::GetCurrentPage())
					COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos);
				else
				{							
					std::pair<bool,Ogre::Real> result = ray.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0, 0)));
					pos = ray.getPoint(result.second);	
				}

				// 移动摄像机 

				SCENE_CAM->setPosition(pos + Ogre::Vector3(0.0f, 15.0f, 15.0f));
				SCENE_CAM->lookAt(pos);					
				
				if(gIHelperObj)
				{
					gIHelperObj->getParentSceneNode()->setPosition(pos);

					SCENE_MGR->SetCurFocusObj(gIHelperObj);

					if(COTETilePage::GetCurrentPage())
						COTETilePage::GetCurrentPage()->SetBaseHeight(gIHelperObj->getParentSceneNode()->getPosition().y - 5.0f);
                    					
				}

				// update aabb

				COTEEntityPhysicsMgr::GetTriangleIntersection((COTEActorEntity*)gIHelperObj, Ogre::Ray());

			//	COTEShadowMap::AddShadowObj((COTEActorEntity*)gIHelperObj);	
				

			}
			else
			{
				
				SCENE_MGR->RemoveEntity(gIHelperObj->getName(), false);

				gIHelperObj = NULL;				
			}


			/*COTERenderTexture* rt = COTERenderTexture::Create("ssss", 128, 128);

			WAIT_LOADING_RES(gIHelperObj)

			rt->BindObj((Ogre::Entity*)gIHelperObj);		*/

			//Ogre::MovableObject* mo = MAGIC_MGR->AutoCreateMagic(".BBS");
			//SCENE_MGR->AttachObjToSceneNode(mo);

			//COTETrack* tk = COTETrackManager::GetInstance()->CreateNewTrack(10000);		
			//COTEKeyFrame* kf = COTEKeyFrame::CreateNewKeyFrame();
			//kf->m_TextureName = "water_reflection_rt";
			////kf->m_Position = gIHelperObj->getParentSceneNode()->getPosition() + Ogre::Vector3(0, 4.0, 0);
			//mo->getParentSceneNode()->setPosition(gIHelperObj->getParentSceneNode()->getPosition() + Ogre::Vector3(0, 4.0, 0));
			//
			//tk->AddKeyFrame(kf);
			//COTEActTrackUsage::UpdateTrackBinding(NULL, tk, mo, 0);


		} 
		
		if(keyCode == 'O')
		{
			//g_TestBoardChain = MAGIC_MGR->AutoCreateMagic("XXX.BBC");

		}


		// 编辑对象 

		if( ::GetKeyState(VK_F1) & 0x80 || ::GetKeyState(VK_F2) & 0x80 || ::GetKeyState(VK_F3) & 0x80)
		{	
			if(gIHelperObj)
			{
				SCENE_MGR->SetCurFocusObj(gIHelperObj);				
			}
		}

		// 三视图

		if(!(::GetKeyState(VK_CONTROL) & 0x80) &&
			(keyCode == 'F' || keyCode == 'T' ||  keyCode == 'R')
			)
		{		
			static Ogre::Vector3 oldPos;
			static Ogre::Vector3 oldDir;
			static Ogre::ProjectionType oldPT;
			static Ogre::Radian oldFOVy;
			float	oldNCDDis = 0.1f;
			float	oldMaxDis = 100.0f;

			Ogre::Camera* cam = SCENE_CAM;

			if(cam->getProjectionType() != Ogre::PT_ORTHOGRAPHIC)
			{
				// 保存现场

				oldPos = cam->getPosition();
				oldDir = cam->getDirection();
				oldPT = cam->getProjectionType();
				oldFOVy = cam->getFOVy();
				oldNCDDis = cam->getNearClipDistance();	

				cam->setNearClipDistance( 2.0f );				
				
				cam->setFOVy(Radian(Degree(90.0f))); 
				cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

				// 计算中心点
				CRect r;	
				::GetWindowRect(::GetActiveWindow(), r);

				CPoint tP;		GetCursorPos(&tP);			

				Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay((float)tP.x / r.Width(), (float)tP.y / r.Height());
				Vector3 centerPos;

				if(SCENE_MGR->GetCurObjSection())
				{
					centerPos = SCENE_MGR->GetCurObjSection()->getParentSceneNode()->getPosition();
				}
				else
				{
					if(COTETilePage::GetCurrentPage())
						COTETilePage::GetCurrentPage()->TerrainHitTest(ray, centerPos);
					else
					{							
						std::pair<bool,Ogre::Real> result = ray.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0, 0)));
						centerPos = ray.getPoint(result.second);	
					}
				}


				if(keyCode == 'F') // 前视图
				{
					cam->setPosition(centerPos + Ogre::Vector3(0.0f, 0.0f, 60.0f));
					cam->setDirection(0.0f, 0.0f, -1.0f);
					cam->setUserObject((Ogre::UserDefinedObject*)0xF0000000);
				}
				else if(keyCode == 'T') // 俯视图
				{
					cam->setPosition(centerPos + Ogre::Vector3(0.0f, 260.0f, 0.0f));
					cam->setDirection(0.0f, -1.0f, 0.000001f);
					cam->roll(Degree(180.0f));
					cam->setUserObject((Ogre::UserDefinedObject*)0x0F000000);
				}
				else if(keyCode == 'R') // 右视图
				{
					cam->setPosition(centerPos + Ogre::Vector3(60.0f, 0.0f,  0.0f));
					cam->setDirection(-1.0f, 0.0f, 0.0f);	
					cam->setUserObject((Ogre::UserDefinedObject*)0x00F00000);
				}
					
			}
			else
			{	
				cam->setNearClipDistance( oldNCDDis );
					
				cam->setFOVy(oldFOVy); 
				cam->setProjectionType(oldPT);

				cam->setPosition(oldPos);
				cam->setDirection(oldDir);
				// cam->roll(Degree(-180.0f));	
				cam->setUserObject((Ogre::UserDefinedObject*)0x00000000);
			
			}	
		}

		if(keyCode == 186)
		{
			//if(!g_TestReactor)
			//{				
			//	g_TestReactor = (COTEFootPrintReactor*)COTEReactorManager::GetSingleton()->CreateReactor("TER.FootPrint", -1);
			//	g_TestReactor->Trigger(NULL);
			//	//g_Magic = MAGIC_MGR->CreateMagic("DDD.BillBoard", "Ent_xxxxxx");
			//	//SCENE_MGR->SetCurFocusObj(g_Magic);
			//}
			if(!g_ParabolaTrackCtrller)
			{
				g_ParabolaTrackCtrller = (COTEParabolaTrackCtrller*)COTEReactorManager::GetSingleton()->CreateReactor("TC.Target.1", -1);
			}
			g_ParabolaTrackCtrller->Trigger(gIHelperObj);
			gIHelperObj->getParentSceneNode()->setPosition(Ogre::Vector3::ZERO);

		}

		return false;

	}
	// --------------------------------------------------------
	// 这里实现角色控制部分逻辑
	// --------------------------------------------------------
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(!gIHelperObj)
			return false;

		if(COTEPluginManager::GetInstance()->GetUserString("EditState") != "true")
		{
			// 打击目标
			
			CRect r;	
			::GetWindowRect(hwnd, r);

			Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay((float)x / r.Width(), (float)y / r.Height());	

			MovableObject* m = COTEActCollisionManager::PickTriangleIntersection(tCameraRay, "default");
			if(m)
			{
// #######################################
				COTEAction* ac = COTEActorActionMgr::SetAction((COTEActorEntity*)gIHelperObj, OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Attack1"));
				if(ac)
				{	
					ac->BindEffectsToTarget(m);					
					ac->GetTrackCtrllerInstance()->SetLength(1.0f);
						
					((COTEActorEntity*)gIHelperObj)->AddEventHandler("OnHitTarget",  OnBehit);
					((COTEActorEntity*)gIHelperObj)->AddEventHandler("OnMissTarget", OnActionMiss);					
					
				}			
// #######################################
				return true;
			}
			else
			{

				// 移动目标

				if(g_ActorAI)
				{
					g_ActorAI->SetCursorPos((float)x / r.Width(), (float)y / r.Height());

					return true;
				}
			}
		}

		return false;
	}

	// --------------------------------------------------------
	virtual bool OnLButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		
		return false;
	}

};


// --------------------------------------------------------
// 角色逻辑
// --------------------------------------------------------

void OnActorLogicRender(int ud)
{
	if(gIHelperObj	&& 
			COTEPluginManager::GetInstance()->GetUserString("EditState") != "true")
	{
		if(!g_ActorAI)
		{
			g_ActorAI = (COTEActorMoveAIReactor*)COTEReactorManager::GetSingleton()->CreateReactor("AI.Move", -1);
			g_ActorAI->Init(gIHelperObj);
			g_ActorAI->SetTargetPos( ((COTEActorEntity*)gIHelperObj)->GetParentSceneNode()->getPosition() );
		}
		static DWORD sTimeCnt = ::GetTickCount();
		g_ActorAI->Update(gIHelperObj, (::GetTickCount() - sTimeCnt) / 1000.0f);
		sTimeCnt = ::GetTickCount();
	}
}

// --------------------------------------------------------
// 反应器测试
// --------------------------------------------------------

void OnReactorTestRender(int ud)
{
	if(g_TestBoardChain)
	{
		g_TestBoardChain->getParentSceneNode()->setPosition(Ogre::Vector3(
						5.0f * Ogre::Math::Sin(::GetTickCount() / 1000.0f), 
						0, 
						5.0f * Ogre::Math::Cos(::GetTickCount() / 1000.0f)
						)
						);

		g_TestBoardChain->getParentSceneNode()->setPosition(
						g_TestBoardChain->getParentSceneNode()->getPosition() + 
						Ogre::Vector3(
						- 2.0f * Ogre::Math::Sin(::GetTickCount() / 1000.0f), 
						0, 
						4.0f * Ogre::Math::Cos(::GetTickCount() / 1000.0f)
						)
						);
	}
	
	if(gIHelperObj && g_ParabolaTrackCtrller/*g_TestReactor*//* && SCENE_MGR->GetCurObjSection() && 
		SCENE_MGR->GetCurObjSection()->getMovableType() == "OTEEntity"*/)
	{
		static DWORD sTimeCnt = ::GetTickCount();
		//g_TestReactor->Update(gIHelperObj, (::GetTickCount() - sTimeCnt) / 1000.0f);
		g_ParabolaTrackCtrller->Update(gIHelperObj, (::GetTickCount() - sTimeCnt) / 1000.0f);
		sTimeCnt = ::GetTickCount();
	}	
}


// ========================================================
// Plugin 接口处理
// ========================================================	
CCommonEditorPluginInterface gPI;
// --------------------------------------------------------
extern "C" void __declspec(dllexport) dllStartPlugin( void )
{		
	COTEPluginManager::GetInstance()->AddPluginInterface(&gPI);		
			
	// 特效渲染回调函数
	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnSceneRender);

	//SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnActorLogicRender);

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnReactorTestRender);

	SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, OnClearRender);	

	// open grid first				 
	//gGridDummy.DrawGridDummy();	
	//gGridDummy.SetVisible(false);

	//要初始化创建东西测试马？
	// ################# 开始 ######################### //		

	//g_TestBoardChain = (OTE::COTEBillBoardChain*)MAGIC_MGR->AutoCreateMagic("XXX.BBC");
	//SCENE_MGR->AttachObjToSceneNode(g_TestBoardChain);
	

	// ################# 结束 ######################### //			
}

// --------------------------------------------------------
extern "C" void __declspec(dllexport) dllStopPlugin( void )
{	
	// 回调函数注册
	SCENE_MGR -> RemoveListener(OnSceneRender);
	SCENE_MGR -> RemoveListener(OnClearRender);
	SCENE_MGR -> RemoveListener(OnActorLogicRender);
	SCENE_MGR -> RemoveListener(OnReactorTestRender);		

	// 清理
	gGridDummy.Destroy();	

	// the plugin
	COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);


}