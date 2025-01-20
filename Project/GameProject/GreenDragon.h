#pragma once
#include "Game/Carry.h"
#include "Game/EnemyBase.h"
#include "Game/Enemy_Base.h"
#include "Game/Player.h"
#include "Game/Weapon.h"

class GreenDragon : public Enemy_Base
{
private:
	CModelA3M m_model;
	CVector3D m_rot_target;
	CMatrix enemy_matrix;	//�G�̍s��
	int m_state;
	int m_hp;
	int count;
	float m_weight;
	float m_moveSpeed;
	float m_chargeSpeed;
	float MaxChargeDistance;
	float m_chargeDistance;
	float m_mutekijikan;
	float AttackRange;
	float m_viewAngle;
	float m_viewLength;
	bool m_isAttacking;
	bool m_isChargeing;
	bool m_isTakeOff;

	CVector3D m_pushBackForce;	// �����Ԃ��̗�

	enum EnemyState {
		eState_Idle,
		eState_Move,
		eState_TakeOff,
		eState_Land,
		eState_Attack00,
		eState_Attack01,
		eState_Breath,
		eState_Charge,
		eState_Hit,
		eState_Down,
	};

public:
	GreenDragon(CVector3D& pos,int enemy_hp,float weight,float moveSpeed);
	~GreenDragon();

	void Idle();
	void Attack();
	void Down();
	void Hit();
	void Damage();
	bool IsFoundPlayer();
	bool CheckWallCollision(const CVector3D& dir);
	void breath();
	void Charge();
	void TakeOff();		//�󒆂�
	void Land();		//���n

	/*
	bool IsFoundTarget();

	bool IsLookPlayer() const;

	bool CheckWallCollision(const CVector3D& dir);*/

	void UpdateAI();

	void Update();
	void Render();
	void Collision(Base* b);

	void GiveMoney(int Value);

	CVector3D GetPos();
	int GetHP();

	Player* mp_player;		// �v���C���[
	Carry* m_carry;

};