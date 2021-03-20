#pragma once
#ifndef INTERSECTIONTESTS_H
#define INTERSECTIONTESTS_H

#include "CollisionPrimitives.h"

class IntersectionTests
{
public:
	// sphere collisions
	static bool SphereAndHalfSpace(
		const CollisionSphere& sphere,
		const CollisionPlane& plane );

	static bool SphereAndSphere(
		const CollisionSphere& a,
		const CollisionSphere& b );

	// cube intersections
	static bool CubeAndCube(
		const CollisionCube& a,
		const CollisionCube& b );

	static bool CubeAndHalfSpace(
		const CollisionCube& box,
		const CollisionPlane& plane );
};

#endif