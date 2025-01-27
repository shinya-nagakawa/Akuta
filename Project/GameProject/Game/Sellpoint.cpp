#include "Sellpoint.h"
#include "Enemy.h"
#include "Spider.h"
#include "Player.h"
#include "GreenDragon.h"
#include "Game/Camera.h"

Sellpoint::Sellpoint(const CVector3D& pos): Carry(eSellpoint),m_totalEarnings(0.0f), m_isSellFrag(false),
m_showEarnings(false), m_displayDuration(0.0f), m_elapsedDisplayTime(0.0f), m_displayedEarnings(0.0f)
{
	m_rad = 0.6f;
	m_pos = pos;
	m_isSellFrag = false;
}

void Sellpoint::Update()
{
	if (m_showEarnings) {
		//徐々に金額を目標金額に近づける
		if (m_displayedEarnings < m_totalEarnings)
		{
			float increment = (m_totalEarnings - m_displayedEarnings) * 0.1f;
			m_displayedEarnings += increment;
			//効果音を再生する
			if (!buyPlayed)
			{
				SOUND("売却中")->Play();
				buyPlayed = true;
			}

			//誤差を避けるため、最終的にピッタリ合わせる
			if (fabs(m_displayedEarnings - m_totalEarnings) < 0.01f)
			{
				m_displayedEarnings = m_totalEarnings;

				//効果音を再生する
				if (!soundPlayed)
				{
					SOUND("売却中")->Stop();
					SOUND("売却音")->Play();
					soundPlayed = true;
				}
			}
		}

		//表示時間を管理
		m_elapsedDisplayTime += CFPS::GetDeltaTime();
		if (m_elapsedDisplayTime >= m_displayDuration)
		{
			m_showEarnings = false;  // 通知を非表示
			m_elapsedDisplayTime = 0.0f;  // 時間リセット
			m_totalEarnings = 0.0f;       // 総額リセット
			m_displayedEarnings = 0.0f;   // 表示金額リセット

			//効果音のフラグをリセット
			soundPlayed = false;
			buyPlayed = false;
		}
	}
}

void Sellpoint::Render()
{
	
	 if (m_showEarnings) 
	 {
        std::wstring totalText = L"総売却金額: " + std::to_wstring(static_cast<int>(m_displayedEarnings)) + L" G";
        m_renderer.DrawTextWindow(100, 100, 400, 100, totalText.c_str(),
            CVector4D(0.0f, 0.0f, 0.0f, 0.7f),  // 背景色
            CVector3D(1.0f, 1.0f, 1.0f),        // 文字色
            10);                                // パディング
    }

     // カプセルを描画
     m_lineS = m_pos + CVector3D(0, 2.0f - m_rad, 0);
     m_lineE = m_pos + CVector3D(0, m_rad, 0);
     Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));

     /*
     // カプセルの真上の位置（ワールド座標）
     CVector3D textWorldPos = m_pos + CVector3D(0, 2.5f, 0); // 高さを調整

     // カメラのインスタンスを取得
     Camera* camera = Camera::Instance();
     if (!camera) return; // カメラが存在しない場合は何もしない

     // カメラの行列を取得
     CMatrix viewMatrix = camera->GetViewMatrix();
     CMatrix projectionMatrix = camera->GetProjectionMatrix();

     // ワールド座標をスクリーン座標に変換
     CVector4D worldPos4(textWorldPos.x, textWorldPos.y, textWorldPos.z, 1.0f);
     CVector4D screenPos = projectionMatrix * (viewMatrix * worldPos4);

     // スクリーン座標を正規化
     if (screenPos.w <= 0.0f) return; // 背面にある場合は表示しない
     screenPos.x /= screenPos.w;
     screenPos.y /= screenPos.w;
     screenPos.z /= screenPos.w;

     // スクリーン座標をピクセル単位に変換
     int screenX = static_cast<int>((screenPos.x * 0.5f + 0.5f) * SCREEN_WIDTH);
     int screenY = static_cast<int>((-screenPos.y * 0.5f + 0.5f) * SCREEN_HEIGHT);

     // === 修正: カメラの上下移動で文字が飛ばないようにする ===
     // カプセルの位置が画面外なら表示しない
     if (screenX < 0 || screenX > SCREEN_WIDTH || screenY < 0 || screenY > SCREEN_HEIGHT) return;

     // テキストを描画
     const char* message = isInside ? "売却可能" : "売却不可";
     FONT_T()->Draw(screenX, screenY, 1.0f, 1.0f, 1.0f, message);*/
}

void Sellpoint::Collision(Base* b)
{
    isInside = false;

    switch (b->GetType())
    {
    case ePlayer:
        if (Player* player = dynamic_cast<Player*>(b))
        {
            if (!Base::FindObject(eEnemy)) return;

            // プレイヤーがカプセル内にいる場合
            if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
                m_lineS, m_lineE, m_rad))
            {
                FONT_T()->Draw(1500, 750, 1.0f, 1.0f, 1.0f, "気絶した敵を落とし");
                FONT_T()->Draw(1500, 850, 1.0f, 1.0f, 1.0f, "クリックで売却");

                // マウスクリック時に売却フラグを立てる
                if (1/*PUSH(CInput::eMouseL)*/)
                {
                    m_isSellFrag = true;
                }
                else
                {
                    m_isSellFrag = false;
                }
            }
        }
        break;

    case eEnemy:
        // 敵（Enemy, Spider, GreenDragon）がカプセル内にいるかチェック
        if (Enemy* enemy = dynamic_cast<Enemy*>(b))
        {
            if (IsInsideCapsule(enemy))
            {
                if (enemy->m_stateItem != e_Equip && enemy->m_stateItem != e_PickUp)
                {
                    isInside = true;
                }
                else 
                {
                    isInside = false;
                }
                if (enemy->GetHP() <= 0 && m_isSellFrag && enemy->m_stateItem != e_Equip)
                {
                    enemy->SetKill();
                    int earnings1 = 300;
                    enemy->GiveMoney(earnings1);
                    TriggerEarnings(earnings1);
                }
            }
        }
        if (Spider* spider = dynamic_cast<Spider*>(b))
        {
            if (IsInsideCapsule(spider))
            {
                if (spider->m_stateItem != e_Equip && spider->m_stateItem != e_PickUp)
                {
                    isInside = true;
                }
                else 
                {
                    isInside = false;
                }
                if (spider->GetHP() <= 0 && m_isSellFrag && spider->m_stateItem != e_Equip)
                {
                    spider->SetKill();
                    int earnings2 = 450;
                    spider->GiveMoney(earnings2);
                    TriggerEarnings(earnings2);
                }
            }
        }
        if (GreenDragon* dragon = dynamic_cast<GreenDragon*>(b))
        {
            if (IsInsideCapsule(dragon))
            {
                if (dragon->m_stateItem != e_Equip && dragon->m_stateItem != e_PickUp)
                {
                    isInside = true;
                }
                else
                {
                    isInside = false;
                }
                if (dragon->GetHP() <= 0 && m_isSellFrag && dragon->m_stateItem != e_Equip)
                {
                    dragon->SetKill();
                    int earnings3 = 1050;
                    dragon->GiveMoney(earnings3);
                    TriggerEarnings(earnings3);
                }
            }
        }
        break;
    }

    // カプセル内に敵がいる場合のメッセージ表示
    if (isInside)
    {
        FONT_T()->Draw(1500, 900, 1.0f, 1.0f, 1.0f, "敵が売却可能な位置にいます");
    }
}

// カプセル内にオブジェクトがいるかを判定する共通関数
bool Sellpoint::IsInsideCapsule(Base* b)
{
    return CCollision::CollisionCapsule(b->m_lineS, b->m_lineE, b->m_rad, m_lineS, m_lineE, m_rad);
}

void Sellpoint::TriggerEarnings(int earnings)
{
	m_totalEarnings += earnings;   // 総額を加算
	m_showEarnings = true;         // 通知を表示
	m_displayDuration = 3.0f;      // 表示時間を設定
	m_elapsedDisplayTime = 0.0f;   // 表示経過時間をリセット
}
