#pragma once


// CActionListDlg �Ի���

class CActionListDlg : public CDialog
{
	DECLARE_DYNAMIC(CActionListDlg)

public:
	CActionListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CActionListDlg();

// �Ի�������
	enum { IDD = IDD_ACTIONLISTDLG };

	CListBox m_List;

	//�����б�
	void UpdtatActionList();

	BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnDblclkActlist();
};
