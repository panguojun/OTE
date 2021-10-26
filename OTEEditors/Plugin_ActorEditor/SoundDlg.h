#pragma once

#include "Resource.h"
#include <list>
#include <string>
#include "afxwin.h"

// CSoundDlg �Ի���

class CSoundDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoundDlg)

public:
	static CSoundDlg s_Inst;

	CSoundDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSoundDlg();

// �Ի�������
	enum { IDD = IDD_SOUND_DLG };

	void InitSoundList();

	void UpdateSoundList();
	
	virtual BOOL OnInitDialog();

	std::list<std::string>   m_SoundList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	CEdit m_PathEdit;

	afx_msg void OnLbnSelchangeList1();
	CListBox m_SoundListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheck1();
	CButton m_CheckIsLoop;
};
