#include "stdafx.h"
#include "Level2.h"
#include <imgui/imgui.h>

void Level2::Initialize( Graphics& gfx )
{
	LevelManager::Initialize( gfx );

	// setup particle system
	particles.resize( PARTICLE_COUNT );
	for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
	{
		particles[i] = std::make_unique<Particle>( "Particle " + std::to_string( i + 1 ) );
		particles[i]->GetAppearance()->SetTextureRV( textureLava.Get() );
		particles[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		particles[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}
}

void Level2::Update( Mouse& mouse, Keyboard& keyboard, float dt )
{
	UpdateInput( mouse, keyboard, dt );

	// particle movement
	for ( uint32_t i = 0; i < particles.size(); i++ )
	{
		float randomNum = fmod( static_cast< float >( rand() ), xDist + 1.0f ) - ( xDist / 2.0f );
		if ( particles[i]->GetStartTimer() < 0 )
			particles[i]->GetParticleModel()->Move( fmod( randomNum, i ), 1.5f, 0.0f );
	}
	
	// update particles
	if ( useParticles )
		for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
			particles[i]->Update( dt );

	LevelManager::Update( mouse, keyboard, dt );
}

void Level2::UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt )
{
	LevelManager::UpdateInput( mouse, keyboard, dt );
}

void Level2::Render( Graphics& gfx )
{
	LevelManager::BeginRender( gfx );

	// Render Particles
	cb_vs_matrix.data.UseLighting = 1.0f;
	for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( particles[i]->GetTransform()->GetTransformMatrix() );
		GetContext( gfx )->PSSetShaderResources( 0, 1, particles[i]->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		if ( useParticles ) particles[i]->Draw( GetContext( gfx ) );
	}

	SpawnControlWindow( particles );

	LevelManager::EndRender( gfx );
}

void Level2::SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec )
{
	if ( ImGui::Begin( "Particle Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		ImGui::Checkbox( " Enable Particles", &useParticles );

		float size = vec[0]->GetMaxSize();
		ImGui::SliderFloat( "Size", &size, 0.001f, 0.02f );
		ImGui::SliderFloat( "Distribution", &xDist, 1.0f, 5.0f, "%1.f" );

		for ( uint32_t i = 0; i < vec.size(); i++ )
			vec[i]->SetMaxSize( size );
	}
	ImGui::End();
}