#include "otequadtreescenemanager.h"
#include "QuadTreeCamera.h"
namespace OTE
{
//	----------------------------------------------------------------------
CQuadTreeSceneManager::CQuadTreeSceneManager(const Ogre::AxisAlignedBox &maxBox, int maxDepth) : SceneManager()
{
	m_pQuadTree = NULL;	
	Init(maxBox, maxDepth);
}

CQuadTreeSceneManager::~CQuadTreeSceneManager(void)
{
	if(m_pQuadTree)
		delete m_pQuadTree;
}

//	----------------------------------------------------------------------
void CQuadTreeSceneManager::Init(const Ogre::AxisAlignedBox &box, int depth)
{
	if(mSceneRoot)
		delete mSceneRoot; 

	mSceneRoot = new CQuadTreeNode(this, "SceneRoot" );
	mSceneRoot->_notifyRootNode();

	if (m_pQuadTree)
		delete m_pQuadTree;

	m_pQuadTree = new CQuadTree(NULL);

	m_MaxDepth = depth;

	m_Box = box;
	m_pQuadTree->m_Box = box;

	Ogre::Vector3 min = box.getMinimum();
	Ogre::Vector3 max = box.getMaximum();

	m_pQuadTree-> m_HalfSize = ( max - min ) / 2;		

	m_NumObjects = 0;

	m_ShowBoxes = false;	// �Ƿ���ʾ�߿� 

	
}

//	----------------------------------------------------------------------
// �������óߴ�
//	----------------------------------------------------------------------

void CQuadTreeSceneManager::Resize( const Ogre::AxisAlignedBox &box )
{
	std::list < Ogre::SceneNode * > nodes;
	std::list < Ogre::SceneNode * > ::iterator it;

	FindNodes( m_pQuadTree->m_Box, nodes, 0, true, m_pQuadTree );

	delete m_pQuadTree;

	m_pQuadTree = new CQuadTree( 0 );
	m_pQuadTree->m_Box = box;

	it = nodes.begin();

	while ( it != nodes.end() )
	{
		CQuadTreeNode * qn = static_cast < CQuadTreeNode * > ( *it );
		qn -> SetQuadTree( 0 );
		UpdateQuadTreeNode( qn );
		++it;
	}

}

//	----------------------------------------------------------------------
//	�ڵ����
//	----------------------------------------------------------------------

Ogre::SceneNode * CQuadTreeSceneManager::createSceneNode (void)
{
	CQuadTreeNode* qn = new CQuadTreeNode(this);
	mSceneNodes[qn->getName()] = qn;

	return qn;	
}
	
Ogre::SceneNode * CQuadTreeSceneManager::createSceneNode (const Ogre::String &name)
{
	CQuadTreeNode* qn = new CQuadTreeNode(this, name);
	mSceneNodes[qn->getName()] = qn;

	return qn;
}

void CQuadTreeSceneManager::destroySceneNode( const Ogre::String &name )
{
	CQuadTreeNode * qn = static_cast < CQuadTreeNode* > ( getSceneNode( name ) );

	if ( qn != 0 )
		RemoveQuadTreeNode( qn );

	Ogre::SceneManager::destroySceneNode( name );
}

//	----------------------------------------------------------------------
//	�����
//	----------------------------------------------------------------------

Ogre::Camera * CQuadTreeSceneManager::createCamera( const Ogre::String &name )
{
	Ogre::Camera * c = new CQuadTreeCamera( name, this );
	mCameras.insert( Ogre::SceneManager::CameraList::value_type( name, c ) );
	return c;
}

//	----------------------------------------------------------------------
//	����Ĳ����ڵ�
//	----------------------------------------------------------------------

void CQuadTreeSceneManager::AddQuadTreeNode(CQuadTreeNode * n, CQuadTree *qt, int depth )
{
	Ogre::AxisAlignedBox bx = n->_getWorldAABB();

	//if the quadtree is twice as big as the scene node,
	//we will add it to a child.

	if ((depth < m_MaxDepth ) && qt->IsTwiceSize(bx))
	{
		int x, z;
		qt->GetChildIndexes( bx, &x, &z );

		if (!qt->m_Children[x][z])
		{
			qt->m_Children[x][z] = new CQuadTree(qt);

			const Ogre::Vector3 *corners = qt->m_Box.getAllCorners();
			Ogre::Vector3 min, max;
			
			// �߶ȵĴ���(ȡ����Χ�и߶�)

			min.y = 0;
			max.y = corners[4].y;

			if ( x == 0 )
			{
				min.x = corners[ 0 ].x;
				max.x = ( corners[ 0 ].x + corners[ 4 ].x ) / 2;
			}

			else
			{
				min.x = ( corners[ 0 ].x + corners[ 4 ].x ) / 2;
				max.x = corners[ 4 ].x;
			}		

			if ( z == 0 )
			{
				min.z = corners[ 0 ].z;
				max.z = ( corners[ 0 ].z + corners[ 4 ].z ) / 2;
			}

			else
			{
				min.z = ( corners[ 0 ].z + corners[ 4 ].z ) / 2;
				max.z = corners[ 4 ].z;
			}

			qt -> m_Children[ x ][ z ] -> m_Box.setExtents(min, max);
			qt -> m_Children[ x ][ z ] -> m_HalfSize = (max - min) / 2;
		}

		AddQuadTreeNode(n, qt -> m_Children[ x ][ z ], ++depth );
	}

	else
	{
		qt->AddNode( n );
	}
}

//	----------------------------------------------------------------------
//	��Box���˽ڵ�
//	----------------------------------------------------------------------

enum eIntersection
{
	OUTSIDE = 0,
	INSIDE = 1,
	INTERSECT = 2
};

eIntersection Intersect( const Ogre::AxisAlignedBox &one, const Ogre::AxisAlignedBox &two )
{	
	// Null box?
	if (one.isNull() || two.isNull()) return OUTSIDE;

	const Ogre::Vector3 * outside = one.getAllCorners();
	const Ogre::Vector3 *inside = two.getAllCorners();

	if (	inside[ 4 ].x < outside[ 0 ].x ||			
			inside[ 4 ].z < outside[ 0 ].z ||
			inside[ 0 ].x > outside[ 4 ].x ||			
			inside[ 0 ].z > outside[ 4 ].z )
	{
		return OUTSIDE;
	}

	bool full = (	inside[ 0 ].x > outside[ 0 ].x &&					
					inside[ 0 ].z > outside[ 0 ].z &&
					inside[ 4 ].x < outside[ 4 ].x &&					
					inside[ 4 ].z < outside[ 4 ].z );

	if ( full )
		return INSIDE;
	else
		return INTERSECT;

}

//	----------------------------------------------------------------------
void CQuadTreeSceneManager::FindNodes(std::list < Ogre::SceneNode * > &list)
{
	FindNodes( m_pQuadTree->m_Box, list, 0, true, m_pQuadTree );
}

//	----------------------------------------------------------------------
void CQuadTreeSceneManager::FindNodes( const Ogre::AxisAlignedBox &t, std::list < Ogre::SceneNode * > &list, Ogre::SceneNode *exclude, bool full, CQuadTree *qt )
{
	// Boxȫ�������Ĳ���

	if (!full)
	{
		Ogre::AxisAlignedBox obox;
		qt -> GetCullBounds( &obox );

		eIntersection isect = Intersect( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}

	// �������еĽڵ�

	NodeList::iterator it = qt -> m_Nodes.begin();

	while ( it != qt -> m_Nodes.end() )
	{
		CQuadTreeNode * qn = ( *it );

		if ( qn != exclude )
		{
			if(full) // ȫ������
			{
				list.push_back( qn );
			}

			else	// ���ְ���
			{
				eIntersection nsect = Intersect( t, qn -> _getWorldAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( qn );
				}
			}

		}

		++it;
	}

	if ( qt -> m_Children[ 0 ][ 0 ] != 0 )
		FindNodes( t, list, exclude, full, qt -> m_Children[ 0 ][ 0 ] );

	if ( qt -> m_Children[ 1 ][ 0 ] != 0 )
		FindNodes( t, list, exclude, full, qt -> m_Children[ 1 ][ 0 ] );

	if ( qt -> m_Children[ 0 ][ 1 ] != 0 )
		FindNodes( t, list, exclude, full, qt -> m_Children[ 0 ][ 1 ] );

	if ( qt -> m_Children[ 1 ][ 1 ] != 0 )
		FindNodes( t, list, exclude, full, qt -> m_Children[ 1 ][ 1 ] );

}

//	----------------------------------------------------------------------
//	WalkOn QuadTree
//	----------------------------------------------------------------------

void CQuadTreeSceneManager::WalkQuadTree( CQuadTreeCamera *camera, Ogre::RenderQueue *queue,
												CQuadTree *qt, bool foundvisible, bool onlyShadowCasters )
{		
	// return immediately if nothing is in the node.
	if ( qt -> NumNodes() == 0 )
		return ;

	CQuadTreeCamera::eVisibility v = CQuadTreeCamera::NONE;

	if ( foundvisible )
	{
		v = CQuadTreeCamera::FULL;
	}

	else if ( qt == m_pQuadTree )	// ���ڽڵ�Ϊ������������
	{
		v = CQuadTreeCamera::PARTIAL;
	}

	else
	{
		Ogre::AxisAlignedBox box;
		qt->GetCullBounds( &box );
		v = camera->GetVisibility( box );
	}


	// if the qu is visible, or if it's the root node...
	if ( v != CQuadTreeCamera::NONE )
	{
		// Add stuff to be rendered.	

		if ( m_ShowBoxes )
		{
			m_Boxes.push_back( qt->GetWireBoundingBox() );
		}	

		bool vis = true;

		// ɾ���սڵ�

		NodeList::iterator it = qt -> m_Nodes.begin();

		while ( it != qt -> m_Nodes.end() )
		{
			CQuadTreeNode * sn = *it;	

			// if this quadtree is partially visible, manually cull all
			// scene nodes attached directly to this level.
			
			if ( v == CQuadTreeCamera::PARTIAL )
				vis = camera -> isVisible( sn ->_getWorldAABB() );

			if ( vis )	// �ɼ�
			{
				m_NumObjects++;
				sn->_addToRenderQueue(camera, queue, onlyShadowCasters );

				m_Visible.push_back( sn );

				if ( mDisplayNodes )
					queue->addRenderable( sn );
					
				// check if the scene manager or this node wants the bounding box shown.

				if (sn->getShowBoundingBox() || mShowBoundingBoxes)
					sn->_addBoundingBoxToQueue(queue);					
			}

			++it;
		}


		if ( qt -> m_Children[ 0 ][ 0 ] != 0 )
			WalkQuadTree( camera, queue, qt -> m_Children[ 0 ][ 0 ], ( v == CQuadTreeCamera::FULL ), onlyShadowCasters );

		if ( qt -> m_Children[ 1 ][ 0 ] != 0 )
			WalkQuadTree( camera, queue, qt -> m_Children[ 1 ][ 0 ], ( v == CQuadTreeCamera::FULL ), onlyShadowCasters );

		if ( qt -> m_Children[ 0 ][ 1 ] != 0 )
			WalkQuadTree( camera, queue, qt -> m_Children[ 0 ][ 1 ], ( v == CQuadTreeCamera::FULL ), onlyShadowCasters );

		if ( qt -> m_Children[ 1 ][ 1 ] != 0 )
			WalkQuadTree( camera, queue, qt -> m_Children[ 1 ][ 1 ], ( v == CQuadTreeCamera::FULL ), onlyShadowCasters );

	}


}

//	----------------------------------------------------------------------
//	���ҿ�������
//	----------------------------------------------------------------------

void CQuadTreeSceneManager::_findVisibleObjects( Ogre::Camera * cam, bool onlyShadowCasters )
{
	getRenderQueue()->clear();
	m_Boxes.clear();
	m_Visible.clear();	

	m_NumObjects = 0;

	// �����������Լ�����Ⱦ�б� ���Ǿ�ֻ��Ⱦ�б��е�
	if(!cam->m_RenderableList.empty())
	{		
		for (std::vector<Ogre::Renderable*>::iterator it = cam->m_RenderableList.begin(); it != cam->m_RenderableList.end(); ++it )
		{
			getRenderQueue()->addRenderable(*it);
		}	
		cam->m_RenderableList.clear(); // ���������
	}
	else
	{	
		WalkQuadTree( static_cast < CQuadTreeCamera * > ( cam ), getRenderQueue(), m_pQuadTree, false, onlyShadowCasters );

		// Show the quadtree boxes & cull camera if required

		if ( m_ShowBoxes )
		{
			for ( BoxList::iterator it = m_Boxes.begin(); it != m_Boxes.end(); ++it )
			{
				getRenderQueue()->addRenderable(*it);
			}
		}	
	}
}

//	----------------------------------------------------------------------
//	���³���
//  ���������������������ø����߼�
//	----------------------------------------------------------------------

void CQuadTreeSceneManager::UpdateQuadTreeNode( CQuadTreeNode * qn )
{
	Ogre::AxisAlignedBox box = qn ->_getWorldAABB(); 

	if (box.isNull())
		return;

	if (qn -> GetQuadTree() == 0) 
	{
		//if outside the quadtree, force into the root node.
		if ( ! qn ->IsIn( m_pQuadTree -> m_Box ) ) 
			m_pQuadTree->AddNode( qn );
		else
			AddQuadTreeNode( qn, m_pQuadTree );
		return ;
	}

	if (!qn -> IsIn(qn -> GetQuadTree() -> m_Box))  // ��������ʹ�ýڵ��С������أ�
	{
		RemoveQuadTreeNode( qn ); 

		//if outside the quadtree, force into the root node.
		if ( ! qn -> IsIn( m_pQuadTree -> m_Box ) )
			m_pQuadTree->AddNode( qn );
		else
			AddQuadTreeNode( qn, m_pQuadTree );
	}
}

//	----------------------------------------------------------------------
// �Ƴ��ڵ�
//	----------------------------------------------------------------------
void CQuadTreeSceneManager::RemoveQuadTreeNode( CQuadTreeNode * n )
{
	CQuadTree * qt = n -> GetQuadTree();

	if ( qt )
	{
		qt -> RemoveNode( n );
	}

	n->SetQuadTree(0);
	
}


//	----------------------------------------------------------------------
//	������
//	----------------------------------------------------------------------

void CQuadTreeSceneManager::clearScene(void)
{

	SceneManager::clearScene();
	Init(m_Box, m_MaxDepth);
}	

}