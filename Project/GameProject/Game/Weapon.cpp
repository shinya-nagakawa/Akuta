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
	//プレイヤーの攻撃を認識
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
		//重力落下
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
			//剣のカプセル描画
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

			//剣のカプセル描画
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



	