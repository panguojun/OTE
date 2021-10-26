#pragma once
#include "resource.h"
#include "OTEStaticInclude.h"
#include "OTEActorEntity.h"
#include "afxwin.h"

// CPointLightCrtDlg �Ի���

class CPointLightCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CPointLightCrtDlg)

public:
	CPointLightCrtDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPointLightCrtDlg();

// �Ի�������
	enum { IDD = IDD_CREATELIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	BOOL OnInitDialog();

public:

	static CPointLightCrtDlg	s_Inst;			// ��̬����

	std::string					m_LightName;	// ���Դ����ʵ����

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