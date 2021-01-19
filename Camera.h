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

	void Update();

	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProjection() const { return _projection; }

	XMFLOAT4X4 GetViewProjection() const;

	v3df GetPosition() const { return _eye; }
	v3df GetLookAt() const { return _at; }
	v3df GetUp() const { return _up; }

	void SetPosition( v3df position ) { _eye = position; }
	void SetLookAt( v3df lookAt ) { _at = lookAt; }
	void SetUp( v3df up ) { _up = up; }

	void Reshape( FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth );
};

#endif