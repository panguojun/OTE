#pragma once

#include "Ogre.h"
#include "EditListCtrl.h"

// CMaterialEdit �Ի���

class CMaterialEdit : public CDialog
{
	DECLARE_DYNAMIC(CMaterialEdit)

public:
	CMaterialEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMaterialEdit();

	static CMaterialEdit	s_Inst;
	CEditListCtrl mMatList;
	CEditListCtrl mColorList;

// �Ի�������
	enum { IDD = IDD_Material };

	Ogre::Material *m_pMat;	//����

	//����ģ�Ͳ����嵥
	void UpdataMaterialList();

	//������ɫ����
	enum _ColorType{
		NONE,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};
	//��ʾ��Ӧ����ɫ
	void Display(Ogre::ColourValue &cv, _ColorType ct = NONE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	bool isInited;//�Ƿ��Ѿ���ʼ��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickMateriallist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkColorlist(NMHDR *pNMHDR, LRESULT *pResult);
};
