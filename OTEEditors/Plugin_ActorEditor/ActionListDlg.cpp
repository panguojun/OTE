// TrackSelect.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "ActionListDlg.h"
#include "tinyxml.h"
#include "ActionEdit.h"
#include "ActorEditorMainDlg.h"
#include "ActionEdit.h"
#include "OTEActorActionMgr.h"
#include "OTEActionXmlLoader.h"

using namespace Ogre;
using namespace OTE;

// -------------------------------------
//更新可用轨道列表
void CActionListDlg::UpdtatActionList()
{
	m_List.ResetContent();
	
	for(int i = 0; i < CActionEdit::s_Inst.m_ActList.GetCount(); i ++)
	{
		CString txt;
		CActionEdit::s_Inst.m_ActList.GetText(i, txt);
		m_List.AddString(txt); 
	}

	m_List.Invalidate();
}

IMPLEMENT_DYNAMIC(CActionListDlg, CDialog)
CActionListDlg::CActionListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActionListDlg::IDD, pParent)
{
}

CActionListDlg::~CActionListDlg()
{
}

void CActionListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ActList, m_List);
}


BEGIN_MESSAGE_MAP(CActionListDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_DBLCLK(IDC_ActList, OnLbnDblclkActlist)
END_MESSAGE_MAP()

// -------------------------------------
//确定按钮
void CActionListDlg::OnBnClickedOk()
{
	if(CActorEditorMainDlg::s_pActorEntity == NULL || 
		CActionEdit::s_Inst.m_pAct == NULL)
		return;
	
	CString sSel;
	m_List.GetText(m_List.GetCurSel(), sSel);
	if(!sSel.IsEmpty())
	{			
		COTEActionData* act = COTEActorActionMgr::GetAction(
			CActorEditorMainDlg::s_pActorEntity,
			(char*)LPCTSTR(sSel)
			);
		if(!act)
			act = OTE::COTEActionXmlLoader::GetSingleton()->GetActionData((char*)LPCTSTR(sSel));

		if(act)
		{
			OTE::COTEAction::CloneInterDatas(
				act, 
				CActionEdit::s_Inst.m_pAct
				);		}
	}

	OnOK();
}
// -------------------------------------
BOOL CActionListDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	UpdtatActionList();
		
	return ret;
}

// -------------------------------------
//取消按钮
void CActionListDlg::OnBnClickedCancel()
{	
	OnCancel();
}

// -------------------------------------
void CActionListDlg::OnBnClickedButton1()
{
	OnBnClickedOk();
	OnOK();
}

// -------------------------------------
void CActionListDlg::OnLbnDblclkActlist()
{
	OnBnClickedOk();
}
