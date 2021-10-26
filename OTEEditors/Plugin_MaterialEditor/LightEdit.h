#pragma once

#include "EditListCtrl.h"

// CLightEdit 对话框

class CLightEdit : public CDialog
{
	DECLARE_DYNAMIC(CLightEdit)

public:
	CLightEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightEdit();

	static CLightEdit s_Inst;
	// 亮度调节控件
	CSliderCtrl m_SilderBrightness;
	CEditListCtrl mColorList;

	int m_Y, m_cB, m_cR;

// 对话框数据
	enum { IDD = IDD_Light };

	//清除窗体显示
	void ClearAll();

	//更新窗体数据
	void UpdataAll();

	//各种颜色类型
	enum _ColorType{
		NONE,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};
	//显示对应的颜色
	void Display(Ogre::ColourValue &cv, _ColorType ct = NONE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//RGB转YCBCR
	void RGBtoYCBCR(int r, int g, int b);
	//YCBCR转RGB
	void YCBCRtoRGB(int &r, int &g, int &b);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkColorlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickColorlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
