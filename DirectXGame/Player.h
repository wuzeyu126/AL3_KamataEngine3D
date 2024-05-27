#pragma once
#define NOMINMAX
#include "Model.h"
#include "WorldTransform.h"
#include <cassert>
#include <numbers>
#include <input.h>
#include <algorithm>

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;
	WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.01f;
	static inline const float kLimitRunSpeed = 3.0f;

	enum class LRDirection {
		kRight,
		kLeft
	};
	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;
	static inline const float kGravityAcceleartion = 0.3f;
	static inline const float kLimitFallSpeed = 0.8f;
	static inline const float kJumpAcceleration = 2.0f;
};