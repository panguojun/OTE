#pragma once
#include "afxcmn.h"
#include "EditListCtrl.h"
#include "afxwin.h"
#include "ColorSpectrum.h"

// CLightEditDlg 对话框
class ColorEdit: public CEdit
{
public:
	ColorEdit() {}
	virtual ~ColorEdit() {}

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
};

class CLightEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightEditDlg)

public:
	static CLightEditDlg s_Inst;
	CLightEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightEditDlg();

	virtual BOOL OnInitDialog();
	void UpdateLightColorList();

// 对话框数据
	enum { IDD = IDD_LIGHTEDIT };

	float m_fTimeValue;//记录时间

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	CEditListCtrl m_LightProp;
	afx_msg void OnHdnItemdblclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_LightSlider;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_SliderCtrl;
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	
	int m_iRedText;
	int m_iGreenText;
	int m_iBlueText;
	ColorEdit m_RedText;
	ColorEdit m_GreenText;
	ColorEdit m_BlueText;

	COLORREF m_crCurrent;
	ColorSpectrum m_ColorSpectrum;
	void SetValues();
	void SetSceneLight();

	afx_msg LRESULT OnSelChange( WPARAM, LPARAM );
	afx_msg void OnEnChangeRedEdit();
	afx_msg void OnEnChangeGreenEdit();
	afx_msg void OnEnChangeBlueEdit();
	afx_msg void OnStnClickedColorStatic();
};