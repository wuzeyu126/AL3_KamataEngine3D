#include "Enemy.h"

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.translation_.x -= 2.0f;
	worldTransform_.translation_.y -= 2.0f;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	walkTimer_ = 0.0f;
}

void Enemy::Update() { walkTimer_ += 1.0f / 60.0f; }

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }

void Enemy::SetMapChipField(MapChipField* mapChipField) { mapChipField = mapChipField_; }
