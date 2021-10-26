#ifndef _PATH_TOOLS_H_
#define _PATH_TOOLS_H_

#pragma once

#include <string>
#include <stdio.h>
#include "OgreString.h"

//路径转换
void ComToAbs(char *tPath, const char *path);
void AbsToCom(char *tPath, const char *path);
//文件名转换，去掉末尾的.xml
Ogre::String ReplaceName(const Ogre::String OldName);
//根据文件名获取绝对路径
void GetFilePath(const char *filename, char *Path);
//去除文件名中的路径
void GetFileFromPath(const char *Path, char *filename);
//获取一个模型的动画文件列表
void SearchAniFile(const char* szMeshXml,std::vector<std::string>* vDirList);

#endif