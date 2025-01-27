#include "Spider.h"
#include "Player.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Effekseer/EffekseerEffect.h"
#include "SpiderBullet.h"
#include "Field.h"

Spider::Spider(const CVector3D& pos, int enemy_Hp, float weight):Enemy_Base(pos,60.0f,12.0f,0.6f)
{
	m_model = COPY_RESOURCE("Spider", CModelA3M);
	m_pos = pos;
	m_rad = 0.6f;
	moveSpeed = 0.04f;
	m_hp = enemy_Hp;
	m_weight = weight;
	m_damage = 10;
	m_state = eState_Idle;
	m_previousState = eState_Idle;
	m_isAttacking = false;
	m_attackCooldown = 0.0f;
	m_attackCooldownMax = 3.0f;

	m_viewAngle = 180.0f;
	m_viewLength = 20.0f;
	AttackRange = 80.0f;
	m_gravity = 0.005;
	m_jumpCooldown = 0.0f;
	m_jumpCooldownMin = 3.0f;
	m_jumpCooldownMax = 7.0f;
	m_jumpProbability = 0.1f;	//�ǂɒ���t���Ă���Ƃ��ɃW�����v����m���i0.0f�@~�@1.0f�j

	m_debugAttachPos = CVector3D(0, 0, 0);
	m_debugAttachNormal = CVector3D(0, 0, 0);
	
	m_jumpDisableTime = 0.0f; // �W�����v���̒n�ʔ���𖳌������鎞��

	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));
	m_isCanJump = true;
	m_ground_normal = CVector3D(0, 1, 0);
	m_rot_matrix.SetIndentity();

	m_item_id = 2;

}

Spider::~Spider()
{
	// SellCount ���J�E���g�A�b�v
	Field* field = dynamic_cast<Field*>(Base::FindObject(eField));
	if (field) {
		field->IncrementSellCount();
	}
}

//�ړ���
void Spider::Idle()
{
	static float moveTimer = 0.0f;  // �ړ����̃^�C�}�[
	static float stopTimer = 0.0f;  // ��~���̃^�C�}�[
	static bool isStopped = true;   // ������Ԃ͒�~����J�n

	// �ǂ����m�����ꍇ�A����t����ԂɑJ��
	CVector3D attachPos, attachNormal;
	if (FindSurfaceToAmbush(attachPos, attachNormal)) {
		m_pos = attachPos;       // �ǂ̐ڐG�ʒu��ۑ�
		m_ground_normal = attachNormal; // �ǂ̖@����ۑ�
		m_rot_matrix = CalcRotMatrix(m_ground_normal);
		m_state = eState_Wall;		          // �ǒ���t����Ԃ�
		m_isWallAttached = true;
		return;                               // ������Idle�������I��
	}

	// �ʏ�̈ړ��E��~����
	if (IsFoundTarget()) {
		if (isStopped) {
			// ��~���̏���
			stopTimer -= CFPS::GetDeltaTime();
			if (stopTimer <= 0.0f) 
			{
				// ��~�I���A�ړ��J�n
				isStopped = false;
				moveTimer = Utility::Rand(1.2f, 3.5f);  //�ړ����Ԃ������_���ݒ�
				UpdateAI();
			}

			m_model.ChangeAnimation(4); // Idle�A�j���[�V����
			return;  // ��~���͉������Ȃ�
		}
		else {
			// �ړ����̏���
			moveTimer -= CFPS::GetDeltaTime();
			if (moveTimer <= 0.0f) {
				// �ړ��I���A��~�J�n
				isStopped = true;
				stopTimer = Utility::Rand(0.3f, 0.8f);  // ��~���Ԃ������_���ݒ�
				return;
			}

			// �v���C���[�����Ɉړ�
			if (Base* p = Base::FindObject(ePlayer)) {
				CVector3D vec = p->m_pos - m_pos;
				m_rot_target.y = atan2(vec.x, vec.z);
				CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
				float currentSpeed = moveSpeed * Utility::Rand(1.2f, 1.8f);
				m_pos += dir * currentSpeed;
				m_model.ChangeAnimation(1); // �O�i�A�j���[�V����
				m_se_cnt++;
				if (m_se_cnt % 16 == 0) {
					SOUND("����")->Play3D(m_pos, CVector3D::zero, false, true, EFX_REVERB_PRESET_CAVE);
				}

				float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
				m_rot.y += a * 0.2f;  // ��]�̑��x����������
			}
		}
	}
	else {
		// �^�[�Q�b�g��������Ȃ��ꍇ�͒ʏ��Idle�A�j���[�V����
		m_model.ChangeAnimation(4);
	}
}

//�U����Ԃ̊Ǘ�
void Spider::Attack()
{
	if (!mp_player) return; // �v���C���[�����Ȃ��ꍇ�͍U�����Ȃ�

	// �v���C���[�̈ʒu���擾
	CVector3D player_pos = mp_player->GetPos();

	if (thread == true)
	{
		const int numBullets = 4; // ���˂���e�̐�
		const float spreadRadius = 6.0f; // �v���C���[���͂̍L����i���a�j
		const float speed = 0.20f; // Web�e�̑��x

		// �v���C���[�𒆐S�ɒe������������
		for (int i = 0; i < numBullets; ++i) {
			// �v���C���[�̎��͂Ƀ����_���Ȓ��e�_�𐶐�
			float angle = DtoR(static_cast<float>(rand() % 360)); // 0�`360�x�̃����_���Ȋp�x
			float distance = static_cast<float>(rand()) / RAND_MAX * spreadRadius; // ���aspreadRadius���̃����_���ȋ���
			CVector3D offset(sin(angle) * distance, 0.0f, cos(angle) * distance); // �΍����v�Z

			// �e�̃^�[�Q�b�g�n�_�i�v���C���[�̎��́j
			CVector3D targetPos = player_pos + offset;

			// Web�e�𐶐�
			Base::Add(new SpiderBullet(this,m_pos, speed, targetPos));
		}

		// ���˃t���O�����Z�b�g
		thread = false;
	}

	// �U���A�j���[�V�����̍Đ�
	m_model.ChangeAnimation(2, false);
	m_attack_no++;

	// �v���C���[�Ƃ̋������v�Z
	CVector3D playerVec = mp_player->GetPos() - m_pos;
	float distanceToPlayer = playerVec.Length();

	

	// �A�j���[�V�������I���������Ԃ�O�̏�Ԃɖ߂�
	if (m_model.isAnimationEnd()) {
		m_isAttacking = false;
		m_state = m_previousState;
	}
}

void Spider::StateWall()
{
	const float move_speed = 0.05f;

	// �ǖʏ���ړ�
	CVector3D right = (m_rot_matrix * CMatrix::MRotationY(m_rot.y)*CVector3D(0,0,1));
	m_pos += right * move_speed;

	// �ǒ[���m�F
	m_ray_s = m_pos + (m_rot_matrix * CMatrix::MRotationY(m_rot.y)).GetUp() * 1.0f;
	m_ray_e = m_ray_s + (m_rot_matrix * CMatrix::MRotationY(m_rot.y)).GetUp() * -2.0f;
	CVector3D c, n;
	
	if (Field* field = dynamic_cast<Field*>(Base::FindObject(eField))) {
		if (!field->GetModel()->CollisionRay(&c, &n, m_ray_s, m_ray_e)) {
			m_rot_matrix = CMatrix::indentity;
			m_isWallAttached = false;
			m_state = eState_Idle;
			m_previousState = m_state;
			return;
		}
	}

	/*
	// �v���C���[�����E�O�ɏo���ꍇ������
	if (!IsFoundTarget() || (mp_player->GetPos() - m_pos).Length() > m_viewLength) {
		m_rot_matrix = CMatrix::indentity;
		m_isWallAttached = false;
		m_state = eState_Idle;
		return;
	}*/

	if (m_jumpCooldown <= 0.0f && m_isCanJump)
	{
		float randomValue = static_cast<float>(rand()) / RAND_MAX;
			if (randomValue < m_jumpProbability)
			{
				MoveToTarget();
				m_jumpCooldown = Utility::Rand(m_jumpCooldownMin, m_jumpCooldownMax);
				return;
			}
	}
	
	// �U���\�Ȃ�U��
	if (m_attackCooldown <= 0.0f && IsFoundTarget() && (mp_player->GetPos() - m_pos).Length() < AttackRange) {
		m_previousState = m_state;
		thread = true;
		m_isAttacking = true;
		m_state = eState_Attack00;
		m_attackCooldown = m_attackCooldownMax;
	}

	//�G�̕�������
	if (Base* e = Base::FindObject(ePlayer)) {
		//�v���C���[����G�ւ̃x�N�g��
		CVector3D v = e->m_pos - m_pos;
		//�v���C���[��Ԃ֕ϊ�
		v = m_rot_matrix.GetInverse() * v;
		//��]�l���t�Z
		m_rot.y = atan2(v.x, v.z);
	}

	m_model.ChangeAnimation(1);
}

//�G�̎��S
void Spider::Down()
{
	//m_isDead = true;

	if (m_stateItem != e_Equip) {
		m_model.ChangeAnimation(6, false);
	}
	else {
		m_model.ChangeAnimation(4, false);
	}

}

//�_���[�W���󂯂����̏���
void Spider::Damage()
{

	if (m_mutekijikan <= 0)
	{
		m_hp -= 1;

		if (m_hp <= 0)
		{
			m_rot_matrix.SetIndentity();
			m_state = eState_Down;
			//SetKill();
			//printf("sibou");
		}
	}

	m_mutekijikan = 1.6f;

}

//�_���[�W���󂯂����̏���
void Spider::Hit()
{

	//�U����Ԃ������͉�荞�ݏ�ԂȂ�A���܂Ȃ�
	if (m_state == eState_Attack00 )return;

	m_isAttacking = false;
	m_state = eState_Hit;
	m_model.ChangeAnimation(5, false);
	m_model.SetAnimationSpeed(2.0f);
	if (m_model.isAnimationEnd())
	{
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
	if (m_hp <= 0)
	{
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Down;
		//SetKill();
		//printf("sibou");
	}
}

void Spider::HandleJump()
{
	if (m_isJumping)
	{
		// ���������̈ړ� (�W�����v�����ɑ��x��K�p)
		m_vec.x = m_jumpDirection.x * m_jumpSpeedX;
		m_vec.z = m_jumpDirection.z * m_jumpSpeedX;

		// ���������̈ړ�
		m_vec.y = m_jumpSpeedY;
		m_jumpSpeedY -= m_gravity; // �d�͂�K�p

		// �ʒu���X�V
		m_pos += m_vec;

		// �n�ʂɓ��B������W�����v�I��
		if (IsTouchingGround())
		{
			m_isJumping = false;   // �W�����v�I��
			m_vec = CVector3D(0, 0, 0);  // �S���x���Z�b�g
			m_isCanJump = true;    // �W�����v�\��Ԃɖ߂�

		}
	}
}

bool Spider::IsTouchingGround()
{
	if (m_jumpDisableTime > 0.0f)
		return false; // �n�ʔ���𖳌�����

	CVector3D start = m_pos + CVector3D(0, -m_rad, 0);
	CVector3D end = m_pos + CVector3D(0, -m_rad - 0.5f, 0);

	CVector3D hitPos, hitNormal;

	if (Base* field = Base::FindObject(eField))
	{
		if (field->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			//Utility::DrawSphere(hitPos, 0.2f, CVector4D(1, 0, 0, 1)); // �Փ˓_��Ԃ����ŕ`��
			if (m_pos.y - m_rad > hitPos.y - 0.1f)
			{
				m_pos.y = hitPos.y + m_rad;
				if (m_vec.y < 0) m_vec.y = 0;
				return true;
			}
		}
	}
	return false;
}

// �W�����v�J�n���Ƀ^�C�}�[��ݒ�
void Spider::MoveToTarget()
{
	if (!mp_player || !m_isCanJump) return;

	// �v���C���[�����̃W�����v�x�N�g���v�Z
	CVector3D targetDir = mp_player->GetPos() - m_pos;
	targetDir.y = 0; // ���������̂ݍl��
	targetDir.Normalize();

	m_jumpDirection = targetDir;
	m_jumpSpeedX = 0.15f; // ���������̑��x
	float jumpHeight = 1.0f; // �ő�W�����v����
	m_jumpSpeedY = sqrt(2 * m_gravity * jumpHeight);

	// �W�����v�J�n���ɒn�ʔ���𖳌���
	m_jumpDisableTime = 0.2f; // 200ms�Ԓn�ʔ���𖳌���

	m_isJumping = true;
	m_isCanJump = false;
}

//�v���C���[�ɂ�����^����
void Spider::GiveMoney(int Value)
{
	Player::Instance()->GainMoney(Value);
}

//�ǂƂ̏Փ˔���
bool Spider::CheckWallCollision(const CVector3D& dir)
{
	CVector3D newPos = m_pos + dir * moveSpeed;
	CVector3D start = newPos;
	CVector3D end = start + dir * 1.0f;

	start.y = -3.5f;
	end.y = -3.5f;

	//Ray�̕\��
	//Utility::DrawCapsule(start, end, m_rad, CVector4D(0, 1, 0, 0.5));

	CVector3D hitPos;
	CVector3D hitNormal;
	if (Base* b = Base::FindObject(eField))
	{
		if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			//m_state = eState_Idle;
			return true;
		}
	}
	return false;
}


//�v���C���[��������
bool Spider::IsFoundTarget()
{
	//��]�l��������x�N�g�����v�Z
	m_dir =m_rot_matrix * (CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

	//�ǂ����邩�̃`�F�b�N
	if (CheckWallCollision(m_dir))
	{
		m_pos.y += moveSpeed;
	}

	if (!mp_player) return false;
	CVector3D playerPos = mp_player->m_pos;

	// �G����v���C���[�܂ł̃x�N�g�������߂�
	CVector3D vec = playerPos - m_pos;


	// �G�̐��ʃx�N�g�� �� �G����v���C���[�܂ł̃x�N�g�� ��
	// ���ς�����Ċp�x�����߂�
	float dot = CVector3D::Dot(m_dir, vec.GetNormalize());
	// ���߂��p�x(����)������p�x�������f����
	if (dot < cos(DtoR(m_viewAngle))) return false;

	// �G����v���C���[�܂ł̋��������߂�
	float length = vec.Length();
	// ���߂����������싗���������f����
	if (length > m_viewLength) return false;

	// �Ō�Ɏ��g����v���C���[�܂ł̊Ԃ�
	// �Օ������Ȃ������肷��
	if (!IsLookPlayer()) return false;

	/*
	//����1 �T���͈͂̊p�x�����ǂ���
		//�G�̐��ʕ����̃x�N�g���ƖڕW���܂ł̃x�N�g������ςƂ��Ċp�x�����߂�

	float dot = CVector3D::Dot(m_dir, vec.GetNormalize());
	if (dot < cosf(m_searchLength+100))return false;
	//�����Q�@�T���͈͂̋����Ȃ����ǂ���
	float length = vec.Length();
	if (length > m_searchLength)return false;
	//2�̏����𖞂������̂ŁAtrue ��Ԃ�
	*/
	return true;

}

bool Spider::FindSurfaceToAmbush(CVector3D& attachPos, CVector3D& attachNormal)
{
	// �v���C���[�Ɍ����������ɕǂ�T��
	m_ray_s = m_pos + CVector3D(0, 1.0f, 0); // �w偂̓��t��
	m_ray_e = m_ray_s + m_dir * 0.8f;         // �O���Ƀ��C��L�΂�

	CVector3D hitPos, hitNormal;
	if (Field* field = dynamic_cast<Field*>(Base::FindObject(eField))) {
		if (field->GetModel()->CollisionRay(&hitPos, &hitNormal, m_ray_s,m_ray_e)) {
			attachPos = hitPos;         // �ǂ̐ڐG�ʒu
			attachNormal = hitNormal;   // �ǂ̖@��
			return true;                // �ǂ����o����
		}
	}
	return false; // �ǂ�������Ȃ�����
}

CMatrix Spider::CalcRotMatrix(const CVector3D& normal) const
{
	CVector3D up = normal; // �ǂ̖@�����u��v�Ƃ���
	CVector3D forward = m_dir; // ���݂̈ړ�������O�����Ƃ���

	// forward���@���ƕ��s�łȂ����m�F���A����
	if (fabs(CVector3D::Dot(forward, up)) > 0.99f) {
		forward = CVector3D(0, 0, 1); // �قڐ����Ȃ��ւ̕�����ݒ�
	}

	// �ǂ̖@���Ɋ�Â��āu�E�����v���v�Z
	CVector3D right = CVector3D::Cross(forward, up).GetNormalize();

	// �������u�O�����v���Čv�Z
	forward = CVector3D::Cross(up, right).GetNormalize();

	// ��]�s����\�z
	CMatrix rotMatrix;
	rotMatrix.m[0][0] = right.x;    rotMatrix.m[0][1] = right.y;    rotMatrix.m[0][2] = right.z;
	rotMatrix.m[1][0] = up.x;       rotMatrix.m[1][1] = up.y;       rotMatrix.m[1][2] = up.z;
	rotMatrix.m[2][0] = forward.x;  rotMatrix.m[2][1] = forward.y;  rotMatrix.m[2][2] = forward.z;

	return rotMatrix;
}

void Spider::Ambush()
{
	if (IsFoundPlayer())
	{
		m_state = eState_Idle;
	}
}

bool Spider::IsLookPlayer() const
{
	if (Player* player = Player::Instance()) {
		CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // �G�̖ڐ��̍���
		CVector3D end = player->m_pos + CVector3D(0, 1.0f, 0); // �v���C���[�̖ڐ��̍���

		// �f�o�b�O�p: ���C������
		//Utility::DrawLine(start, end, CVector4D(1.0f, 1.0f, 0.0f, 1.0f)); // ���F�����ŕ`��

		CVector3D hitPos, hitNormal;
		if (Base* b = Base::FindObject(eField)) {
			if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
				// �v���C���[�܂ł̋��������Փ˓_����O�Ȃ王�����Ղ��Ă���
				if ((hitPos - start).Length() < (end - start).Length()) {
					//Utility::DrawSphere(hitPos, 0.2f, CVector4D(1.0f, 0.0f, 0.0f, 1.0f)); // �Ԃ����ŏՓ˓_��`��
					return false;
				}
			}
		}

		// �Օ����Ȃ�
		//Utility::DrawLine(start, end, CVector4D(0.0f, 1.0f, 0.0f, 1.0f)); // �΂̐��ŕ`��
		return true;
	}

	return false;
}

void Spider::RenderVision()
{
	const float stepAngle = 5.0f; // �����`�悷��p�x�̊Ԋu
	const CVector4D visionColor(0, 1, 0, 0.5f); // �ΐF
	const CVector4D detectionColor(1, 0, 0, 0.5f); // �ԐF

	// ����͈͂�`��
	for (float angle = -m_viewAngle / 2.0f; angle <= m_viewAngle / 2.0f; angle += stepAngle)
	{
		float rad = DtoR(angle) + m_rot.y; // �G�̉�]�Ɋ�Â��Ċp�x�𒲐�
		CVector3D dir = m_rot_matrix * CVector3D(sin(rad), 0, cos(rad));
		CVector3D endPos = m_pos + dir * m_viewLength; // ����͈͂̐�[
		//Utility::DrawLine(m_pos, endPos, visionColor);
	}

	// �v���C���[�����E���̏ꍇ�A�Ԃ����ŕ`��
	if (IsFoundPlayer())
	{
		CVector3D playerPos = mp_player->m_pos;
		//Utility::DrawLine(m_pos, playerPos, detectionColor);
	}
}


//AI�̍X�V
void Spider::UpdateAI()
{
	if (!mp_player) return;

	// �v���C���[�Ƃ̋������v�Z
	CVector3D playerVec = mp_player->GetPos() - m_pos;
	float distanceToPlayer = playerVec.Length();

	// �U���͈͓��Ȃ�U��
	if (distanceToPlayer < AttackRange && !m_isAttacking && !m_isJumping) {
		m_isAttacking = true;
		thread = true;
		m_state = eState_Attack00;
	}

	// �W�����v�͈͓��Ȃ�W�����v
	if (distanceToPlayer < 20.0f && m_isCanJump && m_jumpCooldown <= 0.0f) {
		m_rot_matrix.indentity;
		MoveToTarget(); // �W�����v����
		m_jumpCooldown = Utility::Rand(m_jumpCooldownMin, m_jumpCooldownMax); // �N�[���_�E���ݒ�
	}
}

void Spider::Update()
{
	m_dir = (m_rot_matrix* CMatrix::MRotationY(m_rot.y)).GetFront();
	//RenderVision();

	// �n�ʔ��薳���^�C�}�[�̍X�V
	if (m_jumpDisableTime > 0.0f)
		m_jumpDisableTime -= 0.016f;

	m_mutekijikan -= 0.016f;

	// �N�[���_�E��������

	if (m_attackCooldown > 0.0f)
	{
		m_attackCooldown -= CFPS::GetDeltaTime();
		if (m_attackCooldown < 0.0f)
			m_attackCooldown = 0.0f;
	}

	if (m_jumpCooldown > 0.0f)
	{
		m_jumpCooldown -= CFPS::GetDeltaTime();
		if (m_jumpCooldown < 0.0f)
			m_jumpCooldown = 0.0f;
	}
	

	// �����Ԃ��͂����Z�b�g
	m_pushBackForce = CVector3D(0, 0, 0);

	// ���̓G�Ƃ̏Փ˂��`�F�b�N
	for (auto& b : Base::FindObjects(eEnemy))
	{
		if (b != this) {
			Collision(b);
		}
	}

	/*
	if (m_state == eState_Ambush || m_state == eState_Wall)
	{
		Ambush();
	}*/

	// �����Ԃ��̗͂��ŏI�I�ɓK�p
	m_pos += m_pushBackForce;

	// ��Ԃɉ���������
	switch (m_state)
	{
	case eState_Idle:
	case eState_Move:
		Idle();
		break;
	case eState_Attack00:
		Attack();
		break;
	case eState_Hit:
		Hit();
		break;
	case eState_Down:
		Down();
		break;
	case eState_Wall:
		StateWall();
		break;
	}

	// �W�����v����
	HandleJump();

	// �A�j���[�V�����X�V
	m_model.UpdateAnimation();

	if (m_stateItem == e_Drop&&!m_isWallAttached)
	{
		// �d�͗���
		m_pos.y += m_vec.y;
		m_vec.y -= GRAVITY;
	}
}

void Spider::Render()
{
	if (m_stateItem == e_Equip)
	{
		Player* player = Player::Instance();
		if (!player)return;
		//bool m_isAttacking = Player::Instance()->GetAttack();

		//m_rot.y = Camera::Instance()->cam_rot.y;

		//m_model.BindFrameMatrix(2,CMatrix::MRotation(Camera::Instance()->cam_rot));


		CMatrix player_bone = player->GetModel()->GetFrameMatrix(14);
		//�ʒu�����p
		CVector3D offset(0, -0.6f, -0.1f);
		m_pos = player_bone.GetPosition() + offset;
		m_rot = player->m_rot;


		m_model.SetScale(0.004f, 0.004f, 0.004f);

		enemy_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);

		if (PUSH(CInput::eButton3))
		{
			m_pos + CVector3D(0.0f, 0.0f, 0.3f);
			Unequip();
		}

		if (m_hp <= 0)
		{
			m_model.ChangeAnimation(4);
		}

	}

	/*
	if (m_debugAttachPos != CVector3D(0, 0, 0)) // �f�o�b�O�p�̃^�[�Q�b�g������ꍇ
	{
		Utility::DrawSphere(m_debugAttachPos, 0.2f, CVector4D(1, 0, 0, 1)); // �Փ˓_�i�Ԃ����j
		Utility::DrawLine(m_debugAttachPos, m_debugAttachPos + m_debugAttachNormal, CVector4D(0, 1, 0, 1)); // �@���i�΂̐��j
	}*/
	

	CVector3D Scall(0.004, 0.004, 0.004);
	if (m_stateItem == e_Drop)
	Scall = CVector3D(0.005f, 0.005f, 0.005f);
	if(m_stateItem !=e_PickUp)
	m_model.Render(CMatrix::MTranselate(m_pos) * m_rot_matrix * CMatrix::MRotationY(m_rot.y) * CMatrix::MScale(Scall));
	


	m_lineS = m_model.GetFrameMatrix(4).GetPosition();
	m_lineE = m_model.GetFrameMatrix(6).GetPosition();

	/*
	Utility::DrawLine(m_ray_s, m_ray_e, CVector4D(1, 0, 0, 1));
	Utility::DrawLine(m_pos, m_pos + m_dir, CVector4D(0, 1, 0, 1));*/

	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));
	//if (m_isAttacking && m_hp > 0)
	//	Utility::DrawCapsule(m_AttackS, m_AttackE, m_rad-0.1, CVector4D(1, 0, 0, 0.5));
}

void Spider::Collision(Base* b)
{
	switch (b->GetType())
	{

	case eEnemy:
		if (Spider* otherSpider = dynamic_cast<Spider*>(b)) {
			// �J�v�Z�����m�̏Փ˔���
			float overlapDistance = 0.0f;
			CVector3D collisionPoint;

			if (CCollision::CollisionCapsule(
				otherSpider->m_lineS, otherSpider->m_lineE, otherSpider->m_rad,
				m_lineS, m_lineE, m_rad, &overlapDistance, &collisionPoint))
			{
				// �d�Ȃ�����x�N�g���̌v�Z
				CVector3D pushBackDir = m_pos - otherSpider->GetPos();
				pushBackDir.Normalize();

				// �����Ԃ��ʂ��v�Z
				overlapDistance = (m_rad + otherSpider->m_rad) - (m_pos - otherSpider->GetPos()).Length();

				if (overlapDistance > 0.0f&&m_hp >=1) {
					// �o�����ɉ����Ԃ���K�p
					CVector3D pushAmount = pushBackDir * (overlapDistance * 0.5f);
					m_pos += pushAmount; // �����������Ԃ�
					otherSpider->m_pos -= pushAmount; // ����������Ԃ�
				}
			}
		}
		break;

	case eWeapon:
		//�E�F�|���Ƃ̓����蔻��̒ǉ����������B
		if (Weapon* d = dynamic_cast<Weapon*>(b))
		{
			CVector3D c1;
			float dist;

			if (Player::Instance()->GetAttack())
			{
				//�J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsule(d->m_lineS, d->m_lineE, d->m_rad,
					m_lineS, m_lineE, m_rad, &dist, &c1))
				{

					if (m_mutekijikan <= 0 && m_hp > 0)
					{
						//printf("hit");
						//m_state = eState_Hit;
						CVector3D pos = c1;
						//�؂�G�t�F�N�g
						//Base::Add(new CEffect("Slash", pos, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 0.2f, 0.4f, -0.1f, CEffect::eBlendAlpha, 1, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 10));
						Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
						Damage();
						Hit();
					}
				}
			}
		}

		break;

	case eField:
		//���f���Ƃ̔���(��)
		//�����߂���
		CVector3D v(0, 0, 0);
		//���ƃ��f���̏Փ�
		//�J�v�Z���ƃ��f���̏Փ�
		auto tri = b->GetModel()->CollisionCupsel(m_pos + CVector3D(0, 2.0 - m_rad, 0),	//�n�_�i���j
			m_pos + CVector3D(0, m_rad, 0),		//�I�_�i�����j
			m_rad);

		//�ڐG�����ʂ̐��J��Ԃ�
		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				//�ʂ����������V��ɓ�������
				//�㏸���x��0�ɖ߂�
				if (m_vec.y > 0)
					m_vec.y = 0;
			}
			else if (t.m_normal.y > 0.5f) {
				//�ʂ���������n�ʂɓ�������
				//�d�͗������x��0�ɖ߂�
				if (m_vec.y < 0)
					m_vec.y = 0;
			}
			float max_y = max(t.m_vertex[0].y, max(t.m_vertex[1].y, t.m_vertex[2].y));
			//�ڐG�����ʂ̕����ցA�߂荞�񂾕������߂�
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			//�ł��傫�Ȉړ��ʂ����߂�
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			//�G���܂ł͏��z����
			//�G��ȏ�̕ǂ̂݉����߂����
			if (max_y > m_pos.y + 0.2f) {
				v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
				v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
			}
		}
		//�����߂�
		m_pos += v;
	}
}

void Spider::Equip()
{
	m_stateItem ==e_Equip;
}

void Spider::Unequip()
{
	m_stateItem == e_Drop;
}

int Spider::GetHP()
{
	return m_hp;
}

CVector3D Spider::GetPos()
{
	return m_pos;
}

int Spider::GetDamage()
{
	return m_damage;
}

bool Spider::GetAttacking()
{
	return m_isAttacking;
}