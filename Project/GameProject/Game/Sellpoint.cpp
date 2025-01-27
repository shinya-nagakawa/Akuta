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
	
	 if (m_showEarnings) 
	 {
        std::wstring totalText = L"�����p���z: " + std::to_wstring(static_cast<int>(m_displayedEarnings)) + L" G";
        m_renderer.DrawTextWindow(100, 100, 400, 100, totalText.c_str(),
            CVector4D(0.0f, 0.0f, 0.0f, 0.7f),  // �w�i�F
            CVector3D(1.0f, 1.0f, 1.0f),        // �����F
            10);                                // �p�f�B���O
    }

     // �J�v�Z����`��
     m_lineS = m_pos + CVector3D(0, 2.0f - m_rad, 0);
     m_lineE = m_pos + CVector3D(0, m_rad, 0);
     Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));

     /*
     // �J�v�Z���̐^��̈ʒu�i���[���h���W�j
     CVector3D textWorldPos = m_pos + CVector3D(0, 2.5f, 0); // �����𒲐�

     // �J�����̃C���X�^���X���擾
     Camera* camera = Camera::Instance();
     if (!camera) return; // �J���������݂��Ȃ��ꍇ�͉������Ȃ�

     // �J�����̍s����擾
     CMatrix viewMatrix = camera->GetViewMatrix();
     CMatrix projectionMatrix = camera->GetProjectionMatrix();

     // ���[���h���W���X�N���[�����W�ɕϊ�
     CVector4D worldPos4(textWorldPos.x, textWorldPos.y, textWorldPos.z, 1.0f);
     CVector4D screenPos = projectionMatrix * (viewMatrix * worldPos4);

     // �X�N���[�����W�𐳋K��
     if (screenPos.w <= 0.0f) return; // �w�ʂɂ���ꍇ�͕\�����Ȃ�
     screenPos.x /= screenPos.w;
     screenPos.y /= screenPos.w;
     screenPos.z /= screenPos.w;

     // �X�N���[�����W���s�N�Z���P�ʂɕϊ�
     int screenX = static_cast<int>((screenPos.x * 0.5f + 0.5f) * SCREEN_WIDTH);
     int screenY = static_cast<int>((-screenPos.y * 0.5f + 0.5f) * SCREEN_HEIGHT);

     // === �C��: �J�����̏㉺�ړ��ŕ�������΂Ȃ��悤�ɂ��� ===
     // �J�v�Z���̈ʒu����ʊO�Ȃ�\�����Ȃ�
     if (screenX < 0 || screenX > SCREEN_WIDTH || screenY < 0 || screenY > SCREEN_HEIGHT) return;

     // �e�L�X�g��`��
     const char* message = isInside ? "���p�\" : "���p�s��";
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

            // �v���C���[���J�v�Z�����ɂ���ꍇ
            if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
                m_lineS, m_lineE, m_rad))
            {
                FONT_T()->Draw(1500, 750, 1.0f, 1.0f, 1.0f, "�C�₵���G�𗎂Ƃ�");
                FONT_T()->Draw(1500, 850, 1.0f, 1.0f, 1.0f, "�N���b�N�Ŕ��p");

                // �}�E�X�N���b�N���ɔ��p�t���O�𗧂Ă�
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
        // �G�iEnemy, Spider, GreenDragon�j���J�v�Z�����ɂ��邩�`�F�b�N
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

    // �J�v�Z�����ɓG������ꍇ�̃��b�Z�[�W�\��
    if (isInside)
    {
        FONT_T()->Draw(1500, 900, 1.0f, 1.0f, 1.0f, "�G�����p�\�Ȉʒu�ɂ��܂�");
    }
}

// �J�v�Z�����ɃI�u�W�F�N�g�����邩�𔻒肷�鋤�ʊ֐�
bool Sellpoint::IsInsideCapsule(Base* b)
{
    return CCollision::CollisionCapsule(b->m_lineS, b->m_lineE, b->m_rad, m_lineS, m_lineE, m_rad);
}

void Sellpoint::TriggerEarnings(int earnings)
{
	m_totalEarnings += earnings;   // ���z�����Z
	m_showEarnings = true;         // �ʒm��\��
	m_displayDuration = 3.0f;      // �\�����Ԃ�ݒ�
	m_elapsedDisplayTime = 0.0f;   // �\���o�ߎ��Ԃ����Z�b�g
}
