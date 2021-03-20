#pragma once
#ifndef MATRIX4_H
#define MATRIX4_H

#include "Vector.h"
#include "Quaternion.h"

class Matrix4
{
public:
	float _data[12];

	v3df operator*( const v3df& vector ) const noexcept
	{
		return {
			vector.x * _data[0] + vector.y * _data[1] + vector.z * _data[2] + _data[3],
			vector.x * _data[4] + vector.y * _data[5] + vector.z * _data[6] + _data[7],
			vector.x * _data[8] + vector.y * _data[9] + vector.z * _data[10] + _data[11]
		};
	}

	Matrix4() {
		_data[1] = _data[2] = _data[3] =
		_data[4] = _data[6] = _data[7] =
		_data[8] = _data[9] = _data[11] =0.0f;
		_data[0] = _data[5] = _data[10] = 1.0f;
	}

	v3df Transform( const v3df& vector ) const noexcept { return ( *this ) * vector; }
	v3df GetAxisVector( int i ) const noexcept { return v3df( _data[i], _data[i + 4], _data[i + 8] ); }

	Matrix4 operator*( const Matrix4& matrix ) const noexcept
	{
		Matrix4 result;
		result._data[0] = matrix._data[0] * _data[0] + matrix._data[4] * _data[1] + matrix._data[8] * _data[2];
		result._data[4] = matrix._data[0] * _data[4] + matrix._data[4] * _data[5] + matrix._data[8] * _data[6];
		result._data[8] = matrix._data[0] * _data[8] + matrix._data[4] * _data[9] + matrix._data[8] * _data[10];
		result._data[1] = matrix._data[1] * _data[0] + matrix._data[5] * _data[1] + matrix._data[9] * _data[2];
		result._data[5] = matrix._data[1] * _data[4] + matrix._data[5] * _data[5] + matrix._data[9] * _data[6];
		result._data[9] = matrix._data[1] * _data[8] + matrix._data[5] * _data[9] + matrix._data[9] * _data[10];
		result._data[2] = matrix._data[2] * _data[0] + matrix._data[6] * _data[1] + matrix._data[10] * _data[2];
		result._data[6] = matrix._data[2] * _data[4] + matrix._data[6] * _data[5] + matrix._data[10] * _data[6];
		result._data[10] = matrix._data[2] * _data[8] + matrix._data[6] * _data[9] + matrix._data[10] * _data[10];
		result._data[3] = matrix._data[3] * _data[0] + matrix._data[7] * _data[1] + matrix._data[11] * _data[2] + _data[3];
		result._data[7] = matrix._data[3] * _data[4] + matrix._data[7] * _data[5] + matrix._data[11] * _data[6] + _data[7];
		result._data[11] = matrix._data[3] * _data[8] + matrix._data[7] * _data[9] + matrix._data[11] * _data[10] + _data[11];
		return result;
	}


	float GetDeterminant() const noexcept
	{
		return _data[8] * _data[5] * _data[2] +
			   _data[4] * _data[9] * _data[2] +
			   _data[8] * _data[1] * _data[6] -
			   _data[0] * _data[9] * _data[6] -
			   _data[4] * _data[1] * _data[10] +
			   _data[0] * _data[5] * _data[10];
	}

	void SetInverse( const Matrix4& m ) noexcept
	{
		float det = GetDeterminant();
		if ( det == 0 ) return;
		det = 1.0f / det;
		_data[0] = ( -m._data[9] * m._data[6] + m._data[5] * m._data[10] ) * det;
		_data[4] = ( m._data[8] * m._data[6] - m._data[4] * m._data[10] ) * det;
		_data[8] = ( -m._data[8] * m._data[5] + m._data[4] * m._data[9] * m._data[15] ) * det;
		_data[1] = ( m._data[9] * m._data[2] - m._data[1] * m._data[10] ) * det;
		_data[5] = ( -m._data[8] * m._data[2] + m._data[0] * m._data[10] ) * det;
		_data[9] = ( m._data[8] * m._data[1] - m._data[0] * m._data[9] * m._data[15] ) * det;
		_data[2] = ( -m._data[5] * m._data[2] + m._data[1] * m._data[6] * m._data[15] ) * det;
		_data[6] = ( +m._data[4] * m._data[2] - m._data[0] * m._data[6] * m._data[15] ) * det;
		_data[10] = ( -m._data[4] * m._data[1] + m._data[0] * m._data[5] * m._data[15] ) * det;
		
		_data[3] = ( m._data[9] * m._data[6] * m._data[3]
			- m._data[5] * m._data[10] * m._data[3]
			- m._data[9] * m._data[2] * m._data[7]
			+ m._data[1] * m._data[10] * m._data[7]
			+ m._data[5] * m._data[2] * m._data[11]
			- m._data[1] * m._data[6] * m._data[11] ) * det;
		
		_data[7] = ( -m._data[8] * m._data[6] * m._data[3]
			+ m._data[4] * m._data[10] * m._data[3]
			+ m._data[8] * m._data[2] * m._data[7]
			- m._data[0] * m._data[10] * m._data[7]
			- m._data[4] * m._data[2] * m._data[11]
			+ m._data[0] * m._data[6] * m._data[11] ) * det;
		
		_data[11] = ( m._data[8] * m._data[5] * m._data[3]
			- m._data[4] * m._data[9] * m._data[3]
			- m._data[8] * m._data[1] * m._data[7]
			+ m._data[0] * m._data[9] * m._data[7]
			+ m._data[4] * m._data[1] * m._data[11]
			- m._data[0] * m._data[5] * m._data[11] ) * det;
	}

	Matrix4 Inverse() const noexcept
	{
		Matrix4 result;
		result.SetInverse( *this );
		return result;
	}

	void Invert() noexcept { SetInverse( *this ); }

	void SetOrientationAndPosition( const Quaternion& q, const v3df& pos ) noexcept
	{
		_data[0] = 1 - ( 2 * q.j * q.j + 2 * q.k * q.k );
		_data[1] = 2 * q.i * q.j + 2 * q.k * q.r;
		_data[2] = 2 * q.i * q.k - 2 * q.j * q.r;
		_data[3] = pos.x;
		_data[4] = 2 * q.i * q.j - 2 * q.k * q.r;
		_data[5] = 1 - ( 2 * q.i * q.i + 2 * q.k * q.k );
		_data[6] = 2 * q.j * q.k + 2 * q.i * q.r;
		_data[7] = pos.y;
		_data[8] = 2 * q.i * q.k + 2 * q.j * q.r;
		_data[9] = 2 * q.j * q.k - 2 * q.i * q.r;
		_data[10] = 1 - ( 2 * q.i * q.i + 2 * q.j * q.j );
		_data[11] = pos.z;
	}

	v3df LocalToWorld( const v3df& local, const Matrix4& transform ) const noexcept{ return transform.Transform( local ); }
	v3df WorldToLocal( const v3df& world, const Matrix4& transform ) const noexcept { return transform.TransformInverse( world ); }

	v3df TransformInverse( const v3df& vector ) const noexcept
	{
		v3df tmp = vector;
		tmp.x -= _data[3];
		tmp.y -= _data[7];
		tmp.z -= _data[11];

		return {
			tmp.x * _data[0] + tmp.y * _data[4] + tmp.z * _data[8],
			tmp.x * _data[1] + tmp.y * _data[5] + tmp.z * _data[9],
			tmp.x * _data[2] + tmp.y * _data[6] + tmp.z * _data[10]
		};
	}

	v3df TransformDirection( const v3df& vector ) const noexcept
	{
		return {
			vector.x * _data[0] + vector.y * _data[1] + vector.z * _data[2],
			vector.x * _data[4] + vector.y * _data[5] + vector.z * _data[6],
			vector.x * _data[8] + vector.y * _data[9] + vector.z * _data[10]
		};
	}

	v3df TransformInverDirection( const v3df& vector ) const noexcept
	{
		return {
			vector.x * _data[0] + vector.y * _data[4] + vector.z * _data[8],
			vector.x * _data[1] + vector.y * _data[5] + vector.z * _data[9],
			vector.x * _data[2] + vector.y * _data[6] + vector.z * _data[10]
		};
	}

	v3df LocalToWorldDirection( const v3df& local, const Matrix4& transform ) const noexcept { return transform.TransformDirection( local ); }
	v3df WorldToLocalDirection( const v3df& world, const Matrix4& transform ) const noexcept { return transform.TransformInverDirection( world ); }

	void DirectXArray( float array[16] ) const noexcept
	{
		array[0] = _data[0];
		array[1] = _data[4];
		array[2] = _data[8];
		array[3] = 0.0f;

		array[4] = _data[1];
		array[5] = _data[5];
		array[6] = _data[9];
		array[7] = 0.0f;

		array[8] = _data[2];
		array[9] = _data[6];
		array[10] = _data[10];
		array[11] = 0.0f;

		array[12] = _data[3];
		array[13] = _data[7];
		array[14] = _data[11];
		array[15] = 1.0f;
	}
};

static inline void CalculateTransformMatrix( Matrix4& transformMatrix, const v3df& position, const Quaternion& orientation )
{
	transformMatrix._data[0] = 1 - 2 * orientation.j * orientation.j - 2 * orientation.k * orientation.k;
	transformMatrix._data[1] = 2 * orientation.i * orientation.j - 2 * orientation.r * orientation.k;
	transformMatrix._data[2] = 2 * orientation.i * orientation.k + 2 * orientation.r * orientation.j;
	transformMatrix._data[3] = position.x;
	transformMatrix._data[4] = 2 * orientation.i * orientation.j + 2 * orientation.r * orientation.k;
	transformMatrix._data[5] = 1 - 2 * orientation.i * orientation.i - 2 * orientation.k * orientation.k;
	transformMatrix._data[6] = 2 * orientation.j * orientation.k - 2 * orientation.r * orientation.i;
	transformMatrix._data[7] = position.y;
	transformMatrix._data[8] = 2 * orientation.i * orientation.k - 2 * orientation.r * orientation.j;
	transformMatrix._data[9] = 2 * orientation.j * orientation.k + 2 * orientation.r * orientation.i;
	transformMatrix._data[10] = 1 - 2 * orientation.i * orientation.i - 2 * orientation.j * orientation.j;
	transformMatrix._data[11] = position.z;
}

#endif