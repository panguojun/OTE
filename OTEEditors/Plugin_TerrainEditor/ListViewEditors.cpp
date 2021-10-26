// ListViewEditors.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "ListViewEditors.h"
#include ".\listvieweditors.h"


// CListViewEditors �Ի���
CString CListViewEditors::s_Text;

IMPLEMENT_DYNAMIC(CListViewEditors, CDialog)
CListViewEditors::CListViewEditors(int nType, const CString& rText, const CRect& rRect, int NumUpper, int NumLower, CWnd* pParent /*=NULL*/)
	: CDialog(CListViewEditors::IDD, pParent)
{
	s_Text = rText;
	m_Rect = rRect;	
	m_Type = nType;
	m_NumUpper = NumUpper;
	m_NumLower = NumLower;
}

CListViewEditors::~CListViewEditors()
{
}

void CListViewEditors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditBox);
	DDX_Control(pDX, IDC_SPIN1, m_SpinBtn);
}


BEGIN_MESSAGE_MAP(CListViewEditors, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()

BOOL CListViewEditors::OnInitDialog()
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

	return tRet;
}

void CListViewEditors::OnOK()
{	
	m_EditBox.GetWindowText(s_Text);	
	CDialog::OnOK();
	InvalidateRect(NULL, true);
}


// CListViewEditors ��Ϣ�������

void CListViewEditors::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CListViewEditors::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString str;
	m_EditBox.GetWindowText(str);
}
