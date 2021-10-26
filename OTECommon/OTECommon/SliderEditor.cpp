// SliderEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "SliderEditor.h"
#include "Resource.h"

// -------------------------------------------
// CSliderEditor 对话框
CSliderEditor	CSliderEditor::s_Inst;
IMPLEMENT_DYNAMIC(CSliderEditor, CDialog)
CSliderEditor::CSliderEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CSliderEditor::IDD, pParent)
{
}

CSliderEditor::~CSliderEditor()
{
}

void CSliderEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderCtrl);
}


BEGIN_MESSAGE_MAP(CSliderEditor, CDialog)

	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_WM_ACTIVATE()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KEYDOWN()
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()

// -------------------------------
CSliderEditor* g_pSliderEditor = NULL;
WNDPROC gOldWndProcB = NULL;

LRESULT CALLBACK WndProcB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	long tResult = 0;

	if ( ::GetKeyState(VK_RETURN) & 0x80 )
	{		
		if(g_pSliderEditor)		
		{
			g_pSliderEditor->OnOK();			
		}
	}
	else
	{
		tResult = (*gOldWndProcB)( hWnd, uMsg, wParam, lParam );
	}

	return tResult;
}
// -------------------------------------------
BOOL CSliderEditor::OnInitDialog()
{
	BOOL tRet = CDialog::OnInitDialog();	

	return tRet;
}
// -------------------------------------------
void CSliderEditor::Show(pCBFun_t pCBFun, pCBUpdateFun_t pCBUFun, CString ud, 
						 float v, float min, float max, CRect rt, float scl)
{	
	if(!this->m_hWnd)
		Create(IDD_SLIDER_EDITOR);
	
	m_Scaler   = scl;

	m_SliderCtrl.SetRange(min * m_Scaler, max * m_Scaler);

	m_SliderCtrl.SetPos(v * m_Scaler);

	m_UserData = ud;
	
	this->ShowWindow(SW_SHOW);
	this->MoveWindow(rt);

	CString str;
	str.Format("%.1f", v);
	SetDlgItemText(IDC_EDIT1, str);	

	m_pCBFun = pCBFun;
	m_pCBUpdateFun = pCBUFun;

	if(!gOldWndProcB)
	{
		gOldWndProcB = (WNDPROC)::SetWindowLongPtr( this->GetDlgItem(IDC_EDIT1)->m_hWnd, GWLP_WNDPROC, (INT_PTR) WndProcB );
	}	
	g_pSliderEditor = this;
}	
// -------------------------------------------
void CSliderEditor::Hide()
{
	if(!this->m_hWnd)
		Create(IDD_SLIDER_EDITOR);

	CDialog::ShowWindow(SW_HIDE);
}
// -------------------------------------------
void CSliderEditor::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	float v = m_SliderCtrl.GetPos() / m_Scaler;
	CString str;
	str.Format("%.1f", v);
	SetDlgItemText(IDC_EDIT1, str);	

	if(m_pCBFun)
		(*m_pCBFun)(v, m_UserData);
}
// -------------------------------------------
void CSliderEditor::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if(nState == WA_INACTIVE && m_pCBFun)
	{
		Hide();
		

		// 暂时
		if(m_pCBUpdateFun && m_pCBFun)
		{
			(*m_pCBUpdateFun)();

		}
		m_pCBFun = NULL;		
	}
}
// -------------------------------------------
void CSliderEditor::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);

}

int CSliderEditor::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
// -------------------------------------------
void CSliderEditor::OnEnChangeEdit1()
{
	CString str;
	GetDlgItemText(IDC_EDIT1, str);

	if(m_pCBFun)
		(*m_pCBFun)(atof((char*)LPCTSTR(str)), m_UserData);
	
}

// -------------------------------------------
void CSliderEditor::OnOK()
{	
	g_pSliderEditor = NULL;
	
	ShowWindow(SW_HIDE);
	InvalidateRect(NULL, true);
}

// -------------------------------------------
void CSliderEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSliderEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}
