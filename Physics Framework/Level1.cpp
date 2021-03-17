#include "stdafx.h"
#include "Level1.h"
#include "Input/Keyboard.h"
#include <imgui/imgui.h>

void Level1::Initialize( Graphics& gfx )
{
	LevelManager::Initialize( gfx );

	// initialize cubes
	cubes.resize( NUMBER_OF_CUBES );
	for ( uint32_t i = 0; i < NUMBER_OF_CUBES; i++ )
	{
		cubes[i] = std::make_unique<GameObject>( "Cube " + std::to_string( i + 1 ) );
		cubes[i]->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		cubes[i]->GetTransform()->SetInitialPosition( -4.0f + ( i * 2.0f ), 0.5f, 10.0f );
		cubes[i]->GetAppearance()->SetTextureRV( textureMarble.Get() );
		cubes[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		cubes[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}
}

void Level1::CollisionResolution( std::unique_ptr<GameObject>& cube1, std::unique_ptr<GameObject>& cube2, float dt )
{
	float velocityOne = max( cube1->GetParticleModel()->GetNetForce().magnitude(), 1.0f );
	float velocityTwo = max( cube2->GetParticleModel()->GetNetForce().magnitude(), 1.0f );

	float forceMagnitude = (
		cube1->GetParticleModel()->GetMass() * velocityOne +
		cube2->GetParticleModel()->GetMass() * velocityTwo
		) / dt;

	v3df force = (
		cube2->GetTransform()->GetPosition() -
		cube1->GetTransform()->GetPosition()
		).normalize() * forceMagnitude * 0.0015f;

	v3df inverseForce;
	inverseForce.x = -force.x;
	inverseForce.y = -force.y;
	inverseForce.z = -force.z;

	cube1->GetParticleModel()->AddForce( inverseForce );
	cube2->GetParticleModel()->AddForce( force );
}

void Level1::Update( Mouse& mouse, Keyboard& keyboard, float dt )
{
	UpdateInput( mouse, keyboard, dt );

	// Update Cubes
	for ( uint32_t i = 0; i < cubes.size(); i++ )
		cubes[i]->Update( dt );

	// Check Collisions
	for ( uint32_t i = 0; i < cubes.size(); i++ )
	{
		for ( uint32_t j = 0; j < cubes.size(); j++ )
		{
			if ( i != j )
			{
				if ( useAABB )
				{
					if ( cubes[i]->GetParticleModel()->CollisionCheckAABB( cubes[j]->GetTransform()->GetPosition() ) )
						CollisionResolution( cubes[i], cubes[j], dt );
				}
				else
				{
					if ( cubes[i]->GetParticleModel()->CollisionCheckCircle(
							cubes[j]->GetTransform()->GetPosition(),
							cubes[j]->GetParticleModel()->GetCollisionRadius() )
						)
						CollisionResolution( cubes[i], cubes[j], dt );
				}
			}
		}
	}

	LevelManager::Update( mouse, keyboard, dt );
}

void Level1::UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt )
{
	LevelManager::UpdateInput( mouse, keyboard, dt );
	
	while ( !keyboard.KeyBufferIsEmpty() )
	{
		Keyboard::KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		if ( kbe.IsPress() )
		{
			static int cubeToUse = 0;

			// select cube to move
			switch ( kbe.GetKeyCode() )
			{
			case '1': cubeToUse = 0; break;
			case '2': cubeToUse = 1; break;
			case '3': cubeToUse = 2; break;
			case '4': cubeToUse = 3; break;
			case '5': cubeToUse = 4; break;
			}

			// object movement
			switch ( kbe.GetKeyCode() )
			{
			case VK_UP: cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.0f,  0.1f }, 25.0f ); break;
			case VK_LEFT: cubes[cubeToUse]->GetParticleModel()->AddThrust( { -0.1f, 0.0f,  0.0f }, 25.0f ); break;
			case VK_DOWN: cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.0f, -0.1f }, 25.0f ); break;
			case VK_RIGHT: cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.1f, 0.0f,  0.0f }, 25.0f ); break;
			case VK_HOME: cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.5f, 0.0f }, 25.0f ); break;
			}
		}
	}

	// reset object position
	if ( keyboard.KeyIsPressed( 'R' ) )
		for ( uint32_t i = 0; i < cubes.size(); i++ )
			cubes[i]->GetTransform()->ResetPosition();

	// reset object forces
	if ( keyboard.KeyIsPressed( 'F' ) )
		for ( uint32_t i = 0; i < cubes.size(); i++ )
			cubes[i]->GetParticleModel()->ResetForces();
}

void Level1::Render( Graphics& gfx )
{
	LevelManager::BeginRender( gfx );

	// Render Scene Objects
	for ( uint32_t i = 0; i < cubes.size(); i++ )
	{
		// Get Materials
		Material material = cubes[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( cubes[i]->GetTransform()->GetWorldMatrix() );

		// Set Textures
		if ( cubes[i]->GetAppearance()->HasTexture() )
		{
			GetContext( gfx )->PSSetShaderResources( 0, 1, cubes[i]->GetAppearance()->GetTextureRV() );
			cb_vs_matrix.data.HasTexture = 1.0f;
		}
		else
		{
			cb_vs_matrix.data.HasTexture = 0.0f;
		}

		if ( !cb_vs_matrix.ApplyChanges() ) return;
		cubes[i]->Draw( GetContext( gfx ) );
	}

	SpawnControlWindow( cubes );

	LevelManager::EndRender( gfx );
}

void Level1::SpawnControlWindow( std::vector<std::unique_ptr<GameObject>>& vec )
{
	if ( ImGui::Begin( "Cube Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		ImGui::Text( "Collision Type:" );
		ImGui::SameLine();
		static int collisionGroup = 0;
		if ( ImGui::RadioButton( "AABB", &collisionGroup, 0 ) )
			useAABB = true;
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Circle", &collisionGroup, 1 ) )
			useAABB = false;

		// update individual cube properties
		for ( uint32_t i = 0; i < vec.size(); i++ )
		{
			if ( ImGui::CollapsingHeader( vec[i]->GetID().c_str() ) )
			{
				float mass = vec[i]->GetParticleModel()->GetMass();
				ImGui::SliderFloat( std::string( "Mass##" ).append( std::to_string( i ) ).c_str(), &mass, 1.0f, 100.0f, "%1.f" );
				vec[i]->GetParticleModel()->SetMass( mass );

				float friction = vec[i]->GetParticleModel()->GetFriction();
				ImGui::SliderFloat( std::string( "Friction##" ).append( std::to_string( i ) ).c_str(), &friction, 0.0f, 0.0008f, "%.7f", 10 );
				vec[i]->GetParticleModel()->SetFriction( friction );

				float dragFactor = vec[i]->GetParticleModel()->GetDragFactor();
				ImGui::SliderFloat( std::string( "Drag Factor##" ).append( std::to_string( i ) ).c_str(), &dragFactor, 0.0f, 10.0f, "%1.f" );
				vec[i]->GetParticleModel()->SetDragFactor( dragFactor );

				ImGui::Text( "Drag Type: " );
				ImGui::SameLine();
				static int dragFactorGroup = 0;
				if ( ImGui::RadioButton( std::string( "Laminar##" ).append( std::to_string( i ) ).c_str(), &dragFactorGroup + i, 0 ) )
					vec[i]->GetParticleModel()->SetLaminar( true );
				ImGui::SameLine();
				if ( ImGui::RadioButton( std::string( "Turbulent##" ).append( std::to_string( i ) ).c_str(), &dragFactorGroup + i, 1 ) )
					vec[i]->GetParticleModel()->SetLaminar( false );
			}
		}
	}
	ImGui::End();
}