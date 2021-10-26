#include "PathTool.h"
#include "tinyxml.h"
#include <direct.h>
#include <windows.h>
//·��ת��
void ComToAbs(char *tPath, const char *path)
{
	if(strstr(path, ":\\"))
	{		
		strcpy(tPath, path);
	}
	else
	{
		char filename[64];
		GetFileFromPath(path, filename);

		char curDir[256];
		getcwd(curDir, 256);	
		
		char s[256];
		GetPrivateProfileString("ResConfig","ResPath","\0",
									s,sizeof(s),".\\ResPath.cfg");
		strcat(s, "\\");
		strcat(s, path);
		GetFilePath(s, s);
		//�򵥷���תΪ����·��:)
		chdir(s);
		getcwd(s, 256);
		chdir(curDir);
		strcpy(tPath, s);
		strcat(tPath, "\\");
		strcat(tPath, filename);	
	}
}

void AbsToCom(char *tPath, const char *path)
{
	char Buff[64];
	GetPrivateProfileString("ResConfig","ResFloderName","\0",
								Buff,sizeof(Buff),".\\ResPath.cfg");

	char *pdest = strstr(path, Buff);

	if( pdest != NULL )
		strcpy(tPath, pdest + strlen(Buff) + 1);
	else
		strcpy(tPath, path);
}

//�ļ���ת����ȥ��δβ��.xml
Ogre::String ReplaceName(const Ogre::String OldName)
{
	if(OldName.find(".xml") != -1)
	{
		return OldName.substr(0, OldName.size() - 4);
	}
	
	return OldName;
}

//�����ļ�����ȡ����·��
void GetFilePath(const char *filename, char *Path)
{
	std::string sfile = filename;
	strcpy(Path, filename);	
	char* pChar = Path + strlen(Path);
	int i = 0;
	for(; i < strlen(Path); i ++)
	{
		if(*pChar=='\\')
		{	
			i --;
			pChar ++;
			break;
		}
		pChar --;
	}
	sfile.erase(strlen(Path) - i);	
	strcpy(Path, sfile.c_str());
}

//ȥ���ļ����е�·��
void GetFileFromPath(const char *Path, char *filename)
{
	strcpy(filename, Path);
	char *key = "\\";
	char *pdest= strstr(Path, key);

	while(pdest != NULL)
	{
		strcpy(filename, ++pdest);
		pdest = strstr(filename, key);
	}
}

// ��ȡһ��ģ�͵Ķ����б�
void SearchAniFile(const char* szMeshXml,std::vector<std::string>* vDirList)
{
	TiXmlDocument *mXMLDoc = new TiXmlDocument(szMeshXml);
	mXMLDoc->LoadFile();
	
	TiXmlElement* elem = mXMLDoc->RootElement()->FirstChildElement("animations");
    if(elem)
	{
		for (TiXmlElement* smElem = elem->FirstChildElement();
				smElem != 0; smElem = smElem->NextSiblingElement())
		{
			vDirList->push_back(smElem->Attribute("src"));
		}	
	}
}
