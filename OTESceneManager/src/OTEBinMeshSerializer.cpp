#include "OTEStaticInclude.h"
#include "OTEBinMeshSerializer.h"
#include "OgreString.h"
#include "OgreMaterial.h"
#include "OgreMesh.h"
#include <windows.h>

// ------------------------------------------------
using namespace Ogre;
using namespace OTE;

// ------------------------------------------------
//导出SubMesh
void COTEBinMeshSerizlizer::WriteSubMesh(const SubMesh* s, const std::string& SubName, const std::string& filename)
{
	mpfFile = fopen(filename.c_str(), "wb");

	//char* SubMeshName
	writeString(SubName);

	MeshSerializerImpl::writeSubMesh(s);

	fclose(mpfFile);
}

// ------------------------------------------------
//导入SubMesh
Ogre::String COTEBinMeshSerizlizer::ReadSubMesh(DataStreamPtr& stream, Mesh* pMesh)
{
	OGRE_LOCK_AUTO_MUTEX

	String SubName = readString(stream);

	Mesh::SubMeshNameMap nm = pMesh->getSubMeshNameMap();
	Mesh::SubMeshNameMap::iterator it = nm.begin();
	for(; it != nm.end(); ++ it)
	{
		if(SubName == it->first)
		{
			//MessageBox(NULL, "已经存在同名的SubMesh!", "ERROR", MB_OK);
			//OTE_TRACE_ERR("已经存在同名的SubMesh! SubMesh : " << SubName)
			//OTE_LOGMSG("已经存在同名的SubMesh! SubMesh : " << SubName)
			return SubName;
		}
	}

	unsigned short i = 0;
	unsigned int j = 0;
	readShorts(stream, &i, 1);
	readInts(stream, &j, 1);

	MeshSerializerImpl::readSubMesh(stream, pMesh);	
	
	i = pMesh->getNumSubMeshes();
	pMesh->nameSubMesh(SubName, i-1);

	return SubName;
}

// ------------------------------------------------
void COTEBinMeshSerizlizer::readSubMesh(DataStreamPtr& stream, Mesh* pMesh)
{
	OGRE_LOCK_AUTO_MUTEX

	String SubName = readString(stream);

	Mesh::SubMeshNameMap nm = pMesh->getSubMeshNameMap();
	Mesh::SubMeshNameMap::iterator it = nm.begin();
	for(; it != nm.end(); ++ it)
	{
		if(SubName == it->first)
		{
			MessageBox(NULL, "已经存在同名的SubMesh!", "ERROR", MB_OK);
			return;
		}
	}

	unsigned short i = 0;
	unsigned int j = 0;
	readShorts(stream, &i, 1);
	readInts(stream, &j, 1);

	MeshSerializerImpl::readSubMesh(stream, pMesh);	
	
	i = pMesh->getNumSubMeshes();
	pMesh->nameSubMesh(SubName, i-1);


}