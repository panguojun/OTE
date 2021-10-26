// CustomGen.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "CustomGen.h"
#include "OTEQTSceneManager.h"
#include "KeyFrameDlg.h"

using namespace Ogre;
using namespace OTE;

// ------------------------------------------
// CCustomGen 对话框
CCustomGen CCustomGen::s_Inst;
IMPLEMENT_DYNAMIC(CCustomGen, CDialog)
CCustomGen::CCustomGen(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomGen::IDD, pParent)
{
}

CCustomGen::~CCustomGen()
{
}

void CCustomGen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCustomGen, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT11, OnEnChangeEdit11)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// ----------------------------------------------
// 圆锥曲线生成

void CCustomGen::OnBnClickedButton1()
{	
	CKeyFrameDlg::s_Inst.ClearKeyList();	

	float e = 0.5f, p = 5.0f, r0 = 0.0f;

	CString str;
	GetDlgItemText(IDC_EDIT1, str);	e = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT2, str);	p = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT7, str);	r0 = atof((char*)LPCTSTR(str));

	Ogre::Vector3 center( 1.0f, 2.0f, 1.5f);

	GetDlgItemText(IDC_EDIT3, str);	center.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT4, str);	center.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT5, str);	center.z = atof((char*)LPCTSTR(str));

	Ogre::Vector3 scale;

	GetDlgItemText(IDC_EDIT8, str);		scale.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT9, str);		scale.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT10, str);	scale.z = atof((char*)LPCTSTR(str));

	Ogre::Vector3 rot;

	GetDlgItemText(IDC_EDIT11, str);	rot.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT12, str);	rot.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT13, str);	rot.z = atof((char*)LPCTSTR(str));

	float angle;
	GetDlgItemText(IDC_EDIT6, str);	angle = atof((char*)LPCTSTR(str));
	
	int unitDegree = 15;
	GetDlgItemText(IDC_EDIT14, str);	unitDegree = atof((char*)LPCTSTR(str));
	

	for(int sita = 0; sita <= 360; sita += unitDegree)
	{
		float r;
		Ogre::Radian rad = Ogre::Radian(Ogre::Degree(sita));
		Ogre::Radian angRad = Ogre::Radian(Ogre::Degree(angle));		

		float factor = 1 - e * Ogre::Math::Cos(rad);
		if(factor < 0.01 && factor > - 0.01)		
			r = 30.0f + r0; // max distance		
		else
			r = e * p / ( 1 - e * Ogre::Math::Cos(rad) ) + r0;

		Ogre::Vector3 pt(	r * Ogre::Math::Cos(rad) * Ogre::Math::Cos(angRad),
							r * Ogre::Math::Sin(angRad),
							r * Ogre::Math::Sin(rad) * Ogre::Math::Cos(angRad) 
							);

		// 下一点

		Ogre::Radian nrad = Ogre::Radian(Ogre::Degree(sita + unitDegree));
		float nr;
		float nfactor = 1 - e * Ogre::Math::Cos(nrad);
		if(nfactor < 0.01 && nfactor > - 0.01)		
			nr = 30.0f + r0; // max distance		
		else
			nr = e * p / ( 1 - e * Ogre::Math::Cos(nrad) ) + r0;

		Ogre::Vector3 npt(	nr * Ogre::Math::Cos(nrad) * Ogre::Math::Cos(angRad),
							nr * Ogre::Math::Sin(angRad),
							nr * Ogre::Math::Sin(nrad)/* * Ogre::Math::Cos(angRad)*/ 
							);


		// 转角计算

		float time = CKeyFrameDlg::s_Inst.GetTimeRange() * sita / 360.0f;
		
		Ogre::Vector3 dir = npt - pt;
		
		Ogre::Real c = Ogre::Vector3::UNIT_X.dotProduct(dir);
		Ogre::Real s = Ogre::Vector3::UNIT_X.crossProduct(dir).dotProduct(Ogre::Vector3::UNIT_Y);			
		
		Ogre::Radian angle1 = Ogre::Math::ATan2(c, s);		

		CKeyFrameDlg::s_Inst.SetKey(
			time, 
			center + pt, scale,
			rot	+ Ogre::Vector3(0, 180.0f - angle1.valueDegrees(), 0)				
			);

	}

	CKeyFrameDlg::s_Inst.InvalidateRect(NULL);

	this->ShowWindow(SW_HIDE);
}

// ----------------------------------------------
// 初始化
BOOL CCustomGen::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	this->SetDlgItemText(IDC_EDIT1, "0.5");
	this->SetDlgItemText(IDC_EDIT2, "5");

	this->SetDlgItemText(IDC_EDIT3, "0");
	this->SetDlgItemText(IDC_EDIT4, "0");
	this->SetDlgItemText(IDC_EDIT5, "0");

	this->SetDlgItemText(IDC_EDIT6, "0");

	this->SetDlgItemText(IDC_EDIT7, "0");

	this->SetDlgItemText(IDC_EDIT8, "1");	
	this->SetDlgItemText(IDC_EDIT9, "1");	
	this->SetDlgItemText(IDC_EDIT10, "1");

	this->SetDlgItemText(IDC_EDIT11, "0");	
	this->SetDlgItemText(IDC_EDIT12, "0");	
	this->SetDlgItemText(IDC_EDIT13, "0");	

	this->SetDlgItemText(IDC_EDIT14, "30");		

	return ret;
}	

void CCustomGen::OnEnChangeEdit11()
{
}

// ------------------------------------------------
// 螺线生成
void CCustomGen::OnBnClickedButton2()
{
	CString str;

	Ogre::Vector3 center;

	GetDlgItemText(IDC_EDIT3, str);	center.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT4, str);	center.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT5, str);	center.z = atof((char*)LPCTSTR(str));

	Ogre::Vector3 scale;

	GetDlgItemText(IDC_EDIT8, str);		scale.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT9, str);		scale.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT10, str);	scale.z = atof((char*)LPCTSTR(str));

	Ogre::Vector3 rot;

	GetDlgItemText(IDC_EDIT11, str);	rot.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT12, str);	rot.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT13, str);	rot.z = atof((char*)LPCTSTR(str));

	float angle;
	GetDlgItemText(IDC_EDIT6, str);	angle = atof((char*)LPCTSTR(str));

	int unitDegree = 15;
	GetDlgItemText(IDC_EDIT14, str);	unitDegree = atof((char*)LPCTSTR(str));

	int nRnd = 3;

	for(int sita = 0; sita <= nRnd * 360; sita += unitDegree)
	{			
		Ogre::Radian angRad = Ogre::Radian(Ogre::Degree(angle));
		Ogre::Radian nrad = Ogre::Radian(Ogre::Degree(sita + unitDegree));		

		float time = CKeyFrameDlg::s_Inst.GetTimeRange() * sita / 360.0f;

		float nr = sita * 0.01f;


		Ogre::Vector3 npt(	
			    nr * Ogre::Math::Cos(nrad) * Ogre::Math::Cos(angRad),
				sita * 0.01f * Ogre::Math::Sin(angRad),
				nr * Ogre::Math::Sin(nrad) * Ogre::Math::Cos(angRad) 
				);

		Ogre::Vector3 dir = npt - npt;
		
		Ogre::Real c = Ogre::Vector3::UNIT_X.dotProduct(dir);
		Ogre::Real s = Ogre::Vector3::UNIT_X.crossProduct(dir).dotProduct(Ogre::Vector3::UNIT_Y);			
		
		Ogre::Radian angle1 = Ogre::Math::ATan2(c, s);

		Ogre::Quaternion trkQ((sita == 360 ? Ogre::Radian(Degree(0)) : Ogre::Radian(Degree(180)) - angle1), Ogre::Vector3::UNIT_Y);
		
		CKeyFrameDlg::s_Inst.SetKey(
				time, 
				center + npt, scale, 
				rot
				);

	}

}
