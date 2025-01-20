#pragma once
#include "../Base/Base.h"
#include "Game/Player.h"
#include "Game/NavNode.h"
#include "Carry.h"

//�G�̃x�[�X�N���X

class EnemyBase : public Carry
{
    protected:
        int m_hp;             // �̗�
        float m_weight;       // �d��
        float moveSpeed1;
        float m_viewAngle;		// ����p�x
        float m_viewLength;		// ���싗��

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

        Player* mp_player;		// �v���C���[
        Carry* m_carry;
};

