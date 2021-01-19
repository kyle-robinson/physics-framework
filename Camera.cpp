#include "Camera.h"

Camera::Camera( v3df position, v3df at, v3df up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth ) :
	_eye( position ), _at( at ), _up( up ),
	_windowWidth( windowWidth ), _windowHeight( windowHeight ),
	_nearDepth( nearDepth ), _farDepth( farDepth )
{
	Update();
}

void Camera::Update()
{
    // Initialize the view matrix

	v4df eye( _eye[0], _eye[1], _eye[2], 1.0f );
	v4df at( _at[0], _at[1], _at[2], 1.0f );
	v4df up( _up[0], _up[1], _up[2], 0.0f );

	XMVECTOR EyeVector = XMLoadFloat4( &reinterpret_cast<XMFLOAT4&>( eye ) );
	XMVECTOR AtVector = XMLoadFloat4( &reinterpret_cast<XMFLOAT4&>( at ) );
	XMVECTOR UpVector = XMLoadFloat4( &reinterpret_cast<XMFLOAT4&>( up ) );

	XMStoreFloat4x4( &_view, XMMatrixLookAtLH( EyeVector, AtVector, UpVector ) );

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