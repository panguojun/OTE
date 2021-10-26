#pragma once
#include "FBGUIRender.h"
#include "Math.h"

namespace FBGUILogicLayer
{
// ************************************************************ //
// CGUISystem
//
// 控件管理器，负责管理所有控件的配置信息。
// ************************************************************ //

class CControlManager 
{
protected:
	int							mLastPage;		// 上次显示的游戏界面页
	int							mCurrentPage;	// 当前显示的游戏界面页
	int							mNextPage;		// 即将显示的游戏界面页
	std::vector<CControlPage*>	mPages;			// 游戏界面数组
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
// 所有控件的基类，拥有控件的基本属性，负责处理消息相应。
// ************************************************************ //

class CControl
{
	friend class CControlManager;
	friend class CControlDialog;
	friend class CControlPage;
	friend class CControlRadioGroup;
	friend class CControlListBox;

	// 拖放动作的事情处理
	typedef void ( *OnDragFromHandler )( CControl*, CControl*, const CGUIPoint& );
	typedef void ( *OnDragToHandler )( CControl*, CControl*, const CGUIPoint& );

public:
	static CRenderDevice*		spRender;
	static CGUITextureFactory*	spTextureFactory;

protected:
	OnDragFromHandler			mpDragFromHandler;	// 点击列表项事件处理函数指针
	OnDragToHandler				mpDragToHandler;	// 点击列表项事件处理函数指针
	CControl*					mpParent;			// 父控件
	std::string					mTitle;				// 控件标题字符串
	unsigned int				mID;				// 控件 ID
	unsigned int				mFlags;				// 控件状态
	unsigned int				mStyle;				// 控件风格
	CGUIRect					mRect;
	CGUITexture*				mpTexture;
	
protected:
	CControl( unsigned int vStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControl( );

public:
	// 取得控件相关信息。
	CControlPage*	GetPage( ) const;
	CControl*		GetParent( ) const	{ return mpParent; }
	const CGUIRect&	GetRect( ) const	{ return mRect; }
	unsigned int	GetID( ) const		{ return mID; }
	unsigned int	GetFlags( ) const	{ return mFlags; }

	void SetRect( const CGUIRect& rRect )		{ mRect = rRect; }
	// 设置和取得标志位，用来表示一个控件的状态。
	void InitFlag( unsigned int vFlags )		{ mFlags = vFlags; }
	void SetFlag( unsigned int vFlag )			{ mFlags |= vFlag; }
	void UnsetFlag( unsigned int vFlag )		{ mFlags &= ~vFlag; }
	bool IsFlag( unsigned int vFlags ) const	{ return ( mFlags & vFlags ) != 0; }

	// 设置从控件拖放时的事件处理函数。
	void SetDragFromHandler( OnDragFromHandler pDragFromHandler )
		{ mpDragFromHandler = pDragFromHandler; }

	// 设置拖放到控件时的事件处理函数。
	void SetDragToHandler( OnDragToHandler pDragToHandler )
		{ mpDragToHandler = pDragToHandler; }

	void SetParent( CControl* pParent )			{ mpParent = pParent; }

public:
	// 事件处理虚函数，可以被子类重载。
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

	// 控件获得或失去焦点时的事件处理。
	virtual void OnFocus( const CGUIPoint& rPoint, bool vFocus );

	// 控件获得或失去悬停时的事件处理。
	virtual void OnHover( const CGUIPoint& rPoint, bool vHover )
		{ vHover ? SetFlag( GUI_CTL_HOVER ) : UnsetFlag( GUI_CTL_HOVER ); }

	// 移动控件。
	virtual void Offset( const CGUIPoint& rPoint )							{ mRect += rPoint; }
	virtual void Offset( const CGUIPoint& rPoint, const CGUISize& rSize );

	// 设置控件标题。
	virtual void SetTitle( const char* tTitle )	{ mTitle = tTitle; }
	// 取得控件标题。
	virtual const char* GetTitle( ) const		{ return mTitle.c_str( ); }
	// 渲染控件，不同的 UI 库可以自定义渲染方法。
	virtual void Draw( )							{ }
	virtual void UpdateStatus( )					{ }
	virtual void InitControl( bool vIsInit )		{ }
};

// ************************************************************ //
// CControlDialog
//
// 控件对话框，控件的容器，在其上可以放置其他任何控件。控件对话框
// 得到的所有消息都会发送到相应子控件中，自己本身不做处理。
// ************************************************************ //

class CControlDialog : public CControl
{
	friend class CControlPage;

protected:
	std::vector<CControl*>		mChildrenList;		// 子控件列表
	CControlScrollBar*			mpVScrollBar;		// 纵向滚动条
	CControlScrollBar*			mpHScrollBar;		// 横向滚动条
	CControlButton*				mpExit;
	CGUIPoint					mCamPoint;
	const int					mVScrollBarWidth;	// 纵向滚动条宽度
	const int					mHScrollBarHeight;	// 纵向滚动条高度
	const int					mCloseWidth;		// 关闭按钮宽度
	const int					mCloseHeight;		// 关闭按钮高度
	const int					mBorderLength;		// 边长度
	const int					mCaptionHeight;		// 标题栏高度

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
// 弹出式对话框，在其关闭前，所有消息都会发往此控件。
// ************************************************************ //

class CControlPopup : public CControlDialog
{
public:
	CControlPopup( unsigned int vDialogStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlPopup( );

public:
	// 得到最大和最小的滚动范围
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
	CGUIRect					mRect;				// 该菜单项的区域大小
	CGUIRect					mPopupRect;			// 如果该菜单项是MSF_POPUPMENU, 区域是子菜单整体大小

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
	const int mItemHeight;		// 菜单项高度
	const int mHorzPrefix;		// 菜单横向边宽
	const int mVertPrefix;		// 菜单纵向边宽

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
// 游戏主窗口，所有其他控件的容器，负责窗口消息的发送。
// ************************************************************ //

class CControlPage : public CControlDialog
{
protected:
	CGUIPoint						mDownPoint;
	CGUIPoint						mMousePoint;
	CControl*						mpLastFocus;			// 当前焦点控件
	CControl*						mpLastHover;			// 当前悬停控件
	CControl*						mpDragSource;			// 拖放操作源控件
	CControl*						mpCapture;				
	std::vector<CControlDialog*>	mPopups;				// 打开的弹出式对话框
	CGUITexture*					mpDragImage;
	CGUIRect						mImageRect;
	unsigned int					mToolTipTick;
	CControlMenu*					mpPopupMenu;

public:
	CControlPage( const CGUIRect& rRect );
public:
	virtual ~CControlPage( );

public:
	// 设置焦点控件。
	void SetFocus( CControl* pFocus );
	// 设置悬停控件。
	void SetHover( CControl* pHover )		{ mpLastHover = pHover; }
	void SetCapture( CControl* pCapture )	{ mpCapture = pCapture; }
	CControl* GetFocus( )					{ return mpLastFocus; }

	// 弹出一个对话框。
	void CreatePopup( CControlDialog* pControlPopup );
	// 关闭最上层的弹出式对话框。
	void ClosePopup( CControlDialog* pPopup );
	// 是否有弹出框打开。
	bool HasPopup( ) const { return mPopups.size( ) != 0; }

	// 将子控件放置顶层
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
// 按钮控件，自动处理鼠标消息，提供点击事件的处理。
// 按钮图片为纵排，依次为：正常、悬停、按下、禁止。
// ************************************************************ //

class CControlButton : public CControl
{
	typedef void ( *OnClickedHandler )( CControlButton* );

protected:
	OnClickedHandler		mpClickedHandler;	// 点击事件处理函数指针

public:
	CControlButton( char* pTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlButton( );

public:
	// 设置事件处理函数。
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
// 复选框控件，自动处理鼠标消息，提供选择事件的处理。
// 复选框图片为纵排，依次为：非选中正常、选中正常、非选中悬停、选
// 中悬停、按下、禁止。
// ************************************************************ //

class CControlCheck : public CControl
{
	typedef void ( *OnCheckedHandler )( CControlCheck*, bool );

protected:
	OnCheckedHandler		mpCheckedHandler;	// 选择事件处理函数指针
	bool					mIsChecked;
	const CGUISize			mBtnSize;

public:
	CControlCheck( char* pTitle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlCheck( );

public:
	bool GetCheck( ) { return mIsChecked; }
	void SetCheck( bool vCheck );
	// 设置事件处理函数。
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
// 输入框控件，自动处理键盘字符输入消息。（目前支持多字符）
// 支持多行，支持换行符号（自动换行），支持剪贴板操作，
// 支持横向自动滚动，支持纵向滚动，有滚动条功能, 不支持鼠标操作
// 注释:Edit处理多字节功能极其复杂,以后重做
// ************************************************************ //

class CTextObject
{
public:
	int			mCount;
	char		mText[3];		// 单字符或是双字符,mText[0]的高位决定
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
	OnChangedHandler			mpChangedHandler;	// 输入内容改变事件处理函数指针
	OnKeyDownHandler			mpKeyDownHandler;
	std::vector<CTextObject>	mTexts;				// 格式化后的文本
	int							mSelectionBegin;	// 选择的开始位置，字节为单位
	int							mSelectionEnd;		// 选择的结束位置，字节为单位
	int							mCursorIndex;		// 光标位置，字节为单位
	bool						mInputChinese;		// 当前输入的是否汉字
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
	// 设置事件处理函数。
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
// 滚动条控件，自动处理滚动消息，滚动光标的最小宽度（高度）是上
// （下）滚按键的宽度（高度），列表项滚动的最小距离是一个列表项
// 的高度。
// ************************************************************ //

class CControlScrollBar : public CControl
{
	typedef void ( *OnScrolledHandler )( CControlScrollBar* );

protected:
	OnScrolledHandler	mpScrolledHandler;	// 滚动事件处理函数指针
	unsigned int		mPageUpFlags;		// 上滚按键标志
	unsigned int		mPageDownFlags;		// 下滚按键标志
	unsigned int		mCursorFlags;		// 滚动光标标志
	unsigned int		mLastTickCount;		// 计时器
	CGUIRect			mCursorRect;		// 滚动光标矩形
	CGUIPoint			mMouseCursor;		// 鼠标点击滚动光标时的坐标
	int					mRangeMax;			// 滚动条所能显示的最大范围
	int					mRangeMin;			// 滚动光标所代表的范围
	int					mLineSize;			// 滚动时一行的相对尺寸
	int					mPageSize;			// 滚动时一页的相对尺寸
	int					mPosition;			// 当前滚动光标在滚动条中的相对位置
	int					mClickArea;			// 鼠标点中区域
	int					mScrollPoint;		// 滚动点，相对于滚动光标矩形的Top

	// 源图中尺寸
	CGUISize	mPageBtnSize;			// 上下滚动按钮尺寸
	CGUISize	mCursorPrefixSize;		// 滑块按钮上下半部份尺寸
	CGUISize	mScrollSliderSize;		// 滑道尺寸
	CGUISize	mCursorSize;			// 滑块按钮中间尺寸

public:
	CControlScrollBar( unsigned int vScrollStyle, const CGUIRect& rRect, unsigned int vID );
public:
	virtual ~CControlScrollBar( );

protected:
	// 设置和取得上滚按键、下滚按键和滚动光标的标志位。
	void SetSubFlag( unsigned int& rSubFlags, unsigned int vFlag )			{ rSubFlags |= vFlag; }
	void UnsetSubFlag( unsigned int& rSubFlags, unsigned int vFlag )		{ rSubFlags &= ~vFlag; }
	bool IsSubFlag( unsigned int& rSubFlags, unsigned int vFlags ) const	{ return ( rSubFlags & vFlags ) != 0; }

	// 移动滚动光标。
	void MoveCursor( const CGUIPoint& rPoint );

	virtual CGUIRect GetPageUpRect( ) const;
	virtual CGUIRect GetPageDownRect( ) const;
	virtual int GetMinCursorLength( ) const;

public:
	// 上翻一行。
	void LineUp( )		{ SetPosition( mPosition - mLineSize ); }
	// 下翻一行。
	void LineDown( )	{ SetPosition( mPosition + mLineSize ); }
	// 上翻一页。
	void PageUp( )		{ SetPosition( mPosition - mPageSize ); }
	// 下翻一页。
	void PageDown( )	{ SetPosition( mPosition + mPageSize ); }

	// 设置滚动光标在滚动条中的相对位置。
	void SetPosition( int vPosition );
	// 设置滚动条所能显示的最大范围和滚动光标所代表的范围。
	void SetRange( int vRangeMax, int vRangeMin );
	// 设置滚动时一行和一页的相对尺寸。
	void SetScrollSize( int vLineSize, int vPageSize ) { mLineSize = vLineSize; mPageSize = vPageSize; }

	// 取得滚动光标在滚动条中的相对位置。
	int  GetPosition( ) const	{ return mPosition; }
	// 取得滚动条所能显示的最大范围。
	int  GetRangeMax( ) const	{ return mRangeMax; }
	// 取得滚动光标所代表的范围。
	int  GetRangeMin( ) const	{ return mRangeMin; }

	// 设置事件处理函数。
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
// 树形控件，自动处理消息，支持横向滚动，纵向滚动
// ************************************************************ //

class CTreeNode
{
	friend class CControlTree;

protected:
	std::vector<CTreeNode*>	mNodes;			// 子节点
	std::string				mText;			// 节点文本
	CGUIRect				mRect;			// 节点矩形区域
	bool					mIsExpanded;	// 节点是否被展开
	bool					mIsChecked;		// 节点是否选中
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
	CTreeNode*		mpRoot;		// 根节点
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
	int				mSortType;		// 排序类型 0:无排序 1;升序 2:降序
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
// 按钮图片为纵排，依次为：正常、悬停、按下、禁止。
// ************************************************************ //

class CControlSlider : public CControlScrollBar
{
public:
	// 源图中尺寸
	CGUISize	mCursorSize;		// 滑块尺寸
	CGUISize	mSliderSize;		// 滑道尺寸

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
// 按钮控件，自动处理鼠标消息，提供点击事件的处理。
// 按钮图片为纵排，依次为：正常、悬停、按下、禁止。
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
// 按钮控件，自动处理鼠标消息，提供点击事件的处理。
// 按钮图片为纵排，依次为：非选中正常、选中正常、非选中悬停、选
// 中悬停、按下、禁止。
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