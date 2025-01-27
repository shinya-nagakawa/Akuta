#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"
#include "Camera.h"
#include "Effekseer/EffekseerEffect.h"
#include "NavNode.h"
#include "NavManager.h"
#include "Field.h"

#define MOVE_RANGE_MIN -50.0f
#define MOVE_RANGE_MAX 50.0f

Enemy::Enemy_RoleData EnemyRoleData[Enemy::eRole_Max] = {
	{1,1.0f,3.0f},
	{80,0.5f,8.0f},

};

Enemy::Enemy(const CVector3D& pos,int enemy_Hp,float weight,EnemyRole role ) :Enemy_Base(pos,90.0f,12.0f,0.6f), m_role(role)
{
	m_model = COPY_RESOURCE("Golem", CModelA3M);
	m_pos = pos;
	m_rad = 0.6f;
	m_hp = enemy_Hp;
	m_weight = weight;
	m_damage = 10;
	move_speed = 0.06f;
	AttackRange = 1.8f;
	FlankRange = EnemyRoleData[m_role].searchRangeMultiplier;
	SpinRange = 1.5f;
	m_state = eState_Idle;
	AttackInterval = 60.0f;
	AttackTimer = 30.0f;
	dashCooldownTimer = 0.0f;
	flankTimer = 0.0f;
	m_viewAngle = 90.0f;
	m_viewLength = 12.0f;
	m_searchLength = 500.0f;
	m_isAttacking = false;
	m_attackCount = 0;
	DirectionChange = 60;
	currentFrame = 0;
	m_forceDetectPlayer = false;

	mp_player = dynamic_cast<Player*>(Base::FindObject(ePlayer));

	m_idleProbability = 0.6f; // 60%�̊m���őҋ@
	m_stateChangeDuration = 2.0f; // 2�b���Ƃɏ�Ԃ�ύX

	wanderTime = 0.0f;
	wanderInterval = 5.0f;  // 5�b���Ƃɕ����]��
	wanderSpeed = 0.035f;   // �����_���ړ����̑��x

	// �G�̈ʒu�Ɍo�H�T���p�̃m�[�h���쐬
	m_navNode = new NavNode(m_pos, NavNode::NodeType::Destination);
	m_navNode->SetNodeColor(CVector3D(0.125f, 0.25f, 1.0f));

	m_item_id = 1;
}

Enemy::~Enemy()
{
	// SellCount ���J�E���g�A�b�v
	Field* field = dynamic_cast<Field*>(Base::FindObject(eField));
	if (field) {
		field->IncrementSellCount();
	}
}



void Enemy::SetRole(EnemyRole role)
{
	m_role = role;
}

//�ړ���
void Enemy::Idle()
{
	m_stateChangeTimer += 0.016f;

	if (IsFoundPlayer())
	{
		Min_speed = 0.04f;
		Max_speed = 0.06f;
		if (Min_speed <= Max_speed)
			Min_speed += 0.01f;

		//���X�g����v���C���[��T��
		if (Base* p = Base::FindObject(ePlayer))
		{
			count--;
			if (count <= 0)
			{
				UpdateAI();
			}
			//�^�[�Q�b�g�ւ̃x�N�g��
			CVector3D vec = p->m_pos - m_pos;
			//�x�N�g�������]�l���t�Z
			m_rot_target.y = atan2(vec.x, vec.z);
			//��]�l��������x�N�g�����v�Z
			CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
			//�^�[�Q�b�g�ւ̋�����1.5����艓�����
			if (vec.Length() > 1.5f)
			{
				//�O�i
				m_pos += dir * Min_speed;
				//�O�i�A�j���[�V����
				m_model.ChangeAnimation(1);
			}
		
			float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
			//�����ɂ����
			m_rot.y += a * 0.1f;
		}

	}
	else
	{
		//Move()
		Wander();
	}

	//�o�H�T������������܂ŃR�����g��
	/*
	else if (m_stateChangeTimer >= m_stateChangeDuration)
		
		{

			//�^�C�}�[���o�߂�����Ē��I
			if (static_cast<float>(rand()) / RAND_MAX < m_idleProbability) 
			{
				m_isWanderOn = false;
				m_model.ChangeAnimation(0);
			}
			else
			{
				m_isWanderOn = true;
			}
			m_stateChangeTimer = 0;
		}

	if (m_isWanderOn)
	{
		Wander();
	}*/
}

void Enemy::Move()
{
	//�����A�j���[�V�����ɕύX

	//�e�m�[�h�̍��W���擾
	NavNode* enemyNode = m_navNode;
	CVector3D moveNodePos = m_moveNode->GetPos();
	CVector3D enemyNodePos = enemyNode->GetPos();
	//�ړI�n�܂ł̃x�N�g�������߁A�������擾
	CVector3D vec = moveNodePos - enemyNodePos;
	float dist = vec.Length();

	NavManager* navMgr = NavManager::Instance();

	//���݈ʒu����ړI�n�̃m�[�h�܂ł̌o�H�T�����s��
	m_nextNode = navMgr->Navigate(enemyNode, m_moveNode);
	//���Ɉړ����ׂ��m�[�h�����݂���΁A
	if (m_nextNode != nullptr)
	{
		//���Ɉړ����ׂ��m�[�h�֌����Ĉړ�
		if (m_nextNode != nullptr)
		{
			//���Ɉړ����ׂ��m�[�h�֌����Ĉړ�
			if (MoveTo(m_nextNode->GetPos(), move_speed))
			{
				//�ړ����I���΁A�ҋ@��Ԃ֖߂�
				m_moveNode = nullptr;
				m_nextNode = nullptr;
				m_state = eState_Wander;
			}
		}
	}
}

//�v���C���[�փ_�b�V��
void Enemy::Dash()
{
	static const float dashDuration = 0.5f;  // �ːi���ԁi�b�j
	static const float maxDashSpeed = 0.18f; // �ő�ːi���x
	static const float dashAccel = 0.05f;    // �ːi�̉����x
	static const float dashCooldown = 0.5f; // �ːi��̃N�[���_�E�����ԁi�b�j

	// �^�[�Q�b�g��������Ȃ��ꍇ�A�ҋ@��Ԃɖ߂�
	if (!IsFoundPlayer()) {
		m_state = eState_Idle;
		return;
	}

	// �ːi���̏���
	if (dashTimer > 0.0f) {
		dashTimer -= CFPS::GetDeltaTime(); // 1�t���[�����̎��Ԃ����Z

		// �����x���l�����Č��݂̑��x���v�Z
		m_currentDashSpeed += dashAccel; // �����x�ő��x�𑝉�
		if (m_currentDashSpeed > maxDashSpeed) {
			m_currentDashSpeed = maxDashSpeed; // �ő呬�x�𒴂��Ȃ��悤�ɂ���
		}

		// �ːi�����Ɉړ�
		m_pos += m_dashDir * m_currentDashSpeed;

		// �ːi���I������
		if (dashTimer <= 0.0f) {
			dashCooldownTimer = dashCooldown; // �N�[���_�E���^�C�}�[���Z�b�g
			m_state = eState_Idle; // �ːi��͈ړ���Ԃɖ߂�
		}
		return; // �ːi���͂����ŏI��
	}

	// �N�[���_�E�����̏���
	if (dashCooldownTimer > 0.0f) {
		dashCooldownTimer -= 0.016f;

		// �v���C���[�Ƃ̋������v�Z���Ĉړ�������ǉ�
		if (Player* player = Player::Instance()) {
			CVector3D vecToPlayer = player->GetPos() - m_pos;
			float distance = vecToPlayer.Length();

			// �v���C���[�������ꍇ�͑ҋ@��Ԃɖ߂�
			if (distance > 1.5f) {
				m_state = eState_Idle;
			}
		}
		return; // �N�[���_�E�����͎��t���[����
	}

	// �ːi�J�n����
	if (Player* player = Player::Instance()) {
		CVector3D direction = player->GetPos() - m_pos;
		direction.y = 0.0f; // ���������𖳎�
		m_dashDir = direction.GetNormalize();

		// �G�̉�]���v���C���[�����ɍ��킹��
		m_rot.y = atan2(m_dashDir.x, m_dashDir.z);

		// �ːi���[�V�������Đ�
		m_model.ChangeAnimation(12); // �ːi�p�A�j���[�V�����ԍ�

		dashTimer = dashDuration; // �^�C�}�[��������
		m_currentDashSpeed = 0.0f; // �ːi���x��������
	}
}

//�v���C���[����̍U���������
void Enemy::Evade() {

	// �^�[�Q�b�g��������Ȃ��ꍇ�A�ҋ@��Ԃɖ߂�
	if (!IsFoundPlayer()) {
		m_state = eState_Idle;
		return;
	}

	static const float evadeDuration = 0.3f; // ������ԁi�b�j
	static const float evadeSpeed = 0.10f;   // ��𑬓x

	// ��������߂Ď��s�����ꍇ
	if (evadeTimer <= 0.0f) {
		// �^�C�}�[���Z�b�g
		evadeTimer = evadeDuration;

		// �����_���ō��E�̕��������߂�
		bool goLeft = (rand() % 2 == 0);
		m_evadeDir = goLeft ? CVector3D(-1.0f, 0, 0) : CVector3D(1.0f, 0, 0);

		// �G�̌����ɍ��킹�����������v�Z
		m_evadeDir = CVector3D(
			m_evadeDir.x * cos(m_rot.y) - m_evadeDir.z * sin(m_rot.y),
			0,
			m_evadeDir.x * sin(m_rot.y) + m_evadeDir.z * cos(m_rot.y)
		);

		// ������[�V�����̍Đ��i�C�Ӂj
		m_model.ChangeAnimation(11);
	}

	// �^�C�}�[��i�߂�
	evadeTimer -= 0.016f; // 1�t���[�����̎��Ԃ����Z

	// ��𓮍�𑱂���
	if (evadeTimer > 0.0f) {
		m_pos += m_evadeDir * evadeSpeed;
	}
	else {
		// ����I��
		evadeTimer = 0.0f;
		m_state = eState_Idle; // �ҋ@��Ԃɖ߂�
	}
}

bool Enemy::MoveTo(const CVector3D& target, float moveSpeed)
{
	CVector3D vec = target - m_pos;
	//�x�N�g�������]�l���t�Z
	m_rot.y = atan2(vec.x, vec.z);
	//��]�l��������x�N�g�����v�Z
	CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

	
	vec.y = 0.0f;
	dir = vec.GetNormalize();

	//�ړI�n�܂ł̋������ړ����x���傫���Ȃ��
	float dist = vec.Length();
	float speed = moveSpeed ;
	if (dist > speed)
	{
		CVector3D moveVec = dir * moveSpeed;
		m_model.ChangeAnimation(1); //�O�i
		m_pos += CVector3D(moveVec.x, m_vec.y, moveVec.z);
	}
	//�������ړ����x���Z���̂ł���΁A
	else
	{
		CVector3D moveVec = dir * dist;
		m_model.ChangeAnimation(1); //�O�i
		m_pos += CVector3D(moveVec.x, m_vec.y, moveVec.z) ;
		//�ړ��I��
		return true;
	}
	//�ړ��p��
	return false;
}

//�U����Ԃ̊Ǘ�
void Enemy::Attack()
{

	// �^�[�Q�b�g��������Ȃ��ꍇ�A�ҋ@��Ԃɖ߂�
	if (!IsFoundPlayer()) {
		m_state = eState_Idle;
		return;
	}
	
		if (m_isAttacking) {
			// ���݂̓G�̌����i��]�l�j�Ɋ�Â��Đ��ʕ������v�Z
			CVector3D forwardDir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));

			// ���f���̃{�[���ʒu���擾
			m_AttackS = m_model.GetFrameMatrix(29).GetPosition();
			m_AttackE = m_model.GetFrameMatrix(30).GetPosition();

			m_model.ChangeAnimation(22, false);

			if (m_model.isAnimationEnd())
			{
				m_state = eState_Idle;

				m_isAttacking = false;
			}

			// `m_AttackS` �������O���ɃI�t�Z�b�g
			float offsetDistance = 0.5f; // �O���ɐL�΂�����
			m_AttackE += forwardDir * offsetDistance;

			// �f�o�b�O�p�F�U���͈͂��J�v�Z���ŉ���
			//Utility::DrawCapsule(m_AttackS, m_AttackE, 0.5f, CVector4D(1, 0, 0, 1)); // �ԐF
		}
}

//�G�̎��S
void Enemy::Down()
{
		//m_isDead = true;

		if (m_stateItem != e_Equip) {
			m_model.ChangeAnimation(26, false);
		}
		else {
			m_model.ChangeAnimation(0, false);
		}
	
}

//�_���[�W���󂯂����̏���
void Enemy::Damage()
{

	if (m_mutekijikan <= 0)
	{
		m_hp -= 1;

		if (m_hp <= 0)
		{
			m_state = eState_Down;
			//SetKill();
			//printf("sibou");
		}
	}

	m_mutekijikan = 1.6f;
	
}

//�_���[�W���󂯂����̏���
void Enemy::Hit()
{

//�U����Ԃ������͉�荞�ݏ�ԂȂ�A���܂Ȃ�
	if (m_state == eState_Attack00||m_state == eState_Flank)return;

		m_isAttacking = false;
		m_state = eState_Hit;
		m_model.ChangeAnimation(15, false);
		m_model.SetAnimationSpeed(2.0f);
		if (m_model.isAnimationEnd())
		{
			m_model.SetAnimationSpeed(1.0f);
			m_state = eState_Idle;
		}
		if (m_hp <= 0)
		{
			m_model.SetAnimationSpeed(1.0f);
			m_state = eState_Down;
			//SetKill();
			//printf("sibou");
		}
	

}

//�w��Ɏ��肱�ޓ���
void Enemy::Flank()
{
	if (!Dashing)
	{
		static const float flankDuration = 2.0f; // ��荞�݂̍ő厞��
		static const float flankOffset = 5.0f;   // �v���C���[�̉������ɖc��ޗ�
		static const float flankDistance = 3.0f; // �v���C���[�̌���܂ł̋���

		static float directionChangeTimer = 0.0f; // �����ύX�p�^�C�}�[

		Player* player = Player::Instance();
		if (!player) return;

		// ��荞�݂̌o�ߎ��Ԃ��X�V
		flankTimer += CFPS::GetDeltaTime(); // 1�t���[���̎��ԁi60FPS�̏ꍇ�j
		directionChangeTimer += CFPS::GetDeltaTime(); // �����ύX�^�C�}�[��i�߂�

		// ���Ԃ��o�߂������荞�ݏI��
		if (flankTimer >= flankDuration) {
			m_state = eState_Idle; // ��荞�ݏI����ɑҋ@
			flankTimer = 0.0f;     // �^�C�}�[�����Z�b�g
			directionChangeTimer = 0.0f; // �����ύX�^�C�}�[�����Z�b�g
			return;
		}

		// �v���C���[�̐��ʕ����x�N�g�����v�Z
		CVector3D playerForward(sin(player->m_rot.y), 0, cos(player->m_rot.y));

		// �v���C���[�̉E�����x�N�g�����v�Z
		CVector3D playerRight(-playerForward.z, 0, playerForward.x);

		// ����I�ɕ�����ύX

		if (rand() % 100 < 30) { // 30%�̊m���ŕ�����ύX
			goLeft = !goLeft; // ���݂̕����𔽓]
		}
		directionChangeTimer = 0.0f; // �^�C�}�[�����Z�b�g

	// �^�[�Q�b�g�ʒu���v�Z
		float biasOffset = (goLeft ? -1.0f : 1.0f) * (rand() % 100 / 100.0f); // �����_���ɍ��E���𒲐�
		CVector3D offset = (goLeft ? -playerRight : playerRight) * (flankOffset + biasOffset);
		CVector3D targetPos = player->m_pos - playerForward * flankDistance + offset;

		// **VecToTarget�̌v�Z**
		CVector3D vecToTarget = targetPos - m_pos; // �ڕW�n�_�ւ̃x�N�g��
		float distance = vecToTarget.Length();    // �������v�Z

		// �^�[�Q�b�g�̕������v�Z���ĉ�]
		m_rot_target.y = atan2(vecToTarget.x, vecToTarget.z);
		float angleDiff = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
		m_rot.y += angleDiff * 0.1f;

		// �������\���łȂ��ꍇ�͈ړ�
		if (distance > 0.5f) {
			CVector3D dir(sin(m_rot.y), 0, cos(m_rot.y));

			// �Փ˔���
			if (CheckWallCollision(dir)) {
				m_state = eState_Idle; // �Փˎ��͒�~
				return;
			}

			// �ړ�����
			m_pos += dir * move_speed * 1.3f;
			m_model.ChangeAnimation(11); // �ړ��A�j���[�V����
		}
		else {
			// �U����Ԃֈڍs
			m_state = eState_Attack00;
		}

		// �f�o�b�O�p�F�ڕW�ʒu�̉���
		Utility::DrawSphere(targetPos, 0.5f, CVector4D(1, 0, 0, 1)); // �ڕW�ʒu��\��
	}
}

//�v���C���[�ɂ�����^����
void Enemy::GiveMoney(int Value)
{
	Player::Instance()->GainMoney(Value);
}

//�v���C���[��������
bool Enemy::IsFoundPlayer() const
{

	/*
	//�����������[�h���I���Ȃ疳�����Ńv���C���[�𔭌�
	if (m_forceDetectPlayer)
	{
		return true;
	}*/
	
	if (mp_player == nullptr) return false;

	CVector3D playerPos = mp_player->m_pos;
	CVector3D vec = playerPos - m_pos;
	vec.y = 0.0f;

	//����p�x��������
	CVector3D dir = vec.GetNormalize();
	float dot = CVector3D::Dot(m_dir, dir);
	float angle = DtoR(m_viewAngle);
	if (dot < cosf(angle))return false;

	//����͈͋���������
	float dist = vec.Length();
	if (dist > m_viewLength)return false;

	//�v���C���[�Ƃ̊ԂɎՕ������Ȃ����ǂ����`�F�b�N����
	if (!IsLookPlayer())
	{
		return false;
	}

	return true;
}



//������
void Enemy::Initialize()
{
	//���������Ƀ����_���ȕ����]����ݒ�
	wanderInterval = static_cast<float>(rand()) / RAND_MAX * 5.0f + 1.0f;
	wanderTime = wanderInterval;
	m_rot_target.y = DtoR(static_cast<float>(rand()) / RAND_MAX * 360.0f);
}

//�����_���ɓ���
void Enemy::Wander()
{
	wanderTime -= 0.016f;
	if (wanderTime <= 0) {
		bool directionFound = false;
		int maxAttempts = 10;
		int attempts = 0;

		while (!directionFound && attempts < maxAttempts) {
			float angle = static_cast<float>(rand()) / RAND_MAX * 360.0f;
			m_rot_target.y = DtoR(angle);
			CVector3D dir(CVector3D(sin(m_rot_target.y), 0, cos(m_rot_target.y)));

			if (!CheckWallCollision(dir)) {
				directionFound = true;
			}
			attempts++;
		}

		if (!directionFound) {
			m_rot_target.y = DtoR(0); // �f�t�H���g����
		}

		wanderInterval = static_cast<float>(rand()) / RAND_MAX * 5.0f + 1.0f;
		wanderTime = wanderInterval;
	}

	CVector3D dir(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
	if (!CheckWallCollision(dir)) {
		m_pos += dir * wanderSpeed;
		m_model.ChangeAnimation(1);
	}
	else {
		m_model.ChangeAnimation(0);
	}

	float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
	m_rot.y += a * 0.1f;
}







//�ǂ�����Ăق���
void Enemy::AvoidWalls()
{
	float CheckDistance = 2.0f;
	CVector3D avoidDir = CalculateAvoidDirection(CheckDistance);

	//�ǂ��߂��ꍇ�͉�𓮍���s��
	if (avoidDir.Length() > 0.0f)
	{
		CVector3D dir = avoidDir;

		//�Փ˂��Ȃ����ă`�F�b�N
		if (!CheckWallCollision(dir))
		{
			m_pos += dir * move_speed; // �������Ɉړ�
			m_model.ChangeAnimation(1); // �����A�j���[�V����
		}
		else {
			m_model.ChangeAnimation(0); // �ҋ@�A�j���[�V�����i����ł��Ȃ��ꍇ�j
		}
	}
}

//�������̃f�o�b�O�p
void Enemy::DebugAvoidance()
{
	float checkDistance = 2.0f;
	CVector3D avoidDir = CalculateAvoidDirection(checkDistance);

	if (avoidDir.Length() > 0.0f) {
		// ���������f�o�b�O���C���ŕ\��
		Utility::DrawLine(m_pos + CVector3D(0, 1, 0), m_pos + avoidDir * checkDistance, CVector4D(0, 1, 0, 1));
	}
}

//�v���C���[�̍U���񐔃J�E���^�[
void Enemy::IncreaseAttackCount()
{
	m_attackCount++;
}

//AI�̍X�V
void Enemy::UpdateAI()
{
	{
		Base* playerBase = Base::FindObject(ePlayer);
		if (!playerBase) {
			m_state = eState_Idle; // �v���C���[�����Ȃ��Ȃ�ҋ@
			return;
		}

		// �v���C���[�Ƃ̋����v�Z
		CVector3D Player_vec = playerBase->m_pos - m_pos;
		float distance_Player = Player_vec.Length();

		// **�D�揇��1: �v���C���[���U�����[�V���������������u�Ԃɉ��**
		if (Player::Instance()->GetAttackRelease() && distance_Player < AttackRange * 2.0f) {
			if (rand() % 100 < 25) { // 25%�̊m���ŉ��
				m_state = eState_Evade;
				return;
			}
		}

		// **�D�揇��2: �ːi**
		if (distance_Player > 4.0f && distance_Player < 8.0f) { // �ːi�ɓK��������
			if (rand() % 100 < 15) { // 15%�̊m���œːi
				m_state = eState_Dash; // �ːi��Ԃ�
				return;
			}
		}

		// **�D�揇��3: �U��**
		if (distance_Player < AttackRange && !m_isAttacking) {
			m_isAttacking = true;
			m_state = eState_Attack00; // �U�����[�h
			return;
		}

		
		// **�D�揇��4: �w���荞��**
		if (distance_Player < 5.0&& distance_Player <FlankRange &&!m_isAttacking) {
			if (rand() % 100 < EnemyRoleData[role].flankPersent) { 
				m_state = eState_Flank;
				return;
			}
		}

		// **�D�揇��5: �ړ����ċ߂Â�**
		if (distance_Player > 1.5f && !m_isAttacking) {
			m_state = eState_Move; // �ǂ�������
			return;
		}

		if (IsWallNearby(3.0f))
		{
			//�ǂ����m�����ꍇ�A�ǉ����Ɉړ���������𒲐�
			m_rot.y += DtoR(90);
		}

		// **�D�揇��6: �ҋ@**
		m_state = eState_Idle; // �߂�����A�������͉������Ȃ��Ƃ�
	}
}

//�X�V����
void Enemy::Update()
{

	// R�L�[�Ńv���C���[�����������[�h��؂�ւ�
	if (PUSH(CInput::eButton6)) { // �Ⴆ�� R �L�[
		m_forceDetectPlayer = !m_forceDetectPlayer; // ���[�h���g�O���i�I��/�I�t�؂�ւ��j
		m_model.ChangeAnimation(17,true);
		printf("�v���C���[�����������[�h: %s\n", m_forceDetectPlayer ? "�I��" : "�I�t");
	}

	// �t���[�����Ƃɉ����Ԃ��̗͂����Z�b�g
	m_pushBackForce = CVector3D(0, 0, 0);

	// ���̓G�Ƃ̏Փ˂��`�F�b�N
	for (auto& b : Base::FindObjects(eEnemy))
	{
		if (b != this) {
			Collision(b);
		}

	}
	
	// �����Ԃ��̗͂��ʒu�ɓK�p
    m_pos += m_pushBackForce;

	//AttackInterval -= 0.016f;
	m_evadeCooldown -= 0.016f;
	m_mutekijikan -= 0.016f;

	// �ǂ��߂��ꍇ�A���D��
	if (IsWallNearby(1.0f)) {
		AvoidWalls();
		return; // �ǉ�����͑��̏������X�L�b�v
	}

	//DebugAvoidance();

	switch (m_state)
	{
	case eState_Idle:
	case eState_Move:
		Idle();
		break;
	break;
	case eState_Attack00:
		Attack();
		break;
	case eState_Hit:
		Hit();
		break;
	case eState_Down:
		Down();
		break;
	case eState_Flank:
		Flank();
		break;
	case eState_Wander:
		//Wander();
		break;
	case eState_Dash:
		Dash();
		break;
	case eState_Evade:
		Evade();
		break;
	}
	
	
	//�A�j���[�V�����X�V
	m_model.UpdateAnimation();

	if (m_stateItem ==e_Drop)
	{
		//�d�͗���
		m_pos.y += m_vec.y;
		m_vec.y -= GRAVITY;
	}
}

//�`�揈��
void Enemy::Render()
{
	if (m_stateItem == e_Equip)
	{
		Player* player = Player::Instance();
		if (!player)return;
		//bool m_isAttacking = Player::Instance()->GetAttack();

		//m_rot.y = Camera::Instance()->cam_rot.y;

		//m_model.BindFrameMatrix(2,CMatrix::MRotation(Camera::Instance()->cam_rot));

		
		CMatrix player_bone = player->GetModel()->GetFrameMatrix(14);
		//�ʒu�����p
		CVector3D offset(0, -0.6f, -0.1f);
		m_pos = player_bone.GetPosition()+offset;
		m_rot = player->m_rot;
		

		m_model.SetScale(0.005f, 0.005f, 0.005f);

		enemy_matrix = player_bone
			* CMatrix::MTranselate(0, 15, 0)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);
		
		

		if (m_hp <= 0)
		{
			m_model.ChangeAnimation(0);
		}

	}
	
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	if(m_stateItem ==e_Drop)
	m_model.SetScale(0.01f, 0.01f, 0.01f);
	if(m_stateItem !=e_PickUp)
	m_model.Render();

	m_lineS = m_model.GetFrameMatrix(4).GetPosition();
	m_lineE = m_model.GetFrameMatrix(6).GetPosition();

	// ����̃f�o�b�O�\��
	//RenderVision();

	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 1, 0, 0.5));
	//if(m_isAttacking&&m_hp > 0)
	//Utility::DrawCapsule(m_AttackS, m_AttackE, m_rad, CVector4D(1, 0, 0, 0.5));
}

void Enemy::RenderVision()
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
		Utility::DrawLine(m_pos, endPos, visionColor);
	}

	// �v���C���[�����E���̏ꍇ�A�Ԃ����ŕ`��
	if (IsFoundPlayer())
	{
		CVector3D playerPos = mp_player->m_pos;
		Utility::DrawLine(m_pos, playerPos, detectionColor);
	}
}

//�I�u�W�F�N�g�Ƃ̏Փ˔���
void Enemy::Collision(Base* b)
{		
		
	if (m_stateItem != e_Drop)return;

	switch (b->GetType())
	{
	
	case eEnemy:
		if (Enemy* e = dynamic_cast<Enemy*>(b)) {
			int EnemyHp = e->GetHP();
			if (EnemyHp > 0) {
				// �J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsule(e->m_lineS, e->m_lineE, e->m_rad,
					m_lineS, m_lineE, m_rad))
				{
					// �����Ԃ��̌v�Z
					CVector3D pushBackDir = m_pos - e->GetPos(); // ���̓G���炱�̓G�ւ̕����x�N�g��
					pushBackDir.Normalize(); // ���K�����ĕ����x�N�g���ɂ���

					if (m_hp > 0)
					{
						// �����Ԃ��̗͂𗼕��̓G�ɓK�p����
						m_pushBackForce += pushBackDir * 0.05; // �����𒲐�
						e->m_pushBackForce -= pushBackDir * 0.05; // ���Ε���

					}
				}
			}
		}
		break;

	case eWeapon:
	
		//�E�F�|���Ƃ̓����蔻��̒ǉ����������B
		if (Weapon* d = dynamic_cast<Weapon*>(b))
		{
			CVector3D c1;
			float dist;

			if (Player::Instance()->GetAttack())
			{
				//�J�v�Z�����m�̏Փ�
				if (CCollision::CollisionCapsule(d->m_lineS, d->m_lineE, d->m_rad,
					m_lineS, m_lineE, m_rad,&dist,&c1))
				{

					if (m_mutekijikan <= 0&&m_hp >0)
					{
						//printf("hit");
						//m_state = eState_Hit;
						CVector3D pos = c1;
						//�؂�G�t�F�N�g
						//Base::Add(new CEffect("Slash", pos, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 0.2f, 0.4f, -0.1f, CEffect::eBlendAlpha, 1, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 10));
						Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
						Damage();
						Hit();
					}
				}
			}
		}

		break;

	case eField:
		//���f���Ƃ̔���(��)
		//�����߂���
		CVector3D v(0, 0, 0);
		//���ƃ��f���̏Փ�
		//�J�v�Z���ƃ��f���̏Փ�
		auto tri = b->GetModel()->CollisionCupsel(m_pos + CVector3D(0, 2.0 - m_rad, 0),	//�n�_�i���j
			m_pos + CVector3D(0, m_rad, 0),		//�I�_�i�����j
			m_rad);

		//�ڐG�����ʂ̐��J��Ԃ�
		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				//�ʂ����������V��ɓ�������
				//�㏸���x��0�ɖ߂�
				if (m_vec.y > 0)
					m_vec.y = 0;
			}
			else if (t.m_normal.y > 0.5f) {
				//�ʂ���������n�ʂɓ�������
				//�d�͗������x��0�ɖ߂�
				if (m_vec.y < 0)
					m_vec.y = 0;
			}
			float max_y = max(t.m_vertex[0].y, max(t.m_vertex[1].y, t.m_vertex[2].y));
			//�ڐG�����ʂ̕����ցA�߂荞�񂾕������߂�
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			//�ł��傫�Ȉړ��ʂ����߂�
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			//�G���܂ł͏��z����
			//�G��ȏ�̕ǂ̂݉����߂����
			if (max_y > m_pos.y + 0.2f) {
				v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
				v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
			}
		}
		//�����߂�
		m_pos += v;
	}
}


//������Ԃɂ��鏈��
void Enemy::Equip()
{
	m_stateItem = e_Equip;

	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() + m_weight);
	}
}

//�񑕔���Ԃɂ��鏈��
void Enemy::Unequip()
{
	m_stateItem = e_Drop;

	Player* player = Player::Instance();
	if (player)
	{
		player->SetWeight(player->GetWeight() - m_weight);
	}
}

int Enemy::GetHP()
{
	return m_hp;
}


bool Enemy::GetAttacking()
{
	return m_isAttacking;
}

int Enemy::GetAttackno()
{
	return m_attack_no;
}

int Enemy::GetDamage()
{
	return m_damage;
}


CVector3D Enemy::GetPos()
{
	return m_pos;
}
