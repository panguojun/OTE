#pragma once
#include "OTEActorEntity.h"

// ************************************
// ��ײ����
// ************************************

#define DEFAULT		  E_DEFAULT			// Ĭ�� ��ͨ��ײ���ԣ�Ч�ʽϿ죩
#define HITPOS		  E_HITPOS			// ����ײ���
#define ALPHATEST	  E_ALPHATEST		// ��alpha���Ե� 

namespace OTE
{
// ************************************
// ��ײ��, ������ �����ݽṹ
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
// �ڴ�mesh���ݽṹ
// *******************************************

struct IntersectResult_t
{
	bool		  IsHit;
	Ogre::Vector3 HitPos;
	Ogre::Vector2 UV;
	unsigned int  AlphaTest;	// 0 ����ͨ�� ����ͨ��

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

	/// ����

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

	/// ����

	int						id;
	int						size;
	Tringle_t*				tringle;
	Ogre::AxisAlignedBox	box;
};

// *******************************************
// COTEEntityPhysicsMgr
// ��ײ��ع���ʵ��
// *******************************************

class _OTEExport COTEEntityPhysicsMgr
{
friend class COTEActorEntity;
public:		

	/// ���߲�ѯ

	// ʵ��

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

	// ��������ײ��ѯ

	static bool GetSubList_TriangleIntersection(
						COTEEntity* ae, const Ogre::Ray& ray, 
						std::list< COTESubEntity* >& subEntList, 
						float maxDis = MAX_FLOAT);

protected:

	// ����cach

	static void ReleaseIntersectionCach();	

};

}
