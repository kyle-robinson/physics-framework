#pragma once
#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

class Camera
{
private:
	v3df _eye; 
	v3df _at;
	v3df _up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
public:
	Camera( v3df position, v3df at, v3df up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth );
	void Reshape( FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth );
	void Update();

	XMFLOAT4X4 GetViewProjection() const;
	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProjection() const { return _projection; }

	v3df GetUp() const { return _up; }
	v3df GetLookAt() const { return _at; }
	v3df GetPosition() const { return _eye; }

	void SetUp( v3df up ) { _up = up; }
	void SetLookAt( v3df lookAt ) { _at = lookAt; }
	void SetPosition( v3df position ) { _eye = position; }
	void AdjustRotation( float x, float y, float z ) noexcept;

	static void MoveFoward( std::unique_ptr<Camera>& camera );
	static void MoveBackward( std::unique_ptr<Camera>& camera );
	static void MoveRight( std::unique_ptr<Camera>& camera );
	static void MoveLeft( std::unique_ptr<Camera>& camera );
private:
	float cameraOrbitRadius = 7.0f;
	float cameraOrbitRadiusMin = 2.0f;
	float cameraOrbitRadiusMax = 50.0f;
	float cameraOrbitAngleXZ = -90.0f;
	float cameraSpeed = 2.0f;
	v3df rotation;
};

#endif