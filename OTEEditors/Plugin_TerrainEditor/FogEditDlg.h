#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CFogEditDlg 对话框

class CFogEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CFogEditDlg)

public:
	static CFogEditDlg	s_Inst;

	CFogEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFogEditDlg();

// 对话框数据
	enum { IDD = IDD_FOGEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_FogSliderNear;
	CSliderCtrl m_SliderFogFar;

	void ShowFogColor();

	void UpdateFogColor(unsigned int id);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheck1();
	CButton m_CheckBox;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnEnChangeEdit9();
};
