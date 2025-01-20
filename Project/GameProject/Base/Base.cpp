#include "Base.h"
std::list<Base*> Base::m_list;
Base::Base(unsigned int type)
	:m_kill(0), m_type(type), m_pos(0, 0, 0), m_rot(0, 0, 0), m_hit_no(rand()), m_attack_no(rand()) {
}
Base::~Base() {

}
void Base::SetKill() 
{
	//�폜�t���OON
	if (!m_kill) m_kill = 2;
}
void Base::Update() {

}
void Base::Render() {

}
void Base::Draw() {

}
void Base::Collision(Base* b) {

}
void Base::KillALL() {
	//�S�Ă̍폜�t���O��ON�ɂ���
	for (auto& b : m_list) {
		b->SetKill();
	}
}
void Base::Kill(unsigned int mask)
{
	for (auto& b : m_list) {
		if ((1 << (int)b->m_type) & (mask))
			b->SetKill();
	}
}
void Base::ClearInstance()
{

	auto itr = m_list.begin();
	//�����܂ŌJ��Ԃ�
	while (itr != m_list.end()) {
		//�폜
		delete* itr;
		//���X�g���珜�O����
		//���̃I�u�W�F�N�g���󂯎��
		itr = m_list.erase(itr);
	}
}
void Base::CheckKillALL() {
	auto itr = m_list.begin();
	//�����܂ŌJ��Ԃ�
	while (itr != m_list.end()) {
		//�폜�`�F�b�N
		if ((*itr)->m_kill) {
			if (--(*itr)->m_kill == 0) {
				//�폜
				delete* itr;
				//���X�g���珜�O����
				//���̃I�u�W�F�N�g���󂯎��
				itr = m_list.erase(itr);
				continue;
			}
		}
		//���̃I�u�W�F�N�g
		itr++;
	}
}
void Base::UpdateALL() {
	for (auto& b : m_list) {
		b->Update();
	}
}
void Base::RenderALL() {
	for (auto& b : m_list) {
		b->Render();
	}
}
void Base::DrawALL() {
	for (auto& b : m_list) {
		b->Draw();
	}
}

void Base::CollisionALL() {
	auto itr = m_list.begin();
	//�����܂ŌJ��Ԃ�
	while (itr != m_list.end()) {
		if (!(*itr)->m_kill) {
			auto ct = itr;
			ct++;
			while (ct != m_list.end()) {
				if (!(*ct)->m_kill) {
					(*itr)->Collision(*ct);
					(*ct)->Collision(*itr);
				}
				//���̃I�u�W�F�N�g
				ct++;
			}
		}
		//���̃I�u�W�F�N�g
		itr++;
	}

}

void Base::Add(Base* b) {
	//
	auto itr = m_list.begin();
	//�����܂ŌJ��Ԃ�
	while (itr != m_list.end()) {
		if ((*itr)->GetType() > b->GetType()) {
			m_list.insert(itr, b);
			return;
		}
		//���̃I�u�W�F�N�g
		itr++;
	}
	m_list.push_back(b);

}



Base* Base::FindObject(unsigned int type) {

	for (auto& b : m_list) {
		if (b->GetType() == type && !b->m_kill) return b;
	}
	return nullptr;
}

std::vector<Base*> Base::FindObjects(unsigned int type)
{
	std::vector<Base*> list;

	for (auto& b : m_list) {
		if (b->GetType() == type && !b->m_kill) list.push_back(b);
	}
	return list;
}



