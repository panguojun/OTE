/************************************************************
 ************************************************************

 ģ�����ƣ� ʵ�����ͼ����ʵ��
 ���ģ�飺 ʵ���� COTEEntity, 
            ��ײ��� COTECollisionManager, 
					 COTEEntityPhysicsMgr 
						
 ����    �� ����Ϲ���ͼ������
            ͨ�������ε�uv ����һ��2ά�����Σ�Ȼ��������ؽ��в�ֵ
			��������άλ�ã����䷢�ߣ�Ȼ�������ײ���ԡ�

            ʵ�ֵ��Ѷ���Ҫ�Ǽ��㾫�ȣ�����༭�����ϡ�
            ���ǵķ����Ǳ߽��ϵ�����ֻ����һ�ߣ������䴦���ĸ������Σ�
			����������ͼͨ���������ηŴ�ķ���ʹ�������ܹ�����������
			�ڲ���ֵ��������άλ�á�

			����ͼΪbmp��ʽ��ʹ��ʱ��ģ�濽��������
			ע�⣺ģ�����ͼ����Ϊ��ɫ͸����

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
// ��ѧ����
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
// Ҫ��֤ a b c ��ʱ��˳�� 
static bool  CheckPointIn2DTriangle_LineLogic(
				const Ogre::Vector2& point, const Ogre::Vector2& a, const Ogre::Vector2& b, const Ogre::Vector2& c,
				float tolerance = 0.00001f) 
{		
	static struct Line_Dita_t	{float A;float B;float C;float RR;} Line_Dita[3];

	// ��֤��ʱ��˳�� 

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

// -----------------------------------------------------------------
// Gouraud Shading lerp
// http://medialab.di.unipi.it/web/IUM/Waterloo/node84.html
// �����β�ֵ

inline float _CROSS(const Ogre::Vector2&v1, const Ogre::Vector2&v2)	{ return (v1).x * (v2).y - (v2).x * (v1).y; }

static void Lerp(
	const Ogre::Vector2& a,  const Ogre::Vector2& b,  const Ogre::Vector2& c,	const Ogre::Vector2& p,
	const Ogre::Vector3& va, const Ogre::Vector3& vb, const Ogre::Vector3& vc,	
	Ogre::Vector3& vp, int scaleDepth = 2)
{
	// �������ǲ������Ϊ��

	float DABC = _CROSS((b - a), (c - a));
	if(DABC == 0.0f)
	{
		vp = va;
		return;
	}

	// �ȼ��p�Ƿ����������� ����� ��ô�������������

	if(scaleDepth > 0 && !CheckPointIn2DTriangle(p, a, b, c, 0.000001f))
	{
		Ogre::Vector2 center = (a + b + c) / 3.0f;
		float scl = 1.1f; // �Ŵ�ϵ��
		Ogre::Vector2 _a = (a - center) * scl + center;
		Ogre::Vector2 _b = (b - center) * scl + center;
		Ogre::Vector2 _c = (c - center) * scl + center;

		// �ݹ�

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
// �����߶ηָ���� �����Բ�ֵ

static void Plane_Lerp(
	const Ogre::Vector2& a,  const Ogre::Vector2& b,  const Ogre::Vector2& c,	const Ogre::Vector2& p,
	const Ogre::Vector3& va, const Ogre::Vector3& vb, const Ogre::Vector3& vc,	
	Ogre::Vector3& vp, int scaleDepth = 2)
{
	// ���������Ϊ��

	float DABC = _CROSS((b - a), (c - a));
	if(DABC == 0.0f)
	{
		vp = va;
		return;
	}

	// �ȼ��p�Ƿ����������� ����� ��ô�������������

	if(scaleDepth > 0 && !CheckPointIn2DTriangle(p, a, b, c, 0.00001f))
	{
		Ogre::Vector2 center = (a + b + c) / 3.0f;
		float scl = 1.1f; // �Ŵ�ϵ��
		Ogre::Vector2 _a = (a - center) * scl + center;
		Ogre::Vector2 _b = (b - center) * scl + center;
		Ogre::Vector2 _c = (c - center) * scl + center;

		// �ݹ�

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
			
		if(k2 == k1) // ƽ�е���� �ԼӴ���		
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
// �����ͼ

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

const int			c_max_lm_size	= 2048; // ע�� ����ͼ�ߴ粻����������ߴ�

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
			OTE_MSG("paintTexture �� tex->LockRect ʧ��!", "ʧ��")
			return;
		} 	

		rect = &_rect;
	}	 
		
	// ���	
	// ��ɫֵΪargb��ʽ

	DWORD* dst = (DWORD*)rect->pBits; 

	dst += yOffset * rect->Pitch / sizeof(DWORD);	

	int xOffsetEnd = (xOffset + paintSize) % tex->getWidth();
	int yOffsetEnd = (yOffset + paintSize) % tex->getHeight();

	paintColor |= 0xFF000000;	// ��͸��

	for (int i = yOffset; i < yOffsetEnd; ++ i) 
	{ 
		for (int j = xOffset; j < xOffsetEnd; ++ j) 
		{ 
			if(!failPainted || !(dst[j] & 0x00FFFFFF)) // ԭʼ����ͼΪ��ɫ
			{
				// �滻

				if(abs(alpha - 1.0f) < 0.001f)
				{
					dst[j] = paintColor; 
				}

				/// ע���±����ַ��� ���ڹ���ͼ��һ�������ϵ��ظ�������ɵ����� ��ʱ���ܴﵽ�����Ч������
				
				// ����

				else if(alpha > 1.0f)
				{
					if(g_pLMDirtyCach[i * c_max_lm_size + j] == 0) // ����Ƿ��ظ�����
					{
						Ogre::ColourValue srcColor;
						srcColor.r = GetRVal(paintColor) / 255.0f; 
						srcColor.g = GetGVal(paintColor) / 255.0f; 
						srcColor.b = GetBVal(paintColor) / 255.0f; 
							
						Ogre::ColourValue desColor;
						desColor.r = GetRVal(dst[j]    ) / 255.0f; 
						desColor.g = GetGVal(dst[j]    ) / 255.0f; 
						desColor.b = GetBVal(dst[j]    ) / 255.0f; 

						desColor = srcColor + desColor; // ����

						desColor.r > 1.0f ? desColor.r = 1.0f : NULL;
						desColor.g > 1.0f ? desColor.g = 1.0f : NULL;
						desColor.b > 1.0f ? desColor.b = 1.0f : NULL;
						
						desColor.a = 1.0f;	// ��͸��

						dst[j] = desColor.getAsARGB();	

						g_pLMDirtyCach[i * c_max_lm_size + j] = 1;
					}
				}

				// ���

				else
				{		
					if(g_pLMDirtyCach[i * c_max_lm_size + j] == 0) // ����Ƿ��ظ�����
					{
						Ogre::ColourValue srcColor;
						srcColor.r = GetRVal(paintColor) / 255.0f; 
						srcColor.g = GetGVal(paintColor) / 255.0f; 
						srcColor.b = GetBVal(paintColor) / 255.0f; 
							
						Ogre::ColourValue desColor;
						desColor.r = GetRVal(dst[j]    ) / 255.0f; 
						desColor.g = GetGVal(dst[j]    ) / 255.0f; 
						desColor.b = GetBVal(dst[j]    ) / 255.0f; 

						desColor   = srcColor * alpha + desColor * (1.0f - alpha); // ���
						
						desColor.a = 1.0f;	// ��͸��

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
			OTE_MSG("paintTexture �� tex->LockRect ʧ��!", "ʧ��")
			return;
		} 	

		rect = &_rect;
	}			
	
	// ��ɫֵΪargb��ʽ

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
// ��ȡ��ɫ	

static void pickColor(    Ogre::ColourValue& rColor,
					      const SubMesh *sm, const uint16 *p16, const VertexElement* texCoodElem, const VertexElement* normElem, int vertexSize, unsigned char* vertex,
						  Ogre::D3D9Texture* tex, const D3DLOCKED_RECT* rect,
						  size_t triangleIndex, const Tringle_t* tri, const Ogre::Quaternion& orientation, 
						  const Ogre::Ray&	_ray,
						  const Ogre::Vector3& rHitPos
						)
{
	assert((sm && triangleIndex != -1) && "paintTriangle ������Ч");

	// ��ȡuv ����

	float Coord[3][2];
	Ogre::Vector3 normal[3];

	// ��ѯ��������
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

	// �Ƿ����������ڲ�
	
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
// ���һ��������	

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

	assert((sm && triangleIndex != -1) && "paintTriangle ������Ч");

	// ��ȡuv ����

	float Coord[3][2];
	Ogre::Vector3 normal[3];
		
	// ��ѯ��������
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
	/// ��������ε��㷨
	///////////////////////////////////////////	

	// ����

	float pixelUnitSizeX = 1.0f / tex->getWidth();
	float pixelUnitSizeZ = 1.0f / tex->getHeight();

	// ����

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

	// �Ѿ�������һ��

	minX = minX <= pixelUnitSizeX		? 0.0f	: minX - pixelUnitSizeX;
	minZ = minZ <= pixelUnitSizeZ		? 0.0f	: minZ - pixelUnitSizeZ;
	maxX = maxX + pixelUnitSizeX > 1.0f ? 1.0f	: maxX + pixelUnitSizeX;
	maxZ = maxZ + pixelUnitSizeZ > 1.0f ? 1.0f	: maxZ + pixelUnitSizeZ;

	// �Ƿ����������ڲ�
	
	Ogre::Vector2 a(Coord[0][0], Coord[0][1]);
	Ogre::Vector2 b(Coord[1][0], Coord[1][1]);
	Ogre::Vector2 c(Coord[2][0], Coord[2][1]);

	bool isAnticlockwise = (a.x - b.x) * (a.y - c.y) - (a.x - c.x) * (a.y - b.y) > 0.0f; // ����˳��
	bool isInTri;	

	// ������ֹ��༭

	if(manualEditSize > 0.0f)
	{
		Ogre::Vector3 nt = (normal[0] + normal[1] + normal[2]) / 3.0f; 

		if(nt.dotProduct(_ray.getDirection()) < - 0.5) // ֻ������
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
		Ogre::Vector2 p(i, j); 	// uvλ��		

		if(isAnticlockwise)			
			isInTri = CheckPointIn2DTriangle_LineLogic(p, a, c, b, pixelUnitSizeZ * 2.0f);
		else	
			isInTri = CheckPointIn2DTriangle_LineLogic(p, a, b, c, pixelUnitSizeZ * 2.0f);				
		
		if(isInTri)
		{
			uint xOffset = p.x * tex->getWidth() ;
			uint yOffset = p.y * tex->getHeight();

			// �Է������Բ�ֵ

			static Ogre::Vector3 nt;
			Lerp(a, b, c, p, 
				normal[0], normal[1], normal[2], 
				nt);
			// nt = (normal[0] + normal[1] + normal[2]) / 3.0f;	

			static Ogre::ColourValue paintColour;

			// ������ײ		
			
			static Ogre::Vector3 hitPos;
						
			// ��λ�����Բ�ֵ				

			static Ogre::Vector3 point3d;

			Plane_Lerp(a, b, c, p, 
				tri->TriVec[0], tri->TriVec[1], tri->TriVec[2], 
				point3d, 2);			
				
			bool isIn3DTri = false;

			if(CheckPointInTriangle(point3d, tri->TriVec[0], tri->TriVec[1], tri->TriVec[2], 0.00001f))
			{
				isIn3DTri = true;
			}	

			if(pLightInforHndler) // �ر�Ĺ�Դ������Դ
			{
				Ogre::Vector3		_position;
				Ogre::Vector3		_direction = lightDir;
				Ogre::ColourValue	_diffuse   = diffuse;
				float				reflectDis;
				float				reflectDis0;

				(*pLightInforHndler)(point3d, _position, _direction, _diffuse, reflectDis, reflectDis0, blend_alpha);			
				
				Ogre::Ray ray(point3d + nt * 0.40f /*�����ϵ�Сƫ��*/, - _direction);
				
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
					//bFaceCulling = true; //  ��ʱ�ر�
					float factor = bFaceCulling ? - _direction.dotProduct(nt) : abs(lightDir.dotProduct(nt));		// ���ڷ����ڹ�Դ�ļнǶ�����Ч��		
					factor > 1.0f ? factor = 1.0f : NULL;
					factor < 0.0f ? factor = 0.0f : NULL;
					paintColour = _diffuse * factor + ambient;	// ���������µ���ɫ
				}

			}
			else
			{
				Ogre::Ray ray(point3d + nt * 0.40f /*�����ϵ�Сƫ��*/, - lightDir);

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
					//bFaceCulling = true; //  ��ʱ�ر�
					float factor = bFaceCulling ?  - lightDir.dotProduct(nt) : abs(lightDir.dotProduct(nt));		// ���ڷ����ڹ�Դ�ļнǶ�����Ч��		
					factor > 1.0f ? factor = 1.0f : NULL;
					factor < 0.0f ? factor = 0.0f : NULL;
					paintColour = diffuse * factor + ambient;		// ���������µ���ɫ
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
		//OTE_MSG_ERR("���ɹ���ͼʧ�ܣ� ʵ�岻���� ���� Mesh��Դ������!")
		return false;	  
	}

	const Ogre::Vector3& lightDir = ray.getDirection();

	// ���ع���ͼ��һ��ʵ���Ӧһ�Ź���ͼ �� 	
	
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
			OTE_MSG("GenLightmap ʱ ����ͼû���ҵ�! " << lightmapTextName, "ʧ��")
			return false;
		}

		// ���ع���ͼ

		texPtr = TextureManager::getSingleton().load(
					lightmapTextName,
					ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					TEX_TYPE_2D, 0);
		
	}		
	
	Ogre::D3D9Texture* d3dtex = (Ogre::D3D9Texture*)texPtr.getPointer();

	// ������ͼ�ߴ��Ƿ����

	if(	d3dtex->getWidth () > c_max_lm_size ||
		d3dtex->getHeight() > c_max_lm_size	 )
	{
		OTE_MSG_ERR("����ͼ�ߴ���� ���ߴ���� < " << c_max_lm_size)
		return false;
	}

	D3DLOCKED_RECT rect; 
	HRESULT hr = d3dtex->getNormTexture()->LockRect( 0, &rect, NULL, 0 ); 

	if (FAILED(hr)) 
	{
		OTE_MSG("GenLightmap ʱ tex->LockRect ʧ��!", "ʧ��")
		return false;
	} 

	// ���� ����ͼ��ɫ 

	if(manualEditSize <= 0)
		paintTexture(0, 0, d3dtex->getWidth(), 0, &rect, d3dtex, true);

	// λ����ת����

	const Ogre::Vector3& position		= e->getParentNode()->_getDerivedPosition();
	const Ogre::Quaternion& orientation = e->getParentNode()->_getDerivedOrientation();
	const Ogre::Vector3& scale			= e->getParentNode()->_getDerivedScale();

	// ÿ��submesh����

	Ogre::Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		Ogre::SubMesh * sm = smIt.getNext();

		// ��cach���ѯ
		
		SubMeshCach_t* smt = SubMeshCach_t::GetFromCach((int)sm);			

		if(smt == NULL)
		{
			smt = SubMeshCach_t::AddToCach((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
		}	
		
		const VertexElement* texCoodElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);
		if(!texCoodElem)
		{
			//OTE_MSG_ERR("���ɹ���ͼʧ�ܣ� ģ�Ͳ�֧����������! Model: " << e->getName())
			OTE_TRACE  ("���ɹ���ͼʧ�ܣ� ģ�Ͳ�֧����������! Model: " << e->getName())
				
			d3dtex->getNormTexture()->UnlockRect(0); 
			return false;
		}

		uint16 *p16 = static_cast<uint16*>(sm->indexData->indexBuffer->lock(
			sm->indexData->indexStart, sm->indexData->indexCount, HardwareBuffer::HBL_READ_ONLY));

		const VertexElement* normElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
	
		HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(0);
		
		unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		// ����Ƿ�˫��
		
		bool isFaceCull = true;
		if(sm->getMaterialName().find("Scene_cull") != std::string::npos)
		{
			std::string meshName = e->getMesh()->getName();
			std::string path, fullpath;
			OTEHelper::GetFullPath(meshName, path, fullpath);
			if(fullpath.find("��ħֲ��") != std::string::npos)
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
				// ���һ��������

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
			else // �ֹ��༭
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

		// �ֹ��༭

		if(manualEditSize > 0.0f && index >= 0)
		{	
			// ���һ��������

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

	// unlock ����ͼ

	d3dtex->getNormTexture()->UnlockRect(0); 

	// �ѹ���ͼ�����ģ�Ͳ���

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
		
			// ��¡����

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
			//mp->getBestTechnique(0)->getPass(0)->setVertexColourTracking(TVC_NONE);// �رն���ɫ

			// ��һ����ɫһ������
			
			se->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(
					LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);

			// �ڶ���

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

	// �ѹ���ͼ��������

	Ogre::String lightmapPath = OTEHelper::GetOTESetting("ResConfig", "EntityLightmapFilePath");

	std::string secName = SCENE_MGR->GetUserData("SectorName");

	if(secName.empty())
		return false;

	Ogre::D3D9Texture* d3dtex = (Ogre::D3D9Texture*)texPtr.getPointer();
	d3dtex->writeContentsToFile(lightmapPath + secName + "_LM\\" + lightmapTextName);

#ifdef __OTEZH

	// ��ͼƬ���������Ŀ¼

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

	// ���ع���ͼ��һ��ʵ���Ӧһ�Ź���ͼ �� 

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
		OTE_MSG("PickLightmapColor ʱ tex->LockRect ʧ��!", "ʧ��")
		return;
	} 

	// λ����ת����

	const Ogre::Vector3& position		= e->getParentNode()->_getDerivedPosition();
	const Ogre::Quaternion& orientation = e->getParentNode()->_getDerivedOrientation();
	const Ogre::Vector3& scale			= e->getParentNode()->_getDerivedScale();

	// ÿ��submesh����

	Ogre::Mesh::SubMeshIterator smIt = e->getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		Ogre::SubMesh * sm = smIt.getNext();

		// ��cach���ѯ
		
		SubMeshCach_t* smt = SubMeshCach_t::GetFromCach((int)sm);			

		if(smt == NULL)
		{
			smt = SubMeshCach_t::AddToCach((int)sm, sm->indexData->indexCount / 3);					
			SubMeshCach_t::SetDataToSubMeshCach(smt, sm->indexData, sm->vertexData);
		}	
		
		const VertexElement* texCoodElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);
		if(!texCoodElem)
		{
			//OTE_MSG_ERR("���ɹ���ͼʧ�ܣ� ģ�Ͳ�֧����������! Model: " << e->getName())
			OTE_TRACE  ("���ɹ���ͼʧ�ܣ� ģ�Ͳ�֧����������! Model: " << e->getName())
				
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

	// unlock ����ͼ

	d3dtex->getNormTexture()->UnlockRect(0); 	

}

// -----------------------------------------------
// Ӧ�ýӿ�
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
	// �������ͼ

	if(g_pLMDirtyCach)
	{
		memset(g_pLMDirtyCach, 0, c_max_lm_size * c_max_lm_size);
	}

	const static String c_lightmap_template  = "lightMapTemplate256.bmp";   // Ĭ�Ϲ���ͼģ���ļ�

	const static String c_lightmap_dir_helper = "entlightmapdirhelper.ote"; // ����ļ����ڲ����������ͼ��Ŀ¼


	if(!e || e->getMesh().isNull())	
	{
		OTE_MSG("���ɹ���ͼʧ�ܣ� ʵ�岻���� ���� Mesh��Դ������!", "ʧ��")
		return false;	  
	}	
	
	// �Զ�����ͼƬ(��ģ��ͼƬ��������)
	
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
		//OTE_MSG_ERR("��������ͼʧ�ܣ� " << fullpath.c_str() << "->" << lightmapPath + lightmapTexName)
		//return false;
	}

	// �������ͼ
	
	if(!_GenLightmap(e, ray, diffuse, ambient, lightmapTexName, bRenderShadow, pLightInforHndler, blend_alpha, manualEditSize))
	{
		::DeleteFile((lightmapPath + lightmapTexName).c_str());
		return false;
	}
	
	return true; // ��ʱ��������Ϣ

	// �ѹ���Ϣ��¼���ļ���
	
	Ogre::String lightmapInfo = e->getName() + "_LM.txt";
	FILE* file = fopen((lightmapPath + lightmapInfo).c_str(), "w");
	if(!file)
	{
		OTE_MSG_ERR("���ļ�ʧ�ܣ�" << lightmapPath + lightmapInfo)
		return false;
	}

	const Ogre::Vector3& lightDir = ray.getDirection();

	fprintf(file, "ʵ�壺 %s\n", e->getName().c_str());
	fprintf(file, "�����⣺ %.2f %.2f %.2f\n", ambient.r, ambient.g, ambient.b);
	fprintf(file, "�����䣺 %.2f %.2f %.2f\n", diffuse.r, diffuse.g, diffuse.b);
	fprintf(file, "��ķ�λ�� %.2f %.2f %.2f\n", lightDir.x, lightDir.y, lightDir.z);

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


	// ������ �����tga�ļ�

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

			// ֻ���˫����ͼ

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
					//OTE_MSG_ERR("ȱ�ټ������ͼ����Ҫ��tga! ��ͼ������: " << texName);
					OTE_LOGMSG("ȱ�ټ������ͼ����Ҫ��tga! : " << texName)
				}
			}
		}

		++ it1;
	}

	// ���û�������tga ��ֹͣ

	if(!isTgaComplete)
	{
		OTE_MSG_ERR("ȱ�ټ������ͼ����Ҫ��tga! " << " ������Ϣ��: Ogre.log" );
		return false;
	}

	const static String c_lightmap_template = "lightMapTemplate256.bmp";		// Ĭ�Ϲ���ͼģ���ļ�
	
	// ����б������е���� ��ѯ���������ļ���

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

	// ���ع���ͼ��һ��ʵ���Ӧһ�Ź���ͼ �� 

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
	// ����б������е���� ��ѯ���������ļ���

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