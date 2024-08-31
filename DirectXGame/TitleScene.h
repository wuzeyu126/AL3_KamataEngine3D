#pragma once
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "titleMoji.h"

class TitleScene {
private:
	DirectXCommon* dxCommon_ = nullptr;

	Model* modelTitle_ = nullptr;
	ViewProjection viewProjection_;

	titleMoji* moji_ = nullptr;
	Player* player_ = nullptr;
	bool finished_ = false;

public:
	TitleScene();
	~TitleScene();
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }
};