#pragma once
#include "OTEStaticInclude.h"
#include "OTETile.h"

namespace OTE
{

// ******************************************
// 鼠标操作的标示

#define		AUTOLINK_OPER_ADD			0xF0000000		// 添加操作
#define		AUTOLINK_RAISE				0x00F00000		// 升高
#define		AUTOLINK_UPDATETERRAIN		0x0000000F		// 更新山体

// ******************************************
// COTEEntityAutoLink
// ******************************************

class _OTEExport COTEEntityAutoLink
{
public:	
	static COTEEntityAutoLink*	GetInstance();	

public:	
	
	// 自动连接调用接口

	void AutoLink(float X, float Z, unsigned int operFlag);
	
	// 前缀

	void SetMeshGroupNamePrefix(const Ogre::String& prefix)
	{
		m_MeshGroupNamePrefix = prefix;
	}
	void SetEntityGroupNamePrefix(const Ogre::String& prefix)
	{
		m_EntGroupNamePrefix = prefix;
	}

	// 清理高度　准备根据地表高度重新制定悬崖高度　

	void ClearHeightCach()
	{
		m_LastHgt = -1.0f;
		m_LastBottomHgt = -1.0f;
	}

	// 设置高度

	void SetBottomHeight(float height)
	{	
		ClearHeightCach();
		m_LastBottomHgt = height;
	}

protected:	

	COTEEntityAutoLink();

	void AutoLink(int uX, int uZ, int uIndex, int vIndex, short cornerType, unsigned int operFlag);

	void UpdateEntities(int uX, int uZ, int u, int v, unsigned int operFlag);

	void GetWorldPosition(int uX, int uZ, Ogre::Vector3& position);

	COTEEntity* GetALEntityAtPoint(int uX, int uZ, int& u, int& v);

	void UpdateBorder(int uX, int uZ, int offsetX, int offsetZ, unsigned int operFlag);	
	
protected:	

	static COTEEntityAutoLink s_Inst;

	Ogre::Vector3				m_Scale;					// 缩放比

	Ogre::String				m_MeshGroupNamePrefix;		// 模型组前缀名			
	Ogre::String				m_EntGroupNamePrefix;		// 实体组前缀名	
	
	float						m_LastHgt;					// 顶部高度
	float						m_LastBottomHgt;			// 底部高度
};


}