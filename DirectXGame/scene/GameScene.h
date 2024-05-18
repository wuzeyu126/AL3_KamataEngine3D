#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"
#include "DebugCamera.h"
#include "AxisIndicator.h"
#include <cassert>
#include "SkyDome.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	Model* modelBlock_ = nullptr;
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	uint32_t textureHandle_ = 0;

	SkyDome* skyDome_ = nullptr;
	Model* modelSkyDome_ = nullptr;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
