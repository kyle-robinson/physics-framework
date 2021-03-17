#pragma once
#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

class Camera
{
public:
	Camera( const XMFLOAT3& initialPosition );
	void SetProjectionValues( FLOAT fovDegrees,FLOAT aspectRatio, FLOAT nearZ, FLOAT farZ  );

	const XMMATRIX& GetViewMatrix() const noexcept { return view; };
	const XMMATRIX& GetProjectionMatrix() const noexcept { return projection; };
	const XMMATRIX& GetViewProjectionMatrix() const noexcept { return view * projection; };

	const XMFLOAT3& GetPositionFloat3() const noexcept { return position; };
	const v3df& GetPositionVector3() const noexcept { return { position.x, position.y, position.z }; };
	const XMFLOAT3& GetRotationFloat3() const noexcept { return rotation; };
	const v3df& GetRotationVector3() const noexcept { return { rotation.x, rotation.y, rotation.z }; };

	const XMVECTOR& GetForwardVector( BOOL omitY = false ) noexcept;
	const XMVECTOR& GetBackwardVector( BOOL omitY = false ) noexcept;
	const XMVECTOR& GetLeftVector( BOOL omitY = false ) noexcept;
	const XMVECTOR& GetRightVector( BOOL omitY = false ) noexcept;
	const XMVECTOR& GetUpVector() noexcept;

	void SetPosition( const XMFLOAT3& pos ) noexcept;
	void SetPosition( const XMVECTOR& pos ) noexcept;
	void SetPosition( FLOAT x, FLOAT y, FLOAT z ) noexcept;
	void AdjustPosition( const XMFLOAT3& pos ) noexcept;
	void AdjustPosition( const XMVECTOR& pos ) noexcept;
	void AdjustPosition( FLOAT x, FLOAT y, FLOAT z ) noexcept;

	void SetRotation( const XMFLOAT3& rot ) noexcept;
	void SetRotation( const XMVECTOR& rot ) noexcept;
	void SetRotation( FLOAT x, FLOAT y, FLOAT z ) noexcept;
	void AdjustRotation( const XMFLOAT3& rot ) noexcept;
	void AdjustRotation( const XMVECTOR& rot ) noexcept;
	void AdjustRotation( FLOAT x, FLOAT y, FLOAT z ) noexcept;

	void UpdateMatrix();

	// CAMERA MOVEMENT
	const FLOAT& Camera::GetCameraSpeed() const noexcept { return cameraSpeed; };
	void SetCameraSpeed( FLOAT newSpeed ) noexcept { cameraSpeed = newSpeed; };
	void UpdateCameraSpeed( FLOAT updateSpeed ) noexcept { cameraSpeed += updateSpeed; };

	static void MoveForward( std::shared_ptr<Camera>& camera, FLOAT dt ) noexcept
	{
		camera->AdjustPosition( camera->GetForwardVector() * camera->GetCameraSpeed() * dt );
	}
	static void MoveBackward( std::shared_ptr<Camera>& camera, FLOAT dt ) noexcept
	{
		camera->AdjustPosition( camera->GetBackwardVector() * camera->GetCameraSpeed() * dt );
	}
	static void MoveLeft( std::shared_ptr<Camera>& camera, FLOAT dt ) noexcept
	{
		camera->AdjustPosition( camera->GetLeftVector() * camera->GetCameraSpeed() * dt );
	}
	static void MoveRight( std::shared_ptr<Camera>& camera, FLOAT dt ) noexcept
	{
		camera->AdjustPosition( camera->GetRightVector() * camera->GetCameraSpeed() * dt );
	}
	static void MoveUp( std::shared_ptr<Camera>& camera, FLOAT dt ) noexcept
	{
		camera->AdjustPosition( 0.0f, camera->GetCameraSpeed() * dt, 0.0f );
	}
	static void MoveDown( std::shared_ptr<Camera>& camera, FLOAT dt ) noexcept
	{
		camera->AdjustPosition( 0.0f, -camera->GetCameraSpeed() * dt, 0.0f );
	}

private:
	void UpdateDirectionVectors();
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR vec_forward, vec_left, vec_right, vec_backward;
	XMVECTOR vec_forward_noY, vec_left_noY, vec_right_noY, vec_backward_noY;

	XMMATRIX view, projection;
	XMFLOAT3 position, rotation;
	XMVECTOR posVector, rotVector;
	FLOAT fovDegrees, aspectRatio, nearZ, farZ, cameraSpeed;
};

#endif