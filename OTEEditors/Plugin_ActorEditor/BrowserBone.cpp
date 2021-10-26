#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "BrowserBone.h"
#include "ActorEditorMainDlg.h"
#include "HookEditDlg.h"
#include "KeyFrameDlg.h"
#include "OTEActorHookMgr.h"
#include "ActionEdit.h"

using namespace OTE;
using namespace Ogre;

// ----------------------------------------------
CBrowserBone CBrowserBone::s_Inst;

// ----------------------------------------------
IMPLEMENT_DYNAMIC(CBrowserBone, CDialog)
CBrowserBone::CBrowserBone(CWnd* pParent /*=NULL*/)
	: CDialog(CBrowserBone::IDD, pParent)
{
	isAddHook = false;
	isAddTrack = false;
}

CBrowserBone::~CBrowserBone()
{
}

// ----------------------------------------------
void CBrowserBone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BoneList, BoneList);
}

// ----------------------------------------------
//获取骨骼树
void CBrowserBone::GetBoneTree(Ogre::Node::ChildNodeIterator& it)
{
	while (it.hasMoreElements())
	{	
		Ogre::Node* n = it.getNext();

		if(n->getName().find("Unnamed") != -1)
			continue;

		BoneList.AddString(n->getName().c_str());

		GetBoneTree(n->getChildIterator());
	}	
}
// ----------------------------------------------
//更新骨骼对话框
void CBrowserBone::UpdataBoneList()
{
	BoneList.ResetContent();
        
	GetDlgItem(IDOK)->EnableWindow(false);

	if(CActorEditorMainDlg::s_pActorEntity && CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
	{
		GetBoneTree(CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getRootBone()->getChildIterator());
	}

	BoneList.Invalidate();
}

BEGIN_MESSAGE_MAP(CBrowserBone, CDialog)
	ON_LBN_SELCHANGE(IDC_BoneList, OnLbnSelchangeBonelist)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

// ----------------------------------------------
// CBrowserBone 消息处理程序
void CBrowserBone::OnLbnSelchangeBonelist()
{
	CString sel;
	BoneList.GetText(BoneList.GetCurSel(), sel);
	if(!sel.IsEmpty())
	{
		Ogre::Bone *b = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getBone(sel.GetString());
		if(b != NULL)
		{
			CActorEditorMainDlg::s_TransMgr.SetDummyPos(b->getWorldPosition());
		}
		GetDlgItem(IDOK)->EnableWindow(true);
	}
}
// ----------------------------------------------
void CBrowserBone::OnBnClickedOk()
{
	CString sel;
	BoneList.GetText(BoneList.GetCurSel(), sel);

	if(!sel.IsEmpty())
	{
		if(isAddHook && CHookEditDlg::s_Inst.m_hWnd)
		{
			COTEActorHookMgr::AddHook(CActorEditorMainDlg::s_pActorEntity, 
					sel.GetString(), sel.GetString());
			CHookEditDlg::s_Inst.UpdateHookList();
			CHookEditDlg::s_Inst.Save();
		}
		else if(isAddTrack && CKeyFrameDlg::s_Inst.m_hWnd)
		{
			CKeyFrameDlg::s_Inst.m_pTrack->SetBindBone(sel.GetString());
		}
	}

	isAddHook = false;
	isAddTrack = false;

	if(CKeyFrameDlg::s_Inst.m_hWnd)
		CKeyFrameDlg::s_Inst.UpdateTrackPath();

	OnOK();
}
// ----------------------------------------------
void CBrowserBone::OnBnClickedCancel()
{
	isAddHook = false;
	isAddTrack = false;
	OnCancel();
}
// ----------------------------------------------
void CBrowserBone::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(!bShow)
	{
		isAddHook = false;
		isAddTrack = false;
	}
}
// ----------------------------------------------
void CBrowserBone::OnBnClickedButton1()
{
	if(CKeyFrameDlg::s_Inst.m_hWnd && 
		CKeyFrameDlg::s_Inst.IsWindowVisible())
	{
		CActionEdit::s_Inst.m_IsShowBones.SetCheck(BST_CHECKED);
		CActionEdit::s_Inst.m_WaitForPickBone = true;
	}

	if(CHookEditDlg::s_Inst.m_hWnd && 
		CHookEditDlg::s_Inst.IsWindowVisible())
	{
		CHookEditDlg::s_Inst.m_IsShowBone.SetCheck(BST_CHECKED);
		CHookEditDlg::s_Inst.m_WaitForPickBone = true;
	}

	OnOK();
}
