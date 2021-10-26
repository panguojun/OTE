// ParticleCrtDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ParticleEditor.h"
#include "ParticleCrtDlg.h"

#include "Ogre.h"
#include "OTEParticleSystem.h"
#include "OTEQTSceneManager.h"

#include "EffDoc.h"
#include "particlecrtdlg.h"

// CParticleCrtDlg 对话框

CParticleCrtDlg CParticleCrtDlg::s_Inst;

IMPLEMENT_DYNAMIC(CParticleCrtDlg, CDialog)
CParticleCrtDlg::CParticleCrtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParticleCrtDlg::IDD, pParent)
{	
}

CParticleCrtDlg::~CParticleCrtDlg()
{
}

void CParticleCrtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_TempListBox);
}


BEGIN_MESSAGE_MAP(CParticleCrtDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
END_MESSAGE_MAP()


BOOL CParticleCrtDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	UpdataTemplate();
		
	return ret;
}
// ---------------------------------------
// 创建

void CParticleCrtDlg::OnBnClickedButton2()
{	
	
}

void CParticleCrtDlg::OnBnClickedButton1()
{
}

// -----------------------------------------
// 选择一个模板创建

void CParticleCrtDlg::OnLbnSelchangeList1()
{
	CString text;
	m_TempListBox.GetText(m_TempListBox.GetCurSel(), text);
	CEffDoc::s_Inst.m_ParticleSystemTempName = text.GetString();

	OTE::COTEParticleSystemManager::GetInstance()->RemoveParticleSystem("Test");
	CEffDoc::s_Inst.m_ParticleSystem = OTE::COTEParticleSystemManager::GetInstance()->CreateParticleSystem(
		"Test", CEffDoc::s_Inst.m_ParticleSystemTempName, true);

	SCENE_MGR->AttachObjToSceneNode(CEffDoc::s_Inst.m_ParticleSystem);

	OTE::COTEParticleSystemRes* resPtr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();
	OTE::COTESimpleEmitter* emitter = (OTE::COTESimpleEmitter*)resPtr->m_Emitter;
	emitter->Show(CEffDoc::s_Inst.m_ParticleSystem);

}

// -----------------------------------------
//更新资源列表
void CParticleCrtDlg::UpdataTemplate()
{
	m_TempListBox.ResetContent();

	char resPath[256];
	GetPrivateProfileString("ResConfig","ParticlePath","\0",
									resPath,sizeof(resPath),".\\OTESettings.cfg");
	int index = 0;

	// 搜索文件夹
	CFileFind finder;
	BOOL bWorking = finder.FindFile((CString(resPath) + "\\*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(!finder.IsDirectory())
		{			
			if(finder.GetFileName().Find(".xml") != -1)
			{
               m_TempListBox.InsertString(index ++, finder.GetFileName());
			}
		}
	}
	finder.Close();

}
