#pragma once
#define GRAVITY (9.8f*0.001)
//��ނ̗�
enum {
	eCamera,
	eField,
	eEnemy,
	ePlayer,
	eWeapon,
	eSpiderBullet,
	ePoisonBreath,
	eItem,
	eSellpoint,
	ePlayer_Attack,
	eEnemy_Attack,
	eBullet,
	eEffect,
	eLight,
	eEffectManager,
	eUI,
	eBuypoint,
	eMoney,
	eScene,
	eTitle,
	eRedFilter,
};



class Base {
public:
	unsigned int m_type;		//�I�u�W�F�N�g�̎��
	CVector3D	m_pos;		//���W
	CVector3D	m_rot;		//��]�l
	CVector3D	m_vec;		//�ړ��x�N�g��
	CVector3D	m_scale;	//�X�P�[��
	CVector3D	m_dir;		//�O����
	int		m_kill;		//�폜�t���O

	//���A�J�v�Z���p
	float		m_rad;		//���a
	//�J�v�Z���p
	CVector3D	m_lineS;	//�����n�_
	CVector3D	m_lineE;	//�����I�_

	CVector3D m_AttackS;
	CVector3D m_AttackE;
	//OBB�p
	COBB m_obb;


	//�U���̘A���q�b�g�h�~�[�u
	int m_hit_no;			//�U���q�b�g���ԍ�
	int m_attack_no;		//�U�����ԍ�
private:
	//�I�u�W�F�N�g���X�g
	static std::list<Base*> m_list;
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="type">�I�u�W�F�N�g�̎��</param>
	Base(unsigned int type);
	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Base();
	/// <summary>
	/// �폜�t���OON
	/// </summary>
	void SetKill();
	/// <summary>
	/// �I�u�W�F�N�g�̎�ނ��擾
	/// </summary>
	/// <returns></returns>
	unsigned int GetType() {
		return m_type;
	}
	/// <summary>
	/// �I�u�W�F�N�g�������f���̎擾
	/// </summary>
	/// <returns></returns>
	virtual CModel* GetModel() {
		return nullptr;
	}
	/// <summary>
	/// �X�V����
	/// </summary>
	virtual void Update();
	/// <summary>
	/// �`�揈��
	/// </summary>
	virtual void Render();
	/// <summary>
	/// �`�揈��(2D)
	/// </summary>
	virtual void Draw();
	/// <summary>
	/// �I�u�W�F�N�g���m�̓����蔻�茟��
	/// </summary>
	/// <param name="b">�ՓˑΏۃI�u�W�F�N�g</param>
	virtual void Collision(Base* b);

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̍폜
	/// </summary>
	static void KillALL();

	/// <summary>
	/// �w��̃I�u�W�F�N�g�̍폜
	/// </summary>
	static void Kill(unsigned int mask);

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̍폜�i���j���j
	/// </summary>
	static void ClearInstance();

	virtual CModel* GetNavModel() const {
		return nullptr;
	}


	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̍폜�`�F�b�N
	/// </summary>
	static void CheckKillALL();
	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̍X�V
	/// </summary>
	static void UpdateALL();
	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̕`��
	/// </summary>
	static void RenderALL();
	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̕`��(2D)
	/// </summary>
	static void DrawALL();
	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̓����蔻��
	/// </summary>
	static void CollisionALL();
	/// <summary>
	/// �I�u�W�F�N�g��ǉ�
	/// </summary>
	/// <param name="b">�ǉ��I�u�W�F�N�g</param>
	static void Add(Base* b);
	/// <summary>
	/// �I�u�W�F�N�g�̌���
	/// </summary>
	/// <param name="type">�����Ώۂ̎��</param>
	/// <returns></returns>
	static Base* FindObject(unsigned int type);
	/// <summary>
	/// �I�u�W�F�N�g�̌���(�z��Ŏ擾)
	/// </summary>
	/// <param name="type">�����Ώۂ̎��</param>
	/// <returns></returns>
	static std::vector<Base*> FindObjects(unsigned int type);
	//�t�����h�N���X
	friend class Base;


};
