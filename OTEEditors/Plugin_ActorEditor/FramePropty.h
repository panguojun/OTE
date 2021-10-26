#pragma once
#include "afxcmn.h"
#include "EditListCtrl.h"
#include "OTEKeyFrame.h"
#include "SliderEditor.h"

// CFramePropty 对话框

class CFramePropty : public CDialog
{
	DECLARE_DYNAMIC(CFramePropty)

public:
	CFramePropty(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFramePropty();

// 对话框数据
	enum { IDD = IDD_FRM_PP };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()	
public:

	static CFramePropty	s_Inst;

	CEditListCtrl m_PorpList;

	void UpdateFramePropertyList(OTE::COTEKeyFrame* kf);

	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult);
};
