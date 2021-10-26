#pragma once


// CMeshEditDlg �Ի���

class CMeshEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeshEditDlg)

public:
	CMeshEditDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMeshEditDlg();

	static CMeshEditDlg s_Inst;

	void UpdateList();//�������

	virtual BOOL OnInitDialog();

	void OnEndPlugin();

// �Ի�������
	enum { IDD = IDD_MeshEdit };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
