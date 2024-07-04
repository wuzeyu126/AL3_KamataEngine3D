#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <numbers>

class DeathParticles {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	private:
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

	static inline const float kDuration = 1.0f;
	static inline const float kSpeed = 0.1f;
	static inline const float kAngleUnit = std::numbers::pi_v<float> * 2 / kNumParticles;

	bool isFinished_ = false;
	float particleDeleteTimer_ = 0.0f;

	ObjectColor objectColor_;
	Vector4 color_;
};