#ifndef _BINMESHSERIALIZER_H_
#define _BINMESHSERIALIZER_H_

#include "OgreMeshSerializerImpl.h"
#include "OgreSubMesh.h"

namespace OTE
{
	// ********************************************************
    // COTEBinMeshSerizlizer
	// submesh文件加载器
	// ********************************************************
	class COTEBinMeshSerizlizer : public Ogre::MeshSerializerImpl
	{
	public:
		COTEBinMeshSerizlizer():MeshSerializerImpl(){}
		~COTEBinMeshSerizlizer(){}

		//导出SubMesh
		void WriteSubMesh(const Ogre::SubMesh* s, const std::string& SubName, const std::string& filename);
		
		//导入SubMesh
		void readSubMesh(Ogre::DataStreamPtr& stream, Ogre::Mesh* pMesh);

		Ogre::String ReadSubMesh(Ogre::DataStreamPtr& stream, Ogre::Mesh* pMesh);

	protected:

		Ogre::CriSection OGRE_AUTO_MUTEX_NAME;
	};
}

#endif