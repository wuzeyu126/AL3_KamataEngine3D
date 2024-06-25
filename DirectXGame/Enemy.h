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

class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle);
	void Update();
	void Draw();
	void SetMapChipField(MapChipField* mapChipField);

private:
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;
	WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_ = {};

	MapChipField* mapChipField_ = nullptr;

	float walkTimer_ = 0.0f;
};
