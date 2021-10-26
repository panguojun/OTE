#pragma once
#include "afxcmn.h"

#include "resource.h"

// CMeshListDlg 对话框

class CMeshListDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeshListDlg)

public:
	CMeshListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMeshListDlg();

// 对话框数据
	enum { IDD = IDD_MESHLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();

public:
	static CString s_MouseMesh;

public:	
	CListCtrl m_MeshList;
	afx_msg void OnBnClickedOk();


	CTreeCtrl m_MeshTreeList;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
