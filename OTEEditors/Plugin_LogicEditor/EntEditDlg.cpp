// EntEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_EntEditor.h"
#include "EntEditDlg.h"
#include "OgreEntity.h"
#include "OTEQTSceneManager.h"
#include "EntPorpDlg.h"
#include "EntEditorDlg.h"
#include "OTEActorEntity.h"
#include "OTEMagicManager.h"

// --------------------------------------------------------
IMPLEMENT_DYNAMIC(CEntEditDlg, CDialog)

CEntEditDlg	CEntEditDlg::sInst;
CEntEditDlg::CEntEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEntEditDlg::IDD, pParent)
{
}

CEntEditDlg::~CEntEditDlg()
{
}

void CEntEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_SceneTreeCtrl);
}


BEGIN_MESSAGE_MAP(CEntEditDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()

// --------------------------------------------------------
void CEntEditDlg::UpdateSceneTree()
{	
	if(!this->IsWindowVisible())
	{
		return;
	}
	m_SceneTreeCtrl.DeleteAllItems();

	std::vector<Ogre::MovableObject*> elist;
	SCENE_MGR->GetEntityList(&elist);
	MAGIC_MGR->GetMagicList(&elist);

	m_SceneTreeCtrl.ShowWindow(SW_SHOW);	

	for(int i = 0; i < elist.size(); i ++)
	{
		if(elist[i] == NULL || (elist[i]->getName().find("Ent_") == -1 &&
			elist[i]->getName().find("Mag_") == -1)) continue;

		m_SceneTreeCtrl.InsertItem(elist[i]->getName().c_str(), NULL);
	}
}

// --------------------------------------------------------
// 选择
void CEntEditDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CString txt = m_SceneTreeCtrl.GetItemText(m_SceneTreeCtrl.GetSelectedItem());
	if(!txt.IsEmpty())
	{
		CLogicEditorDlg::s_Inst.CancelSelection();
		
//		if(txt.Find("Ent_") != -1)
			CLogicEditorDlg::s_Inst.SelectSceneEntity(SCENE_MGR->SelectEntity((LPCTSTR)txt, true));
		//else if((txt.Find("Mag_") != -1))
		//	CLogicEditorDlg::s_Inst.SelectSceneEntity(MAGIC_MGR->GetMagic(txt.GetString()));
		
		if(!CLogicEditorDlg::s_pSelGroup.empty())
		{
			(*CLogicEditorDlg::s_pSelGroup.begin())->getParentSceneNode()->showBoundingBox(true);
			CEntPorpDlg::sInst.UpdateEntityPropertyList((*CLogicEditorDlg::s_pSelGroup.begin()));
			CLogicEditorDlg::s_TransMgr.SetDummyPos((*CLogicEditorDlg::s_pSelGroup.begin())->getParentNode()->getPosition());
				
			// 摄像机

			OTE::COTEActorEntity* ent = ((OTE::COTEActorEntity*)(*CLogicEditorDlg::s_pSelGroup.begin()));
			Ogre::Vector3 entPos = ent->getParentSceneNode()->getPosition();
			
			SCENE_CAM->setPosition(Ogre::Vector3(entPos.x + 10.0f, entPos.y + 10.0f, entPos.z));

			SCENE_CAM->lookAt(entPos); 
		}	
	}
}
// --------------------------------------------------------
void CEntEditDlg::OnBnClickedButton1()
{
	CLogicEditorDlg::s_Inst.RemoveSelEntities();
	
	UpdateSceneTree();
}

void CEntEditDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	//if(bShow)
	//	UpdateSceneTree();
}

void CEntEditDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{

}
// --------------------------------------------------------
// 更新
void CEntEditDlg::OnBnClickedButton2()
{
	UpdateSceneTree();
}
// --------------------------------------------------------
// 搜索
void CEntEditDlg::OnBnClickedButton3()
{
	CString str;
	this->GetDlgItemText(IDC_SEARCH, str);
	
	m_SceneTreeCtrl.DeleteAllItems();	

	std::vector<Ogre::MovableObject*> elist;
	SCENE_MGR->GetEntityList(&elist);
	MAGIC_MGR->GetMagicList(&elist);

	for(int i = 0; i < elist.size(); i ++)
	{
		if(elist[i] == NULL || 
			(elist[i]->getName().find("Ent_") != -1 || elist[i]->getName().find("Mag_") != -1)) 
			continue;
	
		if(elist[i]->getName().find(str.GetString()) != std::string::npos)
			m_SceneTreeCtrl.InsertItem(elist[i]->getName().c_str(), NULL);		
	}

}
