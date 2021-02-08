#include "stdafx.h"
#include "Camera.h"

Camera::Camera( v3df position, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth ) :
	_windowWidth( windowWidth ), _windowHeight( windowHeight ),
	_nearDepth( nearDepth ), _farDepth( farDepth )
{
	this->position = { position[0], position[1], position[2] };
	posVector = XMLoadFloat3( &this->position );
	rotation = { 0.0f, 0.0f, 0.0f };
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
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

// DIRECTION VECTORS
const XMVECTOR& Camera::GetForwardVector( bool omitY ) noexcept
{
	return omitY ? vec_forward_noY : vec_forward;
}

const XMVECTOR& Camera::GetBackwardVector( bool omitY ) noexcept
{
	return omitY ? vec_backward_noY : vec_backward;
}

const XMVECTOR& Camera::GetLeftVector( bool omitY ) noexcept
{
	return omitY ? vec_left_noY : vec_left;
}

const XMVECTOR& Camera::GetRightVector( bool omitY ) noexcept
{
	return omitY ? vec_right_noY : vec_right;
}

const XMVECTOR& Camera::GetUpVector() noexcept
{
	return DEFAULT_UP_VECTOR;
}

// CAMERA POSITION
void Camera::SetPosition( float x, float y, float z ) noexcept
{
	SetPosition( XMFLOAT3( x, y, z ) );
}

void Camera::SetPosition( const XMVECTOR& pos ) noexcept
{
	XMStoreFloat3( &position, pos );
	posVector = pos;
	UpdateMatrix();
}

void Camera::SetPosition( const XMFLOAT3& pos ) noexcept
{
	position = pos;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

void Camera::AdjustPosition( float x, float y, float z ) noexcept
{
	AdjustPosition( XMFLOAT3( x, y, z ) );
}

void Camera::AdjustPosition( const XMVECTOR& pos ) noexcept
{
	posVector += pos;
	XMStoreFloat3( &position, posVector );
	UpdateMatrix();
}

void Camera::AdjustPosition( const XMFLOAT3& pos ) noexcept
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

// CAMERA ROTATION
void Camera::SetRotation( float x, float y, float z ) noexcept
{
	SetRotation( XMFLOAT3( x, y, z ) );
}

void Camera::SetRotation( const XMVECTOR& rot ) noexcept
{
	rotVector = rot;
	XMStoreFloat3( &rotation, rot );
	UpdateMatrix();
}

void Camera::SetRotation( const XMFLOAT3& rot ) noexcept
{
	rotation = rot;
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void Camera::AdjustRotation( float x, float y, float z ) noexcept
{
	AdjustRotation( XMFLOAT3( x, y, z ) );
}

void Camera::AdjustRotation( const XMVECTOR& rot ) noexcept
{
	rotVector += rot;
	XMStoreFloat3( &rotation, rotVector );
	UpdateMatrix();
}

void Camera::AdjustRotation( const XMFLOAT3& rot ) noexcept
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;

	if ( rotation.x >= XMConvertToRadians( 90.0f ) )
		rotation.x = XMConvertToRadians( 90.0f );

	if ( rotation.x <= XMConvertToRadians( -90.0f ) )
		rotation.x = XMConvertToRadians( -90.0f );

	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

// UPDATE MATRICES
void Camera::UpdateMatrix()
{
	// Update Camera Target
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, rotation.z );
	XMVECTOR camTarget = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, cameraRotation );
	camTarget += posVector;

	XMVECTOR upDir = XMVector3TransformCoord( DEFAULT_UP_VECTOR, cameraRotation );
	XMStoreFloat4x4( &_view, XMMatrixLookAtLH( posVector, camTarget, upDir ) );

    // Initialize the projection matrix
	XMStoreFloat4x4( &_projection, XMMatrixPerspectiveFovLH( 0.25f * XM_PI, _windowWidth / _windowHeight, _nearDepth, _farDepth ) );

	UpdateDirectionVectors();
}

void Camera::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, 0.0f );
	vec_forward = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, vecRotationMatrix );
	vec_backward = XMVector3TransformCoord( DEFAULT_BACKWARD_VECTOR, vecRotationMatrix );
	vec_left = XMVector3TransformCoord( DEFAULT_LEFT_VECTOR, vecRotationMatrix );
	vec_right = XMVector3TransformCoord( DEFAULT_RIGHT_VECTOR, vecRotationMatrix );

	XMMATRIX vecRotationMatrix_noY = XMMatrixRotationRollPitchYaw( 0.0f, rotation.y, 0.0f );
	vec_forward_noY = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, vecRotationMatrix_noY );
	vec_backward_noY = XMVector3TransformCoord( DEFAULT_BACKWARD_VECTOR, vecRotationMatrix_noY );
	vec_left_noY = XMVector3TransformCoord( DEFAULT_LEFT_VECTOR, vecRotationMatrix_noY );
	vec_right_noY = XMVector3TransformCoord( DEFAULT_RIGHT_VECTOR, vecRotationMatrix_noY );
}