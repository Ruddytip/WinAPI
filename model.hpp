#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "geometry.hpp"
#include "tgaimage.hpp"

class Model{
    private:
	//Массив координат всех вершин
	std::vector<Vec3d> verts;
	//Массив текстурных координат
	std::vector<Vec2d> uv;
	//Массив нормалей
	std::vector<Vec3d> normals;
	//Массив с всех граней, содержит массивы индексов всех вершин, текстурных координат и веторов нормалей
	std::vector<face> faces;
	//
	std::vector<std::string> groups;
	//Размер окна
	Vec2i size_screen;
	//Размеры модели
	Vec3d min, max;
	Vec3d size;
	double scale;
	//Z-буфер
	double* z_buffer;
	//
	TGAImage texture;
	void line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color);
	void triangle(HDC hdc, Vec3d* t, const COLORREF &color);
	bool helpZ(Vec2i point, Vec3d* t);
public:
	Model(std::string filename, Vec2i _size_screen);
	~Model();
	void draw(HDC hdc);
	void drawMesh(HDC hdc, const COLORREF &color);
	void drawMeshTriangle(HDC hdc);
	void drawZ_buffer(HDC hdc);
};