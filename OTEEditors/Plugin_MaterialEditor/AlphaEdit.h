#pragma once
#include "afxwin.h"
#include "Ogre.h"

// CAlphaEdit 对话框

class CAlphaEdit : public CDialog
{
	DECLARE_DYNAMIC(CAlphaEdit)

public:
	CAlphaEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAlphaEdit();

	static CAlphaEdit s_Inst;

// 对话框数据
	enum { IDD = IDD_Alpha };

	//更新
	void UpdataAlpha(Ogre::Material *pMat);

	Ogre::Material *m_pMat;
	Ogre::Pass *m_pPass;

	bool isInit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	//设置AlphaTestFunc
	void SetAlphaTestFunc();

public:
	afx_msg void OnBnClickedCheck1();
	CButton m_isAlphaTest;
	CButton m_isDepTest;
	CComboBox m_AlphaTestFormula;
	CComboBox m_AlphaBlendFormula;
	CComboBox m_DepTestFormula;
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnCbnSelchangeCombo4();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_HightLight;
};
