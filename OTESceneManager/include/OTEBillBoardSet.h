#pragma once
#include "OTEStaticInclude.h"
#include "OgreBillboardSet.h"
#include "OTEActorEntity.h"

namespace OTE
{

// **********************************************
// COTEBillBoardSet
// **********************************************
class _OTEExport COTEBillBoardSet : public Ogre::BillboardSet
{
public:
	COTEBillBoardSet(const std::string& name, int poolSize);
	COTEBillBoardSet(void);

	virtual ~COTEBillBoardSet(void);

public:
	virtual const Ogre::String& getMovableType(void) const
	{
		static Ogre::String sType = "BBS";
		return sType;
	}

	virtual void genBillboardAxes(Ogre::Camera* cam, Ogre::Vector3* pX, 	
									 Ogre::Vector3 *pY, const Ogre::Billboard* bb);
	    
	virtual void _notifyCurrentCamera(Ogre::Camera* cam);

	virtual void getRenderOperation(Ogre::RenderOperation& op);

	void SetScale(const Ogre::Vector3& s);

	void SetAngle(const Ogre::Radian& angle);

	const Ogre::Radian& GetAngle()
	{
		return m_Angle;
	}

	void SetRotation(const Ogre::Vector3& angles, const Ogre::Quaternion& quat);	// 从3个轴旋转角度力提取

	const Ogre::Quaternion& GetQuaternion()
	{
		return m_Quaternion;
	}

	void SetColour(const Ogre::ColourValue& c);

	const Ogre::ColourValue& GetColour()
	{
		return m_Colour;
	}

public:

	enum eBillBoardType{eCommon, eRotY, eFaceY, eFree};
	eBillBoardType	m_BillBoardType;

protected:

	Ogre::Radian		m_Angle;			// 角度
	Ogre::Quaternion	m_Quaternion;		// 旋转四元数 （主要用于 自由方向）
	Ogre::Vector3		m_Scale;			// 缩放
	Ogre::ColourValue	m_Colour;			// 颜色

};


// **************************************
// COTEBillBoardSetFactory
// **************************************
class COTEBillBoardSetFactory : public CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat);

	virtual void Delete(const std::string& Name);

	virtual void GetTypeList(std::list<std::string>& rTypeList);

	virtual void GetTypeDescList(std::list<std::string>& rTypeDescList);

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "公告牌";
	}
};

}
