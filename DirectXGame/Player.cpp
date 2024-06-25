#include "Player.h"
#include <algorithm>
#include <cassert>
#include <numbers>

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	viewProjection_ = viewProjection;
	textureHandle_ = textureHandle;
}

void Player::Update() {

	bool landing = false;
	LandingJudgment(landing);
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		// 移动
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_D)) {


				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotation_ = std::numbers::pi_v<float>;
					turnTimer_ = 1.0f;
				}

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_A)) {
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotation_ = 0.0f;
					turnTimer_ = 1.0f;
				}
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
			}
			velocity_ = Add(velocity_, acceleration);
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} 
		else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_ = Add(velocity_, {0.0f, kJumpAcceleration, 0.0f});
			onGround_ = false;
		}

	}else {
		velocity_ = Add(velocity_, {0.0f, -kGravityAcceleration, 0.0f});
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		if (landing) {
			worldTransform_.translation_.y = 2.0f; 
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
	ConvolutionalControl();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	mapChipCollision(collisionMapInfo);

	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();
}



void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }

WorldTransform& Player::GetWorldTransform() { return worldTransform_; }

void Player::LandingJudgment(bool& landing) {
	if (velocity_.y < 0.0f) {
		if (worldTransform_.translation_.y <= 2.0f) { 
			landing = true;
		}
	}
}


void Player::ConvolutionalControl() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::clamp(turnTimer_ - 1 / 30.0f, 0.0f, turnTimer_);
		float destinationRotationYTable[] = {
		    0, std::numbers::pi_v<float>,
		   
		};
		
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = std::lerp(destinationRotationY, turnFirstRotation_, turnTimer_);
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f,  -kHeight / 2.0f, 0.0f},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {kWidth / 2.0f,  kHeight / 2.0f,  0.0f},
	    {-kWidth / 2.0f, kHeight / 2.0f,  0.0f},
	};

	return Add(center, offsetTable[static_cast<uint32_t>(corner)]);
}


void Player::mapChipCollision(CollisionMapInfo& info) {
	mapChipCollisionTop(info);
	/*isMapChipDownCollision(info);
	isMapChipRightCollision(info);
	isMapChipLeftCollision(info);*/
}

void Player::mapChipCollisionTop(CollisionMapInfo& info) {

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	if (info.move.y <= 0) {
		return;
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(worldTransform_.translation_, info.move));
		MapChipField::Rect rect = mapChipField_->GetRectByIndexSet(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, info.move.y);
		info.ceiling = true;
	}

	collisionResult(info);

	CeilingCollision(info);
}

// void Player::isMapChipDownCollision(CollisionMapInfo& info) {}
//
// void Player::isMapChipRightCollision(CollisionMapInfo& info) {}
//
// void Player::isMapChipLeftCollision(CollisionMapInfo& info) {}

void Player::collisionResult(CollisionMapInfo& info) { worldTransform_.translation_ = Add(worldTransform_.translation_, info.move); }

void Player::CeilingCollision(Player::CollisionMapInfo& info) {
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}