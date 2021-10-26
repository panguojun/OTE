// NewSceneDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "NewSceneDlg.h"
#include "newscenedlg.h"
#include "otetile.h"
#include "OTETerrainSerializer.h"
#include "OTEQTSceneManager.h"
#include "OTETilePage.h"
#include ".\newscenedlg.h"

// ----------------------------------------------
using namespace Ogre;
using namespace OTE;

// ----------------------------------------------

TileCreateData_t					g_SceneCreateData;

// ----------------------------------------------
// CNewSceneDlg 对话框

IMPLEMENT_DYNAMIC(CNewSceneDlg, CDialog)
CNewSceneDlg::CNewSceneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewSceneDlg::IDD, pParent)
{
}

CNewSceneDlg::~CNewSceneDlg()
{
}

void CNewSceneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNewSceneDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

// -----------------------------------------------
// 初始化

BOOL CNewSceneDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	this->SetDlgItemText(IDC_EDIT8, "default.jpg");
	this->SetDlgItemText(IDC_EDIT4, "default.jpg");
	this->SetDlgItemText(IDC_EDIT9, "default.jpg");

	return ret;
}

// -----------------------------------------------
// 场景名称更改

void CNewSceneDlg::OnEnChangeEdit1()
{	
	CString str;
	this->GetDlgItemText(IDC_EDIT1, str);
	g_SceneCreateData.name = str.GetString();

	this->SetDlgItemText(IDC_EDIT5, str + "_hm.raw");
	this->SetDlgItemText(IDC_EDIT6, str + "_uv.raw");
	this->SetDlgItemText(IDC_EDIT7, str + "_nm.raw");

}
// -----------------------------------------------
// 建立新地图
void CNewSceneDlg::OnBnClickedOk()
{
	CString str;	
	this->GetDlgItemText(IDC_EDIT3, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.layerTexes[0] = str.GetString();

	this->GetDlgItemText(IDC_EDIT8, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.layerTexes[1] = str.GetString();

	this->GetDlgItemText(IDC_EDIT4, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.layerTexes[2] = str.GetString();

	this->GetDlgItemText(IDC_EDIT9, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.layerTexes[3] = str.GetString();

	this->GetDlgItemText(IDC_EDIT5, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.hgtmapName = str.GetString();

	this->GetDlgItemText(IDC_EDIT6, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.uvInfoName = str.GetString();

	this->GetDlgItemText(IDC_EDIT7, str);
	if(str.IsEmpty())
		return;

	g_SceneCreateData.normalName = str.GetString();
	
	this->GetDlgItemText(IDC_COMBO1, str);
	if(str.IsEmpty())
		return;

	sscanf(str.GetString(), "%d X %d", &g_SceneCreateData.pageSizeX, &g_SceneCreateData.pageSizeZ);
	if(g_SceneCreateData.pageSizeX == 0 || g_SceneCreateData.pageSizeZ == 0)
		return;

	COTETerrainXmlSerializer::GetSingleton()->CreateNewTerrainPage(
		g_SceneCreateData
		);

	OnOK();

}

// -----------------------------------------------
void CNewSceneDlg::OnBnClickedButton1()
{
	
}

void CNewSceneDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}
