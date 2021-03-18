#pragma once
#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "../Graphics/Bindables/GraphicsResource.h"
#include "../Graphics/Bindables/VertexBuffer.h"
#include "../Graphics/Bindables/IndexBuffer.h"
#include "../Graphics/Objects/GameObject.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

class Camera;

class LevelManager : public GraphicsResource
{
public:
	virtual void Initialize( Graphics& gfx );
	virtual void Update( Mouse& mouse, Keyboard& keyboard, float dt );

	void BeginRender( Graphics& gfx );
	virtual void Render( Graphics& gfx ) {}
	void EndRender( Graphics& gfx );
protected:
	virtual void UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt );

	enum ActiveScene
	{
		SUMMER,
		WINTER,
		APERATURE,
		MINECRAFT
	} activeScene = SUMMER;

	// Buffers
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;
	VertexBuffer<SimpleVertex> vb_cube;
	VertexBuffer<SimpleVertex> vb_plane;
	IndexBuffer ib_cube;
	IndexBuffer ib_plane;

	// Materials
	Material shinyMaterial;
	Material noSpecMaterial;

	// Geometry
	Geometry herculesGeometry;
	Geometry sphereGeometry;
	Geometry cubeGeometry;
	Geometry planeGeometry;

	// Textures
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures;
private:
	void InitializeMaterials( Graphics& gfx );
	void InitializeGeometry( Graphics& gfx );
	bool InitializeTextures( Graphics& gfx );
	bool InitializeBuffers( Graphics& gfx );
	void InitializeObjects( Graphics& gfx );
	void SpawnControlWindow();

	// Local Objects
	Light basicLight;
	bool updateWalls = true;
	bool changeScene = false;
	std::shared_ptr<Camera> camera;
	std::unique_ptr<GameObject> torus;
	std::unique_ptr<GameObject> ground;
	std::unique_ptr<GameObject> skysphere;
	std::vector<std::unique_ptr<GameObject>> walls;
};

#endif