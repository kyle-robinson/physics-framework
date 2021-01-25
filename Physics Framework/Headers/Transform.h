#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "GameObject.h"

class Transform
{
public:
	Transform() {}
	void Update( float t )
	{
		// Calculate world matrix
		XMMATRIX scale = XMMatrixScaling( GetScale()[0], GetScale()[1], GetScale()[2] );
		XMMATRIX rotation = XMMatrixRotationX( GetRotation()[0] ) * XMMatrixRotationY( GetRotation()[1] ) * XMMatrixRotationZ( GetRotation()[2] );
		XMMATRIX translation = XMMatrixTranslation( GetPosition()[0], GetPosition()[1], GetPosition()[2] );

		XMStoreFloat4x4( &_world, scale * rotation * translation );

		//if ( _parent != nullptr )
		//	XMStoreFloat4x4( &_world, this->GetWorldMatrix() * _parent->GetTransform()->GetWorldMatrix() );
	}

	void SetPosition( v3df position ) { _position = position; }
	void SetPosition( float x, float y, float z ) { _position = { x, y, z }; }
	v3df GetPosition() const noexcept { return _position; }

	void SetScale( const v3df& scale ) { _scale = scale; }
	void SetScale( float x, float y, float z ) { _scale = { x, y, z }; }
	v3df GetScale() const noexcept { return _scale; }

	void SetRotation( v3df rotation ) { _rotation = rotation; }
	void SetRotation( float x, float y, float z ) { _rotation = { x, y, z }; }
	v3df GetRotation() const noexcept { return _rotation; }

	void const MoveForward() { _position[2] += 0.002f; }
	void const MoveBackward() { _position[2] -= 0.002f; }
	void const MoveLeft() { _position[0] -= 0.002f; }
	void const MoveRight() { _position[0] += 0.002f; }

	//void SetParent( GameObject* parent ) { _parent = parent; }
	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4( &_world ); }
private:
	v3df _scale;
	v3df _position;
	v3df _rotation;
	XMFLOAT4X4 _world;
	//GameObject* _parent;
};

#endif