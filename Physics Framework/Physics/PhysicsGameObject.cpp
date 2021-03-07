#include "stdafx.h"
#include "PhysicsGameObject.h"

PhysicsGameObject::PhysicsGameObject( const std::string& id ) : GameObject( id ) { }

PhysicsGameObject::~PhysicsGameObject() { }

void PhysicsGameObject::Update( const float dt )
{
	GameObject::Update( dt );
	GetRigidBody()->Update( dt );
	//UpdateWorldSpaceBoundingBox();
}

void PhysicsGameObject::InitializeInertiaTensor()
{
	//BoundingBox::CreateFromPoints( boundingBox, GetAppearance()->GetGeometryData().numberOfVertices,
	//	&( GetAppearance()->GetGeometryData().vertices[0] ), sizeof( XMFLOAT3 ) );
	GetRigidBody()->ComputeBoxInertiaTensor( boundingBox.Extents.x * 2.0f, boundingBox.Extents.y * 2.0f,
		boundingBox.Extents.z * 2.0f );
}

void PhysicsGameObject::UpdateWorldSpaceBoundingBox()
{
	BoundingBox modelSpaceBoundingBox = boundingBox;
	worldSpaceBoundingBox.Extents = modelSpaceBoundingBox.Extents;
	worldSpaceBoundingBox.Center = XMFLOAT3( GetTransform()->GetPosition().x,
		GetTransform()->GetPosition().y, GetTransform()->GetPosition().z );
	XMStoreFloat4( &worldSpaceBoundingBox.Orientation, GetTransform()->GetOrientation() );
}