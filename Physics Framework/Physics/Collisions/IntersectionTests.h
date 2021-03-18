#pragma once
#ifndef INTERSECTIONTESTS_H
#define INTERSECTIONTESTS_H

#include "CollisionPrimitives.h"

class IntersectionTests
{
public:
	static bool SphereAndHalfSpace(
		const Sphere& sphere,
		const CollisionPlane& plane );

	static bool SphereAndSphere(
		const Sphere& a,
		const Sphere& b );

	static bool BoxAndBox(
		const Box& a,
		const Box& b );

	static bool BoxAndHalfSpace(
		const Box& box,
		const CollisionPlane& plane );
};

#endif