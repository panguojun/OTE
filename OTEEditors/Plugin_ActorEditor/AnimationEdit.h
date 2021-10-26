#pragma once
#include "afxwin.h"

// CAnimationEdit 对话框

class CAnimationEdit : public CDialog
{
	DECLARE_DYNAMIC(CAnimationEdit)

public:
	CAnimationEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAnimationEdit();

	virtual BOOL OnInitDialog();

	//更新窗口
	void UpdataAnimation();

	static CAnimationEdit s_Inst;
// 对话框数据
	enum { IDD = IDD_AnimationEdit };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
