#pragma once
#include "afxcmn.h"
#include "resource.h"

// CVecEditor �Ի���

class CVecEditor : public CDialog
{
	DECLARE_DYNAMIC(CVecEditor)

public:
	typedef void (*pCBFun_t) (float x, float y, float z);

	CVecEditor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVecEditor();

// �Ի�������
	enum { IDD = IDD_VEC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();

	virtual BOOL OnInitDialog();

public:
	pCBFun_t	 m_pCBFun;
	CString		 m_UserString;

	void Show(pCBFun_t pCBFun, float x, float y, float z, float min, float max, 
		CString caption = "�����༭��", CString xLabel = "X:", CString yLabel = "Y:", CString zLabel = "Z:");
	void Update(float x, float y, float z);

	void Hide();

	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_SliderCtrl1;
	CSliderCtrl m_SliderCtrl2;
	CSliderCtrl m_SliderCtrl3;
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
};
