#pragma once
#define NOMINMAX
#include <ViewProjection.h>
#include "MT3.h"

class Player;

struct Rect {
	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;
};

class CameraController {
public:
	void Initialize();

	void Update();

	void Reset();

	void SetTarget(Player* target);

	ViewProjection& GetViewProjection() { return viewProjection_; }

	void SetMoveableArea(Rect area) { movableArea_ = area; }

private:
	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -30.0f};
	Rect movableArea_ = {0, 100, 0, 100};

	Vector3 targetCoordinates_ = {};
	static inline const float kInterpolationRate_ = 0.5f;
};