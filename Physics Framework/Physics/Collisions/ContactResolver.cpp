#include "stdafx.h"
#include "ContactResolver.h"

ContactResolver::ContactResolver( uint32_t iterations, float velocityEps, float positionEps )
{
	SetIterations( iterations );
	SetEpsilon( velocityEps, positionEps );
}

ContactResolver::ContactResolver( uint32_t velocityIt, uint32_t positionIt, float positionEp , float velocityEp  )
{
	SetIterations( positionIterations, velocityIterations );
	SetEpsilon( velocityEp, positionEp );
}

bool ContactResolver::IsValid()
{
	return ( velocityIterations > 0 ) &&
		   ( positionIterations > 0 ) &&
		   ( positionEpsilon >= 0.0f ) &&
		   ( velocityEpsilon >= 0.0f );
}

void ContactResolver::SetIterations( uint32_t velocityIt, uint32_t positionIt )
{
	positionIterations = positionIt;
	velocityIterations = velocityIt;
}

void ContactResolver::SetIterations( uint32_t iterations )
{
	positionIterations = iterations;
	velocityIterations = iterations;
}

void ContactResolver::SetEpsilon( float velocityEp, float positionEp )
{
	velocityEpsilon = velocityEp;
	positionEpsilon = positionEp;
}

void ContactResolver::ResolveContacts( Contact* contacts, uint32_t numContacts, float dt )
{
	if ( numContacts == 0 ) return;
	if ( !IsValid() ) return;

	PrepareContacts( contacts, numContacts, dt );
	AdjustPositions( contacts, numContacts, dt );
	AdjustVelocities( contacts, numContacts, dt );
}

void ContactResolver::AdjustVelocities( Contact* contacts, uint32_t numContacts, float dt )
{
	v3df velocityChange[2], rotationChange[2];
	velocityIterationsUsed = 0;
	v3df deltaVel;

	while ( velocityIterationsUsed < velocityIterations )
	{
		float max = velocityEpsilon;
		uint32_t index = numContacts;
		for ( uint32_t i = 0; i < numContacts; i++ )
		{
			if ( contacts[i]._desiredDeltaVelocity > max )
			{
				max = contacts[i]._desiredDeltaVelocity;
				index = i;
			}
		}

		if ( index == numContacts ) break;
		contacts[index].MatchAwakeState();
		contacts[index].ApplyVelocityChange( velocityChange, rotationChange );

		for ( uint32_t i = 0; i < numContacts; i++ )
		{
			// check each body in the contact
			for ( uint32_t b = 0; b < 2; b++ ) if ( contacts[i]._bodies[b] )
			{
				// check for a match with each body in the newly resolved contact
				for ( uint32_t d = 0; d < 2; d++ )
				{
					if ( contacts[i]._bodies[b] == contacts[index]._bodies[d] )
					{
						deltaVel = velocityChange[d] + rotationChange[d].VectorProduct(
								contacts[i]._relativeContactPosition[b] );

						// adjust for negative values if dealing with second body in contact
						contacts[i]._contactVelocity +=
							contacts[i]._contactToWorld.TransformTranspose( deltaVel ) * ( b ? -1 : 1 );
						contacts[i].CalculateDesiredDeltaVelocity( dt );
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}

void ContactResolver::AdjustPositions( Contact* contacts, uint32_t numContacts, float dt )
{
	float max;
	uint32_t i, index;
	v3df deltaPosition;
	v3df linearChange[2], angularChange[2];

	positionIterationsUsed = 0;
	while ( positionIterationsUsed < positionIterations ) {

		// find the largest penetration
		max = positionEpsilon;
		index = numContacts;
		for ( i = 0; i < numContacts; i++ )
		{
			if ( contacts[i]._penetration > max )
			{
				max = contacts[i]._penetration;
				index = i;
			}
		}

		if ( index == numContacts ) break;
		contacts[index].MatchAwakeState();
		contacts[index].ApplyPositionChange( linearChange, angularChange, max );

		for ( i = 0; i < numContacts; i++ )
		{
			// check each rigid body in the contact
			for ( uint32_t j = 0; j < 2; j++ )
			{
				if ( contacts[i]._bodies[j] )
				{
					// check for a match with body in new contact
					for ( uint32_t k = 0; k < 2; k++ )
					{
						if ( contacts[i]._bodies[j] == contacts[index]._bodies[k] )
						{
							deltaPosition = linearChange[k] +
								angularChange[k].VectorProduct(
									contacts[i]._relativeContactPosition[j] );

							// adjust for negative values if dealing with second body in contact
							contacts[i]._penetration +=
								deltaPosition.ScalarProduct( contacts[i]._contactNormal ) * ( j ? 1 : -1 );
						}
					}
				}
			}
		}
		positionIterationsUsed++;
	}
}

void ContactResolver::PrepareContacts( Contact* contacts, uint32_t numContacts, float dt )
{
	Contact* lastContact = contacts + numContacts;
	for ( Contact* contact = contacts; contact < lastContact; contact++ )
		contact->CalculateInternals( dt );
}