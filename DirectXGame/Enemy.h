#pragma once
#include "ImGuiManager.h"
#include "MapChipField.h"
#include "Model.h"
#include "Windows.h"
#include "WorldTransform.h"
#include <algorithm>
#include <cassert>
#include <input.h>
#include <iostream>
#include <numbers>

class Player;

class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position,uint32_t textHandle);

	void Update();

	void Draw();

	WorldTransform& GetWorldTransform() { return worldTransform_; }

	Vector3 GetWorldPosition();

	AABB GetAABB();

	void OnCollision(const Player* player);

	private:
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;

	static inline const float kWalkSpeed = 0.05f;
	Vector3 velocity_ = {-kWalkSpeed, 0.0f, 0.0f};

	static inline const float kWalkRadianBegin = -1 * std::numbers::pi_v<float>;
	static inline const float kWalkRadianEnd = 2 * std::numbers::pi_v<float>;
	static inline const float kWalkRadianTime = 2.0f;

	float walkTimer_ = 0.0f;

	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;

	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;
};