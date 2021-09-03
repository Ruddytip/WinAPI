#pragma once
#include <cmath>

template <class T> struct Vec2 {
	T x, y;
	Vec2(): x(0), y(0) {};
	Vec2(T _x, T _y): x(_x), y(_y) {};
	inline Vec2<T> operator +(const Vec2<T> &V) const { return Vec2<T>(x + V.x, y + V.y); }
	inline Vec2<T> operator -(const Vec2<T> &V) const { return Vec2<T>(x - V.x, y - V.y); }
	inline Vec2<T> operator *(float f)          const { return Vec2<T>(x * f, y * f); }
};

template <class T> struct Vec3 {
	T x, y, z;
	Vec3(): x(0), y(0), z(0) {};
	Vec3(T _x, T _y, T _z): x(_x), y(_y), z(_z) {};
	Vec3(const Vec3<T> &V): x(V.x), y(V.y), z(V.z) {};
	inline Vec3<T> operator +(const Vec3<T> &V) const { return Vec3<T>(x + V.x, y + V.y, z + V.z); }
	inline Vec3<T> operator -(const Vec3<T> &V) const { return Vec3<T>(x - V.x, y - V.y, z - V.z); }
	inline Vec3<T> operator *(float f)          const { return Vec3<T>(x * f, y * f, z * f); }

	//inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	//inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	//float norm () const { return std::sqrt(x*x+y*y+z*z); }
	//Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
};

typedef Vec2<int>   Vec2i;
typedef Vec3<int>   Vec3i;
typedef Vec2<double> Vec2d;
typedef Vec3<double> Vec3d;