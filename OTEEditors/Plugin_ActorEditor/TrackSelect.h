#pragma once


// CTrackSelect �Ի���

class CTrackSelect : public CDialog
{
	DECLARE_DYNAMIC(CTrackSelect)

public:
	CTrackSelect(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTrackSelect();

// �Ի�������
	enum { IDD = IDD_TrackDlg };

	CListBox MTrackList;
	static CTrackSelect s_Inst;

	//���¿��ù���б�
	void UpdtatTrackList(bool hasSkeleton);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkTracklist();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeTracklist();
	afx_msg void OnBnClickedButton2();
};
