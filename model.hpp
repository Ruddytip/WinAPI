#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "geometry.hpp"

void line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color);
void triangle(HDC hdc, Vec2i t0, Vec2i t1, Vec2i t2, const COLORREF &color);

class Model{
    private:
	std::vector<Vec3d> verts;
	std::vector<std::vector<int> > faces;
	int size;
	Vec3d Min, Max;
public:
	Model(const char *filename, int Size);
	~Model();
	int getCantidadVerts();
	int getCantidadFaces();
	Vec3d getVert(int i);
	std::vector<int> getFace(int id);
	void drawMesh(HDC hdc, const COLORREF &color);
	void drawMeshTriangle(HDC hdc);
};