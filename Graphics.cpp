#include "Graphics.hpp"

Scene::Scene(TGAImage* ptrImage, const char* nameFile)
:PtrImage(ptrImage), NameFile(nameFile){
	
}

Scene::Scene(int w, int h, int bpp, const char* nameFile)
:NameFile(nameFile){
	 TGAImage image(w, h, bpp);
	 PtrImage = &image;
}

Scene::~Scene(){
	
}

void Scene::save(){
	PtrImage->flip_vertically();
    PtrImage->write_tga_file(NameFile);
}

void Scene::line(int x0, int y0, int x1, int y1, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep) {
            PtrImage->set(y, x, color);
        } else {
            PtrImage->set(x, y, color);
        }
    }
}