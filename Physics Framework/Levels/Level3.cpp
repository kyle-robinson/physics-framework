#include "stdafx.h"
#include "Level3.h"
#include <imgui/imgui.h>

void Level3::Initialize( Graphics& gfx )
{
	LevelManager::Initialize( gfx );

	// initialize cubes
	rigidCubes.resize( NUMBER_OF_RIGID_CUBES );
	for ( uint32_t i = 0; i < NUMBER_OF_RIGID_CUBES; i++ )
	{
		rigidCubes[i] = std::make_unique<GameObject>( "Cube " + std::to_string( i + 1 ), true );

		if ( i == 0 )
		{
			rigidCubes[i]->SetPosition( 1.0f, 5.0f, 1.0f );
			rigidCubes[i]->GetRigidBody()->SetOrientation( 0.5f, 0.5f, 0.25f, 0.0f );
		}
		if ( i == 1 )
			rigidCubes[i]->SetPosition( 0.0f, 0.0f, 1.0f );

		rigidCubes[i]->GetAppearance()->SetTextureRV( textureMarble.Get() );
		rigidCubes[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		rigidCubes[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}

	// initialize rigid bodies
	bottomCube = new Box();
	bottomCube->_halfSize = v3df( 1.0f, 1.0f, 1.0f );
	bottomCube->_body = rigidCubes[0]->GetRigidBody();
	bottomCube->CalculateInternals();

	topCube = new Box();
	topCube->_halfSize = v3df( 1.0f, 1.0f, 1.0f );
	topCube->_body = rigidCubes[1]->GetRigidBody();
	topCube->CalculateInternals();

	rigidCubes[0]->GetRigidBody()->SetAwake();
	rigidCubes[1]->GetRigidBody()->SetAwake();

	collisionPlane = new CollisionPlane();
	collisionPlane->_direction = v3df( 0.0f, 1.0f, 0.0f );
	collisionPlane->_offset = 0.0f;

	contactResolver = new ContactResolver( MAX_CONTACTS );
	collisionData._contactArray = contacts;
	collisionData._friction = 0.9f;
	collisionData._restitution = 0.1f;
	collisionData._tolerance = 0.1f;
}

void Level3::Update( Mouse& mouse, Keyboard& keyboard, float dt )
{
	UpdateInput( mouse, keyboard, dt );

	// update cubes
	for ( uint32_t i = 0; i < rigidCubes.size(); i++ )
		rigidCubes[i]->Update( dt / 50.0f, true );

	// update rigid bodies
	topCube->CalculateInternals();
	bottomCube->CalculateInternals();

	collisionData.Reset( MAX_CONTACTS );
	collisionData._friction = 0.9f;
	collisionData._restitution = 0.1f;
	collisionData._tolerance = 0.1f;

	if ( collisionData.HasMoreContacts() )
	{
		CollisionDetector::BoxAndHalfSpace( *topCube, *collisionPlane, &collisionData );
		CollisionDetector::BoxAndHalfSpace( *bottomCube, *collisionPlane, &collisionData );
		CollisionDetector::BoxAndBox( *topCube, *bottomCube, &collisionData );

		contactResolver->ResolveContacts( collisionData._contactArray, collisionData._contactCount, dt );
	}

	for ( uint32_t i = 0; i < rigidCubes.size(); i++ )
		rigidCubes[i]->UpdateTransforms();

	LevelManager::Update( mouse, keyboard, dt );
}

void Level3::UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt )
{
	LevelManager::UpdateInput( mouse, keyboard, dt );
}

void Level3::Render( Graphics& gfx )
{
	LevelManager::BeginRender( gfx );

	// Render Scene Objects
	for ( uint32_t i = 0; i < rigidCubes.size(); i++ )
	{
		// Get Materials
		Material material = rigidCubes[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( rigidCubes[i]->GetRigidBody()->GetTransform()->GetTransformMatrix() );

		// Set Textures
		if ( rigidCubes[i]->GetAppearance()->HasTexture() )
		{
			GetContext( gfx )->PSSetShaderResources( 0, 1, rigidCubes[i]->GetAppearance()->GetTextureRV() );
			cb_vs_matrix.data.HasTexture = 1.0f;
		}
		else
		{
			cb_vs_matrix.data.HasTexture = 0.0f;
		}

		if ( !cb_vs_matrix.ApplyChanges() ) return;
		rigidCubes[i]->Draw( GetContext( gfx ) );
	}

	SpawnControlWindow( gfx );

	LevelManager::EndRender( gfx );
}

void Level3::SpawnControlWindow( Graphics& gfx )
{
	if ( ImGui::Begin( "Rigid Body Controls" ) )
	{
		// set/reset rigid body simulation
		static bool resetSimulation = false;
		static std::string simulationString = "Reset Simulation";
		if ( ImGui::Button( ( activeSimulation == ActiveSimulation::SIMULATION_1 ) ? simulationString.c_str() : "Simulation 1" ) )
		{
			activeSimulation = ActiveSimulation::SIMULATION_1;
			resetSimulation = true;
		}
		if ( ImGui::Button( ( activeSimulation == ActiveSimulation::SIMULATION_2 ) ? simulationString.c_str() : "Simulation 2" ) )
		{
			activeSimulation = ActiveSimulation::SIMULATION_2;
			resetSimulation = true;
		}
		if ( ImGui::Button( ( activeSimulation == ActiveSimulation::SIMULATION_3 ) ? simulationString.c_str() : "Simulation 3" ) )
		{
			activeSimulation = ActiveSimulation::SIMULATION_3;
			resetSimulation = true;
		}

		// reset active simulation
		if ( resetSimulation )
		{
			resetSimulation = false;
			switch ( activeSimulation )
			{
			case ActiveSimulation::SIMULATION_1:
			{
				rigidCubes[0]->SetPosition( 1.0f, 5.0f, 1.0f );
				rigidCubes[0]->GetRigidBody()->SetOrientation( 0.5f, 0.5f, 0.25f, 0.0f );
				rigidCubes[0]->GetRigidBody()->ResetForces();
				rigidCubes[0]->GetRigidBody()->SetAcceleration( 0.0f, -10.0f, 0.0f );
				rigidCubes[0]->GetRigidBody()->SetVelocity( 0.0f, 0.0f, 0.0f );
				rigidCubes[0]->GetRigidBody()->SetAwake( true );

				rigidCubes[1]->SetPosition( 0.0f, 0.0f, 1.0f );
				rigidCubes[1]->GetRigidBody()->SetOrientation( 0.0f, 0.0f, 0.0f, 0.0f );
				rigidCubes[1]->GetRigidBody()->ResetForces();
				rigidCubes[1]->GetRigidBody()->SetAcceleration( 0.0f, -10.0f, 0.0f );
				rigidCubes[1]->GetRigidBody()->SetVelocity( 0.0f, 0.0f, 0.0f );
				rigidCubes[1]->GetRigidBody()->SetAwake( true );
				break;
			}
			case ActiveSimulation::SIMULATION_2:
			{
				rigidCubes[0]->SetPosition( -2.0f, 5.0f, 1.0f );
				rigidCubes[0]->GetRigidBody()->SetOrientation( 1.0f, 0.25f, 0.75f, 0.0f );
				rigidCubes[0]->GetRigidBody()->ResetForces();
				rigidCubes[0]->GetRigidBody()->SetAcceleration( 0.0f, -10.0f, 0.0f );
				rigidCubes[0]->GetRigidBody()->SetVelocity( 0.0f, 0.0f, 0.0f );
				rigidCubes[0]->GetRigidBody()->SetAwake( true );

				rigidCubes[1]->SetPosition( 0.0f, 0.0f, 1.0f );
				rigidCubes[1]->GetRigidBody()->SetOrientation( 0.5f, 0.0f, 0.0f, 0.0f );
				rigidCubes[1]->GetRigidBody()->ResetForces();
				rigidCubes[1]->GetRigidBody()->SetAcceleration( 0.0f, -10.0f, 0.0f );
				rigidCubes[1]->GetRigidBody()->SetVelocity( 0.0f, 0.0f, 0.0f );
				rigidCubes[1]->GetRigidBody()->SetAwake( true );
				break;
			}
			case ActiveSimulation::SIMULATION_3:
			{

				break;
			}
			}
		}

		if ( ImGui::Button( "Stop Simulation" ) )
		{
			rigidCubes[0]->GetRigidBody()->SetAwake( false );
			rigidCubes[1]->GetRigidBody()->SetAwake( false );
		}
	}
	ImGui::End();
}