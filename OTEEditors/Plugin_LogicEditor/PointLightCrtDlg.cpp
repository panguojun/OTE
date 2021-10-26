// PointLightCrtDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PointLightCrtDlg.h"
#include "EntCrtDlg.h"
#include "OTEEntityLightmapPainter.h"
#include "OTERenderLight.h"
#include "OTETerrainLightmapMgr.h"
#include "OTEEntityVertexColorPainter.h"
#include ".\pointlightcrtdlg.h"

// ----------------------------------------------
// ���Դ���Ӱ�췶Χ

#define  PL_MAX_RANGE			40.0f;

std::vector<DWORD*>				g_VertexColorHistoryList;	// ����ɫ��ʷ

// ----------------------------------------------
// CPointLightCrtDlg �Ի���

CPointLightCrtDlg	CPointLightCrtDlg::s_Inst;

IMPLEMENT_DYNAMIC(CPointLightCrtDlg, CDialog)
CPointLightCrtDlg::CPointLightCrtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPointLightCrtDlg::IDD, pParent)
{
}

CPointLightCrtDlg::~CPointLightCrtDlg()
{
}

void CPointLightCrtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_PtLgtList);
}


BEGIN_MESSAGE_MAP(CPointLightCrtDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON14, OnBnClickedButton14)
END_MESSAGE_MAP()


// ----------------------------------------------
void SelectPtnLgtEntity(const std::string& selLightName)
{
	if(!CPointLightCrtDlg::s_Inst.m_LightName.empty() && 
		SCENE_MGR->IsEntityPresent(CPointLightCrtDlg::s_Inst.m_LightName)
	)
	{
		SCENE_MGR->GetEntity(CPointLightCrtDlg::s_Inst.m_LightName)->getParentSceneNode()->showBoundingBox(false);
	}

	// ѡ���µĵ��Դ����

	CPointLightCrtDlg::s_Inst.m_LightName = selLightName;

	if(!CPointLightCrtDlg::s_Inst.m_LightName.empty() && 
		SCENE_MGR->IsEntityPresent(CPointLightCrtDlg::s_Inst.m_LightName)
	)
	{
		SCENE_MGR->GetEntity(CPointLightCrtDlg::s_Inst.m_LightName)->getParentSceneNode()->showBoundingBox(true);
	}		
}

// ----------------------------------------------
void OnSelectEntity(int ud)
{
	
	if(SCENE_MGR->GetCurObjSection() && SCENE_MGR->GetCurObjSection()->getName().find("Lgt_") != std::string::npos)
	{
		OTE::COTEActorEntity* pLightEntity = (OTE::COTEActorEntity*)SCENE_MGR->GetCurObjSection();
		SelectPtnLgtEntity(pLightEntity->getName());	

		CPointLightCrtDlg::s_Inst.UpdateUI();
	}
}

// ----------------------------------------------
BOOL CPointLightCrtDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	//((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(BST_CHECKED);
	
	if(!CEntCrtDlg::sInst.m_MouseObj)
		CEntCrtDlg::sInst.m_MouseObj = SCENE_MGR->AutoCreateEntity("lighthelper.mesh", "Lgt");	
		
	OTE::COTEActorEntity* pLightEntity = (OTE::COTEActorEntity*)CEntCrtDlg::sInst.m_MouseObj;
	assert(pLightEntity);

	SelectPtnLgtEntity(pLightEntity->getName());

	// ����Ĭ�ϲ���

	pLightEntity->m_DymProperty.AddProperty("minDis",		0.2f	);
	pLightEntity->m_DymProperty.AddProperty("maxDis",		0.5f	);

	UpdateUI();

	SCENE_MGR->AddListener(COTEQTSceneManager::eSelectEntity, OnSelectEntity);

	// Ĭ�ϼ������

	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(BST_CHECKED);

	return ret;
}

// ----------------------------------------------
// ������Դ����ˢ

void CPointLightCrtDlg::OnBnClickedButton1()
{
	CEntCrtDlg::sInst.m_MouseObj = SCENE_MGR->AutoCreateEntity("lighthelper.mesh", "Lgt");	

	UpdateChange();
}

void CPointLightCrtDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{	
}

void CPointLightCrtDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{

}

void CPointLightCrtDlg::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{

}

// ----------------------------------------------
void CPointLightCrtDlg::OnEnChangeEdit1()
{
	UpdateChange();
	UpdateSlider(IDC_SLIDER1);
}

// ----------------------------------------------
void CPointLightCrtDlg::OnEnChangeEdit2()
{
	UpdateChange();
	UpdateSlider(IDC_SLIDER2);
}

// ----------------------------------------------
void CPointLightCrtDlg::OnEnChangeEdit3()
{
}

// ----------------------------------------------
void CPointLightCrtDlg::UpdateSlider(int id)
{	
	if(m_LightName.empty())
		return;

	OTE::COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(m_LightName);
	assert(pLightEntity);

	_MAPPING	(int,		  std::string	);
	_ADDMAPPING	(IDC_SLIDER1, "minDis"		);
	_ADDMAPPING	(IDC_SLIDER2, "maxDis"		);
	
	const int __IDC_SLIDER	=	id;

	int slider_min = ((CSliderCtrl*)this->GetDlgItem(__IDC_SLIDER))->GetRangeMin();
	int slider_max = ((CSliderCtrl*)this->GetDlgItem(__IDC_SLIDER))->GetRangeMax();		
	
	double val = pLightEntity->m_DymProperty.GetfloatVal(_GETMAPPING(__IDC_SLIDER));	

	((CSliderCtrl*)this->GetDlgItem(__IDC_SLIDER))->SetPos(val * (slider_max - slider_min));
}

// ----------------------------------------------
void CPointLightCrtDlg::UpdateEditBox(int id)
{
	if(m_LightName.empty())
		return;

	OTE::COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(m_LightName);
	assert(pLightEntity);

	_MAPPING	(int,		std::string	);
	_ADDMAPPING	(IDC_EDIT1, "minDis"	);
	_ADDMAPPING	(IDC_EDIT2, "maxDis"	);

	const int __IDC_EDIT	=	id;

	double val = pLightEntity->m_DymProperty.GetfloatVal(_GETMAPPING(__IDC_EDIT));
	std::stringstream ss;
	ss << val * PL_MAX_RANGE;
	SetDlgItemText(__IDC_EDIT, ss.str().c_str());

}

// ----------------------------------------------
void CPointLightCrtDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateChange();
	UpdateEditBox(IDC_EDIT1);
}

// ----------------------------------------------
void CPointLightCrtDlg::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateChange();
	UpdateEditBox(IDC_EDIT2);
}

// ----------------------------------------------
void CPointLightCrtDlg::OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateChange();
}

// ----------------------------------------------
void CPointLightCrtDlg::UpdateChange()
{
	if(m_LightName.empty())
		return;

	OTE::COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(m_LightName);
	if(!pLightEntity)
		return;

	int slider1_min = ((CSliderCtrl*)this->GetDlgItem(IDC_SLIDER1))->GetRangeMin();
	int slider1_max = ((CSliderCtrl*)this->GetDlgItem(IDC_SLIDER1))->GetRangeMax();
	pLightEntity->m_DymProperty.AddProperty("minDis",   float(((CSliderCtrl*)this->GetDlgItem(IDC_SLIDER1))->GetPos()) / (slider1_max - slider1_min));
	
	int slider2_min = ((CSliderCtrl*)this->GetDlgItem(IDC_SLIDER2))->GetRangeMin();
	int slider2_max = ((CSliderCtrl*)this->GetDlgItem(IDC_SLIDER2))->GetRangeMax();
	pLightEntity->m_DymProperty.AddProperty("maxDis",   float(((CSliderCtrl*)this->GetDlgItem(IDC_SLIDER2))->GetPos()) / (slider2_max - slider2_min));

}

// ----------------------------------------------
void CPointLightCrtDlg::UpdateUI()
{
	if(m_LightName.empty())
		return;

	UpdateSlider(IDC_SLIDER1);
	UpdateSlider(IDC_SLIDER2);

	UpdateEditBox(IDC_EDIT1);
	UpdateEditBox(IDC_EDIT2);

	// ���µ��Դʵ���б�

	m_PtLgtList.ResetContent();
	std::vector<Ogre::MovableObject*> elist;
	SCENE_MGR->GetEntityList(&elist);
	std::vector<Ogre::MovableObject*>::iterator it = elist.begin();
	while(it != elist.end())
	{
		if((*it)->getName().find("Lgt_") != std::string::npos)
		{
			m_PtLgtList.AddString((*it)->getName().c_str());
		}

		++ it;
	}

}


// ----------------------------------------------
void CPointLightCrtDlg::OnBnClickedButton3()
{
	if(m_LightName.empty())
		return;

	static std::string c_colorName = "diffuse";

	OTE::COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(m_LightName);

	if(!pLightEntity)
		return;

	Ogre::ColourValue cor = pLightEntity->m_DymProperty.GetCorVal(c_colorName);

	CColorDialog color(cor.getAsABGR(), CC_FULLOPEN );

	if(color.DoModal() == IDOK)
	{	
		cor.r = GetRValue(color.GetColor()) / 255.0f;
		cor.g = GetGValue(color.GetColor()) / 255.0f;
		cor.b = GetBValue(color.GetColor()) / 255.0f;

		pLightEntity->m_DymProperty.AddProperty(c_colorName, cor);
	}
}

// ----------------------------------------------
// ���¹�����Ϣ��������ʵ�ֵ��ԴЧ��
void OnGenLightInfor(const Ogre::Vector3& paintPos,
					 Ogre::Vector3& lightPos,
					 Ogre::Vector3& direction, 
					 Ogre::ColourValue& diffuse, 
					 float& reflectDis,
					 float& reflectDis0,
					 float& alpha
					 )
{
	if(CPointLightCrtDlg::s_Inst.m_LightName.empty())
		return;

	COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(CPointLightCrtDlg::s_Inst.m_LightName);
	assert(pLightEntity);

	// ���Դλ��

	lightPos = pLightEntity->GetParentSceneNode()->getPosition();
	
	direction = paintPos - lightPos;

	// ������Դ��ɢǿ��
	
	float minDis	= pLightEntity->m_DymProperty.GetfloatVal("minDis")	* PL_MAX_RANGE;
	float maxDis	= pLightEntity->m_DymProperty.GetfloatVal("maxDis")	* PL_MAX_RANGE;
	
	float I0 = 1.0f;	
	float linear = I0 / (maxDis - minDis);

	float dis = direction.length(); 

	// �ⷽ��

	direction.normalise();

	// ���ǿ��
			
	if(dis < minDis)
		alpha = I0;
	else if(dis > maxDis)
		alpha = 0.0f;
	else
		alpha = I0 - (dis - minDis) * linear;	

	// Ӱ�����

	reflectDis = dis;

	// ���Ӱ�췶Χ

	reflectDis0 = maxDis * 1.414;

	// ����ɫ

	diffuse = pLightEntity->m_DymProperty.GetCorVal("diffuse")/* * alpha*/;

	// ����

	//alpha = 2.0f;
}

// ----------------------------------------------
// �ر��ϵĵ��Դ
void OnGenTerrainLightInfor(const Ogre::Vector3& paintPos,
					 Ogre::Vector3& lightPos,
					 Ogre::Vector3& direction, 
					 Ogre::ColourValue& diffuse, 
					 float& reflectDis,
					 float& reflectDis0,
					 float& alpha
					 )
{
	OnGenLightInfor(paintPos, lightPos, direction, diffuse, reflectDis, reflectDis0, alpha);
	diffuse *= alpha;
}

// ----------------------------------------------
// ���㶥��ɫ

void CPointLightCrtDlg::OnBnClickedButton2()
{
	if(m_LightName.empty())
		return;

	OTE::COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(m_LightName);
	OTE_ASSERT(pLightEntity);
	
	UpdateChange();

	//COTERenderLight::GetInstance()->Update();
	//Ogre::ColourValue& ambient = COTERenderLight::GetInstance()->GetLightInfo()->ambient.GetColor();
	Ogre::ColourValue& ambient = Ogre::ColourValue::Black;
	Ogre::ColourValue& diffuse = COTERenderLight::GetInstance()->GetLightKey()->diffuse.GetColor();
	
	if(((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() == BST_CHECKED)
	{
		// ����������Դ

		COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
		if(e && e != pLightEntity && e->getName().find("Lgt_") == std::string::npos)
		{			
			COTEEntityVertexColorPainter::GenLightDiffmap(
				(COTEEntity*)e,
				SCENE_MAINLIGHT->getDirection(),
				diffuse, ambient,
				1.0f,					
				((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED,
				OnGenLightInfor
				);		
			
		}
		else
		{		
			COTEEntityVertexColorPainter::GenLightDiffmap(			
						SCENE_MAINLIGHT->getDirection(), 
						diffuse, 
						ambient,  
						1.0f, "Lgt_",
						((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED,
						OnGenLightInfor);
		
		}
	}

	if(((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == BST_CHECKED && COTETilePage::GetCurrentPage())
	{
		// ��Ƥ

		COTETerrainLightmapMgr::GetInstance()->GenLightmap(	COTETilePage::GetCurrentPage(),
																	SCENE_MAINLIGHT->getDirection(),	
																	diffuse, 		
																	ambient,	
																	COTETerrainLightmapMgr::E_ADD,
																	((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED,
																	OnGenTerrainLightInfor
																);
	}
}

// ----------------------------------------------
void CPointLightCrtDlg::OnBnClickedCheck3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

// ----------------------------------------------
// ���涥��ɫ 

void CPointLightCrtDlg::OnBnClickedButton4()
{

	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityVertexColorPainter::SaveVertexColorData(e);

	}
	else
	{
		COTEEntityVertexColorPainter::SaveVertexColorData();

	}

	if(COTETilePage::GetCurrentPage())
		COTETerrainLightmapMgr::GetInstance()->SaveLightMapFile(COTETilePage::GetCurrentPage());


}

// ----------------------------------------------
// ѡ����Դ
void CPointLightCrtDlg::OnCbnSelchangeCombo1()
{	
	CString ptLgtName;
	m_PtLgtList.GetWindowText(ptLgtName);
	int curSel = m_PtLgtList.GetCurSel();

	Ogre::MovableObject* mo = SCENE_MGR->GetEntity(ptLgtName.GetString());

	if(mo && mo->getName().find("Lgt_") != std::string::npos)
	{
		OTE::COTEActorEntity* pLightEntity = (OTE::COTEActorEntity*)mo;
		SelectPtnLgtEntity(pLightEntity->getName());	
		
		UpdateUI();
	}

	m_PtLgtList.SetCurSel(curSel);
	
}


// ------------------------------------------------
Ogre::String GetTemplateLightmap()
{
	return "__autolm";
}

// ------------------------------------------------
std::string lmTempGener(const std::string& entityName)
{	
	std::string lMName = GetTemplateLightmap();
	
	// ����������� �Զ��������� 

	if(lMName == "__autolm")
	{
		COTEActorEntity* ent = SCENE_MGR->GetEntity(entityName);
		if(ent)
		{
			std::string meshName = ent->getMesh()->getName();
			std::string path, fullpath;
			OTEHelper::GetFullPath(meshName, path, fullpath);

			// ֲ�� ʹ�� 128 ����ͼ
			
			if(fullpath.find("��ħֲ��") != std::string::npos)
			{
				lMName = "lightMapTemplate128.bmp";
			}

			// С��� ʹ�� 128 ����ͼ

			else if(fullpath.find("�������") != std::string::npos)
			{	
				lMName = "lightMapTemplate128.bmp";
			}

			// ��ͨ��� ʹ�� 256 ����ͼ

			else 
			{
				lMName = "lightMapTemplate256.bmp";
			}
		}
	
	}

	return lMName; 
}

// ----------------------------------------------
// �������ͼ

void CPointLightCrtDlg::OnBnClickedButton5()
{
	if(m_LightName.empty())
		return;

	OTE::COTEActorEntity* pLightEntity = SCENE_MGR->GetEntity(m_LightName);
	OTE_ASSERT(pLightEntity);
	
	UpdateChange();

	//COTERenderLight::GetInstance()->Update();
	//Ogre::ColourValue& ambient = COTERenderLight::GetInstance()->GetLightInfo()->ambient.GetColor();
	Ogre::ColourValue& ambient = Ogre::ColourValue::Black;
	Ogre::ColourValue& diffuse = COTERenderLight::GetInstance()->GetLightKey()->diffuse.GetColor();
	
	if(((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() == BST_CHECKED)
	{
		// ����������Դ

		COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
		if(e && e != pLightEntity)
		{
			COTEEntityLightmapPainter::GenLightmap(
									e,
									Ogre::Ray(Ogre::Vector3::ZERO, SCENE_MAINLIGHT->getDirection()),
									diffuse, 
									ambient,//SCENE_MGR->getAmbientLight(),			
									"lightMapTemplate256.bmp",
									((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED,									
									OnGenLightInfor,
									1.0f
									);
			
			
		}
		else
		{
			COTEEntityLightmapPainter::GenSceneLightmaps(	
				SCENE_MAINLIGHT->getDirection(),
				diffuse, 
				ambient,//SCENE_MGR->getAmbientLight(),	
				lmTempGener,
				"Lgt_",
				((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED,
				OnGenLightInfor,
				1.0f
				);
			
		}
	}

	if(((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == BST_CHECKED)
	{
		// ��Ƥ

		COTETerrainLightmapMgr::GetInstance()->GenLightmap(	COTETilePage::GetCurrentPage(),
																	SCENE_MAINLIGHT->getDirection(),	
																	diffuse, 		
																	ambient,	
																	COTETerrainLightmapMgr::E_ADD,
																	((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED,
																	OnGenTerrainLightInfor
																);
	}
}

// ----------------------------------------------
// �������ͼ

void CPointLightCrtDlg::OnBnClickedButton6()
{
	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityLightmapPainter::SaveLightmap(e);

	}
	else
	{
		COTEEntityLightmapPainter::SaveLightmap();

	}

	if(COTETilePage::GetCurrentPage())
		COTETerrainLightmapMgr::GetInstance()->SaveLightMapFile(COTETilePage::GetCurrentPage());
}

// ----------------------------------------------
// ���

void CPointLightCrtDlg::OnBnClickedButton7()
{
	

}

// ----------------------------------------------
void CPointLightCrtDlg::OnBnClickedButton14()
{
	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityLightmapPainter::ReLoadLightmapFromFile(e);
		
	}
	else
	{
		COTEEntityLightmapPainter::ReLoadLightmapFromFile();

	}
}
