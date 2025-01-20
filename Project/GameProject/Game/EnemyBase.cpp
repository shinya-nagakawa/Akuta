#include "EnemyBase.h"]
#include "Weapon.h"
//#include "Effekseer/EffekseerEffect.h"
#include "NavManager.h"

EnemyBase::EnemyBase(const CVector3D& position, int hp, float weight,float moveSpeed):Carry(eEnemy)
{
	moveSpeed1 = moveSpeed;
	m_viewAngle = 180.0f;
	m_viewLength = 22.0f;
	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));
	m_rad = 0.4f;
}

bool EnemyBase::IsFoundTarget()
{
	//��]�l��������x�N�g�����v�Z
	m_dir = (CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

	if (!mp_player) return false;
	CVector3D playerPos = mp_player->m_pos;

	// �G����v���C���[�܂ł̃x�N�g�������߂�
	CVector3D vec = playerPos - m_pos;


	// �G�̐��ʃx�N�g�� �� �G����v���C���[�܂ł̃x�N�g�� ��
	// ���ς�����Ċp�x�����߂�
	float dot = CVector3D::Dot(m_dir, vec.GetNormalize());
	// ���߂��p�x(����)������p�x�������f����
	if (dot < cos(DtoR(m_viewAngle))) return false;

	// �G����v���C���[�܂ł̋��������߂�
	float length = vec.Length();
	// ���߂����������싗���������f����
	if (length > m_viewLength) return false;

	// �Ō�Ɏ��g����v���C���[�܂ł̊Ԃ�
	// �Օ������Ȃ������肷��
	if (!IsLookPlayer()) return false;

	/*
	//����1 �T���͈͂̊p�x�����ǂ���
		//�G�̐��ʕ����̃x�N�g���ƖڕW���܂ł̃x�N�g������ςƂ��Ċp�x�����߂�

	float dot = CVector3D::Dot(m_dir, vec.GetNormalize());
	if (dot < cosf(m_searchLength+100))return false;
	//�����Q�@�T���͈͂̋����Ȃ����ǂ���
	float length = vec.Length();
	if (length > m_searchLength)return false;
	//2�̏����𖞂������̂ŁAtrue ��Ԃ�
	*/
	return true;

}

//�ǂƂ̏Փ˔���
bool EnemyBase::CheckWallCollision(const CVector3D& dir)
{
	CVector3D newPos = m_pos + CVector3D(0, 1, 0);
	CVector3D start = newPos;
	CVector3D end = start + dir * 1.0f;

	//Ray�̕\��
	Utility::DrawCapsule(start, end, m_rad, CVector4D(0, 1, 0, 0.5));

	CVector3D hitPos;
	CVector3D hitNormal;
	if (Base* b = Base::FindObject(eField))
	{
		if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			m_pos -= dir * 0.2f;
			//m_state = eState_Idle;
			return true;
		}
	}
	return false;
}


bool EnemyBase::IsLookPlayer() const
{
	CVector3D playerPos = mp_player->m_pos;
	CVector3D vec = playerPos - m_pos;
	// ���݈ʒu����v���C���[�܂ł̋��������߂�
	float dist = vec.Length();

	// �v���C���[�̈ʒu�܂ł̃��C�ƕǂƂ̏Փ˔�����s��
	CVector3D start = m_pos;
	CVector3D end = playerPos;
	start.y = 1.0f;
	end.y = 1.0f;
	CVector3D hitPos, hitNormal;
	if (Base* b = Base::FindObject(eField)) {
		if (b->GetModel()->CollisionRay(&hitPos, &hitNormal, start, end))
		{
			float hitDist = (hitPos - start).Length();
			if (dist > hitDist) return false;
		}
	}

	// �ǂƏՓ˂��Ă��Ȃ��������́A
	// �Փˈʒu���v���C���[��艜�̈ʒu�ł���Ȃ�΁A
	// �������ʂ��Ă���̂ŁA�v���C���[����������
	return true;
}


