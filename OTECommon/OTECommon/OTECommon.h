#pragma once
#include "OTEStaticInclude.h"

namespace OTE
{
// *******************************************
// CRectA
// *******************************************

template <class  T> 
class CRectA 
{
	public:
		T  sx,sz,ex,ez;

	public:
		CRectA()
		{}
		template <class  T>
		CRectA(T x1, T z1,T x2, T z2)
		{
			setRect<T>(x1,z1,x2, z2);			
		}
		~CRectA()
		{}
		//
		template <class  T>
		void setRect(T x1, T z1,T x2, T z2)
		{
			sx=x1,sz=z1,ex=x2,ez=z2;
		}

		template <class  T>			
		inline bool isInRect(const T& x,const T& z) const
		{
			if(x>=sx && z>=sz && x<ex && z<ez)
				return 1;
			else 
				return 0;
		}
		template <class  T>	
		void getCenter(T& cx,T& cz )
		{
           cx= (sx+ex)/2;
		   cz= (sz+ez)/2;
		}
		template <class  T>	
		T getCenterX()
		{
            return (sx+ex)/2;
		}
		template <class  T>	
		T getCenterZ()
		{
            return (sz+ez)/2;
		}
		template <class  T>	
		T getSize()
		{
            return ex-sx;
		}
		template <class  T>	
		T getHalfSize()
		{
            return (ex-sx)/2;
		}
		template <class  T>	
		T getWidth()
		{
            return ex-sx;
		}
		template <class  T>	
		T getHeight()
		{
            return ez-sz;
		}
		template <class  T>
		void operator=(const CRectA<T>& rect) 
		{
            sx=rect.sx,sz=rect.sz,ex=rect.ex,ez=rect.ez;
		}
		template <class  T>	
        bool operator==(const CRectA<T>& rect)  const
		{
            if(sx==rect.sx && sz==rect.sz && ex==rect.ex && ez==rect.ez)
				return 1;
			return 0;
		}
		template <class  T>	
        CRectA<T> operator*(const T factor)
		{
            CRectA<T> ret;
            ret.sx=sx*factor,ret.sz=sz*factor;
			ret.ex=ex*factor,ret.ez=ez*factor;
			return ret;
		}
	
		Ogre::Vector3 getStartVector3()
		{
			return Vector3(sx,0,sz);
        }
			
		Ogre::Vector3 getEndVector3()
		{
			return Vector3(ex,0,ez);
        }
		template <class  T>	
		T getLeft()
		{
            return sx;
		}
		template <class  T>	
		T getTop()
		{
            return sz;
		}
		template <class  T>	
		T getRight()
		{
            return ex;
		}
		template <class  T>	
		T getBottom()
		{
            return ez;
		}
};


// -----------------------------------------------------------------------
// 常用函数
// -----------------------------------------------------------------------
struct OTEHelper
{		
	// -----------------------------------------------------------------------
	static bool IsZero(float v)
	{
		return abs(v) < 0.00001f;
	}
	// -----------------------------------------------------------------------
	static bool IsEqual(float v1, float v2)
	{
		return abs(v1 - v2) < 0.00001f;
	}
	// -----------------------------------------------------------------------
	static char* Str(int v)
	{
		static char buff[32];
		sprintf(buff, "%d", v);
		return buff;
	}

	// -----------------------------------------------------------------------
	static char* Str(float v)
	{
		static char buff[32];
		sprintf(buff, "%.2f", v);
		return buff;
	}

	// -----------------------------------------------------------------------
	// 得到资源路径

	static const char* GetResPath(const char* GroupName, const char* PathName, 
											const char* CfgName = ".\\OTESettings.cfg")
	{
		static char path[256];
		GetPrivateProfileString(GroupName, PathName, "\0", path, sizeof(path), CfgName);
		return path;
	}

	// -----------------------------------------------------------------------
	// 组织资源文件名称

	static const char* MakeResPath(const char* GroupName, const char* PathName, const char* FileName,
																const char* CfgName = ".\\OTESettings.cfg")
	{
		static char path[256];
		GetPrivateProfileString(GroupName, PathName, "\0", path, sizeof(path), CfgName);
		strcat(path, FileName);

		return path;
	}

	// -----------------------------------------------------------------------
	static bool SaveRawFile(const std::string& fileName, unsigned char* pData, unsigned int size)
	{
		FILE* file = fopen(fileName.c_str(), "wb");
		if(!file || !pData)
		{
			return false;
		}

		fwrite( pData, size, 1, file );	
		fclose(file);

		return true;
	}

	// -----------------------------------------------------------------------
	static bool SaveTgaFile(const std::string& fileName, unsigned char* pData, 
									unsigned int width, unsigned int height, unsigned int depth)
	{
		if(depth != 24 && depth != 32) // 现在只支持２４，３２位
			return false;		

		//// 由于数据按照正序 (内存地址从小到大) 要转换为图片数据

		unsigned int pixelSize = depth / 8;
		unsigned int size = width * height;
		unsigned char* pSrc = pData + (size - width) * pixelSize;		

		unsigned char* pDestTmpData = new unsigned char[size * pixelSize];

		unsigned char* pDest = pDestTmpData;
		
		for(unsigned int i = 0; i < height; i ++)
		{
			for(unsigned int j = 0;	j < width; j ++)
			{	
				pSrc += pixelSize;
				for(int k = 0; k < int(pixelSize); ++ k)
					*pDest ++ = *pSrc --;
				pSrc += pixelSize;
			}	
			pSrc -= 2 * width * pixelSize;
		}

		FILE* file = fopen(fileName.c_str(), "wb");
		if(!file || !pData)
		{
			return false;
		}

		unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};   
		fwrite(TGAheader,	sizeof(TGAheader), 1, file);

        unsigned char	header[6];	
        header[1]	=	width  / 256;
		header[0]	=	width  % 256;
        header[3]	=	height / 256;
		header[2]	=	height % 256;
		header[4]	=	depth;
 		header[5]	=	0;

        fwrite(header,	sizeof(header), 1, file);

		fwrite(pDestTmpData,	width * height * depth / 8, 1, file);
	
		fclose(file);

		delete [] pDestTmpData;

		return true;	

	}	

	// -----------------------------------------------------------------------
	static bool SaveToBmp(const std::string& fileName, const unsigned char* pData, 
									unsigned int width, unsigned int height, unsigned int depth)
	{
		FILE* file = fopen(fileName.c_str(), "wb");
		if(!file || !pData)
		{
			OTE_MSG_ERR("SaveToBmp 失败！")
			return false;
		}

		// File header
		BITMAPFILEHEADER FileHeader;
		WORD sign = ((WORD) ('M' << 8) | 'B');
		FileHeader.bfType = sign;
		FileHeader.bfSize = 14 + 40 + width * height * depth / 8; 
		FileHeader.bfReserved1 = 0; 
		FileHeader.bfReserved2 = 0; 
		FileHeader.bfOffBits = 54; 
		
		fwrite(&FileHeader,sizeof(BITMAPFILEHEADER), 1, file);

		BITMAPINFOHEADER Header;

		Header.biWidth = width;
		Header.biHeight = height;
		Header.biSizeImage = width * height * depth / 8;

		Header.biSize = 40;
		Header.biPlanes = 1;
		Header.biBitCount = depth;
		Header.biCompression = 0;
		Header.biXPelsPerMeter = 0;
		Header.biYPelsPerMeter = 0;
		Header.biClrUsed = 0;
		Header.biClrImportant = 0;

		fwrite(&Header,sizeof(BITMAPINFOHEADER), 1, file);	
	
		fwrite(pData, Header.biSizeImage, 1, file);

		fclose(file);

		return true;
	}

	// -----------------------------------------------------------------------
	// 得到当前程序运行目录

	static bool GetAppDirection(Ogre::String& path)
	{
		char fullpath[MAX_PATH] = {0};
		::GetModuleFileName( NULL, fullpath, MAX_PATH );
		
		std::string _fullPath = fullpath;

		int index = _fullPath.rfind ( '\\' );
		if ( index != std::string::npos )
		{
			path = _fullPath.substr( 0, index );
			return true;
		}
		else
			return false;
	}

	// -----------------------------------------------------------------------
	// 通过文件名得到文件的全目录

	static bool GetFullPath(const Ogre::String& fileName, Ogre::String& path, Ogre::String& fullPath)
	{
		/*Ogre::FileInfoList fileInfoList;
		Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(
											Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
											fileName,
											fileInfoList
											);
		if(fileInfoList.size() == 0)
			return false;

		Ogre::FileInfo& fi = fileInfoList[0];

		path = fi.archive->getName();
		fullPathName = Ogre::String(fi.archive->getName() + "\\") + fileName;		

		fileInfoList.clear();*/		

		if(!GetAppDirection(path))
			return false;

		path += "\\";
		path += Ogre::ResourceGroupManager::getSingleton().getResourceLocation(
											Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
											fileName);
		fullPath = Ogre::String(path + "\\") + fileName;

		return true;
	}

	// -----------------------------------------------------------------------
	// 程序目录

	static bool GetFullPath(Ogre::String& path, Ogre::String& fullPath)
	{
		if(!GetAppDirection(fullPath))
			return false;

		fullPath += "\\";
		fullPath = Ogre::String(fullPath + "\\") + path;

		return true;
	}
	// -----------------------------------------------------------------------
	// string -> number

	static int HashString(const Ogre::String& str)
	{
		// from q3

		unsigned int register hash, i;

		hash = 0;
		for (i = 0; i < str.length() && str.c_str()[i] != '\0'; i++) {
			hash += str.c_str()[i] * (119 + i);
		}
		hash = (hash ^ (hash >> 10) ^ (hash >> 20));		

		return hash;
	}

	// -----------------------------------------------------------------------
	// 得到OTESetting.cfg里的设置值

	static const char* GetOTESetting(const char* GroupName, const char* PathName, 
											const char* CfgName = ".\\OTESettings.cfg")
	{
		static char path[256];
		GetPrivateProfileString(GroupName, PathName, "\0", path, sizeof(path), CfgName);
		return path;
	}

	// -----------------------------------------------------------------------
	// 分析字符串函数

	static std::string		ScanfStringAt(const std::string& str, int pos, char flag)
	{		
		std::string sub_str;			
		int count = 1;
		std::stringstream ss(str);
		while(std::getline(ss,sub_str,flag))
		{
			if(count == pos)					
				return sub_str;							

			count ++;
		}	
		return "";
	}

	// -----------------------------------------
	static std::string		ScanfStringAt(const std::string& str, int pos, char flag1, char flag2)
	{			
		int count = 1;
		std::string sub_str;	
		std::stringstream ss(str);
		
		while(std::getline(ss,sub_str,flag1))
		{
			if(count == pos)
			{
				std::string sub_str1;
				std::stringstream ss1(sub_str);
				while(std::getline(ss1,sub_str1,flag2))
				{
					return sub_str1;
				}									
			}		
			count ++;
		}

		return "";
	}
	
};


}


