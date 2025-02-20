#pragma once
#define GRAVITY (9.8f*0.001)
//種類の列挙
enum {
	eCamera,
	eField,
	eEnemy,
	ePlayer,
	eWeapon,
	eSpiderBullet,
	ePoisonBreath,
	eItem,
	eSellpoint,
	ePlayer_Attack,
	eEnemy_Attack,
	eBullet,
	eEffect,
	eLight,
	eEffectManager,
	eUI,
	eBuypoint,
	eMoney,
	eScene,
	eTitle,
	eRedFilter,
};



class Base {
public:
	unsigned int m_type;		//オブジェクトの種類
	CVector3D	m_pos;		//座標
	CVector3D	m_rot;		//回転値
	CVector3D	m_vec;		//移動ベクトル
	CVector3D	m_scale;	//スケール
	CVector3D	m_dir;		//前方向
	int		m_kill;		//削除フラグ

	//球、カプセル用
	float		m_rad;		//半径
	//カプセル用
	CVector3D	m_lineS;	//線分始点
	CVector3D	m_lineE;	//線分終点

	CVector3D m_AttackS;
	CVector3D m_AttackE;
	//OBB用
	COBB m_obb;


	//攻撃の連続ヒット防止措置
	int m_hit_no;			//攻撃ヒット側番号
	int m_attack_no;		//攻撃側番号
private:
	//オブジェクトリスト
	static std::list<Base*> m_list;
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="type">オブジェクトの種類</param>
	Base(unsigned int type);
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Base();
	/// <summary>
	/// 削除フラグON
	/// </summary>
	void SetKill();
	/// <summary>
	/// オブジェクトの種類を取得
	/// </summary>
	/// <returns></returns>
	unsigned int GetType() {
		return m_type;
	}
	/// <summary>
	/// オブジェクトが持つモデルの取得
	/// </summary>
	/// <returns></returns>
	virtual CModel* GetModel() {
		return nullptr;
	}
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Render();
	/// <summary>
	/// 描画処理(2D)
	/// </summary>
	virtual void Draw();
	/// <summary>
	/// オブジェクト同士の当たり判定検証
	/// </summary>
	/// <param name="b">衝突対象オブジェクト</param>
	virtual void Collision(Base* b);

	/// <summary>
	/// 全てのオブジェクトの削除
	/// </summary>
	static void KillALL();

	/// <summary>
	/// 指定のオブジェクトの削除
	/// </summary>
	static void Kill(unsigned int mask);

	/// <summary>
	/// 全てのオブジェクトの削除（即破棄）
	/// </summary>
	static void ClearInstance();

	virtual CModel* GetNavModel() const {
		return nullptr;
	}


	/// <summary>
	/// 全てのオブジェクトの削除チェック
	/// </summary>
	static void CheckKillALL();
	/// <summary>
	/// 全てのオブジェクトの更新
	/// </summary>
	static void UpdateALL();
	/// <summary>
	/// 全てのオブジェクトの描画
	/// </summary>
	static void RenderALL();
	/// <summary>
	/// 全てのオブジェクトの描画(2D)
	/// </summary>
	static void DrawALL();
	/// <summary>
	/// 全てのオブジェクトの当たり判定
	/// </summary>
	static void CollisionALL();
	/// <summary>
	/// オブジェクトを追加
	/// </summary>
	/// <param name="b">追加オブジェクト</param>
	static void Add(Base* b);
	/// <summary>
	/// オブジェクトの検索
	/// </summary>
	/// <param name="type">検索対象の種類</param>
	/// <returns></returns>
	static Base* FindObject(unsigned int type);
	/// <summary>
	/// オブジェクトの検索(配列で取得)
	/// </summary>
	/// <param name="type">検索対象の種類</param>
	/// <returns></returns>
	static std::vector<Base*> FindObjects(unsigned int type);
	//フレンドクラス
	friend class Base;


};
