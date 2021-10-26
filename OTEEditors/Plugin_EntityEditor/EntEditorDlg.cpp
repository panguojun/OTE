// MeshEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
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

using namespace OTE;
using namespace Ogre;
using namespace std;

// 鼠标操作的标示

#define		MOUSE_FLAG_EMPTY		0x00000000
#define		MOUSE_FLAG_TRANS		0x0F000000
#define		MOUSE_FLAG_COPY			0xF0000000
#define		MOUSE_FLAG_FREEMOV		0x00F00000
#define		MOUSE_FLAG_GROUPSEL		0x000F0000
#define		MOUSE_FLAG_SNAPSTART	0x0000F000
#define		MOUSE_FLAG_SNAPEND		0x00000F00

// ========================================================
// CMeshEditorDlg 对话框
// ========================================================

CMeshEditorDlg				CMeshEditorDlg::s_Inst;
CTransformManager			CMeshEditorDlg::s_TransMgr;
CString						CMeshEditorDlg::s_EditState = "选择";

std::list<Ogre::MovableObject*>		CMeshEditorDlg::s_pSelGroup;
std::list<EntityEditData_t>			CMeshEditorDlg::s_pCopyGroup;

DWORD						g_MouseFlag = MOUSE_FLAG_EMPTY;	// 鼠标状态

// 群选效果

OTE::CRectA<float>			g_Rect;						
C3DPathView*				g_pPathView = NULL;

// --------------------------------------------------------
IMPLEMENT_DYNAMIC(CMeshEditorDlg, CDialog)
CMeshEditorDlg::CMeshEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMeshEditorDlg::IDD, pParent),
	m_Snap3DMark(NULL)	
{
}

CMeshEditorDlg::~CMeshEditorDlg()
{
	ClearTrackBall();
}

void CMeshEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CMeshEditorDlg, CDialog)
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
END_MESSAGE_MAP()



// --------------------------------------------------------
// Clear scene
void CMeshEditorDlg::ClearScene()
{
	s_pSelGroup.clear();

	SCENE_MGR->RemoveAllEntities();

	CEntEditDlg::sInst.UpdateSceneTree();

	SAFE_DELETE(g_pPathView)
}

void CMeshEditorDlg::OnBnClickedButton1()
{
	ClearScene();	
}

// --------------------------------------------------------
// Load From File
void CMeshEditorDlg::OnBnClickedButton2()
{
	CSaveDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		CHAR curDir[256];
		::GetCurrentDirectory(256, curDir);
		CFileDialog* tpFileDialog=new CFileDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"场景物体 (*.xml)|*.xml|All Files (*.*)|*.*||",
			NULL 
			);
		int nResponse = tpFileDialog->DoModal();
		::SetCurrentDirectory(curDir);

		if (nResponse == IDOK)
		{ 			
			CString saveType = dlg.m_SaveType;
			if(saveType == "IsNpc")
			{
				OTE::COTESceneXmlLoader::GetInstance()->ImportNPCFromFile(LPCTSTR (tpFileDialog->GetPathName()));
			}
			else if(saveType == "IsLight")
			{
				OTE::COTESceneXmlLoader::GetInstance()->ImportPtLgtFromFile(LPCTSTR (tpFileDialog->GetPathName()));
			}
			else if(saveType == "Cam")
			{
				// 清理

				std::vector<Ogre::MovableObject*> eList;
				SCENE_MGR->GetEntityList(&eList);
				for(unsigned int i = 0; i < eList.size(); ++ i)
				{			
					if(eList[i]->getName().find("Cam_") == std::string::npos)
					{
						continue;
					}
					SCENE_MGR->RemoveEntity(eList[i]->getName());
				}

				OTE::COTESceneXmlLoader::GetInstance()->ImportCamPathFromFile(LPCTSTR (tpFileDialog->GetPathName()));
			
			}
			else
			{
				ClearScene();
				OTE::COTESceneXmlLoader::GetInstance()->ImportSceneFromFile(
					LPCTSTR (tpFileDialog->GetPathName()), 
					"default"
					);
			}

			CEntEditDlg::sInst.UpdateSceneTree();

			ClearTrackBall();
		}
		delete tpFileDialog; 	


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

	}

}
// --------------------------------------------------------
// Save To File
void CMeshEditorDlg::OnBnClickedButton3()
{	
	//先清除鼠标实体
	CEntCrtDlg::sInst.ClearMouseEntity();

	CSaveDlg dlg;

	if(dlg.DoModal() == IDOK)
	{
		CHAR curDir[256];
		::GetCurrentDirectory(256, curDir);
		CFileDialog* tpFileDialog=new CFileDialog(FALSE, // TRUE for FileOpen, FALSE for FileSaveAs
			 NULL,
			 NULL,
			 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"场景物体 (*.xml)|*.xml|All Files (*.*)|*.*||",
			 NULL 
			);
		int nResponse = tpFileDialog->DoModal();
		::SetCurrentDirectory(curDir);

		if (nResponse == IDOK)
		{ 			
			std::string file = tpFileDialog->GetPathName();
			if(file.find(".xml") == -1)
				file = file + ".xml";
			
			CString saveType = dlg.m_SaveType;

			// 输出
			if(!saveType.IsEmpty())
			{
				if(saveType == "IsNpc")
				{
					OTE::COTESceneXmlLoader::GetInstance()->ExportNPCToFile(file.c_str());
				}
				else if(saveType == "IsLight")
				{
					OTE::COTESceneXmlLoader::GetInstance()->ExportPtLgtToFile(file.c_str());
				}
				else if(saveType == "Cam")
				{
					OTE::COTESceneXmlLoader::GetInstance()->ExportCamPathToFile(file.c_str());
				}
				else
					OTE::COTESceneXmlLoader::GetInstance()->ExportEntityInfo(file.c_str(), (LPCTSTR)saveType);
			}
		}
		delete tpFileDialog; 

		Ogre::Vector3 camPos = SCENE_CAM->getPosition();

		std::stringstream ss;
		ss << camPos.x << " " << camPos.y << " " << camPos.z;
		WritePrivateProfileString("CommonEditor", "CameraPosition", ss.str().c_str(), ".\\Editor.ini"); 


	}

}
// --------------------------------------------------------
void CMeshEditorDlg::OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult)
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
BOOL CMeshEditorDlg::OnInitDialog()
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

	((CButton*)GetDlgItem(IDC_BUTTON1))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP1)));  
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


void CMeshEditorDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}

void CMeshEditorDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{

}

// ----------------------------------------------------------------
// 移动
void CMeshEditorDlg::OnBnClickedButton4()
{
	if(!s_pSelGroup.empty())
	{
		CMeshEditorDlg::s_EditState = "变换";
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(true);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(false);
		s_TransMgr.ShowDummy(CTransformManager::ePOSITION);
		s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getWorldPosition());
	}
}

// ----------------------------------------------------------------
// 旋转
void CMeshEditorDlg::OnBnClickedButton5()
{
	if(!s_pSelGroup.empty())
	{
		CMeshEditorDlg::s_EditState = "变换";
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(true);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(false);
		s_TransMgr.ShowDummy(CTransformManager::eROTATION);
		s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getPosition());
	}
}

// ----------------------------------------------------------------
// 缩放
void CMeshEditorDlg::OnBnClickedButton6()
{
	if(!s_pSelGroup.empty())
	{
		CMeshEditorDlg::s_EditState = "变换";
		((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(false);
		((CButton*)GetDlgItem(IDC_BUTTON6))->SetState(true);
		s_TransMgr.ShowDummy(CTransformManager::eSCALING);
		s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getPosition());
	}
}

// ----------------------------------------------------------------
void CMeshEditorDlg::SelectSceneEntity(Ogre::MovableObject* e, bool autoSel)
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

	
	if(!s_pSelGroup.empty() && CEntPorpDlg::sInst.IsWindowVisible())
	{	
		CEntPorpDlg::sInst.UpdateEntityPropertyList((*s_pSelGroup.begin()));
	}

	
}
// ----------------------------------------------------------------
void CMeshEditorDlg::CancelSelection()
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
void CMeshEditorDlg::OnLButtonUp(float x, float y)
{
	if(CEntCrtDlg::sInst.m_MouseObj)
		return;

	// 群选

	if( !(::GetKeyState( VK_SHIFT) & 0x80) &&
		!(g_MouseFlag & MOUSE_FLAG_FREEMOV) &&
		!(g_MouseFlag & MOUSE_FLAG_TRANS)
		)
	{
		g_Rect.ex = x; g_Rect.ez = y;

		if(Ogre::Math::Abs(g_Rect.getSize<float>()) > 0.1f)
		{
			int oldET = s_TransMgr.GetEditType();

			RectSelEntities(g_Rect);
					
			if(!s_pSelGroup.empty())
			{
				//CMeshEditorDlg::s_EditState = "变换";
				s_TransMgr.ShowDummy(oldET);		
				s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getWorldPosition());
			}			
		}		
	}

	if(g_pPathView && !g_pPathView->m_NodeList.empty())
	{
		g_pPathView->ClearNodes();
		g_pPathView->Update();
	}


	if(!s_pSelGroup.empty())
	{	
		CEntPorpDlg::sInst.UpdateEntityPropertyList((*s_pSelGroup.begin()));
	}

	g_MouseFlag = MOUSE_FLAG_EMPTY;
}

// ---------------------------------------------------------------
// 左键双击
void CMeshEditorDlg::OnLButtonDClick(float x, float y)
{	
	this->CancelSelection();
	
	SelectEntityByMouse(x, y);

	if(s_pSelGroup.size() > 0)
	{		
		s_TransMgr.ShowDummyA();	
	}
	
}


// ---------------------------------------------------------------
// 移动 / 旋转 / 缩放
void CMeshEditorDlg::OnSceneMouseMove(const Ogre::Vector2& rScreenMove, float x, float y)
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
	

	// 用于特效实体的编辑

	if(::GetKeyState(VK_LBUTTON) & 0x80)
	{
		// 移动
		if(s_TransMgr.GetEditType() == CTransformManager::ePOSITION)
		{
			// 如果选中物件则自由移动
			if(g_MouseFlag & MOUSE_FLAG_FREEMOV && rScreenMove.length() > 5.0f)
			{
				if(SCENE_CAM->getProjectionType() != Ogre::PT_ORTHOGRAPHIC)
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
						(*it)->getParentNode()->setPosition(
							(*it)->getParentNode()->getPosition() + p - pos);					

						++ it;
					}

					s_TransMgr.SetDummyPos(p);
				}
				else
				{
					// 前视图
					
					Ogre::Vector3 dMove;
					if((DWORD)SCENE_CAM->getUserObject() & 0xF0000000)
					{
						dMove =	Ogre::Vector3(rScreenMove.x, - rScreenMove.y, 0);								
					}

					// 俯视图

					else if((DWORD)SCENE_CAM->getUserObject() & 0x0F000000)
					{
						dMove =	Ogre::Vector3(rScreenMove.x, 0, rScreenMove.y);							
					}

					// 右视图

					else if((DWORD)SCENE_CAM->getUserObject() & 0x00F00000)
					{
						dMove =	Ogre::Vector3(0, - rScreenMove.y, - rScreenMove.x);					
					}	

					dMove /= 300.0f;				

					std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
					while(it != s_pSelGroup.end())
					{						
						(*it)->getParentSceneNode()->setPosition(
							(*it)->getParentSceneNode()->getPosition() + dMove);					

						++ it;
					}

					s_TransMgr.SetDummyPos(s_TransMgr.GetDummyPos() + dMove);
				}

			}
			else
			{
				Ogre::Vector3 pos = (*s_pSelGroup.begin())->getParentNode()->getPosition();
				Ogre::Vector3 newPos;
				s_TransMgr.GetViewportMove(pos, pos, rScreenMove, newPos);
				//s_pCurSel->getParentNode()->setPosition(newPos);
				s_TransMgr.SetDummyPos(newPos);

				// 组
		
				std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
				while(it != s_pSelGroup.end())
				{	
					(*it)->getParentNode()->setPosition(
						(*it)->getParentNode()->getPosition() + newPos - pos);				

					++ it;
				}
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
void CMeshEditorDlg::OnBnClickedButton7()
{
	CMeshEditorDlg::s_EditState = "选择";
}

// ---------------------------------------------------------------
// 移除选择	
void CMeshEditorDlg::RemoveSelEntities()
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
			//TrackBallTrack(new DeleteTrack_t(*it));			
			SCENE_MGR->RemoveEntity((*it)->getName());
		}
	
	/*	else if((*it)->getName().find("IsNpc_") != std::string::npos)
		{
			TrackBallTrack(new DeleteTrack_t(*it));
			SCENE_MGR->RemoveEntity((*it)->getName(), false);		
		}*/

		++ it;
	}
	s_pSelGroup.clear();

	s_TransMgr.HideAllDummies();

	CEntEditDlg::sInst.UpdateSceneTree();

}

// ----------------------------------------------------------------
// 群选
void CMeshEditorDlg::RectSelEntities(const OTE::CRectA<float>& rect)
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

void CMeshEditorDlg::OnLButtonDown(float x, float y)
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
	else
	{			
		// 磁铁对齐(SHIFT键按下点两下）

		if(::GetKeyState( VK_SHIFT) & 0x80 && s_pSelGroup.size() > 0)
		{
			s_TransMgr.ShowDummyA(false); // 隐藏坐标系

			Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay(x, y );	
			
			std::pair<Ogre::MovableObject*, Ogre::Vector3> ret = OTE::COTEObjectIntersection::TriangleIntersection(tCameraRay, "", "", LONG_MAX, true);
			Ogre::Vector3 hitPos = ret.second;

			if(CMeshEditorDlg::s_EditState !=  "Snap_started") // 对齐开始
			{
				if(!m_Snap3DMark)
				{
					m_Snap3DMark = new CLineObj("SnapHelper"); 	
		
					SceneNode* node = SCENE_MGR->getRootSceneNode()->createChildSceneNode("SnapHelper", Vector3( 0, 0, 0 ));
					node->attachObject(m_Snap3DMark); 	
				}

				// 绘制	

				m_SnapStartPos = hitPos;

				m_Snap3DMark->addPoint(Ogre::Vector3(m_SnapStartPos.x, m_SnapStartPos.y + 2.0f, m_SnapStartPos.z));
				m_Snap3DMark->addPoint(m_SnapStartPos);
				
				m_Snap3DMark->changeColor(ColourValue(.8,.0,.8));
				m_Snap3DMark->drawLines(); 			
				
				CMeshEditorDlg::s_EditState =  "Snap_started";
				g_MouseFlag |= MOUSE_FLAG_SNAPSTART; 
			}
			else // 对齐完成 
			{
				m_SnapEndPos = hitPos;

				if(m_Snap3DMark)
				{
					m_Snap3DMark->updatePoint(0, m_SnapEndPos);
					
					m_Snap3DMark->changeColor(ColourValue(0.8f, 0.0f, 0.8f));
					m_Snap3DMark->drawLines();
				}
				g_MouseFlag |= MOUSE_FLAG_SNAPEND; 
			}	

		}
		else
		{
			/// 选择

			if(s_pSelGroup.size() < 2 || ::GetKeyState(VK_CONTROL) & 0x80)
			{
				int oldET = s_TransMgr.GetEditType();

				bool isSelected = SelectEntityByMouse(x, y);	

				if(!s_pSelGroup.empty() && isSelected)
				{				
					s_TransMgr.ShowDummy(oldET);		
					s_TransMgr.SetDummyPos((*s_pSelGroup.begin())->getParentNode()->getWorldPosition());
					
					g_MouseFlag |= MOUSE_FLAG_FREEMOV;
				}				
			}	

			// 框选

			//if(s_pSelGroup.empty())
			if(g_MouseFlag == 0)
			{
				g_Rect.sx = x;g_Rect.sz = y;

				// 线显示

				if(!g_pPathView)
					g_pPathView = new C3DPathView("Dummy.RectSelection");

				g_pPathView->ClearNodes();

				g_MouseFlag |= MOUSE_FLAG_GROUPSEL;

			}
		}

	}

	// 准备后悔

	if(g_MouseFlag & MOUSE_FLAG_SNAPEND ||
	   g_MouseFlag & MOUSE_FLAG_TRANS   ||
	   g_MouseFlag & MOUSE_FLAG_FREEMOV	   
	   )
	{
		ClearTrackBall();

		std::list<Ogre::MovableObject*>::iterator it = s_pSelGroup.begin();
		while(it != s_pSelGroup.end())
		{		
			TrackBallTrack(new EditTrack_t(*it));				

			++ it;
		}
	}
	
}


// ---------------------------------------------------------------
// 选择
bool CMeshEditorDlg::SelectEntityByMouse(float x, float y)
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
			SelectSceneEntity(m);	
		}
		else
		{
			// 点选特效

			std::list<Ogre::MovableObject*> ml;
			COTECollisionManager::PickAABB(tCameraRay, ml);
			std::list<Ogre::MovableObject*>::iterator it = ml.begin();
			
			while(it != ml.end())
			{
				std::string en = (*it)->getName();
				if(en.find("Mag_") != std::string::npos)
				{
					std::list<Ogre::MovableObject*>::iterator it2 = ml.begin();
					bool isSmall = false;
					while(it2 != ml.end())
					{
						if((*it)->getBoundingRadius() > (*it2)->getBoundingRadius())
						{
							isSmall = false;
							break;
						}
						else
							isSmall = true;
						++it2;
					}

					if(isSmall)
					{
						SelectSceneEntity(*it);	
						bFind = true;
						break;
					}
				}
				++it;	
			}
		}

		if(!bFind)
			SelectSceneEntity(NULL);					
	}

	return bFind;
}


// ---------------------------------------------------------------
void CMeshEditorDlg::OnSceneKeyDown()
{
	if(::GetKeyState( VK_ESCAPE ) & 0x80)
	{
		CancelSelection();
		CMeshEditorDlg::s_EditState = "选择";
	}
	else if(::GetKeyState( VK_DELETE ) & 0x80)
	{		
		ClearTrackBall();
		RemoveSelEntities();
	}

	// 把摄像机放到物件上

	if(::GetKeyState('K') & 0x80)
	{
		static bool sIsObjView = false;
		static Ogre::Vector3 sOldPos;
		static Ogre::Vector3 sOldDir;
		if(!s_pSelGroup.empty())
		{
			if(!sIsObjView)
			{
				sOldPos = SCENE_CAM->getPosition();
				sOldDir = SCENE_CAM->getDirection();
				sIsObjView = true;
				SCENE_CAM->setPosition((*s_pSelGroup.begin())->getParentSceneNode()->getPosition());
				SCENE_CAM->setDirection(-(*s_pSelGroup.begin())->getParentSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z);
			}
			else
			{
				sIsObjView = false;
				SCENE_CAM->setPosition(sOldPos);
				SCENE_CAM->setDirection(sOldDir);
			}
		}
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
			if(m)
			{
				WAIT_LOADING_RES(m);
				SelectSceneEntity( m );
				TrackBallTrack(new CreateTrack_t(m)); 
			}

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
			CMeshEditorDlg::s_EditState = "变换";
			s_TransMgr.ShowDummy( CTransformManager::ePOSITION );
			s_TransMgr.SetDummyPos( ( *s_pSelGroup.begin() )->getParentNode()->getPosition() );
		}
	}

	if( ::GetKeyState('W') & 0x80 )
	{
		if( !s_pSelGroup.empty() )
		{
			CMeshEditorDlg::s_EditState = "变换";
			s_TransMgr.ShowDummy( CTransformManager::eROTATION );
			s_TransMgr.SetDummyPos( ( *s_pSelGroup.begin() )->getParentNode()->getPosition() );
		}
	}

	if( ::GetKeyState('E') & 0x80 )
	{
		if( !s_pSelGroup.empty() )
		{
			CMeshEditorDlg::s_EditState = "变换";
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

			CMeshEditorDlg::s_EditState	= "选择";
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

		if(CMeshEditorDlg::s_EditState ==  "Snap_started")
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
			CMeshEditorDlg::s_EditState =  "选择";
			
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
