#include "OTEStaticInclude.h"
#include "otequadtree.h"
#include "OTEQuadTreeSceneManager.h"

namespace OTE
{
// ========================================================
// CQuadTreeNode
// ========================================================

CQuadTreeNode::CQuadTreeNode(Ogre::SceneManager* creator, const Ogre::String& name) : 
Ogre::SceneNode(creator, name), m_pQuadTree(NULL)
{		
}

CQuadTreeNode::CQuadTreeNode(Ogre::SceneManager* creator) : 
Ogre::SceneNode(creator), m_pQuadTree(NULL)
{		
}

// -------------------------------------------------------
void CQuadTreeNode::_addToRenderQueue( Ogre::Camera* cam, Ogre::RenderQueue *queue, bool onlyShadowCasters )
{
	Ogre::SceneNode::ObjectMap::iterator mit = mObjectsByName.begin();

	while ( mit != mObjectsByName.end() )
	{
		Ogre::MovableObject * mo = mit->second;

		mo->_notifyCurrentCamera(cam);
		if ( mo->isVisible() &&
			(!onlyShadowCasters || mo->getCastShadows()))
		{	
if( (!(::GetKeyState('E') & 0x80) || mo->getMovableType() != "OTEEntity" ) )
			mo -> _updateRenderQueue( queue );
else
{
	// 打印一些信息
	
	//OTE_TRACE("QuadTree NodeName: " << this->getName())
	//OTE_TRACE("QuadTree Node Object Number: " << mObjectsByName.size())	
}

		}

		++mit;
	}

}

// -------------------------------------------------------
void CQuadTreeNode::getRenderOperation( Ogre::RenderOperation& op )
{

}

// -------------------------------------------------------
void CQuadTreeNode::_updateBounds( void )
{
	mWorldAABB.setNull();
	m_LocalAABB.setNull();

	// Update bounds from own attached objects
	Ogre::SceneNode::ObjectMap::iterator i = mObjectsByName.begin();
	Ogre::AxisAlignedBox bx;

	while ( i != mObjectsByName.end() )
	{

		// Get local bounds of object
		bx = i->second ->getBoundingBox();

		m_LocalAABB.merge( bx );

		mWorldAABB.merge( i->second ->getWorldBoundingBox(true) );	

		++i;
	}


	//update the quadtreeSceneManager that things might have moved.
	// if it hasn't been added to the quadtree, add it, and if has moved
	// enough to leave it's current node, we'll update it.
	if ( ! mWorldAABB.isNull() )
	{
		static_cast < CQuadTreeSceneManager * > ( mCreator ) ->UpdateQuadTreeNode( this );
	}	
}

// -------------------------------------------------------
bool CQuadTreeNode::IsIn( const Ogre::AxisAlignedBox& box )
{
	// Always fail if not in the scene graph
	if (!mIsInSceneGraph) return false;

	Ogre::Vector3 center = mWorldAABB.getMaximum().midPoint( mWorldAABB.getMinimum() );

	Ogre::Vector3 bmin = box.getMinimum();
	Ogre::Vector3 bmax = box.getMaximum();

	return ( bmax > center && bmin < center );
}

// -------------------------------------------------------
// 节点管理
// -------------------------------------------------------

Ogre::Node * CQuadTreeNode::RemoveChild( unsigned short index )
{
	CQuadTreeNode *qn = static_cast<CQuadTreeNode* >( Ogre::SceneNode::removeChild( index ) );
	qn -> RemoveNodeAndChildren(); 
	return qn; 
}
// -------------------------------------------------------
Ogre::Node * CQuadTreeNode::RemoveChild( const Ogre::String & name )
{
	CQuadTreeNode *qn = static_cast<CQuadTreeNode* >( Ogre::SceneNode::removeChild( name ) );
	qn -> RemoveNodeAndChildren(); 
	return qn; 		
}
// -------------------------------------------------------
Ogre::Node * CQuadTreeNode::RemoveChild( Ogre::Node* child)
{	
	CQuadTreeNode *qn = static_cast<CQuadTreeNode* >( Ogre::SceneNode::removeChild( child ) );
	qn -> RemoveNodeAndChildren(); 
	return qn; 		
}	
// -------------------------------------------------------
void CQuadTreeNode::RemoveNodeAndChildren( )
{
	static_cast< CQuadTreeSceneManager * > ( mCreator ) -> RemoveQuadTreeNode( this ); 
	
	//remove all the children nodes as well from the quadtree.
	Ogre::SceneNode::ChildNodeMap::iterator it = mChildren.begin();
	while( it != mChildren.end() )
	{
		static_cast<CQuadTreeNode *>( it->second ) -> RemoveNodeAndChildren();
		++it;
	}
}

// ========================================================
// CQuadTree
// ========================================================

CQuadTree::CQuadTree(CQuadTree* parent)
{
	//initialize
	for(int i = 0; i < 2; i++)
	for(int j = 0; j < 2; j++)
	{
		m_Children[i][j] = 0;
	}

	m_pParent = parent;
	m_NumNodes = 0;
	m_pWireBoundingBox = 0;
}

CQuadTree::~CQuadTree(void)
{
	Clear();
}

// -------------------------------------------------------
void CQuadTree::Clear()
{
	//initialize all children to null.
	for(int i = 0; i < 2; i++)
	for(int j = 0; j < 2; j++)
	{
		if(m_Children[i][j])
			delete m_Children[i][j];
	}

	m_pParent = 0;
	m_NumNodes = 0;

	if(m_pWireBoundingBox)
		delete m_pWireBoundingBox;
}

// -------------------------------------------------------
void CQuadTree::AddNode(CQuadTreeNode * n)
{
	m_Nodes.push_back(n);
	n -> SetQuadTree( this );

	Ref();
}

// -------------------------------------------------------
void CQuadTree::RemoveNode( CQuadTreeNode * n )
{
	m_Nodes.erase(std::find(m_Nodes.begin(), m_Nodes.end(), n));
	n -> SetQuadTree ( NULL );

	Unref();
}

// -------------------------------------------------------
bool CQuadTree::IsTwiceSize(const Ogre::AxisAlignedBox &box)
{
	const Ogre::Vector3 * pts1 = m_Box.getAllCorners();
	const Ogre::Vector3 * pts2 = box.getAllCorners();

	return ( ( pts2[ 4 ].x -pts2[ 0 ].x ) <= ( pts1[ 4 ].x - pts1[ 0 ].x ) / 2 ) &&
		( ( pts2[ 4 ].z - pts2[ 0 ].z ) <= ( pts1[ 4 ].z - pts1[ 0 ].z ) / 2 );
}

// -------------------------------------------------------
void CQuadTree::GetChildIndexes( Ogre::AxisAlignedBox &box, int *x, int *z )
{
	Ogre::Vector3 max = m_Box.getMaximum();
	Ogre::Vector3 min = box.getMinimum();

	Ogre::Vector3 center = m_Box.getMaximum().midPoint( m_Box.getMinimum() );

	Ogre::Vector3 ncenter = box.getMaximum().midPoint( box.getMinimum() );

	if ( ncenter.x > center.x )
		*x = 1;
	else
		*x = 0;	

	if ( ncenter.z > center.z )
		*z = 1;
	else
		*z = 0;
}

}