#pragma once

#include "OTEActorEntity.h"
#include "afxwin.h"

// CHookEditDlg 对话框
class CHookEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CHookEditDlg)

public:
	CHookEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHookEditDlg();

	CListBox m_HookList;

	OTE::Hook_t* mHook;//当前挂点

	std::string TestObjFile;

	static CHookEditDlg s_Inst;

	//更新挂点
	void UpdateHookList();

	//更新挂点信息
	void UpdateHInfomation();

	//加载挂点信息
	void Load();

	//保存挂点信息
	void Save();
	
	void OnMouseMove(int x, int y);

	void OnSceneLButtonUp(float x, float y);

	void OnSceneLButtonDown(float x, float y);

	void UpdateDummyPos();

// 对话框数据
	enum { IDD = IDD_HookEdit };
public:

	bool		m_WaitForPickBone;

protected:
	HashMap<std::string, OTE::Hook_t*> *m_pActorHookList;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
