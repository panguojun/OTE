#pragma once

#include "TerrainMatBrush.h"
#include "afxwin.h"

// CBarrancaEdit �Ի���

class CBarrancaEdit : public CDialog
{
	DECLARE_DYNAMIC(CBarrancaEdit)

public:
	CBarrancaEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBarrancaEdit();

// �Ի�������
	enum { IDD = IDD_BarrancaEdit };

	static CBarrancaEdit s_Inst;
	CTerrainMatBrush*	m_pBrushEntity;

	void OnMouseMove(int x, int z, int width, int height);
	void OnLButtonDown(int x, int z, int width, int height);
	void OnLButtonUp(int x, int z, int width, int height);
	void OnKeyDown(unsigned int keyCode, bool isShifDwon);



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CEdit txtHeightValue;

	virtual BOOL OnInitDialog();

protected:
	void updateShowText(Ogre::Vector3 beginPos, int width, int height);

	enum{ter=1, hatch = 2};
public:
	CButton m_radioTerrain;
	int opType;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
