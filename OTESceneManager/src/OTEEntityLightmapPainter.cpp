/************************************************************
 ************************************************************

 模块名称： 实体光照图计算实现
 相关模块： 实体类 COTEEntity, 
            碰撞相关 COTECollisionManager, 
					 COTEEntityPhysicsMgr 
						
 描述    ： 物件上光照图技术。
            通过三角形的uv 构造一个2维三角形，然后逐个像素进行插值
			计算其三维位置，及其发线，然后进行碰撞测试。

            实现的难度主要是计算精度，处理编辑问题上。
            我们的方法是边界上的像素只计算一边，无论其处于哪个三角形，
			此外我们试图通过把三角形放大的方法使得像素能够按照三角形
			内部插值计算其三维位置。

			光照图为bmp格式，使用时从模版拷贝创建。
			注意：模版光照图必须为黑色透明。

 *************************************************************
 *************************************************************/

#include "oteentitylightmappainter.h"
#include "OgreD3D9Texture.h"
#include "OTEMathLib.h"
#include "OTECollisionManager.h"
#include "OTEQTSceneManager.h"
#include "direct.h"

using namespace Ogre;
using namespace OTE;

// -----------------------------------------------------------------
// 数学函数
// -----------------------------------------------------------------

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
static bool  CheckPointIn2DTriangle(
				const Ogre::Vector2& point, const Ogre::Vector2& a, const Ogre::Vector2& b, const Ogre::Vector2& c,
				float tolerance = 0.005f) 
{		
	Ogre::Real total_angles = 0.0f;	

	// make the 3 vectors

	Ogre::Vector2 v1(point.x - a.x, point.y - a.y);
	v1.normalise();
	Ogre::Vector2 v2(point.x - b.x, point.y - b.y);
	v2.normalise();
	Ogre::Vector2 v3(point.x - c.x, point.y - c.y);	
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


// ----------------------------------------------------- 
// 要保证 a b c 逆时针顺序 
static bool  CheckPointIn2DTriangle_LineLogic(
				const Ogre::Vector2& point, const Ogre::Vector2& a, const Ogre::Vector2& b, const Ogre::Vector2& c,
				float tolerance = 0.00001f) 
{		
	static struct Line_Dita_t	{float A;float B;float C;float RR;} Line_Dita[3];

	// 保证逆时针顺序 

	Line_Dita[0].A = b.y - a.y;
	Line_Dita[0].B = a.x - b.x;
	Line_Dita[0].C = b.x * a.y - a.x * b.y;
	Line_Dita[0].RR = Line_Dita[0].A * Line_Dita[0].A + Line_Dita[0].B * Line_Dita[0].B;

	Line_Dita[1].A = c.y - b.y;
	Line_Dita[1].B = b.x - c.x;
	Line_Dita[1].C = c.x * b.y - b.x * c.y;
	Line_Dita[1].RR = Line_Dita[1].A * Line_Dita[1].A + Line_Dita[1].B * Line_Dita[1].B;

	Line_Dita[2].A = a.y - c.y;
	Line_Dita[2].B = c.x - a.x;
	Line_Dita[2].C = a.x * c.y - c.x * a.y;
	Line_Dita[2].RR = Line_Dita[2].A * Line_Dita[2].A + Line_Dita[2].B * Line_Dita[2].B;
	
	for( int i = 0; i < 3; ++ i)
	{		
		float d = Line_Dita[i].A * point.x + Line_Dita[i].B * point.y + Line_Dita[i].C;		

		if((d < 0.0f ? -1.0f : 1.0f) * d * d / Line_Dita[i].RR <= - (tolerance * tolerance)) 		
		{
			return false;
		}
	}

	return true;	
}

// -----------------------------------------------------------------
// 插值

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

// -----------------------------------------------------------------
// Gouraud Shading lerp
// http://medialab.di.unipi.it/web/IUM/Waterloo/node84.html
// 三角形插值

inline float _CROSS(const Ogre::Vector2&v1, const Ogre::Vector2&v2)	{ return (v1).x * (v2).y - (v2).x * (v1).y; }

static void Lerp(
	const Ogre::Vector2& a,  const Ogre::Vector2& b,  const Ogre::Vector2& c,	const Ogre::Vector2& p,
	const Ogre::Vector3& va, const Ogre::Vector3& vb, const Ogre::Vector3& vc,	
	Ogre::Vector3& vp, int scaleDepth = 2)
{
	// 三角形是不是面积为零

	float DABC = _CROSS((b - a), (c - a));
	if(DABC == 0.0f)
	{
		vp = va;
		return;
	}

	// 先检测p是否在三角形内 如果否 那么扩大这个三角形

	if(scaleDepth > 0 && !CheckPointIn2DTriangle(p, a, b, c, 0.000001f))
	{
		Ogre::Vector2 center = (a + b + c) / 3.0f;
		float scl = 1.1f; // 放大系数
		Ogre::Vector2 _a = (a - center) * scl + center;
		Ogre::Vector2 _b = (b - center) * scl + center;
		Ogre::Vector2 _c = (c - center) * scl + center;

		// 递归

		return Lerp(_a, _b, _c, p, va, vb, vc, vp, -- scaleDepth );	
	}

	float DPBC = _CROSS((b - p), (c - p));
	float DAPC = _CROSS((c - p), (a - p));
	float DABP = _CROSS((a - p), (b - p));	

	if((DPBC * DABC) < 0.0f)
	{
		DPBC = - DPBC;
		
	}
	if((DAPC * DABC) < 0.0f)
	{
		DAPC = - DAPC;
	
	}
	if((DABP * DABC) < 0.0f)
	{
		DABP = - DABP;			
	}


	vp = (DPBC * va + DAPC * vb + DABP * vc) / (DPBC + DAPC + DABP);
}

// -----------------------------------------------------------------
// 根据线段分割比例 做线性插值

static void Plane_Lerp(
	const Ogre::Vector2& a,  const Ogre::Vector2& b,  const Ogre::Vector2& c,	const Ogre::Vector2& p,
	const Ogre::Vector3& va, const Ogre::Vector3& vb, const Ogre::Vector3& vc,	
	Ogre::Vector3& vp, int scaleDepth = 2)
{
	// 三角形面积为零

	float DABC = _CROSS((b - a), (c - a));
	if(DABC == 0.0f)
	{
		vp = va;
		return;
	}

	// 先检测p是否在三角形内 如果否 那么扩大这个三角形

	if(scaleDepth > 0 && !CheckPointIn2DTriangle(p, a, b, c, 0.00001f))
	{
		Ogre::Vector2 center = (a + b + c) / 3.0f;
		float scl = 1.1f; // 放大系数
		Ogre::Vector2 _a = (a - center) * scl + center;
		Ogre::Vector2 _b = (b - center) * scl + center;
		Ogre::Vector2 _c = (c - center) * scl + center;

		// 递归

		return Plane_Lerp(_a, _b, _c, p, va, vb, vc, vp, -- scaleDepth );	
	}

	static Ogre::Vector2 q;

	if(c.x == b.x && p.x == a.x)
	{
		vp = va;
		return;
	}
	else if(c.x == b.x)
	{		
		q.x = b.x;	
		float k2 = (p.y - a.y) / (p.x - a.x);
		q.y = (q.x - a.x) * k2 + a.y;		
	}
	else if(p.x == a.x)
	{
		q.x = a.x;	
		float k1 = (c.y - b.y) / (c.x - b.x);
		q.y = (q.x - b.x) * k1 + b.y;
	}
	else
	{
		float k1 = (c.y - b.y) / (c.x - b.x);
		float k2 = (p.y - a.y) / (p.x - a.x);	
			
		if(k2 == k1) // 平行的情况 略加处理		
			k2 = k1 + 0.0001f;
			
		q.x = (a.x * k2 - b.x * k1 + b.y - a.y) / (k2 - k1);
		q.y = k1 * (q.x - b.x) + b.y;
	}

	static Ogre::Vector3 vq;

	Ogre::Vector2 cb = (c - b);
	float lcb = cb.length();
	if(lcb < 0.00001f)
	{
		vq = vb;
	}
	else
	{
		float fbq = (q - b).dotProduct(cb / lcb) / lcb;		
		MathLib::Lerp(vb, vc, fbq, vq);		
	}

	Ogre::Vector2 qa = (q - a);
	float lqa = qa.length();
	if(lqa < 0.00001f)
	{
		vp = va;
	}
	else
	{
		float fap = (p - a).dotProduct(qa / lqa) / lqa;		
		MathLib::Lerp(va, vq, fap, vp);	
	}	

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

// --------------------------------------------------------
// 喷绘贴图

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

const int			c_max_lm_size	= 2048; // 注意 光照图尺寸不允许超过这个尺寸

unsigned char*		g_pLMDirtyCach		= NULL;

// --------------------------------------------------------
static inline void paintTexture(int xOffset, int yOffset, int paintSize, ARGB paintColor, 
						 const D3DLOCKED_RECT* rect, Ogre::D3D9Texture* tex, 
						 bool isOutLocked = false, bool failPainted = false, float alpha = 1.0f )
{
	if(!g_pLMDirtyCach)
	{
		g_pLMDirtyCach = new unsigned char[c_max_lm_size * c_max_lm_size]; 
		memset(g_pLMDirtyCach, 0, c_max_lm_size * c_max_lm_size);
	}

	if(!isOutLocked)
	{	
		HRESULT hr;
		static D3DLOCKED_RECT _rect;

		hr = tex->getNormTexture()->LockRect( 0, &_rect, NULL, 0 ); 

		if (FAILED(hr)) 
		{
			OTE_MSG("paintTexture 中 tex->LockRect 失败!", "失败")
			return;
		} 	

		rect = &_rect;
	}	 
		
	// 喷绘	
	// 颜色值为argb格式

	DWORD* dst = (DWORD*)rect->pBits; 

	dst += yOffset * rect->Pitch / sizeof(DWORD);	

	int xOffsetEnd = (xOffset + paintSize) % tex->getWidth();
	int yOffsetEnd = (yOffset + paintSize) % tex->getHeight();

	paintColor |= 0xFF000000;	// 不透明

	for (int i = yOffset; i < yOffsetEnd; ++ i) 
	{ 
		for (int j = xOffset; j < xOffsetEnd; ++ j) 
		{ 
			if(!failPainted || !(dst[j] & 0x00FFFFFF)) // 原始光照图为黑色
			{
				// 替换

				if(abs(alpha - 1.0f) < 0.001f)
				{
					dst[j] = paintColor; 
				}

				/// 注意下边两种方法 由于光照图在一个像素上的重复计算造成的问题 暂时不能达到满意的效果！！
				
				// 叠加

				else if(alpha > 1.0f)
				{
					if(g_pLMDirtyCach[i * c_max_lm_size + j] == 0) // 检测是否重复计算
					{
						Ogre::ColourValue srcColor;
						srcColor.r = GetRVal(paintColor) / 255.0f; 
						srcColor.g = GetGVal(paintColor) / 255.0f; 
						srcColor.b = GetBVal(paintColor) / 255.0f; 
							
						Ogre::ColourValue desColor;
						desColor.r = GetRVal(dst[j]    ) / 255.0f; 
						desColor.g = GetGVal(dst[j]    ) / 255.0f; 
						desColor.b = GetBVal(dst[j]    ) / 255.0f; 

						desColor = srcColor + desColor; // 叠加

						desColor.r > 1.0f ? desColor.r = 1.0f : NULL;
						desColor.g > 1.0f ? desColor.g = 1.0f : NULL;
						desColor.b > 1.0f ? desColor.b = 1.0f : NULL;
						
						desColor.a = 1.0f;	// 不透明

						dst[j] = desColor.getAsARGB();	

						g_pLMDirtyCach[i * c_max_lm_size + j] = 1;
					}
				}

				// 混合

				else
				{		
					if(g_pLMDirtyCach[i * c_max_lm_size + j] == 0) // 检测是否重复计算
					{
						Ogre::ColourValue srcColor;
						srcColor.r = GetRVal(paintColor) / 255.0f; 
						srcColor.g = GetGVal(paintColor) / 255.0f; 
						srcColor.b = GetBVal(paintColor) / 255.0f; 
							
						Ogre::ColourValue desColor;
						desColor.r = GetRVal(dst[j]    ) / 255.0f; 
						desColor.g = GetGVal(dst[j]    ) / 255.0f; 
						desColor.b = GetBVal(dst[j]    ) / 255.0f; 

						desColor   = srcColor * alpha + desColor * (1.0f - alpha); // 混合
						
						desColor.a = 1.0f;	// 不透明

						dst[j]	   = desColor.getAsARGB();	

						g_pLMDirtyCach[i * c_max_lm_size + j] = 1;
					}
				}

			}
		} 

		dst += rect->Pitch / sizeof(DWORD);
	}
	
	// unlock

	if(!isOutLocked)
		tex->getNormTexture()->UnlockRect(0); 

}

// --------------------------------------------------------
static inline void pickTextureColor(Ogre::ColourValue& rColor, int xOffset, int yOffset, 
									const D3DLOCKED_RECT* rect, Ogre::D3D9Texture* tex, 
									bool isOutLocked = false )
{
	if(!isOutLocked)
	{	
		HRESULT hr;
		static D3DLOCKED_RECT _rect;

		hr = tex->getNormTexture()->LockRect( 0, &_rect, NULL, 0 ); 

		if (FAILED(hr)) 
		{
			OTE_MSG("paintTexture 中 tex->LockRect 失败!", "失败")
			return;
		} 	

		rect = &_rect;
	}			
	
	// 颜色值为argb格式

	DWORD* dst = (DWORD*)rect->pBits; 

	dst += yOffset * rect->Pitch / sizeof(DWORD);

	rColor.r = GetRVal(dst[xOffset] ) / 255.0f; 
	rColor.g = GetGVal(dst[xOffset] ) / 255.0f; 
	rColor.b = GetBVal(dst[xOffset] ) / 255.0f; 

	// unlock

	if(!isOutLocked)
		tex->getNormTexture()->UnlockRect(0); 

}

// --------------------------------------------------------
// 吸取颜色	

static void pickColor(    Ogre::ColourValue& rColor,
					      const SubMesh *sm, const uint16 *p16, const VertexElement* texCoodElem, const VertexElement* normElem, int vertexSize, unsigned char* vertex,
						  Ogre::D3D9Texture* tex, const D3DLOCKED_RECT* rect,
						  size_t triangleIndex, const Tringle_t* tri, const Ogre::Quaternion& orientation, 
						  const Ogre::Ray&	_ray,
						  const Ogre::Vector3& rHitPos
						)
{
	assert((sm && triangleIndex != -1) && "paintTriangle 输入无效");

	// 获取uv 坐标

	float Coord[3][2];
	Ogre::Vector3 normal[3];

	// 查询纹理坐标
	{
		unsigned int v1 = *(p16 + triangleIndex * 3	   );
		unsigned int v2 = *(p16 + triangleIndex * 3 + 1);
		unsigned int v3 = *(p16 + triangleIndex * 3 + 2);

		float* pCoord;	
		texCoodElem->baseVertexPointerToElement(vertex + v1 * vertexSize, &pCoord);
		Coord[0][0] = *pCoord ++;
		Coord[0][1] = *pCoord   ;

		texCoodElem->baseVertexPointerToElement(vertex + v2 * vertexSize, &pCoord);
		Coord[1][0] = *pCoord ++;
		Coord[1][1] = *pCoord   ;

		texCoodElem->baseVertexPointerToElement(vertex + v3 * vertexSize, &pCoord);	
		Coord[2][0] = *pCoord ++;
		Coord[2][1] = *pCoord   ;	

		float* pNorm;
		normElem->baseVertexPointerToElement(vertex + v1 * vertexSize, &pNorm);
		normal[0].x = *pNorm++;
		normal[0].y = *pNorm++;
		normal[0].z = *pNorm  ;	
		normal[0] = orientation * normal[0];
				
		normElem->baseVertexPointerToElement(vertex + v2 * vertexSize, &pNorm);
		normal[1].x = *pNorm++;
		normal[1].y = *pNorm++;
		normal[1].z = *pNorm  ;	
		normal[1] = orientation * normal[1];

		normElem->baseVertexPointerToElement(vertex + v3 * vertexSize, &pNorm);
		normal[2].x = *pNorm++;
		normal[2].y = *pNorm++;
		normal[2].z = *pNorm  ;	
		normal[2] = orientation * normal[2];

	}

	// 是否在三角形内部
	
	Ogre::Vector2 a(Coord[0][0], Coord[0][1]);
	Ogre::Vector2 b(Coord[1][0], Coord[1][1]);
	Ogre::Vector2 c(Coord[2][0], Coord[2][1]);

	Ogre::Vector2 p;
	Lerp(tri->TriVec[0], tri->TriVec[1], tri->TriVec[2], rHitPos, a, b, c, p);

	uint xOffset = p.x * tex->getWidth() ;
	uint yOffset = p.y * tex->getHeight();

	pickTextureColor(rColor, xOffset % tex->getWidth(), yOffset % tex->getHeight(), rect, tex, true);	

}


// --------------------------------------------------------
// 喷绘一个三角形	

static void paintTriangle(const SubMesh *sm, const uint16 *p16, const VertexElement* texCoodElem, const VertexElement* normElem,
						  int vertexSize, unsigned char* vertex,
						  Ogre::D3D9Texture* tex, const D3DLOCKED_RECT* rect,
						  size_t triangleIndex, const Tringle_t* tri, const Ogre::Quaternion& orientation, 
						  const Ogre::Ray&	_ray,
						  const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 
						  bool bRenderShadow = true,
						  bool bFaceCulling = true,
						  COTEEntityLightmapPainter::GetLightInforHndler_t pLightInforHndler = NULL, 
						  float blend_alpha = 1.0f,
						  float manualEditSize = 0.0f,
						  const Ogre::Vector3& rHitPos = Ogre::Vector3::ZERO)
{
	const Ogre::Vector3&	lightDir = _ray.getDirection();

	assert((sm && triangleIndex != -1) && "paintTriangle 输入无效");

	// 获取uv 坐标

	float Coord[3][2];
	Ogre::Vector3 normal[3];
		
	// 查询纹理坐标
	{
		unsigned int v1 = *(p16 + triangleIndex * 3	   );
		unsigned int v2 = *(p16 + triangleIndex * 3 + 1);
		unsigned int v3 = *(p16 + triangleIndex * 3 + 2);

		float* pCoord;	
		texCoodElem->baseVertexPointerToElement(vertex + v1 * vertexSize, &pCoord);
		Coord[0][0] = *pCoord ++;
		Coord[0][1] = *pCoord   ;

		texCoodElem->baseVertexPointerToElement(vertex + v2 * vertexSize, &pCoord);
		Coord[1][0] = *pCoord ++;
		Coord[1][1] = *pCoord   ;

		texCoodElem->baseVertexPointerToElement(vertex + v3 * vertexSize, &pCoord);	
		Coord[2][0] = *pCoord ++;
		Coord[2][1] = *pCoord   ;

		float* pNorm;
		normElem->baseVertexPointerToElement(vertex + v1 * vertexSize, &pNorm);
		normal[0].x = *pNorm++;
		normal[0].y = *pNorm++;
		normal[0].z = *pNorm  ;	
		normal[0] = orientation * normal[0];
				
		normElem->baseVertexPointerToElement(vertex + v2 * vertexSize, &pNorm);
		normal[1].x = *pNorm++;
		normal[1].y = *pNorm++;
		normal[1].z = *pNorm  ;	
		normal[1] = orientation * normal[1];

		normElem->baseVertexPointerToElement(vertex + v3 * vertexSize, &pNorm);
		normal[2].x = *pNorm++;
		normal[2].y = *pNorm++;
		normal[2].z = *pNorm  ;	
		normal[2] = orientation * normal[2];
	}
	
	///////////////////////////////////////////
	/// 填充三角形的算法
	///////////////////////////////////////////	

	// 步长

	float pixelUnitSizeX = 1.0f / tex->getWidth();
	float pixelUnitSizeZ = 1.0f / tex->getHeight();

	// 矩形

	float minX = MAX_FLOAT, maxX = - MAX_FLOAT;
	float minZ = MAX_FLOAT, maxZ = - MAX_FLOAT;	

	for(int i = 0; i < 3; ++ i)
	{		
		float x = Coord[i][0];
		float z = Coord[i][1];

		x < minX ? minX = x - pixelUnitSizeX : NULL;
		z < minZ ? minZ = z - pixelUnitSizeZ : NULL;
		x > maxX ? maxX = x + pixelUnitSizeX : NULL;
		z > maxZ ? maxZ = z + pixelUnitSizeZ : NULL;
	}

	// 把矩形扩大一点

	minX = minX <= pixelUnitSizeX		? 0.0f	: minX - pixelUnitSizeX;
	minZ = minZ <= pixelUnitSizeZ		? 0.0f	: minZ - pixelUnitSizeZ;
	maxX = maxX + pixelUnitSizeX > 1.0f ? 1.0f	: maxX + pixelUnitSizeX;
	maxZ = maxZ + pixelUnitSizeZ > 1.0f ? 1.0f	: maxZ + pixelUnitSizeZ;

	// 是否在三角形内部
	
	Ogre::Vector2 a(Coord[0][0], Coord[0][1]);
	Ogre::Vector2 b(Coord[1][0], Coord[1][1]);
	Ogre::Vector2 c(Coord[2][0], Coord[2][1]);

	bool isAnticlockwise = (a.x - b.x) * (a.y - c.y) - (a.x - c.x) * (a.y - b.y) > 0.0f; // 排列顺序
	bool isInTri;	

	// 如果是手工编辑

	if(manualEditSize > 0.0f)
	{
		Ogre::Vector3 nt = (normal[0] + normal[1] + normal[2]) / 3.0f; 

		if(nt.dotProduct(_ray.getDirection()) < - 0.5) // 只画正面
		{			
			Ogre::Vector2 p;
			Lerp(tri->TriVec[0], tri->TriVec[1], tri->TriVec[2], rHitPos, a, b, c, p);

			uint xOffset = p.x * tex->getWidth() ;
			uint yOffset = p.y * tex->getHeight();
		
			paintTexture(xOffset % tex->getWidth(), yOffset % tex->getHeight(), (int)manualEditSize, ambient.getAsARGB() | 0xFF000000, rect, tex, true, false, blend_alpha);		
		}
		return;
	}

	
	for(float j = minZ; j <= maxZ; j += pixelUnitSizeZ)
	for(float i = minX; i <= maxX; i += pixelUnitSizeX)
	{
		Ogre::Vector2 p(i, j); 	// uv位置		

		if(isAnticlockwise)			
			isInTri = CheckPointIn2DTriangle_LineLogic(p, a, c, b, pixelUnitSizeZ * 2.0f);
		else	
			isInTri = CheckPointIn2DTriangle_LineLogic(p, a, b, c, pixelUnitSizeZ * 2.0f);				
		
		if(isInTri)
		{
			uint xOffset = p.x * tex->getWidth() ;
			uint yOffset = p.y * tex->getHeight();

			// 对发线线性插值

			static Ogre::Vector3 nt;
			Lerp(a, b, c, p, 
				normal[0], normal[1], normal[2], 
				nt);
			// nt = (normal[0] + normal[1] + normal[2]) / 3.0f;	

			static Ogre::ColourValue paintColour;

			// 计算碰撞		
			
			static Ogre::Vector3 hitPos;
						
			// 对位置线性插值				

			static Ogre::Vector3 point3d;

			Plane_Lerp(a, b, c, p, 
				tri->TriVec[0], tri->TriVec[1], tri->TriVec[2], 
				point3d, 2);			
				
			bool isIn3DTri = false;

			if(CheckPointInTriangle(point3d, tri->TriVec[0], tri->TriVec[1], tri->TriVec[2], 0.00001f))
			{
				isIn3DTri = true;
			}	

			if(pLightInforHndler) // 特别的光源比如点光源
			{
				Ogre::Vector3		_position;
				Ogre::Vector3		_direction = lightDir;
				Ogre::ColourValue	_diffuse   = diffuse;
				float				reflectDis;
				float				reflectDis0;

				(*pLightInforHndler)(point3d, _position, _direction, _diffuse, reflectDis, reflectDis0, blend_alpha);			
				
				Ogre::Ray ray(point3d + nt * 0.40f /*发线上的小偏移*/, - _direction);
				
				int hits = 0;
				
				bool isHit = (bRenderShadow && (
							(COTETilePage::s_CurrentPage ? COTECollisionManager::IntersectMap(COTETilePage::s_CurrentPage, 
							 ray, 0.25f, reflectDis, hits, hitPos) : false) ||
							 COTECollisionManager::TriangleIntersection(ray, hitPos, reflectDis, E_ALPHATEST))
								);
				if(isHit)
				{
					paintColour = ambient;
				}	
				else
				{		
					//bFaceCulling = true; //  暂时关闭
					float factor = bFaceCulling ? - _direction.dotProduct(nt) : abs(lightDir.dotProduct(nt));		// 由于法线于光源的夹角而产生效果		
					factor > 1.0f ? factor = 1.0f : NULL;
					factor < 0.0f ? factor = 0.0f : NULL;
					paintColour = _diffuse * factor + ambient;	// 光线照射下的颜色
				}

			}
			else
			{
				Ogre::Ray ray(point3d + nt * 0.40f /*发线上的小偏移*/, - lightDir);

				int hits = 0;
				bool isHit = (bRenderShadow && (
							(COTETilePage::s_CurrentPage ? COTECollisionManager::IntersectMap(COTETilePage::s_CurrentPage, 
							 ray, 0.25f, 100.0f, hits, hitPos) : false) ||
							 COTECollisionManager::TriangleIntersection(ray, hitPos, MAX_FLOAT, E_ALPHATEST))
								);

				if(isHit)
				{
					paintColour = ambient;
				}	
				else
				{
					//bFaceCulling = true; //  暂时关闭
					float factor = bFaceCulling ?  - lightDir.dotProduct(nt) : abs(lightDir.dotProduct(nt));		// 由于法线于光源的夹角而产生效果		
					factor > 1.0f ? factor = 1.0f : NULL;
					factor < 0.0f ? factor = 0.0f : NULL;
					paintColour = diffuse * factor + ambient;		// 光线照射下的颜色
				}
			}			

			/// fixed

			paintColour.r < 0.0f ? paintColour.r = 0.0f : NULL;
			paintColour.g < 0.0f ? paintColour.g = 0.0f : NULL;
			paintColour.b < 0.0f ? paintColour.b = 0.0f : NULL;
		
			paintColour.r > 1.0f ? paintColour.r = 1.0f : NULL;
			paintColour.g > 1.0f ? paintColour.g = 1.0f : NULL;
			paintColour.b > 1.0f ? paintColour.b = 1.0f : NULL;				
			
			paintTexture(xOffset % tex->getWidth(), yOffset % tex->getHeight(), 1, paintColour.getAsARGB() | 0xFF000000, rect, tex, true, !isIn3DTri, blend_alpha);			

		}		
	}
}

// -----------------------------------------------
bool COTEEntityLightmapPainter::_GenLightmap(COTEEntity* e,
											const Ogre::Ray&	 ray,			
											const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,
											Ogre::String& lightmapTextName,
											bool bRenderShadow,
											COTEEntityLightmapPainter::GetLightInforHndler_t	 pLightInforHndler,
											float blend_alpha,
											float manualEditSize
											)
{
	if(!e || e->getMesh().isNull())	
	{
		//OTE_MSG_ERR("生成光照图失败！ 实体不存在 或者 Mesh资源不存在!")
		return false;	  
	}

	const Ogre::Vector3& lightDir = ray.getDirection();

	// 加载光照图（一个实体对应一张光照图 ） 	
	
	Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().getByName(			
		lightmapTextName
		);

	if(texPtr.isNull())
	{	
		if(!Ogre::ResourceGroupManager::getSingleton().resourceExists(
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			lightmapTextName)
			)
		{
			OTE_MSG("GenLightmap 时 光照图没有找到! " << lightmapTextName, "失败")
			return false;
		}

		// 加载光照图

		texPtr = TextureManager::getSingleton().load(
					lightmapTextName,
					ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					TEX_TYPE_2D, 0);
		
	}		
	
	Ogre::D3D9Texture* d3dtex = (Ogre::D3D9Texture*)texPtr.getPointer();

	// 检查光照图尺寸是否过大

	if(	d3dtex->getWidth () > c_max_lm_size ||
		d3dtex->getHeight() > c_max_lm_size	 )
	{
		OTE_MSG_ERR("光照图尺寸过大！ 最大尺寸必须 < " << c_max_lm_size)
		return false;
	}

	D3DLOCKED_RECT rect; 
	HRESULT hr = d3dtex->getNormTexture()->LockRect( 0, &rect, NULL, 0 ); 

	if (FAILED(hr)) 
	{
		OTE_MSG("GenLightmap 时 tex->LockRect 失败!", "失败")
		return false;
	} 

	// 清理 光照图颜色 

	if(manualEditSize <= 0)
		paintTexture(0, 0, d3dtex->getWidth(), 0, &rect, d3dtex, true);

	// 位置旋转所放

	const Ogre::Vector3& position		= e->getParentNode()->_getDerivedPosition();
	const Ogre::Quaternion& orientation = e->getParentNode()->_getDerivedOrientation();
	const Ogre::Vector3& scale			= e->getParentNode()->_getDerivedScale();

	// 每个submesh计算

	Ogre::Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		Ogre::SubMesh * sm = smIt.getNext();

		// 从cach里查询
		
		SubMeshCach_t* smt = SubMeshCach_t::GetFromCach((int)sm);			

		if(smt == NULL)
		{
			smt = SubMeshCach_t::AddToCach((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
		}	
		
		const VertexElement* texCoodElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);
		if(!texCoodElem)
		{
			//OTE_MSG_ERR("生成光照图失败！ 模型不支持两层纹理! Model: " << e->getName())
			OTE_TRACE  ("生成光照图失败！ 模型不支持两层纹理! Model: " << e->getName())
				
			d3dtex->getNormTexture()->UnlockRect(0); 
			return false;
		}

		uint16 *p16 = static_cast<uint16*>(sm->indexData->indexBuffer->lock(
			sm->indexData->indexStart, sm->indexData->indexCount, HardwareBuffer::HBL_READ_ONLY));

		const VertexElement* normElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
	
		HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(0);
		
		unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		// 检查是否双面
		
		bool isFaceCull = true;
		if(sm->getMaterialName().find("Scene_cull") != std::string::npos)
		{
			std::string meshName = e->getMesh()->getName();
			std::string path, fullpath;
			OTEHelper::GetFullPath(meshName, path, fullpath);
			if(fullpath.find("恶魔植物") != std::string::npos)
				isFaceCull = false;
		}

		float nearstDis = MAX_FLOAT;
		Tringle_t hitTvs;
		int index = -1;
		Ogre::Vector3 hitPos;
		for(int i = 0; i < smt->size; i ++)
		{		
			static Tringle_t tvs;
			tvs.TriVec[0] = (orientation * (smt->tringle[i].TriVec[0]* scale)) + position;                         			
			tvs.TriVec[1] = (orientation * (smt->tringle[i].TriVec[1]* scale)) + position;                         			
			tvs.TriVec[2] = (orientation * (smt->tringle[i].TriVec[2]* scale)) + position; 
			
			if(manualEditSize <= 0.0f)
			{
				// 喷绘一个三角形

				paintTriangle(
					sm, p16, texCoodElem, normElem, vbuf->getVertexSize(), vertex,
					d3dtex, &rect,
					i, &tvs, orientation,
					ray, diffuse, ambient,
					bRenderShadow,
					isFaceCull,
					pLightInforHndler,
					blend_alpha,
					manualEditSize
					);
			}
			else // 手工编辑
			{
				std::pair<bool, Ogre::Vector3> tResult = IntersectTrangle(ray,tvs.TriVec[0],tvs.TriVec[1],tvs.TriVec[2]);
				
				if(tResult.first)
				{
					float dis = (tResult.second - ray.getOrigin()).length();

					if(dis < nearstDis)
					{
						nearstDis = dis;
						index = i;
						hitPos = tResult.second;
						hitTvs = tvs;

					}
				}
			}
		}

		// 手工编辑

		if(manualEditSize > 0.0f && index >= 0)
		{	
			// 喷绘一个三角形

			paintTriangle(
				sm, p16, texCoodElem, normElem, vbuf->getVertexSize(), vertex,
				d3dtex, &rect,
				index, &hitTvs, orientation,
				ray, Ogre::ColourValue::Black, ambient,
				false,
				isFaceCull,
				NULL,
				blend_alpha,
				manualEditSize,
				hitPos
				);	
			
		}

		// unlock

		vbuf->unlock();
		sm->indexData->indexBuffer->unlock();
	}

	// unlock 光照图

	d3dtex->getNormTexture()->UnlockRect(0); 

	// 把光照图加入进模型材质

	//d3dtex->getNormTexture()->GenerateMipSubLevels(); // MINMAPS

	if(manualEditSize <= 0.0f)
	{
		int submeshInd = 0;

		int NumEnts = e->getNumSubEntities();

		for(int i = 0; i < NumEnts; i ++)
		{
			COTESubEntity* se = e->getSubEntity(i);	

			Ogre::MaterialPtr mp = se->getMaterial();

			if(!mp->isLoaded())
				mp->touch();
		
			// 克隆材质

			if(mp->getName().find(e->getName()) == std::string::npos)
			{
				std::stringstream ss;
				ss << mp->getName() << e->getName() << submeshInd ++;			

				mp->clone(ss.str());

				mp = Ogre::MaterialManager::getSingleton().getByName(ss.str());

				se->setMaterialName(ss.str());			
			}		

			if(mp->getBestTechnique(0)->getPass(0)->getNumTextureUnitStates() == 1)
			{			
				mp->getBestTechnique(0)->getPass(0)->createTextureUnitState(lightmapTextName, 1);			
			}	

			mp->getBestTechnique(0)->getPass(0)->getTextureUnitState(1)->setTextureName(lightmapTextName);
			//mp->getBestTechnique(0)->getPass(0)->getTextureUnitState(1)->setColourOperationEx(
			//	LBX_MODULATE_X2, LBS_TEXTURE, LBS_CURRENT);	
			//mp->getBestTechnique(0)->getPass(0)->getTextureUnitState(0)->setColourOperationEx(
			//	LBX_MODULATE, LBS_TEXTURE, LBS_CURRENT);
			//mp->getBestTechnique(0)->getPass(0)->setVertexColourTracking(TVC_NONE);// 关闭顶点色

			// 第一层颜色一倍处理
			
			se->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(
					LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);

			// 第二层

			se->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(1)->setColourOperationEx(
					LBX_MODULATE_X2, LBS_TEXTURE, LBS_CURRENT);	
			
		}
	}

	return true;
}

// -----------------------------------------------
bool COTEEntityLightmapPainter::SaveLightmap()
{
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
	while(it != eList.end())
	{			
		OTE::COTEEntity* e = (OTE::COTEEntity*)(*it);	
		SaveLightmap(e);	
			
		++ it;
	}

	return true;
}

// -----------------------------------------------
#ifdef __OTEZH

static std::string ConvertString(const std::string& str)
{
	int  x, z, entInd;
	char buff[64];
	sscanf(str.c_str(), "entity.tile[%d,%d].%d.%s", &x, &z, &entInd, buff);	
	
	std::stringstream sSS;
	sSS << x << z << entInd << "LM.bmp";
	return sSS.str();
}

#endif

// -----------------------------------------------
bool COTEEntityLightmapPainter::SaveLightmap(COTEEntity* e)
{
	if(!e->CheckLightmapValid())
		return false;

	Ogre::String lightmapTextName = e->getName() + "_LM.bmp";

	Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().getByName(			
		lightmapTextName
		);
	if(texPtr.isNull())
	{
		return false;
	}

	// 把光照图保存下来

	Ogre::String lightmapPath = OTEHelper::GetOTESetting("ResConfig", "EntityLightmapFilePath");

	std::string secName = SCENE_MGR->GetUserData("SectorName");

	if(secName.empty())
		return false;

	Ogre::D3D9Texture* d3dtex = (Ogre::D3D9Texture*)texPtr.getPointer();
	d3dtex->writeContentsToFile(lightmapPath + secName + "_LM\\" + lightmapTextName);

#ifdef __OTEZH

	// 把图片输出到其它目录

	std::string entityName = lightmapTextName; 
	entityName.replace(entityName.find("Ent_"), 4, "");
	entityName.replace(entityName.find("_LM.bmp"), 7, "");	

	Ogre::String path = OTEHelper::GetOTESetting("ResConfig", "EntityLightmapExport");
	Ogre::String fullPath;
	OTEHelper::GetFullPath(path, fullPath);
	fullPath += secName;

	if(_mkdir(fullPath.c_str()) == ENOENT)
		return false;

	std::stringstream lmSS;
	lmSS << fullPath << "\\"  << ConvertString(entityName);

	d3dtex->writeContentsToFile(lmSS.str());

#endif

	return true;
}

// -------------------------------------------------------------------
void COTEEntityLightmapPainter::PickLightmapColor(COTEEntity* e,
													const Ogre::Ray&   ray,
													Ogre::ColourValue& rColor 
													)
{
	if(!e || e->getMesh().isNull())	
	{
		return;	  
	}	

	// 加载光照图（一个实体对应一张光照图 ） 

	Ogre::String lightmapTextName = e->getName() + "_LM.bmp";
	
	Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().getByName(lightmapTextName);

	if(texPtr.isNull())
	{	
		return;		
	}		
	
	Ogre::D3D9Texture* d3dtex = (Ogre::D3D9Texture*)texPtr.getPointer();
	D3DLOCKED_RECT rect; 
	HRESULT hr = d3dtex->getNormTexture()->LockRect( 0, &rect, NULL, 0 ); 

	if (FAILED(hr)) 
	{
		OTE_MSG("PickLightmapColor 时 tex->LockRect 失败!", "失败")
		return;
	} 

	// 位置旋转所放

	const Ogre::Vector3& position		= e->getParentNode()->_getDerivedPosition();
	const Ogre::Quaternion& orientation = e->getParentNode()->_getDerivedOrientation();
	const Ogre::Vector3& scale			= e->getParentNode()->_getDerivedScale();

	// 每个submesh计算

	Ogre::Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		Ogre::SubMesh * sm = smIt.getNext();

		// 从cach里查询
		
		SubMeshCach_t* smt = SubMeshCach_t::GetFromCach((int)sm);			

		if(smt == NULL)
		{
			smt = SubMeshCach_t::AddToCach((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
		}	
		
		const VertexElement* texCoodElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);
		if(!texCoodElem)
		{
			//OTE_MSG_ERR("生成光照图失败！ 模型不支持两层纹理! Model: " << e->getName())
			OTE_TRACE  ("生成光照图失败！ 模型不支持两层纹理! Model: " << e->getName())
				
			d3dtex->getNormTexture()->UnlockRect(0); 
			return;
		}

		uint16 *p16 = static_cast<uint16*>(sm->indexData->indexBuffer->lock(
			sm->indexData->indexStart, sm->indexData->indexCount, HardwareBuffer::HBL_READ_ONLY));

		const VertexElement* normElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
	
		HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(0);
		
		unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		float nearstDis = MAX_FLOAT;
		Tringle_t hitTvs;
		int index = -1;
		Ogre::Vector3 hitPos;
		for(int i = 0; i < smt->size; i ++)
		{		
			static Tringle_t tvs;
			tvs.TriVec[0] = (orientation * (smt->tringle[i].TriVec[0]* scale)) + position;                         			
			tvs.TriVec[1] = (orientation * (smt->tringle[i].TriVec[1]* scale)) + position;                         			
			tvs.TriVec[2] = (orientation * (smt->tringle[i].TriVec[2]* scale)) + position; 		
			
			{
				std::pair<bool, Ogre::Vector3> tResult = IntersectTrangle(ray,tvs.TriVec[0],tvs.TriVec[1],tvs.TriVec[2]);
				
				if(tResult.first)
				{
					float dis = (tResult.second - ray.getOrigin()).length();

					if(dis < nearstDis)
					{
						nearstDis = dis;
						index = i;
						hitPos = tResult.second;
						hitTvs = tvs;

					}
				}

			}
		}	

		if(index >= 0)
		{	
			pickColor(
				rColor,
				sm, p16, texCoodElem, normElem, vbuf->getVertexSize(), vertex,
				d3dtex, &rect,
				index, &hitTvs, orientation,
				ray,
				hitPos
				);
			
		}

		// unlock

		vbuf->unlock();
		sm->indexData->indexBuffer->unlock();
	}

	// unlock 光照图

	d3dtex->getNormTexture()->UnlockRect(0); 	

}

// -----------------------------------------------
// 应用接口
// -----------------------------------------------

bool COTEEntityLightmapPainter::GenLightmap(	
								COTEEntity* e,
								const Ogre::Ray&	 ray,
								const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,
								const Ogre::String&      lightmapTemplate,
								bool  bRenderShadow,
								COTEEntityLightmapPainter::GetLightInforHndler_t	 pLightInforHndler,
								float blend_alpha,
								float manualEditSize
								)
{
	// 清理光照图

	if(g_pLMDirtyCach)
	{
		memset(g_pLMDirtyCach, 0, c_max_lm_size * c_max_lm_size);
	}

	const static String c_lightmap_template  = "lightMapTemplate256.bmp";   // 默认光照图模版文件

	const static String c_lightmap_dir_helper = "entlightmapdirhelper.ote"; // 这个文件便于查找物件光照图的目录


	if(!e || e->getMesh().isNull())	
	{
		OTE_MSG("生成光照图失败！ 实体不存在 或者 Mesh资源不存在!", "失败")
		return false;	  
	}	
	
	// 自动生成图片(从模板图片拷贝而成)
	
	Ogre::String path, fullpath;
	OTEHelper::GetFullPath(lightmapTemplate.empty() ? c_lightmap_template : lightmapTemplate, path, fullpath);

	Ogre::String lightmapPath, fullLightmappath;
	OTEHelper::GetFullPath(c_lightmap_dir_helper, lightmapPath, fullLightmappath);

	std::string sceneName = SCENE_MGR->GetUserData("SectorName");
	
	if(sceneName.empty())
		return false;

	lightmapPath += "\\";

	sceneName += "_LM\\";

	lightmapPath += sceneName;

	Ogre::String lightmapTexName = e->getName() + "_LM.bmp";

	// OTE_TRACE((lightmapPath + lightmapTexName).c_str())

	if(!CopyFile(fullpath.c_str(), (lightmapPath + lightmapTexName).c_str(), true))
	{
		//OTE_MSG_ERR("拷贝光照图失败！ " << fullpath.c_str() << "->" << lightmapPath + lightmapTexName)
		//return false;
	}

	// 计算光照图
	
	if(!_GenLightmap(e, ray, diffuse, ambient, lightmapTexName, bRenderShadow, pLightInforHndler, blend_alpha, manualEditSize))
	{
		::DeleteFile((lightmapPath + lightmapTexName).c_str());
		return false;
	}
	
	return true; // 暂时不保存信息

	// 把光信息记录在文件里
	
	Ogre::String lightmapInfo = e->getName() + "_LM.txt";
	FILE* file = fopen((lightmapPath + lightmapInfo).c_str(), "w");
	if(!file)
	{
		OTE_MSG_ERR("打开文件失败！" << lightmapPath + lightmapInfo)
		return false;
	}

	const Ogre::Vector3& lightDir = ray.getDirection();

	fprintf(file, "实体： %s\n", e->getName().c_str());
	fprintf(file, "环境光： %.2f %.2f %.2f\n", ambient.r, ambient.g, ambient.b);
	fprintf(file, "漫反射： %.2f %.2f %.2f\n", diffuse.r, diffuse.g, diffuse.b);
	fprintf(file, "光的方位： %.2f %.2f %.2f\n", lightDir.x, lightDir.y, lightDir.z);

	fclose(file);

	return true;

}

// -----------------------------------------------
bool COTEEntityLightmapPainter::GenSceneLightmaps(
								const Ogre::Vector3&	 lightDir,
								const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 
								COTEEntityLightmapPainter::GetLightTemplateHndler_t pLMTempGenerHndler,
								const Ogre::String&		 hideEntityKeyName,
								bool  bRenderShadow,								
								COTEEntityLightmapPainter::GetLightInforHndler_t	 pLightInforHndler,
								float blend_alpha
								)
{
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);


	// 先搜索 所需的tga文件

	bool isTgaComplete = true;

	std::vector<Ogre::MovableObject*>::iterator it1 = eList.begin();
	while(it1 != eList.end())
	{
		OTE::COTEEntity* e = (OTE::COTEEntity*)(*it1);

		int NumEnts = e->getNumSubEntities();

		for(int i = 0; i < NumEnts; i ++)
		{
			COTESubEntity* se = e->getSubEntity(i);		

			Ogre::MaterialPtr mp = se->getMaterial();

			// 只检查双面贴图

			if(mp->getBestTechnique(0)->getPass(0)->getCullingMode() == CULL_NONE)
			{		
				Ogre::String texName = mp->getBestTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
				
				if(texName.find(".dds") != std::string::npos)
				{
					texName.replace(texName.find(".dds"), 4, ".tga");  
				}	

				if(!CHECK_RES(texName))
				{
					isTgaComplete = false;
					//OTE_MSG_ERR("缺少计算光照图所需要的tga! 贴图不存在: " << texName);
					OTE_LOGMSG("缺少计算光照图所需要的tga! : " << texName)
				}
			}
		}

		++ it1;
	}

	// 如果没有所需的tga 将停止

	if(!isTgaComplete)
	{
		OTE_MSG_ERR("缺少计算光照图所需要的tga! " << " 具体信息见: Ogre.log" );
		return false;
	}

	const static String c_lightmap_template = "lightMapTemplate256.bmp";		// 默认光照图模版文件
	
	// 物件列表中所有的物件 查询符合条件的计算

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

		std::string lightmapTemplate = c_lightmap_template;

		if(pLMTempGenerHndler)
			lightmapTemplate = (*pLMTempGenerHndler)(e->getName());

		GenLightmap(e, Ogre::Ray(Ogre::Vector3::ZERO, lightDir), diffuse, ambient, lightmapTemplate.empty() ? c_lightmap_template : lightmapTemplate, bRenderShadow, pLightInforHndler);	
			
		++ it;
	}

	return true;
}

// -----------------------------------------------
void COTEEntityLightmapPainter::ReLoadLightmapFromFile(COTEEntity* e)
{
	if(!e || e->getMesh().isNull())	
	{
		return;	  
	}	

	// 加载光照图（一个实体对应一张光照图 ） 

	Ogre::String lightmapTextName = e->getName() + "_LM.bmp";
	
	if(!CHECK_RES(lightmapTextName))
	{
		return;
	}

	Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().getByName(lightmapTextName);

	if(texPtr.isNull())
	{	
		return;		
	}		

	texPtr->reload();
}

// -----------------------------------------------
void COTEEntityLightmapPainter::ReLoadLightmapFromFile()
{
	// 物件列表中所有的物件 查询符合条件的计算

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
	while(it != eList.end())
	{	
		ReLoadLightmapFromFile((OTE::COTEEntity*)(*it));	
			
		++ it;
	}

}

// -----------------------------------------------
std::string COTEEntityLightmapPainter::GetLightMapName(COTEEntity* e)
{
	return e->getName() + "_LM.bmp";
}