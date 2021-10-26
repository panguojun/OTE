// EntPorpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_EntEditor.h"
#include "EntPorpDlg.h"
#include "ListViewEditor.h"
#include "OTEQTSceneManager.h"
#include "EntEditorDlg.h"
#include "OTEMathLib.h"
#include "OTEActorActionMgr.h"
#include "OTEEntityLightmapPainter.h"

// CEntPorpDlg �Ի���

IMPLEMENT_DYNAMIC(CEntPorpDlg, CDialog)

CEntPorpDlg CEntPorpDlg::sInst;

CEntPorpDlg::CEntPorpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEntPorpDlg::IDD, pParent)
{
}

CEntPorpDlg::~CEntPorpDlg()
{
}

void CEntPorpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_EntPorpList);
	DDX_Control(pDX, IDC_ActList, mActList);
}


BEGIN_MESSAGE_MAP(CEntPorpDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_LBN_DBLCLK(IDC_ActList, OnLbnDblclkActlist)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
END_MESSAGE_MAP()

// --------------------------------------------------------
BOOL CEntPorpDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	m_EntPorpList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_MULTIWORKAREAS);
	m_EntPorpList.InsertColumn(0, "����", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 70); 
	m_EntPorpList.InsertColumn(1, "ֵ", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 130); 

	return tRet;
}
	
// --------------------------------------------------------
int g_LgtMapItem = -1;
// --------------------------------------------------------
#define DO_EDITOK(type)  CListViewEditor dlg(type, m_EntPorpList.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && (!CListViewEditor::s_Text.IsEmpty() && type != 1 || type == 1) )

void CEntPorpDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!SCENE_MGR->IsEntityPresent(m_CurEntName.GetString()))
		return;

	int tItem, tSubItem;
	CRect tSubItemRect;
	
	if(!m_CurEntName.IsEmpty() && m_EntPorpList.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{
		COTEEntity* e = SCENE_MGR->GetEntity((LPCTSTR)m_CurEntName);
		if(!e)		
			return;
		
		switch(tItem)
		{
		case 0:
			{
				// ����
			/*	DO_EDITOK(1)
				{
					not valid
				}*/
			}break;
		case 1:
			{		
				// X����
				DO_EDITOK(0)
				{
					Ogre::Vector3 oldPos = e->getParentSceneNode()->getPosition();					
					e->getParentSceneNode()->setPosition(atof(CListViewEditor::s_Text), oldPos.y, oldPos.z);
					UpdateEntityPropertyList(e);
				}
			}break;
		case 2:
			{		
				// Y����
				DO_EDITOK(0)
				{
					Ogre::Vector3 oldPos = e->getParentSceneNode()->getPosition();					
					e->getParentSceneNode()->setPosition(oldPos.x, atof(CListViewEditor::s_Text), oldPos.z);
					UpdateEntityPropertyList(e);
				}
			}break;
	    case 3:
			{		
				// Z����
				DO_EDITOK(0)
				{
					Ogre::Vector3 oldPos = e->getParentSceneNode()->getPosition();					
					e->getParentSceneNode()->setPosition(oldPos.x, oldPos.y, atof(CListViewEditor::s_Text));
					UpdateEntityPropertyList(e);
				}
			}break;
	    case 4:
			{		
				// Y��ת
				DO_EDITOK(0)
				{	
					Ogre::Quaternion q( - Ogre::Radian(Ogre::Degree(atof( CListViewEditor::s_Text) - 90)), Ogre::Vector3::UNIT_Y);
					e->getParentSceneNode()->setOrientation(q);
					UpdateEntityPropertyList(e);
				}
			}break;
		case 5:
			{
				// X����
				DO_EDITOK(0)
				{	
					Ogre::Vector3 oldScl = e->getParentSceneNode()->getScale();					
					e->getParentSceneNode()->setScale(atof(CListViewEditor::s_Text), oldScl.y, oldScl.z);
					UpdateEntityPropertyList(e);
				}
			}break;
		case 6:
			{
				// Y����
				DO_EDITOK(0)
				{	
					Ogre::Vector3 oldScl = e->getParentSceneNode()->getScale();					
					e->getParentSceneNode()->setScale(oldScl.x, atof(CListViewEditor::s_Text), oldScl.z);
					UpdateEntityPropertyList(e);
				}
			}break;			
		case 7:
			{
				// Z����
				DO_EDITOK(0)
				{	
					Ogre::Vector3 oldScl = e->getParentSceneNode()->getScale();					
					e->getParentSceneNode()->setScale(oldScl.x, oldScl.y, atof(CListViewEditor::s_Text));
					UpdateEntityPropertyList(e);
				}
			}break;
		case 8:
			{
				if(e->getName().find("Cam_") != std::string::npos)
				{
					// �ٶ�
					DO_EDITOK(0)
					{	
						//float oldSpeed = ((COTEActorEntity*)e)->m_DymProperty.GetfloatVal("speed");
						((COTEActorEntity*)e)->m_DymProperty.SetVal("speed", (float)atof(CListViewEditor::s_Text));
						UpdateEntityPropertyList(e);
					}
				}
			}break;
		
		}

		// ����ͼ

		if(g_LgtMapItem == tItem)
		{
			/// �ѹ���ͼ������ָ��Ŀ¼		

			// Դλ��

			std::string lightmapSrc = m_EntPorpList.GetItemText(tItem, tSubItem).GetString();
		
			std::string path, lightmapSrcPath;
			OTEHelper::GetFullPath(lightmapSrc, path, lightmapSrcPath);			
			
			
			// Ŀ��λ��			

			std::string lightmapViewPath = OTEHelper::GetResPath("ResConfig", "EntityLightmapView");
			
			std::string lightmapDesPath;
			OTEHelper::GetFullPath(lightmapViewPath, lightmapDesPath);

			std::stringstream ss;
			ss << lightmapDesPath << "\\" << lightmapSrc;
			::CopyFile(lightmapSrcPath.c_str(), ss.str().c_str(), false);
		}		
	}
}

// --------------------------------------------------------
void CEntPorpDlg::UpdateEntityPropertyList(Ogre::MovableObject* e)
{
	if(!CEntPorpDlg::sInst.IsWindowVisible())
		return;

	g_LgtMapItem = -1;

	if(!e || !m_EntPorpList.m_hWnd)
		return;

	m_CurEntName = e->getName().c_str(); // ��������

	m_EntPorpList.DeleteAllItems();
	m_EntPorpList.InsertItem(0, "����");
	m_EntPorpList.SetItemText(0, 1, e->getName().c_str());

	std::stringstream ss;
	m_EntPorpList.InsertItem(1, "X");	
	ss << e->getParentNode()->getPosition().x;	
	m_EntPorpList.SetItemText(1, 1, ss.str().c_str());

	ss.str("");
	ss << e->getParentNode()->getPosition().y;	
	m_EntPorpList.InsertItem(2, "Y");
	m_EntPorpList.SetItemText(2, 1, ss.str().c_str());

	ss.str("");
	ss << e->getParentNode()->getPosition().z;	
	m_EntPorpList.InsertItem(3, "Z");
	m_EntPorpList.SetItemText(3, 1, ss.str().c_str());

	ss.str("");
	float degree = OTE::MathLib::FixRadian( OTE::MathLib::GetRadian(e->getParentSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z ) ).valueDegrees();
	ss << degree;	
	m_EntPorpList.InsertItem(4, "Y����ת�Ƕ�");
	m_EntPorpList.SetItemText(4, 1, ss.str().c_str());

	ss.str("");
	ss << e->getParentNode()->getScale().x;	
	m_EntPorpList.InsertItem(5, "SX");		
	m_EntPorpList.SetItemText(5, 1, ss.str().c_str());

	ss.str("");
	ss << e->getParentNode()->getScale().y;	
	m_EntPorpList.InsertItem(6, "SY");
	m_EntPorpList.SetItemText(6, 1, ss.str().c_str());

	ss.str("");
	ss << e->getParentNode()->getScale().z;	
	m_EntPorpList.InsertItem(7, "SZ");
	m_EntPorpList.SetItemText(7, 1, ss.str().c_str());

	int itemCnt = 8;

	// �������ͼ

	std::string lmName = OTE::COTEEntityLightmapPainter::GetLightMapName((COTEEntity*)e);

	if(CHECK_RES(lmName))
	{
		g_LgtMapItem = itemCnt;

		m_EntPorpList.InsertItem(itemCnt, "����ͼ");
		m_EntPorpList.SetItemText(itemCnt ++, 1, lmName.c_str());
	}
		
	// ��Դ

	if(e->getName().find("Lgt_") != std::string::npos)
	{
		//((COTEActorEntity*)e)->m_DymProperty.get
		
	}

	// �����

	if(e->getName().find("Cam_") != std::string::npos)
	{
		m_EntPorpList.InsertItem(itemCnt, "�ٶ�");
		m_EntPorpList.SetItemText(itemCnt ++, 1, OTEHelper::Str(((COTEActorEntity*)e)->m_DymProperty.GetfloatVal("speed")));		
	}

	// ����ʵ��

	else if(e->getMovableType().find("OTEEntity") != std::string::npos)
	{
		ss.str("");
		ss << ((Entity*)e)->getMesh()->getName();	
		m_EntPorpList.InsertItem(itemCnt, "Mesh");
		m_EntPorpList.SetItemText(itemCnt ++, 1, ss.str().c_str());
	}


	//�����ж��б�
	mActList.ResetContent();

	if(m_CurEntName.Find("Ent_") != -1 && !((OTE::COTEActorEntity*)e)->m_ActionList.empty())
	{
		HashMap<std::string, COTEAction*>::iterator it = ((OTE::COTEActorEntity*)e)->m_ActionList.begin();
		while(it != ((OTE::COTEActorEntity*)e)->m_ActionList.end())
		{
			mActList.AddString(it->second->GetName().c_str());
			it++;
		}
	}

	mActList.Invalidate();
}

//˫�������ж�
void CEntPorpDlg::OnLbnDblclkActlist()
{
	CString sel;
	mActList.GetText(mActList.GetCurSel(), sel);

	if(!CLogicEditorDlg::s_pSelGroup.empty() && 
		(*CLogicEditorDlg::s_pSelGroup.begin())->getName().find("Ent_") != -1)
	{
		COTEActorActionMgr::SetAction(
			((OTE::COTEActorEntity*)(*CLogicEditorDlg::s_pSelGroup.begin())), 
			sel.GetString());
	}
}

//����ж�
void CEntPorpDlg::OnBnClickedButton1()
{
	if(!CLogicEditorDlg::s_pSelGroup.empty() && 
		(*CLogicEditorDlg::s_pSelGroup.begin())->getName().find("Ent_") != -1)
	{
		COTEActorActionMgr::SetAction(
			((OTE::COTEActorEntity*)(*CLogicEditorDlg::s_pSelGroup.begin())),
			"");		
	}
}

void CEntPorpDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
}

void CEntPorpDlg::OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	*pResult = 0;
}

void CEntPorpDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
