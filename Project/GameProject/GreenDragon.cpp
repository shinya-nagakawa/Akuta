#include "GreenDragon.h"
#include "Game/Enemy.h"
#include "PoisonBreath.h"
#include "Effekseer/EffekseerEffect.h"
#include "Game/Camera.h"

GreenDragon::GreenDragon(CVector3D& pos, int enemy_hp, float weight,float moveSpeed) :Enemy_Base(pos,enemy_hp,weight,moveSpeed)
{
	m_model = COPY_RESOURCE("GreenDragon", CModelA3M);
	m_pos = pos;
	m_hp = enemy_hp;
	m_weight = weight;
	m_rad = 0.8f;
	m_moveSpeed = moveSpeed;
	m_state = eState_Idle;
	AttackRange = 3.6f;
	m_isAttacking = false;
	m_viewAngle = 180.0f;
	m_viewLength = 22.0f;
	count = 120;
	m_isTakeOff = false;

	//�ːi�֘A
	m_chargeSpeed = 6.0f;			// �ːi���x
	MaxChargeDistance = 15.0f;		// �ːi�\�ȍő勗��
	m_chargeDistance = 0.0f;		// �ːi���̈ړ�����
	m_isChargeing = false;			// �ːi�J�n�t���O
	      

	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));

}

GreenDragon::~GreenDragon()
{
}

void GreenDragon::Idle()
{
	if (IsFoundPlayer())
	{
		if (m_hp == 3)
		{
			m_model.ChangeAnimation(13, true);
		}
		//���X�g����v���C���[��T��
		else if (Base* p = Base::FindObject(ePlayer))
		{

			count--;
			if (count <= 0)
			{
				UpdateAI();
			}

			//�^�[�Q�b�g�ւ̃x�N�g��
			CVector3D vec = p->m_pos - m_pos;
			//�x�N�g�������]�l���t�Z
			m_rot_target.y = atan2(vec.x, vec.z);
			//��]�l��������x�N�g�����v�Z
			CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
			//�^�[�Q�b�g�ւ̋�����1.5����艓�����
			if (vec.Length() > 0.1f)
			{
				//�O�i
				m_pos += dir * m_moveSpeed;
				//�O�i�A�j���[�V����
				m_model.ChangeAnimation(11);
			}

			float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
			//�����ɂ����
			m_rot.y += a * 0.1f;
		}
	}
	else
	{
		m_model.ChangeAnimation(13);
	}
}

void GreenDragon::Attack()
{
	if (m_isAttacking)
	{
		// �h���S���̑O�������x�N�g�����v�Z
		CVector3D forwardDir(sin(m_rot.y), 0, cos(m_rot.y));

		// �U���J�v�Z���͈̔͂�ݒ�i�h���S���̈ʒu����ɑO���I�t�Z�b�g�j
		float attackRange = 1.6f;  // �U���̑O���͈�
		float attackHeight = 1.5f; // �U���̍����i�J�v�Z���̏c�����̒����j
		float attackRadius = 1.0f; // �U���̃J�v�Z�����a
		CVector3D capsuleTop = m_pos + forwardDir * attackRange + CVector3D(0, attackHeight, 0);
		CVector3D capsuleBottom = m_pos + forwardDir * attackRange;

		// �f�o�b�O�p�̉����i�U���͈͂�`��j
		//Utility::DrawCapsule(capsuleTop, capsuleBottom, attackRadius, CVector4D(1, 0, 0, 0.5f));

		// �v���C���[�Ƃ̏Փ˔���
		if (mp_player)
		{
			float dist;
			CVector3D c1, dir1, c2, dir2;
			if (CCollision::CollisionCapsule(
				capsuleTop, capsuleBottom, attackRadius,
				mp_player->m_pos + CVector3D(0, 1.0f, 0),  // �v���C���[�̃J�v�Z����[
				mp_player->m_pos,                          // �v���C���[�̃J�v�Z�����[
				mp_player->m_rad,                         // �v���C���[�̃J�v�Z�����a
				&dist, &c1, &dir1, &c2, &dir2))
			{
				// �Փ˂��Ă���ꍇ�A�v���C���[�Ƀ_���[�W��^����
				mp_player->GetDamage(4); // ���̃_���[�W�l

				// �v���C���[������ɐ�����΂�
				float knockbackForce = 0.15f; // ������΂��̋���
				CVector3D knockbackDir = forwardDir * 1; // �h���S���̑O��
				mp_player->ApplyKnockback(knockbackDir * knockbackForce);

				// �J������h�炵�ăt�B�[�h�o�b�N��ǉ�
				Camera::Instance()->Shake(0.03f, 0.5f);
			}
		}
	}

	// �U���A�j���[�V�������Đ�
	m_model.ChangeAnimation(0, false);
	m_attack_no++;

	// �U���A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_model.isAnimationEnd())
	{
		m_state = eState_Idle;
		m_isAttacking = false;
	}
}

void GreenDragon::Down()
{
	if (m_stateItem != e_Equip) {
		m_model.ChangeAnimation(2, false);
	}
	else {
		m_model.ChangeAnimation(11, false);
	}
}

//�_���[�W���󂯂����̏���
void GreenDragon::Hit()
{
	//�U����ԏ�ԂȂ�A���܂Ȃ�
	if (m_state == eState_Attack00)return;

	m_isAttacking = false;
	m_state = eState_Hit;
	m_model.ChangeAnimation(8, false);
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

//�_���[�W���󂯂����̏���
void GreenDragon::Damage()
{

	if (m_mutekijikan <= 0)
	{
		m_hp -= 1;

		if (m_hp <= 0)
		{
			m_state = eState_Down;
			//SetKill();
			//printf("sibou");
		}
	}
	m_mutekijikan = 1.6f;

}

bool GreenDragon::IsFoundPlayer()
{
	if (mp_player == nullptr) return false;

	CVector3D playerPos = mp_player->m_pos;
	CVector3D vec = playerPos - m_pos;
	vec.y = 0.0f;

	//����p�x��������
	CVector3D dir = vec.GetNormalize();
	float dot = CVector3D::Dot(m_dir, dir);
	float angle = DtoR(m_viewAngle);
	if (dot < cosf(angle))return false;

	//����͈͋���������
	float dist = vec.Length();
	if (dist > m_viewLength)return false;

	//�v���C���[�Ƃ̊ԂɎՕ������Ȃ����ǂ����`�F�b�N����
	if (!IsLookPlayer())
	{
		return false;
	}

	return true;
}

//�ǂƂ̏Փ˔���
bool GreenDragon::CheckWallCollision(const CVector3D& dir)
{
	CVector3D newPos = m_pos + CVector3D(0, 1, 0);
	CVector3D start = newPos;
	CVector3D end = start + dir * 1.0f;

	//�J�v�Z���̕\��
	Utility::DrawCapsule(start, end, m_rad, CVector4D(0, 1, 0, 0.5));

	CVector3D hitPos;
	CVector3D hitNormal;
	if (Base* b = Base::FindObject(eField))
	{
		if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			m_pos -= dir * 0.2f;
			m_state = eState_Idle;
			return true;
		}
	}
	return false;
}

//�󒆂�
void GreenDragon::TakeOff()
{
	if (m_isTakeOff == false)
	{
		m_model.ChangeAnimation(15, false);
		if (m_model.isAnimationEnd())
		{
			m_isTakeOff = true;
			m_state = eState_Breath;
		}
	}
}

void GreenDragon::Land()
{
	if (m_isTakeOff == true)
	{
		m_model.ChangeAnimation(10, false);
		if (m_model.isAnimationEnd())
		{
			m_isTakeOff = false;
			m_state = eState_Idle;
		}
	}
}
//�h���S���Ȃ̂Ńu���X�U�����쐬����B
void GreenDragon::breath()
{

	// �u���X�A�j���[�V����
	m_model.ChangeAnimation(4, false);

	
	if (m_model.GetAnimationFrame() == 15)
	{
		// �h���S���̑O�����v�Z
		CVector3D forwardDirection = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
		
		CVector3D poisonPosition = m_model.GetFrameMatrix(5)*CVector4D(0,0,100,1) ;

		// �Ŗ��𐶐�
		Base::Add(new PoisonBreath(poisonPosition, forwardDirection, 0.1, CVector4D(0.0f, 1.0f, 0.0f, 0.5f)));

	}

	if (m_model.isAnimationEnd())
	{
			m_isAttacking = false;
			m_state = eState_Land;
	}


}

//�h���S���̓ːi�U�����쐬����
void GreenDragon::Charge()
{

	//��]�l��������x�N�g�����v�Z
	CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
	//�ǂ����邩�̃`�F�b�N
	if (CheckWallCollision(dir))
	{
		Camera::Instance()->Shake(0.07f, 1.0f); // �h��̋����Ǝ���
		m_state = eState_Idle;
		m_isChargeing = false;
		m_isAttacking = false;
		m_chargeDistance = 0.0f;
		m_vec = CVector3D(0, 0, 0);
		// �Ŗ��𐶐�
		Base::Add(new PoisonBreath(m_pos, dir, 0.1, CVector4D(0.0f, 1.0f, 0.0f, 0.5f)));
	}
	
	// �ːi�A�j���[�V�����̍Đ�
	m_model.ChangeAnimation(6, true); // �ːi�̃A�j���[�V����
	
	if (m_isChargeing == false)
	{
		CVector3D forwardDirection = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
		m_vec = forwardDirection * m_chargeSpeed;
		m_isChargeing = true;
	}


	// �ːi���̈ړ�
	m_pos += m_vec * CFPS::GetDeltaTime();

	// �A�j���[�V�����I���A�܂��͈�苗���ړ��������~
	m_chargeDistance += (m_vec * CFPS::GetDeltaTime()).Length();
	if (m_model.isAnimationEnd() || m_chargeDistance >= MaxChargeDistance)
	{
		m_state = eState_Idle;
		m_isAttacking = false;
		m_isChargeing = false;
		m_chargeDistance = 0.0f;
		m_vec = CVector3D(0, 0, 0);
	}
}


//AI�̍X�V
void GreenDragon::UpdateAI()
{

	Base* playerBase = Base::FindObject(ePlayer);
	if (playerBase) {
		CVector3D Player_vec = playerBase->m_pos - m_pos;
		float distance_Player = Player_vec.Length();
		count = 120;


		// �U�������W��
		if (distance_Player < AttackRange) {
			if (!m_isAttacking) {
				m_state = eState_Attack00; // �ʏ�U��
				m_isAttacking = true;
				return;
			}
		}

		// �������œːi
		if (distance_Player < 10.5f && distance_Player > 1.5f) {
			if (!m_isChargeing && !m_isAttacking) {
				m_state = eState_Charge; // �ːi�U��
				return;
			}
		}

		//�v���C���[����U�������p�x������������u���X

		// �������Őڋ�
		if (distance_Player > 10.5f) {
			m_state = eState_Move; // �ڋ�
			return;
		}

		// �ҋ@���
		m_state = eState_Idle;
	}
}


void GreenDragon::Update()
{
	m_mutekijikan -= 0.016f;

	// �t���[�����Ƃɉ����Ԃ��̗͂����Z�b�g
	m_pushBackForce = CVector3D(0, 0, 0);

	// ���̓G�Ƃ̏Փ˂��`�F�b�N
	for (auto& b : Base::FindObjects(eEnemy))
	{
		if (b != this) {
			Collision(b);
		}

	}

	// �����Ԃ��̗͂��ʒu�ɓK�p
	m_pos += m_pushBackForce;

	switch (m_state)
	{
	case eState_Idle:
	case eState_Move:
		Idle();
		break;
		break;
	case eState_Attack00:
		Attack();
		break;
	case eState_Breath:
		breath();
		break;
	case eState_Hit:
		Hit();
		break;
	case eState_Down:
		Down();
		break;
	case eState_TakeOff:
		TakeOff();
	case eState_Land:
		Land();
		break;
	
	case eState_Charge:
		Charge();
		break;
	}

	//�A�j���[�V�����X�V
	m_model.UpdateAnimation();

	if (m_stateItem == e_Drop)
	{
		//�d�͗���
		m_pos.y += m_vec.y;
		m_vec.y -= GRAVITY;

	}
}

void GreenDragon::Render()
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


		m_model.SetScale(0.002f, 0.002f, 0.002f);

		enemy_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);

	}

	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	if (m_stateItem == e_Drop)
		m_model.SetScale(0.005f, 0.005f, 0.005f);
	if (m_stateItem != e_PickUp)
		m_model.Render();

	m_lineS = m_model.GetFrameMatrix(7).GetPosition();
	m_lineE = m_model.GetFrameMatrix(53).GetPosition();


	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));
	//if (m_isAttacking && m_hp > 0)
	//Utility::DrawCapsule(m_AttackS, m_AttackE, m_rad-0.1, CVector4D(1, 0, 0, 0.5));
}

void GreenDragon::Collision(Base* b)
{
	switch (b->GetType())
	{
	case eEnemy:
		if (Enemy* e = dynamic_cast<Enemy*>(b)) {
			int EnemyHp = e->GetHP();
			if (EnemyHp > 0) {
				// �J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsule(e->m_lineS, e->m_lineE, e->m_rad,
					m_lineS, m_lineE, m_rad))
				{
					// �����Ԃ��̌v�Z
					CVector3D pushBackDir = m_pos - e->GetPos(); // ���̓G���炱�̓G�ւ̕����x�N�g��
					pushBackDir.Normalize(); // ���K�����ĕ����x�N�g���ɂ���

					if (m_hp > 0)
					{
						// �����Ԃ��̗͂𗼕��̓G�ɓK�p����
						m_pushBackForce += pushBackDir * 0.05; // �����𒲐�


					}
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
		break;
	}
}

//�v���C���[�ɂ�����^����
void GreenDragon::GiveMoney(int Value)
{
	Player::Instance()->GainMoney(Value);
}

CVector3D GreenDragon::GetPos()
{
	return m_pos;
}

int GreenDragon::GetHP()
{
	return m_hp;
}
