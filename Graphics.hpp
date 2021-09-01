#pragma once
#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

class Scene{
	private:
	TGAImage* PtrImage;
	const char* NameFile;
	public:
	Scene(TGAImage* ptrImage, const char* nameFile);
	Scene(int w, int h, int bpp, const char* nameFile);
	~Scene();
	void save();
	void line(int x0, int y0, int x1, int y1, TGAColor color);
};