#pragma once

#include "AnimSliderCtrl.h"
#include "TrackManager.h"
#include "MagicManager.h"
#include "OTELineObj.h"
#include "3DPathView.h"
#include "KeyFrameDlg.h"

// CTrackEdit 对话框

class CTrackEdit : public CDialog
{
	DECLARE_DYNAMIC(CTrackEdit)

public:
	CTrackEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTrackEdit();

	CAnimSliderCtrl m_Slider;
	CButton			m_BtnEdit1;
	CButton			m_BtnEdit2;
	//CKeyFrameDlg	m_KeyFrameDlg;

	OTE::CTrackManager *m_pTManager;
	OTE::CMagicManager *m_pMManager;

	//编辑的轨道
	OTE::CActTrack *m_pTrack;
	//对应的物体
	Ogre::MovableObject *m_pMover;
	//编辑的轨道ID
	int	m_TrackID;

	//轨道路径显示
	C3DPathView* m_pPathView;

	//编辑按键是否按下
	bool m_bPush;

	//更新数据
	void UpdateData(const int TrackId);
	//更新Slider
	void UpdataSlider();
	//轨道路径
	void UpdateTrackPath();

	static CTrackEdit s_Inst;

	//处理鼠标移动
	void OnMouseMove(int x, int y, bool isShift);

	//销毁自己
	void DestoryMe();

// 对话框数据
	enum { IDD = IDD_TrackEdit };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	//自动添加关键帧
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
