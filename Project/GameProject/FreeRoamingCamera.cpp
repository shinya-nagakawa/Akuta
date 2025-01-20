#include "FreeRoamingCamera.h"

FreeRoamingCamera::FreeRoamingCamera()
    : Base(eCamera),m_moveSpeed(0.05f), m_rotSpeed(0.002f) 
{
    Speed = 2.0f;
}


void FreeRoamingCamera::Update()
{
    // 入力に基づいてカメラを移動
    CVector3D moveDir(0.0f, 0.0f, 0.0f);
    if (HOLD(CInput::eUp)) moveDir.z += Speed;
    if (HOLD(CInput::eDown)) moveDir.z -= Speed;
    if (HOLD(CInput::eLeft)) moveDir.x += Speed;
    if (HOLD(CInput::eRight)) moveDir.x -= Speed;
    if (HOLD(CInput::eButton2)) moveDir.x += 1.0f;   
    if (HOLD(CInput::eButton7)) moveDir.x -= 1.0f;   

    if (HOLD(CInput::eButton4))
    {
        Speed = 0.0f;
    }
    else
    {
        Speed = 2.0f;
    }

    // カメラ移動処理
    CVector3D forward = CMatrix::MRotation(m_rot).GetFront();
    CVector3D right = CMatrix::MRotation(m_rot).GetLeft();
    m_pos += (forward * moveDir.z + right * moveDir.x + CVector3D(0, moveDir.y, 0)) * m_moveSpeed;

    // マウス入力でカメラ回転
    CVector2D mouseDelta = CInput::GetMouseVec();
    m_rot.x += mouseDelta.y * m_rotSpeed;  // 上下回転
    m_rot.y -= mouseDelta.x * m_rotSpeed;  // 左右回転

    // 回転制限
    m_rot.x = min(DtoR(90), max(DtoR(-90), m_rot.x));

    // カメラの行列設定
    CMatrix cam_matrix = CMatrix::MTranselate(m_pos) * CMatrix::MRotation(m_rot);
    CCamera::GetCurrent()->SetTranseRot(cam_matrix);
}
