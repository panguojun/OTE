#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "ActionEdit.h"
#include "ActorEditorMainDlg.h"
#include "OTEActionXmlLoader.h"
#include "TrackSelect.h"
#include "KeyFrameDlg.h"
#include "FramePropty.h"
#include "OTETrackCtrller.h"
#include "AddEffEntityDlg.h"
#include "ListViewEditor.h"
#include "SoundDlg.h"
#include "TransformManager.h"
#include "HookEditDlg.h"
#include "OTEActorActionMgr.h"
#include "OTETrackXmlLoader.h"
#include "ActionListDlg.h"
#include "OTEReactorManager.h"
#include "OTECollisionManager.h"
#include "ReactorLibDlg.h"
#include "OTEEntityRender.h"
#include "OTEActionXmlLoader.h"

// -------------------------------------------
using namespace OTE;
using namespace Ogre;

#define IS_EDITVALID	if(CActorEditorMainDlg::s_pActorEntity == NULL || m_pAct == NULL)

// -------------------------------------------
CActionEdit CActionEdit::s_Inst;

// -------------------------------------------
// ��Ⱦ�ص�����

// ��˸Ч��
void OnSceneRender1(int ud)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!CActionEdit::s_Inst.m_hWnd || 
		!CActionEdit::s_Inst.IsWindowVisible())
		return;

	if(::GetKeyState(VK_ESCAPE) & 0x80)
	{		
		SCENE_MGR->ClearCurFocusObj();			

		CActionEdit::s_Inst.m_WaitForPickBone = false;	
	}	

	// ��˸

	Ogre::MovableObject* m = SCENE_MGR->GetCurObjSection();
	if(m && !m->getUserObject())
	{
		static DWORD sSubTime = 0;	
		static int sCount = 0;
		if(::GetTickCount() - sSubTime > 100)
		{		
			SCENE_MGR->GetCurObjSection()->setVisible(!m->isVisible());			
			sSubTime = ::GetTickCount();

			// ֻ��˸һ��
			
			if(m->isVisible() && (sCount ++) >= 1 )
			{							
				m->setUserObject((UserDefinedObject*)0xFFFFFFFF);
				sCount = 0;
			}
		}		

	}

	// ��ʾ�Ǹ�

	if(CActorEditorMainDlg::s_pActorEntity && CActionEdit::s_Inst.m_IsShowBones.GetCheck() == BST_CHECKED)
		COTEEntityBoneRender::RenderEntityBones(CActorEditorMainDlg::s_pActorEntity);

}

// -------------------------------------------
// ������Ϣ����

// ActionList
WNDPROC g_ActionListOldWndProc = NULL;
LRESULT CALLBACK WndProcActionList( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( ::GetKeyState(VK_DELETE) & 0x80 && uMsg == WM_KEYDOWN)
	{		
		CActionEdit::s_Inst.DelAction();
	}
	else if ( uMsg == WM_RBUTTONDOWN)
	{		
		if(CActionEdit::s_Inst.m_ActList.GetCount() > 0)
		{
			CPoint point;
			GetCursorPos(&point);

			if (point.x == -1 && point.y == -1){
				//keystroke invocation
				CRect rect;
				CActionEdit::s_Inst.GetClientRect(rect);
				CActionEdit::s_Inst.ClientToScreen(rect);

				point = rect.TopLeft();
				point.Offset(5, 5);
			}

			CActionEdit::s_Inst.PupMenu(point, IDR_MENU2);	
		}
	}
	else
	{
		return CallWindowProc(g_ActionListOldWndProc,  hWnd, uMsg, wParam, lParam);	
	}

	return 0;
}

// -------------------------------------------
// MagList
WNDPROC g_MagListOldWndProc = NULL;
LRESULT CALLBACK WndProcMagList( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( ::GetKeyState(VK_DELETE) & 0x80 && uMsg == WM_KEYDOWN)
	{		
		CActionEdit::s_Inst.DelEffect();
	}
	else if ( uMsg == WM_RBUTTONDOWN)
	{	
		if(CActionEdit::s_Inst.m_MagList.GetItemCount() > 0)	
		{
			CPoint point;
			GetCursorPos(&point);

			if (point.x == -1 && point.y == -1){
				//keystroke invocation
				CRect rect;
				CActionEdit::s_Inst.GetClientRect(rect);
				CActionEdit::s_Inst.ClientToScreen(rect);

				point = rect.TopLeft();
				point.Offset(5, 5);
			}

			CActionEdit::s_Inst.PupMenu(point, IDR_MENU1);

		}
	}
	else
	{
		return CallWindowProc(g_MagListOldWndProc,  hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


// -------------------------------------------
// CActionEdit �Ի���
// -------------------------------------------

IMPLEMENT_DYNAMIC(CActionEdit, CDialog)
CActionEdit::CActionEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CActionEdit::IDD, pParent)
{
	m_Inited = false;
	m_isEdited = false;
	m_WaitForPickBone = false;
	m_pAct = NULL;	
}

CActionEdit::~CActionEdit()
{
}

void CActionEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ActList, m_ActList);
	DDX_Control(pDX, IDC_MagicList, m_MagList);
	DDX_Control(pDX, IDC_ISLOOP, m_IsLoop);
	//DDX_Control(pDX, IDC_IsAutoPlay, m_IsAutoPlayCheck);
	DDX_Control(pDX, IDC_BONEANIMCOMBO, m_AniListCombo);
	DDX_Control(pDX, IDC_CHECK2, m_IsShowBones);
}

// -------------------------------------------
BEGIN_MESSAGE_MAP(CActionEdit, CDialog)
	ON_LBN_SELCHANGE(IDC_ActList, OnLbnSelchangeActlist)
	ON_BN_CLICKED(IDC_Play, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_Stop, OnBnClickedStop)
	ON_LBN_DBLCLK(IDC_ActList, OnLbnDblclkActlist)
	ON_BN_CLICKED(IDC_Import, OnBnClickedImport)
	ON_BN_CLICKED(IDC_Export, OnBnClickedExport)
	ON_BN_CLICKED(IDC_Add, OnBnClickedAdd)
	ON_NOTIFY(NM_CLICK, IDC_MagicList, OnNMClickMagiclist)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_BN_CLICKED(IDC_AddMagic, OnBnClickedAddmagic)
	ON_BN_CLICKED(IDC_UseTrack, OnBnClickedUsetrack)
	ON_EN_CHANGE(IDC_MagSpeed, OnEnKillfocusMagspeed)
	ON_BN_CLICKED(IDC_CreateTrack, OnBnClickedCreatetrack)
	ON_COMMAND(ID__32793, OnCopyMag)
	ON_NOTIFY(NM_DBLCLK, IDC_MagicList, OnNMDblclkMagiclist)
	ON_BN_CLICKED(IDC_AddSound, OnBnClickedAddsound)

	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)

	ON_EN_CHANGE(IDC_Time, OnEnChangeTime)
	ON_CBN_SELCHANGE(IDC_BONEANIMCOMBO, OnCbnSelchangeBoneanimcombo)	

	ON_EN_CHANGE(IDC_AniSpeed, OnEnChangeAnispeed)
	ON_BN_CLICKED(IDC_ChangeActionNanme, OnBnClickedChangeActionName)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MagicList, OnLvnItemchangedMagiclist)
	//ON_BN_CLICKED(IDC_IsAutoPlay, OnBnClickedIsautoplay)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_ISLOOP, OnBnClickedIsloop)
	ON_CBN_SELENDOK(IDC_BONEANIMCOMBO, OnCbnSelendokBoneanimcombo)
	ON_CBN_KILLFOCUS(IDC_BONEANIMCOMBO, OnCbnKillfocusBoneanimcombo)
END_MESSAGE_MAP()

// -------------------------------------------
void CActionEdit::DisableAllItem()
{	
	GetDlgItem(IDC_ChangeActionNanme)->EnableWindow(false	);
	GetDlgItem(IDC_Save)->EnableWindow(false	);
	GetDlgItem(IDC_Play)->EnableWindow(false	);
	GetDlgItem(IDC_Stop)->EnableWindow(false	);
	GetDlgItem(IDC_Export)->EnableWindow(false	);
	GetDlgItem(IDC_Add)->EnableWindow(false		);
	GetDlgItem(IDC_Import)->EnableWindow(false	);
	GetDlgItem(IDC_ISLOOP)->EnableWindow(false	);	
	GetDlgItem(IDC_AddMagic)->EnableWindow(false);
	GetDlgItem(IDC_AddSound)->EnableWindow(false);
	GetDlgItem(IDC_CreateTrack)->EnableWindow(false	);
	GetDlgItem(IDC_UseTrack)->EnableWindow(false	);	
	GetDlgItem(IDC_Import)->EnableWindow(false	);
	GetDlgItem(IDC_Time)->EnableWindow(false	);
	GetDlgItem(IDC_AniSpeed)->EnableWindow(false);
	GetDlgItem(IDC_MagSpeed)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
	//GetDlgItem(IDC_IsAutoPlay)->EnableWindow(false);	

	SetDlgItemText(IDC_ActName,  ""	    );

}

// --------------------------------------------------------
BOOL CActionEdit::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnSceneRender1);

	// �����¼�����

	if(!g_ActionListOldWndProc)
		g_ActionListOldWndProc = (WNDPROC)::SetWindowLongPtr( this->GetDlgItem(IDC_ActList)->m_hWnd, GWLP_WNDPROC, (INT_PTR) WndProcActionList );
	if(!g_MagListOldWndProc)
		g_MagListOldWndProc = (WNDPROC)::SetWindowLongPtr( this->GetDlgItem(IDC_MagicList)->m_hWnd, GWLP_WNDPROC, (INT_PTR) WndProcMagList );
	

	return ret;
}


// -------------------------------------------
//�����ж��б�
void CActionEdit::UpdateActionList()
{
	m_ActList.ResetContent();
	m_MagList.DeleteAllItems();
	DisableAllItem();

	m_pAct = NULL;	

	if(!m_Inited)
	{
		m_MagList.InsertColumn(0, "����", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 80);
		m_MagList.InsertColumn(1, "����ID", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		m_MagList.InsertColumn(2, "����", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		m_MagList.InsertColumn(3, "��ע", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 50);
		m_MagList.InsertColumn(4, "ʱ��", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		m_MagList.InsertColumn(5, "�ٶ�", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		
		m_MagList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);
		m_Inited = true;
	}

	if(CActorEditorMainDlg::s_pActorEntity != NULL)
	{
		// �ж��б�

		HashMap<std::string, ActionDataMap_t>::iterator actDmIt = COTEActionXmlLoader::GetSingleton()->m_ActionDataMap.begin();
		while(actDmIt != COTEActionXmlLoader::GetSingleton()->m_ActionDataMap.end())	
		{
			HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.begin();
			while(it != actDmIt->second.DataMap.end())
			{
				OTE::COTEActionData *act = it->second;
				m_ActList.AddString(act->m_ResName.c_str());

				++ it;
			}

			++ actDmIt;
		}	

		GetDlgItem(IDC_Import)->EnableWindow(true);
		GetDlgItem(IDC_Add)->EnableWindow(true);
		GetDlgItem(IDC_ActName)->EnableWindow(true);
	}

	m_MagList.Invalidate();
	m_ActList.Invalidate();

	// ����

	m_AniListCombo.ResetContent();

	if(CActorEditorMainDlg::s_pActorEntity && CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
	{	
		short numanis = CActorEditorMainDlg::s_pActorEntity->getMesh()->getSkeleton()->getNumAnimations();
		for(short i = 0; i < numanis; i ++)
		{
			m_AniListCombo.AddString(
				CActorEditorMainDlg::s_pActorEntity->getMesh()->getSkeleton()->getAnimation(i)->getName().c_str()
				);				
		}	
		m_AniListCombo.AddString("��");
		m_AniListCombo.SetCurSel(0);
		m_AniListCombo.EnableWindow();
	}
	else
	{
		m_AniListCombo.SetWindowText("��");
		m_AniListCombo.EnableWindow(false);
	}
}

// -------------------------------------------
//������Ч�б�
void CActionEdit::UpdateEffect()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;	

	GetDlgItem(IDC_CreateTrack)->EnableWindow(false);
	GetDlgItem(IDC_UseTrack)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);

	m_MagList.DeleteAllItems();

	// �켣 / ��Ӧ��

	int i = 0;
	MagBindTrackData_t::iterator it = m_pAct->m_MagicInterMapData.begin();
	while(it != m_pAct->m_MagicInterMapData.end())
	{
		char s[64];
		sprintf(s, "%s %d", it->second->MagicFile, it->second->ID);
		m_MagList.InsertItem(i, s);
		
		itoa(it->second->InterID, s, 10);
		m_MagList.SetItemText(i, 1, s);

		if(it->second->InterType == ActMagicData_t::eReactor)
		{
			m_MagList.SetItemText(i, 2, "Reactor");
			m_MagList.SetItemText(i, 3, it->second->InterData);
		}
		else if(it->second->InterType == ActMagicData_t::eTrack)
		{
			m_MagList.SetItemText(i, 2, "Track");	
			
			COTETrack* tk = COTETrackManager::GetInstance()->GetTrack(it->second->InterID, 	COTEActTrackUsage::GetTrackXmlLoader());
			if(tk)
				m_MagList.SetItemText(i, 3, tk->m_Desc.c_str());
		}
			
		sprintf(s, "%.2f", it->second->PlayTime);
		m_MagList.SetItemText(i, 4, s);

		sprintf(s, "%.2f", it->second->PlaySpeed);
		m_MagList.SetItemText(i, 5, s);

		i++;
		it++;
	}	

	SetDlgItemText(IDC_MagSpeed, "");
	m_MagList.Invalidate();
}

// -------------------------------------------
//ѡȡ�ж�

void CActionEdit::OnLbnSelchangeActlist()
{
	DisableAllItem();

	GetDlgItem(IDC_Import)->EnableWindow(true);
	
	if(CActorEditorMainDlg::s_pActorEntity == NULL)
		return;
		
	CString itemName;
	m_ActList.GetText(m_ActList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{	
		if(m_pAct)
			m_pAct->Stop();
		
		char s[32];		
		
		m_pAct = COTEActorActionMgr::GetAction(
						CActorEditorMainDlg::s_pActorEntity,
						itemName.GetString());
		if(!m_pAct)
		{
			m_pAct =COTEActorActionMgr::AddAction(
				CActorEditorMainDlg::s_pActorEntity,
				itemName.GetString());
		}

		SetDlgItemText(IDC_ActName, m_pAct->GetName().c_str());

		if(m_pAct && m_pAct->GetAnimation().empty())
		{
			GetDlgItem(IDC_AniSpeed)->EnableWindow(false);			
			SetDlgItemText(IDC_AniSpeed, "0.0");
			m_AniListCombo.SetWindowText("��");
		}
		else
		{
			GetDlgItem(IDC_ActName)->EnableWindow(true);
			GetDlgItem(IDC_AniSpeed)->EnableWindow(true);	
			m_AniListCombo.SetWindowText(m_pAct->m_AniName.c_str());	
			sprintf(s, "%1.3f", m_pAct->m_AniSpeed);
			SetDlgItemText(IDC_AniSpeed, s);
		}

		sprintf(s, "%1.3f", m_pAct->m_Length);
		SetDlgItemText(IDC_Time, s);	

		if(m_pAct->m_IsAutoPlay)
			m_IsAutoPlayCheck.SetCheck(BST_CHECKED);
		else
			m_IsAutoPlayCheck.SetCheck(BST_UNCHECKED);	
		
		/*if(m_pAct->m_Mode == OTE::COTEAction::eCONTINUES)
			m_IsLoop.SetCheck(BST_CHECKED);	
		else
			m_IsLoop.SetCheck(BST_UNCHECKED);	*/		

		// ��ʾ�Ǹ�

		GetDlgItem(IDC_CHECK2)->EnableWindow(
			CActorEditorMainDlg::s_pActorEntity->hasSkeleton());

		UpdateEffect();		
			
		GetDlgItem(IDC_ChangeActionNanme)->EnableWindow(true);	
		GetDlgItem(IDC_Save)->EnableWindow(true);
		GetDlgItem(IDC_Play)->EnableWindow(true);
		GetDlgItem(IDC_Add)->EnableWindow(true);
		GetDlgItem(IDC_Stop)->EnableWindow(true);
		GetDlgItem(IDC_Export)->EnableWindow(true);
		GetDlgItem(IDC_ISLOOP)->EnableWindow(true);		
		GetDlgItem(IDC_AddMagic)->EnableWindow(true);
		GetDlgItem(IDC_AddSound)->EnableWindow(true);
		GetDlgItem(IDC_Time)->EnableWindow(true);	
		GetDlgItem(IDC_Import)->EnableWindow(true);
		//GetDlgItem(IDC_IsAutoPlay)->EnableWindow(true);
		
	}
}

// -------------------------------------------
//�����ж�

void CActionEdit::OnBnClickedPlay()
{
	if(CActorEditorMainDlg::s_pActorEntity == NULL)
		return;

	if(CKeyFrameDlg::s_Inst.m_hWnd)
	{
		CKeyFrameDlg::s_Inst.UpdateTimePosMove(0);
		CKeyFrameDlg::s_Inst.DestoryMe();
	}

	CString itemName;
	m_ActList.GetText(m_ActList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		if(m_pAct != NULL)
			OnBnClickedStop();
		
		m_pAct = COTEActorActionMgr::SetAction(
			CActorEditorMainDlg::s_pActorEntity,
			itemName.GetString(), 
			m_IsLoop.GetCheck() == BST_CHECKED ? OTE::COTEAction::eCONTINUES : OTE::COTEAction::ePLAY_ONCE);

	}
}
// -------------------------------------------
//ֹͣ����
void CActionEdit::OnBnClickedStop()
{
	SCENE_MGR->ClearCurFocusObj();

	if(m_pAct)
		m_pAct->Stop();
}
// -------------------------------------------
//˫�������ж�
void CActionEdit::OnLbnDblclkActlist()
{
	SCENE_MGR->ClearCurFocusObj();

	OnBnClickedPlay();
}
// -------------------------------------------
//�����ж�
void CActionEdit::OnBnClickedImport()
{
	if(CActorEditorMainDlg::s_pActorEntity == NULL)
		return;
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�ж��ļ�(*.act)|*.act|All Files (*.*)|*.*|",
		NULL 
		); 	

	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{ 
		if(m_pAct != NULL)
		{
			m_pAct->Stop();
			m_pAct = NULL;
		}	
		
		COTEActionXmlLoader::GetSingleton()->ReadActionData(tpFileDialog->GetPathName().GetString());

        UpdateActionList();
	}
}
// -------------------------------------------
//�����ж�
void CActionEdit::OnBnClickedExport()
{
	if(CActorEditorMainDlg::s_pActorEntity == NULL)
		return;

	CString itemName;
	m_ActList.GetText(m_ActList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		CHAR curDir[256];
		::GetCurrentDirectory(256, curDir);
		CFileDialog* tpFileDialog=new CFileDialog(false, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"�ж��ļ�(*.act)|*.act|All Files (*.*)|*.*|",
			NULL 
			); 
		int nResponse =tpFileDialog->DoModal();
		::SetCurrentDirectory(curDir);

		if (nResponse == IDOK)
		{
			std::string file = tpFileDialog->GetPathName();
			if(file.find(".act") == -1)
				file = file + ".act";
			COTEActionXmlLoader::GetSingleton()->SaveActionData(file.c_str());
		}
	}
}

// -------------------------------------------
//ɾ���ж�
void CActionEdit::DelAction()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	CString itemName;
	m_ActList.GetText(m_ActList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{	
		if(MessageBox("ȷ��Ҫɾ�����ж���?", "ȷ��", MB_YESNO) == IDYES)
		{
			if(m_pAct)
			{
				m_pAct->Stop();
				m_pAct = NULL;
			}

			if(CActorEditorMainDlg::s_pActorEntity->GetCurAction())
			{
				CActorEditorMainDlg::s_pActorEntity->GetCurAction()->Stop();
				COTEActorActionMgr::SetAction(
								CActorEditorMainDlg::s_pActorEntity,	"");			
			}
			
			COTEActorActionMgr::RemoveAction(
						CActorEditorMainDlg::s_pActorEntity,
						itemName.GetString());

			COTEActionXmlLoader::GetSingleton()->DeleteActionData(itemName.GetString());

			UpdateActionList();

			if(CKeyFrameDlg::s_Inst.m_hWnd)
				CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);

		}
	}

}

// -------------------------------------------
//ɾ��Ч��
void CActionEdit::DelEffect()
{
	IS_EDITVALID{ return; }	

	if(MessageBox("ȷ��Ҫɾ����Ч����?", "ȷ��", MB_YESNO) == IDYES)
	{
		// clear

		SCENE_MGR->ClearCurFocusObj();			

		if(m_pAct != NULL)
			m_pAct->Stop();

		int tItem = m_MagList.GetSelectionMark();

		if(tItem > m_MagList.GetItemCount() || tItem < 0)
			return;

		CString sText = m_MagList.GetItemText(tItem, 1);
		char fileName[32];	int id;
		sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

		if(sText != "��" && !sText.IsEmpty())		
		{
			// ��������
			
			m_pAct->RemoveEffectData(id);
			MAGIC_MGR->_UpdateGrave();

		}

		UpdateEffect();	

		if(CKeyFrameDlg::s_Inst.m_hWnd)
			CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	}

}

// -------------------------------------------
// ����ж���

bool CActionEdit::IsActionNameValid(const CString& rText)
{	
	if(rText.IsEmpty())
	{
		OTE_MSG("����ж� ʧ��! ���Ʋ���Ϊ��!", "ʧ��")
		return false;
	}

	int id;
	char name[32];
	if(sscanf((char*)LPCTSTR(rText), "%d%s", &id, name) != 2)
	{
		OTE_MSG("����ж� ʧ��! �ж����� ���� [NUMBER][STRING]��ʽ!", "ʧ��")
		return false;
	}

	return true;
}
// -------------------------------------------
//����ж�
void CActionEdit::OnBnClickedAdd()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	CString sText;
	GetDlgItemText(IDC_ActName, sText);

	if(!IsActionNameValid(sText))
		return;

	if(!COTEActorActionMgr::GetAction(
		CActorEditorMainDlg::s_pActorEntity, sText.GetString()))
	{
		if(m_pAct)
			m_pAct->Stop();
		
		COTEActionXmlLoader::GetSingleton()->CreateActionData(sText.GetString());
		m_pAct = COTEActorActionMgr::AddAction(CActorEditorMainDlg::s_pActorEntity, sText.GetString());	
		
		m_AniListCombo.GetWindowText(sText);
		if(!sText.IsEmpty() && sText != "��" 
			&& CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		{	
			if(m_pAct->SetAnimation(sText.GetString()))
			{
				Ogre::AnimationState *tas = CActorEditorMainDlg::s_pActorEntity->getAnimationState(sText.GetString());
				m_pAct->m_Length = tas->getLength();
			}

		}
		else
		{
			m_pAct->SetAnimation("");
			SetActionTime();
			

			GetDlgItemText(IDC_Time, sText);	
		}
		
		m_pAct->m_IsAutoPlay = (m_IsAutoPlayCheck.GetCheck() == BST_CHECKED);		
	

		UpdateActionList();
	}
}


// -------------------------------------------
//��ѡĳһ��Ч
void CActionEdit::OnNMClickMagiclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	SelectObj(false);
}

// -------------------------------------------
void SetSelObj(Ogre::MovableObject* m)
{
	if(m->getParentSceneNode())
	{
		m->getParentSceneNode()->showBoundingBox(true);			
		m->setUserObject((UserDefinedObject*)0);
		SCENE_MGR->SetCurFocusObj(m);
	}
}

// -------------------------------------------
void CActionEdit::SelectObj(bool fixedView)
{	
	int tItem, tSubItem;
	CRect tSubItemRect;

	if( m_MagList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && 
		CActorEditorMainDlg::s_pActorEntity && 
		m_pAct)
	{
		if(tItem > m_MagList.GetItemCount() || tItem < 0)
			return;			
	
		GetDlgItem(IDC_CreateTrack)->EnableWindow(true);
		GetDlgItem(IDC_UseTrack)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(true);

		CString sText = m_MagList.GetItemText(tItem, 1);
		char s[32] = {0};
		if(sText != "��" && !sText.IsEmpty())
		{				
			char fileName[32];	int id;
			sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

			sprintf(s, "%1.3f", m_pAct->GetPlaySpeed(id));	

			int trid = atoi((char*)LPCTSTR(m_MagList.GetItemText(tItem, 1)));

			static int s_trid = trid;
			
			Ogre::MovableObject* m = m_pAct->GetMagByInterID(s_trid);
			if(m)
			{	
				Ogre::SceneNode* sn = m->getParentSceneNode();
				if(sn)
					sn->showBoundingBox(false);
				m->setVisible(true);				
			}

			m = m_pAct->GetMagByInterID(trid);

			if(m)
			{
				SetSelObj(m);

				m_pAct->UpdateTrackMagicPos();

				if(fixedView)
				{					
					SCENE_MGR->SetCurFocusObj(m);					
				}
			}
			s_trid = trid;
		}
		
		GetDlgItem(IDC_MagSpeed)->EnableWindow(true);
		SetDlgItemText(IDC_MagSpeed, s);

	}	
}

// -------------------------------------------
//�����ж�
void CActionEdit::OnBnClickedSave()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	int lastSel = m_ActList.GetCurSel();

	if(m_pAct)
	{		
		CString sText;
		GetDlgItemText(IDC_ActName, sText);

		m_pAct->SetName(sText.GetString());		
		m_pAct->m_IsAutoPlay = (m_IsAutoPlayCheck.GetCheck() == BST_CHECKED);
		
		m_AniListCombo.GetWindowText(sText);
		if(!sText.IsEmpty() && sText != "��" && 
			CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		{
			m_pAct->SetAnimation(sText.GetString());
		}
		else
		{
			m_pAct->SetAnimation("");
			SetActionTime();
		}

		GetDlgItemText(IDC_Time, sText);
		m_pAct->m_Length = atof(sText.GetString());

		// ���浱ǰ�ж��켣

		for(int index = 0; index < m_MagList.GetItemCount(); index ++)
		{
			int id1;
			sscanf((char*)LPCTSTR(m_MagList.GetItemText(index, 1)), "%d", &id1);			
			if(m_MagList.GetItemText(index, 2) == "Track")	
				COTEActTrackUsage::GetTrackXmlLoader()->TrackWriteXml(id1);
			else if(m_MagList.GetItemText(index, 2) == "Reactor")
			{
				// todo
			}

		}

		// ����켣���ļ�

		COTEActTrackUsage::GetTrackXmlLoader()->SaveFile();	

	}

	/// note: д���� ��һ�����屣�� �ڶ��鱣�浱ǰ�༭֮�ж�
	// �����ж�

	COTEActionXmlLoader::GetSingleton()->SaveAllActionData();

	// �����ɫ�ڴ��е��ж�
	
	HashMap<std::string, COTEAction*>::iterator it = 
		CActorEditorMainDlg::s_pActorEntity->m_ActionList.begin();	
	while(it !=	CActorEditorMainDlg::s_pActorEntity->m_ActionList.end())
	{
		COTEActionXmlLoader::GetSingleton()->SaveActionData(it->second);
		++ it;
	}

	// ���������

	UpdateActionList();	

	m_ActList.SetCurSel(lastSel);	
	this->SendDlgItemMessage(IDC_ActList, LBN_DBLCLK);

	m_isEdited = false;
	CKeyFrameDlg::s_Inst.m_IsEdited = false;

	if(CKeyFrameDlg::s_Inst.m_hWnd)
		CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	
}

// -------------------------------------------
//�����Ч
void CActionEdit::OnBnClickedAddmagic()
{
	IS_EDITVALID{ return; }		

	CAddEffEntityDlg dlg;

	if(dlg.DoModal() == IDOK)
	{
		if(CKeyFrameDlg::s_Inst.m_hWnd)
			CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	}
}


// -------------------------------------------
//ʹ�����й켣
void CActionEdit::OnBnClickedUsetrack()
{
	IS_EDITVALID{ return; }		
	
	int tItem = m_MagList.GetLastSelect();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	CString sText = m_MagList.GetItemText(tItem, 0);
	if(!sText.IsEmpty())
	{
		CTrackSelect::s_Inst.ShowWindow(SW_SHOW);
		CTrackSelect::s_Inst.UpdtatTrackList(CActorEditorMainDlg::s_pActorEntity->hasSkeleton());
	}
}

// -------------------------------------------
//�ı���Ч�����ٶ�
void CActionEdit::OnEnKillfocusMagspeed()
{
	if(CActorEditorMainDlg::s_pActorEntity != NULL && m_pAct != NULL)
	{
		int tItem = m_MagList.GetLastSelect();

		if(tItem > m_MagList.GetItemCount() || tItem < 0)
			return;

		CString sText;
		GetDlgItemText(IDC_MagSpeed, sText);
		float speed = atof(sText.GetString());

		if(speed > 0.0f)
		{
			m_MagList.SetItemText(tItem, 5, sText);

			char fileName[32];	int id;
			sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

			sText = m_MagList.GetItemText(tItem, 1);				
		}
	}
}

// -------------------------------------------
//�����켣
void CActionEdit::OnBnClickedCreatetrack()
{
	IS_EDITVALID{ return; }		

	int tItem = 0;
	if(m_MagList.GetItemCount() > 0)
		tItem = m_MagList.GetLastSelect();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	if(m_MagList.GetItemText(tItem, 1) != "��")
	{
		if(::MessageBox(NULL, "��������Ѿ��ﶨ��һ��������Ƿ��½�һ����", "��ʾ", MB_YESNO) == IDYES)
			Createtrack(tItem);
	}
	else
	{
		Createtrack(tItem);
	}

}
// -------------------------------------------
//�����켣
int CActionEdit::Createtrack(int item, int trackId)
{
	if(COTETrackManager::GetInstance()->IsTrackPresent(trackId))
	{
		::MessageBox(NULL, "������������ظ��ɣ�!", "ʧ��", MB_OK);
		return -1;
	}

	int tItem = item;
	CString sText = m_MagList.GetItemText(tItem, 1);
	char fileName[32];	int id;
	sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

	// ����Ҫ��ɾ����������Ч��

	if(!sText.IsEmpty() && sText != "��")
	{
		m_pAct->RemoveEffectData(id);
		MAGIC_MGR->_UpdateGrave();
	}	
	
	// ��������ӹ��

	COTETrackManager::GetInstance()->CreateNewTrack(trackId, COTEActTrackUsage::GetTrackXmlLoader());
		
	OTE::ActMagicData_t *mag = OTE::ActMagicData_t::CreateActMagic();	
	strcpy(mag->MagicFile, fileName);

	m_pAct->AddEffectData(mag, trackId);	

	// �����ٶ�
	OnEnKillfocusMagspeed();

	UpdateEffect();

	// ������ֱ�ӱ༭���

	sText = m_MagList.GetItemText(tItem, 1);
	if(!sText.IsEmpty() && sText != "��")
	{
		if(!CKeyFrameDlg::s_Inst.m_hWnd)
			CKeyFrameDlg::s_Inst.Create(IDD_KEYFRAMEDLG_DIALOG, this);
		CKeyFrameDlg::s_Inst.SetWindowText(CString("�ؼ���༭�� - ") + sText);
		CKeyFrameDlg::s_Inst.ShowWindow(SW_SHOW);
		CKeyFrameDlg::s_Inst.UpdateData(trackId);		

		CRect rt;
		CKeyFrameDlg::s_Inst.GetWindowRect(rt);
		CKeyFrameDlg::s_Inst.InvalidateRect(NULL);
	}

	return trackId;
}

// -------------------------------------------
void CActionEdit::EditEffect()
{
	if(m_MagList.GetItemText(m_MagList.GetSelectionMark(), 2) == "Track")
	{
		// �༭���
		EditTrack();
	}
}

// -------------------------------------------
//�༭�켣
void CActionEdit::EditTrack()
{
	IS_EDITVALID{ return; }		

	int tItem = m_MagList.GetSelectionMark();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	CString sText = m_MagList.GetItemText(tItem, 1);
	if(!sText.IsEmpty() && sText != "��")
	{
		EditTrack(atoi(sText));
	}

}

// -------------------------------------------
//�Զ�ѡ���Ӧ�����Ч���б�����

void CActionEdit::AutoSelEffect(int trackID)
{
	IS_EDITVALID{ return; }		
	
	for(int index = 0; index < m_MagList.GetItemCount(); index ++)
	{
		int id = atoi(m_MagList.GetItemText(index, 1));
		if(trackID == id)
		{
			m_MagList.SetItemState(index, 0);
			break;
		}
	}
}
// -------------------------------------------
void CActionEdit::EditTrack(int trackID)
{	
	if(!CKeyFrameDlg::s_Inst.m_hWnd)
		CKeyFrameDlg::s_Inst.Create(IDD_KEYFRAMEDLG_DIALOG, this);

	CString txt;
	txt.Format("�ؼ���༭�� - %d", trackID);

	CKeyFrameDlg::s_Inst.SetWindowText(txt);
	CKeyFrameDlg::s_Inst.ShowWindow(SW_SHOW);

	//if(CKeyFrameDlg::s_Inst.m_TrackID == trackID)
	{
		CKeyFrameDlg::s_Inst.UpdateData(trackID);
		
		CKeyFrameDlg::s_Inst.InvalidateRect(NULL);

	}	
}

// -------------------------------------------
//�޸��ж�����
void CActionEdit::SetActionTime()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !m_pAct)
		return;

	CString sText;
	GetDlgItemText(IDC_Time, sText);
	float length = atof(sText.GetString());

	if(length > 0.0f)
	{
		m_pAct->m_Length = length;
	}
	else
	{
		m_pAct->m_Length = 2.0f;
	}
		
	if(CKeyFrameDlg::s_Inst.m_hWnd && CKeyFrameDlg::s_Inst.IsWindowVisible())
	{
		CKeyFrameDlg::s_Inst.UpdateData(CKeyFrameDlg::s_Inst.m_TrackID);
		CKeyFrameDlg::s_Inst.InvalidateRect(NULL); 
	}
}

// -------------------------------------------
bool CActionEdit::AnalyseInput(int item, unsigned int& interID)
{
	std::string actName = m_pAct->GetName();
	int actID;
	char tail[32];
	if(sscanf(actName.c_str(), "%d%s", &actID, tail) != 2)
	{
		::MessageBox(NULL, "����trackʧ�ܣ� �������ж����� ���� [NUMBER][STRING]��ʽ", "ʧ��",	MB_OK);
		return false;
	}

	assert(m_MagList.GetItemCount() < 100);
	//interID = actID * 100 + m_MagList.GetItemCount();
	interID = actID * 100/* + item*/; // ���յ�ǰѡ����к� (action ID) + (Item Index) * 100 �ĸ�ʽ

	return true;
}

// -------------------------------------------
int CActionEdit::Createtrack(int item)
{
	unsigned int trackID;
	if(!AnalyseInput(item, trackID))
		return -1;

	int cnt = -1;
	while(cnt < 100)
	{
		if( !COTETrackManager::GetInstance()->IsTrackPresent(trackID + (++ cnt)) )
			break;		
	}

	return Createtrack(item, trackID + cnt);
}

// -------------------------------------------
void CActionEdit::AddEffectMagic(const std::string& magicFile)
{
	this->UpdateEffect();

	int itemInd = m_MagList.GetItemCount();
	m_MagList.InsertItem(itemInd, (magicFile + " 0").c_str());
	m_MagList.SetItemText(itemInd, 1, "��");
}

// -------------------------------------------
// ������Ч

void CActionEdit::OnCopyMag()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !m_pAct)
		return;
				
	int tItem = m_MagList.GetLastSelect();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	char fileName[32];	int id;

	sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

	AddEffectMagic(fileName);	
	
	sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 1)), "%d", &id);	

	// �й����

	if(id > 0)
	{
		OTE::COTETrack * selTrack = COTETrackManager::GetInstance()->GetTrack(id, COTEActTrackUsage::GetTrackXmlLoader());
		
		if(selTrack)
		{ 
			// �ҳ�������Ч���ڵ�item
			int index = 0;
			for(; index < m_MagList.GetItemCount(); index ++)
			{
				char fileName1[32];	int id1;
				sscanf((char*)LPCTSTR(m_MagList.GetItemText(index, 0)), "%s %d", fileName1, &id1);

				CString tt = m_MagList.GetItemText(index, 1);
				if(tt == "��" && strcmp(fileName1, fileName) == 0)
				{
					break;
				}				
			}
			
			// �Լ���֯trackid, �������������淶��			

			int trackID = Createtrack(index);
			assert(trackID >= 0);

			CKeyFrameDlg::s_Inst.m_pTrack = COTETrackManager::GetInstance()->GetTrack(trackID, COTEActTrackUsage::GetTrackXmlLoader());

			COTEActTrackUsage::Clone(selTrack, CKeyFrameDlg::s_Inst.m_pTrack);			
			
		}

		UpdateEffect();

	}	

	m_isEdited = true;

}


// --------------------------------------------------------
#define DO_EDITOK(type)  CListViewEditor dlg(type, m_MagList.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && (!CListViewEditor::s_Text.IsEmpty() && type != 1 || type == 1))

void CActionEdit::OnNMDblclkMagiclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;	

	if(m_MagList.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{	
		switch(tSubItem)
		{
		case 3:
			{
				// ����
				DO_EDITOK(0)
				{								
					CString tt = m_MagList.GetItemText(tItem, 1);
					int id = atoi((char*)LPCTSTR(tt) );

					tt = m_MagList.GetItemText(tItem, 2);
					m_MagList.SetItemText(tItem, 3, CListViewEditor::s_Text);

					if(tt == "Track")
					{
						COTETrack* tk = COTETrackManager::GetInstance()->GetTrack(id, COTEActTrackUsage::GetTrackXmlLoader());
						if(tk)
						{
							tk->m_Desc = CListViewEditor::s_Text;					
							COTEActTrackUsage::GetTrackXmlLoader()->TrackWriteXml(id);
						}
					}
				}

			}break;

		case 4:
			{
				// ����ʱ��

				DO_EDITOK(0)
				{		
					// magic ID
					char fileName[32];	int id;
					sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);
					
					// inter ID		
					int interID = atoi((char*)LPCTSTR(m_MagList.GetItemText(tItem, 1)) );				
					
					m_pAct->SetPlayTime(id, atof(CListViewEditor::s_Text));
					m_MagList.SetItemText(tItem, 4, CListViewEditor::s_Text);
				}	
			}break;
		case 5:
			{
				// �����ٶ�

				DO_EDITOK(0)
				{		
					// magic ID
					char fileName[32];	int id;
					sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);
					
					// inter ID		
					int interID = atoi((char*)LPCTSTR(m_MagList.GetItemText(tItem, 1)) );				
					
					m_pAct->SetPlaySpeed(id, atoi(CListViewEditor::s_Text));
					m_MagList.SetItemText(tItem, 5, CListViewEditor::s_Text);
				}	
			}break;

		default:
			{
				CString tt = m_MagList.GetItemText(tItem, 1);
				int id = atoi((char*)LPCTSTR(tt) );

				if(tt == "Track")
				{
					COTETrack* tk = COTETrackManager::GetInstance()->GetTrack(id, COTEActTrackUsage::GetTrackXmlLoader());
					if(tk)
					{
						// ��ʾ����󶨵Ĺ���
						COTEEntityBoneRender::SetSelectedBoneName(tk->m_AttBoneName);
					}
				}
					
				EditEffect();	
			}
		}
	}	

	SelectObj(true);

}

// --------------------------------------------------------
// �������
void CActionEdit::OnBnClickedAddsound()
{
	if(!CSoundDlg::s_Inst.m_hWnd)
	{
		CSoundDlg::s_Inst.Create(IDD_SOUND_DLG);
	}
	CSoundDlg::s_Inst.ShowWindow(SW_SHOW);
}


// --------------------------------------------------------
void CActionEdit::PupMenu(const CPoint& rPoint, DWORD resourceID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMenu menu;
	menu.LoadMenu(resourceID);
	int aaa = GetLastError();

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = &CActionEdit::s_Inst;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rPoint.x, rPoint.y,
		pWndPopupOwner);

				
}

// -------------------------------------------
void CActionEdit::OnEndPlugin()
{
	SCENE_MGR->RemoveListener(OnSceneRender1);

}

void CActionEdit::OnBnClickedButton2()
{
	
}

// -------------------------------------------
// ������action����track�б�
void CActionEdit::ImportMagList()
{	
	m_isEdited = true;

	CActionListDlg dlg;
	dlg.DoModal();

	UpdateActionList();
}


// -------------------------------------------
// �ı��ж�����
void CActionEdit::OnEnChangeTime()
{
	this->SetActionTime();
	
}

// -------------------------------------------
// �Ǹ춯��
void CActionEdit::OnCbnSelchangeBoneanimcombo()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !m_pAct)	
		return;	
	
	CString tLength;
	CString tText;
	m_AniListCombo.GetWindowText(tText);

	tLength.Format("%1.3f", 2.0f);	
	if(!tText.IsEmpty() && tText != "��")
	{		
		if(CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		{
			if(	CActorEditorMainDlg::s_pActorEntity->IsAnimationStatePresent(tText.GetString()))
			{
				tLength.Format("%1.3f", CActorEditorMainDlg::s_pActorEntity->getAnimationState(tText.GetString())->getLength());
				m_pAct->SetAnimation(tText.GetString());	

				SetDlgItemText(IDC_AniSpeed, "1.0");
				m_pAct->m_AniSpeed = 1.0f;
			}
		}
	}
	else
	{			
		if(!tText.IsEmpty() && tText != "��")
		{
			m_AniListCombo.SetWindowText("��");
		}
	}

	SetDlgItemText(IDC_Time, tLength);	
}


// -------------------------------------------
//�ı��ж��ж����Ĳ����ٶ�
void CActionEdit::OnEnChangeAnispeed()
{
	IS_EDITVALID{ return; }		

	CString sText;
	GetDlgItemText(IDC_AniSpeed, sText);
	float speed = atof(sText.GetString());

	if(speed >= 0.0f)
	{
		m_pAct->m_AniSpeed = speed;
	}
}	

// -------------------------------------------
// �޸��ж���
void CActionEdit::OnBnClickedChangeActionName()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !this->m_pAct)
		return;	

	// �޸����е�
	
	CString sText;
	GetDlgItemText(IDC_ActName, sText);	
	
	if(!IsActionNameValid(sText))
		return;

	if(!sText.IsEmpty() && m_pAct)	
	{
		COTEActionXmlLoader::GetSingleton()->RenameActionData(
			m_pAct->GetName(), sText.GetString());
		
		m_pAct->SetName(sText.GetString());		

		UpdateActionList();
	}
}

// -------------------------------------------
// �󶨷�Ӧ��
void CActionEdit::OnBnClickedButton1()
{
	CReactorLibDlg dlg;
	dlg.DoModal();
	
}

// -------------------------------------------
//������Ӧ��
void CActionEdit::CreateReactor(const std::string& rResName)
{
	if(!CActorEditorMainDlg::s_pActorEntity || !this->m_pAct)
		return;	

	int tItem = m_MagList.GetLastSelect();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	unsigned int reactorID;

	if(AnalyseInput(tItem, reactorID))
	{
		int cnt = -1;
		while(cnt < 100)
		{
			if( !COTEReactorManager::GetSingleton()->GetReactor(reactorID + (++ cnt)) )
				break;		
		}

		CreateReactor(tItem, reactorID + cnt, rResName);
	}
}

// -------------------------------------------
//������Ӧ��
int CActionEdit::CreateReactor(int item, int reactorId, const std::string& rResName)
{
	// ������

	int tItem = item;
	CString sText = m_MagList.GetItemText(tItem, 1);
	char fileName[32];	int id;
	sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

	if(!sText.IsEmpty() && sText != "��")
	{
		m_pAct->RemoveEffectData(id);
		MAGIC_MGR->_UpdateGrave();
	}	

	// ����

	OTE::ActMagicData_t *mag = ActMagicData_t::CreateActMagic(ActMagicData_t::eReactor);
	strcpy(mag->MagicFile, fileName);		  // magic�ļ���
	strcpy(mag->InterData, rResName.c_str()); // ��Ӧ������
	mag->InterType = ActMagicData_t::eReactor;
	
	CActionEdit::s_Inst.m_pAct->AddEffectData(mag, reactorId);

	UpdateEffect();

	
	return reactorId;
}

// ---------------------------------------------------
void CActionEdit::OnSceneLButtonUp(float x, float y)
{		
	//m_WaitForPickBone = false;
}

// ---------------------------------------------------
void CActionEdit::OnSceneRButtonUp(float x, float y)
{
	
}
// ---------------------------------------------------
void CActionEdit::OnSceneLButtonDown(float x, float y)
{	
	if(!CActorEditorMainDlg::s_pActorEntity || !m_pAct)
		return;

	// ��ѡ�Ǹ�

	if(m_WaitForPickBone &&
		m_IsShowBones.GetCheck() == BST_CHECKED)
	{
		Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay((float)x, (float)y);	

		Node* n = COTEEntityBonePicker::PickEntityBones(CActorEditorMainDlg::s_pActorEntity, &tCameraRay);
		if(n)
		{			
			COTEEntityBoneRender::SetSelectedBoneName(n->getName());
			
			if( CKeyFrameDlg::s_Inst.m_hWnd && 
				CKeyFrameDlg::s_Inst.IsWindowVisible() && 
				CKeyFrameDlg::s_Inst.m_pTrack)
			{
				CKeyFrameDlg::s_Inst.m_pTrack->SetBindBone(n->getName());
				CKeyFrameDlg::s_Inst.UpdataSlider();
				m_WaitForPickBone = false;
			}
		}
		return;
	}

	// ѡ�����

	if(m_hWnd && IsWindowVisible() && (::GetKeyState(VK_CONTROL) & 0x80))
	{
		// ��ѡ
		Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay((float)x, (float)y);		

		// ��ѡ��Ч

		MovableObject* m = COTECollisionManager::PickAABB_DisPrio(tCameraRay);
		if(m)
		{
			int trackID = m_pAct->GetInterIDByMag(m);
			if(trackID >= 0)
			{	
				AutoSelEffect(trackID);
				EditTrack(trackID);	
				SetSelObj(m);
			}
		}
							
	}
}

// ---------------------------------------------------
void CActionEdit::OnBnClickedCheck2()
{
	if(m_WaitForPickBone && m_IsShowBones.GetCheck() == BST_CHECKED)
	{
		m_WaitForPickBone = false;
	}

	if(m_IsShowBones.GetCheck() == BST_CHECKED &&
		CKeyFrameDlg::s_Inst.m_pTrack && 
		!CKeyFrameDlg::s_Inst.m_pTrack->m_AttBoneName.empty())
	{
		COTEEntityBoneRender::SetSelectedBoneName(CKeyFrameDlg::s_Inst.m_pTrack->m_AttBoneName);
	}
	
}

// ---------------------------------------------------
void CActionEdit::OnLvnItemchangedMagiclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CActionEdit::OnBnClickedIsautoplay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CActionEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		// ����action�б�

		UpdateActionList();
	}
}

void CActionEdit::OnBnClickedIsloop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CActionEdit::OnCbnSelendokBoneanimcombo()
{
	
}

void CActionEdit::OnCbnKillfocusBoneanimcombo()
{
	OnCbnSelchangeBoneanimcombo();
}
