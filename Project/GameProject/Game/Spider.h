#pragma once
#include "Carry.h"
#include "Enemy_Base.h"

class Player;
class Spider :public  Enemy_Base
{
private:
	CModelA3M m_model;	//モデルオブジェクト
	CVector3D m_rot_target;
	CMatrix enemy_matrix;	//敵の行列

	CVector3D m_ground_normal;
	CMatrix m_rot_matrix;

	int m_hp;
	int m_state;				
	int m_previousState;	//前の状態
	int m_damage;
	int count;
	float AttackRange;
	float moveSpeed;
	float m_weight;
	float m_mutekijikan;
	float Min_speed;
	float Max_speed;
	float m_viewAngle;		// 視野角度
	float m_viewLength;		// 視野距離

	float m_attackCooldown;			// 攻撃クールダウンタイマー
	float m_attackCooldownMax;		// 攻撃クールダウン時間

	CVector3D m_jumpDirection; // ジャンプの水平方向
	float m_jumpSpeedY;        // ジャンプの垂直速度（Y軸）
	float m_jumpSpeedX;        // ジャンプの水平速度（X軸）
	float m_jumpProbability;
	bool m_isJumping;          // ジャンプ中かどうか
	bool m_isCanJump;
	bool m_isGround;
	float m_gravity;           // 重力
	float m_jumpCooldown;      // 次のジャンプまでの待機時間
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
	CVector3D m_targetAttachPos;    // 張り付きターゲットの位置
	CVector3D m_targetAttachNormal; // 張り付きターゲットの法線
	bool m_hasTarget = false;       // ターゲットが設定されているかどうか
	bool m_isMovingToAttach;
	CVector3D m_attachTarget;
	CVector3D m_ray_s;
	CVector3D m_ray_e;
	CVector3D m_debugAttachPos;
	CVector3D m_debugAttachNormal;
	bool m_isWallAttached;

	CVector3D m_pushBackForce;	// 押し返しの力
	float m_pushBackStrength;	// 押し返しの強さ

	

	enum EnemyState {
		eState_Idle,
		eState_Move,
		eState_Attack00,
		eState_Attack01,
		eState_Hit,
		eState_Down,
		eState_Ambush,			//待ち伏せ
		eState_Wall,			//新しく壁に張り付いている状態
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

	Player* mp_player;		// プレイヤー
	Carry* m_carry;
};