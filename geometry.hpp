#pragma once
#include <cmath>
#include <vector>

template <typename T> struct Vec2 {
	T x, y;
	//������������=====================================================================================================
	Vec2(): x(0), y(0) {}
	Vec2(T _x, T _y): x(_x), y(_y) {}
	Vec2(const Vec2<T> &V): x(V.x), y(V.y) {}
	template <typename S> Vec2<T>(S _x, S _y, S _z): x(T(_x)), y(T(_y)) {};
	template <typename S> Vec2<T>(const Vec2<S> &V): x(T(V.x)), y(T(V.y)) {};
	~Vec2() {};
	//���������========================================================================================================
	Vec2<T> operator +(const Vec2<T> &V)	const { return Vec2<T>(x + V.x, y + V.y); }
	Vec2<T> operator +=(const Vec2<T> &V)	{ *this = *this + V; return this; }
	Vec2<T> operator -(const Vec2<T> &V)	const { return Vec2<T>(x - V.x, y - V.y); }
	Vec2<T> operator -=(const Vec2<T> &V)	{ *this = *this - V; return this; }
	//������������ ������� �� �����
	Vec2<T> operator *(T f)					const { return Vec2<T>(x * f, y * f); }
	//������� ������ �� �����
	Vec2<double> operator /(double f)		const { return Vec2<double>(x / f, y / f); }
	//��������� ������������ �������� (���� �������� ����� 0, �� ������� ������������)
	double  operator *(const Vec2<T> &V)	const { return x * V.x + y * V.y; }
	//����� ������� (������ �������)
	double  operator !()					const { return sqrt(x * x + y * y); }
	//�������========================================================================================================
	//����������� ������
	Vec2<double> norm()						{ *this = *this / !*this; return this; }
	//���������� ������� ����� ����� ������ �������� � �������� V
	double getCosAngle(const Vec2<T> &V)	const { return ((*this) * V) / (!(*this) * !V); }
};

template <typename T> struct Vec3 {
	T x, y, z;
	//������������=====================================================================================================
	Vec3(): x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z): x(_x), y(_y), z(_z) {}
	Vec3(const Vec3<T> &V): x(V.x), y(V.y), z(V.z) {}
	template <typename S> Vec3<T>(S _x, S _y, S _z): x(T(_x)), y(T(_y)), z(T(_z)) {};
	template <typename S> Vec3<T>(const Vec3<S> &V): x(T(V.x)), y(T(V.y)), z(T(V.z)) {};
	~Vec3() {};
	//���������========================================================================================================
	Vec3<T> operator +(const Vec3<T> &V)	const { return Vec3<T>(x + V.x, y + V.y, z + V.z); }	
	Vec3<T> operator +=(const Vec3<T> &V)	{ *this = *this + V; return this; }
	Vec3<T> operator -(const Vec3<T> &V)	const { return Vec3<T>(x - V.x, y - V.y, z - V.z); }
	Vec3<T> operator -=(const Vec3<T> &V)	{ *this = *this - V; return this; }
	//��������� ������� �� �����
	Vec3<T> operator *(T f)					const { return Vec3<T>(x * f, y * f, z * f); }
	//������� ������� �� �����
	Vec3<double> operator /(double f)		const { return Vec3<double>(x / f, y / f, z / f); }
	//��������� ������������ �������� (���� �������� ����� 0, �� ������� ������������)
	double  operator *(const Vec3<T> &V)	const { return x * V.x + y * V.y + z * V.z; }
	//��������� ������������ ��������
	Vec3<T> operator ^(const Vec3<T> &v)	const { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	//����� ������� (������ �������)
	double  operator !()					const { return sqrt(x * x + y * y + z * z); }
	//�������========================================================================================================
	//����������� ������
	Vec3<double> norm()						{ *this = (*this) / !(*this); return this; }
	//���������� ������� ����� ����� ������ �������� � �������� V
	double getCosAngle(const Vec3<T> &V)	const { return double((*this) * V) / (!(*this) * !V); }
};

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2d;
typedef Vec3<int> Vec3i;
typedef Vec3<double> Vec3d;

// �����
struct face{
	// ������ �������� ������
	std::vector<int> id_v;
	// ������ �������� ���������� ������
	std::vector<int> id_vt;
	// ������ �������� ��������
	std::vector<int> id_vn;
	// ������ �������� {0-���������, 1-��������, 2-����� ��������}
	std::vector<int> id_mtm;
};

// ����� (����� ������)
struct object{
	// �������� ������
	std::string name;
	// ������ ������
	std::vector<face> faces;
	// ������������ ��� ����������� ��� ���������
	bool visible;
};

// ��������
struct material{
	// �������� ���������
	std::string name;
	// ���������� ���� (���� ������� ��� ������� �����), RGB �� 0 (���.) �� 1 (����.)
	Vec3d Ka;
	// ������������ ��� ���������� ����� (���� ������� ��� ����� �����)
	Vec3d Kd;
	// ������������ ��� ������������ �����.
	Vec3d Ke;
	// ������������ ��� ����������� �����
	Vec3d Ks;
	// ������������ ��� ���������� ����������� ��������� �� 0 �� 100
	double Ns;
	// ������������ ��� ���������� ���������
	double Ni;
	// ������������ ��� ������������ �� 0 �� 1 (��� ������������)
	double d;
	// "�����������" �� ���������� �����
	int illum;
	// �������� ������������ �������� ambient
	std::string map_Ka;
	// �������� ������������ �������� diffuse
	std::string map_Kd;
	// �������� ������������ �������� specular
	std::string map_Ks;
	// �������� ������������ �������� ������������
	std::string map_D;
};