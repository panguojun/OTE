#include "StdAfx.h"
#include "editlistctrl.h"

CEditListCtrl::CEditListCtrl(void)
{
	m_nLastSelect = -1;
}

CEditListCtrl::~CEditListCtrl(void)
{
}



bool CEditListCtrl::GetDClickedItem(int &rItem, int &rSubItem, CRect& rSubItemRect)
{
	CPoint tP;		GetCursorPos(&tP);	
	CRect tWinRect;	GetWindowRect(tWinRect);	
	int item = GetSelectionMark();

	CRect SubItemRect;			
	int subItem = -1;
	if(item > -1)
	{
		for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); i ++)
		{			
			GetSubItemRect(item, i, LVIR_ICON, SubItemRect);	
			SubItemRect = CRect(CPoint(SubItemRect.left, SubItemRect.top), CSize(GetColumnWidth(i), SubItemRect.Height()));
			if(SubItemRect.PtInRect(CPoint(tP.x - tWinRect.left, tP.y - tWinRect.top)))
			{
				subItem = i;
				break;
			}
		}
	}

	//保存最后一次选取的行
	m_nLastSelect = item;

	if(item > -1 && subItem > -1)
	{
		rItem = item;
		rSubItem = subItem;
		rSubItemRect = CRect(CPoint(SubItemRect.left + tWinRect.left + 2, SubItemRect.top + tWinRect.top), CSize(SubItemRect.Width(), SubItemRect.Height() + 1));
		return true;
	}

	return false;
}