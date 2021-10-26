#pragma once
#include "afxcmn.h"
#include "EditListCtrl.h"
#include "Ogre.h"


// CEntPorpDlg 对话框
class CEntPorpDlg : public CDialog
{
	DECLARE_DYNAMIC(CEntPorpDlg)
public:
	static CEntPorpDlg sInst;
	CListBox mActList;
public:
	CEntPorpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEntPorpDlg();

// 对话框数据
	enum { IDD = IDD_PP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();

public:
	void UpdateEntityPropertyList(Ogre::MovableObject* e);


public:
	CString		m_CurEntName;
	CEditListCtrl m_EntPorpList;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnDblclkActlist();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
