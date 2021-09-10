#pragma once
#include <cmath>
#include <vector>

template <typename T> struct Vec2 {
	T x, y;
	// онструкторы=====================================================================================================
	Vec2(): x(0), y(0) {}
	Vec2(T _x, T _y): x(_x), y(_y) {}
	Vec2(const Vec2<T> &V): x(V.x), y(V.y) {}
	template <typename S> Vec2<T>(S _x, S _y, S _z): x(T(_x)), y(T(_y)) {};
	template <typename S> Vec2<T>(const Vec2<S> &V): x(T(V.x)), y(T(V.y)) {};
	~Vec2() {};
	//ќператоры========================================================================================================
	Vec2<T> operator +(const Vec2<T> &V)	const { return Vec2<T>(x + V.x, y + V.y); }
	Vec2<T> operator +=(const Vec2<T> &V)	const { *this + V; return this;}
	Vec2<T> operator -(const Vec2<T> &V)	const { return Vec2<T>(x - V.x, y - V.y); }
	//ѕроизведение вектора на число
	Vec2<T> operator *(float f)				const { return Vec2<T>(x * f, y * f); }
	//—кал€рное произведение векторов (если значение равно 0, то вектора ортогональны)
	double  operator *(const Vec2<T> &V)	const { return x * V.x + y * V.y; }
	//ƒлина вектора (модуль вектора)
	double  operator !()					const { return sqrt(x * x + y * y); }
	//‘ункции========================================================================================================
	//¬озвращает вектор с направл€ющими косинусами дл€ вектора относительно системы координат
	Vec2<double> getCos()					const { double modul = !(*this); return Vec2<double>(x / modul, y / modul); }
	//¬озвращает косинус угола между данным вектором и вектором V
	double getCosAngle(const Vec2<T> &V)	const { return ((*this) * V) / (!(*this) * !V); }
};

template <typename T> struct Vec3 {
	T x, y, z;
	// онструкторы=====================================================================================================
	Vec3(): x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z): x(_x), y(_y), z(_z) {}
	Vec3(const Vec3<T> &V): x(V.x), y(V.y), z(V.z) {}
	template <typename S> Vec3<T>(S _x, S _y, S _z): x(T(_x)), y(T(_y)), z(T(_z)) {};
	template <typename S> Vec3<T>(const Vec3<S> &V): x(T(V.x)), y(T(V.y)), z(T(V.z)) {};
	~Vec3() {};
	//ќператоры========================================================================================================
	Vec3<T> operator +(const Vec3<T> &V)	const { return Vec3<T>(x + V.x, y + V.y, z + V.z); }	
	Vec3<T> operator +=(const Vec3<T> &V)	const { *this + V; return this;}
	Vec3<T> operator -(const Vec3<T> &V)	const { return Vec3<T>(x - V.x, y - V.y, z - V.z); }
	//”множение вектора на число
	Vec3<T> operator *(float f)				const { return Vec3<T>(x * f, y * f, z * f); }
	//—кал€рное произведение векторов (если значение равно 0, то вектора ортогональны)
	double  operator *(const Vec3<T> &V)	const { return x * V.x + y * V.y + z * V.z; }
	//¬екторное произведение векторов
	Vec3<T> operator ^(const Vec3<T> &v)	const { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	//ƒлина вектора (модуль вектора)
	double  operator !()					const { return sqrt(x * x + y * y + z * z); }
	//‘ункции========================================================================================================
	//¬озвращает вектор с направл€ющими косинусами дл€ вектора относительно системы координат
	Vec3<double> getCos()					const { double modul = !(*this); return Vec3<double>(x / modul, y / modul, z / modul); }
	//¬озвращает косинус угола между данным вектором и вектором V
	double getCosAngle(const Vec3<T> &V)	const { return double((*this) * V) / (!(*this) * !V); }
};

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2d;
typedef Vec3<int> Vec3i;
typedef Vec3<double> Vec3d;
