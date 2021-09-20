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
	//Название модели
	std::string nameModel;
	//Массив координат всех вершин
	std::vector<Vec3d> verts;
	//Массив текстурных координат
	std::vector<Vec2d> uv;
	//Массив нормалей
	std::vector<Vec3d> normals;
	//Хранит название груп из obj-файла и массив всех граней группы
	std::vector<group> groups;
	//Размер окна
	Vec2i size_screen;
	//Размеры модели
	Vec3d min, max;
	Vec3d size;
	double scale;
	//Z-буфер
	double* z_buffer;
	//Текстура модели
	TGAImage texture;
	void line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color);
	void triangle(HDC hdc, Vec3d* t, const COLORREF &color);
	void triangle(HDC hdc, Vec3d* t, int count, int n, double nor);
	bool helpZ(Vec2i point, Vec3d* t);
	void printInfo();
	void initGroups();
public:
	Model(std::string filename, Vec2i _size_screen);
	~Model();
	void draw(HDC hdc);
	void drawMesh(HDC hdc, const COLORREF &color);
	void drawMeshTriangle(HDC hdc);
	void drawMeshTexture(HDC hdc);
	void drawZ_buffer(HDC hdc);
};