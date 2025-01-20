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
        CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // �G�̖ڐ��̍���
        CVector3D end = player->m_pos + CVector3D(0, 1.0f, 0); // �v���C���[�̖ڐ��̍���

        // �f�o�b�O�p: ���C������
       // Utility::DrawLine(start, end, CVector4D(1.0f, 1.0f, 0.0f, 1.0f)); // ���F�����ŕ`��

        CVector3D hitPos, hitNormal;
        if (Base* b = Base::FindObject(eField)) {
            if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
                // �v���C���[�܂ł̋��������Փ˓_����O�Ȃ王�����Ղ��Ă���
                if ((hitPos - start).Length() < (end - start).Length()) {
                    //Utility::DrawSphere(hitPos, 0.2f, CVector4D(1.0f, 0.0f, 0.0f, 1.0f)); // �Ԃ����ŏՓ˓_��`��
                    return false;
                }
            }
        }

        // �Օ����Ȃ�
       // Utility::DrawLine(start, end, CVector4D(0.0f, 1.0f, 0.0f, 1.0f)); // �΂̐��ŕ`��
        return true;
    }

    return false;
}

bool Enemy_Base::CheckWallCollision(const CVector3D& dir)
{
    CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // �ڐ��̍���
    CVector3D end = start + dir * m_collisionRadius; // �Փ˔͈͂�L�΂����ʒu

    CVector3D hitPos, hitNormal;
    if (Base* b = Base::FindObject(eField)) {
        if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
            // �ǂ��Փ˔͈͓��ɂ���ꍇ
            //Utility::DrawSphere(hitPos, 0.2f, CVector4D(1.0f, 0.0f, 0.0f, 1.0f)); // �Փ˓_��Ԃ����ŕ`��
            return true;
        }
    }

    return false; // �ǂ��Ȃ�
}

bool Enemy_Base::IsWallNearby(float checkDistance)
{
    // �`�F�b�N����������X�g
    std::vector<CVector3D> directions = {
        {1, 0, 0},   // �O
        {-1, 0, 0},  // ��
        {0, 0, 1},   // �E
        {0, 0, -1},  // ��
        {0.707f, 0, 0.707f},   // �E�O
        {-0.707f, 0, 0.707f},  // ���O
        {0.707f, 0, -0.707f},  // �E��
        {-0.707f, 0, -0.707f}  // ����
    };

    for (const auto& dir : directions) {
        if (CheckWallCollision(dir * checkDistance)) {
            return true; // �ǂ���������
        }
    }

    return false; // �ǂ�������Ȃ�
}

CVector3D Enemy_Base::CalculateAvoidDirection(float checkDistance)
{
    CVector3D avoidDir(0, 0, 0);

    // �`�F�b�N����������X�g
    std::vector<CVector3D> directions = {
        {1, 0, 0},   // �O
        {-1, 0, 0},  // ��
        {0, 0, 1},   // �E
        {0, 0, -1},  // ��
        {0.707f, 0, 0.707f},   // �E�O
        {-0.707f, 0, 0.707f},  // ���O
        {0.707f, 0, -0.707f},  // �E��
        {-0.707f, 0, -0.707f}  // ����
    };

    for (const auto& dir : directions) {
        CVector3D start = m_pos + CVector3D(0, 1.0f, 0); // �ڐ��̍���
        CVector3D end = start + dir * checkDistance;

        CVector3D hitPos, hitNormal;
        if (Base* b = Base::FindObject(eField)) {
            if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end)) {
                avoidDir += hitNormal; // �ǂ̖@�����������ɉ��Z
            }
        }
    }

    if (avoidDir.Length() > 0.0f) {
        return avoidDir.GetNormalize(); // ���K�����ĉ�������Ԃ�
    }

    return avoidDir; // �ǂ��Ȃ��ꍇ�� (0, 0, 0)
}

void Enemy_Base::RenderVision()
{
    const float stepAngle = 5.0f; // �����`�悷��p�x�̊Ԋu
    const CVector4D visionColor(0, 1, 0, 0.5f); // �ΐF
    const CVector4D detectionColor(1, 0, 0, 0.5f); // �ԐF

    // ����͈͂�`��
    for (float angle = -m_viewAngle / 2.0f; angle <= m_viewAngle / 2.0f; angle += stepAngle)
    {
        float rad = DtoR(angle) + m_rot.y; // �G�̉�]�Ɋ�Â��Ċp�x�𒲐�
        CVector3D dir(sin(rad), 0, cos(rad));
        CVector3D endPos = m_pos + dir * m_viewLength; // ����͈͂̐�[
        //Utility::DrawLine(m_pos, endPos, visionColor);
    }

    // �v���C���[�����E���̏ꍇ�A�Ԃ����ŕ`��
    if (IsFoundPlayer())
    {
        CVector3D playerPos = mp_player->m_pos;
        //Utility::DrawLine(m_pos, playerPos, detectionColor);
    }
}
