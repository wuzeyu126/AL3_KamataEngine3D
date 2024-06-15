#include "Player.h"

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;

	worldTransform_.translation_.x -= 2.0f;
	worldTransform_.translation_.y -= 2.0f;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {

	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
					lrDirection_ = LRDirection::kRight;	
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
					lrDirection_ = LRDirection::kLeft;
				}
			}
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
			if (velocity_.x * velocity_.x < 0.001f) {
				velocity_.x = 0.0f;
			}
		}
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_.y += kJumpAcceleration;
		}
	} else {
		velocity_.y += -kGravityAcceleartion;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	CollisionMapInfo info;
	info.move = velocity_;
	CollisionMap(info);

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.UpdateMatrix();


	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 0.0166f;
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		float easing = 1.0f - turnTimer_ / kTimeTurn;
		float nowRotationY = std::lerp(turnFirstRotationY_, destinationRotationY, easing);
		worldTransform_.rotation_.y = nowRotationY;
	}

	bool landing = false;
	if (velocity_.y < 0.0f) {
		if(worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}         
	}

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 2.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
}

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }

WorldTransform& Player::GetWorldTransform() { return worldTransform_; }

void Player::SetMapChipField(MapChipField* mapChipField) { mapChipField = mapChipField_; }

//void Player::CollisionMap(CollisionMapInfo& info) {
//	CollisionMapTop(info);
//	CollisionMapBottom(info);
//	CollisionMapRight(info);
//	CollisionMapLeft(info);
//}



void Player::CollisionMap(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		Vector3 result;
		result.x = worldTransform_.translation_.x + info.move.x;
		result.y = worldTransform_.translation_.y + info.move.y;
		result.z = worldTransform_.translation_.z + info.move.z;

		positionsNew[i] = CornerPositon(result, static_cast<Corner>(i));
	}

	if (info.move.y <= 0) {
		return;
	}

	MapChipType mapChipType;
	bool hit = false;
	IndexSet indexSet;

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
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, kBlank);
		info.celling = true;
	}
}

Vector3 Player::CornerPositon(const Vector3& center, Corner corner) {
	Vector3 result;
	

	Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f,  -kHeight / 2.0f, 0}, 
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, 
	    {kWidth / 2.0f,  +kHeight / 2.0f, 0}, 
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, 
	};

	result.x = center.x + offsetTable[static_cast<uint32_t>(corner)].x;
	result.y = center.y + offsetTable[static_cast<uint32_t>(corner)].y;
	result.z = center.z + offsetTable[static_cast<uint32_t>(corner)].z;
	return result;
}

void Player::AfterCollision(const CollisionMapInfo& info) { 
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
	worldTransform_.translation_.z += info.move.z;

}

void Player::HitCelling(const CollisionMapInfo& info) {
	if (info.celling) {
		velocity_.y = 0;
	}
}
