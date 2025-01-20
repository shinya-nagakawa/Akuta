#include "NavEdit.h"
#include "../Game/Field.h"
#include "NavManager.h"

// コンストラクタ
NavEdit::NavEdit() : Base(eUI)
{
	NavManager::Instance()->SetRenderFlag(true);
}

// デストラクタ
NavEdit::~NavEdit()
{

}

void NavEdit::Update()
{

	Base* field = Base::FindObject(eField);
	CVector3D c, n;
	CVector2D mp = CInput::GetMousePoint();
	CVector3D start = Utility::ScreenToWorld(CVector3D(mp.x, mp.y, 0.0f));
	CVector3D end = Utility::ScreenToWorld(CVector3D(mp.x, mp.y, 0.98f));
	if (field && field->GetModel() && field->GetModel()->CollisionRay(&c, &n, start, end))
	{
		if (n.y > 0.7)
			m_pos = c;
		if (PUSH(CInput::eMouseL)) {
			if (!HOLD(CInput::eButton4)) {
				new NavNode(m_pos);
			}
			else {
				for (auto& n : NavManager::Instance()->m_nodes) {
					if ((n->GetPos() - m_pos).LengthSq() < 1.0) {
						NavManager::Instance()->RemoveNode(n);
						break;
					}
				}
			}
		}
	}
	ControllCamera();
	if (PUSH(CInput::eButton1)) {
		NavManager::Instance()->WriteNavNodes("NavNodes.txt");
	}

}
void NavEdit::Render()
{
	NavManager::Instance()->Render();
	Utility::DrawCube(m_pos, CVector3D(1, 1, 1), CVector4D(1, 0, 0, 1));
}
void NavEdit::Draw() {
	FONT_T()->Draw(0, 64, 1, 0, 0, "1キーで保存 左クリック：ノード追加　Shit＋左クリック：ノード削除");
	FONT_T()->Draw(0, 1000, 1, 0, 0, "%f %f %f", m_pos.x, m_pos.y, m_pos.z);

}

void NavEdit::ControllCamera() {

	//カメラの位置
	static CVector3D cam_pos(0, 0, 0);

	//カメラの注視点
	static CVector3D cam_at(0, 1, 0);

	//カメラの回転値
	static CVector3D cam_rot(DtoR(45), DtoR(180), 0);
	//カメラの距離
	static float cam_dist = 10;

	float speed = cam_dist / 1000;
	//回転行列
	CMatrix matrix_rot = CMatrix::MRotation(cam_rot);


	//マウス操作からカメラを回転
	//マウス移動量
	CVector2D mouse_vec = CInput::GetMouseVec();
	CVector3D key_dir(0, 0, 0);
	//方向キーから入力方向ベクトルを設定
	if (HOLD(CInput::eUp)) key_dir.z = 1;
	if (HOLD(CInput::eDown)) key_dir.z = -1;
	if (HOLD(CInput::eLeft)) key_dir.x = 1;
	if (HOLD(CInput::eRight)) key_dir.x = -1;
	key_dir.z += CInput::GetMouseWheel() * 5;

	//キー入力があれば
	if (key_dir.LengthSq() > 0) {
		float move_speed = 5.0f * CFPS::GetDeltaTime();
		CVector3D dir = CMatrix::MRotation(cam_rot) * key_dir;
		cam_at += dir * move_speed;
	}

	if (HOLD(CInput::eMouseR) && HOLD(CInput::eMouseL)) {

		cam_at += matrix_rot * CVector3D(0, 0, mouse_vec.y) * speed;

	}
	else if (HOLD(CInput::eMouseR)) {
		if (HOLD(CInput::eMouseC)) {
			//平行移動
			cam_at += matrix_rot * CVector3D(0, 0, mouse_vec.y) * speed;
		}
		else {
			//回転
			cam_rot.y += mouse_vec.x * -0.01f;
			cam_rot.x -= mouse_vec.y * -0.01f;
		}
		if (HOLD(CInput::eUp))
			//平行移動
			cam_at += matrix_rot * CVector3D(0, 0, 1) * speed;
		if (HOLD(CInput::eDown))
			//平行移動
			cam_at += matrix_rot * CVector3D(0, 0, -1) * speed;
		if (HOLD(CInput::eLeft))
			//平行移動
			cam_at += matrix_rot * CVector3D(1, 0, 0) * speed;
		if (HOLD(CInput::eRight))
			//平行移動
			cam_at += matrix_rot * CVector3D(-1, 0, 0) * speed;

		if (HOLD(CInput::eButton6))
			//平行移動
			cam_at += matrix_rot * CVector3D(0, 1, 0) * speed;
		if (HOLD(CInput::eButton7))
			//平行移動
			cam_at += matrix_rot * CVector3D(0, -1, 0) * speed;
	}
	else if (HOLD(CInput::eMouseC)) {
		//平行移動
		cam_at += matrix_rot * CVector3D(mouse_vec.x, mouse_vec.y, 0) * speed;
	}

	//カメラの位置を計算
	cam_pos = cam_at + matrix_rot.GetFront() * -cam_dist;

	//位置と注視点を設定してカメラを更新
	CCamera::GetCurrent()->LookAt(cam_pos, cam_at, matrix_rot.GetUp());

}