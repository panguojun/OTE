#pragma once
#include "VegBrush.h"
#include "afxwin.h"
#include "OgreNoMemoryMacros.h"
#include "texture.h"
#include "OgreMemoryMacros.h"

// CGrassEditorDlg 对话框

class CGrassEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CGrassEditorDlg)

public:
	CGrassEditorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGrassEditorDlg();

// 对话框数据
	enum { IDD = IDD_VEGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

	void OnMouseMove(float x, float y, float width, float height);

	virtual BOOL OnInitDialog();

public:
	static CGrassEditorDlg s_Inst;

	CVegBrush		m_VegBrush;
	int				m_IsBrush;
	
	CTexture		m_GrassPreTex;

	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	CStatic m_PrePic;
	afx_msg void OnStnClickedPrepic();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
};
