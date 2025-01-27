#include "Player.h"
#include "Camera.h"
#include "Weapon.h"
#include "Item.h"
#include "Enemy.h"
#include "Spider.h"
#include "GreenDragon.h"
#include "SpiderBullet.h"
#include "Gauge.h"
#include "RedFilter.h"
#include "BlackFilter.h"
#include "SceneManager.h"
#include "Title.h"
#include "Game.h"
#include "UI.h"


Player* Player::ms_instance = nullptr;
// �����ʒu��ۑ����邽�߂̕ϐ�
CVector3D initialPosition;

Player::Player(const CVector3D& pos) :Base(ePlayer), m_carry(nullptr), m_gauge(nullptr), m_maxItems(4)
{
	ms_instance = this;

	// ���f���Ə����ݒ�
	m_model = GET_RESOURCE("vanguard", CModelA3M);
	m_model->ChangeAnimation(0);
	m_pos = pos;
	initialPosition = pos;//�����ʒu���L�^

	// �����X�e�[�^�X
	m_hp = m_hp_max = 100;
	m_stamina = m_stamina_max = 100;
	m_weight = 0.0f;
	m_pushBackStrength = 0.25f;
	move_speed = 1.0f;
	BaseSpead = 0.010f;
	m_rad = 0.4f;

	m_jetpackFuel = 1000.0f;
	m_jetpackForce = 0.005f;

	// ��ԃt���O
	m_isGround = true;
	m_isJump = true;
	m_isWalking = false;
	m_isAttacking = false;
	m_isChargeing = false;
	m_isKnockback = false;

	m_footstepTimer = 0.0f;
	m_footstepInterval = 0.5f;

	// ���̑�������
	m_pushBackForce = CVector3D(0, 0, 0);
	m_velocity = CVector3D(0, 0, 0);
	m_friction = 0.85f;

	// �A�j���[�V�����̃��C���[�ݒ�
	for (int i = 5; i <= 58; i++) {
		m_model->GetNode(i)->SetAnimationLayer(1);
	}
	m_inventory.resize(m_maxItems);
	m_inventory_image = COPY_RESOURCE("Inventory", CImage);

	// �Q�[�W����
	Base::Add(m_gauge = new Gauge(Gauge::GaugeType::ePlayerGauge, 0.28f));
	Base::Add(m_gauge1 = new Gauge(Gauge::GaugeType::ePlayerStamina, 0.3f));

}

// �n�ʔ�����X�V
void Player::UpdateGroundStatus()
{
	m_isGround = false;

	auto collisions = Base::FindObject(eField)->GetModel()->CollisionCupsel(
		m_pos + CVector3D(0, 2.0 - m_rad, 0),  // �n�_�i���j
		m_pos + CVector3D(0, m_rad, 0),        // �I�_�i�����j
		m_rad
	);

	for (auto& tri : collisions) {
		// �n�ʂƐڐG�����Ɣ��肷�����
		if (tri.m_normal.y > 0.5f) {
			m_isGround = true;
			if (m_vec.y < 0) {
				m_vec.y = 0; // ���������Z�b�g
			}
		}
	}
}

// ������я���
void Player::ApplyKnockback(const CVector3D& knockbackForce)
{
	// ������΂��̗͂����Z
	m_vec += knockbackForce;

	// ������ю��ł��n�ʐڐG���m�F
	UpdateGroundStatus();

	if (m_isGround) {
		// �n�ʂɐڂ��Ă���ꍇ�ł�������т𑱍s
		m_isKnockback = true;
		m_knockbackTimer = 0.5f;
	}
	else {
		// �󒆂ł̐�����т�����
		m_isKnockback = true;
		m_knockbackTimer = 0.5f;
	}
}

// �_���[�W���󂯂��ۂ̐ԃt�B���^�[�\��
void Player::RedFilterDisplay()
{
	RedFilter* r = dynamic_cast<RedFilter*>(Base::FindObject(eRedFilter));
	if (!r) {
		Base::Add(r = new RedFilter());
	}
	r->Show(30.0f);
}

// ��ԕʏ���
void Player::UpdateState()
{
	switch (m_state) {
	case eState_Idle:
	case eState_Move:
		Idle();
		break;
	case eState_Attack00:
		Attack();
		break;
	case eState_Attack01:
		tameAttack();
		break;
	case eState_Down:
		Down();
		break;
	}
}

void Player::Idle()
{
	CVector3D key_dir(0, 0, 0);
	int animation = 0;
	SelectItem();
	HandleMovement(key_dir, animation);

	//�W�F�b�g�p�b�N
	if (HOLD(CInput::eMouseL) && m_isJetpackActive && m_jetpackFuel > 0.0f)
	{
		m_isGround = false;
		m_isFalling = false;

		//���シ��
		m_vec += (CMatrix::MRotation(Camera::Instance()->m_rot).GetUp() * m_jetpackForce * 1.3f);

		if (m_Force <= 0.015f)
		{
			m_Force += 0.0003f;
		}

		m_vec += CVector3D(0, m_Force, 0);

		//�R������
		JetPackFuel(0);
	}
	else
	{
		JetPackOff();
	}


	// �L�[���͂������
	if (key_dir.LengthSq() > 0) {
		// ���ړ����� ��]�s��~�L�[����
		CVector3D dir = CMatrix::MRotationY(m_rot.y) * key_dir;
		m_velocity += dir * BaseSpead;
		if (m_isGround)
		{
			m_model->ChangeAnimation(0, animation);
			static int idx = 0;
			static int anim_old = 0;
			if (animation != anim_old) {
				anim_old = animation;
				idx = 0;
			}
			static const int se_frame[11][2] = {
				{4,15 },	//0
				{13,32 },	// ���ړ��A�j���[�V����
				{4,15 },	//2
				{4,15 },	//3
				{4,15 },	//4
				{4,15 },	//5
				{13,32 },	// �E�ړ��A�j���[�V����
				{4,15 },	// �O�i�A�j���[�V����
				{13,32 },	// ��ރA�j���[�V����
				{4,15 },	//9
				{4,15 },	//10
			};

			if (m_model->GetAnimationFrame(0) == se_frame[animation][idx]) {
				idx = (idx + 1) % 2;
				SOUND("����")->Play3D(m_pos, CVector3D::zero, false, true, EFX_REVERB_PRESET_CAVE);
			}
		}
		m_state = eState_Move;
		m_isMove = true;

	}

	else
	{
		m_isMove = false;

		//�L�[���͂��Ȃ����
		if (!m_carry)
		{
			m_model->ChangeAnimation(1, 10, 0.2f);
			m_model->ChangeAnimation(0, animation, 0.2f);
		}
		if (m_carry && m_carry->m_type == eEnemy)
		{
			m_model->ChangeAnimation(1, 11, 0.2f);
			m_model->ChangeAnimation(0, 9, 0.2f);
		}
		if (m_carry && m_carry->m_type == eWeapon)
		{
			m_model->ChangeAnimation(1, 9, 0.2f);
			m_model->ChangeAnimation(0, 9, 0.2f);
		}
		if (m_carry && m_carry->m_type == eItem)
		{
			m_model->ChangeAnimation(0, 9, 0.2f);
		}


	}
	m_state = eState_Idle;

	if (!m_isAttacking && PUSH(CInput::eMouseL) && m_carry && m_carry->m_type == eWeapon) {
		m_isChargeing = true;   // �`���[�W�J�n
		m_ChargeTime = 0.0f;
		m_state = eState_Attack00;

	}
}

void Player::Attack()
{

	CVector3D key_dir(0, 0, 0);
	int animation = 0;

	HandleMovement(key_dir, animation);


	//�L�����N�^�[�̉�]�l���J�����̉�]�l�ɍ��킹��
	m_rot.y = Camera::Instance()->m_rot.y;

	// �L�[���͂������
	if (key_dir.LengthSq() > 0) {
		// ���ړ����� ��]�s��~�L�[����
		CVector3D dir = CMatrix::MRotationY(m_rot.y) * key_dir;
		m_model->ChangeAnimation(0, animation);
		m_velocity += dir * BaseSpead * move_speed;
	}
	else
	{
		//�L�[���͂��Ȃ����
		if (!m_carry)
		{
			m_model->ChangeAnimation(1, 10, 0.2f);
		}
		if (m_carry)
		{

			m_model->ChangeAnimation(0, 9, 0.2f);
		}


	}

	//���ߍU������肽��
	if (!m_isAttacking && HOLD(CInput::eMouseL) && m_carry)
	{
		m_isAttacking = false;
		m_model->ChangeAnimation(1, 3, false);
		m_ChargeTime++;
		if (!tamekougeki)
			SOUND("���ߍU��")->Play(false);

		tamekougeki = true;
	}

	if (!m_isAttacking && PULL(CInput::eMouseL) && m_carry && m_stamina > 20)
	{
		SOUND("���ߍU��")->Stop();
		m_isAttackReleased = true;//�N���b�N�𗣂����u��
		if (m_isChargeing)
		{
			m_isAttacking = true;
			if (m_ChargeTime >= 35.0f) {
				m_stamina -= 20;
				tamekougeki = false;
				m_state = eState_Attack01; // �`���[�W�U��
			}
			else {
				m_stamina -= 20;
				tamekougeki = false;
				m_model->ChangeAnimation(1, 5, false);
				m_model->SetAnimationSpeed(1, move_speed);
				m_model->ChangeAnimation(0, 9, 0.2f);
			}
			if (!tamekougeki)
				SOUND("���ߍU��")->Stop();
		}
	}
	else
	{
		m_isAttackReleased = false; //�N���b�N��b���ĂȂ����̓��Z�b�g
	}

	//---------------------------------------

	// �U�������ǂ����̃`�F�b�N

	if (m_isAttacking && m_model->isAnimationEnd(1))
	{
		m_model->ChangeAnimation(1, 9, 0.2f);
		m_model->ChangeAnimation(0, 9, 0.2f);
		m_isAttacking = false;
		m_isChargeing = false;
		m_state = eState_Idle;

	}

	float speadscale = 1.0f;
	if (HOLD(CInput::eButton4) && m_stamina > 0)
	{
		m_stamina -= 0.2;
		speadscale = 1.5f;

	}

	//�`���[�W���̃X�^�~�i�񕜂Ńo�O�������
	if (m_stamina < m_stamina_max && tamekougeki)
		m_stamina += 0.5;
}


void Player::tameAttack()
{
	int animation = 0;
	CVector3D key_dir(0, 0, 0);

	HandleMovement(key_dir, animation);

	//�L�����N�^�[�̉�]�l���J�����̉�]�l�ɍ��킹��
	m_rot.y = Camera::Instance()->m_rot.y;

	// �L�[���͂������
	if (key_dir.LengthSq() > 0) {
		// ���ړ����� ��]�s��~�L�[����
		CVector3D dir = CMatrix::MRotationY(m_rot.y) * key_dir;
		m_model->ChangeAnimation(0, animation);
		m_velocity += dir * BaseSpead * move_speed;
	}
	else
	{
		//�L�[���͂��Ȃ����
		m_model->ChangeAnimation(0, 9, 0.2f);
	}

	m_model->ChangeAnimation(1, 4, false);



	if (m_isAttacking && m_model->isAnimationEnd(1))
	{
		m_model->ChangeAnimation(1, 9, 0.2f);
		m_model->ChangeAnimation(0, 9, 0.2f);
		m_isAttacking = false;
		m_isChargeing = false;
		m_ChargeTime = 0.0f;
		m_state = eState_Idle;
	}
}

void Player::Down()
{
	//SetKill();
	m_model->ChangeAnimation(12, false);
}

void Player::HandleMovement(CVector3D& key_dir, int& animation)
{
	// �d���ɂ��ړ����x�̒���
	float weightFactor = m_weight / 150.0f;
	float adjustedSpeed = move_speed * (1.0f - weightFactor); // �d���ɉ����Ĉړ����x�𒲐�

	// �L�[���͏���
	if (HOLD(CInput::eUp)) {
		key_dir.z = 0.9 * adjustedSpeed;
		animation = 7; // �O�i�A�j���[�V����
	}
	if (HOLD(CInput::eDown)) {
		key_dir.z = -0.8 * adjustedSpeed;
		animation = 8; // ��ރA�j���[�V����
	}
	if (HOLD(CInput::eLeft)) {
		key_dir.x = 0.8 * adjustedSpeed;
		animation = 1; // ���ړ��A�j���[�V����
		m_footstepInterval = 0.6f; // �����̊Ԋu��ݒ�

	}
	if (HOLD(CInput::eRight)) {
		key_dir.x = -0.8 * adjustedSpeed;
		animation = 6; // �E�ړ��A�j���[�V����
		m_footstepInterval = 0.6f;
	}

	// �_�b�V������
	if (HOLD(CInput::eButton4) && m_stamina > 0) {
		if (m_isMove) m_stamina -= 0.2;
		key_dir *= 1.5f; // �_�b�V�����x
		m_isDashing = true;
	}
	else {
		m_isDashing = false;
	}

	//�W�����v
	if (HOLD(CInput::eButton5) && m_isJump && m_isGround && m_stamina > 0) {
		m_isJump = false; // �W�����v���t���O���Z�b�g
		m_isGround = false; // �n�ʔ�������Z�b�g
		m_vec.y = 0.2f; // �W�����v�͂�ݒ�
		m_stamina -= 15; // �X�^�~�i����
	}

	if (m_isAction)
	{
		if (m_JumpDelay > 0.5)
		{
			m_vec.y = 0.20;
			m_stamina -= 15;
			m_isAction = false;
		}
	}
	else
	{
		m_JumpDelay = 0;
	}


	if (m_isDashing) {
		m_footstepInterval = 0.3f; // �_�b�V�����͊Ԋu��Z��
	}
	else {
		m_footstepInterval = 0.4f; // �ʏ�̕��s
	}

	// �����^�C�}�[���X�V
	if (m_footstepTimer > 0.0f) {
		m_footstepTimer -= CFPS::GetDeltaTime();
	}
}



Player::~Player()
{
	//�Q�[�W�I�u�W�F�N�g�j��
	if (m_gauge) m_gauge->SetKill();

}

Player* Player::Instance()
{

	return ms_instance;
}

//�A�C�e�����Ƃ̏Փ˔���
bool Player::CheckItemCollision(const CVector3D& dir)
{

	CVector3D newPos = m_pos + dir * move_speed;
	CVector3D start = m_pos + CVector3D(0, 1, 0);
	CVector3D end = start + dir * 1.2f;

	//���Ղ���̕\��
	//Utility::DrawCapsule(start, end, m_rad, CVector4D(0, 1, 0, 0.5));

	CVector3D hitPos;
	CVector3D hitNormal;

	std::vector<Base*> list = FindObjects(eWeapon);
	std::vector<Base*> list1 = FindObjects(eEnemy);
	std::vector<Base*> list2 = FindObjects(eItem);

	//���X�g�̌���
	list.insert(list.end(), list1.begin(), list1.end());
	list.insert(list.end(), list2.begin(), list2.end());

	for (Base* b : list)
	{
		if (Enemy* enemy = dynamic_cast<Enemy*>(b))
			if (enemy->GetHP() > 0)continue;

		if (Spider* spider = dynamic_cast<Spider*>(b))
			if (spider->GetHP() > 0)continue;

		if (GreenDragon* greendragon = dynamic_cast<GreenDragon*>(b))
			if (greendragon->GetHP() > 0)continue;

		if (Carry* carry = dynamic_cast<Carry*>(b))
		{
			if (carry->m_stateItem != Carry::e_Drop)continue;


			//�J�v�Z�����m�̏Փ�
			if (CCollision::CollisionCapsule(b->m_lineS, b->m_lineE, b->m_rad,
				start, end, m_rad))
			{

				if (m_isEkeyPressd)
				{

					SetCarry(carry);
					m_isEkeyPressd = false;
					return true;
				}

				FONT_T()->Draw(800, 680, 1.0f, 1.0f, 1.0f, "Pickup Ekey");
				//printf("Item");
				//m_state = eState_Idle;
				return true;

			}
		}
	}


	return false;
}

void Player::Update()
{

	// === �ޗ��`�F�b�N���� ===
	const float FALL_THRESHOLD = -20.0f; // �ޗ��Ƃ݂Ȃ�����
	if (m_pos.y <= FALL_THRESHOLD)
	{
		// �����ʒu�ɖ߂�
		m_pos = initialPosition;
		m_velocity = CVector3D(0.0f, 0.0f, 0.0f); // �������x�����Z�b�g
		printf("�ޗ����畜�A���܂���\n"); // �f�o�b�O�p���O
	}

	//��]�l��������x�N�g�����v�Z
	//CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
	CheckItemCollision(CMatrix::MRotation(Camera::Instance()->m_rot).GetFront());

	CVector3D flashlightPos = m_pos + m_dir * 1.0f; // 1.0f�͑O���I�t�Z�b�g����

	//���C�g�ݒ�
	CVector3D dir = CMatrix::MRotation(Camera::Instance()->m_rot).GetFront();
	CLight::SetType(0, CLight::eLight_Spot);
	CLight::SetType(1, CLight::eLight_Point);
	CLight::SetDir(0, dir);
	CLight::SetRadiationAngle(0, DtoR(35));
	CLight::SetPos(0, m_model->GetFrameMatrix(8).GetPosition() + dir * 0.2f);
	CLight::SetPos(1, m_model->GetFrameMatrix(8).GetPosition() + dir * 0.2f);
	//CVector3D flashlightDir = dir;  // �v���C���[�̕����x�N�g�����擾
	//CLight::SetDir(1, flashlightDir.GetNormalize()); // ���C�g�̕�����ݒ�
	CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.7f, 0.7f, 0.7f));
	CLight::SetColor(0, CVector3D(0.0f, 0.0f, 0.0f), CVector3D(0.8f, 0.8f, 0.8f));
	CLight::SetRange(1, 2.5f);
	CLight::SetRange(0, 20);
	//CLight::SetSpotCutoff(1, 30.0f);
	//CLight::SetSpotExponent(1, 2.0f);
	//CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.6f, 0.6f, 0.6f));
	//�}�b�N�X�l��菊���������Ȃ�������
	//�����𑝂₷
	if (PlayerMoneyMax > PlayerMoney)
	{
		PlayerMoney++;
	}
	//�}�b�N�X�l��菊����������������
	//���������炷
	if (PlayerMoneyMax < PlayerMoney)
	{
		PlayerMoney--;
	}



	if (m_hp > 0)
	{
		//�������̃t�H���g
		//FONT_T()->Draw(1500, 712, 1.0f, 1.0f, .0f, "������%d", PlayerMoney);

		FONT_T()->Draw(0, 100, 0.0f, 0.0f, .0f, "�}�E�X�X�N���[���ň�l��/�O�l�̐؂�ւ�");
	}


	m_isEkeyPressd = PUSH(CInput::eButton2);
	m_isSiftPressd = HOLD(CInput::eButton4);
	m_isSpacePressd = PUSH(CInput::eButton5);
	m_isLclickPressd = PUSH(CInput::eMouseL);

	//�W�����v�C���^�[�o��
	jump_interval -= 1.0f;

	//���G����--
	mutekijikan -= 1.0f;
	threadattachtimer -= 1.0f;

	if (m_isAction)
	{
		m_JumpDelay += 0.08;
	}

	//�X�^�~�i��
	if (m_stamina < m_stamina_max && !m_isSiftPressd && !m_isSpacePressd && m_state != eState_Attack00 && m_state != eState_Attack01)
	{
		count++;
		if (count >= 25)
			m_stamina += 1.0;
	}
	else
	{
		count = 0;
	}

	//�m�b�N�o�b�N��
	if (m_isKnockback)
	{
		// ������я����̌p��
		m_pos += m_velocity * CFPS::GetDeltaTime(); // ���x���ʒu�ɓK�p
		m_velocity *= 0.9f; // ���X�Ɍ���

		// ������ю��Ԃ��I�����������
		m_knockbackTimer -= CFPS::GetDeltaTime();
		if (m_knockbackTimer <= 0.0f)
		{
			m_isKnockback = false;
			m_velocity = CVector3D(0, 0, 0); // ���x�����Z�b�g
		}
	}

	//���S�������Ƀ^�C�g���ɖ߂�
	if (m_hp <= 0)
	{
		m_state = eState_Down;
		GameOvercount += 1;
		if (GameOvercount > 230)
		{
			BlackFilter* b = dynamic_cast<BlackFilter*>(Base::FindObject(eRedFilter));
			if (b == __nullptr)
				Base::Add(b = new BlackFilter());
			b->BlackShow(130.0f);
		}
		if (GameOvercount > 360)
		{
			SceneManager::LoadTitle();//�^�C�g���ɖ߂�
		}

	}


	if (m_hp > 0)
	{
		//�L�����N�^�[�̉�]�l���J�����̉�]�l�ɍ��킹��
		m_rot.y = Camera::Instance()->m_rot.y;

		m_model->BindFrameMatrix(5, CMatrix::MRotation(Camera::Instance()->m_rot));

		m_velocity *= m_friction;

		m_pos += m_velocity;
	}

	//�n�ʂ��痣��Ă���ꍇ�̏���
	if (!m_isGround)
	{
		m_isFalling = true;
	}

	if (m_weight <= 0)
	{
		m_weight = 0.0f;
	}
	float GravityEffect = GRAVITY + ((m_weight / 2000.0f));

	//�d�͗���
	m_pos += m_vec;
	m_vec.y -= GravityEffect;
	m_vec.x *= 0.95;
	m_vec.z *= 0.95;

	if (m_carry)
	{
		FONT_T()->Draw(800, 712, 1.0f, 1.0f, 1.0f, "Drop Fkey");
	}

	if (PUSH(CInput::eButton3) && m_carry)
	{
		if (m_isAttacking)
		{
			return;
		}

		//	if (eWeapon)
		//		SOUND("�����Ƃ�")->Play(false);
		DropCarry(m_carry);

	}

	if (m_state == eState_Down && m_carry)
	{
		DropCarry(m_carry);
	}

	UpdateState();

	m_model->UpdateAnimation();

	// �����Ԃ���K�p
	if (m_pushBackForce.LengthSq() > 0)
	{
		m_pos += m_pushBackForce; // �����Ԃ��̗͂�K�p
		m_pushBackForce *= 0.9f; // �͂�����������i���킶��Ɖ����o����銴���j
	}

	//��ʍ����ɃQ�[�W�̈ʒu��ݒ�
	if (m_gauge) {
		m_gauge->m_pos = CVector3D(0, 900, 0);
		m_gauge->SetValue((float)m_hp / m_hp_max);
	}
	if (m_gauge1) {
		m_gauge1->m_pos = CVector3D(0, 980, 0);
		m_gauge1->SetValue((float)m_stamina / m_stamina_max);
	}

}

bool Player::GetAttack()
{
	return m_isAttacking;
}

bool Player::GetAttackRelease() const
{
	return m_isAttackReleased;
}

CVector3D Player::GetPos()
{
	return m_pos;
}

bool Player::GetMove()
{
	return m_isMove;
}

bool Player::GetCarry()
{
	return m_carry;
}

bool Player::GetDashing()
{
	return m_isDashing;
}

//��������ϓ�������֐�����
int Player::Pay(int Money)
{
	return PlayerMoneyMax -= Money;
}

void Player::Draw()
{
	CImage& img = m_inventory_image; // �X���b�g�摜
	CVector2D base_pos(540, 960);   // �X���b�g�\���̊�ʒu
	const int icon_size = 96;       // �X���b�g1�̃T�C�Y
	const int src_size = 128;       // �X���b�g�摜����1�A�C�R���̃T�C�Y
	img.SetSize(icon_size, icon_size);

	for (int i = 0; i < m_maxItems; i++) {
		CVector2D slot_pos = base_pos + CVector2D((icon_size + 10) * i, 0);

		// 1. �w�i��`��
		img.SetPos(slot_pos);
		img.SetRect(0, 0, src_size, src_size); // �w�i�p�̉摜�͈�
		img.SetColor(0.2f, 0.2f, 0.2f, 1.0f); // �f�t�H���g�w�i�F�i�Â��O���[�j
		img.Draw();

		// �A�C�e��������ꍇ�͔w�i���������邭
		if (m_inventory[i]) {
			img.SetColor(0.4f, 0.4f, 0.4f, 1.0f); // ���邢�O���[
			img.Draw();
		}

		// 2. �X���b�g�g��`��i�I����Ԃ��n�C���C�g�j
		img.SetRect(0, 0, src_size, src_size); // �X���b�g�g
		if (m_inventory[i] && m_inventory[i] == m_carry) {
			img.SetColor(1.0f, 1.0f, 0.0f, 1.0f); // �I�𒆂̘g�F�i���F�j
		} else {
			img.SetColor(0.6f, 0.6f, 0.6f, 1.0f); // �ʏ�g�F
		}
		img.Draw();

		// 3. �A�C�e����`��
		if (m_inventory[i]) {
			int idx = 2 + m_inventory[i]->m_item_id; // �A�C�e��ID�ɉ������A�C�R��
			img.SetRect(0, idx * src_size, src_size, (idx + 1) * src_size);
			img.SetColor(1.0f, 1.0f, 1.0f, 1.0f); // �A�C�R���͖��邭�`��
			img.Draw();
		}

		// 4. �X���b�g�ԍ���`��
		FONT_T()->Draw(slot_pos.x + 36, slot_pos.y + 100, 0.8f, 0.8f, 1.0f, "%d", i + 1);
	}
}

int Player::GetPlayerMoney()
{
	return PlayerMoneyMax;
}

void Player::GainMoney(int Money)
{
	PlayerMoneyMax += Money;

}

void Player::Heal(int HealValue)
{
	m_hp += HealValue;
	if (m_hp > m_hp_max) m_hp = m_hp_max;

}

void Player::MovespeedUp(float speedUp)
{
	move_speed += speedUp;
}

bool Player::PlayerJump(bool canjump)
{
	return m_isJump = canjump;
}

void Player::ApplyFallDamage()
{
	//�����_���[�W���������鑬�x�̓��l
	float fallDamageValue = -0.5f; //���̒l��葬���ƃ_���[�W

	//�v���C���[���n�ʂɒ��n�����Ƃ��ɁA�������x�Ɋ�Â��ă_���[�W���v�Z
	if (m_vec.y < fallDamageValue)
	{
		//���x�ɉ������_���[�W���v�Z
		float fallSpeed = m_vec.y;
		float damage = fallSpeed * -75.0f;

		//�v���C���[�Ƀ_���[�W��K�p
		m_hp -= damage;
		RedFilterDisplay();

		//HP��0�ɂȂ�����ꍇ�̏���
		if (m_hp <= 0)
		{
			m_hp = 0;
			//���S����
			m_state = eState_Down;
		}
	}

}

// �v���C���[�ɒS������
void Player::SetCarry(Carry* carry)
{
	if (m_carry) {
		m_carry->PutInPocket();  //���̑������O��
	}
	//�A�C�e�����E��
	if (!PickUpItem(carry)) {
		//�E���Ȃ�������
		// ���łɎ����Ă���A�C�e��������΁A�̂Ă�
		if (m_carry) {
			m_carry->Unequip();
			// �A�C�e����������ɔz�u�i�h���b�v�j
			m_carry->m_pos = m_pos + CVector3D(0, 0.8f, 0);
			// �C���x���g������폜
			auto it = std::find(m_inventory.begin(), m_inventory.end(), m_carry);
			if (it != m_inventory.end()) {
				*it = nullptr;
			}
			//�������E��
			PickUpItem(carry);
		}
	}
	m_carry = carry;
	if (m_carry) {
		m_carry->Equip();  // �V�����A�C�e���𑕔�
		m_isCarrying = true;
		carry->m_stateItem != Carry::e_Equip;
	}
}



float Player::GetHp()
{
	return m_hp;
}

float Player::GetDamage(float damage)
{
	return m_hp -= damage;
}



CVector3D Player::GetRotation() const
{
	return m_rot;
}

void Player::SetRotation(const CVector3D& rotation)
{
	m_rot = rotation;
}

void Player::UpdateModelRotation(const CVector3D& rotation)
{
	m_model->BindFrameMatrix(5, CMatrix::MRotation(rotation));
}


// �A�C�e�����h���b�v����
void Player::DropCarry(Carry* carry)
{
	if (m_carry && m_carry == carry) {
		// ���݂̃A�C�e���̑�������
		m_carry->Unequip();
		m_carry->m_stateItem = Carry::e_Drop; // ��Ԃ��h���b�v�ɐݒ�
		m_isCarrying = false;

		// �A�C�e����������ɔz�u�i�h���b�v�j
		m_carry->m_pos = m_pos + CVector3D(0, 0.8f, 0);


		// �C���x���g������폜
		auto it = std::find(m_inventory.begin(), m_inventory.end(), carry);
		if (it != m_inventory.end()) {
			*it = nullptr;
		}

		m_carry = nullptr;
		// �C���x���g���ɑ��̃A�C�e��������ꍇ������������
		if (!m_isCarrying && m_carry == nullptr) {
			for (auto& item : m_inventory) {
				if (item) {
					m_carry = item;
					m_carry->Equip();
					m_carry->m_stateItem = Carry::e_Equip;
					m_isCarrying = true;
					break;
				}
			}
		}
	}
}


//
bool Player::PickUpItem(Carry* carry)
{
	int empty_idx = -1;
	int i = 0;
	//�󂫃X���b�g�𒲂ׂ�
	for (auto& c : m_inventory) {
		if (!c) {
			empty_idx = i;
			break;
		}
		i++;
	}
	//�󂫂������
	if (empty_idx >= 0) {
		m_inventory[empty_idx] = carry;
		carry->PutInPocket();
		return true;
	}
	else {
		// �����A�C�e����������ɒB���Ă���
		return false;
	}
}

void Player::SwapItem(int idx)
{
	//�X���b�g����Ȃ牽�����Ȃ�
	if (!m_inventory[idx])
	{
		return;
	}

	//���ɑI������Ă���ꍇ���������Ȃ�
	if (m_inventory[idx] == m_carry) return;
	
	//���ݑ������̃A�C�e�����|�P�b�g�ɖ߂��B
	if (m_carry) {
		// ���݂̃A�C�e���̑�������
		m_carry->PutInPocket();
	}

	// �C���x���g���̃A�C�e���Ɠ���ւ�
	m_carry = m_inventory[idx];
	m_carry->Equip();

}

void Player::SelectItem()
{
	for (int i = 0; i < m_inventory.size(); i++) {
		if (PUSH((CInput::E_BUTTON)(CInput::eNum1 + i))) {
			SwapItem(i);
		}
	}
}

//�d���֘A
void Player::SetWeight(float weight)
{
	m_weight = weight;
}

float Player::GetWeight() const
{
	return m_weight;
}

// ���̏d����ۑ�����֐�
void Player::SaveOriginalWeight() {
	m_originalWeight = m_weight;
}


// �d�������ɖ߂��֐�
void Player::ResetToOriginalWeight() {
	if (m_inventory.size() <= 0)
		m_weight = m_originalWeight;
}

/*
void Player::Sell(Enemy * enemy)
{
	int money = enemy->GetSellValue();
	m_moneys.AddMoney(money);
	enemy->SetKill();
}
*/

//�W�F�b�g�p�b�N�֘A
void Player::JetPackOn()
{
	if (m_jetpackFuel > 0.0f)
	{
		m_isJetpackActive = true;
	}
}

void Player::JetPackOff()
{
	m_Force = 0.0f;
	m_isJetpackActive = false;
}

void Player::JetPackFuel(int shouhiryou)
{
	if (m_jetpackFuel > 0.0f)
	{
		m_jetpackFuel -= shouhiryou;
		if (m_jetpackFuel <= 0.0f)
		{
			m_jetpackFuel = 0.0f;
			JetPackOff();
		}
	}
}
bool Player::GetjetpackActive()
{
	return m_isJetpackActive;
}

//�`��
void Player::Render()
{
	m_model->SetPos(m_pos);
	m_model->SetRot(m_rot);
	m_model->SetScale(0.01f, 0.01f, 0.01f);
	m_model->UpdateMatrix();
	m_model->Render();

	printf("%f\n", m_Force);

	m_lineS = m_pos + CVector3D(0, 1.8f - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 0, 1, 0.5));

	//�W�F�b�g�p�b�N�̔R����\��������
	if (m_isJetpackActive)
	{
		FONT_T()->Draw(200, 200, 1.0f, 0.0f, 0.0f, "�R�� %d", m_jetpackFuel);
	}

	// �v���C���[�ƃJ�����̋������v�Z
	float DistanceToCamera = Camera::Instance()->GetDistance();

	// �������\���ɂ��鋗����臒l
	float hideHeadDistance = 0.2f;

	// �������\���ɂ��������ݒ�
	bool hideHead = (DistanceToCamera < hideHeadDistance);

	if (hideHead)
	{
		m_model->GetNode(69)->SetVisibility(false);

	}
	else
	{
		m_model->GetNode(69)->SetVisibility(true);
	}

}

void Player::Collision(Base* b)
{

	switch (b->GetType()) {

	case eEnemy:
		if (Enemy* e = dynamic_cast<Enemy*>(b))
		{
			int EnemyHp = e->GetHP();
			if (EnemyHp > 0)
			{
				//�J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsule(e->m_lineS, e->m_lineE, e->m_rad,
					m_lineS, m_lineE, m_rad))
				{
					// �����Ԃ��̌v�Z
					CVector3D pushBackDir = m_pos - e->GetPos(); // �G����v���C���[�ւ̕����x�N�g��
					pushBackDir.Normalize(); // ���K�����ĕ����x�N�g���ɂ���

					// �����Ԃ��̗͂�ݐς���
					m_pushBackForce += pushBackDir * 0.008; // �����𒲐�
				}
			}

			if (e->GetAttacking() && e->GetHP() > 0 && m_hp > 0)
			{
				{
					//�J�v�Z�����m�̏Փ�
					if (CCollision::CollisionCapsule(e->m_AttackS, e->m_AttackE, e->m_rad,
						m_lineS, m_lineE, m_rad))

					{

						if (mutekijikan <= 0)
						{
							int damage = e->GetDamage();

							m_hp -= damage;
							mutekijikan = 80.0f;

							RedFilterDisplay();

							// �����Ԃ��̌v�Z
							CVector3D pushBackDir = m_pos - e->GetPos(); // �G����v���C���[�ւ̕����x�N�g��
							pushBackDir.Normalize(); // ���K�����ĕ����x�N�g���ɂ���

							// �����Ԃ��̗͂�ݐς���
							m_pushBackForce += pushBackDir * m_pushBackStrength; // �����𒲐�


						}
					}

				}
			}
		}

		if (GreenDragon* g = dynamic_cast<GreenDragon*>(b))
		{
			{
				int EnemyHp = g->GetHP();
				if (EnemyHp > 0)
				{
					//�J�v�Z�����m�̏Փ�
					if (CCollision::CollisionCapsule(g->m_lineS, g->m_lineE, g->m_rad,
						m_lineS, m_lineE, m_rad))
					{
						// �����Ԃ��̌v�Z
						CVector3D pushBackDir = m_pos - g->GetPos(); // �G����v���C���[�ւ̕����x�N�g��
						pushBackDir.Normalize(); // ���K�����ĕ����x�N�g���ɂ���

						// �����Ԃ��̗͂�ݐς���
						m_pushBackForce += pushBackDir * 0.005; // �����𒲐�
					}
				}
			}
		}

		break;

	case eSpiderBullet:
	{
		if (SpiderBullet* w = dynamic_cast<SpiderBullet*>(b))
		{
			//�J�v�Z�����m�̏Փ�
			if (CCollision::CollisionCapsuleShpere(m_lineS, m_lineE, 0.4f, w->m_pos, w->m_rad))
			{
				if (threadattachtimer <= 0)
				{
					int damage = 2;

					m_hp -= damage;
					threadattachtimer = 180.0f;

					RedFilterDisplay();
				}
			}
		}

	}
	break;

	case eField: {
		CVector3D v(0, 0, 0);

		auto tri = b->GetModel()->CollisionCupsel(
			m_pos + CVector3D(0, 2.0f - m_rad, 0), // �n�_
			m_pos + CVector3D(0, m_rad, 0),       // �I�_
			m_rad
		);

		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				if (m_vec.y > 0) m_vec.y = 0; // �㏸�����Z�b�g
			}
			else if (t.m_normal.y > 0.5f) {
				ApplyFallDamage();
				if (m_vec.y < 0) m_vec.y = 0; // ���������Z�b�g

				// �W�����v�����n�ʂɒ��n�����ꍇ�̂݃t���O�����Z�b�g
				if (!m_isGround) {
					m_isGround = true;
					m_isJump = true;
				}
			}

			// �߂荞�ݕ��̉����߂��v�Z
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
			v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
		}

		// �����߂���K�p
		m_pos += v;
	} break;
	}

}
