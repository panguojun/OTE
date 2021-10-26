// OtherEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "OtherEdit.h"
#include "ShaderFXEditor.h"
#include "ShaderFXEditor.h"
#include "MaterialEditorMainDlg.h"

#include "OgreHighLevelGpuProgramManager.h"
#include "OgreGpuProgram.h"
#include "OTEShader.h"

using namespace Ogre;
using namespace OTE;

COtherEdit COtherEdit::s_Inst;
// COtherEdit 对话框

IMPLEMENT_DYNAMIC(COtherEdit, CDialog)
COtherEdit::COtherEdit(CWnd* pParent /*=NULL*/)
	: CDialog(COtherEdit::IDD, pParent)
{
}

COtherEdit::~COtherEdit()
{
}

void COtherEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK2, m_isUseCull);
	DDX_Control(pDX, IDC_RADIO1, m_isClockWise);
	DDX_Control(pDX, IDC_RADIO2, m_isAntiClockWise);
}


BEGIN_MESSAGE_MAP(COtherEdit, CDialog)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


//更新
void COtherEdit::UpdataOther(Ogre::Material *pMat)
{
	m_pMat = pMat;

	if(m_pMat != NULL)
        m_pPass = m_pMat->getTechnique(0)->getPass(0);
	else
		m_pPass = NULL;

	m_isUseCull.SetCheck(0);
	m_isClockWise.SetCheck(0);
	m_isAntiClockWise.SetCheck(0);

	GetDlgItem(IDC_CHECK2)->EnableWindow(false);
	GetDlgItem(IDC_RADIO1)->EnableWindow(false);
	GetDlgItem(IDC_RADIO2)->EnableWindow(false);

	if(m_pPass != NULL)
	{
		GetDlgItem(IDC_CHECK2)->EnableWindow(true);
		
		if(m_pPass->getCullingMode() != Ogre::CULL_NONE)
		{
			m_isUseCull.SetCheck(1);
			GetDlgItem(IDC_RADIO1)->EnableWindow(true);
			GetDlgItem(IDC_RADIO2)->EnableWindow(true);

			m_isClockWise.SetCheck(m_pPass->getCullingMode() == Ogre::CULL_CLOCKWISE);
			m_isAntiClockWise.SetCheck(m_pPass->getCullingMode() == Ogre::CULL_ANTICLOCKWISE);
		}
	}
}
//打开/关闭剪裁模式
void COtherEdit::OnBnClickedCheck2()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	if(m_isUseCull.GetCheck())
	{
		m_pPass->setCullingMode(Ogre::CULL_CLOCKWISE);
	}
	else
	{
		m_pPass->setCullingMode(Ogre::CULL_NONE);
	}

	UpdataOther(m_pMat);
}
//使用顺时针剪裁
void COtherEdit::OnBnClickedRadio1()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	if(m_isUseCull.GetCheck())
	{
		m_pPass->setCullingMode(Ogre::CULL_CLOCKWISE);
	}

	UpdataOther(m_pMat);
}
//使用逆时针剪裁
void COtherEdit::OnBnClickedRadio2()
{
	if(m_pMat == NULL || m_pPass == NULL)
		return;

	if(m_isUseCull.GetCheck())
	{
		m_pPass->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
	}

	UpdataOther(m_pMat);
}

void COtherEdit::OnBnClickedButton1()
{
	CShaderFXEditor sfe;
	sfe.DoModal();

	
}
