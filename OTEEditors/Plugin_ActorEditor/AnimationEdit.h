#pragma once
#include "afxwin.h"

// CAnimationEdit �Ի���

class CAnimationEdit : public CDialog
{
	DECLARE_DYNAMIC(CAnimationEdit)

public:
	CAnimationEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAnimationEdit();

	virtual BOOL OnInitDialog();

	//���´���
	void UpdataAnimation();

	static CAnimationEdit s_Inst;
// �Ի�������
	enum { IDD = IDD_AnimationEdit };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_AnimList;
	afx_msg void OnLbnSelchangeAnimationlist();
	afx_msg void OnLbnDblclkAnimationlist();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedAddtoaction();
};
