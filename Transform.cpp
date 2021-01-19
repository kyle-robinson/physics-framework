#include "PCH.h"
#include "Transform.h"
#include "GameObject.h"

void Transform::MoveForward( GameObject& object )
{
	v3df position = object.GetPosition();
	position[2] -= 0.002f;
	object.SetPosition( position );
}

void Transform::MoveBackward( GameObject& object )
{
	v3df position = object.GetPosition();
	position[2] += 0.002f;
	object.SetPosition( position );
}