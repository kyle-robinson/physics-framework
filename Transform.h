#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

class GameObject;

class Transform
{
public:
	static void MoveForward( GameObject& object );
	static void MoveBackward( GameObject& object );

	void SetPosition( v3df position ) { _position = position; }
	void SetPosition( float x, float y, float z ) { _position = { x, y, z }; }
	v3df GetPosition() const { return _position; }

	void SetScale( const v3df& scale ) { _scale = scale; }
	void SetScale( float x, float y, float z ) { _scale = { x, y, z }; }
	v3df GetScale() const { return _scale; }

	void SetRotation( v3df rotation ) { _rotation = rotation; }
	void SetRotation( float x, float y, float z ) { _rotation = { x, y, z }; }
	v3df GetRotation() const { return _rotation; }

private:
	v3df _position;
	v3df _rotation;
	v3df _scale;
};

#endif