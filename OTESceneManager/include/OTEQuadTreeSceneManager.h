#pragma once

#include "OgreSceneManager.h"
#include "OTEQuadTree.h"
#include "QuadTreeCamera.h"
#include "OTEStaticInclude.h"

namespace OTE
{
// **************************************************
// �Ĳ�������������
// **************************************************

typedef std::list < Ogre::WireBoundingBox * > BoxList;	

class _OTEExport CQuadTreeSceneManager : public Ogre::SceneManager
{
public:
	CQuadTreeSceneManager(const Ogre::AxisAlignedBox &maxBox, int maxDepth);
	~CQuadTreeSceneManager(void);

public:

	Ogre::AxisAlignedBox			m_Box;			// �����BoundingBox

protected:
	
	CQuadTree*						m_pQuadTree;	// �Ĳ���
	
	int								m_MaxDepth;		// ������
	
	int								m_NumObjects;	// �ɼ���������

	NodeList						m_Visible;		// �ɼ������б�
	
	BoxList							m_Boxes;		// �Ĳ���WireBox������(����ɼ�m_ShowBoxes == true)

	bool							m_ShowBoxes;	// �Ĳ���WireBox�Ƿ�ɼ�

public:
	
	/// ���������ڵ�

	virtual	Ogre::SceneNode * createSceneNode ( void );

	

	virtual Ogre::SceneNode * createSceneNode ( const Ogre::String &name );

	// ɾ�������ڵ�

	virtual void destroySceneNode( const Ogre::String &name );

	virtual Ogre::Camera * createCamera( const Ogre::String &name );		

	// ���ҿ���Ⱦ���������Ⱦ����

	virtual void _findVisibleObjects( Ogre::Camera * cam, bool onlyShadowCasters );

public:

	// �������ô�С

	void Resize( const Ogre::AxisAlignedBox &box );

	// ��ӽڵ�

	void AddQuadTreeNode(CQuadTreeNode * n, CQuadTree *qnode, int depth = 0 );
		
	// �ܳ��� 

	void WalkQuadTree( CQuadTreeCamera *camera, Ogre::RenderQueue *queue,
												CQuadTree *qn, bool foundvisible, bool onlyShadowCasters );
	
	// ��������

	void UpdateQuadTreeNode( CQuadTreeNode * qn );

	// �Ƴ��ڵ�

	void RemoveQuadTreeNode( CQuadTreeNode * n );

	virtual void clearScene(void);

	// ���ҽڵ�

	void FindNodes(std::list < Ogre::SceneNode * > &list);

private:

	// ��ʼ��

	void Init( const Ogre::AxisAlignedBox &box, int d );
	
	// ���˽ڵ�

	void FindNodes( const Ogre::AxisAlignedBox &t, std::list < Ogre::SceneNode * > &list, Ogre::SceneNode *exclude, bool full, CQuadTree *qt );
		
};

}