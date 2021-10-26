#pragma once
#include "Ogre.h"
#include "OTEStaticInclude.h"

namespace OTE
{
	struct _OTEExport MathLib
	{
		// -----------------------------------------------------
		// 围绕着小角度旋转

		static Ogre::Vector2 RotateByDAngle(
							const Ogre::Vector2& Ptin, 
							const Ogre::Vector2& PtCenter,			
							float radious, const Ogre::Radian& angle
								);
		

		// -----------------------------------------------------
		// 围绕着某角度旋转
		// [ cos(a) sin(a) ] 
		// [ -sin(a) cos(a) ] 

		static Ogre::Vector2 RotateByAngle(
							const Ogre::Vector2& Ptin, 
							const Ogre::Vector2& PtCenter,			
							const Ogre::Radian& angle
							);	

		// -----------------------------------------------------
		// 有方向得到旋转角度
	
		static void QuaternionToEuler( const Ogre::Quaternion& quaternion, 
			Ogre::Vector3& angles );

		// -----------------------------------------------------
		// 角度得到旋转四元数	

		static void EulerToQuaternion( const Ogre::Vector3& angles, 
			Ogre::Quaternion& quaternion );

		// -----------------------------------------------------
		// 如果物件只有一个旋转轴(y轴), 那么我们可以这样得到它的旋转角度

		static Ogre::Radian GetRadian(const Ogre::Vector3& dir);

		// -----------------------------------------------------
		// 使得角度在 0 - 360 之间

		static Ogre::Radian FixRadian(Ogre::Radian angle);

		// -----------------------------------------------------
		// 创建变换矩阵

		static void MakeTransform(const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation, 
			Ogre::Matrix4& destMatrix);				

		// 线性插值

		static float Lerp(float a, float b, float factor);

		static void  Lerp(const Ogre::Vector2& a, const Ogre::Vector2& b, float factor, Ogre::Vector2& out);

		static void  Lerp(const Ogre::Vector3& a, const Ogre::Vector3& b, float factor, Ogre::Vector3& out);

	};




}