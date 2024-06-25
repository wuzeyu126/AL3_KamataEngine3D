#include "Player.h"

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

	velocity_ = AddV3(velocity_, {0.0f, -kGravityAcceleration, 0.0f});
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotation_ = std::numbers::pi_v<float>;
				turnTimer_ = 1.0f;
			}
			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			acceleration.x += kAcceleration;
		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
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
		velocity_ = AddV3(velocity_, acceleration);
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		velocity_.x *= (1.0f - kAttenuation);
	}
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ = AddV3(velocity_, {0.0f, kJumpAcceleration, 0.0f});
		}
	}

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	mapChipCollision(collisionMapInfo);
	collisionResult(collisionMapInfo);
	CeilingCollision(collisionMapInfo);
	WallCollision(collisionMapInfo);
	landingSwitch(collisionMapInfo);
	ConvolutionalControl();
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
		    0,
		    std::numbers::pi_v<float>,
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

	return AddV3(center, offsetTable[static_cast<uint32_t>(corner)]);
}

void Player::mapChipCollision(CollisionMapInfo& info) {
	mapChipCollisionTop(info);
	mapChipCollisionBottom(info);
	mapChipCollisionRight(info);
	mapChipCollisionLeft(info);
}

void Player::mapChipCollisionTop(CollisionMapInfo& info) {

	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(AddV3(worldTransform_.translation_, info.move), static_cast<Corner>(i));
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
		Vector3 offset = {0.0f, kHeight / 2.0f, 0.0f};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(AddV3(AddV3(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndexSet(indexSet.xIndex, indexSet.yIndex);

		float moveY = (rect.bottom - worldTransform_.translation_.y + kBlank) - (kHeight / 2.0f);
		info.move.y = std::max(0.0f, moveY);
		info.ceiling = true;
	}
}

void Player::mapChipCollisionBottom(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(AddV3(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		Vector3 offset = {0.0f, kHeight / 2.0f, 0.0f};

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(MinusV3(AddV3(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndexSet(indexSet.xIndex, indexSet.yIndex);

		float moveY = (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f) + kBlank;

		info.move.y = std::min(0.0f, moveY);
		info.landing = true;
	}
}

void Player::mapChipCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(AddV3(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		Vector3 offset = {kWidth / 2.0f, 0.0f, 0.0f};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(AddV3(AddV3(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndexSet(indexSet.xIndex, indexSet.yIndex);

		float moveX = rect.left - worldTransform_.translation_.x - kWidth / 2 - kBlank;
		info.move.x = std::max(0.0f, moveX);
		info.wall = true;
	}
}

void Player::mapChipCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(AddV3(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		Vector3 offset = {kWidth / 2.0f, 0.0f, 0.0f};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(MinusV3(AddV3(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndexSet(indexSet.xIndex, indexSet.yIndex);

		float moveX = rect.right - worldTransform_.translation_.x + kWidth / 2 + kBlank;
		info.move.x = std::min(0.0f, moveX);
		info.wall = true;
	}
}

void Player::collisionResult(CollisionMapInfo& info) { worldTransform_.translation_ = AddV3(worldTransform_.translation_, info.move); }

void Player::CeilingCollision(CollisionMapInfo& info) {
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}

void Player::WallCollision(CollisionMapInfo& info) {
	if (info.wall) {
		velocity_.x *= (1 - kAttenuationWall);
	}
}

void Player::landingSwitch(CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(AddV3(worldTransform_.translation_, info.move), static_cast<Corner>(i));
			}

			if (info.move.y <= 0) {
				return;
			}

			MapChipType mapChipType;
			bool hit = false;
			MapChipField::IndexSet indexSet;

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			if (!hit) {
				onGround_ = false;
			}
		}
	} else {

		if (info.landing) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}