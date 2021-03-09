#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
class GameObject;

#define DEFAULT_FRONT_VECTOR XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f )
#define DEFAULT_UP_VECTOR XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )
#define DEFAULT_RIGHT_VECTOR XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f )
#define DEFAULT_POSITION XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f )

class Transform
{
public:
	Transform() :
		_scale( 0.0f, 0.0f, 0.0f ),
		_rotation( 0.0f, 0.0f, 0.0f ),
		_position( 0.0f, 0.0f, 0.0f ),
		_initialPosition( 0.0f, 0.0f, 0.0f ),
		_parent( nullptr ),
		_orientation( XMQuaternionIdentity() )
	{
		XMMATRIX matrix = XMMatrixIdentity();
		XMStoreFloat4x4( &_worldMatrix, matrix );
		XMStoreFloat4x4( &_rotationMatrix, matrix );
	}
	Transform( const v3df& position, const XMVECTOR& orientation )
	{
		SetPosition( position );
		SetOrientationQuaternion( orientation );
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
	
	const XMVECTOR& GetOrientation() const noexcept { return _orientation; }
	void SetOrientationQuaternion( const XMVECTOR& orientation ) { _orientation = orientation; }
	void SetOrientationRotationMatrix( const XMMATRIX& rotationMatrix )
	{
		XMStoreFloat4x4( &_rotationMatrix, rotationMatrix );
		_orientation = XMQuaternionRotationMatrix( XMLoadFloat4x4( &_rotationMatrix ) );
		UpdateAxisVectorsUsingRotationMatrix();
	}
	void SetOrientationRollPitchYaw( float roll, float pitch, float yaw )
	{
		XMMATRIX matrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );
		XMStoreFloat4x4( &_rotationMatrix, matrix );
		_orientation = XMQuaternionRotationMatrix( XMLoadFloat4x4( &_rotationMatrix ) );
		UpdateAxisVectorsUsingRotationMatrix();
	}

	void RotateUsingAxis( XMVECTOR axis, float angle )
	{
		XMMATRIX matrix = XMMatrixRotationAxis( axis, angle );
		XMMATRIX rotationMatrix = XMLoadFloat4x4( &_rotationMatrix );
		XMMATRIX newRotationMatrix = XMMatrixMultiply( matrix, rotationMatrix );
		XMStoreFloat4x4( &_rotationMatrix, newRotationMatrix );
		_orientation = XMQuaternionRotationMatrix( rotationMatrix );
		UpdateAxisVectorsUsingRotationMatrix();
	}
	void UpdateAxisVectorsUsingRotationMatrix()
	{
		_front = XMVector3Transform( DEFAULT_FRONT_VECTOR, XMLoadFloat4x4( &_rotationMatrix ) );
		_right = XMVector3Transform( DEFAULT_RIGHT_VECTOR, XMLoadFloat4x4( &_rotationMatrix ) );
		_up = XMVector3Transform( DEFAULT_UP_VECTOR, XMLoadFloat4x4( &_rotationMatrix ) );
	}
private:
	v3df _scale;
	v3df _rotation;
	v3df _position;
	v3df _initialPosition;

	GameObject* _parent;
	XMVECTOR _orientation;
	XMFLOAT4X4 _worldMatrix;
	XMFLOAT4X4 _rotationMatrix;

	XMVECTOR _up = DEFAULT_UP_VECTOR;
	XMVECTOR _front = DEFAULT_FRONT_VECTOR;
	XMVECTOR _right = DEFAULT_RIGHT_VECTOR;
	XMVECTOR _positionVector = DEFAULT_POSITION;
};

#endif