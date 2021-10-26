#pragma once

#include "OgreSceneManager.h"
#include "OTEQuadTree.h"
#include "QuadTreeCamera.h"
#include "OTEStaticInclude.h"

namespace OTE
{
// **************************************************
// 四叉树场景管理器
// **************************************************

typedef std::list < Ogre::WireBoundingBox * > BoxList;	

class _OTEExport CQuadTreeSceneManager : public Ogre::SceneManager
{
public:
	CQuadTreeSceneManager(const Ogre::AxisAlignedBox &maxBox, int maxDepth);
	~CQuadTreeSceneManager(void);

public:

	Ogre::AxisAlignedBox			m_Box;			// 世界的BoundingBox

protected:
	
	CQuadTree*						m_pQuadTree;	// 四叉树
	
	int								m_MaxDepth;		// 最大深度
	
	int								m_NumObjects;	// 可见物体数量

	NodeList						m_Visible;		// 可见物体列表
	
	BoxList							m_Boxes;		// 四叉树WireBox的数量(如果可见m_ShowBoxes == true)

	bool							m_ShowBoxes;	// 四叉树WireBox是否可见

public:
	
	/// 创建场景节点

	virtual	Ogre::SceneNode * createSceneNode ( void );

	

	virtual Ogre::SceneNode * createSceneNode ( const Ogre::String &name );

	// 删除场景节点

	virtual void destroySceneNode( const Ogre::String &name );

	virtual Ogre::Camera * createCamera( const Ogre::String &name );		

	// 查找可渲染物件放入渲染队列

	virtual void _findVisibleObjects( Ogre::Camera * cam, bool onlyShadowCasters );

public:

	// 重新设置大小

	void Resize( const Ogre::AxisAlignedBox &box );

	// 添加节点

	void AddQuadTreeNode(CQuadTreeNode * n, CQuadTree *qnode, int depth = 0 );
		
	// 跑场景 

	void WalkQuadTree( CQuadTreeCamera *camera, Ogre::RenderQueue *queue,
												CQuadTree *qn, bool foundvisible, bool onlyShadowCasters );
	
	// 场景更新

	void UpdateQuadTreeNode( CQuadTreeNode * qn );

	// 移除节点

	void RemoveQuadTreeNode( CQuadTreeNode * n );

	virtual void clearScene(void);

	// 查找节点

	void FindNodes(std::list < Ogre::SceneNode * > &list);

private:

	// 初始化

	void Init( const Ogre::AxisAlignedBox &box, int d );
	
	// 过滤节点

	void FindNodes( const Ogre::AxisAlignedBox &t, std::list < Ogre::SceneNode * > &list, Ogre::SceneNode *exclude, bool full, CQuadTree *qt );
		
};

}