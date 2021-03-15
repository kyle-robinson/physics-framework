#pragma once
#ifndef COLLISIONPRIMITIVES_H
#define COLLISIONPRIMITIVES_H

#include "Math/Vector.h"
#include "Physics/RigidBody.h"

class CollisionPrimitive {
public:
	std::shared_ptr<RigidBody> _body;
	Matrix4 _offset;

	void CalculateInternals() {
		_transform = _body->GetTransformMat() * _offset;
	}

	v3df GetAxis( unsigned index ) const {
		return _transform.GetAxisVector( index );
	}

	const Matrix4& GetTransform() const {
		return _transform;
	}

protected:
	Matrix4 _transform;
};

class Sphere : public CollisionPrimitive {
public:
	float _radius;
};

class CollisionPlane {
public:
	v3df _direction;
	float _offset = 0.0f;
};

class Box : public CollisionPrimitive {
public:
	v3df _halfSize;
};

#endif