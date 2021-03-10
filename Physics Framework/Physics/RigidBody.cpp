#include "stdafx.h"
#include "RigidBody.h"

#pragma region Quaternion_Calculations
static inline void CalculateTransformMatrix(
	Matrix4& transformMatrix, const v3df& position, const Quaternion& orientation )
{
	transformMatrix._data[0] = 1 - 2 * orientation.j * orientation.j - 2 * orientation.k * orientation.k;
	transformMatrix._data[1] = 2 * orientation.i * orientation.j - 2 * orientation.r * orientation.k;
	transformMatrix._data[2] = 2 * orientation.i * orientation.k + 2 * orientation.r * orientation.j;
	transformMatrix._data[3] = position.x;
	transformMatrix._data[4] = 2 * orientation.i * orientation.j + 2 * orientation.r * orientation.k;
	transformMatrix._data[5] = 1 - 2 * orientation.i * orientation.i - 2 * orientation.k * orientation.k;
	transformMatrix._data[6] = 2 * orientation.j * orientation.k - 2 * orientation.r * orientation.i;
	transformMatrix._data[7] = position.y;
	transformMatrix._data[8] = 2 * orientation.i * orientation.k - 2 * orientation.r * orientation.j;
	transformMatrix._data[9] = 2 * orientation.j * orientation.k + 2 * orientation.r * orientation.i;
	transformMatrix._data[10] = 1 - 2 * orientation.i * orientation.i - 2 * orientation.j * orientation.j;
	transformMatrix._data[11] = position.z;

}

static inline void CalculateInertiaTensor( Matrix3& iitWorld, const Quaternion& q, const Matrix3& iitBody, const Matrix4& rotmat ) {
	float t4 = rotmat._data[0] * iitBody._data[0] + rotmat._data[1] * iitBody._data[3] + rotmat._data[2] * iitBody._data[6];
	float t9 = rotmat._data[0] * iitBody._data[1] + rotmat._data[1] * iitBody._data[4] + rotmat._data[2] * iitBody._data[7];
	float t14 = rotmat._data[0] * iitBody._data[2] + rotmat._data[1] * iitBody._data[5] + rotmat._data[2] * iitBody._data[8];
	float t28 = rotmat._data[4] * iitBody._data[0] + rotmat._data[5] * iitBody._data[3] + rotmat._data[6] * iitBody._data[6];
	float t33 = rotmat._data[4] * iitBody._data[1] + rotmat._data[5] * iitBody._data[4] + rotmat._data[6] * iitBody._data[7];
	float t38 = rotmat._data[4] * iitBody._data[2] + rotmat._data[5] * iitBody._data[5] + rotmat._data[6] * iitBody._data[8];
	float t52 = rotmat._data[8] * iitBody._data[0] + rotmat._data[9] * iitBody._data[3] + rotmat._data[10] * iitBody._data[6];
	float t57 = rotmat._data[8] * iitBody._data[1] + rotmat._data[9] * iitBody._data[4] + rotmat._data[10] * iitBody._data[7];
	float t62 = rotmat._data[8] * iitBody._data[2] + rotmat._data[9] * iitBody._data[5] + rotmat._data[10] * iitBody._data[8];


	iitWorld._data[0] = t4 * rotmat._data[0] + t9 * rotmat._data[1] + t14 * rotmat._data[2];
	iitWorld._data[1] = t4 * rotmat._data[4] + t9 * rotmat._data[5] + t14 * rotmat._data[6];
	iitWorld._data[2] = t4 * rotmat._data[8] + t9 * rotmat._data[9] + t14 * rotmat._data[10];
	iitWorld._data[3] = t28 * rotmat._data[0] + t33 * rotmat._data[1] + t38 * rotmat._data[2];
	iitWorld._data[4] = t28 * rotmat._data[4] + t33 * rotmat._data[5] + t38 * rotmat._data[6];
	iitWorld._data[5] = t28 * rotmat._data[8] + t33 * rotmat._data[9] + t38 * rotmat._data[10];
	iitWorld._data[6] = t52 * rotmat._data[0] + t57 * rotmat._data[1] + t62 * rotmat._data[2];
	iitWorld._data[7] = t52 * rotmat._data[4] + t57 * rotmat._data[5] + t62 * rotmat._data[6];
	iitWorld._data[8] = t52 * rotmat._data[8] + t57 * rotmat._data[9] + t62 * rotmat._data[10];
}
#pragma endregion

void RigidBody::CalculateDerivedData()
{
	_orientation.normalise();
	CalculateTransformMatrix( _transformMatrix, GetTransform()->GetPosition(), _orientation );
	CalculateInertiaTensor( _inverseInertiaTensorWorld, _orientation, _inverseInertiaTensor, _transformMatrix );
}

void RigidBody::Update( const float dt )
{
	if ( !_isAwake ) return;

	//Calculate linear acceleration from the force inputs
	_previousAcceleration = GetAcceleration();
	_previousAcceleration.AddScaledVector( GetNetForce(), -GetMass() );

	//Calculate angular acceleration from torque forces
	v3df angularAcceleration = _inverseInertiaTensorWorld.Transform( _torque );

	//Calculate Velocity and rotation changes
	GetVelocity().AddScaledVector( _previousAcceleration, dt );
	_rotation.AddScaledVector( angularAcceleration, dt );

	//Applies damping to velocity a
	GetVelocity() *= powf( GetDragFactor(), dt );
	_rotation *= powf( _angularDamping, dt );

	//Updates position
	GetTransform()->GetPosition().AddScaledVector( GetVelocity(), dt );

	//Updates Orientation
	_orientation.AddScaledVector( _rotation, dt );

	//Normalize the orientation and update the transform matrix
	CalculateDerivedData();

	//Clear the force accumulators
	ResetForces();

	if ( _canSleep ) {
		float currentMotion = GetVelocity().ScalarProduct( GetVelocity() ) + _rotation.ScalarProduct( _rotation );
		float bias = powf( 0.5, dt );

		_motion = bias * _motion + ( 1 - bias ) * currentMotion;

		if ( _motion < _sleepEpsilon ) SetAwake( false );
		else if ( _motion > 10 * _sleepEpsilon ) _motion = 10 * _sleepEpsilon;
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
	SetDragFactor( linearDamping );
}

void RigidBody::SetLinearDamping( const float linearDamping )
{
	SetDragFactor( linearDamping );
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
void RigidBody::SetAwake( const bool awake )
{
	if ( awake )
	{
		_isAwake = true;
		_motion = _sleepEpsilon * 2.0f;
	}
	else
	{
		_isAwake = false;
		SetVelocity( 0.0f, 0.0f, 0.0f );
		_rotation = { 0.0f, 0.0f, 0.0f };
	}
}

void RigidBody::SetCanSleep( const bool canSleep )
{
	_canSleep = canSleep;
	if ( !canSleep && !_isAwake ) SetAwake();
}
#pragma endregion

#pragma region Forces
void RigidBody::AddVelocity( const v3df& deltaVelocity )
{
	v3df velocity = GetVelocity();
	velocity += deltaVelocity;
	SetVelocity( velocity );
}

void RigidBody::AddTorque( const v3df& torque )
{
	_torque += torque;
	_isAwake = true;
}

void RigidBody::AddForce( const v3df& force )
{
	ParticleModel::AddForce( force );
}

void RigidBody::AddForceAtPoint( const v3df& force, const v3df& point )
{
	v3df pt = point;

	v3df newForce = GetNetForce() + force;
	newForce += pt % force;
	SetNetForce( newForce );
}

void RigidBody::AddForceAtBodyPoint( const v3df& force, const v3df& point )
{
	v3df pt = GetPointInWorldSpace( point );
	AddForceAtPoint( force, pt );
}


void RigidBody::ResetForces()
{
	ParticleModel::ResetForces();
	_torque = { 0.0f, 0.0f, 0.0f };
}
#pragma endregion