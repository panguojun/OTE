// HeightEdtorDlg.cpp : ʵ���ļ�
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

//���θ߶���ʷ��¼
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

	m_EditorTypeCombo2.InsertString(0, "����");
	m_EditorTypeCombo2.InsertString(1, "����");
	m_EditorTypeCombo2.InsertString(2, "�⻬");
	m_EditorTypeCombo2.InsertString(3, "ƽ��");
	m_EditorTypeCombo2.InsertString(4, "����");
	m_EditorTypeCombo2.InsertString(5, "���Ƹ߶�");
	m_EditorTypeCombo2.InsertString(6, "����");

	m_OutSizeSlider.SetRange(1, 50);
	m_OutSizeSlider.SetTicFreq(1);
	m_InnerSizeSlider.SetRange(1, 50);
	m_InnerSizeSlider.SetTicFreq(1);
	m_BrushStrengthSlider.SetRange(1, 50);
	m_BrushStrengthSlider.SetTicFreq(1);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(true);

	// �����б�

	std::string cliffPath = OTEHelper::GetResPath("ResConfig", "CliffPath");
	AllDirScanfCliffFiles(cliffPath);

	return tRet;
}

// ----------------------------------------------------------------
//����������ԴĿ¼
void CHeightEditorDlg::AllDirScanfCliffFiles(std::string strPathName)
{
	// �����ļ���

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
// �����߶�

Ogre::Vector3 pos(0, 0, 0);
Ogre::Ray ray;

void CHeightEditorDlg::OnLButtonDown(float x, float y)
{
	if(!COTETilePage::GetCurrentPage())
		return;

	COTETerrainHeightMgr::GetInstance()->m_LastEditTileList.clear();

	if(m_BrushEntity.m_HeightEditType == 6)
	{
		// ��ȡ���µײ��߶�

		CString bottomHgtTxt;
		m_BottomHgtEdit.GetWindowText(bottomHgtTxt);
		if(bottomHgtTxt == "������ȡ�߶�")
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
				g_TerrainEditTrackList.erase(g_TerrainEditTrackList.begin());	//�Ƴ��б��ж���
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

	if(m_BrushEntity.m_HeightEditType != 6) // ���¡���
	{
		// ����

		//std::list<COTETile*>& tileList = COTETerrainHeightMgr::GetInstance()->m_LastEditTileList;
		//std::list<COTETile*>::iterator it = tileList.begin();
		//while(it != tileList.end())
		//{			
		//	(*it)->AverageVertexNormal();
		//	++ it;
		//}	
		//COTETerrainHeightMgr::GetInstance()->m_LastEditTileList.clear();

		// ����ͼ
		
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
		
		Ogre::Vector3 mainLightDir = SCENE_MAINLIGHT->getDirection();			//	���ⷽ��
		Ogre::ColourValue mainLightCor = SCENE_MAINLIGHT->getDiffuseColour();	//	����ɫ

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
	case '1'://[����] ��ݼ�
		m_EditorTypeCombo2.SetCurSel(0);
		m_BrushEntity.m_HeightEditType = 0;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '2'://[����] ��ݼ�
		m_EditorTypeCombo2.SetCurSel(1);
		m_BrushEntity.m_HeightEditType = 1;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '3'://[�⻬] ��ݼ�
		m_EditorTypeCombo2.SetCurSel(2);
		m_BrushEntity.m_HeightEditType = 3;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '4'://[ƽ��] ��ݼ�
		m_EditorTypeCombo2.SetCurSel(3);
		m_BrushEntity.m_HeightEditType = 2;
		//m_BrushEntity.m_fDragHeight = pos.y;
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '5'://[����] ��ݼ�
		m_EditorTypeCombo2.SetCurSel(4);
		m_BrushEntity.m_HeightEditType = 4; 
		yes = true;
		m_txtHeight.ShowWindow(false);
		break;
	case '6'://[���Ƹ߶�] ��ݼ�
		m_EditorTypeCombo2.SetCurSel(5);
		m_BrushEntity.m_HeightEditType = 5; 
		yes = true;
		m_txtHeight.ShowWindow(true);
		break;
	case 'Z':
		{
			if(::GetKeyState(VK_CONTROL) & 0x80)
			{
				//������ԭ����
				if(g_TerrainEditTrackList.size() != 0)
				{
					for(int i = g_TerrainEditTrackList.size(); i > 0; i--)
					{
						COTETerrainHeightMgr::GetInstance()->updateTerrainAllHeight(COTETilePage::GetCurrentPage(), &g_TerrainEditTrackList[i-1]);
						delete [] g_TerrainEditTrackList[i-1].heightData;
						delete [] g_TerrainEditTrackList[i-1].normal;

						g_TerrainEditTrackList.erase(g_TerrainEditTrackList.begin() + (i-1));	//�Ƴ��б��ж���
						
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
		// ����

		if(m_BrushEntity.m_HeightEditType == 6)
		{
			DrawCliff(m_BrushEntity.m_Pos.x, m_BrushEntity.m_Pos.z);
		}
		// ��ɽ��߶Ƚ��б༭

		else if(m_BrushEntity.m_HeightEditType == 4)
		{ // �������
						
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
		{ // �����
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
				m_BrushEntity.m_Pos.x, //x����
				m_BrushEntity.m_Pos.z, //z����
				m_BrushEntity.m_Strength * m_BrushEntity.m_fDragHeight, //y�߶ȳ�ǿ��
				_Intensity,							//ǿ��
				m_BrushEntity.m_InnerRad,			//��Ȧ
				m_BrushEntity.m_OutterRad,			//��Ȧ
				m_BrushEntity.m_HeightEditType);	//��������
			
			// ���±�ˢ
			m_BrushEntity.m_fDragHeight = y - oldMPY;
		}		
	}

	oldMPX = x; oldMPY = y;
	
	std::stringstream ss;
	ss << "��ˢλ�� x: " << m_BrushEntity.m_Pos.x << ", z: " << m_BrushEntity.m_Pos.z;
	SetDlgItemText(IDC_STATIC_MSG, ss.str().c_str()); 
}

// ------------------------------------------------------
void CHeightEditorDlg::OnCbnSelchangeList1()
{		
	m_txtHeight.ShowWindow(false);

	CString str;
	m_EditorTypeCombo2.GetText(m_EditorTypeCombo2.GetCurSel(), str);
	
	if(		str == "����")
		m_BrushEntity.m_HeightEditType = 0;
	else if(str == "����")
		m_BrushEntity.m_HeightEditType = 1;	
	else if(str == "ƽ��")
		m_BrushEntity.m_HeightEditType = 2;
	else if(str == "�⻬")
		m_BrushEntity.m_HeightEditType = 3;
	else if(str == "����")
		m_BrushEntity.m_HeightEditType = 4; 
	else if(str == "���Ƹ߶�")
	{
		m_txtHeight.ShowWindow(true);
		m_BrushEntity.m_HeightEditType = 5;
	}
	else if(str == "����")
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

// ------------------------------------------------------
void CHeightEditorDlg::ResetBrushEntity()
{
	m_BrushEntity.DeleteBrushEntity();
}

// ------------------------------------------------------
// ��Ȧ
int Circledifference = 0;
float RadScale = 36.0f;	//��ˢ��Ȧ�Ŵ����ֵ
void CHeightEditorDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_BrushEntity.m_OutterRad/*m_Intensity*/ = (/*1.0f - */(float)m_OutSizeSlider.GetPos()/m_OutSizeSlider.GetRangeMax() * RadScale);
	if(m_BrushEntity.m_HeightEditType == 5)
		m_BrushEntity.m_OutterRad /= 3;

	Circledifference = m_OutSizeSlider.GetPos() - m_InnerSizeSlider.GetPos();
}

// ------------------------------------------------------
// ��Ȧ
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
// ǿ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

// ------------------------------------------------------
void CHeightEditorDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

// ------------------------------------------------------
void CHeightEditorDlg::OnBnClickedBtnupdatenorm()
{
	COTETilePage::GetCurrentPage()->AverageVertexNormal();
}


// ------------------------------------------------------
// ����Ч��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CHeightEditorDlg::OnEnChangeHeighttext()
{

}

void CHeightEditorDlg::OnBnClickedButton1()
{
	m_BottomHgtEdit.SetWindowText("������ȡ�߶�");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

// ------------------------------------------------------
void CHeightEditorDlg::OnBnClickedBtnflatness()
{
	CString str = "";
	m_txtSensitivity.GetWindowText(str);
	if(str.Trim().GetLength() == 0 || str.Trim().GetLength() > 1)
	{
		::MessageBox(NULL, "���ж�ֵ����ȷ��", "��ʾ", MB_OK);
		return;
	}
	char * a = str.GetBuffer();
	char cSensitivity = a[0];
	if(cSensitivity < 48 || cSensitivity > 57)
	{
		::MessageBox(NULL, "���ж�ֵ����ȷ��", "��ʾ", MB_OK);
		return;
	}

	int answer = ::MessageBox(NULL, "ȷ�ϴ���߶�ͼ��?", "��ʾ",
								MB_YESNO);
	if (answer == IDYES)
	{
		COTETilePage::GetCurrentPage()->heightMapDeleteDirt(atoi(&cSensitivity));
		//::MessageBox(NULL, "����ƽ����ɣ�", "������ʾ", MB_OK);
	}
}

// ------------------------------------------------------
// ���㷨��
void CHeightEditorDlg::OnBnClickedButton2()
{
	if(COTETilePage::GetCurrentPage())
	{
		COTETilePage::GetCurrentPage()->AverageVertexNormal();
	}
}
