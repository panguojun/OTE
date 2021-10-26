#ifndef _BINMESHSERIALIZER_H_
#define _BINMESHSERIALIZER_H_

#include "OgreMeshSerializerImpl.h"
#include "OgreSubMesh.h"

namespace OTE
{
	// ********************************************************
    // COTEBinMeshSerizlizer
	// submesh�ļ�������
	// ********************************************************
	class COTEBinMeshSerizlizer : public Ogre::MeshSerializerImpl
	{
	public:
		COTEBinMeshSerizlizer():MeshSerializerImpl(){}
		~COTEBinMeshSerizlizer(){}

		//����SubMesh
		void WriteSubMesh(const Ogre::SubMesh* s, const std::string& SubName, const std::string& filename);
		
		//����SubMesh
		void readSubMesh(Ogre::DataStreamPtr& stream, Ogre::Mesh* pMesh);

		Ogre::String ReadSubMesh(Ogre::DataStreamPtr& stream, Ogre::Mesh* pMesh);

	protected:

		Ogre::CriSection OGRE_AUTO_MUTEX_NAME;
	};
}

#endif