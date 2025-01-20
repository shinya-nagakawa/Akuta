#include "SpiderBullet.h"

SpiderBullet::SpiderBullet(Spider* parent,const CVector3D& pos, const float speed, const CVector3D& target) :Base(eSpiderBullet)
{
	gravity = 0.98f / 60 * 0.1;
	m_pos = pos + CVector3D(0, 0.2f, 0); // Y軸を0.2fだけ持ち上げる
	count = 3.5f;

	mp_parent = parent;
	m_targetPos = target;
	m_state = eState_Normal;

	m_isAttachedToPlayer = false;

	// 弾速にばらつきを持たせる
	float speedVariance = 0.05f; // 弾速のばらつき幅 (例: ±0.05)
	float randomSpeed = speed + ((rand() % 100) / 100.0f * 2.0f - 1.0f) * speedVariance;

	// ランダムな初期角度を設定
	float randomAngle = DtoR(5 + (rand() % 15)); // 5～20度の範囲で角度をランダム化

	// 目標へのベクトル
	CVector3D vec = target - pos;
	float y = atan2(vec.x, vec.z);

	// 目標までの距離
	float l = (CVector3D(target.x, 1.5f, target.z) - CVector3D(pos.x, 0, pos.z)).Length();
	
	//高さの補正因子
	float a = (gravity * l * l) / (2.0 * randomSpeed * randomSpeed);
	//時間に基づく係数
	float b = l / a;
	//高さの補正項
	float c = (a + vec.y) / a;

	//放射線
	float root = pow(-c + (b * b) / 4.0, 0.5);

	//射出角度
	float ts = (b * b / 4) - c;
	if (ts < 0.0)
	{
		m_vec = CMatrix::MRotation(-DtoR(randomAngle), y, 0).GetFront() * randomSpeed;
		m_state = eState_Flying;
	}
	else
	{
		float s = atan((-b / 1.95) + root);
		m_vec = CMatrix::MRotation(s, y, 0).GetFront() * randomSpeed;
		m_state = eState_Flying;
	}

	m_lines.push_back(m_pos);
}

void SpiderBullet::Update()
{
	// デバッグ描画：糸のカプセル判定範囲を描画
	//Utility::DrawCapsule(m_pos, m_targetPos, 0.2f, CVector4D(1, 0, 0, 0.5f)); // 赤いカプセルで描画

	// 常に親の位置を始点に反映
	if (mp_parent)
	{
		CVector3D parentPos = mp_parent->GetPos() + CVector3D(0, 0.2f, 0); // 蜘蛛のお尻位置
		if (!m_lines.empty())
		{
			m_lines[0] = parentPos; // 糸の始点を更新
		}
	}

	// プレイヤーにくっついている場合
	if (m_isAttachedToPlayer&&m_state !=eState_Cut)
	{
		if (Player* player = Player::Instance())
		{
			// プレイヤーの移動に合わせて衝突点を更新
			m_attachmentPoint = player->m_pos + m_relativeAttachmentPoint;
			m_pos = m_attachmentPoint; // 糸の終点を更新

			// 一定距離以上離れた場合、糸を消す
			float distance = (m_attachmentPoint - m_lines[0]).Length();
			if (distance > 12.0f || mp_parent->GetHP() <= 0) // 消える距離
			{
				SetKill(); // 糸を消す
				return;
			}
		}

		// 糸の再生成
		RegenerateLine(0.0f);
		return; // くっついている間は飛行処理をスキップ
	}

	// 衝突判定
	if (CheckCollisionWithPlayer())
	{
		if (!m_isAttachedToPlayer) // 初回の衝突時のみ処理
		{
			if (Player* player = Player::Instance())
			{
				// 衝突時の相対位置を計算
				m_relativeAttachmentPoint = m_attachmentPoint - player->m_pos;
			}
		}
	}

	if (m_state == eState_Cut)
	{
		m_disappearTimer += CFPS::GetDeltaTime();
		m_scale = 1.0f - (m_disappearTimer / 1.5f);

		//スケールが0なら消滅
		if (m_scale <= 0.0f)
		{
			SetKill();
			return;
		}

		RegenerateLine(0.1f * m_scale);
		return;
	}

	if (m_state == eState_Flying)
	{
		m_vec.y += -gravity;  // 重力適用
		m_pos += m_vec;       // 移動

		// 糸の再生成
		RegenerateLine(0.15f);

		// 衝突判定（地形など）
		Base* field = Base::FindObject(eField);
		if (field)
		{
			auto collisions = field->GetModel()->CollisionSphere(m_pos, 0.1f);
			if (!collisions.empty())
			{
				m_state = eState_Stuck;
				m_vec = CVector3D(0, 0, 0); // 速度をゼロにする
				return;
			}
		}

		// 飛行中の消滅タイマー
		count -= CFPS::GetDeltaTime();
		if (count <= 0)
		{
			SetKill(); // 飛行中でも時間経過で消える
			return;
		}
	}
	else if (m_state == eState_Stuck)
	{
		// 停止状態でもタイマーを減らす
		count -= CFPS::GetDeltaTime();
		if (count <= 0)
		{
			SetKill(); // 停止後も一定時間で消える
			return;
		}

		// 停止中でも糸の再生成を行う
		RegenerateLine(0.07);
	}

}

void SpiderBullet::Render() 
{
	//Utility::DrawSphere(m_pos, 0.2f, CVector4D(0, 0, 1, 1));
	//2点で線を引く
	if (m_state == eState_Cut && m_scale > 0.0f)
	{
		// スケールを考慮して描画
		auto it = m_lines.begin();
		while (1)
		{
			auto it2 = it;
			it2++;
			if (it2 == m_lines.end()) break;

			// 線分描画（スケールを適用）
			Utility::DrawLine(*it, *it2, CVector4D(1, 1, 1, m_scale), 4 * m_scale);
			it++;
		}
	}
	else
	{
		// 通常の糸描画
		auto it = m_lines.begin();
		while (1)
		{
			auto it2 = it;
			it2++;
			if (it2 == m_lines.end()) break;

			Utility::DrawLine(*it, *it2, CVector4D(1, 1, 1, 1), 4);
			it++;
		}
	}

	/*
	if (m_isAttachedToPlayer)
	{
		// デバッグ：プレイヤーにくっついた場合の点を描画
		Utility::DrawSphere(m_attachmentPoint, 0.3f, CVector4D(0, 1, 0, 1));
	}
	if (m_state == eState_Stuck)
	{
		//デバッグ用
		Utility::DrawSphere(m_pos, 0.3f, CVector4D(1, 0, 0, 1));
	}*/

}

void SpiderBullet::Cut()
{
	if (m_state != eState_Cut)
	{
		m_state = eState_Cut; // 状態を切断中に設定
		m_disappearTimer = 0.0f; // 消えるまでの時間を初期化
		m_scale = 1.0f; // 縮小処理の初期値
		m_isAttachedToPlayer = false;

		// 糸の消滅アニメーションを初期化
		m_lines.clear();
		m_lines.push_back(m_pos); // 開始位置だけ残す
	}
}
;

void SpiderBullet::RegenerateLine(float noiseStrength)
{
	if (m_lines.empty()) return;

	// 現在の始点と終点を取得
	CVector3D start = m_lines[0]; // 始点
	CVector3D end = m_pos;        // 終点

	// 新しい糸を生成
	m_lines.clear();
	const int segments = 20; // 線分の数
	for (int i = 0; i <= segments; ++i)
	{
		float t = static_cast<float>(i) / segments;

		// 線形補間
		CVector3D point = start * (1.0f - t) + end * t;

		if (i > 0 && i < segments)
		{
			// ノイズを加える
			float noiseX = ((rand() % 100) / 500.0f) * (2 * noiseStrength) - noiseStrength; // -0.1 ～ +0.1
			float noiseY = ((rand() % 100) / 500.0f) * (2 * noiseStrength) - noiseStrength; // -0.1 ～ +0.1
			float noiseZ = ((rand() % 100) / 500.0f) * (2 * noiseStrength) - noiseStrength; // -0.1 ～ +0.1

			point += CVector3D(noiseX, noiseY, noiseZ);
		}

		/*
			t の計算
			各線分が全体のどこに位置するかを割合で計算。

			point の計算
			線形補間で始点から終点までの位置を算出。

			結果
			m_lines に補間された点が順番に格納されるので、滑らかな糸が描画できる。
		*/


		m_lines.push_back(point);
	}

}

bool SpiderBullet::CheckCollisionWithPlayer()
{
	const int numSegments = 10; // 糸を分割するセグメント数
	for (int i = 0; i < numSegments; ++i)
	{
		// 各セグメントの始点と終点を計算
		float t = static_cast<float>(i) / numSegments;
		float nextT = static_cast<float>(i + 1) / numSegments;

		CVector3D segmentStart = m_lines[0] + (m_lines.back() - m_lines[0]) * t;
		CVector3D segmentEnd = m_lines[0] + (m_lines.back() - m_lines[0]) * nextT;

		// プレイヤーとのコリジョン判定
		if (Player* player = Player::Instance())
		{
			CVector3D collisionPoint; // 衝突点
			if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
				segmentStart, segmentEnd, 0.1f, nullptr, &collisionPoint))
			{
				m_isAttachedToPlayer = true;

				// ランダムな接続点を生成
				float randomHeight = (rand() % 100) / 100.0f; // 0.0 ～ 1.0 のランダム値
				float randomOffset = ((rand() % 200) - 100) / 100.0f; // -1.0 ～ 1.0 のランダム値

				// プレイヤーのコライダー範囲内でランダムな位置を計算
				CVector3D basePoint = player->m_lineS * (1.0f - randomHeight) + player->m_lineE * randomHeight;
				m_attachmentPoint = basePoint + CVector3D(randomOffset * player->m_rad, 0.0f, randomOffset * player->m_rad);

				// 衝突時の相対位置を計算
				m_relativeAttachmentPoint = m_attachmentPoint - player->m_pos;

				player->GetDamage(0.1f); // ダメージを与える
				return true;
			}
		}
	}
	return false; // 衝突なし
}

