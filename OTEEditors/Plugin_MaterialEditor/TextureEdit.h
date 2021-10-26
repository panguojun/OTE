#pragma once
#include "afxwin.h"
#include "Ogre.h"
#include "EditListCtrl.h"

// CTextureEdit 对话框

class CTextureEdit : public CDialog
{
	DECLARE_DYNAMIC(CTextureEdit)

public:
	CTextureEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTextureEdit();

	static CTextureEdit s_Inst;
// 对话框数据
	enum { IDD = IDD_Texture };

	Ogre::Material *m_pMat;
	Ogre::TextureUnitState *m_pTextureUnitState;

	void UpdataTexture(Ogre::Material *pMat);
	void UpdateCurTextUnit();
	bool isInit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//更新动画纹理状态
	void UpdataAniTextState(Ogre::TextureUnitState *pTextureUnitState);

	DECLARE_MESSAGE_MAP()
public:
	CButton m_SingleTexture;
	CButton m_MultiTexture;
	CEditListCtrl m_TextureList;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnKillfocusEdit2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnEditchangeCombo1();
	CComboBox m_TextAddrModeComb;
	afx_msg void OnBnClickedCheck1();
	CButton m_CullModeCheck;
	CListBox m_TexUnitList;
	afx_msg void OnLbnSelchangeTexunitlist();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton6();
};
