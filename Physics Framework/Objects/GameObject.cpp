#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& id ) : _id( id )
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();

	//_particleModel = std::make_shared<ParticleModel>( _transform );
	_rigidBody = std::make_shared<RigidBody>( _transform );

	_rigidBody->SetMass( 5.0f );
	_rigidBody->SetInverseMass( 5.0f );
	_rigidBody->SetOrientation( 1.0f, 0.0f, 0.0f, 0.0f );
	_rigidBody->SetRotation( 0.0f, 0.0f, 0.0f );
	_rigidBody->SetCanSleep( true );
	_rigidBody->SetAwake( false );
	_rigidBody->SetAngularDamping( 0.8f );
	_rigidBody->SetLinearDamping( 0.95f );
	_rigidBody->SetVelocity( 0.0f, 0.0f, 0.0f );
	_rigidBody->SetAcceleration( 0.0f, -10.0f, 0.0f );

	Matrix3 tensor;

	float coeff = 0.4f * _rigidBody->GetMass() * 1.0f * 1.0f;
	tensor.SetInertiaTensorCoeffs( coeff, coeff, coeff );
	tensor.SetBlockInertiaTensor( v3df( 1.0f, 1.0f, 1.0f ), 5.0f );
	_rigidBody->SetInertiaTensor( tensor );

	_rigidBody->ResetForces();
	_rigidBody->CalculateDerivedData();
}

void GameObject::UpdateTransforms()
{
	_transform->SetPosition( _rigidBody->GetPosition() );
	_transform->SetRotation( _rigidBody->GetOrientation().identity() );

	float transform[16];
	_rigidBody->GetTransformMat().DirectXArray( transform );

	_transform->SetTransform( XMFLOAT4X4( transform ) );

	//Updates transform
	_transform->Update();
}

void GameObject::Update( const float dt )
{
	//_rigidBody->GetTransform()->SetPosition( _transform->GetPosition() );
	_rigidBody->SetPosition( _transform->GetPosition() );
	
	//_particleModel->Update( dt );
	_rigidBody->Update( dt );
	//_transform->Update();
	//if ( _transform->GetParent() != nullptr )
	//		XMStoreFloat4x4( &_transform->GetWorldMatrixFloat4x4(),
	//			_transform->GetWorldMatrix() * _transform->GetParent()->GetTransform()->GetWorldMatrix() );
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	_appearance->Update( pImmediateContext );
}