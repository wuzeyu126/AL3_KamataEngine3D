#include "GameScene.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "PrimitiveDrawer.h"


GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelBlock_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
	delete skyDome_;
	delete mapChipField_;
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	delete deathParticles_;
	delete cameraController_;
}

void GameScene::Initialize() {

	phase_ = Phase::kPlay;
	isDebugCameraActive_ = false;

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	modelBlock_ = Model::Create();
	worldTransform_.Initialize();
	viewProjection_.Initialize();
	textureHandleEnemy_ = TextureManager::Load("enemyImage.png");
	textureHandlePlayer_ = TextureManager::Load("tex.png");

	debugCamera_ = new DebugCamera(1024, 720);

	/*AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());*/

	modelSkyDome_ = Model::CreateFromOBJ("skyDome", true);
	skyDome_ = new SkyDome;
	skyDome_->Initialize(modelSkyDome_, &viewProjection_);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	player_ = new Player();
	modelPlayer_ = Model::CreateFromOBJ("float_Head", true);
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 99);
	player_->Initialize(modelPlayer_, &viewProjection_, playerPosition, textureHandlePlayer_,this);
	player_->SetMapChipField(mapChipField_);
	
	modelEnemy_ = Model::Create();
	Vector3 enemyPosition[enemyNums];
	for (uint32_t i = 0; i < enemyNums; ++i) {
		Enemy* newEnemy = new Enemy();
		enemyPosition[i] = mapChipField_->GetMapChipPositionByIndex(98 - i,1- i * 3);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition[i], textureHandleEnemy_);
		enemies_.push_back(newEnemy);
	}
	GenerateBlocks();

	cameraController_ = new CameraController();
	cameraController_->Initalize(&viewProjection_);

	
	CameraController::Rect cameraArea;

	cameraArea.left = -50.0f;
	cameraArea.right = 300.0f;
	cameraArea.bottom = -50.0f;
	cameraArea.top = 300.0f;

	// 设置相机的可移动区域
	cameraController_->SetMoveableArea(cameraArea);
	//====================================================

	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	modelParticle_ = Model::CreateFromOBJ("particle", true);
	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelParticle_, &viewProjection_, playerPosition);


	
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}
#endif 
	ChangePhase();
	/*if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.UpdateMatrix();
	}*/

	
}


void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	skyDome_->Draw();
	if (!isPlayerDead_) {
		player_->Draw();
	}
	
	/*for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}*/

	if (deathParticles_ != nullptr && isPlayerDead_) {
		deathParticles_->Draw();
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {
	uint32_t kNumBlockVirtical = mapChipField_->GetNunBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNunBlockHorizontal();
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
#pragma region 玩家和敌人
	AABB aabb1, aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
}


void GameScene::ChangePhase() {
	skyDome_->Update();

	//=======================地图块更新================
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatrix();
		}
	}

	
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	//=======================追踪相机更新================
	cameraController_->Update();

	//===========================阶段=============================

	switch (phase_) {

		// 游玩状态
	case Phase::kPlay:
		//=======================Player更新================
		player_->Update();

		//=======================碰撞更新================
		// all collisions check
		CheckAllCollisions();

	//=======================死亡判定================
		if (isPlayerDead_) {
			const Vector3& deathPosition = player_->GetWorldTransform().translation_;
			deathParticles_->Initialize(modelParticle_, &viewProjection_, deathPosition);
			phase_ = Phase::kDeath;
		}
		
		


		break;



		// 玩家死亡
	case Phase::kDeath:

		//=======================粒子更新================

		if (deathParticles_ != nullptr) {
			deathParticles_->Update();
		}

		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_ = true;
		}

		break;
	}
}