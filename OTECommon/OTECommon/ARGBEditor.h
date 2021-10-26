#pragma once
#include "afxwin.h"
#include "ogrecolourvalue.h"

#include "OgreNoMemoryMacros.h"
#include "D3DX9.h"
#include "OgreMemoryMacros.h"
#include "resource.h"
// CARGBEditor �Ի���

class CARGBEditor : public CDialog
{
	DECLARE_DYNAMIC(CARGBEditor)

public:
	CARGBEditor(CWnd* pParent = NULL);
	virtual ~CARGBEditor();

// �Ի�������
	enum { IDD = IDD_ARGBEDITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	BOOL OnInitDialog();
	
public:
	static CRect					sColorSelRect;
	
	static  Ogre::ColourValue*		spColour;
	static	D3DXVECTOR4*			spColourD3DVec;
	
	void SetColourSliders();

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);	

	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
};
