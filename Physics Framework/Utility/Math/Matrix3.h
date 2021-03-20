#pragma once
#ifndef MATRIX3_H
#define MATRIX3_H

#include "Matrix4.h"

class Matrix3
{
public:
	float _data[9];

	Matrix3()
	{
		_data[0] = _data[1] = _data[2] =
		_data[3] = _data[4] = _data[5] =
		_data[6] = _data[7] = _data[8] =
		0.0f;
	}

	Matrix3(
		float e0, float e1, float e2,
		float e3, float e4, float e5,
		float e6, float e7, float e8 )
	{
		_data[0] = e0;
		_data[1] = e1;
		_data[2] = e2;
		_data[3] = e3;
		_data[4] = e4;
		_data[5] = e5;
		_data[6] = e6;
		_data[7] = e7;
		_data[8] = e8;
	}

	void SetBlockInertiaTensor( const v3df& halfSizes, float mass )
	{
		v3df squares = halfSizes.ComponentProduct( halfSizes );
		SetInertiaTensorCoeffs(
			0.3f * mass * ( squares.y + squares.z ),
			0.3f * mass * ( squares.x + squares.z ),
			0.3f * mass * ( squares.x + squares.y )
		);
	}

	void SetInertiaTensorCoeffs(
		float ix, float iy, float iz,
		float ixy = 0.0f, float ixz = 0.0f, float iyz = 0.0f )
	{
		_data[0] = ix;
		_data[1] = _data[3] = -ixy;
		_data[2] = _data[6] = -ixz;
		_data[4] = iy;
		_data[5] = _data[7] = -iyz;
		_data[8] = iz;
	}

	void SetComponents( const v3df& a, const v3df& b, const v3df& c )
	{
		_data[0] = a.x;
		_data[1] = b.x;
		_data[2] = c.x;
		_data[3] = a.y;
		_data[4] = b.y;
		_data[5] = c.y;
		_data[6] = a.z;
		_data[7] = b.z;
		_data[8] = c.z;
	}

	void SetSkewSymmetric( const v3df vector )
	{
		_data[0] = _data[4] = _data[8] = 0.0f;
		_data[1] = -vector.z;
		_data[2] = vector.y;
		_data[3] = vector.z;
		_data[5] = -vector.x;
		_data[6] = -vector.y;
		_data[7] = vector.x;
	}

	void operator+=( const Matrix3& o )
	{
		_data[0] += o._data[0];
		_data[1] += o._data[1];
		_data[2] += o._data[2];
		_data[3] += o._data[3];
		_data[4] += o._data[4];
		_data[5] += o._data[5];
		_data[6] += o._data[6];
		_data[7] += o._data[7];
		_data[8] += o._data[8];
	}

	void operator*=( const float scalar )
	{
		_data[0] *= scalar;
		_data[1] *= scalar;
		_data[2] *= scalar;
		_data[3] *= scalar;
		_data[4] *= scalar;
		_data[5] *= scalar;
		_data[6] *= scalar;
		_data[7] *= scalar;
		_data[8] *= scalar;
	}

	v3df operator*( const v3df& vector ) const noexcept
	{
		return {
			vector.x * _data[0] + vector.y * _data[1] + vector.z * _data[2],
			vector.x * _data[3] + vector.y * _data[4] + vector.z * _data[5],
			vector.x * _data[6] + vector.y * _data[7] + vector.z * _data[8]
		};
	}

	v3df Transform( const v3df& vector ) const noexcept { return ( *this ) * vector; }

	v3df TransformTranspose( const v3df& vector ) const noexcept
	{
		return {
			vector.x * _data[0] + vector.y * _data[3] + vector.z * _data[6],
			vector.x * _data[1] + vector.y * _data[4] + vector.z * _data[7],
			vector.x * _data[2] + vector.y * _data[5] + vector.z * _data[8]
		};
	}

	Matrix3 operator*( const Matrix3& matrix ) const noexcept
	{
		return Matrix3(
			_data[0] * matrix._data[0] + _data[1] * matrix._data[3] + _data[2] * matrix._data[6],
			_data[0] * matrix._data[1] + _data[1] * matrix._data[4] + _data[2] * matrix._data[7],
			_data[0] * matrix._data[2] + _data[1] * matrix._data[5] + _data[2] * matrix._data[8],
			_data[3] * matrix._data[0] + _data[4] * matrix._data[3] + _data[5] * matrix._data[6],
			_data[3] * matrix._data[1] + _data[4] * matrix._data[4] + _data[5] * matrix._data[7],
			_data[3] * matrix._data[2] + _data[4] * matrix._data[5] + _data[5] * matrix._data[8],
			_data[6] * matrix._data[0] + _data[7] * matrix._data[3] + _data[8] * matrix._data[6],
			_data[6] * matrix._data[1] + _data[7] * matrix._data[4] + _data[8] * matrix._data[7],
			_data[6] * matrix._data[2] + _data[7] * matrix._data[5] + _data[8] * matrix._data[8]
		);
	}

	void operator*=( const Matrix3& matrix ) noexcept
	{
		float t1 = _data[0] * matrix._data[0] + _data[1] * matrix._data[3] + _data[2] * matrix._data[6];
		float t2 = _data[0] * matrix._data[1] + _data[1] * matrix._data[4] + _data[2] * matrix._data[7];
		float t3 = _data[0] * matrix._data[2] + _data[1] * matrix._data[5] + _data[2] * matrix._data[8];
		_data[0] = t1;
		_data[1] = t2;
		_data[2] = t3;

		t1 = _data[3] * matrix._data[0] + _data[4] * matrix._data[3] + _data[5] * matrix._data[6];
		t2 = _data[3] * matrix._data[1] + _data[4] * matrix._data[4] + _data[5] * matrix._data[7];
		t3 = _data[3] * matrix._data[2] + _data[4] * matrix._data[5] + _data[5] * matrix._data[8];
		_data[3] = t1;
		_data[4] = t2;
		_data[5] = t3;

		t1 = _data[6] * matrix._data[0] + _data[7] * matrix._data[3] + _data[8] * matrix._data[6];
		t2 = _data[6] * matrix._data[1] + _data[7] * matrix._data[4] + _data[8] * matrix._data[7];
		t3 = _data[6] * matrix._data[2] + _data[7] * matrix._data[5] + _data[8] * matrix._data[8];
		_data[6] = t1;
		_data[7] = t2;
		_data[8] = t3;
	}

	void SetInverse( const Matrix3& m ) noexcept
	{
		float e1 = m._data[0] * m._data[4];
		float e2 = m._data[0] * m._data[5];
		float e3 = m._data[1] * m._data[3];
		float e4 = m._data[2] * m._data[3];
		float e5 = m._data[1] * m._data[6];
		float e6 = m._data[2] * m._data[6];

		// calculate the determinant
		float det =
		(
			e1 * m._data[8] -
			e2 * m._data[7] -
			e3 * m._data[8] +
			e4 * m._data[7] +
			e5 * m._data[5] -
			e6 * m._data[4]
		);

		if ( det == 0.0f ) return;
		float inverseDet = 1.0f / det;

		_data[0] =  ( m._data[4] * m._data[8] - m._data[5] * m._data[7] ) * inverseDet;
		_data[1] = -( m._data[1] * m._data[8] - m._data[2] * m._data[7] ) * inverseDet;
		_data[2] =  ( m._data[1] * m._data[5] - m._data[2] * m._data[4] ) * inverseDet;
		_data[3] = -( m._data[3] * m._data[8] - m._data[5] * m._data[6] ) * inverseDet;
		_data[4] =  ( m._data[0] * m._data[8] - e6 ) * inverseDet;
		_data[5] = -( e2 - e4 ) * inverseDet;
		_data[6] =  ( m._data[3] * m._data[7] - m._data[4] * m._data[6] ) * inverseDet;
		_data[7] = -( m._data[0] * m._data[7] - e5 ) * inverseDet;
		_data[8] =  ( e1 - e3 ) * inverseDet;
	}

	Matrix3 Inverse() const noexcept
	{
		Matrix3 result;
		result.SetInverse( *this );
		return result;
	}

	void Invert() noexcept { SetInverse( *this ); }

	void SetTranspose( const Matrix3& matrix )
	{
		_data[0] = matrix._data[0];
		_data[1] = matrix._data[3];
		_data[2] = matrix._data[6];
		_data[3] = matrix._data[1];
		_data[4] = matrix._data[4];
		_data[5] = matrix._data[7];
		_data[6] = matrix._data[2];
		_data[7] = matrix._data[5];
		_data[8] = matrix._data[8];
	}

	Matrix3 Transpose() const noexcept
	{
		Matrix3 result;
		result.SetTranspose( *this );
		return result;
	}

	void SetOrientation( const Quaternion& q )
	{
		_data[0] = 1 - ( 2 * q.j * q.j + 2 * q.k * q.k );
		_data[1] = 2 * q.i * q.j + 2 * q.k * q.r;
		_data[2] = 2 * q.i * q.k - 2 * q.j * q.r;
		_data[3] = 2 * q.i * q.j - 2 * q.k * q.r;
		_data[4] = 1 - ( 2 * q.i * q.i + 2 * q.k * q.k );
		_data[5] = 2 * q.j * q.k + 2 * q.i * q.r;
		_data[6] = 2 * q.i * q.k + 2 * q.j * q.r;
		_data[7] = 2 * q.j * q.k - 2 * q.i * q.r;
		_data[8] = 1 - ( 2 * q.i * q.i + 2 * q.j * q.j );
	}
};

static inline void CalculateInertiaTensor( Matrix3& iitWorld, const Quaternion& q, const Matrix3& iitBody, const Matrix4& rotmat )
{
	float t4 = rotmat._data[0] * iitBody._data[0] + rotmat._data[1] * iitBody._data[3] + rotmat._data[2] * iitBody._data[6];
	float t9 = rotmat._data[0] * iitBody._data[1] + rotmat._data[1] * iitBody._data[4] + rotmat._data[2] * iitBody._data[7];
	float t14 = rotmat._data[0] * iitBody._data[2] + rotmat._data[1] * iitBody._data[5] + rotmat._data[2] * iitBody._data[8];
	float t28 = rotmat._data[4] * iitBody._data[0] + rotmat._data[5] * iitBody._data[3] + rotmat._data[6] * iitBody._data[6];
	float t33 = rotmat._data[4] * iitBody._data[1] + rotmat._data[5] * iitBody._data[4] + rotmat._data[6] * iitBody._data[7];
	float t38 = rotmat._data[4] * iitBody._data[2] + rotmat._data[5] * iitBody._data[5] + rotmat._data[6] * iitBody._data[8];
	float t52 = rotmat._data[8] * iitBody._data[0] + rotmat._data[9] * iitBody._data[3] + rotmat._data[10] * iitBody._data[6];
	float t57 = rotmat._data[8] * iitBody._data[1] + rotmat._data[9] * iitBody._data[4] + rotmat._data[10] * iitBody._data[7];
	float t62 = rotmat._data[8] * iitBody._data[2] + rotmat._data[9] * iitBody._data[5] + rotmat._data[10] * iitBody._data[8];

	iitWorld._data[0] = t4 * rotmat._data[0] + t9 * rotmat._data[1] + t14 * rotmat._data[2];
	iitWorld._data[1] = t4 * rotmat._data[4] + t9 * rotmat._data[5] + t14 * rotmat._data[6];
	iitWorld._data[2] = t4 * rotmat._data[8] + t9 * rotmat._data[9] + t14 * rotmat._data[10];
	iitWorld._data[3] = t28 * rotmat._data[0] + t33 * rotmat._data[1] + t38 * rotmat._data[2];
	iitWorld._data[4] = t28 * rotmat._data[4] + t33 * rotmat._data[5] + t38 * rotmat._data[6];
	iitWorld._data[5] = t28 * rotmat._data[8] + t33 * rotmat._data[9] + t38 * rotmat._data[10];
	iitWorld._data[6] = t52 * rotmat._data[0] + t57 * rotmat._data[1] + t62 * rotmat._data[2];
	iitWorld._data[7] = t52 * rotmat._data[4] + t57 * rotmat._data[5] + t62 * rotmat._data[6];
	iitWorld._data[8] = t52 * rotmat._data[8] + t57 * rotmat._data[9] + t62 * rotmat._data[10];
}

#endif