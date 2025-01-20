#include "PoisonBreath.h"
#include "GreenDragon.h"
#include "Game/Player.h"
#include "Effekseer/EffekseerEffect.h"
PoisonBreath::PoisonBreath(const CVector3D& pos, CVector3D& dir, float speed, const CVector4D& color):Base(ePoisonBreath)
{
    m_pos = pos;
    m_poisonColor = color;
	m_vec = dir * speed;
	m_rad = 0.2f;
    count = 2.0f;
	AttackOn = false;
	BreathGround = false;
}

void PoisonBreath::Update()
{
	m_lineS = m_pos;

	if (!BreathGround) {
		m_pos += m_vec;
		m_vec.y -= GRAVITY;
	}
	else {
		m_rad += 0.03f;
	}

	m_lineE = m_pos;

	count -= CFPS::GetDeltaTime();
	if (count <= 0.0f) {
		SetKill();
		return;
	}

	// �n�ʐڐG��ɍU���\��ԂɑJ��
	if (BreathGround && !AttackOn) {
		AttackOn = true;
	}
}

void PoisonBreath::Render()
{
	if (!BreathGround) {
		Utility::DrawSphere(m_pos, m_rad, m_poisonColor);
	}
}

void PoisonBreath::Collision(Base* b)
{

	switch (b->GetType())
	{

	case eField:
		 {

			//���f���Ƃ̔���(��)
			//�����߂���
			CVector3D v(0, 0, 0);
			//���ƃ��f���̏Փ�
			//�J�v�Z���ƃ��f���̏Փ�
			CVector3D c, m;
			if (b->GetModel()->CollisionRay(&c, &m, m_lineS, m_lineE))	//�n�_�i���j
			{
				Base::Add(new EffekseerEffect("DragonBreath", c, CVector3D(0, 0, 0), CVector3D(0.6, 0.6, 0.6)));
				
				AttackOn = true;
				BreathGround = true;
			}

			break;
		}
	case ePlayer:
		if (AttackOn == true)
		{
			if (Player* p = dynamic_cast<Player*>(b))
			{
				//�J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsuleShpere(p->m_lineS, p->m_lineE, p->m_rad, m_pos, m_rad))
				{
					Player* player = Player::Instance();
					if (player)
					{
						player->GetDamage(0.3);
					}
				}
			}
		}
	}
}

