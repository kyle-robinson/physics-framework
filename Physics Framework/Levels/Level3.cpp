#include "stdafx.h"
#include "Level3.h"
#include <imgui/imgui.h>

void Level3::Initialize( Graphics& gfx )
{
	LevelManager::Initialize( gfx );

	// initialize cubes
	rigidCubes.resize( NUMBER_OF_RIGID_CUBES );
	for ( uint32_t i = 0u; i < rigidCubes.size(); i++ )
	{
		rigidCubes[i] = std::make_unique<GameObject>( "Cube " + std::to_string( i + 1u ), true );
		rigidCubes[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		rigidCubes[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}
	LoadSimulation( SIMULATION_1 );

	// initialize rigid bodies
	bottomCube = std::make_unique<CollisionCube>();
	bottomCube->_halfSize = v3df( 1.0f, 1.0f, 1.0f );
	bottomCube->_body = rigidCubes[0]->GetRigidBody();
	bottomCube->CalculateInternals();

	topCube = std::make_unique<CollisionCube>();
	topCube->_halfSize = v3df( 1.0f, 1.0f, 1.0f );
	topCube->_body = rigidCubes[1]->GetRigidBody();
	topCube->CalculateInternals();

	collisionPlane = std::make_unique<CollisionPlane>();
	collisionPlane->_direction = v3df( 0.0f, 1.0f, 0.0f );
	collisionPlane->_offset = 0.0f;

	contactResolver = std::make_unique<ContactResolver>( MAX_CONTACTS );
	collisionData._contactArray = contacts;
	collisionData._friction = friction;
	collisionData._restitution = restitution;
	collisionData._tolerance = tolerance;
}

void Level3::LoadSimulation( ActiveSimulation simulation )
{
	activeSimulation = simulation;

	rigidCubes[0]->GetRigidBody()->SetAcceleration( 0.0f, -10.0f, 0.0f );
	rigidCubes[1]->GetRigidBody()->SetAcceleration( 0.0f, -10.0f, 0.0f );

	rigidCubes[0]->GetRigidBody()->SetActive();
	rigidCubes[1]->GetRigidBody()->SetActive();

	switch ( simulation )
	{
	case SIMULATION_1:
	{
		rigidCubes[0]->SetPosition( 1.0f, 5.0f, 6.0f );
		rigidCubes[1]->SetPosition( 0.0f, 1.0f, 6.0f );

		rigidCubes[0]->GetRigidBody()->SetOrientation( 0.5f, 0.5f, 0.25f, 0.0f );
		rigidCubes[1]->GetRigidBody()->SetOrientation( 0.0f, 0.0f, 0.0f, 0.0f );
		break;
	}
	case SIMULATION_2:
	{
		rigidCubes[0]->SetPosition( -2.0f, 5.0f, 6.0f );
		rigidCubes[1]->SetPosition( 0.0f, 1.0f, 6.0f );

		rigidCubes[0]->GetRigidBody()->SetOrientation( 1.0f, 0.25f, 0.75f, 0.0f );
		rigidCubes[1]->GetRigidBody()->SetOrientation( 0.5f, 0.0f, 0.0f, 0.0f );
		break;
	}
	case SIMULATION_3:
	{
		rigidCubes[0]->SetPosition( -2.0f, 5.0f, 6.0f );
		rigidCubes[1]->SetPosition( 2.0f, 5.0f, 6.0f );

		rigidCubes[0]->GetRigidBody()->SetOrientation( 1.0f, 0.25f, 0.75f, 0.0f );
		rigidCubes[1]->GetRigidBody()->SetOrientation( 0.5f, 0.0f, 0.0f, 0.0f );

		rigidCubes[1]->GetRigidBody()->SetAcceleration( -10.0f, -10.0f, 0.0f );		
		break;
	}
	case SIMULATION_4:
	{		
		rigidCubes[0]->SetPosition( -2.0f, 5.0f, 6.0f );
		rigidCubes[1]->SetPosition( 2.0f, 5.0f, 6.0f );

		rigidCubes[0]->GetRigidBody()->SetOrientation( 0.0f, 0.0f, 0.0f, 0.0f );
		rigidCubes[1]->GetRigidBody()->SetOrientation( 0.0f, 0.0f, 0.0f, 0.0f );

		rigidCubes[0]->GetRigidBody()->SetAcceleration( 0.0f, 0.0f, 0.0f );
		rigidCubes[1]->GetRigidBody()->SetAcceleration( 0.0f, 0.0f, 0.0f );
		break;
	}
	}
}

void Level3::StopSimulation()
{
	rigidCubes[0]->GetRigidBody()->SetActive( false );
	rigidCubes[1]->GetRigidBody()->SetActive( false );
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

	// update collision data
	collisionData.Reset( MAX_CONTACTS );
	collisionData._friction = friction;
	collisionData._restitution = restitution;
	collisionData._tolerance = tolerance;

	// resolve collisions
	if ( collisionData.HasMoreContacts() )
	{
		CollisionDetector::CubeAndHalfSpace( *topCube, *collisionPlane, &collisionData );
		CollisionDetector::CubeAndHalfSpace( *bottomCube, *collisionPlane, &collisionData );
		CollisionDetector::CubeAndCube( *topCube, *bottomCube, &collisionData );
		contactResolver->ResolveContacts( collisionData._contactArray, collisionData._contactCount, dt );
	}

	// update positions
	for ( uint32_t i = 0; i < rigidCubes.size(); i++ )
		rigidCubes[i]->UpdateTransforms();

	LevelManager::Update( mouse, keyboard, dt );
}

void Level3::UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt )
{
	LevelManager::UpdateInput( mouse, keyboard, dt );

	if ( keyboard.KeyIsPressed( '1' ) ) LoadSimulation( SIMULATION_1 );
	if ( keyboard.KeyIsPressed( '2' ) ) LoadSimulation( SIMULATION_2 );
	if ( keyboard.KeyIsPressed( '3' ) ) LoadSimulation( SIMULATION_3 );
	if ( keyboard.KeyIsPressed( '4' ) ) LoadSimulation( SIMULATION_4 );
	if ( keyboard.KeyIsPressed( 'R' ) ) LoadSimulation( activeSimulation );
	if ( keyboard.KeyIsPressed( 'F' ) ) StopSimulation();
}

void Level3::Render( Graphics& gfx )
{
	LevelManager::BeginRender( gfx );

	// render cubes
	for ( uint32_t i = 0; i < rigidCubes.size(); i++ )
	{
		// get materials
		Material material = rigidCubes[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( rigidCubes[i]->GetRigidBody()->GetTransform()->GetTransformMatrix() );

		// set textures
		if ( rigidCubes[i]->GetAppearance()->HasTexture() )
			cb_vs_matrix.data.HasTexture = 1.0f;
		else
			cb_vs_matrix.data.HasTexture = 0.0f;

		// update textures
		switch ( activeScene )
		{
		case SUMMER: rigidCubes[i]->GetAppearance()->SetTextureRV( textures["Marble"].Get() ); break;
		case WINTER: rigidCubes[i]->GetAppearance()->SetTextureRV( textures["Crate"].Get() ); break;
		case APERATURE: rigidCubes[i]->GetAppearance()->SetTextureRV( textures["Companion"].Get() ); break;
		case MINECRAFT: rigidCubes[i]->GetAppearance()->SetTextureRV( textures["Slime"].Get() ); break;
		}
		GetContext( gfx )->PSSetShaderResources( 0, 1, rigidCubes[i]->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		rigidCubes[i]->Draw( GetContext( gfx ) );
	}

	SpawnInstructionWindow();
	SpawnControlWindow( gfx );

	LevelManager::EndRender( gfx );
}

void Level3::SpawnControlWindow( Graphics& gfx )
{
	if ( ImGui::Begin( "Simulation Controls" ), FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove )
	{
		ImGui::Text( "Collision Count: %u", collisionData._contactCount );
		ImGui::SliderFloat( "Friction", &friction, 0.0f, 2.0f, "%.1f" );
		ImGui::SliderFloat( "Restitution", &restitution, 0.0f, 1.0f, "%.1f" );
		ImGui::SliderFloat( "Tolerance", &tolerance, 0.0f, 1.0f, "%.1f" );

		// pause current simulation
		if ( ImGui::Button( "Stop Simulation", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) )
			StopSimulation();
		ImGui::NewLine();

		// set/reset rigid body simulation
		static std::string simulationString = "Reset Simulation";
		if ( ImGui::CollapsingHeader( "Load Simulations" ) )
		{
			if ( ImGui::Button(
				 ( activeSimulation == SIMULATION_1 ) ? simulationString.c_str() : "Load Simulation 1",
				 ImVec2( ImGui::GetWindowSize().x, 0.0f )
			   ) )
			{
				LoadSimulation( SIMULATION_1 );
			}
			if ( ImGui::Button(
				 ( activeSimulation == SIMULATION_2 ) ? simulationString.c_str() : "Load Simulation 2",
				 ImVec2( ImGui::GetWindowSize().x, 0.0f )
			   ) )
			{
				LoadSimulation( SIMULATION_2 );
			}
			if ( ImGui::Button(
				 ( activeSimulation == SIMULATION_3 ) ? simulationString.c_str() : "Load Simulation 3",
				 ImVec2( ImGui::GetWindowSize().x, 0.0f )
			   ) )
			{
				LoadSimulation( SIMULATION_3 );
			}
			if ( ImGui::Button(
				( activeSimulation == SIMULATION_4 ) ? simulationString.c_str() : "Load Simulation 4",
				ImVec2( ImGui::GetWindowSize().x, 0.0f )
			) )
			{
				LoadSimulation( SIMULATION_4 );
			}
		}

		// set/reset angular rotation simulation
		if ( activeSimulation == SIMULATION_4 )
		{
			if ( ImGui::CollapsingHeader( "Angular Rotation Simulation" ) )
			{
				ImGui::Text( "Apply Forces" );
				ImGui::NewLine();
				if ( ImGui::Button( "Front Face, Left Edge" ) )
				{
					rigidCubes[0]->GetRigidBody()->AddTorque( { 0.0f, 100.0f, 0.0f } );
					rigidCubes[1]->GetRigidBody()->AddTorque( { 0.0f, 100.0f, 0.0f } );
				}
				if ( ImGui::Button( "Front Face, Top Edge" ) )
				{
					rigidCubes[0]->GetRigidBody()->AddTorque( { 100.0f, 0.0f, 0.0f } );
					rigidCubes[1]->GetRigidBody()->AddTorque( { 100.0f, 0.0f, 0.0f } );
				}
				if ( ImGui::Button( "Front Face, Right Edge" ) )
				{
					rigidCubes[0]->GetRigidBody()->AddTorque( { 0.0f, -100.0f, 0.0f } );
					rigidCubes[1]->GetRigidBody()->AddTorque( { 0.0f, -100.0f, 0.0f } );
				}
			}
		}
	}
	ImGui::End();
}

void Level3::SpawnInstructionWindow()
{
	if ( ImGui::Begin( "Scene Instructions", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		if ( ImGui::CollapsingHeader( "Rigid Body Simulations", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ImGui::Text( "1       ->  Start Simulation 1" );
			ImGui::Text( "2       ->  Start Simulation 2" );
			ImGui::Text( "3       ->  Start Simulation 3" );
			ImGui::Text( "3       ->  Start Simulation 4" );
			ImGui::Text( "F       ->  Stop Simulation" );
			ImGui::Text( "R       ->  Reset Simulation" );
		}
	}
	ImGui::End();
}