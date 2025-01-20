#pragma once
#include "Carry.h"
#include "Enemy_Base.h"

class Player;
class NavNode;
class Enemy : public Enemy_Base
{
public:
	//敵の役割系
	enum EnemyRole
	{
		eRole_Attacker,
		eRole_Flanker,
		eRole_Max,
	};

	struct Enemy_RoleData {
		int flankPersent;				//回り込みの確率
		float attackPriority;			//攻撃優先度
		float searchRangeMultiplier;	//視野角の倍率
	};

private:
	
	CModelA3M m_model;	//モデルオブジェクト
	CVector3D m_rot_target;
	CVector3D m_movePos;
	CMatrix enemy_matrix;	//敵の行列
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
	float m_viewAngle;		// 視野角度
	float m_viewLength;		// 視野距離
	float m_weight;

	CVector3D m_lastSeenPlayerPos;//最後にプレイヤーを見た場所
	bool m_isLastSeenPlayerPos;	  //最後に見た位置が有効かどうか
	
	//ランダム移動関連
	float randomX;
	float randomZ;
	float wanderTime;		//放浪（歩き回る）時間
	float wanderInterval;	//歩き回る間隔
	float wanderSpeed;		
	float wandercount;

	//回避行動
	float m_evadeCooldown;
	CVector3D m_evadeDir;

	//ダッシュ行動
	bool Dashing;
	float dashCooldownTimer;
	float m_dashAccel;   // 突進の加速度
	float m_currentDashSpeed; // 現在の突進速度
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

		eState_ChaseLastSeen,	//最後に見たプレイヤーの位置を追跡
		eState_Wander,
		eState_Flank,		//回り込む
		eState_Spin,
		eState_wait,
		eState_Dash,
		eState_Evade,

	};

	
	void SetRole(EnemyRole role);
	EnemyRole GetRole() const { return m_role;}

	EnemyRole m_role;//敵の役割

	EnemyState m_currentState; //現在の状態
	float m_idleProbability;	//待機する確率

	CVector3D m_pushBackForce;	// 押し返しの力
	//float m_pushBackStrength;	// 押し返しの強さ

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
	// 指定座標への移動処理
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

	Player* mp_player;		// プレイヤー
	Carry* m_carry;
	NavNode* m_navNode;
	NavNode* m_moveNode;	//移動先の座標
	NavNode* m_nextNode;	//次に移動するノード格納用
	NavNode* m_lostNode;	//プレイヤーを見失った



};