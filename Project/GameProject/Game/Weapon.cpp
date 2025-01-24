#include "Weapon.h"
#include "Player.h"
#include "Enemy.h"
#include "Game/SpiderBullet.h"


Weapon::Weapon(bool Equippsed,const CVector3D& pos,float weight) :Carry(eWeapon)
{
	m_weight = weight;
	m_pos = pos;
	sword_model = COPY_RESOURCE("Sword", CModelObj);
	m_rad = 0.1;
	m_stateItem = Equippsed ? e_Equip : e_Drop;

	m_item_id = 0;
	
}

Weapon::~Weapon()
{

}

void Weapon::NotifyAttack()
{
	//�v���C���[�̍U����F��
	for (auto& enemy : Base::FindObjects(eEnemy))
	{
		if (Enemy* e = dynamic_cast<Enemy*>(enemy))
		{
			e->IncreaseAttackCount();
		}
	}
}

void Weapon::Update()
{

	if (m_stateItem==e_Equip)
	{
		Player* player = Player::Instance();
		if (!player) return;

		CMatrix player_bone = player->GetModel()->GetFrameMatrix(38);
		sword_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);

		m_pos = sword_matrix.GetPosition();
	}
	else if(m_stateItem == e_Drop)
	{
		//�d�͗���
		m_pos.y += m_vec.y;
		m_vec.y -= GRAVITY;
	}
	
}

void Weapon::Render()
{
	if (m_stateItem == e_Equip)
	{
	Player* player = Player::Instance();
	if (!player)return;
	bool m_isAttacking = Player::Instance()->GetAttack();

		CMatrix player_bone = player->GetModel()->GetFrameMatrix(38);
		sword_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);

		sword_model.Render(sword_matrix);

		if (m_isAttacking)
		{
			//���̃J�v�Z���`��
			m_rad = 0.1f;
			m_lineS = (sword_matrix * CMatrix::MTranselate(0, 0, 20)).GetPosition();
			m_lineE = (sword_matrix * CMatrix::MTranselate(0, 0, 150)).GetPosition();
			NotifyAttack();
		//	Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(1, 0, 0, 0.5));
		}
	}
	else if(m_stateItem == e_Drop)
	{

		
		sword_matrix = 
			 CMatrix::MTranselate(m_pos)
			* CMatrix::MRotation(m_rot)
			* CMatrix::MScale(0.005f, 0.005f, 0.005f);

		sword_model.Render(sword_matrix);

			//���̃J�v�Z���`��
			m_rad = 0.1f;
			m_lineS = (sword_matrix * CMatrix::MTranselate(0, 0, 20)).GetPosition();
			m_lineE = (sword_matrix * CMatrix::MTranselate(0, 0, 150)).GetPosition();
			//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(1, 0, 0, 0.5));
		
	}
}

void Weapon::Collision(Base*b)
{
	switch (b->GetType()) 
	{
	case eSpiderBullet:
		if (SpiderBullet* spiderBullet = dynamic_cast<SpiderBullet*>(b))
		{
			Player* player = Player::Instance();
			if (player && player->GetAttack())
			{
				if (CCollision::CollisionCapsule(m_lineS, m_lineE, m_rad,
					spiderBullet->m_lines[0], spiderBullet->m_lines.back(), 0.1f))
				{
					
					spiderBullet->Cut();
				}
			}
		}
		break;

	case eField:
		//���f���Ƃ̔���(��)(�J�v�Z��)
	{
		//�����߂���
		CVector3D v(0, 0, 0);
		//���ƃ��f���Ƃ̏Փ�
		//auto tri = stage_col.CollisionSphere(pos + CVector3D(0, rad, 0),rad);
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
	break;
	}
}


void Weapon::Equip()
{
	m_stateItem == e_Equip;
	m_pos = sword_matrix.GetPosition();

	
	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() + m_weight);
	}
}

void Weapon::Unequip()
{
	m_stateItem == e_Drop;
	m_pos = sword_matrix.GetPosition();

	
	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() - m_weight);
	}

}


void Weapon::Setpos(const CVector3D& pos)
{
	m_pos = pos;
}

float Weapon::GetWeight()
{
	return m_weight;
}



	