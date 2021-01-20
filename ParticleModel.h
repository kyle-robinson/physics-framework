#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

class GameObject;

class ParticleModel
{
public:
	static void MoveForward( GameObject& object );
	static void MoveBackward( GameObject& object );
	static void MoveLeft( GameObject& object );
	static void MoveRight( GameObject& object );
};

#endif