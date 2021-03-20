#include "stdafx.h"
#include "IntersectionTests.h"

bool IntersectionTests::SphereAndHalfSpace( const CollisionSphere& sphere, const CollisionPlane& plane )
{
	float ballDistance = plane._direction * sphere.GetAxis( 3u ) - sphere._radius;
	return ballDistance <= plane._offset;
}

bool IntersectionTests::SphereAndSphere( const CollisionSphere& a, const CollisionSphere& b )
{
	v3df midline = a.GetAxis( 3u ) - b.GetAxis( 3u );
	return midline.squareMagnitude() < ( a._radius + b._radius ) * ( a._radius + b._radius );
}

static inline float TransformToAxis( const CollisionCube& box, const v3df& axis )
{
	return box._halfSize.x * fabsf( axis * box.GetAxis( 0u ) ) +
		   box._halfSize.y * fabsf( axis * box.GetAxis( 1u ) ) +
		   box._halfSize.z * fabsf( axis * box.GetAxis( 2u ) );
}

static inline bool OverlapOnAxis( const CollisionCube& a, const CollisionCube& b, const v3df& axis, const v3df& centre )
{
	float aProject = TransformToAxis( a, axis );
	float bProject = TransformToAxis( b, axis );
	float distance = fabsf( centre * axis );
	return ( distance < aProject + bProject );
}

bool IntersectionTests::CubeAndCube( const CollisionCube& a, const CollisionCube& b )
{
	v3df toCentre = a.GetAxis( 3u ) - a.GetAxis( 3u );

	return (
		// collision cube a
		OverlapOnAxis( a, b, a.GetAxis( 0u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2u ), toCentre ) &&

		// collision cube b
		OverlapOnAxis( a, b, b.GetAxis( 0u ), toCentre ) &&
		OverlapOnAxis( a, b, b.GetAxis( 1u ), toCentre ) &&
		OverlapOnAxis( a, b, b.GetAxis( 2u ), toCentre ) &&

		// cross products
		OverlapOnAxis( a, b, a.GetAxis( 0u ) % b.GetAxis( 0u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 0u ) % b.GetAxis( 1u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 0u ) % b.GetAxis( 2u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1u ) % b.GetAxis( 0u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1u ) % b.GetAxis( 1u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1u ) % b.GetAxis( 2u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2u ) % b.GetAxis( 0u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2u ) % b.GetAxis( 1u ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2u ) % b.GetAxis( 2u ), toCentre )
	);
}

bool IntersectionTests::CubeAndHalfSpace( const CollisionCube& box, const CollisionPlane& plane )
{
	float projectedRadius = TransformToAxis( box, plane._direction );
	float boxDistance = plane._direction * box.GetAxis( 3u ) - projectedRadius;
	return boxDistance <= plane._offset;
}