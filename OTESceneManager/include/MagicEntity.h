#pragma once

#include "OTEStaticInclude.h"
#include "ogrerenderable.h"
#include "ogreMovableObject.h"
#include "OgreAxisAlignedBox.h"
#include "OgreSceneManager.h"

class CDymMagicToOTE_Obj;
class CDymMagicToOTE;

namespace OTE
{

// ======================================================
// 特效实体对象
// ======================================================
class _OTEExport COTEMagicEntity :  public Ogre::MovableObject
{
public:
	COTEMagicEntity(const Ogre::String& name, Ogre::SceneManager *sm);
	virtual ~COTEMagicEntity(void);

public:
	/** Returns the name of this object. */
    virtual const Ogre::String& getName(void) const
	{
		return m_Name;
	}

    /** Returns the type name of this object. */
    virtual const Ogre::String& getMovableType(void) const
	{
		static std::string sType = "OTEMagicEntity";
		return sType;
	}

	/** Internal method to notify the object of the camera to be used for the next rendering operation.
	@remarks
		Certain objects may want to do specific processing based on the camera position. This method notifies
		them incase they wish to do this.
	*/
	virtual void _notifyCurrentCamera(Ogre::Camera* cam);

	/** Retrieves the local axis-aligned bounding box for this object.
		@remarks
			This bounding box is in local coordinates.
	*/
	virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const
	{
		return m_Box;		
	}

	/** Retrieves the radius of the origin-centered bounding sphere 
		 	for this object.
	*/
	virtual Ogre::Real getBoundingRadius(void) const
	{
		return (m_Box.getMaximum() - m_Box.getMinimum()).length() / 2.0f;
	}


	/** Internal method by which the movable object must add Renderable subclass instances to the rendering queue.
    @remarks
        The engine will call this method when this object is to be rendered. The object must then create one or more
        Renderable subclass instances which it places on the passed in Queue for rendering.
    */
    virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

	// 可见性
	inline void SetVisible(bool bVis = true);	

	bool GetVisible()	{ return m_Visible;}

public:

	// 特效接口	
	static CDymMagicToOTE* GetDymMagicInterface();	

	static Ogre::MovableObject* CreateMagicEntity(const char* szName, const char* szFileName);

	static void DeleteMagicEntity(const char* szName);

	static void DeleteAllMagics();

	static void SetAllMagicsVisible(bool isVisible);

public:

	// 特效模块接口对象
	CDymMagicToOTE_Obj*				m_pMagicObj;

protected:

	Ogre::String					m_Name;	
	Ogre::AxisAlignedBox			m_Box;

	bool							m_Visible;
	bool							m_AutoPlay;

};
// **************************************
// COTEBillBoardChainFactory
// **************************************
class COTEDymEffFactory : public CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat);

	virtual void Delete(const std::string& Name);

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "特效实体";
	}
};


}