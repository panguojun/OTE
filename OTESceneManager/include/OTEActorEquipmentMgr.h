#pragma once
#include "OTEActorEntity.h"

namespace OTE
{
// ************************************
// COTEActorEquipmentMgr
// 角色装备相关功能实现
// ************************************

class _OTEExport COTEActorEquipmentMgr
{
public:

	//显示装备
	static void ShowEquipment(COTEActorEntity* ae, const std::string& EquipmentName, bool isVisble = true);

	//显示所有装备
	static void ShowAllEquipment(COTEActorEntity* ae, bool isVisble = true);

	//添加SubMesh
	static Ogre::String AddSubMesh(COTEActorEntity* ae, const std::string& EquipmentName, const std::string& ResName);

	//删除SubMesh
	static void RemoveSubMesh(COTEActorEntity* ae, const std::string& EquipmentName);

	//安装submesh（AddSubMesh->SetupEquipments）
	static void SetupEquipments(COTEActorEntity* ae, bool HasMesh);

	//改变实体颜色
	static void SetColor(COTEActorEntity* ae, Ogre::ColourValue color);
	
	//改变颜色
	static void SetSubmeshColor(COTEActorEntity* ae, const std::string& EquipmentName, const Ogre::ColourValue& color);
	
	//改变贴图
	static void SetSubmeshTexture(COTEActorEntity* ae, const std::string& EquipmentName, const std::string& TextureName);
		
	//更改SubMeshName
	static void SetSubMeshName(COTEActorEntity* ae, const std::string& OldName, const std::string& NewName);

	//导出Mesh
	static void ExportMesh(COTEActorEntity* ae, const std::string& ResName);

	//导出SubMesh
	static void ExportSubMesh(COTEActorEntity* ae, const std::string& SubMeshName, const std::string& ResName);
	
	// 得到名称
	static const std::string& GetSubEntityName(COTEActorEntity* ae, const std::string& EquipmentName);
	
	// 得到subentity
	static COTESubEntity* GetSubEntity(COTEActorEntity* ae, const std::string& EquipmentName);

private:
	
	//复制
	static void CloneMaterial(COTEActorEntity* ae, const std::string& EquipmentName, Ogre::MaterialPtr& newMat);	

};


}
