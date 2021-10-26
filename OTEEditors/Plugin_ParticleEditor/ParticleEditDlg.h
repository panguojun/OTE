#pragma once
#include "afxcmn.h"
#include "EditListCtrl.h"
#include "VecEditor.h"

// CParticleEditDlg �Ի���

class CParticleEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleEditDlg)

public:
	static CParticleEditDlg s_Inst;

	CParticleEditDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticleEditDlg();

	BOOL OnInitDialog();

	void UpdatePropertyList();

// �Ի�������
	enum { IDD = IDD_PARTICLE_EDIT_EDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEditListCtrl m_PorpList;
	CVecEditor	m_VectorEditor;

	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
