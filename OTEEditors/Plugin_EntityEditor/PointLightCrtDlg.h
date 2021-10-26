#pragma once
#include "resource.h"
#include "OTEStaticInclude.h"
#include "OTEActorEntity.h"
#include "afxwin.h"

// CPointLightCrtDlg 对话框

class CPointLightCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CPointLightCrtDlg)

public:
	CPointLightCrtDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPointLightCrtDlg();

// 对话框数据
	enum { IDD = IDD_CREATELIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	BOOL OnInitDialog();

public:

	static CPointLightCrtDlg	s_Inst;			// 静态对象

	std::string					m_LightName;	// 点光源帮助实体名

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButton3();

	void UpdateChange();

	void UpdateUI();

	void UpdateSlider(int id);

	void UpdateEditBox(int id);

	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_PtLgtList;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton14();
};