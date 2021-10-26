// ListViewEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "ListViewEditor.h"

// -------------------------------
// CListViewEditor 对话框
CString CListViewEditor::s_Text;

IMPLEMENT_DYNAMIC(CListViewEditor, CDialog)
CListViewEditor::CListViewEditor(int nType, const CString& rText, const CRect& rRect, int NumUpper, int NumLower, CWnd* pParent /*=NULL*/, 
								 NotifyFuncHdl_t fun, OKFuncHdl_t okfun)
	: CDialog(CListViewEditor::IDD, pParent)
{
	s_Text = rText;
	m_Rect = rRect;	
	m_Type = nType;
	m_NumUpper = NumUpper;
	m_NumLower = NumLower;
	m_pNotifyFuncHdl = fun;
	m_pOKFunHdl = okfun;
	m_PtrOffset = 0;
	m_userState = 0;
}

CListViewEditor::~CListViewEditor()
{
}

void CListViewEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditBox);
	DDX_Control(pDX, IDC_SPIN1, m_SpinBtn);
}


BEGIN_MESSAGE_MAP(CListViewEditor, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
	ON_EN_KILLFOCUS(IDC_EDIT1, OnEnKillfocusEdit1)
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// -------------------------------
// CListViewEditor 消息处理程序

void CListViewEditor::OnEnChangeEdit1()
{
	CString str;
	m_EditBox.GetWindowText(str);
	//if(atoi(str) < m_NumLower)	m_EditBox.SetWindowText(Str(m_NumLower).c_str());
	//if(atoi(str) > m_NumUpper)	m_EditBox.SetWindowText(Str(m_NumUpper).c_str());

	if(m_pNotifyFuncHdl)
	{
		(*m_pNotifyFuncHdl)( m_PtrOffset, (char*)LPCTSTR(str) );
	}
}

// -------------------------------
CListViewEditor* g_pListViewEditor = NULL;
WNDPROC gOldWndProcA;
LRESULT CALLBACK WndProcA( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	long tResult = 0;

	if ( uMsg == 0x86)
	{		
		if(g_pListViewEditor && g_pListViewEditor->m_userState == 0)
		{
			CRect rect;
			g_pListViewEditor->GetWindowRect(rect);
			CPoint mousePnt;
			::GetCursorPos(&mousePnt);

			if(!rect.PtInRect(mousePnt))
			{
				g_pListViewEditor->OprOK();
			}
		}
	}
	else
	{
		tResult = (*gOldWndProcA)( hWnd, uMsg, wParam, lParam );
	}

	return tResult;
}


// -------------------------------
// 初始化

BOOL CListViewEditor::OnInitDialog()
{
	BOOL tRet = CDialog::OnInitDialog();	
	
	MoveWindow(m_Rect);
	m_EditBox.MoveWindow(CRect(CPoint(0, 0), CSize(m_Rect.Width(), m_Rect.Height())));
	CRect tEditRect;	m_EditBox.GetRect(tEditRect);
	m_SpinBtn.MoveWindow(CRect(CPoint(tEditRect.top, tEditRect.right), 	CSize(20, 25)));

	m_EditBox.SetWindowText(s_Text);

	if(m_Type == eNumberEditor)
	{
		m_SpinBtn.SetBuddy(&m_EditBox);
		m_SpinBtn.SetRange(m_NumLower, m_NumUpper);
	}

	gOldWndProcA = (WNDPROC)::SetWindowLongPtr( this->m_hWnd, GWLP_WNDPROC, (INT_PTR) WndProcA );
	
	g_pListViewEditor = this;
	m_userState = 0;

	return tRet;
}

// -------------------------------
// OK
void CListViewEditor::OprOK()
{
	m_userState = 1;
	m_EditBox.GetWindowText(s_Text);

	CDialog::OnOK();
	InvalidateRect(NULL, true);

	if(m_pOKFunHdl)
		(*m_pOKFunHdl)();
}

void CListViewEditor::OnOK()
{	
	OprOK();
}

// -------------------------------
void CListViewEditor::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
// -------------------------------
void CListViewEditor::SetCallBackFun(NotifyFuncHdl_t fun)
{
	m_pNotifyFuncHdl = fun;
}

// -------------------------------
void CListViewEditor::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);	
}

void CListViewEditor::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	//this->ShowWindow(SW_HIDE);
}
// -------------------------------
void CListViewEditor::OnEnKillfocusEdit1()
{
	this->ShowWindow(SW_HIDE);
	InvalidateRect(NULL);

	if(m_pOKFunHdl)
		(*m_pOKFunHdl)();
}

// -------------------------------
int CListViewEditor::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
// -------------------------------
void CListViewEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);
}


