#pragma once
#include "OTEStaticInclude.h"
#include "OTETile.h"

namespace OTE
{

// ******************************************
// �������ı�ʾ

#define		AUTOLINK_OPER_ADD			0xF0000000		// ��Ӳ���
#define		AUTOLINK_RAISE				0x00F00000		// ����
#define		AUTOLINK_UPDATETERRAIN		0x0000000F		// ����ɽ��

// ******************************************
// COTEEntityAutoLink
// ******************************************

class _OTEExport COTEEntityAutoLink
{
public:	
	static COTEEntityAutoLink*	GetInstance();	

public:	
	
	// �Զ����ӵ��ýӿ�

	void AutoLink(float X, float Z, unsigned int operFlag);
	
	// ǰ׺

	void SetMeshGroupNamePrefix(const Ogre::String& prefix)
	{
		m_MeshGroupNamePrefix = prefix;
	}
	void SetEntityGroupNamePrefix(const Ogre::String& prefix)
	{
		m_EntGroupNamePrefix = prefix;
	}

	// ����߶ȡ�׼�����ݵر�߶������ƶ����¸߶ȡ�

	void ClearHeightCach()
	{
		m_LastHgt = -1.0f;
		m_LastBottomHgt = -1.0f;
	}

	// ���ø߶�

	void SetBottomHeight(float height)
	{	
		ClearHeightCach();
		m_LastBottomHgt = height;
	}

protected:	

	COTEEntityAutoLink();

	void AutoLink(int uX, int uZ, int uIndex, int vIndex, short cornerType, unsigned int operFlag);

	void UpdateEntities(int uX, int uZ, int u, int v, unsigned int operFlag);

	void GetWorldPosition(int uX, int uZ, Ogre::Vector3& position);

	COTEEntity* GetALEntityAtPoint(int uX, int uZ, int& u, int& v);

	void UpdateBorder(int uX, int uZ, int offsetX, int offsetZ, unsigned int operFlag);	
	
protected:	

	static COTEEntityAutoLink s_Inst;

	Ogre::Vector3				m_Scale;					// ���ű�

	Ogre::String				m_MeshGroupNamePrefix;		// ģ����ǰ׺��			
	Ogre::String				m_EntGroupNamePrefix;		// ʵ����ǰ׺��	
	
	float						m_LastHgt;					// �����߶�
	float						m_LastBottomHgt;			// �ײ��߶�
};


}