#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "geometry.hpp"

void line(HDC hdc, int x0, int y0, int x1, int y1);
void triangle(HDC hdc, Vec2i t0, Vec2i t1, Vec2i t2);

class Model{
    private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
};