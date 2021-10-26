#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "otelineobj.h"

// ========================================================
// C3DPathView
// 绘制网格的帮助物体
// ========================================================
class C3DPathView
{
public:

	std::string					m_EntName;

	std::string					m_Name;

	bool						m_IsVisible;

	std::vector<Ogre::Vector3>	m_NodeList;			

public:
	// ---------------------------------------------------
	C3DPathView(const std::string& name)
	{	
		Init(name); 
	}
	C3DPathView(){}

	// ---------------------------------------------------
	~C3DPathView()
	{
		Destroy();
	}

public:

	void Init(const std::string& name)
	{
		m_Name = name;
		m_IsVisible = true;
	}

	bool IsVisible()
	{
		return m_IsVisible;
	}
	void SetVisible(bool bVisible)
	{
		m_IsVisible = bVisible;
	}
	void Destroy()
	{
		ClearRes();
		m_IsVisible = false;
	}
	void ClearRes();

	void ClearNodes()
	{
		m_NodeList.clear();		
	}

	void Update();
		
	void AddPathNode(const Ogre::Vector3& nodePos);
};