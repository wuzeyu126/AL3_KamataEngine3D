#include "CameraController.h"
#include "Player.h"
#include <cmath>

void CameraController::Initialize() { viewProjection_.Initialize(); }

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	targetCoordinates_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	targetCoordinates_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	targetCoordinates_.z = targetWorldTransform.translation_.z + targetOffset_.z;



	viewProjection_.translation_.x = Lerp(viewProjection_.translation_.x, targetCoordinates_.x, kInterpolationRate_);
	viewProjection_.translation_.y = Lerp(viewProjection_.translation_.y, targetCoordinates_.y, kInterpolationRate_);
	viewProjection_.translation_.z = Lerp(viewProjection_.translation_.z, targetCoordinates_.z, kInterpolationRate_);


	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, movableArea_.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, movableArea_.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, movableArea_.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, movableArea_.top);
	viewProjection_.UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	viewProjection_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	viewProjection_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	viewProjection_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}

void CameraController::SetTarget(Player* target) { target_ = target; }