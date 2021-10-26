#pragma once
#include "afxcmn.h"

class CEditListCtrl : public CListCtrl
{
public:
	CEditListCtrl(void);
	virtual ~CEditListCtrl(void);

public:
	bool GetDClickedItem(int &rItem, int &rSubItem, CRect& rSubItemRect);
};
