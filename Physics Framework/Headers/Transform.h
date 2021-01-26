#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

class GameObject;

class Transform
{
public:
	
	void SetInitialPosition( v3df initialPosition ) { _initialPosition = initialPosition; _position = _initialPosition; }
	void SetInitialPosition( float x, float y, float z ) { _initialPosition = { x, y, z }; _position = _initialPosition; }
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

	void const MoveForward() { _position[2] += 0.002f; }
	void const MoveBackward() { _position[2] -= 0.002f; }
	void const MoveLeft() { _position[0] -= 0.002f; }
	void const MoveRight() { _position[0] += 0.002f; }

	void SetParent( GameObject* parent ) { _parent = parent; }
	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4( &_world ); }
public:
	XMFLOAT4X4 _world;
	GameObject* _parent;
private:
	v3df _scale;
	v3df _rotation;
	v3df _position;
	v3df _initialPosition;
};

#endif