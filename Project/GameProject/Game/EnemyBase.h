#pragma once
#include "../Base/Base.h"
#include "Game/Player.h"
#include "Game/NavNode.h"
#include "Carry.h"

//敵のベースクラス

class EnemyBase : public Carry
{
    protected:
        int m_hp;             // 体力
        float m_weight;       // 重さ
        float moveSpeed1;
        float m_viewAngle;		// 視野角度
        float m_viewLength;		// 視野距離

    public:
        EnemyBase(const CVector3D& pos, int hp, float weight,float moveSpeed);
        virtual ~EnemyBase() {}


       bool IsFoundTarget();
       bool CheckWallCollision(const CVector3D& dir);
       bool IsLookPlayer() const;

      // int GetHP();

       bool GetAttacking();

       int GetAttackno();

       int GetDamage();

       CVector3D GetPos();

        

        int GetHP() const { return m_hp; }
        CVector3D GetPosition() const { return m_pos; }

        Player* mp_player;		// プレイヤー
        Carry* m_carry;
};

