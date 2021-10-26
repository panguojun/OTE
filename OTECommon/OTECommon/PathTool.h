#ifndef _PATH_TOOLS_H_
#define _PATH_TOOLS_H_

#pragma once

#include <string>
#include <stdio.h>
#include "OgreString.h"

//·��ת��
void ComToAbs(char *tPath, const char *path);
void AbsToCom(char *tPath, const char *path);
//�ļ���ת����ȥ��ĩβ��.xml
Ogre::String ReplaceName(const Ogre::String OldName);
//�����ļ�����ȡ����·��
void GetFilePath(const char *filename, char *Path);
//ȥ���ļ����е�·��
void GetFileFromPath(const char *Path, char *filename);
//��ȡһ��ģ�͵Ķ����ļ��б�
void SearchAniFile(const char* szMeshXml,std::vector<std::string>* vDirList);

#endif