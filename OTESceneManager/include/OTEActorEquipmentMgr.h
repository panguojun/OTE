#pragma once
#include "OTEActorEntity.h"

namespace OTE
{
// ************************************
// COTEActorEquipmentMgr
// ��ɫװ����ع���ʵ��
// ************************************

class _OTEExport COTEActorEquipmentMgr
{
public:

	//��ʾװ��
	static void ShowEquipment(COTEActorEntity* ae, const std::string& EquipmentName, bool isVisble = true);

	//��ʾ����װ��
	static void ShowAllEquipment(COTEActorEntity* ae, bool isVisble = true);

	//���SubMesh
	static Ogre::String AddSubMesh(COTEActorEntity* ae, const std::string& EquipmentName, const std::string& ResName);

	//ɾ��SubMesh
	static void RemoveSubMesh(COTEActorEntity* ae, const std::string& EquipmentName);

	//��װsubmesh��AddSubMesh->SetupEquipments��
	static void SetupEquipments(COTEActorEntity* ae, bool HasMesh);

	//�ı�ʵ����ɫ
	static void SetColor(COTEActorEntity* ae, Ogre::ColourValue color);
	
	//�ı���ɫ
	static void SetSubmeshColor(COTEActorEntity* ae, const std::string& EquipmentName, const Ogre::ColourValue& color);
	
	//�ı���ͼ
	static void SetSubmeshTexture(COTEActorEntity* ae, const std::string& EquipmentName, const std::string& TextureName);
		
	//����SubMeshName
	static void SetSubMeshName(COTEActorEntity* ae, const std::string& OldName, const std::string& NewName);

	//����Mesh
	static void ExportMesh(COTEActorEntity* ae, const std::string& ResName);

	//����SubMesh
	static void ExportSubMesh(COTEActorEntity* ae, const std::string& SubMeshName, const std::string& ResName);
	
	// �õ�����
	static const std::string& GetSubEntityName(COTEActorEntity* ae, const std::string& EquipmentName);
	
	// �õ�subentity
	static COTESubEntity* GetSubEntity(COTEActorEntity* ae, const std::string& EquipmentName);

private:
	
	//����
	static void CloneMaterial(COTEActorEntity* ae, const std::string& EquipmentName, Ogre::MaterialPtr& newMat);	

};


}
