#include "Enemy_Base.h"

Enemy_Base::Enemy_Base(const CVector3D& pos, float viewAngle, float viewLength, float collisionRadius):Carry(eEnemy)
	, m_pos(pos), m_viewAngle(viewAngle), m_viewLength(viewLength), m_collisionRadius(collisionRadius)
{

}

void Enemy_Base::Render()
{
}

bool Enemy_Base::IsFoundPlayer() const
{
    if (Player* player = Player::Instance()) {
        CVector3D vec = player->m_pos - m_pos;
        vec.y = 0.0f;

        float dist = vec.Length();
        if (dist > m_viewLength) return false;

        CVector3D dir = vec.GetNormalize();
        float dot = CVector3D::Dot(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)), dir);

        if (dot < cosf(DtoR(m_viewAngle / 2))) return false;

        return IsLookPlayer();
    }
    return false;
}

bool Enemy_Base::IsLookPlayer() const
{
    if (Player* player = Player::Instance()) {
        CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // 敵の目線の高さ
        CVector3D end = player->m_pos + CVector3D(0, 1.0f, 0); // プレイヤーの目線の高さ

        // デバッグ用: レイを可視化
       // Utility::DrawLine(start, end, CVector4D(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色い線で描画

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
       // Utility::DrawLine(start, end, CVector4D(0.0f, 1.0f, 0.0f, 1.0f)); // 緑の線で描画
        return true;
    }

    return false;
}

bool Enemy_Base::CheckWallCollision(const CVector3D& dir)
{
    CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // 目線の高さ
    CVector3D end = start + dir * m_collisionRadius; // 衝突範囲を伸ばした位置

    CVector3D hitPos, hitNormal;
    if (Base* b = Base::FindObject(eField)) {
        if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
            // 壁が衝突範囲内にある場合
            //Utility::DrawSphere(hitPos, 0.2f, CVector4D(1.0f, 0.0f, 0.0f, 1.0f)); // 衝突点を赤い球で描画
            return true;
        }
    }

    return false; // 壁がない
}

bool Enemy_Base::IsWallNearby(float checkDistance)
{
    // チェックする方向リスト
    std::vector<CVector3D> directions = {
        {1, 0, 0},   // 前
        {-1, 0, 0},  // 後
        {0, 0, 1},   // 右
        {0, 0, -1},  // 左
        {0.707f, 0, 0.707f},   // 右前
        {-0.707f, 0, 0.707f},  // 左前
        {0.707f, 0, -0.707f},  // 右後
        {-0.707f, 0, -0.707f}  // 左後
    };

    for (const auto& dir : directions) {
        if (CheckWallCollision(dir * checkDistance)) {
            return true; // 壁が見つかった
        }
    }

    return false; // 壁が見つからない
}

CVector3D Enemy_Base::CalculateAvoidDirection(float checkDistance)
{
    CVector3D avoidDir(0, 0, 0);

    // チェックする方向リスト
    std::vector<CVector3D> directions = {
        {1, 0, 0},   // 前
        {-1, 0, 0},  // 後
        {0, 0, 1},   // 右
        {0, 0, -1},  // 左
        {0.707f, 0, 0.707f},   // 右前
        {-0.707f, 0, 0.707f},  // 左前
        {0.707f, 0, -0.707f},  // 右後
        {-0.707f, 0, -0.707f}  // 左後
    };

    for (const auto& dir : directions) {
        CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // 目線の高さ
        CVector3D end = start + dir * checkDistance;

        CVector3D hitPos, hitNormal;
        if (Base* b = Base::FindObject(eField)) {
            if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
                avoidDir += hitNormal; // 壁の法線を回避方向に加算
            }
        }
    }

    if (avoidDir.Length() > 0.0f) {
        return avoidDir.GetNormalize(); // 正規化して回避方向を返す
    }

    return avoidDir; // 壁がない場合は (0, 0, 0)
}

void Enemy_Base::RenderVision()
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
        //Utility::DrawLine(m_pos, endPos, visionColor);
    }

    // プレイヤーが視界内の場合、赤い線で描画
    if (IsFoundPlayer())
    {
        CVector3D playerPos = mp_player->m_pos;
        //Utility::DrawLine(m_pos, playerPos, detectionColor);
    }
}
