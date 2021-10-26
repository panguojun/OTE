#pragma once

#include <OgreAxisAlignedBox.h>
#include <OgreWireBoundingBox.h>
#include <OgreSceneNode.h>

namespace OTE
{
// ***********************************************
// �Ĳ������ 
// ***********************************************	

class CQuadTree;

class CQuadTreeNode :  public Ogre::SceneNode
{
public:

	CQuadTreeNode(Ogre::SceneManager* creator, const Ogre::String& name);

	CQuadTreeNode(Ogre::SceneManager* creator);

public:

	virtual void _addToRenderQueue( Ogre::Camera* cam, Ogre::RenderQueue * q, bool onlyShadowCasters );

	virtual void getRenderOperation( Ogre::RenderOperation& op );
	
	// ��Ⱦʱ�����Ĳ���

	virtual void _updateBounds( void );


	Ogre::AxisAlignedBox	GetLocalAABB()
	{
		return m_LocalAABB; 
	}	

	bool IsIn( const Ogre::AxisAlignedBox& box );
	
	
	CQuadTree * GetQuadTree()
	{
		return m_pQuadTree;
	};
	
	void SetQuadTree( CQuadTree *qt )
	{
		m_pQuadTree = qt;
	};

	Ogre::Node * RemoveChild( unsigned short index );	    

	Ogre::Node * RemoveChild( const Ogre::String & name );
	
	Ogre::Node * RemoveChild( Ogre::Node* child);
	
public:		

	Ogre::AxisAlignedBox		m_LocalAABB;	// Local AABB Box

	CQuadTree*					m_pQuadTree;	// ����Ĳ���

protected:	

	void RemoveNodeAndChildren();
};	


// ***********************************************
// �Ĳ��� 
// ***********************************************	

typedef std::list < CQuadTreeNode * > NodeList;

class CQuadTree  
{

public:
	CQuadTree(CQuadTree* parent);
	virtual ~CQuadTree();

public:	

	NodeList					m_Nodes;

	int							m_NumNodes;		// ���Լ������ڵ������ܺ�

	Ogre::AxisAlignedBox		m_Box;
		
	CQuadTree*					m_Children[2][2];

	CQuadTree*					m_pParent;

	Ogre::Vector3				m_HalfSize;

	Ogre::WireBoundingBox*		m_pWireBoundingBox;

public:	

	bool IsTwiceSize(const Ogre::AxisAlignedBox &box);

	void AddNode( CQuadTreeNode * );
	
	void RemoveNode( CQuadTreeNode * );
	
	int NumNodes()
	{
		return m_NumNodes;
	};	
	
	// ����λ�õõ�������

	void GetChildIndexes( Ogre::AxisAlignedBox &box, int *x, int *z );
	
	// �ü�BOX(2���߳�)

	void GetCullBounds( Ogre::AxisAlignedBox *b )
	{
		const Ogre::Vector3 * corners = m_Box.getAllCorners();
		b -> setExtents( corners[ 0 ] - m_HalfSize, corners[ 4 ] + m_HalfSize );	
	}
	
	// �߿�BOX

	Ogre::WireBoundingBox* GetWireBoundingBox()
	{			
		if(!m_pWireBoundingBox)
			m_pWireBoundingBox = new Ogre::WireBoundingBox();

		m_pWireBoundingBox->setupBoundingBox(m_Box);
		return m_pWireBoundingBox;
	}

protected:

	void Clear();

	
	inline void Ref()
	{
		m_NumNodes++;

		if ( m_pParent != 0 ) m_pParent -> Ref();
	};
	
	inline void Unref()
	{
		m_NumNodes--;

		if ( m_pParent != 0 ) m_pParent -> Unref();
	};

};

}