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

void Level2::Update( float dt )
{
	// Update Particles
	if ( useParticles )
		for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
			particles[i]->Update( dt );

	LevelManager::Update( dt );
}

void Level2::Render( Graphics& gfx )
{
	LevelManager::BeginRender( gfx );

	// Render Particles
	for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( particles[i]->GetTransform()->GetTransformMatrix() );
		GetContext( gfx )->PSSetShaderResources( 0, 1, particles[i]->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		if ( useParticles )
			particles[i]->Draw( GetContext( gfx ) );
	}

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