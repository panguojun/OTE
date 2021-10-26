// FogEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "FogEditDlg.h"
#include "OTEFog.h"

#include "OTEQTSceneManager.h"
#include "OTECommon.h"
#include "OTESky.h"
#include ".\fogeditdlg.h"

// ------------------------------------------------
using namespace Ogre;
using namespace OTE;
// ------------------------------------------------

CFogEditDlg	CFogEditDlg::s_Inst;

// CFogEditDlg 对话框

IMPLEMENT_DYNAMIC(CFogEditDlg, CDialog)
CFogEditDlg::CFogEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFogEditDlg::IDD, pParent)
{
}

CFogEditDlg::~CFogEditDlg()
{
}

void CFogEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_FogSliderNear);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderFogFar);
	DDX_Control(pDX, IDC_CHECK1, m_CheckBox);
}


BEGIN_MESSAGE_MAP(CFogEditDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, OnEnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON10, OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton11)
	ON_EN_CHANGE(IDC_EDIT7, OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, OnEnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT9, OnEnChangeEdit9)
END_MESSAGE_MAP()

// ------------------------------------------------
void CFogEditDlg::ShowFogColor()
{
	// 显示雾颜色

	char buff[32];
	sprintf(buff, "%.2f", OTE::COTEFog::GetInstance()->m_ForColour.r);
	SetDlgItemText(IDC_EDIT7, buff);
	sprintf(buff, "%.2f", OTE::COTEFog::GetInstance()->m_ForColour.g);
	SetDlgItemText(IDC_EDIT8, buff);
	sprintf(buff, "%.2f", OTE::COTEFog::GetInstance()->m_ForColour.b);
	SetDlgItemText(IDC_EDIT9, buff);
}

// ------------------------------------------------
void CFogEditDlg::UpdateFogColor(unsigned int id)
{
	_MAPPING	(int,		float*	);
	_ADDMAPPING	(IDC_EDIT7, &COTEFog::GetInstance()->m_ForColour.r);
	_ADDMAPPING	(IDC_EDIT8, &COTEFog::GetInstance()->m_ForColour.g);
	_ADDMAPPING	(IDC_EDIT9, &COTEFog::GetInstance()->m_ForColour.b);

	const int __IDC_EDIT	=	id;
	
	CString value;
	GetDlgItemText(__IDC_EDIT, value);

	*(_GETMAPPING(__IDC_EDIT)) = atof((char*)LPCTSTR(value));
}

// ------------------------------------------------
BOOL CFogEditDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();
	
	m_FogSliderNear.SetRangeMin(0.0f);
	m_FogSliderNear.SetRangeMax(300.0f);
	m_FogSliderNear.SetPos(COTEFog::GetInstance()->m_FogStart);

	m_SliderFogFar.SetRangeMin(0.0f);
	m_SliderFogFar.SetRangeMax(500.0f);
	m_SliderFogFar.SetPos(COTEFog::GetInstance()->m_FogEnd);

	m_CheckBox.SetCheck(BST_CHECKED);

	ShowFogColor();

	return tRet;
}

// ------------------------------------------------
// 选择雾的颜色

void CFogEditDlg::OnBnClickedButton1()
{
	Ogre::ColourValue& ambient = COTEFog::GetInstance()->m_ForColour;
	COLORREF cor = RGB(ambient.r * 255, ambient.g * 255, ambient.b * 255);
	
	CColorDialog color(cor, CC_FULLOPEN );

	if(color.DoModal() == IDOK)
	{	
		OTE::COTEFog::GetInstance()->m_ForColour.r = GetRValue(color.GetColor()) / 255.0f;
		OTE::COTEFog::GetInstance()->m_ForColour.g = GetGValue(color.GetColor()) / 255.0f;
		OTE::COTEFog::GetInstance()->m_ForColour.b = GetBValue(color.GetColor()) / 255.0f;

		OTE::COTEFog::GetInstance()->UpdateFogState();

		ShowFogColor();
	}

}

// ------------------------------------------------
void CFogEditDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int value = m_FogSliderNear.GetPos();
	OTE::COTEFog::GetInstance()->m_FogStart = value;	

	COTEFog::GetInstance()->UpdateFogState();

}
// ------------------------------------------------
void CFogEditDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
// ------------------------------------------------
void CFogEditDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
// ------------------------------------------------
void CFogEditDlg::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int value = m_SliderFogFar.GetPos();
	OTE::COTEFog::GetInstance()->m_FogEnd = value;	

	COTEFog::GetInstance()->UpdateFogState();
}
// ------------------------------------------------
void CFogEditDlg::OnBnClickedCheck1()
{
	COTEFog::GetInstance()->SetFogEnable(m_CheckBox.GetCheck() == BST_CHECKED);
}

// ------------------------------------------------
// 更换天空贴图
// ------------------------------------------------

void CFogEditDlg::OnEnChangeEdit1()
{
	CString str;
	GetDlgItemText(IDC_EDIT1, str);
	OTE::COTESky::GetInstance()->SetSkyTextures(str.GetString());
}

void CFogEditDlg::OnEnChangeEdit3()
{
	CString str;
	GetDlgItemText(IDC_EDIT3, str);
	OTE::COTESky::GetInstance()->SetSkyTextures("", str.GetString());	
}

void CFogEditDlg::OnEnChangeEdit4()
{
	CString str;
	GetDlgItemText(IDC_EDIT4, str);
	OTE::COTESky::GetInstance()->SetSkyTextures("", "", str.GetString());	
}

void CFogEditDlg::OnEnChangeEdit5()
{
	CString str;
	GetDlgItemText(IDC_EDIT5, str);
	OTE::COTESky::GetInstance()->SetSkyTextures("", "", "", str.GetString());	
}

// ------------------------------------------------
// 旋转速度

void CFogEditDlg::OnEnChangeEdit2()
{
	CString str;
	GetDlgItemText(IDC_EDIT2, str);

	OTE::COTESky::GetInstance()->SetSkyCloudRollSpeed(atof((char*)LPCTSTR(str)));
}

// ------------------------------------------------
void CFogEditDlg::OnBnClickedButton3()
{
	CWinApp* pApp = AfxGetApp(); 
	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "选择天空背景贴图");	

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "贴图 (*.dds)|*.dds|All Files (*.*)|*.*||",
		 NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "选择天空背景贴图", strPath);	
		
		SetDlgItemText(IDC_EDIT1, tpFileDialog->GetFileName().GetString());
	}
	
	delete tpFileDialog; 
}
// ------------------------------------------------
void CFogEditDlg::OnBnClickedButton4()
{
	CWinApp* pApp = AfxGetApp(); 
	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "选择天空背景贴图");	

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "贴图 (*.dds)|*.dds|All Files (*.*)|*.*||",
		 NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "选择天空背景贴图", strPath);	
		
		SetDlgItemText(IDC_EDIT3, tpFileDialog->GetFileName().GetString());
	}
	
	delete tpFileDialog; 
}
// ------------------------------------------------
void CFogEditDlg::OnBnClickedButton5()
{
	CWinApp* pApp = AfxGetApp(); 
	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "选择天空背景贴图");	

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "贴图 (*.dds)|*.dds|All Files (*.*)|*.*||",
		 NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "选择天空背景贴图", strPath);	
		
		SetDlgItemText(IDC_EDIT4, tpFileDialog->GetFileName().GetString());
	}
	
	delete tpFileDialog; 
}
// ------------------------------------------------
void CFogEditDlg::OnBnClickedButton10()
{
	CWinApp* pApp = AfxGetApp(); 
	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "选择天空背景贴图");	

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "贴图 (*.dds)|*.dds|All Files (*.*)|*.*||",
		 NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "选择天空背景贴图", strPath);	
		
		SetDlgItemText(IDC_EDIT5, tpFileDialog->GetFileName().GetString());
	}
	
	delete tpFileDialog; 
}

// ------------------------------------------------
// 保存

void CFogEditDlg::OnBnClickedButton2()
{
	std::string path, fullpath;
	OTEHelper::GetFullPath("skydirhelper.ote", path, fullpath);

	std::stringstream ss;
	ss << path << "\\" << (COTETilePage::GetCurrentPage() ? COTETilePage::GetCurrentPage()->m_PageName : "Default") << "_sky.oraw";

	COTESkyMgr::GetInstance()->SaveToFile(ss.str());

	// 雾文本

	ss.str("");
	ss << path << "\\" << (COTETilePage::GetCurrentPage() ? COTETilePage::GetCurrentPage()->m_PageName : "Default") << "_fog.txt";

	COTEFog::GetInstance()->SaveToFile(ss.str());

	// 雾二进制

	ss.str("");
	ss << path << "\\" << (COTETilePage::GetCurrentPage() ? COTETilePage::GetCurrentPage()->m_PageName : "Default") << "_fog.oraw";

	COTEFog::GetInstance()->SaveSettingsToFile(ss.str());
}

// ------------------------------------------------
// 加载

void CFogEditDlg::OnBnClickedButton11()
{
	std::string path, fullpath;
	OTEHelper::GetFullPath("skydirhelper.ote", path, fullpath);

	std::stringstream ss;
	ss << path << "\\" << COTETilePage::GetCurrentPage()->m_PageName << "_sky.oraw";

	if(CHECK_RES(ss.str()))
		COTESkyMgr::GetInstance()->CreateFromFile(ss.str());

	/// 加载雾

	// 文本

	ss.str("");
	ss << path << "\\" << COTETilePage::GetCurrentPage()->m_PageName << "_fog.txt";
	COTEFog::GetInstance()->CreateFromFile(ss.str());

	// 二进制

	ss.str("");
	ss << path << "\\" << COTETilePage::GetCurrentPage()->m_PageName << "_fog.oraw";
	COTEFog::GetInstance()->SetFromFile(ss.str());


}
// ------------------------------------------------
void CFogEditDlg::OnEnChangeEdit7()
{
	UpdateFogColor(IDC_EDIT7);
}
// ------------------------------------------------
void CFogEditDlg::OnEnChangeEdit8()
{
	UpdateFogColor(IDC_EDIT8);
}
// ------------------------------------------------
void CFogEditDlg::OnEnChangeEdit9()
{
	UpdateFogColor(IDC_EDIT9);
}
