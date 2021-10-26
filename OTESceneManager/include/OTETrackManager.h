#pragma once

#include "OTEStaticInclude.h"
#include "OTEKeyFrame.h"
#include "OTETrack.h"
#include "tinyxml.h"
#include "OTETrackXmlLoader.h"

namespace OTE
{
	// ****************************************
	// ���ڸ��ֲ�ͬ��;��track�������ԣ� ���
	// ��������Ʊ�������Ψһ, �ڲ�ͬ����;�� 
	// �в�ͬ�� usage ��ƥ��
	// ****************************************
	class _OTEExport COTETrackManager
	{
	public:
		// ��ȡTrackManager
		static COTETrackManager* GetInstance();

		// ����TrackManager
		static void Destroy();

	public:		
		// �Ƿ��Ѿ�����
		bool IsTrackPresent(int id);

		// ȡ��ָ��id��Track
		COTETrack* GetTrack(int id, COTETrackLoader* loarder);

		// �Ƴ�ָ����Track
		void Delete(int id);

		// �����µ�Track

		COTETrack* CreateNewTrack(int id, COTETrackLoader* loarder = NULL); // loarder = NULLʱ ������
	
	protected:

		//�����ڴ�
		void Clear();	

	protected:	

		static COTETrackManager* s_Instance;

		//����ȫ���Ѿ�ʹ�õľ�̬Track
		HashMap<int, COTETrack*> m_TrackListCach;

	};	

}