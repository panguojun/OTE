#pragma once
#include "afxcmn.h"


// CEntEditDlg 对话框

class CEntEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CEntEditDlg)
public:
	static CEntEditDlg		sInst;

public:
	CEntEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEntEditDlg();

// 对话框数据
	enum { IDD = IDD_EDIT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_SceneTreeCtrl;

public:
	void DeleteCur();

	void UpdateSceneTree();

	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
