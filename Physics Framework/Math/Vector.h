#pragma once
#ifndef VECTOR_H
#define VECTOR_H

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

	Vector3D operator + ( const XMVECTOR& rhs ) { return Vector3D( x + XMVectorGetX( rhs ), y + XMVectorGetY( rhs ), z + XMVectorGetZ( rhs ) ); }
	Vector3D operator * ( const Vector3D& rhs ) { return Vector3D( x * rhs.x, y * rhs.y, z * rhs.z ); }
	Vector3D operator += ( const float& rhs ) { return Vector3D( x += rhs, y += rhs, z += rhs ); }

	Vector3D operator + ( const Vector3D& rhs ) { return Vector3D( x + rhs.x, y + rhs.y, z + rhs.z ); }
	Vector3D operator - ( const Vector3D& rhs ) { return Vector3D( x - rhs.x, y - rhs.y, z - rhs.z ); }
	Vector3D operator * ( const T& rhs ) { return Vector3D( x * rhs, y * rhs, z * rhs ); }
	Vector3D operator / ( const T& rhs ) { return Vector3D( x / rhs, y / rhs, z / rhs ); }
	Vector3D& operator += ( const Vector3D& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3D& operator -= ( const Vector3D& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	//Vector3D& operator *= ( const T& rhs ) { x *= rhs; y *= rhs; z *= rhs; return *this; }
	void operator*=( const float val ) { x *= val; y *= val; z *= val; }
	Vector3D& operator /= ( const T& rhs ) { x /= rhs; y /= rhs; z /= rhs; return *this; }
	T& operator [] ( const std::size_t& i ) { return *((T*)this + i); }

	Vector3D ComponentProduct( const Vector3D& vec ) const { return Vector3D( x * vec.x, y * vec.y, z * vec.z ); }
	void AddScaledVector( const Vector3D& vec, float scale ) { x += vec.x * scale; y += vec.y * scale; z += vec.z * scale; }
	float ScalarProduct( const Vector3D& vec ) const { return x * vec.x + y * vec.y + z * vec.z; }
	Vector3D operator % ( const Vector3D& vec ) { return Vector3D( y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x ); }
public:
	T x = 0;
	T y = 0;
	T z = 0;
};

typedef Vector3D<float> v3df;
typedef Vector3D<int> v3di;

template<typename T>
class Vector4D
{
public:
	Vector4D() : x( 0 ), y( 0 ), z( 0 ), w( 0 ) {}
	Vector4D( T x, T y, T z, T w ) : x( x ), y( y ), z( z ), w( w ) {}
	Vector4D( const Vector4D& vec ) : x( vec.x ), y( vec.y ), z( vec.z ), w( vec.w ) {}

	T magnitude() { return sqrt( x * x + y * y + z * z + w * w ); }
	Vector4D normalize() { T unit = 1 / magnitude(); return Vector4D( unit * x, unit * y, unit * z, unit * w ); }
	T dot( const Vector4D& rhs ) { return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w); }

	Vector4D operator + ( const Vector4D& rhs ) { return Vector4D( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w ); }
	Vector4D operator - ( const Vector4D& rhs ) { return Vector4D( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w ); }
	Vector4D operator * ( const T& rhs ) { return Vector4D( x * rhs, y * rhs, z * rhs, w * rhs ); }
	Vector4D operator / ( const T& rhs ) { return Vector4D( x / rhs, y / rhs, z / rhs, w / rhs ); }
	Vector4D& operator += ( const Vector4D& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	Vector4D& operator -= ( const Vector4D& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
	Vector4D& operator *= ( const T& rhs ) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
	Vector4D& operator /= ( const T& rhs ) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }
	T& operator [] ( const std::size_t& i ) { return *((T*)this + i); }

	Vector4D ComponentProduct( const Vector4D& vec ) const { return Vector4D( x * vec.x, y * vec.y, z * vec.z, w * vec.w ); }
	void AddScaledVector( const Vector4D& vec, float scale ) { x += vec.x * scale; y += vec.y * scale; z += vec.z * scale; w += vec.w * scale; }
	float ScalarProduct( const Vector4D& vec ) const { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }
	Vector4D operator % ( const Vector4D& vec ) { return Vector4D( y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x, vec.w ); }
public:
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;
};

typedef Vector4D<float> v4df;
typedef Vector4D<int> v4di;

#endif