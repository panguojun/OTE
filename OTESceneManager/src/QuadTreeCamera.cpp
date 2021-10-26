#include <OgreMath.h>
#include <OgreAxisAlignedBox.h>
#include <OgreRoot.h>

#include "quadtreecamera.h"

namespace OTE
{

CQuadTreeCamera::CQuadTreeCamera(const Ogre::String& name, Ogre::SceneManager* sm) : 
Ogre::Camera(name, sm)
{
}

CQuadTreeCamera::~CQuadTreeCamera(void)
{
}


CQuadTreeCamera::eVisibility CQuadTreeCamera::GetVisibility( const Ogre::AxisAlignedBox &bound )
{		
	// Null boxes always invisible
	if ( bound.isNull() )
		return NONE;

	// Make any pending updates to the calculated frustum
	updateView();

	// Get corners of the box
	const Ogre::Vector3* pCorners = bound.getAllCorners();

	// For each plane, see if all points are on the negative side
	// If so, object is not visible.
	// If one or more are, it's partial.
	// If all aren't, full

	int corners[ 8 ] = {0, 4, 3, 5, 2, 6, 1, 7};
	
	int planes[ 6 ] = {	FRUSTUM_PLANE_TOP,	FRUSTUM_PLANE_BOTTOM,
						FRUSTUM_PLANE_LEFT,	FRUSTUM_PLANE_RIGHT,
						FRUSTUM_PLANE_FAR,	FRUSTUM_PLANE_NEAR };

	bool all_inside = true;

	for ( int plane = 0; plane < 6; ++plane )
	{

		// 远视面无限远的情况
		if (mFarDist == 0 && planes[ plane ] == FRUSTUM_PLANE_FAR)
			continue;

		bool all_outside = true;

		float distance = 0;

		for ( int corner = 0; corner < 8; ++corner )
		{
			distance = mFrustumPlanes[ planes[ plane ] ].getDistance( pCorners[ corners[ corner ] ] );
			all_outside = all_outside && ( distance < 0 );
			all_inside = all_inside && ( distance >= 0 );

			if ( !all_outside && !all_inside )
				break;
		}
		
		if ( all_outside )	// COOL!
			return NONE;
	}

	if ( all_inside )
		return FULL;
	else
		return PARTIAL;

}

}