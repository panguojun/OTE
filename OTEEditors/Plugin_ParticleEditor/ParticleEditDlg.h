#pragma once
#include "afxcmn.h"
#include "EditListCtrl.h"
#include "VecEditor.h"

// CParticleEditDlg 对话框

class CParticleEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleEditDlg)

public:
	static CParticleEditDlg s_Inst;

	CParticleEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticleEditDlg();

	BOOL OnInitDialog();

	void UpdatePropertyList();

// 对话框数据
	enum { IDD = IDD_PARTICLE_EDIT_EDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEditListCtrl m_PorpList;
	CVecEditor	m_VectorEditor;

	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
