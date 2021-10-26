#pragma once

#include "resource.h"
#include "OTEActorEntity.h"
#include "EditListCtrl.h"

// CMaterialEditorMainDlg �Ի���

class CMaterialEditorMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMaterialEditorMainDlg)

public:
	CMaterialEditorMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMaterialEditorMainDlg();

	static CMaterialEditorMainDlg s_Inst;
// �Ի�������
	enum { IDD = IDD_MeterialEditor };

	static Ogre::MovableObject *s_pObj;

	CEditListCtrl mMatList;
	
	Ogre::Material *m_pMat;
	CString			m_SubName;
	//�����������
	void UpdataAbout();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	CTabCtrl m_TabCtrl;
	bool isInited;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpen();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickMateriallist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSave();
	afx_msg void OnEnKillfocusMaterialname();
	afx_msg void OnBnClickedButton1();
};
