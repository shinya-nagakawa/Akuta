#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"
#include "Camera.h"
#include "Effekseer/EffekseerEffect.h"
#include "NavNode.h"
#include "NavManager.h"
#include "Field.h"

#define MOVE_RANGE_MIN -50.0f
#define MOVE_RANGE_MAX 50.0f

Enemy::Enemy_RoleData EnemyRoleData[Enemy::eRole_Max] = {
	{1,1.0f,3.0f},
	{80,0.5f,8.0f},

};

Enemy::Enemy(const CVector3D& pos,int enemy_Hp,float weight,EnemyRole role ) :Enemy_Base(pos,90.0f,12.0f,0.6f), m_role(role)
{
	m_model = COPY_RESOURCE("Golem", CModelA3M);
	m_pos = pos;
	m_rad = 0.6f;
	m_hp = enemy_Hp;
	m_weight = weight;
	m_damage = 10;
	move_speed = 0.06f;
	AttackRange = 1.8f;
	FlankRange = EnemyRoleData[m_role].searchRangeMultiplier;
	SpinRange = 1.5f;
	m_state = eState_Idle;
	AttackInterval = 60.0f;
	AttackTimer = 30.0f;
	dashCooldownTimer = 0.0f;
	flankTimer = 0.0f;
	m_viewAngle = 90.0f;
	m_viewLength = 12.0f;
	m_searchLength = 500.0f;
	m_isAttacking = false;
	m_attackCount = 0;
	DirectionChange = 60;
	currentFrame = 0;
	m_forceDetectPlayer = false;

	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));

	m_idleProbability = 0.6f; // 60%の確率で待機
	m_stateChangeDuration = 2.0f; // 2秒ごとに状態を変更

	wanderTime = 0.0f;
	wanderInterval = 5.0f;  // 5秒ごとに方向転換
	wanderSpeed = 0.035f;   // ランダム移動時の速度

	// 敵の位置に経路探索用のノードを作成
	m_navNode = new NavNode(m_pos, NavNode::NodeType::Destination);
	m_navNode->SetNodeColor(CVector3D(0.125f, 0.25f, 1.0f));

	m_item_id = 1;
}

Enemy::~Enemy()
{
	// SellCount をカウントアップ
	Field* field = dynamic_cast<Field*>(Base::FindObject(eField));
	if (field) {
		field->IncrementSellCount();
	}
}



void Enemy::SetRole(EnemyRole role)
{
	m_role = role;
}

//移動等
void Enemy::Idle()
{
	m_stateChangeTimer += 0.016f;

	if (IsFoundPlayer())
	{
		Min_speed = 0.04f;
		Max_speed = 0.06f;
		if (Min_speed <= Max_speed)
			Min_speed += 0.01f;

		//リストからプレイヤーを探索
		if (Base* p = Base::FindObject(ePlayer))
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
			if (vec.Length() > 1.5f)
			{
				//前進
				m_pos += dir * Min_speed;
				//前進アニメーション
				m_model.ChangeAnimation(1);
			}
		
			float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
			//割合による補間
			m_rot.y += a * 0.1f;
		}

	}
	else
	{
		//Move()
		Wander();
	}

	//経路探索が完成するまでコメント化
	/*
	else if (m_stateChangeTimer >= m_stateChangeDuration)
		
		{

			//タイマーが経過したら再抽選
			if (static_cast<float>(rand()) / RAND_MAX < m_idleProbability) 
			{
				m_isWanderOn = false;
				m_model.ChangeAnimation(0);
			}
			else
			{
				m_isWanderOn = true;
			}
			m_stateChangeTimer = 0;
		}

	if (m_isWanderOn)
	{
		Wander();
	}*/
}

void Enemy::Move()
{
	//歩きアニメーションに変更

	//各ノードの座標を取得
	NavNode* enemyNode = m_navNode;
	CVector3D moveNodePos = m_moveNode->GetPos();
	CVector3D enemyNodePos = enemyNode->GetPos();
	//目的地までのベクトルを求め、距離を取得
	CVector3D vec = moveNodePos - enemyNodePos;
	float dist = vec.Length();

	NavManager* navMgr = NavManager::Instance();

	//現在位置から目的地のノードまでの経路探索を行う
	m_nextNode = navMgr->Navigate(enemyNode, m_moveNode);
	//次に移動すべきノードが存在すれば、
	if (m_nextNode != nullptr)
	{
		//次に移動すべきノードへ向けて移動
		if (m_nextNode != nullptr)
		{
			//次に移動すべきノードへ向けて移動
			if (MoveTo(m_nextNode->GetPos(), move_speed))
			{
				//移動が終われば、待機状態へ戻る
				m_moveNode = nullptr;
				m_nextNode = nullptr;
				m_state = eState_Wander;
			}
		}
	}
}

//プレイヤーへダッシュ
void Enemy::Dash()
{
	static const float dashDuration = 0.5f;  // 突進時間（秒）
	static const float maxDashSpeed = 0.18f; // 最大突進速度
	static const float dashAccel = 0.05f;    // 突進の加速度
	static const float dashCooldown = 0.5f; // 突進後のクールダウン時間（秒）

	// ターゲットが見つからない場合、待機状態に戻る
	if (!IsFoundPlayer()) {
		m_state = eState_Idle;
		return;
	}

	// 突進中の処理
	if (dashTimer > 0.0f) {
		dashTimer -= CFPS::GetDeltaTime(); // 1フレーム分の時間を減算

		// 加速度を考慮して現在の速度を計算
		m_currentDashSpeed += dashAccel; // 加速度で速度を増加
		if (m_currentDashSpeed > maxDashSpeed) {
			m_currentDashSpeed = maxDashSpeed; // 最大速度を超えないようにする
		}

		// 突進方向に移動
		m_pos += m_dashDir * m_currentDashSpeed;

		// 突進が終わる条件
		if (dashTimer <= 0.0f) {
			dashCooldownTimer = dashCooldown; // クールダウンタイマーをセット
			m_state = eState_Idle; // 突進後は移動状態に戻る
		}
		return; // 突進中はここで終了
	}

	// クールダウン中の処理
	if (dashCooldownTimer > 0.0f) {
		dashCooldownTimer -= 0.016f;

		// プレイヤーとの距離を計算して移動処理を追加
		if (Player* player = Player::Instance()) {
			CVector3D vecToPlayer = player->GetPos() - m_pos;
			float distance = vecToPlayer.Length();

			// プレイヤーが遠い場合は待機状態に戻る
			if (distance > 1.5f) {
				m_state = eState_Idle;
			}
		}
		return; // クールダウン中は次フレームへ
	}

	// 突進開始処理
	if (Player* player = Player::Instance()) {
		CVector3D direction = player->GetPos() - m_pos;
		direction.y = 0.0f; // 高さ方向を無視
		m_dashDir = direction.GetNormalize();

		// 敵の回転をプレイヤー方向に合わせる
		m_rot.y = atan2(m_dashDir.x, m_dashDir.z);

		// 突進モーションを再生
		m_model.ChangeAnimation(12); // 突進用アニメーション番号

		dashTimer = dashDuration; // タイマーを初期化
		m_currentDashSpeed = 0.0f; // 突進速度を初期化
	}
}

//プレイヤーからの攻撃を避ける
void Enemy::Evade() {

	// ターゲットが見つからない場合、待機状態に戻る
	if (!IsFoundPlayer()) {
		m_state = eState_Idle;
		return;
	}

	static const float evadeDuration = 0.3f; // 回避時間（秒）
	static const float evadeSpeed = 0.10f;   // 回避速度

	// 回避が初めて実行される場合
	if (evadeTimer <= 0.0f) {
		// タイマーリセット
		evadeTimer = evadeDuration;

		// ランダムで左右の方向を決める
		bool goLeft = (rand() % 2 == 0);
		m_evadeDir = goLeft ? CVector3D(-1.0f, 0, 0) : CVector3D(1.0f, 0, 0);

		// 敵の向きに合わせた回避方向を計算
		m_evadeDir = CVector3D(
			m_evadeDir.x * cos(m_rot.y) - m_evadeDir.z * sin(m_rot.y),
			0,
			m_evadeDir.x * sin(m_rot.y) + m_evadeDir.z * cos(m_rot.y)
		);

		// 回避モーションの再生（任意）
		m_model.ChangeAnimation(11);
	}

	// タイマーを進める
	evadeTimer -= 0.016f; // 1フレーム分の時間を減算

	// 回避動作を続ける
	if (evadeTimer > 0.0f) {
		m_pos += m_evadeDir * evadeSpeed;
	}
	else {
		// 回避終了
		evadeTimer = 0.0f;
		m_state = eState_Idle; // 待機状態に戻る
	}
}

bool Enemy::MoveTo(const CVector3D& target, float moveSpeed)
{
	CVector3D vec = target - m_pos;
	//ベクトルから回転値を逆算
	m_rot.y = atan2(vec.x, vec.z);
	//回転値から方向ベクトルを計算
	CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

	
	vec.y = 0.0f;
	dir = vec.GetNormalize();

	//目的地までの距離が移動速度より大きいならば
	float dist = vec.Length();
	float speed = moveSpeed ;
	if (dist > speed)
	{
		CVector3D moveVec = dir * moveSpeed;
		m_model.ChangeAnimation(1); //前進
		m_pos += CVector3D(moveVec.x, m_vec.y, moveVec.z);
	}
	//距離が移動速度より短いのであれば、
	else
	{
		CVector3D moveVec = dir * dist;
		m_model.ChangeAnimation(1); //前進
		m_pos += CVector3D(moveVec.x, m_vec.y, moveVec.z) ;
		//移動終了
		return true;
	}
	//移動継続
	return false;
}

//攻撃状態の管理
void Enemy::Attack()
{

	// ターゲットが見つからない場合、待機状態に戻る
	if (!IsFoundPlayer()) {
		m_state = eState_Idle;
		return;
	}
	
		if (m_isAttacking) {
			// 現在の敵の向き（回転値）に基づいて正面方向を計算
			CVector3D forwardDir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));

			// モデルのボーン位置を取得
			m_AttackS = m_model.GetFrameMatrix(29).GetPosition();
			m_AttackE = m_model.GetFrameMatrix(30).GetPosition();

			m_model.ChangeAnimation(22, false);

			if (m_model.isAnimationEnd())
			{
				m_state = eState_Idle;

				m_isAttacking = false;
			}

			// `m_AttackS` を少し前方にオフセット
			float offsetDistance = 0.5f; // 前方に伸ばす距離
			m_AttackE += forwardDir * offsetDistance;

			// デバッグ用：攻撃範囲をカプセルで可視化
			//Utility::DrawCapsule(m_AttackS, m_AttackE, 0.5f, CVector4D(1, 0, 0, 1)); // 赤色
		}
}

//敵の死亡
void Enemy::Down()
{
		//m_isDead = true;

		if (m_stateItem != e_Equip) {
			m_model.ChangeAnimation(26, false);
		}
		else {
			m_model.ChangeAnimation(0, false);
		}
	
}

//ダメージを受けた時の処理
void Enemy::Damage()
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

//ダメージを受けた時の処理
void Enemy::Hit()
{

//攻撃状態もしくは回り込み状態なら、怯まない
	if (m_state == eState_Attack00||m_state == eState_Flank)return;

		m_isAttacking = false;
		m_state = eState_Hit;
		m_model.ChangeAnimation(15, false);
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

//背後に周りこむ動き
void Enemy::Flank()
{
	if (!Dashing)
	{
		static const float flankDuration = 2.0f; // 回り込みの最大時間
		static const float flankOffset = 5.0f;   // プレイヤーの横方向に膨らむ量
		static const float flankDistance = 3.0f; // プレイヤーの後方までの距離

		static float directionChangeTimer = 0.0f; // 方向変更用タイマー

		Player* player = Player::Instance();
		if (!player) return;

		// 回り込みの経過時間を更新
		flankTimer += CFPS::GetDeltaTime(); // 1フレームの時間（60FPSの場合）
		directionChangeTimer += CFPS::GetDeltaTime(); // 方向変更タイマーを進める

		// 時間が経過したら回り込み終了
		if (flankTimer >= flankDuration) {
			m_state = eState_Idle; // 回り込み終了後に待機
			flankTimer = 0.0f;     // タイマーをリセット
			directionChangeTimer = 0.0f; // 方向変更タイマーもリセット
			return;
		}

		// プレイヤーの正面方向ベクトルを計算
		CVector3D playerForward(sin(player->m_rot.y), 0, cos(player->m_rot.y));

		// プレイヤーの右方向ベクトルを計算
		CVector3D playerRight(-playerForward.z, 0, playerForward.x);

		// 定期的に方向を変更

		if (rand() % 100 < 30) { // 30%の確率で方向を変更
			goLeft = !goLeft; // 現在の方向を反転
		}
		directionChangeTimer = 0.0f; // タイマーをリセット

	// ターゲット位置を計算
		float biasOffset = (goLeft ? -1.0f : 1.0f) * (rand() % 100 / 100.0f); // ランダムに左右寄りを調整
		CVector3D offset = (goLeft ? -playerRight : playerRight) * (flankOffset + biasOffset);
		CVector3D targetPos = player->m_pos - playerForward * flankDistance + offset;

		// **VecToTargetの計算**
		CVector3D vecToTarget = targetPos - m_pos; // 目標地点へのベクトル
		float distance = vecToTarget.Length();    // 距離を計算

		// ターゲットの方向を計算して回転
		m_rot_target.y = atan2(vecToTarget.x, vecToTarget.z);
		float angleDiff = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
		m_rot.y += angleDiff * 0.1f;

		// 距離が十分でない場合は移動
		if (distance > 0.5f) {
			CVector3D dir(sin(m_rot.y), 0, cos(m_rot.y));

			// 衝突判定
			if (CheckWallCollision(dir)) {
				m_state = eState_Idle; // 衝突時は停止
				return;
			}

			// 移動処理
			m_pos += dir * move_speed * 1.3f;
			m_model.ChangeAnimation(11); // 移動アニメーション
		}
		else {
			// 攻撃状態へ移行
			m_state = eState_Attack00;
		}

		// デバッグ用：目標位置の可視化
		Utility::DrawSphere(targetPos, 0.5f, CVector4D(1, 0, 0, 1)); // 目標位置を表示
	}
}

//プレイヤーにお金を与える
void Enemy::GiveMoney(int Value)
{
	Player::Instance()->GainMoney(Value);
}

//プレイヤーを見つける
bool Enemy::IsFoundPlayer() const
{

	/*
	//強制発見モードがオンなら無条件でプレイヤーを発見
	if (m_forceDetectPlayer)
	{
		return true;
	}*/
	
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



//初期化
void Enemy::Initialize()
{
	//初期化時にランダムな方向転換を設定
	wanderInterval = static_cast<float>(rand()) / RAND_MAX * 5.0f + 1.0f;
	wanderTime = wanderInterval;
	m_rot_target.y = DtoR(static_cast<float>(rand()) / RAND_MAX * 360.0f);
}

//ランダムに動く
void Enemy::Wander()
{
	wanderTime -= 0.016f;
	if (wanderTime <= 0) {
		bool directionFound = false;
		int maxAttempts = 10;
		int attempts = 0;

		while (!directionFound && attempts < maxAttempts) {
			float angle = static_cast<float>(rand()) / RAND_MAX * 360.0f;
			m_rot_target.y = DtoR(angle);
			CVector3D dir(CVector3D(sin(m_rot_target.y), 0, cos(m_rot_target.y)));

			if (!CheckWallCollision(dir)) {
				directionFound = true;
			}
			attempts++;
		}

		if (!directionFound) {
			m_rot_target.y = DtoR(0); // デフォルト方向
		}

		wanderInterval = static_cast<float>(rand()) / RAND_MAX * 5.0f + 1.0f;
		wanderTime = wanderInterval;
	}

	CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
	if (!CheckWallCollision(dir)) {
		m_pos += dir * wanderSpeed;
		m_model.ChangeAnimation(1);
	}
	else {
		m_model.ChangeAnimation(0);
	}

	float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
	m_rot.y += a * 0.1f;
}







//壁を避けてほしい
void Enemy::AvoidWalls()
{
	float CheckDistance = 2.0f;
	CVector3D avoidDir = CalculateAvoidDirection(CheckDistance);

	//壁が近い場合は回避動作を行う
	if (avoidDir.Length() > 0.0f)
	{
		CVector3D dir = avoidDir;

		//衝突しないか再チェック
		if (!CheckWallCollision(dir))
		{
			m_pos += dir * move_speed; // 回避方向に移動
			m_model.ChangeAnimation(1); // 歩きアニメーション
		}
		else {
			m_model.ChangeAnimation(0); // 待機アニメーション（回避できない場合）
		}
	}
}

//回避方向のデバッグ用
void Enemy::DebugAvoidance()
{
	float checkDistance = 2.0f;
	CVector3D avoidDir = CalculateAvoidDirection(checkDistance);

	if (avoidDir.Length() > 0.0f) {
		// 回避方向をデバッグラインで表示
		Utility::DrawLine(m_pos + CVector3D(0, 1, 0), m_pos + avoidDir * checkDistance, CVector4D(0, 1, 0, 1));
	}
}

//プレイヤーの攻撃回数カウンター
void Enemy::IncreaseAttackCount()
{
	m_attackCount++;
}

//AIの更新
void Enemy::UpdateAI()
{
	{
		Base* playerBase = Base::FindObject(ePlayer);
		if (!playerBase) {
			m_state = eState_Idle; // プレイヤーがいないなら待機
			return;
		}

		// プレイヤーとの距離計算
		CVector3D Player_vec = playerBase->m_pos - m_pos;
		float distance_Player = Player_vec.Length();

		// **優先順位1: プレイヤーが攻撃モーションを解除した瞬間に回避**
		if (Player::Instance()->GetAttackRelease() && distance_Player < AttackRange * 2.0f) {
			if (rand() % 100 < 25) { // 25%の確率で回避
				m_state = eState_Evade;
				return;
			}
		}

		// **優先順位2: 突進**
		if (distance_Player > 4.0f && distance_Player < 8.0f) { // 突進に適した距離
			if (rand() % 100 < 15) { // 15%の確率で突進
				m_state = eState_Dash; // 突進状態へ
				return;
			}
		}

		// **優先順位3: 攻撃**
		if (distance_Player < AttackRange && !m_isAttacking) {
			m_isAttacking = true;
			m_state = eState_Attack00; // 攻撃モード
			return;
		}

		
		// **優先順位4: 背後回り込み**
		if (distance_Player < 5.0&& distance_Player <FlankRange &&!m_isAttacking) {
			if (rand() % 100 < EnemyRoleData[role].flankPersent) { 
				m_state = eState_Flank;
				return;
			}
		}

		// **優先順位5: 移動して近づく**
		if (distance_Player > 1.5f && !m_isAttacking) {
			m_state = eState_Move; // 追いかける
			return;
		}

		if (IsWallNearby(3.0f))
		{
			//壁を検知した場合、壁沿いに移動する方向を調整
			m_rot.y += DtoR(90);
		}

		// **優先順位6: 待機**
		m_state = eState_Idle; // 近すぎる、もしくは何もしないとき
	}
}

//更新処理
void Enemy::Update()
{

	// Rキーでプレイヤー強制発見モードを切り替え
	if (PUSH(CInput::eButton6)) { // 例えば R キー
		m_forceDetectPlayer = !m_forceDetectPlayer; // モードをトグル（オン/オフ切り替え）
		m_model.ChangeAnimation(17,true);
		printf("プレイヤー強制発見モード: %s\n", m_forceDetectPlayer ? "オン" : "オフ");
	}

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

	//AttackInterval -= 0.016f;
	m_evadeCooldown -= 0.016f;
	m_mutekijikan -= 0.016f;

	// 壁が近い場合、回避優先
	if (IsWallNearby(1.0f)) {
		AvoidWalls();
		return; // 壁回避時は他の処理をスキップ
	}

	//DebugAvoidance();

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
	case eState_Hit:
		Hit();
		break;
	case eState_Down:
		Down();
		break;
	case eState_Flank:
		Flank();
		break;
	case eState_Wander:
		//Wander();
		break;
	case eState_Dash:
		Dash();
		break;
	case eState_Evade:
		Evade();
		break;
	}
	
	
	//アニメーション更新
	m_model.UpdateAnimation();

	if (m_stateItem ==e_Drop)
	{
		//重力落下
		m_pos.y += m_vec.y;
		m_vec.y -= GRAVITY;
	}
}

//描画処理
void Enemy::Render()
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
		m_pos = player_bone.GetPosition()+offset;
		m_rot = player->m_rot;
		

		m_model.SetScale(0.005f, 0.005f, 0.005f);

		enemy_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);
		
		

		if (m_hp <= 0)
		{
			m_model.ChangeAnimation(0);
		}

	}
	
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	if(m_stateItem ==e_Drop)
	m_model.SetScale(0.01f, 0.01f, 0.01f);
	if(m_stateItem !=e_PickUp)
	m_model.Render();

	m_lineS = m_model.GetFrameMatrix(4).GetPosition();
	m_lineE = m_model.GetFrameMatrix(6).GetPosition();

	// 視野のデバッグ表示
	//RenderVision();

	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));
	//if(m_isAttacking&&m_hp > 0)
	//Utility::DrawCapsule(m_AttackS, m_AttackE, m_rad, CVector4D(1, 0, 0, 0.5));
}

void Enemy::RenderVision()
{
	const float stepAngle = 5.0f; // 視野を描画する角度の間隔
	const CVector4D visionColor(0, 1, 0, 0.5f); // 緑色
	const CVector4D detectionColor(1, 0, 0, 0.5f); // 赤色

	// 視野範囲を描画
	for (float angle = -m_viewAngle / 2.0f; angle <= m_viewAngle / 2.0f; angle += stepAngle)
	{
		float rad = DtoR(angle) + m_rot.y; // 敵の回転に基づいて角度を調整
		CVector3D dir(sin(rad), 0, cos(rad));
		CVector3D endPos = m_pos + dir * m_viewLength; // 視野範囲の先端
		Utility::DrawLine(m_pos, endPos, visionColor);
	}

	// プレイヤーが視界内の場合、赤い線で描画
	if (IsFoundPlayer())
	{
		CVector3D playerPos = mp_player->m_pos;
		Utility::DrawLine(m_pos, playerPos, detectionColor);
	}
}

//オブジェクトとの衝突判定
void Enemy::Collision(Base* b)
{		
		
	if (m_stateItem != e_Drop)return;

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
						e->m_pushBackForce -= pushBackDir * 0.05; // 反対方向

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
					m_lineS, m_lineE, m_rad,&dist,&c1))
				{

					if (m_mutekijikan <= 0&&m_hp >0)
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


//装備状態にする処理
void Enemy::Equip()
{
	m_stateItem = e_Equip;

	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() + m_weight);
	}
}

//非装備状態にする処理
void Enemy::Unequip()
{
	m_stateItem = e_Drop;

	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() - m_weight);
	}
}

int Enemy::GetHP()
{
	return m_hp;
}


bool Enemy::GetAttacking()
{
	return m_isAttacking;
}

int Enemy::GetAttackno()
{
	return m_attack_no;
}

int Enemy::GetDamage()
{
	return m_damage;
}


CVector3D Enemy::GetPos()
{
	return m_pos;
}
