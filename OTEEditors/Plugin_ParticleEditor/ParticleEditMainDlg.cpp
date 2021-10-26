// ParticleEditMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ParticleEditor.h"
#include "ParticleEditMainDlg.h"
#include "ParticleCrtDlg.h"
#include "ParticleEditDlg.h"
#include "EffDoc.h"
#include "OTEQTSceneManager.h"
#include ".\particleeditmaindlg.h"

// CParticleEditMainDlg 对话框
CParticleEditMainDlg	CParticleEditMainDlg::s_Inst;
IMPLEMENT_DYNAMIC(CParticleEditMainDlg, CDialog)
CParticleEditMainDlg::CParticleEditMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParticleEditMainDlg::IDD, pParent)
{
}

CParticleEditMainDlg::~CParticleEditMainDlg()
{
}

void CParticleEditMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CParticleEditMainDlg, CDialog)	
	ON_BN_CLICKED(IDC_Open, OnBnClickedOpen)
	ON_BN_CLICKED(IDC_Close, OnBnClickedClose)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_WM_SETFOCUS()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CParticleEditMainDlg 消息处理程序

void CParticleEditMainDlg::OnBnClickedSave()
{
	if(!CEffDoc::s_Inst.m_ParticleSystem)
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
		
	CWinApp* pApp = AfxGetApp();   

	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "保存粒子特效");	
 
	CFileDialog* tpFileDialog = new CFileDialog(false, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"贴图文件(*.xml)|*.xml|All Files (*.*)|*.*|",
			NULL 
			); 

	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();	

	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
																		tpFileDialog->GetFileName().GetLength());   
		pApp->WriteProfileString("OTE文件路径配置", "保存粒子特效", strPath);

		CString fileName = tpFileDialog->GetPathName().GetString();
		if(fileName.Find(".xml") == -1)
			fileName += ".xml";

		OTE::COTEParticleSystemManager::GetInstance()->WriteXml(
							CEffDoc::s_Inst.m_ParticleSystem->GetResPtr()->m_Name, 
							fileName.GetString());	
						
	}
	delete tpFileDialog;
			
}

// -----------------------------------------
void CParticleEditMainDlg::OnBnClickedOpen()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
		
	CWinApp* pApp = AfxGetApp();   

	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "打开一个粒子特效");
	
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"粒子特效文件(*.xml)|*.xml|All Files (*.*)|*.*|",						
		NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();	

	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
																		tpFileDialog->GetFileName().GetLength());   
		pApp->WriteProfileString("OTE文件路径配置", "打开一个粒子特效", strPath);

		CEffDoc::s_Inst.m_ParticleSystemTempName = tpFileDialog->GetFileName().GetString();
		OTE::COTEParticleSystemManager::GetInstance()->RemoveParticleSystem("Test");
		CEffDoc::s_Inst.m_ParticleSystem = OTE::COTEParticleSystemManager::GetInstance()->CreateParticleSystem(
																	"Test", CEffDoc::s_Inst.m_ParticleSystemTempName, true);
		SCENE_MGR->AttachObjToSceneNode(CEffDoc::s_Inst.m_ParticleSystem);
							
	}
	delete tpFileDialog;	

}

void CParticleEditMainDlg::OnBnClickedClose()
{
	SCENE_MGR->RemoveEntity("emitter_helper");
	OTE::COTEParticleSystemManager::GetInstance()->RemoveParticleSystem("Test");
	CEffDoc::s_Inst.m_ParticleSystem = NULL;
}

// -----------------------------------------
void CParticleEditMainDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{	
	CParticleCrtDlg::s_Inst.ShowWindow(SW_HIDE);
	CParticleEditDlg::s_Inst.ShowWindow(SW_HIDE);

	switch(m_TabCtrl.GetCurSel())
	{
		case 0:
			CParticleCrtDlg::s_Inst.ShowWindow(SW_SHOW);			
			break;	

		case 1:
			CParticleEditDlg::s_Inst.ShowWindow(SW_SHOW);
			CParticleEditDlg::s_Inst.UpdatePropertyList();
			break;	
	}

}

// -----------------------------------------
BOOL CParticleEditMainDlg::OnInitDialog()
{
	bool ret = CDialog::OnInitDialog();

	m_TabCtrl.InsertItem(0, "创建");
	m_TabCtrl.InsertItem(1, "编辑");

	CParticleCrtDlg::s_Inst.Create (IDD_PARTICLE_EDIT_CRTDLG,  this);	
	CParticleEditDlg::s_Inst.Create(IDD_PARTICLE_EDIT_EDITDLG, this);	

	CRect tRect;
	GetClientRect(tRect);	

	CParticleCrtDlg::s_Inst.MoveWindow (tRect.left + 10, tRect.top + 64, tRect.right - 10, tRect.bottom - 40);
	CParticleEditDlg::s_Inst.MoveWindow(tRect.left + 10, tRect.top + 64, tRect.right - 10, tRect.bottom - 40);
	
	CParticleCrtDlg::s_Inst.ShowWindow(SW_SHOW);

	//打开
	((CButton*)GetDlgItem(IDC_Open))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP3)));

	//保存
	((CButton*)GetDlgItem(IDC_Save))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP1)));  

	//关闭
	((CButton*)GetDlgItem(IDC_Close))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP2)));

	return ret;
}


void CParticleEditMainDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);	
}

void CParticleEditMainDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if( nState == WA_ACTIVE && 
		CParticleEditDlg::s_Inst.m_hWnd &&
		CParticleEditDlg::s_Inst.IsWindowVisible())
	{
		CParticleEditDlg::s_Inst.Invalidate();
	}
}
