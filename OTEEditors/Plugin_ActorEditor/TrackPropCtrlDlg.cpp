// TrackPropCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "TrackPropCtrlDlg.h"

#include "ActorEditorMainDlg.h"
#include "KeyFrameDlg.h"
#include "OTEMathLib.h"
#include "ActionEdit.h"

using namespace Ogre;
using namespace OTE;

// -------------------------------------------
CTrackPropCtrlDlg	CTrackPropCtrlDlg::s_Inst;
// -------------------------------------------
IMPLEMENT_DYNAMIC(CTrackPropCtrlDlg, CDialog)
CTrackPropCtrlDlg::CTrackPropCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrackPropCtrlDlg::IDD, pParent)
{
}

CTrackPropCtrlDlg::~CTrackPropCtrlDlg()
{
}

void CTrackPropCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK2, m_IsInvCheck);
	DDX_Control(pDX, IDC_CHECK3, m_IsCurveInvCheck);
}


BEGIN_MESSAGE_MAP(CTrackPropCtrlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_EN_CHANGE(IDC_EDIT10, OnEnChangeEdit10)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
END_MESSAGE_MAP()

// ------------------------------------------
BOOL CTrackPropCtrlDlg::OnInitDialog()
{
	BOOL ret =  CDialog::OnInitDialog();

	this->SetDlgItemText(IDC_EDIT1, "0");
	this->SetDlgItemText(IDC_EDIT2, "0");
	this->SetDlgItemText(IDC_EDIT3, "0");

	this->SetDlgItemText(IDC_EDIT4, "1");	
	this->SetDlgItemText(IDC_EDIT5, "1");	
	this->SetDlgItemText(IDC_EDIT6, "1");

	this->SetDlgItemText(IDC_EDIT7, "0");	
	this->SetDlgItemText(IDC_EDIT8, "0");	
	this->SetDlgItemText(IDC_EDIT9, "0");	

	this->SetDlgItemText(IDC_EDIT10, "0");	
	this->SetDlgItemText(IDC_EDIT11, "0");	
	this->SetDlgItemText(IDC_EDIT12, "0");

	return ret;
}

// ------------------------------------------

void CTrackPropCtrlDlg::OnBnClickedButton1()
{
	OnCancel();
}

// ------------------------------------------
// 应用修改
void CTrackPropCtrlDlg::OnBnClickedButton2()
{
	CString str;

	Ogre::Vector3 center;

	GetDlgItemText(IDC_EDIT1, str);		center.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT2, str);		center.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT3, str);		center.z = atof((char*)LPCTSTR(str));

	Ogre::Vector3 scale;

	GetDlgItemText(IDC_EDIT4, str);		scale.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT5, str);		scale.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT6, str);		scale.z = atof((char*)LPCTSTR(str));

	Ogre::Vector3 rot;

	GetDlgItemText(IDC_EDIT7, str);	rot.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT8, str);	rot.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT9, str);	rot.z = atof((char*)LPCTSTR(str));
	
	for(int i = 0; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
	{		
		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
		
		CKeyFrameDlg::s_Inst.SetKey(
				CKeyFrameDlg::s_Inst.m_KeyList[i].Time, 
				kf->m_Position + center, kf->m_Scaling * scale, 
				rot						
			);		
	}
	if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
}

// ------------------------------------------
// 物体方向适应轨道
void CTrackPropCtrlDlg::OnBnClickedButton3()
{	
	// 沿直线

	Ogre::Vector3 lastAxis = Ogre::Vector3::ZERO;
	Ogre::Radian lastAngle;
	Ogre::Quaternion trkQ;

	int ii;
	for(int i = 0; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
	{
		bool isEnd = false;
		ii = i + 1;
		if(ii == CKeyFrameDlg::s_Inst.m_KeyList.size())
		{	
			isEnd = true;
			ii = 0;
		}	

		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
		OTE::COTEKeyFrame* nkf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[ii].Time);
		
		Ogre::Vector3 dir;
		if(!isEnd)
		{	
			Ogre::Vector3 npt = nkf->m_Position;
			Ogre::Vector3 pt  = kf->m_Position;

			if(m_IsInvCheck.GetCheck() == BST_CHECKED)
				dir = pt - npt;			
			else
				dir = npt - pt;			
			
			trkQ = Ogre::Vector3::UNIT_Z.getRotationTo(dir);
		}
		else
		{
			//trkQ = Ogre::Quaternion(lastAngle, lastAxis);
		}

		Ogre::Vector3 angles;
		OTE::MathLib::QuaternionToEuler(trkQ, angles);	

		CKeyFrameDlg::s_Inst.SetKey(
				CKeyFrameDlg::s_Inst.m_KeyList[i].Time, 
				kf->m_Position, kf->m_Scaling, 
				angles					
			);					
	}
	if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
}

// ------------------------------------------
// 赤道旋转
void CTrackPropCtrlDlg::OnBnClickedButton6()
{
	Ogre::Vector3 v;
	CString str;
	GetDlgItemText(IDC_EDIT10, str);		v.x = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT11, str);		v.y = atof((char*)LPCTSTR(str));
	GetDlgItemText(IDC_EDIT12, str);		v.z = atof((char*)LPCTSTR(str));

	for(int i = 0; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
	{		
		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
		
		CKeyFrameDlg::s_Inst.SetKey(
				CKeyFrameDlg::s_Inst.m_KeyList[i].Time, 
				Ogre::Quaternion(Ogre::Radian(Degree(v.z)), Ogre::Vector3::UNIT_Z) *
				Ogre::Quaternion(Ogre::Radian(Degree(v.y)), Ogre::Vector3::UNIT_Y) *
				Ogre::Quaternion(Ogre::Radian(Degree(v.x)), Ogre::Vector3::UNIT_X) * kf->m_Position, 
				kf->m_Scaling, 
				kf->m_Angles + v
				);			
	}

	if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();

}

void CTrackPropCtrlDlg::OnEnChangeEdit10()
{
}

// ------------------------------------------
// 圆滑位置

void CTrackPropCtrlDlg::OnBnClickedButton4()
{
	OTE::COTEKeyFrame* kf1 = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[0].Time);
	OTE::COTEKeyFrame* kf2 = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[CKeyFrameDlg::s_Inst.m_KeyList.size() - 1].Time);
	
	float totalTime = kf2->m_Time - kf1->m_Time;
	float totalLen = 0;

	for(int i = 0; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
	{	
		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
		
		float dis = (kf->m_Position - kf1->m_Position).length();
		totalLen += dis;
		kf1 = kf;		
	}

	if(totalLen > 0)
	{
		kf1 = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[0].Time);
		float curTime = 0;

		for(int i = 0; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
		{	
			OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
			
			float dis = (kf->m_Position - kf1->m_Position).length();
			
			kf->m_Time = (dis / totalLen) * totalTime + curTime;
			curTime = kf->m_Time;

			kf1 = kf;		
		}
	}

	if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
}
// ------------------------------------------
void CTrackPropCtrlDlg::OnBnClickedButton5()
{
	// 沿直线

	Ogre::Vector3 lastAxis = Ogre::Vector3::ZERO;
	Ogre::Radian lastAngle;

	int ii;
	for(int i = 0; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
	{
		bool isEnd = false;
		ii = i + 1;
		if(ii == CKeyFrameDlg::s_Inst.m_KeyList.size())
		{	
			isEnd = true;
			ii = 0;
		}

		Ogre::Quaternion trkQ;

		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
		OTE::COTEKeyFrame* nkf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[ii].Time);

		if(!isEnd)
		{	
			Ogre::Vector3 npt = nkf->m_Position;
			Ogre::Vector3 pt  = kf->m_Position;

			Ogre::Vector3 dir = npt - pt;			

			trkQ = Ogre::Vector3::UNIT_Z.getRotationTo(dir);
						
			Ogre::Vector3 axis;
			Ogre::Radian angle;
			trkQ.ToAngleAxis(angle, axis);		

			if(lastAxis != Ogre::Vector3::ZERO && axis.dotProduct(lastAxis) < 0)
			{		
				angle = Ogre::Radian(Ogre::Degree(360)) - angle;
				axis = - axis;
				trkQ = Ogre::Quaternion(angle, axis);
			}	
			
			if(lastAxis != Ogre::Vector3::ZERO && axis.dotProduct(lastAxis) > 0 && 
				lastAngle.valueDegrees() > 180 && angle.valueDegrees() < 180)
			{
				angle = Ogre::Radian(Ogre::Degree(360)) + angle;							
				axis = - axis;
				trkQ = Ogre::Quaternion(angle, axis);
			}

			lastAxis = axis;
			lastAngle = angle;
		}
		else
		{
			trkQ = Ogre::Quaternion(lastAngle, lastAxis);
		}

		Ogre::Vector3 angles;
		OTE::MathLib::QuaternionToEuler(trkQ, angles);

		CKeyFrameDlg::s_Inst.SetKey(
				CKeyFrameDlg::s_Inst.m_KeyList[i].Time, 
				kf->m_Position, kf->m_Scaling, 
				angles					
			);					
	}
	
	// 第二次调整

	for(int i = 1; i < CKeyFrameDlg::s_Inst.m_KeyList.size(); i ++)
	{	
		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i].Time);
		OTE::COTEKeyFrame* pkf = CKeyFrameDlg::s_Inst.GetKey(CKeyFrameDlg::s_Inst.m_KeyList[i - 1].Time);

		Ogre::Quaternion pq = pkf->m_Quaternion;
		Ogre::Quaternion q  = kf->m_Quaternion;			
		
		Ogre::Quaternion trkQ = Ogre::Quaternion::Slerp(0.5f, q, pq);		
		Ogre::Vector3 angles;
		OTE::MathLib::QuaternionToEuler(m_IsInvCheck.GetCheck() == BST_CHECKED ? -trkQ : trkQ, angles);

		CKeyFrameDlg::s_Inst.SetKey(
				CKeyFrameDlg::s_Inst.m_KeyList[i].Time, 
				kf->m_Position, kf->m_Scaling, 
				angles					
			);	

		if(i == 1)
		{
			Ogre::Vector3 axis;
			Ogre::Radian angle1, angle2;
			trkQ.ToAngleAxis(angle1, axis);	
			pq.ToAngleAxis(angle2, axis);
			if(angle1 > angle2)
				trkQ.FromAngleAxis(angle1 - angle2,  axis);			
			else
				trkQ.FromAngleAxis(angle2 - angle1,  axis);	

			Ogre::Vector3 angles;
			OTE::MathLib::QuaternionToEuler(m_IsInvCheck.GetCheck() == BST_CHECKED ? -trkQ : trkQ, angles);

			if(m_IsCurveInvCheck.GetCheck() == BST_CHECKED)
				angles = -angles;

			CKeyFrameDlg::s_Inst.SetKey(
				CKeyFrameDlg::s_Inst.m_KeyList[i - 1].Time, 
				pkf->m_Position, pkf->m_Scaling, 
				angles					
			);	
			
		}

	}

	if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
}
