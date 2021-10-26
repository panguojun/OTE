// AddEffEntityDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "AddEffEntityDlg.h"
#include "ActionEdit.h"
#include "OTEMagicManager.h"
#include "OTEReactorManager.h"
#include ".\addeffentitydlg.h"

using namespace Ogre;
using namespace OTE;

// -----------------------------------------------
IMPLEMENT_DYNAMIC(CAddEffEntityDlg, CDialog)
CAddEffEntityDlg::CAddEffEntityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEffEntityDlg::IDD, pParent)
{
}

CAddEffEntityDlg::~CAddEffEntityDlg()
{
}

void CAddEffEntityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_SubTypeList);
	DDX_Control(pDX, IDC_COMBO1, m_TypeCombo);
	DDX_Control(pDX, IDC_COMBO2, m_ReactorTypeListCombo);
	DDX_Control(pDX, IDC_LIST3, m_ReactorSubTypeList);
}


BEGIN_MESSAGE_MAP(CAddEffEntityDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)

	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
	ON_LBN_DBLCLK(IDC_LIST2, OnLbnDblclkList2)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_CBN_DROPDOWN(IDC_COMBO1, OnCbnDropdownCombo1)	
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeCombo2)
	ON_LBN_SELCHANGE(IDC_LIST3, OnLbnSelchangeList3)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()
	
// -------------------------------------------
BOOL CAddEffEntityDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	// ����

	HashMap<std::string, CMagicFactoryBase*>::iterator it = MAGIC_MGR->m_MagFactoryList.begin();
	while(it != MAGIC_MGR->m_MagFactoryList.end())
	{
		if(it->first != "PS" && it->first != "OTEEntity")
		{
			std::string desc;
			it->second->GetDescription(desc);
			m_TypeCombo.AddString((it->first + " " + desc).c_str());	
			
		}

		++ it;
	}

	//// ��Ӧ��

	//HashMap<std::string, CReactorFactoryBase*>::iterator ita = COTEReactorManager::GetSingleton()->m_ReactorFactoryList.begin();
	//while(ita != COTEReactorManager::GetSingleton()->m_ReactorFactoryList.end())
	//{
	//	std::string desc;
	//	ita->second->GetDescription(desc);
	//	m_ReactorTypeListCombo.AddString((ita->first + " " + desc).c_str());		
	//	++ ita;
	//}

	return ret;
}

// -------------------------------------------
// ���ʵ��

void CAddEffEntityDlg::OnBnClickedButton1()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	   
	CWinApp* pApp = AfxGetApp();   

    static CString path;
	path = pApp->GetProfileString("OTE�ļ�·������", "���ʵ��");
	
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,		
		"ģ���ļ�(*.mesh)|*.mesh|ʵ���ļ�(*.obj)|*.obj|All Files (*.*)|*.*|",						

		NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();	

	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		 CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		 tpFileDialog->GetFileName().GetLength());   
		 pApp->WriteProfileString("OTE�ļ�·������", "���ʵ��", strPath);

		std::string file = tpFileDialog->GetFileName();
		SetDlgItemText(IDC_EDIT1, file.c_str());
	}
	delete tpFileDialog;

	// ���� ��Ӧ��

	m_ReactorTypeListCombo.ResetContent();
	HashMap<std::string, CReactorFactoryBase*>::iterator ita = COTEReactorManager::GetSingleton()->m_ReactorFactoryList.begin();
	while(ita != COTEReactorManager::GetSingleton()->m_ReactorFactoryList.end())
	{
		if(ita->second->ValidBind("OTEEntity") && ita->first != "EMT.Bas")
		{
			std::string desc;
			ita->second->GetDescription(desc);
			m_ReactorTypeListCombo.AddString((ita->first + " " + desc).c_str());					
		}
		++ ita;
	}

}

// -------------------------------------------
// �������

void CAddEffEntityDlg::OnBnClickedButton2()
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

	// ���� ��Ӧ��

	m_ReactorTypeListCombo.ResetContent();
	HashMap<std::string, CReactorFactoryBase*>::iterator ita = COTEReactorManager::GetSingleton()->m_ReactorFactoryList.begin();
	while(ita != COTEReactorManager::GetSingleton()->m_ReactorFactoryList.end())
	{
		if(ita->second->ValidBind("PS") && ita->first != "EMT.Bas")
		{
			std::string desc;
			ita->second->GetDescription(desc);
			m_ReactorTypeListCombo.AddString((ita->first + " " + desc).c_str());					
		}
		++ ita;
	}
}

// -------------------------------------------
// ��ɫ����
void CAddEffEntityDlg::OnBnClickedButton4()
{
	SetDlgItemText(IDC_EDIT1, "__self_.obj");	
	m_ReactorTypeListCombo.ResetContent();
}

// -------------------------------------------
// �����
void CAddEffEntityDlg::OnBnClickedButton8()
{	

}

// -------------------------------------------
void CAddEffEntityDlg::OnCbnSelchangeCombo1()
{
	CString factoryName;
	m_TypeCombo.GetWindowText(factoryName);
	m_SubTypeList.ResetContent();
		
	char facName[32];
	sscanf((char*)LPCTSTR(factoryName), "%s", facName);

	CMagicFactoryBase* mf = MAGIC_MGR->m_MagFactoryList[facName];
	if(mf)
	{
		std::list<std::string> subTypeList;
		mf->GetTypeList(subTypeList);

		std::list<std::string> subTypeDescList;
		mf->GetTypeDescList(subTypeDescList);

		std::list<std::string>::iterator it = subTypeList.begin();
		std::list<std::string>::iterator descIt = subTypeDescList.begin();
		while(it != subTypeList.end())
		{			
			m_SubTypeList.AddString(((*it) + " " + (*descIt)).c_str());
			++ it;
			++ descIt;
		}

		// ���� ��Ӧ��

		m_ReactorTypeListCombo.ResetContent();
		HashMap<std::string, CReactorFactoryBase*>::iterator ita = COTEReactorManager::GetSingleton()->m_ReactorFactoryList.begin();
		while(ita != COTEReactorManager::GetSingleton()->m_ReactorFactoryList.end())
		{
			if(ita->second->ValidBind(facName) && ita->first != "EMT.Bas")
			{
				std::string desc;
				ita->second->GetDescription(desc);
				m_ReactorTypeListCombo.AddString((ita->first + " " + desc).c_str());					
			}
			++ ita;
		}

	}	
	SetDlgItemText(IDC_EDIT1, CString(".") + facName);
}

// -------------------------------------------
void CAddEffEntityDlg::OnLbnSelchangeList2()
{
	CString name;
	if(m_SubTypeList.GetCurSel() >= 0)
		m_SubTypeList.GetText(m_SubTypeList.GetCurSel(), name);
	if(name.IsEmpty())
		m_TypeCombo.GetWindowText(name);

	char tName[32];
	sscanf((char*)LPCTSTR(name), "%s", tName);	

	SetDlgItemText(IDC_EDIT1, CString(".") + tName);
}

// -------------------------------------------
// ȷ������

void CAddEffEntityDlg::OnLbnDblclkList2()
{

}

// -------------------------------------------
// ���
void CAddEffEntityDlg::OnBnClickedButton6()
{
	CString objName, reactorName;
	GetDlgItemText(IDC_EDIT1, objName);
	GetDlgItemText(IDC_EDIT2, reactorName);
	if(!objName.IsEmpty() || !reactorName.IsEmpty())
	{
		if(!reactorName.IsEmpty())
			CActionEdit::s_Inst.AddEffectMagic((objName + "@" + reactorName).GetString()); 
		else
				CActionEdit::s_Inst.AddEffectMagic(objName.GetString()); 
		OnOK();
	}
	else
	{
		OTE_MSG("����Ϊ��!", "��ʾ")
	}
}

// -------------------------------------------
void CAddEffEntityDlg::OnCbnDropdownCombo1()
{
	
}

// -------------------------------------------
void CAddEffEntityDlg::OnCbnSelchangeCombo2()
{
	// ��Ӧ��

	CString factoryName;
	m_ReactorTypeListCombo.GetWindowText(factoryName);
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

	SetDlgItemText(IDC_EDIT2, facName);
}

// -------------------------------------------
void CAddEffEntityDlg::OnLbnSelchangeList3()
{
	// ��Ӧ��

	CString name;
	if(m_ReactorSubTypeList.GetCurSel() >= 0)
		m_ReactorSubTypeList.GetText(m_ReactorSubTypeList.GetCurSel(), name);
	if(name.IsEmpty())
		m_ReactorTypeListCombo.GetWindowText(name);

	// todo
	SetDlgItemText(IDC_EDIT2, name);
}
void CAddEffEntityDlg::OnBnClickedButton3()
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
		SetDlgItemText(IDC_EDIT2, file.c_str());	

	}
}
