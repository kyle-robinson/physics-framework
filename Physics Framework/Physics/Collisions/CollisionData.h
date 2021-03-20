#pragma once
#ifndef COLLISIONDATA_H
#define COLLISIONDATA_H

#include "Contact.h"

struct CollisionData
{
	// variables
	Contact* _contacts;
	Contact* _contactArray;
	uint32_t _contactCount;
	int _contactsLeft;

	float _friction;
	float _restitution;
	float _tolerance;

	// functions
	bool HasMoreContacts() const noexcept { return _contactsLeft > 0; }
	void Reset( uint32_t maxContacts )
	{
		_contactsLeft = maxContacts;
		_contactCount = 0;
		_contacts = _contactArray;
	}
	void AddContacts( uint32_t count )
	{
		_contactsLeft -= count;
		_contactCount += count;
		_contacts += count;
	}
};

#endif