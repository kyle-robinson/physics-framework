#include "PCH.h"
#include "ParticleModel.h"
#include "GameObject.h"

void ParticleModel::MoveForward( GameObject& object )
{
	v3df position = object.GetPosition();
	position[2] += 0.002f;
	object.SetPosition( position );
}

void ParticleModel::MoveBackward( GameObject& object )
{
	v3df position = object.GetPosition();
	position[2] -= 0.002f;
	object.SetPosition( position );
}

void ParticleModel::MoveLeft( GameObject& object )
{
	v3df position = object.GetPosition();
	position[0] -= 0.002f;
	object.SetPosition( position );
}

void ParticleModel::MoveRight( GameObject& object )
{
	v3df position = object.GetPosition();
	position[0] += 0.002f;
	object.SetPosition( position );
}