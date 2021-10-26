#pragma once
#include "FBGUIRender.h"
#include "Math.h"

namespace FBGUILogicLayer
{
// ************************************************************ //
// CGUISystem
//
// �ؼ�������������������пؼ���������Ϣ��
// ************************************************************ //

class CControlManager 
{
protected:
	int							mLastPage;		// �ϴ���ʾ����Ϸ����ҳ
	int							mCurrentPage;	// ��ǰ��ʾ����Ϸ����ҳ
	int							mNextPage;		// ������ʾ����Ϸ����ҳ
	std::vector<CControlPage*>	mPages;			// ��Ϸ��������
	CGUIViewport*				mpViewport;

public:
	CControlManager( CGUIViewport* pViewport, CGUIRender* pRender, CGUITextureFactory* pFactory );
public:
	virtual ~CControlManager( );

protected:
	void SetCurrentPage( int vCurrentPage );
	void UpdateCurrentPage( );
	void DrawCurrentPage( );

public:
	void AddPage( CControlPage* pControlPage )		{ mPages.push_back( pControlPage ); }
	void SwitchPage( int vNextPage )				{ mNextPage = vNextPage;			}
	CControlPage* GetCurrentPage( )	const			{ return ( mCurrentPage < 0 ) ? NULL : mPages[ mCurrentPage ]; }
	CControlPage* GetPage( int vPageIndex ) const;

	void Draw( );
	int InputProcess( unsigned int vMsg, unsigned int wParam, unsigned int lParam );
};

// ************************************************************ //
// CCommonHandler
// ************************************************************ //

class CCommonHandler
{
public:
	static void ClosePopup( CControlButton* pButton );
};

// ************************************************************ //
// CControl
//
// ���пؼ��Ļ��࣬ӵ�пؼ��Ļ������ԣ���������Ϣ��Ӧ��
// ************************************************************ //

class CControl
{
	friend class CControlManager;
	friend class CControlDialog;
	friend class CControlPage;
	friend class CControlRadioGroup;
	friend class CControlListBox;

	// �ϷŶ��������鴦��
	typedef void ( *OnDragFromHandler )( CControl*, CControl*, const CGUIPoint& );
	typedef void ( *OnDragToHandler )( CControl*, CControl*, const CGUIPoint& );

public:
	static CRenderDevice*		spRender;
	static CGUITextureFactory*	spTextureFactory;

protected:
	OnDragFromHandler			mpDragFromHandler;	// ����б����¼�������ָ��
	OnDragToHandler				mpDragToHandler;	// ����б����¼�������ָ��
	CControl*					mpParent;			// ���ؼ�
	std::string					mTitle;				// �ؼ������ַ���
	unsigned int				mID;				// �ؼ� ID
	unsigned int				mFlags;				// �ؼ�״̬
	unsigned int				mStyle;				// �ؼ����
	CGUIRect					mRect;
	CGUITexture*				mpTexture;
	
protected:
	CControl( unsigned int vStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControl( );

public:
	// ȡ�ÿؼ������Ϣ��
	CControlPage*	GetPage( ) const;
	CControl*		GetParent( ) const	{ return mpParent; }
	const CGUIRect&	GetRect( ) const	{ return mRect; }
	unsigned int	GetID( ) const		{ return mID; }
	unsigned int	GetFlags( ) const	{ return mFlags; }

	void SetRect( const CGUIRect& rRect )		{ mRect = rRect; }
	// ���ú�ȡ�ñ�־λ��������ʾһ���ؼ���״̬��
	void InitFlag( unsigned int vFlags )		{ mFlags = vFlags; }
	void SetFlag( unsigned int vFlag )			{ mFlags |= vFlag; }
	void UnsetFlag( unsigned int vFlag )		{ mFlags &= ~vFlag; }
	bool IsFlag( unsigned int vFlags ) const	{ return ( mFlags & vFlags ) != 0; }

	// ���ôӿؼ��Ϸ�ʱ���¼���������
	void SetDragFromHandler( OnDragFromHandler pDragFromHandler )
		{ mpDragFromHandler = pDragFromHandler; }

	// �����Ϸŵ��ؼ�ʱ���¼���������
	void SetDragToHandler( OnDragToHandler pDragToHandler )
		{ mpDragToHandler = pDragToHandler; }

	void SetParent( CControl* pParent )			{ mpParent = pParent; }

public:
	// �¼������麯�������Ա��������ء�
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )			{ }
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )			{ }
	virtual void OnMouseRButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )			{ }
	virtual void OnMouseRButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )			{ }
	virtual void OnMouseMButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )			{ }
	virtual void OnMouseMButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )			{ }
	virtual void OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )	{ }
	virtual void OnMouseRButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )	{ }
	virtual void OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags )				{ }
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )				{ }
	virtual void OnKeyUp( unsigned int vKeyCode, unsigned int vFlags )						{ }
	virtual void OnKeyDown( unsigned int vKeyCode, unsigned int vFlags )					{ }
	virtual void OnChar( unsigned int vCharCode, unsigned int vFlags )						{ }
	virtual void OnSysKeyDown( unsigned int vKeyCode, unsigned int vFlags )					{ }
	virtual void OnDrawToolTip( const CGUIPoint& rPoint )									{ }

	// �ؼ���û�ʧȥ����ʱ���¼�����
	virtual void OnFocus( const CGUIPoint& rPoint, bool vFocus );

	// �ؼ���û�ʧȥ��ͣʱ���¼�����
	virtual void OnHover( const CGUIPoint& rPoint, bool vHover )
		{ vHover ? SetFlag( GUI_CTL_HOVER ) : UnsetFlag( GUI_CTL_HOVER ); }

	// �ƶ��ؼ���
	virtual void Offset( const CGUIPoint& rPoint )							{ mRect += rPoint; }
	virtual void Offset( const CGUIPoint& rPoint, const CGUISize& rSize );

	// ���ÿؼ����⡣
	virtual void SetTitle( const char* tTitle )	{ mTitle = tTitle; }
	// ȡ�ÿؼ����⡣
	virtual const char* GetTitle( ) const		{ return mTitle.c_str( ); }
	// ��Ⱦ�ؼ�����ͬ�� UI ������Զ�����Ⱦ������
	virtual void Draw( )							{ }
	virtual void UpdateStatus( )					{ }
	virtual void InitControl( bool vIsInit )		{ }
};

// ************************************************************ //
// CControlDialog
//
// �ؼ��Ի��򣬿ؼ��������������Ͽ��Է��������κοؼ����ؼ��Ի���
// �õ���������Ϣ���ᷢ�͵���Ӧ�ӿؼ��У��Լ�����������
// ************************************************************ //

class CControlDialog : public CControl
{
	friend class CControlPage;

protected:
	std::vector<CControl*>		mChildrenList;		// �ӿؼ��б�
	CControlScrollBar*			mpVScrollBar;		// ���������
	CControlScrollBar*			mpHScrollBar;		// ���������
	CControlButton*				mpExit;
	CGUIPoint					mCamPoint;
	const int					mVScrollBarWidth;	// ������������
	const int					mHScrollBarHeight;	// ����������߶�
	const int					mCloseWidth;		// �رհ�ť���
	const int					mCloseHeight;		// �رհ�ť�߶�
	const int					mBorderLength;		// �߳���
	const int					mCaptionHeight;		// �������߶�

public:
	CControlDialog( unsigned int vDialogStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlDialog( );

public:
	virtual void UpdateStatus( );
	virtual void Offset( const CGUIPoint& rPoint );
	virtual void Offset( const CGUIPoint& rPoint, const CGUISize& rSize );
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );

public:
	bool RefreshDialog( int vLogicWidth, int vLogicHeight );

public:
	bool CanMove( const CGUIPoint& rPoint ) const;
	void CenterControl( CControl* pControl ) const;
	void InsertControl( CControl* pControl, unsigned int vArchor );
	void RemoveControl( CControl* pControl );
	void TabControl( CControl*& pLastFocus );
	CGUIRect GetVScrollBarRect( ) const;
	CGUIRect GetHScrollBarRect( ) const;
	CGUIRect GetClientRect( ) const;
	CGUIRect GetCaptionRect( ) const;
	CGUIRect GetCloseRect( ) const;

public:
	virtual void OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags );

public:
	virtual void ClearControl( );
	virtual CControl* GetSubControl( const CGUIPoint& rPoint ) const;
	virtual CControl* GetSubControl( unsigned int  vID ) const;

	int GetSubControlCount( );
	CControl* GetSubControlByIndex( int vIndex );

public:
	static void OnHScrollBar( CControlScrollBar* pScrollBar );
	static void OnVScrollBar( CControlScrollBar* pScrollBar );
};

// ************************************************************ //
// CControlPopup
//
// ����ʽ�Ի�������ر�ǰ��������Ϣ���ᷢ���˿ؼ���
// ************************************************************ //

class CControlPopup : public CControlDialog
{
public:
	CControlPopup( unsigned int vDialogStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlPopup( );

public:
	// �õ�������С�Ĺ�����Χ
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );
};

// ************************************************************ //
// CMenuItem
// ************************************************************ //

class CMenuItem
{
	friend class CControlMenu;

protected:
	CMenuItem*					mpParent;
	CMenuItem*					mpExpand;
	std::vector<CMenuItem*>		mItems;
	std::string					mText;
	unsigned int				mStyle;
	unsigned int				mMenuID;
	bool						mIsHover;
	bool						mIsExpanded;
	CGUIRect					mRect;				// �ò˵���������С
	CGUIRect					mPopupRect;			// ����ò˵�����MSF_POPUPMENU, �������Ӳ˵������С

public:
	CMenuItem( const char* pText, unsigned int vMenuID, unsigned int vStyle );
public:
	~CMenuItem( );

public:
	CMenuItem* HitTest( const CGUIPoint& rPoint );
	bool Expand( );
	void UnExpand( );
	void DrawItems( CControlMenu* pMenu );
	void GetRectRange( CGUIRect& rRect );
};

// ************************************************************ //
// CControlMenu
// ************************************************************ //

class CControlMenu : public CControl
{
	typedef void (*OnItemClickedHandler)( CControlMenu*, unsigned int vMenuID );

protected:
	const int mItemHeight;		// �˵���߶�
	const int mHorzPrefix;		// �˵�����߿�
	const int mVertPrefix;		// �˵�����߿�

	CMenuItem*				mpRoot;
	CMenuItem*				mpHover;
	OnItemClickedHandler	mpItemClickedHandler;

public:
	CControlMenu( const CGUIPoint& rPoint, unsigned int vID );
public:
	virtual ~CControlMenu( );

public:
	void* AppendItem( void* vParentMenu, const char* pText, unsigned int vMenuID, unsigned int vStyle );

public:
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );

public:
	void SetItemClickedHandler( OnItemClickedHandler pItemClickedHandler )
		{ mpItemClickedHandler = pItemClickedHandler; }
	void SetItemData( int vItemIndex, unsigned int vUserData );
	unsigned int GetItemData( int vItemIndex );

public:
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnHover( const CGUIPoint& rPoint, bool vHover );

public:
	virtual void OnDrawItem( const char* pText, const CGUIRect& rRect, unsigned int vStyle, bool vIsHover );
	virtual void OnDrawMenu( const CGUIRect& rRect );
};

// ************************************************************ //
// CControlPage
//
// ��Ϸ�����ڣ����������ؼ������������𴰿���Ϣ�ķ��͡�
// ************************************************************ //

class CControlPage : public CControlDialog
{
protected:
	CGUIPoint						mDownPoint;
	CGUIPoint						mMousePoint;
	CControl*						mpLastFocus;			// ��ǰ����ؼ�
	CControl*						mpLastHover;			// ��ǰ��ͣ�ؼ�
	CControl*						mpDragSource;			// �ϷŲ���Դ�ؼ�
	CControl*						mpCapture;				
	std::vector<CControlDialog*>	mPopups;				// �򿪵ĵ���ʽ�Ի���
	CGUITexture*					mpDragImage;
	CGUIRect						mImageRect;
	unsigned int					mToolTipTick;
	CControlMenu*					mpPopupMenu;

public:
	CControlPage( const CGUIRect& rRect );
public:
	virtual ~CControlPage( );

public:
	// ���ý���ؼ���
	void SetFocus( CControl* pFocus );
	// ������ͣ�ؼ���
	void SetHover( CControl* pHover )		{ mpLastHover = pHover; }
	void SetCapture( CControl* pCapture )	{ mpCapture = pCapture; }
	CControl* GetFocus( )					{ return mpLastFocus; }

	// ����һ���Ի���
	void CreatePopup( CControlDialog* pControlPopup );
	// �ر����ϲ�ĵ���ʽ�Ի���
	void ClosePopup( CControlDialog* pPopup );
	// �Ƿ��е�����򿪡�
	bool HasPopup( ) const { return mPopups.size( ) != 0; }

	// ���ӿؼ����ö���
	void BringControlToTop( CControl* pControl );
	void BringControlToBottom( CControl* pControl );
	void SetDragImage( CGUITexture* pTexture, const CGUIRect& rSrcRect );

	void TrackPopupMenu( CControlMenu* pMenu );

public:
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseRButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseRButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseRButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnKeyUp( unsigned int vKeyCode, unsigned int vFlags );
	virtual void OnKeyDown( unsigned int vKeyCode, unsigned int vFlags );
	virtual void OnSysKeyDown( unsigned int vKeyCode, unsigned int vFlags );
	virtual void OnChar( unsigned int vCharCode, unsigned int vFlags );

	virtual CControl* GetSubControl( const CGUIPoint& rPoint );
	virtual CControl* GetSubControl( unsigned int vID );

	virtual void ShowPage( bool vIsShow );
	virtual void UpdateStatus( );

	virtual void Draw( );
};

// ************************************************************ //
// CControlButton
//
// ��ť�ؼ����Զ����������Ϣ���ṩ����¼��Ĵ���
// ��ťͼƬΪ���ţ�����Ϊ����������ͣ�����¡���ֹ��
// ************************************************************ //

class CControlButton : public CControl
{
	typedef void ( *OnClickedHandler )( CControlButton* );

protected:
	OnClickedHandler		mpClickedHandler;	// ����¼�������ָ��

public:
	CControlButton( char* pTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlButton( );

public:
	// �����¼���������
	void SetClickedHandler( OnClickedHandler pClickedHander )
		{ mpClickedHandler = pClickedHander; }

public:
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnHover( const CGUIPoint& rPoint, bool vHover );

public:
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );
};

// ************************************************************ //
// CControlCheck
//
// ��ѡ��ؼ����Զ����������Ϣ���ṩѡ���¼��Ĵ���
// ��ѡ��ͼƬΪ���ţ�����Ϊ����ѡ��������ѡ����������ѡ����ͣ��ѡ
// ����ͣ�����¡���ֹ��
// ************************************************************ //

class CControlCheck : public CControl
{
	typedef void ( *OnCheckedHandler )( CControlCheck*, bool );

protected:
	OnCheckedHandler		mpCheckedHandler;	// ѡ���¼�������ָ��
	bool					mIsChecked;
	const CGUISize			mBtnSize;

public:
	CControlCheck( char* pTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlCheck( );

public:
	bool GetCheck( ) { return mIsChecked; }
	void SetCheck( bool vCheck );
	// �����¼���������
	void SetCheckedHandler( OnCheckedHandler pCheckedHandler )
		{ mpCheckedHandler = pCheckedHandler; }

public:
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnHover( const CGUIPoint& rPoint, bool vHover );

public:
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );
};

// ************************************************************ //
// CControlEdit
// 
// �����ؼ����Զ���������ַ�������Ϣ����Ŀǰ֧�ֶ��ַ���
// ֧�ֶ��У�֧�ֻ��з��ţ��Զ����У���֧�ּ����������
// ֧�ֺ����Զ�������֧������������й���������, ��֧��������
// ע��:Edit������ֽڹ��ܼ��临��,�Ժ�����
// ************************************************************ //

class CTextObject
{
public:
	int			mCount;
	char		mText[3];		// ���ַ�����˫�ַ�,mText[0]�ĸ�λ����
	CGUIRect	mRect;

public:
	CTextObject( );
public:
	~CTextObject( );

public:
	int GetTextWidth( ) const;
	CGUIRect GetCursorRect( ) const;
};

class CControlEdit : public CControlDialog
{
public:
	typedef void ( *OnChangedHandler )( CControlEdit* );
	typedef void ( *OnKeyDownHandler )( CControlEdit*, unsigned int vKeyCode, unsigned int vFlags );

protected:
	OnChangedHandler			mpChangedHandler;	// �������ݸı��¼�������ָ��
	OnKeyDownHandler			mpKeyDownHandler;
	std::vector<CTextObject>	mTexts;				// ��ʽ������ı�
	int							mSelectionBegin;	// ѡ��Ŀ�ʼλ�ã��ֽ�Ϊ��λ
	int							mSelectionEnd;		// ѡ��Ľ���λ�ã��ֽ�Ϊ��λ
	int							mCursorIndex;		// ���λ�ã��ֽ�Ϊ��λ
	bool						mInputChinese;		// ��ǰ������Ƿ���
	unsigned int				mLastTickCount;
	unsigned int				mTurn;
	CTextObject					mCurrentInput;

public:
	CControlEdit( unsigned int vEditStyle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlEdit( );

protected:
	void SelectionLeft( int vOffset, int vCursorIndex );
	void SelectionRight( int vOffset, int vCursorIndext );
	void CursorLeft( int vShiftDown );
	void CursorRight( int vShiftDown );
	void CursorUp( int vShiftDown );
	void CursorDown( int vShiftDown );
	void VisibleCursor( );
	void UnSelection( );
	void DeleteSelection( );
	void CalcTextRect( );
	bool InsertChar( char vCharCode );
	int GetTextIndex( const CGUIPoint& rPoint ) const;
	CGUIRect GetCursorRect( ) const;
		
public:
	// �����¼���������
	void SetChangedHandler( OnChangedHandler pChangedHandler )
		{ mpChangedHandler = pChangedHandler; }
	void SetKeyDownHandler( OnKeyDownHandler pKeyDownHandler )
		{ mpKeyDownHandler = pKeyDownHandler; }

	void LineScroll( int vLine );
	void SetSel( int vStartChar, int vEndChar );
	void Clear( );
	void Copy( );
	void Paste( );
	void Cut( );
	void ResetContext( );

public:
	virtual void OnKeyDown( unsigned int vKeyCode, unsigned int vFlags );
	virtual void OnChar( unsigned int vCharCode, unsigned int vFlags );
	virtual void OnFocus( const CGUIPoint& rPoint, bool vFocus );
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnDrawShade( );

public:
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );

public:
	void SetEditText( const char* pText );
	std::string GetEditText( );
};

// ************************************************************ //
// CControlComboBox
// ************************************************************ //

class CControlComboBox : public CControlDialog
{
protected:
	const int	mComboHeight;
	int			mListHeight;

public:
	CControlComboBox( const CGUIRect& rRect, unsigned int vID );
public:
	~CControlComboBox( );

protected:
	CGUIRect GetListRect( ) const;
	CGUIRect GetComboRect( ) const;
	bool IsComboExpanded( ) const;
	void ComboExpand( );
	void ComboExpand( bool vExpand );

public:
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );

public:
	void AppendItem( const char* pText );

public:
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnFocus( const CGUIPoint& rPoint, bool vFocus );

public:
	static void OnComboItemClicked( CControlListBox* pListBox, int vItemIndex );
};

// ************************************************************ //
// CControlScrollBar
//
// �������ؼ����Զ����������Ϣ������������С��ȣ��߶ȣ�����
// ���£��������Ŀ�ȣ��߶ȣ����б����������С������һ���б���
// �ĸ߶ȡ�
// ************************************************************ //

class CControlScrollBar : public CControl
{
	typedef void ( *OnScrolledHandler )( CControlScrollBar* );

protected:
	OnScrolledHandler	mpScrolledHandler;	// �����¼�������ָ��
	unsigned int		mPageUpFlags;		// �Ϲ�������־
	unsigned int		mPageDownFlags;		// �¹�������־
	unsigned int		mCursorFlags;		// ��������־
	unsigned int		mLastTickCount;		// ��ʱ��
	CGUIRect			mCursorRect;		// ����������
	CGUIPoint			mMouseCursor;		// ������������ʱ������
	int					mRangeMax;			// ������������ʾ�����Χ
	int					mRangeMin;			// �������������ķ�Χ
	int					mLineSize;			// ����ʱһ�е���Գߴ�
	int					mPageSize;			// ����ʱһҳ����Գߴ�
	int					mPosition;			// ��ǰ��������ڹ������е����λ��
	int					mClickArea;			// ����������
	int					mScrollPoint;		// �����㣬����ڹ��������ε�Top

	// Դͼ�гߴ�
	CGUISize	mPageBtnSize;			// ���¹�����ť�ߴ�
	CGUISize	mCursorPrefixSize;		// ���鰴ť���°벿�ݳߴ�
	CGUISize	mScrollSliderSize;		// �����ߴ�
	CGUISize	mCursorSize;			// ���鰴ť�м�ߴ�

public:
	CControlScrollBar( unsigned int vScrollStyle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlScrollBar( );

protected:
	// ���ú�ȡ���Ϲ��������¹������͹������ı�־λ��
	void SetSubFlag( unsigned int& rSubFlags, unsigned int vFlag )			{ rSubFlags |= vFlag; }
	void UnsetSubFlag( unsigned int& rSubFlags, unsigned int vFlag )		{ rSubFlags &= ~vFlag; }
	bool IsSubFlag( unsigned int& rSubFlags, unsigned int vFlags ) const	{ return ( rSubFlags & vFlags ) != 0; }

	// �ƶ�������ꡣ
	void MoveCursor( const CGUIPoint& rPoint );

	virtual CGUIRect GetPageUpRect( ) const;
	virtual CGUIRect GetPageDownRect( ) const;
	virtual int GetMinCursorLength( ) const;

public:
	// �Ϸ�һ�С�
	void LineUp( )		{ SetPosition( mPosition - mLineSize ); }
	// �·�һ�С�
	void LineDown( )	{ SetPosition( mPosition + mLineSize ); }
	// �Ϸ�һҳ��
	void PageUp( )		{ SetPosition( mPosition - mPageSize ); }
	// �·�һҳ��
	void PageDown( )	{ SetPosition( mPosition + mPageSize ); }

	// ���ù�������ڹ������е����λ�á�
	void SetPosition( int vPosition );
	// ���ù�����������ʾ�����Χ�͹������������ķ�Χ��
	void SetRange( int vRangeMax, int vRangeMin );
	// ���ù���ʱһ�к�һҳ����Գߴ硣
	void SetScrollSize( int vLineSize, int vPageSize ) { mLineSize = vLineSize; mPageSize = vPageSize; }

	// ȡ�ù�������ڹ������е����λ�á�
	int  GetPosition( ) const	{ return mPosition; }
	// ȡ�ù�����������ʾ�����Χ��
	int  GetRangeMax( ) const	{ return mRangeMax; }
	// ȡ�ù������������ķ�Χ��
	int  GetRangeMin( ) const	{ return mRangeMin; }

	// �����¼���������
	void SetScrolledHandler( OnScrolledHandler pScrolledHandler )
		{ mpScrolledHandler = pScrolledHandler; }

public:
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );

public:
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnKeyDown( unsigned int vKeyCode, unsigned int vFlags );
	virtual void OnHover( const CGUIPoint& rPoint, bool vHover );
	virtual void UpdateStatus( );
	virtual void Offset( const CGUIPoint& rPoint );
	virtual void Offset( const CGUIPoint& rPoint, const CGUISize& rSize );
};

// ************************************************************ //
// CControlTree
//
// ���οؼ����Զ�������Ϣ��֧�ֺ���������������
// ************************************************************ //

class CTreeNode
{
	friend class CControlTree;

protected:
	std::vector<CTreeNode*>	mNodes;			// �ӽڵ�
	std::string				mText;			// �ڵ��ı�
	CGUIRect				mRect;			// �ڵ��������
	bool					mIsExpanded;	// �ڵ��Ƿ�չ��
	bool					mIsChecked;		// �ڵ��Ƿ�ѡ��
	bool					mIsSelected;

public:
	CTreeNode( const char* pText );
public:
	CTreeNode( );
public:
	~CTreeNode( );

public:
	int RefreshItems( int vLeft, int vTop, int vHeight, int& rWidth );
	void DrawItems( CControlTree* pTree );
	void Expand( );
	CTreeNode* HitTest( const CGUIPoint& rPoint );
};

class CControlTree : public CControlDialog
{
	friend class CTreeNode;

protected:
	CTreeNode*		mpFocus;
	CTreeNode*		mpRoot;		// ���ڵ�
	unsigned int	mStyle;
	const int		mItemHeight;

public:
	CControlTree( const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlTree( );

public:
	void* InsertItem( const char* pText, void* vParent = NULL, void* vInsertAfter = NULL );
	void RefreshItems( );

public:
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void Offset( const CGUIPoint& rPoint );
	virtual void Offset( const CGUIPoint& rPoint, const CGUISize& rSize );

public:
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );
	virtual void OnDrawItem( const char* pText, const CGUIRect& rRect, bool vIsExpanded, bool vIsChecked, bool vIsSelected, bool vIsLeafNode );
};

// ************************************************************ //
// CColumnItem
// ************************************************************ //
 
class CColumnItem
{
	friend class CControlList;

protected:
	std::string		mText;
	CGUIRect		mRect;
	bool			mIsClicked;
	int				mSortType;		// �������� 0:������ 1;���� 2:����
	int				mFormat;

public:
	CColumnItem( const char* pText );
public:
	~CColumnItem( );
};

// ************************************************************ //
// CGridItem
// ************************************************************ //

class CGridItem
{
	friend class CControlList;
	friend class CRowItem;

protected:
	std::string			mText;

public:
	CGridItem( const char* pText );
public:
	~CGridItem( );
};

// ************************************************************ //
// CRowItem
// ************************************************************ //

class CRowItem
{
	friend class CControlList;

protected:
	std::vector<CGridItem*>		mItems;
	bool						mIsSelected;
	unsigned int				mItemData;

public:
	CRowItem( );
public:
	~CRowItem( );
};

// ************************************************************ //
// CControlList
// ************************************************************ //

class CControlList : public CControlDialog
{
	typedef void ( *OnDblClkItem )( CControlList*, int );
	typedef void ( *OnClickItem )( CControlList*, int );
	
protected:
	const int					mItemHeight;
	const int					mColumnHeight;
	std::vector<CRowItem*>		mItems;
	std::vector<CGUIRect>		mSlotRects;
	std::vector<CColumnItem*>	mColumns;
	OnDblClkItem				mpDblClkItem;
	OnClickItem					mpClickItem;

public:
	CControlList( const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlList( );

public:
	void RemoveAllItems( );
	void RemoveItem( int vItem );
	void AppendColumn( const char* pText, int vWidth, int vFormat );
	int AppendItem( const char* pText );
	int HitTest( const CGUIPoint& rPoint );
	void SetItem( int vItem, int vSubItem, const char* pText );
	void SetItemData( int vItem, unsigned int vUserData );
	unsigned int GetItemData( int vItemIndex );
	std::string GetItemText( int vItemIndex );
	int GetCurSel( );
	void SetCurSel( int vItem );
	void SetDblClkHandler( OnDblClkItem pDblClkItem )	{ mpDblClkItem = pDblClkItem; }
	void SetClickHandler( OnClickItem pClickItem )		{ mpClickItem  = pClickItem;  }

protected:
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void Sort( int vColumnIndex, bool vIsAscending );
	virtual void OnDrawColumn( const char* pText, const CGUIRect& rRect, bool vIsClicked, int vSortType, int vFormat = GUI_LCSFMT_LEFT );
	virtual void OnDrawGrid( const char* pText, const CGUIRect& rRect, int vFormat );
	virtual void OnDrawItem( const CGUIRect& rRect, bool vIsSelected );
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );
};

// ************************************************************ //
// CListItem
// ************************************************************ //

class CListItem
{
	friend class CControlListBox;

protected:
	std::string		mText;
	CGUIPoint		mPoint;
	bool			mIsSelected;
	bool			mIsHover;
	unsigned int	mItemData;

public:
	CListItem( const char* pText );
public:
	~CListItem( );
};

// ************************************************************ //
// CControlListBox
// ************************************************************ //

class CControlListBox : public CControlDialog
{
	typedef void ( *OnItemHandler )( CControlListBox*, int );

protected:
	const int				mItemHeight;
	std::vector<CListItem*>	mItems;
	OnItemHandler			mpDblClkItem;
	OnItemHandler			mpClickItem;

public:
	CControlListBox( int vStyle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlListBox( );

protected:
	void RefreshItems( );
	int	HitTest( const CGUIPoint& rPoint );

public:
	void RemoveAllItems( );
	int AppendItem( const char* pText );
	void SetCurSel( int vItem );
	int GetCurSel( ) const;
	int GetItemCount( ) const									{ return int( mItems.size( ) );				}
	std::string GetItemText( int vItemIndex )					{ return mItems[vItemIndex]->mText;			}
	void SetItemData( int vItemIndex, unsigned int vItemData )	{ mItems[vItemIndex]->mItemData = vItemData; }
	unsigned int GetItemData( int vItemIndex ) const			{ return mItems[vItemIndex]->mItemData;		}
	void SetDblClkHandler( OnItemHandler pDblClkItem )			{ mpDblClkItem = pDblClkItem; }
	void SetClickedHandler( OnItemHandler pClickItem )			{ mpClickItem = pClickItem; }

public:
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags );
	virtual void OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags );

public:
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );
	virtual void OnDrawItem( const char* pText, const CGUIRect& rTextRect, const CGUIRect& rItemRect, bool vIsSelected, bool vIsHover );
};

// ************************************************************ //
// CControlSlider
// ��ťͼƬΪ���ţ�����Ϊ����������ͣ�����¡���ֹ��
// ************************************************************ //

class CControlSlider : public CControlScrollBar
{
public:
	// Դͼ�гߴ�
	CGUISize	mCursorSize;		// ����ߴ�
	CGUISize	mSliderSize;		// �����ߴ�

public:
	CControlSlider( unsigned int vStyle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlSlider( );

public:
	virtual int GetMinCursorLength( );
	virtual CGUIRect GetPageUpRect( ) const;
	virtual CGUIRect GetPageDownRect( ) const;
};

// ************************************************************ //
// CControlImageButton
//
// ��ť�ؼ����Զ����������Ϣ���ṩ����¼��Ĵ���
// ��ťͼƬΪ���ţ�����Ϊ����������ͣ�����¡���ֹ��
// ************************************************************ //

class CControlImageButton : public CControlButton
{
public:
	CGUISize	mImageSize;
	std::string mResName;
public:
	CControlImageButton( const char* pResName, const CGUISize& rImageSize, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlImageButton( );

public:
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );
};

// ************************************************************ //
// CControlImageCheck
//
// ��ť�ؼ����Զ����������Ϣ���ṩ����¼��Ĵ���
// ��ťͼƬΪ���ţ�����Ϊ����ѡ��������ѡ����������ѡ����ͣ��ѡ
// ����ͣ�����¡���ֹ��
// ************************************************************ //

class CControlImageCheck : public CControlCheck
{
public:
	CGUISize	mImageSize;
	std::string mResName;
public:
	CControlImageCheck( const char* pResName, const CGUISize& rImageSize, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlImageCheck( );

public:
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );
};

// ************************************************************ //
// CTabItem
// ************************************************************ //

class CTabItem
{
	friend class CControlTab;

protected:
	std::string		mText;
	CGUIRect		mRect;
	bool			mIsSelected;

public:
	CTabItem( const char* pText );
public:
	~CTabItem( );
};

// ************************************************************ //
// CControlTab
// ************************************************************ //

class CControlTab : public CControlDialog
{
	typedef void (*OnTabChangedHandler)( CControlTab*, unsigned int vNewIndex, unsigned int vOldIndex );

protected:
	std::vector<CTabItem*>		mItems;
	const	int					mTabActiveHeight;
	const	int					mTabUnactiveHeight;
	const	int					mBorderLength;
	const	int					mTabInterval;
	const	int					mHorzPrefix;
	const	int					mTabBtnPrefix;
	OnTabChangedHandler			mpTabChangedHandler;

public:
	CControlTab( const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlTab( );

public:
	void AppendItem( const char* pText );
	CGUIRect GetTabClientRect( ) const;
	void SetSel( int vIndex );
	void SetTabChangedHandler( OnTabChangedHandler pChangedHandler )	{ mpTabChangedHandler = pChangedHandler; }
public:
	virtual void OnDrawItem( const char* pText, const CGUIRect& rRect, bool vIsSelected );
	virtual void OnDrawTab( const CGUIRect& rRect );

public:
	virtual void OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags );

public:
	virtual void Draw( );
	virtual void InitControl( bool vIsInit );
};

// ************************************************************ //
// CMessageBox
// ************************************************************ //

class CMessageBox : public CControlPopup
{
public:
	typedef void ( *OnClicked )( CControlButton* );

protected:
	std::string		mText;
	CControlButton*	mpYes;
	CControlButton*	mpNo;

public:
	CMessageBox( unsigned int vStyle, const char* pTitle, const char* pText, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CMessageBox( );

public:
	void SetClickYes( OnClicked pClickYes )	{ if ( mpYes != NULL ) mpYes->SetClickedHandler( pClickYes );	}
	void SetClickNo( OnClicked pClickNo )	{ if ( mpNo != NULL ) mpNo->SetClickedHandler( pClickNo );		}
	void SetText( const char* pText )		{ mText = pText; }

public:
	virtual void Draw( );
	virtual void GetMinMaxInfo( CGUISize& rSizeMin, CGUISize& rSizeMax );
	virtual void Offset( const CGUIPoint& rPoint, const CGUISize& rSize );
	virtual void Offset( const CGUIPoint& rPoint );
};

// ************************************************************ //
// CControlProgress
// ************************************************************ //

class CControlProgress : public CControl
{
public:
	int		mRangeMin;
	int		mRangeMax;
	int		mPos;

public:
	CControlProgress( const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlProgress( );

protected:
	virtual void InitControl( bool vIsInit );
	virtual void Draw( );

public:
	void SetPos( int vPos )							{ mPos = vPos; }
	void SetRange( int vRangeMin, int vRangeMax )	{ mRangeMin = vRangeMin; mRangeMax = vRangeMax; }
	int GetPos( )									{ return mPos; }

protected:
	virtual void OnDrawShade( );
	virtual void OnDrawSlider( const CGUIRect& rRect );
};

// ************************************************************ //
// CPopupFileSystem
// ************************************************************ //

class CPopupFileSystem : public CControlPopup
{
	typedef void (*OnFileSelectHandler)( CPopupFileSystem* pFileSystem, const char* pFileName );

protected:
	std::string					mCurrentPath;
	std::vector<std::string>	mFileList;
	OnFileSelectHandler			mpFileSelected;

public:
	CPopupFileSystem( unsigned int vID );
public:
	virtual ~CPopupFileSystem( );

public:
	void InitControl( bool vIsInit );
	void Draw( );

public:
	void SetFileSelectedHandler( OnFileSelectHandler pClickedHandler ) { mpFileSelected = pClickedHandler; }
	std::string GetFileName( ) const;

protected:
	void RefreshFileList( );

protected:
	static void OnUpDirectory( CControlButton* );
	static void OnFileListDblClk( CControlList* pList, int vItemIndex );
	static void OnFileListClick( CControlList* pList, int vItemIndex );
	static void OnClickOK( CControlButton* pButton );
};

};