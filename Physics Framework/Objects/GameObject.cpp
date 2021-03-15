#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& id ) : _id( id )
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();

	_particleModel = std::make_shared<ParticleModel>( _transform );
	_rigidBody = std::make_shared<RigidBody>( _transform );

	_rigidBody->SetMass( 5.0 );
	_rigidBody->SetOrientation( 1, 0, 0, 0 );
	_rigidBody->SetRotation( 0.0, 0.0, 0.0 );
	_rigidBody->SetCanSleep( true );
	_rigidBody->SetAwake( false );
	_rigidBody->SetAngularDamping( 0.8f );
	_rigidBody->SetLinearDamping( 0.95f );
	_rigidBody->SetVelocity( 0, 0, 0 );
	_rigidBody->SetAcceleration( 0.0, -10.0, 0 );

	Matrix3 tensor;

	float coeff = 0.4 * _rigidBody->GetMass() * 1.0 * 1.0;
	tensor.SetInertiaTensorCoeffs( coeff, coeff, coeff );
	tensor.SetBlockInertiaTensor( v3df( 1.0, 1.0, 1.0 ), 5.0 );
	_rigidBody->SetInertiaTensor( tensor );

	_rigidBody->ResetForces();
	_rigidBody->CalculateDerivedData();
}

void GameObject::UpdateTransforms()
{
	_transform->SetPosition( _rigidBody->GetTransform()->GetPosition() );
	_transform->SetRotation( _rigidBody->GetOrientation().identity() );

	float transform[16];
	_rigidBody->GetTransformMat().DirectXArray( transform );

	_transform->SetRotationMatrixFloat4x4( XMFLOAT4X4( transform ) );

	//Updates transform
	_transform->Update();
}

void GameObject::Update( const float dt )
{
	_rigidBody->GetTransform()->SetPosition( _transform->GetPosition() );
	
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