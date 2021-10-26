#pragma once
#include "afxcmn.h"

class CEditListCtrl : public CListCtrl
{
public:
	CEditListCtrl(void);
	virtual ~CEditListCtrl(void);

public:
	bool GetDClickedItem(int &rItem, int &rSubItem, CRect& rSubItemRect);

	//取最后一次选取的行
	int GetLastSelect()
	{
		return m_nLastSelect;
	}

private:
	//保存最后一次点选的行数
	int m_nLastSelect;
};
