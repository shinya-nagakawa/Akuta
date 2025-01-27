#include "SceneManager.h"
#include "BlackFilter.h"

void SceneManager::LoadGame(int fieldId)
{
	KillALL();
	Base::Add(new Game(fieldId));
}

void SceneManager::LoadTitle()
{
	KillALL();
	Base::Add(new Title());

	while (ShowCursor(true) < 0);  // カーソルを再表示
	CInput::ShowCursor(true);
}