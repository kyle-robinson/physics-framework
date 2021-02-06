#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include "Transform.h"
#include "Appearance.h"
#include "ParticleModel.h"
#include "ConstantBuffer.h"
//#include "ParticleSystem.h"

class Particle
{
public:
	Particle( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, Geometry geometry, Material material );
	void Update();
	void Draw( ID3D11DeviceContext* pImmediateContext, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix );
	//void Respawn( v3df position, v3df velocity, float maxAge );
	void Respawn();

	//bool GetActive() const noexcept { return _active; };
	//float GetAge() const noexcept { return _curAge; };
	//float GetMaxAge() const noexcept { return _maxAge; };

	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; };
	std::shared_ptr<Appearance> GetAppearance() const noexcept { return _appearance; };
	std::shared_ptr<ParticleModel> GetParticleModel() const noexcept { return _particleModel; };
private:
	v3df position;
	v3df oldPosition;
	v3df velocity;
	DWORD color;
	int energy;
	float size;

	//int _curAge;
	//int _maxAge;
	//bool _active;
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<Appearance> _appearance;
	std::shared_ptr<ParticleModel> _particleModel;
};

#endif