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
	m_jumpProbability = 0.1f;	//壁に張り付いているときにジャンプする確率（0.0f　~　1.0f）

	m_debugAttachPos = CVector3D(0, 0, 0);
	m_debugAttachNormal = CVector3D(0, 0, 0);
	
	m_jumpDisableTime = 0.0f; // ジャンプ中の地面判定を無効化する時間

	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));
	m_isCanJump = true;
	m_ground_normal = CVector3D(0, 1, 0);
	m_rot_matrix.SetIndentity();

	m_item_id = 2;

}

Spider::~Spider()
{
	// SellCount をカウントアップ
	Field* field = dynamic_cast<Field*>(Base::FindObject(eField));
	if (field) {
		field->IncrementSellCount();
	}
}

//移動等
void Spider::Idle()
{
	static float moveTimer = 0.0f;  // 移動中のタイマー
	static float stopTimer = 0.0f;  // 停止中のタイマー
	static bool isStopped = true;   // 初期状態は停止から開始

	// 壁を検知した場合、張り付き状態に遷移
	CVector3D attachPos, attachNormal;
	if (FindSurfaceToAmbush(attachPos, attachNormal)) {
		m_pos = attachPos;       // 壁の接触位置を保存
		m_ground_normal = attachNormal; // 壁の法線を保存
		m_rot_matrix = CalcRotMatrix(m_ground_normal);
		m_state = eState_Wall;		          // 壁張り付き状態へ
		m_isWallAttached = true;
		return;                               // ここでIdle処理を終了
	}

	// 通常の移動・停止処理
	if (IsFoundTarget()) {
		if (isStopped) {
			// 停止中の処理
			stopTimer -= CFPS::GetDeltaTime();
			if (stopTimer <= 0.0f) 
			{
				// 停止終了、移動開始
				isStopped = false;
				moveTimer = Utility::Rand(1.2f, 3.5f);  //移動時間をランダム設定
				UpdateAI();
			}

			m_model.ChangeAnimation(4); // Idleアニメーション
			return;  // 停止中は何もしない
		}
		else {
			// 移動中の処理
			moveTimer -= CFPS::GetDeltaTime();
			if (moveTimer <= 0.0f) {
				// 移動終了、停止開始
				isStopped = true;
				stopTimer = Utility::Rand(0.3f, 0.8f);  // 停止時間をランダム設定
				return;
			}

			// プレイヤー方向に移動
			if (Base* p = Base::FindObject(ePlayer)) {
				CVector3D vec = p->m_pos - m_pos;
				m_rot_target.y = atan2(vec.x, vec.z);
				CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
				float currentSpeed = moveSpeed * Utility::Rand(1.2f, 1.8f);
				m_pos += dir * currentSpeed;
				m_model.ChangeAnimation(1); // 前進アニメーション
				m_se_cnt++;
				if (m_se_cnt % 16 == 0) {
					SOUND("足音")->Play3D(m_pos, CVector3D::zero, false, true, EFX_REVERB_PRESET_CAVE);
				}

				float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
				m_rot.y += a * 0.2f;  // 回転の速度を少し速め
			}
		}
	}
	else {
		// ターゲットが見つからない場合は通常のIdleアニメーション
		m_model.ChangeAnimation(4);
	}
}

//攻撃状態の管理
void Spider::Attack()
{
	if (!mp_player) return; // プレイヤーがいない場合は攻撃しない

	// プレイヤーの位置を取得
	CVector3D player_pos = mp_player->GetPos();

	if (thread == true)
	{
		const int numBullets = 4; // 発射する弾の数
		const float spreadRadius = 6.0f; // プレイヤー周囲の広がり（半径）
		const float speed = 0.20f; // Web弾の速度

		// プレイヤーを中心に弾を収束させる
		for (int i = 0; i < numBullets; ++i) {
			// プレイヤーの周囲にランダムな着弾点を生成
			float angle = DtoR(static_cast<float>(rand() % 360)); // 0～360度のランダムな角度
			float distance = static_cast<float>(rand()) / RAND_MAX * spreadRadius; // 半径spreadRadius内のランダムな距離
			CVector3D offset(sin(angle) * distance, 0.0f, cos(angle) * distance); // 偏差を計算

			// 弾のターゲット地点（プレイヤーの周囲）
			CVector3D targetPos = player_pos + offset;

			// Web弾を生成
			Base::Add(new SpiderBullet(this,m_pos, speed, targetPos));
		}

		// 発射フラグをリセット
		thread = false;
	}

	// 攻撃アニメーションの再生
	m_model.ChangeAnimation(2, false);
	m_attack_no++;

	// プレイヤーとの距離を計算
	CVector3D playerVec = mp_player->GetPos() - m_pos;
	float distanceToPlayer = playerVec.Length();

	

	// アニメーションが終了したら状態を前の状態に戻す
	if (m_model.isAnimationEnd()) {
		m_isAttacking = false;
		m_state = m_previousState;
	}
}

void Spider::StateWall()
{
	const float move_speed = 0.05f;

	// 壁面上を移動
	CVector3D right = (m_rot_matrix * CMatrix::MRotationY(m_rot.y)*CVector3D(0,0,1));
	m_pos += right * move_speed;

	// 壁端を確認
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
	// プレイヤーが視界外に出た場合も解除
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
	
	// 攻撃可能なら攻撃
	if (m_attackCooldown <= 0.0f && IsFoundTarget() && (mp_player->GetPos() - m_pos).Length() < AttackRange) {
		m_previousState = m_state;
		thread = true;
		m_isAttacking = true;
		m_state = eState_Attack00;
		m_attackCooldown = m_attackCooldownMax;
	}

	//敵の方を見る
	if (Base* e = Base::FindObject(ePlayer)) {
		//プレイヤーから敵へのベクトル
		CVector3D v = e->m_pos - m_pos;
		//プレイヤー空間へ変換
		v = m_rot_matrix.GetInverse() * v;
		//回転値を逆算
		m_rot.y = atan2(v.x, v.z);
	}

	m_model.ChangeAnimation(1);
}

//敵の死亡
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

//ダメージを受けた時の処理
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

//ダメージを受けた時の処理
void Spider::Hit()
{

	//攻撃状態もしくは回り込み状態なら、怯まない
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
		// 水平方向の移動 (ジャンプ方向に速度を適用)
		m_vec.x = m_jumpDirection.x * m_jumpSpeedX;
		m_vec.z = m_jumpDirection.z * m_jumpSpeedX;

		// 垂直方向の移動
		m_vec.y = m_jumpSpeedY;
		m_jumpSpeedY -= m_gravity; // 重力を適用

		// 位置を更新
		m_pos += m_vec;

		// 地面に到達したらジャンプ終了
		if (IsTouchingGround())
		{
			m_isJumping = false;   // ジャンプ終了
			m_vec = CVector3D(0, 0, 0);  // 全速度リセット
			m_isCanJump = true;    // ジャンプ可能状態に戻す

		}
	}
}

bool Spider::IsTouchingGround()
{
	if (m_jumpDisableTime > 0.0f)
		return false; // 地面判定を無効化中

	CVector3D start = m_pos + CVector3D(0, -m_rad, 0);
	CVector3D end = m_pos + CVector3D(0, -m_rad - 0.5f, 0);

	CVector3D hitPos, hitNormal;

	if (Base* field = Base::FindObject(eField))
	{
		if (field->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			//Utility::DrawSphere(hitPos, 0.2f, CVector4D(1, 0, 0, 1)); // 衝突点を赤い球で描画
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

// ジャンプ開始時にタイマーを設定
void Spider::MoveToTarget()
{
	if (!mp_player || !m_isCanJump) return;

	// プレイヤー方向のジャンプベクトル計算
	CVector3D targetDir = mp_player->GetPos() - m_pos;
	targetDir.y = 0; // 水平方向のみ考慮
	targetDir.Normalize();

	m_jumpDirection = targetDir;
	m_jumpSpeedX = 0.15f; // 水平方向の速度
	float jumpHeight = 1.0f; // 最大ジャンプ高さ
	m_jumpSpeedY = sqrt(2 * m_gravity * jumpHeight);

	// ジャンプ開始時に地面判定を無効化
	m_jumpDisableTime = 0.2f; // 200ms間地面判定を無効化

	m_isJumping = true;
	m_isCanJump = false;
}

//プレイヤーにお金を与える
void Spider::GiveMoney(int Value)
{
	Player::Instance()->GainMoney(Value);
}

//壁との衝突判定
bool Spider::CheckWallCollision(const CVector3D& dir)
{
	CVector3D newPos = m_pos + dir * moveSpeed;
	CVector3D start = newPos;
	CVector3D end = start + dir * 1.0f;

	start.y = -3.5f;
	end.y = -3.5f;

	//Rayの表示
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


//プレイヤーを見つける
bool Spider::IsFoundTarget()
{
	//回転値から方向ベクトルを計算
	m_dir =m_rot_matrix * (CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

	//壁があるかのチェック
	if (CheckWallCollision(m_dir))
	{
		m_pos.y += moveSpeed;
	}

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

bool Spider::FindSurfaceToAmbush(CVector3D& attachPos, CVector3D& attachNormal)
{
	// プレイヤーに向かう方向に壁を探す
	m_ray_s = m_pos + CVector3D(0, 1.0f, 0); // 蜘蛛の頭付近
	m_ray_e = m_ray_s + m_dir * 0.8f;         // 前方にレイを伸ばす

	CVector3D hitPos, hitNormal;
	if (Field* field = dynamic_cast<Field*>(Base::FindObject(eField))) {
		if (field->GetModel()->CollisionRay(&hitPos, &hitNormal, m_ray_s,m_ray_e)) {
			attachPos = hitPos;         // 壁の接触位置
			attachNormal = hitNormal;   // 壁の法線
			return true;                // 壁を検出した
		}
	}
	return false; // 壁が見つからなかった
}

CMatrix Spider::CalcRotMatrix(const CVector3D& normal) const
{
	CVector3D up = normal; // 壁の法線を「上」とする
	CVector3D forward = m_dir; // 現在の移動方向を前方向とする

	// forwardが法線と平行でないか確認し、調整
	if (fabs(CVector3D::Dot(forward, up)) > 0.99f) {
		forward = CVector3D(0, 0, 1); // ほぼ垂直なら代替の方向を設定
	}

	// 壁の法線に基づいて「右方向」を計算
	CVector3D right = CVector3D::Cross(forward, up).GetNormalize();

	// 正しい「前方向」を再計算
	forward = CVector3D::Cross(up, right).GetNormalize();

	// 回転行列を構築
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
		CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // 敵の目線の高さ
		CVector3D end = player->m_pos + CVector3D(0, 1.0f, 0); // プレイヤーの目線の高さ

		// デバッグ用: レイを可視化
		//Utility::DrawLine(start, end, CVector4D(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色い線で描画

		CVector3D hitPos, hitNormal;
		if (Base* b = Base::FindObject(eField)) {
			if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
				// プレイヤーまでの距離よりも衝突点が手前なら視線が遮られている
				if ((hitPos - start).Length() < (end - start).Length()) {
					//Utility::DrawSphere(hitPos, 0.2f, CVector4D(1.0f, 0.0f, 0.0f, 1.0f)); // 赤い球で衝突点を描画
					return false;
				}
			}
		}

		// 遮蔽物なし
		//Utility::DrawLine(start, end, CVector4D(0.0f, 1.0f, 0.0f, 1.0f)); // 緑の線で描画
		return true;
	}

	return false;
}

void Spider::RenderVision()
{
	const float stepAngle = 5.0f; // 視野を描画する角度の間隔
	const CVector4D visionColor(0, 1, 0, 0.5f); // 緑色
	const CVector4D detectionColor(1, 0, 0, 0.5f); // 赤色

	// 視野範囲を描画
	for (float angle = -m_viewAngle / 2.0f; angle <= m_viewAngle / 2.0f; angle += stepAngle)
	{
		float rad = DtoR(angle) + m_rot.y; // 敵の回転に基づいて角度を調整
		CVector3D dir = m_rot_matrix * CVector3D(sin(rad), 0, cos(rad));
		CVector3D endPos = m_pos + dir * m_viewLength; // 視野範囲の先端
		//Utility::DrawLine(m_pos, endPos, visionColor);
	}

	// プレイヤーが視界内の場合、赤い線で描画
	if (IsFoundPlayer())
	{
		CVector3D playerPos = mp_player->m_pos;
		//Utility::DrawLine(m_pos, playerPos, detectionColor);
	}
}


//AIの更新
void Spider::UpdateAI()
{
	if (!mp_player) return;

	// プレイヤーとの距離を計算
	CVector3D playerVec = mp_player->GetPos() - m_pos;
	float distanceToPlayer = playerVec.Length();

	// 攻撃範囲内なら攻撃
	if (distanceToPlayer < AttackRange && !m_isAttacking && !m_isJumping) {
		m_isAttacking = true;
		thread = true;
		m_state = eState_Attack00;
	}

	// ジャンプ範囲内ならジャンプ
	if (distanceToPlayer < 20.0f && m_isCanJump && m_jumpCooldown <= 0.0f) {
		m_rot_matrix.indentity;
		MoveToTarget(); // ジャンプ動作
		m_jumpCooldown = Utility::Rand(m_jumpCooldownMin, m_jumpCooldownMax); // クールダウン設定
	}
}

void Spider::Update()
{
	m_dir = (m_rot_matrix* CMatrix::MRotationY(m_rot.y)).GetFront();
	//RenderVision();

	// 地面判定無効タイマーの更新
	if (m_jumpDisableTime > 0.0f)
		m_jumpDisableTime -= 0.016f;

	m_mutekijikan -= 0.016f;

	// クールダウンを減少

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
	

	// 押し返し力をリセット
	m_pushBackForce = CVector3D(0, 0, 0);

	// 他の敵との衝突をチェック
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

	// 押し返しの力を最終的に適用
	m_pos += m_pushBackForce;

	// 状態に応じた動作
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

	// ジャンプ処理
	HandleJump();

	// アニメーション更新
	m_model.UpdateAnimation();

	if (m_stateItem == e_Drop&&!m_isWallAttached)
	{
		// 重力落下
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
		//位置調整用
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
	if (m_debugAttachPos != CVector3D(0, 0, 0)) // デバッグ用のターゲットがある場合
	{
		Utility::DrawSphere(m_debugAttachPos, 0.2f, CVector4D(1, 0, 0, 1)); // 衝突点（赤い球）
		Utility::DrawLine(m_debugAttachPos, m_debugAttachPos + m_debugAttachNormal, CVector4D(0, 1, 0, 1)); // 法線（緑の線）
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
			// カプセル同士の衝突判定
			float overlapDistance = 0.0f;
			CVector3D collisionPoint;

			if (CCollision::CollisionCapsule(
				otherSpider->m_lineS, otherSpider->m_lineE, otherSpider->m_rad,
				m_lineS, m_lineE, m_rad, &overlapDistance, &collisionPoint))
			{
				// 重なり解消ベクトルの計算
				CVector3D pushBackDir = m_pos - otherSpider->GetPos();
				pushBackDir.Normalize();

				// 押し返し量を計算
				overlapDistance = (m_rad + otherSpider->m_rad) - (m_pos - otherSpider->GetPos()).Length();

				if (overlapDistance > 0.0f&&m_hp >=1) {
					// 双方向に押し返しを適用
					CVector3D pushAmount = pushBackDir * (overlapDistance * 0.5f);
					m_pos += pushAmount; // 自分を押し返す
					otherSpider->m_pos -= pushAmount; // 相手を押し返す
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