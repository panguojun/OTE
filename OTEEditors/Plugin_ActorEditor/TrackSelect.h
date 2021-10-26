#pragma once


// CTrackSelect 对话框

class CTrackSelect : public CDialog
{
	DECLARE_DYNAMIC(CTrackSelect)

public:
	CTrackSelect(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTrackSelect();

// 对话框数据
	enum { IDD = IDD_TrackDlg };

	CListBox MTrackList;
	static CTrackSelect s_Inst;

	//更新可用轨道列表
	void UpdtatTrackList(bool hasSkeleton);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkTracklist();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeTracklist();
	afx_msg void OnBnClickedButton2();
};
