#pragma once
#ifndef PHYSICSGAMEOBJECT_H
#define PHYSICSGAMEOBJECT_H

#include "GameObject.h"
#include <DirectXCollision.h>

class PhysicsGameObject : public GameObject
{
public:
	PhysicsGameObject( const std::string& id );
	~PhysicsGameObject();

	void Update( const float dt ) override;
	void UpdateWorldSpaceBoundingBox();
	void InitializeInertiaTensor();

	BoundingBox GetBoundingbox() const noexcept { return boundingBox; }
	BoundingOrientedBox GetWorldSpaceBoundingBox() const noexcept { return worldSpaceBoundingBox; }
private:
	BoundingBox boundingBox;
	BoundingOrientedBox worldSpaceBoundingBox;
};

#endif