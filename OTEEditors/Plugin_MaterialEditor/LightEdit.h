#pragma once

#include "EditListCtrl.h"

// CLightEdit �Ի���

class CLightEdit : public CDialog
{
	DECLARE_DYNAMIC(CLightEdit)

public:
	CLightEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLightEdit();

	static CLightEdit s_Inst;
	// ���ȵ��ڿؼ�
	CSliderCtrl m_SilderBrightness;
	CEditListCtrl mColorList;

	int m_Y, m_cB, m_cR;

// �Ի�������
	enum { IDD = IDD_Light };

	//���������ʾ
	void ClearAll();

	//���´�������
	void UpdataAll();

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

	//RGBתYCBCR
	void RGBtoYCBCR(int r, int g, int b);
	//YCBCRתRGB
	void YCBCRtoRGB(int &r, int &g, int &b);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkColorlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickColorlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
