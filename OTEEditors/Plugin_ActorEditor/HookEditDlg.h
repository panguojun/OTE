#pragma once

#include "OTEActorEntity.h"
#include "afxwin.h"

// CHookEditDlg �Ի���
class CHookEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CHookEditDlg)

public:
	CHookEditDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHookEditDlg();

	CListBox m_HookList;

	OTE::Hook_t* mHook;//��ǰ�ҵ�

	std::string TestObjFile;

	static CHookEditDlg s_Inst;

	//���¹ҵ�
	void UpdateHookList();

	//���¹ҵ���Ϣ
	void UpdateHInfomation();

	//���عҵ���Ϣ
	void Load();

	//����ҵ���Ϣ
	void Save();
	
	void OnMouseMove(int x, int y);

	void OnSceneLButtonUp(float x, float y);

	void OnSceneLButtonDown(float x, float y);

	void UpdateDummyPos();

// �Ի�������
	enum { IDD = IDD_HookEdit };
public:

	bool		m_WaitForPickBone;

protected:
	HashMap<std::string, OTE::Hook_t*> *m_pActorHookList;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadtest();
	afx_msg void OnLbnSelchangePothooklist();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDeltest();
	CButton m_IsShowBone;
	afx_msg void OnBnClickedCheck2();
};
