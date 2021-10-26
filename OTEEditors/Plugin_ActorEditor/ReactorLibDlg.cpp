#include "stdafx.h"
#include "ReactorLibDlg.h"
#include "ActionEdit.h"
#include "OTEReactorManager.h"
#include "OTEMagicManager.h"

using namespace Ogre;
using namespace OTE;

// ----------------------------------------

IMPLEMENT_DYNAMIC(CReactorLibDlg, CDialog)
CReactorLibDlg::CReactorLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReactorLibDlg::IDD, pParent)
{
}

CReactorLibDlg::~CReactorLibDlg()
{
}

void CReactorLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_ReactorFactoryCombo);
	DDX_Control(pDX, IDC_LIST3, m_ReactorSubTypeList);
}

BEGIN_MESSAGE_MAP(CReactorLibDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_LBN_SELCHANGE(IDC_LIST3, OnLbnSelchangeList3)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeCombo2)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// ----------------------------------------
BOOL CReactorLibDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	//// ��Ӧ��

	//HashMap<std::string, CReactorFactoryBase*>::iterator ita = COTEReactorManager::GetSingleton()->m_ReactorFactoryList.begin();
	//while(ita != COTEReactorManager::GetSingleton()->m_ReactorFactoryList.end())
	//{		
	//	std::string desc;
	//	ita->second->GetDescription(desc);
	//	m_ReactorFactoryCombo.AddString((ita->first + " " + desc).c_str());	

	//	++ ita;
	//}

	return ret;
}

// ----------------------------------------
void CReactorLibDlg::OnBnClickedButton2()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CWinApp* pApp = AfxGetApp();   
	static CString path;

	path = pApp->GetProfileString("OTE�ļ�·������", "�������");

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,		
		"�����ļ�(*.xml)|*.xml|All Files (*.*)|*.*|",
		NULL 
		); 

	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();		

	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE�ļ�·������", "�������", strPath);

		std::string file = tpFileDialog->GetFileName();	
	
		SetDlgItemText(IDC_EDIT1, file.c_str());
	}
}

// ----------------------------------------
void CReactorLibDlg::OnBnClickedButton3()
{

}

// ----------------------------------------
// ɽ��߶�
void CReactorLibDlg::OnBnClickedButton6()
{
	
	CActionEdit::s_Inst.UpdateEffect();	

	OnOK();	
}

void CReactorLibDlg::OnLbnSelchangeList3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

// ----------------------------------------
void CReactorLibDlg::OnCbnSelchangeCombo2()
{
	// ��Ӧ��

	CString factoryName;
	m_ReactorFactoryCombo.GetWindowText(factoryName);
	m_ReactorSubTypeList.ResetContent();
	char facName[32];
	sscanf((char*)LPCTSTR(factoryName), "%s", facName);

	CReactorFactoryBase* f = COTEReactorManager::GetSingleton()->m_ReactorFactoryList[facName];
	if(f)
	{
		std::list<std::string> subTypeList;
		f->GetTypeList(subTypeList);		

		std::list<std::string>::iterator it = subTypeList.begin();
		while(it != subTypeList.end())
		{
			m_ReactorSubTypeList.AddString((*it).c_str());
			++ it;
		}
	}

	SetDlgItemText(IDC_EDIT1, factoryName);

}

// ----------------------------------------
// ���

void CReactorLibDlg::OnBnClickedButton1()
{
	CString txt;

	GetDlgItemText(IDC_EDIT1, txt);

	if(!txt.IsEmpty())
	{	
		char facName[32];
		sscanf((char*)LPCTSTR(txt), "%s", facName);

		CActionEdit::s_Inst.CreateReactor(facName);
		CActionEdit::s_Inst.UpdateEffect();	
		OnOK();
	}
	else
	{
		OTE_MSG("����Ϊ��!", "��ʾ")
	}
}

// ------------------------------------------
void CReactorLibDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{		
		int tItem = CActionEdit::s_Inst.m_MagList.GetLastSelect();
		if(tItem < 0)
			return;

		CString sText = CActionEdit::s_Inst.m_MagList.GetItemText(tItem, 1);
		char fileName[32];	int id;
		sscanf((char*)LPCTSTR(CActionEdit::s_Inst.m_MagList.GetItemText(tItem, 0)), "%s %d", fileName, &id);
		std::string magType = COTEMagicManager::GetMagType(std::string(fileName));

		// ��Ӧ��

		m_ReactorFactoryCombo.ResetContent();
		HashMap<std::string, CReactorFactoryBase*>::iterator ita = COTEReactorManager::GetSingleton()->m_ReactorFactoryList.begin();
		while(ita != COTEReactorManager::GetSingleton()->m_ReactorFactoryList.end())
		{		
			if(ita->second->ValidBind(magType) && ita->first != "EMT.Bas")
			{
				std::string desc;
				ita->second->GetDescription(desc);
				m_ReactorFactoryCombo.AddString((ita->first + " " + desc).c_str());	
			}

			++ ita;
		}
	}
}
