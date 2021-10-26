// PointLightCrtDlg.cpp : 实现文件
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
// 点光源最大影响范围

#define  PL_MAX_RANGE			40.0f;

std::vector<DWORD*>				g_VertexColorHistoryList;	// 顶点色历史

// ----------------------------------------------
// CPointLightCrtDlg 对话框

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

	// 选择新的点光源名字

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

	// 设置默认参数

	pLightEntity->m_DymProperty.AddProperty("minDis",		0.2f	);
	pLightEntity->m_DymProperty.AddProperty("maxDis",		0.5f	);

	UpdateUI();

	SCENE_MGR->AddListener(COTEQTSceneManager::eSelectEntity, OnSelectEntity);

	// 默认计算物件

	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(BST_CHECKED);

	return ret;
}

// ----------------------------------------------
// 创建光源到笔刷

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

	// 更新点光源实体列表

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
// 更新光照信息，　这里实现点光源效果
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

	// 点光源位置

	lightPos = pLightEntity->GetParentSceneNode()->getPosition();
	
	direction = paintPos - lightPos;

	// 计算点光源扩散强度
	
	float minDis	= pLightEntity->m_DymProperty.GetfloatVal("minDis")	* PL_MAX_RANGE;
	float maxDis	= pLightEntity->m_DymProperty.GetfloatVal("maxDis")	* PL_MAX_RANGE;
	
	float I0 = 1.0f;	
	float linear = I0 / (maxDis - minDis);

	float dis = direction.length(); 

	// 光方向

	direction.normalise();

	// 混合强度
			
	if(dis < minDis)
		alpha = I0;
	else if(dis > maxDis)
		alpha = 0.0f;
	else
		alpha = I0 - (dis - minDis) * linear;	

	// 影响距离

	reflectDis = dis;

	// 最大影响范围

	reflectDis0 = maxDis * 1.414;

	// 光颜色

	diffuse = pLightEntity->m_DymProperty.GetCorVal("diffuse")/* * alpha*/;

	// 叠加

	//alpha = 2.0f;
}

// ----------------------------------------------
// 地表上的点光源
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
// 计算顶点色

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
		// 计算物件点光源

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
		// 地皮

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
	// TODO: 在此添加控件通知处理程序代码
}

// ----------------------------------------------
// 保存顶点色 

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
// 选择点光源
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
	
	// 根据物件类型 自动分配名称 

	if(lMName == "__autolm")
	{
		COTEActorEntity* ent = SCENE_MGR->GetEntity(entityName);
		if(ent)
		{
			std::string meshName = ent->getMesh()->getName();
			std::string path, fullpath;
			OTEHelper::GetFullPath(meshName, path, fullpath);

			// 植物 使用 128 的贴图
			
			if(fullpath.find("恶魔植物") != std::string::npos)
			{
				lMName = "lightMapTemplate128.bmp";
			}

			// 小物件 使用 128 的贴图

			else if(fullpath.find("公共物件") != std::string::npos)
			{	
				lMName = "lightMapTemplate128.bmp";
			}

			// 普通物件 使用 256 的贴图

			else 
			{
				lMName = "lightMapTemplate256.bmp";
			}
		}
	
	}

	return lMName; 
}

// ----------------------------------------------
// 计算光照图

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
		// 计算物件点光源

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
		// 地皮

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
// 保存光照图

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
// 天光

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
