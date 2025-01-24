#pragma once
#include "../Base/Base.h"
#include "Carry.h"

class Gauge;
class Player : public Base {
private:
	CModelA3M * m_model;
	CVector3D dir;
	CVector3D m_velocity;		//�v���C���[�̑��x
	float m_hp;					//����HP
	float m_hp_max;				//�ő�HP
	float m_stamina;			//���݂̃X�^�~�i
	float m_stamina_max;		//�ő�X�^�~�i
	float recoveryspeed;		//�X�^�~�i�̉񕜑��x
	float mutekijikan;			//���G����
	float threadattachtimer;	//�����������Ă��鎞��timer
	float move_speed;			//�ړ����x
	float m_weight;				//�v���C���[�̏d��
	float m_originalWeight;
	float weightFactor;
	float weightSpeed;
	//float key_length;		
	float m_friction;			//���C��
	CVector3D m_pushBackForce;	// �����Ԃ��̗�
	float m_pushBackStrength;	// �����Ԃ��̋���
	int m_state;				//���
	int PlayerMoney;			//���݂̏�����
	int PlayerMoneyMax;			//�}�b�N�X�l��ύX����Ə��������㉺����
	int count;					//�J�E���^�[
	int GameOvercount;          //�Q�[���I�[�o�[�ɂȂ��Ă���ăX�^�[�g�܂ł̎���
	float jump_interval;
	float AttackInterbal;		//�U���̊Ԋu
	float m_ChargeTime;			//�`���[�W�̃^�C�}�[
	float m_ChargeDuration;		//�`���[�W�̊Ԋu
	float BaseSpead;
	float m_JumpDelay;
	float m_startFallHeight;	//������������
	float m_knockbackTimer;
	bool m_isAttacking;			//�U�����Ă��邩
	bool m_isAttackReleased;	//�U���m��
	bool tamekougeki;
	bool m_isChargeing;			//���ߒ���
	bool m_isDashing;			//�_�b�V�����Ă��邩
	bool m_isJump;				//�W�����v�\��
	bool m_isGround;			//�n�ʂɂ��Ă��邩
	bool m_isFalling;			//�������Ă���
	bool m_isAction;
	bool m_isMove;
	bool m_isCarrying;			//�A�C�e����S���ł��邩�ǂ���
	bool nearItem;				//�߂��ɃA�C�e�������邩
	bool m_isEkeyPressd;		//Ekey�����������ǂ���
	bool m_isSiftPressd;		//Shift�����������ǂ���
	bool m_isSpacePressd;		//Space�����������ǂ���
	bool m_isLclickPressd;		//Lclick�����������ǂ���
	bool EnemyAttacking;		//�G���U�����Ă��邩
	bool m_isKnockback;	

	//�A�C�e���n
	bool m_isJetpackActive;	//�W�F�b�g�p�b�N�̃I���I�t
	int m_jetpackFuel;	//�W�F�b�g�p�b�N�̔R��
	float m_jetpackForce;	//�W�F�b�g�p�b�N�̗�
	float m_Force;
	//���ʉ��n
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
	void Idle();		//�ҋ@
	void Attack();		//�U��
	void Move();		//�ړ�
	void tameAttack();	//���ߍU��
	void Down();		//�_�E���i���S��ԁH�j
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

	//�Q�b�^�[�n
	bool GetAttack();
	bool GetAttackRelease() const; // �t���O�擾�p
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
	
	//�����֘A
	int GetPlayerMoney();
	void GainMoney(int Money);
	int Pay(int Money);
	void Draw();
	//void Sell(Enemy* enemy);
	
//�S���֘A
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

	//�A�C�e���֘A
	void JetPackOn();
	void JetPackOff();
	void JetPackFuel(int shouhiryou);
	//float GetJetPackFuel();
	bool GetjetpackActive(); //�I�����I�t���擾

	Carry* m_carry;
	Gauge* m_gauge;
	Gauge* m_gauge1;

	CModelA3M* GetModel() 
	{
		return m_model;
	}

	

};
