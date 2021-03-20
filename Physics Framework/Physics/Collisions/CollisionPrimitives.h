#pragma once
#ifndef COLLISIONPRIMITIVES_H
#define COLLISIONPRIMITIVES_H

#include "../RigidBody.h"

class CollisionPrimitive
{
public:
	std::shared_ptr<RigidBody> _body;
	Matrix4 _offset;
public:
	const Matrix4& GetTransform() const noexcept { return _transform; }
	void CalculateInternals() noexcept { _transform = _body->GetTransformMat() * _offset; }
	v3df GetAxis( uint32_t index ) const noexcept { return _transform.GetAxisVector( index ); }
protected:
	Matrix4 _transform;
};

#pragma region Primitives
class CollisionSphere : public CollisionPrimitive
{
public:
	float _radius;
};

class CollisionPlane : public CollisionPrimitive
{
public:
	v3df _direction;
	float _offset = 0.0f;
};

class CollisionCube : public CollisionPrimitive
{
public:
	v3df _halfSize;
};
#pragma endregion

#endif