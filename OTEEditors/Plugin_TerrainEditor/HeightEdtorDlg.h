#pragma once

#include "resource.h"
#include "afxwin.h"
#include "TerrainBrush.h"
#include "afxcmn.h"

// CHeightEditorDlg 对话框

class CHeightEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CHeightEditorDlg)

public:
	static CHeightEditorDlg s_Inst;
	CHeightEditorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHeightEditorDlg();

// 对话框数据
	enum { IDD = IDD_HEIGHT_EDITOR };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void DragHeight();	

	virtual BOOL OnInitDialog();

	void ResetBrushEntity();
	
	void DrawCliff(float x, float y);

	void AllDirScanfCliffFiles(std::string strPathName);

public:

	CTerrainBrush	m_BrushEntity;

	CComboBox	m_EditorTypeCombo;

public:
	
	void OnMouseMove(float x, float y);
	void OnLButtonDown(float x, float y);
	void OnLButtonUp(float x, float y);
	void OnKeyDown(unsigned int keyCode);	

	afx_msg void OnCbnSelchangeList1();
	CSliderCtrl m_InnerSizeSlider;
	CSliderCtrl m_OutSizeSlider;
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_BrushStrengthSlider;
	afx_msg void OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnupdatenorm();
	afx_msg void OnBnClickedBtnflatness();
	CEdit m_txtSensitivity;
	CListBox m_EditorTypeCombo2;
	// 复制高度时显示的值
	CEdit m_txtHeight;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnEnChangeHeighttext();
	afx_msg void OnBnClickedButton1();
	CEdit m_BottomHgtEdit;
	afx_msg void OnEnChangeEdit1();
	CComboBox m_CliffCombo;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedButton2();
};
