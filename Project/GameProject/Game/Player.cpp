#include "Player.h"
#include "Camera.h"
#include "Weapon.h"
#include "Item.h"
#include "Enemy.h"
#include "Spider.h"
#include "GreenDragon.h"
#include "SpiderBullet.h"
#include "Gauge.h"
#include "RedFilter.h"
#include "BlackFilter.h"
#include "SceneManager.h"
#include "Title.h"
#include "Game.h"
#include "UI.h"


Player* Player::ms_instance = nullptr;
// 初期位置を保存するための変数
CVector3D initialPosition;

Player::Player(const CVector3D& pos) :Base(ePlayer), m_carry(nullptr), m_gauge(nullptr), m_maxItems(4)
{
	ms_instance = this;

	// モデルと初期設定
	m_model = GET_RESOURCE("vanguard", CModelA3M);
	m_model->ChangeAnimation(0);
	m_pos = pos;
	initialPosition = pos;//初期位置を記録

	// 初期ステータス
	m_hp = m_hp_max = 100;
	m_stamina = m_stamina_max = 100;
	m_weight = 0.0f;
	m_pushBackStrength = 0.25f;
	move_speed = 1.0f;
	BaseSpead = 0.010f;
	m_rad = 0.4f;

	m_jetpackFuel = 1000.0f;
	m_jetpackForce = 0.005f;

	// 状態フラグ
	m_isGround = true;
	m_isJump = true;
	m_isWalking = false;
	m_isAttacking = false;
	m_isChargeing = false;
	m_isKnockback = false;

	m_footstepTimer = 0.0f;
	m_footstepInterval = 0.5f;

	// その他初期化
	m_pushBackForce = CVector3D(0, 0, 0);
	m_velocity = CVector3D(0, 0, 0);
	m_friction = 0.85f;

	// アニメーションのレイヤー設定
	for (int i = 5; i <= 58; i++) {
		m_model->GetNode(i)->SetAnimationLayer(1);
	}
	m_inventory.resize(m_maxItems);
	m_inventory_image = COPY_RESOURCE("Inventory", CImage);

	// ゲージ生成
	Base::Add(m_gauge = new Gauge(Gauge::GaugeType::ePlayerGauge, 0.28f));
	Base::Add(m_gauge1 = new Gauge(Gauge::GaugeType::ePlayerStamina, 0.3f));

}

// 地面判定を更新
void Player::UpdateGroundStatus()
{
	m_isGround = false;

	auto collisions = Base::FindObject(eField)->GetModel()->CollisionCupsel(
		m_pos + CVector3D(0, 2.0 - m_rad, 0),  // 始点（頭）
		m_pos + CVector3D(0, m_rad, 0),        // 終点（足元）
		m_rad
	);

	for (auto& tri : collisions) {
		// 地面と接触したと判定する条件
		if (tri.m_normal.y > 0.5f) {
			m_isGround = true;
			if (m_vec.y < 0) {
				m_vec.y = 0; // 落下をリセット
			}
		}
	}
}

// 吹き飛び処理
void Player::ApplyKnockback(const CVector3D& knockbackForce)
{
	// 吹き飛ばしの力を加算
	m_vec += knockbackForce;

	// 吹き飛び時でも地面接触を確認
	UpdateGroundStatus();

	if (m_isGround) {
		// 地面に接している場合でも吹き飛びを続行
		m_isKnockback = true;
		m_knockbackTimer = 0.5f;
	}
	else {
		// 空中での吹き飛びも処理
		m_isKnockback = true;
		m_knockbackTimer = 0.5f;
	}
}

// ダメージを受けた際の赤フィルター表示
void Player::RedFilterDisplay()
{
	RedFilter* r = dynamic_cast<RedFilter*>(Base::FindObject(eRedFilter));
	if (!r) {
		Base::Add(r = new RedFilter());
	}
	r->Show(30.0f);
}

// 状態別処理
void Player::UpdateState()
{
	switch (m_state) {
	case eState_Idle:
	case eState_Move:
		Idle();
		break;
	case eState_Attack00:
		Attack();
		break;
	case eState_Attack01:
		tameAttack();
		break;
	case eState_Down:
		Down();
		break;
	}
}

void Player::Idle()
{
	CVector3D key_dir(0, 0, 0);
	int animation = 0;
	SelectItem();
	HandleMovement(key_dir, animation);

	//ジェットパック
	if (HOLD(CInput::eMouseL) && m_isJetpackActive && m_jetpackFuel > 0.0f)
	{
		m_isGround = false;
		m_isFalling = false;

		//浮上する
		m_vec += (CMatrix::MRotation(Camera::Instance()->m_rot).GetUp() * m_jetpackForce * 1.3f);

		if (m_Force <= 0.015f)
		{
			m_Force += 0.0003f;
		}

		m_vec += CVector3D(0, m_Force, 0);

		//燃料消費
		JetPackFuel(0);
	}
	else
	{
		JetPackOff();
	}


	// キー入力があれば
	if (key_dir.LengthSq() > 0) {
		// ■移動処理 回転行列×キー方向
		CVector3D dir = CMatrix::MRotationY(m_rot.y) * key_dir;
		m_velocity += dir * BaseSpead;
		if (m_isGround)
		{
			m_model->ChangeAnimation(0, animation);
			static int idx = 0;
			static int anim_old = 0;
			if (animation != anim_old) {
				anim_old = animation;
				idx = 0;
			}
			static const int se_frame[11][2] = {
				{4,15 },	//0
				{13,32 },	// 左移動アニメーション
				{4,15 },	//2
				{4,15 },	//3
				{4,15 },	//4
				{4,15 },	//5
				{13,32 },	// 右移動アニメーション
				{4,15 },	// 前進アニメーション
				{13,32 },	// 後退アニメーション
				{4,15 },	//9
				{4,15 },	//10
			};

			if (m_model->GetAnimationFrame(0) == se_frame[animation][idx]) {
				idx = (idx + 1) % 2;
				SOUND("足音")->Play3D(m_pos, CVector3D::zero, false, true, EFX_REVERB_PRESET_CAVE);
			}
		}
		m_state = eState_Move;
		m_isMove = true;

	}

	else
	{
		m_isMove = false;

		//キー入力がなければ
		if (!m_carry)
		{
			m_model->ChangeAnimation(1, 10, 0.2f);
			m_model->ChangeAnimation(0, animation, 0.2f);
		}
		if (m_carry && m_carry->m_type == eEnemy)
		{
			m_model->ChangeAnimation(1, 11, 0.2f);
			m_model->ChangeAnimation(0, 9, 0.2f);
		}
		if (m_carry && m_carry->m_type == eWeapon)
		{
			m_model->ChangeAnimation(1, 9, 0.2f);
			m_model->ChangeAnimation(0, 9, 0.2f);
		}
		if (m_carry && m_carry->m_type == eItem)
		{
			m_model->ChangeAnimation(0, 9, 0.2f);
		}


	}
	m_state = eState_Idle;

	if (!m_isAttacking && PUSH(CInput::eMouseL) && m_carry && m_carry->m_type == eWeapon) {
		m_isChargeing = true;   // チャージ開始
		m_ChargeTime = 0.0f;
		m_state = eState_Attack00;

	}
}

void Player::Attack()
{

	CVector3D key_dir(0, 0, 0);
	int animation = 0;

	HandleMovement(key_dir, animation);


	//キャラクターの回転値をカメラの回転値に合わせる
	m_rot.y = Camera::Instance()->m_rot.y;

	// キー入力があれば
	if (key_dir.LengthSq() > 0) {
		// ■移動処理 回転行列×キー方向
		CVector3D dir = CMatrix::MRotationY(m_rot.y) * key_dir;
		m_model->ChangeAnimation(0, animation);
		m_velocity += dir * BaseSpead * move_speed;
	}
	else
	{
		//キー入力がなければ
		if (!m_carry)
		{
			m_model->ChangeAnimation(1, 10, 0.2f);
		}
		if (m_carry)
		{

			m_model->ChangeAnimation(0, 9, 0.2f);
		}


	}

	//ため攻撃を作りたい
	if (!m_isAttacking && HOLD(CInput::eMouseL) && m_carry)
	{
		m_isAttacking = false;
		m_model->ChangeAnimation(1, 3, false);
		m_ChargeTime++;
		if (!tamekougeki)
			SOUND("ため攻撃")->Play(false);

		tamekougeki = true;
	}

	if (!m_isAttacking && PULL(CInput::eMouseL) && m_carry && m_stamina > 20)
	{
		SOUND("ため攻撃")->Stop();
		m_isAttackReleased = true;//クリックを離した瞬間
		if (m_isChargeing)
		{
			m_isAttacking = true;
			if (m_ChargeTime >= 35.0f) {
				m_stamina -= 20;
				tamekougeki = false;
				m_state = eState_Attack01; // チャージ攻撃
			}
			else {
				m_stamina -= 20;
				tamekougeki = false;
				m_model->ChangeAnimation(1, 5, false);
				m_model->SetAnimationSpeed(1, move_speed);
				m_model->ChangeAnimation(0, 9, 0.2f);
			}
			if (!tamekougeki)
				SOUND("ため攻撃")->Stop();
		}
	}
	else
	{
		m_isAttackReleased = false; //クリックを話してない時はリセット
	}

	//---------------------------------------

	// 攻撃中かどうかのチェック

	if (m_isAttacking && m_model->isAnimationEnd(1))
	{
		m_model->ChangeAnimation(1, 9, 0.2f);
		m_model->ChangeAnimation(0, 9, 0.2f);
		m_isAttacking = false;
		m_isChargeing = false;
		m_state = eState_Idle;

	}

	float speadscale = 1.0f;
	if (HOLD(CInput::eButton4) && m_stamina > 0)
	{
		m_stamina -= 0.2;
		speadscale = 1.5f;

	}

	//チャージ中のスタミナ回復でバグを避ける
	if (m_stamina < m_stamina_max && tamekougeki)
		m_stamina += 0.5;
}


void Player::tameAttack()
{
	int animation = 0;
	CVector3D key_dir(0, 0, 0);

	HandleMovement(key_dir, animation);

	//キャラクターの回転値をカメラの回転値に合わせる
	m_rot.y = Camera::Instance()->m_rot.y;

	// キー入力があれば
	if (key_dir.LengthSq() > 0) {
		// ■移動処理 回転行列×キー方向
		CVector3D dir = CMatrix::MRotationY(m_rot.y) * key_dir;
		m_model->ChangeAnimation(0, animation);
		m_velocity += dir * BaseSpead * move_speed;
	}
	else
	{
		//キー入力がなければ
		m_model->ChangeAnimation(0, 9, 0.2f);
	}

	m_model->ChangeAnimation(1, 4, false);



	if (m_isAttacking && m_model->isAnimationEnd(1))
	{
		m_model->ChangeAnimation(1, 9, 0.2f);
		m_model->ChangeAnimation(0, 9, 0.2f);
		m_isAttacking = false;
		m_isChargeing = false;
		m_ChargeTime = 0.0f;
		m_state = eState_Idle;
	}
}

void Player::Down()
{
	//SetKill();
	m_model->ChangeAnimation(12, false);
}

void Player::HandleMovement(CVector3D& key_dir, int& animation)
{
	// 重さによる移動速度の調整
	float weightFactor = m_weight / 150.0f;
	float adjustedSpeed = move_speed * (1.0f - weightFactor); // 重さに応じて移動速度を調整

	// キー入力処理
	if (HOLD(CInput::eUp)) {
		key_dir.z = 0.9 * adjustedSpeed;
		animation = 7; // 前進アニメーション
	}
	if (HOLD(CInput::eDown)) {
		key_dir.z = -0.8 * adjustedSpeed;
		animation = 8; // 後退アニメーション
	}
	if (HOLD(CInput::eLeft)) {
		key_dir.x = 0.8 * adjustedSpeed;
		animation = 1; // 左移動アニメーション
		m_footstepInterval = 0.6f; // 足音の間隔を設定

	}
	if (HOLD(CInput::eRight)) {
		key_dir.x = -0.8 * adjustedSpeed;
		animation = 6; // 右移動アニメーション
		m_footstepInterval = 0.6f;
	}

	// ダッシュ処理
	if (HOLD(CInput::eButton4) && m_stamina > 0) {
		if (m_isMove) m_stamina -= 0.2;
		key_dir *= 1.5f; // ダッシュ速度
		m_isDashing = true;
	}
	else {
		m_isDashing = false;
	}

	//ジャンプ
	if (HOLD(CInput::eButton5) && m_isJump && m_isGround && m_stamina > 0) {
		m_isJump = false; // ジャンプ中フラグをセット
		m_isGround = false; // 地面判定をリセット
		m_vec.y = 0.2f; // ジャンプ力を設定
		m_stamina -= 15; // スタミナ消費
	}

	if (m_isAction)
	{
		if (m_JumpDelay > 0.5)
		{
			m_vec.y = 0.20;
			m_stamina -= 15;
			m_isAction = false;
		}
	}
	else
	{
		m_JumpDelay = 0;
	}


	if (m_isDashing) {
		m_footstepInterval = 0.3f; // ダッシュ時は間隔を短く
	}
	else {
		m_footstepInterval = 0.4f; // 通常の歩行
	}

	// 足音タイマーを更新
	if (m_footstepTimer > 0.0f) {
		m_footstepTimer -= CFPS::GetDeltaTime();
	}
}



Player::~Player()
{
	//ゲージオブジェクト破棄
	if (m_gauge) m_gauge->SetKill();

}

Player* Player::Instance()
{

	return ms_instance;
}

//アイテム等との衝突判定
bool Player::CheckItemCollision(const CVector3D& dir)
{

	CVector3D newPos = m_pos + dir * move_speed;
	CVector3D start = m_pos + CVector3D(0, 1, 0);
	CVector3D end = start + dir * 1.2f;

	//かぷせるの表示
	//Utility::DrawCapsule(start, end, m_rad, CVector4D(0, 1, 0, 0.5));

	CVector3D hitPos;
	CVector3D hitNormal;

	std::vector<Base*> list = FindObjects(eWeapon);
	std::vector<Base*> list1 = FindObjects(eEnemy);
	std::vector<Base*> list2 = FindObjects(eItem);

	//リストの結合
	list.insert(list.end(), list1.begin(), list1.end());
	list.insert(list.end(), list2.begin(), list2.end());

	for (Base* b : list)
	{
		if (Enemy* enemy = dynamic_cast<Enemy*>(b))
			if (enemy->GetHP() > 0)continue;

		if (Spider* spider = dynamic_cast<Spider*>(b))
			if (spider->GetHP() > 0)continue;

		if (GreenDragon* greendragon = dynamic_cast<GreenDragon*>(b))
			if (greendragon->GetHP() > 0)continue;

		if (Carry* carry = dynamic_cast<Carry*>(b))
		{
			if (carry->m_stateItem != Carry::e_Drop)continue;


			//カプセル同士の衝突
			if (CCollision::CollisionCapsule(b->m_lineS, b->m_lineE, b->m_rad,
				start, end, m_rad))
			{

				if (m_isEkeyPressd)
				{

					SetCarry(carry);
					m_isEkeyPressd = false;
					return true;
				}

				FONT_T()->Draw(800, 680, 1.0f, 1.0f, 1.0f, "Pickup Ekey");
				//printf("Item");
				//m_state = eState_Idle;
				return true;

			}
		}
	}


	return false;
}

void Player::Update()
{

	// === 奈落チェック処理 ===
	const float FALL_THRESHOLD = -20.0f; // 奈落とみなす高さ
	if (m_pos.y <= FALL_THRESHOLD)
	{
		// 初期位置に戻す
		m_pos = initialPosition;
		m_velocity = CVector3D(0.0f, 0.0f, 0.0f); // 落下速度をリセット
		printf("奈落から復帰しました\n"); // デバッグ用ログ
	}

	//回転値から方向ベクトルを計算
	//CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
	CheckItemCollision(CMatrix::MRotation(Camera::Instance()->m_rot).GetFront());

	CVector3D flashlightPos = m_pos + m_dir * 1.0f; // 1.0fは前方オフセット距離

	//ライト設定
	CVector3D dir = CMatrix::MRotation(Camera::Instance()->m_rot).GetFront();
	CLight::SetType(0, CLight::eLight_Spot);
	CLight::SetType(1, CLight::eLight_Point);
	CLight::SetDir(0, dir);
	CLight::SetRadiationAngle(0, DtoR(35));
	CLight::SetPos(0, m_model->GetFrameMatrix(8).GetPosition() + dir * 0.2f);
	CLight::SetPos(1, m_model->GetFrameMatrix(8).GetPosition() + dir * 0.2f);
	//CVector3D flashlightDir = dir;  // プレイヤーの方向ベクトルを取得
	//CLight::SetDir(1, flashlightDir.GetNormalize()); // ライトの方向を設定
	CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.7f, 0.7f, 0.7f));
	CLight::SetColor(0, CVector3D(0.0f, 0.0f, 0.0f), CVector3D(0.8f, 0.8f, 0.8f));
	CLight::SetRange(1, 2.5f);
	CLight::SetRange(0, 20);
	//CLight::SetSpotCutoff(1, 30.0f);
	//CLight::SetSpotExponent(1, 2.0f);
	//CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.6f, 0.6f, 0.6f));
	//マックス値より所持金が少なかったら
	//お金を増やす
	if (PlayerMoneyMax > PlayerMoney)
	{
		PlayerMoney++;
	}
	//マックス値より所持金が多かったら
	//お金を減らす
	if (PlayerMoneyMax < PlayerMoney)
	{
		PlayerMoney--;
	}



	if (m_hp > 0)
	{
		//所持金のフォント
		//FONT_T()->Draw(1500, 712, 1.0f, 1.0f, .0f, "所持金%d", PlayerMoney);

		FONT_T()->Draw(0, 100, 0.0f, 0.0f, .0f, "マウススクロールで一人称/三人称切り替え");
	}


	m_isEkeyPressd = PUSH(CInput::eButton2);
	m_isSiftPressd = HOLD(CInput::eButton4);
	m_isSpacePressd = PUSH(CInput::eButton5);
	m_isLclickPressd = PUSH(CInput::eMouseL);

	//ジャンプインターバル
	jump_interval -= 1.0f;

	//無敵時間--
	mutekijikan -= 1.0f;
	threadattachtimer -= 1.0f;

	if (m_isAction)
	{
		m_JumpDelay += 0.08;
	}

	//スタミナ回復
	if (m_stamina < m_stamina_max && !m_isSiftPressd && !m_isSpacePressd && m_state != eState_Attack00 && m_state != eState_Attack01)
	{
		count++;
		if (count >= 25)
			m_stamina += 1.0;
	}
	else
	{
		count = 0;
	}

	//ノックバック中
	if (m_isKnockback)
	{
		// 吹き飛び処理の継続
		m_pos += m_velocity * CFPS::GetDeltaTime(); // 速度を位置に適用
		m_velocity *= 0.9f; // 徐々に減速

		// 吹き飛び時間が終了したら解除
		m_knockbackTimer -= CFPS::GetDeltaTime();
		if (m_knockbackTimer <= 0.0f)
		{
			m_isKnockback = false;
			m_velocity = CVector3D(0, 0, 0); // 速度をリセット
		}
	}

	//死亡した時にタイトルに戻る
	if (m_hp <= 0)
	{
		m_state = eState_Down;
		GameOvercount += 1;
		if (GameOvercount > 230)
		{
			BlackFilter* b = dynamic_cast<BlackFilter*>(Base::FindObject(eRedFilter));
			if (b == __nullptr)
				Base::Add(b = new BlackFilter());
			b->BlackShow(130.0f);
		}
		if (GameOvercount > 360)
		{
			SceneManager::LoadTitle();//タイトルに戻る
		}

	}


	if (m_hp > 0)
	{
		//キャラクターの回転値をカメラの回転値に合わせる
		m_rot.y = Camera::Instance()->m_rot.y;

		m_model->BindFrameMatrix(5, CMatrix::MRotation(Camera::Instance()->m_rot));

		m_velocity *= m_friction;

		m_pos += m_velocity;
	}

	//地面から離れている場合の処理
	if (!m_isGround)
	{
		m_isFalling = true;
	}

	if (m_weight <= 0)
	{
		m_weight = 0.0f;
	}
	float GravityEffect = GRAVITY + ((m_weight / 2000.0f));

	//重力落下
	m_pos += m_vec;
	m_vec.y -= GravityEffect;
	m_vec.x *= 0.95;
	m_vec.z *= 0.95;

	if (m_carry)
	{
		FONT_T()->Draw(800, 712, 1.0f, 1.0f, 1.0f, "Drop Fkey");
	}

	if (PUSH(CInput::eButton3) && m_carry)
	{
		if (m_isAttacking)
		{
			return;
		}

		//	if (eWeapon)
		//		SOUND("剣落とす")->Play(false);
		DropCarry(m_carry);

	}

	if (m_state == eState_Down && m_carry)
	{
		DropCarry(m_carry);
	}

	UpdateState();

	m_model->UpdateAnimation();

	// 押し返しを適用
	if (m_pushBackForce.LengthSq() > 0)
	{
		m_pos += m_pushBackForce; // 押し返しの力を適用
		m_pushBackForce *= 0.9f; // 力を減衰させる（じわじわと押し出される感じ）
	}

	//画面左下にゲージの位置を設定
	if (m_gauge) {
		m_gauge->m_pos = CVector3D(0, 900, 0);
		m_gauge->SetValue((float)m_hp / m_hp_max);
	}
	if (m_gauge1) {
		m_gauge1->m_pos = CVector3D(0, 980, 0);
		m_gauge1->SetValue((float)m_stamina / m_stamina_max);
	}

}

bool Player::GetAttack()
{
	return m_isAttacking;
}

bool Player::GetAttackRelease() const
{
	return m_isAttackReleased;
}

CVector3D Player::GetPos()
{
	return m_pos;
}

bool Player::GetMove()
{
	return m_isMove;
}

bool Player::GetCarry()
{
	return m_carry;
}

bool Player::GetDashing()
{
	return m_isDashing;
}

//所持金を変動させる関数たち
int Player::Pay(int Money)
{
	return PlayerMoneyMax -= Money;
}

void Player::Draw()
{
	CImage& img = m_inventory_image; // スロット画像
	CVector2D base_pos(540, 960);   // スロット表示の基準位置
	const int icon_size = 96;       // スロット1つのサイズ
	const int src_size = 128;       // スロット画像内の1アイコンのサイズ
	img.SetSize(icon_size, icon_size);

	for (int i = 0; i < m_maxItems; i++) {
		CVector2D slot_pos = base_pos + CVector2D((icon_size + 10) * i, 0);

		// 1. 背景を描画
		img.SetPos(slot_pos);
		img.SetRect(0, 0, src_size, src_size); // 背景用の画像範囲
		img.SetColor(0.2f, 0.2f, 0.2f, 1.0f); // デフォルト背景色（暗いグレー）
		img.Draw();

		// アイテムがある場合は背景を少し明るく
		if (m_inventory[i]) {
			img.SetColor(0.4f, 0.4f, 0.4f, 1.0f); // 明るいグレー
			img.Draw();
		}

		// 2. スロット枠を描画（選択状態をハイライト）
		img.SetRect(0, 0, src_size, src_size); // スロット枠
		if (m_inventory[i] && m_inventory[i] == m_carry) {
			img.SetColor(1.0f, 1.0f, 0.0f, 1.0f); // 選択中の枠色（黄色）
		} else {
			img.SetColor(0.6f, 0.6f, 0.6f, 1.0f); // 通常枠色
		}
		img.Draw();

		// 3. アイテムを描画
		if (m_inventory[i]) {
			int idx = 2 + m_inventory[i]->m_item_id; // アイテムIDに応じたアイコン
			img.SetRect(0, idx * src_size, src_size, (idx + 1) * src_size);
			img.SetColor(1.0f, 1.0f, 1.0f, 1.0f); // アイコンは明るく描画
			img.Draw();
		}

		// 4. スロット番号を描画
		FONT_T()->Draw(slot_pos.x + 36, slot_pos.y + 100, 0.8f, 0.8f, 1.0f, "%d", i + 1);
	}
}

int Player::GetPlayerMoney()
{
	return PlayerMoneyMax;
}

void Player::GainMoney(int Money)
{
	PlayerMoneyMax += Money;

}

void Player::Heal(int HealValue)
{
	m_hp += HealValue;
	if (m_hp > m_hp_max) m_hp = m_hp_max;

}

void Player::MovespeedUp(float speedUp)
{
	move_speed += speedUp;
}

bool Player::PlayerJump(bool canjump)
{
	return m_isJump = canjump;
}

void Player::ApplyFallDamage()
{
	//落下ダメージが発生する速度の闘値
	float fallDamageValue = -0.5f; //この値より速いとダメージ

	//プレイヤーが地面に着地したときに、垂直速度に基づいてダメージを計算
	if (m_vec.y < fallDamageValue)
	{
		//速度に応じたダメージを計算
		float fallSpeed = m_vec.y;
		float damage = fallSpeed * -75.0f;

		//プレイヤーにダメージを適用
		m_hp -= damage;
		RedFilterDisplay();

		//HPが0になったら場合の処理
		if (m_hp <= 0)
		{
			m_hp = 0;
			//死亡処理
			m_state = eState_Down;
		}
	}

}

// プレイヤーに担がせる
void Player::SetCarry(Carry* carry)
{
	if (m_carry) {
		m_carry->PutInPocket();  //今の装備を外す
	}
	//アイテムを拾う
	if (!PickUpItem(carry)) {
		//拾えなかったら
		// すでに持っているアイテムがあれば、捨てる
		if (m_carry) {
			m_carry->Unequip();
			// アイテムを少し上に配置（ドロップ）
			m_carry->m_pos = m_pos + CVector3D(0, 0.8f, 0);
			// インベントリから削除
			auto it = std::find(m_inventory.begin(), m_inventory.end(), m_carry);
			if (it != m_inventory.end()) {
				*it = nullptr;
			}
			//もう一回拾う
			PickUpItem(carry);
		}
	}
	m_carry = carry;
	if (m_carry) {
		m_carry->Equip();  // 新しいアイテムを装備
		m_isCarrying = true;
		carry->m_stateItem != Carry::e_Equip;
	}
}



float Player::GetHp()
{
	return m_hp;
}

float Player::GetDamage(float damage)
{
	return m_hp -= damage;
}



CVector3D Player::GetRotation() const
{
	return m_rot;
}

void Player::SetRotation(const CVector3D& rotation)
{
	m_rot = rotation;
}

void Player::UpdateModelRotation(const CVector3D& rotation)
{
	m_model->BindFrameMatrix(5, CMatrix::MRotation(rotation));
}


// アイテムをドロップする
void Player::DropCarry(Carry* carry)
{
	if (m_carry && m_carry == carry) {
		// 現在のアイテムの装備解除
		m_carry->Unequip();
		m_carry->m_stateItem = Carry::e_Drop; // 状態をドロップに設定
		m_isCarrying = false;

		// アイテムを少し上に配置（ドロップ）
		m_carry->m_pos = m_pos + CVector3D(0, 0.8f, 0);


		// インベントリから削除
		auto it = std::find(m_inventory.begin(), m_inventory.end(), carry);
		if (it != m_inventory.end()) {
			*it = nullptr;
		}

		m_carry = nullptr;
		// インベントリに他のアイテムがある場合だけ装備する
		if (!m_isCarrying && m_carry == nullptr) {
			for (auto& item : m_inventory) {
				if (item) {
					m_carry = item;
					m_carry->Equip();
					m_carry->m_stateItem = Carry::e_Equip;
					m_isCarrying = true;
					break;
				}
			}
		}
	}
}


//
bool Player::PickUpItem(Carry* carry)
{
	int empty_idx = -1;
	int i = 0;
	//空きスロットを調べる
	for (auto& c : m_inventory) {
		if (!c) {
			empty_idx = i;
			break;
		}
		i++;
	}
	//空きがあれば
	if (empty_idx >= 0) {
		m_inventory[empty_idx] = carry;
		carry->PutInPocket();
		return true;
	}
	else {
		// 所持アイテム数が上限に達している
		return false;
	}
}

void Player::SwapItem(int idx)
{
	//スロットが空なら何もしない
	if (!m_inventory[idx])
	{
		return;
	}

	//既に選択されている場合も何もしない
	if (m_inventory[idx] == m_carry) return;
	
	//現在装備中のアイテムをポケットに戻す。
	if (m_carry) {
		// 現在のアイテムの装備解除
		m_carry->PutInPocket();
	}

	// インベントリのアイテムと入れ替え
	m_carry = m_inventory[idx];
	m_carry->Equip();

}

void Player::SelectItem()
{
	for (int i = 0; i < m_inventory.size(); i++) {
		if (PUSH((CInput::E_BUTTON)(CInput::eNum1 + i))) {
			SwapItem(i);
		}
	}
}

//重さ関連
void Player::SetWeight(float weight)
{
	m_weight = weight;
}

float Player::GetWeight() const
{
	return m_weight;
}

// 元の重さを保存する関数
void Player::SaveOriginalWeight() {
	m_originalWeight = m_weight;
}


// 重さを元に戻す関数
void Player::ResetToOriginalWeight() {
	if (m_inventory.size() <= 0)
		m_weight = m_originalWeight;
}

/*
void Player::Sell(Enemy * enemy)
{
	int money = enemy->GetSellValue();
	m_moneys.AddMoney(money);
	enemy->SetKill();
}
*/

//ジェットパック関連
void Player::JetPackOn()
{
	if (m_jetpackFuel > 0.0f)
	{
		m_isJetpackActive = true;
	}
}

void Player::JetPackOff()
{
	m_Force = 0.0f;
	m_isJetpackActive = false;
}

void Player::JetPackFuel(int shouhiryou)
{
	if (m_jetpackFuel > 0.0f)
	{
		m_jetpackFuel -= shouhiryou;
		if (m_jetpackFuel <= 0.0f)
		{
			m_jetpackFuel = 0.0f;
			JetPackOff();
		}
	}
}
bool Player::GetjetpackActive()
{
	return m_isJetpackActive;
}

//描画
void Player::Render()
{
	m_model->SetPos(m_pos);
	m_model->SetRot(m_rot);
	m_model->SetScale(0.01f, 0.01f, 0.01f);
	m_model->UpdateMatrix();
	m_model->Render();

	printf("%f\n", m_Force);

	m_lineS = m_pos + CVector3D(0, 1.8f - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 0, 1, 0.5));

	//ジェットパックの燃料を表示したい
	if (m_isJetpackActive)
	{
		FONT_T()->Draw(200, 200, 1.0f, 0.0f, 0.0f, "燃料 %d", m_jetpackFuel);
	}

	// プレイヤーとカメラの距離を計算
	float DistanceToCamera = Camera::Instance()->GetDistance();

	// 頭部を非表示にする距離の閾値
	float hideHeadDistance = 0.2f;

	// 頭部を非表示にする条件を設定
	bool hideHead = (DistanceToCamera < hideHeadDistance);

	if (hideHead)
	{
		m_model->GetNode(69)->SetVisibility(false);

	}
	else
	{
		m_model->GetNode(69)->SetVisibility(true);
	}

}

void Player::Collision(Base* b)
{

	switch (b->GetType()) {

	case eEnemy:
		if (Enemy* e = dynamic_cast<Enemy*>(b))
		{
			int EnemyHp = e->GetHP();
			if (EnemyHp > 0)
			{
				//カプセル同士の衝突
				if (CCollision::CollisionCapsule(e->m_lineS, e->m_lineE, e->m_rad,
					m_lineS, m_lineE, m_rad))
				{
					// 押し返しの計算
					CVector3D pushBackDir = m_pos - e->GetPos(); // 敵からプレイヤーへの方向ベクトル
					pushBackDir.Normalize(); // 正規化して方向ベクトルにする

					// 押し返しの力を累積する
					m_pushBackForce += pushBackDir * 0.008; // 強さを調整
				}
			}

			if (e->GetAttacking() && e->GetHP() > 0 && m_hp > 0)
			{
				{
					//カプセル同士の衝突
					if (CCollision::CollisionCapsule(e->m_AttackS, e->m_AttackE, e->m_rad,
						m_lineS, m_lineE, m_rad))

					{

						if (mutekijikan <= 0)
						{
							int damage = e->GetDamage();

							m_hp -= damage;
							mutekijikan = 80.0f;

							RedFilterDisplay();

							// 押し返しの計算
							CVector3D pushBackDir = m_pos - e->GetPos(); // 敵からプレイヤーへの方向ベクトル
							pushBackDir.Normalize(); // 正規化して方向ベクトルにする

							// 押し返しの力を累積する
							m_pushBackForce += pushBackDir * m_pushBackStrength; // 強さを調整


						}
					}

				}
			}
		}

		if (GreenDragon* g = dynamic_cast<GreenDragon*>(b))
		{
			{
				int EnemyHp = g->GetHP();
				if (EnemyHp > 0)
				{
					//カプセル同士の衝突
					if (CCollision::CollisionCapsule(g->m_lineS, g->m_lineE, g->m_rad,
						m_lineS, m_lineE, m_rad))
					{
						// 押し返しの計算
						CVector3D pushBackDir = m_pos - g->GetPos(); // 敵からプレイヤーへの方向ベクトル
						pushBackDir.Normalize(); // 正規化して方向ベクトルにする

						// 押し返しの力を累積する
						m_pushBackForce += pushBackDir * 0.005; // 強さを調整
					}
				}
			}
		}

		break;

	case eSpiderBullet:
	{
		if (SpiderBullet* w = dynamic_cast<SpiderBullet*>(b))
		{
			//カプセル同士の衝突
			if (CCollision::CollisionCapsuleShpere(m_lineS, m_lineE, 0.4f, w->m_pos, w->m_rad))
			{
				if (threadattachtimer <= 0)
				{
					int damage = 2;

					m_hp -= damage;
					threadattachtimer = 180.0f;

					RedFilterDisplay();
				}
			}
		}

	}
	break;

	case eField: {
		CVector3D v(0, 0, 0);

		auto tri = b->GetModel()->CollisionCupsel(
			m_pos + CVector3D(0, 2.0f - m_rad, 0), // 始点
			m_pos + CVector3D(0, m_rad, 0),       // 終点
			m_rad
		);

		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				if (m_vec.y > 0) m_vec.y = 0; // 上昇をリセット
			}
			else if (t.m_normal.y > 0.5f) {
				ApplyFallDamage();
				if (m_vec.y < 0) m_vec.y = 0; // 落下をリセット

				// ジャンプ中かつ地面に着地した場合のみフラグをリセット
				if (!m_isGround) {
					m_isGround = true;
					m_isJump = true;
				}
			}

			// めり込み分の押し戻し計算
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
			v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
		}

		// 押し戻しを適用
		m_pos += v;
	} break;
	}

}
