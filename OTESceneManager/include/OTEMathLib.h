#pragma once
#include "Ogre.h"
#include "OTEStaticInclude.h"

namespace OTE
{
	struct _OTEExport MathLib
	{
		// -----------------------------------------------------
		// Χ����С�Ƕ���ת

		static Ogre::Vector2 RotateByDAngle(
							const Ogre::Vector2& Ptin, 
							const Ogre::Vector2& PtCenter,			
							float radious, const Ogre::Radian& angle
								);
		

		// -----------------------------------------------------
		// Χ����ĳ�Ƕ���ת
		// [ cos(a) sin(a) ] 
		// [ -sin(a) cos(a) ] 

		static Ogre::Vector2 RotateByAngle(
							const Ogre::Vector2& Ptin, 
							const Ogre::Vector2& PtCenter,			
							const Ogre::Radian& angle
							);	

		// -----------------------------------------------------
		// �з���õ���ת�Ƕ�
	
		static void QuaternionToEuler( const Ogre::Quaternion& quaternion, 
			Ogre::Vector3& angles );

		// -----------------------------------------------------
		// �Ƕȵõ���ת��Ԫ��	

		static void EulerToQuaternion( const Ogre::Vector3& angles, 
			Ogre::Quaternion& quaternion );

		// -----------------------------------------------------
		// ������ֻ��һ����ת��(y��), ��ô���ǿ��������õ�������ת�Ƕ�

		static Ogre::Radian GetRadian(const Ogre::Vector3& dir);

		// -----------------------------------------------------
		// ʹ�ýǶ��� 0 - 360 ֮��

		static Ogre::Radian FixRadian(Ogre::Radian angle);

		// -----------------------------------------------------
		// �����任����

		static void MakeTransform(const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation, 
			Ogre::Matrix4& destMatrix);				

		// ���Բ�ֵ

		static float Lerp(float a, float b, float factor);

		static void  Lerp(const Ogre::Vector2& a, const Ogre::Vector2& b, float factor, Ogre::Vector2& out);

		static void  Lerp(const Ogre::Vector3& a, const Ogre::Vector3& b, float factor, Ogre::Vector3& out);

	};




}