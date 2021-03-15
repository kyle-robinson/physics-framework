#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
class GameObject;

class Transform
{
public:
	Transform() :
		_scale( 0.0f, 0.0f, 0.0f ),
		_rotation( 0.0f, 0.0f, 0.0f ),
		_position( 0.0f, 0.0f, 0.0f ),
		_initialPosition( 0.0f, 0.0f, 0.0f ),
		_parent( nullptr )
	{
		XMMATRIX matrix = XMMatrixIdentity();
		XMStoreFloat4x4( &_worldMatrix, matrix );
		XMStoreFloat4x4( &_rotationMatrix, matrix );
	}

	void Update()
	{
		// Calculate world matrix
		XMMATRIX scale = XMMatrixScaling( GetScale().x, GetScale().y, GetScale().z );
		XMMATRIX rotation = XMMatrixRotationX( GetRotation().x ) * XMMatrixRotationY( GetRotation().y ) * XMMatrixRotationZ( GetRotation().z );
		XMMATRIX translation = XMMatrixTranslation( GetPosition().x, GetPosition().y, GetPosition().z );
		XMStoreFloat4x4( &_worldMatrix, scale * rotation * translation );

		// Calculate rotation matrix
		XMStoreFloat4x4( &_rotationMatrix, scale * rotation );
	}

	void SetInitialPosition( v3df initialPosition ) { _initialPosition = initialPosition; _position = _initialPosition; }
	void SetInitialPosition( float x, float y, float z ) { _initialPosition = { x, y, z }; _position = _initialPosition; }
	v3df GetInitialPosition() const noexcept { return _initialPosition; }

	void SetPosition( v3df position ) { _position = position; }
	void SetPosition( float x, float y, float z ) { _position = { x, y, z }; }
	v3df GetPosition() const noexcept { return _position; }
	void ResetPosition() { _position = _initialPosition; }

	void SetScale( const v3df& scale ) { _scale = scale; }
	void SetScale( float x, float y, float z ) { _scale = { x, y, z }; }
	v3df GetScale() const noexcept { return _scale; }

	void SetRotation( v3df rotation ) { _rotation = rotation; }
	void SetRotation( float x, float y, float z ) { _rotation = { x, y, z }; }
	v3df GetRotation() const noexcept { return _rotation; }

	GameObject* GetParent() const noexcept { return _parent; }
	void SetParent( GameObject* parent ) noexcept { _parent = parent; }
	XMFLOAT4X4 GetWorldMatrixFloat4x4() const noexcept { return _worldMatrix; }
	XMFLOAT4X4 GetRotationMatrixFloat4x4() const noexcept { return _rotationMatrix; }
	const XMMATRIX& GetWorldMatrix() const noexcept { return XMLoadFloat4x4( &_worldMatrix ); }
	const XMMATRIX& GetRotationMatrix() const noexcept { return XMLoadFloat4x4( &_rotationMatrix ); }

	void SetWorldMatrixFloat4x4( XMFLOAT4X4 matrix ) noexcept { _worldMatrix = matrix; }
	void SetRotationMatrixFloat4x4( XMFLOAT4X4 matrix ) noexcept { _rotationMatrix = matrix; }
private:
	v3df _scale;
	v3df _rotation;
	v3df _position;
	v3df _initialPosition;

	GameObject* _parent;
	//XMFLOAT4X4 _transform;
	XMFLOAT4X4 _worldMatrix;
	XMFLOAT4X4 _rotationMatrix;
};

#endif