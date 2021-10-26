#pragma once
#include "afxwin.h"

#include "Ogre.h"
// COtherEdit 对话框

class COtherEdit : public CDialog
{
	DECLARE_DYNAMIC(COtherEdit)

public:
	COtherEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COtherEdit();

    static COtherEdit s_Inst;

// 对话框数据
	enum { IDD = IDD_Other };

	//更新
	void UpdataOther(Ogre::Material *pMat);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	Ogre::Material *m_pMat;
	Ogre::Pass *m_pPass;

	DECLARE_MESSAGE_MAP()
public:
	CButton m_isUseCull;
	CButton m_isClockWise;
	CButton m_isAntiClockWise;
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedButton1();
};
