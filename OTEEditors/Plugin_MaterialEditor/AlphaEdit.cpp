// AlphaEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "AlphaEdit.h"

CAlphaEdit CAlphaEdit::s_Inst;

// CAlphaEdit 对话框

IMPLEMENT_DYNAMIC(CAlphaEdit, CDialog)
CAlphaEdit::CAlphaEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAlphaEdit::IDD, pParent) 
{
	isInit = false;
}

CAlphaEdit::~CAlphaEdit()
{
}

void CAlphaEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_isAlphaTest);
	DDX_Control(pDX, IDC_CHECK3, m_isDepTest);
	DDX_Control(pDX, IDC_COMBO2, m_AlphaTestFormula);
	DDX_Control(pDX, IDC_COMBO3, m_AlphaBlendFormula);
	DDX_Control(pDX, IDC_COMBO4, m_DepTestFormula);
	DDX_Control(pDX, IDC_COMBO1, m_HightLight);
}


BEGIN_MESSAGE_MAP(CAlphaEdit, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeCombo2)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnEnKillfocusEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnCbnSelchangeCombo4)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnCbnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

//更新
void CAlphaEdit::UpdataAlpha(Ogre::Material *pMat)
{
	m_pMat = pMat;

	if(!isInit)
	{
		m_AlphaTestFormula.InsertString(0, "always_fail");
		m_AlphaTestFormula.InsertString(1, "always_pass");
		m_AlphaTestFormula.InsertString(2, "less");
		m_AlphaTestFormula.InsertString(3, "less_equal");
		m_AlphaTestFormula.InsertString(4, "equal");
		m_AlphaTestFormula.InsertString(5, "not_equal");
		m_AlphaTestFormula.InsertString(6, "greater_equal");
		m_AlphaTestFormula.InsertString(7, "greater");
		
		m_AlphaBlendFormula.InsertString(0, "无效果");
		m_AlphaBlendFormula.InsertString(1, "叠加");
		m_AlphaBlendFormula.InsertString(2, "相乘");
		m_AlphaBlendFormula.InsertString(3, "Alpha混合");
		m_AlphaBlendFormula.InsertString(4, "其它");
		
		m_DepTestFormula.InsertString(0, "always_fail");
		m_DepTestFormula.InsertString(1, "always_pass");
		m_DepTestFormula.InsertString(2, "less");
		m_DepTestFormula.InsertString(3, "less_equal");
		m_DepTestFormula.InsertString(4, "equal");
		m_DepTestFormula.InsertString(5, "not_equal");
		m_DepTestFormula.InsertString(6, "greater_equal");
		m_DepTestFormula.InsertString(7, "greater");		

		isInit = true;
	}

	m_isDepTest.SetCheck(0);
	m_isAlphaTest.SetCheck(0);

	GetDlgItem(IDC_EDIT1)->EnableWindow(false);
	GetDlgItem(IDC_COMBO4)->EnableWindow(false);
	GetDlgItem(IDC_COMBO3)->EnableWindow(false);
	GetDlgItem(IDC_COMBO2)->EnableWindow(false);

	if(m_pMat != NULL)
        m_pPass = m_pMat->getTechnique(0)->getPass(0);
	else
		m_pPass = NULL;

	if(m_pPass != NULL)
	{
		GetDlgItem(IDC_COMBO3)->EnableWindow(true);

		if(m_pPass->getDepthCheckEnabled())
		{
			m_isDepTest.SetCheck(1);
			GetDlgItem(IDC_COMBO4)->EnableWindow(true);
		}
		else
		{
			m_isDepTest.SetCheck(0);
			GetDlgItem(IDC_COMBO4)->EnableWindow(false);
		}
		m_DepTestFormula.SetCurSel(m_pPass->getDepthFunction());

		if(m_pPass->getSourceBlendFactor() == Ogre::SBF_ONE && m_pPass->getDestBlendFactor() == Ogre::SBF_ZERO)
		{
			m_AlphaBlendFormula.SetCurSel(0);
		}
		else if(m_pPass->getSourceBlendFactor() == Ogre::SBF_ONE && m_pPass->getDestBlendFactor() == Ogre::SBF_ONE)
		{
			m_AlphaBlendFormula.SetCurSel(1);
		}
		else if(m_pPass->getSourceBlendFactor() == Ogre::SBF_SOURCE_COLOUR && m_pPass->getDestBlendFactor() == Ogre::SBF_ONE_MINUS_SOURCE_COLOUR)
		{
			m_AlphaBlendFormula.SetCurSel(2);
		}
		else if(m_pPass->getSourceBlendFactor() == Ogre::SBF_SOURCE_ALPHA && m_pPass->getDestBlendFactor() == Ogre::SBF_ONE_MINUS_SOURCE_ALPHA)
		{
			m_AlphaBlendFormula.SetCurSel(3);
		}
		else
		{
			m_AlphaBlendFormula.SetCurSel(4);
		}

		if(m_pPass->getAlphaRejectFunction() != Ogre::CMPF_ALWAYS_PASS || 
			m_pPass->getAlphaRejectValue() != 0)
		{
			m_isAlphaTest.SetCheck(1);			
			GetDlgItem(IDC_COMBO2)->EnableWindow(true);
			GetDlgItem(IDC_EDIT1)->EnableWindow(true);
			m_AlphaTestFormula.SetCurSel(m_pPass->getAlphaRejectFunction());
			char buf[32];
			itoa(m_pPass->getAlphaRejectValue(), buf, 10);
			SetDlgItemText(IDC_EDIT1, buf);
		}
		else
		{
			m_isAlphaTest.SetCheck(0);
			GetDlgItem(IDC_COMBO2)->EnableWindow(false);
			GetDlgItem(IDC_EDIT1)->EnableWindow(false);
		}
	
	}
}

//设置AlphaTestFunc
void CAlphaEdit::SetAlphaTestFunc()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;
	
	switch(m_AlphaTestFormula.GetCurSel())
	{
	case 0:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_ALWAYS_FAIL);
		break;
	default:
	case 1:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_ALWAYS_PASS);
		break;
	case 2:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_LESS);
		break;
	case 3:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_LESS_EQUAL);
		break;
	case 4:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_EQUAL);
		break;
	case 5:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_NOT_EQUAL);
		break;
	case 6:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_GREATER_EQUAL);
		break;
	case 7:
		m_pPass->setAlphaRejectFunction(Ogre::CMPF_GREATER);
		break;
	}
}

// Alpha测试
void CAlphaEdit::OnBnClickedCheck1()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	if(m_isAlphaTest.GetCheck())
	{
		CString buf;
		GetDlgItemText(IDC_EDIT1, buf);
		unsigned char val = atoi(buf.GetString());
		if(val <= 0)
			val = 128;
		SetAlphaTestFunc();
		m_pPass->setAlphaRejectValue(val);
	}
	else
	{
		m_pPass->setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS , 0);
	}

	UpdataAlpha(m_pMat);
}

//深度测试
void CAlphaEdit::OnBnClickedCheck3()
{	
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	if(m_isDepTest.GetCheck())
	{
		m_pPass->setDepthCheckEnabled(true);
	}
	else
	{
		m_pPass->setDepthCheckEnabled(false);
	}

	UpdataAlpha(m_pMat);
}
//选取Alpha测试函数
void CAlphaEdit::OnCbnSelchangeCombo2()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	SetAlphaTestFunc();

	UpdataAlpha(m_pMat);
}
//更改AlphaTest参考值
void CAlphaEdit::OnEnKillfocusEdit1()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	CString buf;
	GetDlgItemText(IDC_EDIT1, buf);
	unsigned char val = atoi(buf.GetString());
	m_pPass->setAlphaRejectValue(val);

	UpdataAlpha(m_pMat);
}
//更改DepTest函数
void CAlphaEdit::OnCbnSelchangeCombo4()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	switch(m_DepTestFormula.GetCurSel())
	{
	case 0:
		m_pPass->setDepthFunction(Ogre::CMPF_ALWAYS_FAIL);
		break;
	case 1:
		m_pPass->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
		break;
	case 2:
		m_pPass->setDepthFunction(Ogre::CMPF_LESS);
		break;
	default:
	case 3:
		m_pPass->setDepthFunction(Ogre::CMPF_LESS_EQUAL);
		break;
	case 4:
		m_pPass->setDepthFunction(Ogre::CMPF_EQUAL);
		break;
	case 5:
		m_pPass->setDepthFunction(Ogre::CMPF_NOT_EQUAL);
		break;
	case 6:
		m_pPass->setDepthFunction(Ogre::CMPF_GREATER_EQUAL);
		break;
	case 7:
		m_pPass->setDepthFunction(Ogre::CMPF_GREATER);
		break;
	}

	UpdataAlpha(m_pMat);
}
//更改场景混合模式
void CAlphaEdit::OnCbnSelchangeCombo3()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	switch(m_AlphaBlendFormula.GetCurSel())
	{
	default:
	case 0:
		m_pPass->setSceneBlending(Ogre::SBF_ONE, Ogre::SBF_ZERO);
		break;
	case 1:
		m_pPass->setSceneBlending(Ogre::SBF_ONE, Ogre::SBF_ONE);
		break;
	case 2:
		m_pPass->setSceneBlending(Ogre::SBF_SOURCE_COLOUR, Ogre::SBF_ONE_MINUS_SOURCE_COLOUR);
		break;
	case 3:
		m_pPass->setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
		break;
	}

	UpdataAlpha(m_pMat);
}

void CAlphaEdit::OnCbnSelchangeCombo1()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	switch(m_HightLight.GetCurSel())
	{
	default:
	case 2:
		m_pPass->getTextureUnitState(0)->setColourOperationEx(Ogre::LBX_MODULATE);
		break;
	case 0:
		m_pPass->getTextureUnitState(0)->setColourOperationEx(Ogre::LBX_MODULATE_X2);
		break;
	case 1:
		m_pPass->getTextureUnitState(0)->setColourOperationEx(Ogre::LBX_MODULATE_X4);
		break;
	}
	
}
