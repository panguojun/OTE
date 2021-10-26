#pragma once
#include "Texture.h"
#include "Resource.h"
#include "afxwin.h"
#include "OTEKeyFrame.h"


// CFrameAnimEditor 对话框

class CFrameAnimEditor : public CDialog
{
	DECLARE_DYNAMIC(CFrameAnimEditor)

public:
	static CFrameAnimEditor s_Inst;
	CFrameAnimEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFrameAnimEditor();
	
	virtual BOOL OnInitDialog();

	void Update();

// 对话框数据
	enum { IDD = IDD_FRAMEANIM_DLG };

public:

	CTexture	m_Texture;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	CStatic m_AnimPic;
	afx_msg void OnPaint();

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnEnChangeFilenameEdit();
	CListBox m_FileNameList;
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnLbnDblclkList2();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnLbnSelchangeList2();
};
