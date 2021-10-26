// CustomDrawSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AnimSliderCtrl.h"

// the following are needed for the HLS to RGB (and RGB to HLS) conversion functions
#include <shlwapi.h>		
#pragma comment( lib, "shlwapi.lib" )

/////////////////////////////////////////////////////////////////////////////
// CAnimSliderCtrl

CAnimSliderCtrl::CAnimSliderCtrl()
{
	SetPrimaryColor( RGB(0, 255, 0) );	// defaults to a hideous green
		
	m_MaxAnimTime = 2.0;
	m_MinAnimTime = 0.0;
}

CAnimSliderCtrl::CAnimSliderCtrl( COLORREF cr )
{
	SetPrimaryColor( cr );	// user-selected color
}

CAnimSliderCtrl::~CAnimSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CAnimSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CAnimSliderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnReflectedCustomDraw)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimSliderCtrl message handlers

void CAnimSliderCtrl::SetPrimaryColor(COLORREF cr)
{
	// sets primary color of control, and derives shadow and hilite colors
	// also initializes brushes that are used in custom draw functions

	m_crPrimary = cr;

	// get hilite and shadow colors
	// uses the very-nice shell function of ColorAdjustLuma, described at
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/shlwapi/gdi/coloradjustluma.asp
	// for which we need at least IE version 5.0 and above, and a link to shlwapi.lib

	//m_crHilite		= ColorAdjustLuma( cr, 500, TRUE );	// increase by 50%
	//m_crMidShadow	= ColorAdjustLuma( cr, -210, TRUE );	// decrease by 21.0%
	//m_crShadow		= ColorAdjustLuma( cr, -333, TRUE );	// decrease by 33.3%
	//m_crDarkerShadow = ColorAdjustLuma( cr, -500, TRUE );	// decrease by 50.0%

	m_crHilite		= RGB(100, 0, 0);	// increase by 50%
	m_crMidShadow	= RGB(100, 0, 0);	// decrease by 21.0%
	m_crShadow		= RGB(100, 0, 0);	// decrease by 33.3%
	m_crDarkerShadow = RGB(100, 0, 0);	// decrease by 50.0%

	// create normal (solid) brush 

	if ( m_normalBrush.m_hObject )
		m_normalBrush.DeleteObject();
	
	m_normalBrush.CreateSolidBrush( cr );


	// create a hatch-patterned pixel pattern for patterned brush (used when thumb has focus/is selected)	

	// see http://www.codeproject.com/gdi/custom_pattern_brush.asp
	// or look for BrushTool.exe for the code that generates these bits
	
	WORD bitsBrush1[8] = { 0x0055,0x00aa,0x0055,0x00aa,0x0055,0x00aa,0x0055,0x00aa };
	
	CBitmap bm;
	bm.CreateBitmap( 8, 8, 1, 1, bitsBrush1);
	
	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_PATTERN;
	logBrush.lbHatch = (int) bm.GetSafeHandle();
	logBrush.lbColor = 0;		// ignored anyway; must set DC background and text colors
	
	if ( m_focusBrush.m_hObject )
		m_focusBrush.DeleteObject();

	m_focusBrush.CreateBrushIndirect(&logBrush);

}

void CAnimSliderCtrl::OnReflectedCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// for additional info, read beginning MSDN "Customizing a Control's Appearance Using Custom Draw" at
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/custdraw/custdraw.asp
	// slider controls (which are actually called "trackbar" controls) are specifically discussed at
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/trackbar/notifications/nm_customdraw_trackbar.asp
	
	NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNMHDR;
	
	UINT drawStage = nmcd.dwDrawStage;
	UINT itemSpec = nmcd.dwItemSpec;
		
	switch ( drawStage )
	{
		// drawstage flags (for debugging purposes; copied from CommCtrl.h)
		// values under 0x00010000 are reserved for global custom draw values.
		// above that are for specific controls
		// #define CDDS_PREPAINT           0x00000001
		// #define CDDS_POSTPAINT          0x00000002
		// #define CDDS_PREERASE           0x00000003
		// #define CDDS_POSTERASE          0x00000004
		// the 0x000010000 bit means it's individual item specific
		// #define CDDS_ITEM               0x00010000
		// #define CDDS_ITEMPREPAINT       (CDDS_ITEM | CDDS_PREPAINT)
		// #define CDDS_ITEMPOSTPAINT      (CDDS_ITEM | CDDS_POSTPAINT)
		// #define CDDS_ITEMPREERASE       (CDDS_ITEM | CDDS_PREERASE)
		// #define CDDS_ITEMPOSTERASE      (CDDS_ITEM | CDDS_POSTERASE)
				
		// custom draw item specs (for debugging purposes; copied from CommCtrl.h)
		// #define TBCD_TICS    0x0001
		// #define TBCD_THUMB   0x0002
		// #define TBCD_CHANNEL 0x0003

		// itemState flags (for debugging purposes; copied from CommCtrl.h)
		//	#define CDIS_SELECTED       0x0001
		//	#define CDIS_GRAYED         0x0002
		//	#define CDIS_DISABLED       0x0004
		//	#define CDIS_CHECKED        0x0008
		//	#define CDIS_FOCUS          0x0010
		//	#define CDIS_DEFAULT        0x0020
		//	#define CDIS_HOT            0x0040
		//	#define CDIS_MARKED         0x0080
		//	#define CDIS_INDETERMINATE  0x0100

	case CDDS_PREPAINT:		// Before the paint cycle begins
		
		// most important of the drawing stages
		// must return CDRF_NOTIFYITEMDRAW or else we will not get further 
		// NM_CUSTOMDRAW notifications for this drawing cycle
		// we also return CDRF_NOTIFYPOSTPAINT so that we will get post-paint notifications
		
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT ;
		break;
		
	case CDDS_PREERASE:		// Before the erase cycle begins
	case CDDS_POSTERASE:	// After the erase cycle is complete
	case CDDS_ITEMPREERASE:	// Before an item is erased
	case CDDS_ITEMPOSTERASE:	// After an item has been erased
		
		// these are not handled now, but you might like to do so in the future
		
		*pResult = CDRF_DODEFAULT;
		break;
		
	case CDDS_ITEMPREPAINT:	// Before an item is drawn
		
		// this is where we perform our item-specific custom drawing

		
		switch ( itemSpec )
		{
		case TBCD_CHANNEL:	// channel that the trackbar control's thumb marker slides along
			// For the pre-item-paint of the channel, we simply tell the control to draw the default
			// and then tell us when it's done drawing the channel (i.e., item-post-paint) using
			// CDRF_NOTIFYPOSTPAINT.  In post-item-paint of the channel, we draw a simple 
			// colored highlight in the channel's recatngle

			// Frankly, when I returned CDRF_SKIPDEFAULT, in an attempt to skip drawing here
			// and draw everything in post-paint, the control seems to ignore the CDRF_SKIPDEFAULT flag,
			// and it seems to draw the channel even if we returned CDRF_SKIPDEFAULT

			*pResult = CDRF_DODEFAULT| CDRF_NOTIFYPOSTPAINT;
			break;

		case TBCD_TICS:		// the increment tick marks that appear along the edge of the trackbar control
			// currently, there is no special drawing of the  tics

			*pResult = CDRF_DODEFAULT;
			break;
			
		case TBCD_THUMB:	// trackbar control's thumb marker. This is the portion of the control that the user moves
			// For the pre-item-paint of the thumb, we draw everything completely here, during item 
			// pre-paint, and then tell the control to skip default painting and NOT to notify 
			// us during post-paint.
			// If I asked for a post-paint notification, then for some reason, when the control gets to 
			// post-paint, it completely over-writes everthing that I do here, and draws the default thumb
			// (which is partially obscured by the thumb drawn here).  This happens even if in post-paint
			// I return another CDRF_SKIPDEFAULT.  I don't understand why.  
			// Anyway, it works fine if I draw everthing here, return CDRF_SKIPDEFAULT, and do not ask for
			// a post-paint item notification
			
			{
				CDC* pDC = CDC::FromHandle( nmcd.hdc );
				int iSaveDC = pDC->SaveDC();
				
				CBrush* pB = &m_normalBrush;
				CPen pen( PS_SOLID, 1, m_crShadow );
				
				// if thumb is selected/focussed, switch brushes
				
				if ( nmcd.uItemState && CDIS_FOCUS )
				{
					pB = &m_focusBrush;
					
					pDC->SetBrushOrg( nmcd.rc.right%8, nmcd.rc.top%8 );
					pDC->SetBkColor( m_crPrimary );
					pDC->SetTextColor( m_crHilite );				
				}
				
				pDC->SelectObject( pB );
				pDC->SelectObject( &pen );

//#if 1	// draw an ellipse

				//pDC->Ellipse( &(nmcd.rc) );

//#else	// draw a diamond
				
				int xx, yy, dx, dy, cx, cy;
				xx = nmcd.rc.left;
				yy = nmcd.rc.top;
				dx = 2;
				dy = 2;
				cx = nmcd.rc.right - xx - 1;
				cy = nmcd.rc.bottom - yy - 1;
				POINT pts[8] = { {xx+dx, yy}, {xx, yy+dy}, {xx, yy+cy-dy}, {xx+dx, yy+cy},
						{xx+cx-dx, yy+cy}, {xx+cx, yy+cy-dy}, {xx+cx, yy+dy}, {xx+cx-dx, yy} };

				pDC->Polygon( pts, 8 );

//#endif	// which shape to draw

				pDC->RestoreDC( iSaveDC );
				pDC->Detach();
			}
			*pResult = CDRF_SKIPDEFAULT;	// don't let control draw itself, or it will un-do our work
			break;
			
		default:
			ASSERT( FALSE );	// all of a slider's items have been listed, so we shouldn't get here
		};
		
		break;

	case CDDS_ITEMPOSTPAINT:	// After an item has been drawn

		switch ( itemSpec )
		{
		case TBCD_CHANNEL:	// channel that the trackbar control's thumb marker slides along
			// For the item-post-paint of the channel, we basically like what the control has drawn, 
			// which is a four-line high rectangle whose colors (in order) are white, mid-gray, black, 
			// and dark-gray.
			// However, to emphasize the control's color, we will replace the middle two lines
			// (i.e., the mid-gray and black lines) with hilite and shadow colors of the control
			// using CDC::Draw3DRect.
			{
				CDC* pDC = CDC::FromHandle( nmcd.hdc );

				RECT rrc = {nmcd.rc.left+1, nmcd.rc.top+1, nmcd.rc.right-1, nmcd.rc.bottom-1};
				pDC->Draw3dRect( &rrc, m_crMidShadow, m_crHilite );
				pDC->Detach();
			}
			*pResult = CDRF_SKIPDEFAULT;
			break;

		case TBCD_TICS:		// the increment tick marks that appear along the edge of the trackbar control
			// currently, there is no special post-item-paint drawing of the tics

			*pResult = CDRF_DODEFAULT;
			break;
			
		case TBCD_THUMB:	// trackbar control's thumb marker. This is the portion of the control that the user moves
			// currently, there is no special post-item-paint drawing for the thumb
			
			*pResult = CDRF_DODEFAULT ;	// don't let control draw itself, or it will un-do our work
			break;
			
		default:
			ASSERT( FALSE );	// all of a slider's items have been listed, so we shouldn't get here
		};

		break;

	case CDDS_POSTPAINT:		// After the paint cycle is complete
		// this is the post-paint for the entire control, and it's possible to add to whatever is 
		// now visible on the control
		// To emphasize the directivity of the control, we simply draw in two colored dots at the 
		// extreme edges of the control
		{
			CDC* pDC = CDC::FromHandle( nmcd.hdc );

			CBrush bWhite( RGB(255, 255, 255) );	// white brush
			CBrush bRed( RGB(255, 0, 0) );
			CBrush bDark( m_crDarkerShadow );		// dark but still colored brush
			CPen p(PS_SOLID, 1, m_crPrimary);

			CRect rClient;
			GetClientRect( &rClient );
			DWORD dwStyle = GetStyle();


			int cx = 8;
			CRect rrcFirst;( 1, 1, cx, cx );
			CRect rrcLast;

			// TBS_RIGHT, TBS_BOTTOM and TBS_HORZ are all defined as 0x0000, so avoid testing on them

			if ( dwStyle & TBS_VERT )
			{
				if ( dwStyle & TBS_LEFT )
				{
					rrcFirst = CRect( rClient.right-cx, 1, rClient.right-1, cx ); 
					rrcLast = CRect( rClient.right-cx, rClient.bottom-cx, rClient.right-1, rClient.bottom-1 );
				}
				else
				{
					rrcFirst = CRect( 1, 1, cx, cx ); 
					rrcLast = CRect( 1, rClient.bottom-cx, cx, rClient.bottom-1 );
				}
			}
			else
			{
				if ( dwStyle & TBS_TOP )
				{
					rrcFirst = CRect( 1, rClient.top+cx, cx, rClient.top+1 ); 
					rrcLast = CRect( rClient.right-cx, rClient.top+cx, rClient.right-1, rClient.top+1 );
				}
				else
				{
					rrcFirst = CRect( 1, 1, cx, cx ); 
					rrcLast = CRect( rClient.right-cx, 1, rClient.right-1, cx );
				}
			}
			
			int iSave = pDC->SaveDC();
			pDC->SelectObject( &bWhite );
			pDC->SelectObject( &p );
			pDC->Ellipse( &rrcFirst );
			pDC->SelectObject( &bDark );
			pDC->Ellipse( &rrcLast );		

			int Length = rClient.Width() - 26;

			//绘制动作生效帧
			pDC->SelectObject( &bRed );
			
			float perc = (m_AttackTime - m_MinAnimTime) / (m_MaxAnimTime - m_MinAnimTime);

			CRect AttRect(int(Length * perc) + 12, 0, int(Length * perc) + 16, 10);

			pDC->Rectangle(&AttRect);

			// 绘制桢列表
			std::list<KeyFrame_t>::iterator it = m_KeyList.begin();
			while(it != m_KeyList.end())
			{
				if((*it).foucsed)
					pDC->SelectObject( &bDark );
				else
					pDC->SelectObject( &bWhite );

				perc = ((*it).time - m_MinAnimTime) / (m_MaxAnimTime - m_MinAnimTime);
							
				CRect rect(int(Length * perc) + 12, 0, int(Length * perc) + 16, 10);
				pDC->Rectangle(&rect);

				++ it;
			}

			pDC->RestoreDC( iSave );
			pDC->Detach();
		}

		*pResult = CDRF_SKIPDEFAULT;
		break;



	default:			
		ASSERT( FALSE );	// all drawing stages are listed, so we shouldn't get here
		
	};
	
}

COLORREF CAnimSliderCtrl::GetPrimaryColor() const
{
	return m_crPrimary;
}

COLORREF CAnimSliderCtrl::GetHiliteColor() const
{
	return m_crHilite;
}

COLORREF CAnimSliderCtrl::GetShadowColor() const
{
	return m_crShadow;
}

// ----------------------------------------------------
// 点选

void CAnimSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rClient;
	GetClientRect( &rClient );
	int Length = rClient.Width() - 26;

	std::list<KeyFrame_t>::iterator it = m_KeyList.begin();
	while(it != m_KeyList.end())
	{
		float perc = ((*it).time - m_MinAnimTime) / (m_MaxAnimTime - m_MinAnimTime);
		CRect rect(Length * perc + 12, 0, Length * perc + 16,10);

		(*it).foucsed = false;
		if(rect.PtInRect(point))
		{
			(*it).foucsed = true;
		}

		++ it;
	}

	CSliderCtrl::OnLButtonDown(nFlags, point);

	InvalidateRect(NULL, false);
}

// ----------------------------------------------------
// 得到当前时间

float CAnimSliderCtrl::GetCurSliderTime()
{
	int pos = GetPos();
	float perc = (float)(pos - GetRangeMin()) / (GetRangeMax() - GetRangeMin());
	
	return perc * (m_MaxAnimTime - m_MinAnimTime) + m_MinAnimTime;
}	

