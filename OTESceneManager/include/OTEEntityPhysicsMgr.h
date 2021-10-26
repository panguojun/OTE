#pragma once
#include "OTEActorEntity.h"

// ************************************
// 碰撞策略
// ************************************

#define DEFAULT		  E_DEFAULT			// 默认 普通碰撞测试（效率较快）
#define HITPOS		  E_HITPOS			// 带碰撞点的
#define ALPHATEST	  E_ALPHATEST		// 带alpha测试的 

namespace OTE
{
// ************************************
// 碰撞点, 三角形 的数据结构
// ************************************
enum eIntersectionType
{
	E_DEFAULT		=	0x00000000,
	E_HITPOS		=	0x00000001, 
	E_SNAP			=	0x00000010,
	E_ALPHATEST		=	0x00000100,
};

struct IntersectInfo_t
{
	bool			isHit;
	Ogre::Vector3	hitPos;
	Ogre::Vector2	hitUV;
	unsigned int	alphaTest;

	IntersectInfo_t() : 
	isHit(false),
	alphaTest(0)
	{}
};	

struct Tringle_t
{
	Ogre::Vector2 UV[3];
	Ogre::Vector3 TriVec[3];
	Ogre::Vector3 Normal[3];

};

// *******************************************
// 内存mesh数据结构
// *******************************************

struct IntersectResult_t
{
	bool		  IsHit;
	Ogre::Vector3 HitPos;
	Ogre::Vector2 UV;
	unsigned int  AlphaTest;	// 0 代表通过 否则不通过

	IntersectResult_t() : 
	IsHit(false),
	AlphaTest(0)
	{		
	}
};

struct SubMeshCach_t
{
	SubMeshCach_t(int sz, int _id)
	{
		box.setExtents(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT, - MAX_FLOAT);
    
		size = sz;
		id = _id;

		//OTE_TRACE("new SubMeshCach_t " << sz << " " << id);	

		tringle = new Tringle_t[sz];
	}

	~SubMeshCach_t()
	{
		delete []tringle;
	}

	/// 操作

	static SubMeshCach_t*  AddToCach(int id, int sz);

	static SubMeshCach_t*  GetFromCach(unsigned int id);

	static bool			   GetColliPos(			
											const Ogre::Ray& ray, SubMeshCach_t* smt, 
											const Ogre::Vector3& position, 
											const Ogre::Quaternion& orientation,
											const Ogre::Vector3& scale, 
											unsigned int flag,
											IntersectResult_t& rResult
											);

	static void SetDataToSubMeshCach(SubMeshCach_t* smt, Ogre::IndexData* indexData, Ogre::VertexData* vertexData);

	/// 数据

	int						id;
	int						size;
	Tringle_t*				tringle;
	Ogre::AxisAlignedBox	box;
};

// *******************************************
// COTEEntityPhysicsMgr
// 碰撞相关功能实现
// *******************************************

class _OTEExport COTEEntityPhysicsMgr
{
friend class COTEActorEntity;
public:		

	/// 射线查询

	// 实体

	static IntersectInfo_t GetTriangleIntersection(
				COTEEntity* ae, const Ogre::Ray& ray, unsigned int flag = E_HITPOS);

	// mesh

	static IntersectInfo_t GetTriangleIntersection(
												Ogre::Mesh* mesh, 
												const Ogre::Vector3& position, 
												const Ogre::Quaternion& orientation, 
												const Ogre::Vector3&	scale,
												const Ogre::Ray& ray, unsigned int flag);
	// AutoEntity

	static IntersectInfo_t GetTriangleIntersection(
				COTEAutoEntity* ae, const Ogre::Ray& ray, unsigned int flag = E_HITPOS);

	// 材质球碰撞查询

	static bool GetSubList_TriangleIntersection(
						COTEEntity* ae, const Ogre::Ray& ray, 
						std::list< COTESubEntity* >& subEntList, 
						float maxDis = MAX_FLOAT);

protected:

	// 清理cach

	static void ReleaseIntersectionCach();	

};

}
