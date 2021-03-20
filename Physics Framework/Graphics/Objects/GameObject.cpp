#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& id, bool useRigidBodies ) : _id( id )
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();

	if ( !useRigidBodies )
		_particleModel = std::make_shared<ParticleModel>( _transform );
	else
		_rigidBody = std::make_shared<RigidBody>( _transform );
}

void GameObject::UpdateTransforms()
{
	_transform->SetPosition( _rigidBody->GetPosition() );
	_transform->SetRotation( _rigidBody->GetOrientation().identity() );

	float transform[16];
	_rigidBody->GetTransformMat().DirectXArray( transform );
	_transform->SetTransform( XMFLOAT4X4( transform ) );
	_transform->Update( true );
}

void GameObject::Update( const float dt, bool useRigidBodies )
{
	if ( !useRigidBodies )
	{
		_particleModel->Update( dt );
		_transform->Update( false );
	}
	else
	{
		_rigidBody->SetPosition( _transform->GetPosition() );
		_rigidBody->Update( dt );
	}
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	_appearance->Update( pImmediateContext );
}