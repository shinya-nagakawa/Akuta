#pragma once
#include "../Base/Base.h"
#include "Carry.h"

class Gauge;
class Player : public Base {
private:
	CModelA3M * m_model;
	CVector3D dir;
	CVector3D m_velocity;		//プレイヤーの速度
	float m_hp;					//今のHP
	float m_hp_max;				//最大HP
	float m_stamina;			//現在のスタミナ
	float m_stamina_max;		//最大スタミナ
	float recoveryspeed;		//スタミナの回復速度
	float mutekijikan;			//無敵時間
	float threadattachtimer;	//糸がくっついている時のtimer
	float move_speed;			//移動速度
	float m_weight;				//プレイヤーの重さ
	float m_originalWeight;
	float weightFactor;
	float weightSpeed;
	//float key_length;		
	float m_friction;			//摩擦力
	CVector3D m_pushBackForce;	// 押し返しの力
	float m_pushBackStrength;	// 押し返しの強さ
	int m_state;				//状態
	int PlayerMoney;			//現在の所持金
	int PlayerMoneyMax;			//マックス値を変更すると所持金が上下する
	int count;					//カウンター
	int GameOvercount;          //ゲームオーバーになってから再スタートまでの時間
	float jump_interval;
	float AttackInterbal;		//攻撃の間隔
	float m_ChargeTime;			//チャージのタイマー
	float m_ChargeDuration;		//チャージの間隔
	float BaseSpead;
	float m_JumpDelay;
	float m_startFallHeight;	//落下した距離
	float m_knockbackTimer;
	bool m_isAttacking;			//攻撃しているか
	bool m_isAttackReleased;	//攻撃確定
	bool tamekougeki;
	bool m_isChargeing;			//溜め中か
	bool m_isDashing;			//ダッシュしているか
	bool m_isJump;				//ジャンプ可能か
	bool m_isGround;			//地面についているか
	bool m_isFalling;			//落下している
	bool m_isAction;
	bool m_isMove;
	bool m_isCarrying;			//アイテムを担いでいるかどうか
	bool nearItem;				//近くにアイテムがあるか
	bool m_isEkeyPressd;		//Ekeyを押したかどうか
	bool m_isSiftPressd;		//Shiftを押したかどうか
	bool m_isSpacePressd;		//Spaceを押したかどうか
	bool m_isLclickPressd;		//Lclickを押したかどうか
	bool EnemyAttacking;		//敵が攻撃しているか
	bool m_isKnockback;	

	//アイテム系
	bool m_isJetpackActive;	//ジェットパックのオンオフ
	int m_jetpackFuel;	//ジェットパックの燃料
	float m_jetpackForce;	//ジェットパックの力
	float m_Force;
	//効果音系
	bool m_isWalking;
	float m_footstepTimer;
	float m_footstepInterval;
	
	std::vector<Carry*>m_inventory;
	const int m_maxItems;
	CImage m_inventory_image;

	static Player* ms_instance;
	

	enum {
		eState_Idle,
		eState_Move,
		eState_Attack00,
		eState_Attack01,
		eState_Down
	};
	void UpdateState();
	void Idle();		//待機
	void Attack();		//攻撃
	void Move();		//移動
	void tameAttack();	//ため攻撃
	void Down();		//ダウン（死亡状態？）
	void HandleMovement(CVector3D& key_dir, int& animation);
	//void Damage();

public:
	Player(const CVector3D& pos);
	~Player();
	static Player* Instance();
	void Update();
	void Render();
	void Collision(Base* b);

	void ApplyKnockback(const CVector3D& knockbackForce);

	void UpdateGroundStatus();

	//ゲッター系
	bool GetAttack();
	bool GetAttackRelease() const; // フラグ取得用
	CVector3D GetPos();
	bool GetMove();
	bool GetCarry();
	bool GetDashing();
	float GetHp();
	float GetDamage(float damage);

	CVector3D GetRotation()const;
	void SetRotation(const CVector3D& rotation);
	void UpdateModelRotation(const CVector3D& rotation);

	void Heal(int HealValue);
	void MovespeedUp(float speedUp);
	float m_fallSpeedOnLand;
	void ApplyFallDamage();
	void RedFilterDisplay();
	bool PlayerJump(bool canjump);
	
	//お金関連
	int GetPlayerMoney();
	void GainMoney(int Money);
	int Pay(int Money);
	void Draw();
	//void Sell(Enemy* enemy);
	
//担ぎ関連
	bool CheckItemCollision(const CVector3D& dir);
	void SetCarry(Carry* carry);
	void DropCarry(Carry* carry);
	bool PickUpItem(Carry* item);
	void SwapItem(int idx);
	void SelectItem();
	void SetWeight(float weight);
	void SaveOriginalWeight();
	void ResetToOriginalWeight();
	float GetWeight()const;

	//アイテム関連
	void JetPackOn();
	void JetPackOff();
	void JetPackFuel(int shouhiryou);
	//float GetJetPackFuel();
	bool GetjetpackActive(); //オンかオフか取得

	Carry* m_carry;
	Gauge* m_gauge;
	Gauge* m_gauge1;

	CModelA3M* GetModel() 
	{
		return m_model;
	}

	

};
