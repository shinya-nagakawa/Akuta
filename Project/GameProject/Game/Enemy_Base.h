#pragma once
#include "Carry.h"
#include "NavNode.h"
#include "Player.h"

class Enemy_Base :public Carry
{
protected:
	CVector3D m_pos;

	float m_viewAngle;		//����p�x
	float m_viewLength;		//���싗��
	float m_collisionRadius;	//�Փ˔��a

public:
	Enemy_Base(const CVector3D& pos, float viewAngle,float viewLength, float collisionRadius);	//�Փ˔��a

	virtual void Update() = 0;	//�h���N���X�ŕK�{�̎���
	virtual void Render();

	bool IsFoundPlayer()const;
	bool IsLookPlayer()const;

	bool CheckWallCollision(const CVector3D& dir);
	bool IsWallNearby(float checkDistance);
	CVector3D CalculateAvoidDirection(float checkDistance);

	void RenderVision();
private:
	Player* mp_player;

};