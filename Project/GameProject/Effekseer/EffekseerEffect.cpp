#include "EffekseerEffect.h"

EffekseerEffect::EffekseerEffect(const std::string &name, const CVector3D& pos, const CVector3D& rot, const CVector3D& scale, CMatrix* parent) :
	Base(eEffect)
{

	//エフェクトを発生。ハンドルを取得する。
	m_handle = EffekseerManager::GetInstance()->GetManager()->Play(EffekseerManager::GetInstance()->GetEffect(name), pos.x, pos.y, pos.z);
	//位置、回転値、拡縮値、親を設定
	m_pos = pos;
	m_rot = rot;
	m_scale = scale;
	m_parent = parent;
	//行列を更新
	UpdateMatrix();
}
EffekseerEffect::~EffekseerEffect() {
	// エフェクトの停止
	if(EffekseerManager::isInstance()) EffekseerManager::GetInstance()->GetManager()->StopEffect(m_handle);
}
void EffekseerEffect::Update() {
	//再生終了チェック
	if (!EffekseerManager::isInstance() || !EffekseerManager::GetInstance()->GetManager()->Exists(m_handle)) {
		//タスク削除
		SetKill();
	}

}
void EffekseerEffect::Render() {
	//行列の更新
	UpdateMatrix();
	//描画はEffectManagerまかせ

}

void EffekseerEffect::UpdateMatrix()
{
	//モデル行列を作成
	m_matrix = CMatrix::MTranselate(m_pos) * CMatrix::MRotation(m_rot) * CMatrix::MScale(m_scale);

	//親がいる場合
	if (m_parent) {
		//子になるよう親の行列を掛ける
		m_matrix = *m_parent * m_matrix;
	}
	//Effekseer用行列にコピー
	Effekseer::Matrix43 mat;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 3; j++)
			mat.Value[i][j] = m_matrix.m[i][j];

	//モデル行列を反映
	EffekseerManager::GetInstance()->GetManager()->SetMatrix(m_handle, mat);
}
