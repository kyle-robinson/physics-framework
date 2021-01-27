#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform )
	: _transform( transform ), _useConstAccel( false ), _velocity( 0.0f, 0.001f, 0.0f ), _acceleration( 0.0f, 0.001f, 0.0f ),
	_mass( 10.0f ), _netForce( 0.0f, 0.0f, 0.0f ), _force( 0.0f, 0.0f, 0.0f )
{}

void const ParticleModel::MoveForward()
{
	_force = { 0.0f, 0.0f, 0.0f };
	_force[2] = MOVEMENT_SPEED;
}

void const ParticleModel::MoveBackward()
{
	_force = { 0.0f, 0.0f, 0.0f };
	_force[2] = -MOVEMENT_SPEED;
}

void const ParticleModel::MoveLeft()
{
	_force = { 0.0f, 0.0f, 0.0f };
	_force[0] = -MOVEMENT_SPEED;
}

void const ParticleModel::MoveRight()
{
	_force = { 0.0f, 0.0f, 0.0f };
	_force[0] = MOVEMENT_SPEED;
}

void ParticleModel::MoveConstVelocity( const float deltaTime )
{
	_transform->SetPosition(
		_transform->GetPosition()[0] + _velocity[0] * TIME_STEP,
		_transform->GetPosition()[1] + _velocity[1] * TIME_STEP,
		_transform->GetPosition()[2] + _velocity[2] * TIME_STEP
	);
}

void ParticleModel::MoveConstAcceleration( const float deltaTime )
{
	v3df position = _transform->GetPosition();

	position[0] += _velocity[0] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[0] * ( TIME_STEP * 0.00001f ) * ( TIME_STEP * 0.000001f );
	position[1] += _velocity[1] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[1] * ( TIME_STEP * 0.00001f ) * ( TIME_STEP * 0.000001f );
	position[2] += _velocity[2] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[2] * ( TIME_STEP * 0.00001f ) * ( TIME_STEP * 0.000001f );

	_velocity[0] += _acceleration[0] * ( deltaTime * 0.000001f );
	_velocity[1] += _acceleration[1] * ( deltaTime * 0.000001f );
	_velocity[2] += _acceleration[2] * ( deltaTime * 0.000001f );

	_transform->SetPosition( position );
}

void ParticleModel::UpdateState()
{
	UpdateNetForce();
	UpdateAcceleration();
	Move();
}

void ParticleModel::UpdateNetForce()
{
	_netForce[0] = _force[0];
	//_netForce[1] -= GRAVITY;
	_netForce[2] = _force[2];
}

void ParticleModel::UpdateAcceleration()
{
	_acceleration[0] = _netForce[0] / _mass;
	//_acceleration[1] = _netForce[1] / _mass;
	_acceleration[2] = _netForce[2] / _mass;
}

void ParticleModel::Move()
{
	_transform->SetPosition(
		_transform->GetPosition()[0] + _velocity[0] * TIME_STEP + 0.5f * _acceleration[0] * TIME_STEP * TIME_STEP,
		0.5f,
		_transform->GetPosition()[2] + _velocity[2] * TIME_STEP + 0.5f * _acceleration[2] * TIME_STEP * TIME_STEP
	);

	_velocity[0] = _velocity[0] + _acceleration[0] * TIME_STEP;
	//_velocity[1] = _velocity[1] + _acceleration[1] * TIME_STEP;
	_velocity[2] = _velocity[2] + _acceleration[2] * TIME_STEP;

	//_force = { 0.0f, 0.0f, 0.0f };
}