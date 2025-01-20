#include "FreeRoamingCamera.h"

FreeRoamingCamera::FreeRoamingCamera()
    : Base(eCamera),m_moveSpeed(0.05f), m_rotSpeed(0.002f) 
{
    Speed = 2.0f;
}


void FreeRoamingCamera::Update()
{
    // ���͂Ɋ�Â��ăJ�������ړ�
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

    // �J�����ړ�����
    CVector3D forward = CMatrix::MRotation(m_rot).GetFront();
    CVector3D right = CMatrix::MRotation(m_rot).GetLeft();
    m_pos += (forward * moveDir.z + right * moveDir.x + CVector3D(0, moveDir.y, 0)) * m_moveSpeed;

    // �}�E�X���͂ŃJ������]
    CVector2D mouseDelta = CInput::GetMouseVec();
    m_rot.x += mouseDelta.y * m_rotSpeed;  // �㉺��]
    m_rot.y -= mouseDelta.x * m_rotSpeed;  // ���E��]

    // ��]����
    m_rot.x = min(DtoR(90), max(DtoR(-90), m_rot.x));

    // �J�����̍s��ݒ�
    CMatrix cam_matrix = CMatrix::MTranselate(m_pos) * CMatrix::MRotation(m_rot);
    CCamera::GetCurrent()->SetTranseRot(cam_matrix);
}
