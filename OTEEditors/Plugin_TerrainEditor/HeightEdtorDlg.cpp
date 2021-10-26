// HeightEdtorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "HeightEdtorDlg.h"
#include "OTEQTSceneManager.h"
#include "TerrainBrush.h"
#include "TransformManager.h"
#include "OTETile.h"
#include "OTETerrainHeightMgr.h"
#include "OTEEntityAutoLink.h"
#include "OTERenderLight.h"
#include "OTETerrainLightmapMgr.h"


// ----------------------------------------
using namespace OTE;
using namespace Ogre;

// ----------------------------------------
CHeightEditorDlg CHeightEditorDlg::s_Inst;

//地形高度历史记录
std::vector<OTE::TileCreateData_t> g_TerrainEditTrackList;

// ----------------------------------------
IMPLEMENT_DYNAMIC(CHeightEditorDlg, CDialog)
CHeightEditorDlg::CHeightEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHeightEditorDlg::IDD, pParent)
{	
}

CHeightEditorDlg::~CHeightEditorDlg()
{
}

void CHeightEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HeightText, m_txtHeight);
	DDX_Control(pDX, IDC_SLIDER2, m_InnerSizeSlider);
	DDX_Control(pDX, IDC_SLIDER1, m_OutSizeSlider);
	DDX_Control(pDX, IDC_SLIDER4, m_BrushStrengthSlider);
	//DDX_Control(pDX, IDC_txtSensitivity, m_txtSensitivity);
	DDX_Control(pDX, IDC_LIST1, m_EditorTypeCombo2);
	DDX_Control(pDX, IDC_EDIT1, m_BottomHgtEdit);
	DDX_Control(pDX, IDC_COMBO1, m_CliffCombo);
}

BEGIN_MESSAGE_MAP(CHeightEditorDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_LIST1, OnCbnSelchangeList1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER4, OnNMReleasedcaptureSlider4)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, OnNMCustomdrawSlider4)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
	ON_BN_CLICKED(IDC_BtnUpdateNorm, OnBnClickedBtnupdatenorm)
	ON_BN_CLICKED(IDC_BtnFlatness, OnBnClickedBtnflatness)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_EN_CHANGE(IDC_HeightText, OnEnChangeHeighttext)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// ------------------------------------------------

BOOL CHeightEditorDlg::OnInitDialog()
{
	BOOL tRet = CDialog::OnInitDialog();	

	m_EditorTypeCombo2.InsertString(0, "升高");
	m_EditorTypeCombo2.InsertString(1, "降低");
	m_EditorTypeCombo2.InsertString(2, "光滑");
	m_EditorTypeCombo2.InsertString(3, "平整");
	m_EditorTypeCombo2.InsertString(4, "顶点");
	m_EditorTypeCombo2.InsertString(5, "复制高度");
	m_EditorTypeCombo2.InsertString(6, "悬崖");

	m_OutSizeSlider.SetRange(1, 50);
	m_OutSizeSlider.SetTicFreq(1);
	m_InnerSizeSlider.SetRange(1, 50);
	m_InnerSizeSlider.SetTicFreq(1);
	m_BrushStrengthSlider.SetRange(1, 50);
	m_BrushStrengthSlider.SetTicFreq(1);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(true);

	// 悬崖列表

	std::string cliffPath = OTEHelper::GetResPath("ResConfig", "CliffPath");
	AllDirScanfCliffFiles(cliffPath);

	return tRet;
}

// ----------------------------------------------------------------
//搜索悬崖资源目录
void CHeightEditorDlg::AllDirScanfCliffFiles(std::string strPathName)
{
	// 搜索文件夹

	CFileFind finder;
	BOOL bWorking = finder.FindFile((strPathName + "\\*.*").c_str());
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDirectory())
		{
			if(finder.IsDots() || finder.IsHidden())
				continue;

			AllDirScanfCliffFiles(strPathName + "\\" + finder.GetFileName().GetString());
		}
		else
		{
			if( finder.GetFileName().Find(".obj") != -1 || 
				finder.GetFileName().Find(".mesh") != -1)
			{   
				std::string name = OTEHelper::ScanfStringAt(finder.GetFileName().GetString(), 1, '_');
				static std::string lastName;
				if(stricmp(lastName.c_str(), name.c_str()) != 0 && !name.empty())
				{
					m_CliffCombo.AddString(name.c_str());
					lastName = name;
				}

			}
		}
	}
	finder.Close();
}

// ------------------------------------------------
// 拉扯高度

Ogre::Vector3 pos(0, 0, 0);
Ogre::Ray ray;

void CHeightEditorDlg::OnLButtonDown(float x, float y)
{
	if(!COTETilePage::GetCurrentPage())
		return;

	COTETerrainHeightMgr::GetInstance()->m_LastEditTileList.clear();

	if(m_BrushEntity.m_HeightEditType == 6)
	{
		// 吸取悬崖底部高度

		CString bottomHgtTxt;
		m_BottomHgtEdit.GetWindowText(bottomHgtTxt);
		if(bottomHgtTxt == "正在吸取高度")
		{
			Ogre::Ray mouseRay = SCENE_CAM->getCameraToViewportRay(x, y);
			if(COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
			{
				float hgt = pos.y;
				COTEEntityAutoLink::GetInstance()->SetBottomHeight(hgt);
				char str[20];
				sprintf(str, "%f", hgt);
				m_BottomHgtEdit.SetWindowText(str);
			}
			return;
		}		
	}
	else if(m_BrushEntity.m_HeightEditType == 5)
	{
		if(::GetKeyState(VK_CONTROL) & 0x80)
		{
			Ogre::Ray mouseRay = SCENE_CAM->getCameraToViewportRay(x, y);

			if(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
			{
				float a = pos.y;
				char str[20];
				sprintf(str, "%f", a);
				m_txtHeight.SetWindowText(str);
			}
		}
	}

	switch(m_BrushEntity.m_HeightEditType)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		{
			OTE::TileCreateData_t data;			
			int size = (COTETilePage::GetCurrentPage()->m_PageSizeX + 1) * (COTETilePage::GetCurrentPage()->m_PageSizeZ + 1);
			//-------------------
			data.heightData = new unsigned char[size];
			data.normal = new Ogre::Vector3[size];
			COTETerrainHeightMgr::GetInstance()->getVertexHeightData(COTETilePage::GetCurrentPage(), &data);
			int i = g_TerrainEditTrackList.size();
			if( i > 6)
			{
				delete [] g_TerrainEditTrackList[0].heightData;
				delete [] g_TerrainEditTrackList[0].normal;
				g_TerrainEditTrackList.erase(g_TerrainEditTrackList.begin());	//移除列表中对象
			}
			g_TerrainEditTrackList.push_back(data);
		}
		break;
	}
}

// ------------------------------------------------
void CHeightEditorDlg::OnLButtonUp(float x, float y)
{
	if(::GetKeyState(VK_CONTROL) & 0x80)
		return;
	if(!COTETilePage::GetCurrentPage())
		return;

	if(m_BrushEntity.m_HeightEditType != 6) // 悬崖　　
	{
		// 法线

		//std::list<COTETile*>& tileList = COTETerrainHeightMgr::GetInstance()->m_LastEditTileList;
		//std::list<COTETile*>::iterator it = tileList.begin();
		//while(it != tileList.end())
		//{			
		//	(*it)->AverageVertexNormal();
		//	++ it;
		//}	
		//COTETerrainHeightMgr::GetInstance()->m_LastEditTileList.clear();

		// 光照图
		
		Ogre::AxisAlignedBox aabb;
		std::list<COTETile*>& tileList = COTETerrainHeightMgr::GetInstance()->m_LastEditTileList;
		std::list<COTETile*>::iterator it = tileList.begin();
		while(it != tileList.end())
		{			
			aabb.merge((*it)->getBoundingBox());
			++ it;
		}	
		COTETerrainHeightMgr::GetInstance()->m_LastEditTileList.clear();


		float offset = m_BrushEntity.m_OutterRad / 2.0f;

		Ogre::ColourValue ambientColor = COTERenderLight::GetInstance()->GetLightKey()->ambient.GetColor();
		
		Ogre::Vector3 mainLightDir = SCENE_MAINLIGHT->getDirection();			//	阳光方向
		Ogre::ColourValue mainLightCor = SCENE_MAINLIGHT->getDiffuseColour();	//	阳光色

		COTETerrainLightmapMgr::GetInstance()->GenLightmapOnTiles(
			COTETilePage::GetCurrentPage(), 
			aabb.getCenter().x,
			aabb.getCenter().z,
			std::max(aabb.getMaximum().x - aabb.getMinimum().x, aabb.getMaximum().z - aabb.getMinimum().z)
			* COTETilePage::GetLightmapScale(),
			mainLightDir, mainLightCor, 
			ambientColor);

	}
}

// ------------------------------------------------
void CHeightEditorDlg::OnKeyDown(unsigned int keyCode)
{	
	bool yes = false;
	switch(keyCode)
	{
	case '1'://[升高] 快捷键
		m_EditorTypeCombo2.SetCurSel(0);
		m_BrushEntity.m_HeightEditType = 0;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '2'://[降低] 快捷键
		m_EditorTypeCombo2.SetCurSel(1);
		m_BrushEntity.m_HeightEditType = 1;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '3'://[光滑] 快捷键
		m_EditorTypeCombo2.SetCurSel(2);
		m_BrushEntity.m_HeightEditType = 3;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '4'://[平整] 快捷键
		m_EditorTypeCombo2.SetCurSel(3);
		m_BrushEntity.m_HeightEditType = 2;
		//m_BrushEntity.m_fDragHeight = pos.y;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '5'://[顶点] 快捷键
		m_EditorTypeCombo2.SetCurSel(4);
		m_BrushEntity.m_HeightEditType = 4; 
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '6'://[复制高度] 快捷键
		m_EditorTypeCombo2.SetCurSel(5);
		m_BrushEntity.m_HeightEditType = 5; 
		yes = true;
		m_txtHeight.ShowWindow(true);
		break;
	case 'Z':
		{
			if(::GetKeyState(VK_CONTROL) & 0x80)
			{
				//撤消还原功能
				if(g_TerrainEditTrackList.size() != 0)
				{
					for(int i = g_TerrainEditTrackList.size(); i > 0; i--)
					{
						COTETerrainHeightMgr::GetInstance()->updateTerrainAllHeight(COTETilePage::GetCurrentPage(), &g_TerrainEditTrackList[i-1]);
						delete [] g_TerrainEditTrackList[i-1].heightData;
						delete [] g_TerrainEditTrackList[i-1].normal;

						g_TerrainEditTrackList.erase(g_TerrainEditTrackList.begin() + (i-1));	//移除列表中对象
						
						break;
					}
				}
			}
		}
		break;
	}
}

// ------------------------------------------------
void CHeightEditorDlg::OnMouseMove(float x, float y)
{	
	if(!COTETilePage::GetCurrentPage() || ::GetKeyState(VK_CONTROL) & 0x80 )
		return;

	static float oldMPX = x, oldMPY = y;
		
	if(!(::GetKeyState(VK_SHIFT) & 0x80))
	{
		ray = SCENE_CAM->getCameraToViewportRay(x, y);

		if(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
		{
			m_BrushEntity.UpdateDrawing(pos);
		}
	}

	if(pos == Ogre::Vector3(-1, -1, -1) || pos == Ogre::Vector3::ZERO)
		return;

	if(::GetKeyState(VK_LBUTTON) & 0x80)
	{
		// 悬崖

		if(m_BrushEntity.m_HeightEditType == 6)
		{
			DrawCliff(m_BrushEntity.m_Pos.x, m_BrushEntity.m_Pos.z);
		}
		// 对山体高度进行编辑

		else if(m_BrushEntity.m_HeightEditType == 4)
		{ // 单点操作
						
			if(oldMPY - y > 0)
				pos.y += 1.0f;
			else
				pos.y -= 1.0f;

			COTETerrainHeightMgr::GetInstance()->SetHeightAt(
				COTETilePage::GetCurrentPage(), 
				m_BrushEntity.m_Pos.x, 
				m_BrushEntity.m_Pos.z, 
				pos.y);	
		}
		else
		{ // 块操作
			float _Intensity = 0.0;
			
			if(m_BrushEntity.m_HeightEditType == 5)
			{
				CString strIntensity;
				m_txtHeight.GetWindowText(strIntensity);
				if(strIntensity == "0")
					return;

				_Intensity = atof(strIntensity.GetString());
			}
			else
			{
				_Intensity = m_BrushEntity.m_Intensity * m_BrushEntity.m_fDragHeight * 10.0f;
			}			

			COTETerrainHeightMgr::GetInstance()->SetHeightAt(
				COTETilePage::GetCurrentPage(), 
				m_BrushEntity.m_Pos.x, //x坐标
				m_BrushEntity.m_Pos.z, //z坐标
				m_BrushEntity.m_Strength * m_BrushEntity.m_fDragHeight, //y高度乘强度
				_Intensity,							//强度
				m_BrushEntity.m_InnerRad,			//内圈
				m_BrushEntity.m_OutterRad,			//外圈
				m_BrushEntity.m_HeightEditType);	//计算类型
			
			// 更新笔刷
			m_BrushEntity.m_fDragHeight = y - oldMPY;
		}		
	}

	oldMPX = x; oldMPY = y;
	
	std::stringstream ss;
	ss << "笔刷位置 x: " << m_BrushEntity.m_Pos.x << ", z: " << m_BrushEntity.m_Pos.z;
	SetDlgItemText(IDC_STATIC_MSG, ss.str().c_str()); 
}

// ------------------------------------------------------
void CHeightEditorDlg::OnCbnSelchangeList1()
{		
	m_txtHeight.ShowWindow(false);

	CString str;
	m_EditorTypeCombo2.GetText(m_EditorTypeCombo2.GetCurSel(), str);
	
	if(		str == "升高")
		m_BrushEntity.m_HeightEditType = 0;
	else if(str == "降低")
		m_BrushEntity.m_HeightEditType = 1;	
	else if(str == "平整")
		m_BrushEntity.m_HeightEditType = 2;
	else if(str == "光滑")
		m_BrushEntity.m_HeightEditType = 3;
	else if(str == "顶点")
		m_BrushEntity.m_HeightEditType = 4; 
	else if(str == "复制高度")
	{
		m_txtHeight.ShowWindow(true);
		m_BrushEntity.m_HeightEditType = 5;
	}
	else if(str == "悬崖")
	{
		m_BrushEntity.m_HeightEditType = 6;		
		
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(true);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(false);
	}

}

// ------------------------------------------------------
void CHeightEditorDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// ------------------------------------------------------
void CHeightEditorDlg::ResetBrushEntity()
{
	m_BrushEntity.DeleteBrushEntity();
}

// ------------------------------------------------------
// 外圈
int Circledifference = 0;
float RadScale = 36.0f;	//笔刷外圈放大比例值
void CHeightEditorDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_BrushEntity.m_OutterRad/*m_Intensity*/ = (/*1.0f - */(float)m_OutSizeSlider.GetPos()/m_OutSizeSlider.GetRangeMax() * RadScale);
	if(m_BrushEntity.m_HeightEditType == 5)
		m_BrushEntity.m_OutterRad /= 3;

	Circledifference = m_OutSizeSlider.GetPos() - m_InnerSizeSlider.GetPos();
}

// ------------------------------------------------------
// 内圈
void CHeightEditorDlg::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	float scale = 6.0f;
	
	m_BrushEntity.m_InnerRad = (float)m_InnerSizeSlider.GetPos()/m_InnerSizeSlider.GetRangeMax() * scale;
	if(m_BrushEntity.m_HeightEditType == 5)
		m_BrushEntity.m_InnerRad /= 3;
	
	int a = m_InnerSizeSlider.GetPos() + Circledifference;
	if(a <= m_OutSizeSlider.GetRangeMax())
		m_OutSizeSlider.SetPos(a);
	else
		m_OutSizeSlider.SetPos(m_OutSizeSlider.GetRangeMax());

	m_BrushEntity.m_OutterRad = (float)m_OutSizeSlider.GetPos() / m_OutSizeSlider.GetRangeMax() * RadScale;
	if(m_BrushEntity.m_HeightEditType == 5)
		m_BrushEntity.m_OutterRad /= 3;
}

// ------------------------------------------------------
// 强度
void CHeightEditorDlg::OnNMReleasedcaptureSlider4(NMHDR *pNMHDR, LRESULT *pResult)
{
	float scale = 15.0f;
	m_BrushEntity.m_Strength = 5.0f * (float)m_BrushStrengthSlider.GetPos()/ m_BrushStrengthSlider.GetRangeMax() * scale;
	m_BrushEntity.m_Intensity = m_BrushEntity.m_Strength;
}

// ------------------------------------------------------
void CHeightEditorDlg::OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// ------------------------------------------------------
void CHeightEditorDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// ------------------------------------------------------
void CHeightEditorDlg::OnBnClickedBtnupdatenorm()
{
	COTETilePage::GetCurrentPage()->AverageVertexNormal();
}


// ------------------------------------------------------
// 悬崖效果
// ------------------------------------------------------

void CHeightEditorDlg::DrawCliff(float x, float z)
{	
	unsigned int flag = 0;

	if(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED)
		flag |= AUTOLINK_UPDATETERRAIN;

	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == BST_CHECKED)
		flag |= AUTOLINK_OPER_ADD | AUTOLINK_RAISE;
	else if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == BST_CHECKED)
		flag |= AUTOLINK_RAISE;

	COTEEntityAutoLink::GetInstance()->AutoLink(x, z, flag);
}

// ------------------------------------------------------
void CHeightEditorDlg::OnBnClickedRadio1()
{	
}

// ------------------------------------------------------
void CHeightEditorDlg::OnLbnSelchangeList1()
{
	
}

void CHeightEditorDlg::OnBnClickedCheck3()
{
	
}

// ------------------------------------------------------
void CHeightEditorDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CHeightEditorDlg::OnEnChangeHeighttext()
{

}

void CHeightEditorDlg::OnBnClickedButton1()
{
	m_BottomHgtEdit.SetWindowText("正在吸取高度");
}

void CHeightEditorDlg::OnEnChangeEdit1()
{
	
}

// ------------------------------------------------------
void CHeightEditorDlg::OnCbnSelchangeCombo1()
{
	CString str;
	m_CliffCombo.GetWindowText(str);
	if(!str.IsEmpty())
	{
		LINK_STR("Ent_" << str.GetString(), entNamePre)
		COTEEntityAutoLink::GetInstance()->SetEntityGroupNamePrefix(entNamePre);
		COTEEntityAutoLink::GetInstance()->SetMeshGroupNamePrefix(str.GetString());
	}
}

void CHeightEditorDlg::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
}

// ------------------------------------------------------
void CHeightEditorDlg::OnBnClickedBtnflatness()
{
	CString str = "";
	m_txtSensitivity.GetWindowText(str);
	if(str.Trim().GetLength() == 0 || str.Trim().GetLength() > 1)
	{
		::MessageBox(NULL, "敏感度值不正确！", "提示", MB_OK);
		return;
	}
	char * a = str.GetBuffer();
	char cSensitivity = a[0];
	if(cSensitivity < 48 || cSensitivity > 57)
	{
		::MessageBox(NULL, "敏感度值不正确！", "提示", MB_OK);
		return;
	}

	int answer = ::MessageBox(NULL, "确认处理高度图吗?", "提示",
								MB_YESNO);
	if (answer == IDYES)
	{
		COTETilePage::GetCurrentPage()->heightMapDeleteDirt(atoi(&cSensitivity));
		//::MessageBox(NULL, "地形平滑完成！", "友情提示", MB_OK);
	}
}

// ------------------------------------------------------
// 计算法线
void CHeightEditorDlg::OnBnClickedButton2()
{
	if(COTETilePage::GetCurrentPage())
	{
		COTETilePage::GetCurrentPage()->AverageVertexNormal();
	}
}
