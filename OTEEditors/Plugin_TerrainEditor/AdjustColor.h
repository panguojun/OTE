#pragma once
#include "afxcmn.h"


// CAdjustColor 对话框

class CAdjustColor : public CDialog
{
	DECLARE_DYNAMIC(CAdjustColor)

public:
	static CAdjustColor m_Inst;
	CAdjustColor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdjustColor();

// 对话框数据
	enum { IDD = IDD_AdjustColor };
	enum enumColor{red=1, green=2, blue=3, alpha=4};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()

	

public :
	void silderBindColor();

	void updateLightColorList(enumColor c, float value);

	enum operateObject { Ambient=0, MainLight=1, complementarityLight=2};
	float m_aryColor[4];
	operateObject m_operateObject;


public :
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClickedCancel();
	CSliderCtrl m_silderRed;
	CSliderCtrl m_silderGreen;
	CSliderCtrl m_silderBlue;
	CSliderCtrl m_silderAlpha;
	afx_msg void OnNMReleasedcapturesilderred(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcapturesildergreen(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcapturesilderblue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcapturesilderalpha(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedsilderred(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMOutofmemorysilderred(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawsilderred(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawsildergreen(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawsilderblue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawsilderalpha(NMHDR *pNMHDR, LRESULT *pResult);
};
