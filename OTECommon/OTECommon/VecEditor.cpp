// VecEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "VecEditor.h"
#include "commctrl.h"

// CVecEditor 对话框

IMPLEMENT_DYNAMIC(CVecEditor, CDialog)
CVecEditor::CVecEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CVecEditor::IDD, pParent)
{
	m_pCBFun = NULL;
}

CVecEditor::~CVecEditor()
{
}

void CVecEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderCtrl1);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderCtrl2);
	DDX_Control(pDX, IDC_SLIDER3, m_SliderCtrl3);
}


BEGIN_MESSAGE_MAP(CVecEditor, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER3, OnNMReleasedcaptureSlider3)
//	ON_NOTIFY(NM_THEMECHANGED, IDC_SLIDER1, OnNMThemeChangedSlider1)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, OnNMCustomdrawSlider3)
END_MESSAGE_MAP()

// ----------------------------------------
BOOL CVecEditor::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	return ret;
}

// CVecEditor 消息处理程序

void CVecEditor::OnEnChangeEdit1()
{
	char str[32];
	float x, y, z;
	this->GetDlgItemText(IDC_EDIT1, str, 32);	x = atof(str);
	this->GetDlgItemText(IDC_EDIT2, str, 32);	y = atof(str);
	this->GetDlgItemText(IDC_EDIT3, str, 32);	z = atof(str);
	
	if(m_pCBFun)
		(*m_pCBFun)(x, y, z);
}

void CVecEditor::OnEnChangeEdit2()
{
	char str[32];
	float x, y, z;
	this->GetDlgItemText(IDC_EDIT1, str, 32);	x = atof(str);
	this->GetDlgItemText(IDC_EDIT2, str, 32);	y = atof(str);
	this->GetDlgItemText(IDC_EDIT3, str, 32);	z = atof(str);
	
	if(m_pCBFun)
		(*m_pCBFun)(x, y, z);
}

void CVecEditor::OnEnChangeEdit3()
{
	char str[32];
	float x, y, z;
	this->GetDlgItemText(IDC_EDIT1, str, 32);	x = atof(str);
	this->GetDlgItemText(IDC_EDIT2, str, 32);	y = atof(str);
	this->GetDlgItemText(IDC_EDIT3, str, 32);	z = atof(str);
	
	if(m_pCBFun)
		(*m_pCBFun)(x, y, z);
}

void CVecEditor::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	char str[32];	
	float val = m_SliderCtrl1.GetPos() / 10.0f;
	sprintf(str, "%.2f", val);
	SetDlgItemText(IDC_EDIT1, str);
}

void CVecEditor::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	char str[32];	
	float val = m_SliderCtrl2.GetPos() / 10.0f;
	sprintf(str, "%.2f", val);
	SetDlgItemText(IDC_EDIT2, str);
}

void CVecEditor::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	

}

void CVecEditor::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{

}

void CVecEditor::OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{

}

// ---------------------------------------------
void CVecEditor::Show(pCBFun_t pCBFun, float x, float y, float z, float min, float max, 
											CString caption, CString xLabel, CString yLabel, CString zLabel)
{
	m_pCBFun = pCBFun;
	if(!this->m_hWnd)
		Create(IDD_VEC_DLG);
		
	char str[32];		
	sprintf(str, "%.2f", x);
	SetDlgItemText(IDC_EDIT1, str);
	sprintf(str, "%.2f", y);
	SetDlgItemText(IDC_EDIT2, str);
	sprintf(str, "%.2f", z);
	SetDlgItemText(IDC_EDIT3, str);

	m_SliderCtrl1.SetRange(min * 10, max * 10);
	m_SliderCtrl2.SetRange(min * 10, max * 10);
	m_SliderCtrl3.SetRange(min * 10, max * 10);

	m_SliderCtrl1.SetPos(x * 10);
	m_SliderCtrl2.SetPos(y * 10);
	m_SliderCtrl3.SetPos(z * 10);

	SetWindowText(caption);
	SetDlgItemText(IDC_STATIC_X, xLabel);
	SetDlgItemText(IDC_STATIC_Y, yLabel);
	SetDlgItemText(IDC_STATIC_Z, zLabel);

	this->ShowWindow(SW_SHOW);
}

// ---------------------------------------------
void CVecEditor::Update(float x, float y, float z)
{
	if(!this->m_hWnd)
		Create(IDD_VEC_DLG);

	char str[32];		
	sprintf(str, "%.2f", x);
	SetDlgItemText(IDC_EDIT1, str);
	sprintf(str, "%.2f", y);
	SetDlgItemText(IDC_EDIT2, str);
	sprintf(str, "%.2f", z);
	SetDlgItemText(IDC_EDIT3, str);

	m_SliderCtrl1.SetPos(x * 10);
	m_SliderCtrl2.SetPos(y * 10);
	m_SliderCtrl3.SetPos(z * 10);

}

// ---------------------------------------------
void CVecEditor::Hide()
{
	if(this->m_hWnd)	
		this->ShowWindow(SW_HIDE);	
}

// ---------------------------------------------
void CVecEditor::OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{

}

void CVecEditor::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{	
	char str[32];	
	float val = m_SliderCtrl3.GetPos() / 10.0f;
	sprintf(str, "%.2f", val);
	SetDlgItemText(IDC_EDIT3, str);

}
