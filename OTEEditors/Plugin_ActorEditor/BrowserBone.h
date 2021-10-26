#pragma once

#include "Ogre.h"

// CBrowserBone �Ի���
class CBrowserBone : public CDialog
{
	DECLARE_DYNAMIC(CBrowserBone)

public:
	CBrowserBone(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBrowserBone();

	CListBox BoneList;
	static CBrowserBone s_Inst;

	bool isAddHook;
	bool isAddTrack;

	//���¹����Ի���
	void UpdataBoneList();

	//��ȡ������
	void GetBoneTree(Ogre::Node::ChildNodeIterator& it);

// �Ի�������
	enum { IDD = IDD_Bone };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeBonelist();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButton1();
};
