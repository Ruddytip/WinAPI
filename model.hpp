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
	// �������� ������
	std::string nameModel;
	// ������ ��������� ���� ������ ������
	std::vector<Vec3d> verts;
	// ������ ���������� ��������� ������
	std::vector<Vec2d> uv;
	// ������ �������� ������
	std::vector<Vec3d> normals;
	// ������ ������
	std::vector<object> objects;
	// ������ �������
	std::vector<TGAImage> textures;
	// ������ ����������
	std::vector<material> materials;
	// ������ ����
	Vec2i size_screen;
	// ������� ������
	Vec3d min, max;
	Vec3d size;
	double scale;
	// Z-�����
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