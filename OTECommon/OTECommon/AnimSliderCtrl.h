#pragma once

// CustomDrawSliderCtrl.h : header file

#include <list>

// *************************************** //
// KeyFrame_t
// *************************************** //
struct KeyFrame_t
{
	bool	foucsed;
	float   time;

	KeyFrame_t()
	{		
		foucsed = false;
		time = 0;
	}
};


// CAnimSliderCtrl window
class CAnimSliderCtrl : public CSliderCtrl
{
public:

// Construction
public:
	CAnimSliderCtrl();				// default constructor
	CAnimSliderCtrl( COLORREF cr );	// constructs the control and sets its primary color


// Attributes
public:
	
	// ���б�

	std::list<KeyFrame_t>		m_KeyList;
	
	// ������С/��󳤶�
	float						m_MaxAnimTime;

	float						m_MinAnimTime;

	//������Чʱ��
	float						m_AttackTime;

// Operations
public:
	
	// ���

	void AddKeyFrame(KeyFrame_t kf)
	{
		m_KeyList.push_back(kf);
	}

	// ��չؼ���
	
	void ClearKeyFrames()
	{
		m_KeyList.clear();
	}

	//�õ���ѡ�ؼ�֡��ʱ��
	float getFoucsKeyFrameTime()
	{
		float s = - 1.0f;
		std::list<KeyFrame_t>::iterator it = m_KeyList.begin();
		while(it != m_KeyList.end())
		{
			if((*it).foucsed)
			{
				s = (*it).time;
				break;
			}

			++ it;
		}

		return s;
	}

	// ��ǰʱ��

	float GetCurSliderTime();

// Implementation
public:
	COLORREF GetShadowColor() const;
	COLORREF GetHiliteColor() const;
	COLORREF GetPrimaryColor() const;
	void SetPrimaryColor( COLORREF cr );
	virtual ~CAnimSliderCtrl();

	// Generated message map functions
protected:

	COLORREF m_crPrimary;
	COLORREF m_crShadow;
	COLORREF m_crHilite;
	COLORREF m_crMidShadow;
	COLORREF m_crDarkerShadow;

	CBrush m_normalBrush;
	CBrush m_focusBrush;

	//{{AFX_MSG(CAnimSliderCtrl)
	afx_msg void OnReflectedCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

};