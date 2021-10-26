#pragma once

#include "Resource.h"
#include <list>
#include <string>
#include "afxwin.h"

// CSoundDlg 对话框

class CSoundDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoundDlg)

public:
	static CSoundDlg s_Inst;

	CSoundDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSoundDlg();

// 对话框数据
	enum { IDD = IDD_SOUND_DLG };

	void InitSoundList();

	void UpdateSoundList();
	
	virtual BOOL OnInitDialog();

	std::list<std::string>   m_SoundList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	CEdit m_PathEdit;

	afx_msg void OnLbnSelchangeList1();
	CListBox m_SoundListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheck1();
	CButton m_CheckIsLoop;
};
