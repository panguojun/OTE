#pragma once

#include <OgreCamera.h>
#include <OgreHardwareBufferManager.h>
#include <OgreSimpleRenderable.h>

namespace OTE
{
// ******************************************
// CQuadTreeCamera
// ******************************************

class CQuadTreeCamera : public Ogre::Camera
{
public:
	enum FrustumPlane	// 跟Ogre引擎的要保持匹配！
	{
		FRUSTUM_PLANE_NEAR   = 0,
		FRUSTUM_PLANE_FAR    = 1,
		FRUSTUM_PLANE_LEFT   = 2,
		FRUSTUM_PLANE_RIGHT  = 3,
		FRUSTUM_PLANE_TOP    = 4,
		FRUSTUM_PLANE_BOTTOM = 5
	};
	
	/** Visibility types */
	enum eVisibility
	{
		NONE,
		PARTIAL,
		FULL
	};
	
public:
	CQuadTreeCamera(const Ogre::String& name, Ogre::SceneManager* sm);	

	virtual ~CQuadTreeCamera(void);

public:

	eVisibility GetVisibility( const Ogre::AxisAlignedBox &bound );

};

}
