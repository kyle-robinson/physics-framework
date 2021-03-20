#include "stdafx.h"
#include "RigidBody.h"

RigidBody::RigidBody( std::shared_ptr<Transform> transform ) : ParticleModel( transform )
{
	_mass = 5.0f;
	_inverseMass = 1.0f / 5.0f;

	_drag = 0.95f;
	_angularDamping = 0.8f;

	_canRest = true;
	_restEpsilon = 0.1f;

	Matrix3 tensor;
	float coeff = 0.4f * GetMass() * 1.0f * 1.0f;
	tensor.SetInertiaTensorCoeffs( coeff, coeff, coeff );
	tensor.SetBlockInertiaTensor( v3df( 1.0f, 1.0f, 1.0f ), 5.0f );
	SetInertiaTensor( tensor );
	ResetForces();
	CalculateDerivedData();
}

void RigidBody::CalculateDerivedData()
{
	_orientation.normalise();
	CalculateTransformMatrix( _transformMatrix, _position, _orientation );
	CalculateInertiaTensor( _inverseInertiaTensorWorld, _orientation, _inverseInertiaTensor, _transformMatrix );
}

void RigidBody::Update( const float dt )
{
	if ( !_isActive ) return;

	// calculate linear acceleration
	_previousAcceleration = _acceleration;
	_previousAcceleration.AddScaledVector( _netForce, _inverseMass );

	// calculate angular acceleration
	v3df angularAcceleration = _inverseInertiaTensorWorld.Transform( _torque );

	// calculate velocity/rotation
	_velocity.AddScaledVector( _previousAcceleration, dt );
	_rotation.AddScaledVector( angularAcceleration, dt );

	// apply drag
	_velocity *= powf( _drag, dt );
	_rotation *= powf( _angularDamping, dt );
	
	// update position/orientation
	_position.AddScaledVector( _velocity, dt );
	_orientation.AddScaledVector( _rotation, dt );
	CalculateDerivedData();
	ResetForces();

	if ( _canRest )
	{
		float currentMotion = _velocity.ScalarProduct( _velocity ) + _rotation.ScalarProduct( _rotation );
		float bias = powf( 0.5f, dt );

		_motion = bias * _motion + ( 1.0f - bias ) * currentMotion;
		if ( _motion < _restEpsilon )
			SetActive( false );
		else if ( _motion > 10.0f * _restEpsilon )
			_motion = 10.0f * _restEpsilon;
	}
}

#pragma region Inertia_Tensor
void RigidBody::SetInertiaTensor( const Matrix3& inertiaTensor ) noexcept
{
	_inverseInertiaTensor.SetInverse( inertiaTensor );
}

void RigidBody::GetInertiaTensor( Matrix3* inertiaTensor ) const
{
	inertiaTensor->SetInverse( _inverseInertiaTensorWorld );
}

Matrix3 RigidBody::GetInertiaTensor() const
{
	Matrix3 it;
	GetInertiaTensor( &it );
	return it;
}

void RigidBody::SetInverseInertiaTensor( const Matrix3& inverseIntertiaTensor )
{
	_inverseInertiaTensor = inverseIntertiaTensor;
}

void RigidBody::GetInverseInertiaTensor( Matrix3* inverseInertiaTensor ) const
{
	*inverseInertiaTensor = _inverseInertiaTensor;
}

void RigidBody::GetInverseInertiaTensorWorld( Matrix3* inverseInertiaTensor ) const
{
	*inverseInertiaTensor = _inverseInertiaTensorWorld;
}
#pragma endregion

#pragma region Angular_Damping
void RigidBody::SetDamping( const float linearDamping, const float angularDamping )
{
	_angularDamping = angularDamping;
	_drag = linearDamping;
}

void RigidBody::SetLinearDamping( const float linearDamping )
{
	_drag = linearDamping;
}

void RigidBody::SetAngularDamping( const float angularDamping )
{
	_angularDamping = angularDamping;
}
#pragma endregion

#pragma region Orientation
void RigidBody::SetOrientation( const Quaternion& orientation )
{
	_orientation = orientation;
	_orientation.normalise();
}

void RigidBody::SetOrientation( const float r, const float i, const float j, const float k )
{
	_orientation.r = r;
	_orientation.i = i;
	_orientation.j = j;
	_orientation.k = k;
}

void RigidBody::GetOrientation( Quaternion* orientation ) const
{
	*orientation = _orientation;
}

void RigidBody::GetOrientation( Matrix3* matrix ) const
{
	GetOrientation( matrix->_data );
}

void RigidBody::GetOrientation( float matrix[9] ) const
{
	matrix[0] = _transformMatrix._data[0];
	matrix[1] = _transformMatrix._data[1];
	matrix[2] = _transformMatrix._data[2];

	matrix[3] = _transformMatrix._data[4];
	matrix[4] = _transformMatrix._data[5];
	matrix[5] = _transformMatrix._data[6];

	matrix[6] = _transformMatrix._data[8];
	matrix[7] = _transformMatrix._data[9];
	matrix[8] = _transformMatrix._data[10];
}
#pragma endregion

#pragma region Transform
void RigidBody::GetTransformMat( Matrix4* transform ) const
{
	memcpy( transform, &_transformMatrix._data, sizeof( float ) * 12 );
}

void RigidBody::GetTransformMat( float matrix[16] ) const
{
	memcpy( matrix, _transformMatrix._data, sizeof( float ) * 12 );
	matrix[12] = matrix[13] = matrix[14] = 0;
	matrix[15] = 1;
}

v3df RigidBody::GetPointInWorldSpace( const v3df& point ) const
{
	return _transformMatrix.Transform( point );
}

v3df RigidBody::GetDirectionInWorldSpace( const v3df& point ) const
{
	return _transformMatrix.TransformDirection( point );
}

v3df RigidBody::GetDirectionInLocalSpace( const v3df& point ) const
{
	return _transformMatrix.TransformInverDirection( point );
}
#pragma endregion

#pragma region Rotation
void RigidBody::SetRotation( const v3df& rotation )
{
	_rotation = rotation;
	_orientation.i = rotation.x;
	_orientation.j = rotation.y;
	_orientation.k = rotation.z;
}

void RigidBody::SetRotation( const float x, const float y, const float z )
{
	_rotation.x = x;
	_rotation.y = y;
	_rotation.z = z;
	_orientation.i = x;
	_orientation.j = y;
	_orientation.k = z;
}

void RigidBody::AddRotation( const v3df& deltaRotation )
{
	_rotation += deltaRotation;
}
#pragma endregion

#pragma region Sleeping
void RigidBody::SetActive( const bool active )
{
	if ( active )
	{
		_isActive = true;
		_motion = _restEpsilon * 2.0f;
	}
	else
	{
		_isActive = false;
		_velocity = { 0.0f, 0.0f, 0.0f };
		_rotation = { 0.0f, 0.0f, 0.0f };
	}
}

void RigidBody::SetCanRest( const bool canSleep )
{
	_canRest = canSleep;
	if ( !canSleep && !_isActive )
		SetActive();
}
#pragma endregion

#pragma region Forces
void RigidBody::AddVelocity( const v3df& deltaVelocity )
{
	v3df velocity = _velocity;
	velocity += deltaVelocity;
	_velocity = velocity;
}

void RigidBody::AddTorque( const v3df& torque )
{
	_torque += torque;
	_isActive = true;
}

void RigidBody::AddForce( const v3df& force )
{
	ParticleModel::AddForce( force );
	_isActive = true;
}

void RigidBody::AddForceAtPoint( const v3df& force, const v3df& point )
{
	v3df pt = point;
	v3df newForce = _netForce + force;
	newForce += pt % force;
	_netForce = newForce;
	_isActive = true;
}

void RigidBody::AddForceAtBodyPoint( const v3df& force, const v3df& point )
{
	v3df pt = GetPointInWorldSpace( point );
	AddForceAtPoint( force, pt );
	_isActive = true;
}


void RigidBody::ResetForces()
{
	ParticleModel::ResetForces();
	_torque = { 0.0f, 0.0f, 0.0f };
}
#pragma endregion