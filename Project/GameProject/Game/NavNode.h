#pragma once
#include <list>
#include <GLLibrary/CVector.h>
class NavManager;
// 探すノードの距離
#define FIND_NODE_DISTANCE 50.0f

// 経路探索用のノードクラス
class NavNode
{
	friend NavManager;

public:
	// ノードの種類
	enum class NodeType
	{
		// 経路探索時に通過ノードとして使用できるノード
		Navigation,

		// 目的地としてのみ使用できるノード
		Destination,
	};

	// 接続しているノードの情報
	class ConnectData
	{
	public:
		NavNode* node;	// 接続ノード
		float cost;		// 移動に掛かるコスト
		ConnectData(NavNode* node, float cost) : node(node), cost(cost) {}
	};

private:
	int m_no;			// ノード番号
	NodeType m_type;	// ノードの種類
	CVector3D m_pos;	// ノードの座標
	// 接続しているノード情報リスト
	std::list<ConnectData> m_connects;

	float m_cost;		// 移動コスト記憶用
	// 目的地まで移動するために、
	// 次に移動すべきノード
	NavNode* m_nextNodeToGoal;

	// デバッグ用：ノードの色
	CVector4D m_nodeColor;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">ノードの座標</param>
	/// <param name="type">ノードの種類</param>
	NavNode(const CVector3D& pos, NodeType type = NodeType::Navigation);
	// デストラクタ
	~NavNode();
	// ノードの状態をリセット
	void Reset();

	// ノード番号を取得
	int GetNodeNo() const;

	// ノードの座標を取得
	const CVector3D& GetPos() const;
	// ノードの座標を設定
	void SetPos(const CVector3D& pos, float distance = FIND_NODE_DISTANCE);

	// 接続しているノードを追加
	void AddConnect(NavNode* node);
	// 接続しているノードをリストで追加
	void AddConnects(std::list<NavNode*> nodes);
	// 接続しているノードを取り除く
	void RemoveConnect(NavNode* node);
	// 接続ノードリストをクリア
	void ClearConnects();

	// デバッグ用：描画するノードの色を設定
	void SetNodeColor(const CVector3D& color);
	// デバッグ用：ノードの描画処理
	void Render();
};