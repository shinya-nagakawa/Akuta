#pragma once
/*!
*	@brief	エフェクトクラス
*
*	エフェクトの発生を担当
*/
#include "../Base/Base.h"
#include "EffekseerManager.h"
//エフェクトクラス
class EffekseerEffect :public Base{
	Effekseer::Handle	m_handle;		//!発生したエフェクトのハンドル
	CMatrix *m_parent;					//!親
	CMatrix m_matrix;					//!モデル行列
public:
	/*!
	@brief	コンストラクタ
	@param	name		[in] 発生させるエフェクトの名前
	@param	pos			[in] 位置
	@param	rot			[in] 回転値
	@param	scale		[in] 拡縮値
	@param	parent		[in] 親
	@retval
	**/
	EffekseerEffect(const std::string& name,const CVector3D &pos, const CVector3D &rot, const CVector3D &scale,CMatrix* parent=nullptr);
	/*!
	@brief	デストラクタ
	**/
	~EffekseerEffect();
	/*!
	@brief	更新処理　削除チェックのみ実行
			更新自体はEffectManagerが行っている。
	@retval 無し
	**/
	void Update();
	/*!
	@brief	描画処理　行列の更新を実行
	描画自体はEffectManagerが行っている。
	@retval 無し
	**/
	void Render();

	/*!
	@brief	モデル行列を更新する
	@retval 無し
	**/
	void UpdateMatrix();
};
