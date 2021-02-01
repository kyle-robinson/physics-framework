#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

/**
* Holds a three degree of freedom orientation.
*
* Quaternions have
* several mathematical properties that make them useful for
* representing orientations, but require four items of data to
* hold the three degrees of freedom. These four items of data can
* be viewed as the coefficients of a complex number with three
* imaginary parts. The mathematics of the quaternion is then
* defined and is roughly correspondent to the math of 3D
* rotations. A quaternion is only a valid rotation if it is
* normalised: i.e. it has a length of 1.
*
* @note Angular velocity and acceleration can be correctly
* represented as vectors. Quaternions are only needed for
* orientation.
*/

#include <float.h>
#include <math.h>
#include <directxmath.h>
#include <d3d11_1.h>
#include "Vector.h"

using namespace DirectX;

class Quaternion
{
public:
	/**
	* Holds the real component of the quaternion.
	*/
	float r;

	/**
	* Holds the first complex component of the
	* quaternion.
	*/
	float i;

	/**
	* Holds the second complex component of the
	* quaternion.
	*/
	float j;

	/**
	* Holds the third complex component of the
	* quaternion.
	*/
	float k;

	/**
	* The default constructor creates a quaternion representing
	* a zero rotation.
	*/
	Quaternion() : r(1), i(0), j(0), k(0) {}

	/**
	* The explicit constructor creates a quaternion with the given
	* components.
	*
	* @param r The real component of the rigid body's orientation
	* quaternion.
	*
	* @param i The first complex component of the rigid body's
	* orientation quaternion.
	*
	* @param j The second complex component of the rigid body's
	* orientation quaternion.
	*
	* @param k The third complex component of the rigid body's
	* orientation quaternion.
	*
	* @note The given orientation does not need to be normalised,
	* and can be zero. This function will not alter the given
	* values, or normalise the quaternion. To normalise the
	* quaternion (and make a zero quaternion a legal rotation),
	* use the normalise function.
	*
	* @see normalise
	*/
	Quaternion(const float r, const float i, const float j, const float k)
		: r(r), i(i), j(j), k(k)
	{
	}

	/**
	* Normalises the quaternion to unit length, making it a valid
	* orientation quaternion.
	*/
	void normalise()
	{
		float d = r*r + i*i + j*j + k*k;

		// Check for zero length quaternion, and use the no-rotation
		// quaternion in that case.
		if (d < FLT_EPSILON) 
		{
			r = 1;
			return;
		}

		d = static_cast<float>(1.0) / sqrtf(d);
		r *= d;
		i *= d;
		j *= d;
		k *= d;
	}

	/**
	* Multiplies the quaternion by the given quaternion.
	*
	* @param multiplier The quaternion by which to multiply.
	*/
	void operator *=(const Quaternion &multiplier)
	{
		Quaternion q = *this;
		r = q.r*multiplier.r - q.i*multiplier.i -
			q.j*multiplier.j - q.k*multiplier.k;
		i = q.r*multiplier.i + q.i*multiplier.r +
			q.j*multiplier.k - q.k*multiplier.j;
		j = q.r*multiplier.j + q.j*multiplier.r +
			q.k*multiplier.i - q.i*multiplier.k;
		k = q.r*multiplier.k + q.k*multiplier.r +
			q.i*multiplier.j - q.j*multiplier.i;
	}

	/**
	* Adds the given vector to this, scaled by the given amount.
	* This is used to update the orientation quaternion by a rotation
	* and time.
	*
	* @param vector The vector to add.
	*
	* @param scale The amount of the vector to add.
	*/
	void addScaledVector(v3df& vector, float scale)
	{
		Quaternion q(0,
			vector[0] * scale,
			vector[1] * scale,
			vector[2] * scale);
		q *= *this;
		r += q.r * 0.5f;
		i += q.i * 0.5f;
		j += q.j * 0.5f;
		k += q.k * 0.5f;
	}

	void rotateByVector(v3df& vector)
	{
		Quaternion q(0, vector[0], vector[1], vector[2]);
		(*this) *= q;
	}
};

/**
* Inline function that creates a transform matrix from a
* position and orientation.
*/
static inline void CalculateTransformMatrixColumnMajor(XMMATRIX &transformMatrix,
	v3df& position,
	const Quaternion &orientation)
{
	transformMatrix.r[0] = XMVectorSetX(transformMatrix.r[0], 1 - 2 * orientation.j*orientation.j - 2 * orientation.k*orientation.k);
	transformMatrix.r[0] = XMVectorSetY(transformMatrix.r[0], 2 * orientation.i*orientation.j -
		2 * orientation.r*orientation.k);
	transformMatrix.r[0] = XMVectorSetZ(transformMatrix.r[0], 2 * orientation.i*orientation.k +
		2 * orientation.r*orientation.j);
	transformMatrix.r[0] = XMVectorSetW(transformMatrix.r[0], 0.0f);

	transformMatrix.r[1] = XMVectorSetX(transformMatrix.r[1], 2 * orientation.i*orientation.j +
		2 * orientation.r*orientation.k);
	transformMatrix.r[1] = XMVectorSetY(transformMatrix.r[1], 1 - 2 * orientation.i*orientation.i -
		2 * orientation.k*orientation.k);
	transformMatrix.r[1] = XMVectorSetZ(transformMatrix.r[1], 2 * orientation.j*orientation.k -
		2 * orientation.r*orientation.i);
	transformMatrix.r[1] = XMVectorSetW(transformMatrix.r[1], 0.0f);

	transformMatrix.r[2] = XMVectorSetX(transformMatrix.r[2], 2 * orientation.i*orientation.k -
		2 * orientation.r*orientation.j);
	transformMatrix.r[2] = XMVectorSetY(transformMatrix.r[2], 2 * orientation.j*orientation.k +
		2 * orientation.r*orientation.i);
	transformMatrix.r[2] = XMVectorSetZ(transformMatrix.r[2], 1 - 2 * orientation.i*orientation.i -
		2 * orientation.j*orientation.j);
	transformMatrix.r[2] = XMVectorSetW(transformMatrix.r[2], 0.0f);

	transformMatrix.r[3] = XMVectorSetX(transformMatrix.r[3], position[0]);
	transformMatrix.r[3] = XMVectorSetY(transformMatrix.r[3], position[1]);
	transformMatrix.r[3] = XMVectorSetZ(transformMatrix.r[3], position[2]);
	transformMatrix.r[3] = XMVectorSetW(transformMatrix.r[3], 1.0f);
}

static inline void CalculateTransformMatrixRowMajor(XMMATRIX &transformMatrix,
	v3df& position,
	const Quaternion &orientation)
{
	transformMatrix.r[0] = XMVectorSetX(transformMatrix.r[0], 1 - 2 * orientation.j*orientation.j - 2 * orientation.k*orientation.k);
	transformMatrix.r[0] = XMVectorSetY(transformMatrix.r[0], 2 * orientation.i*orientation.j - 2 * orientation.r*orientation.k);
	transformMatrix.r[0] = XMVectorSetZ(transformMatrix.r[0], 2 * orientation.i*orientation.k + 2 * orientation.r*orientation.j);
	transformMatrix.r[0] = XMVectorSetW(transformMatrix.r[0], 0.0f); 

	transformMatrix.r[1] = XMVectorSetX(transformMatrix.r[1], 2 * orientation.i*orientation.j + 2 * orientation.r*orientation.k);
	transformMatrix.r[1] = XMVectorSetY(transformMatrix.r[1], 1 - 2 * orientation.i*orientation.i - 2 * orientation.k*orientation.k);
	transformMatrix.r[1] = XMVectorSetZ(transformMatrix.r[1], 2 * orientation.j*orientation.k - 2 * orientation.r*orientation.i);
	transformMatrix.r[1] = XMVectorSetW(transformMatrix.r[1], 0.0f);

	transformMatrix.r[2] = XMVectorSetX(transformMatrix.r[2], 2 * orientation.i*orientation.k - 2 * orientation.r*orientation.j);
	transformMatrix.r[2] = XMVectorSetY(transformMatrix.r[2], 2 * orientation.j*orientation.k + 2 * orientation.r*orientation.i);
	transformMatrix.r[2] = XMVectorSetZ(transformMatrix.r[2], 1 - 2 * orientation.i*orientation.i - 2 * orientation.j*orientation.j);
	transformMatrix.r[2] = XMVectorSetW(transformMatrix.r[2], 0.0f);

	transformMatrix.r[3] = XMVectorSetX(transformMatrix.r[3], position[0]);
	transformMatrix.r[3] = XMVectorSetY(transformMatrix.r[3], position[1]);
	transformMatrix.r[3] = XMVectorSetZ(transformMatrix.r[3], position[2]);
	transformMatrix.r[3] = XMVectorSetW(transformMatrix.r[3], 1.0f);

	transformMatrix = XMMatrixTranspose(transformMatrix);
}

static inline void CalculateTransformMatrix(XMMATRIX& transformMatrix,
	v3df& position,
	const Quaternion& orientation)
{
	transformMatrix.r[0] = XMVectorSetX(transformMatrix.r[0], 1 - 2 * orientation.j * orientation.j - 2 * orientation.k * orientation.k);
	transformMatrix.r[0] = XMVectorSetY(transformMatrix.r[0], 2 * orientation.i * orientation.j - 2 * orientation.r * orientation.k);
	transformMatrix.r[0] = XMVectorSetZ(transformMatrix.r[0], 2 * orientation.i * orientation.k + 2 * orientation.r * orientation.j);
	transformMatrix.r[0] = XMVectorSetW(transformMatrix.r[0], 0.0f);

	transformMatrix.r[1] = XMVectorSetX(transformMatrix.r[1], 2 * orientation.i * orientation.j + 2 * orientation.r * orientation.k);
	transformMatrix.r[1] = XMVectorSetY(transformMatrix.r[1], 1 - 2 * orientation.i * orientation.i - 2 * orientation.k * orientation.k);
	transformMatrix.r[1] = XMVectorSetZ(transformMatrix.r[1], 2 * orientation.j * orientation.k - 2 * orientation.r * orientation.i);
	transformMatrix.r[1] = XMVectorSetW(transformMatrix.r[1], 0.0f);

	transformMatrix.r[2] = XMVectorSetX(transformMatrix.r[2], 2 * orientation.i * orientation.k - 2 * orientation.r * orientation.j);
	transformMatrix.r[2] = XMVectorSetY(transformMatrix.r[2], 2 * orientation.j * orientation.k + 2 * orientation.r * orientation.i);
	transformMatrix.r[2] = XMVectorSetZ(transformMatrix.r[2], 1 - 2 * orientation.i * orientation.i - 2 * orientation.j * orientation.j);
	transformMatrix.r[2] = XMVectorSetW(transformMatrix.r[2], 0.0f);

	transformMatrix.r[3] = XMVectorSetX(transformMatrix.r[3], position[0]);
	transformMatrix.r[3] = XMVectorSetY(transformMatrix.r[3], position[1]);
	transformMatrix.r[3] = XMVectorSetZ(transformMatrix.r[3], position[2]);
	transformMatrix.r[3] = XMVectorSetW(transformMatrix.r[3], 1.0f);

	transformMatrix = XMMatrixTranspose(transformMatrix);
}

#endif