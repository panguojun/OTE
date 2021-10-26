#pragma once


// CActionListDlg 对话框

class CActionListDlg : public CDialog
{
	DECLARE_DYNAMIC(CActionListDlg)

public:
	CActionListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CActionListDlg();

// 对话框数据
	enum { IDD = IDD_ACTIONLISTDLG };

	CListBox m_List;

	//更新列表
	void UpdtatActionList();

	BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnDblclkActlist();
};
