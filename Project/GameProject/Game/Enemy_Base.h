#pragma once
#include "Carry.h"
#include "NavNode.h"
#include "Player.h"

class Enemy_Base :public Carry
{
protected:
	CVector3D m_pos;

	float m_viewAngle;		//‹–ìŠp“x
	float m_viewLength;		//‹–ì‹——£
	float m_collisionRadius;	//Õ“Ë”¼Œa

public:
	Enemy_Base(const CVector3D& pos, float viewAngle,float viewLength, float collisionRadius);	//Õ“Ë”¼Œa

	virtual void Update() = 0;	//”h¶ƒNƒ‰ƒX‚Å•K{‚ÌÀ‘•
	virtual void Render();

	bool IsFoundPlayer()const;
	bool IsLookPlayer()const;

	bool CheckWallCollision(const CVector3D& dir);
	bool IsWallNearby(float checkDistance);
	CVector3D CalculateAvoidDirection(float checkDistance);

	void RenderVision();
private:
	Player* mp_player;

};