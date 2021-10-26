// EntCrtDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_EntEditor.h"
#include "OTEQTSceneManager.h"
#include "EntCrtDlg.h"
#include "EntEditDlg.h"
#include "EntPorpDlg.h"
#include "EntEditorDlg.h"
#include "OTEActorEntity.h"
#include "otecollisionmanager.h"
#include "NPCCrtDlg.h"
#include "PointLightCrtDlg.h"
#include "resource.h"

// --------------------------------------------------------

using namespace Ogre;
using namespace OTE;
// --------------------------------------------------------

// CEntCrtDlg �Ի���
IMPLEMENT_DYNAMIC(CEntCrtDlg, CDialog)

CEntCrtDlg CEntCrtDlg::sInst;

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

void CEntCrtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_MeshResTree);
	DDX_Control(pDX, IDC_COMBO1, m_CrtType);
}


BEGIN_MESSAGE_MAP(CEntCrtDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// --------------------------------------------------------
BOOL CEntCrtDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();	

	m_MeshResTree.ShowWindow(SW_SHOW);	
	
	RootItem = m_MeshResTree.InsertItem("��Դ�б�", NULL);
	UpdataRes();

	m_MeshResTree.Expand(m_MeshResTree.GetRootItem(), TVE_EXPAND);

	return tRet;
}

// ----------------------------------------------------------------
//���ĳһ��֦
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
//������Դ�б�
void CEntCrtDlg::UpdataRes()
{
	Clear(RootItem);

	char resPath[256];
	GetPrivateProfileString("ResConfig","ResPathSceneModel","\0",
									resPath,sizeof(resPath),".\\OTESettings.cfg");

	AllDirScanfFiles(resPath, RootItem);

	GetPrivateProfileString("ResConfig","ResPathActorModel","\0",
									resPath,sizeof(resPath),".\\OTESettings.cfg");

	AllDirScanfFiles(resPath, RootItem);

	GetPrivateProfileString("ResConfig","ResPathEffectModel","\0",
									resPath,sizeof(resPath),".\\OTESettings.cfg");

	AllDirScanfFiles(resPath, RootItem);

	GetPrivateProfileString("ResConfig","ResPathParticles","\0",
									resPath,sizeof(resPath),".\\OTESettings.cfg");

	AllDirScanfFiles(resPath, RootItem);	

}

// ----------------------------------------------------------------
//������ԴĿ¼
void CEntCrtDlg::AllDirScanfFiles(std::string strPathName, HTREEITEM& item)
{
	// �����ļ���
	CFileFind finder;
	BOOL bWorking = finder.FindFile((strPathName + "\\*.*").c_str());
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDirectory())
		{
			if(finder.IsDots() || finder.IsHidden() || finder.GetFileName().Find("Template") != -1)
				continue;
			HTREEITEM t = m_MeshResTree.InsertItem(finder.GetFileName(), item);
			AllDirScanfFiles(strPathName + "\\" + finder.GetFileName().GetString(), t);
		}
		else
		{
			if((finder.GetFileName().Find(".obj") != -1 && finder.GetFileName().Find(".xml") == -1 ) || 		
				finder.GetFileName().Find(".mesh") != -1 ||
				finder.GetFileName().Find(".particle.xml") != -1)
			{
               m_MeshResTree.InsertItem(finder.GetFileName(), item);
			}
		}
	}
	finder.Close();
}

// ----------------------------------------------------------------
// ��ӵ�����
void CEntCrtDlg::OnBnClickedButton1()
{
	//�����MouseEntity();
	ClearMouseEntity();

	std::string selItemText = m_MeshResTree.GetItemText(m_MeshResTree.GetSelectedItem());
		
	if(selItemText.find("particle.xml") != -1)
	{
		//������Ч
		m_MouseObj = MAGIC_MGR->AutoCreateMagic(selItemText.c_str()); // ������̬Ч��
		SCENE_MGR->AttachObjToSceneNode(m_MouseObj);
	}
	else if(selItemText.find(".obj") != -1 || selItemText.find(".mesh") != -1)
	{
		//����ʵ��
		m_MouseObj = SCENE_MGR->CreateEntity(selItemText.c_str(), selItemText.c_str());
	}
	
}

// --------------------------------------------------------
bool CEntCrtDlg::OnSceneLBDown(float x, float y) 
{
	bool retVal = false;
    Ogre::Ray r = SCENE_CAM->getCameraToViewportRay( x, y );
	Ogre::Vector3 p(0, 0, 0);
	
	if(m_MouseObj && 
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

		//if(!TestCollide(s))
		{			
			Ogre::MovableObject* mo = NULL;

			CString str;
			m_CrtType.GetWindowText(str);
			if(str == "Auto" || str == "")
			{
				if(m_MouseObj->getMovableType().find("OTEEntity") != std::string::npos)
				{
					//����ʵ��					

					mo = SCENE_MGR->AutoCreateEntity(m_MouseObj->getName(), "Ent"/*, true*/);
				
					WAIT_LOADING_RES(mo)
					TrackBallTrack(new CreateTrack_t(mo));	
					retVal = true;
				}
				else if(m_MouseObj->getMovableType().find("Magic") != std::string::npos)
				{
					// ������Ч

					//mo = COTEMagicEntity::CreateMagicEntity(m_MouseObj->getName());	
				}
				else
				{
					// �������󣨷�ʵ�壩

					mo = MAGIC_MGR->AutoCreateMagic(m_MouseObj->getName());		
									
				}

				RandomSizeDir();
			}
			else if(str == "IsNpc") 
			{
				if(!CNPCCrtDlg::s_Inst.m_hWnd)
				{
					CNPCCrtDlg::s_Inst.Create(IDD_NPC_CRT_DLG);
					CNPCCrtDlg::s_Inst.ShowWindow(SW_SHOW);
					return false;
				}
				if(!CNPCCrtDlg::s_Inst.IsWindowVisible())
				{
					CNPCCrtDlg::s_Inst.ShowWindow(SW_SHOW);
					return false;
				}

				// �����Σ��
				char idTxt[32];
				CNPCCrtDlg::s_Inst.m_NpcIDEdit.GetWindowText(idTxt, 32);
				if(strcmp(idTxt, "") == 0)
					return false;

				char nmTxt[32];
				CNPCCrtDlg::s_Inst.m_NpcNameEdit.GetWindowText(nmTxt, 32);
				if(strcmp(nmTxt, "") == 0)
					return false;

				std::stringstream ss;
				ss << "IsNpc_" << idTxt << "_" << nmTxt;
				mo = SCENE_MGR->CreateEntity("��ͷ��ŮNPC.obj", ss.str().c_str());
				
			}	
			else if(str == "IsLight") // ��ӵ��Դ
			{
				mo = SCENE_MGR->AutoCreateEntity("lighthelper.mesh", "Lgt");

				if(!CPointLightCrtDlg::s_Inst.m_hWnd)
					CPointLightCrtDlg::s_Inst.Create(IDD_CREATELIGHT);
				CPointLightCrtDlg::s_Inst.ShowWindow(SW_SHOW);

				CPointLightCrtDlg::s_Inst.m_LightName = mo->getName();
				CPointLightCrtDlg::s_Inst.UpdateChange();		
			}
			else if(str == "Cam") // ��������
			{
				mo = SCENE_MGR->AutoCreateEntity("��Ӱ��.mesh", "Cam");
			}

			if(mo != NULL)
			{
				CMeshEditorDlg::s_Inst.SelectSceneEntity(mo);
				(*CMeshEditorDlg::s_pSelGroup.begin())->getParentNode()->setPosition(p);	
				(*CMeshEditorDlg::s_pSelGroup.begin())->getParentNode()->setScale(scale, scale, scale);
				(*CMeshEditorDlg::s_pSelGroup.begin())->getParentNode()->setOrientation(direction);
				CEntEditDlg::sInst.UpdateSceneTree();
			}
		}		
	}

	return retVal;
}

// --------------------------------------------------------
//ʹ�������С�ͷ���
void CEntCrtDlg::RandomSizeDir()
{
	//�����С
	if(m_bUserRSize)
	{
		scale = Ogre::Math::RangeRandom(0.8, 1.2);
	}
	else
	{
		scale = 1.0f;
	}
	
	//�������
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
// ���ʵ�����˫�����ļ���
void CEntCrtDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButton1();
}

//---------------------------------------------------------
//�ж��Ƿ�ᷢ����ײ
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
	CString str;
	this->GetDlgItemText(IDC_COMBO1, str);
	if(str == "IsNpc")
	{
		if(!CNPCCrtDlg::s_Inst.m_hWnd)
			CNPCCrtDlg::s_Inst.Create(IDD_NPC_CRT_DLG);
		CNPCCrtDlg::s_Inst.ShowWindow(SW_SHOW);	
		m_MouseObj = SCENE_MGR->CreateEntity("��ͷ��ŮNPC.obj", "��ͷ��ŮNPC.obj");

	}
	else if(str == "IsLight")
	{
		m_MouseObj = SCENE_MGR->AutoCreateEntity("lighthelper.mesh", "Lgt");

		if(!CPointLightCrtDlg::s_Inst.m_hWnd)
			CPointLightCrtDlg::s_Inst.Create(IDD_CREATELIGHT);
		CPointLightCrtDlg::s_Inst.ShowWindow(SW_SHOW);			
		
	}
	else if(str == "Cam")
	{
		m_MouseObj = SCENE_MGR->AutoCreateEntity("��Ӱ��.mesh", "Cam");
	}
}


void CEntCrtDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
