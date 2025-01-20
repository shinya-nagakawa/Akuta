#include "SpiderBullet.h"

SpiderBullet::SpiderBullet(Spider* parent,const CVector3D& pos, const float speed, const CVector3D& target) :Base(eSpiderBullet)
{
	gravity = 0.98f / 60 * 0.1;
	m_pos = pos + CVector3D(0, 0.2f, 0); // Y����0.2f���������グ��
	count = 3.5f;

	mp_parent = parent;
	m_targetPos = target;
	m_state = eState_Normal;

	m_isAttachedToPlayer = false;

	// �e���ɂ΂������������
	float speedVariance = 0.05f; // �e���̂΂���� (��: �}0.05)
	float randomSpeed = speed + ((rand() % 100) / 100.0f * 2.0f - 1.0f) * speedVariance;

	// �����_���ȏ����p�x��ݒ�
	float randomAngle = DtoR(5 + (rand() % 15)); // 5�`20�x�͈̔͂Ŋp�x�������_����

	// �ڕW�ւ̃x�N�g��
	CVector3D vec = target - pos;
	float y = atan2(vec.x, vec.z);

	// �ڕW�܂ł̋���
	float l = (CVector3D(target.x, 1.5f, target.z) - CVector3D(pos.x, 0, pos.z)).Length();
	
	//�����̕␳���q
	float a = (gravity * l * l) / (2.0 * randomSpeed * randomSpeed);
	//���ԂɊ�Â��W��
	float b = l / a;
	//�����̕␳��
	float c = (a + vec.y) / a;

	//���ː�
	float root = pow(-c + (b * b) / 4.0, 0.5);

	//�ˏo�p�x
	float ts = (b * b / 4) - c;
	if (ts < 0.0)
	{
		m_vec = CMatrix::MRotation(-DtoR(randomAngle), y, 0).GetFront() * randomSpeed;
		m_state = eState_Flying;
	}
	else
	{
		float s = atan((-b / 1.95) + root);
		m_vec = CMatrix::MRotation(s, y, 0).GetFront() * randomSpeed;
		m_state = eState_Flying;
	}

	m_lines.push_back(m_pos);
}

void SpiderBullet::Update()
{
	// �f�o�b�O�`��F���̃J�v�Z������͈͂�`��
	//Utility::DrawCapsule(m_pos, m_targetPos, 0.2f, CVector4D(1, 0, 0, 0.5f)); // �Ԃ��J�v�Z���ŕ`��

	// ��ɐe�̈ʒu���n�_�ɔ��f
	if (mp_parent)
	{
		CVector3D parentPos = mp_parent->GetPos() + CVector3D(0, 0.2f, 0); // �w偂̂��K�ʒu
		if (!m_lines.empty())
		{
			m_lines[0] = parentPos; // ���̎n�_���X�V
		}
	}

	// �v���C���[�ɂ������Ă���ꍇ
	if (m_isAttachedToPlayer&&m_state !=eState_Cut)
	{
		if (Player* player = Player::Instance())
		{
			// �v���C���[�̈ړ��ɍ��킹�ďՓ˓_���X�V
			m_attachmentPoint = player->m_pos + m_relativeAttachmentPoint;
			m_pos = m_attachmentPoint; // ���̏I�_���X�V

			// ��苗���ȏ㗣�ꂽ�ꍇ�A��������
			float distance = (m_attachmentPoint - m_lines[0]).Length();
			if (distance > 12.0f || mp_parent->GetHP() <= 0) // �����鋗��
			{
				SetKill(); // ��������
				return;
			}
		}

		// ���̍Đ���
		RegenerateLine(0.0f);
		return; // �������Ă���Ԃ͔�s�������X�L�b�v
	}

	// �Փ˔���
	if (CheckCollisionWithPlayer())
	{
		if (!m_isAttachedToPlayer) // ����̏Փˎ��̂ݏ���
		{
			if (Player* player = Player::Instance())
			{
				// �Փˎ��̑��Έʒu���v�Z
				m_relativeAttachmentPoint = m_attachmentPoint - player->m_pos;
			}
		}
	}

	if (m_state == eState_Cut)
	{
		m_disappearTimer += CFPS::GetDeltaTime();
		m_scale = 1.0f - (m_disappearTimer / 1.5f);

		//�X�P�[����0�Ȃ����
		if (m_scale <= 0.0f)
		{
			SetKill();
			return;
		}

		RegenerateLine(0.1f * m_scale);
		return;
	}

	if (m_state == eState_Flying)
	{
		m_vec.y += -gravity;  // �d�͓K�p
		m_pos += m_vec;       // �ړ�

		// ���̍Đ���
		RegenerateLine(0.15f);

		// �Փ˔���i�n�`�Ȃǁj
		Base* field = Base::FindObject(eField);
		if (field)
		{
			auto collisions = field->GetModel()->CollisionSphere(m_pos, 0.1f);
			if (!collisions.empty())
			{
				m_state = eState_Stuck;
				m_vec = CVector3D(0, 0, 0); // ���x���[���ɂ���
				return;
			}
		}

		// ��s���̏��Ń^�C�}�[
		count -= CFPS::GetDeltaTime();
		if (count <= 0)
		{
			SetKill(); // ��s���ł����Ԍo�߂ŏ�����
			return;
		}
	}
	else if (m_state == eState_Stuck)
	{
		// ��~��Ԃł��^�C�}�[�����炷
		count -= CFPS::GetDeltaTime();
		if (count <= 0)
		{
			SetKill(); // ��~�����莞�Ԃŏ�����
			return;
		}

		// ��~���ł����̍Đ������s��
		RegenerateLine(0.07);
	}

}

void SpiderBullet::Render() 
{
	//Utility::DrawSphere(m_pos, 0.2f, CVector4D(0, 0, 1, 1));
	//2�_�Ő�������
	if (m_state == eState_Cut && m_scale > 0.0f)
	{
		// �X�P�[�����l�����ĕ`��
		auto it = m_lines.begin();
		while (1)
		{
			auto it2 = it;
			it2++;
			if (it2 == m_lines.end()) break;

			// �����`��i�X�P�[����K�p�j
			Utility::DrawLine(*it, *it2, CVector4D(1, 1, 1, m_scale), 4 * m_scale);
			it++;
		}
	}
	else
	{
		// �ʏ�̎��`��
		auto it = m_lines.begin();
		while (1)
		{
			auto it2 = it;
			it2++;
			if (it2 == m_lines.end()) break;

			Utility::DrawLine(*it, *it2, CVector4D(1, 1, 1, 1), 4);
			it++;
		}
	}

	/*
	if (m_isAttachedToPlayer)
	{
		// �f�o�b�O�F�v���C���[�ɂ��������ꍇ�̓_��`��
		Utility::DrawSphere(m_attachmentPoint, 0.3f, CVector4D(0, 1, 0, 1));
	}
	if (m_state == eState_Stuck)
	{
		//�f�o�b�O�p
		Utility::DrawSphere(m_pos, 0.3f, CVector4D(1, 0, 0, 1));
	}*/

}

void SpiderBullet::Cut()
{
	if (m_state != eState_Cut)
	{
		m_state = eState_Cut; // ��Ԃ�ؒf���ɐݒ�
		m_disappearTimer = 0.0f; // ������܂ł̎��Ԃ�������
		m_scale = 1.0f; // �k�������̏����l
		m_isAttachedToPlayer = false;

		// ���̏��ŃA�j���[�V������������
		m_lines.clear();
		m_lines.push_back(m_pos); // �J�n�ʒu�����c��
	}
}
;

void SpiderBullet::RegenerateLine(float noiseStrength)
{
	if (m_lines.empty()) return;

	// ���݂̎n�_�ƏI�_���擾
	CVector3D start = m_lines[0]; // �n�_
	CVector3D end = m_pos;        // �I�_

	// �V�������𐶐�
	m_lines.clear();
	const int segments = 20; // �����̐�
	for (int i = 0; i <= segments; ++i)
	{
		float t = static_cast<float>(i) / segments;

		// ���`���
		CVector3D point = start * (1.0f - t) + end * t;

		if (i > 0 && i < segments)
		{
			// �m�C�Y��������
			float noiseX = ((rand() % 100) / 500.0f) * (2 * noiseStrength) - noiseStrength; // -0.1 �` +0.1
			float noiseY = ((rand() % 100) / 500.0f) * (2 * noiseStrength) - noiseStrength; // -0.1 �` +0.1
			float noiseZ = ((rand() % 100) / 500.0f) * (2 * noiseStrength) - noiseStrength; // -0.1 �` +0.1

			point += CVector3D(noiseX, noiseY, noiseZ);
		}

		/*
			t �̌v�Z
			�e�������S�̂̂ǂ��Ɉʒu���邩�������Ōv�Z�B

			point �̌v�Z
			���`��ԂŎn�_����I�_�܂ł̈ʒu���Z�o�B

			����
			m_lines �ɕ�Ԃ��ꂽ�_�����ԂɊi�[�����̂ŁA���炩�Ȏ����`��ł���B
		*/


		m_lines.push_back(point);
	}

}

bool SpiderBullet::CheckCollisionWithPlayer()
{
	const int numSegments = 10; // ���𕪊�����Z�O�����g��
	for (int i = 0; i < numSegments; ++i)
	{
		// �e�Z�O�����g�̎n�_�ƏI�_���v�Z
		float t = static_cast<float>(i) / numSegments;
		float nextT = static_cast<float>(i + 1) / numSegments;

		CVector3D segmentStart = m_lines[0] + (m_lines.back() - m_lines[0]) * t;
		CVector3D segmentEnd = m_lines[0] + (m_lines.back() - m_lines[0]) * nextT;

		// �v���C���[�Ƃ̃R���W��������
		if (Player* player = Player::Instance())
		{
			CVector3D collisionPoint; // �Փ˓_
			if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
				segmentStart, segmentEnd, 0.1f, nullptr, &collisionPoint))
			{
				m_isAttachedToPlayer = true;

				// �����_���Ȑڑ��_�𐶐�
				float randomHeight = (rand() % 100) / 100.0f; // 0.0 �` 1.0 �̃����_���l
				float randomOffset = ((rand() % 200) - 100) / 100.0f; // -1.0 �` 1.0 �̃����_���l

				// �v���C���[�̃R���C�_�[�͈͓��Ń����_���Ȉʒu���v�Z
				CVector3D basePoint = player->m_lineS * (1.0f - randomHeight) + player->m_lineE * randomHeight;
				m_attachmentPoint = basePoint + CVector3D(randomOffset * player->m_rad, 0.0f, randomOffset * player->m_rad);

				// �Փˎ��̑��Έʒu���v�Z
				m_relativeAttachmentPoint = m_attachmentPoint - player->m_pos;

				player->GetDamage(0.1f); // �_���[�W��^����
				return true;
			}
		}
	}
	return false; // �Փ˂Ȃ�
}

