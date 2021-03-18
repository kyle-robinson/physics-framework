#pragma once
#include "Contact.h"

#ifndef CONTACTRESOLVER_H
#define CONTACTRESOLVER_H

class ContactResolver
{
	friend class Contact;
public:
	ContactResolver( unsigned iterations, float velocityEps = ( float )0.01, float positionEps = ( float )0.01 );
	ContactResolver( unsigned velocityIt, unsigned positionIt, float positionEp = ( float )0.01, float velocityEp = ( float )0.01 );

	bool IsValid();
	void SetIterations( unsigned velocityIt, unsigned positionIt );
	void SetIterations( unsigned iterations );
	void SetEpsilon( float velocityEp, float positionEp );
	void ResolveContacts( Contact* contacts, unsigned numContacts, float dt );

protected:
	void AdjustVelocities( Contact* contacts, unsigned numContacts, float dt );
	void AdjustPositions( Contact* contacts, unsigned numContacts, float dt );
	void PrepareContacts( Contact* contacts, unsigned numContacts, float dt );

public:
	unsigned velocityIterationsUsed;
	unsigned positionIterationsUsed;

protected:
	unsigned velocityIterations;
	unsigned positionIterations;

	float velocityEpsilon = 0.01;
	float positionEpsilon = 0.01;

private:
	bool validSettings;
};

#endif