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
#include "MapChipField.h"
#include "Player.h"
#include "Enemy.h"
#include "CameraController.h"
#include "DeathParticles.h"

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

	/// <summary>
	/// block
	/// </summary>
	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return finished_; }


	bool IsDead() { return isPlayerDead_; }

	bool SetIsDead(bool value) {
		isPlayerDead_ = value;
		return isPlayerDead_;
	}

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
	uint32_t textureHandlePlayer_ = 0u;
	uint32_t textureHandleEnemy_ = 0u;

	SkyDome* skyDome_ = nullptr;
	Model* modelSkyDome_ = nullptr;

	MapChipField* mapChipField_;

	Model* modelPlayer_ = nullptr;
	Player* player_ = nullptr;

	Model* modelEnemy_ = nullptr;
	std::list<Enemy*> enemies_;
	static inline const uint32_t enemyNums = 3;

	CameraController* cameraController_ = nullptr;
	Rect movableArea_ = {};


	DeathParticles* deathParticles_ = nullptr;
	Model* modelParticle_ = nullptr;



	enum class Phase {
		kPlay,
		kDeath,
	};
	Phase phase_;

	bool finished_ = false;
	bool isPlayerDead_ = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
