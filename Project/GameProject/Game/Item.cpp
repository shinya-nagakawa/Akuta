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

		//重力落下
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
		//位置調整用
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

	//ジェットパックがアクティブ状態か取得
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
		//モデルとの判定(球)(カプセル)
	{
		//押し戻し量
		CVector3D v(0, 0, 0);
		//球とモデルとの衝突
		//auto tri = stage_col.CollisionSphere(pos + CVector3D(0, rad, 0),rad);
		//カプセルとモデルの衝突
		auto tri = b->GetModel()->CollisionCupsel(m_pos + CVector3D(0, 2.0 - m_rad, 0),	//始点（頭）
			m_pos + CVector3D(0, m_rad, 0),		//終点（足元）
			m_rad);
		//接触した面の数繰り返す
		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				//面が下向き→天井に当たった
				//上昇速度を0に戻す
				if (m_vec.y > 0)
					m_vec.y = 0;
			}
			else if (t.m_normal.y > 0.5f) {
				//面が上向き→地面に当たった
				//重力落下速度を0に戻す
				if (m_vec.y < 0)
					m_vec.y = 0;
			}
			float max_y = max(t.m_vertex[0].y, max(t.m_vertex[1].y, t.m_vertex[2].y));
			//接触した面の方向へ、めり込んだ分押し戻す
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			//最も大きな移動量を求める
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			//膝下までは乗り越える
			//膝上以上の壁のみ押し戻される
			if (max_y > m_pos.y + 0.2f) {
				v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
				v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
			}
		}

		//押し戻す
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