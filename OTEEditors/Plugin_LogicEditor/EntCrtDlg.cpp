#include "stdafx.h"
#include "Plugin_EntEditor.h"
#include "OTEQTSceneManager.h"
#include "EntCrtDlg.h"
#include "EntEditDlg.h"
#include "EntPorpDlg.h"
#include "EntEditorDlg.h"
#include "OTEActorEntity.h"
#include "otecollisionmanager.h"
#include "resource.h"
#include ".\entcrtdlg.h"

// --------------------------------------------------------
using namespace Ogre;
using namespace OTE;
// --------------------------------------------------------
IMPLEMENT_DYNAMIC(CEntCrtDlg, CDialog)
CEntCrtDlg CEntCrtDlg::sInst;
// --------------------------------------------------------
CEntCrtDlg::CEntCrtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEntCrtDlg::IDD, pParent)
{
	m_MouseObj = NULL;
	m_bUseRDirection = false;
	m_bUserRSize = false;
	m_bCollide = true;
	scale = 1.0f;
	direction = Ogre::Quaternion::IDENTITY;
}
CEntCrtDlg::~CEntCrtDlg()
{
}

// --------------------------------------------------------
void CEntCrtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_MeshResTree);
	DDX_Control(pDX, IDC_COMBO1, m_CrtType);
}
// --------------------------------------------------------
BEGIN_MESSAGE_MAP(CEntCrtDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
END_MESSAGE_MAP()

// --------------------------------------------------------
BOOL CEntCrtDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();	

	m_MeshResTree.ShowWindow(SW_SHOW);	
	
	RootItem = m_MeshResTree.InsertItem("资源列表", NULL);
	UpdataRes();

	m_MeshResTree.Expand(m_MeshResTree.GetRootItem(), TVE_EXPAND);

	return tRet;
}

// ----------------------------------------------------------------
//清除某一分枝
void CEntCrtDlg::Clear(HTREEITEM &t)
{
	if (m_MeshResTree.ItemHasChildren(t))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_MeshResTree.GetChildItem(t);

		while (hChildItem != NULL)
		{
			hNextItem = m_MeshResTree.GetNextItem(hChildItem, TVGN_NEXT);
			m_MeshResTree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
}

// ----------------------------------------------------------------
void CEntCrtDlg::SearchNpcList()
{
	char resPath[256];
	GetPrivateProfileString("ResConfig","ResPathLogicWorks","\0",
									resPath,sizeof(resPath),".\\OTESettings.cfg");	

	m_vNpcList.clear();
	//read in npc name/id table
	char str[32];
	sprintf(str,"%s%s",resPath,"npcidtable");
    FILE *file=fopen(str,"r+");
	if(!file) 
	{
		OTE_MSG("提示","npcidtable文件没有找到！");		
	}
	else
	{
		int num,success=1;
		success|=fscanf(file,"%d\n",&num);
		for(int i=0;i<num;i++)
		{
			int id;
			char name[32];
			success|=fscanf(file,"id: %d name: %s\n",&id,name);
			char str[32];
			sprintf(str,"%d %s\n",id,name);			
			 
			m_vNpcList.push_back(str);
			
			m_MeshResTree.InsertItem(name, m_MeshResTree.GetRootItem());	
		}
		if(!success)
		{
			OTE_MSG("错误","npcidtable文件格式错误！");
			throw;
		}
		fclose(file);
	}	
}

// ----------------------------------------------------------------
//更新资源列表
void CEntCrtDlg::UpdataRes()
{
	Clear(RootItem);

	SearchNpcList();
}


// ----------------------------------------------------------------
// 从文件读入 
void CEntCrtDlg::OnBnClickedButton1()
{
	
}

// --------------------------------------------------------
bool CEntCrtDlg::OnSceneLBDown(float x, float y) 
{
	bool retVal = false;
    Ogre::Ray r = SCENE_CAM->getCameraToViewportRay( x, y );
	Ogre::Vector3 p(0, 0, 0);
	
	if((m_MouseObj || CLogicEditorDlg::s_pSelGroup.size() > 0) && 
		(COTECollisionManager::TriangleIntersection(r, p) ||
		(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(r, p)) ||
		(!COTETilePage::GetCurrentPage() && COTECollisionManager::PickZeroFloor(r, p)) )
		)
	{
		CString str;
		GetDlgItemText(IDC_EDIT1, str);
		Ogre::Real s = 0.0f;
		if(!str.IsEmpty())
		{
			s= Ogre::StringConverter::parseReal(str.GetString());
		}

		m_CrtType.GetWindowText(str);	
		
		if(COTETilePage::GetCurrentPage())
		{
			if(CLogicEditorDlg::s_Inst.m_vLogicSectors.size() == 0)
			{
				OTE_MSG("请先读入区域!", "提示")
				return retVal;
			}
			if(CLogicEditorDlg::s_pSelGroup.size() == 0)
			{
				COTETilePage::GetCurrentPage()->SetVal("CrtNpcName", str.GetString());
				COTETilePage::GetCurrentPage()->SetVal("CrtPos", p);
				COTETilePage::GetCurrentPage()->TriggerEvent("OnCreateNpc");

				if(CLogicEditorDlg::s_pSelGroup.size() > 0)
				{				
					(*CLogicEditorDlg::s_pSelGroup.begin())->getParentNode()->setPosition(p);	
					(*CLogicEditorDlg::s_pSelGroup.begin())->getParentNode()->setScale(scale, scale, scale);
					(*CLogicEditorDlg::s_pSelGroup.begin())->getParentNode()->setOrientation(direction);
					CEntEditDlg::sInst.UpdateSceneTree();
				}
				retVal = true;
			}
			else
			{
				if(::GetKeyState(VK_CONTROL) & 0x80)
				{
					COTETilePage::GetCurrentPage()->SetVal("CrtPos", p);
					COTETilePage::GetCurrentPage()->TriggerEvent("OnCreateWp");
					retVal = true;
				}
			}
				
		}			
	}
	return retVal;
}

// --------------------------------------------------------
//使用随机大小和方向
void CEntCrtDlg::RandomSizeDir()
{
	//随机大小
	if(m_bUserRSize)
	{
		scale = Ogre::Math::RangeRandom(0.8, 1.2);
	}
	else
	{
		scale = 1.0f;
	}
	
	//随机朝向
	if(m_bUseRDirection)
	{
		Ogre::Real r = Ogre::Math::RangeRandom(0.0, 6.2831852);	
		direction = Ogre::Quaternion(Ogre::Radian(r), Ogre::Vector3::UNIT_Y);
	}
	else
	{
		direction = Ogre::Quaternion::IDENTITY;
	}

	if(m_MouseObj)
	{
		m_MouseObj->getParentNode()->setScale(scale, scale, scale);
		m_MouseObj->getParentNode()->setOrientation(direction);		
	}
}

// ----------------------------------------------------------------
// 添加实体或者双击打开文件夹
void CEntCrtDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButton1();
}

//---------------------------------------------------------
//判断是否会发生碰撞
bool CEntCrtDlg::TestCollide(Ogre::Real r)
{
	if(m_MouseObj == NULL)
	{
		return true;
	}
	std::vector<Ogre::MovableObject*> elist;
	SCENE_MGR->GetEntityList(&elist);
	MAGIC_MGR->GetMagicList(&elist);

	for(int i = 0; i < elist.size(); i ++)
	{
		Ogre::MovableObject* mo = elist[i];

		if(mo == NULL || 
			(mo->getName().find("Ent_") == -1 && mo->getName().find("Mag_") == -1))
			continue;
		
		Ogre::Real s = (m_MouseObj->getParentNode()->getWorldPosition()
				- elist[i]->getParentNode()->getWorldPosition()).length();

		if(s < r)
			return true;
	
		Ogre::Matrix4 mmA = Ogre::Matrix4::IDENTITY;
		Ogre::Matrix4 mmB = Ogre::Matrix4::IDENTITY;

		m_MouseObj->getParentNode()->getWorldTransforms(&mmA);
		mo->getParentNode()->getWorldTransforms(&mmB);

		Ogre::AxisAlignedBox aBox = m_MouseObj->getBoundingBox();
		Ogre::AxisAlignedBox bBox = mo->getBoundingBox();
		
		aBox.transform(mmA);
		bBox.transform(mmB);

		if(m_bCollide && aBox.intersects(bBox))
		{
			return true;
		}		
	}
	return false;
}

// --------------------------------------------------------
void CEntCrtDlg::OnSceneMouseMove(float x, float y)
{
	if(m_MouseObj != NULL)
	{
		Ogre::Ray r = SCENE_CAM->getCameraToViewportRay( x, y );
		Ogre::Vector3 p(0, 0, 0);		
		if( COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(r, p) ||
			(!COTETilePage::GetCurrentPage() && COTECollisionManager::PickZeroFloor(r, p)))
		{
			m_MouseObj->getParentNode()->setPosition(p);
			
			if(::GetKeyState(VK_LBUTTON) & 0x80)
			{
				// OnSceneLBDown(x, y);
			}
		}
	}
}

// --------------------------------------------------------
void CEntCrtDlg::ClearMouseEntity()
{
	if(m_MouseObj != NULL)
	{
		if(m_MouseObj->getMovableType().find("OTEEntity") != std::string::npos)
		{
			SCENE_MGR->RemoveEntity(m_MouseObj->getName());
		}
		else
		{
			MAGIC_MGR->DeleteMagic((m_MouseObj->getName()).c_str());
		}
		
		m_MouseObj = NULL;
	}
}

// --------------------------------------------------------
void CEntCrtDlg::OnBnClickedCheck1()
{
	m_bUseRDirection = !m_bUseRDirection;
}

void CEntCrtDlg::OnBnClickedCheck2()
{
	m_bUserRSize = !m_bUserRSize;
}

void CEntCrtDlg::OnBnClickedCheck3()
{
	m_bCollide = !m_bCollide;
}

void CEntCrtDlg::OnEnChangeEdit1()
{
}

// --------------------------------------------------------
void CEntCrtDlg::OnCbnSelchangeCombo1()
{
}
void CEntCrtDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
}

void CEntCrtDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_MouseObj)
	{
		SCENE_MGR->RemoveEntity(m_MouseObj->getName());
		m_MouseObj = 0;
	}

	std::string selItemText = m_MeshResTree.GetItemText(m_MeshResTree.GetSelectedItem());		

	//创建实体
	m_MouseObj = SCENE_MGR->CreateEntity("heighthelper.mesh", selItemText.c_str());
}
