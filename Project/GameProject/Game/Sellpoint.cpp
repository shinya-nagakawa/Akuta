#include "Sellpoint.h"
#include "Enemy.h"
#include "Spider.h"
#include "Player.h"
#include "GreenDragon.h"

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
		//���X�ɋ��z��ڕW���z�ɋ߂Â���
		if (m_displayedEarnings < m_totalEarnings)
		{
			float increment = (m_totalEarnings - m_displayedEarnings) * 0.1f;
			m_displayedEarnings += increment;
			//���ʉ����Đ�����
			if (!buyPlayed)
			{
				SOUND("���p��")->Play();
				buyPlayed = true;
			}

			//�덷������邽�߁A�ŏI�I�Ƀs�b�^�����킹��
			if (fabs(m_displayedEarnings - m_totalEarnings) < 0.01f)
			{
				m_displayedEarnings = m_totalEarnings;

				//���ʉ����Đ�����
				if (!soundPlayed)
				{
					SOUND("���p��")->Stop();
					SOUND("���p��")->Play();
					soundPlayed = true;
				}
			}
		}

		//�\�����Ԃ��Ǘ�
		m_elapsedDisplayTime += CFPS::GetDeltaTime();
		if (m_elapsedDisplayTime >= m_displayDuration)
		{
			m_showEarnings = false;  // �ʒm���\��
			m_elapsedDisplayTime = 0.0f;  // ���ԃ��Z�b�g
			m_totalEarnings = 0.0f;       // ���z���Z�b�g
			m_displayedEarnings = 0.0f;   // �\�����z���Z�b�g

			//���ʉ��̃t���O�����Z�b�g
			soundPlayed = false;
			buyPlayed = false;
		}
	}
}

void Sellpoint::Render()
{
	m_lineS = m_pos + CVector3D(0, 2.0f - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);
	//Utility::DrawOBB(m_obb, CVector4D(1, 1, 0, 0.5));

	Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));

	 if (m_showEarnings) 
	 {
        std::wstring totalText = L"�����p���z: " + std::to_wstring(static_cast<int>(m_displayedEarnings)) + L" G";
        m_renderer.DrawTextWindow(100, 100, 400, 100, totalText.c_str(),
            CVector4D(0.0f, 0.0f, 0.0f, 0.7f),  // �w�i�F
            CVector3D(1.0f, 1.0f, 1.0f),        // �����F
            10);                                // �p�f�B���O
    }
}

void Sellpoint::Collision(Base* b)
{
	switch (b->GetType())
	{
	case ePlayer:
		if (Player* player = dynamic_cast<Player*>(b))
		{
			if (!Base::FindObject(eEnemy))return;
			
				//�J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
					m_lineS, m_lineE, m_rad))
				{
					FONT_T()->Draw(1500, 750, 1.0f, 1.0f, 1.0f, "�C�₵���G�𗎂Ƃ�");
					FONT_T()->Draw(1500, 850, 1.0f, 1.0f, 1.0f, "�N���b�N�Ŕ��p");
					
					if (PUSH(CInput::eMouseL)) {
						m_isSellFrag = true;
					}
					else {
						m_isSellFrag = false;
					}
				}
		}
		break;
	case eEnemy:
		//�G�Ɣ��p�n�_
	
		if (Enemy* enemy = dynamic_cast<Enemy*>(b))
			//�J�v�Z�����m�̏Փ�
			if (CCollision::CollisionCapsule(enemy->m_lineS, enemy->m_lineE, enemy->m_rad,
				m_lineS, m_lineE, m_rad))
		{	
			//�G��HP��0�̎����p�ł���
			if (enemy->GetHP() <= 0&& m_isSellFrag&&enemy->m_stateItem !=e_Equip)
			{
				//	printf("baikyaku\n");
					enemy->SetKill();
					int earnings1 = 300;
					enemy->GiveMoney(earnings1);
					TriggerEarnings(300);
				//	printf("%s", enemy);
			}
		}if (Spider* spider = dynamic_cast<Spider*>(b))
			//�J�v�Z�����m�̏Փ�
			if (CCollision::CollisionCapsule(spider->m_lineS, spider->m_lineE, spider->m_rad,
				m_lineS, m_lineE, m_rad))
			{

				//�G��HP��0�̎����p�ł���
				if (spider->GetHP() <= 0 && m_isSellFrag &&spider->m_stateItem != e_Equip)
				{
					//printf("baikyaku\n");
					spider->SetKill();
					int earnings2 = 450;
					spider->GiveMoney(earnings2);
					TriggerEarnings(450);
					//printf("%s", spider);
				}
			}
		if (GreenDragon* dragon = dynamic_cast<GreenDragon*>(b))
			//�J�v�Z�����m�̏Փ�
			if (CCollision::CollisionCapsule(dragon->m_lineS, dragon->m_lineE, dragon->m_rad,
				m_lineS, m_lineE, m_rad))
			{

				//�G��HP��0�̎����p�ł���
				if (dragon->GetHP() <= 0 && m_isSellFrag && dragon->m_stateItem != e_Drop)
				{
					//printf("baikyaku\n");
					dragon->SetKill();
					int earnings3 = 1050;
					dragon->GiveMoney(earnings3);
					TriggerEarnings(1050);
					//printf("%s", spider);
				}
			}
	break;
	}
}

void Sellpoint::TriggerEarnings(int earnings)
{
	m_totalEarnings += earnings;   // ���z�����Z
	m_showEarnings = true;         // �ʒm��\��
	m_displayDuration = 3.0f;      // �\�����Ԃ�ݒ�
	m_elapsedDisplayTime = 0.0f;   // �\���o�ߎ��Ԃ����Z�b�g
}
