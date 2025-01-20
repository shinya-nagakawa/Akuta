#pragma once
#include "Carry.h"
#include "Enemy_Base.h"

class Player;
class Spider :public  Enemy_Base
{
private:
	CModelA3M m_model;	//���f���I�u�W�F�N�g
	CVector3D m_rot_target;
	CMatrix enemy_matrix;	//�G�̍s��

	CVector3D m_ground_normal;
	CMatrix m_rot_matrix;

	int m_hp;
	int m_state;				
	int m_previousState;	//�O�̏��
	int m_damage;
	int count;
	float AttackRange;
	float moveSpeed;
	float m_weight;
	float m_mutekijikan;
	float Min_speed;
	float Max_speed;
	float m_viewAngle;		// ����p�x
	float m_viewLength;		// ���싗��

	float m_attackCooldown;			// �U���N�[���_�E���^�C�}�[
	float m_attackCooldownMax;		// �U���N�[���_�E������

	CVector3D m_jumpDirection; // �W�����v�̐�������
	float m_jumpSpeedY;        // �W�����v�̐������x�iY���j
	float m_jumpSpeedX;        // �W�����v�̐������x�iX���j
	float m_jumpProbability;
	bool m_isJumping;          // �W�����v�����ǂ���
	bool m_isCanJump;
	bool m_isGround;
	float m_gravity;           // �d��
	float m_jumpCooldown;      // ���̃W�����v�܂ł̑ҋ@����
	float m_jumpCooldownMin;
	float m_jumpCooldownMax;
	float jump_count;
	float m_jumpDisableTime;
	float m_ambushWaitTime;
	float m_ambushTimer;

	bool m_isAttacking;
	bool thread;

	bool isAttached;
	bool climbing;
	CVector3D m_targetAttachPos;    // ����t���^�[�Q�b�g�̈ʒu
	CVector3D m_targetAttachNormal; // ����t���^�[�Q�b�g�̖@��
	bool m_hasTarget = false;       // �^�[�Q�b�g���ݒ肳��Ă��邩�ǂ���
	bool m_isMovingToAttach;
	CVector3D m_attachTarget;
	CVector3D m_ray_s;
	CVector3D m_ray_e;
	CVector3D m_debugAttachPos;
	CVector3D m_debugAttachNormal;
	bool m_isWallAttached;

	CVector3D m_pushBackForce;	// �����Ԃ��̗�
	float m_pushBackStrength;	// �����Ԃ��̋���

	

	enum EnemyState {
		eState_Idle,
		eState_Move,
		eState_Attack00,
		eState_Attack01,
		eState_Hit,
		eState_Down,
		eState_Ambush,			//�҂�����
		eState_Wall,			//�V�����ǂɒ���t���Ă�����
	};

public:
	Spider(const CVector3D& pos, int enemy_Hp, float weight);
	~Spider();

	
	void Idle();
	void Attack();
	void Down();
	void Damage();
	void HandleJump();

	bool IsTouchingGround();
	void MoveToTarget();
	bool CheckWallCollision(const CVector3D& dir);
	bool IsFoundTarget();
	bool FindSurfaceToAmbush(CVector3D& attachPos,CVector3D& attachNormal);
	CMatrix CalcRotMatrix(const CVector3D& normal)const;
	void StateWall();
	void Ambush();
	CVector3D ToRotation(const CVector3D& forward)const;
	bool IsLookPlayer() const;
	void RenderVision();
	//void Move();
	void Hit();
	void GiveMoney(int Value);
	//void Down();
	void UpdateAI();
	void Update();
	void Render();
	void Collision(Base* b);
	void Equip();
	void Unequip();
	int GetHP();

	CVector3D GetPos();
	int GetDamage();
	bool GetAttacking();

	Player* mp_player;		// �v���C���[
	Carry* m_carry;
};