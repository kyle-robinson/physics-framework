#pragma once
#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

class Camera
{
private:
	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
public:
	Camera( v3df position, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth );
	void Reshape( FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth );

	XMFLOAT4X4 GetViewProjection() const;
	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProjection() const { return _projection; }

	const XMFLOAT3& GetPositionFloat3() const noexcept { return position; };
	const v3df& GetPositionVector3() const noexcept { return { position.x, position.y, position.z }; };
	const XMFLOAT3& GetRotationFloat3() const noexcept { return rotation; };
	const v3df& GetRotationVector3() const noexcept { return { rotation.x, rotation.y, rotation.z }; };

	const XMVECTOR& GetForwardVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetBackwardVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetLeftVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetRightVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetUpVector() noexcept;

	void SetPosition( const XMFLOAT3& pos ) noexcept;
	void SetPosition( const XMVECTOR& pos ) noexcept;
	void SetPosition( float x, float y, float z ) noexcept;
	void AdjustPosition( const XMFLOAT3& pos ) noexcept;
	void AdjustPosition( const XMVECTOR& pos ) noexcept;
	void AdjustPosition( float x, float y, float z ) noexcept;

	void SetRotation( const XMFLOAT3& rot ) noexcept;
	void SetRotation( const XMVECTOR& rot ) noexcept;
	void SetRotation( float x, float y, float z ) noexcept;
	void AdjustRotation( const XMFLOAT3& rot ) noexcept;
	void AdjustRotation( const XMVECTOR& rot ) noexcept;
	void AdjustRotation( float x, float y, float z ) noexcept;

	void UpdateMatrix();

	// CAMERA MOVEMENT
	const float& Camera::GetCameraSpeed() const noexcept { return cameraSpeed; };
	void SetCameraSpeed( float newSpeed ) noexcept { cameraSpeed = newSpeed; };
	void UpdateCameraSpeed( float updateSpeed ) noexcept { cameraSpeed += updateSpeed; };

	static void MoveForward( std::unique_ptr<Camera>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetForwardVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveBackward( std::unique_ptr<Camera>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetBackwardVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveLeft( std::unique_ptr<Camera>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetLeftVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveRight( std::unique_ptr<Camera>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetRightVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveUp( std::unique_ptr<Camera>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( 0.0f, camera->GetCameraSpeed() * dt, 0.0f );
	}

	static void MoveDown( std::unique_ptr<Camera>& camera, float dt ) noexcept
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
	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	XMVECTOR vec_forward_noY;
	XMVECTOR vec_left_noY;
	XMVECTOR vec_right_noY;
	XMVECTOR vec_backward_noY;

	float cameraSpeed = 2.0f;
	XMFLOAT3 position, rotation;
	XMVECTOR posVector, rotVector;
};

#endif