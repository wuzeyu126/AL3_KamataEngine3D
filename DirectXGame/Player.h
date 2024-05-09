#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"


class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle,ViewProjection* viewProjection );

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	private:
		WorldTransform worldTransform_;
		Model* model_;
		uint32_t textureHandle_ = 0u;
	    ViewProjection* viewProjection_ = nullptr;
};
