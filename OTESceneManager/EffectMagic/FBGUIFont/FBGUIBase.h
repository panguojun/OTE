#pragma once

#include "FBGUIConfig.h"		// bring in configuration options
#include "FBGUIVersion.h"		// add FBGUI version defines
#include "FBGUIDeclare.h"		// bring in FBGUI forward Declare
#include "FBGUIDef.h"			// FBGUI macro, error define, control style etc.

#define IDC_MESSAGEBOX_BUTTON_OK			60000
#define IDC_MESSAGEBOX_BUTTON_CANCEL		60001
#define IDC_FILEPOPUP_LIST_FILELIST			60002
#define IDC_FILEPOPUP_EDIT_FILENAME			60003
#define IDC_FILEPOPUP_BUTTON_UPPER			60004
#define IDC_FILEPOPUP_BUTTON_OK				60005
#define IDC_COMBODIALOG_LIST_ITEMLIST		60006

#define GUI_INITEXTURE( tIsInit, pTexture, pTexName ) ( tIsInit ) ? ( pTexture ) = CControl::spTextureFactory->LoadTexture( ( pTexName ) ) : delete pTexture;
#define GUI_MAKECOLOUR( tAlpha, tRed, tGreen, tBlue ) ( tAlpha ) << 24 | ( tRed ) << 16 | ( tGreen ) << 8 | ( tBlue )
#define GUI_MIN( a, b )	( ( a ) > ( b ) ? ( b ) : ( a ) )
#define GUI_MAX( a, b )	( ( a ) > ( b ) ? ( a ) : ( b ) )
#define WHEEL_DELTA 120