// GrassEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "GrassEditorDlg.h"
#include "OTERenderLight.h"
#include "OTETile.h"
#include "OTEVegManager.h"

#include "OTEQTSceneManager.h"
#include "grasseditordlg.h"

#include "OgreNoMemoryMacros.h"
#include "texture.h"
#include "OgreMemoryMacros.h"
#include "OTECommon.h"
#include ".\grasseditordlg.h"

// -------------------------------------------------
using namespace Ogre;
using namespace OTE;

// -------------------------------------------------
CGrassEditorDlg CGrassEditorDlg::s_Inst;

// CGrassEditorDlg 对话框

IMPLEMENT_DYNAMIC(CGrassEditorDlg, CDialog)
CGrassEditorDlg::CGrassEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrassEditorDlg::IDD, pParent)
{
	m_IsBrush = 1;
	m_VegBrush.m_GrassID = 0;
}

CGrassEditorDlg::~CGrassEditorDlg()
{
}

void CGrassEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREPIC, m_PrePic);
}


BEGIN_MESSAGE_MAP(CGrassEditorDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_STN_CLICKED(IDC_PREPIC, OnStnClickedPrepic)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
END_MESSAGE_MAP()


// CGrassEditorDlg 消息处理程序

void CGrassEditorDlg::OnBnClickedButton2()
{
	m_IsBrush = 0;
}

// ------------------------------------------------
void CGrassEditorDlg::OnMouseMove(float x, float y, float width, float height)
{	
	if(!COTETilePage::GetCurrentPage())
		return;

	static float oldMPX = x, oldMPY = y;
		
	if(!(::GetKeyState(VK_SHIFT) & 0x80))
	{
		Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay(x / width, y / height);
		Ogre::Vector3 pos;
		if(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
		{
			m_VegBrush.UpdateDrawing(pos);
		}

		if(::GetKeyState(VK_LBUTTON) & 0x80)
		{
			static Ogre::Vector2 s_oldPos;
			if((s_oldPos - Ogre::Vector2(x, y)).length() < 0.5)
				return;
			s_oldPos = Ogre::Vector2(x, y);	

			if(m_VegBrush.m_Strength == 0)
			{				
				unsigned char grassVal = ((m_VegBrush.m_GrassID % 64) << 2)/*草的索引*/ | 0 /*随即种子参数*/;
				
				COTETile* tile = COTETilePage::GetCurrentPage()->GetTileAt(m_VegBrush.m_Pos.x, m_VegBrush.m_Pos.z);
				if(tile)
				{				
					COTEVegManager::GetInstance()->SetVegAt(
						(std::vector<SingleGrassVertex>*)tile->m_pGrassList, NULL, NULL,
						m_VegBrush.m_Pos.x, m_VegBrush.m_Pos.z,
						grassVal * m_IsBrush
						);			
				}

			}
			else if(abs(rand()) % 256 <= m_VegBrush.m_Strength)
			{	
				unsigned char grassVal = ((m_VegBrush.m_GrassID % 64) << 2)/*草的索引*/ | ((abs(rand()) + 1) % 4)/*随即种子参数*/;
				COTETile* tile = COTETilePage::GetCurrentPage()->GetTileAt(m_VegBrush.m_Pos.x, m_VegBrush.m_Pos.z);
				if(tile)
				{				
					COTEVegManager::GetInstance()->SetVegAt(
						(std::vector<SingleGrassVertex>*)tile->m_pGrassList, NULL, NULL,
						m_VegBrush.m_Pos.x, m_VegBrush.m_Pos.z,
						grassVal * m_IsBrush
						);
				}
			}
		}
	}
	
	oldMPX = x; oldMPY = y;
	
}

// ------------------------------------------------
void CGrassEditorDlg::OnEnChangeEdit1()
{
	CString str;
	this->GetDlgItemText(IDC_EDIT1, str);
	if(!str.IsEmpty())
	{
		m_VegBrush.m_GrassID = atof(str.GetString());

		if(!m_GrassPreTex.GetData())
		{		
			std::string path, fullpath;
			OTEHelper::GetFullPath(COTEVegManager::GetInstance()->GetGrassTextureName(), path, fullpath);

			m_GrassPreTex.ReadFileTGA(fullpath.c_str());
		}

		int index = m_VegBrush.m_GrassID - 1;
		if(index < 32)
			m_GrassPreTex.Draw(m_PrePic.GetDC(), 0, 0, 128, 128, (index % 8) * 128, 1024 - (index / 8 + 1) * 128);
		else
			m_GrassPreTex.Draw(m_PrePic.GetDC(), 0, 0, 128, 128, ((index - 32) % 4) * 256, 1024 - ((index - 32) / 4 + 3) * 256);
	}
}

// ------------------------------------------------
void CGrassEditorDlg::OnBnClickedButton1()
{
	m_IsBrush = 1;
}

// ------------------------------------------------
BOOL CGrassEditorDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_EDIT1, "0");
	SetDlgItemText(IDC_EDIT2, COTEVegManager::GetInstance()->GetGrassTextureName().c_str());

	return ret;
}

// ------------------------------------------------
void CGrassEditorDlg::OnStnClickedPrepic()
{	

}

void CGrassEditorDlg::OnBnClickedButton3()
{
	CString str;
	this->GetDlgItemText(IDC_EDIT1, str);
	int ind = atoi(str);
	if(ind > 40)
		return;
	
	str.Format("%d", ind + 1);
	this->SetDlgItemText(IDC_EDIT1, str);
}

void CGrassEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	int index = m_VegBrush.m_GrassID - 1;
	if(index < 32)
	{
		m_GrassPreTex.Draw(
			m_PrePic.GetDC(),
			0, 0, 128, 128,
			(index % 8) * 128, 
			1024 - (index / 8 + 1) * 128);
	}
	else
	{
		m_GrassPreTex.Draw(
			m_PrePic.GetDC(),
			0, 0, 128, 128,
			((index - 32) % 4) * 256, 
			1024 - ((index - 32) / 4 + 3) * 256);
	}	
	CDialog::OnPaint();
}

void CGrassEditorDlg::OnBnClickedButton4()
{
	CString str;
	this->GetDlgItemText(IDC_EDIT1, str);
	int ind = atoi(str);
	if(ind <= 1)
		return;

	str.Format("%d", ind - 1);
	this->SetDlgItemText(IDC_EDIT1, str);

}

// 一级
void CGrassEditorDlg::OnBnClickedRadio1()
{
	m_VegBrush.m_Strength = 0;
}

// 二级
void CGrassEditorDlg::OnBnClickedRadio2()
{
	m_VegBrush.m_Strength = 128;
}

// 三级
void CGrassEditorDlg::OnBnClickedRadio3()
{
	m_VegBrush.m_Strength = 255;
}

// ----------------------------------------
// 选择区域草贴图
void CGrassEditorDlg::OnBnClickedButton5()
{
	CWinApp* pApp = AfxGetApp(); 
	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "选择区域草贴图");	

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "草贴图 (*.tga)|*.tga|All Files (*.*)|*.*||",
		 NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "选择区域草贴图", strPath);	

		COTEVegManager::GetInstance()->SetGrassTexture(tpFileDialog->GetFileName().GetString());
		SetDlgItemText(IDC_EDIT2, COTEVegManager::GetInstance()->GetGrassTextureName().c_str());
		
		std::string path, fullpath;
		OTEHelper::GetFullPath(COTEVegManager::GetInstance()->GetGrassTextureName(), path, fullpath);

		m_GrassPreTex.ReadFileTGA(fullpath.c_str());
	}
	
	delete tpFileDialog; 
}

// ----------------------------------------
// 保存
void CGrassEditorDlg::OnBnClickedButton8()
{
	std::string path, fullpath;
	OTEHelper::GetFullPath("grassdirhelper.ote", path, fullpath);

	std::stringstream ss;
	ss << path << "\\" << COTETilePage::GetCurrentPage()->m_PageName << "_veg.oraw";
	COTEVegManager::GetInstance()->SaveToFile(ss.str());
}

// ----------------------------------------
// 加载 
void CGrassEditorDlg::OnBnClickedButton9()
{
	std::string path, fullpath;
	OTEHelper::GetFullPath("grassdirhelper.ote", path, fullpath);

	std::stringstream ss;
	ss << path << "\\" << COTETilePage::GetCurrentPage()->m_PageName << "_veg.oraw";

	if(CHECK_RES(ss.str()))
		COTEVegManager::GetInstance()->CreateFromFile(ss.str());
}
