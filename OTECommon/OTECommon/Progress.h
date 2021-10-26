#ifndef _PROGRESS_H
#define _PROGRESS_H

#include <commctrl.h>
#pragma comment( lib, "comctl32.lib" )

#include "..\OTEEditors\Plugin_TerrainEditor\Resource.h"

#define PROGRESS_DIALOG_ID		 ( IDD_PROGRESS_DIALOG )
#define PROGRESS_TEXTBOX_ID		 ( IDC_PROGRESS_TEXT )
#define PROGRESS_PROGRESSBAR_ID	 ( IDC_PROGRESS_BAR )
#define PROGRESS_CANCELBUTTON_ID ( ID_CANCEL )
#define PROGRESS_WM_MESSAGE		 ( WM_USER + 1234 )
#define PROGRESS_TERMINATE_DELAY 500
#define PROGRESS_CONTROL_CLASSES ( ICC_PROGRESS_CLASS )	

class Progress;
typedef struct ProgressUserProcData PROGRESSDATA;
typedef bool ( *LP_PROGRESS_USERPROC )( const PROGRESSDATA* );

typedef struct ProgressThreadData
{
protected:
	ProgressThreadData() { ::ZeroMemory( this, sizeof( ProgressThreadData ) ); }

public:
	HWND hThreadWnd;	
	LPVOID pUserProcParam;	
	bool bAlive;	
	bool bTerminate;

	LP_PROGRESS_USERPROC m_lpUserProc;

	enum{ WM_DISABLECONTROLS = PROGRESS_WM_MESSAGE,
		  WM_PROGRESSTHREADCOMPLETED,
		  WM_PROGRESSBARUPDATE,
		  WM_PROGRESSTEXTUPDATE,
		  WM_CANCELPROGRESSTHREAD };

} PROGRESSTHREADDATA;

typedef PROGRESSTHREADDATA *LPPROGRESSTHREADDATA;

struct ProgressUserProcData: private ProgressThreadData
{
	friend class Progress;

public:
	inline LPVOID GetAppData() const { return this->pUserProcParam; }
	inline bool ShouldTerminate() const	{ return this->bTerminate; }
	
	inline void SetProgress( LPCTSTR lpszProgressText ) const
	{
		if( ::IsWindow( this->hThreadWnd ) && this->bTerminate == false )
			::SendMessage( this->hThreadWnd, ProgressThreadData::WM_PROGRESSTEXTUPDATE, 0, ( LPARAM )lpszProgressText );
	}

	inline void SetProgress( UINT_PTR dwProgressbarPos ) const
	{
		if( ::IsWindow( this->hThreadWnd ) && this->bTerminate == false )
			::SendMessage( this->hThreadWnd, ProgressThreadData::WM_PROGRESSBARUPDATE, dwProgressbarPos, 0 );
	}

	inline void SetProgress( LPCTSTR lpszProgressText, UINT_PTR dwProgressbarPos ) const
	{
		SetProgress( lpszProgressText );
		SetProgress( dwProgressbarPos );
	}

private:
	ProgressUserProcData() { ::ZeroMemory( this, sizeof( PROGRESSDATA ) ); }
};

// Progress 对话框
class Progress
{
public:
	Progress( HWND hParentWnd, LP_PROGRESS_USERPROC lpUserProc, LPVOID lpUserProcParam, LPCTSTR lpszDlgTitle = _T( "请等待。。。" ), bool bAllowCancel = true );
	virtual ~Progress();

	INT_PTR DoModal( HINSTANCE hInstance );
	enum { IDD = PROGRESS_DIALOG_ID };

private:	
	friend INT_PTR CALLBACK ProgressDlgProc( HWND, UINT, WPARAM, LPARAM );
	void Cleanup();

	static struct tagInitCommonControls { tagInitCommonControls(); }m_InitCommonControls;

	HWND m_hWnd;
	HWND m_hParentWnd;
	bool m_bAllowCancel;
	HANDLE m_hThread;
	TCHAR m_szDialogCaption[ MAX_PATH ];
	PROGRESSDATA m_ThreadData;
};

//重写
class CProgressWnd: public CWnd
{
public:
    CProgressWnd();
    CProgressWnd( CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth = FALSE );
    virtual ~CProgressWnd();

    BOOL Create( CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth = FALSE );
    BOOL Modal( LPCTSTR pszTitle = _T( "Progress" ), BOOL bSmooth = FALSE );

    void SetRange( int nLower, int nUpper, int nStep = 1 );
    int OffsetPos( int nPos );        
    int StepIt();                   
    int SetStep( int nStep );         
    int SetPos( int nPos );           
    void SetText( LPCTSTR fmt, ... ); 
    void Clear();                   
    void Hide();                    
    void Show();                    

	void SetCancelled( BOOL bCancelled ) { m_bCancelled = bCancelled; }
    BOOL Cancelled() { return m_bCancelled; } 
	void SetWindowSize( int nNumTextLines, int nWindowWidth = 390 );
    void PeekAndPump( BOOL bCancelOnESCkey = TRUE );

	virtual BOOL DestroyWindow();

protected:  
	void CommonConstruct();

    void GetPreviousSettings();
    void SaveCurrentSettings();

    CWnd *m_wRenenableWnd;

    BOOL m_bCancelled;
    BOOL m_bModal;
    BOOL m_bPersistantPosition;
    int m_nPrevPos, m_nPrevPercent;
    int m_nStep;
    int m_nMaxValue, m_nMinValue;
    int m_nNumTextLines;

    CStatic m_Text;
    CProgressCtrl m_wndProgress;
    CButton m_CancelButton;
    CString m_strTitle, m_strCancelLabel;
    CFont m_font;

    afx_msg BOOL OnEraseBkgnd( CDC* pDC );
    afx_msg void OnCancel();
    DECLARE_MESSAGE_MAP()
};

#endif