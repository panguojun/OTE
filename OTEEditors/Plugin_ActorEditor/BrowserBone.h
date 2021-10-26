#pragma once

#include "Ogre.h"

// CBrowserBone 对话框
class CBrowserBone : public CDialog
{
	DECLARE_DYNAMIC(CBrowserBone)

public:
	CBrowserBone(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBrowserBone();

	CListBox BoneList;
	static CBrowserBone s_Inst;

	bool isAddHook;
	bool isAddTrack;

	//更新骨骼对话框
	void UpdataBoneList();

	//获取骨骼树
	void GetBoneTree(Ogre::Node::ChildNodeIterator& it);

// 对话框数据
	enum { IDD = IDD_Bone };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeBonelist();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButton1();
};
