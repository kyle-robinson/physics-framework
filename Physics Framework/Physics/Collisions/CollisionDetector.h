#pragma once
#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "CollisionPrimitives.h"
#include "CollisionData.h"

class CollisionDetector
{
public:
	// sphere collisions
	static void SphereAndHalfSpace(
		const CollisionSphere& sphere,
		const CollisionPlane& plane,
		CollisionData* data );

	static void SphereAndTruePlane(
		const CollisionSphere& sphere,
		const CollisionPlane& plane,
		CollisionData* data );

	static void SphereAndSphere(
		const CollisionSphere& a,
		const CollisionSphere& b,
		CollisionData* data );

	// cube collisions
	static uint32_t CubeAndHalfSpace(
		const CollisionCube& box,
		const CollisionPlane& plane,
		CollisionData* data );

	static void CubeAndCube(
		const CollisionCube& a,
		const CollisionCube& b,
		CollisionData* data );

	static void CubeAndPoint(
		const CollisionCube& box,
		const v3df& point,
		CollisionData* data );

	static void CubeAndSphere(
		const CollisionCube& box,
		const CollisionSphere& sphere,
		CollisionData* data );
};

#endif