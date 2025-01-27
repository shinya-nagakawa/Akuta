#include "Camera.h"
#include "Player.h"
#include "UI.h"
#include "FreeRoamingCamera.h"

Camera* Camera::ms_instance = nullptr;

Camera::Camera() :Base(eCamera), m_dist(4.0f)
{
	ms_instance = this;
	m_shakeIntensity = 0.0f;
	m_shakeDuration = 0.0f;
}

CVector3D Camera::GetPos()
{
	return m_pos;
}

CVector3D Camera::GetRotation()
{
    return m_rot;
}

float Camera::GetDistance()
{
	return m_dist;
}

Camera* Camera::Instance()
{
	return ms_instance;
}

CMatrix Camera::GetViewMatrix() const
{
    // �J�����̉�]�𔽉f
    CMatrix rotationMatrix = CMatrix::MRotation(m_rot);
    // �J�����̈ʒu�𔽉f
    CMatrix translationMatrix = CMatrix::MTranselate(-m_pos);
    // ��]�ƈʒu���|�����킹�ăr���[�s����쐬
    return rotationMatrix * translationMatrix;
}

CMatrix Camera::GetProjectionMatrix() const
{
    CMatrix projectionMatrix;
    float fov = DtoR(45.0f);  // ����p�i45�x�j
    float aspect = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT; // �A�X�y�N�g��
    float nearPlane = 0.1f;   // �j�A�N���b�v
    float farPlane = 100.0f;  // �t�@�[�N���b�v

    projectionMatrix.Perspective(fov, aspect, nearPlane, farPlane);
    return projectionMatrix;
}

void Camera::Shake(float intensity, float duration) {
	m_shakeIntensity = intensity;
	m_shakeDuration = duration;
	m_shakeTimer = 0.0f;
}




void Camera::SetFixedCamera(const CVector3D& position, const CVector3D& rotation)
{
    m_fixedPosition = position;
    m_fixedRotation = rotation;
    m_mode = eFixed; // �Œ�J�������[�h�ɂ���
}

void Camera::SetMode(CameraMode mode)
{
    m_mode = mode;

    if (m_mode == eFreeRoam) {
        // �t���[���[�~���O�J�����ɏ����ʒu�Ɖ�]���Z�b�g
        m_freeCam.SetPosition(m_pos);
        m_freeCam.SetRotation(m_rot);

        // **���݂̃J�����s����t���[���[�~���O�J�����ɐ؂�ւ�**
        CMatrix cam_matrix = CMatrix::MTranselate(m_freeCam.GetPosition()) * CMatrix::MRotation(m_freeCam.GetRotation());
        CCamera::GetCurrent()->SetTranseRot(cam_matrix);
    }
}

void Camera::UpdatePlayerCamera()
{ 
    // �J�����̉�]
    float cam_speed = 0.001f;
    CVector2D mouse_vec = CInput::GetMouseVec();
    m_rot += CVector3D(mouse_vec.y, -mouse_vec.x, 0) * cam_speed;
    m_rot.x = min(DtoR(60), max(DtoR(-45), m_rot.x));
    m_rot.y = Utility::NormalizeAngle(m_rot.y);
    m_dist = min(2.0f, max(0.0f, m_dist + CInput::GetMouseWheel()));

    if (Player::Instance()->GetHp() <= 0) {
        m_dist = 2.0f;
    }
}

void Camera::Update() {
    // **�Œ�J�������[�h�̏ꍇ**
    if (m_mode == eFixed) {
        CMatrix cam_matrix = CMatrix::MTranselate(m_fixedPosition) * CMatrix::MRotation(m_fixedRotation);
        CCamera::GetCurrent()->SetTranseRot(cam_matrix);
    }

    // **�t���[���[�~���O���[�h�̏ꍇ**
    if (m_mode == eFreeRoam&&!HOLD(CInput::eMouseR)) {
        m_freeCam.Update();

        // F�L�[�ŃJ�������Œ�
        if (PUSH(CInput::eButton6)) {
            SetFixedCamera(m_freeCam.GetPosition(), m_freeCam.GetRotation());
        }
      
    }

    // **�v���C���[�J�������[�h�̏ꍇ**
    UpdatePlayerCamera();

    if (HOLD(CInput::eMouseR))
    {
        UpdatePlayerCamera();
    }
}

void Camera::Render() {
    if (CShadow::isDoShadow()) return;

    // **�Œ�J�������[�h**
    if (m_mode == eFixed) {
        CMatrix cam_matrix = CMatrix::MTranselate(m_fixedPosition) * CMatrix::MRotation(m_fixedRotation);
        CCamera::GetCurrent()->SetTranseRot(cam_matrix);
    }

    // **�t���[���[�~���O�J�������[�h�̏ꍇ**
    if (m_mode == eFreeRoam) {
        CMatrix cam_matrix = CMatrix::MTranselate(m_freeCam.GetPosition()) * CMatrix::MRotation(m_freeCam.GetRotation());
        CCamera::GetCurrent()->SetTranseRot(cam_matrix);
        return;  // **�����ŏ������I����**
    }

    // �ʏ�J�����̗h�ꏈ��
    CVector3D shakeOffset(0.0f, 0.0f, 0.0f);

    if (m_shakeDuration > 0.0f) {
        m_shakeTimer += CFPS::GetDeltaTime();
        if (m_shakeTimer < m_shakeDuration) {
            float offsetX = Utility::Rand(-m_shakeIntensity, m_shakeIntensity);
            float offsetY = Utility::Rand(-m_shakeIntensity, m_shakeIntensity);
            shakeOffset = CVector3D(offsetX, offsetY, 0.0f);
        }
        else {
            m_shakeDuration = 0.0f;
            m_shakeTimer = 0.0f;
        }
    }

    // �_�b�V���h�ꏈ��
    CVector3D dashShakeOffset(0.0f, 0.0f, 0.0f);
    if (Player::Instance()->GetDashing() && Player::Instance()->GetMove()) {
        static float dashShakeTimer = 0.0f;
        dashShakeTimer += CFPS::GetDeltaTime() * 20.0f;
        if (m_dist == 0.0f) {
            float dashShakeIntensity = 0.07f;
            dashShakeOffset.y = sin(dashShakeTimer) * dashShakeIntensity;
        }
        else {
            float dashShakeIntensity = 0.05f;
            dashShakeOffset.y = sin(dashShakeTimer) * dashShakeIntensity;
        }
    }

    // �v���C���[�̈ʒu����ɃJ������ݒ�
    CVector3D playerPos = Player::Instance()->GetPos();
    CVector3D headOffset(0.0f, 1.0f, 0.0f);
    CVector3D cameraOffset(0.0f, 0.7f, -m_dist);

    m_pos = playerPos + headOffset + CMatrix::MRotation(m_rot) * cameraOffset + shakeOffset + dashShakeOffset;

    if (Player::Instance()->GetDashing() && Player::Instance()->GetMove() && m_dist == 0.0f) {
        m_pos += CVector3D(0, 0.12f, 0);
    }

    // �J�����̍s����v���C���[��Őݒ�
    CMatrix cam_matrix = CMatrix::MTranselate(m_pos) * CMatrix::MRotation(m_rot);
    CCamera::GetCurrent()->SetTranseRot(cam_matrix);
}


