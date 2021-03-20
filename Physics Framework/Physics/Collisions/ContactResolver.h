#pragma once
#include "Contact.h"

#ifndef CONTACTRESOLVER_H
#define CONTACTRESOLVER_H

class ContactResolver
{
	friend class Contact;
public:
	ContactResolver( uint32_t iterations, float velocityEps = 0.01f, float positionEps = 0.01f );
	ContactResolver( uint32_t velocityIt, uint32_t positionIt, float positionEp = 0.01f, float velocityEp = 0.01f );

	bool IsValid();
	void SetIterations( uint32_t velocityIt, uint32_t positionIt );
	void SetIterations( uint32_t iterations );
	void SetEpsilon( float velocityEp, float positionEp );
	void ResolveContacts( Contact* contacts, uint32_t numContacts, float dt );
protected:
	void AdjustVelocities( Contact* contacts, uint32_t numContacts, float dt );
	void AdjustPositions( Contact* contacts, uint32_t numContacts, float dt );
	void PrepareContacts( Contact* contacts, uint32_t numContacts, float dt );
public:
	uint32_t velocityIterationsUsed;
	uint32_t positionIterationsUsed;
protected:
	uint32_t velocityIterations;
	uint32_t positionIterations;
	float velocityEpsilon = 0.01;
	float positionEpsilon = 0.01;
private:
	bool validSettings;
};

#endif