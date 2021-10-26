#pragma once

#include "Ogre.h"
#include "EditListCtrl.h"

// CMaterialEdit 对话框

class CMaterialEdit : public CDialog
{
	DECLARE_DYNAMIC(CMaterialEdit)

public:
	CMaterialEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMaterialEdit();

	static CMaterialEdit	s_Inst;
	CEditListCtrl mMatList;
	CEditListCtrl mColorList;

// 对话框数据
	enum { IDD = IDD_Material };

	Ogre::Material *m_pMat;	//材质

	//更新模型材质清单
	void UpdataMaterialList();

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

	bool isInited;//是否已经初始化

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickMateriallist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkColorlist(NMHDR *pNMHDR, LRESULT *pResult);
};
