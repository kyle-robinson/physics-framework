#pragma once
#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "CollisionPrimitives.h"
#include "CollisionData.h"

class CollisionDetector
{
public:
	static unsigned SphereAndHalfSpace(
		const Sphere& sphere,
		const CollisionPlane& plane,
		CollisionData* data );

	static unsigned SphereAndTruePlane(
		const Sphere& sphere,
		const CollisionPlane& plane,
		CollisionData* data );

	static unsigned SphereAndSphere(
		const Sphere& a,
		const Sphere& b,
		CollisionData* data );

	static unsigned BoxAndHalfSpace(
		const Box& box,
		const CollisionPlane& plane,
		CollisionData* data );

	static unsigned BoxAndBox(
		const Box& a,
		const Box& b,
		CollisionData* data );

	static unsigned BoxAndPoint(
		const Box& box,
		const v3df& point,
		CollisionData* data );

	static unsigned BoxAndSphere(
		const Box& box,
		const Sphere& sphere,
		CollisionData* data );
};

#endif