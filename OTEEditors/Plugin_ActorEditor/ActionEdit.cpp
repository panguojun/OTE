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
// 渲染回调函数

// 闪烁效果
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

	// 闪烁

	Ogre::MovableObject* m = SCENE_MGR->GetCurObjSection();
	if(m && !m->getUserObject())
	{
		static DWORD sSubTime = 0;	
		static int sCount = 0;
		if(::GetTickCount() - sSubTime > 100)
		{		
			SCENE_MGR->GetCurObjSection()->setVisible(!m->isVisible());			
			sSubTime = ::GetTickCount();

			// 只闪烁一次
			
			if(m->isVisible() && (sCount ++) >= 1 )
			{							
				m->setUserObject((UserDefinedObject*)0xFFFFFFFF);
				sCount = 0;
			}
		}		

	}

	// 显示骨胳

	if(CActorEditorMainDlg::s_pActorEntity && CActionEdit::s_Inst.m_IsShowBones.GetCheck() == BST_CHECKED)
		COTEEntityBoneRender::RenderEntityBones(CActorEditorMainDlg::s_pActorEntity);

}

// -------------------------------------------
// 窗口消息钩子

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
// CActionEdit 对话框
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

	// 窗口事件钩子

	if(!g_ActionListOldWndProc)
		g_ActionListOldWndProc = (WNDPROC)::SetWindowLongPtr( this->GetDlgItem(IDC_ActList)->m_hWnd, GWLP_WNDPROC, (INT_PTR) WndProcActionList );
	if(!g_MagListOldWndProc)
		g_MagListOldWndProc = (WNDPROC)::SetWindowLongPtr( this->GetDlgItem(IDC_MagicList)->m_hWnd, GWLP_WNDPROC, (INT_PTR) WndProcMagList );
	

	return ret;
}


// -------------------------------------------
//更新行动列表
void CActionEdit::UpdateActionList()
{
	m_ActList.ResetContent();
	m_MagList.DeleteAllItems();
	DisableAllItem();

	m_pAct = NULL;	

	if(!m_Inited)
	{
		m_MagList.InsertColumn(0, "对象", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 80);
		m_MagList.InsertColumn(1, "作用ID", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		m_MagList.InsertColumn(2, "类型", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		m_MagList.InsertColumn(3, "备注", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 50);
		m_MagList.InsertColumn(4, "时刻", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		m_MagList.InsertColumn(5, "速度", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		
		m_MagList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);
		m_Inited = true;
	}

	if(CActorEditorMainDlg::s_pActorEntity != NULL)
	{
		// 行动列表

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

	// 动画

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
		m_AniListCombo.AddString("无");
		m_AniListCombo.SetCurSel(0);
		m_AniListCombo.EnableWindow();
	}
	else
	{
		m_AniListCombo.SetWindowText("无");
		m_AniListCombo.EnableWindow(false);
	}
}

// -------------------------------------------
//更新特效列表
void CActionEdit::UpdateEffect()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;	

	GetDlgItem(IDC_CreateTrack)->EnableWindow(false);
	GetDlgItem(IDC_UseTrack)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);

	m_MagList.DeleteAllItems();

	// 轨迹 / 反应器

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
//选取行动

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
			m_AniListCombo.SetWindowText("无");
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

		// 显示骨胳

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
//播放行动

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
//停止播放
void CActionEdit::OnBnClickedStop()
{
	SCENE_MGR->ClearCurFocusObj();

	if(m_pAct)
		m_pAct->Stop();
}
// -------------------------------------------
//双击播放行动
void CActionEdit::OnLbnDblclkActlist()
{
	SCENE_MGR->ClearCurFocusObj();

	OnBnClickedPlay();
}
// -------------------------------------------
//导入行动
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
		"行动文件(*.act)|*.act|All Files (*.*)|*.*|",
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
//导出行动
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
			"行动文件(*.act)|*.act|All Files (*.*)|*.*|",
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
//删除行动
void CActionEdit::DelAction()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	CString itemName;
	m_ActList.GetText(m_ActList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{	
		if(MessageBox("确定要删除该行动吗?", "确认", MB_YESNO) == IDYES)
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
//删除效果
void CActionEdit::DelEffect()
{
	IS_EDITVALID{ return; }	

	if(MessageBox("确定要删除该效果吗?", "确认", MB_YESNO) == IDYES)
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

		if(sText != "无" && !sText.IsEmpty())		
		{
			// 解除邦定作用
			
			m_pAct->RemoveEffectData(id);
			MAGIC_MGR->_UpdateGrave();

		}

		UpdateEffect();	

		if(CKeyFrameDlg::s_Inst.m_hWnd)
			CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	}

}

// -------------------------------------------
// 检测行动名

bool CActionEdit::IsActionNameValid(const CString& rText)
{	
	if(rText.IsEmpty())
	{
		OTE_MSG("添加行动 失败! 名称不能为空!", "失败")
		return false;
	}

	int id;
	char name[32];
	if(sscanf((char*)LPCTSTR(rText), "%d%s", &id, name) != 2)
	{
		OTE_MSG("添加行动 失败! 行动命名 不是 [NUMBER][STRING]格式!", "失败")
		return false;
	}

	return true;
}
// -------------------------------------------
//添加行动
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
		if(!sText.IsEmpty() && sText != "无" 
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
//点选某一特效
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
		if(sText != "无" && !sText.IsEmpty())
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
//保存行动
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
		if(!sText.IsEmpty() && sText != "无" && 
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

		// 保存当前行动轨迹

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

		// 保存轨迹到文件

		COTEActTrackUsage::GetTrackXmlLoader()->SaveFile();	

	}

	/// note: 写两边 第一边整体保存 第二遍保存当前编辑之行动
	// 保存行动

	COTEActionXmlLoader::GetSingleton()->SaveAllActionData();

	// 保存角色内存中的行动
	
	HashMap<std::string, COTEAction*>::iterator it = 
		CActorEditorMainDlg::s_pActorEntity->m_ActionList.begin();	
	while(it !=	CActorEditorMainDlg::s_pActorEntity->m_ActionList.end())
	{
		COTEActionXmlLoader::GetSingleton()->SaveActionData(it->second);
		++ it;
	}

	// 保存完更新

	UpdateActionList();	

	m_ActList.SetCurSel(lastSel);	
	this->SendDlgItemMessage(IDC_ActList, LBN_DBLCLK);

	m_isEdited = false;
	CKeyFrameDlg::s_Inst.m_IsEdited = false;

	if(CKeyFrameDlg::s_Inst.m_hWnd)
		CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	
}

// -------------------------------------------
//添加特效
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
//使用现有轨迹
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
//改变特效播放速度
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
//创建轨迹
void CActionEdit::OnBnClickedCreatetrack()
{
	IS_EDITVALID{ return; }		

	int tItem = 0;
	if(m_MagList.GetItemCount() > 0)
		tItem = m_MagList.GetLastSelect();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	if(m_MagList.GetItemText(tItem, 1) != "无")
	{
		if(::MessageBox(NULL, "这个对象已经帮定了一个轨道，是否新建一个？", "提示", MB_YESNO) == IDYES)
			Createtrack(tItem);
	}
	else
	{
		Createtrack(tItem);
	}

}
// -------------------------------------------
//创建轨迹
int CActionEdit::Createtrack(int item, int trackId)
{
	if(COTETrackManager::GetInstance()->IsTrackPresent(trackId))
	{
		::MessageBox(NULL, "轨道创建出现重复ＩＤ!", "失败", MB_OK);
		return -1;
	}

	int tItem = item;
	CString sText = m_MagList.GetItemText(tItem, 1);
	char fileName[32];	int id;
	sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

	// 恐怕要先删掉对象后添加效果

	if(!sText.IsEmpty() && sText != "无")
	{
		m_pAct->RemoveEffectData(id);
		MAGIC_MGR->_UpdateGrave();
	}	
	
	// 创建并添加轨道

	COTETrackManager::GetInstance()->CreateNewTrack(trackId, COTEActTrackUsage::GetTrackXmlLoader());
		
	OTE::ActMagicData_t *mag = OTE::ActMagicData_t::CreateActMagic();	
	strcpy(mag->MagicFile, fileName);

	m_pAct->AddEffectData(mag, trackId);	

	// 播放速度
	OnEnKillfocusMagspeed();

	UpdateEffect();

	// 创建完直接编辑轨道

	sText = m_MagList.GetItemText(tItem, 1);
	if(!sText.IsEmpty() && sText != "无")
	{
		if(!CKeyFrameDlg::s_Inst.m_hWnd)
			CKeyFrameDlg::s_Inst.Create(IDD_KEYFRAMEDLG_DIALOG, this);
		CKeyFrameDlg::s_Inst.SetWindowText(CString("关键桢编辑器 - ") + sText);
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
		// 编辑轨道
		EditTrack();
	}
}

// -------------------------------------------
//编辑轨迹
void CActionEdit::EditTrack()
{
	IS_EDITVALID{ return; }		

	int tItem = m_MagList.GetSelectionMark();

	if(tItem > m_MagList.GetItemCount() || tItem < 0)
		return;

	CString sText = m_MagList.GetItemText(tItem, 1);
	if(!sText.IsEmpty() && sText != "无")
	{
		EditTrack(atoi(sText));
	}

}

// -------------------------------------------
//自动选择对应轨道的效果列表中项

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
	txt.Format("关键桢编辑器 - %d", trackID);

	CKeyFrameDlg::s_Inst.SetWindowText(txt);
	CKeyFrameDlg::s_Inst.ShowWindow(SW_SHOW);

	//if(CKeyFrameDlg::s_Inst.m_TrackID == trackID)
	{
		CKeyFrameDlg::s_Inst.UpdateData(trackID);
		
		CKeyFrameDlg::s_Inst.InvalidateRect(NULL);

	}	
}

// -------------------------------------------
//修改行动长度
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
		::MessageBox(NULL, "创建track失败！ 可能是行动命名 不是 [NUMBER][STRING]格式", "失败",	MB_OK);
		return false;
	}

	assert(m_MagList.GetItemCount() < 100);
	//interID = actID * 100 + m_MagList.GetItemCount();
	interID = actID * 100/* + item*/; // 按照当前选择的行号 (action ID) + (Item Index) * 100 的格式

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
	m_MagList.SetItemText(itemInd, 1, "无");
}

// -------------------------------------------
// 复制特效

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

	// 有轨道吗？

	if(id > 0)
	{
		OTE::COTETrack * selTrack = COTETrackManager::GetInstance()->GetTrack(id, COTEActTrackUsage::GetTrackXmlLoader());
		
		if(selTrack)
		{ 
			// 找出创建特效所在的item
			int index = 0;
			for(; index < m_MagList.GetItemCount(); index ++)
			{
				char fileName1[32];	int id1;
				sscanf((char*)LPCTSTR(m_MagList.GetItemText(index, 0)), "%s %d", fileName1, &id1);

				CString tt = m_MagList.GetItemText(index, 1);
				if(tt == "无" && strcmp(fileName1, fileName) == 0)
				{
					break;
				}				
			}
			
			// 自己组织trackid, 根据美术命名规范！			

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
				// 描述
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
				// 播放时刻

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
				// 播放速度

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
						// 显示轨道绑定的骨骼
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
// 添加声音
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
// 从其他action导入track列表
void CActionEdit::ImportMagList()
{	
	m_isEdited = true;

	CActionListDlg dlg;
	dlg.DoModal();

	UpdateActionList();
}


// -------------------------------------------
// 改变行动长度
void CActionEdit::OnEnChangeTime()
{
	this->SetActionTime();
	
}

// -------------------------------------------
// 骨胳动画
void CActionEdit::OnCbnSelchangeBoneanimcombo()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !m_pAct)	
		return;	
	
	CString tLength;
	CString tText;
	m_AniListCombo.GetWindowText(tText);

	tLength.Format("%1.3f", 2.0f);	
	if(!tText.IsEmpty() && tText != "无")
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
		if(!tText.IsEmpty() && tText != "无")
		{
			m_AniListCombo.SetWindowText("无");
		}
	}

	SetDlgItemText(IDC_Time, tLength);	
}


// -------------------------------------------
//改变行动中动画的播放速度
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
// 修改行动名
void CActionEdit::OnBnClickedChangeActionName()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !this->m_pAct)
		return;	

	// 修改已有的
	
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
// 绑定反应器
void CActionEdit::OnBnClickedButton1()
{
	CReactorLibDlg dlg;
	dlg.DoModal();
	
}

// -------------------------------------------
//创建反应器
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
//创建反应器
int CActionEdit::CreateReactor(int item, int reactorId, const std::string& rResName)
{
	// 先清理

	int tItem = item;
	CString sText = m_MagList.GetItemText(tItem, 1);
	char fileName[32];	int id;
	sscanf((char*)LPCTSTR(m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);

	if(!sText.IsEmpty() && sText != "无")
	{
		m_pAct->RemoveEffectData(id);
		MAGIC_MGR->_UpdateGrave();
	}	

	// 创建

	OTE::ActMagicData_t *mag = ActMagicData_t::CreateActMagic(ActMagicData_t::eReactor);
	strcpy(mag->MagicFile, fileName);		  // magic文件名
	strcpy(mag->InterData, rResName.c_str()); // 反应器类名
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

	// 点选骨胳

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

	// 选择物件

	if(m_hWnd && IsWindowVisible() && (::GetKeyState(VK_CONTROL) & 0x80))
	{
		// 点选
		Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay((float)x, (float)y);		

		// 点选特效

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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CActionEdit::OnBnClickedIsautoplay()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CActionEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		// 更新action列表

		UpdateActionList();
	}
}

void CActionEdit::OnBnClickedIsloop()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CActionEdit::OnCbnSelendokBoneanimcombo()
{
	
}

void CActionEdit::OnCbnKillfocusBoneanimcombo()
{
	OnCbnSelchangeBoneanimcombo();
}
