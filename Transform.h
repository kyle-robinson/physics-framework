#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

class Transform
{
public:
	void const SetPosition( v3df position ) { _position = position; }
	void const SetPosition( float x, float y, float z ) { _position = { x, y, z }; }
	v3df const GetPosition() const noexcept { return _position; }

	void const SetScale( const v3df& scale ) { _scale = scale; }
	void const SetScale( float x, float y, float z ) { _scale = { x, y, z }; }
	v3df const GetScale() const noexcept { return _scale; }

	void const SetRotation( v3df rotation ) { _rotation = rotation; }
	void const SetRotation( float x, float y, float z ) { _rotation = { x, y, z }; }
	v3df const GetRotation() const noexcept { return _rotation; }

	void const MoveForward() { _position[2] += 0.002f; }
	void const MoveBackward() { _position[2] -= 0.002f; }
	void const MoveLeft() { _position[0] -= 0.002f; }
	void const MoveRight() { _position[0] += 0.002f; }

private:
	v3df _position;
	v3df _rotation;
	v3df _scale;
};

#endif