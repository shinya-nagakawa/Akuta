#pragma once
#include "Base/Base.h"
#include "GreenDragon.h"


class PoisonBreath : public Base
{
public:
    PoisonBreath(const CVector3D& pos, CVector3D& dir, float speed, const CVector4D& color);
    void Update();  // �Ŗ��̊g����X�V
    void Render();  // �Ŗ���`��
    void Collision(Base* b);
    bool AttackOn;
    bool BreathGround;
    float breathCooldown;  // �G�t�F�N�g�𐶐�����Ԋu
    float breathTimer ;
    float count;

private:
    CVector3D m_pos;           // �Ŗ��̔��ˈʒu
    CVector3D m_rot_target;
    float m_poisonMistRadius;  // ���݂̓Ŗ��̔��a
    float m_maxRadius;         // �ő唼�a
    float m_Rate;        // �Ŗ����L���鑬��
    CVector4D m_poisonColor;   // �Ŗ��̐F
    float m_timeElapsed;       // �o�ߎ���
};