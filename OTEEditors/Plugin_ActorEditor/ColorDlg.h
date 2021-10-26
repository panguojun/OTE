#pragma once

#include "ColorSpectrum.h"
#include "afxwin.h"

// CColorDlg 对话框

class CColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorDlg)


public:
	CColorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CColorDlg();

	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_COLORDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

	void Show(bool vis, unsigned int color = 0);

	
	void SetNotifyFunc(NotifyFuncHdl_t fun)
	{
		m_ColorPicker.m_pNotifyFuncHdl = fun;
		
	}

	afx_msg void OnStnClickedColorStatic();


	afx_msg void OnPaint();
	afx_msg void OnStnClickedColorBarStatic();
	LRESULT OnSelChange( WPARAM wParam, LPARAM lParam );

public:
	
	static CColorDlg	s_Inst;

	ColorSpectrum 	m_ColorPicker;
		
	int m_iRedText;
	int m_iGreenText;
	int m_iBlueText;

	// 记录初始值
	int m_iRedText0;
	int m_iGreenText0;
	int m_iBlueText0;

	COLORREF m_crCurrent;
	COLORREF m_crOldColor;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeRedEdit();
	CEdit m_RedText;
	CEdit m_GreenText;
	CEdit m_BlueText;
	afx_msg void OnEnChangeGreenEdit();
	afx_msg void OnEnChangeBlueEdit();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
