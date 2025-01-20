#include "NavNode.h"
#include "NavManager.h"
#include <GLLibrary/Utility.h>


// コンストラクタ
NavNode::NavNode(const CVector3D & pos, NodeType type)
	: m_no(0)
	, m_type(type)
	, m_cost(-1.0f)
	, m_nextNodeToGoal(nullptr)
	, m_nodeColor(0.25f, 1.0f, 0.25f, 1.0f)
{
	// 座標を設定
	SetPos(pos);
	// 経路探索管理クラスのノードリストに追加
	NavManager::Instance()->AddNode(this);
}

// デストラクタ
NavNode::~NavNode()
{
	// 経路探索管理クラスのノードリストから取り除く
	NavManager::Instance()->RemoveNode(this);
}

// ノードの状態をリセット
void NavNode::Reset()
{
	m_cost = -1.0f;
	m_nextNodeToGoal = nullptr;
}

// ノード番号を取得
int NavNode::GetNodeNo() const
{
	return m_no;
}

// ノードの座標を取得
const CVector3D& NavNode::GetPos() const
{
	return m_pos;
}

// ノードの座標を設定
void NavNode::SetPos(const CVector3D& pos, float distance)
{
	this->m_pos = pos;
	// 座標設定後に接続しているノードを自動でリストアップする
	NavManager::Instance()->FindConnectNavNodes(this, distance);
}

// 接続しているノードを追加
void NavNode::AddConnect(NavNode* node)
{
	// 既に接続しているノードであれば、スルー
	for (NavNode::ConnectData connect : m_connects)
	{
		if (connect.node == node) return;
	}

	// 接続するノードまでの距離をコストとする
	float cost = (node->m_pos - m_pos).Length();

	// 自身と相手の接続ノードリストに追加
	m_connects.push_back(ConnectData(node, cost));
	node->m_connects.push_back(ConnectData(this, cost));
}

// 接続しているノードをリストで追加
void NavNode::AddConnects(std::list<NavNode*> nodes)
{
	for (NavNode* node : nodes)
	{
		AddConnect(node);
	}
}

// 接続しているノードを取り除く
void NavNode::RemoveConnect(NavNode* node)
{
	std::list<ConnectData>::iterator it = m_connects.begin();
	while (it != m_connects.end())
	{
		if ((*it).node == node)
		{
			it = m_connects.erase(it);
		}
		else
		{
			it++;
		}
	}
}

// 接続ノードリストをクリア
void NavNode::ClearConnects()
{
	for (ConnectData connect : m_connects)
	{
		connect.node->RemoveConnect(this);
	}
	m_connects.clear();
}

// デバッグ用：描画するノードの色を設定
void NavNode::SetNodeColor(const CVector3D& color)
{
	m_nodeColor.r = color.r;
	m_nodeColor.g = color.g;
	m_nodeColor.b = color.b;
}

// デバッグ用：ノードの描画処理
void NavNode::Render()
{
	// 地面と重ならないように少し上に上げる値
	float offsetY = 0.1f;
	int nodeNo = m_no;
	// 接続先のノードまでのラインを描画
	for (NavNode::ConnectData& connect : m_connects)
	{
		// ノード番号が自身の方が大きい場合は、
		// 相手のノードの描画処理でラインを描画する
		if (nodeNo >= connect.node->GetNodeNo()) continue;

		// ルート上のノードかどうかで色を分ける
		CVector4D lineColor = CVector4D(0.125f, 0.25f, 1.0f, 1.0f);
		float lineWidth = 2.0f;
		// ラインを描画
		Utility::DrawLine
		(
			m_pos + CVector3D(0.0f, offsetY, 0.0f),
			connect.node->m_pos + CVector3D(0.0f, offsetY, 0.0f),
			lineColor,
			lineWidth
		);
	}

	// ノードの位置に球を描画
	Utility::DrawCube
	(
		m_pos + CVector3D(0.0f, offsetY, 0.0f),
		0.5f,
		m_nodeColor
	);
}
