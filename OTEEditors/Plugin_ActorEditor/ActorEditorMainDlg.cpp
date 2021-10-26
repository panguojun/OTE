// ActorEditorMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "ActorEditorMainDlg.h"
#include "Ogre.h" 
#include "MeshEditDlg.h"
#include "AnimationEdit.h"
#include "HookEditDlg.h"
#include "BrowserBone.h"
#include "ActionEdit.h"
#include "TrackSelect.h"
#include "KeyFrameDlg.h"
#include "OTEObjDataManager.h"

// --------------------------------------------------------
using namespace Ogre;
using namespace OTE;
 
CTransformManager			CActorEditorMainDlg::s_TransMgr;			// 变换管理器
OTE::COTEActorEntity*		CActorEditorMainDlg::s_pActorEntity = NULL;
bool						CActorEditorMainDlg::s_StartSelection = false;

// CActorEditorMainDlg 对话框
CActorEditorMainDlg		CActorEditorMainDlg::s_Inst; 

IMPLEMENT_DYNAMIC(CActorEditorMainDlg, CDialog)
CActorEditorMainDlg::CActorEditorMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActorEditorMainDlg::IDD, pParent)	
{	
}

CActorEditorMainDlg::~CActorEditorMainDlg()
{
}

void CActorEditorMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CActorEditorMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON10, OnBnClickedButton10)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_Open, OnBnClickedOpen)
	ON_BN_CLICKED(IDC_Close, OnBnClickedClose)
	ON_COMMAND(ID__32793, OnCpyMag)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_COMMAND(ID__32795, OnDeleteEffect)
	ON_COMMAND(ID__32798, OnDelAction)
	ON_COMMAND(ID_Menu32786, OnEditEffect)
	//ON_COMMAND(ID__32794, OnCopyAction)
	ON_COMMAND(ID__32801, OnImportMagList)
END_MESSAGE_MAP()

// --------------------------------------------------------
//初始化对话框
BOOL CActorEditorMainDlg::OnInitDialog()
{
	//初始化光源
	/*SCENE_MGR->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));

	Ogre::Light *lm = SCENE_MGR->getLight("MainLight");
	if(lm == NULL)
	{
		lm = SCENE_MGR->createLight("MainLight");
		lm->setType(Ogre::Light::LT_DIRECTIONAL);
		lm->setDiffuseColour(0.8, 0.8, 0.8);
		lm->setDirection(-1, -1, -1);
	}

	lm = SCENE_MGR->getLight("Light2");
	if(lm == NULL)
	{
		lm = SCENE_MGR->createLight("Light2");
		lm->setType(Ogre::Light::LT_DIRECTIONAL);
		lm->setDiffuseColour(0.4, 0.4, 0.4);
		lm->setDirection(1, 1, 1);
	}
    */
	bool tRet = CDialog::OnInitDialog();

	m_TabCtrl.InsertItem(0, "模型");
	m_TabCtrl.InsertItem(1, "动画");
	m_TabCtrl.InsertItem(2, "挂点");
	m_TabCtrl.InsertItem(3, "行动");

	if(!CMeshEditDlg::s_Inst.m_hWnd)
		CMeshEditDlg::s_Inst.Create(IDD_MeshEdit, this);
	CMeshEditDlg::s_Inst.ShowWindow(SW_SHOW);

	if(!CAnimationEdit::s_Inst.m_hWnd)
		CAnimationEdit::s_Inst.Create(IDD_AnimationEdit, this);
	CAnimationEdit::s_Inst.ShowWindow(SW_HIDE);

	if(!CHookEditDlg::s_Inst.m_hWnd)
		CHookEditDlg::s_Inst.Create(IDD_HookEdit, this);
	CHookEditDlg::s_Inst.ShowWindow(SW_HIDE);

	if(!CBrowserBone::s_Inst.m_hWnd)
		CBrowserBone::s_Inst.Create(IDD_Bone, this);
	CBrowserBone::s_Inst.ShowWindow(SW_HIDE);

	if(!CTrackSelect::s_Inst.m_hWnd)
			CTrackSelect::s_Inst.Create(IDD_TrackDlg, this);
	CTrackSelect::s_Inst.ShowWindow(SW_HIDE);

	if(!CActionEdit::s_Inst.m_hWnd)
		CActionEdit::s_Inst.Create(IDD_ActionEdit, this);
	CActionEdit::s_Inst.ShowWindow(SW_HIDE);

	if(!CKeyFrameDlg::s_Inst.m_hWnd)
		CKeyFrameDlg::s_Inst.Create(IDD_KEYFRAMEDLG_DIALOG, this);
	CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	
	CRect tRect;
	GetClientRect(tRect);	
	CMeshEditDlg::s_Inst.MoveWindow(tRect.left + 11, tRect.top + 65, tRect.right - 24, tRect.top + 585);
	CAnimationEdit::s_Inst.MoveWindow(tRect.left + 11, tRect.top + 65, tRect.right - 24, tRect.top + 585);
	CHookEditDlg::s_Inst.MoveWindow(tRect.left + 11, tRect.top + 65, tRect.right - 24, tRect.top + 585);
	CActionEdit::s_Inst.MoveWindow(tRect.left + 11, tRect.top + 65, tRect.right - 24, tRect.top + 585);

	//打开
	((CButton*)GetDlgItem(IDC_Open))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP3)));

	//保存
	((CButton*)GetDlgItem(IDC_Save))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP1)));  

	//关闭
	((CButton*)GetDlgItem(IDC_Close))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP2)));
	//移动
	((CButton*)GetDlgItem(IDC_BUTTON3))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP5))); 
	//旋转
	((CButton*)GetDlgItem(IDC_BUTTON4))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP6))); 
	//缩放
	((CButton*)GetDlgItem(IDC_BUTTON5))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP7))); 
	//点选
	((CButton*)GetDlgItem(IDC_BUTTON9))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP4))); 
	//退出
	((CButton*)GetDlgItem(IDC_BUTTON10))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP8))); 

	return tRet;
}

// --------------------------------------------------------
void CActorEditorMainDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(CMeshEditDlg::s_Inst.m_hWnd)	CMeshEditDlg::s_Inst.ShowWindow(SW_HIDE);
	if(CAnimationEdit::s_Inst.m_hWnd) CAnimationEdit::s_Inst.ShowWindow(SW_HIDE);
	if(CHookEditDlg::s_Inst.m_hWnd) CHookEditDlg::s_Inst.ShowWindow(SW_HIDE);	
	if(CBrowserBone::s_Inst.m_hWnd) CBrowserBone::s_Inst.ShowWindow(SW_HIDE);
	if(CTrackSelect::s_Inst.m_hWnd) CTrackSelect::s_Inst.ShowWindow(SW_HIDE);
	if(CKeyFrameDlg::s_Inst.m_hWnd) CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	if(CActionEdit::s_Inst.m_hWnd) CActionEdit::s_Inst.ShowWindow(SW_HIDE);
	if(CHookEditDlg::s_Inst.m_hWnd) CHookEditDlg::s_Inst.mHook = NULL;
	if(CAnimationEdit::s_Inst.m_hWnd) CAnimationEdit::s_Inst.OnBnClickedStop();

	s_TransMgr.HideAllDummies();

	switch(m_TabCtrl.GetCurSel())
	{
		case 0://Mesh
			CMeshEditDlg::s_Inst.ShowWindow(SW_SHOW);
			//CMeshEditDlg::s_Inst.UpdateList();
			break;
		case 1://Animation
			CAnimationEdit::s_Inst.ShowWindow(SW_SHOW);
			//CAnimationEdit::s_Inst.UpdataAnimation();
			break;
		case 2://PotHook
			CHookEditDlg::s_Inst.ShowWindow(SW_SHOW);
			//CHookEditDlg::s_Inst.UpdateHookList();
			break;
		case 3://Action
			CActionEdit::s_Inst.ShowWindow(SW_SHOW);
			//CActionEdit::s_Inst.UpdateActionList();
			break;
	}	
}

// --------------------------------------------------------
//退出按钮
void CActorEditorMainDlg::OnBnClickedButton10()
{
	OnOK();
}

// --------------------------------------------------------
//加载角色
void CActorEditorMainDlg::LoadEntity(const char* File, const char* name)
{
	//加载角色
	if(s_pActorEntity != NULL)
	{
		SCENE_MGR->RemoveEntity(s_pActorEntity->getName());
		s_pActorEntity = NULL;
	}
		
	if(strlen(File) > 0 && strlen(name) > 0)
	{
		if(SCENE_MGR->IsEntityPresent(name))
		{
			SCENE_MGR->RemoveEntity(name);
		}

		s_pActorEntity = SCENE_MGR->CreateEntity(File, name);
		WAIT_LOADING_RES(s_pActorEntity)

		SCENE_MGR->SetCurFocusObj(s_pActorEntity);		
	}

	UpdateUI();

}

// --------------------------------------------------------
// 更新
void CActorEditorMainDlg::UpdateUI()
{
	if(CMeshEditDlg::s_Inst.m_hWnd)
		CMeshEditDlg::s_Inst.UpdateList();
	if(CAnimationEdit::s_Inst.m_hWnd)
		CAnimationEdit::s_Inst.UpdataAnimation();
	if(CHookEditDlg::s_Inst.m_hWnd)
		CHookEditDlg::s_Inst.UpdateHookList();
	if(CActionEdit::s_Inst.m_hWnd)
		CActionEdit::s_Inst.UpdateActionList();
	if(CBrowserBone::s_Inst.m_hWnd)
	{
		CBrowserBone::s_Inst.ShowWindow(SW_HIDE);
		CBrowserBone::s_Inst.UpdataBoneList();
	}
	if(CTrackSelect::s_Inst.m_hWnd)
		CTrackSelect::s_Inst.ShowWindow(SW_HIDE);
	if(CKeyFrameDlg::s_Inst.m_hWnd)
		CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
	s_TransMgr.HideAllDummies();

}

// --------------------------------------------------------
//移动
void CActorEditorMainDlg::OnBnClickedButton3()
{
	s_TransMgr.SetEditType(CTransformManager::ePOSITION);
	if(CHookEditDlg::s_Inst.mHook != NULL)
	{
		s_TransMgr.ShowDummy(CTransformManager::ePOSITION, true);
		CHookEditDlg::s_Inst.UpdateDummyPos();
	}
	else if( (CKeyFrameDlg::s_Inst.m_bPush || CKeyFrameDlg::s_Inst.m_CurKey) &&
		CKeyFrameDlg::s_Inst.m_pTrack != NULL && CKeyFrameDlg::s_Inst.m_pMover != NULL)
	{
		s_TransMgr.ShowDummy(CTransformManager::ePOSITION, true);
		s_TransMgr.SetDummyPos(CKeyFrameDlg::s_Inst.m_pMover->getParentNode()->getWorldPosition());
	}
}
// --------------------------------------------------------
//旋转
void CActorEditorMainDlg::OnBnClickedButton4()
{
	s_TransMgr.SetEditType(CTransformManager::eROTATION);
	if(CHookEditDlg::s_Inst.mHook != NULL)
	{
		s_TransMgr.ShowDummy(CTransformManager::eROTATION, true);
		CHookEditDlg::s_Inst.UpdateDummyPos();
	}
	else if((CKeyFrameDlg::s_Inst.m_bPush || CKeyFrameDlg::s_Inst.m_CurKey) &&
		CKeyFrameDlg::s_Inst.m_pTrack != NULL && CKeyFrameDlg::s_Inst.m_pMover != NULL)
	{
		s_TransMgr.ShowDummy(CTransformManager::eROTATION, true);
		s_TransMgr.SetDummyPos(CKeyFrameDlg::s_Inst.m_pMover->getParentNode()->getWorldPosition());
	}
}
// --------------------------------------------------------
//缩放
void CActorEditorMainDlg::OnBnClickedButton5()
{
	s_TransMgr.SetEditType(CTransformManager::eSCALING);
	if(CHookEditDlg::s_Inst.mHook != NULL)
	{
		s_TransMgr.ShowDummy(CTransformManager::eSCALING, true);
		CHookEditDlg::s_Inst.UpdateDummyPos();
	}
	else if((CKeyFrameDlg::s_Inst.m_bPush || CKeyFrameDlg::s_Inst.m_CurKey) &&
		CKeyFrameDlg::s_Inst.m_pTrack != NULL && CKeyFrameDlg::s_Inst.m_pMover != NULL)
	{
		s_TransMgr.ShowDummy(CTransformManager::eSCALING, true);
		s_TransMgr.SetDummyPos(CKeyFrameDlg::s_Inst.m_pMover->getParentNode()->getWorldPosition());
	}
}
// --------------------------------------------------------
//点选
void CActorEditorMainDlg::OnBnClickedButton9()
{
	s_StartSelection = true;
	s_TransMgr.HideAllDummies();
}
// --------------------------------------------------------
//保存
void CActorEditorMainDlg::OnBnClickedButton2()
{	

}

// --------------------------------------------------------
//打开实体
void CActorEditorMainDlg::OnBnClickedOpen()
{
	CWinApp* pApp = AfxGetApp();   
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "打开角色");	

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	
		 "模型文件(*.obj)|*.obj|实体文件(*.mesh)|*.mesh|All Files (*.*)|*.*|",
		 NULL 
		); 	
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();	

	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{ 	 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "打开角色", strPath);	
	
		LoadEntity(tpFileDialog->GetFileName().GetString(), "Test");		

	}

	delete tpFileDialog; 
}
// --------------------------------------------------------
//关闭已经打开的实体
void CActorEditorMainDlg::OnBnClickedClose()
{
	LoadEntity();
}

void CActorEditorMainDlg::OnCpyMag()
{
	CActionEdit::s_Inst.OnCopyMag();
}

// --------------------------------------------------------
// 保存 
void CActorEditorMainDlg::OnBnClickedSave()
{
	if(s_pActorEntity)
	{	
		if(s_pActorEntity->m_ResFile[0] != '\0')
		{
			std::string resFile = s_pActorEntity->m_ResFile;
			resFile.replace(resFile.find(".mesh"), 5, ".obj"); 
			COTEObjDataManager::GetInstance()->WriteObjData(resFile, s_pActorEntity);
		}
	}
}

// --------------------------------------------------------
void CActorEditorMainDlg::OnDeleteEffect()
{
	CActionEdit::s_Inst.DelEffect();
}

// --------------------------------------------------------
void CActorEditorMainDlg::OnDelAction()
{
	CActionEdit::s_Inst.DelAction();
}

// --------------------------------------------------------
void CActorEditorMainDlg::OnEditEffect()
{
	CActionEdit::s_Inst.EditEffect();
}

// --------------------------------------------------------
void CActorEditorMainDlg::OnCopyAction()
{
	
}

// --------------------------------------------------------
void CActorEditorMainDlg::OnImportMagList()
{
	CActionEdit::s_Inst.ImportMagList();
}
