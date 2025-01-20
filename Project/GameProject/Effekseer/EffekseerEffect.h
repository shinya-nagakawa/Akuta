#pragma once
/*!
*	@brief	�G�t�F�N�g�N���X
*
*	�G�t�F�N�g�̔�����S��
*/
#include "../Base/Base.h"
#include "EffekseerManager.h"
//�G�t�F�N�g�N���X
class EffekseerEffect :public Base{
	Effekseer::Handle	m_handle;		//!���������G�t�F�N�g�̃n���h��
	CMatrix *m_parent;					//!�e
	CMatrix m_matrix;					//!���f���s��
public:
	/*!
	@brief	�R���X�g���N�^
	@param	name		[in] ����������G�t�F�N�g�̖��O
	@param	pos			[in] �ʒu
	@param	rot			[in] ��]�l
	@param	scale		[in] �g�k�l
	@param	parent		[in] �e
	@retval
	**/
	EffekseerEffect(const std::string& name,const CVector3D &pos, const CVector3D &rot, const CVector3D &scale,CMatrix* parent=nullptr);
	/*!
	@brief	�f�X�g���N�^
	**/
	~EffekseerEffect();
	/*!
	@brief	�X�V�����@�폜�`�F�b�N�̂ݎ��s
			�X�V���̂�EffectManager���s���Ă���B
	@retval ����
	**/
	void Update();
	/*!
	@brief	�`�揈���@�s��̍X�V�����s
	�`�掩�̂�EffectManager���s���Ă���B
	@retval ����
	**/
	void Render();

	/*!
	@brief	���f���s����X�V����
	@retval ����
	**/
	void UpdateMatrix();
};
