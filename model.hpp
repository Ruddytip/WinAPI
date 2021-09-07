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
	int size;
	//Размеры модели
	Vec3d min, max;
	void line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color);
	void triangle(HDC hdc, int* z_buffer, Vec2i t0, Vec2i t1, Vec2i t2, const COLORREF &color);
public:
	Model(const char *filename, int Size);
	~Model();
	void draw(HDC hdc);
	void drawMesh(HDC hdc, const COLORREF &color);
	void drawMeshTriangle(HDC hdc);
};