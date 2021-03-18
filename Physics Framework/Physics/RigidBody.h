#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "ParticleModel.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"

class RigidBody : public ParticleModel
{
public:
	RigidBody( std::shared_ptr<Transform> transform );
	void CalculateDerivedData();
	void Update( const float dt ) override;

	// inertia tensor
	void SetInertiaTensor( const Matrix3& inertiaTensor ) noexcept;
	void GetInertiaTensor( Matrix3* inertiaTensor ) const;
	Matrix3 GetInertiaTensor() const;
	void SetInverseInertiaTensor( const Matrix3& inverseInertiaTensor );
	void GetInverseInertiaTensor( Matrix3* inverseInertiaTensor ) const;
	Matrix3 GetInverseInertiaTensor() const noexcept { return _inverseInertiaTensor; }
	void GetInverseInertiaTensorWorld( Matrix3* inverseInertiaTensor ) const;
	Matrix3 GetInverseInertiaTensorWorld() const noexcept { return _inverseInertiaTensorWorld; }

	// angular damping
	void SetDamping( const float linearDamping, const float angularDamping );
	void SetLinearDamping( const float linearDamping );
	void SetAngularDamping( const float angularDamping );
	float GetAngularDamping() const noexcept { return _angularDamping; }

	// orientation
	void SetOrientation( const Quaternion& orientation );
	void SetOrientation( const float r, const float i, const float j, const float k );
	Quaternion GetOrientation() const noexcept { return _orientation; }
	void GetOrientation( Quaternion* orientation ) const;
	void GetOrientation( Matrix3* matrix ) const;
	void GetOrientation( float matrix[9] ) const;

	// transform
	void GetTransformMat( Matrix4* transform ) const;
	void GetTransformMat( float matrix[16] ) const;
	Matrix4 GetTransformMat() const noexcept { return _transformMatrix; }
	v3df GetPointInWorldSpace( const v3df& point ) const;
	v3df GetDirectionInWorldSpace( const v3df& point ) const;
	v3df GetDirectionInLocalSpace( const v3df& point ) const;
	v3df GetLastFrameAcceleration() const noexcept { return _previousAcceleration; }

	// rotation
	void AddRotation( const v3df& rotation );
	v3df GetRotation() const noexcept { return _rotation; }
	void SetRotation( const v3df& rotation );
	void SetRotation( const float x, const float y, const float z );

	// sleeping
	void SetAwake( const bool awake = true );
	void SetCanSleep( const bool canSleep );
	bool GetAwake() const noexcept { return _isAwake; }
	void SetSleepEpsilon( float sleepEpsilon ) noexcept { _sleepEpsilon = sleepEpsilon; }
	float GetSleepEpsilon() const noexcept { return _sleepEpsilon; }

	// add forces
	void AddVelocity( const v3df& velocity );
	void AddTorque( const v3df& torque );

	void AddForce( const v3df& force );
	void AddForceAtPoint( const v3df& force, const v3df& point );
	void AddForceAtBodyPoint( const v3df& force, const v3df& point );

	void ResetForces() override;
private:
	float _motion;
	bool _isAwake;
	bool _canSleep;
	float _sleepEpsilon = 0.1f;
	v3df _previousAcceleration;
	
	v3df _torque;
	v3df _rotation;
	float _angularDamping;
	Quaternion _orientation;
	Matrix4 _transformMatrix;
	Matrix3 _inverseInertiaTensor;
	Matrix3 _inverseInertiaTensorWorld;
};

#endif