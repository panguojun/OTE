#pragma once
#include "afxwin.h"
#include "resource.h"

#include "OTETrackManager.h"
#include "OTEMagicManager.h"
#include "OTELineObj.h"
#include "3DPathView.h"

// ----------------------------------------------
// 鼠标状态

#define		MOUSESTATE_FLAG_KEYITLOCKED			0xF0000000
#define		MOUSESTATE_FLAG_MOUSELOCKED			0x0F000000
#define		MOUSESTATE_FLAG_LBSLIDERDOWN		0x00F00000
#define		MOUSESTATE_FLAG_EDITPREPARED		0x000F0000

// **********************************************
// CKeyFrameDlg
// **********************************************
class CKeyFrameDlg : public CDialog
{
// 构造
public:
	CKeyFrameDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_KEYFRAMEDLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void Reset();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton3();

	CButton m_BtnEdit1;
	CButton m_BtnEdit2;

	CButton m_KeyPreBtn;
	CButton m_KeyNextBtn;

	afx_msg void OnBnClickedButton2();

	/// conversation

	static float ToEffTime(float keyTime)
	{
		return keyTime * 10.0f;
	}

	static float ToKeyTime(float effTime)
	{
		return effTime / 10.0f;
	}

public:

	// 关键桢

	struct Key_t
	{
		float Time;	
		unsigned char state;	
		void* pUserData;
		COLORREF Cor;

		Key_t(float time, COLORREF cor, void* UserData)
		{
			pUserData = UserData;
			Time = time;
			state = 0;
			Cor = cor;
		}

		void operator = (const Key_t& key)
		{
			Time = key.Time;
			pUserData = key.pUserData;
			state = key.state;
			Time = key.Time;
		}
	};

public:

	// 设置当前时间

	void SetCurTimePos(float time);

	void SetKeyBtnsPos(float time);	

	// 得到当前时间

	float GetCurTimePos()
	{
		return m_CurTimePos;
	}

	// 得到桢跨度

	void SetTimeRange(float minTime, float maxTime)
	{
		m_MinTimePos = minTime;

		m_MaxTimePos = maxTime;
	}

	// 绘制

	void DrawKeyMark(CDC* hdc, float time, bool showText = false);

	void DrawRect(CDC* hdc, const CRect& rect, COLORREF cor, bool isFill = true);

	void DrawKeyIterator(CDC* hdc);

	void DrawKeyMarkList(CDC* hdc);

	void DrawKey(CDC* hdc, float time, COLORREF cor);	

	void DrawKeyList(CDC* hdc);

	void DrawInureTimeMark(CDC* hdc, float time, COLORREF cor);	

	void DrawEndMark(CDC* hdc, float time, COLORREF cor);

	// 坐标变换

	CPoint TimePosToWindowPos(float time);

	float WindowPosToTimePos(int winPos);

	// 添加

	void AddToKeyList(float time, COLORREF cor, void* pUserData = NULL)
	{
		m_KeyList.push_back( Key_t(time, cor, pUserData) );
	}

	// 删除
	
	void RemoveKeyFromList(float time)
	{
		ClearSelection();

		std::vector<Key_t>::iterator it = m_KeyList.begin();
		while( it != m_KeyList.end() )
		{
			if( (*it).Time == time )
			{
				if(m_CurKey == &(*it))
					m_CurKey = NULL;

				m_KeyList.erase(it);
				break;
			}
			++ it;
		}		
	}

	// 清理所有
	
	void ClearKeyList()
	{
		ClearSelection();

		m_KeyList.clear();		
		
		if(m_pTrack)
			m_pTrack->Clear();
		UpdataSlider();

	}

	// 选择

	Key_t* SelectKeyFromList(float time1, float time2)
	{		
		Key_t* ret = NULL;
		m_SelectedTimePos = -1;
		m_CurKey = NULL;

		std::vector<Key_t>::iterator it = m_KeyList.begin();
		while( it != m_KeyList.end() )
		{
			//(*it).state = 0; // 还原状态

			if( (*it).Time >= time1 && (*it).Time <= time2 )
			{
				(*it).state = 1;
				m_SelectedTimePos = (*it).Time;
				m_CurKey = &(*it);
				m_SelKeys.push_back(m_CurKey);

				ret =  &(*it);				
			}
			++ it;
		}	

		return ret;
	}

public:

	static CKeyFrameDlg s_Inst;

	float m_CurTimePos;

	std::vector<Key_t> m_KeyList;		// 桢列表

	Key_t*	m_CurKey;					// 当前桢
	
	CRect	m_TimeLinePaneRect;			// 整个时间线的显示矩形

	CRect	m_OperationPaneRect;		// 操控矩形

protected:

	CPoint m_MouseButtonDownPos;		// 鼠标按下位置

	float m_MarkSpace;					// 标记间隔				

	float m_MarkTextSpace;				// 文字间隔

	std::list<Key_t*>	m_SelKeys;		// 被选中桢列表

	float m_MinTimePos;

	float m_MaxTimePos;

	float m_SelectedTimePos;	

	CRect	m_KeyIteratorRect;			// 滑条显示矩形

	CRect	m_InureTimeRect;			// 生效时间矩形

	unsigned int m_MouseStateFlag;		// 鼠标状态

	bool	mouseLocked;

	bool	m_MouseLButtonSliderDown;		// 鼠标是否在滑条内部点下

	CDC		m_MemDC;

	CBitmap m_MemBitmap;

	bool	m_InureTimeLocked;

public:

	//编辑的轨道
	OTE::COTETrack *m_pTrack;

	//帮助物体
	Ogre::MovableObject *m_pMover;

	//编辑的轨道ID
	int		m_TrackID;

	//轨道路径显示
	C3DPathView* m_pPathView;

	//编辑按键是否按下
	bool	m_bPush;

	//是否有修改
	bool	m_IsEdited;

public:

	//更新数据
	void UpdateData(const int TrackId);
	//更新Slider
	void UpdataSlider();
	//轨道路径
	void UpdateTrackPath();

	//处理鼠标移动
	void OnSceneMouseMove(int x, int y, bool isShift);

	//销毁自己
	void DestoryMe(bool sendCloseMsg = true);
	
	enum eTRS_TYPE{eAll, eTrans, eRot, eScl};
	
	void SetKey(float time, eTRS_TYPE tsrType = eAll,
		const Ogre::Vector3& dOffset = Ogre::Vector3::ZERO,
		const Ogre::Vector3& angles = Ogre::Vector3::ZERO, bool m_bAutoAdd = true);

	void SetKey(float stime, 
				const Ogre::Vector3& pos, const Ogre::Vector3& scl, const Ogre::Vector3& angles);
	
	void CloneKey(float stime);

	OTE::COTEKeyFrame* GetKey(float stime);

	OTE::COTEKeyFrame* GetCurrentKey();

	void UpdateMover(OTE::COTEKeyFrame *kf);

	void OnSceneLButtonUp(int x, int y);

	void OnSceneLButtonDown(int x, int y);

	void UpdateTimePosMove(float timePos);	

	void OnEndPlugin();

	float GetTimeRange()
	{
		return m_MaxTimePos - m_MinTimePos;
	}

	// 清理选择
	
	void ClearSelection()
	{
		if(m_SelKeys.size() > 0)
		{
			std::list<Key_t*>::iterator it = m_SelKeys.begin();
			while(it != m_SelKeys.end())
			{
				(*it)->state = 0;
				++ it;
			}
		}
		m_SelKeys.clear();
		m_CurKey = NULL;
	}
	
public:

	afx_msg void OnBnClickedButton1();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChangeEdit1();
	void UpdateKeyBtnPosTxt();

	afx_msg void OnBnClickedErase();
	afx_msg void OnBnBindBone();
	afx_msg void OnBnClickedSave();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CButton m_IsBezier;
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnChangeEdit2();
};
