#pragma once
#include "../Base/Base.h"
#include "FreeRoamingCamera.h"

class Camera : public Base {
public:
	
	enum CameraMode 
	{
		ePlayerCamera,
		eFreeRoam,
		eFixed
	};

	Camera();

	void Update();
	void Render();
	CVector3D GetPos();
	CVector3D GetRotation();
	float GetDistance();
	static Camera* Instance();

	

	void Shake(float intensity, float duration);
	void SetFixedCamera(const CVector3D& position, const CVector3D& rotation);
	void SetMode(CameraMode mode);
	CameraMode GetCurrentMode() const { return m_mode; }

private:
	
	static Camera* ms_instance;
	FreeRoamingCamera m_freeCam;
	CameraMode m_mode;
	float m_dist;
	float cam_speed;
	//�J�����V�F�C�N�֘A
	float m_shakeIntensity;
	float m_shakeDuration;
	float m_shakeTimer;
	//�t���[���[�~���O�J�����֘A
	bool m_isFreeRoaming;	//�t���[���[�~���O���[�h���ǂ���
	float m_moveSpeed;		//���R�J�����̈ړ����x
	float m_rotateSpeed;	//��]���x

	void UpdatePlayerCamera();

	CVector3D playerPos;
	CVector3D stableCameraPos;
	CVector3D m_fixedPosition;
	CVector3D m_fixedRotation;

};