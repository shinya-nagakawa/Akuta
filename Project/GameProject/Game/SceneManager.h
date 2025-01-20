#pragma once

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Game/Game.h"
#include "Game.h"
#include "Title.h"

class SceneManager:public Base
{
public:
	//指定したシーンをロード
	static void LoadGame(int fieldId);
	static void LoadTitle();
};

#endif // SCENE_MANAGER_H
