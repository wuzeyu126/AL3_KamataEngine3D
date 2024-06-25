#pragma once
#define NOMINMAX
#include "Input.h"
#include "MT3.h"
#include "MapChipField.h"
#include "Model.h"
#include "WorldTransform.h"
#include <algorithm>
#include <cassert>
#include <numbers>

enum class LRDirection {
	kRight,
	kLeft,
};

class MapChipField;

class Player {
public:
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool wall = false;
		Vector3 move;
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
		kNumCorner,
	};

	Player();
	~Player();

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle);

	void Update();

	void LandingJudgment(bool& landing);

	void ConvolutionalControl();

	void Draw();

	const Vector3& GetVelocity() { return velocity_; }

	WorldTransform& GetWorldTransform();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void mapChipCollision(CollisionMapInfo& info);

	void mapChipCollisionTop(CollisionMapInfo& info);

	void mapChipCollisionBottom(CollisionMapInfo& info);

	void mapChipCollisionRight(CollisionMapInfo& info);

	void mapChipCollisionLeft(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void collisionResult(CollisionMapInfo& info);

	void CeilingCollision(CollisionMapInfo& info);

	void WallCollision(CollisionMapInfo& info);

	void landingSwitch(CollisionMapInfo& info);

private:
	uint32_t textureHandle_ = 0u;
	Model* model_ = nullptr;

	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kLimitRunSpeed = 0.4f;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotation_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;
	bool onGround_ = true;
	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 0.35f;
	static inline const float kJumpAcceleration = 0.5f;

	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 1.99f;
	static inline const float kHeight = 1.99f;
	static inline const float kBlank = 0.01f;

	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;

	

	static inline const float kAttenuationLanding = 0.05f;
	static inline const float kAttenuationWall = 0.05f;
};