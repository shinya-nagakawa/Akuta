#pragma once
#include "Base/Base.h"
#include "Spider.h"

class SpiderBullet: public Base
{
private:
	CVector3D StartPos;
	CVector3D Forward;
	CVector3D m_targetPos;
	CVector3D m_attachmentPoint;//�v���C���[�̃��f���Ɏ������������n�_
	CVector3D m_relativeAttachmentPoint;

	float RayDistance;
	float count;
	float gravity;
	float m_disappearTimer;
	float m_scale;


	int m_state;
	bool m_isAttachedToPlayer;//�v���C���[�Ɏ����������Ă��邩�B

	Spider* mp_parent;

	enum State
	{
		eState_Normal,		//�ʏ�
		eState_Flying,		//�������ł�����
		eState_Stuck,		//�ǂɂ����������
		eState_Cut,
	};

public:
	Base* m_hit_object;
	CVector3D m_hit_point;

	SpiderBullet(Spider*parent,const CVector3D& pos, const float speed, const CVector3D& target);
	void Update();
	void Render();
	void Cut();
	void RegenerateLine(float noiseStrength);
	bool CheckCollisionWithPlayer();

	//���\���p
	std::vector<CVector3D> m_lines;
	std::list<SpiderBullet*> m_list;

};