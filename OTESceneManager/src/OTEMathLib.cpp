#include "OTEMathLib.h"
#include <math.h>

using namespace Ogre;
using namespace OTE;

// -----------------------------------------------------
// 围绕着小角度旋转
Ogre::Vector2 MathLib::RotateByDAngle(
				const Ogre::Vector2& Ptin, 
				const Ogre::Vector2& PtCenter,			
				float radious, const Ogre::Radian& angle
					)
{
	Ogre::Vector2 Ptout;

	Ogre::Vector2 vec = Ptin - PtCenter;	

	/// 一阶近似			

	float sita = angle.valueRadians();  // 角度			

	float x = vec.x;
	float y = vec.y;
	
	x = x - y * sita; y = y + x * sita;				
	
	// 得到摄像机新位置

	Ptout.x = x + PtCenter.x; 
	Ptout.y = y + PtCenter.y;
	
	// 纠正位置	
	
	Ptout = (Ptout - PtCenter).normalisedCopy() * radious + PtCenter;

	return Ptout;
}

// -----------------------------------------------------
// 围绕着某角度旋转
// [ cos(a) sin(a) ] 
// [ -sin(a) cos(a) ] 
Ogre::Vector2 MathLib::RotateByAngle(
					const Ogre::Vector2& Ptin, 
					const Ogre::Vector2& PtCenter,			
					const Ogre::Radian& angle					
					)
{		
	Ogre::Vector2 vec = Ptin - PtCenter;	

	return Ogre::Vector2(	vec.x * Ogre::Math::Cos(angle) - vec.y * Ogre::Math::Sin(angle) + PtCenter.x,
							vec.x * Ogre::Math::Sin(angle) + vec.y * Ogre::Math::Cos(angle) + PtCenter.y
							);

}

// ----------------------------------------------------- 
// 围绕着绝对坐标轴旋转的角度
void MathLib::QuaternionToEuler( const Ogre::Quaternion& quaternion, Ogre::Vector3& angles )
{
	float q0 = quaternion.w; 
	float q1 = quaternion.x; 
	float q2 = quaternion.y; 
	float q3 = quaternion.z; 

	float factor = 180.0f / Ogre::Math::PI;

	angles.x =  atan2( 2.0f * (q2*q3 + q0*q1),   (q0*q0 - q1*q1 - q2*q2 + q3*q3)) * factor; 
	angles.y =  asin (-2.0f * (q1*q3 - q0*q2)) * factor; 
	angles.z =  atan2( 2.0f * (q1*q2 + q0*q3),   (q0*q0 + q1*q1 - q2*q2 - q3*q3)) * factor;
}

// ----------------------------------------------------- 
void MathLib::EulerToQuaternion( const Ogre::Vector3& angles, Ogre::Quaternion& quaternion )
{
	quaternion =    Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angles.z)), Ogre::Vector3::UNIT_Z) *
					Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angles.y)), Ogre::Vector3::UNIT_Y) *
					Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angles.x)), Ogre::Vector3::UNIT_X);

}

// ----------------------------------------------------- 
// 与Z轴的夹角
Ogre::Radian MathLib::GetRadian(const Ogre::Vector3& dir)
{
	Ogre::Vector3 curDir = dir;	
	Ogre::Real cos = curDir.dotProduct(Ogre::Vector3::UNIT_Z);
	Ogre::Real sin = curDir.crossProduct(Ogre::Vector3::UNIT_Z).dotProduct(Ogre::Vector3::UNIT_Y);

	return Ogre::Math::ATan2(sin, cos);
}

// ----------------------------------------------------- 
Ogre::Radian MathLib::FixRadian(Ogre::Radian angle)
{
	if(angle.valueDegrees() < 0)
	{
		angle += Ogre::Radian(Ogre::Degree(360));
		
	}
	else if(angle.valueDegrees() > 360)
	{
		angle -= Ogre::Radian(Ogre::Degree(360));
	}
	else
	{
		return angle;
	}

	return FixRadian(angle);
}

// ----------------------------------------------------- 
void MathLib::MakeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation, 
			Matrix4& destMatrix)
{
	destMatrix = Matrix4::IDENTITY;
	// Ordering:
	//    1. Scale
	//    2. Rotate
	//    3. Translate

	// Parent scaling is already applied to derived position
	// Own scale is applied before rotation
	Matrix3 rot3x3, scale3x3;
	orientation.ToRotationMatrix(rot3x3);
	scale3x3 = Matrix3::ZERO;
	scale3x3[0][0] = scale.x;
	scale3x3[1][1] = scale.y;
	scale3x3[2][2] = scale.z;

	destMatrix = rot3x3 * scale3x3;
	destMatrix.setTrans(position);
}

// -----------------------------------------------------------------
float MathLib::Lerp(float a, float b, float factor)
{
	return a * (1.0f - factor) + b * factor;
}

// -----------------------------------------------------------------
void MathLib::Lerp(const Ogre::Vector2& a, const Ogre::Vector2& b, float factor, 
				 Ogre::Vector2& out)
{
	out = a * (1.0f - factor) + b * factor;
}

// -----------------------------------------------------------------
void MathLib::Lerp(const Ogre::Vector3& a, const Ogre::Vector3& b, float factor, 
				 Ogre::Vector3& out)
{
	out = a * (1.0f - factor) + b * factor;
}