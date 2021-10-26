#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OTETile.h"
#include "OTETilePage.h"

namespace OTE
{
	class COTEPrjTextureRQListener;

	// *****************************************
	// COTEPrjTexture
	// *****************************************

	class _OTEExport COTEPrjTexture : public Ogre::MovableObject, public Ogre::Renderable
	{
	friend COTEPrjTextureRQListener;
	public:	

		virtual void setMaterialName(const Ogre::String& name);
	
		virtual const Ogre::String& getMaterialName(void) const
		{
			return m_pMaterial->getName();	
		}
		
		virtual void _notifyCurrentCamera(Ogre::Camera* cam);
		
		virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const 
		{ 
			return m_AABB; 
		}
		
		virtual Ogre::Real getBoundingRadius(void) const { return m_BoundingRadius; }
		
		virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

		virtual const Ogre::Quaternion& getWorldOrientation(void) const;	    
	
		virtual const Ogre::Vector3& getWorldPosition(void) const;

		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;

		virtual void getRenderOperation(Ogre::RenderOperation& op);	

		virtual const Ogre::MaterialPtr& getMaterial(void) const
		{
			return m_pMaterial;
		}
	
		virtual const Ogre::String& getName(void) const
		{
			return m_strName;
		}
		
		virtual const Ogre::String& getMovableType(void) const
		{
			static Ogre::String	sMovableType = "PrjTex";
			return sMovableType;
		}
		
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
		
		const Ogre::LightList& getLights(void) const;	

	public:	

		void Init(const std::string& texture, const std::string& name);

		void Render();

		void Update(const Ogre::Vector3& TargetPos);

		void Destroy();	

		void Show(bool isVisible)
		{
			m_IsVisible = isVisible;
		}

	protected:

		Ogre::Camera*				m_pRTTCamera;

		std::list<void*>			m_RenderTileList;

		float						m_Width;		

		float						m_Height;

		bool						m_IsVisible;

		std::string					m_strName;

		Ogre::MaterialPtr			m_pMaterial;

		Ogre::AxisAlignedBox		m_AABB;

		float						m_BoundingRadius;

		/// ÌùÍ¼Ãû

		std::string					m_strTextureFile;

		Ogre::TexturePtr			m_Texture;

		Ogre::RenderOperation		m_RenderOper;

		Ogre::HardwareVertexBufferSharedPtr		m_VertexBuffer;

		Ogre::HardwareIndexBufferSharedPtr		m_IndexBuffer;
	
	};

	// *********************************************
	// COTEPrjTextureRQListener
	// *********************************************

	class _OTEExport COTEPrjTextureRQListener : public Ogre::RenderQueueListener
	{
	public:	

		static void Init();

		static void Destroy();

		static COTEPrjTexture* CreatePrjTex(const std::string& name, const std::string& texture = "µã»÷Í¼±ê.dds");

		static void DestroyPrjTex(const std::string& name);

		static void DestroyPrjTex(COTEPrjTexture* pt);	

	public:

		COTEPrjTextureRQListener(Ogre::uint8 queue_id, bool post_queue)
		{		
			m_queue_id		= queue_id;
			m_post_queue	= post_queue;
			
		}

		virtual ~COTEPrjTextureRQListener() {}					

		virtual void renderQueueStarted(Ogre::RenderQueueGroupID id, bool& skipThisQueue);
		virtual void renderQueueEnded(Ogre::RenderQueueGroupID id, bool& repeatThisQueue);

		// methods for adjusting target queue settings
		void	setTargetRenderQueue(Ogre::uint8 queue_id)		{m_queue_id = queue_id;}
		void	setPostRenderQueue(bool post_queue)		{m_post_queue = post_queue;}
		
	private:

		static COTEPrjTextureRQListener*	m_pInst;

		Ogre::uint8						m_queue_id;		
		bool							m_post_queue;		

	public:

		HashMap<std::string, COTEPrjTexture*>		m_pPrjTextureList;

	};


}