#include "Player.h"
#include "TextureManager.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection) { 
	assert(model);

	textureHandle_ = textureHandle;
	model_ = model;
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
}

void Player::Update() {
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
}
