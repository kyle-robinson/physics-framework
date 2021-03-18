#pragma once
#ifndef COLLISIONDATA_H
#define COLLISIONDATA_H

#include "Contact.h"

struct CollisionData
{
	Contact* _contacts;
	Contact* _contactArray;

	int _contactsLeft;

	unsigned _contactCount;

	float _friction;
	float _restitution;
	float _tolerance;

	bool HasMoreContacts() {
		return _contactsLeft > 0;
	}

	void Reset( unsigned maxContacts ) {
		_contactsLeft = maxContacts;
		_contactCount = 0;
		_contacts = _contactArray;
	}

	void AddContacts( unsigned count ) {
		_contactsLeft -= count;
		_contactCount += count;

		_contacts += count;
	}
};

#endif