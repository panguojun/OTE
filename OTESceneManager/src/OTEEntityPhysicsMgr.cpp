/************************************************************
 ************************************************************

 ģ�����ƣ� ʵ����ײ��
 ���ģ�飺 ʵ���� COTEEntity, ����"�Ż�ʵ����": COTEAutoEntity         			  
						
 ����    �� �����ײ������  
            ��ײ������ͨ��������ײ��alpha�οգ���Ҫ�ṩģ��tga��ͼ����
			Ϊ�˼���lock���� ����ʹ�����������ڴ�cach���� 

 *************************************************************
 *************************************************************/

#include "OTEEntityPhysicsMgr.h"
#include "OTEMathLib.h"
#include "OgreD3D9Texture.h"
#include "OTEEntityRender.h"

// -----------------------------------------------------
#define AUTO_UPDATE_BOUNDINGS					// �Զ�����bounding
#define CACH_SIZE					1024		// �ڴ�cach�ߴ�

// ----------------------------------------------------- 
using namespace Ogre;
namespace OTE
{

// ----------------------------------------------------- 
// ��ѧ
// ----------------------------------------------------- 

/// ��������ײ 

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

// -----------------------------------------------------------------
// ��ֵ

static void Lerp(
				   const Ogre::Vector3& a,  const Ogre::Vector3& b,  const Ogre::Vector3& c, const Ogre::Vector3& p,
				   const Ogre::Vector2& va, const Ogre::Vector2& vb, const Ogre::Vector2& vc,	
				   Ogre::Vector2& vp)

{
	Ogre::Vector3 DABC = (b - a).crossProduct(c - a);
	if(DABC.length() < 0.000001f)
	{
		vp = va;
		return;
	}

	Ogre::Vector3 DPBC = (b - p).crossProduct(c - p);
	Ogre::Vector3 DAPC = (c - p).crossProduct(a - p);
	Ogre::Vector3 DABP = (a - p).crossProduct(b - p);	

	if(DPBC.dotProduct(DABC) < 0.0f)
	{
		DPBC = - DPBC;		
	}
	if(DAPC.dotProduct(DABC) < 0.0f)
	{
		DAPC = - DAPC;		
	}
	if(DABP.dotProduct(DABC) < 0.0f)
	{
		DABP = - DABP;		
	}

	vp = (DPBC.length() * va + DAPC.length() * vb + DABP.length() * vc) / (DPBC + DAPC + DABP).length();
}

// =========================================================
// ������ͼ��Ϣ
// =========================================================

struct TextureCach_t
{
	TextureCach_t(int width, int height) : 
	Width(width),Height(height)
	{
		pData = new unsigned int[height * width];
	}

	~TextureCach_t()
	{
		SAFE_DELETE_ARRAY(pData)
	}

	// Get/Set Color

	unsigned int GetColor(int xOffset, int yOffset)
	{
		assert(yOffset * Width + xOffset < Height * Width);
		return pData[yOffset * Width + xOffset];
	}

	void SetColor(int xOffset, int yOffset, unsigned int color)
	{
		assert(yOffset * Width + xOffset < Height * Width);
		pData[yOffset * Width + xOffset] = color;
	}

	// ����

	int Width, Height;

	unsigned int* pData;
};

static HashMap<int, TextureCach_t*>		s_TextureCach; // <texturePtr, textureCach>

// --------------------------------------------------------
// cach��ͼ

static inline bool addTextureToCach(Ogre::D3D9Texture* tex, const D3DLOCKED_RECT* rect = NULL, bool isOutLocked = false)
{
	// �����Ƿ��Ѿ����

	if(s_TextureCach.find(int(tex)) != s_TextureCach.end())
		return true; // �Ѿ����Ҳ��ɹ�

	if(!isOutLocked)
	{	
		HRESULT hr;
		static D3DLOCKED_RECT _rect;

		hr = tex->getNormTexture()->LockRect( 0, &_rect, NULL, 0 ); 

		if (FAILED(hr)) 
		{
			OTE_MSG_ERR("paintTexture �� tex->LockRect ʧ��!")
			return false;
		} 	

		rect = &_rect;
	}	 
	
	// �½�cach

	DWORD* dst = (DWORD*)rect->pBits; 	

	TextureCach_t* pTexCach = NULL;

	// һ���ѹ�� argb��ʽ��ͼƬ
	
	D3DSURFACE_DESC desc;		
		
	if (FAILED(tex->getNormTexture()->GetLevelDesc(0, &desc)))
	{
		OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Can't get texture description", "addTextureToCach" );		
	}

	if(desc.Format == D3DFMT_A8R8G8B8)
	{
		int texWidth = tex->getWidth();
		int texHeight = tex->getHeight();

		pTexCach = new TextureCach_t(texWidth, texHeight);

		for (int i = 0; i < texHeight; ++ i) 
		{ 
			for (int j = 0; j < texWidth; ++ j) 
			{ 		
				pTexCach->SetColor(j, i, dst[j]);			
			} 

			dst += rect->Pitch / sizeof(DWORD);
		}
	}	
	else
	{
		// ������ʽ�ݲ�֧��
		
	}

	// ��ӵ��б�

	if(pTexCach)
		s_TextureCach[int(tex)] = pTexCach;
	
	// unlock

	if(!isOutLocked)
		tex->getNormTexture()->UnlockRect(0); 

	return true;
}

// -------------------------------------------------	
// ���ͼƬ��ɫֵ

static unsigned int getTextureColorAt(const Ogre::Vector2& uv, Ogre::D3D9Texture* tex, 
									   int xOffset, int yOffset,
									   D3DLOCKED_RECT* rect = NULL, bool isOutLocked = false)
{
	if(s_TextureCach.find(int(tex)) != s_TextureCach.end())
	{
		TextureCach_t* tc = s_TextureCach[int(tex)];
		return tc->GetColor(xOffset, yOffset);
	}

	if(!isOutLocked)
	{	
		HRESULT hr;
		static D3DLOCKED_RECT _rect;

		hr = tex->getNormTexture()->LockRect( 0, &_rect, NULL, D3DLOCK_READONLY ); 

		if (FAILED(hr)) 
		{
			OTE_MSG_ERR("GetColorAt �� tex->LockRect ʧ��!")
			return 0xFF000000;
		} 	

		rect = &_rect;
	}	 
		
	// ��ѯͼƬ��ɫ	

	DWORD paintColor;

	DWORD* dst = (DWORD*)rect->pBits; 

	dst += yOffset * rect->Pitch / sizeof(DWORD);	

	dst += xOffset;
	
	paintColor = *dst; 	
	
	// unlock

	if(!isOutLocked)
		tex->getNormTexture()->UnlockRect(0); 

	return paintColor;
}


// --------------------------------------------------
// ����alpha

bool CheckAlpha(const Ogre::Vector2& hitUV, const Ogre::String& matName)
{
	Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(matName);

	bool result = true; // Ĭ��������ǲ�͸����

	// �ж�alpha�ο�
	// �������˫����ͼ �ͱ��ų�����

	if(mp->getBestTechnique(0)->getPass(0)->getCullingMode() != CULL_NONE)
		return result;

	
	{// ������ͨ���ж���ͼ�ϵ�alphaֵ����ȡ�ο���Ϣ

		// �õ���ͼ

		Ogre::String texName = mp->getBestTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
		
		// �����dds��β�� �����滻��tga	

		if(texName.find(".dds") != std::string::npos)
		{
			texName.replace(texName.find(".dds"), 4, ".tga");  
		}		 

		Ogre::TexturePtr texPtr = TextureManager::getSingleton().getByName(texName);
		if(texPtr.isNull())
		{
			if(!Ogre::ResourceGroupManager::getSingleton().resourceExists(
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				texName)
				)
			{
				//OTE_MSG("GetTriangleIntersection ʱ ��ͼû�ҵ�û���ҵ�! " << texName, "ʧ��")
				return result;   
			}

			// ������ͼ

			texPtr = TextureManager::getSingleton().load(
						texName,
						ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						TEX_TYPE_2D, 0);    
		}

		Ogre::D3D9Texture* tex = (Ogre::D3D9Texture*)texPtr.getPointer();

		uint xOffset = hitUV.x * tex->getWidth() ;
		uint yOffset = hitUV.y * tex->getHeight();

		// ��ӵ����� �����Ч��

		if(!addTextureToCach(tex))
			return result;

		DWORD color = getTextureColorAt(hitUV, tex,
										xOffset % tex->getWidth(), yOffset % tex->getHeight());
		
		result = ((color & 0xFF000000) >> 24) >= mp->getBestTechnique()->getPass(0)->getAlphaRejectValue();
	}

	return result;
}

// ==================================================
// SubMeshCach_t
// ==================================================

std::list<SubMeshCach_t*>		 gSubMeshCachList;

// -------------------------------------------------
// ��ӵ�cach

SubMeshCach_t*  SubMeshCach_t::AddToCach(int id, int sz)
{
#ifndef GAME_DEBUG // ����Ϸ�ͻ��˿���ס�صĳߴ�

	if((int)gSubMeshCachList.size() > CACH_SIZE)
	{
		//::OutputDebugString("gSubMeshCachList.pop_back\n");
		delete (gSubMeshCachList.back());
		gSubMeshCachList.pop_back();
	}

#endif

	// ��ӵ��б�

	SubMeshCach_t* smt = new SubMeshCach_t(sz, id);

	//std::stringstream ss;
	//ss << "gSubMeshCachList.push_front(" << sz << " " << id << "\n";
	//::OutputDebugString(ss.str().c_str());

	gSubMeshCachList.push_front(smt);
	
	return smt;
}

// -------------------------------------------------
// ��cach����

SubMeshCach_t*  SubMeshCach_t::GetFromCach(unsigned int id)
{
	std::list<SubMeshCach_t*>::iterator it = gSubMeshCachList.begin();
	while(it != gSubMeshCachList.end())
	{
		if((*it)->id == id)	
			return (*it);	
		++ it;
	}	
	return NULL;
}

// -------------------------------------------------
// ��ײ��

bool  SubMeshCach_t::GetColliPos(	
								const Ogre::Ray& ray, SubMeshCach_t* smt, 
								const Ogre::Vector3& position, 
								const Ogre::Quaternion& orientation,
								const Ogre::Vector3& scale, 
								unsigned int flag,
								IntersectResult_t& rResult
								)
{	
	if(smt->box.getMinimum().x < smt->box.getMaximum().x)
	{
		// ����ʵ��box ��λ�� 
		
		Ogre::AxisAlignedBox box = smt->box;

		// ��ת����£���

		const Ogre::Vector3 *corners = smt->box.getAllCorners();	
		for(int i = 0; i < 8; i ++)
		{
			box.merge(orientation * corners[i]);			
		}	

		box.setExtents(
			box.getMinimum()* scale + position,
			box.getMaximum()* scale + position
			);

		if(!ray.intersects(box).first)
			return false;
	}

	Ogre::Real  nearstIntersectSquareDis	=	MAX_FLOAT;	

	Ogre::Vector3 pts[3];	

	for(int i = 0; i < smt->size; ++ i)
	{			
		Ogre::Vector3 pt1 = (orientation * (smt->tringle[i].TriVec[0]* scale)) + position;                         			
		Ogre::Vector3 pt2 = (orientation * (smt->tringle[i].TriVec[1]* scale)) + position;                         			
		Ogre::Vector3 pt3 = (orientation * (smt->tringle[i].TriVec[2]* scale)) + position; 

		std::pair<bool, Ogre::Vector3> tResult = IntersectTrangle(ray, pt1, pt2, pt3);	

		if(tResult.first)
		{
			pts[0] = pt1;
			pts[1] = pt2;
			pts[2] = pt3;

			if(flag & E_HITPOS)
			{					
				Ogre::Real squ_dis = (tResult.second - ray.getOrigin()).squaredLength();

				if(squ_dis < nearstIntersectSquareDis)
				{
					nearstIntersectSquareDis = squ_dis;					
					rResult.IsHit  = tResult.first;	
					rResult.HitPos = tResult.second;						
				}			
			}	
			else if(flag & E_ALPHATEST)
			{
				rResult.IsHit  = tResult.first;		
				rResult.HitPos = tResult.second;

				// ��ֵ 

				Lerp(pt1, pt2, pt3, tResult.second, 
					smt->tringle[i].UV[0], smt->tringle[i].UV[1], smt->tringle[i].UV[2], 
					rResult.UV);	
				
				rResult.AlphaTest |= (unsigned int)CheckAlpha(rResult.UV, ((SubMesh*)smt->id)->getMaterialName());					
			}
			else
			{
				rResult.IsHit = tResult.first;
				return rResult.IsHit;
			}
		}
	}


	if(rResult.IsHit && (flag & E_SNAP))		
	{	
		float minDis = MAX_FLOAT;
		Ogre::Vector3 tpt;
		for(int ii = 0; ii < 3; ii ++)
		{
			float tDis = (rResult.HitPos - pts[ii]).length();
			if(tDis < minDis)
			{
				minDis = tDis;
				tpt = pts[ii];
			}
		}
		rResult.HitPos = tpt;
	}

	return rResult.IsHit;
}

// ���㱣����

#define FIX_MIN_MAX(pt, _min, _max)		{(pt).x < (_min).x ? (_min).x = (pt).x : NULL;(pt).x > (_max).x ? (_max).x = (pt).x : NULL;(pt).y < (_min).y ? (_min).y = (pt).y : NULL;(pt).y > (_max).y ? (_max).y = (pt).y : NULL;(pt).z < (_min).z ? (_min).z = (pt).z : NULL;(pt).z > (_max).z ? (_max).z = (pt).z : NULL;}

// -------------------------------------------------	
void SubMeshCach_t::SetDataToSubMeshCach(SubMeshCach_t* smt, Ogre::IndexData* indexData, Ogre::VertexData* vertexData)
{	
	bool use16bitIndexes = 
		indexData->indexBuffer->getType() == Ogre::HardwareIndexBuffer::IT_16BIT;

	Ogre::uint16 *p16;			
	unsigned int v1, v2, v3;			
	if (use16bitIndexes)
	{						
		p16 = static_cast<Ogre::uint16*>(indexData->indexBuffer->lock(
			indexData->indexStart, indexData->indexCount, Ogre::HardwareBuffer::HBL_READ_ONLY));
		
		const VertexElement* posElem		= vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
		const VertexElement* normElem		= vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
		const VertexElement* texCoodElem	= vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);
		
		Ogre::HardwareVertexBufferSharedPtr vbuf = 
			vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
		unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		
		float* pFloat;	
		
		Ogre::Vector3 box_min = smt->box.getMinimum();
		Ogre::Vector3 box_max = smt->box.getMaximum();

		for(size_t j = 0; j < indexData->indexCount / 3; j ++)
		{						
			v1=*p16++ ;
			v2=*p16++ ;
			v3=*p16++ ;

			// λ��

			Ogre::Vector3 pt1, pt2, pt3;
			posElem->baseVertexPointerToElement(vertex+v1*vbuf->getVertexSize(), &pFloat);
			pt1.x = (*pFloat++);
			pt1.y = (*pFloat++);
			pt1.z = (*pFloat++);
			
			FIX_MIN_MAX(pt1, box_min, box_max)	// ���㱣����

			posElem->baseVertexPointerToElement(vertex+v2*vbuf->getVertexSize(), &pFloat);
			pt2.x = (*pFloat++);
			pt2.y = (*pFloat++);
			pt2.z = (*pFloat++);

			FIX_MIN_MAX(pt2, box_min, box_max)	// ���㱣����

			posElem->baseVertexPointerToElement(vertex+v3*vbuf->getVertexSize(), &pFloat);
			pt3.x = (*pFloat++);
			pt3.y = (*pFloat++);
			pt3.z = (*pFloat++);

			FIX_MIN_MAX(pt3, box_min, box_max)	// ���㱣����

			// �ȷŵ�cach��

			smt->tringle[j].TriVec[0] = pt1;
			smt->tringle[j].TriVec[1] = pt2;
			smt->tringle[j].TriVec[2] = pt3;

			// uv����

			float* pCoord;	
			texCoodElem->baseVertexPointerToElement(vertex + v1 * vbuf->getVertexSize(), &pCoord);
			smt->tringle[j].UV[0].x = *pCoord ++;
			smt->tringle[j].UV[0].y = *pCoord   ;

			texCoodElem->baseVertexPointerToElement(vertex + v2 * vbuf->getVertexSize(), &pCoord);
			smt->tringle[j].UV[1].x = *pCoord ++;
			smt->tringle[j].UV[1].y = *pCoord   ;

			texCoodElem->baseVertexPointerToElement(vertex + v3 * vbuf->getVertexSize(), &pCoord);	
			smt->tringle[j].UV[2].x = *pCoord ++;
			smt->tringle[j].UV[2].y = *pCoord   ;

			// ����

			float* pNorm;
			normElem->baseVertexPointerToElement(vertex + v1 * vbuf->getVertexSize(), &pNorm);
			smt->tringle[j].Normal[0].x = *pNorm ++;
			smt->tringle[j].Normal[0].y = *pNorm ++;
			smt->tringle[j].Normal[0].z = *pNorm   ;				
					
			normElem->baseVertexPointerToElement(vertex + v2 * vbuf->getVertexSize(), &pNorm);
			smt->tringle[j].Normal[1].x = *pNorm ++;
			smt->tringle[j].Normal[1].y = *pNorm ++;
			smt->tringle[j].Normal[1].z = *pNorm   ;					

			normElem->baseVertexPointerToElement(vertex + v3 * vbuf->getVertexSize(), &pNorm);
			smt->tringle[j].Normal[2].x = *pNorm ++;
			smt->tringle[j].Normal[2].y = *pNorm ++;
			smt->tringle[j].Normal[2].z = *pNorm   ;

		}

		// ���ñ�����			

		smt->box.setExtents(box_min, box_max);			

		indexData->indexBuffer->unlock();
		vbuf->unlock();
	}	
	else
	{
		MessageBox(NULL, "Intersection function just support 16 bit index!", "ERROR", MB_OK);
	}

}

// ==================================================
// COTEEntityPhysicsMgr
// ==================================================

// �ͷ��ڴ� 

void COTEEntityPhysicsMgr::ReleaseIntersectionCach()
{
	// ģ��cach

	std::list<SubMeshCach_t*>::iterator it = gSubMeshCachList.begin();
	while(it != gSubMeshCachList.end())
	{				
		delete (*it);

		++ it;
	}
	gSubMeshCachList.clear();

	// ��ͼcach

	HashMap<int, TextureCach_t*>::iterator ita =  s_TextureCach.begin();
	while(ita != s_TextureCach.end())
	{
		delete (ita->second);
		++ ita;
	}
	s_TextureCach.clear();

}

// -------------------------------------------------
// Ӧ�ýӿ�
// -------------------------------------------------

// ��ѯ������

bool COTEEntityPhysicsMgr::GetSubList_TriangleIntersection(COTEEntity* ae, const Ogre::Ray& ray, std::list< COTESubEntity* >& subEntList, float maxDis)
{	
	if(ae->mMesh.isNull())	
		return false;	

	const Ogre::Vector3&	position		  = ae->getParentNode()->_getDerivedPosition();
	const Ogre::Quaternion& orientation		  = ae->getParentNode()->_getDerivedOrientation();
	const Ogre::Vector3&	scale			  = ae->getParentNode()->_getDerivedScale();

	unsigned int NumEnts = ae->getNumSubEntities();
	for(unsigned int i = 0; i < NumEnts; i ++)
	{
		COTESubEntity* se = ae->getSubEntity(i);
		
		Ogre::SubMesh * sm = se->getSubMesh();

		// ��cach���ѯ
		
		SubMeshCach_t* smt = SubMeshCach_t::GetFromCach((int)sm);			

		if(smt == NULL)
		{
#ifdef AUTO_UPDATE_BOUNDINGS // �ڵ�һ�β���ʱ �����ģ�����е�submesh���浽�ڴ�
			
			// ���°�Χ��

			Ogre::AxisAlignedBox bounds(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT);

			Ogre::Mesh::SubMeshIterator smIt1 = ae->mMesh->getSubMeshIterator();
			while(smIt1.hasMoreElements())
			{
				Ogre::SubMesh * sm1 = smIt1.getNext();
				smt = SubMeshCach_t::AddToCach((int)sm1, sm1->indexData->indexCount / 3);					
				SubMeshCach_t::SetDataToSubMeshCach(smt, sm1->indexData, sm1->vertexData);
				bounds.merge(smt->box);

			}

			// ���¼���AABB 

			ae->mMesh->_setBounds(bounds);
			ae->getParentSceneNode()->_update(true, true);

			smt = SubMeshCach_t::GetFromCach((int)sm);

#else

			smt = SubMeshCach_t::AddToCach((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
#endif			
		}

		IntersectResult_t tResult;			
		
		if(SubMeshCach_t::GetColliPos(ray, smt, position, orientation, scale, 0, tResult) && 
			(tResult.HitPos - ray.getOrigin()).length() < maxDis)
		{		
			subEntList.push_back(se);							
		}
	}	

	return subEntList.size() > 0;  
}

// -------------------------------------------------
// ��������ײ

IntersectInfo_t COTEEntityPhysicsMgr::GetTriangleIntersection(
								COTEEntity* ae, const Ogre::Ray& ray, unsigned int flag)
{
	// WAIT_LOADING_RES(ae)

	IntersectInfo_t ret;	

	if(ae->mMesh.isNull())	
		return ret;	  

	// ������ʱ��

	//unsigned int tc = ::GetTickCount();

	const Ogre::Vector3&	position= ae->getParentNode()->_getDerivedPosition();
	const Ogre::Quaternion& orientation = ae->getParentNode()->_getDerivedOrientation();
	const Ogre::Vector3&	scale= ae->getParentNode()->_getDerivedScale();
	
	SubMeshCach_t* smt = NULL; 

	Ogre::Mesh::SubMeshIterator smIt = ae->mMesh->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		Ogre::SubMesh * sm = smIt.getNext();

		// ��cach���ѯ
		
		smt = SubMeshCach_t::GetFromCach((int)sm);							

		if(!smt)
		{
#ifdef AUTO_UPDATE_BOUNDINGS // �ڵ�һ�β���ʱ �����ģ�����е�submesh���浽�ڴ�
			
			// ���°�Χ��

			Ogre::AxisAlignedBox bounds(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT);

			Ogre::Mesh::SubMeshIterator smIt1 = ae->mMesh->getSubMeshIterator();
			while(smIt1.hasMoreElements())
			{
				Ogre::SubMesh * sm1 = smIt1.getNext();
				smt = SubMeshCach_t::AddToCach((int)sm1, sm1->indexData->indexCount / 3);					
				SubMeshCach_t::SetDataToSubMeshCach(smt, sm1->indexData, sm1->vertexData);
				bounds.merge(smt->box);

			}

			// ���¼���AABB 

			ae->mMesh->_setBounds(bounds);
			ae->getParentSceneNode()->_update(true, true);

			smt = SubMeshCach_t::GetFromCach((int)sm);

#else

			smt = SubMeshCach_t::AddToCach((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
#endif
		}

		float nearstIntersectSquareDistance = MAX_FLOAT;	

		IntersectResult_t tResult;	
		if(SubMeshCach_t::GetColliPos(ray, smt, position, orientation, scale, flag, tResult))
		{
			if(flag & E_HITPOS)	// ��Ҫ������ײ��	
			{
				Ogre::Real squ_dis = (tResult.HitPos - ray.getOrigin()).squaredLength();			

				if(squ_dis < nearstIntersectSquareDistance)
				{
					nearstIntersectSquareDistance = squ_dis;
					ret.isHit  = tResult.IsHit;	
					ret.hitPos = tResult.HitPos;
							
				}					
			}				
			else
			{
				ret.isHit		= tResult.IsHit;
				ret.alphaTest  |= tResult.AlphaTest;	
				ret.hitPos		= tResult.HitPos;

				if(flag == E_DEFAULT)
					goto _Exit;					
			}
		}			
	}


_Exit:

	// alpha

	ret.isHit = (flag & E_ALPHATEST) ? (ret.alphaTest != 0) && ret.isHit : ret.isHit;
	
	if(ret.isHit)
	{
	//	OTE_TRACE("ģ��: " << ae->getName() << " ������ײ����ʱ��" << (::GetTickCount() - tc) / 1000.0f)
	}

	return ret;      
}


// -------------------------------------------------
// ��������ײ, ��ֱ��Mesh 

IntersectInfo_t COTEEntityPhysicsMgr::GetTriangleIntersection(
																Ogre::Mesh* mesh, 
																const Ogre::Vector3& position, 
																const Ogre::Quaternion& orientation, 
																const Ogre::Vector3&	scale,
																const Ogre::Ray& ray, unsigned int flag)
{
	IntersectInfo_t ret;	

	SubMeshCach_t* smt = NULL; 

	Ogre::Mesh::SubMeshIterator smIt = mesh->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		Ogre::SubMesh * sm = smIt.getNext();

		// ��cach���ѯ
		
		smt = SubMeshCach_t::GetFromCach((int)sm);							

		if(!smt)
		{
			smt = SubMeshCach_t::AddToCach ((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
		}

		float nearstIntersectSquareDistance = MAX_FLOAT;	

		IntersectResult_t tResult;	
		if(SubMeshCach_t::GetColliPos(ray, smt, position, orientation, scale, flag, tResult))
		{
			if(flag & E_HITPOS)	// ��Ҫ������ײ��	
			{
				Ogre::Real squ_dis = (tResult.HitPos - ray.getOrigin()).squaredLength();			

				if(squ_dis < nearstIntersectSquareDistance)
				{
					nearstIntersectSquareDistance = squ_dis;
					ret.isHit  = tResult.IsHit;	
					ret.hitPos = tResult.HitPos;
							
				}					
			}				
			else
			{
				ret.isHit		= tResult.IsHit;
				ret.alphaTest  |= tResult.AlphaTest;	
				ret.hitPos		= tResult.HitPos;

				if(flag == E_DEFAULT)
					goto _Exit;					
			}
		}			
	}


_Exit:

	// alpha

	ret.isHit = (flag & E_ALPHATEST) ? (ret.alphaTest != 0) && ret.isHit : ret.isHit;

	return ret;      
}



// ----------------------------------------------------------------------
// �Ż�ʵ�����ײ���

IntersectInfo_t COTEEntityPhysicsMgr::GetTriangleIntersection(
			COTEAutoEntity* ae, const Ogre::Ray& ray, unsigned int flag)
{
	IntersectInfo_t ret;	

	float nearstIntersectSquareDistance = MAX_FLOAT;	

	SubMeshCach_t* smt = NULL; 
	
	IntersectInfo_t tResult;

	std::vector<COTEAutoEntity::RenderEntity_t*>::iterator it = ae->m_RenderEntityList.begin();
	while(it != ae->m_RenderEntityList.end())
	{		
		Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingleton().getByName((*it)->MeshName);
		
		if(meshPtr.isNull())
		{
			meshPtr = Ogre::MeshManager::getSingleton().load(
				(*it)->MeshName,
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		}
		
		const Ogre::Matrix4&  mat = (*it)->WorldMatrix;
		Ogre::Vector3 position;		mat.getTrans(position);
		Ogre::Vector3 scale;		mat.getScale(scale);
		tResult = COTEEntityPhysicsMgr::GetTriangleIntersection(
																meshPtr.getPointer(), 
																position, 
																(*it)->Rotation,
																scale,
																ray,
																flag
															);

		if(tResult.isHit)
		{
			if(flag & E_HITPOS)	// ��Ҫ������ײ��	
			{
				Ogre::Real squ_dis = (tResult.hitPos - ray.getOrigin()).squaredLength();			

				if(squ_dis < nearstIntersectSquareDistance)
				{
					nearstIntersectSquareDistance = squ_dis;
					ret.isHit  = tResult.isHit;	
					ret.hitPos = tResult.hitPos;
							
				}					
			}				
			else
			{
				ret.isHit		= tResult.isHit;
				ret.alphaTest  |= tResult.alphaTest;	
				ret.hitPos		= tResult.hitPos;

				if(flag == E_DEFAULT)
					goto _Exit;					
			}
		}
	
		++ it;	
	}	

_Exit:

	// alpha

	ret.isHit = (flag & E_ALPHATEST) ? (ret.alphaTest != 0) && ret.isHit : ret.isHit;

	return ret; 
}


}