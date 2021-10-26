#pragma once
#include "afxcmn.h"
#include "resource.h"

// CParticleEditMainDlg 对话框

class CParticleEditMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleEditMainDlg)

public:
	static CParticleEditMainDlg	s_Inst;
	CParticleEditMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticleEditMainDlg();

// 对话框数据
	enum { IDD = IDD_PARTICLE_EDIT_MAINDLG };

	BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabCtrl;
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedClose();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
