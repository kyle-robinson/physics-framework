#pragma once
#ifndef VECTOR3D_H
#define VECTOR3D_H

template<typename T>
class Vector3D
{
public:
	Vector3D() : x( 0 ), y( 0 ), z( 0 ) {}
	Vector3D( T x, T y, T z ) : x( x ), y( y ), z( z ) {}
	Vector3D( const Vector3D& vec ) : x( vec.x ), y( vec.y ), z( vec.z ) {}

	T magnitude() { return sqrt( x * x + y * y + z * z ); }
	Vector3D normalize() { T unit = 1 / magnitude(); return Vector3D( unit * x, unit * y, unit * z ); }
	T dot( const Vector3D& rhs ) { return (x * rhs.x) + (y * rhs.y) + (z * rhs.z); }
	Vector3D cross( const Vector3D& rhs ) { return Vector3D( (y * rhs.z) - (z * rhs.y), (z * rhs.x) - (x * rhs.z), (x * rhs.y) - (y * rhs.x) ); }

	Vector3D operator + ( const Vector3D& rhs ) { return Vector3D( x + rhs.x, y + rhs.y, z + rhs.z ); }
	Vector3D operator - ( const Vector3D& rhs ) { return Vector3D( x - rhs.x, y - rhs.y, z - rhs.z ); }
	Vector3D operator * ( const T& rhs ) { return Vector3D( x * rhs, y * rhs, z * rhs ); }
	Vector3D operator / ( const T& rhs ) { return Vector3D( x / rhs, y / rhs, z / rhs ); }
	Vector3D& operator += ( const Vector3D& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3D& operator -= ( const Vector3D& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3D& operator *= ( const T& rhs ) { x *= rhs; y *= rhs; z *= rhs; return *this; }
	Vector3D& operator /= ( const T& rhs ) { x /= rhs; y /= rhs; z /= rhs; return *this; }
	T& operator [] ( const std::size_t& i ) { return *((T*)this + i); }
private:
	T x = 0;
	T y = 0;
	T z = 0;
};

#endif