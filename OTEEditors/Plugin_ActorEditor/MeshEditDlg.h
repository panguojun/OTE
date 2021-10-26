#pragma once


// CMeshEditDlg 对话框

class CMeshEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeshEditDlg)

public:
	CMeshEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMeshEditDlg();

	static CMeshEditDlg s_Inst;

	void UpdateList();//窗体更新

	virtual BOOL OnInitDialog();

	void OnEndPlugin();

// 对话框数据
	enum { IDD = IDD_MeshEdit };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedHide();
	afx_msg void OnLbnSelchangeMeshlist();
	afx_msg void OnBnClickedShow();
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedReplace();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedRename();
	afx_msg void OnBnClickedSetmaterial();

	CListBox m_EquipmentList;
};
