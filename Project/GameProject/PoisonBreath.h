#pragma once
#include "Base/Base.h"
#include "GreenDragon.h"


class PoisonBreath : public Base
{
public:
    PoisonBreath(const CVector3D& pos, CVector3D& dir, float speed, const CVector4D& color);
    void Update();  // 毒霧の拡大を更新
    void Render();  // 毒霧を描画
    void Collision(Base* b);
    bool AttackOn;
    bool BreathGround;
    float breathCooldown;  // エフェクトを生成する間隔
    float breathTimer ;
    float count;

private:
    CVector3D m_pos;           // 毒霧の発射位置
    CVector3D m_rot_target;
    float m_poisonMistRadius;  // 現在の毒霧の半径
    float m_maxRadius;         // 最大半径
    float m_Rate;        // 毒霧が広がる速さ
    CVector4D m_poisonColor;   // 毒霧の色
    float m_timeElapsed;       // 経過時間
};