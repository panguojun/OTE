#pragma once
#include "LightmapBrush.h"
#include "afxwin.h"
#include "afxcmn.h"

// CLightmapEditDlg 对话框

class CLightmapEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightmapEditDlg)
public:
	static CLightmapEditDlg s_Inst;
	bool m_bRenderShadow;

public:
	void OnMouseMove(float x, float y, float width, float height);

	void OnLButtonDown(float x, float y);
	
public:
	CLightmapEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightmapEditDlg();

// 对话框数据
	enum { IDD = IDD_LIGHTMAP_EDIT };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	Ogre::String GetTemplateLightmap();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

public:
	CLightmapBrush	m_LightmapBrush;

	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedCheck1();
	CButton m_CheckShadow;
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_StrengthSlider;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	CComboBox m_BrushTypeCombo;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedRadio4();

	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
};
