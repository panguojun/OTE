#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "tinyxml.h"
#include "OTEActorEntity.h"
#include "OTEQTSceneManager.h"
#include "TransformManager.h"

// CActorEditorMainDlg �Ի���

class CActorEditorMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CActorEditorMainDlg)
public:
	
	static CActorEditorMainDlg				s_Inst;
	static OTE::COTEActorEntity*			s_pActorEntity;
	static CTransformManager				s_TransMgr;			// �任������	
	static	bool							s_StartSelection;

	CTabCtrl								m_TabCtrl;

public:
	CActorEditorMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CActorEditorMainDlg();
	//���ؽ�ɫ
	void LoadEntity(const char* File = "", const char* name = "");
	//���½�ɫ
	void UpdateUI();

// �Ի�������
	enum { IDD = IDD_ActorEditorMain };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:	
	afx_msg void OnBnClickedButton10();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedClose();
	afx_msg void OnCpyMag();
	afx_msg void OnBnClickedSave();
	afx_msg void OnDeleteEffect();
	afx_msg void OnDelAction();
	afx_msg void OnEditEffect();
	afx_msg void OnCopyAction();
	afx_msg void OnImportMagList();
};
