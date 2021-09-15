#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "geometry.hpp"

class Model{
    private:
	//Массив координат всех вершин
	std::vector<Vec3d> verts;
	//Массив с всех граней, содержит массив индексов всех вершин грани
	std::vector<std::vector<int> > faces;
	//Размер окна
	Vec2i size_screen;
	//Размеры модели
	Vec3d min, max;
	Vec3d size;
	double scale;
	//Z-буфер
	double* z_buffer;
	void line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color);
	void triangle(HDC hdc, Vec2i* t, const COLORREF &color);
	bool helpZ(Vec2i point, Vec3d t0, Vec3d t1, Vec3d t2, double* z_buffer);
public:
	Model(const char *filename, Vec2<unsigned long> _size_screen);
	~Model();
	void draw(HDC hdc);
	void drawMesh(HDC hdc, const COLORREF &color);
	void drawMeshTriangle(HDC hdc);
	void drawZ_buffer(HDC hdc);
};