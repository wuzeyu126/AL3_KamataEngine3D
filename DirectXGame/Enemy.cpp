#include "Enemy.h"

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	viewProjection_ = viewProjection;

	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_);
	float radian = kWalkRadianBegin + kWalkRadianEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = std::sin(radian);
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos = {};
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Enemy::OnCollision(const Player* player) { (void)player; }