#pragma once
#include "Ogre.h" 
#include <vector> 

#define POSITION_BINDING 0 
#define TEXCOORD_BINDING 1 

// *********************************************
//	CLineObj
// *********************************************

class CLineObj : public Ogre::SimpleRenderable 
{ 
public: 
	CLineObj(Ogre::String strName); 
   ~CLineObj(); 
	
   void clearPoints();
   void addPoint(const Ogre::Vector3 &p); 
   const Ogre::Vector3 &getPoint(unsigned short index) const; 
   unsigned short getNumPoints(void) const; 
   void updatePoint(unsigned short index, const Ogre::Vector3 &value); 
   void drawLine(Ogre::Vector3 &start, Ogre::Vector3 &end); 
   void drawLines(void); 
   void changeColor(Ogre::ColourValue color);

   Ogre::Real getSquaredViewDepth(const Ogre::Camera *cam) const; 
   Ogre::Real getBoundingRadius(void) const; 
	
public: 
   //void getWorldTransforms(Matrix4 *xform) const; 
   const Ogre::Quaternion &getWorldOrientation(void) const; 
   const Ogre::Vector3 &getWorldPosition(void) const; 
   //MaterialPtr m_MaterialPtr;
   Ogre::String m_StrName;
   std::vector<Ogre::Vector3> mPoints; 
   bool mDrawn; 
   Ogre::HardwareVertexBufferSharedPtr mBuf;

}; 
