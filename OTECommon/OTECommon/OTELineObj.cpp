#include "otelineobj.h"

using namespace std; 
using namespace Ogre; 

// ------------------------------------------------
CLineObj::CLineObj(String strName) 
{ 
	mRenderOp.vertexData = new VertexData(); 
    mDrawn = false;
	
	m_StrName = strName;	

	createMaterial(m_StrName);
}

CLineObj::~CLineObj() 
{ 	
   // MaterialManager::getSingleton().remove(m_StrName);
   mBuf.setNull();
   delete mRenderOp.vertexData;
} 
// ------------------------------------------------
void CLineObj::changeColor(ColourValue color)
{
	if( !m_pMaterial.isNull() )
	{		
		m_pMaterial->setAmbient(ColourValue::Black);
		m_pMaterial->setDiffuse(ColourValue::Black);
		m_pMaterial->setSpecular(ColourValue::Black);

		m_pMaterial->setSelfIllumination(color);	
	}
}
// ------------------------------------------------
void CLineObj::clearPoints()
{
	mPoints.clear();
}
// ------------------------------------------------
void CLineObj::addPoint(const Vector3 &p) 
{ 
   mPoints.push_back(p); 
} 
// ------------------------------------------------ 
const Vector3 &CLineObj::getPoint(unsigned short index) const 
{ 
   assert(index < mPoints.size() && "Point index is out of bounds!!"); 

   return mPoints[index]; 
} 
// ------------------------------------------------
unsigned short CLineObj::getNumPoints(void) const 
{ 
   return (unsigned short)mPoints.size(); 
} 
// ------------------------------------------------
void CLineObj::updatePoint(unsigned short index, const Vector3 &value) 
{ 
   assert(index < mPoints.size() && "Point index is out of bounds!!"); 

   mPoints[index] = value; 
} 
// ------------------------------------------------
void CLineObj::drawLine(Vector3 &start, Vector3 &end) 
{ 
   if(mPoints.size()) 
      mPoints.clear(); 

   mPoints.push_back(start); 
   mPoints.push_back(end); 

   drawLines(); 
} 
// ------------------------------------------------
void CLineObj::drawLines(void) 
{ 
   if(!mDrawn)  
   {
		mDrawn = true; 

		// Initialization stuff 

		mRenderOp.indexData = 0; 
		mRenderOp.vertexData->vertexCount = mPoints.size(); 
		mRenderOp.vertexData->vertexStart = 0; 
		mRenderOp.operationType = RenderOperation::OT_LINE_STRIP; // OT_LINE_LIST, OT_LINE_STRIP 
		mRenderOp.useIndexes = false; 

		VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration; 
		VertexBufferBinding *bind = mRenderOp.vertexData->vertexBufferBinding; 

		decl->addElement(POSITION_BINDING, 0, VET_FLOAT3, VES_POSITION); 

		mBuf = 
			HardwareBufferManager::getSingleton().createVertexBuffer( 
				decl->getVertexSize(POSITION_BINDING), 
				mRenderOp.vertexData->vertexCount, 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY); 

		bind->setBinding(POSITION_BINDING, mBuf); 
   }

   // Drawing stuff 
   int size = mPoints.size(); 
   Vector3 vaabMin = mPoints[0]; 
   Vector3 vaabMax = mPoints[0]; 

   Real *prPos = static_cast<Real*>(mBuf->lock(HardwareBuffer::HBL_DISCARD)); 

   for(int i = 0; i < size; i++) 
   { 
      *prPos++ = mPoints[i].x; 
      *prPos++ = mPoints[i].y; 
      *prPos++ = mPoints[i].z; 

      if(mPoints[i].x < vaabMin.x) 
         vaabMin.x = mPoints[i].x; 
      if(mPoints[i].y < vaabMin.y) 
         vaabMin.y = mPoints[i].y; 
      if(mPoints[i].z < vaabMin.z) 
         vaabMin.z = mPoints[i].z; 

      if(mPoints[i].x > vaabMax.x) 
         vaabMax.x = mPoints[i].x; 
      if(mPoints[i].y > vaabMax.y) 
         vaabMax.y = mPoints[i].y; 
      if(mPoints[i].z > vaabMax.z) 
         vaabMax.z = mPoints[i].z; 
   } 

   mBuf->unlock(); 

   mBox.setExtents(vaabMin, vaabMax); 
} 
// ------------------------------------------------
Real CLineObj::getSquaredViewDepth(const Camera *cam) const 
{ 
   Vector3 vMin, vMax, vMid, vDist; 
   vMin = mBox.getMinimum(); 
   vMax = mBox.getMaximum(); 
   vMid = ((vMin - vMax) * 0.5) + vMin; 
   vDist = cam->getDerivedPosition() - vMid; 

   return vDist.squaredLength(); 
} 
// ------------------------------------------------
Real CLineObj::getBoundingRadius(void) const 
{ 
   return Math::Sqrt(max(mBox.getMaximum().squaredLength(), mBox.getMinimum().squaredLength())); 
   //return mRadius; 
} 
// ------------------------------------------------ 
const Quaternion &CLineObj::getWorldOrientation(void) const 
{ 
   return Quaternion::IDENTITY; 
} 
// ------------------------------------------------
const Vector3 &CLineObj::getWorldPosition(void) const 
{ 
   return Vector3::ZERO; 
} 


