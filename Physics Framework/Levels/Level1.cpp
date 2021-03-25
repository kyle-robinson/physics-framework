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
		cubes[i] = std::make_unique<GameObject>( "Cube " + std::to_string( i + 1 ), false );
		cubes[i]->GetParticleModel()->SetMass( 20.0f );
		cubes[i]->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		cubes[i]->GetTransform()->SetInitialPosition( -4.0f + ( i * 2.0f ), 0.5f, 10.0f );
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

	// update cubes
	for ( uint32_t i = 0; i < cubes.size(); i++ )
		cubes[i]->Update( dt, false );

	// check collisions
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
					if ( cubes[i]->GetParticleModel()->CollisionCheckSphere(
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

			// select cube to move
			switch ( kbe.GetKeyCode() )
			{
			case '1': cubeToUse = 0; break;
			case '2': cubeToUse = 1; break;
			case '3': cubeToUse = 2; break;
			case '4': cubeToUse = 3; break;
			case '5': cubeToUse = 4; break;
			}
		}
	}

	// object movement
	if ( useThrustMovement )
	{
		// move cubes using thrust forces
		if ( keyboard.KeyIsPressed( VK_UP ) )
			cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.0f,  0.1f }, 0.25f );
		if ( keyboard.KeyIsPressed( VK_LEFT ) )
			cubes[cubeToUse]->GetParticleModel()->AddThrust( { -0.1f, 0.0f,  0.0f }, 0.25f );
		if ( keyboard.KeyIsPressed( VK_DOWN ) )
			cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.0f, -0.1f }, 0.25f );
		if ( keyboard.KeyIsPressed( VK_RIGHT ) )
			cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.1f, 0.0f,  0.0f }, 0.25f );
	}
	else
	{
		// move cubes using constant acceleration
		static v3df acceleration = { 0.0f, 0.0f, 0.0f };
		if ( keyboard.KeyIsPressed( VK_UP ) )
		{
			if ( acceleration.z < 0.0f ) acceleration.z = 0.0f;
			acceleration.z += 0.01f;
			cubes[cubeToUse]->GetParticleModel()->AddForce( { 0.0f, 0.0f, acceleration.z } );
			cubes[cubeToUse]->GetParticleModel()->SetAcceleration( cubes[cubeToUse]->GetParticleModel()->GetAcceleration() + acceleration );
		}
		if ( keyboard.KeyIsPressed( VK_LEFT ) )
		{
			if ( acceleration.x > 0.0f ) acceleration.x = 0.0f;
			acceleration.x -= 0.01f;
			cubes[cubeToUse]->GetParticleModel()->AddForce( { acceleration.x, 0.0f, 0.0f } );
			cubes[cubeToUse]->GetParticleModel()->SetAcceleration( cubes[cubeToUse]->GetParticleModel()->GetAcceleration() + acceleration );
		}
		if ( keyboard.KeyIsPressed( VK_DOWN ) )
		{
			if ( acceleration.z > 0.0f ) acceleration.z = 0.0f;
			acceleration.z -= 0.01f;
			cubes[cubeToUse]->GetParticleModel()->AddForce( { 0.0f, 0.0f, acceleration.z } );
			cubes[cubeToUse]->GetParticleModel()->SetAcceleration( cubes[cubeToUse]->GetParticleModel()->GetAcceleration() + acceleration );
		}
		if ( keyboard.KeyIsPressed( VK_RIGHT ) )
		{
			if ( acceleration.x < 0.0f ) acceleration.x = 0.0f;
			acceleration.x += 0.01f;
			cubes[cubeToUse]->GetParticleModel()->AddForce( { acceleration.x, 0.0f,  0.0f } );
			cubes[cubeToUse]->GetParticleModel()->SetAcceleration( cubes[cubeToUse]->GetParticleModel()->GetAcceleration() + acceleration );
		}
	}

	if ( keyboard.KeyIsPressed( VK_HOME ) )
		cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.6f, 0.0f }, 0.75f );

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

	// render scene objects
	for ( uint32_t i = 0; i < cubes.size(); i++ )
	{
		// get materials
		Material material = cubes[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( cubes[i]->GetTransform()->GetWorldMatrix() );

		// set textures
		if ( cubes[i]->GetAppearance()->HasTexture() )
			cb_vs_matrix.data.HasTexture = 1.0f;
		else
			cb_vs_matrix.data.HasTexture = 0.0f;


		// update textures
		switch ( activeTheme )
		{
		case SUMMER: cubes[i]->GetAppearance()->SetTextureRV( textures["Marble"].Get() ); break;
		case WINTER: cubes[i]->GetAppearance()->SetTextureRV( textures["Crate"].Get() ); break;
		case APERATURE: cubes[i]->GetAppearance()->SetTextureRV( textures["Companion"].Get() ); break;
		case MINECRAFT: cubes[i]->GetAppearance()->SetTextureRV( textures["Slime"].Get() ); break;
		}
		GetContext( gfx )->PSSetShaderResources( 0, 1, cubes[i]->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		cubes[i]->Draw( GetContext( gfx ) );
	}

	SpawnControlWindow();
	SpawnInstructionWindow();

	LevelManager::EndRender( gfx );
}

void Level1::SpawnControlWindow()
{
	if ( ImGui::Begin( "Cube Physics", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		ImGui::PushItemWidth( 300.0f );

		// select cube to use
		ImGui::Text( "Cube To Use:" );
		for ( uint32_t i = 0; i < cubes.size(); i++ )
		{
			ImGui::SameLine();
			if ( ImGui::Button( cubes[i]->GetID().c_str() ) )
				cubeToUse = i;
		}

		// update cube movement type
		ImGui::Text( "Movement Model:" );
		ImGui::SameLine();
		static int movementGroup = 0;
		if ( ImGui::RadioButton( "Thrust", &movementGroup, 0 ) )
			useThrustMovement = true;
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Constant Acceleration", &movementGroup, 1 ) )
			useThrustMovement = false;
		
		// update cube collision type
		ImGui::Text( "Collision Type:" );
		ImGui::SameLine();
		static int collisionGroup = 0;
		if ( ImGui::RadioButton( "AABB", &collisionGroup, 0 ) )
			useAABB = true;
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Circle", &collisionGroup, 1 ) )
			useAABB = false;

		if ( ImGui::CollapsingHeader( "Cube Properties" ) )
		{
			// update all cube properties
			ImGui::Text( "Drag Type:" );
			ImGui::SameLine();
			static int dragFactorGroup = 0;
			static bool dragLaminar = true;
			if ( ImGui::RadioButton( "Laminar", &dragFactorGroup, 0 ) )
				dragLaminar = true;
			ImGui::SameLine();
			if ( ImGui::RadioButton( "Turbulent", &dragFactorGroup, 1 ) )
				dragLaminar = false;

			static float dragFactor = cubes[0]->GetParticleModel()->GetDragFactor();
			ImGui::SliderFloat( "Drag Factor", &dragFactor, 0.0f, 10.0f, "%1.f" );

			static float friction = cubes[0]->GetParticleModel()->GetFriction();
			ImGui::SliderFloat( "Friction", &friction, 0.0f, 0.0002f, "%.7f", 10 );

			// update individual cube properties
			for ( uint32_t i = 0; i < cubes.size(); i++ )
			{
				if ( ImGui::CollapsingHeader( cubes[i]->GetID().c_str(), ImGuiTreeNodeFlags_OpenOnDoubleClick ) )
				{
					float mass = cubes[i]->GetParticleModel()->GetMass();
					ImGui::SliderFloat( std::string( "Mass##" ).append( std::to_string( i ) ).c_str(), &mass, 5.0f, 20.0f, "%1.f" );
					cubes[i]->GetParticleModel()->SetMass( mass );

					v3df acceleration = cubes[i]->GetParticleModel()->GetAcceleration();
					ImGui::Text( std::string( "Acceleration: " )
						.append( std::string( "x(" ).append( std::to_string( acceleration.x ).c_str() ).append( "), " ).c_str() )
						.append( std::string( "y(" ).append( std::to_string( acceleration.y ).c_str() ).append( "), " ).c_str() )
						.append( std::string( "z(" ).append( std::to_string( acceleration.z ).c_str() ).append( ") " ).c_str() ).c_str()
					);

					v3df velocity = cubes[i]->GetParticleModel()->GetVelocity();
					ImGui::Text( std::string( "Velocity:     " )
						.append( std::string( "x(" ).append( std::to_string( velocity.x ).c_str() ).append( "), " ).c_str() )
						.append( std::string( "y(" ).append( std::to_string( velocity.y ).c_str() ).append( "), " ).c_str() )
						.append( std::string( "z(" ).append( std::to_string( velocity.z ).c_str() ).append( ") " ).c_str() ).c_str()
					);

					v3df netForce = cubes[i]->GetParticleModel()->GetNetForce();
					ImGui::Text( std::string( "Net Force:    " )
						.append( std::string( "x(" ).append( std::to_string( netForce.x ).c_str() ).append( "), " ).c_str() )
						.append( std::string( "y(" ).append( std::to_string( netForce.y ).c_str() ).append( "), " ).c_str() )
						.append( std::string( "z(" ).append( std::to_string( netForce.z ).c_str() ).append( ") " ).c_str() ).c_str()
					);
				}
				cubes[i]->GetParticleModel()->SetFriction( friction );
				cubes[i]->GetParticleModel()->SetDragFactor( dragFactor );
				cubes[i]->GetParticleModel()->SetLaminar( dragLaminar );
			}
		}

		ImGui::PopItemWidth();
	}
	ImGui::End();
}

void Level1::SpawnInstructionWindow()
{
	if ( ImGui::Begin( "Scene Instructions", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		if ( ImGui::CollapsingHeader( "Cube Controls", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ImGui::Text( "12345   ->  Select Cube" );
			ImGui::Text( "UP      ->  Forward" );
			ImGui::Text( "DOWN    ->  Backward" );
			ImGui::Text( "LEFT    ->  Left" );
			ImGui::Text( "RIGHT   ->  Right" );
			ImGui::Text( "HOME    ->  Up" );
			ImGui::Text( "R       ->  Reset Position" );
			ImGui::Text( "F       ->  Reset Forces" );
		}
	}
	ImGui::End();
}