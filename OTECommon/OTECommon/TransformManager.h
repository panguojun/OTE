#pragma once

#include "Ogre.h"
#include "OTEQTSceneManager.h"
#include <string>

// *********************************************
//	CTransformManager
//  �任(����λ�õ�)������
// *********************************************
class CTransformManager
{
public:
	CTransformManager(void);
	~CTransformManager(void);

public:
	enum{ ePOSITION, eROTATION, eSCALING };
	
public:

	void DrawDummies();

	const std::string& GetFocusedCoordString()
	{
		return m_FocusedCoordString;
	}

	const std::string& PickDummies(float mouseX, float mouseY);

	const std::string& HoverDummies(float mouseX, float mouseY);

	const Ogre::Vector3& GetDummyPos()
	{
		if(m_EditType == ePOSITION)	
			return m_TransCoordDummy.center;	
		else if(m_EditType == eROTATION)
			return m_RotateCoordDummy.center;	
		else if(m_EditType == eSCALING)
			return m_ScaleCoordDummy.center;	
		return Ogre::Vector3::ZERO;
	}

	void SetDummyPos(const Ogre::Vector3& pos)
	{
		//if(m_EditType == ePOSITION)	
			m_TransCoordDummy.center = pos;	
		//else if(m_EditType == eROTATION)
			m_RotateCoordDummy.center = pos;	
		//else if(m_EditType == eSCALING)
			m_ScaleCoordDummy.center = pos;	
	}

	// ͨ����Ļ�ƶ��õ�3D�ƶ�
	void GetViewportMove(
			const Ogre::Vector3& rPos, 
			const Ogre::Vector3& oldPos, 
			const Ogre::Vector2& rScreenMove, 
			Ogre::Vector3& newPos);
	
	// ͨ����Ļ�ƶ��õ�3D��ת
	void GetViewportRotate( const Ogre::Vector3& rPos, 
							const Ogre::Vector2& rScreenMove, 
							Ogre::Quaternion& rOldRot,   
							Ogre::Quaternion& rNewRot,
							Ogre::Vector3* rAngles = NULL);
	
	void GetViewportRotate(	const Ogre::Vector3& rPos, 
							const Ogre::Vector2& rScreenMove, 
							Ogre::Quaternion& rDRot  
							);
	// ͨ����Ļ�ƶ��õ�3D����
	void GetViewportScaling(
			const Ogre::Vector3& rPos, const Ogre::Vector2& rScreenMove, Ogre::Vector3& rOldScale, Ogre::Vector3& rNewScale);

	// ���ñ༭����

	void SetEditType(int editType){	m_EditType = editType;	}

	int GetEditType(){	return m_EditType;	}
	
	// ��������

	void HideAllDummies()
	{
		m_TransCoordDummy.visible = false;
		m_RotateCoordDummy.visible = false;
		m_ScaleCoordDummy.visible = false;
	}
	
	bool IsVisible()
	{
		return	m_TransCoordDummy.visible	||
				m_RotateCoordDummy.visible	||
				m_ScaleCoordDummy.visible;  
	}
	// ��ʾ��������

	void ShowDummyA( bool visible = true )
	{
		ShowDummy(m_EditType, visible);		
	}

	void ShowDummy( int editType, bool visible = true )
	{
		HideAllDummies();

		m_EditType = editType;
		if(m_EditType == ePOSITION)	
			m_TransCoordDummy.visible = visible;	
		else if(m_EditType == eROTATION)
			m_RotateCoordDummy.visible = visible;	
		else if(m_EditType == eSCALING)
			m_ScaleCoordDummy.visible = visible;
	}

	// �ֹ����������ַ���

	void SetCoordLockString(const char* lockString){	m_CoordLockString = lockString;	}
	const std::string& GetCoordAxis() { return m_CoordLockString; }

protected:

	// ��ǰ�༭����

	int			m_EditType;
	
	// �����������ַ���

	std::string	m_CoordLockString;
	
	std::string	m_FocusedCoordString;

private:
	
	// ����ƽ�ƵĽṹ��
	struct	TransCoordDummy_t
	{
		bool	visible;
		Ogre::Vector3 center;
		Ogre::Vector3 px;
		Ogre::Vector3 py;
		Ogre::Vector3 pz;
		
		TransCoordDummy_t()
		{
			visible = false;
			center	= Ogre::Vector3(0, 0, 0);
			px		= Ogre::Vector3(0, 0, 0);
			py		= Ogre::Vector3(0, 0, 0);
			pz		= Ogre::Vector3(0, 0, 0);
		}
	};

	// ������ת�Ľṹ��
	struct	RotateCoordDummy_t
	{
		bool	visible;
		Ogre::Vector3 center;
		Ogre::Vector3 px;
		Ogre::Vector3 py;
		Ogre::Vector3 pz;
		
		RotateCoordDummy_t()
		{
			visible = false;
			center	= Ogre::Vector3(0, 0, 0);
			px		= Ogre::Vector3(0, 0, 0);
			py		= Ogre::Vector3(0, 0, 0);
			pz		= Ogre::Vector3(0, 0, 0);
		}
	};
	
	// �������ŵĽṹ��
	struct	ScalingCoordDummy_t
	{
		bool	visible;
		Ogre::Vector3 center;
		Ogre::Vector3 px;
		Ogre::Vector3 py;
		Ogre::Vector3 pz;
		
		ScalingCoordDummy_t()
		{
			visible = false;
			center	= Ogre::Vector3(0, 0, 0);
			px		= Ogre::Vector3(0, 0, 0);
			py		= Ogre::Vector3(0, 0, 0);
			pz		= Ogre::Vector3(0, 0, 0);
		}
	};

private:	

	TransCoordDummy_t		m_TransCoordDummy;		// ����ƽ�ƵĽṹ��
	RotateCoordDummy_t		m_RotateCoordDummy;		// ������ת�Ľṹ��
	ScalingCoordDummy_t		m_ScaleCoordDummy;		// �������ŵĽṹ��

};	

