#ifndef _PROPERTY_LIST_H
#define _PROPERTY_LIST_H

#include <afxtempl.h>
#include <list>

#define BEGIN_ITEM( property_list, nLevel, folder, expand, status, left_name, right_type, iNumChar, bshow, values )\
	{\
		PropertyItem *pItem = new PropertyItem( 0, folder, expand, status, left_name, right_type, iNumChar, bshow );\
		pItem->m_nLevel = nLevel;\
		if( pItem->m_nControlType == CONTROLTYPE_LISTBOX )\
			property_list.ParseStringList( values, *pItem );\
		else pItem->m_text = values;\
		property_list.m_List.Add( pItem );\
	}

enum ITEMTYPE
{
	ITEMTYPE_ITEM,
	ITEMTYPE_FOLDER,
	ITEMTYPE_OPENFOLDER,
	ITEMTYPE_CLOSEFOLDER,
	//ITEMTYPE_UNKNOWN
};

enum ITEMSTATUS
{
	ITEMSTATUS_DEFAULT,
	ITEMSTATUS_DISABLE,
	ITEMSTATUS_DISABLE_ALL
};

enum CONTROLTYPE
{
	CONTROLTYPE_NONE,
	CONTROLTYPE_LISTBOX,
	CONTROLTYPE_EDIT,
	CONTROLTYPE_BUTTON,
	CONTROLTYPE_CUSTOM
};

struct PropertyItem: public CObject
{
	int m_nItemIndex;
	ITEMTYPE m_nType;
	ITEMSTATUS m_nStatus;
	TCHAR m_szName[ 64 ];
	//int m_nLeft;
	//int m_nRight;
	CONTROLTYPE m_nControlType;
	bool m_bShow;

	CStringArray m_strings;
	CString m_text;
	int m_nLevel;
	CRect m_icon;

	int m_iExpand;
	int m_iNumChar;

	PropertyItem();
	PropertyItem( int m_nItemIndex, ITEMTYPE m_nType, int iExpand, ITEMSTATUS m_nStatus, TCHAR* m_szName, CONTROLTYPE m_nControlType, int iNumChar, bool b );
	CString GetValue();
};

typedef  CTypedPtrArray< CObArray, PropertyItem* >PropertyItemList;
const int MARGIN_WIDTH = 16;

class CPropertyList: public CWnd
{
	DECLARE_DYNAMIC( CPropertyList )
public:
	CPropertyList();
	virtual ~CPropertyList();
	void Destroy();

	void SetListValue( int item, float fValue );
	void GetListValue( int item, float &fValue );
	void SetListBoxSelect( int index );
	void SetColorData( DWORD color );
	void GetColorData( DWORD &color );
	void GetListBoxSelect( int &iSelect );
	PropertyItemList m_List;
	void DoDraw( CDC*pDC );
	void ParseStringList( CString lszList, PropertyItem &_item );

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnSelChange();
	afx_msg void OnVisSelChange();	
	afx_msg void OnCbnKillfocusCombo();
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnButtonCliked();

	bool m_bAngleChange;
	bool m_bScaleChange;
	bool m_bAlphaChange;
	bool m_bAlphaRange;
	bool m_bColorChange;
	bool m_bVisChange;
	bool m_bEnterKey;

	DWORD m_Color;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void Draw( CDC *pDC, const CRect &rect );
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	int GetShowItem();
	void FormatList();
	void DrawPlus( bool bPlus, CDC *pDC,const CRect& );
	void CloseNode( int nIndex );
	void OpenNode( int nIndex );
	void ClickValueArea( int nIndex, const CRect& );
	virtual void PreSubclassWindow();

private:
	void InitPropertyList();
	void CalcuRgn();

	//int m_nCurItem;
	CRgn m_rgn;

	int m_nItemHeight;
	int m_nLeft;
	CBitmap m_bmpSub;
	CBitmap m_bmpPlus;
	int m_nItemIndex;

	CComboBox m_ListBox;
	CEdit m_Edit;
	CButton m_Button;

	std::list< bool > m_bFolderList;
};

#endif