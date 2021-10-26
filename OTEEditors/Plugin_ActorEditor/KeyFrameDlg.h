#pragma once
#include "afxwin.h"
#include "resource.h"

#include "OTETrackManager.h"
#include "OTEMagicManager.h"
#include "OTELineObj.h"
#include "3DPathView.h"

// ----------------------------------------------
// ���״̬

#define		MOUSESTATE_FLAG_KEYITLOCKED			0xF0000000
#define		MOUSESTATE_FLAG_MOUSELOCKED			0x0F000000
#define		MOUSESTATE_FLAG_LBSLIDERDOWN		0x00F00000
#define		MOUSESTATE_FLAG_EDITPREPARED		0x000F0000

// **********************************************
// CKeyFrameDlg
// **********************************************
class CKeyFrameDlg : public CDialog
{
// ����
public:
	CKeyFrameDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_KEYFRAMEDLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:

	// ���ɵ���Ϣӳ�亯��
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

	// �ؼ���

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

	// ���õ�ǰʱ��

	void SetCurTimePos(float time);

	void SetKeyBtnsPos(float time);	

	// �õ���ǰʱ��

	float GetCurTimePos()
	{
		return m_CurTimePos;
	}

	// �õ�����

	void SetTimeRange(float minTime, float maxTime)
	{
		m_MinTimePos = minTime;

		m_MaxTimePos = maxTime;
	}

	// ����

	void DrawKeyMark(CDC* hdc, float time, bool showText = false);

	void DrawRect(CDC* hdc, const CRect& rect, COLORREF cor, bool isFill = true);

	void DrawKeyIterator(CDC* hdc);

	void DrawKeyMarkList(CDC* hdc);

	void DrawKey(CDC* hdc, float time, COLORREF cor);	

	void DrawKeyList(CDC* hdc);

	void DrawInureTimeMark(CDC* hdc, float time, COLORREF cor);	

	void DrawEndMark(CDC* hdc, float time, COLORREF cor);

	// ����任

	CPoint TimePosToWindowPos(float time);

	float WindowPosToTimePos(int winPos);

	// ���

	void AddToKeyList(float time, COLORREF cor, void* pUserData = NULL)
	{
		m_KeyList.push_back( Key_t(time, cor, pUserData) );
	}

	// ɾ��
	
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

	// ��������
	
	void ClearKeyList()
	{
		ClearSelection();

		m_KeyList.clear();		
		
		if(m_pTrack)
			m_pTrack->Clear();
		UpdataSlider();

	}

	// ѡ��

	Key_t* SelectKeyFromList(float time1, float time2)
	{		
		Key_t* ret = NULL;
		m_SelectedTimePos = -1;
		m_CurKey = NULL;

		std::vector<Key_t>::iterator it = m_KeyList.begin();
		while( it != m_KeyList.end() )
		{
			//(*it).state = 0; // ��ԭ״̬

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

	std::vector<Key_t> m_KeyList;		// ���б�

	Key_t*	m_CurKey;					// ��ǰ��
	
	CRect	m_TimeLinePaneRect;			// ����ʱ���ߵ���ʾ����

	CRect	m_OperationPaneRect;		// �ٿؾ���

protected:

	CPoint m_MouseButtonDownPos;		// ��갴��λ��

	float m_MarkSpace;					// ��Ǽ��				

	float m_MarkTextSpace;				// ���ּ��

	std::list<Key_t*>	m_SelKeys;		// ��ѡ�����б�

	float m_MinTimePos;

	float m_MaxTimePos;

	float m_SelectedTimePos;	

	CRect	m_KeyIteratorRect;			// ������ʾ����

	CRect	m_InureTimeRect;			// ��Чʱ�����

	unsigned int m_MouseStateFlag;		// ���״̬

	bool	mouseLocked;

	bool	m_MouseLButtonSliderDown;		// ����Ƿ��ڻ����ڲ�����

	CDC		m_MemDC;

	CBitmap m_MemBitmap;

	bool	m_InureTimeLocked;

public:

	//�༭�Ĺ��
	OTE::COTETrack *m_pTrack;

	//��������
	Ogre::MovableObject *m_pMover;

	//�༭�Ĺ��ID
	int		m_TrackID;

	//���·����ʾ
	C3DPathView* m_pPathView;

	//�༭�����Ƿ���
	bool	m_bPush;

	//�Ƿ����޸�
	bool	m_IsEdited;

public:

	//��������
	void UpdateData(const int TrackId);
	//����Slider
	void UpdataSlider();
	//���·��
	void UpdateTrackPath();

	//��������ƶ�
	void OnSceneMouseMove(int x, int y, bool isShift);

	//�����Լ�
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

	// ����ѡ��
	
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
