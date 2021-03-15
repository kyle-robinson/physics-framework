#include "stdafx.h"
#include "IntersectionTests.h"

static inline float TransfromToAxis(
	const Box& box, const v3df& axis ) {
	return
		box._halfSize.x * fabsf( axis * box.GetAxis( 0 ) ) +
		box._halfSize.y * fabsf( axis * box.GetAxis( 1 ) ) +
		box._halfSize.z * fabsf( axis * box.GetAxis( 2 ) );
}

bool IntersectionTests::SphereAndHalfSpace( const Sphere& sphere, const CollisionPlane& plane )
{
	float ballDistance = plane._direction * sphere.GetAxis( 3 ) - sphere._radius;

	return ballDistance <= plane._offset;
}

bool IntersectionTests::SphereAndSphere( const Sphere& a, const Sphere& b )
{
	v3df midline = a.GetAxis( 3 ) - b.GetAxis( 3 );

	return midline.squareMagnitude() < ( a._radius + b._radius ) * ( a._radius + b._radius );
}


static inline bool OverlapOnAxis( const Box& a, const Box& b, const v3df& axis, const v3df& centre ) {
	float aProject = TransfromToAxis( a, axis );
	float bProject = TransfromToAxis( b, axis );

	float distance = fabsf( centre * axis );

	return( distance < aProject + bProject );
}

bool IntersectionTests::BoxAndBox( const Box& a, const Box& b )
{
	v3df toCentre = a.GetAxis( 3 ) - a.GetAxis( 3 );

	return (
		//Box A
		OverlapOnAxis( a, b, a.GetAxis( 0 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2 ), toCentre ) &&
		//Box B
		OverlapOnAxis( a, b, b.GetAxis( 0 ), toCentre ) &&
		OverlapOnAxis( a, b, b.GetAxis( 1 ), toCentre ) &&
		OverlapOnAxis( a, b, b.GetAxis( 2 ), toCentre ) &&

		//Cross products
		OverlapOnAxis( a, b, a.GetAxis( 0 ) % b.GetAxis( 0 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 0 ) % b.GetAxis( 1 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 0 ) % b.GetAxis( 2 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1 ) % b.GetAxis( 0 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1 ) % b.GetAxis( 1 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 1 ) % b.GetAxis( 2 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2 ) % b.GetAxis( 0 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2 ) % b.GetAxis( 1 ), toCentre ) &&
		OverlapOnAxis( a, b, a.GetAxis( 2 ) % b.GetAxis( 2 ), toCentre )
		);
}

bool IntersectionTests::BoxAndHalfSpace( const Box& box, const CollisionPlane& plane )
{
	float projectedRadius = TransfromToAxis( box, plane._direction );

	float boxDistance = plane._direction * box.GetAxis( 3 ) - projectedRadius;

	return boxDistance <= plane._offset;
}