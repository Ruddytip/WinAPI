#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "geometry.hpp"

void line(HDC hdc, int x0, int y0, int x1, int y1, const COLORREF &color);
void triangle(HDC hdc, Vec2i t0, Vec2i t1, Vec2i t2, const COLORREF &color);

class Model{
    private:
	std::vector<Vec3d> verts;
	std::vector<std::vector<int> > faces;
public:
	Model(const char *filename);
	~Model();
	int getCantidadVerts();
	int getCantidadFaces();
	Vec3d getVert(int i);
	std::vector<int> getFace(int id);
	void drawMesh(HDC hdc, int size, const COLORREF &color);
	void drawMeshTriangle(HDC hdc, int size, const COLORREF &color);
};