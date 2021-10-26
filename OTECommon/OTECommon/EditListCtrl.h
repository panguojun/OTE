#pragma once
#include "afxcmn.h"

class CEditListCtrl : public CListCtrl
{
public:
	CEditListCtrl(void);
	virtual ~CEditListCtrl(void);

public:
	bool GetDClickedItem(int &rItem, int &rSubItem, CRect& rSubItemRect);

	//ȡ���һ��ѡȡ����
	int GetLastSelect()
	{
		return m_nLastSelect;
	}

private:
	//�������һ�ε�ѡ������
	int m_nLastSelect;
};
