//********************************************
// Texture.h
//********************************************
// class CTexture
//********************************************
// This object stores a bitmap image used as
// an OpenGL texture.
// Files are stored as .bmp format. 
// Depth are currently 24 or 32 bits,
// Modes are : RGB, RGBA (alpha layer).
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 17/12/97
// Modified : 18/02/98
//********************************************

#ifndef _TEXTURE_
#define _TEXTURE_

class CTexture : public CObject
{

// Members datas
private :

	unsigned char *m_pData;    // datas
	unsigned int   m_Width;    // width (pixels)
	unsigned int   m_Height;   // height (pixels)
	unsigned int   m_Depth;    // bits per pixel 
	CString        m_FileName; // texture image file name

	BITMAPINFOHEADER m_Header;      // image header (display on device context)
	unsigned int     m_WidthByte32; // width (in bytes, and 32 bits aligned)

public :	
	// Construction / destruction
	CTexture();
	virtual ~CTexture();

	// File reading
	int ReadFileTGA(const char* filename);
	int ReadFile(const char *filename,unsigned int width=-1,unsigned int height=-1,unsigned int depth=-1);
	int ReadFileBMP(const char *filename);
	int ReadFileRAW(const char *filename,unsigned int width,unsigned int height,unsigned int depth);

	// File saving
	int SaveFile(const char *filename);
	int SaveFileBMP(const char *filename);
	int SaveFileRAW(const char *filename);
	bool SaveAsTga(const char *filename);
	// Datas (explicit inline functions)
	unsigned char *GetData(void) { return m_pData; }
	unsigned int GetWidth(void)  { return m_Width; }
	unsigned int GetHeight(void) { return m_Height;}
	unsigned int GetDepth(void)  { return m_Depth; }
	CString GetFileName(void)    { return m_FileName; }

	// Misc
	int IsValid();
	int SameSize(CTexture *pTexture);
	int BGRtoRGB(void);
	int HigherPowerOfTwo(int value);
	int LowerPowerOfTwo(int value);
	 
	// Updating
	void UpdateWidthByte32();
	void UpdateHeader();
	unsigned int WidthByte32(unsigned int width,unsigned int depth);

	// Alpha
	int HasAlpha() { return (m_Depth == 32); }
	int AddAlphaLayer(unsigned char alpha);
	int SetAlphaLayer(unsigned char alpha);
	int DeleteAlpha();
	int PutAlpha(CTexture *pTexture);	// Put an alpha layer from grey scale
	bool SetAlphaByColIndex(unsigned char color[],unsigned char alpha);

	// DuplicateMirror
	int DuplicateMirror(int left=0,int top=0,int right=-1,int bottom=-1);
	int DuplicateRepeatWidth(int left=0,int top=0,int right=-1,int bottom=-1);
	int Extract(int left=0,int top=0,int right=-1,int bottom=-1);

	// Display
	int Draw(CDC *pDC,
		 int xOffset=0, int yOffset=0, 
		 int srcWidth=-1, int srcHeight=-1, 
		 int xSrcOffset = 0, int ySrcOffset = 0);

	// Buffer
	int ReadBuffer(unsigned char *buffer, int width, int height, int depth);
	int Grey(unsigned int x,unsigned int y);

	void Free(void);

private :

	// Memory
	int Alloc(unsigned int width,unsigned int height,unsigned int depth);	

};

#endif // _TEXTURE_
