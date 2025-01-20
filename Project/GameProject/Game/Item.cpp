#include "Item.h"
#include "Player.h"
#include "Effekseer/EffekseerEffect.h"

Item::Item(bool Equippsed, const CVector3D& pos,float weight) :Carry(eItem)
{
	m_weight = weight;
	m_pos = pos;
	m_model = COPY_RESOURCE("Jet", CModelA3M);
	m_rad = 0.1;
	m_stateItem = Equippsed ? e_Equip : e_Drop;

}

Item::~Item()
{

}

void Item::Update()
{


	if (m_stateItem == e_Equip)
	{
		Player* player = Player::Instance();
		if (!player) return;

		CMatrix player_bone = player->GetModel()->GetFrameMatrix(8);
		item_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);

		m_pos = item_matrix.GetPosition();
	}
	else
	{

		//�d�͗���
		m_pos.y += m_vec.y;
		m_vec.y -= GRAVITY;

	}
}

void Item::Render()
{
	if (m_stateItem == e_Equip)
	{
		Player* player = Player::Instance();
		if (!player)return;
		//bool m_isAttacking = Player::Instance()->GetAttack();

		//m_rot.y = Camera::Instance()->cam_rot.y;

		//m_model.BindFrameMatrix(2,CMatrix::MRotation(Camera::Instance()->cam_rot));


		CMatrix player_bone = player->GetModel()->GetFrameMatrix(8);
		//�ʒu�����p
		CVector3D offset(0, -1.0f, -0.1f);
		m_pos = player_bone.GetPosition()+ offset;
		m_rot = player->m_rot;


		m_model.SetScale(0.01f, 0.01f, 0.01f);

		item_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);

	}

	//�W�F�b�g�p�b�N���A�N�e�B�u��Ԃ��擾
	bool jetpackActive = Player::Instance()->GetjetpackActive();
	if (HOLD(CInput::eMouseL)&&jetpackActive&& m_stateItem == e_Equip)
	{
		count++;
		if(count ==8)
		Base::Add(new EffekseerEffect("Sword1", m_pos, CVector3D(0, 0, 0), CVector3D(0.3, 0.3, 0.3)));
		if (count >= 8)
		{
			count = 0;
		}
		
		//m_rot.y += count;
	}

	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(0.01f, 0.01f, 0.01f);
	if (m_stateItem != e_PickUp)
		m_model.Render();

	m_lineS = m_model.GetFrameMatrix(4).GetPosition();
	m_lineE = m_model.GetFrameMatrix(6).GetPosition();


	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));
	//if(m_isAttacking&&m_hp > 0)
	//Utility::DrawCapsule(m_AttackS, m_AttackE, m_rad, CVector4D(1, 0, 0, 0.5));

	if (m_stateItem == e_Equip)
	{
		Player::Instance()->JetPackOn();
	}
	else
	{
		Player::Instance()->JetPackOff();
	}
}

void Item::Collision(Base* b)
{
	switch (b->GetType())
	{
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

void Item::Equip()
{
	m_stateItem == e_Equip;
	m_pos = item_matrix.GetPosition();
	Player::Instance()->JetPackOn();
	Player* player = Player::Instance();
		if (player)
		{
			player->SetWeight(player->GetWeight() + m_weight);
		}
}

void Item::Unequip()
{
	m_stateItem == e_Drop;
	m_pos = item_matrix.GetPosition();
	Player::Instance()->JetPackOff();
	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() - m_weight);
	}

}



void Item::Setpos(const CVector3D& pos)
{
	m_pos = pos;
}