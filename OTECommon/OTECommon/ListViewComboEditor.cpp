// ListViewComboEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "ListViewComboEditor.h"

CString CListViewComboEditor::s_Text;
int		CListViewComboEditor::s_SelIndex = -1;
// CListViewComboEditor 对话框

IMPLEMENT_DYNAMIC(CListViewComboEditor, CDialog)
CListViewComboEditor::CListViewComboEditor(const CString& rText, 
			const CRect& rRect, const char* szStringList, CWnd* pParent)
	: CDialog(CListViewComboEditor::IDD, pParent)
{
	s_Text = rText;
	s_SelIndex = -1;
	m_Rect = rRect;
	mStringList = szStringList;
}

CListViewComboEditor::~CListViewComboEditor()
{
}

void CListViewComboEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComBox);
}

BOOL CListViewComboEditor::OnInitDialog()
{
	BOOL tRet = CDialog::OnInitDialog();	
	MoveWindow(m_Rect);
	m_ComBox.MoveWindow(CRect(CPoint(0, 0), CSize(m_Rect.Width(), m_Rect.Height() - 2)));
	int pos = 0;
	do
	{
		int nextPos = mStringList.find(";", pos);		
		std::string substr = mStringList.substr(pos, nextPos - pos);
		m_ComBox.AddString(substr.c_str());
		pos = nextPos + 1;
	}
	while(mStringList.find(";", pos) != std::string::npos);

	m_ComBox.SelectString(0, s_Text);
	return tRet;
}

BEGIN_MESSAGE_MAP(CListViewComboEditor, CDialog)
END_MESSAGE_MAP()


// CListViewComboEditor 消息处理程序
void CListViewComboEditor::OnOK()
{	
	m_ComBox.GetWindowText(s_Text);	
	s_SelIndex = m_ComBox.GetCurSel();
	
	CDialog::OnOK();
}