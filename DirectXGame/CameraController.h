#pragma once
#define NOMINMAX
#include "ViewProjection.h"
#include <algorithm>
#include "MT3.h"

class Player;

class CameraController {
public:
	/// <summary>
	/// カメラ移動範囲
	/// </summary>
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initalize(ViewProjection* viewProjection);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// カメラの位置を更新
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(Player* target) { target_ = target; }
	/// <summary>
	/// 瞬間合わせ
	/// </summary>
	void Reset();
	/// <summary>
	/// カメラ移動範囲 set
	/// </summary>
	/// <param name="area"></param>
	void SetMoveableArea(Rect area) { moveableArea_ = area; }

private:
	ViewProjection* viewProjection_ = nullptr;
	// 获取player对象
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差
	Vector3 targetOffset_ = {10.0f, 10.0f, -35.0f};

	// カメラ移動範囲
	Rect moveableArea_ = {1000, 1000, 1000, 1000};

	// 座標補間
	Vector3 endPosition;
	static inline const float kInterpolationRate = 0.1f;

	// 速度加算 为了让镜头更快一点 看到后面的场景
	static inline const float kVelocityBias = 15.0f;

	// 追従対象画面内
	static inline const Rect margin = {-1000, 1000, -1000, 1000};
};