#pragma once
#include "Carry.h"
#include "Enemy_Base.h"

class Player;
class NavNode;
class Enemy : public Enemy_Base
{
public:
	//�G�̖����n
	enum EnemyRole
	{
		eRole_Attacker,
		eRole_Flanker,
		eRole_Max,
	};

	struct Enemy_RoleData {
		int flankPersent;				//��荞�݂̊m��
		float attackPriority;			//�U���D��x
		float searchRangeMultiplier;	//����p�̔{��
	};

private:
	
	CModelA3M m_model;	//���f���I�u�W�F�N�g
	CVector3D m_rot_target;
	CVector3D m_movePos;
	CMatrix enemy_matrix;	//�G�̍s��
	int m_hp;
	int m_state;
	int role;
	int m_damage;
	int DirectionChange;
	int currentFrame;
	float m_mutekijikan;
	float move_speed;
	float Max_speed;
	float Min_speed;
	float AttackRange;
	float FlankRange;
	float SpinRange;
	float m_viewAngle;		// ����p�x
	float m_viewLength;		// ���싗��
	float m_weight;

	CVector3D m_lastSeenPlayerPos;//�Ō�Ƀv���C���[�������ꏊ
	bool m_isLastSeenPlayerPos;	  //�Ō�Ɍ����ʒu���L�����ǂ���
	
	//�����_���ړ��֘A
	float randomX;
	float randomZ;
	float wanderTime;		//���Q�i�������j����
	float wanderInterval;	//�������Ԋu
	float wanderSpeed;		
	float wandercount;

	//����s��
	float m_evadeCooldown;
	CVector3D m_evadeDir;

	//�_�b�V���s��
	bool Dashing;
	float dashCooldownTimer;
	float m_dashAccel;   // �ːi�̉����x
	float m_currentDashSpeed; // ���݂̓ːi���x
	CVector3D m_dashDir;

	float m_curveRadius;
	float m_curveSpeed;
	float m_curveAngle;
	
	float m_stateChangeTimer;
	float m_stateChangeDuration;
	
	bool goLeft;

	float AttackInterval;
	float AttackTimer;
	float m_searchLength;
	float PlayerHp;
	float m_priority;
	float m_circleAngle;
	float flankTimer;
	bool m_isFoundTarget;
	bool m_forceDetectPlayer;
	bool m_isAttacking;
	bool m_isWanderOn;
	int m_attackCount;
	int count;
	int timer;
	
	float timer2;
	float dashTimer;
	float evadeTimer;


	enum EnemyState {
		eState_Idle,
		eState_Move,
		eState_Attack00,
		eState_Attack01,
		eState_Hit,
		eState_Down,

		eState_ChaseLastSeen,	//�Ō�Ɍ����v���C���[�̈ʒu��ǐ�
		eState_Wander,
		eState_Flank,		//��荞��
		eState_Spin,
		eState_wait,
		eState_Dash,
		eState_Evade,

	};

	
	void SetRole(EnemyRole role);
	EnemyRole GetRole() const { return m_role;}

	EnemyRole m_role;//�G�̖���

	EnemyState m_currentState; //���݂̏��
	float m_idleProbability;	//�ҋ@����m��

	CVector3D m_pushBackForce;	// �����Ԃ��̗�
	//float m_pushBackStrength;	// �����Ԃ��̋���

	void Idle();
	void Attack();
	void Move();
	void Dash();
	void Evade();
	void Hit();
	void Down();
public:
	Enemy(const CVector3D& pos, int enemy_Hp, float weight,EnemyRole role);
	
	
	bool IsFoundTarget();
	bool IsFoundPlayer() const;
	//bool IsLookPlayer()const;
	bool GetAttacking();
	// �w����W�ւ̈ړ�����
	bool MoveTo(const CVector3D& target, float moveSpeed);
	int GetAttackno();
	int GetDamage();
	CVector3D GetPos();

	
	void Update()override;
	void UpdateAI();
	void Initialize();
	void ChaseLastSeen();
	void Wander();
	void Render()override;
	void RenderVision();
	void Collision(Base* b);
	//bool CheckWallCollision(const CVector3D& dir);
	//bool IsWallNearby(float checkDistance);
	//CVector3D CalculateAvoidDirection(float checkDistance);
	void AvoidWalls();
	void DebugAvoidance();
	void Damage();
	void Flank();
	void IncreaseAttackCount();
	void GiveMoney(int Value);
	
	void Equip();
	void Unequip();
	int GetHP();

	struct Node;
	//CVector3D FindPath(const CVector3D& startPos, const CVector3D& targetPos);

	Player* mp_player;		// �v���C���[
	Carry* m_carry;
	NavNode* m_navNode;
	NavNode* m_moveNode;	//�ړ���̍��W
	NavNode* m_nextNode;	//���Ɉړ�����m�[�h�i�[�p
	NavNode* m_lostNode;	//�v���C���[����������



};