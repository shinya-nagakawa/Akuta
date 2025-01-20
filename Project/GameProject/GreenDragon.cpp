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

	//突進関連
	m_chargeSpeed = 6.0f;			// 突進速度
	MaxChargeDistance = 15.0f;		// 突進可能な最大距離
	m_chargeDistance = 0.0f;		// 突進中の移動距離
	m_isChargeing = false;			// 突進開始フラグ
	      

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
		//リストからプレイヤーを探索
		else if (Base* p = Base::FindObject(ePlayer))
		{

			count--;
			if (count <= 0)
			{
				UpdateAI();
			}

			//ターゲットへのベクトル
			CVector3D vec = p->m_pos - m_pos;
			//ベクトルから回転値を逆算
			m_rot_target.y = atan2(vec.x, vec.z);
			//回転値から方向ベクトルを計算
			CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
			//ターゲットへの距離を1.5ｆより遠ければ
			if (vec.Length() > 0.1f)
			{
				//前進
				m_pos += dir * m_moveSpeed;
				//前進アニメーション
				m_model.ChangeAnimation(11);
			}

			float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
			//割合による補間
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
		// ドラゴンの前方方向ベクトルを計算
		CVector3D forwardDir(sin(m_rot.y), 0, cos(m_rot.y));

		// 攻撃カプセルの範囲を設定（ドラゴンの位置を基準に前方オフセット）
		float attackRange = 1.6f;  // 攻撃の前方範囲
		float attackHeight = 1.5f; // 攻撃の高さ（カプセルの縦方向の長さ）
		float attackRadius = 1.0f; // 攻撃のカプセル半径
		CVector3D capsuleTop = m_pos + forwardDir * attackRange + CVector3D(0, attackHeight, 0);
		CVector3D capsuleBottom = m_pos + forwardDir * attackRange;

		// デバッグ用の可視化（攻撃範囲を描画）
		//Utility::DrawCapsule(capsuleTop, capsuleBottom, attackRadius, CVector4D(1, 0, 0, 0.5f));

		// プレイヤーとの衝突判定
		if (mp_player)
		{
			float dist;
			CVector3D c1, dir1, c2, dir2;
			if (CCollision::CollisionCapsule(
				capsuleTop, capsuleBottom, attackRadius,
				mp_player->m_pos + CVector3D(0, 1.0f, 0),  // プレイヤーのカプセル上端
				mp_player->m_pos,                          // プレイヤーのカプセル下端
				mp_player->m_rad,                         // プレイヤーのカプセル半径
				&dist, &c1, &dir1, &c2, &dir2))
			{
				// 衝突している場合、プレイヤーにダメージを与える
				mp_player->GetDamage(4); // 仮のダメージ値

				// プレイヤーを後方に吹き飛ばす
				float knockbackForce = 0.15f; // 吹き飛ばしの強さ
				CVector3D knockbackDir = forwardDir * 1; // ドラゴンの前方
				mp_player->ApplyKnockback(knockbackDir * knockbackForce);

				// カメラを揺らしてフィードバックを追加
				Camera::Instance()->Shake(0.03f, 0.5f);
			}
		}
	}

	// 攻撃アニメーションを再生
	m_model.ChangeAnimation(0, false);
	m_attack_no++;

	// 攻撃アニメーションが終了したら待機状態に戻る
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

//ダメージを受けた時の処理
void GreenDragon::Hit()
{
	//攻撃状態状態なら、怯まない
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

//ダメージを受けた時の処理
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

	//視野角度内か判定
	CVector3D dir = vec.GetNormalize();
	float dot = CVector3D::Dot(m_dir, dir);
	float angle = DtoR(m_viewAngle);
	if (dot < cosf(angle))return false;

	//視野範囲距離か判定
	float dist = vec.Length();
	if (dist > m_viewLength)return false;

	//プレイヤーとの間に遮蔽物がないかどうかチェックする
	if (!IsLookPlayer())
	{
		return false;
	}

	return true;
}

//壁との衝突判定
bool GreenDragon::CheckWallCollision(const CVector3D& dir)
{
	CVector3D newPos = m_pos + CVector3D(0, 1, 0);
	CVector3D start = newPos;
	CVector3D end = start + dir * 1.0f;

	//カプセルの表示
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

//空中へ
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
//ドラゴンなのでブレス攻撃を作成する。
void GreenDragon::breath()
{

	// ブレスアニメーション
	m_model.ChangeAnimation(4, false);

	
	if (m_model.GetAnimationFrame() == 15)
	{
		// ドラゴンの前方を計算
		CVector3D forwardDirection = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
		
		CVector3D poisonPosition = m_model.GetFrameMatrix(5)*CVector4D(0,0,100,1) ;

		// 毒霧を生成
		Base::Add(new PoisonBreath(poisonPosition, forwardDirection, 0.1, CVector4D(0.0f, 1.0f, 0.0f, 0.5f)));

	}

	if (m_model.isAnimationEnd())
	{
			m_isAttacking = false;
			m_state = eState_Land;
	}


}

//ドラゴンの突進攻撃を作成する
void GreenDragon::Charge()
{

	//回転値から方向ベクトルを計算
	CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
	//壁があるかのチェック
	if (CheckWallCollision(dir))
	{
		Camera::Instance()->Shake(0.07f, 1.0f); // 揺れの強さと時間
		m_state = eState_Idle;
		m_isChargeing = false;
		m_isAttacking = false;
		m_chargeDistance = 0.0f;
		m_vec = CVector3D(0, 0, 0);
		// 毒霧を生成
		Base::Add(new PoisonBreath(m_pos, dir, 0.1, CVector4D(0.0f, 1.0f, 0.0f, 0.5f)));
	}
	
	// 突進アニメーションの再生
	m_model.ChangeAnimation(6, true); // 突進のアニメーション
	
	if (m_isChargeing == false)
	{
		CVector3D forwardDirection = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
		m_vec = forwardDirection * m_chargeSpeed;
		m_isChargeing = true;
	}


	// 突進中の移動
	m_pos += m_vec * CFPS::GetDeltaTime();

	// アニメーション終了、または一定距離移動したら停止
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


//AIの更新
void GreenDragon::UpdateAI()
{

	Base* playerBase = Base::FindObject(ePlayer);
	if (playerBase) {
		CVector3D Player_vec = playerBase->m_pos - m_pos;
		float distance_Player = Player_vec.Length();
		count = 120;


		// 攻撃レンジ内
		if (distance_Player < AttackRange) {
			if (!m_isAttacking) {
				m_state = eState_Attack00; // 通常攻撃
				m_isAttacking = true;
				return;
			}
		}

		// 中距離で突進
		if (distance_Player < 10.5f && distance_Player > 1.5f) {
			if (!m_isChargeing && !m_isAttacking) {
				m_state = eState_Charge; // 突進攻撃
				return;
			}
		}

		//プレイヤーから攻撃される頻度が高かったらブレス

		// 遠距離で接近
		if (distance_Player > 10.5f) {
			m_state = eState_Move; // 接近
			return;
		}

		// 待機状態
		m_state = eState_Idle;
	}
}


void GreenDragon::Update()
{
	m_mutekijikan -= 0.016f;

	// フレームごとに押し返しの力をリセット
	m_pushBackForce = CVector3D(0, 0, 0);

	// 他の敵との衝突をチェック
	for (auto& b : Base::FindObjects(eEnemy))
	{
		if (b != this) {
			Collision(b);
		}

	}

	// 押し返しの力を位置に適用
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

	//アニメーション更新
	m_model.UpdateAnimation();

	if (m_stateItem == e_Drop)
	{
		//重力落下
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
		//位置調整用
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
				// カプセル同士の衝突
				if (CCollision::CollisionCapsule(e->m_lineS, e->m_lineE, e->m_rad,
					m_lineS, m_lineE, m_rad))
				{
					// 押し返しの計算
					CVector3D pushBackDir = m_pos - e->GetPos(); // 他の敵からこの敵への方向ベクトル
					pushBackDir.Normalize(); // 正規化して方向ベクトルにする

					if (m_hp > 0)
					{
						// 押し返しの力を両方の敵に適用する
						m_pushBackForce += pushBackDir * 0.05; // 強さを調整


					}
				}
			}
		}
		break;

	case eWeapon:
		//ウェポンとの当たり判定の追加をしたい。
		if (Weapon* d = dynamic_cast<Weapon*>(b))
		{
			CVector3D c1;
			float dist;

			if (Player::Instance()->GetAttack())
			{
				//カプセル同士の衝突
				if (CCollision::CollisionCapsule(d->m_lineS, d->m_lineE, d->m_rad,
					m_lineS, m_lineE, m_rad, &dist, &c1))
				{

					if (m_mutekijikan <= 0 && m_hp > 0)
					{
						//printf("hit");
						//m_state = eState_Hit;
						CVector3D pos = c1;
						//切りエフェクト
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
		//モデルとの判定(球)
		//押し戻し量
		CVector3D v(0, 0, 0);
		//球とモデルの衝突
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
		break;
	}
}

//プレイヤーにお金を与える
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
