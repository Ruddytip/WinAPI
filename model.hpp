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
	// Название модели
	std::string nameModel;
	// Массив координат всех вершин обекта
	std::vector<Vec3d> verts;
	// Массив текстурных координат обекта
	std::vector<Vec2d> uv;
	// Массив нормалей обекта
	std::vector<Vec3d> normals;
	// Обекты модели
	std::vector<object> objects;
	// Массив текстур
	std::vector<TGAImage> textures;
	// Массив материалов
	std::vector<material> materials;
	// Размер окна
	Vec2i size_screen;
	// Размеры модели
	Vec3d min, max;
	Vec3d size;
	double scale;
	// Z-буфер
	double* z_buffer;
	void line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color);
	void triangle(HDC hdc, Vec3d* t, const COLORREF &color);
	void triangle(HDC hdc, Vec3d* t, int count, int n, double nor);
	bool helpZ(Vec2i point, Vec3d* t);
	void printInfo();
	void initGroups();
	void initMaterials(std::string filename);
	void initTextures();
	void initMaps();
public:
	Model(std::string filename, Vec2i _size_screen);
	~Model();
	void draw(HDC hdc);
	void drawMesh(HDC hdc, const COLORREF &color);
	void drawMeshTriangle(HDC hdc);
	void drawMeshTexture(HDC hdc);
	void drawZ_buffer(HDC hdc);
};