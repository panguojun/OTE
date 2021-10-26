// **********************************************************************
// COTEPluginInterface
// **********************************************************************
#pragma once
#include "OgrePrerequisites.h"
#include "OTEStaticInclude.h"
#include "Ogre.h"

namespace OTE
{
// **********************************************************************
// Invalid control ID
#define INVALID_ID						-1

// **********************************************************************
class COTEPluginInterface;

// Menu type
struct OTEMenu_t
{
	int				MenuID;
	std::string		MenuParentTitle;
	std::string		MenuTitile;
	std::string		MenuTips;
	COTEPluginInterface *pPI;
};

// Tool type
struct OTETool_t
{
	int				ToolID;
	std::string		ToolTitile;
	std::string		IconFileName;
	std::string		ToolTips;
	COTEPluginInterface *pPI;
};

// **********************************************************************
// COTEPluginInterface
// 插件接口基类
// **********************************************************************

class _OTEExport COTEPluginInterface
{
public:

	/////// 初始化 ///////

	virtual void Init(HWND mainWnd){}
	
public:	

	/////// Event listeners ///////

	// create single
	virtual bool CreateOTEMenu(int menuID, std::string& title, std::string& toolTip){return false;}
	virtual bool CreateOTETool(int toolID, std::string& title, std::string& iconFileName, std::string& toolTip){return false;}
	
	virtual bool CreateMouseMoveListener(){return false;}
	virtual bool CreateLButtonDownListener(){return false;}
	virtual bool CreateLButtonUpListener(){return false;}
	virtual bool CreateRButtonDownListener(){return false;}
	virtual bool CreateRButtonUpListener(){return false;}
	virtual bool CreateWheelListener(){return false;}

	virtual bool CreateKeyEventListener(){return false;}

	// groups
	virtual int CreateOTEMenuGroup(int startMenuID, HashMap<int, OTEMenu_t>& rMenuGroup){return INVALID_ID;}
	virtual int CreateOTEToolGroup(int startToolID, HashMap<int, OTETool_t>& rToolGroup){return INVALID_ID;}
	
public:

	/////// 界面事件 ///////

	// Menu event invoke
	virtual void OnMenuEven(int menuID){}

	// Tool event invoke
	virtual void OnToolEven(int toolID){}

	// Mouse event invokes
	virtual bool OnMouseMove(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnLeftDClick(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnLButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnRButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnRButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnWheel(HWND hwnd, int x, int y, int delta, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	
	// Keyboard event invokes
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}
	virtual bool OnKeyUp(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShiftDown){return false;}

	/////// 事件系统 ///////

	virtual void OnSceneClear(){}

	virtual void OnCustomMessage(const std::string& msg){}	

};

}