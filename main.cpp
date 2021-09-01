#include "tgaimage.h"

void line(int x0, int y0, int x1, int y1, TGAImage* image, TGAColor* color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        image->set((steep?y:x), (steep?x:y), *color);
        error2 += derror2;

        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx * 2;
        }
    }
}

TGAColor white = TGAColor(255, 255, 255, 255);
TGAColor red   = TGAColor(255, 0,   0,   255);

int main(int argc, char** argv) {
        TGAImage image(100, 100, TGAImage::RGB);
        
        for(unsigned int i = 0; i < 1000000; ++i){
                line(20, 20, 80, 80, &image, &white);
                line(20, 80, 80, 20, &image, &red);
        }

        image.write_tga_file("output.tga");
        return 0;
}