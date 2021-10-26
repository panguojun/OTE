// BrushDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrinEditor.h"
#include "BrushDlg.h"
#include ".\brushdlg.h"


// CBrushDlg 对话框
CBrushDlg CBrushDlg::s_Inst;
IMPLEMENT_DYNAMIC(CBrushDlg, CDialog)
CBrushDlg::CBrushDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushDlg::IDD, pParent)
{
}

CBrushDlg::~CBrushDlg()
{
}

void CBrushDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_OutterSlider);
	DDX_Control(pDX, IDC_SLIDER2, m_InnerSlider);
	DDX_Control(pDX, IDC_SLIDER3, m_Strength);
}


BEGIN_MESSAGE_MAP(CBrushDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
	
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, OnNMCustomdrawSlider3)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER3, OnNMReleasedcaptureSlider3)
END_MESSAGE_MAP()


// CBrushDlg 消息处理程序

void CBrushDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
}

void CBrushDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}

// -----------------------------------------------------
BOOL CBrushDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	m_InnerSlider.SetRange(0, 100);
	m_OutterSlider.SetRange(0, 100);

	return tRet;
}

void CBrushDlg::OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}

void CBrushDlg::OnNMThemeChangedSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}
// ----------------------------------------------------
void CBrushDlg::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	float scale = 100.0f;
	if(m_pBrushEntity)
	  m_pBrushEntity->m_InnerRad = (float)(m_InnerSlider.GetPos() - m_InnerSlider.GetRangeMin()) / (m_InnerSlider.GetRangeMax() - m_InnerSlider.GetRangeMin())  * scale;

}
// ----------------------------------------------------
void CBrushDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	float scale = 100.0f;
	if(m_pBrushEntity)
		m_pBrushEntity->m_OutterRad = (float)(m_OutterSlider.GetPos() - m_OutterSlider.GetRangeMin()) / (m_OutterSlider.GetRangeMax() - m_OutterSlider.GetRangeMin()) * scale;

}

void CBrushDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CBrushDlg::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CBrushDlg::OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	float scale = 100.0f;
	if(m_pBrushEntity)
	  m_pBrushEntity->m_Strength = (float)(m_Strength.GetPos() - m_Strength.GetRangeMin()) / (m_InnerSlider.GetRangeMax() - m_Strength.GetRangeMin())  * scale;

}
