#pragma once
#include "afxwin.h"
#include "Ogre.h"

// CAlphaEdit �Ի���

class CAlphaEdit : public CDialog
{
	DECLARE_DYNAMIC(CAlphaEdit)

public:
	CAlphaEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAlphaEdit();

	static CAlphaEdit s_Inst;

// �Ի�������
	enum { IDD = IDD_Alpha };

	//����
	void UpdataAlpha(Ogre::Material *pMat);

	Ogre::Material *m_pMat;
	Ogre::Pass *m_pPass;

	bool isInit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	//����AlphaTestFunc
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
