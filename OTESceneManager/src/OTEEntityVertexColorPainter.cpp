/************************************************************
 ************************************************************

 模块名称： 实体顶点实现
 相关模块： 实体类 COTEEntity, 
            碰撞相关 COTECollisionManager, 
					 COTEEntityPhysicsMgr 
						
 描述    ： 顶点光的优点是效率高, 对模型每有额外的要求,
            缺点是精确度不够(顶点级别)
			注意: 为了顶点色能够跟实体邦定, mesh资源
			      不能共享。这就意味着场景里的每个实体对应一个
				  mesh资源！所以建议顶点色的实体可以合成到一起
				  进行渲染。

 *************************************************************
 *************************************************************/

#include <fstream>
#include "OTEEntityVertexColorPainter.h"
#include "OTEEntityPhysicsMgr.h"
#include "OTEMathLib.h"
#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"

// ----------------------------------------------
using namespace std;
using namespace Ogre;
using namespace OTE;

// ----------------------------------------------------- 
// 数学
// ----------------------------------------------------- 

static bool  CheckPointInTriangle(
				const Ogre::Vector3& point, 
				const Ogre::Vector3& a, const Ogre::Vector3& b, const Ogre::Vector3& c,
				float tolerance = 0.005f) 
{		
	Ogre::Real total_angles = 0.0f;	

	// make the 3 vectors

	Ogre::Vector3 v1(point.x - a.x, point.y - a.y, point.z - a.z);
	v1.normalise();
	Ogre::Vector3 v2(point.x - b.x, point.y - b.y, point.z - b.z);
	v2.normalise();
	Ogre::Vector3 v3(point.x - c.x, point.y - c.y, point.z - c.z);	
	v3.normalise();
	
	Ogre::Real Dot1 = v2.dotProduct(v1);
	Dot1 < -1.0f ?	 Dot1 = -1.0f : NULL;
	Dot1 >  1.0f ?   Dot1 =  1.0f : NULL;
	total_angles += acos(Dot1); 

	Ogre::Real Dot2 = v3.dotProduct(v2);
	Dot2 < -1.0f ? 	Dot2 = -1.0f : NULL;
	Dot2 >  1.0f ?  Dot2 =  1.0f : NULL;
	total_angles += acos(Dot2);

	Ogre::Real Dot3 = v1.dotProduct(v3);
	Dot3 < -1.0f ?	Dot3 = -1.0f : NULL;
	Dot3 >  1.0f ?	Dot3 =  1.0f : NULL;
	total_angles += acos(Dot3); 
			
	if (fabs(total_angles - 2.0f * Ogre::Math::PI) <= tolerance)			
		return true;

	return false;
}


// -----------------------------------------------------------------
static std::pair<bool, Ogre::Vector3>  IntersectTrangle(
				const Ogre::Ray& ray, const Ogre::Vector3& pt1, const Ogre::Vector3& pt2, const Ogre::Vector3& pt3)
{	
	std::pair<bool, Ogre::Real	 > tpair = ray.intersects(Ogre::Plane(pt1,pt2,pt3));
	std::pair<bool, Ogre::Vector3> ret;
	ret.first = tpair.first;

	if(ret.first)	
	{
		ret.second = ray.getPoint(tpair.second);
		ret.first  = CheckPointInTriangle(ret.second, pt1, pt2, pt3);
	}	
	return ret;
}

// ----------------------------------------------
// color operation

static inline unsigned char GetRVal( unsigned int color)
{
	return (unsigned char)(color>>16);
}
static inline unsigned char GetGVal( unsigned int color)
{
	return (unsigned char)(color>>8);
}
static inline unsigned char GetBVal( unsigned int color)
{
	return (unsigned char)(color);
}
static inline unsigned char GetAVal( unsigned int color)
{
	return (unsigned char)(color>>32);
}
static inline float fGetRVal( unsigned int color)
{
	return (float)(GetRVal(color)/255.0f);
}
static inline float fGetGVal( unsigned int color)
{
	return (float)(GetGVal(color)/255.0f);
}
static inline float fGetBVal( unsigned int color)
{
	return (float)(GetBVal(color)/255.0f);
}
static inline float fGetAVal( unsigned int color)
{
	return (float)(GetAVal(color)/255.0f);
}
static inline unsigned int GetColour( unsigned char R,unsigned char G,unsigned char B)
{
	return (R>>16 | G>>8 | B);
}
static inline Ogre::ColourValue GetColour( unsigned int color)
{
	return Ogre::ColourValue(fGetRVal(color),fGetGVal(color),fGetBVal(color),fGetAVal(color));
}

// --------------------------------------------------------

static void paintPoint(SubMesh *sm, size_t pointIndex, ColourValue diffColor, float alpha)
{	
	if (!sm || pointIndex == -1)	 
	{
		OTE_MSG("COTEEntityVertexColorPainter::paintPoint 失败! 可能是submesh 为空", "失败")
		return;
	}

    diffColor.a=1.0f;
	const VertexElement* diffElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
	if(!diffElem)
		return;
	HardwareVertexBufferSharedPtr vbuf=sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());
	unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	unsigned int* pDiff;
	diffElem->baseVertexPointerToElement(vertex+pointIndex*vbuf->getVertexSize(), &pDiff);
	
	// 替换

	Ogre::ColourValue desColor;

	if(Ogre::Math::Abs(alpha - 1.0f) < 0.0001f)
	{			
		desColor = diffColor; 
	}

	// 叠加

	else if(alpha > 1.0f)
	{		
		desColor.r = GetRVal(*pDiff) / 255.0f; 
		desColor.g = GetGVal(*pDiff) / 255.0f; 
		desColor.b = GetBVal(*pDiff) / 255.0f; 

		desColor = diffColor + desColor; // 叠加

		desColor.r > 1.0f ? desColor.r = 1.0f : NULL;
		desColor.g > 1.0f ? desColor.g = 1.0f : NULL;
		desColor.b > 1.0f ? desColor.b = 1.0f : NULL;
		desColor.a  = 1.0f;
	}
	
	// 混合

	else
	{	
		Ogre::ColourValue desColor;
		desColor.r = GetRVal(*pDiff) / 255.0f; 
		desColor.g = GetGVal(*pDiff) / 255.0f; 
		desColor.b = GetBVal(*pDiff) / 255.0f; 
		
		desColor = diffColor * alpha + desColor * (1.0f - alpha); // 混合	
	}

	*pDiff = desColor.getAsARGB();	

    /*int tRed   = (*pDiff & 0x00ff0000) >> 16;
	int tGreen = (*pDiff & 0x0000ff00) >> 8	;
	int tBlue  = (*pDiff & 0x000000ff)		;

	unsigned char nRed   = (unsigned char)(diffColor.r * 255.0f);
	unsigned char nGreen = (unsigned char)(diffColor.g * 255.0f);
	unsigned char nBlue  = (unsigned char)(diffColor.b * 255.0f);

	if(CoverMode==1)
	{
		tRed+=nRed,tGreen+=nGreen,tBlue+=nBlue;
	}
	if(CoverMode==-1)
	{
		tRed-=nRed,tGreen-=nGreen,tBlue-=nBlue;
	}

	tRed=tRed<255?tRed:255; 
	tRed=tRed>0?tRed:0;
	tGreen=tGreen<255?tGreen:255; 
	tGreen=tGreen>0?tGreen:0;
	tBlue=tBlue<255?tBlue:255; 
	tBlue=tBlue>0?tBlue:0;

	nRed   = (unsigned char)tRed;
	nGreen = (unsigned char)tGreen;	
	nBlue  = (unsigned char)tBlue;*/

    //*pDiff=0xff000000|nRed<<16|nGreen<<8|nBlue;  

	vbuf->unlock(); 	
}

// --------------------------------------------------------
// type = 1 叠加
static void paintTriangle(SubMesh *sm, size_t triangleIndex, ColourValue diffColor, int type)
{
	if(!sm || triangleIndex==-1) 
	{
		OTE_MSG("COTEEntityVertexColorPainter::paintTriangle 失败! 可能是submesh 为空", "失败")
		return;
	}

	diffColor.a = 1.0f;
	uint16 *p16 = static_cast<uint16*>(sm->indexData->indexBuffer->lock(
		sm->indexData->indexStart, sm->indexData->indexCount, HardwareBuffer::HBL_READ_ONLY));
	const VertexElement* diffElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
	HardwareVertexBufferSharedPtr vbuf= sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());
	unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	unsigned int* pDiff;				
	unsigned int v1=*(p16+triangleIndex*3) ;
	unsigned int v2=*(p16+triangleIndex*3+1) ;
	unsigned int v3=*(p16+triangleIndex*3+2) ;
	
	diffElem->baseVertexPointerToElement(vertex+v1*vbuf->getVertexSize(), &pDiff);
	if(type==1) 
	{	
        *pDiff = ColourValue(GetColour(*pDiff)+diffColor).getAsRGBA();
	}
	else
	{
		*pDiff = diffColor.getAsRGBA();
	}

	diffElem->baseVertexPointerToElement(vertex+v2*vbuf->getVertexSize(), &pDiff);
	if(type==1) 
	{
        *pDiff = ColourValue(GetColour(*pDiff) + diffColor).getAsRGBA();
	}
	else
	{
		*pDiff = diffColor.getAsRGBA();
	}

	diffElem->baseVertexPointerToElement(vertex+v3*vbuf->getVertexSize(), &pDiff);	
	if(type==1) 
	{
        *pDiff = ColourValue(GetColour(*pDiff)+diffColor).getAsRGBA();
	}
	else
	{
		*pDiff = diffColor.getAsRGBA();
	}

	vbuf->unlock();
	sm->indexData->indexBuffer->unlock();
}

//-----------------------------------------------------------------------
static void getIntersectionPoint(
						COTEEntity* e,
						const Vector3& point, 
						Real radious,
						vector<SubMesh*>& vIntersectSubMesh, 
						vector<size_t>& vIntersectPointIndices, 
						const Vector3& rayDir )
{
	const Vector3& position = e->getParentNode()->_getDerivedPosition();
	const Quaternion& orientation = e->getParentNode()->_getDerivedOrientation();
	const Vector3& scale = e->getParentNode()->_getDerivedScale();
	Real nearstIntersectDistance = MAX_FLOAT;

	//not use share vertices
	Mesh::SubMeshIterator smIt= e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{	           
			const VertexElement* posElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
			const VertexElement* normElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);//temp!
			
			HardwareVertexBufferSharedPtr vbuf=sm->vertexData->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
			float* pFloat; 
			float* pFloatNorm;
			for(size_t j = 0; j <sm->vertexData->vertexCount; ++j, vertex +=vbuf->getVertexSize())
			{
				Vector3 pt;
				posElem->baseVertexPointerToElement(vertex, &pFloat);
				pt.x = *pFloat++;
				pt.y = *pFloat++;
				pt.z = *pFloat++;	       	
				// Transform to world(scale, rotate, translate)		
				pt = (orientation * (pt* scale)) + position;	
                if((point-pt).length()<radious)
				{	
					if(rayDir!=Vector3::ZERO)//consider the ray's direction
					{
	   					Vector3 nt;
						normElem->baseVertexPointerToElement(vertex, &pFloatNorm);
						nt.x = *pFloatNorm++;
						nt.y = *pFloatNorm++;
						nt.z = *pFloatNorm++; 

						nt = orientation * nt;

						if(nt.dotProduct(rayDir)<0)//facing the ray origin?
						{							
							vIntersectSubMesh.push_back(sm);
							vIntersectPointIndices.push_back(j);	
						}
					}        	
				} 
			}
            vbuf->unlock();		
		}      		 
	}
}


//-----------------------------------------------------------------------
static void setVerDiffOnPoint(COTEEntity* e, const Vector3& point,const ColourValue& diffColor,Real radious,const Vector3& dir)
{
	vector<size_t> vIntersectPointIndices;
	vector<SubMesh*>  vIntersectSubMesh;
    getIntersectionPoint(e, point,radious,vIntersectSubMesh,vIntersectPointIndices,dir);
	for(unsigned int i=0;i<vIntersectSubMesh.size();i++)
	{        
		paintPoint(vIntersectSubMesh.at(i),vIntersectPointIndices.at(i),diffColor,1.0f);
	}
}

// --------------------------------------------------------
static IntersectInfo_t setVerDiffColor(COTEEntity* e, const Ray& ray, const ColourValue& diffColor, Real radious)
{
	IntersectInfo_t ip = COTEEntityPhysicsMgr::GetTriangleIntersection(e, ray, true);
	setVerDiffOnPoint(e, ip.hitPos, diffColor, radious, ray.getDirection());

	return ip;
}

// --------------------------------------------------------
static Vector3 getVertexPositionAt(SubMesh *sm, size_t vertexIndex)
{
	if (!sm || vertexIndex<0 || vertexIndex>sm->vertexData->vertexCount)	   throw;
	Vector3 pt;
	const VertexElement* posElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
	HardwareVertexBufferSharedPtr vbuf=sm->vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
	unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
	float* pPos;
	posElem->baseVertexPointerToElement(vertex+vertexIndex*vbuf->getVertexSize(), &pPos);			
	pt.x = *pPos++;
	pt.y = *pPos++;
	pt.z = *pPos++;		
	vbuf->unlock(); 
	return pt;
}

// --------------------------------------------------------
static Vector3 getVertexNormalAt(COTEEntity* e, SubMesh *sm, size_t vertexIndex)
{
	Vector3 n;
	if (!sm || vertexIndex<0 || vertexIndex>sm->vertexData->vertexCount)	   throw;
	Quaternion orientation = e->getParentNode()->_getDerivedOrientation();
	
	const VertexElement* normElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
	HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(normElem->getSource());
	unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
	float* pNorm;
	normElem->baseVertexPointerToElement(vertex+vertexIndex * vbuf->getVertexSize(), &pNorm);			
	n.x = *pNorm++;
	n.y = *pNorm++;
	n.z = *pNorm++;	
	n = orientation * n;
	vbuf->unlock(); 
	return n;
}


// --------------------------------------------------------
static void rayPaint(	COTEEntity* e, SubMesh *sm, size_t vertexIndex,
						Ray ray,
						const ColourValue& diffuse, const ColourValue&  ambient,
						float alpha = 1.0f,
						bool  bRenderShadow		 = true,
						COTEEntityVertexColorPainter::GetLightInforHndler_t	 pLightInforHndler = NULL
						)
{
	if(pLightInforHndler)	// 比如点光源	
	{
		Ogre::Vector3		_position;
		Ogre::Vector3		_direction = ray.getDirection();
		Ogre::ColourValue	_diffuse   = diffuse;
		float				reflectDis;
		float				reflectDis0;

		(*pLightInforHndler)(ray.getOrigin(), _position, _direction, _diffuse, reflectDis, reflectDis0, alpha);
		
		ray.setDirection(-_direction);

		Ogre::Vector3 hitPos;
		int hits = 0;
		
		if( bRenderShadow && (
			(COTETilePage::s_CurrentPage ? COTECollisionManager::IntersectMap(
			hitPos, COTETilePage::s_CurrentPage, ray.getOrigin(), 
			ray, 0.25f, hits, reflectDis) : false) ||
			//COTEEntityPhysicsMgr::GetTriangleIntersection(e, ray, 0).isHit
			COTECollisionManager::TriangleIntersection(ray, hitPos, MAX_FLOAT, E_DEFAULT)
			)
			)
		{
			//in the shadow	

			ColourValue tcor = ambient;

			tcor.r > 1.0f ? tcor.r = 1.0f : NULL;
			tcor.g > 1.0f ? tcor.g = 1.0f : NULL;
			tcor.b > 1.0f ? tcor.b = 1.0f : NULL;

			paintPoint(sm, vertexIndex, tcor, alpha);
		}
		else
		{
			// in the light

			Vector3 nt		= - getVertexNormalAt(e, sm, vertexIndex);
			float factor	= - ray.getDirection().dotProduct( nt );
			
			factor < 0.0f ? factor = 0.0f : NULL;
			factor > 1.0f ? factor = 1.0f : NULL;

			ColourValue tcor = ambient + _diffuse * factor;

			tcor.r > 1.0f ? tcor.r = 1.0f : NULL;
			tcor.g > 1.0f ? tcor.g = 1.0f : NULL;
			tcor.b > 1.0f ? tcor.b = 1.0f : NULL;

			paintPoint(sm, vertexIndex, tcor, alpha);	
		}

	}
	else
	{
		Ogre::Vector3 hitPos;
		int hits = 0;

		if( bRenderShadow && 
			((COTETilePage::s_CurrentPage ? COTECollisionManager::IntersectMap(hitPos, COTETilePage::s_CurrentPage, ray.getOrigin(), ray, 0.25f, hits) : false) ||
			//COTEEntityPhysicsMgr::GetTriangleIntersection(e, ray, 0).isHit
			 COTECollisionManager::TriangleIntersection(ray, hitPos, MAX_FLOAT, E_DEFAULT)
			)
			)
		{
			//in the shadow	

			ColourValue tcor = ambient;

			tcor.r > 1.0f ? tcor.r = 1.0f : NULL;
			tcor.g > 1.0f ? tcor.g = 1.0f : NULL;
			tcor.b > 1.0f ? tcor.b = 1.0f : NULL;

			paintPoint(sm, vertexIndex, tcor, alpha);
		}
		else
		{
			// in the light

			Vector3 nt		= - getVertexNormalAt(e, sm, vertexIndex);
			float factor	= - ray.getDirection().dotProduct( nt );
			
			factor < 0.0f ? factor = 0.0f : NULL;
			factor > 1.0f ? factor = 1.0f : NULL;

			ColourValue tcor = ambient + diffuse * factor;

			tcor.r > 1.0f ? tcor.r = 1.0f : NULL;
			tcor.g > 1.0f ? tcor.g = 1.0f : NULL;
			tcor.b > 1.0f ? tcor.b = 1.0f : NULL;

			paintPoint(sm, vertexIndex, tcor, alpha);	
		}
	}
	
}

// --------------------------------------------------------
static unsigned int getVertexDiffuseAt(SubMesh *sm,size_t vertexIndex)
{
	if (!sm || vertexIndex<0 || vertexIndex>sm->vertexData->vertexCount)	   throw;
	unsigned int diff;
	const VertexElement* diffElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
	HardwareVertexBufferSharedPtr vbuf=sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());
	unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
	unsigned int* pWORD;
	diffElem->baseVertexPointerToElement(vertex+vertexIndex*vbuf->getVertexSize(), &pWORD);			
	diff= *pWORD;

	vbuf->unlock(); 
	return diff;
}

// ==============================================
// COTEEntityVertexColorPainter
// ==============================================

void COTEEntityVertexColorPainter::RayPaint(COTEEntity* e, 
										   const Vector3& point,Real radious,
										   const Ray& ray, 
										   const ColourValue& diffuse, const ColourValue&  ambient,
										   float blendAlpha,
										   bool  bRenderShadow,
										   COTEEntityVertexColorPainter::GetLightInforHndler_t	 pLightInforHndler
										   )
{
	if(!e)
		return;

	vector<size_t>		vIntersectPointIndices;
	vector<SubMesh*>	vIntersectSubMesh;
	
    getIntersectionPoint(e, point,radious,vIntersectSubMesh,vIntersectPointIndices, ray.getDirection());
	for(unsigned int i=0;i<vIntersectSubMesh.size();i++)
	{        
		rayPaint(e, vIntersectSubMesh.at(i),vIntersectPointIndices.at(i),ray, diffuse, ambient, blendAlpha);
	}
}


// --------------------------------------------------------
void COTEEntityVertexColorPainter::SetVerDiffOnTriangle(COTEEntity* e, Ray ray, ColourValue diffColor)
{	
	if(!e)
		return;

	//===================get collision triangle=================================//
	std::pair<bool, Ogre::Vector3> intersectPoint;		
	intersectPoint.first=false;//not hit by default	
	Real  nearstIntersectDistance = MAX_FLOAT;
	Vector3 position= e->getParentNode()->_getDerivedPosition();
	Quaternion orientation = e->getParentNode()->_getDerivedOrientation();
	Vector3 scale = e->getParentNode()->_getDerivedScale();  
	size_t intersectTrangleIndex=-1;
	SubMesh *  intersectSubMesh=0;
	Vector3 ipt1,ipt2,ipt3;
	//not use share vertices	
	Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{
			//===================Triangle collision=============================//
			bool use16bitIndexes = 	sm->indexData->indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT;					
	
			if (use16bitIndexes)
			{					
				unsigned int v1, v2, v3;		
				uint16 *p16 = static_cast<uint16*>(sm->indexData->indexBuffer->lock(
					sm->indexData->indexStart, sm->indexData->indexCount, HardwareBuffer::HBL_READ_ONLY));		        
				const VertexElement* posElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
				HardwareVertexBufferSharedPtr vbuf= 
					sm->vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
				unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
				float* pFloat;	
				for(size_t j = 0; j <sm->indexData->indexCount/3; j++)
				{						
					v1=*p16++ ;
					v2=*p16++ ;
					v3=*p16++ ;

					Vector3 pt1,pt2,pt3;
					posElem->baseVertexPointerToElement(vertex+v1*vbuf->getVertexSize(), &pFloat);
					pt1.x = (*pFloat++);
					pt1.y = (*pFloat++);
					pt1.z = (*pFloat++);
					posElem->baseVertexPointerToElement(vertex+v2*vbuf->getVertexSize(), &pFloat);
					pt2.x = (*pFloat++);
					pt2.y = (*pFloat++);
					pt2.z = (*pFloat++);
					posElem->baseVertexPointerToElement(vertex+v3*vbuf->getVertexSize(), &pFloat);
					pt3.x = (*pFloat++);
					pt3.y = (*pFloat++);
					pt3.z = (*pFloat++);	

					pt1 = (orientation * (pt1* scale)) + position;                         			
					pt2 = (orientation * (pt2* scale)) + position;                         			
					pt3 = (orientation * (pt3* scale)) + position; 

					std::pair<bool, Ogre::Vector3> tResult= IntersectTrangle(ray,pt1,pt2,pt3);
					if(tResult.first)
					{	
						Real dis=(tResult.second-ray.getOrigin()).length();
						if(dis<nearstIntersectDistance)
						{
							nearstIntersectDistance=dis;
							intersectPoint=tResult;
							intersectSubMesh=sm;
                            intersectTrangleIndex=j ;
							ipt1=pt1;
							ipt2=pt2;
							ipt3=pt3;
						}							   
					}				
				}					
				sm->indexData->indexBuffer->unlock();
				vbuf->unlock();
			}	
			else
			{
				OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, 
				"Intersection function just support 16 bit index "
				"",
				"COTEEntityVertexColorPainter::getTriangleIntersection");
			}
		} 
		else
		{	
				OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, 
				"Intersection function do not support shared vertices"
				"",
				"COTEEntityVertexColorPainter::getTriangleIntersection");			
		}
	}	  
    paintTriangle(intersectSubMesh,intersectTrangleIndex,diffColor,1);
}


// --------------------------------------------------------
void COTEEntityVertexColorPainter::SetVertexColor(COTEEntity* e, const ColourValue& diffColor)
{	
	if(!e || e->getMesh().isNull())
		return;

	//not use share vertices	
	Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{	
			bool use16bitIndexes = 	sm->indexData->indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT;					
	
			if (use16bitIndexes)
			{		
		
				uint16 *p16 = static_cast<uint16*>(sm->indexData->indexBuffer->lock(
					sm->indexData->indexStart, sm->indexData->indexCount, HardwareBuffer::HBL_READ_ONLY));		        
				const VertexElement* posElem=sm->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
				HardwareVertexBufferSharedPtr vbuf= 
					sm->vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
				unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));	
				for(size_t j = 0; j <sm->indexData->indexCount/3; j++)
				{	
					paintTriangle(sm, j, diffColor, 0);				
				}					
				sm->indexData->indexBuffer->unlock();
				vbuf->unlock();
			}	
			else
			{
				OTE_MSG("必须使用16位索引", "失败")
				throw;
			}
		} 
		else
		{	
			OTE_MSG("不支持共享顶点", "失败")			
			throw;		
		}
	}	  
   
}

// --------------------------------------------------------
void COTEEntityVertexColorPainter::SetVertexColor(const ColourValue& diffColor)
{
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
	while(it != eList.end())
	{			
		OTE::COTEEntity* e = (OTE::COTEEntity*)(*it);		
		
		//WAIT_LOADING_RES(e)

		SetVertexColor(e, diffColor);

		++ it;
	}
}

// --------------------------------------------------------
ColourValue COTEEntityVertexColorPainter::GetVertexDiffColor(COTEEntity* e, const Ray& ray, Real radious)
{
	if(!e)
		return ColourValue();

	IntersectInfo_t ip = COTEEntityPhysicsMgr::GetTriangleIntersection(e, ray, true);
	if(!ip.isHit) 
		return ColourValue(-1,-1,-1);

	vector<size_t> vIntersectPointIndices;
	vector<SubMesh*>  vIntersectSubMesh;	
	getIntersectionPoint(e, ip.hitPos, radious, vIntersectSubMesh, vIntersectPointIndices, ray.getDirection());

	if(!vIntersectSubMesh.empty())
	{
		unsigned int cor = getVertexDiffuseAt(vIntersectSubMesh.at(0), vIntersectPointIndices.at(0));
		return GetColour(cor);
	}

    return ColourValue(-1,-1,-1);
}

// parallel light
// --------------------------------------------------------
void COTEEntityVertexColorPainter::GenLightDiffmap(
									COTEEntity* e, const Vector3& lightdir, const ColourValue& diffuse, const ColourValue& ambient, 
									float blendAlpha,
									bool  bRenderShadow,
									GetLightInforHndler_t	 pLightInforHndler
									)
{
	if(!e || e->getMesh().isNull() || CHECK_LM_VALID(e))
		return;

	const Vector3& position= e->getParentNode()->_getDerivedPosition();
	const Quaternion& orientation = e->getParentNode()->_getDerivedOrientation();
	const Vector3& scale = e->getParentNode()->_getDerivedScale(); 

	Mesh::SubMeshIterator smIt= e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{	 
			for(size_t j = 0; j < sm->vertexData->vertexCount; j++)
			{						
				// get the position 
				Vector3 pt=getVertexPositionAt(sm,j);
				pt = (orientation * (pt* scale)) + position;            
       
                //check collision
			    rayPaint(e, sm, j, Ray(pt - lightdir * 0.02f/*小偏移避免误判*/, - lightdir),
					diffuse, ambient, blendAlpha, 
					bRenderShadow, 
					pLightInforHndler);
			}  			
		}      		 
	}

	//SaveVertexColorData(e);
}

// --------------------------------------------------------
void COTEEntityVertexColorPainter::GenLightDiffmap(						
						const Ogre::Vector3& lightdir, 
						const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 						
						float blendAlpha,
						const Ogre::String&		 hideEntityKeyName,						
						bool  bRenderShadow,
						GetLightInforHndler_t	 pLightInforHndler
						)
{
	// 物件列表中所有的物件 查询符合条件的计算

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
	while(it != eList.end())
	{			
		OTE::COTEEntity* e = (OTE::COTEEntity*)(*it);
		if( !hideEntityKeyName.empty() && 
			e->getName().find(hideEntityKeyName) != std::string::npos)
		{
			++ it;
			continue;
		}	
		
		//WAIT_LOADING_RES(e)

		GenLightDiffmap(e, lightdir, diffuse, ambient, blendAlpha, bRenderShadow, pLightInforHndler);

		//SaveVertexColorData(e);
	
		++ it;
	}

}

// --------------------------------------------------------
void COTEEntityVertexColorPainter::GetVertexColorData(COTEEntity* e, DWORD** pData)
{	
	if(!e)
		return;

	// 估计数据的尺寸

	int totalVertsNum = 0;
	
	Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{					
			const VertexElement* diffElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
			if(!diffElem)
				return;

			HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());
			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));

			totalVertsNum += vbuf->getNumVertices();								
		} 		
	}	

	*pData = new DWORD[totalVertsNum];

	// 填充数据

	smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{					
			const VertexElement* diffElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
			if(!diffElem)
				return;

			HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());
			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
			DWORD* pDiff;			

			int numOfVerts = vbuf->getNumVertices();	

			for(int i = 0; i < numOfVerts; ++ i)
			{
				diffElem->baseVertexPointerToElement(vertex + i * vbuf->getVertexSize(), (void**)&pDiff);	
				(*pData)[i] = *pDiff;				
			}

			(*pData) += numOfVerts;

			vbuf->unlock();							
		} 		
	}	

}

// --------------------------------------------------------
void COTEEntityVertexColorPainter::SaveVertexColorData()
{
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
	while(it != eList.end())
	{			
		OTE::COTEEntity* e = (OTE::COTEEntity*)(*it);	
		SaveVertexColorData(e);
	
		++ it;
	}
}

// --------------------------------------------------------
void COTEEntityVertexColorPainter::SaveVertexColorData(COTEEntity* e)
{
	if(!e)
		return;

	const static String c_vcr_dir_helper = "entvcrdirhelper.ote"; // 这个文件便于查找物件光照图的目录

	Ogre::String vcrFilePath, fullPath;
	OTEHelper::GetFullPath(c_vcr_dir_helper, vcrFilePath, fullPath);
	vcrFilePath += "\\";

	//std::string vcrFilePath = OTEHelper::GetOTESetting("ResConfig", "VcrFilePath");
	SaveVertexColorData(e, (vcrFilePath + e->getName() + ".vcr").c_str());
}

// --------------------------------------------------------
// 保存顶点色数据
void COTEEntityVertexColorPainter::SaveVertexColorData(COTEEntity* e, const Ogre::String& fileName)
{	
	if(!e)
		return;

	ofstream fs(fileName.c_str());
	if(fs.fail())
		return;

	Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{					
			const VertexElement* diffElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
			if(!diffElem)
				return;

			HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());
			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
			DWORD* pDiff;

			// 数据交换

			int numOfVerts = vbuf->getNumVertices();

			DWORD* pBuff = new DWORD[numOfVerts];
			for(int i = 0; i < numOfVerts; ++ i)
			{
				diffElem->baseVertexPointerToElement(vertex + i * vbuf->getVertexSize(), (void**)&pDiff);	
				pBuff[i] = *pDiff;				
			}

			vbuf->unlock();	

			fs.write((const char*)pBuff, numOfVerts * sizeof(DWORD));	
				
			delete []pBuff;
					
		} 		
	}	

	fs.close();
}
