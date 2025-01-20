#include "EnemyBase.h"]
#include "Weapon.h"
//#include "Effekseer/EffekseerEffect.h"
#include "NavManager.h"

EnemyBase::EnemyBase(const CVector3D& position, int hp, float weight,float moveSpeed):Carry(eEnemy)
{
	moveSpeed1 = moveSpeed;
	m_viewAngle = 180.0f;
	m_viewLength = 22.0f;
	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));
	m_rad = 0.4f;
}

bool EnemyBase::IsFoundTarget()
{
	//回転値から方向ベクトルを計算
	m_dir = (CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

	if (!mp_player) return false;
	CVector3D playerPos = mp_player->m_pos;

	// 敵からプレイヤーまでのベクトルを求める
	CVector3D vec = playerPos - m_pos;


	// 敵の正面ベクトル と 敵からプレイヤーまでのベクトル で
	// 内積を取って角度を求める
	float dot = CVector3D::Dot(m_dir, vec.GetNormalize());
	// 求めた角度(内積)が視野角度内か判断する
	if (dot < cos(DtoR(m_viewAngle))) return false;

	// 敵からプレイヤーまでの距離を求める
	float length = vec.Length();
	// 求めた距離が視野距離内か判断する
	if (length > m_viewLength) return false;

	// 最後に自身からプレイヤーまでの間に
	// 遮蔽物がないか判定する
	if (!IsLookPlayer()) return false;

	/*
	//条件1 探索範囲の角度内かどうか
		//敵の正面方向のベクトルと目標物までのベクトルを内積とって角度を求める

	float dot = CVector3D::Dot(m_dir, vec.GetNormalize());
	if (dot < cosf(m_searchLength+100))return false;
	//条件２　探索範囲の距離ないかどうか
	float length = vec.Length();
	if (length > m_searchLength)return false;
	//2つの条件を満たしたので、true を返す
	*/
	return true;

}

//壁との衝突判定
bool EnemyBase::CheckWallCollision(const CVector3D& dir)
{
	CVector3D newPos = m_pos + CVector3D(0, 1, 0);
	CVector3D start = newPos;
	CVector3D end = start + dir * 1.0f;

	//Rayの表示
	Utility::DrawCapsule(start, end, m_rad, CVector4D(0, 1, 0, 0.5));

	CVector3D hitPos;
	CVector3D hitNormal;
	if (Base* b = Base::FindObject(eField))
	{
		if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			m_pos -= dir * 0.2f;
			//m_state = eState_Idle;
			return true;
		}
	}
	return false;
}


bool EnemyBase::IsLookPlayer() const
{
	CVector3D playerPos = mp_player->m_pos;
	CVector3D vec = playerPos - m_pos;
	// 現在位置からプレイヤーまでの距離を求める
	float dist = vec.Length();

	// プレイヤーの位置までのレイと壁との衝突判定を行う
	CVector3D start = m_pos;
	CVector3D end = playerPos;
	start.y = 1.0f;
	end.y = 1.0f;
	CVector3D hitPos, hitNormal;
	if (Base* b = Base::FindObject(eField)) {
		if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			float hitDist = (hitPos - start).Length();
			if (dist > hitDist) return false;
		}
	}

	// 壁と衝突していないもしくは、
	// 衝突位置がプレイヤーより奥の位置であるならば、
	// 視線が通っているので、プレイヤーが見える状態
	return true;
}


