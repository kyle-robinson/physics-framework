#include "stdafx.h"
#include "CollisionDetector.h"
#include "IntersectionTests.h"

#pragma region SphereCollisions
void CollisionDetector::SphereAndHalfSpace( const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data )
{
	if ( data->_contactsLeft <= 0 ) return;

	v3df position = sphere.GetAxis( 3u );
	float ballDistance = plane._direction * position - sphere._radius - plane._offset;
	if ( ballDistance >= 0.0f ) return;

	Contact* contact = data->_contacts;
	contact->_contactNormal = plane._direction;
	contact->_penetration = -ballDistance;
	contact->_contactPoint = position - plane._direction * ( ballDistance + sphere._radius );
	contact->SetBodyData( sphere._body.get(), NULL, data->_friction, data->_restitution );
	data->AddContacts( 1u );
}

void CollisionDetector::SphereAndTruePlane( const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data )
{
	if ( data->_contactsLeft <= 0 ) return;

	v3df position = sphere.GetAxis( 3u );
	float centreDisance = plane._direction * position - plane._offset;
	if ( centreDisance * centreDisance > sphere._radius * sphere._radius ) return;
	v3df normal = plane._direction;

	float penetration = -centreDisance;
	if ( centreDisance < 0.0f )
	{
		normal *= -1.0f;
		penetration = -penetration;
	}
	penetration += sphere._radius;

	Contact* contact = data->_contacts;
	contact->_contactNormal = normal;
	contact->_penetration = penetration;
	contact->_contactPoint = position - plane._direction * centreDisance;
	contact->SetBodyData( sphere._body.get(), NULL, data->_friction, data->_restitution );
	data->AddContacts( 1u );
}

void CollisionDetector::SphereAndSphere( const CollisionSphere& a, const CollisionSphere& b, CollisionData* data )
{
	if ( data->_contactsLeft <= 0 ) return;

	v3df positionA = a.GetAxis( 3u );
	v3df poistionB = b.GetAxis( 3u );

	v3df midLine = positionA - poistionB;
	float size = midLine.magnitude();

	if ( size <= 0.0f || size >= a._radius + b._radius ) return;
	v3df normal = midLine * ( 1.0f / size );

	Contact* contact = data->_contacts;
	contact->_contactNormal = normal;
	contact->_contactPoint = positionA + midLine * 0.5f;
	contact->_penetration = ( a._radius + b._radius - size );
	contact->SetBodyData( a._body.get(), b._body.get(), data->_friction, data->_restitution );
	data->AddContacts( 1u );
}
#pragma endregion

#pragma region BoxCollisions
uint32_t CollisionDetector::CubeAndHalfSpace( const CollisionCube& box, const CollisionPlane& plane, CollisionData* data )
{
	if ( data->_contactsLeft <= 0 ) return 0u;
	if ( !IntersectionTests::CubeAndHalfSpace( box, plane ) ) return 0u;

	// vertex combinations needed to check for contact
	static float mults[8][3] = {
		{ 1, 1, 1 }, { -1, 1, 1 }, { 1, -1, 1 }, { -1, -1, 1 },
		{ 1, 1,-1 }, { -1, 1,-1 }, { 1, -1,-1 }, { -1, -1,-1 }
	};

	Contact* contact = data->_contacts;
	uint32_t contactsUsed = 0;
	for ( uint32_t i = 0; i < 8; i++ )
	{
		v3df vertexPos( mults[i][0], mults[i][1], mults[i][2] );
		vertexPos.ComponentProductUpdate( box._halfSize );
		vertexPos = box.GetTransform().Transform( vertexPos );

		float vertexDistance = vertexPos * plane._direction;
		if ( vertexDistance <= plane._offset )
		{
			contact->_contactPoint = plane._direction;
			contact->_contactPoint *= ( vertexDistance - plane._offset );
			contact->_contactPoint += vertexPos;
			contact->_contactNormal = plane._direction;
			contact->_penetration = plane._offset - vertexDistance;
			contact->SetBodyData( box._body.get(), NULL, data->_friction, data->_restitution );

			contact++;
			contactsUsed++;
			if ( contactsUsed == static_cast<uint32_t>( data->_contactsLeft ) )
				return contactsUsed;
		}
	}
	data->AddContacts( contactsUsed );
	return contactsUsed;
}

void FillPointFaceBoxBox( const CollisionCube& a, const CollisionCube& b, const v3df& centre, CollisionData* data, uint32_t best, float pen )
{
	Contact* contact = data->_contacts;

	v3df normal = a.GetAxis( best );
	if ( a.GetAxis( best ) * centre > 0.0f )
		normal = normal * -1.0f;

	v3df vertex = b._halfSize;
	if ( b.GetAxis( 0u ) * normal < 0.0f ) vertex.x = -vertex.x;
	if ( b.GetAxis( 1u ) * normal < 0.0f ) vertex.y = -vertex.y;
	if ( b.GetAxis( 2u ) * normal < 0.0f ) vertex.z = -vertex.z;

	contact->_contactNormal = normal;
	contact->_penetration = pen;
	contact->_contactPoint = b.GetTransform() * vertex;
	contact->SetBodyData( a._body.get(), b._body.get(), data->_friction, data->_restitution );
}

static inline v3df ContactPoint( const v3df& pOne, const v3df& dOne, float oneSize, const v3df& pTwo, const v3df& dTwo, float twoSize, bool useOne )
{
	float smOne = dOne.squareMagnitude();
	float smTwo = dTwo.squareMagnitude();
	float dpOneTwo = dTwo * dOne;

	v3df toSt = pOne - pTwo;
	float dpStaOne = dOne * toSt;
	float dpStaTwo = dTwo * toSt;
	float denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// denominator of zero == parallel lines
	if ( fabsf( denom ) < 0.0001f ) return useOne ? pOne : pTwo;
	float mua = ( dpOneTwo * dpStaTwo - smTwo * dpStaOne ) / denom;
	float mub = ( smOne * dpStaTwo - dpOneTwo * dpStaOne ) / denom;

	// check for edge-edge / edge-face contact
	if ( mua >  oneSize ||
		 mua < -oneSize ||
		 mub >  twoSize ||
		 mub < -twoSize )
	{
		return useOne ? pOne : pTwo;
	}
	else
	{
		v3df cOne = pOne + dOne * mua;
		v3df cTwo = pTwo + dTwo * mub;
		return cOne * 0.5f + cTwo * 0.5f;
	}
}

static inline float TransformToAxis( const CollisionCube& box, const v3df& axis )
{
	return box._halfSize.x * fabsf( axis * box.GetAxis( 0u ) ) +
		   box._halfSize.y * fabsf( axis * box.GetAxis( 1u ) ) +
		   box._halfSize.z * fabsf( axis * box.GetAxis( 2u ) );
}

static inline float PenetrationOnAxis( const CollisionCube& a, const CollisionCube& b, const v3df& axis, const v3df& centre )
{
	float aProject = TransformToAxis( a, axis );
	float bProject = TransformToAxis( b, axis );
	float distance = fabsf( centre * axis );
	return aProject + bProject - distance;
}

static inline bool TryAxis( const CollisionCube& a, const CollisionCube& b, v3df axis, const v3df& centre, uint32_t index, float& smallestPenetration, uint32_t& smallestCase )
{
	if ( axis.squareMagnitude() < 0.0001f ) return true;
	axis.normalize();

	float penetration = PenetrationOnAxis( a, b, axis, centre );
	if ( penetration < 0.0f ) return false;
	if ( penetration < smallestPenetration )
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}

	return true;
}

void CollisionDetector::CubeAndCube( const CollisionCube& a, const CollisionCube& b, CollisionData* data )
{
	float pen = FLT_MAX;
	uint32_t best = 0xffffff;
	v3df toCentre = b.GetAxis( 3u ) - a.GetAxis( 3u );

	if ( !TryAxis( a, b, a.GetAxis( 0u ), toCentre, 0u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 1u ), toCentre, 1u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 2u ), toCentre, 2u, pen, best ) ) return;

	if ( !TryAxis( a, b, b.GetAxis( 0u ), toCentre, 3u, pen, best ) ) return;
	if ( !TryAxis( a, b, b.GetAxis( 1u ), toCentre, 4u, pen, best ) ) return;
	if ( !TryAxis( a, b, b.GetAxis( 2u ), toCentre, 5u, pen, best ) ) return;

	uint32_t bestSingleAxis = best;
	if ( !TryAxis( a, b, a.GetAxis( 0u ) % b.GetAxis( 0u ), toCentre, 6u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 0u ) % b.GetAxis( 1u ), toCentre, 7u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 0u ) % b.GetAxis( 2u ), toCentre, 8u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 1u ) % b.GetAxis( 0u ), toCentre, 9u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 1u ) % b.GetAxis( 1u ), toCentre, 10u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 1u ) % b.GetAxis( 2u ), toCentre, 11u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 2u ) % b.GetAxis( 0u ), toCentre, 12u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 2u ) % b.GetAxis( 1u ), toCentre, 13u, pen, best ) ) return;
	if ( !TryAxis( a, b, a.GetAxis( 2u ) % b.GetAxis( 2u ), toCentre, 14u, pen, best ) ) return;
	assert( best != 0xffffff );

	if ( best < 3u )
	{
		FillPointFaceBoxBox( a, b, toCentre, data, best, pen );
		data->AddContacts( 1u );
	}
	else if ( best < 6u )
	{
		FillPointFaceBoxBox( a, b, toCentre * -1.0f, data, best - 3u, pen );
		data->AddContacts( 1u );
	}
	else
	{
		best -= 6u;
		uint32_t aAxisIndex = best / 3u;
		uint32_t bAxisIndex = best % 3u;
		v3df aAxis = a.GetAxis( aAxisIndex );
		v3df bAxis = b.GetAxis( bAxisIndex );
		v3df axis = aAxis % bAxis;
		axis.normalize();

		if ( axis * toCentre > 0.0f ) axis = axis * -1.0f;
		v3df ptOnOneEdge = a._halfSize;
		v3df ptOnTwoEdge = b._halfSize;

		for ( uint32_t i = 0u; i < 3u; i++ )
		{
			if ( i == aAxisIndex )
				ptOnOneEdge[i] = 0.0f;
			else if ( a.GetAxis( i ) * axis > 0.0f )
				ptOnOneEdge[i] = -ptOnOneEdge[i];

			if ( i == bAxisIndex )
				ptOnTwoEdge[i] = 0.0f;
			else if ( b.GetAxis( i ) * axis < 0.0f )
				ptOnTwoEdge[i] = -ptOnTwoEdge[i];
		}

		// convert to world coordinates
		ptOnOneEdge = a.GetTransform() * ptOnOneEdge;
		ptOnTwoEdge = b.GetTransform() * ptOnTwoEdge;

		// find closet point of two approaching edges
		v3df vertex = ContactPoint(
			ptOnOneEdge, aAxis, a._halfSize[aAxisIndex],
			ptOnTwoEdge, bAxis, b._halfSize[bAxisIndex],
			bestSingleAxis > 2u
		);

		Contact* contact = data->_contacts;
		contact->_penetration = pen;
		contact->_contactNormal = axis;
		contact->_contactPoint = vertex;
		contact->SetBodyData( a._body.get(), b._body.get(), data->_friction, data->_restitution );
		data->AddContacts( 1u );
	}
}

void CollisionDetector::CubeAndPoint( const CollisionCube& box, const v3df& point, CollisionData* data )
{
	// convert point to cube coordinates
	v3df relPt = box.GetTransform().TransformInverse( point );

	// check axes for most shallow penetration
	float min_depth = box._halfSize.x - fabsf( relPt.x ); // x collision
	if ( min_depth < 0.0f ) return;
	v3df normal = box.GetAxis( 0u ) * ( ( relPt.x < 0.0f ) ? -1.0f : 1.0f );

	float depth = box._halfSize.y - fabsf( relPt.y ); // y collision
	if ( depth < 0.0f ) return;
	else if ( depth < min_depth )
	{
		min_depth = depth;
		normal = box.GetAxis( 1u ) * ( ( relPt.y < 0.0f ) ? -1.0f : 1.0f );
	}

	depth = box._halfSize.z - fabsf( relPt.z ); // z collision
	if ( depth < 0.0f ) return;
	else if ( depth < min_depth )
	{
		min_depth = depth;
		normal = box.GetAxis( 2u ) * ( ( relPt.z < 0.0f ) ? -1.0f : 1.0f );
	}

	Contact* contact = data->_contacts;
	contact->_contactNormal = normal;
	contact->_contactPoint = point;
	contact->_penetration = min_depth;
	contact->SetBodyData( box._body.get(), NULL, data->_friction, data->_restitution );
	data->AddContacts( 1u );
}

void CollisionDetector::CubeAndSphere( const CollisionCube& box, const CollisionSphere& sphere, CollisionData* data )
{
	// transform sphere into cube coordinates
	v3df centre = sphere.GetAxis( 3u );
	v3df relCentre = box.GetTransform().TransformInverse( centre );

	// if not in contact, return
	if ( fabsf( relCentre.x ) - sphere._radius > box._halfSize.x ||
		 fabsf( relCentre.y ) - sphere._radius > box._halfSize.y ||
		 fabsf( relCentre.z ) - sphere._radius > box._halfSize.z )
	{
		return;
	}

	v3df closestPt = { 0.0f, 0.0f, 0.0f };
	// clamp each coordinate to the cube
	float dist = relCentre.x; // x collision
	if ( dist > box._halfSize.x ) dist = box._halfSize.x;
	if ( dist < -box._halfSize.x ) dist = -box._halfSize.x;
	closestPt.x = dist;

	dist = relCentre.y; // y collision
	if ( dist > box._halfSize.y ) dist = box._halfSize.y;
	if ( dist < -box._halfSize.y ) dist = -box._halfSize.y;
	closestPt.y = dist;

	dist = relCentre.z; // z collision
	if ( dist > box._halfSize.z ) dist = box._halfSize.z;
	if ( dist < -box._halfSize.z ) dist = -box._halfSize.z;
	closestPt.z = dist;

	// check to see if cube and sphere are in contact
	dist = ( closestPt - relCentre ).squareMagnitude();
	if ( dist > sphere._radius * sphere._radius ) return;
	v3df closestPtWorld = box.GetTransform().Transform( closestPt );

	Contact* contact = data->_contacts;
	contact->_contactNormal = ( closestPtWorld - centre );
	contact->_contactNormal.normalize();
	contact->_contactPoint = closestPtWorld;
	contact->_penetration = sphere._radius - sqrtf( dist );
	contact->SetBodyData( box._body.get(), sphere._body.get(), data->_friction, data->_restitution );
	data->AddContacts( 1u );
}
#pragma endregion