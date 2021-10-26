#pragma once

#include "AnimSliderCtrl.h"
#include "TrackManager.h"
#include "MagicManager.h"
#include "OTELineObj.h"
#include "3DPathView.h"
#include "KeyFrameDlg.h"

// CTrackEdit �Ի���

class CTrackEdit : public CDialog
{
	DECLARE_DYNAMIC(CTrackEdit)

public:
	CTrackEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTrackEdit();

	CAnimSliderCtrl m_Slider;
	CButton			m_BtnEdit1;
	CButton			m_BtnEdit2;
	//CKeyFrameDlg	m_KeyFrameDlg;

	OTE::CTrackManager *m_pTManager;
	OTE::CMagicManager *m_pMManager;

	//�༭�Ĺ��
	OTE::CActTrack *m_pTrack;
	//��Ӧ������
	Ogre::MovableObject *m_pMover;
	//�༭�Ĺ��ID
	int	m_TrackID;

	//���·����ʾ
	C3DPathView* m_pPathView;

	//�༭�����Ƿ���
	bool m_bPush;

	//��������
	void UpdateData(const int TrackId);
	//����Slider
	void UpdataSlider();
	//���·��
	void UpdateTrackPath();

	static CTrackEdit s_Inst;

	//��������ƶ�
	void OnMouseMove(int x, int y, bool isShift);

	//�����Լ�
	void DestoryMe();

// �Ի�������
	enum { IDD = IDD_TrackEdit };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	//�Զ���ӹؼ�֡
	void AutoAddKey();
	
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedEditKey();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedErase();
	afx_msg void OnBnClickedEditkey2();
	afx_msg void OnNMCustomdrawTimeslider(NMHDR *pNMHDR, LRESULT *pResult);
};
