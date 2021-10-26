#pragma once
#include "TerrainBrush.h"
#include "afxcmn.h"

// CBrushDlg �Ի���

class CBrushDlg : public CDialog
{
	DECLARE_DYNAMIC(CBrushDlg)

public:
	static CBrushDlg	s_Inst;
	CBrushDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBrushDlg();

// �Ի�������
	enum { IDD = IDD_BRUSH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();

public:
	CTerrainBrush*	m_pBrushEntity;

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_OutterSlider;
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_InnerSlider;
	afx_msg void OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_Strength;
};
