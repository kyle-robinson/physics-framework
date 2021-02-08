#include "stdafx.h"
#include "Camera.h"

Camera::Camera( v3df position, v3df at, v3df up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth ) :
	_eye( position ), _at( at ), _up( up ),
	_windowWidth( windowWidth ), _windowHeight( windowHeight ),
	_nearDepth( nearDepth ), _farDepth( farDepth )
{
	Update();
	rotation = { 0.0f, 0.0f, 0.0f };
}

void Camera::Update()
{
    //float angleAroundZ = XMConvertToRadians( cameraOrbitAngleXZ );
	//float x = cameraOrbitRadius * cos( angleAroundZ );
	//float z = cameraOrbitRadius * sin( angleAroundZ );
	//this->SetPosition( { x, this->GetPosition()[1], z } );
	
	// Initialize the view matrix
	v4df eye( _eye[0], _eye[1], _eye[2], 1.0f );
	v4df at(   _at[0],  _at[1],  _at[2], 1.0f );
	v4df up(   _up[0],  _up[1],  _up[2], 0.0f );

	XMVECTOR EyeVector = XMLoadFloat4( &reinterpret_cast<XMFLOAT4&>( eye ) );
	XMVECTOR AtVector =  XMLoadFloat4( &reinterpret_cast<XMFLOAT4&>( at  ) );
	XMVECTOR UpVector =  XMLoadFloat4( &reinterpret_cast<XMFLOAT4&>( up  ) );

	// Update Camera Target
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYaw( rotation[0], rotation[1], rotation[2] );
	XMVECTOR camTarget = XMVector3TransformCoord( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), cameraRotation );
	camTarget += XMVectorSet( _eye[0], _eye[1], _eye[2], 0.0f );

	XMVECTOR upDir = XMVector3TransformCoord( XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ), cameraRotation );
	XMStoreFloat4x4( &_view, XMMatrixLookAtLH( EyeVector, camTarget, upDir ) );

    // Initialize the projection matrix
	XMStoreFloat4x4( &_projection, XMMatrixPerspectiveFovLH( 0.25f * XM_PI, _windowWidth / _windowHeight, _nearDepth, _farDepth ) );
}

void Camera::Reshape( FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth )
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

XMFLOAT4X4 Camera::GetViewProjection() const 
{ 
	XMMATRIX view = XMLoadFloat4x4( &_view );
	XMMATRIX projection = XMLoadFloat4x4( &_projection );
	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4( &viewProj, view * projection );

	return viewProj;
}

// CAMERA ROTATION
void Camera::AdjustRotation( float x, float y, float z ) noexcept
{
	rotation[0] += x;
	rotation[1] += y;
	rotation[2] += z;

	if ( rotation[0] >= XMConvertToRadians( 90.0f ) )
		rotation[0] = XMConvertToRadians( 90.0f );

	if ( rotation[0] <= XMConvertToRadians( -90.0f ) )
		rotation[0] = XMConvertToRadians( -90.0f );
}

// CAMERA MOVEMENT
void Camera::MoveFoward( std::unique_ptr<Camera>& cam )
{
	cam->cameraOrbitRadius = max( cam->cameraOrbitRadiusMin, cam->cameraOrbitRadius - ( cam->cameraSpeed * 0.2f ) );
}

void Camera::MoveBackward( std::unique_ptr<Camera>& cam )
{
	cam->cameraOrbitRadius = min( cam->cameraOrbitRadiusMax, cam->cameraOrbitRadius + ( cam->cameraSpeed * 0.2f ) );
}

void Camera::MoveRight( std::unique_ptr<Camera>& cam )
{
	cam->cameraOrbitAngleXZ -= cam->cameraSpeed;
}

void Camera::MoveLeft( std::unique_ptr<Camera>& cam )
{
	cam->cameraOrbitAngleXZ += cam->cameraSpeed;
}