#include "stdafx.h"
#include "OTECommon.h"
#include "SoundDlg.h"
#include "ActionEdit.h"
#include "OTEfmodsystem.h"

// ----------------------------------
using namespace Ogre;
using namespace OTE;

// ----------------------------------
// CSoundDlg 对话框
CSoundDlg CSoundDlg::s_Inst;
IMPLEMENT_DYNAMIC(CSoundDlg, CDialog)
CSoundDlg::CSoundDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundDlg::IDD, pParent)
{
}

CSoundDlg::~CSoundDlg()
{
}

void CSoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, m_SoundListBox);
	DDX_Control(pDX, IDC_CHECK1, m_CheckIsLoop);
}


BEGIN_MESSAGE_MAP(CSoundDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()

// ----------------------------------------
void CSoundDlg::OnBnClickedButton2()
{
	CSimpleBKSoundPlayer::StopSound();

	OnCancel();
}

// ----------------------------------------
void CSoundDlg::OnBnClickedButton3()
{
	CSimpleBKSoundPlayer::StopSound();
	CString soundName;
	m_SoundListBox.GetText(m_SoundListBox.GetCurSel(), soundName);	

	CActionEdit::s_Inst.m_pAct->SetSound( soundName.GetString(), m_CheckIsLoop.GetCheck() == BST_CHECKED );	
	
	OnOK();
}

// ----------------------------------------------
BOOL CSoundDlg::OnInitDialog()
{
	int ret = CDialog::OnInitDialog();

	return ret;
}

// ----------------------------------------------
// 迪归调用搜索文件目录

void AllDirScanfFiles(const CString& strPathName, const CString& strFileName, std::list<std::string>& rFileList)
{ 	
	// 搜索文件夹
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPathName + "/*.*");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDirectory() && !finder.IsDots())
		{
			AllDirScanfFiles(strPathName + "/" + finder.GetFileName(), strFileName, rFileList);					

		}	
	}
	finder.Close();

	// 搜索文件		
	BOOL bFound = finder.FindFile(CString(strPathName) + "/" + strFileName);
	while (bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDirectory() && !finder.IsDots())
		{
			rFileList.push_back((LPCTSTR)(/*strPathName + "/" + */finder.GetFileName()));
		}		
	}				
	finder.Close();	
}

// ----------------------------------------
// 初始化声音列表

void CSoundDlg::InitSoundList()
{		
	static std::string strDownloadDir = OTEHelper::GetResPath("ResConfig", "SoundPath");

	m_SoundList.clear();

	std::list<std::string> tFileList;

	AllDirScanfFiles(strDownloadDir.c_str(), "*.*", tFileList);
	
	std::list<std::string>::iterator  it = tFileList.begin();
	while(it != tFileList.end())
	{
		std::string sss = (*it);

		m_SoundList.push_back(sss);
		
		++ it;
	}
	UpdateSoundList();
}

// ----------------------------------------
void CSoundDlg::UpdateSoundList()
{
	std::list<std::string>::iterator it = m_SoundList.begin();
	m_SoundListBox.ResetContent();
	while(it != m_SoundList.end())
	{
		m_SoundListBox.AddString((*it).c_str());
		++ it;
	}
}


// ----------------------------------------
void CSoundDlg::OnLbnSelchangeList1()
{
	CString soundName;
	m_SoundListBox.GetText(m_SoundListBox.GetCurSel(), soundName);	

	
	CSimpleBKSoundPlayer::PlaySoundA(soundName.GetString(), 1.0f);

}

// ----------------------------------------
// 双击
void CSoundDlg::OnLbnDblclkList1()
{
	CString soundName;
	m_SoundListBox.GetText(m_SoundListBox.GetCurSel(), soundName);	

	if(CActionEdit::s_Inst.m_pAct)
	{
		CActionEdit::s_Inst.m_pAct->SetSound(soundName.GetString(), m_CheckIsLoop.GetCheck() == BST_CHECKED );

	}
	this->ShowWindow(false);
}

// ----------------------------------------
void CSoundDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);	

	if(!CActionEdit::s_Inst.m_pAct)
		return;

	// 声音列表
	InitSoundList();

	for(int i = 0; i < m_SoundListBox.GetCount(); i ++)
	{
		CString txt;
		m_SoundListBox.GetText(i, txt);
		if(txt.GetString() == CActionEdit::s_Inst.m_pAct->m_3DSoundName)
		{
			m_SoundListBox.SetCurSel(i);
			break;
		}
	}

	m_CheckIsLoop.SetCheck(CActionEdit::s_Inst.m_pAct->m_Is3DSoundLoop ? BST_CHECKED : BST_UNCHECKED);

}

// ----------------------------------------
void CSoundDlg::OnClose()
{
	CSimpleBKSoundPlayer::StopSound();

	CDialog::OnClose();
}

void CSoundDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
}
